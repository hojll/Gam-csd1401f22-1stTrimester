#include "entities.h"
#include <stdio.h>
#include "messenger.h"
Messenger g_messenger;
////////////////////////////////////////////////////////////////////////
/*--------*/
// PLAYER //
/*--------*/
// The functions for player
void Player_ActiveUpdate(E_Player* player) {
	// INPUT LOGIC HERE
	// If 2 players, maybe add var, playerNumber into struct?
	// Placeholder logic
	const float SPEED = 100.f;
	CP_Vector movementAxis = CP_Vector_Zero();
	movementAxis.x += -((float)CP_Input_KeyDown(KEY_A)) + ((float)CP_Input_KeyDown(KEY_D));
	player->go.vel = CP_Vector_Scale(movementAxis, SPEED * CP_System_GetDt());
	//player->go.dir;
	// Shooting
	if (CP_Input_KeyTriggered(KEY_SPACE)) {
		SpawnBulletMessage bullet;
		bullet.position = player->go.pos;
		bullet.vel.x = SPEED * 1.5f;
		bullet.vel.y = 0.f;

		g_messenger.messages[MSG_SPAWN_BULLET](&bullet);
	}
}

void Player_DeadUpdate(E_Player* player) {
	// Play the game end state here?

}

E_Player InitializePlayer() {
	E_Player retVal;
	// Point the update functions
	retVal.Update[PLAYER_DEAD] = Player_DeadUpdate;
	retVal.Update[PLAYER_ACTIVE] = Player_ActiveUpdate;
	retVal.state = PLAYER_ACTIVE;
	retVal.active = 0;
	return retVal;
}
