#pragma once

#include "fx.h"

namespace jp::cg {
	class Arena {
	public:
		[[nodiscard]] f32 get_radius() const;

		void set_radius(f32 radius);

	private:
		struct Data {
			f32 radius = 0.0;
		} data;

		explicit Arena(Data data);

	public:
		[[nodiscard]] static Arena create();

	};
}