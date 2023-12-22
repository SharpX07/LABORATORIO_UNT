#pragma once
#include <imgui.h>
#include <string>
namespace debugUI
{
	bool activePhysics = true;
	void debug_pendulum()
	{
		ImGui::Begin("Debug");   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Checkbox("Active Physics",&activePhysics);
			//ImGui::SliderFloat("")
		//ImGui::SetWindowPos(ImVec2(600, 2));
		ImGui::End();
	}
}