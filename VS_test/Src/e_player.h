#ifndef EPLAYER_H
#define EPLAYER_H
#include "entities.h"
#include "e_bullet.h"

#define PLAYER_ROLL_AFTERIMAGE 10
typedef struct E_Player E_Player;
typedef enum PLAYER_STATES {
	STATE_PLAYER_DEAD,
	STATE_PLAYER_ACTIVE,
	STATE_PLAYER_ROLLING,
	STATE_PLAYER_NUM_STATES
}PLAYER_STATES;
typedef struct E_Player {
	GameObject go;
	// Other variables here
	PLAYER_STATES state;
	CP_BOOL grounded;
	CP_Vector rollPrevPos[PLAYER_ROLL_AFTERIMAGE];
	float rollAlpha[PLAYER_ROLL_AFTERIMAGE];
	BULLET_TYPE currBullet;
	int currAmmo;
	void (*Update[STATE_PLAYER_NUM_STATES])(E_Player*);
};
// FUNCTIONS
E_Player InitializePlayer();

#endif // !EPLAYER_H