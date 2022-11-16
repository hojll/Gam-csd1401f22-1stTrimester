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
typedef enum ENEMY_1_ANIMATIONS {
	ANIM_ENEMY_1_ACTIVE_MOVING,
	ANIM_ENEMY_1_NUM_ANIM
}ENEMY_1_ANIMATIONS;

typedef enum ENEMY_TYPE {
	ENEMY_TYPE_1,
	ENEMY_TYPE_2,
	ENEMY_TYPE_3,
	NUM_ENEMY_TYPE
}ENEMY_TYPE;
typedef struct E_Basic_Enemy {
	// TODO: MIGRATE ENEMY DETERMINED STATS HERE
	// THAT MEANS CONSTANT VALUES IN .c SHOULD BE REPRESENTED HERE
	GameObject go;
	int state;
	int type;

	short grounded;
	short tracking;

	GameObject *nodes;
	GameObject *myfloor;
	GameObject* debugshortestnode;

	// Animations
	SpriteAnimData* animData;
	int animState;
	SpriteAnimInstance currAnim;
	
	// Stats
	double HP;
	int enemytype;
	int enemyscore;
	float floatingtimer;


	void (*Update[STATE_ENEMY_NUM_STATES])(E_BASIC_ENEMY_STATES_1*);
} E_Basic_Enemy;
/*-----------------------------------------------------------------*/
// Other references
/*-----------------------------------------------------------------*/

/*-----------------------------------------------------------------*/
// Functions 
/*-----------------------------------------------------------------*/
void InitAnimdata_E1();

E_Basic_Enemy InitializeEnemy_1();

void InitEnemyList(E_Basic_Enemy arr[], int size, GameObject nodes[]);

void UpdateEnemyList(E_Basic_Enemy arr[], int size);

void EnemytoWallCollision(E_Basic_Enemy *enemy, GameObject wallreference[]);

void EnemyPathing(E_Basic_Enemy* enemy, GameObject nodes[], E_Player* player, GameObject* prevfloor, int size);

void EnemyPathing3(E_Basic_Enemy* enemy, GameObject nodes[], E_Player* player, GameObject* prevfloor, int size, GameObject walls[], GameObject nodes2[]);

void SpawnEnemy(int type, CP_Vector pos);

void ResetEnemy(E_Basic_Enemy* enemy);

/*-----------------------------------------------------------------*/
// Declarations for enemy state functions (for reuses)
/*-----------------------------------------------------------------*/
void Enemy_ActiveUpdate(E_Basic_Enemy* enemy);
#endif