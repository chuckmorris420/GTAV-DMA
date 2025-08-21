#pragma once

class GodMode {
public:
	static inline bool bRequestedGodmode = false;
	static inline bool bPlayerGodMode = false;
	static inline bool bVehicleGodMode = false;

	static inline DWORD BytesRead = 0x0;

public:
	static bool OnDMAFrame();

	static bool PlayerSet(bool GodMode);
	static bool VehicleSet(bool GodMode);

	static bool VehicleEnable();
	static bool VehicleDisable();

};