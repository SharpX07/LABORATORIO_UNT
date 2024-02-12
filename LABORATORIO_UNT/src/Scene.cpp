#include "GameLogic/Scene.h"

// Agrega una luz a la escena
void Scene::addLight(Light* light)
{
    lights.push_back(light);
}

// Elimina la �ltima luz agregada a la escena
void Scene::removeLight()
{
    lights.pop_back();
}

// Agrega una instancia a la escena
void Scene::addInstance(Instance* instance)
{
    instances.push_back(instance);
}

// Elimina la �ltima instancia agregada a la escena
void Scene::removeInstance()
{
    instances.pop_back();
}

// Dibuja la escena
void Scene::draw()
{
    for (int i = 0; i < instances.size(); i++)
    {
        // Verifica si la instancia no es visible
        if (!instances.at(i)->isVisible)
            continue;

        // Configura las matrices del shader con la informaci�n de la c�mara
        instances.at(i)->configureShaderMatrices(
            camera->getProjectionMatrix(),
            camera->GetViewMatrix(),
            camera->Position);

        // Configura las luces en el shader
        for (int j = 0; j < lights.size(); j++)
        {
            instances.at(i)->configureShaderLights(lights.at(j)->color, lights.at(j)->position);
        }

        // Dibuja la instancia
        instances.at(i)->draw();
    }
}

// Actualiza la escena con el paso de tiempo especificado
void Scene::update(float timeStep, int maxSubSteps)
{
    // Realiza la simulaci�n f�sica
    physicsManager->dynamicsWorld->stepSimulation(timeStep, maxSubSteps);

    // Actualiza cada instancia en la escena
    for (int i = 0; i < instances.size(); i++)
    {
        instances.at(i)->update();
    }
}
