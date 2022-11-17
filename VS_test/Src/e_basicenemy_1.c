#include <stdio.h>
#include <stdlib.h>

#include "e_basicenemy_1.h"
#include "messenger.h"
#include "collision.h"
#include "utilities.h"
#include "easing.h"

Messenger g_messenger;
SpriteAnimData enemy1Animations[ANIM_ENEMY_1_NUM_ANIM];

static const float E_SPEED = 350.f;
static const float E_SPEED2 = 450.f;
static const float GRAVITY = 200.f;
static const float MAX_GRAV_VEL = 1500.f;
static const float JUMP_VEL1 = -2500.f;
static const float JUMP_VEL2 = -1150.f;
static const float JUMP_VEL3 = -1200.f;
static const float JUMP_RANGE = 20.f;
static const float ACTIVE_ANIM_SPEED = 0.05f;
static const float RED_TINT_DECAY_RATE = 0.7f;
enum {
	FRAME_DIM_WIDTH = 48,
	FRAME_DIM_HEIGHT = 48,
	IMAGE_DIM_WIDTH = 192,
	IMAGE_DIM_HEIGHT = 48,
	NUM_FRAMES = 4
};

/*----------------------------------------------------*/
// ENEMI
/*----------------------------------------------------*/

void Enemy_ActiveUpdate(E_Basic_Enemy *enemy)
{
	if (enemy->go.active)	// TODO: Remove either this one or updateenemylist active check
	{
		UpdateSpriteAnim(&enemy->currAnim, g_scaledDt);
		//if(enemy->tracking == 0)
		//	enemy->go.vel.x = enemy->go.dir.x * E_SPEED;
		//else
		//	enemy->go.vel.x = enemy->go.dir.x * E_SPEED2;

		switch (enemy->enemytype)
		{
		case 0:
			enemy->go.vel.x = enemy->go.dir.x * E_SPEED;
			enemy->go.pos.x += enemy->go.vel.x * g_scaledDt;
			enemy->go.pos.y += enemy->go.vel.y * g_scaledDt;

			break;
		case 1:
			enemy->go.vel.x = enemy->go.dir.x * E_SPEED2;
			enemy->go.pos.x += enemy->go.vel.x * g_scaledDt;
			enemy->go.pos.y += enemy->go.vel.y * g_scaledDt;

			break;
		case 2:
			/*enemy->go.vel.x = enemy->go.dir.x * E_SPEED2;
			enemy->go.vel.y = enemy->go.dir.y * E_SPEED2;

			enemy->go.pos.x += enemy->go.vel.x * g_scaledDt;
			enemy->go.pos.y += enemy->go.vel.y * g_scaledDt;*/

			if (enemy->debugshortestnode)
			{
				enemy->floatingtimer += 1 * CP_System_GetDt();
				enemy->go.pos.x = EaseInOutQuint(enemy->go.pos.x, enemy->debugshortestnode->pos.x, enemy->floatingtimer / 10);
				enemy->go.pos.y = EaseInOutQuint(enemy->go.pos.y, enemy->debugshortestnode->pos.y, enemy->floatingtimer / 10);
			}
			//printf("??? %.1f %.1f\n", enemy->debugshortestnode->pos.x, enemy->debugshortestnode->pos.y);
			//printf("enemy dir, enemy speed %.1f %.1f   %.1f%.1f\n", enemy->go.dir.x, enemy->go.dir.y, enemy->go.vel.x, enemy->go.vel.y);
			break;
		default:
			break;
		}
		
		

		if (enemy->enemytype == 0 || enemy->enemytype == 1)
		{
			if (!enemy->grounded && enemy->go.vel.y < MAX_GRAV_VEL)
			{
				enemy->go.vel.y += GRAVITY;
			}
			if (enemy->go.pos.y >= 2000.f)
			{
				enemy->go.pos.y = -10.f;
				enemy->go.pos.x = 960.f;

			}
		}
		enemy->go.faceDir = (int)enemy->go.vel.x;
		enemy->currAnim.flip = enemy->go.faceDir < 0;
		enemy->redTintVal = enemy->redTintVal + (-255.f) * RED_TINT_DECAY_RATE * g_scaledDt;
	}
}

void Enemy_DeadUpdate(E_Basic_Enemy* enemy)
{

}

void InitAnimdata_E1()
{
	{
		SpriteAnimData activeAnim = { 0 };
		activeAnim.frameDim[0] = FRAME_DIM_WIDTH;
		activeAnim.frameDim[1] = FRAME_DIM_HEIGHT;
		activeAnim.imageDim[0] = IMAGE_DIM_WIDTH;
		activeAnim.imageDim[1] = IMAGE_DIM_HEIGHT;

		activeAnim.numFrames = NUM_FRAMES;
		activeAnim.imageStart[0] = 0;
		activeAnim.imageStart[1] = 0;

		activeAnim.loop = 1;
		enemy1Animations[ANIM_ENEMY_1_ACTIVE_MOVING] = activeAnim;
	}
}

E_Basic_Enemy InitializeEnemy_1()
{
	E_Basic_Enemy retval;
	retval.Update[STATE_ENEMY_DEAD] = Enemy_DeadUpdate;
	retval.Update[STATE_ENEMY_ACTIVE] = Enemy_ActiveUpdate;

	retval.state = STATE_ENEMY_ACTIVE;
	retval.HP = 0;
	retval.go.active = 0;
	retval.go.height = 100.f;
	retval.go.width = 100.f;
	retval.grounded = 0;
	retval.go.dir.y = 0;
	retval.go.vel.y = 0;
	retval.tracking = 0;
	retval.myfloor = NULL;
	retval.floatingtimer = 0;
	retval.currAnim = SetSpriteAnim(&enemy1Animations[ANIM_ENEMY_1_ACTIVE_MOVING], ACTIVE_ANIM_SPEED);
	retval.type = ENEMY_TYPE_1;
	retval.redTintVal = 0.f;
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

void ResetEnemy(E_Basic_Enemy* enemy)
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

int EnemyTakeDamage(E_Basic_Enemy* enemy, int dmg)
{
	if ((enemy->HP -= dmg) <= 0) 
		return enemy->go.active = 0;
	enemy->redTintVal = 255.f;
	return enemy->go.active;
}

// Helper function
void InitEnemyList(E_Basic_Enemy arr[], int size, GameObject nodes[])
{
	srand(123);	
	for (int i = 0; i < size; ++i)
	{
		arr[i] = InitializeEnemy_1();
		arr[i].nodes = nodes;
	}
}

void UpdateEnemyList(E_Basic_Enemy arr[], int size)
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

void EnemytoWallCollision(E_Basic_Enemy *enemy, GameObject wallreference[])
{
	if (enemy->enemytype == 2)
		return;

	//hard code for now
	for (int i = 0; i < 10; ++i)
	{
		if (!enemy->go.active)
			continue;
		CP_BOOL enemyGroundedFlag = 0;
		
		// Player - Wall
		for (int j = 0; j < 12; ++j)
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
					enemy->go.pos.y = wallreference[j].pos.y + wallreference[j].height / 2.f + enemy->go.height / 2.f + 5;				
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

void EnemyPathing(E_Basic_Enemy* enemy, GameObject nodes[], E_Player* player, GameObject* prevfloor, int size)
{
	if (!enemy->tracking)
		return;
	if (enemy->myfloor == NULL && enemy->enemytype == 1)
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
				enemy->go.vel.y = JUMP_VEL1;
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

// UPDATED ENEMY PATHING CODE
float GameObjectDistance(GameObject object1, GameObject object2, int mode)
{
	if (mode > 0)
		return fabsf(CP_Math_Distance(object1.pos.x, 0, object2.pos.x, 0));
	if (mode < 0)
		return fabsf(CP_Math_Distance(0, object1.pos.y, 0, object2.pos.y));
	return fabsf(CP_Vector_Distance(object1.pos, object2.pos));
}

GameObject* GetShortestNode(E_Basic_Enemy* enemy, GameObject nodes[], E_Player* player, int size)
{
	GameObject* shortest_Node = &nodes[0];
	float shortest_Distance = GameObjectDistance(enemy->go, nodes[0], 0);
	for (int i = 1; i < size; i++)
	{
		if (nodes[i].pos.y > enemy->go.pos.y + 60)
			continue;
		float distance = GameObjectDistance(enemy->go, nodes[i], 0);
		shortest_Node = shortest_Distance > distance ? &nodes[i] : shortest_Node;
		shortest_Distance = shortest_Distance > distance ? distance : shortest_Distance;
	}
	return shortest_Node;
}

GameObject* Jump2ShortestNode(E_Basic_Enemy* enemy, GameObject nodes[], E_Player* player, int size, GameObject currnode)
{
	GameObject* shortest_Node = &nodes[0];
	float shortest_Distance = GameObjectDistance(enemy->go, nodes[0], 0);
	for (int i = 1; i < size; i++)
	{
		// if nodes are below me 
		if (nodes[i].pos.y > enemy->go.pos.y + 5)
			continue;
		if (&nodes[i] == &currnode)
			continue;
		float distance = GameObjectDistance(enemy->go, nodes[i], 0);


		shortest_Node = shortest_Distance > distance ? &nodes[i] : shortest_Node;
		shortest_Distance = shortest_Distance > distance ? distance : shortest_Distance;
	}

	//CP_Settings_Stroke(CP_Color_Create(0, 255, 255, 255));
	// draw a line from the origin to the mouse position
	//CP_Graphics_DrawLine(shortest_Node->pos.x, shortest_Node->pos.y, enemy->go.pos.x, enemy->go.pos.y);

	return shortest_Node;
}

GameObject* FindShortestNodeBelow(E_Basic_Enemy* enemy, GameObject nodes[], E_Player* player, int size, GameObject currnode)
{
	GameObject* shortest_Node = &nodes[0];
	float shortest_Distance = GameObjectDistance(enemy->go, nodes[0], 0);
	for (int i = 1; i < size; i++)
	{
		if (shortest_Node == &currnode)
			continue;

		float distance = GameObjectDistance(enemy->go, nodes[i], 0);
		shortest_Node = shortest_Distance > distance ? &nodes[i] : shortest_Node;
		shortest_Distance = shortest_Distance > distance ? distance : shortest_Distance;
	}

	//CP_Settings_Stroke(CP_Color_Create(0, 255, 255, 255));
	// draw a line from the origin to the mouse position
	//CP_Graphics_DrawLine(shortest_Node->pos.x, shortest_Node->pos.y, enemy->go.pos.x, enemy->go.pos.y);

	return shortest_Node;
}

GameObject* Xaxis_check(GameObject* currnode, GameObject nodes[], E_Player* player, int size, GameObject walls[])
{
	GameObject* shortestNode = currnode;
	for (int i = 0; i < size; i++)
	{
		if (shortestNode == &nodes[i])
			continue;
		if (shortestNode)
			if (nodes[i].pos.y == shortestNode->pos.y) // if same position check if they same wall // now i need to teach my ai what a platform is
			{
				int breakcheck = 0;
				// check if they are the same platform
				for (int j = 0; j < 12; j++)
				{
					// find wall with the same height
					if (walls[i].pos.y == shortestNode->pos.y)
					{
						CP_BOOL check_1 = 0;
						CP_BOOL check_2 = 0;

						check_1 = AABB(walls[i], nodes[i]);
						check_2 = AABB(walls[i], *shortestNode);
						if (check_1 == 0 || check_2 == 0)
						{
							breakcheck = 1;
							break;
						}
					}
				}
				if (breakcheck == 1)
					continue;


				float distancecheck1 = GameObjectDistance(player->go, *shortestNode, 0);
				float distancecheck2 = GameObjectDistance(player->go, nodes[i], 0);
				if (distancecheck1 > distancecheck2)
					shortestNode = &nodes[i];
			}
	}
	return shortestNode;
}

// tracking 
void EnemyPathing3(E_Basic_Enemy* enemy, GameObject nodes[], E_Player* player, GameObject* prevfloor, int size, GameObject walls[], GameObject nodes2[])
{
	if (!enemy->go.active ||
		enemy->tracking == 0 ||
		(enemy->myfloor == NULL && enemy->enemytype == 1))
		return;

	GameObject* shortest_node = NULL;
	GameObject* jumpNode = NULL;
	float shortestdist;
	int ycheck = 0;
	ycheck = player->go.pos.y < enemy->go.pos.y ? 1 : -1;
	//printf("\nYcheck %d\n", ycheck);
	//printf("ycheck %d\n", ycheck);

	switch (enemy->enemytype)
	{
	case 1:
	{
		if (player->grounded && prevfloor != enemy->myfloor && enemy->grounded)
		{
			if (ycheck == 1)
			{
				enemy->debugshortestnode = GetShortestNode(enemy, nodes, player, size);
				float y_axis_check = GameObjectDistance(player->go, enemy->go, -1);
				float x_axis_check = GameObjectDistance(player->go, enemy->go, 1);
				printf("\n yaxis %.1f   xaxis %.1f\n", y_axis_check, x_axis_check);
				printf("total distance %.1f\n", GameObjectDistance(player->go, enemy->go, 0));

				if (enemy->myfloor != prevfloor && player->go.pos.y != enemy->go.pos.y)
					enemy->debugshortestnode = Xaxis_check(enemy->debugshortestnode, nodes, player, size, walls);


				float jumpingrange = GameObjectDistance(enemy->go, *enemy->debugshortestnode, 1);
				// if can jump
				if (jumpingrange <= JUMP_RANGE)
				{
					// find next node
					enemy->grounded = 0;
					enemy->debugshortestnode = Jump2ShortestNode(enemy, nodes, player, size, *enemy->debugshortestnode);
					//float jumpdischeck = GameObjectDistance(enemy->go, *enemy->debugshortestnode, -1);
					enemy->go.vel.y = JUMP_VEL1;


				}
				enemy->go.dir.x = enemy->debugshortestnode->pos.x > enemy->go.pos.x ? 1 : -1;
			}
			else if (ycheck == -1)
			{
				enemy->debugshortestnode = GetShortestNode(enemy, nodes, player, size);
				float y_axis_check = GameObjectDistance(player->go, enemy->go, -1);
				float x_axis_check = GameObjectDistance(player->go, enemy->go, 1);
				//printf("\n yaxis %.1f   xaxis %.1f\n", y_axis_check, x_axis_check);
				if (y_axis_check > 220.f)
					enemy->debugshortestnode = Xaxis_check(enemy->debugshortestnode, nodes, player, size, walls);



				float walkoffrange = GameObjectDistance(enemy->go, *enemy->debugshortestnode, 1);
				if (walkoffrange <= JUMP_RANGE)
				{
					enemy->grounded = 0;
					enemy->debugshortestnode = FindShortestNodeBelow(enemy, nodes, player, size, *enemy->debugshortestnode);
					enemy->go.vel.y = JUMP_VEL3;
				}
				enemy->go.dir.x = enemy->debugshortestnode->pos.x > enemy->go.pos.x ? 1 : -1;

			}
		}
		else if (prevfloor == enemy->myfloor)
		{
			enemy->go.dir.x = player->go.pos.x > enemy->go.pos.x ? 1 : -1;
		}
	}
		break;
	case 2:
	{
		if (enemy->debugshortestnode == NULL)
		{
			// random new locatioin
			int newpos = CP_Random_RangeInt(1, 4);
			switch (newpos)
			{
			case 1:
				enemy->debugshortestnode = &nodes2[0];
				break;
			case 2:
				enemy->debugshortestnode = &nodes2[1];
				break;
			case 3:
				enemy->debugshortestnode = &nodes2[2];
				break;
			case 4:
				enemy->debugshortestnode = &nodes2[3];
				break;
			}
			if (enemy->debugshortestnode != NULL)
			{
				printf("pos %.1f %.1f \n", enemy->debugshortestnode->pos.x, enemy->debugshortestnode->pos.y);
				
				enemy->go.dir = CP_Vector_Normalize(CP_Vector_Set(enemy->debugshortestnode->pos.x - enemy->go.pos.x, 
					enemy->debugshortestnode->pos.y - enemy->go.pos.y));
			}
		}
		else
		{
			if (GameObjectDistance(*enemy->debugshortestnode, enemy->go, 0) <= 10)
			{
				enemy->floatingtimer = 0;
				enemy->debugshortestnode = NULL;
			}
		}
	}
		break;
	}
	
}

void SpawnEnemy(int type, CP_Vector pos)
{
	SpawnEnemyMessage enemy;
	enemy.position = pos;
	//enemy.type = type;
	enemy.tracking = 0;
	enemy.score = 0;
	switch (type)
	{
	case 0:
		enemy.type = 0;
		enemy.score = 100;
		break;
	case 1:
		enemy.tracking = 1;
		enemy.type = 1;
		enemy.score = 200;
		break;
	case 2:
		enemy.type = 2;
		enemy.tracking = 1;
		enemy.score = 150;

		break;
	}

	g_messenger.messages[MSG_SPAWN_ENEMY1](&enemy);
}


