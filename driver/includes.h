using uint8_t = unsigned char;
using uint16_t = unsigned short;
using uint32_t = unsigned int;
using uint64_t = unsigned long long;
typedef uint16_t WORD;
using BYTE = uint8_t;

#include <ntifs.h>
#include <ntddk.h>
#include <ntstrsafe.h>
#include <ifdef.h>
#include <ndis.h>
#include <intrin.h>

#include "offsets.hpp"