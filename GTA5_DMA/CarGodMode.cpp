#include "pch.h"
#include "CarGodMode.h"
#include "DMA.h"

bool CarGodMode::OnDMAFrame() {
    if (!bEnable) return true;

    auto now = std::chrono::steady_clock::now();
    if (now - LastCheckTime < CheckInterval) return true;
    LastCheckTime = now;

    // Check if player is in a vehicle
    bool isInVehicle = false;
    if (DMA::LocalPlayerAddress) {
        uint8_t inVehicleBits = 0;
        uintptr_t vehicleFlagAddr = DMA::LocalPlayerAddress + offsetof(PED, InVehicleBits);
        VMMDLL_MemReadEx(DMA::vmh, DMA::PID, vehicleFlagAddr, (BYTE*)&inVehicleBits, sizeof(uint8_t), nullptr, 0);
        isInVehicle = (inVehicleBits & 0x01);
    }

    // Toggle god mode on vehicle change
    if (isInVehicle != bWasInVehicle) {
        VehicleSet(isInVehicle);
        bWasInVehicle = isInVehicle;
    }

    return true;
}

bool CarGodMode::VehicleSet(bool GodMode) {
    if (!DMA::LocalPlayerAddress) return false;

    // Read current vehicle address from player's PED
    uintptr_t pCVehicle = 0;
    uintptr_t vehiclePtrAddr = DMA::LocalPlayerAddress + offsetof(PED, pCVehicle);
    VMMDLL_MemReadEx(DMA::vmh, DMA::PID, vehiclePtrAddr, (BYTE*)&pCVehicle, sizeof(uintptr_t), &BytesRead, 0);

    if (BytesRead != sizeof(uintptr_t) || !pCVehicle) return false;

    uintptr_t GodBitsAddress = pCVehicle + offsetof(CVehicle, GodBits);
    uint32_t OriginalBits = 0;
    VMMDLL_MemReadEx(DMA::vmh, DMA::PID, GodBitsAddress, (BYTE*)&OriginalBits, sizeof(uint32_t), &BytesRead, VMMDLL_FLAG_NOCACHE);

    if (BytesRead != sizeof(uint32_t)) {
        std::println("{} failed!", __FUNCTION__);
        return false;
    }

    std::bitset<16> GodBits(OriginalBits);
    if (GodMode) {
        GodBits.set(4);
        GodBits.set(8);
    }
    else {
        GodBits.reset(4);
        GodBits.reset(8);
    }

    uint32_t Bits = GodBits.to_ulong();
    VMMDLL_MemWrite(DMA::vmh, DMA::PID, GodBitsAddress, (BYTE*)&Bits, sizeof(uint32_t));
    return true;
}