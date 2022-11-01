#include "e_player.h"
#include <stdio.h>
#include "messenger.h"
#include "collision.h"
#include "utilities.h"

Messenger g_messenger;
#define PLAYER_SPEED 250.f
#define ROLL_SPEED PLAYER_SPEED*3.f
#define ROLL_DURATION 0.5f
#define GRAVITY 60.f
#define MAX_GRAV_VEL 500.f
#define JUMP_VEL -1000.f
////////////////////////////////////////////////////////////////////////
/*--------*/
// PLAYER //
/*--------*/
// The functions for player
void Player_ActiveUpdate(E_Player* player) {
	// INPUT LOGIC HERE
	// If 2 players, maybe add var, playerNumber into struct?
	// Placeholder logic

	// Player Movement
	int newLookDir = -((int)CP_Input_KeyDown(KEY_A) || (int)CP_Input_KeyDown(KEY_LEFT))
		+ ((int)CP_Input_KeyDown(KEY_D) || (int)CP_Input_KeyDown(KEY_RIGHT));
	if (newLookDir)
		player->go.faceDir = newLookDir;
	player->go.vel.x = (float)newLookDir * PLAYER_SPEED;

	// Gravity
	if (!player->grounded && player->go.vel.y < MAX_GRAV_VEL)
		player->go.vel.y += GRAVITY;

	// Jump
	if (player->grounded && (CP_Input_KeyDown(KEY_W) || CP_Input_KeyDown(KEY_UP)))
	{
		player->go.vel.y = JUMP_VEL * ((int)CP_Input_KeyDown(KEY_W) || (int)CP_Input_KeyDown(KEY_UP));
		player->grounded = 0;
	}

	//printf("x_vel:%.2f, y_vel:%.2f\n", player->go.vel.x, player->go.vel.y);

	// Update Pos
	player->go.prevPos = player->go.pos;
	player->go.pos = CP_Vector_Add(player->go.pos, CP_Vector_Scale(player->go.vel, g_scaledDt));


	// Shooting
	if (CP_Input_KeyTriggered(KEY_SPACE)) {
		if (player->currAmmo <= 0)
			player->currBullet = BULLET_DEFAULT;
		else
			player->currAmmo--;

		SpawnBulletMessage bullet;
		GameObject go;
		go.pos = player->go.pos;
		go.vel.x = PLAYER_SPEED * 1.5f * player->go.faceDir;
		go.vel.y = 0.f;
		go.width = 20.f;
		go.height = 20.f;
		bullet.go = go;
		bullet.type = player->currBullet;
		bullet.lifetime = -100.f;
		switch (player->currBullet)
		{
		case BULLET_SCATTER:
			bullet.color = CP_Color_Create(200, 200, 200, 255);
			break;
		default:
			bullet.color = CP_Color_Create(0, 100, 0, 255);
		}
		g_messenger.messages[MSG_SPAWN_BULLET](&bullet);
	}
	// Rolling
	if (CP_Input_KeyTriggered(KEY_LEFT_SHIFT)) {
		player->state = STATE_PLAYER_ROLLING;
		player->go.timer = 0.f;
		player->go.vel.y = 0.f;
		for (int i = 0; i < PLAYER_ROLL_AFTERIMAGE; i++)
			player->rollPrevPos[i] = CP_Vector_Zero();
	}
}

/// <summary>
/// Update loop to make player roll. Should not take in inputs
/// </summary>
/// <param name="player">pointer to player</param>
void Player_RollUpdate(E_Player* player) {
	// Can be changed any time. Const values until then
	player->go.timer += g_scaledDt;
	if (player->go.timer > ROLL_DURATION) {
		player->state = STATE_PLAYER_ACTIVE;
		return;
	}

	// If it's not over then update velocity
	player->go.vel.x = (float)player->go.faceDir * ROLL_SPEED;
	player->go.pos = CP_Vector_Add(player->go.pos, CP_Vector_Scale(player->go.vel, g_scaledDt));

	// Particle Effect (Trail)
	int currAfterImage = player->go.timer / (ROLL_DURATION / (float)PLAYER_ROLL_AFTERIMAGE);
	if (CP_Vector_Equal(player->rollPrevPos[currAfterImage], CP_Vector_Zero()))
	{
		player->rollPrevPos[currAfterImage] = player->go.pos;
		player->rollAlpha[currAfterImage] = 255;
	}
	for (int i = 0; i < PLAYER_ROLL_AFTERIMAGE; i++)
	{
		player->rollAlpha[i] -= 1500 * g_scaledDt;
	}

}

void Player_DeadUpdate(E_Player* player) {
	// Play the game end state here?

}

E_Player InitializePlayer() {
	E_Player retVal;
	// Point the update functions
	retVal.Update[STATE_PLAYER_DEAD] = Player_DeadUpdate;
	retVal.Update[STATE_PLAYER_ACTIVE] = Player_ActiveUpdate;
	retVal.Update[STATE_PLAYER_ROLLING] = Player_RollUpdate;
	retVal.state = STATE_PLAYER_ACTIVE;
	retVal.go.dir.x = 1.f;	// A base direction
	retVal.go.pos = CP_Vector_Zero();
	retVal.go.vel = CP_Vector_Zero();
	retVal.go.pos.y = 200.f;
	retVal.go.pos.x = 450.f;
	retVal.go.width = 50.f;
	retVal.go.height = 50.f;
	retVal.grounded = 0;
	retVal.currAmmo = 10;
	retVal.currBullet = BULLET_SCATTER;

	return retVal;
}
