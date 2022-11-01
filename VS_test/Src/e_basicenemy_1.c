#include <stdio.h>
#include "e_basicenemy_1.h"
#include "messenger.h"
#include "collision.h"
#include "utilities.h"
#include <stdlib.h>

Messenger g_messenger;
#define E_SPEED 150
#define GRAVITY 70.f
#define MAX_GRAV_VEL1 = 600.f
#define JUMP_VEL 1000.f
/*----------------------------------------------------*/
// ENEMI
/*----------------------------------------------------*/

void Enemy_ActiveUpdate(E_Basic_Enemy_1 *enemy)
{
	if (enemy->go.active)
	{
		if (!enemy->tracking)
		{
			enemy->go.vel.x = enemy->go.dir.x * E_SPEED;
			enemy->go.pos.x += enemy->go.vel.x * g_scaledDt;
		}
		
		enemy->go.pos.y += enemy->go.vel.y * g_scaledDt;
		if (!enemy->grounded && enemy->go.vel.y < 600.f)
		{
			enemy->go.vel.y += GRAVITY;		
		}
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
	retval.grounded = 0;
	retval.go.dir.y = 0;
	retval.go.vel.y = 0;
	retval.tracking = 0;
	// random direction
	int randomdir = returnRange(1, 20);
	if (randomdir >= 10)
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
void InitEnemyList(E_Basic_Enemy_1 arr[], int size, GameObject nodes[])
{
	srand(123);	
	for (int i = 0; i < size; ++i)
	{
		arr[i] = InitializeEnemy();
		arr[i].nodes = nodes;
	}
}

void UpdateEnemyList(E_Basic_Enemy_1 arr[], int size)
{
	for (int i = 0; i < size; ++i)
	{
		// May refactor the code.
		// movement
		if (!arr[i].go.active)
			continue;


		arr[i].Update[arr[i].state](&arr[i],g_scaledDt);
	}
}


void EnemytoWallCollision(E_Basic_Enemy_1 *enemy, GameObject wallreference[])
{
	//hard code for now
	for (int i = 0; i < 10; ++i)
	{
		if (!enemy->go.active)
			continue;
		CP_BOOL enemyGroundedFlag = 0;
		//printf("collided \n");
		// Player - Wall
		for (int j = 0; j < 10; ++j)
		{
			if (!wallreference[j].active)
				continue;
			if (AABB(enemy->go, wallreference[j]))
			{

				COLLISION_DIRECTION collision_dir = AABB_Direction(enemy->go, wallreference[j]);
				if (collision_dir == COLLISION_TOP)
				{
					enemy->go.pos.y = wallreference[j].pos.y - wallreference[j].height / 2.f - enemy->go.height / 2.f;
					enemy->grounded = 1;
					enemy->go.vel.y = 0;
				}
				else if (collision_dir == COLLISION_BOTTOM)
				{
					enemy->go.pos.y = wallreference[j].pos.y + wallreference[j].height / 2.f + enemy->go.height / 2.f;				
					enemy->go.vel.y = 0;
				}
				else if (collision_dir == COLLISION_LEFT)
				{
					enemy->state = STATE_ENEMY_ACTIVE;
					enemy->go.pos.x = wallreference[j].pos.x - wallreference[j].width / 2.f - enemy->go.width / 2.f;
					enemy->go.dir.x = -1;
					//printf("left collision \n");
				}
				else
				{
					enemy->state = STATE_ENEMY_ACTIVE;
					enemy->go.pos.x = wallreference[j].pos.x + wallreference[j].width / 2.f + enemy->go.width / 2.f;					
					enemy->go.dir.x = 1;
					//printf("right collision \n");
				}
			}

			// Grounded check (Walking off platforms)
			if (!enemy->grounded)
				continue;
			// This creates a point right below the player and check if there is a wall there
			if (Point_AABB(CP_Vector_Set(enemy->go.pos.x + enemy->go.width / 2.f, enemy->go.pos.y + enemy->go.height / 2.f + 0.1f), wallreference[j]) ||
				Point_AABB(CP_Vector_Set(enemy->go.pos.x - enemy->go.width / 2.f, enemy->go.pos.y + enemy->go.height / 2.f + 0.1f), wallreference[j]))
			{
				enemyGroundedFlag = 1;
				//enemy->go.dir.y = 0;

			}
		}
		if (!enemyGroundedFlag)
			enemy->grounded = 0;		
	}
}


