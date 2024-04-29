#include "circleobstacle.h"

namespace jp::cg {
	CircleObstacle CircleObstacle::create(f32 radius, Vector center) {
		return CircleObstacle({radius, center});
	}

	CircleObstacle::CircleObstacle(CircleObstacle::Data data) : data(data) {

	}

	Vector CircleObstacle::get_center() const {
		return data.center;
	}

	f32 CircleObstacle::get_radius() const {
		return data.radius;
	}
}
