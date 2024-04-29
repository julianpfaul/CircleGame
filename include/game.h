#pragma once

#include <set>
#include <numeric>
#include <cmath>
#include <iostream>
#include <random>
#include "SFML/Graphics.hpp"
#include "vector.h"
#include "arena.h"
#include "player.h"
#include "circleobstacle.h"

namespace jp::cg {

	void draw_vector(sf::RenderWindow& window, Vector vector) {
		auto buffer = sf::VertexArray(sf::Lines, 2);
		buffer[0].position = sf::Vector2f(0.0f, 0.0f);
		buffer[1].position = sf::Vector2f(vector.get_x(), vector.get_y());
		window.draw(buffer);
	}

	void draw_vector_from(sf::RenderWindow& window, Vector vector, Vector origin) {
		auto buffer = sf::VertexArray(sf::Lines, 2);
		buffer[0].position = sf::Vector2f(origin.get_x(), origin.get_y());
		buffer[1].position = sf::Vector2f(vector.get_x() + origin.get_x(), vector.get_y() + origin.get_y());
		window.draw(buffer);
	}

	static void run() {
		/* constants */
		constexpr auto window_width = 800;
		constexpr auto window_height = 800;

		constexpr auto game_scaler = static_cast<f32>(std::min(window_width, window_height));

		/* window */
		auto context_settings = sf::ContextSettings();
		context_settings.antialiasingLevel = 8;
		context_settings.depthBits = 24;
		auto window = sf::RenderWindow(sf::VideoMode(window_width, window_height), "Circle Game", sf::Style::Default, context_settings);

		/* camera */
		constexpr auto camera_pull_factor = 3.0f;
		auto camera_zoom = 1.0f;
		auto camera_position = Vector::zero();

		/* mutable objects */
		auto arena = Arena::create();
		arena.set_radius(1.0f * game_scaler);

		constexpr auto player_radius = 0.025f * game_scaler;
		constexpr auto player_strength = 0.9f * game_scaler;
		auto player = Player::create();
		player.set_position(Vector::zero());
		player.set_velocity(Vector::zero());

		/* immutable objects */
		auto circle_list = std::vector<CircleObstacle>();
		circle_list.push_back(CircleObstacle::create(0.5f * game_scaler, Vector::from_polar_coordinates(0.5f * game_scaler, 0.0f)));
		circle_list.push_back(CircleObstacle::create(0.2f * game_scaler, Vector::from_cartesian_coordinates(-0.2f * game_scaler, 0.4f * game_scaler)));

		auto random = std::random_device();
		auto generator = std::mt19937(random());
		auto distribution = std::uniform_real_distribution<f32>(-1.0f, 1.0f);

		/* populate the arena with random circles */
		for (auto index = 0; index < 10; index++) {
			auto circle = CircleObstacle::create(0.05f * game_scaler, Vector::from_cartesian_coordinates(distribution(generator) * game_scaler, distribution(generator) *
			game_scaler));
			circle_list.push_back(circle);
		}

		/* physics */
		constexpr auto multi_body_collision_resolution_factor = 0.01f;

		constexpr auto gravitational_strength = 0.02f * game_scaler;
		auto gravitational_force = Vector::from_cartesian_coordinates(0.0, gravitational_strength);

		/* game loop stuff */
		auto clock = sf::Clock();

		auto time_accumulator = sf::Time::Zero;
		auto current_time = sf::Time::Zero;
		auto delta_time = sf::Time::Zero;
		auto previous_time = clock.getElapsedTime();

		constexpr auto time_scale = 9.5f; //  this has to be a float because of some ambiguous function overload when using double

		auto current_update_time = sf::Time::Zero;
		auto delta_update_time = sf::Time::Zero;
		auto previous_update_time = clock.getElapsedTime();

		/* input */
		auto left_pressed = false;
		auto right_pressed = false;

		while (window.isOpen()) {
			current_time = clock.getElapsedTime();
			delta_time = current_time - previous_time;
			previous_time = current_time;

			time_accumulator += delta_time;

			auto event = sf::Event{};
			while (window.pollEvent(event)) {
				if (event.type == sf::Event::Closed) {
					window.close();
				}

				if (event.type == sf::Event::KeyPressed) {
					if (event.key.code == sf::Keyboard::A) {
						left_pressed = true;
					}

					if (event.key.code == sf::Keyboard::D) {
						right_pressed = true;
					}
				}

				if (event.type == sf::Event::KeyReleased) {
					if (event.key.code == sf::Keyboard::A) {
						left_pressed = false;
					}

					if (event.key.code == sf::Keyboard::D) {
						right_pressed = false;
					}
				}

				if (event.type == sf::Event::MouseWheelScrolled) {
					if (event.mouseWheelScroll.delta > 0) {
						camera_zoom *= 1.1f;
					} else {
						camera_zoom /= 1.1f;
					}
				}
			}

			constexpr auto target_tps = 240.0;

			/* update */
			if (time_accumulator >= sf::seconds(1.0 / target_tps)) {
				current_update_time = clock.getElapsedTime();
				delta_update_time = current_update_time - previous_update_time;
				previous_update_time = current_update_time;

				delta_update_time *= time_scale;

				auto acceleration = Vector::zero();
				acceleration += gravitational_force;

				if (left_pressed) {
					acceleration += Vector::from_cartesian_coordinates(-player_strength, 0.0) * delta_update_time.asSeconds();
				}

				if (right_pressed) {
					acceleration += Vector::from_cartesian_coordinates(player_strength, 0.0) * delta_update_time.asSeconds();
				}

				auto predicted_velocity = player.get_velocity() + acceleration * delta_update_time.asSeconds();
				auto predicted_position = player.get_position() + predicted_velocity * delta_update_time.asSeconds();

				bool did_hit_arena = false;
				std::vector<CircleObstacle> hit_obstacle_list;
				/* arena collision check */
				did_hit_arena = predicted_position.get_magnitude() + player_radius > arena.get_radius();

				/* obstacle collision check */
				for (auto& circle_obstacle: circle_list) {
					auto difference_vector = (predicted_position - circle_obstacle.get_center());
					if (difference_vector.get_magnitude() < circle_obstacle.get_radius() + player_radius) {
						hit_obstacle_list.push_back(circle_obstacle);
					}
				}

				auto resolution_count = 0.0f;

				do {
					/* collision resolution */
					if (did_hit_arena) {
						auto normal_vector = predicted_position.get_normalized().get_negated().get_rotated_in_degrees(90);

						predicted_position.set_magnitude(arena.get_radius() - player_radius - resolution_count);
						predicted_velocity = normal_vector * predicted_velocity.get_dot_product(normal_vector);
					}

					for (auto& hit_obstacle: hit_obstacle_list) {
						auto normal_vector = (predicted_position - hit_obstacle.get_center()).get_normalized();

						predicted_position = hit_obstacle.get_center() + normal_vector * (hit_obstacle.get_radius() + player_radius + resolution_count);
						predicted_velocity = normal_vector.get_rotated_in_degrees(-90) * predicted_velocity.get_dot_product(normal_vector.get_rotated_in_degrees(-90));
					}

					/* check if collisions resolved */
					hit_obstacle_list.clear();

					/* arena collision check */
					did_hit_arena = predicted_position.get_magnitude() + player_radius > arena.get_radius();

					/* obstacle collision check */
					for (auto& circle_obstacle: circle_list) {
						auto difference_vector = (predicted_position - circle_obstacle.get_center());
						if (difference_vector.get_magnitude() < circle_obstacle.get_radius() + player_radius) {
							hit_obstacle_list.push_back(circle_obstacle);
						}
					}

					resolution_count += multi_body_collision_resolution_factor * delta_update_time.asSeconds();
				} while ((did_hit_arena || !hit_obstacle_list.empty()));

				player.set_velocity(predicted_velocity);
				player.set_position(predicted_position);

				/* update view */ {
					camera_position += (player.get_position() - camera_position) * camera_pull_factor * delta_update_time.asSeconds();

					auto view = window.getDefaultView();
					view.setCenter(camera_position.get_x(), camera_position.get_y());
					view.zoom(camera_zoom);
					window.setView(view);
				}

				time_accumulator -= sf::seconds(1.0 / target_tps);
			}

			/* render */ {
				window.clear();

				/* render arena */ {
					auto circle = sf::CircleShape(0.0f, 265);
					circle.setRadius(arena.get_radius());
					circle.setOrigin(arena.get_radius(), arena.get_radius());
					circle.setPosition(0.0, 0.0);
					circle.setOutlineColor(sf::Color::White);
					circle.setOutlineThickness(1.0);
					circle.setFillColor(sf::Color::Blue);

					window.draw(circle);
				}

				/* render player */ {
					auto circle = sf::CircleShape();
					circle.setRadius(player_radius);
					circle.setOrigin(player_radius, player_radius);
					circle.setPosition(player.get_position().get_x(), player.get_position().get_y());
					circle.setOutlineColor(sf::Color::White);
					circle.setOutlineThickness(1.0);
					circle.setFillColor(sf::Color::Red);

					window.draw(circle);
				}

				/* render obstacles */
				for (const auto& circle_obstacle: circle_list) {
					auto circle = sf::CircleShape(0.0f, 64);
					circle.setRadius(circle_obstacle.get_radius());
					circle.setOrigin(circle_obstacle.get_radius(), circle_obstacle.get_radius());
					circle.setPosition(circle_obstacle.get_center().get_x(), circle_obstacle.get_center().get_y());
					circle.setOutlineColor(sf::Color::White);
					circle.setOutlineThickness(1.0);
					circle.setFillColor(sf::Color::Black);

					window.draw(circle);
				}

				draw_vector_from(window, player.get_velocity(), player.get_position());

				window.display();
			}
		}
	}
}