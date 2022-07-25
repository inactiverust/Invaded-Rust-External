#pragma once

#include "common.hpp"
#include "memory.hpp"

class AdminConVar {
public:
	void set_admin_time(float time) { memory::write<float>(reinterpret_cast<uintptr_t>(this) + 0x0, time); }
private:
};