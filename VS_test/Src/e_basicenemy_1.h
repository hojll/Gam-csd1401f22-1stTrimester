#ifndef E_BASICENEMY_1
#define E_BASICENEMY_1

#include "entities.h"

#define MAX_PATHFINDING_NODES 6
/*-----------------------------------------------------------------*/
// E_BASICENEMY ENTITY 1
/*-----------------------------------------------------------------*/
typedef struct E_Basic_Enemy_1 E_Basic_Enemy_1;
typedef enum E_BASIC_ENEMY_STATES_1 {
	STATE_ENEMY_DEAD,
	STATE_ENEMY_ACTIVE,
	STATE_ENEMY_MOVING,
	STATE_ENEMY_JUMPING,
	STATE_ENEMY_NUM_STATES,
}E_BASIC_ENEMY_STATES_1;
typedef struct E_Basic_Enemy_1 {
	GameObject go;
	E_BASIC_ENEMY_STATES_1 state;
	CP_BOOL grounded;
	CP_BOOL tracking;
	GameObject *nodes;
	// Stats
	double HP;

	void (*Update[STATE_ENEMY_NUM_STATES])(E_BASIC_ENEMY_STATES_1*);
} E_Basic_Enemy_1;
/*-----------------------------------------------------------------*/
// Other references
/*-----------------------------------------------------------------*/

/*-----------------------------------------------------------------*/
// Functions 
/*-----------------------------------------------------------------*/
E_Basic_Enemy_1 InitializeEnemy();

void InitEnemyList(E_Basic_Enemy_1 arr[], int size, GameObject nodes[]);

void UpdateEnemyList(E_Basic_Enemy_1 arr[], int size);

void EnemytoWallCollision(E_Basic_Enemy_1 *enemy, GameObject wallreference[]);

void EnemyPathingTest(E_Basic_Enemy_1 *enemy, GameObject nodes[]);

#endif