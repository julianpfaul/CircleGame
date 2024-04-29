#pragma once

#include <set>
#include "SFML/Graphics.hpp"
#include "vector.h"
#include "arena.h"
#include "player.h"
#include "circleobstacle.h"

namespace jp::cg {
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
		constexpr auto player_strength = 0.4f * game_scaler;
		auto player = Player::create();
		player.set_position(Vector::zero());
		player.set_velocity(Vector::zero());

		/* immutable objects */
		auto circle_list = std::vector<CircleObstacle>();
		circle_list.push_back(CircleObstacle::create(0.5f * game_scaler, Vector::from_polar_coordinates(0.5f * game_scaler, 0.0f)));

		/* physics */
		constexpr auto gravitational_strength = 0.2f * game_scaler;
		auto gravitational_force = Vector::from_cartesian_coordinates(0.0, gravitational_strength);

		/* game loop stuff */
		auto clock = sf::Clock();

		auto time_accumulator = sf::Time::Zero;
		auto current_time = sf::Time::Zero;
		auto delta_time = sf::Time::Zero;
		auto previous_time = clock.getElapsedTime();

		constexpr auto time_scale = 5.0f; //  this has to be a float because of some ambiguous function overload when using double
		auto current_update_time = sf::Time::Zero;
		auto delta_update_time = sf::Time::Zero;
		auto previous_update_time = clock.getElapsedTime();

		/* input */
		auto left_pressed = false;
		auto right_pressed = false;
		auto down_pressed = false;

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
					if (event.key.code == sf::Keyboard::Left) {
						left_pressed = true;
					}

					if (event.key.code == sf::Keyboard::Right) {
						right_pressed = true;
					}

					if (event.key.code == sf::Keyboard::Down) {
						down_pressed = true;
					}
				}

				if (event.type == sf::Event::KeyReleased) {
					if (event.key.code == sf::Keyboard::Left) {
						left_pressed = false;
					}

					if (event.key.code == sf::Keyboard::Right) {
						right_pressed = false;
					}

					if (event.key.code == sf::Keyboard::Down) {
						down_pressed = false;
					}
				}

				if(event.type == sf::Event::MouseWheelScrolled) {
					if(event.mouseWheelScroll.delta > 0) {
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
					acceleration += Vector::from_cartesian_coordinates(-player_strength, 0.0);
				}

				if (right_pressed) {
					acceleration += Vector::from_cartesian_coordinates(player_strength, 0.0);
				}

				if (down_pressed) {
					acceleration += Vector::from_cartesian_coordinates(0.0, player_strength);
				}

				auto predicted_velocity = player.get_velocity() + acceleration * delta_update_time.asSeconds();
				auto predicted_position = player.get_position() + predicted_velocity * delta_update_time.asSeconds();

				/*
				/* arena collision x */
				if (predicted_position.get_magnitude() + player_radius > arena.get_radius()) {
					auto normal_force = predicted_velocity;
					normal_force.set_angle(predicted_position.get_negated().get_angle());
					normal_force.set_x(0.0);

					acceleration += normal_force;
					predicted_velocity = player.get_velocity() + acceleration * delta_update_time.asSeconds();
					predicted_position = player.get_position() + predicted_velocity * delta_update_time.asSeconds();

					if (predicted_position.get_magnitude() + player_radius > arena.get_radius()) {
						auto correction_vector = predicted_position;
						correction_vector.set_magnitude(arena.get_radius() - player_radius);

						predicted_position.set_x(correction_vector.get_x());
					}
				}

				/* obstacle collision x */
				for (auto& circle_obstacle: circle_list) {
					auto difference_vector = (predicted_position - circle_obstacle.get_center());
					if (difference_vector.get_magnitude() < circle_obstacle.get_radius() + player_radius) {
						auto normal_force = predicted_velocity;
						normal_force.set_angle(difference_vector.get_angle());
						normal_force.set_x(0.0);

						acceleration += normal_force;
						predicted_velocity = player.get_velocity() + acceleration * delta_update_time.asSeconds();
						predicted_position = player.get_position() + predicted_velocity * delta_update_time.asSeconds();

						difference_vector = (predicted_position - circle_obstacle.get_center());
						if (difference_vector.get_magnitude() < circle_obstacle.get_radius() + player_radius) {
							difference_vector.set_magnitude(circle_obstacle.get_radius() + player_radius);

							predicted_position.set_x((circle_obstacle.get_center() + difference_vector).get_x());
						}
					}
				}

				/* arena collision y */
				if (predicted_position.get_magnitude() + player_radius > arena.get_radius()) {
					auto normal_force = predicted_velocity;
					normal_force.set_angle(predicted_position.get_negated().get_angle());
					normal_force.set_y(0.0);

					acceleration += normal_force;
					predicted_velocity = player.get_velocity() + acceleration * delta_update_time.asSeconds();
					predicted_position = player.get_position() + predicted_velocity * delta_update_time.asSeconds();

					if (predicted_position.get_magnitude() + player_radius > arena.get_radius()) {
						auto correction_vector = predicted_position;
						correction_vector.set_magnitude(arena.get_radius() - player_radius);

						predicted_position.set_y(correction_vector.get_y());
					}
				}

				/* obstacle collision y */
				for (auto& circle_obstacle: circle_list) {
					auto difference_vector = (predicted_position - circle_obstacle.get_center());
					if (difference_vector.get_magnitude() < circle_obstacle.get_radius() + player_radius) {
						auto normal_force = predicted_velocity;
						normal_force.set_angle(difference_vector.get_angle());
						normal_force.set_y(0.0);

						acceleration += normal_force;
						predicted_velocity = player.get_velocity() + acceleration * delta_update_time.asSeconds();
						predicted_position = player.get_position() + predicted_velocity * delta_update_time.asSeconds();

						difference_vector = (predicted_position - circle_obstacle.get_center());
						if (difference_vector.get_magnitude() < circle_obstacle.get_radius() + player_radius) {
							difference_vector.set_magnitude(circle_obstacle.get_radius() + player_radius);

							predicted_position.set_y((circle_obstacle.get_center() + difference_vector).get_y());
						}
					}
				}
				*/

					/* arena collision */
				if (predicted_position.get_magnitude() + player_radius > arena.get_radius()) {
					auto normal_force = predicted_velocity;
					normal_force.set_angle(predicted_position.get_negated().get_angle());

					acceleration += normal_force;
					predicted_velocity = player.get_velocity() + acceleration * delta_update_time.asSeconds();
					predicted_position = player.get_position() + predicted_velocity * delta_update_time.asSeconds();

					if (predicted_position.get_magnitude() + player_radius > arena.get_radius()) {
						auto correction_vector = predicted_position;
						correction_vector.set_magnitude(arena.get_radius() - player_radius);

						predicted_position = correction_vector;
					}
				}

				/* obstacle collision */
				for (auto& circle_obstacle: circle_list) {
					auto difference_vector = (predicted_position - circle_obstacle.get_center());
					if (difference_vector.get_magnitude() < circle_obstacle.get_radius() + player_radius) {
						auto normal_force = predicted_velocity;
						normal_force.set_angle(difference_vector.get_angle());

						acceleration += normal_force;
						predicted_velocity = player.get_velocity() + acceleration * delta_update_time.asSeconds();
						predicted_position = player.get_position() + predicted_velocity * delta_update_time.asSeconds();

						difference_vector = (predicted_position - circle_obstacle.get_center());
						if (difference_vector.get_magnitude() < circle_obstacle.get_radius() + player_radius) {
							difference_vector.set_magnitude(circle_obstacle.get_radius() + player_radius);

							predicted_position = circle_obstacle.get_center() + difference_vector;
						}
					}
				}

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

				window.display();
			}
		}
	}
}
