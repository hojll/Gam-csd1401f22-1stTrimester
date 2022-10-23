#pragma once
#include "bullet_go.h"
#include <stdio.h>
#include <stdlib.h>

void CreateBullet(Bullet* bullet_, CP_Vector pos_, CP_Vector dir_, CP_Image sprite_, float spriteScale_, float speed_, float lifetime_)
{
	CreateGameObject(
		bullet_->base,
		pos_,
		dir_,
		BULLET,
		1,
		bullet_,
		sprite_,
		spriteScale_
	);
	bullet_->speed = speed_;
	bullet_->lifetime = lifetime_;
	bullet_->update = UpdateBullet;
	bullet_->destroy = DestroyBullet;
}

void UpdateBullet(void* bullet_)
{
	Bullet* bullet = (Bullet*)bullet_;
	bullet->lifetime -= CP_System_GetDt();
	if (bullet->lifetime <= 0)
		bullet->base->active = 0;

	bullet->base->pos = CP_Vector_Add(bullet->base->pos, CP_Vector_Scale(bullet->base->dir, bullet->speed * CP_System_GetDt()));
}

void DestroyBullet(void* bullet)
{
	DestroyGameObject(((Bullet*)bullet)->base);
	bullet = NULL;
	free(bullet);
}
