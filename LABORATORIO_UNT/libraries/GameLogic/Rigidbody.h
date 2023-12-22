#pragma once
#include <bullet/btBulletDynamicsCommon.h>
class Rigidbody
{
public:
	btCollisionShape* CollisionShape;
	btScalar Mass;
	btTransform Transform;
	btVector3 LocalInertia;
	btDefaultMotionState* MotionState;
	btRigidBody *Body;

	Rigidbody(btCollisionShape* CollisionShape,btScalar Mass, btVector3 Position);
	void setPosition(btVector3 position);
	btVector3 getPosition();

};

