#pragma once
#include <chrono>  // Add this include

class GodMode {
public:
    static inline bool bEnable = false;
    static inline DWORD BytesRead = 0;

    // Timer-related members
    static inline std::chrono::steady_clock::time_point LastCheckTime;
    static inline const std::chrono::seconds CheckInterval{ 30 };

public:
    static bool OnDMAFrame();
    static bool Enable();
    static bool Disable();

private:
    static uint32_t ReadGodBits();
};