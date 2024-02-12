#include "GameLogic/Rigidbody.h"

// Constructor de Rigidbody
Rigidbody::Rigidbody(btCollisionShape* CollisionShape, btScalar Mass, btVector3 Position)
{
    // Inicialización de propiedades
    this->Mass = Mass;
    this->CollisionShape = CollisionShape;
    Transform.setIdentity();
    Transform.setOrigin(Position);

    // Verifica si el cuerpo rígido es dinámico
    bool isDynamic = (this->Mass != 0.f);

    LocalInertia = btVector3(0, 0, 0);
    if (isDynamic)
        this->CollisionShape->calculateLocalInertia(this->Mass, LocalInertia);

    MotionState = new btDefaultMotionState(Transform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(this->Mass, MotionState, this->CollisionShape, LocalInertia);
    Body = new btRigidBody(rbInfo);
}

// Establece la posición del cuerpo rígido
void Rigidbody::setPosition(btVector3 newPosition)
{
    btTransform newTransform;
    newTransform.setIdentity();
    newTransform.setOrigin(newPosition);

    // Actualiza el estado de movimiento y la transformación del centro de masa
    Body->getMotionState()->setWorldTransform(newTransform);
    Body->setCenterOfMassTransform(newTransform);
}

// Obtiene la posición actual del cuerpo rígido
btVector3 Rigidbody::getPosition()
{
    btTransform currentTransform;
    Body->getMotionState()->getWorldTransform(currentTransform);

    return currentTransform.getOrigin();
}
