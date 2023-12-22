#pragma once
#include <imgui.h>
#include <string>
#include <glm/ext/vector_float3.hpp>
#include <GameObjects/Pendulum.h>
#include <GameObjects/Spring.h>

void UI_Spring(Spring *S)
{
	ImGui::Begin("Resorte");   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)	
	float position_arr[3] = { S->position.x,S->position.y,S->position.z };
	ImGui::InputFloat3("Position", position_arr);
	ImGui::InputFloat("displacement", &S->displacement.y);
	ImGui::InputFloat("Gravedad", &S->gravity);
	ImGui::InputFloat("Constante de Resorte", &S->k);
	ImGui::InputFloat("Masa del peso", &S->mass);
	ImGui::End();
}

void UI_Pendullum(Pendulum &p, std::vector<float>& arr, bool &pause)
{
	ImGui::Begin("Pendulo");   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
	float position_arr[3] = { p.position.x,p.position.y,p.position.z };
	ImGui::InputFloat3("Position", position_arr);
	ImGui::SliderFloat("Ángulo Inicial", &p.angle, -SIMD_PI, SIMD_PI);
	ImGui::SliderFloat("Largo de Cuerda", &p.lenght, 0.1, 10);
	ImGui::InputFloat("Gravedad", &p.gravity);

	ImVec2 graph_size(400, 300);
	ImGui::PlotLines("Frame Times", arr.data(), arr.size(), 0, nullptr, FLT_MAX, FLT_MAX, graph_size);
	if (ImGui::Button("Button"))
	{
		p.restartAngular();
		pause = !pause;
	}
	ImGui::End();
}

void UI_FPS(float fps)
{
	ImGui::Begin("FPS");   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
	ImGui::SetWindowPos(ImVec2(700, 2));
	ImGui::SetWindowSize(ImVec2(300, 50));
	ImGui::Text(std::to_string(fps).c_str());
	ImGui::End();
}

void UI_Position(glm::vec3& position)
{
	ImGui::Begin("Position");   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
	ImGui::SetWindowPos(ImVec2(2, 2));
	ImGui::SetWindowSize(ImVec2(300, 50));
	float position_arr[3] = { position.x,position.y,position.z };
	ImGui::InputFloat3("Position", position_arr);
	position.x = position_arr[0];
	position.y = position_arr[1];
	position.z = position_arr[2];
	ImGui::End();
}

int UI_Menu()
{
	const char* items[] = { "Pendulo", "Resorete", "Colisiones"};
	static int item_current = 0;
	ImGui::Combo("Experimento", &item_current, items, IM_ARRAYSIZE(items));
	return item_current;
}