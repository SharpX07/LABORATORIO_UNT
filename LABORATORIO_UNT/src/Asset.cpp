#include "GameRender/Asset.h"

Asset::Asset(Shader* shader, Model* model)
{
	this->shader = shader;
	this->model = model;
}
