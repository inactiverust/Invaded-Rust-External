#pragma once

#include "common.hpp"
#include "memory.hpp"
#include "offsets.hpp"
#include "math.hpp"
#include "List.hpp"

class PlayerModel {
public:
	void set_flag(int flag)
	{
		uintptr_t model_state = memory::read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + oModelState);
		memory::write<int>(model_state + oModelFlags, flag);
	}
	void set_cham(uint64_t mat)
	{
		uintptr_t skinned_multi_mesh = memory::read<uintptr_t>((uintptr_t)this + 0x2d0);
		Sleep(1);
		uintptr_t list_renderer = memory::read<uintptr_t>(skinned_multi_mesh + 0x70);
		Sleep(1);
		uintptr_t list_base = memory::read<uintptr_t>(list_renderer + 0x10);
		Sleep(1);
		int sz = memory::read<int>(list_base + 0x18);
		Sleep(1);
		std::vector<uintptr_t> renderer_list = List::get_list(list_base, sz);
		Sleep(1);
		for (auto& renderer : renderer_list)
		{
			uintptr_t pMaterialDict = memory::read<uintptr_t>(renderer + 0x10);
			Sleep(1);
			int count = memory::read<int>(pMaterialDict + 0x150);
			Sleep(1);
			if (count > 0 && count < 5)
			{
				uint64_t MaterialDictionaryBase = memory::read<uint64_t>(pMaterialDict + 0x140);
				Sleep(1);
				if (!MaterialDictionaryBase)
					continue;
				for (int l = 0; l < count; l++)
				{
					Sleep(1);
					memory::write<uint64_t>(MaterialDictionaryBase + (l * 0x50), mat);
				}
				Sleep(1);
			}
		}
	}
	bool is_local_player() { return memory::read<bool>(reinterpret_cast<uintptr_t>(this) + oIsLocalPlayer); }
	bool is_npc() { return memory::read<bool>(reinterpret_cast<uintptr_t>(this) + oIsNPC); }
	bool is_visible() { return memory::read<bool>(reinterpret_cast<uintptr_t>(this) + oIsVisible); }
	Vector3 get_position() { return(memory::read<Vector3>(reinterpret_cast<uintptr_t>(this) + oPosition)); }
private:
};