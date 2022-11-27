//All content © 2021 DigiPen (SINGAPORE) Corporation, all rights reserved.
//Alvin Yeo	    a.yeo@digipen.edu


#ifndef EPARTICLES_H
#define EPARTICLES_H
#include "entities.h"
#include "e_bullet.h"
#include "e_basicenemy_1.h"

typedef struct E_Particle E_Particle;
typedef struct E_Particle {
	GameObject go;
	float lifetime;
	CP_Color color;
	CP_BOOL collided;
	void (*Update)(E_Particle*);
}E_Particle;

E_Particle InitializeParticle();
void BulletHitParticles(E_Bullet bullet);
void EnemyBloodSplatter(E_Basic_Enemy enemy);

#endif
