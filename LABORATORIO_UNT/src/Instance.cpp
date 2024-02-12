#include <GameRender/Instance.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

// Constructor de la instancia con un asset dado
Instance::Instance(Asset* asset)
{
    ID = generateID();
    this->asset = asset;
    this->Name = "Instance " + std::to_string(ID);
}

// Constructor de la instancia con un asset y un nombre dado
Instance::Instance(Asset* asset, std::string name)
{
    ID = generateID();
    this->asset = asset;
    this->Name = name;
}

// Obtiene la matriz de transformaci�n de la instancia
glm::mat4 Instance::getTransformationMatrix()
{
    glm::mat4 matrix = glm::mat4(1.0f);
    matrix = glm::translate(matrix, Position);
    matrix = glm::rotate(matrix, glm::angle(Rotation), glm::axis(Rotation));
    matrix = glm::scale(matrix, scale);
    return matrix;
}

// Configura las matrices del shader con la informaci�n de la c�mara
void Instance::configureShaderMatrices(glm::mat4 projection, glm::mat4 view, glm::vec3 viewPos)
{
    asset->shader->use();
    asset->shader->setMat4("projection", projection);
    asset->shader->setMat4("view", view);
    asset->shader->setVec3("viewPos", viewPos);
    asset->shader->setMat4("model", getTransformationMatrix());
}

// Configura las luces en el shader
void Instance::configureShaderLights(glm::vec3 LightColor, glm::vec3 LightPos)
{
    asset->shader->use();
    asset->shader->setVec3("lightColor", LightColor);
    asset->shader->setVec3("lightPos", LightPos);
}

// Dibuja la instancia
void Instance::draw()
{
    asset->shader->use();
    asset->model->Draw(*asset->shader);
}

// Convierte una matriz de float a glm::mat4
glm::mat4 convertFloatMatrixtoGlmMat4(float matrixValues[16])
{
    glm::mat4 mat4;
    for (int i = 0; i < 4; i++)
    {
        mat4[i][0] = matrixValues[0 + i * 4];
        mat4[i][1] = matrixValues[1 + i * 4];
        mat4[i][2] = matrixValues[2 + i * 4];
        mat4[i][3] = matrixValues[3 + i * 4];
    }
    return mat4;
}

// Actualiza la instancia, especialmente �til para objetos con f�sica asociada
void Instance::update()
{
    if (rigidBody && havePhysics)
    {
        btTransform t;
        glm::vec3 traslation, scale;
        glm::vec3 free;
        glm::vec4 free2;
        glm::quat orientation;

        glm::mat4 mat4(1.0f);

        // Obtiene la transformaci�n de Bullet y la almacena en 't'
        t = rigidBody->Body->getWorldTransform();
        float matrixValues[16];
        t.getOpenGLMatrix(matrixValues);
        mat4 = convertFloatMatrixtoGlmMat4(matrixValues);

        glm::decompose(mat4, scale, orientation, traslation, free, free2);

        this->Rotation = orientation;
        this->Position = traslation;
    }
}

// Genera un ID �nico para la instancia
int Instance::generateID()
{
    static int IDcounter = 0;
    return ++IDcounter;
}
