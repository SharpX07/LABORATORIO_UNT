#pragma once

#include <GameRender/Asset.h>
#include <GameLogic/Rigidbody.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/ext/vector_float3.hpp>

// Clase que representa una instancia de un objeto en la escena
class Instance
{
public:
    int ID;                   // Identificador �nico de la instancia
    std::string Name;         // Nombre de la instancia
    Asset* asset;             // Puntero al asset asociado a la instancia
    glm::vec3 Position{ 0,0,0 };  // Posici�n de la instancia en el espacio
    glm::quat Rotation{ 1,0,0,0 };  // Rotaci�n de la instancia en cuaterniones
    glm::vec3 scale{ 1,1,1 };  // Escala de la instancia
    bool isVisible = true;     // Indica si la instancia es visible
    Rigidbody* rigidBody;      // Puntero al componente de f�sica asociado a la instancia

    // Constructores
    Instance(Asset* asset);
    Instance(Asset* asset, std::string name);

    // Obtiene la matriz de transformaci�n de la instancia
    glm::mat4 getTransformationMatrix();

    // Configura las matrices del shader con la informaci�n de la c�mara
    void configureShaderMatrices(glm::mat4 projection,
        glm::mat4 view,
        glm::vec3 viewPos);

    // Configura las luces en el shader
    void configureShaderLights(glm::vec3 LightColor,
        glm::vec3 LightPos);

    // Dibuja la instancia
    void draw();

    // Actualiza la instancia
    void update();

private:
    // Genera un ID �nico para la instancia
    static int generateID();
};
