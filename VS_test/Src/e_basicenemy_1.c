//All content © 2021 DigiPen (SINGAPORE) Corporation, all rights reserved.
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
static const float JUMP_VEL1 = -2300.f;
static const float JUMP_VEL2 = -1150.f;
static const float JUMP_VEL3 = -1200.f;
static const float JUMP_RANGE = 20.f;
static const float ACTIVE_ANIM_SPEED = 0.05f;
static const float RED_TINT_DECAY_RATE = 0.7f;
static const float dedgeSpeedX = 500.f;

enum {
	FRAME_DIM_WIDTH = 48,//41,
	FRAME_DIM_HEIGHT = 48,//38,
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
		if (enemy->hitstun > 0) {
			enemy->hitstun -= 1 * CP_System_GetDt();
			
			return;
		}
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

			if (enemy->enemy_shortestNode)
			{
				enemy->floatingtimer += 1 * CP_System_GetDt();
				enemy->go.pos.x = EaseInOutQuint(enemy->go.pos.x, enemy->enemy_shortestNode->pos.x, enemy->floatingtimer / 10);
				enemy->go.pos.y = EaseInOutQuint(enemy->go.pos.y, enemy->enemy_shortestNode->pos.y, enemy->floatingtimer / 10);
			}
			//printf("??? %.1f %.1f\n", enemy->enemy_shortestNode->pos.x, enemy->enemy_shortestNode->pos.y);
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
	int end = 2;
	enemy->floatingtimer += 1 * CP_System_GetDt();
	if (enemy->floatingtimer > end) {
		enemy->go.active = 0;
		enemy->isDying = 0;
		ResetEnemy(enemy);
	}
	else {
		enemy->go.pos.x = EaseOutExpo(enemy->go.pos.x, enemy->go.prevPos.x, enemy->floatingtimer / end);
		enemy->go.pos.y = EaseOutExpo(enemy->go.pos.y, enemy->go.prevPos.y, enemy->floatingtimer / end);
	}

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

E_Basic_Enemy* InitializeEnemy_1(E_Basic_Enemy* enemy)
{
	enemy->Update[STATE_ENEMY_DEAD] = Enemy_DeadUpdate;
	enemy->Update[STATE_ENEMY_ACTIVE] = Enemy_ActiveUpdate;

	enemy->animData = enemy1Animations;
	enemy->state = STATE_ENEMY_ACTIVE;
	enemy->HP = 0;
	enemy->go.active = 0;
	enemy->go.height = 100.f;
	enemy->go.width = 100.f;
	enemy->grounded = 0;
	enemy->go.dir.y = 0;
	enemy->go.vel.y = 0;
	enemy->tracking = 0;
	enemy->myfloor = NULL;
	enemy->floatingtimer = 0;
	enemy->currAnim = SetSpriteAnim(&enemy1Animations[ANIM_ENEMY_1_ACTIVE_MOVING], ACTIVE_ANIM_SPEED);
	enemy->type = ENEMY_TYPE_1;
	enemy->redTintVal = 0.f;
	enemy->hitstun = 0;
	enemy->isDying = 0;
	// random direction
	int randomdir = returnRange(1, 20);
	if (randomdir >= 10)
	{
		enemy->go.dir.x = 1;
	}
	else
	{
		enemy->go.dir.x = -1;
	}
	return enemy;
}

void ResetEnemy(E_Basic_Enemy* enemy)
{
	
	enemy->state = STATE_ENEMY_ACTIVE;
	enemy->HP = 0;
	enemy->go.active = 0;
	enemy->go.height = 100.f;
	enemy->go.width = 100.f;
	enemy->grounded = 0;
	enemy->go.dir.y = 0;
	enemy->go.vel.y = 0;
	enemy->tracking = 0;
	enemy->isDying = 0;
	enemy->myfloor = NULL;
}

int EnemyTakeDamage(E_Basic_Enemy* enemy, int dmg, GameObject *bullet)
{
	if ((enemy->HP -= dmg) <= 0) {
		enemy->floatingtimer = 0.0f;
		enemy->go.dir.x = 0;
		enemy->go.dir.y = 0;
		// nice
		if (bullet->faceDir < 1) {
			enemy->go.prevPos.x = -60;
		}
		else
			enemy->go.prevPos.x = 1980;

		enemy->go.prevPos.y = CP_Random_RangeFloat(-20, enemy->go.pos.y - 30);

		enemy->isDying = 1;
		enemy->state = STATE_ENEMY_DEAD;
		return 0;
	}
		//return enemy->go.active = 0;
	enemy->redTintVal = 255.f;
	enemy->hitstun = 0.05f;
	return enemy->go.active;
}

// Helper function
void InitEnemyList(E_Basic_Enemy arr[], int size, GameObject nodes[])
{
	srand(123);	
	for (int i = 0; i < size; ++i)
	{
		InitializeEnemy_1(&arr[i]);
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
	if (enemy->isDying == 1)
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
					if (enemy->enemytype == 0)
						enemy->go.vel.y = 0;
				}
				else if (collision_dir == COLLISION_LEFT)
				{
					enemy->state = STATE_ENEMY_ACTIVE;
					enemy->go.pos.x = wallreference[j].pos.x - wallreference[j].width / 2.f - enemy->go.width / 2.f;
					if (enemy->enemytype == 0)
						enemy->go.dir.x = -1;

				}
				else
				{
					enemy->state = STATE_ENEMY_ACTIVE;
					enemy->go.pos.x = wallreference[j].pos.x + wallreference[j].width / 2.f + enemy->go.width / 2.f;					
					if (enemy->enemytype == 0)
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

// UPDATED ENEMY PATHING CODE
float GameObjectDistance(GameObject object1, GameObject object2, int mode)
{
	if (mode > 0)
		return fabsf(CP_Math_Distance(object1.pos.x, 0, object2.pos.x, 0));
	if (mode < 0)
		return fabsf(CP_Math_Distance(0, object1.pos.y, 0, object2.pos.y));
	return fabsf(CP_Vector_Distance(object1.pos, object2.pos));
}

float GameObjectDistance2(CP_Vector object1, GameObject object2, int mode)
{
	if (mode > 0)
		return fabsf(CP_Math_Distance(object1.x, 0, object2.pos.x, 0));
	if (mode < 0)
		return fabsf(CP_Math_Distance(0, object1.y, 0, object2.pos.y));
	return fabsf(CP_Vector_Distance(object1, object2.pos));
}



// Enemy pathing finding 4 electric boogaloo

float getDist(CP_Vector pos, CP_Vector pos2, int type)
{
	if (type > 0)
		return fabsf(CP_Math_Distance(pos.x, 0, pos2.x, 0));
	if (type < 0)
		return fabsf(CP_Math_Distance(0, pos.y, 0, pos2.y));
	return fabsf(CP_Vector_Distance(pos, pos2));
}

GameObject *getShortestNode2(GameObject *enemy, GameObject nodes[], int size)
{
	GameObject *returnNode = NULL;
	CP_Vector enemyPivot;
	enemyPivot = CP_Vector_Set(enemy->pos.x, enemy->pos.y + enemy->height * 0.5f);
	float dist = getDist(nodes[0].pos, enemyPivot, 0);
	returnNode = &nodes[0];
	for (int i = 1; i < size; ++i){
		if (nodes[i].pos.y < enemyPivot.y)
			continue;
		float newdist = getDist(nodes[i].pos, enemyPivot, 0);
		if (dist > newdist){
			returnNode = &nodes[i];
			dist = newdist;
		}
	}
	return returnNode;
}

GameObject* shortestNode2PlayeronMyAxis(E_Basic_Enemy* enemy, GameObject *player, GameObject nodes[], int size)
{
	GameObject* returnNode = NULL;
	CP_Vector enemyPivot;
	enemyPivot = CP_Vector_Set(enemy->go.pos.x, enemy->go.pos.y + enemy->go.height * 0.5f);
	float dist = getDist(nodes[0].pos, player->pos, 0);
	returnNode = &nodes[0];
	CP_BOOL check;
	for (int i = 1; i < size; ++i) {
		check = AABB(nodes[i], *enemy->myfloor);
		if (check == 0)
			continue;
		float newDist = getDist(nodes[i].pos, player->pos, 0);
		if (dist > newDist) {
			dist = newDist;
			returnNode = &nodes[i];
		}
	}
	return returnNode;
}

GameObject *getNodeonSameAxis(GameObject *player, GameObject* currNode, GameObject nodes[], int size)
{
	GameObject* returnNode = NULL;
	returnNode = &nodes[0];
	float dist = 9999;
	for (int i = 0; i < size; ++i) {
		if (nodes[i].pos.y != currNode->pos.y)
			continue;
		float newDist = getDist(nodes[i].pos, player->pos,0);
		if (dist > newDist) {
			returnNode = &nodes[i];
		}
	}

	return returnNode;
}

GameObject* getNodeHigher(GameObject* enemy, GameObject* currNode, GameObject nodes[], int size)
{
	GameObject* returnNode = currNode;
	float dist = 9999;
	for (int i = 0; i < size; ++i) {
		if (nodes[i].pos.y > enemy->pos.y)
			continue; // ignroe if below me
		if (&nodes[i] == &currNode)
			continue;
		float newdist = getDist(nodes[i].pos, enemy->pos, 0);
		if (newdist < dist) {
			dist = newdist;
			returnNode = &nodes[i];
		}
	}
	return returnNode;
}

GameObject* getNodeLower(GameObject* enemy, GameObject* currNode, GameObject nodes[], int size)
{
	GameObject* returnNode = currNode;
	float dist = 9999;
	for (int i = 0; i < size; ++i) {
		if (nodes[i].pos.y < enemy->pos.y)
			continue; // ignroe if above me
		if (&nodes[i] == &currNode)
			continue;
		float newdist = getDist(nodes[i].pos, enemy->pos, 0);
		if (newdist < dist) {
			dist = newdist;
			returnNode = &nodes[i];
		}
	}
	return returnNode;
}


void EnemyPathing4(E_Basic_Enemy* enemy, E_Player* player, GameObject* p_prevfloor, int size, 
	GameObject walls[], GameObject nodes[], GameObject nodes2[])
{
	if (!enemy->go.active)
		return;
	if (enemy->enemytype == 1)
		if (p_prevfloor == NULL || enemy->myfloor == NULL)
			return;
	CP_Vector enemyLegs;
	enemyLegs = CP_Vector_Set(enemy->go.pos.x, enemy->go.pos.y + enemy->go.height * 0.5f);
	switch (enemy->enemytype)
	{
	case 0:
		break;
	case 1:
	{
		//printf("enemy 1 working \n");
		if (enemy->floatingtimer > 0.f) {
			enemy->floatingtimer -= 1 * CP_System_GetDt();
		}
		//printf("enemy distance %.1f\n", getDist(player->go.pos, enemyLegs, -1));
		if ((p_prevfloor != enemy->myfloor) && (enemy->floatingtimer <= 0.f)) {
			//enemy->enemy_shortestNode = getShortestNode2(&enemy->go, nodes, size);
			enemy->enemy_shortestNode = shortestNode2PlayeronMyAxis(&enemy->go, &player->go, nodes, size);

			if (getDist(enemy->enemy_shortestNode->pos, enemy->go.pos, 1) <= JUMP_RANGE) {
				if (player->go.pos.y >= enemy->go.pos.y + enemy->go.height * 0.5f) { // lower
					//printf("falling\n");
					enemy->enemy_shortestNode = getNodeLower(&enemy->go, enemy->enemy_shortestNode, nodes, size);
					enemy->floatingtimer = 0.57f;
				}
				else { // higher
					//printf("jumping\n");
					enemy->enemy_shortestNode = getNodeHigher(&enemy->go, enemy->enemy_shortestNode, nodes, size);
					enemy->grounded = 0;
					enemy->go.vel.y = JUMP_VEL1;
					enemy->floatingtimer = 0.57f;
				}
			}
			enemy->go.dir.x = enemy->enemy_shortestNode->pos.x > enemy->go.pos.x ? 1 : -1;
		}
		if (p_prevfloor == enemy->myfloor)
		{
			if (enemy->enemy_shortestNode)
				enemy->enemy_shortestNode = NULL;
			enemy->go.dir.x = player->go.pos.x > enemy->go.pos.x ? 1 : -1;
		}
		//enemy->go.dir.x = 0;
		/*if (enemy->enemy_shortestNode) {
			CP_Settings_Stroke(CP_Color_Create(255, 0, 0, 255));
			CP_Graphics_DrawLine(enemy->go.pos.x, enemy->go.pos.y + enemy->go.height * 0.5f, enemy->enemy_shortestNode->pos.x, enemy->enemy_shortestNode->pos.y);
		}*/
	}
		break;
	case 2:
	{
		if (enemy->enemy_shortestNode == NULL)
		{
			// random new locatioin
			int newpos = CP_Random_RangeInt(1, 4);
			switch (newpos)
			{
			case 1:
				enemy->enemy_shortestNode = &nodes2[0];
				break;
			case 2:
				enemy->enemy_shortestNode = &nodes2[1];
				break;
			case 3:
				enemy->enemy_shortestNode = &nodes2[2];
				break;
			case 4:
				enemy->enemy_shortestNode = &nodes2[3];
				break;
			}
			if (enemy->enemy_shortestNode != NULL)
			{
				//printf("pos %.1f %.1f \n", enemy->enemy_shortestNode->pos.x, enemy->enemy_shortestNode->pos.y);

				enemy->go.dir = CP_Vector_Normalize(CP_Vector_Set(enemy->enemy_shortestNode->pos.x - enemy->go.pos.x,
					enemy->enemy_shortestNode->pos.y - enemy->go.pos.y));
			}
		}
		else
		{
			if (GameObjectDistance(*enemy->enemy_shortestNode, enemy->go, 0) <= 10)
			{
				enemy->floatingtimer = 0;
				enemy->enemy_shortestNode = NULL;
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


