//All content © 2021 DigiPen (SINGAPORE) Corporation, all rights reserved.

#ifndef E_BASICENEMY_1
#define E_BASICENEMY_1

#include "entities.h"
#include "e_player.h"

#define MAX_PATHFINDING_NODES 8
enum {
	HEALTH_ENEMY_1 = 2,
	HEALTH_ENEMY_2 = 1,
	HEALTH_ENEMY_3 = 1,
	HEALTH_BOSS_1 = 4
};

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
	ENEMY_BOSS_1,
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
	GameObject* enemy_prevNode;
	GameObject *enemy_shortestNode;

	// Animations
	SpriteAnimData *animData;
	int animState;
	SpriteAnimInstance currAnim;
	float redTintVal; // goes from 0 to 255
	// Stats
	int HP;
	int enemytype;
	int enemyscore;
	float hitstun;
	float floatingtimer;
	float bossTimer;	// For the boss
	int isDying;

	void (*Update[STATE_ENEMY_NUM_STATES])(E_Basic_Enemy*);
} E_Basic_Enemy;
/*-----------------------------------------------------------------*/
// Other references
/*-----------------------------------------------------------------*/

/*-----------------------------------------------------------------*/
// Functions 
/*-----------------------------------------------------------------*/
void InitAnimdata_E1();

E_Basic_Enemy* InitializeEnemy_1(E_Basic_Enemy* enemy);

void InitEnemyList(E_Basic_Enemy arr[], int size, GameObject nodes[]);

void UpdateEnemyList(E_Basic_Enemy arr[], int size);

void EnemytoWallCollision(E_Basic_Enemy *enemy, GameObject wallreference[]);



void EnemyPathing4(E_Basic_Enemy* enemy, E_Player* player, GameObject* p_prevfloor, int size,
	GameObject walls[], GameObject nodes[], GameObject nodes2[]);

void SpawnEnemy(int type, CP_Vector pos);

void ResetEnemy(E_Basic_Enemy* enemy);

// Enemy takes input damage. Returns go.active (0 for dead)
int EnemyTakeDamage(E_Basic_Enemy* enemy, int dmg, GameObject *bullet);
/*-----------------------------------------------------------------*/
// Declarations for enemy state functions (for reuses)
/*-----------------------------------------------------------------*/
void Enemy_ActiveUpdate(E_Basic_Enemy* enemy);
#endif