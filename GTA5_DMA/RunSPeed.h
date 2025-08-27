#pragma once

class RunSpeed
{
public:
    static inline bool bEnable = false;
    static inline float DesiredSpeed = 1.0f; // Default speed multiplier

    static void OnDMAFrame();
};
