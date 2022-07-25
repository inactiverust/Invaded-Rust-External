#include "includes.h"

#include "EPROCESS.hpp"
#include "ETHREAD.hpp"
#include "KPROCESS.hpp"
#include "KTHREAD.hpp"
#include "PspCidTable.hpp"

#include "utilities.hpp"

#define release false;

_CEPROCESS* find_process_by_name(const char* proc_name)
{
	PEPROCESS system_proc;
	PsLookupProcessByProcessId((HANDLE)4, &system_proc);
	_LIST_ENTRY* system_list_entry = (_LIST_ENTRY*)((uintptr_t)system_proc + EPROCESS::ActiveProcessLinks);
	_LIST_ENTRY* current = system_list_entry;
	while (current->Flink != system_list_entry)
	{
		current = current->Flink;
		_CEPROCESS* current_proc = (_CEPROCESS*)((uintptr_t)current - EPROCESS::ActiveProcessLinks);
		if (current_proc->compare_image_name(proc_name))
			return current_proc;
	}
	return NULL;
}


_CETHREAD* find_desired_thread(_CEPROCESS* process)
{
	_LIST_ENTRY* desired_list_entry = process->get_thread_list_head()->Flink;
	return (_CETHREAD*)((uintptr_t)desired_list_entry - ETHREAD::ThreadListEntry);
}

#if release
DRIVER_UNLOAD UnloadDriver;

extern "C" NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT  DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
	DriverObject->DriverUnload = UnloadDriver;

	_CEPROCESS* desired_proc = find_process_by_name("loader.exe");

	if (!desired_proc)
		return STATUS_SUCCESS;

	_CETHREAD* desired_thread = find_desired_thread(desired_proc);

	desired_thread->get_kthread()->set_previous_mode(0);

	desired_thread->remove_thread_list_entry();

	PspCidTable::hide_psp_table(desired_thread->get_tid());

	return STATUS_SUCCESS;
}

VOID UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	return;
}
#else
DRIVER_UNLOAD UnloadDriver;

extern "C" NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT  DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
	DriverObject->DriverUnload = UnloadDriver;

	_CEPROCESS* desired_proc = find_process_by_name("usermode.exe");

	if (desired_proc)
	{
		_CETHREAD* desired_thread = find_desired_thread(desired_proc);

		desired_thread->get_kthread()->set_previous_mode(0);
	}

	return STATUS_SUCCESS;
}

VOID UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	return;
}
#endif