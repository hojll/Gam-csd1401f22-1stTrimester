#pragma once
#ifndef BULLETGO_H
#define BULLETGO_H
#include "gameObject.h"

typedef struct Bullet
{
	GameObject* base;
	void (*update)(void*);
	void (*destroy)(void*);
	float speed;
	float lifetime;
}Bullet;

void CreateBullet(
	Bullet* bullet_,
	CP_Vector pos_,
	CP_Vector dir_,
	CP_Image sprite_,
	float spriteScale_,
	float speed_,
	float lifetime_
);

void UpdateBullet(void* bullet_);

void DestroyBullet(void* bullet);

#endif