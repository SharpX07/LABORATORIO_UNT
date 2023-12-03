#include "GameLogic/Rigidbody.h"

Rigidbody::Rigidbody(btCollisionShape* CollisionShape, btScalar Mass, btVector3 Position)
{
	this->Mass = Mass;
	this->CollisionShape = CollisionShape;
	Transform.setIdentity();
	Transform.setOrigin(Position);
		//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (this->Mass != 0.f);
	LocalInertia = btVector3(0, 0, 0);
	if (isDynamic)
		this->CollisionShape->calculateLocalInertia(this->Mass, LocalInertia);

	//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	MotionState = new btDefaultMotionState(Transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(this->Mass, MotionState, this->CollisionShape, LocalInertia);
	Body = new btRigidBody(rbInfo);
}