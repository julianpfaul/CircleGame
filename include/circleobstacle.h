#pragma once

#include "fx.h"
#include "vector.h"

namespace jp::cg {
	class CircleObstacle {
	public:
		[[nodiscard]] f32 get_radius() const;
		[[nodiscard]] Vector get_center() const;

	private:
		struct Data {
			f32 radius;
			Vector center;
		} data;

		explicit CircleObstacle(Data data);

	public:
		static CircleObstacle create(f32 radius, Vector center);

	};
}