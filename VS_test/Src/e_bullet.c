#include "e_bullet.h"
#include "messenger.h"

#define SCATTER_AMOUNT 10
#define SCATTER_LIFETIME -100.f
#define SCATTER_SPEED 300.f

void Bullet_ActiveUpdate(E_Bullet* bullet)
{
	bullet->go.pos = CP_Vector_Add(bullet->go.pos, CP_Vector_Scale(bullet->go.vel, g_scaledDt));
	bullet->lifetime -= g_scaledDt;
	// Set lifetime to below -50.f for infinite lifetime
	if (bullet->lifetime <= 0 && bullet->lifetime >= -50.f)
		bullet->go.active = 0;
	switch (bullet->bullet_type)
	{
	default:
		break;
	}
}

E_Bullet InitializeBullet()
{
	E_Bullet bullet;
	bullet.Update = Bullet_ActiveUpdate;
	bullet.Destroy = DestroyBullet;
	bullet.collide_pos = CP_Vector_Zero();
	bullet.go.active = 0;
	bullet.go.height = 20;
	bullet.go.width = 20;
	bullet.go.pos = CP_Vector_Zero();
	bullet.go.vel = CP_Vector_Zero();
	bullet.bullet_type = BULLET_DEFAULT;
	bullet.lifetime = -1;
	return bullet;
}

void DestroyBullet(E_Bullet* bullet)
{
	bullet->go.active = 0;
	SpawnBulletMessage bullet_msg;
	GameObject temp_go;
	switch (bullet->bullet_type)
	{
	case BULLET_SCATTER:
 		for (int i = 0; i < SCATTER_AMOUNT; i++)
		{
			temp_go.pos = bullet->collide_pos;
			temp_go.vel = CP_Vector_Scale(CP_Vector_Normalize(CP_Vector_Set(CP_Random_RangeFloat(-1, 1), CP_Random_RangeFloat(-1, 1))), SCATTER_SPEED);
			temp_go.width = 10.f;
			temp_go.height = 10.f;
			bullet_msg.type = BULLET_DEFAULT;
			bullet_msg.lifetime = SCATTER_LIFETIME;
			bullet_msg.go = temp_go;
			bullet_msg.color = CP_Color_Create(255, 255, 255, 255);
			g_messenger.messages[MSG_SPAWN_BULLET](&bullet_msg);
		}
		break;
	default:
		break;
	}
}
