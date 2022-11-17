#ifndef MESSENGER_H
#define MESSENGER_H
#include "cprocessing.h"
#include "e_bullet.h"
typedef enum MessageType {
	MSG_SPAWN_BULLET,
	MSG_SPAWN_ENEMY1,
	MSG_SPAWN_ENEMY2,
	MSG_GAME_OVER,
	MSG_NUM_MESSAGE_TYPE
}MessageType;

typedef struct SpawnBulletMessage {
	GameObject go;
	BULLET_TYPE type;
	CP_Color color;
	float lifetime;
}SpawnBulletMessage;

/// <summary>
/// Spawn an enemy of specific type
/// </summary>
typedef struct SpawnEnemyMessage {
	CP_Vector position;
	CP_BOOL tracking;
	int type;
	int score;
	
}SpawnEnemyMessage;

/// <summary>
/// Message to query which direction player is
/// Will give 0 if player's y axis different
/// </summary>
typedef struct ToPlayerDirMessage {
	// Input value:
	CP_Vector entityPos;
	// Output value:
	int direction;
} ToPlayerDirMessage;

/// <summary>
/// Shoots enemy bullet from position in 1 direction x axis
/// </summary>
typedef struct SpawnEnemyBulletMessage {
	CP_Vector startPos;
	float vel;
}SpawnEnemyBulletMessage;

typedef struct Messenger {
	void (*messages[MSG_NUM_MESSAGE_TYPE])(void*);
}Messenger;

extern Messenger g_messenger;
#endif