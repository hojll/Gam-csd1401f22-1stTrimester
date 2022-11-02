#include <stdio.h>
#include <stdlib.h>

#include "e_basicenemy_1.h"
#include "messenger.h"
#include "collision.h"
#include "utilities.h"


Messenger g_messenger;
#define E_SPEED 150
#define GRAVITY 70.f
#define MAX_GRAV_VEL 600.f
#define JUMP_VEL -980
#define JUMP_RANGE 5
/*----------------------------------------------------*/
// ENEMI
/*----------------------------------------------------*/

void Enemy_ActiveUpdate(E_Basic_Enemy_1 *enemy)
{
	if (enemy->go.active)
	{
		
		enemy->go.vel.x = enemy->go.dir.x * E_SPEED;
		enemy->go.pos.x +=  enemy->go.vel.x * g_scaledDt;
		enemy->go.pos.y += enemy->go.vel.y * g_scaledDt;
		if (!enemy->grounded && enemy->go.vel.y < MAX_GRAV_VEL)
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
	retval.myfloor = NULL;
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


		arr[i].Update[arr[i].state](&arr[i]);
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
					enemy->myfloor = &wallreference[j];
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


void EnemyPathing(E_Basic_Enemy_1* enemy, GameObject walls[], E_Player* player, GameObject* prevfloor, int size)
{
	if (!enemy->tracking)
		return;
	if (enemy->myfloor == NULL)
		return;
	

	GameObject *nextfloor = walls;
	float shortest_dist = 0;
	int direction = 0;

	// Debug
	//for (int i = 0; i < size; ++i)
	//{
	//	printf("wall %2f : %2f\n", (nextfloor + i)->pos.x, (nextfloor + i)->pos.y);		
	//}
	
	// move towards player if same floor 
	// if player changes floor change pathing 
	if (prevfloor != enemy->myfloor && enemy->grounded){ //if different floor path up or down
		CP_BOOL up_or_down;
		up_or_down = (&player->go.pos.y < &enemy->go.pos.y) ? 0 : 1;
		//printf("up or down %d\n", up_or_down);
		if (up_or_down){	// Path up		
			CP_Vector floorposx_0 = CP_Vector_Set(0, 0);
			CP_Vector floorposx_1 = enemy->go.pos;
			// Move to node*
			// floor node*
			//direction = enemy->myfloor->pos.x > enemy->go.pos.x ? 1 : -1;			
			//enemy->go.dir.x = (float)direction;
			
			// find closest node
			float prevdist = 0;
			for (int i = 0; i < size; ++i)
			{
				if ((nextfloor + i) == NULL)
					continue;
				if ((nextfloor + i)->active == 0)
					continue;



				CP_Vector xdistance_only1 = (nextfloor + i)->pos;
				CP_Vector xdistance_only2 = enemy->go.pos;
				float currdist = fabsf(CP_Vector_Distance(xdistance_only1,xdistance_only2));				
				if (i > 0)
				{
					
					if (prevdist > currdist)
					{
						prevdist = currdist;
						direction = (nextfloor + i)->pos.x > enemy->go.pos.x ? 1 : -1;
						floorposx_0 = (nextfloor + i)->pos;
					}
				}
				else
				{
					prevdist = currdist;
					direction = (nextfloor + i)->pos.x > enemy->go.pos.x ? 1 : -1;
					floorposx_0 = (nextfloor + i)->pos;
				}
			}
			enemy->go.dir.x = (float)direction;
			printf("player pos %2f,%2f\n", player->go.pos.x, player->go.pos.y);
			printf("Enemy pos %2f,%2f\n", enemy->go.pos.x, enemy->go.pos.y);
			printf("floor pos %2f,%2f\n", floorposx_0.x, floorposx_0.y);
			// range calc to jump once near node
			//floorposx_0 = enemy->myfloor->pos;
			floorposx_1 = enemy->go.pos;
						
			floorposx_0.y = 0;
			floorposx_1.y = 0;
			
			calc_distance = CP_Vector_Distance(floorposx_0, floorposx_1);
			
			if (calc_distance <= JUMP_RANGE )
			{
				// jump here
				
				for (int i = 0; i < size; ++i)
				{
					if ((nextfloor + i) == NULL)
						continue;
					if ((nextfloor + i)->active == 0)
						continue;
					if ((nextfloor + i) == enemy->myfloor)
						continue;


					//printf("wall %2f : %2f\n", (nextfloor + i)->pos.x, (nextfloor + i)->pos.y);
					float check_distance = fabsf(CP_Vector_Distance((nextfloor + i)->pos, enemy->go.pos));
					if (i == 0)
					{
						shortest_dist = check_distance;
						direction = (nextfloor + i)->pos.x > enemy->go.pos.x ? 1 : -1;
					}
					else
					{
						if (shortest_dist < check_distance)
						{
							shortest_dist = check_distance;
							direction = (nextfloor + i)->pos.x > enemy->go.pos.x ? 1 : -1;
						}
					}
				}
				enemy->go.dir.x = direction;

				enemy->grounded = 0;
				enemy->go.vel.y = JUMP_VEL;
			}
		}
		else { //path down

		}		
	}
	else if (prevfloor == enemy->myfloor) // if same floor move towards player
	{
		int dir = player->go.pos.x > enemy->go.pos.x ? 1 : -1;
		enemy->go.dir.x = (float)dir;
	}
}