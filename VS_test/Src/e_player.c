#include "entities.h"
#include <stdio.h>
#include "messenger.h"
Messenger g_messenger;
#define PLAYER_SPEED 100.f
#define ROLL_SPEED PLAYER_SPEED*2.f
////////////////////////////////////////////////////////////////////////
/*--------*/
// PLAYER //
/*--------*/
// The functions for player
void Player_ActiveUpdate(E_Player* player) {
	// INPUT LOGIC HERE
	// If 2 players, maybe add var, playerNumber into struct?
	// Placeholder logic
	CP_Vector movementAxis = CP_Vector_Zero();
	int newLookDir = -(int)CP_Input_KeyDown(KEY_A) + (int)CP_Input_KeyDown(KEY_D);
	player->go.faceDir = newLookDir | player->go.faceDir;
	movementAxis.x += (float)newLookDir;
	player->go.vel = CP_Vector_Scale(movementAxis, PLAYER_SPEED * CP_System_GetDt());
	// Shooting
	if (CP_Input_KeyTriggered(KEY_SPACE)) {
		SpawnBulletMessage bullet;
		bullet.position = player->go.pos;
		bullet.vel.x = PLAYER_SPEED * 1.5f;
		bullet.vel.y = 0.f;

		g_messenger.messages[MSG_SPAWN_BULLET](&bullet);
	}
	// Rolling
	if (CP_Input_KeyTriggered(KEY_LEFT_SHIFT)) {
		player->state = STATE_PLAYER_ROLLING;
	}
}

/// <summary>
/// Update loop to make player roll. Should not take in inputs
/// </summary>
/// <param name="player">pointer to player</param>
void Player_RollUpdate(E_Player* player) {
	// Can be changed any time. Const values until then
	
}

void Player_DeadUpdate(E_Player* player) {
	// Play the game end state here?

}

E_Player InitializePlayer() {
	E_Player retVal;
	// Point the update functions
	retVal.Update[STATE_PLAYER_DEAD] = Player_DeadUpdate;
	retVal.Update[STATE_PLAYER_ACTIVE] = Player_ActiveUpdate;
	retVal.state = STATE_PLAYER_ACTIVE;
	retVal.active = 0;
	retVal.go.dir.x = 1.f;	// A base direction
	return retVal;
}