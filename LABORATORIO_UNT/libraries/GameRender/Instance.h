#pragma once
#include <GameRender/Asset.h>
#include <GameLogic/Rigidbody.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/ext/vector_float3.hpp>
class Instance
{
public:

	int ID;
	string Name;
	Asset* asset;

	glm::vec3 Position{ 0,0,0 };
	glm::quat Rotation{ 1,0,0,0 };
	glm::vec3 scale{ 1,1,1 };
	bool isVisible = true;
	Rigidbody* rigidBody;

	Instance(Asset* asset);
	Instance(Asset* asset, string name);

	glm::mat4 getTranformationMatrix();

	void configureShaderMatrices(glm::mat4 projection,
		glm::mat4 view,
		glm::vec3 viewPos
	);
	void configureShaderLights(glm::vec3 LightColor,
		glm::vec3 LightPos);

	void draw();
	void update();
private:
	static int generateID();
};


