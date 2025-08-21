#include "pch.h"

#include "Dev.h"

bool Dev::Render()
{
	ImGui::Begin("Dev", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::InputInt("Global", &Dev::DesiredGlobalIndex);

	if (ImGui::Button("Get Global Float"))
	{
		float Output = 0.0f;
		DMA::GetGlobalValue<float>(static_cast<DWORD>(DesiredGlobalIndex), Output);
		std::println("Global({0:d}) {1:f}", DesiredGlobalIndex, Output);
	}	
	ImGui::SameLine();
	if (ImGui::Button("Get Global DWORD"))
	{
		DWORD Output = 0;
		DMA::GetGlobalValue<DWORD>(static_cast<DWORD>(DesiredGlobalIndex), Output);
		std::println("Global({0:d}) {1:d}", DesiredGlobalIndex, Output);
	}

	ImGui::End();

	return 1;
}