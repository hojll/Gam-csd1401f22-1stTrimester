#ifndef EBULLET_H
#define EBULLET_H
#include "entities.h"

typedef struct E_Bullet E_Bullet;
typedef enum BULLET_TYPE
{
	BULLET_DEFAULT,
	BULLET_SCATTER,
	BULLET_SMG,
	BULLET_SHOTGUN,
	BULLET_TOTAL
}BULLET_TYPE;
typedef struct E_Bullet {
	GameObject go;
	// Other variables here
	BULLET_TYPE bullet_type;
	// Set lifetime to below -50.f for infinite lifetime
	float lifetime;
	CP_Vector collide_pos;
	CP_Color color;
	void (*Update)(E_Bullet*);
	void (*Destroy)(E_Bullet*);
}E_Bullet;
E_Bullet InitializeBullet();
void DestroyBullet(E_Bullet* bullet);
void CreateBullet(CP_Vector position, int facedir, BULLET_TYPE type);
#endif
