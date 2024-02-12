#include "GameLogic/Physics_manager.h"
#include <GameLogic/Rigidbody.h>

// Constructor de PhysicsManager
PhysicsManager::PhysicsManager()
{
    // Inicialización en el constructor
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    overlappingPairCache = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver;
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
}

// Agrega un cuerpo rígido al mundo de física
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

    // Verificar si hubo intersección
    if (rayCallback.hasHit()) {
        // Se ha golpeado algo
        const btRigidBody* cuerpoGolpeado = static_cast<const btRigidBody*>(rayCallback.m_collisionObject);

        // Recuperar el user pointer para obtener el identificador único
        identificadorGolpeado = static_cast<int>(reinterpret_cast<intptr_t>(cuerpoGolpeado->getUserPointer()));

        // Devolver true para indicar que se ha golpeado un cuerpo
        return true;
    }
    else {
        // No se ha golpeado nada
        identificadorGolpeado = -1; // Otra señal de que no se golpeó ningún cuerpo
        return false;
    }
}



// Destructor de PhysicsManager
PhysicsManager::~PhysicsManager()
{
    // Liberación de memoria en el destructor

    // Elimina los cuerpos rígidos y las formas de colisión
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

    // Elimina las formas de colisión restantes
    for (int j = 0; j < collisionShapes.size(); j++)
    {
        btCollisionShape* shape = collisionShapes[j];
        collisionShapes[j] = 0;
        delete shape;
    }

    // Elimina los objetos de física
    delete dynamicsWorld;
    delete solver;
    delete overlappingPairCache;
    delete dispatcher;
    delete collisionConfiguration;
}
