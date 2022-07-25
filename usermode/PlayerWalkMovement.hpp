#pragma once

#include "common.hpp"
#include "offsets.hpp"
#include "memory.hpp"

class PlayerWalkMovement {
public:
	void set_ground_angles(float angle)
	{
		memory::write<float>(reinterpret_cast<uintptr_t>(this) + oGroundAngle, angle);
		memory::write<float>(reinterpret_cast<uintptr_t>(this) + oGroundAngleNew, angle);
	}

	void set_jump_time(float time) { memory::write<float>(reinterpret_cast<uintptr_t>(this) + oJumpTime, time); }

	void set_land_time(float time) { memory::write<float>(reinterpret_cast<uintptr_t>(this) + oLandTime, time); }

	void set_ground_time(float time) { memory::write<float>(reinterpret_cast<uintptr_t>(this) + oGroundTime, time); }

	void set_gravity_multiplier(float mult) { memory::write<float>(reinterpret_cast<uintptr_t>(this) + oGravity, mult); }

	void set_gravity_multiplier_swimming(float mult) { memory::write<float>(reinterpret_cast<uintptr_t>(this) + oGravitySwimming, mult); }
private:
};