#pragma once
#include "DMA.h"

class CarInspector
{
public: /* Interface methods */
	static bool Render();
	static bool OnDMAFrame();
	static bool UpdateEssentials();

public: /* Interface variables */
	static inline bool bEnable = false;
	static bool IsPlayerInVehicle();

private: /* Private functions */
	static bool UpdateCurrentVehicle();
	static bool UpdateVehicleInfo();
	static bool RevertToOriginalValues();

private: /* 'Essentials' */
	static inline CHandlingData VehicleHandling;
	static inline CVehicle Vehicle;
	static inline CHandlingData OriginalVehicleHandling; // Stores original values before modification

private: /* Remaining private variables */
	static inline CHandlingData DesiredVehicleHandling;
	static inline CVehicle DesiredVehicle;
	static inline DWORD BytesRead = 0x0;
	static inline bool bNeedsOverwrite = false;
	static inline bool bRequestedCopyToDesired = false;
	static inline bool bHasOriginalValues = false; // Flag to track if we've stored original values
	static inline bool bNeedsRevert = false; // Flag to indicate revert was requested
};