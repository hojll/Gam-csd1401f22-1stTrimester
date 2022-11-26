#include "e_bullet.h"
#include "messenger.h"
#include "utilities.h"

#define BULLET_SPEED 1200.f

#define SCATTER_AMOUNT 10
#define SCATTER_LIFETIME -100.f
#define SCATTER_SPEED 800.f

#define SHOTGUN_LIFETIME 0.4f
#define SHOTGUN_SPEED CP_Random_RangeFloat(700.f, 1000.f)
#define SHOTGUN_Y_DIR CP_Random_RangeFloat(-0.3f, 0.3f)

#define BLACK_HOLE_MAX_TIME 1.0f
#define BLACK_HOLE_LIFETIME 1.3f
#define BLACK_HOLE_MAX_SIZE 500.f
#define BLACK_HOLE_GROWTH_TIME ((BLACK_HOLE_LIFETIME - BLACK_HOLE_MAX_TIME) / 2.f)
#define BLACK_HOLE_GROWTH_RATE BLACK_HOLE_MAX_SIZE / BLACK_HOLE_GROWTH_TIME

extern CP_Sound SoundArray[50];

void Bullet_ActiveUpdate(E_Bullet* bullet)
{
	bullet->go.pos = CP_Vector_Add(bullet->go.pos, CP_Vector_Scale(bullet->go.vel, g_scaledDt));
	bullet->lifetime -= g_scaledDt;
	// Set lifetime to below -50.f for infinite lifetime
	if (bullet->lifetime <= 0 && bullet->lifetime >= -50.f)
		DestroyBullet(bullet);

	switch (bullet->bullet_type)
	{
	case BULLET_BLACK_HOLE:
		if(!CP_Vector_Equal(bullet->collide_pos, CP_Vector_Zero()) &&
			bullet->lifetime > BLACK_HOLE_GROWTH_TIME + BLACK_HOLE_MAX_TIME)
		{
			bullet->go.width += BLACK_HOLE_GROWTH_RATE * g_scaledDt;
			bullet->go.height = bullet->go.width;
		}
		else if (!CP_Vector_Equal(bullet->collide_pos, CP_Vector_Zero()) &&
			bullet->lifetime < BLACK_HOLE_GROWTH_TIME)
		{
			bullet->go.width -= BLACK_HOLE_GROWTH_RATE * g_scaledDt;
			bullet->go.height = bullet->go.width;
		}
		break;
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
	CP_Vector tempScatterPos = bullet->collide_pos;
	switch (bullet->bullet_type)
	{
	case BULLET_SCATTER:
 		for (int i = 0; i < SCATTER_AMOUNT; i++)
		{
			temp_go.pos = tempScatterPos;
			temp_go.faceDir = bullet->go.faceDir;
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
	case BULLET_SHOTGUN:
		for (unsigned int i = 0; i < CP_Random_RangeInt(7, 10); i++)
		{
			temp_go.pos = bullet->go.pos;
			temp_go.faceDir = bullet->go.faceDir;
			temp_go.vel = CP_Vector_Scale(CP_Vector_Normalize(CP_Vector_Set(bullet->go.faceDir, SHOTGUN_Y_DIR)), SHOTGUN_SPEED);
			temp_go.width = CP_Random_RangeFloat(10.f, 20.f);
			temp_go.height = temp_go.width;
			bullet_msg.type = BULLET_DEFAULT;
			bullet_msg.lifetime = SHOTGUN_LIFETIME;
			bullet_msg.go = temp_go;
			bullet_msg.color = CP_Color_Create(255, 255, 255, 255);
			g_messenger.messages[MSG_SPAWN_BULLET](&bullet_msg);
		}
		break;
	case BULLET_BLACK_HOLE:
		if (bullet->lifetime <= 0.f && bullet->lifetime >= -50.f)
			break;

		bullet->go.active = 1;
		bullet->go.vel = CP_Vector_Zero();

		if (bullet->lifetime <= -50.f)
			bullet->lifetime = BLACK_HOLE_LIFETIME;
		
		break;
	default:
		break;
	}
}

void CreateBullet(CP_Vector position, int facedir, BULLET_TYPE type)
{
	SpawnBulletMessage bullet;
	GameObject go;

	go.pos = position;
	go.vel.x = BULLET_SPEED * facedir;
	go.faceDir = facedir;
	go.vel.y = 0.f;
	go.width = 20.f;
	go.height = 20.f;
	bullet.lifetime = -100.f;
	bullet.color = CP_Color_Create(0, 100, 0, 255);
	switch (type)
	{
	case BULLET_SCATTER:
		bullet.color = CP_Color_Create(200, 200, 200, 255);
		CP_Sound_Play(SoundArray[3]);
		break;
	case BULLET_SMG:
		go.width = 10.f;
		go.height = 10.f;
		go.vel.x = BULLET_SPEED * facedir * 1.5f;
		CP_Sound_Play(SoundArray[4]);
		break;
	case BULLET_SHOTGUN:
		bullet.lifetime = 0.0f;
		CP_Sound_Play(SoundArray[5]);
		break;
	case BULLET_DUAL:
		CreateBullet(position, -facedir, BULLET_DEFAULT, 1);
		CP_Sound_Play(SoundArray[6]);
		break;
	case BULLET_BLACK_HOLE:
		go.vel.x = BULLET_SPEED * facedir / 1.5f;
		bullet.color = CP_Color_Create(0, 0, 0, 255);
		CP_Sound_Play(SoundArray[7]);
		break;
	default:
		CP_Sound_Play(SoundArray[2]);
		break;
	}
	
	bullet.go = go;
	bullet.type = type;
	g_messenger.messages[MSG_SPAWN_BULLET](&bullet);
}
