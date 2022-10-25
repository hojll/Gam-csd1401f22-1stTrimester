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
typedef struct GameObject {
	CP_Vector pos;
	CP_Vector vel;
	CP_Vector dir;
	int active;
}GameObject;


/*--------------------------------------------------------------------*/
// ALL THE ENTITY STRUCTS COME HERE

////////////////////////////////////////////////////////////////////////
/*--------*/
// PLAYER //
/*--------*/
typedef struct E_Player E_Player;
typedef enum PLAYER_STATES {
	PLAYER_DEAD,
	PLAYER_ACTIVE,
	PLAYER_NUM_STATES
}PLAYER_STATES;
typedef struct E_Player{
	GameObject go;
	// Other variables here
	int active;
	PLAYER_STATES state;
	void (*Update[PLAYER_NUM_STATES])(E_Player*);
};
// FUNCTIONS
E_Player InitializePlayer();

////////////////////////////////////////////////////////////////////////
/*-------*/
// ENEMY //
/*-------*/
// CAN BE used for more than 1 type of enemy
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
