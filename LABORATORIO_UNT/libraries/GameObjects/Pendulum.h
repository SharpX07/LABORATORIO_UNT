#pragma once
#include <GameLogic/Rigidbody.h>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtc/type_precision.hpp>


class Pendulum
{
public:
	glm::vec3 pivot{};
	glm::f32 lenght{};
	glm::f32 angle{};
	glm::f32 gravity{};
	glm::vec3 position{};
	
	/*
	- angle is in radians
	- gravity is in m/s^2
	- lenght is in meters
	*/
	Pendulum(glm::vec3 pivot,glm::f32 angle, glm::f32 lenght , glm::f32 gravity);
	void update(float delta_time);
	void restartAngular();
private:
	glm::f32 angular_acceleration{};
	glm::f32 angular_velocity{};
};

