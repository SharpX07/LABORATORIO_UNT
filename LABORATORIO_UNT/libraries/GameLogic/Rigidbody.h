#pragma once

#include <bullet/btBulletDynamicsCommon.h>

// Clase que representa un cuerpo r�gido en el motor de f�sica
class Rigidbody
{
public:
    btCollisionShape* CollisionShape;     // Forma de colisi�n del cuerpo r�gido
    btScalar Mass;                        // Masa del cuerpo r�gido
    btTransform Transform;                 // Transformaci�n del cuerpo r�gido
    btVector3 LocalInertia;               // Inercia local del cuerpo r�gido
    btDefaultMotionState* MotionState;     // Estado de movimiento por defecto
    btRigidBody* Body;                     // Cuerpo r�gido en s�

    // Constructor que inicializa el cuerpo r�gido con una forma de colisi�n, masa y posici�n
    Rigidbody(btCollisionShape* CollisionShape, btScalar Mass, btVector3 Position);

    // Establece la posici�n del cuerpo r�gido
    void setPosition(btVector3 position);

    // Obtiene la posici�n actual del cuerpo r�gido
    btVector3 getPosition();
};
