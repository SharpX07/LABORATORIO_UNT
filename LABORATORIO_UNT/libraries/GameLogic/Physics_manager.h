#pragma once

#include "bullet/btBulletDynamicsCommon.h"
#include <GameLogic/Rigidbody.h>
#include <GameRender/Instance.h>
#include <vector>

// Clase que gestiona la física en el motor de juego
class PhysicsManager
{
public:
    // Configuradores
    btDefaultCollisionConfiguration* collisionConfiguration;  // Configuración de colisiones por defecto
    btCollisionDispatcher* dispatcher;                        // Despachador de colisiones
    btBroadphaseInterface* overlappingPairCache;              // Algoritmo de ampliación de pares (para detectar colisiones)
    btSequentialImpulseConstraintSolver* solver;               // Solucionador de restricciones de impulso secuencial
    btDiscreteDynamicsWorld* dynamicsWorld;                   // Mundo dinámico discreto para la simulación física

    // Objetos
    btAlignedObjectArray<btCollisionShape*> collisionShapes;  // Formas de colisión
    std::vector<btRigidBody*> rigidbodies;                    // Cuerpos rígidos

    // Constructor
    PhysicsManager();

    // Destructor
    ~PhysicsManager();

    // Agrega un cuerpo rígido al mundo de física
    void addRigidBody(Rigidbody* rgbody);
    bool rayCast(const btVector3& desde, const btVector3& hacia,  btRigidBody*& cuerpoGolpeado)const;
};
