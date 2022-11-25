#include "e_enemyBoss_1.h"
#include "messenger.h"
#include "collision.h"
#include "easing.h"

Messenger g_messenger;
//static const float E_SPEED = 700.f;
//static const float GRAVITY = 200.f;
//static const float MAX_GRAV_VEL = 1500.f;
//static const float JUMP_VEL = -2500.f;
//static const float JUMP_RANGE = 20.f;
static const float JUMP_ANIM_SPEED = 0.1f;
static const float ACTIVE_ANIM_SPEED = 0.1f;
static const float RED_TINT_DECAY_RATE = 0.7f;
static const float BOSS_SHOOT_TIMER = 1.f;
static const float BOSS_PROJECTILE_VEL = 1500.f;
static const float BOSS_PROJECTILE_LIFE = 4.f;
enum {
	FRAME_DIM_WIDTH = 48,
	FRAME_DIM_HEIGHT = 48,
	IMAGE_DIM_WIDTH = 288,
	IMAGE_DIM_HEIGHT = 96,
	NUM_FRAMES_ACTIVE = 4,
	NUM_FRAMES_JUMP = 6
};
SpriteAnimData enemyBoss1Animations[ANIM_ENEMY_BOSS_1_NUM_ANIM];

void Boss_1_ActiveUpdate(E_Basic_Enemy* enemy) {
	Enemy_ActiveUpdate(enemy);	// Base check for velocity etc
	if (enemy->animState != ANIM_ENEMY_BOSS_1_JUMPING) {
		if (!enemy->grounded) {
			enemy->animState = ANIM_ENEMY_BOSS_1_JUMPING;
			enemy->currAnim = SetSpriteAnim(&enemy->animData[ANIM_ENEMY_BOSS_1_JUMPING], JUMP_ANIM_SPEED);
		}
	}
	else {	// So we are in the air here
		if (enemy->currAnim.state) {
			enemy->animState = ANIM_ENEMY_BOSS_1_ACTIVE_MOVING;
			enemy->currAnim = SetSpriteAnim(&enemy->animData[ANIM_ENEMY_BOSS_1_ACTIVE_MOVING], ACTIVE_ANIM_SPEED);
		}
		// We cannot shoot in the air so return
		return;
	}
	// For ranged attacks here:
	if ((enemy->bossTimer -= g_scaledDt) < 0.f) {
   		enemy->bossTimer += BOSS_SHOOT_TIMER;
		SpawnEnemyBulletMessage msg;
		msg.startPos = enemy->go.pos;
		msg.vel = BOSS_PROJECTILE_VEL;
		msg.lifetime = BOSS_PROJECTILE_LIFE;
		g_messenger.messages[MSG_SPAWN_ENEMY_BULLET](&msg);
	}
}

void Boss_1_DeadUpdate(E_Basic_Enemy* enemy) {
	int end = 2;
	enemy->floatingtimer += 1 * CP_System_GetDt();
	if (enemy->floatingtimer > end) {
		enemy->go.active = 0;
		enemy->isDying = 0;
		ResetEnemy(enemy);
	}
	else {
		enemy->go.pos.x = EaseOutExpo(enemy->go.pos.x, enemy->go.prevPos.x, enemy->floatingtimer / end);
		enemy->go.pos.y = EaseOutExpo(enemy->go.pos.y, enemy->go.prevPos.y, enemy->floatingtimer / end);
	}
}


E_Basic_Enemy* InitializeBoss_1(E_Basic_Enemy* enemy)
{
	enemy->Update[STATE_ENEMY_DEAD] = Boss_1_DeadUpdate;
	enemy->Update[STATE_ENEMY_ACTIVE] = Boss_1_ActiveUpdate;

	enemy->animData = enemyBoss1Animations;
	enemy->state = STATE_ENEMY_ACTIVE;
	enemy->currAnim = SetSpriteAnim(&enemy->animData[ANIM_ENEMY_BOSS_1_ACTIVE_MOVING], ACTIVE_ANIM_SPEED);
	enemy->HP = 0;
	enemy->go.active = 0;
	enemy->go.height = 100.f;
	enemy->go.width = 100.f;
	enemy->grounded = 0;
	enemy->go.dir.y = 0;
	enemy->go.vel.y = 0;
	enemy->tracking = 0;
	enemy->isDying = 0;
	enemy->myfloor = NULL;
	// random direction
	int randomdir = returnRange(1, 20);
	if (randomdir >= 10)
	{
		enemy->go.dir.x = 1;
	}
	else
	{
		enemy->go.dir.x = -1;
	}
	return enemy;
}

void InitAnimdata_EB1()
{
	{
		SpriteAnimData activeAnim = { 0 };
		activeAnim.frameDim[0] = FRAME_DIM_WIDTH;
		activeAnim.frameDim[1] = FRAME_DIM_HEIGHT;
		activeAnim.imageDim[0] = IMAGE_DIM_WIDTH;
		activeAnim.imageDim[1] = IMAGE_DIM_HEIGHT;

		activeAnim.numFrames = NUM_FRAMES_ACTIVE;
		activeAnim.imageStart[0] = 0;
		activeAnim.imageStart[1] = 0;

		activeAnim.loop = 1;
		enemyBoss1Animations[ANIM_ENEMY_BOSS_1_ACTIVE_MOVING] = activeAnim;
	}
	{
		SpriteAnimData jumpAnim = { 0 };
		jumpAnim.frameDim[0] = FRAME_DIM_WIDTH;
		jumpAnim.frameDim[1] = FRAME_DIM_HEIGHT;
		jumpAnim.imageDim[0] = IMAGE_DIM_WIDTH;
		jumpAnim.imageDim[1] = IMAGE_DIM_HEIGHT;

		jumpAnim.numFrames = NUM_FRAMES_JUMP;
		jumpAnim.imageStart[0] = 0;
		jumpAnim.imageStart[1] = FRAME_DIM_HEIGHT;

		jumpAnim.loop = 0;
		enemyBoss1Animations[ANIM_ENEMY_BOSS_1_JUMPING] = jumpAnim;
	}
}

E_Basic_Enemy *InitializeEnemyBoss_1(E_Basic_Enemy* enemy)
{
	enemy->Update[STATE_ENEMY_DEAD] = Boss_1_DeadUpdate;
	enemy->Update[STATE_ENEMY_ACTIVE] = Boss_1_ActiveUpdate;

	enemy->animData = enemyBoss1Animations;
	enemy->state = STATE_ENEMY_ACTIVE;
	enemy->currAnim = SetSpriteAnim(&enemy->animData[ANIM_ENEMY_BOSS_1_ACTIVE_MOVING], ACTIVE_ANIM_SPEED);
	enemy->HP = 0;
	enemy->go.active = 0;
	enemy->go.height = 125.f;
	enemy->go.width = 125.f;
	enemy->grounded = 0;
	enemy->go.dir.y = 0;
	enemy->go.vel.y = 0;
	enemy->tracking = 0;
	enemy->isDying = 0;
	enemy->bossTimer = BOSS_SHOOT_TIMER;
	enemy->myfloor = NULL;
	// random direction
	int randomdir = returnRange(1, 20);
	if (randomdir >= 10)
	{
		enemy->go.dir.x = 1;
	}
	else
	{
		enemy->go.dir.x = -1;
	}
	return enemy;
}
