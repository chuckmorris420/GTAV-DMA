#pragma once
#include "DMA.h"
class CarControl
{
public:
    static inline bool bEnable = false;
    static inline WORD OriginalValue = 0;
    static inline bool bOriginalStored = false;
    static inline WORD CurrentValue = 0x0360; // Default to "All" features
    static void OnDMAFrame();
};