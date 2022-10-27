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
/*--------*/
// PLAYER //
/*--------*/
// Forward declaration here to allow pointers to itself in update
typedef struct E_Player E_Player;
typedef enum PLAYER_STATES {
	STATE_PLAYER_DEAD,
	STATE_PLAYER_ACTIVE,
	STATE_PLAYER_ROLLING,
	STATE_PLAYER_NUM_STATES
}PLAYER_STATES;
typedef struct E_Player{
	GameObject go;
	// Other variables here
	PLAYER_STATES state;
	CP_BOOL grounded;
	void (*Update[STATE_PLAYER_NUM_STATES])(E_Player*);
};
// FUNCTIONS
E_Player InitializePlayer();

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
};

typedef struct E_BulletTest {
	GameObject go;
}E_BulletTest;
#pragma endregion

#endif
