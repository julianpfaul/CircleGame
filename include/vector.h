#pragma once

#include "fx.h"

namespace jp {
	class Vector {
	public:
		[[nodiscard]] f32 get_x() const;
		[[nodiscard]] f32 get_y() const;

		void set_x(f32 x);
		void set_y(f32 y);

		[[nodiscard]] f32 get_magnitude() const;
		[[nodiscard]] f32 get_angle() const;

		void set_magnitude(f32 magnitude);
		void set_angle(f32 angle);
		void set_angle_in_degrees(f32 degrees);

		[[nodiscard]] Vector get_negated() const;
		[[nodiscard]] Vector get_normalized() const;

		[[nodiscard]] Vector get_resized(f32 magnitude) const;
		[[nodiscard]] Vector get_stretched(f32 scalar) const;

		[[nodiscard]] Vector get_rotated(f32 radians) const;
		[[nodiscard]] Vector get_rotated_in_degrees(f32 degrees) const;

		[[nodiscard]] f32 get_dot_product(const Vector& other) const;

		void operator+=(const Vector& other);
		void operator-=(const Vector& other);
		void operator*=(f32 scalar);
		void operator/=(f32 scalar);

		Vector operator+(const Vector& other) const;
		Vector operator-(const Vector& other) const;
		Vector operator*(f32 scalar) const;
		Vector operator/(f32 scalar) const;

	private:
		struct Data {
			f32 x;
			f32 y;
		} data;

		explicit Vector(Data data);

	public:
		[[nodiscard]] static Vector zero();
		[[nodiscard]] static Vector unit_x();
		[[nodiscard]] static Vector unit_y();
		[[nodiscard]] static Vector from_angle(f32 angle);
		[[nodiscard]] static Vector from_polar_coordinates(f32 magnitude, f32 angle);
		[[nodiscard]] static Vector from_cartesian_coordinates(f32 x, f32 y);
	};
}