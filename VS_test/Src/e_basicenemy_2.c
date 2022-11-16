#include "e_basicenemy_2.h"
#include "messenger.h"
#include "collision.h"

Messenger g_messenger;
//static const float E_SPEED = 700.f;
//static const float GRAVITY = 200.f;
//static const float MAX_GRAV_VEL = 1500.f;
//static const float JUMP_VEL = -2500.f;
//static const float JUMP_RANGE = 20.f;


void Enemy_2_ActiveUpdate(E_Basic_Enemy* enemy) {
	Enemy_ActiveUpdate(enemy);	// Base check for velocity etc
	// For ranged attacks here:
	g_messenger.messages[1];
}

void Enemy_2_DeadUpdate() {

}

E_Basic_Enemy*InitializeEnemy_2(E_Basic_Enemy *enemy)
{
	enemy->Update[STATE_ENEMY_DEAD] = Enemy_2_DeadUpdate;
	enemy->Update[STATE_ENEMY_ACTIVE] = Enemy_2_ActiveUpdate;

	enemy->state = STATE_ENEMY_ACTIVE;
	enemy->HP = 0;
	enemy->go.active = 0;
	enemy->go.height = 50.f;
	enemy->go.width = 50.f;
	enemy->grounded = 0;
	enemy->go.dir.y = 0;
	enemy->go.vel.y = 0;
	enemy->tracking = 0;
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
