#pragma once
#include "bullet/btBulletDynamicsCommon.h"
#include <GameLogic/Rigidbody.h>
#include <vector>
class PhysicsManager
{
public:
    // Configurators
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* overlappingPairCache;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;
    // Objects
    btAlignedObjectArray<btCollisionShape*> collisionShapes;
    std::vector<btRigidBody*> rigidbodies;

    PhysicsManager();
    ~PhysicsManager();

    void addRigidBody(Rigidbody &rgbody);
};

