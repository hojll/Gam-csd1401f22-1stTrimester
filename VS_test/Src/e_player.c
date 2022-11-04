#include "e_player.h"
#include <stdio.h>
#include "messenger.h"
#include "collision.h"
#include "utilities.h"

Messenger g_messenger;
#define PLAYER_SPEED 250.f
#define ROLL_SPEED PLAYER_SPEED*3.f
#define ROLL_DURATION 0.5f
#define JUMP_VEL -1000.f
// DEFINITIONS FOR ANIMATIONS
#define FRAME_DIM_WIDTH 32
#define FRAME_DIM_HEIGHT 32
#define IMAGE_DIM_WIDTH 160
#define IMAGE_DIM_HEIGHT 32
#define DEFAULT_ATTACK_SPEED 0.3f;
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

	// ANIMATIONS
	UpdateSpriteAnim(&player->currAnim, g_scaledDt);

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
	player->attackSpeedTimer += g_scaledDt;
	if (CP_Input_KeyDown(KEY_SPACE) && player->attackSpeedTimer >= player->attackSpeed) {
		player->attackSpeedTimer = 0.f;

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
	int currAfterImage = (int)(player->go.timer / (ROLL_DURATION / PLAYER_ROLL_AFTERIMAGE));
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

void InitializePlayer(E_Player *player) {
	// Point the update functions
	player->Update[STATE_PLAYER_DEAD] = Player_DeadUpdate;
	player->Update[STATE_PLAYER_ACTIVE] = Player_ActiveUpdate;
	player->Update[STATE_PLAYER_ROLLING] = Player_RollUpdate;
	player->state = STATE_PLAYER_ACTIVE;
	player->go.dir.x = 1.f;	// A base direction
	/*------------*/
	// ANIMATIONS //
	/*------------*/
	// ACTIVE ANIMATION
	{
		SpriteAnimData activeAnim = {0};
		activeAnim.frameDim[0] = FRAME_DIM_WIDTH;
		activeAnim.frameDim[1] = FRAME_DIM_HEIGHT;
		activeAnim.imageDim[0] = IMAGE_DIM_WIDTH;
		activeAnim.imageDim[1] = IMAGE_DIM_HEIGHT;

		activeAnim.numFrames = 5;
		activeAnim.imageStart[0] = 0;
		activeAnim.imageStart[1] = 0;
		activeAnim.loop = 1;
		player->animations[ANIM_PLAYER_ACTIVE] = activeAnim;
	}
	/*
	SPRITE spriteID;
	int frameDim[2];	// X and Y dimensions of each frame
	int imageDim[2];	// Y and Y dimensions of image
	int numFrames;		// number of frames in an animation
	int imageStart[2];	// Start coord of image
	short loop;		// 1 is loop, 0 is no loop*/
	// ROLLING ANIMATION
	{
		SpriteAnimData rollingAnim = { 0 };
		rollingAnim.frameDim[0] = FRAME_DIM_WIDTH;
		rollingAnim.frameDim[1] = FRAME_DIM_HEIGHT;
		rollingAnim.numFrames = 5;
		rollingAnim.imageStart[0] = 0;
		rollingAnim.imageStart[1] = 0;
		rollingAnim.loop = 0;
		player->animations[ANIM_PLAYER_ROLLING] = rollingAnim;
	}
	player->animState = ANIM_PLAYER_ACTIVE;
	player->currAnim = SetSpriteAnim(&player->animations[ANIM_PLAYER_ACTIVE], 0.1f);
	///////////////////////////////////////////////////////////
	player->go.pos = CP_Vector_Zero();
	player->go.vel = CP_Vector_Zero();
	player->go.pos.y = 200.f;
	player->go.pos.x = 450.f;
	player->go.width = 50.f;
	player->go.height = 50.f;
	player->grounded = 0;
	player->currAmmo = 10;
	player->attackSpeed = DEFAULT_ATTACK_SPEED;
	player->attackSpeedTimer = 0.f;
	player->currBullet = BULLET_SCATTER;
}