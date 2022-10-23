#pragma once
#ifndef GAMEOBJ_H
#define GAMEOBJ_H
#include "cprocessing.h"

typedef enum GO_TYPE
{
	PLAYER,
	BULLET
}GO_TYPE;

typedef struct GameObject
{
	CP_Vector pos;
	CP_Vector dir;
	GO_TYPE type;
	int active;
	void* parent;
	CP_Image sprite;
	float spriteScale;
}GameObject;

void CreateGameObject(
	GameObject* gameObj,
	CP_Vector pos_,
	CP_Vector dir_,
	GO_TYPE type_,
	int active_,
	void* parent_,
	CP_Image sprite_,
	float spriteScale_
);

void DestroyGameObject(GameObject* gameObject);

#endif