#include "pch.h"
#include <print>
#include <thread>

#include "MyImGui.h"

bool bAlive = true;

int main(int, char**)
{
	MyImGui::Initialize();

	if (!DMA::Initialize())
	{
		MyImGui::Close();
		system("pause");
		return 0;
	}

	std::thread DMAThread(DMA::DMAThreadEntry);

	while (bAlive)
	{
		if (GetAsyncKeyState(VK_END) & 1)
			bAlive = false;

		MyImGui::OnFrame();
	}

	DMAThread.join();

	MyImGui::Close();

	return 0;
}
