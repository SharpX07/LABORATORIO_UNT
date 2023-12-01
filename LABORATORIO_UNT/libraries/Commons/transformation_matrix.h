#pragma once
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

static inline glm::mat4 getTranformationMatrix(
	const glm::vec3 position,
	const glm::vec4 rotation, 
	const glm::vec3 scale)
{
	glm::mat4 matrix = glm::mat4(1.0f);
	//matrix = glm::rotate(matrix, glm::radians(rotation.w), glm::vec3(rotation));
	matrix = glm::translate(matrix, position); // translate it down so it's at the center of the scene
	matrix = glm::scale(matrix, scale);	// it's a bit too big for our scene, so scale it down
	return matrix;
}