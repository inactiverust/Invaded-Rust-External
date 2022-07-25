#pragma once

#include "common.hpp"
#include "offsets.hpp"
#include "memory.hpp"

class OcclusionCulling
{
public:
	void set_esp(bool esp)
	{
		if (esp)
			memory::write<int>(reinterpret_cast<uintptr_t>(this) + oDebugShow, 1);
		else
			memory::write<int>(reinterpret_cast<uintptr_t>(this) + oDebugShow, 0);
	}
	void disable_animals()
	{
		uintptr_t debug_pointer = memory::read_chain(reinterpret_cast<uintptr_t>(this), { 128, 0x18 });
		memory::write<int>(debug_pointer + 0x20, 131072);
	}
private:
};