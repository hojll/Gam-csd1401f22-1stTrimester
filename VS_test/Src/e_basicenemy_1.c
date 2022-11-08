#include <stdio.h>
#include <stdlib.h>

#include "e_basicenemy_1.h"
#include "messenger.h"
#include "collision.h"
#include "utilities.h"


Messenger g_messenger;
static const float E_SPEED = 200.f;
static const float GRAVITY = 70.f;
static const float MAX_GRAV_VEL = 600.f;
static const float JUMP_VEL = -1500.f;
static const float JUMP_RANGE = 10.f;

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

void ResetEnemy(E_Basic_Enemy_1* enemy)
{
	
	enemy->state = STATE_ENEMY_ACTIVE;
	enemy->HP = 0;
	enemy->go.active = 0;
	enemy->go.height = 50.f;
	enemy->go.width = 50.f;
	enemy->grounded = 0;
	enemy->go.dir.y = 0;
	enemy->go.vel.y = 0;
	enemy->tracking = 0;
	enemy->myfloor = NULL;
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

				}
				else
				{
					enemy->state = STATE_ENEMY_ACTIVE;
					enemy->go.pos.x = wallreference[j].pos.x + wallreference[j].width / 2.f + enemy->go.width / 2.f;					
					enemy->go.dir.x = 1;

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


void EnemyPathing(E_Basic_Enemy_1* enemy, GameObject nodes[], E_Player* player, GameObject* prevfloor, int size)
{
	if (!enemy->tracking)
		return;
	if (enemy->myfloor == NULL)
		return;


	GameObject* node_ptr = nodes;
	GameObject* shortest_node = NULL;

	float shortest_dist = 0;
	//CP_Vector shortest_point = CP_Vector_Set(0, 0);

	int direction = 0;
	CP_BOOL ycheck = 0;
	ycheck = player->go.pos.y < enemy->go.pos.y ? 1 : 0;

	// move towards player if same floor 
	// if player changes floor change pathing 
	if (prevfloor != enemy->myfloor && enemy->grounded)
	{ 
		//if different floor path up or down
		// check up of down
		if (ycheck)
		{			
			// Assign one node first for checking 
			float node_dist_check = fabsf(CP_Vector_Distance((node_ptr)->pos, enemy->go.pos));
			shortest_dist = node_dist_check;
			shortest_node = node_ptr;
			// Get closest node / jumping points
			for (int i = 0; i < size; ++i)
			{
				if ((node_ptr + i)->active == 0)
					continue;
				if ((node_ptr + i) == NULL)
					continue;
				if ((node_ptr + i)->pos.y < enemy->go.pos.y) // ignore platforms that are above me
					continue;
				node_dist_check = fabsf(CP_Vector_Distance((node_ptr + i)->pos, enemy->go.pos));				
				// distance check between nodes
				if (node_dist_check < shortest_dist)
				{
					shortest_dist = node_dist_check;
					shortest_node = node_ptr + i;			
				}
				enemy->go.dir.x = shortest_node->pos.x > enemy->go.pos.x ? 1 : -1;
			}

			// OCD 
			CP_Vector shortestpoint_xonly = shortest_node->pos;
			shortestpoint_xonly.y = 0;
			CP_Vector enemypos_xonly = enemy->go.pos;
			enemypos_xonly.y = 0;

			// Check how close the enemy needs to be before jumping
			float rangecheck = CP_Vector_Distance(shortestpoint_xonly, enemypos_xonly);			
			GameObject* shortest_node2jump = shortest_node;
			float shortest_dist2 = 0;
			
			int trigger2 = 0;
			if (rangecheck <= JUMP_RANGE)
			{
				// find the closest point to jump to 
				for (int i = 0; i < size; ++i)
				{
					if ((node_ptr + i)->active == 0)
						continue;
					if ((node_ptr + i) == NULL)
						continue;
					if ((node_ptr + i)->pos.y > enemy->go.pos.y) // ignore platforms that are below me
						continue;
					if ((node_ptr + i) == shortest_node) // ignore the shortest that i am running 2 
						continue;

					float player_2_currentNode_distance = fabsf(CP_Vector_Distance(player->go.pos, (node_ptr + i)->pos));
					float player_2_prevNode_distance = fabsf(CP_Vector_Distance(player->go.pos, shortest_node2jump->pos));
					float enemy_2_node_distance = fabsf(CP_Vector_Distance((node_ptr + i)->pos, enemy->go.pos));
					if (trigger2 == 0)
					{
						// set first occurence
						shortest_node2jump = node_ptr + i;
						shortest_dist2 = enemy_2_node_distance;
						trigger2 = 1;
					}
					else
					{
						// if so happen the platform i am going to jump to have the same Y value.
						if ((node_ptr + i)->pos.y == shortest_node2jump->pos.y)
						{
							// check which platform the player is closest 2
							if (player_2_prevNode_distance > player_2_currentNode_distance)
							{
								shortest_dist2 = enemy_2_node_distance;
								shortest_node2jump = node_ptr + i;
							}
						}
						else
						{
							// else just find the closest platform 2 jump 2
							if (shortest_dist2 > enemy_2_node_distance)
							{
								shortest_dist2 = enemy_2_node_distance;
								shortest_node2jump = node_ptr + i;
							}
						}
					}	
				}				
				// do jumping magic
				enemy->go.dir.x = shortest_node2jump->pos.x > enemy->go.pos.x ? 1 : -1;
				enemy->grounded = 0;
				enemy->go.vel.y = JUMP_VEL;
			}
		}
		else
		{
			float node_dist_check = fabsf(CP_Vector_Distance((node_ptr)->pos, enemy->go.pos));
			shortest_dist = node_dist_check;
			shortest_node = node_ptr;
			// Get closest node / descending point
			for (int i = 0; i < size; ++i)
			{
				if ((node_ptr + i)->active == 0)
					continue;
				if ((node_ptr + i) == NULL)
					continue;

				node_dist_check = fabsf(CP_Vector_Distance((node_ptr + i)->pos, enemy->go.pos));
				// distance check between nodes				
				if (node_dist_check < shortest_dist)
				{
					shortest_dist = node_dist_check;
					shortest_node = node_ptr + i;
				}
			}

			// find the next floor to descend to
			int trigger2 = 0;
			GameObject* shortest_node2jump = shortest_node;
			float shortest_dist2 = 0;
			for (int i = 0; i < size; ++i)
			{
				if ((node_ptr + i)->active == 0)
					continue;
				if ((node_ptr + i) == NULL)
					continue;
				if ((node_ptr + i) == shortest_node) // ignore the shortest that i am running 2 
					continue;
				if ((node_ptr + i)->pos.y < enemy->go.pos.y)
					continue;
				
				float player_2_currentNode_distance = fabsf(CP_Vector_Distance(player->go.pos, (node_ptr + i)->pos));
				float player_2_prevNode_distance = fabsf(CP_Vector_Distance(player->go.pos, shortest_node2jump->pos));
				float enemy_2_node_distance = fabsf(CP_Vector_Distance((node_ptr + i)->pos, enemy->go.pos));
				if (trigger2 == 0)
				{
					// set first occurence
					shortest_node2jump = node_ptr + i;
					shortest_dist2 = enemy_2_node_distance;
					trigger2 = 1;
				}
				else
				{
					// if so happen the platform i am going to jump to have the same Y value.
					if ((node_ptr + i)->pos.y == shortest_node2jump->pos.y)
					{
						// check which platform the player is closest 2
						if (player_2_prevNode_distance > player_2_currentNode_distance)
						{
							shortest_dist2 = enemy_2_node_distance;
							shortest_node2jump = node_ptr + i;
						}
					}
					else
					{
						// else just find the closest platform 2 jump 2
						if (shortest_dist2 > enemy_2_node_distance)
						{
							shortest_dist2 = enemy_2_node_distance;
							shortest_node2jump = node_ptr + i;
						}
					}
				}
			
			}		
			enemy->go.dir.x = shortest_node2jump->pos.x > enemy->go.pos.x ? 1 : -1;

		}
	}
	else if (prevfloor == enemy->myfloor) // if same floor move towards player
	{
		float dir = player->go.pos.x > enemy->go.pos.x ? 1 : -1;
		enemy->go.dir.x = dir;
	}
}


void SpawnEnemy(int type, CP_Vector pos)
{
	SpawnEnemyMessage enemy;
	enemy.position = pos;
	enemy.type = type;
	enemy.tracking = 0;
	enemy.score = 0;
	switch (type)
	{
	case 0:
		enemy.score = 100;
		break;
	case 1:
		enemy.tracking = 1;
		enemy.score = 200;
		break;
	case 2:
		break;
	}

	g_messenger.messages[MSG_SPAWN_ENEMY1](&enemy);
}


