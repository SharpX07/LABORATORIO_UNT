#pragma once

#include <GameRender/model_animation.h>  

#include <glm/vec3.hpp>

struct Object {

	glm::vec4 rotation{ 0.0f, 1.0f, 0.0f, 0.0f };
	glm::vec3 position{ 0.0f, 0.0f, 0.0f };
	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };

	Model* model;

	Object(const string& modelPath) {
		model = new Model(modelPath);
	}
};