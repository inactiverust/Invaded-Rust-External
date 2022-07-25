#pragma once
#include "includes.h"

/* OFFSETS VALID FOR WIN10 2004 - WIN10 21H2 */
namespace EPROCESS
{
	uint64_t ActiveProcessLinks = 0x448;
	uint64_t ImageFileName = 0x5a8;
	uint64_t ThreadListHead = 0x5e0;
}
namespace ETHREAD
{
	uint64_t ThreadListEntry = 0x4e8;
	uint64_t Tcb = 0x0;
	uint64_t Cid = 0x478;
}
namespace KTHREAD
{
	uint64_t PreviousMode = 0x232;
	uint64_t Flags = 0x74;
}
