#pragma once
#include "camera.h"
#include <GameRender/Instance.h>
#include <GameObjects/Light.h>
class Scene
{
public:
	Camera *camera;
	std::vector<Instance*> instances;
	std::vector<Light*> lights;

	void addLight(Light* light);
	void removeLight();
	void addInstance(Instance* instance);
	void removeInstance();
	void draw();
};

