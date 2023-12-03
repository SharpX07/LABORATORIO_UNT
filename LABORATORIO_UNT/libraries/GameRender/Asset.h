#pragma once
#include "shader.h"
#include <GameRender/model_animation.h>  

class Asset
{
public:
	Shader* shader;
	Model* model;

	Asset(Shader* shader, Model* model);
};

