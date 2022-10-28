#include <stdio.h>
#include "e_basicenemy_1.h"
#include "messenger.h"
#include "collision.h"
#include "utilities.h"

Messenger g_messenger;
#define E_SPEED 10;
#define GRAVITY 70.f
#define MAX_GRAV_VEL = 600.f
#define JUMP_VEL 1000.f
/*----------------------------------------------------*/
// ENEMI
/*----------------------------------------------------*/

void Enemy_ActiveUpdate(E_BASIC_ENEMY_1 *enemy)
{
	
}

void Enemy_DeadUpdate(E_BASIC_ENEMY_1* enemy)
{

}

E_BASIC_ENEMY_1 InitializeEnemy()
{
	E_BASIC_ENEMY_1 retval;
	retval.Update[STATE_ENEMY_DEAD] = Enemy_DeadUpdate;
	retval.Update[STATE_ENEMY_ACTIVE] = Enemy_ActiveUpdate;
	retval.state = STATE_ENEMY_ACTIVE;

	return retval;
}