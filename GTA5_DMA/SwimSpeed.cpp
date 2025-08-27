#include "pch.h"
#include "SwimSpeed.h"
#include "DMA.h"

void SwimSpeed::OnDMAFrame()
{
    if (!bEnable || !DMA::PlayerInfoAddress)
        return;

    // Calculate SwimSpeed address (PlayerInfo + 0x1C8)
    uintptr_t SwimSpeedAddress = DMA::PlayerInfoAddress + 0x1C8;

    // Write the desired speed value
    VMMDLL_MemWrite(DMA::vmh, DMA::PID, SwimSpeedAddress, (BYTE*)&DesiredSpeed, sizeof(float));
}