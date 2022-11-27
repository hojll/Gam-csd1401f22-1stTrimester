//All content © 2021 DigiPen (SINGAPORE) Corporation, all rights reserved.
//Alonzo Nalpon	a.nalpon@digipen.edu
//Ryan Ho		r.ho@digipen.edu

#ifndef E_BASIC_ENEMY_2_H
#define E_BASIC_ENEMY_2_H


#include "e_basicenemy_1.h"
//#define MAX_PATHFINDING_NODES 8
/*-----------------------------------------------------------------*/
// E_BASICENEMY ENTITY 2
/*-----------------------------------------------------------------*/
typedef enum ENEMY_2_ANIMATIONS {
	ANIM_ENEMY_2_ACTIVE_MOVING,
	ANIM_ENEMY_2_JUMPING,
	ANIM_ENEMY_2_NUM_ANIM
}ENEMY_2_ANIMATIONS;
/*-----------------------------------------------------------------*/
// Other references
/*-----------------------------------------------------------------*/

/*-----------------------------------------------------------------*/
// Functions 
/*-----------------------------------------------------------------*/

/// <summary>
/// Only need to call once. Will initialize enemy 2 sprite data
/// </summary>
void InitAnimdata_E2();
/// <summary>
/// Initializes enemy 2 types on basic enemy struct
/// We got different sprite anim data so this is required
/// </summary>
/// <param name="enemy">pointer to enemy to init</param>
/// <returns>pointer to same enemy after initialization</returns>
E_Basic_Enemy *InitializeEnemy_2(E_Basic_Enemy *enemy);	// 

#endif