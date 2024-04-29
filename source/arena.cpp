#include "arena.h"

namespace jp::cg {
	f32 Arena::get_radius() const {
		return data.radius;
	}

	void Arena::set_radius(f32 radius) {
		data.radius = radius;
	}

	Arena::Arena(Arena::Data data) : data(data) {

	}

	Arena Arena::create() {
		return Arena({});
	}
}