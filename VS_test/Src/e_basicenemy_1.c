#include <stdio.h>
#include "e_basicenemy_1.h"
#include "messenger.h"
#include "collision.h"
#include "utilities.h"
#include <stdlib.h>

Messenger g_messenger;
#define E_SPEED 10;
#define GRAVITY 70.f
#define MAX_GRAV_VEL = 600.f
#define JUMP_VEL 1000.f
/*----------------------------------------------------*/
// ENEMI
/*----------------------------------------------------*/

void Enemy_ActiveUpdate(E_Basic_Enemy_1 *enemy)
{
	if (!enemy->go.active)
	{
		
	}
	else
	{
		enemy->go.pos.x += (enemy->go.dir.x * 10) + g_scaledDt;
	}
}

void Enemy_DeadUpdate(E_Basic_Enemy_1* enemy)
{

}

E_Basic_Enemy_1 InitializeEnemy()
{
	E_Basic_Enemy_1 retval;
	retval.Update[STATE_ENEMY_DEAD] = Enemy_DeadUpdate;
	retval.Update[STATE_ENEMY_ACTIVE] = Enemy_ActiveUpdate;
	retval.state = STATE_ENEMY_ACTIVE;
	retval.HP = 0;
	retval.go.active = 0;
	retval.go.height = 50.f;
	retval.go.width = 50.f;

	// random direction
	int randomdir = returnRange(0, 1);
	printf("%d direction\n", randomdir);
	if (randomdir > 0)
	{
		retval.go.dir.x = 1;
	}
	else
	{
		retval.go.dir.x = -1;
	}
	return retval;
}

// Helper function
void InitEnemyList(E_Basic_Enemy_1 arr[], int size)
{
	for (int i = 0; i < size; i++)
	{
		arr[i] = InitializeEnemy();
	}
}

void UpdateEnemyList(E_Basic_Enemy_1 arr[], int size)
{
	for (int i = 0; i < size; ++i)
	{
		if (arr[i].go.active)
		{
			arr[i].Update[arr[i].state](&arr[i],g_scaledDt);
		}
	}
}

