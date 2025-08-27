#include "pch.h"
#include "Seatbelt.h"
#include "DMA.h"

bool Seatbelt::OnDMAFrame()
{
    if (!bEnable) return true;

    auto now = std::chrono::steady_clock::now();
    if (now - LastCheckTime < CheckInterval) return true;
    LastCheckTime = now;

    // Check if player is in vehicle
    bool isInVehicle = false;
    if (DMA::LocalPlayerAddress)
    {
        uint8_t inVehicleBits = 0;
        uintptr_t vehicleFlagAddr = DMA::LocalPlayerAddress + offsetof(PED, InVehicleBits);
        VMMDLL_MemReadEx(DMA::vmh, DMA::PID, vehicleFlagAddr, (BYTE*)&inVehicleBits, sizeof(uint8_t), nullptr, 0);
        isInVehicle = (inVehicleBits & 0x01); // Adjust mask if needed
    }

    // Toggle seatbelt when vehicle status changes
    if (isInVehicle != bWasInVehicle)
    {
        ToggleSeatbelt(isInVehicle);
        bWasInVehicle = isInVehicle;
    }

    return true;
}

bool Seatbelt::ToggleSeatbelt(bool state)
{
    if (!DMA::LocalPlayerAddress) return false;

    // Seatbelt address
    uintptr_t seatbeltAddr = DMA::LocalPlayerAddress + 0x143C;

    // Write value
    uint8_t value = state ? 0xC9 : 0xC8; // -55/-56 in Lua
    VMMDLL_MemWrite(DMA::vmh, DMA::PID, seatbeltAddr, (BYTE*)&value, sizeof(uint8_t));

    return true;
}