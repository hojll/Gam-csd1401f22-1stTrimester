#pragma once
#ifndef PLAYERGO_H
#define PLAYERGO_H
#include "gameObject.h"

typedef struct Player
{
	GameObject* base;
	void (*update)(void*);
	void (*destroy)(void*);
	float health;
	float speed;
	//More shit
}Player;

void CreatePlayer(
	Player* player_,
	CP_Vector pos_,
	CP_Vector dir_,
	CP_Image sprite_,
	float spriteScale_
);

void UpdatePlayer(void* player_);

void DestroyPlayer(void* player);


#endif