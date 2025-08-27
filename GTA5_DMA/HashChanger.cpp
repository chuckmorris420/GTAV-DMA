#include "pch.h"
#include "HashChanger.h"
#include "DMA.h"

void LSCForcer::OnDMAFrame()
{
    if (!DMA::LocalPlayerAddress || !DMA::VehicleAddress)
        return;

    try {
        // Pointer chain: LocalPlayer -> pCVehicle -> pCModelInfo
        uintptr_t pCModelInfo = 0;
        DWORD BytesRead = 0;

        // Read pCModelInfo from vehicle (offset 0x20 in most versions, verify with your structures)
        constexpr DWORD CMODELINFO_OFFSET = 0x20;
        VMMDLL_MemReadEx(DMA::vmh, DMA::PID, DMA::VehicleAddress + CMODELINFO_OFFSET,
            (BYTE*)&pCModelInfo, sizeof(uintptr_t), &BytesRead, VMMDLL_FLAG_NOCACHE);

        if (!pCModelInfo || BytesRead != sizeof(uintptr_t))
            throw std::runtime_error("Failed to read pCModelInfo");

        // Get oModelHash address
        constexpr DWORD MODEL_HASH_OFFSET = 0x18;
        uintptr_t modelHashAddr = pCModelInfo + MODEL_HASH_OFFSET;

        if (bEnable) {
            // Store original value once
            if (!bOriginalStored) {
                VMMDLL_MemReadEx(DMA::vmh, DMA::PID, modelHashAddr,
                    (BYTE*)&OriginalHash, sizeof(DWORD), &BytesRead, VMMDLL_FLAG_NOCACHE);
                if (BytesRead != sizeof(DWORD))
                    throw std::runtime_error("Failed to read original hash");
                bOriginalStored = true;
            }

            // Write new value (1349725314)

            VMMDLL_MemWrite(DMA::vmh, DMA::PID, modelHashAddr, (BYTE*)&NewHash, sizeof(DWORD));
        }
        else if (bOriginalStored) {
            // Restore original value
            VMMDLL_MemWrite(DMA::vmh, DMA::PID, modelHashAddr, (BYTE*)&OriginalHash, sizeof(DWORD));
            bOriginalStored = false;
        }
    }
    catch (const std::exception& e) {
        std::println("ModelHashChanger error: {}", e.what());
        bEnable = false;
    }
}