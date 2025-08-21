#include "pch.h"

#include "MyMenu.h"
#include "MyImGui.h"
#include "Features.h"

bool MyMenu::Render(HWND hwnd)
{
	ImGui::Begin("CyNickal's GTA Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize);;

	ImGui::SeparatorText("Window Toggles");

	if (ImGui::Button(g_IsFullscreen ? "Exit Fullscreen" : "Go Fullscreen"))
	{
		ToggleFullscreen(hwnd);
	}

	ImGui::Checkbox("Weapon Inspector", &WeaponInspector::bEnable);

	// Weapon Inspector section
	if (WeaponInspector::bEnable)
		WeaponInspector::Render();

	ImGui::Checkbox("Teleport", &Teleport::bEnable);

	ImGui::SeparatorText("Feature Toggles");

	ImGui::Checkbox("Refresh Health", &RefreshHealth::bEnable);

	ImGui::Checkbox("Never Wanted", &NoWanted::bEnable);

	ImGui::SeparatorText("God Mode");
	ImGui::Checkbox("Player", &GodMode::bPlayerGodMode);
	ImGui::SameLine();
	ImGui::Checkbox("Vehicle", &GodMode::bVehicleGodMode);
	ImGui::Indent();
	if (ImGui::Button("Update"))
		GodMode::bRequestedGodmode = 1;
	ImGui::Unindent();

	ImGui::End();

	return 1;
}