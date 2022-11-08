#ifndef E_BASICENEMY_1
#define E_BASICENEMY_1

#include "entities.h"
#include "e_player.h"

#define MAX_PATHFINDING_NODES 6




/*-----------------------------------------------------------------*/
// E_BASICENEMY ENTITY 1
/*-----------------------------------------------------------------*/
typedef struct E_Basic_Enemy_1 E_Basic_Enemy_1;
typedef enum E_BASIC_ENEMY_STATES_1 {
	STATE_ENEMY1_DEAD,
	STATE_ENEMY1_ACTIVE,
	STATE_ENEMY1_MOVING,
	STATE_ENEMY1_JUMPING,
	STATE_ENEMY1_NUM_STATES,
}E_BASIC_ENEMY_STATES_1;
typedef struct E_Basic_Enemy_1 {
	GameObject go;
	E_BASIC_ENEMY_STATES_1 state;
	
	short grounded;
	short tracking;

	GameObject *nodes;
	GameObject *myfloor;

	GameObject e_path[MAX_PATHFINDING_NODES];
	int pathcount;
	// Stats
	double HP;
	int enemytype;
	int enemyscore;


	void (*Update[STATE_ENEMY1_NUM_STATES])(E_BASIC_ENEMY_STATES_1*);
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

void EnemyPathing(E_Basic_Enemy_1* enemy, GameObject nodes[], E_Player* player, GameObject* prevfloor, int size);

void SpawnEnemy(int type, CP_Vector pos);

void ResetEnemy(E_Basic_Enemy_1* enemy);
#endif