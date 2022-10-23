#pragma once
#include "player_go.h"
#include "bullet_go.h"
#include "game.h"

void CreatePlayer(Player* player_, CP_Vector pos_, CP_Vector dir_, CP_Image sprite_, float spriteScale_)
{
	CreateGameObject(
		player_->base,
		pos_,
		dir_,
		PLAYER,
		1,
		player_,
		sprite_,
		spriteScale_
	);
    player_->update = UpdatePlayer;
	player_->destroy = DestroyPlayer;
}

void UpdatePlayer(void* player_)
{

}

void DestroyPlayer(void* player)
{
    DestroyGameObject(((Player*)player)->base);
    player = NULL;
    free(player);
}
