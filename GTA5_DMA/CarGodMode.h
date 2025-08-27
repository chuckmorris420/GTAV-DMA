#pragma once
#include <chrono>

class CarGodMode {
public:
    static inline bool bEnable = false;         // Master switch for the feature
    static inline bool bWasInVehicle = false;   // Track previous vehicle state
    static inline std::chrono::steady_clock::time_point LastCheckTime;
    static inline const std::chrono::milliseconds CheckInterval{ 750 };
    static inline DWORD BytesRead = 0;

public:
    static bool OnDMAFrame();
    static bool VehicleSet(bool GodMode);
};