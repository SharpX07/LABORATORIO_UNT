#include "Light.h"

int Light::setLightShader(Shader& shader)
{
	color.x /= 255;
	color.y /= 255;
	color.z /= 255;
	shader.setVec3("lightColor", color);
	shader.setVec3("lightPos", position);
	return 1;
}
