#ifndef ENTITIES_H
#define ENTITIES_H
#include "cprocessing.h"
// Array of pointers: int (*foo_ptr_array[2])( int )
// Single function pointer: int (*foo_ptr)( int )
#pragma region STRUCTS
//typedef struct Sprite {
//	int currFrame;
//	int maxFrame;
//	float elapsedTime;
//};


typedef enum LOOK_DIR {
	LEFT = -1,
	NONE = 0,
	RIGHT = 1
}LOOK_DIR;
typedef struct GameObject {
	CP_Vector pos;
	CP_Vector prevPos; // Collision
	CP_Vector vel;
	CP_Vector dir;	// Remove if not using next week
	int faceDir;
	int active;
	float timer; // Anything you want
	float width; // Collision
	float height; // Collision
}GameObject;
float g_scaledDt; // global variable dt

/*--------------------------------------------------------------------*/
// ALL THE ENTITY STRUCTS COME HERE

////////////////////////////////////////////////////////////////////////
/*-------*/
// ENEMY //
/*-------*/
// CAN BE used for more than 1 type of enemy
// Forward declaration here to allow pointers to itself in update
typedef struct E_EnemyTest E_EnemyTest;
typedef enum ENEMY_TEST_STATES {
	ENEMY_TEST_DEAD,
	ENEMY_TEST_ACTIVE,
	ENEMY_TEST_ATTACK
}ENEMY_TEST_STATES;
typedef struct E_EnemyTest{
	GameObject go;
	void (*Update[2])(E_EnemyTest*);
} E_EnemyTest;
#pragma endregion

#endif
