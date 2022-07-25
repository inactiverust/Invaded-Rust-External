#pragma once
#include "includes.h"
#include "KTHREAD.hpp"

class _CETHREAD
{
public:
	_CKTHREAD* get_kthread() { return reinterpret_cast<_CKTHREAD*>(this); }

	VOID remove_thread_list_entry()
	{
		_LIST_ENTRY* current_list_entry = get_list_entry();
		_LIST_ENTRY* flink = current_list_entry->Flink;
		_LIST_ENTRY* blink = current_list_entry->Blink;
		flink->Blink = blink;
		blink->Flink = flink;
	}

	_LIST_ENTRY* get_list_entry() { return reinterpret_cast<_LIST_ENTRY*>(this + ETHREAD::ThreadListEntry); }

	HANDLE get_tid()
	{
		_CLIENT_ID* Cid = reinterpret_cast<_CLIENT_ID*>(this + ETHREAD::Cid);
		return (HANDLE)Cid->UniqueThread;
	}

private:
};