#include <cmath>
#include "vector.h"

namespace jp {
	f32 Vector::get_x() const {
		return data.x;
	}

	f32 Vector::get_y() const {
		return data.y;
	}

	void Vector::set_x(f32 x) {
		data.x = x;
	}

	void Vector::set_y(f32 y) {
		data.y = y;
	}

	f32 Vector::get_magnitude() const {
		return std::sqrt(data.x * data.x + data.y * data.y);
	}

	f32 Vector::get_angle() const {
		return std::atan2(data.y, data.x);
	}

	void Vector::set_magnitude(f32 magnitude) {
		f32 old_magnitude = get_magnitude();

		if (old_magnitude == 0.0) {
			return;
		}

		data.x *= magnitude / old_magnitude;
		data.y *= magnitude / old_magnitude;
	}

	void Vector::set_angle(f32 angle) {
		f32 magnitude = get_magnitude();

		data.x = magnitude * std::cos(angle);
		data.y = magnitude * std::sin(angle);
	}

	void Vector::set_angle_in_degrees(f32 degrees) {
		set_angle(static_cast<f32>(degrees * M_PI / 180.0));
	}

	Vector Vector::get_negated() const {
		return Vector::from_cartesian_coordinates(-data.x, -data.y);
	}

	Vector Vector::get_normalized() const {
		return Vector::from_angle(std::atan2(data.y, data.x));
	}

	Vector Vector::get_resized(f32 magnitude) const {
		return Vector::from_polar_coordinates(magnitude, get_angle());
	}

	Vector Vector::get_stretched(f32 scalar) const {
		return Vector::from_cartesian_coordinates(data.x * scalar, data.y * scalar);
	}

	Vector Vector::get_rotated(f32 radians) const {
		return Vector::from_polar_coordinates(get_magnitude(), get_angle() + radians);
	}

	Vector Vector::get_rotated_in_degrees(f32 degrees) const {
		return Vector::from_polar_coordinates(get_magnitude(), static_cast<f32>(get_angle() + degrees * M_PI / 180.0));
	}

	void Vector::operator+=(const Vector& other) {
		data.x += other.data.x;
		data.y += other.data.y;
	}

	void Vector::operator-=(const Vector& other) {
		data.x -= other.data.x;
		data.y -= other.data.y;
	}

	void Vector::operator*=(f32 scalar) {
		data.x *= scalar;
		data.y *= scalar;
	}

	void Vector::operator/=(f32 scalar) {
		data.x /= scalar;
		data.y /= scalar;
	}

	Vector Vector::operator+(const Vector& other) const {
		return Vector::from_cartesian_coordinates(data.x + other.data.x, data.y + other.data.y);
	}

	Vector Vector::operator-(const Vector& other) const {
		return Vector::from_cartesian_coordinates(data.x - other.data.x, data.y - other.data.y);
	}

	Vector Vector::operator*(f32 scalar) const {
		return Vector::from_cartesian_coordinates(data.x * scalar, data.y * scalar);
	}

	Vector Vector::operator/(f32 scalar) const {
		return Vector::from_cartesian_coordinates(data.x / scalar, data.y / scalar);
	}

	Vector::Vector(Vector::Data data) : data(data) {

	}

	Vector Vector::zero() {
		return Vector({0.0, 0.0});
	}

	Vector Vector::unit_x() {
		return Vector({1.0, 0.0});
	}

	Vector Vector::unit_y() {
		return Vector({0.0, 1.0});
	}

	Vector Vector::from_angle(f32 angle) {
		return Vector({static_cast<f32>(std::cos(angle)), static_cast<f32>(std::sin(angle))});
	}
	Vector Vector::from_polar_coordinates(f32 magnitude, f32 angle) {
		return Vector({magnitude * std::cos(angle), magnitude * std::sin(angle)});
	}
	Vector Vector::from_cartesian_coordinates(f32 x, f32 y) {
		return Vector({x, y});
	}
} // jp