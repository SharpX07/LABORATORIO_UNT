#pragma once

#include <bullet/btBulletDynamicsCommon.h>

// Clase que representa un cuerpo rígido en el motor de física
class Rigidbody
{
public:
    btCollisionShape* CollisionShape;     // Forma de colisión del cuerpo rígido
    btScalar Mass;                        // Masa del cuerpo rígido
    btTransform Transform;                 // Transformación del cuerpo rígido
    btVector3 LocalInertia;               // Inercia local del cuerpo rígido
    btDefaultMotionState* MotionState;     // Estado de movimiento por defecto
    btRigidBody* Body;                     // Cuerpo rígido en sí

    // Constructor que inicializa el cuerpo rígido con una forma de colisión, masa y posición
    Rigidbody(btCollisionShape* CollisionShape, btScalar Mass, btVector3 Position);

    // Establece la posición del cuerpo rígido
    void setPosition(btVector3 position);

    // Obtiene la posición actual del cuerpo rígido
    btVector3 getPosition();
};
