#ifndef EPLAYER_H
#define EPLAYER_H
#include "entities.h"
#include "spriteData.h"
#define PLAYER_ROLL_AFTERIMAGE 10
typedef struct E_Player E_Player;
typedef enum PLAYER_STATES {
	STATE_PLAYER_DEAD,
	STATE_PLAYER_ACTIVE,
	STATE_PLAYER_ROLLING,
	STATE_PLAYER_NUM_STATES
}PLAYER_STATES;
typedef enum PLAYER_ANIMATIONS {
	ANIM_PLAYER_ACTIVE,
	ANIM_PLAYER_ROLLING,
	ANIM_PLAYER_NUM_ANIM
}PLAYER_ANIMATIONS;
typedef struct E_Player {
	GameObject go;
	SpriteAnimData animations[ANIM_PLAYER_NUM_ANIM];
	// Other variables here
	PLAYER_STATES state;
	// Animation related
	PLAYER_ANIMATIONS animState;
	SpriteAnimInstance currAnim;
	CP_BOOL grounded;
	CP_Vector rollPrevPos[PLAYER_ROLL_AFTERIMAGE];
	float rollAlpha[PLAYER_ROLL_AFTERIMAGE];
	void (*Update[STATE_PLAYER_NUM_STATES])(E_Player*);
};
// FUNCTIONS
E_Player InitializePlayer();

#endif // !EPLAYER_H