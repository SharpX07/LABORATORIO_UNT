#pragma once

#include "bullet/btBulletDynamicsCommon.h"
#include <GameLogic/Rigidbody.h>
#include <GameRender/Instance.h>
#include <vector>

// Clase que gestiona la f�sica en el motor de juego
class PhysicsManager
{
    class MyContactResultCallback : public btCollisionWorld::ContactResultCallback
    {
    public:
        bool hasCollision = false;

        virtual btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0,
            const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) override
        {
            // Se llama cuando se encuentra una colisi�n
            hasCollision = true;
            return 0; // Ignorar los dem�s puntos de contacto
        }
    };
public:
    // Configuradores
    btDefaultCollisionConfiguration* collisionConfiguration;  // Configuraci�n de colisiones por defecto
    btCollisionDispatcher* dispatcher;                        // Despachador de colisiones
    btBroadphaseInterface* overlappingPairCache;              // Algoritmo de ampliaci�n de pares (para detectar colisiones)
    btSequentialImpulseConstraintSolver* solver;               // Solucionador de restricciones de impulso secuencial
    btDiscreteDynamicsWorld* dynamicsWorld;                   // Mundo din�mico discreto para la simulaci�n f�sica

    // Objetos
    btAlignedObjectArray<btCollisionShape*> collisionShapes;  // Formas de colisi�n
    std::vector<btRigidBody*> rigidbodies;                    // Cuerpos r�gidos

    // Constructor
    PhysicsManager();

    // Destructor
    ~PhysicsManager();

    // Agrega un cuerpo r�gido al mundo de f�sica
    void addRigidBody(Rigidbody* rgbody);
    bool rayCast(const btVector3& desde, const btVector3& hacia,  btRigidBody*& cuerpoGolpeado)const;
    bool checkForCollisions(btCollisionObject* obj1, btCollisionObject* obj2);
};
