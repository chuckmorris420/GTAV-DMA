#include "pch.h"
#include "CarControl.h"
#include "DMA.h"

void CarControl::OnDMAFrame()
{
    if (!DMA::LocalPlayerAddress || !DMA::VehicleAddress)
        return;

    try {
        uintptr_t pCModelInfo = 0;
        DWORD BytesRead = 0;
        constexpr DWORD CMODELINFO_OFFSET = 0x20; // Offset in CVehicle

        // Read pCModelInfo from CVehicle
        VMMDLL_MemReadEx(DMA::vmh, DMA::PID, DMA::VehicleAddress + CMODELINFO_OFFSET,
            (BYTE*)&pCModelInfo, sizeof(uintptr_t), &BytesRead, VMMDLL_FLAG_NOCACHE);

        if (!pCModelInfo || BytesRead != sizeof(uintptr_t))
            throw std::runtime_error("Failed to read pCModelInfo");

        constexpr DWORD OV_EXTRAS_OFFSET = 0x58B;
        uintptr_t targetAddress = pCModelInfo + OV_EXTRAS_OFFSET;

        if (bEnable) {
            // Store original value once
            if (!bOriginalStored) {
                VMMDLL_MemReadEx(DMA::vmh, DMA::PID, targetAddress,
                    (BYTE*)&OriginalValue, sizeof(WORD), &BytesRead, VMMDLL_FLAG_NOCACHE);
                if (BytesRead != sizeof(WORD))
                    throw std::runtime_error("Failed to read original value");
                bOriginalStored = true;
            }

            // Write new value
            VMMDLL_MemWrite(DMA::vmh, DMA::PID, targetAddress, (BYTE*)&CurrentValue, sizeof(WORD));
        }
        else if (bOriginalStored) {
            // Restore original value
            VMMDLL_MemWrite(DMA::vmh, DMA::PID, targetAddress, (BYTE*)&OriginalValue, sizeof(WORD));
            bOriginalStored = false;
        }
    }
    catch (const std::exception& e) {
        std::println("CarControl error: {}", e.what());
        bEnable = false;
    }
}

