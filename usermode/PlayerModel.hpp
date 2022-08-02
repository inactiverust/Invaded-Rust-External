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
		uintptr_t list_renderer = memory::read<uintptr_t>(skinned_multi_mesh + 0x70);
		uintptr_t list_base = memory::read<uintptr_t>(list_renderer + 0x10);
		int sz = memory::read<int>(list_base + 0x18);
		std::vector<uintptr_t> renderer_list = List::get_list(list_base, sz);
		for (auto& entry : renderer_list)
		{
			uintptr_t pMaterialDict = memory::read<uintptr_t>(entry + 0x10);
			int count = memory::read<int>(pMaterialDict + 0x150);
			if (count > 0 && count < 40)
			{
				uint64_t MaterialDictionaryBase = memory::read<uint64_t>(pMaterialDict + 0x140);
				if (!MaterialDictionaryBase)
					continue;
				for (int l = 0; l < count; l++)
				{
					memory::write<uint64_t>(MaterialDictionaryBase + (l * 0x50), mat);
				}
			}
		}
	}
	bool is_local_player() { return memory::read<bool>(reinterpret_cast<uintptr_t>(this) + oIsLocalPlayer); }
	bool is_npc() { return memory::read<bool>(reinterpret_cast<uintptr_t>(this) + oIsNPC); }
	bool is_visible() { return memory::read<bool>(reinterpret_cast<uintptr_t>(this) + oIsVisible); }
	void set_skin(uintptr_t material)
	{
		int skin_type = memory::read<int>(reinterpret_cast<uintptr_t>(this) + oSkinType); // 0 if male 1 if female
		uintptr_t skin_set_collection = memory::read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + skin_type * 0x8);
		uintptr_t skin_list = memory::read<uintptr_t>(skin_set_collection + 0x18);
		int skin_list_sz = memory::read<int>(skin_list + 0x18);
		for (int i = 0; i < skin_list_sz; i++)
		{
			uintptr_t current_skin = memory::read<uintptr_t>(skin_list + 0x20 + (i * 0x8));
			memory::write<uintptr_t>(current_skin + 0x68, material);
			memory::write<uintptr_t>(current_skin + 0x70, material);
			memory::write<uintptr_t>(current_skin + 0x78, material);
		}
	}
	Vector3 get_position() { return(memory::read<Vector3>(reinterpret_cast<uintptr_t>(this) + oPosition)); }
private:
};