#pragma once
#include "common.hpp"
#include "memory.hpp"
class List {
public:
	static std::vector<uintptr_t> get_list(uintptr_t list_base, int list_size)
	{
		std::vector<uintptr_t> return_list{};
		std::unique_ptr<uintptr_t> buffer(new uintptr_t[list_size * sizeof(uintptr_t)]);
		memory::copy_memory(list_base + 0x20, reinterpret_cast<uintptr_t>(buffer.get()), list_size * sizeof(uintptr_t));
		return_list = { buffer.get(), buffer.get() + (uintptr_t)list_size };
		return return_list;
	}
};