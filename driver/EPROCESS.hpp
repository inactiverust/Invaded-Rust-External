#pragma once
#include "includes.h"

class _CEPROCESS
{
public:
	_LIST_ENTRY* get_active_processes()
	{
		return reinterpret_cast<_LIST_ENTRY*>(this + EPROCESS::ActiveProcessLinks);
	}

	_LIST_ENTRY* get_thread_list_head()
	{
		return reinterpret_cast<_LIST_ENTRY*>(this + EPROCESS::ThreadListHead);
	}

	UCHAR* get_image_name()
	{
		return reinterpret_cast<UCHAR*>(this + EPROCESS::ImageFileName);
	}

	BOOLEAN compare_image_name(const char* desired_name)
	{
		UCHAR* current_name = get_image_name();
		for (int i = 0; i < sizeof(desired_name) / sizeof(char); i++)
		{
			if (current_name[i] != desired_name[i])
				return false;
		}
		return true;
	}
private:
};