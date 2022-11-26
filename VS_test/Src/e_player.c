#include "e_player.h"
#include <stdio.h>
#include "messenger.h"
#include "collision.h"
#include "utilities.h"

Messenger g_messenger;
#define PLAYER_SPEED 750.f
#define ROLL_SPEED PLAYER_SPEED*2.f
#define ROLL_DURATION 0.5f
#define GRAVITY 7000.f
#define MAX_GRAV_VEL 3500.f
#define JUMP_VEL -2000.f
#define DEFAULT_ATTACK_SPEED 0.3f
// DEFINITIONS FOR ANIMATIONS
#define FRAME_DIM_WIDTH 37
#define FRAME_DIM_HEIGHT 37
#define IMAGE_DIM_WIDTH 192
#define IMAGE_DIM_HEIGHT 144
#define NUM_ROLL_FRAMES 5
#define ACTIVE_ANIM_SPEED 0.1f

extern CP_Sound SoundArray[50];

////////////////////////////////////////////////////////////////////////
/*--------*/
// PLAYER //
/*--------*/
// The functions for player

// Player Gun Inits
void PlayerWeapon_Init(E_Player* player, BULLET_TYPE bulletType)
{
	player->currBullet = bulletType;
	switch (bulletType)
	{
	case BULLET_DEFAULT:
		player->attackSpeed = DEFAULT_ATTACK_SPEED;
		player->currAmmo = -1;
		break;
	case BULLET_SCATTER:
		player->attackSpeed = DEFAULT_ATTACK_SPEED;
		player->currAmmo = 5;
		break;
	case BULLET_SMG:
		player->attackSpeed = DEFAULT_ATTACK_SPEED / 5.f;
		player->currAmmo = 25;
		break;
	case BULLET_SHOTGUN:
		player->attackSpeed = DEFAULT_ATTACK_SPEED * 1.5f;
		player->currAmmo = 5;
		break;
	case BULLET_DUAL:
		player->attackSpeed = DEFAULT_ATTACK_SPEED;
		player->currAmmo = 15;
		break;
	case BULLET_BLACK_HOLE:
		player->attackSpeed = DEFAULT_ATTACK_SPEED * 3.f;
		player->currAmmo = 3;
		break;
	}
	player->maxAmmo = player->currAmmo;
}

void Player_ActiveUpdate(E_Player* player) {
	// INPUT LOGIC HERE
	// If 2 players, maybe add var, playerNumber into struct?
	// Placeholder logic

	// Player Movement
	int newLookDir = -((int)CP_Input_KeyDown(KEY_A) || (int)CP_Input_KeyDown(KEY_LEFT))
		+ ((int)CP_Input_KeyDown(KEY_D) || (int)CP_Input_KeyDown(KEY_RIGHT));
	if (newLookDir) {
		player->go.faceDir = newLookDir;
		if (player->animState != ANIM_PLAYER_ACTIVE_MOVING) {
			player->animState = ANIM_PLAYER_ACTIVE_MOVING;
			player->currAnim = SetSpriteAnim(&player->animations[ANIM_PLAYER_ACTIVE_MOVING], ACTIVE_ANIM_SPEED);// Change animation too
		}
	}
	else {	// NOT MOVING, CHANGE ANIMATION IF NEED TO
		if (player->animState != ANIM_PLAYER_ACTIVE_IDLE) {
			player->animState = ANIM_PLAYER_ACTIVE_IDLE;
			player->currAnim = SetSpriteAnim(&player->animations[ANIM_PLAYER_ACTIVE_IDLE], ACTIVE_ANIM_SPEED);// Change animation too
		}
	}
	player->go.vel.x = (float)newLookDir * PLAYER_SPEED;

	// ANIMATIONS
	player->currAnim.flip = player->go.faceDir < 0;
	UpdateSpriteAnim(&player->currAnim, g_scaledDt);

	// Gravity
	if (!player->grounded && player->go.vel.y < MAX_GRAV_VEL)
		player->go.vel.y += GRAVITY * CP_System_GetDt(); //changed from scaled dt -> normal Dt, prevents player from clipping tru floor

	// Jump
	if (player->grounded && (CP_Input_KeyDown(KEY_W) || CP_Input_KeyDown(KEY_UP)))
	{
		player->go.vel.y = JUMP_VEL * ((int)CP_Input_KeyDown(KEY_W) || (int)CP_Input_KeyDown(KEY_UP));
		player->grounded = 0;
		CP_Sound_Play(SoundArray[1]);
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
			PlayerWeapon_Init(player, BULLET_DEFAULT);
		else
			player->currAmmo--;

		CreateBullet(player->go.pos, player->go.faceDir, player->currBullet, 1);
	}
	// Rolling
	if (CP_Input_KeyTriggered(KEY_LEFT_SHIFT)) {
		player->state = STATE_PLAYER_ROLLING;
		player->animState = ANIM_PLAYER_ROLLING;
		player->currAnim = SetSpriteAnim(&player->animations[ANIM_PLAYER_ROLLING], ROLL_DURATION / NUM_ROLL_FRAMES);// Change animation too
		player->currAnim.flip = player->go.faceDir == -1;
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
	UpdateSpriteAnim(&player->currAnim, g_scaledDt);
	player->go.timer += g_scaledDt;
	if (player->currAnim.state) {

		player->state = STATE_PLAYER_ACTIVE;
		player->animState = ANIM_PLAYER_ACTIVE_IDLE;
		player->currAnim = SetSpriteAnim(&player->animations[ANIM_PLAYER_ACTIVE_IDLE], ACTIVE_ANIM_SPEED);// Change animation too
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

		activeAnim.numFrames = 2;
		activeAnim.imageStart[0] = 0;
		activeAnim.imageStart[1] = 0;
		
		activeAnim.loop = 1;
		player->animations[ANIM_PLAYER_ACTIVE_IDLE] = activeAnim;
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
		rollingAnim.imageDim[0] = IMAGE_DIM_WIDTH;
		rollingAnim.imageDim[1] = IMAGE_DIM_HEIGHT;
		rollingAnim.frameDim[0] = FRAME_DIM_WIDTH;
		rollingAnim.frameDim[1] = FRAME_DIM_HEIGHT;
		rollingAnim.numFrames = 3;
		rollingAnim.imageStart[0] = 0;
		rollingAnim.imageStart[1] = FRAME_DIM_HEIGHT * 2 + 1;	// +1 to exclude 1 row of pixels in anim above
		rollingAnim.loop = 0;
		player->animations[ANIM_PLAYER_ROLLING] = rollingAnim;
	}

	{
		SpriteAnimData runningAnim = { 0 };
		runningAnim.imageDim[0] = IMAGE_DIM_WIDTH;
		runningAnim.imageDim[1] = IMAGE_DIM_HEIGHT;
		runningAnim.frameDim[0] = FRAME_DIM_WIDTH;
		runningAnim.frameDim[1] = FRAME_DIM_HEIGHT;
		runningAnim.numFrames = 4;
		runningAnim.imageStart[0] = 0;
		runningAnim.imageStart[1] = FRAME_DIM_HEIGHT + 1;
		runningAnim.loop = 1;
		player->animations[ANIM_PLAYER_ACTIVE_MOVING] = runningAnim;
	}
	player->animState = ANIM_PLAYER_ACTIVE_IDLE;
	player->currAnim = SetSpriteAnim(&player->animations[ANIM_PLAYER_ACTIVE_IDLE], ACTIVE_ANIM_SPEED);
	///////////////////////////////////////////////////////////
	player->go.pos = CP_Vector_Zero();
	player->go.vel = CP_Vector_Zero();
	player->go.pos.y = 460.f;
	player->go.pos.x = 960.f;
	player->go.width = 75.f;
	player->go.height = 75.f;
	player->grounded = 0;
	player->currAmmo = -1;
	player->maxAmmo = -1;
	player->attackSpeed = DEFAULT_ATTACK_SPEED;
	player->attackSpeedTimer = 0.f;
	player->currBullet = BULLET_DEFAULT;
}

char* Player_RandomWeapon(E_Player* player)
{
	PlayerWeapon_Init(player, (BULLET_TYPE)CP_Random_RangeInt(1, BULLET_TOTAL - 1));
	switch (player->currBullet)
	{
	case BULLET_SCATTER:
		return "SCATTER SHOT";
	case BULLET_SMG:
		return "SMG";
	case BULLET_SHOTGUN:
		return "SHOTGUN";
	case BULLET_DUAL:
		return "DUAL BERETTAS";
	case BULLET_BLACK_HOLE:
		return "BLACK HOLE";
	}
	return "";
}