#ifndef E_BASICENEMY_1
#define E_BASICENEMY_1

#include "entities.h"
#include "e_player.h"

#define MAX_PATHFINDING_NODES 12

/*-----------------------------------------------------------------*/
// E_BASICENEMY ENTITY 1
/*-----------------------------------------------------------------*/
typedef struct E_Basic_Enemy E_Basic_Enemy;
typedef enum E_BASIC_ENEMY_STATES_1 {
	STATE_ENEMY_DEAD,
	STATE_ENEMY_ACTIVE,
	STATE_ENEMY_MOVING,
	STATE_ENEMY_JUMPING,
	STATE_ENEMY_NUM_STATES,
}E_BASIC_ENEMY_STATES_1;
typedef struct E_Basic_Enemy {
	// TODO: MIGRATE ENEMY DETERMINED STATS HERE
	// THAT MEANS CONSTANT VALUES IN .c SHOULD BE REPRESENTED HERE
	GameObject go;
	int state;
	
	short grounded;
	short tracking;

	GameObject *nodes;
	GameObject *myfloor;
	GameObject* debugshortestnode;

	// Stats
	double HP;
	int enemytype;
	int enemyscore;


	void (*Update[STATE_ENEMY_NUM_STATES])(E_BASIC_ENEMY_STATES_1*);
} E_Basic_Enemy;
/*-----------------------------------------------------------------*/
// Other references
/*-----------------------------------------------------------------*/

/*-----------------------------------------------------------------*/
// Functions 
/*-----------------------------------------------------------------*/
E_Basic_Enemy InitializeEnemy_1();

void InitEnemyList(E_Basic_Enemy arr[], int size, GameObject nodes[]);

void UpdateEnemyList(E_Basic_Enemy arr[], int size);

void EnemytoWallCollision(E_Basic_Enemy *enemy, GameObject wallreference[]);

void EnemyPathing(E_Basic_Enemy* enemy, GameObject nodes[], E_Player* player, GameObject* prevfloor, int size);

void EnemyPathing3(E_Basic_Enemy* enemy, GameObject nodes[], E_Player* player, GameObject* prevfloor, int size, GameObject walls[]);

void SpawnEnemy(int type, CP_Vector pos);

void ResetEnemy(E_Basic_Enemy* enemy);

/*-----------------------------------------------------------------*/
// Declarations for enemy state functions (for reuses)
/*-----------------------------------------------------------------*/
void Enemy_ActiveUpdate(E_Basic_Enemy* enemy);
#endif