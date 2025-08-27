#include "pch.h"

#include "MyMenu.h"
#include "MyImGui.h"
#include "Features.h"

bool MyMenu::Render(HWND hwnd)
{
	ImGui::Begin("GTA V Enhanced Edition DMA", nullptr, ImGuiWindowFlags_AlwaysAutoResize);;

	ImGui::SeparatorText("Menu Toggles");

	if (ImGui::Button(g_IsFullscreen ? "Exit Fullscreen" : "Go Fullscreen"))
	{
		ToggleFullscreen(hwnd);
	}

	ImGui::Checkbox("Weapon Inspector", &WeaponInspector::bEnable);
	ImGui::Spacing();
	ImGui::Checkbox("Car Inspector", &CarInspector::bEnable);
	ImGui::Spacing();
	ImGui::Checkbox("Teleport", &Teleport::bEnable);
	ImGui::Spacing();
	ImGui::SeparatorText("Player");
	ImGui::Spacing();
	ImGui::Checkbox("Run Speed Modifier", &RunSpeed::bEnable);
	ImGui::SameLine();
	ImGui::InputFloat("##RunSpeedValue", &RunSpeed::DesiredSpeed, 0.1f, 1.0f, "%.1f");
	ImGui::Checkbox("Swim Speed Modifier", &SwimSpeed::bEnable); // Add this
	ImGui::SameLine();
	ImGui::InputFloat("##SwimSpeedValue", &SwimSpeed::DesiredSpeed, 0.1f, 1.0f, "%.1f");
	ImGui::Checkbox("Refresh Health", &RefreshHealth::bEnable);
	ImGui::SameLine();
	ImGui::InputFloat("##RefreshHealthThreshold", &RefreshHealth::HealThresholdPercent);
	ImGui::Checkbox("Never Wanted", &NoWanted::bEnable);
	ImGui::Spacing();
	ImGui::Checkbox("God Mode", &GodMode::bEnable);
	ImGui::Spacing();
	ImGui::SeparatorText("Vehicle");
	ImGui::Checkbox("Vehicle God Mode", &CarGodMode::bEnable);
	ImGui::Spacing();
	ImGui::Checkbox("Seat Belt", &Seatbelt::bEnable);
	ImGui::Spacing();
	ImGui::Checkbox("Enable Car Control", &CarControl::bEnable);


	if (CarControl::bEnable)  // Only show dropdown when enabled
	{
		static const WORD values[] = {
			0x0000, 0x0040, 0x0020, 0x0100, 0x0200,
			0x0140, 0x0120, 0x0060, 0x0160, 0x0340, 0x0360
		};

		const char* items =
			"OFF\0Rocket Boost (RB)\0Vehicle Jump (VJ)\0Parachute (P)\0"
			"Ramp Buggy (RmB)\0RB+P\0VJ+P\0RB+VJ\0RB+VJ+P\0"
			"RB+P+RmB (Best)\0RB+VJ+P+RmB (All)\0";

		// Find current selection index
		int current_index = 0;
		for (int i = 0; i < IM_ARRAYSIZE(values); i++) {
			if (values[i] == CarControl::CurrentValue) {
				current_index = i;
				break;
			}
		}

		// Display the combo box only when enabled
		if (ImGui::Combo("Car Features", &current_index, items)) {
			CarControl::CurrentValue = values[current_index];
		}
	}
	ImGui::Spacing();
	ImGui::Checkbox("Vehicle Hash Changer", &HashChanger::bEnable);
	if (HashChanger::bEnable)
	{
		ImGui::Indent();
		ImGui::SeparatorText("Beware if you set a invalid hash you will crash");
		ImGui::InputInt("Model Hash (Decimal)", &HashChanger::NewHash);

		if (ImGui::Button("Set to Drag"))
		{
			HashChanger::NewHash = -255678177; // Hakachou Drag hash
		}

		ImGui::SameLine();

		if (ImGui::Button("Set to Sultan RS"))
		{
			HashChanger::NewHash = -295689028; // Sultan RS hash
		}

		ImGui::SameLine();
		if (ImGui::Button("Set to Kuruma"))
		{
			HashChanger::NewHash = 410882957; // Karuma Armored hash
		}

		ImGui::SameLine();
		if (ImGui::Button("Set to Oppressor Mk2"))
		{
			HashChanger::NewHash = 2069146067; // Oppressor Mk2 hash
		}

		ImGui::SameLine();
		if (ImGui::Button("Set to Draugur"))
		{
			HashChanger::NewHash = 3526730918; // Drauger hash
		}
		ImGui::Unindent();

	}
	ImGui::End();
	return 1;
}