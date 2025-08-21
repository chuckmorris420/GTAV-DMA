#include "pch.h"

#include "GodMode.h"

bool GodMode::OnDMAFrame()
{
	if (bRequestedGodmode)
	{
		PlayerSet(bPlayerGodMode);
		VehicleSet(bVehicleGodMode);
		bRequestedGodmode = 0;
	}

	return 1;
}

bool GodMode::PlayerSet(bool GodMode)
{
	uintptr_t GodBitsAddress = DMA::LocalPlayerAddress + offsetof(PED, GodFlags);

	uint32_t OriginalBits = 0x0;

	VMMDLL_MemReadEx(DMA::vmh, DMA::PID, GodBitsAddress, (BYTE*)&OriginalBits, sizeof(uint32_t), &BytesRead, VMMDLL_FLAG_NOCACHE);

	if (BytesRead != sizeof(uint32_t))
	{
		std::println("{} failed!", __FUNCTION__);
		return 0;
	}

	std::bitset<16>GodBits(OriginalBits);

	if (GodMode)
	{
		GodBits.set(4);
		GodBits.set(8);
	}
	else 
	{
		GodBits.reset(4);
		GodBits.reset(8);
	}

	uint32_t Bits = GodBits.to_ulong();

	VMMDLL_MemWrite(DMA::vmh, DMA::PID, GodBitsAddress, (BYTE*)&Bits, sizeof(uint32_t));

	return 1;
}

bool GodMode::VehicleSet(bool GodMode)
{
	uintptr_t GodBitsAddress = DMA::VehicleAddress + offsetof(CVehicle, GodBits);

	uint32_t OriginalBits = 0x0;

	VMMDLL_MemReadEx(DMA::vmh, DMA::PID, GodBitsAddress, (BYTE*)&OriginalBits, sizeof(uint32_t), &BytesRead, VMMDLL_FLAG_NOCACHE);

	if (BytesRead != sizeof(uint32_t))
	{
		std::println("{} failed!", __FUNCTION__);
		return 0;
	}

	std::bitset<16>GodBits(OriginalBits);

	if (GodMode)
	{
		GodBits.set(4);
		GodBits.set(8);
	}
	else 
	{
		GodBits.reset(4);
		GodBits.reset(8);
	}

	uint32_t Bits = GodBits.to_ulong();

	VMMDLL_MemWrite(DMA::vmh, DMA::PID, GodBitsAddress, (BYTE*)&Bits, sizeof(uint32_t));

	return 1;
}