#pragma once
#include <chrono>

class Seatbelt
{
public:
    static inline bool bEnable = false;
    static inline bool bWasInVehicle = false;

    // Timer for updates
    static inline std::chrono::steady_clock::time_point LastCheckTime;
    static inline const std::chrono::milliseconds CheckInterval{ 750 };

    static bool OnDMAFrame();
    static bool ToggleSeatbelt(bool state);
};