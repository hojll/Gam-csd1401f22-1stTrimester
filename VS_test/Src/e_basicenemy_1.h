#ifndef E_BASICENEMY_1
#define E_BASICENEMY_1
#include "entities.h"
/*-----------------------------------------------------------------*/
// E_BASICENEMY ENTITY 1
/*-----------------------------------------------------------------*/
typedef struct E_BASIC_ENEMY_1 E_BASIC_ENEMY_1;
typedef enum E_BASIC_ENEMY_STATES_1 {
	STATE_ENEMY_DEAD,
	STATE_ENEMY_ACTIVE,
	STATE_ENEMY_MOVING,
	STATE_ENEMY_JUMPING,
	STATE_ENEMY_NUM_STATES,
}E_BASIC_ENEMY_STATES_1;
typedef struct E_BASIC_ENEMY_1 {
	GameObject go;
	E_BASIC_ENEMY_STATES_1 state;
	CP_BOOL grounded;
	void (*Update[STATE_ENEMY_NUM_STATES])(E_BASIC_ENEMY_STATES_1*);
};

E_BASIC_ENEMY_1 InitializeEnemy();



#endif