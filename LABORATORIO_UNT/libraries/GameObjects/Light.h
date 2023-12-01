#pragma once

#include <GameRender/model_animation.h>  
#include <GameRender/shader.h>
#include <glm/vec3.hpp>

struct Light {
	glm::vec3 position{ 0.0f, 0.0f, 0.0f };
	glm::vec3 color{ 255,255,255 };

	Light() {
		
	}

	//Return -1 if any attribute didn't load correctly
	int setLightShader(Shader &shader)
	{
		color.x /= 255;
		color.y /= 255;
		color.z /= 255;
		shader.setVec3("lightColor", color);
		shader.setVec3("lightPos", position);
		return 1;
	}
};