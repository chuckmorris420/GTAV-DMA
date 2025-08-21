#include "pch.h"

#include "WeaponInspector.h"

bool WeaponInspector::Render() {

	ImGui::Begin("Weapon Inspector", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::SeparatorText("Current Weapon Info");

	ImGui::Text("Damage: %.2f", WepInfo.WeaponDamage);
	ImGui::Text("Fire Rate: %.2f", WepInfo.WeaponFireRate);
	ImGui::Text("Range: %.2f", WepInfo.WeaponRange);
	ImGui::Text("Penetration: %.2f", WepInfo.WeaponPenetration);
	ImGui::Text("Recoil: %.2f", WepInfo.RecoilAmplitude);
	ImGui::Text("Impact Type: %d", WepInfo.ImpactType);
	ImGui::Text("Impact Explosion: %d", WepInfo.ImpactExplosion);
	ImGui::Checkbox("Infinite Ammo", &bInfiniteAmmo);
	ImGui::SameLine();
	ImGui::Checkbox("No Reload", &bNoReload);

	ImGui::SeparatorText("Overwrite Values");

	ImGui::InputFloat("Weapon Damage", &DesiredWepInfo.WeaponDamage);
	ImGui::InputFloat("Weapon Fire Rate", &DesiredWepInfo.WeaponFireRate);
	ImGui::InputFloat("Weapon Range", &DesiredWepInfo.WeaponRange);
	ImGui::InputFloat("Weapon Penetration", &DesiredWepInfo.WeaponPenetration);
	ImGui::InputFloat("Recoil Amplitude", &DesiredWepInfo.RecoilAmplitude);
	ImGui::InputInt("Impact Type", (int*)&DesiredWepInfo.ImpactType);
	ImGui::InputInt("Impact Explosion", (int*)&DesiredWepInfo.ImpactExplosion);

	if (ImGui::Button("Update"))
		bNeedsOverwrite = true;

	ImGui::SameLine();

	if (ImGui::Button("Copy Current To Desired"))
		bRequestedCopyToDesired = true;

	ImGui::End();

	return 1;
}

bool WeaponInspector::OnDMAFrame()
{
	if (!UpdateEssentials())
	{
		std::println("{} failed!",__FUNCTION__);
		return 0;
	}

	if (bNeedsOverwrite)
	{
		UpdateCurrentWeapon();
		bNeedsOverwrite = false;
	}

	if (bRequestedCopyToDesired)
	{
		DesiredWepInfo = WepInfo;
		bRequestedCopyToDesired = false;
	}

	if (bNoReload && !bPrevNoReload)
		EnableNoReload();
	else if (!bNoReload && bPrevNoReload)
		DisableNoReload();

	if (bInfiniteAmmo && !bPrevInfiniteAmmo)
		EnableInfAmmo();
	else if (!bInfiniteAmmo && bPrevInfiniteAmmo)
		DisableInfAmmo();

	return 1;
}

bool WeaponInspector::UpdateEssentials()
{
	if (!UpdateWeaponInfo())
		return 0;

	if (!UpdateWeaponInventory())
		return 0;

	return 1;
}

bool WeaponInspector::UpdateCurrentWeapon()
{
	if (!DMA::WeaponInfoAddress)
	{
		std::println("Cannot update weapon info when in vehicle.");
		return 0;
	}

	DWORD BytesRead = 0x0;

	WeaponInfo LocalWeaponInfo;
	ZeroMemory(&LocalWeaponInfo, sizeof(WeaponInfo));

	VMMDLL_MemReadEx(DMA::vmh, DMA::PID, DMA::WeaponInfoAddress, (BYTE*)&LocalWeaponInfo, sizeof(WeaponInfo), &BytesRead, VMMDLL_FLAG_NOCACHE);

	if (BytesRead != sizeof(WeaponInfo))
		return 0;

	LocalWeaponInfo.WeaponDamage = DesiredWepInfo.WeaponDamage;
	LocalWeaponInfo.WeaponFireRate = DesiredWepInfo.WeaponFireRate;
	LocalWeaponInfo.WeaponRange = DesiredWepInfo.WeaponRange;
	LocalWeaponInfo.WeaponPenetration = DesiredWepInfo.WeaponPenetration;
	LocalWeaponInfo.ImpactType = DesiredWepInfo.ImpactType;
	LocalWeaponInfo.ImpactExplosion = DesiredWepInfo.ImpactExplosion;
	LocalWeaponInfo.RecoilAmplitude = DesiredWepInfo.RecoilAmplitude;

	VMMDLL_MemWrite(DMA::vmh, DMA::PID, DMA::WeaponInfoAddress, (BYTE*)&LocalWeaponInfo, sizeof(WeaponInfo));

	return 1;
}

bool WeaponInspector::UpdateWeaponInventory()
{
	uintptr_t WeaponInvPtr = DMA::LocalPlayerAddress + offsetof(PED, pCWeaponInventory);

	VMMDLL_MemReadEx(DMA::vmh, DMA::PID, WeaponInvPtr, (BYTE*)&WeaponInvAddress, sizeof(uintptr_t), &BytesRead, VMMDLL_FLAG_NOCACHE);

	if (BytesRead != sizeof(uintptr_t))
	{
		std::println("{} failed reading WeaponInvPtr!", __FUNCTION__);
		return 0;
	}

	VMMDLL_MemReadEx(DMA::vmh, DMA::PID, WeaponInvAddress, (BYTE*)&WepInv, sizeof(WeaponInventory), &BytesRead, VMMDLL_FLAG_NOCACHE);

	if (BytesRead != sizeof(WeaponInventory))
	{
		std::println("{} failed reading WeaponInventory!", __FUNCTION__);
		return 0;
	}

	return 1;
}

bool WeaponInspector::UpdateWeaponInfo()
{
	if (!DMA::WeaponInfoAddress)
	{
		ZeroMemory(&WepInfo, sizeof(WeaponInfo));
		return 1;
	}

	DWORD BytesRead = 0x0;

	VMMDLL_MemReadEx(DMA::vmh, DMA::PID, DMA::WeaponInfoAddress, (BYTE*)&WepInfo, sizeof(WeaponInfo), &BytesRead, VMMDLL_FLAG_NOCACHE);

	if (BytesRead != sizeof(WeaponInfo))
	{
		std::println("{} failed reading WeaponInfo!", __FUNCTION__);
		return 0;
	}

	return 1;
}

bool WeaponInspector::EnableInfAmmo()
{
	std::bitset<8>Bits(WepInv.AmmoModifier);
	Bits.set(0);

	uint8_t NewBits = Bits.to_ulong();

	uintptr_t AmmoModifierAddress = WeaponInvAddress + offsetof(WeaponInventory, AmmoModifier);

	VMMDLL_MemWrite(DMA::vmh, DMA::PID, AmmoModifierAddress, (BYTE*)&NewBits, sizeof(uint8_t));

	bPrevInfiniteAmmo = true;

	return 1;
}

bool WeaponInspector::DisableInfAmmo()
{
	std::bitset<8>Bits(WepInv.AmmoModifier);
	Bits.reset(0);

	uint8_t NewBits = Bits.to_ulong();

	uintptr_t AmmoModifierAddress = WeaponInvAddress + offsetof(WeaponInventory, AmmoModifier);

	VMMDLL_MemWrite(DMA::vmh, DMA::PID, AmmoModifierAddress, (BYTE*)&NewBits, sizeof(uint8_t));

	bPrevInfiniteAmmo = false;

	return 1;
}

bool WeaponInspector::EnableNoReload()
{
	std::bitset<8>Bits(WepInv.AmmoModifier);
	Bits.set(1);

	uint8_t NewBits = Bits.to_ulong();

	uintptr_t AmmoModifierAddress = WeaponInvAddress + offsetof(WeaponInventory, AmmoModifier);

	VMMDLL_MemWrite(DMA::vmh, DMA::PID, AmmoModifierAddress, (BYTE*)&NewBits, sizeof(uint8_t));

	bPrevNoReload = true;

	return 1;
}

bool WeaponInspector::DisableNoReload()
{
	std::bitset<8>Bits(WepInv.AmmoModifier);
	Bits.reset(1);

	uint8_t NewBits = Bits.to_ulong();

	uintptr_t AmmoModifierAddress = WeaponInvAddress + offsetof(WeaponInventory, AmmoModifier);

	VMMDLL_MemWrite(DMA::vmh, DMA::PID, AmmoModifierAddress, (BYTE*)&NewBits, sizeof(uint8_t));

	bPrevNoReload = false;

	return 1;
}