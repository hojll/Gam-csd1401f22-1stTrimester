#pragma once
#include "gameObject.h"
#include <stdlib.h>

void CreateGameObject(GameObject* gameObj, CP_Vector pos_, CP_Vector dir_, GO_TYPE type_, int active_, void* parent_, CP_Image sprite_, float spriteScale_)
{
	gameObj->pos = pos_;
	gameObj->dir = dir_;
	gameObj->type = type_;
	gameObj->active = active_;
	gameObj->parent = parent_;
	gameObj->sprite = sprite_;
	gameObj->spriteScale = spriteScale_;
}

void DestroyGameObject(GameObject* gameObject)
{
	gameObject = NULL;
	free(gameObject);
}
