#include "player.h"

namespace jp::cg {
	Vector Player::get_position() const {
		return data.position;
	}

	Vector Player::get_velocity() const {
		return data.velocity;
	}

	void Player::set_position(const Vector& position) {
		data.position = position;
	}

	void Player::set_velocity(const Vector& velocity) {
		data.velocity = velocity;
	}

	Player::Player(Player::Data data) : data(data) {

	}

	Player Player::create() {
		return Player({Vector::zero(), Vector::zero()});
	}
}