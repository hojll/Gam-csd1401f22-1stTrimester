#ifndef E_BASIC_ENEMY_2_H
#define E_BASIC_ENEMY_2_H

#include "entities.h"
#include "e_player.h"

#define MAX_PATHFINDING_NODES 6
/*-----------------------------------------------------------------*/
// E_BASICENEMY ENTITY 2
/*-----------------------------------------------------------------*/
typedef struct E_Basic_Enemy_2 E_Basic_Enemy_2;

typedef enum E_BASIC_ENEMY_STATES_2 {
	STATE_ENEMY2_DEAD,
	STATE_ENEMY2_ACTIVE,
	STATE_ENEMY2_MOVING,
	STATE_ENEMY2_JUMPING,
	STATE_ENEMY2_NUM_STATES,
}E_BASIC_ENEMY_STATES_2;
typedef struct E_Basic_Enemy_2 {
	GameObject go;
	E_BASIC_ENEMY_STATES_2 state;

	CP_BOOL grounded;
	CP_BOOL tracking;

	GameObject* nodes;
	GameObject* myfloor;

	GameObject e_path[MAX_PATHFINDING_NODES];
	int pathcount;
	// Stats
	double HP;
	int enemytype;
	int enemyscore;


	void (*Update[STATE_ENEMY_NUM_STATES])(E_BASIC_ENEMY_STATES_2*);
} E_Basic_Enemy_2;

/*-----------------------------------------------------------------*/
// Other references
/*-----------------------------------------------------------------*/

/*-----------------------------------------------------------------*/
// Functions 
/*-----------------------------------------------------------------*/
E_Basic_Enemy_2 *InitializeEnemy2(E_Basic_Enemy_2 *enemy);

void InitEnemyList2(E_Basic_Enemy_2 arr[], int size, GameObject nodes[]);

void UpdateEnemyList2(E_Basic_Enemy_2 arr[], int size);

void EnemytoWallCollision2(E_Basic_Enemy_2* enemy, GameObject wallreference[]);

void EnemyPathing2(E_Basic_Enemy_2* enemy, GameObject nodes[], E_Player* player, GameObject* prevfloor, int size);

void SpawnEnemy2(int type, CP_Vector pos);

void ResetEnemy2(E_Basic_Enemy_2* enemy);
#endif