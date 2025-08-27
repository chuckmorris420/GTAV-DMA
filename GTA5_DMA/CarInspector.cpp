#include "pch.h"
#include "CarInspector.h"

bool CarInspector::Render() {
	ImGui::Begin("Car Inspector");

	ImGui::SeparatorText("Current Vehicle Info");

	// Check if player is in a vehicle using bit flag
	bool isInVehicle = IsPlayerInVehicle();

	// Only show vehicle info if we're in a vehicle
	if (isInVehicle && DMA::VehicleAddress) {
		ImGui::Text("Vehicle Address: 0x%llx", DMA::VehicleAddress);
		ImGui::Text("Vehicle Boost Speed: %.f", VehicleHandling.BoostSpeed);
		ImGui::Text("Vehicle Health: %.2f", Vehicle.Health);
		ImGui::Text("Mass: %.2f", VehicleHandling.Mass);
		ImGui::Text("Acceleration: %.2f", VehicleHandling.Acceleration);
		ImGui::Text("Drive Inertia: %.2f", VehicleHandling.DriveInertia);
		ImGui::Text("Up Shift Ratio: %.2f", VehicleHandling.Upshiftratio);
		ImGui::Text("Down Shift Ratio: %.2f", VehicleHandling.downshiftratio);
		ImGui::Text("Initial Drive Force: %.2f", VehicleHandling.InitialDriveForce);
		ImGui::Text("Brake Force: %.2f", VehicleHandling.BrakeForce);
		ImGui::Text("Thrust: %.2f", VehicleHandling.Thrust);
		ImGui::Text("Hand Brake: %.2f", VehicleHandling.HandbrakeForce);
		ImGui::Text("Max Steering Angle: %.2f", VehicleHandling.MaxSteeringAngle);
		ImGui::Text("Traction Curve Max: %.2f", VehicleHandling.TractionCurveMax);
		ImGui::Text("Traction Curve Min: %.2f", VehicleHandling.TractionCurveMin);
		ImGui::Text("Collision Mult: %.2f", VehicleHandling.CollisionMult);
		ImGui::Text("Weapon Damage Mult: %.2f", VehicleHandling.WeaponMult);
		ImGui::Text("Deformation Mult: %.2f", VehicleHandling.DeformationMult);
		ImGui::Text("Engine Mult: %.2f", VehicleHandling.EngineMult);
		ImGui::Text("Heli Thrust Multi: %.2f", VehicleHandling.HeliThrustMulti);
		ImGui::Text("Car Camara Distance: %.2f", VehicleHandling.CamDist);

		ImGui::SeparatorText("Overwrite Values");
		ImGui::InputFloat("Vehicle Boost Speed", &DesiredVehicleHandling.BoostSpeed);
		ImGui::InputFloat("Mass", &DesiredVehicleHandling.Mass);
		ImGui::InputFloat("Acceleration", &DesiredVehicleHandling.Acceleration);
		ImGui::InputFloat("Drive Inertia", &DesiredVehicleHandling.DriveInertia);
		ImGui::InputFloat("Up Shift Ratio", &DesiredVehicleHandling.Upshiftratio);
		ImGui::InputFloat("Down Shift Ratio", &DesiredVehicleHandling.downshiftratio);
		ImGui::InputFloat("Initial Drive Force", &DesiredVehicleHandling.InitialDriveForce);
		ImGui::InputFloat("Brake Force", &DesiredVehicleHandling.BrakeForce);
		ImGui::InputFloat("Thrust", &DesiredVehicleHandling.Thrust);
		ImGui::InputFloat("Hand Brake", &DesiredVehicleHandling.HandbrakeForce);
		ImGui::InputFloat("Max Steering Angle", &DesiredVehicleHandling.MaxSteeringAngle);
		ImGui::InputFloat("Traction Curve Max", &DesiredVehicleHandling.TractionCurveMax);
		ImGui::InputFloat("Traction Curve Min", &DesiredVehicleHandling.TractionCurveMin);
		ImGui::InputFloat("Collision Mult", &DesiredVehicleHandling.CollisionMult);
		ImGui::InputFloat("Weapon Damage Mult", &DesiredVehicleHandling.WeaponMult);
		ImGui::InputFloat("Deformation Mult", &DesiredVehicleHandling.DeformationMult);
		ImGui::InputFloat("Engine Mult", &DesiredVehicleHandling.EngineMult);
		ImGui::InputFloat("Heli Thrust Multi", &DesiredVehicleHandling.HeliThrustMulti);
		ImGui::InputFloat("Car Camara Distance", &DesiredVehicleHandling.CamDist);

		if (ImGui::Button("Update")) {
			// Store original values before first modification if not already stored
			if (!bHasOriginalValues) {
				OriginalVehicleHandling = VehicleHandling;
				bHasOriginalValues = true;
			}
			bNeedsOverwrite = true;
		}

		ImGui::SameLine();

		if (ImGui::Button("Copy Current To Desired"))
			bRequestedCopyToDesired = true;

		// Only show Revert button if we have stored original values
		if (bHasOriginalValues) {
			ImGui::SameLine();

			if (ImGui::Button("Revert Changes")) {
				bNeedsRevert = true;
			}
		}
	}
	else {
		ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Not in a vehicle");

		// Reset desired vehicle handling values when not in a vehicle
		ZeroMemory(&DesiredVehicleHandling, sizeof(CHandlingData));
		// Also reset the original values flag when exiting vehicle
		bHasOriginalValues = false;
	}

	ImGui::End();

	return true;
}

bool CarInspector::OnDMAFrame()
{
	if (!bEnable) return true;

	// Check if player is in a vehicle
	bool isInVehicle = IsPlayerInVehicle();

	if (!UpdateEssentials())
	{
		return false;
	}

	if (bNeedsOverwrite && isInVehicle && DMA::VehicleAddress)
	{
		UpdateCurrentVehicle();
		bNeedsOverwrite = false;
	}

	if (bRequestedCopyToDesired && isInVehicle && DMA::VehicleAddress)
	{
		DesiredVehicleHandling = VehicleHandling;
		bRequestedCopyToDesired = false;
	}

	if (bNeedsRevert && isInVehicle && DMA::VehicleAddress && bHasOriginalValues)
	{
		RevertToOriginalValues();
		bNeedsRevert = false;
	}

	return true;
}

bool CarInspector::UpdateEssentials()
{
	if (!DMA::VehicleAddress)
	{
		// No vehicle present, nothing to do

		ZeroMemory(&Vehicle, sizeof(CVehicle));
		ZeroMemory(&VehicleHandling, sizeof(CHandlingData));
		return true;
	}

	if (!UpdateVehicleInfo())
		return false;

	return true;
}

bool CarInspector::UpdateCurrentVehicle()
{
	if (!DMA::VehicleAddress)
	{
		return false;
	}

	// Get current vehicle handling data pointer
	uintptr_t handlingDataPtr = 0;
	DWORD bytesRead = 0;

	uintptr_t handlingPtrAddress = DMA::VehicleAddress + offsetof(CVehicle, pCHandlingData);
	VMMDLL_MemReadEx(DMA::vmh, DMA::PID, handlingPtrAddress, (BYTE*)&handlingDataPtr, sizeof(uintptr_t), &bytesRead, VMMDLL_FLAG_NOCACHE);

	if (bytesRead != sizeof(uintptr_t) || !handlingDataPtr)
	{
		std::println("{} failed reading handling data pointer!", __FUNCTION__);
		return false;
	}

	// Read current handling data
	CHandlingData currentHandling;
	VMMDLL_MemReadEx(DMA::vmh, DMA::PID, handlingDataPtr, (BYTE*)&currentHandling, sizeof(CHandlingData), &bytesRead, VMMDLL_FLAG_NOCACHE);

	if (bytesRead != sizeof(CHandlingData))
	{
		std::println("{} failed reading handling data!", __FUNCTION__);
		return false;
	}

	// Update the values we want to change
	currentHandling.Mass = DesiredVehicleHandling.Mass;
	currentHandling.Acceleration = DesiredVehicleHandling.Acceleration;
	currentHandling.DriveInertia = DesiredVehicleHandling.DriveInertia;
	currentHandling.Upshiftratio = DesiredVehicleHandling.Upshiftratio;
	currentHandling.downshiftratio = DesiredVehicleHandling.downshiftratio;
	currentHandling.InitialDriveForce = DesiredVehicleHandling.InitialDriveForce;
	currentHandling.BrakeForce = DesiredVehicleHandling.BrakeForce;
	currentHandling.Thrust = DesiredVehicleHandling.Thrust;
	currentHandling.HandbrakeForce = DesiredVehicleHandling.HandbrakeForce;
	currentHandling.MaxSteeringAngle = DesiredVehicleHandling.MaxSteeringAngle;
	currentHandling.TractionCurveMax = DesiredVehicleHandling.TractionCurveMax;
	currentHandling.TractionCurveMin = DesiredVehicleHandling.TractionCurveMin;
	currentHandling.CollisionMult = DesiredVehicleHandling.CollisionMult;
	currentHandling.WeaponMult = DesiredVehicleHandling.WeaponMult;
	currentHandling.DeformationMult = DesiredVehicleHandling.DeformationMult;
	currentHandling.EngineMult = DesiredVehicleHandling.EngineMult;
	currentHandling.HeliThrustMulti = DesiredVehicleHandling.HeliThrustMulti;
	currentHandling.CamDist = DesiredVehicleHandling.CamDist;
	// Write back the modified data
	VMMDLL_MemWrite(DMA::vmh, DMA::PID, handlingDataPtr, (BYTE*)&currentHandling, sizeof(CHandlingData));

	return true;
}

bool CarInspector::RevertToOriginalValues()
{
	if (!DMA::VehicleAddress || !bHasOriginalValues)
	{
		return false;
	}

	// Get current vehicle handling data pointer
	uintptr_t handlingDataPtr = 0;
	DWORD bytesRead = 0;

	uintptr_t handlingPtrAddress = DMA::VehicleAddress + offsetof(CVehicle, pCHandlingData);
	VMMDLL_MemReadEx(DMA::vmh, DMA::PID, handlingPtrAddress, (BYTE*)&handlingDataPtr, sizeof(uintptr_t), &bytesRead, VMMDLL_FLAG_NOCACHE);

	if (bytesRead != sizeof(uintptr_t) || !handlingDataPtr)
	{
		std::println("{} failed reading handling data pointer!", __FUNCTION__);
		return false;
	}

	// Write back the original data
	VMMDLL_MemWrite(DMA::vmh, DMA::PID, handlingDataPtr, (BYTE*)&OriginalVehicleHandling, sizeof(CHandlingData));

	// Update our current values to match original values
	VehicleHandling = OriginalVehicleHandling;
	DesiredVehicleHandling = OriginalVehicleHandling;

	// Print confirmation message
	std::println("Vehicle handling restored to original values");

	return true;
}

bool CarInspector::IsPlayerInVehicle()
{
	if (!DMA::LocalPlayerAddress) {
		return false;
	}

	uint8_t inVehicleBits = 0;
	uintptr_t vehicleFlagAddr = DMA::LocalPlayerAddress + offsetof(PED, InVehicleBits);
	DWORD bytesRead = 0;
	VMMDLL_MemReadEx(DMA::vmh, DMA::PID, vehicleFlagAddr, (BYTE*)&inVehicleBits, sizeof(uint8_t), &bytesRead, 0);

	return (inVehicleBits & 0x01);
}

bool CarInspector::UpdateVehicleInfo()
{
	// Read vehicle data
	DWORD bytesRead = 0;
	VMMDLL_MemReadEx(DMA::vmh, DMA::PID, DMA::VehicleAddress, (BYTE*)&Vehicle, sizeof(CVehicle), &bytesRead, VMMDLL_FLAG_NOCACHE);

	if (bytesRead != sizeof(CVehicle))
	{
		std::println("{} failed reading vehicle data!", __FUNCTION__);
		return false;
	}

	// Read handling data
	uintptr_t handlingDataPtr = reinterpret_cast<uintptr_t>(Vehicle.pCHandlingData);
	if (!handlingDataPtr)
	{
		std::println("{} handling data pointer is null!", __FUNCTION__);
		return false;
	}

	VMMDLL_MemReadEx(DMA::vmh, DMA::PID, (uintptr_t)handlingDataPtr, (BYTE*)&VehicleHandling, sizeof(CHandlingData), &bytesRead, VMMDLL_FLAG_NOCACHE);

	if (bytesRead != sizeof(CHandlingData))
	{
		std::println("{} failed reading handling data!", __FUNCTION__);
		return false;
	}

	return true;
}