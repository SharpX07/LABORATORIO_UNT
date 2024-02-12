#pragma once

#include "GameRender/camera.h"
#include <GameRender/Instance.h>
#include <GameObjects/Light.h>
#include <GameLogic/Physics_manager.h>

// Clase que representa una escena en el motor gráfico
class Scene
{
public:
    Camera* camera;           // Puntero a la cámara de la escena
    PhysicsManager* physicsManager;  // Puntero al motor de física de la escena
    std::vector<Instance*> instances;  // Vector de instancias en la escena
    std::vector<Light*> lights;        // Vector de luces en la escena

    // Agrega una luz a la escena
    void addLight(Light* light);

    // Elimina la última luz agregada a la escena
    void removeLight();

    // Agrega una instancia a la escena
    void addInstance(Instance* instance);

    // Elimina la última instancia agregada a la escena
    void removeInstance();

    // Dibuja la escena
    void draw();

    // Actualiza la escena con el paso de tiempo especificado
    void update(float timeStep, int maxSubSteps);
};
