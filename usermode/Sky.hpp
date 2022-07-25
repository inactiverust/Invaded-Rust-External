#pragma once

#include "common.hpp"
#include "offsets.hpp"
#include "memory.hpp"

class Sky
{
public:
	void set_ambient_mult_night(float mult)
	{
		uintptr_t night = memory::read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + oNight);
		memory::write<float>(night + oAmbientMult, mult);
	}
	void set_ambient_mult_day(float mult)
	{
		uintptr_t night = memory::read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + oDay);
		memory::write<float>(night + oAmbientMult, mult);
	}
	void set_ambient()
	{
		uintptr_t ambient = memory::read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + 0x90);
		memory::write<float>(ambient + 0x18, 0.1);
	}
private:
};