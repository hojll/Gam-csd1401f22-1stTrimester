#ifndef E_ENEMY_BOSS_1_H
#define E_ENEMY_BOSS_1_H

#include "e_basicenemy_1.h"
//#define MAX_PATHFINDING_NODES 8
/*-----------------------------------------------------------------*/
// E_ENEMY_BOSS ENTITY 1
/*-----------------------------------------------------------------*/
typedef enum ENEMY_BOSS_1_ANIMATIONS {
	ANIM_ENEMY_BOSS_1_ACTIVE_MOVING,
	ANIM_ENEMY_BOSS_1_JUMPING,
	ANIM_ENEMY_BOSS_1_NUM_ANIM
}ENEMY_BOSS_1_ANIMATIONS;
/*-----------------------------------------------------------------*/
// Other references
/*-----------------------------------------------------------------*/

/*-----------------------------------------------------------------*/
// Functions 
/*-----------------------------------------------------------------*/
void InitAnimdata_EB1();
E_Basic_Enemy* InitializeEnemyBoss_1(E_Basic_Enemy* enemy);

#endif