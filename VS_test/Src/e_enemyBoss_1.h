//All content © 2021 DigiPen (SINGAPORE) Corporation, all rights reserved.
//Alonzo Nalpon	a.nalpon@digipen.edu
//Ryan Ho		r.ho@digipen.edu


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
/// <summary>
/// Only need to call once. Will initialize boss sprite data
/// </summary>
void InitAnimdata_EB1();

/// <summary>
/// Initializes boss on basic enemy struct
/// We got different sprite anim data so this is required
/// </summary>
/// <param name="enemy">pointer to enemy to init</param>
/// <returns>pointer to same enemy after initialization</returns>
E_Basic_Enemy* InitializeEnemyBoss_1(E_Basic_Enemy* enemy);

#endif