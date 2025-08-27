#pragma once

class LSCForcer
{
public:
    static inline bool bEnable = false;
    static inline DWORD OriginalHash = 0;
    static inline bool bOriginalStored = false;
    static inline int NewHash = 1349725314; // Use signed integer for decimal input 
    static void OnDMAFrame();
};