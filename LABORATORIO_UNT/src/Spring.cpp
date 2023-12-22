#include "GameObjects/Spring.h"

Spring::Spring(glm::vec3 origin, glm::f32 gravity, glm::f32 mass, glm::f32 k)
{
	displacement = { 0,0,0 };
	this->k = k;
	this->origin = origin;
	this->gravity = gravity;
	this->mass = mass;
	position = origin + displacement;
}

void Spring::update(float delta_time)
{
	float SpringForce = -k * displacement.y * delta_time;
	float WeightForce = -mass * gravity * delta_time;
	
	velocity += (SpringForce + WeightForce) * delta_time;
	displacement.y += velocity;
	velocity *= 0.99;
	position = (origin + displacement);
}
