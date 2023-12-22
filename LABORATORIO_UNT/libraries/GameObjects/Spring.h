#pragma once
#include <glm/ext/vector_float3.hpp>
#include <glm/gtc/type_precision.hpp>
class Spring
{
public:
	glm::f32 mass{};
	glm::f32 k{};
	glm::f32 gravity{};
	glm::vec3 displacement{};
	glm::vec3 origin{};
	glm::vec3 position{};

	Spring(glm::vec3 origin, glm::f32 gravity, glm::f32 mass, glm::f32 k);
	void update(float delta_time);

private:
	glm::f32 acceleration{};
	glm::f32 velocity{};
};

