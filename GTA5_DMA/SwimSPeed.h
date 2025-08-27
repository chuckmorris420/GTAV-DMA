#pragma once

class SwimSpeed
{
public:
    static inline bool bEnable = false;
    static inline float DesiredSpeed = 1.0f; // Default swim speed multiplier

    static void OnDMAFrame();
};