#include "e_particles.h"
#include "messenger.h"
#include <math.h>

#define AIR_RESISTANCE 200
#define GRAVITY 500

#define PARTICLE_SPEED 350
#define BULLET_PARTICLE_AMOUNT 5
#define BLOOD_SPLATTER_AMOUNT 25

void Particle_ActiveUpdate(E_Particle* particle)
{
	particle->lifetime -= g_scaledDt;
	if (particle->lifetime <= 0)
		particle->go.active = 0;

	particle->go.pos = CP_Vector_Add(particle->go.pos, CP_Vector_Scale(particle->go.vel, g_scaledDt));

	if (!particle->collided)
	{
		particle->go.vel.x += AIR_RESISTANCE * g_scaledDt * -(particle->go.vel.x / abs(particle->go.vel.x));
		particle->go.vel.y += AIR_RESISTANCE * g_scaledDt * -(particle->go.vel.y / abs(particle->go.vel.y));
		particle->go.vel.y += GRAVITY * g_scaledDt;
	}
}

E_Particle InitializeParticle()
{
	E_Particle particle;
	particle.Update = Particle_ActiveUpdate;
	particle.go.active = 0;
	particle.go.height = 5;
	particle.go.width = 5;
	particle.go.pos = CP_Vector_Zero();
	particle.go.vel = CP_Vector_Zero();
	particle.lifetime = 5.f;
	particle.collided = 0;
	return particle;
}

void BulletHitParticles(E_Bullet bullet)
{
	if (bullet.bullet_type == BULLET_BLACK_HOLE)
		return;
	for (int i = 0; i < BULLET_PARTICLE_AMOUNT; i++)
	{
		SpawnParticleMessage particle_msg;
		particle_msg.go.pos = bullet.collide_pos;
		particle_msg.go.vel = CP_Vector_Scale(CP_Vector_Normalize((CP_Vector_Set(CP_Random_RangeFloat(-1.f, 1.f), CP_Random_RangeFloat(-1.f, 1.f)))), PARTICLE_SPEED);
		particle_msg.lifetime = 60.f;
		particle_msg.color = bullet.color;
		particle_msg.go.width = 5;
		particle_msg.go.height = 5;
		particle_msg.collided = 0;
		g_messenger.messages[MSG_SPAWN_PARTICLE](&particle_msg);
	}
}

void EnemyBloodSplatter(E_Basic_Enemy enemy)
{
	for (int i = 0; i < BLOOD_SPLATTER_AMOUNT; i++)
	{
		SpawnParticleMessage particle_msg;
		particle_msg.go.pos = enemy.go.pos;
		particle_msg.go.vel = CP_Vector_Scale(CP_Vector_Normalize((CP_Vector_Set(CP_Random_RangeFloat(-1.f, 1.f), CP_Random_RangeFloat(-1.f, 1.f)))), PARTICLE_SPEED);
		particle_msg.lifetime = 100.f;
		particle_msg.color = CP_Color_Create(120, 200, 200, 255); //CP_Color_Create(200, 3, 3, 255); //BLOOD COLOUR
		particle_msg.go.width = 6;
		particle_msg.go.height = 6;
		particle_msg.collided = 0;
		g_messenger.messages[MSG_SPAWN_PARTICLE](&particle_msg);
	}
}
