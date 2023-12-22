#include "GameObjects/Pendulum.h"


Pendulum::Pendulum(glm::vec3 pivot, glm::f32 angle, glm::f32 lenght, glm::f32 gravity)
{
	this->pivot = pivot;
	this->angle = angle;
	this->lenght = lenght;
	this->gravity = gravity;
	position.x = pivot.x + lenght * sin(angle);
	position.y = pivot.y + lenght * cos(angle);
	position.z = pivot.z;
}

void Pendulum::update(float delta_time)
{
	float force = -gravity / lenght * sin(angle);
	angular_acceleration = force;
	angular_velocity += angular_acceleration * delta_time;
	angle += angular_velocity * delta_time;
	position.x = pivot.x + lenght * sin(angle);
	position.y = -(pivot.y + lenght * cos(angle));
	position.z = pivot.z;
	angle *= 0.999;
}

void Pendulum::restartAngular()
{
	angular_acceleration = 0;
	angular_velocity = 0;
}
