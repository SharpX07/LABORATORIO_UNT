#include "GameLogic/Scene.h"

void Scene::addLight(Light* light)
{
	lights.push_back(light);
}
void Scene::removeLight()
{
	lights.pop_back();
}
void Scene::addInstance(Instance* instance)
{
	instances.push_back(instance);
}
void Scene::removeInstance()
{
	instances.pop_back();
}

void Scene::draw()
{
	for(int i=0;i<instances.size();i++)
	{
		instances.at(i)->configureShaderMatrices(
			camera->getProjectionMatrix(),
			camera->GetViewMatrix(),
			camera->Position);
		for(int j=0;j<lights.size();j++)
		{
			instances.at(i)->configureShaderLights(lights.at(j)->color, lights.at(j)->position);
		}
		instances.at(i)->draw();
	}

	/*for (int i = 0; i < instances.size(); i++)
	{
		instances.at(i)->draw();
	}*/
}

void Scene::update(float timeStep)
{
	physicsManager->dynamicsWorld->stepSimulation(timeStep);
}
