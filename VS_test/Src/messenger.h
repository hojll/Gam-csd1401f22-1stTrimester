#ifndef MESSENGER_H
#define MESSENGER_H
#include "cprocessing.h"
typedef enum MessageType {
	MSG_SPAWN_BULLET,
	MSG_NUM_MESSAGE_TYPE
}MessageType;

typedef struct SpawnBulletMessage {
	CP_Vector position;
	CP_Vector vel;
}SpawnBulletMessage;

typedef struct Messenger {
	void (*messages[MSG_NUM_MESSAGE_TYPE])(void*);
}Messenger;

extern Messenger g_messenger;
#endif