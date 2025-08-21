#pragma once
#include <cstdint>

namespace Offsets
{
	static const uintptr_t WorldPtr = 0x43dbc98;	     // 48 8B 0D ? ? ? ? 49 8B 9E
	static const uintptr_t GlobalPtr = 0x47c87a8;		 // 48 8B 0D ? ? ? ? 0F 1F 44 00
	static const uintptr_t BlipPtr = 0x3e7bf50;			 // 4C 8D 3D ? ? ? ? 49 8B 34 C7
}