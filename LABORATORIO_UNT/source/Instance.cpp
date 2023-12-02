#include <GameRender/Instance.h>
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Instance::getTranformationMatrix()
{
	glm::mat4 matrix = glm::mat4(1.0f);

	glm::quat Qrotation = glm::quat(glm::radians(rotation));

	matrix = glm::translate(matrix, position);
	matrix = glm::rotate(matrix, glm::angle(rotationTrue), glm::axis(rotationTrue));
	matrix = glm::scale(matrix, scale);
	return matrix;
}

void Instance::configureShaderMatrices(glm::mat4 projection, glm::mat4 view, glm::vec3 viewPos)
{
	asset->shader->use();
	asset->shader->setMat4("projection", projection);
	asset->shader->setMat4("view", view);
	asset->shader->setVec3("viewPos", viewPos);
	asset->shader->setMat4("model", getTranformationMatrix());
}

void Instance::configureShaderLights(glm::vec3 LightColor, glm::vec3 LightPos)
{
	asset->shader->use();
	asset->shader->setVec3("lightColor", LightColor);
	asset->shader->setVec3("lightPos", LightPos);
}

void Instance::draw()
{
	asset->shader->use();
	asset->model->Draw(*asset->shader);
}

