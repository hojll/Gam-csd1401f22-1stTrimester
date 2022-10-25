#include <stdio.h>
#include "cprocessing.h"
#include <stdlib.h>
#include "entities.h"
#include "messenger.h"


#define MAX_BULLETS 100
#define MAX_PLAYERS 2

Messenger g_messenger;
float g_scaledDt;
/*----------------------*/
// All the entities here//
/*----------------------*/
E_Player player[MAX_PLAYERS];
int playerCount;
E_BulletTest bullets[MAX_BULLETS];
#pragma region MESSAGES
void MessageSpawnBullet(void* messageInfo) {
    SpawnBulletMessage* bulletMSG = (SpawnBulletMessage*)messageInfo;
    // Spawn bullet here
    for (int i = 0; i < MAX_BULLETS; ++i) {
        E_BulletTest* curr = &bullets[i];
        if (curr->go.active)
            continue;
        curr->go.active = 1;
        curr->go.vel = bulletMSG->vel;
        curr->go.pos = bulletMSG->position;
        break;
    }
}
#pragma endregion
void game_init(void)
{
    CP_System_SetWindowSize(900, 900);
    CP_Settings_RectMode(CP_POSITION_CENTER);
    //Assets/DigiPen_Singapore_WEB_RED.png
    player[0] = InitializePlayer();
    player[1] = InitializePlayer();
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        player[i].go.pos = CP_Vector_Zero();
        player[i].go.vel = CP_Vector_Zero();
        player[i].go.pos.y = 100.f;
    }
    for (int i = 0; i < MAX_BULLETS; ++i) {
        bullets[i].go.active = 0;
        bullets[i].go.pos = CP_Vector_Zero();
        bullets[i].go.vel = CP_Vector_Zero();
    }
    g_scaledDt = 0.f;
    g_messenger.messages[MSG_SPAWN_BULLET] = MessageSpawnBullet;
    playerCount = 1;
    CP_Settings_TextSize(50.0f);
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

}

void game_update(void)
{
    // Clean Render
    CP_Graphics_ClearBackground(CP_Color_Create(150, 150, 150, 255));
    // Update scaled dt
    g_scaledDt = CP_System_GetDt();
    // Update players
    for (int i = 0; i < playerCount; ++i) {
        player[i].Update[player[i].state](&player[i]);
        // Update position
        player[i].go.pos = CP_Vector_Add(player[i].go.pos, CP_Vector_Scale(player[i].go.vel, g_scaledDt));
    }
    // Update bullets
    for (int i = 0; i < MAX_BULLETS; ++i) {
        // DO NOT KEEP THIS. VEL SHOULD BE UPDATED IN BULLET UPDATE
        bullets[i].go.pos = CP_Vector_Add(bullets[i].go.pos, CP_Vector_Scale(bullets[i].go.vel, g_scaledDt));
    }
    if (CP_Input_KeyTriggered(KEY_Q))
        CP_Engine_Terminate();


    // Render stuff here
    // 1
    // Player 
    const CP_Color playerColor = CP_Color_Create(100, 0, 0, 255);
    CP_Settings_Fill(playerColor);
    for (int i = 0; i < playerCount; ++i) {
        const float PLAYER_SIZE_TEMP = 50.f;
        CP_Graphics_DrawCircle(player[i].go.pos.x, player[i].go.pos.y, PLAYER_SIZE_TEMP);
    }
    // Bullets
    const CP_Color bulletColor = CP_Color_Create(0, 100, 0, 255);
    CP_Settings_Fill(bulletColor);
    for (int i = 0; i < MAX_BULLETS; ++i) {
        const float BULLET_SIZE_TEMP = 20.f;
        CP_Graphics_DrawCircle(bullets[i].go.pos.x, bullets[i].go.pos.y, BULLET_SIZE_TEMP);
    }
}

void game_exit(void)
{
}
