#pragma once

#include "includes.h"
#include "utilities.hpp"

typedef struct _HANDLE_TABLE_ENTRY_INFO
{
    ULONG AuditMask;                //Uint4B
    ULONG MaxRelativeAccessMask;    //Uint4b
} HANDLE_TABLE_ENTRY_INFO, * PHANDLE_TABLE_ENTRY_INFO;

typedef struct _HANDLE_TABLE_ENTRY
{
    union                                           //that special class
    {
        ULONG64 VolatileLowValue;                   //Int8B
        ULONG64 LowValue;                           //Int8B
        ULONG64 RefCountField;                      //Int8B
        _HANDLE_TABLE_ENTRY_INFO* InfoTable;        //Ptr64 _HANDLE_TABLE_ENTRY_INFO
        struct
        {
            ULONG64 Unlocked : 1;        //1Bit
            ULONG64 RefCnt : 16;       //16Bits
            ULONG64 Attributes : 3;        //3Bits
            ULONG64 ObjectPointerBits : 44;       //44Bits
        };
    };
    union
    {
        ULONG64 HighValue;                          //Int8B
        _HANDLE_TABLE_ENTRY* NextFreeHandleEntry;   //Ptr64 _HANDLE_TABLE_ENTRY
    };
} HANDLE_TABLE_ENTRY, * PHANDLE_TABLE_ENTRY;

typedef struct _HANDLE_TABLE
{
    ULONG       NextHandleNeedingPool;  //Uint4B
    LONG        ExtraInfoPages;         //Int4B
    ULONG64     TableCode;              //Uint8B 
    PEPROCESS   QuotaProcess;           //Ptr64 _EPROCESS
    _LIST_ENTRY HandleTableList;        //_LIST_ENTRY
    ULONG       UniqueProcessId;        //Uint4B
} HANDLE_TABLE, * PHANDLE_TABLE;

typedef BOOLEAN(*func)(const PHANDLE_TABLE, const HANDLE, const PHANDLE_TABLE_ENTRY);
func ExDestroyHandle;

namespace PspCidTable
{
    PHANDLE_TABLE_ENTRY ExpLookupHandleTableEntry(const ULONG64* pHandleTable, const LONGLONG Handle)
    {
        ULONGLONG v2; // rdx
        LONGLONG v3; // r8

        v2 = Handle & 0xFFFFFFFFFFFFFFFC;
        if (v2 >= *pHandleTable)
            return 0;
        v3 = *(pHandleTable + 1);
        if ((v3 & 3) == 1)
            return reinterpret_cast<PHANDLE_TABLE_ENTRY>(*reinterpret_cast<ULONG_PTR*>(v3 + 8 * (v2 >> 10) - 1) + 4 * (v2 & 0x3FF));
        if ((v3 & 3) != 0)
            return reinterpret_cast<PHANDLE_TABLE_ENTRY>(*reinterpret_cast<ULONG_PTR*>(*reinterpret_cast<ULONG_PTR*>(v3 + 8 * (v2 >> 19) - 2) + 8 * ((v2 >> 10) & 0x1FF)) + 4 * (v2 & 0x3FF));
        return reinterpret_cast<PHANDLE_TABLE_ENTRY>(v3 + 4 * v2);
    }

    void DestroyPspCidTableEntry(const ULONG64* pPspCidTable, const HANDLE threadId)
    {
        ULONG64* pHandleTable = reinterpret_cast<ULONG64*>(*pPspCidTable); //deref for pointer to handle table
        const PHANDLE_TABLE_ENTRY pCidEntry = ExpLookupHandleTableEntry(pHandleTable, reinterpret_cast<LONGLONG>(threadId));

        if (pCidEntry != NULL)
        {
            //DbgPrintEx(0, 0, "Handle table: %p\n", pHandleTable);
            //DbgPrintEx(0, 0, "Cid entry: %p\n", pCidEntry);
           // DbgPrintEx(0, 0, "ObjectPointerBits: %p\n", pCidEntry->ObjectPointerBits);

            ExDestroyHandle(reinterpret_cast<PHANDLE_TABLE>(pHandleTable), threadId, pCidEntry);

            if (pCidEntry->ObjectPointerBits == 0)
            {
                //DbgPrintEx(0, 0, "Entry should be removed removed");
                //DbgPrintEx(0, 0, "ObjectPointerBits now: %p", pCidEntry->ObjectPointerBits);
            }
        }
    }

	VOID hide_psp_table(const HANDLE tid)
	{
		SIZE_T kernel_size;
		PVOID kernel_base = utils::get_module_base("ntoskrnl", &kernel_size);
		const ULONG64* psp_cid_ptr = utils::resolve(utils::find_pattern<ULONG64>("\x4C\x8B\x35\x00\x00\x00\x00\x0F\x0D\x08", "xxx????xxx", kernel_base, kernel_size), 3, 4);
        const ULONG64* ExDestroyHandle_ptr = utils::resolve(utils::find_pattern<ULONG64>("\xE8\x00\x00\x00\x00\x49\x8B\xCC\xE8\x00\x00\x00\x00\x48\x8B\xCF", "x????xxxx????xxx", kernel_base, kernel_size), 1, 4);

       // DbgPrintEx(0, 0, "%p\n", psp_cid_ptr);
       // DbgPrintEx(0, 0, "%p\n", ExDestroyHandle_ptr);
        ExDestroyHandle = reinterpret_cast<func>(ExDestroyHandle_ptr);
        DestroyPspCidTableEntry(psp_cid_ptr, tid);
	}
}
