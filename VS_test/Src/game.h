#pragma once
#ifndef GAME_H
#define GAME_H
#include "gameObject.h"

void AddToGameObjectList(GameObject* gameObject);
void game_init(void);
void game_update(void);
void game_exit(void);

#endif