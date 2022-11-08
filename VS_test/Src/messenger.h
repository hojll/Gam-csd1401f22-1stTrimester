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

typedef struct SpawnEnemyMessage {
	CP_Vector position;
	CP_BOOL tracking;
	int type;
	int score;
}SpawnEnemyMessage;

typedef struct Messenger {
	void (*messages[MSG_NUM_MESSAGE_TYPE])(void*);
}Messenger;

extern Messenger g_messenger;
#endif