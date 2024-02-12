#pragma once

#include "shader.h"
#include <GameRender/model_animation.h>

// Clase que representa un recurso gráfico (shader y modelo)
class Asset
{
public:
    Shader* shader;  // Puntero al shader asociado al asset
    Model* model;    // Puntero al modelo asociado al asset

    // Constructor que inicializa el asset con un shader y un modelo
    Asset(Shader* shader, Model* model);
};
