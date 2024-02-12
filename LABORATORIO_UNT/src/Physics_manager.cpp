#include "GameLogic/Physics_manager.h"
#include <GameLogic/Rigidbody.h>

// Constructor de PhysicsManager
PhysicsManager::PhysicsManager()
{
    // Inicializaci�n en el constructor
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    overlappingPairCache = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver;
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
}

// Agrega un cuerpo r�gido al mundo de f�sica
void PhysicsManager::addRigidBody(Rigidbody* rgbody)
{
    rigidbodies.push_back(rgbody->Body);
    dynamicsWorld->addRigidBody(rgbody->Body);
}

bool PhysicsManager::rayCast(const btVector3& desde, const btVector3& hacia, int& identificadorGolpeado) const
{
    // Crear un rayo
    btCollisionWorld::ClosestRayResultCallback rayCallback(desde, hacia);

    // Realizar el ray casting
    dynamicsWorld->rayTest(desde, hacia, rayCallback);

    // Verificar si hubo intersecci�n
    if (rayCallback.hasHit()) {
        // Se ha golpeado algo
        const btRigidBody* cuerpoGolpeado = static_cast<const btRigidBody*>(rayCallback.m_collisionObject);

        // Recuperar el user pointer para obtener el identificador �nico
        identificadorGolpeado = static_cast<int>(reinterpret_cast<intptr_t>(cuerpoGolpeado->getUserPointer()));

        // Devolver true para indicar que se ha golpeado un cuerpo
        return true;
    }
    else {
        // No se ha golpeado nada
        identificadorGolpeado = -1; // Otra se�al de que no se golpe� ning�n cuerpo
        return false;
    }
}



// Destructor de PhysicsManager
PhysicsManager::~PhysicsManager()
{
    // Liberaci�n de memoria en el destructor

    // Elimina los cuerpos r�gidos y las formas de colisi�n
    for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
    {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }
        dynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }

    // Elimina las formas de colisi�n restantes
    for (int j = 0; j < collisionShapes.size(); j++)
    {
        btCollisionShape* shape = collisionShapes[j];
        collisionShapes[j] = 0;
        delete shape;
    }

    // Elimina los objetos de f�sica
    delete dynamicsWorld;
    delete solver;
    delete overlappingPairCache;
    delete dispatcher;
    delete collisionConfiguration;
}
