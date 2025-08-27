#include "pch.h"
#include "GodMode.h"
#include <bitset>

uint32_t GodMode::ReadGodBits() {
    uintptr_t GodBitsAddress = DMA::LocalPlayerAddress + offsetof(PED, GodFlags);
    uint32_t OriginalBits = 0;
    VMMDLL_MemReadEx(DMA::vmh, DMA::PID, GodBitsAddress, (BYTE*)&OriginalBits, sizeof(uint32_t), &BytesRead, VMMDLL_FLAG_NOCACHE);
    if (BytesRead != sizeof(uint32_t)) {
        std::println("ReadGodBits failed in {}", __FUNCTION__);
        return 0;
    }
    return OriginalBits;
}

bool GodMode::OnDMAFrame() {
    uint32_t currentBits = ReadGodBits();
    if (BytesRead != sizeof(uint32_t)) {
        return false; // Read failed
    }

    std::bitset<16> bits(currentBits);
    bool isGodModeActive = bits.test(4) && bits.test(8);

    if (bEnable) {
        if (!isGodModeActive) {
            return Enable();
        }
    }
    else {
        if (isGodModeActive) {
            return Disable();
        }
    }
    return true;
}

bool GodMode::Enable() {
    uint32_t OriginalBits = ReadGodBits();
    if (BytesRead != sizeof(uint32_t)) {
        std::println("Enable failed to read bits.");
        return false;
    }

    std::bitset<16> GodBits(OriginalBits);
    GodBits.set(4);
    GodBits.set(8);

    uint32_t Bits = GodBits.to_ulong();
    uintptr_t GodBitsAddress = DMA::LocalPlayerAddress + offsetof(PED, GodFlags);
    VMMDLL_MemWrite(DMA::vmh, DMA::PID, GodBitsAddress, (BYTE*)&Bits, sizeof(uint32_t));

    return true;
}

bool GodMode::Disable() {
    uint32_t OriginalBits = ReadGodBits();
    if (BytesRead != sizeof(uint32_t)) {
        std::println("Disable failed to read bits.");
        return false;
    }

    std::bitset<16> GodBits(OriginalBits);
    GodBits.reset(4);
    GodBits.reset(8);

    uint32_t Bits = GodBits.to_ulong();
    uintptr_t GodBitsAddress = DMA::LocalPlayerAddress + offsetof(PED, GodFlags);
    VMMDLL_MemWrite(DMA::vmh, DMA::PID, GodBitsAddress, (BYTE*)&Bits, sizeof(uint32_t));

    return true;
}