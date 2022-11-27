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
void InitAnimdata_E2();
E_Basic_Enemy *InitializeEnemy_2(E_Basic_Enemy *enemy);

#endif