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
#define JUMP_RANGE 10

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

		//if (enemy->grounded)
		//{
		//	printf("grounded\n");
		//}
		//else
		//{
		//	printf("not grounded\n");
		//}
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


	GameObject* node_ptr = walls;
	float shortest_dist = 0;
	CP_Vector shortest_point = CP_Vector_Set(0, 0);

	int direction = 0;
	CP_BOOL ycheck = 0;

	for (int i = 0; i < size; ++i)
	{
		int istaller = 0;
		if ((node_ptr + i)->pos.y < enemy->go.pos.y)
			istaller = 1;

		float dist_check_debug = fabsf(CP_Vector_Distance((node_ptr + i)->pos, enemy->go.pos));
		//printf("%i) x: %.2f y: %.2f | Dist %.2f | taller %i\n", i, (node_ptr + i)->pos.x, (node_ptr + i)->pos.y, dist_check_debug, istaller);
	}
	//printf("\n");
	//exit(0);



	// move towards player if same floor 
	// if player changes floor change pathing 
	if (prevfloor != enemy->myfloor && enemy->grounded)
	{ 
		//if different floor path up or down
		// check up of down
		ycheck = player->go.pos.y < enemy->go.pos.x ? 1 : -1;
		if (ycheck)
		{
			int trigger = 0;
			// get closest node
			for (int i = 0; i < size; ++i)
			{
				if ((node_ptr + i)->active == 0)
					continue;
				if ((node_ptr + i) == NULL)
					continue;
				if ((node_ptr + i)->pos.y < enemy->go.pos.y) // ignore platforms that are above me
					continue;
				float node_dist_check = fabsf(CP_Vector_Distance((node_ptr + i)->pos, enemy->go.pos));
				if (trigger == 0)
				{
					shortest_dist = node_dist_check;
					shortest_point = CP_Vector_Set((node_ptr + i)->pos.x, (node_ptr + i)->pos.y);
					trigger = 1;
				}
				// distance check between nodes

				if (node_dist_check < shortest_dist)
				{
					shortest_dist = node_dist_check;
					shortest_point = CP_Vector_Set((node_ptr + i)->pos.x, (node_ptr + i)->pos.y);
				}

				enemy->go.dir.x = shortest_point.x > enemy->go.pos.x ? 1 : -1;
			}
			//printf("shortest point is %.2f,%.2f\n", shortest_point.x, shortest_point.y);			
			// When reaching close to the node

			CP_Vector shortestpoint_xonly = shortest_point;
			shortestpoint_xonly.y = 0;
			CP_Vector enemypos_xonly = enemy->go.pos;
			enemypos_xonly.y = 0;
			CP_Vector shortest_point2 = CP_Vector_Set(0, 0);

			float rangecheck = CP_Vector_Distance(shortestpoint_xonly, enemypos_xonly);
			int secondtrigger = 0;
			float shortestnode_2player = 0;
			if (rangecheck <= JUMP_RANGE)
			{
				// find the closest point to jump to 
				for (int i = 0; i < size; ++i)
				{
					if ((node_ptr + i)->active == 0)
						continue;
					if ((node_ptr + i) == NULL)
						continue;
					// if same point ignore
					if (CP_Vector_Equal((node_ptr + i)->pos, shortest_point)) 
						continue;
					// Have to take in account where the player is going to stand
					shortestnode_2player = CP_Vector_Distance(player->go.pos, (node_ptr + i)->pos);
					if (secondtrigger == 0)
					{
						shortest_dist = shortestnode_2player;
						shortest_point = CP_Vector_Set((node_ptr + i)->pos.x, (node_ptr + i)->pos.y);
						secondtrigger = 1;
					}
					// distance check between nodes

					if (shortestnode_2player < shortest_dist)
					{
						shortest_dist = shortestnode_2player;
						shortest_point = CP_Vector_Set((node_ptr + i)->pos.x, (node_ptr + i)->pos.y);
					}
				}
				
				enemy->go.dir.x = shortest_point.x > enemy->go.pos.x ? 1 : -1;
				enemy->grounded = 0;
				enemy->go.vel.y = JUMP_VEL;

			}
		}
		else
		{

		}
	}
	else if (prevfloor == enemy->myfloor) // if same floor move towards player
	{
		float dir = player->go.pos.x > enemy->go.pos.x ? 1 : -1;
		enemy->go.dir.x = dir;
	}
}