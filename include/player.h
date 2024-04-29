#pragma once

#include "vector.h"

namespace jp::cg {
	class Player {
	public:
		[[nodiscard]] Vector get_position() const;
		[[nodiscard]] Vector get_velocity() const;

		void set_position(const Vector& position);
		void set_velocity(const Vector& velocity);

	private:
		struct Data {
			Vector position;
			Vector velocity;
		} data;

		explicit Player(Data data);

	public:
		[[nodiscard]] static Player create();

	};
}