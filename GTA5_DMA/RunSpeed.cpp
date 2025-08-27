#include "pch.h"
#include "RunSpeed.h"
#include "DMA.h"

void RunSpeed::OnDMAFrame()
{
    if (!bEnable || !DMA::PlayerInfoAddress)
        return;

    // Calculate RunSpeed address (PlayerInfo + 0x0D50)
    uintptr_t RunSpeedAddress = DMA::PlayerInfoAddress + offsetof(PlayerInfo, RunSpeed);

    // Write the desired speed value
    VMMDLL_MemWrite(DMA::vmh, DMA::PID, RunSpeedAddress, (BYTE*)&DesiredSpeed, sizeof(float));
}

