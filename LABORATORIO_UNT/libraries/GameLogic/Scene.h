#pragma once
#include "GameRender/camera.h"
#include <GameRender/Instance.h>
#include <GameObjects/Light.h>
#include <GameLogic/Physics_manager.h>
class Scene
{
public:
	Camera *camera;
	PhysicsManager *physicsManager;
	std::vector<Instance*> instances;
	std::vector<Light*> lights;

	void addLight(Light* light);
	void removeLight();
	void addInstance(Instance* instance);
	void removeInstance();
	void draw();
	void update(float timeStep, int maxSubSteps);
};

