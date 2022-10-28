#include <stdio.h>
#include "cprocessing.h"
#include <stdlib.h>
#include "entities.h"
#include "messenger.h"
#include "collision.h"
#include "e_player.h"
#include "utilities.h"

#define MAX_BULLETS 100
#define MAX_PLAYERS 2
#define MAX_WALLS 10

Messenger g_messenger;
float g_scaledDt;
/*----------------------*/
// All the entities here//
/*----------------------*/
E_Player player[MAX_PLAYERS];
int playerCount;
E_BulletTest bullets[MAX_BULLETS];
GameObject walls[MAX_WALLS];
int current_bullet_count, total_bullet_count; // For UI by Joel

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
    //player[1] = InitializePlayer();

    for (int i = 0; i < MAX_PLAYERS; ++i) {
        player[i].go.pos = CP_Vector_Zero();
        player[i].go.vel = CP_Vector_Zero();
        player[i].go.pos.y = 200.f;
        player[i].go.pos.x = 450.f;
        player[i].go.width = 50.f;
        player[i].go.height = 50.f;
        player[i].grounded = 0;
    }
    for (int i = 0; i < MAX_BULLETS; ++i) {
        bullets[i].go.active = 0;
        bullets[i].go.pos = CP_Vector_Zero();
        bullets[i].go.vel = CP_Vector_Zero();
    }

    // Walls
    // Bottom
    walls[0].pos = CP_Vector_Set(450, 850);
    walls[0].height = 30.f;
    walls[0].width = 800.f;
    walls[0].active = 1;
    // Top
    walls[1].pos = CP_Vector_Set(450, 50);
    walls[1].height = 30.f;
    walls[1].width = 800.f;
    walls[1].active = 1;
    // Left
    walls[2].pos = CP_Vector_Set(50, 450);
    walls[2].height = 830.f;
    walls[2].width = 30.f;
    walls[2].active = 1;
    // Right
    walls[3].pos = CP_Vector_Set(850, 450);
    walls[3].height = 830.f;
    walls[3].width = 30.f;
    walls[3].active = 1;

    // Walls in the level
    walls[4].pos = CP_Vector_Set(250, 650);
    walls[4].height = 20.f;
    walls[4].width = 200.f;
    walls[4].active = 1;

    walls[5].pos = CP_Vector_Set(650, 650);
    walls[5].height = 20.f;
    walls[5].width = 200.f;
    walls[5].active = 1;

    walls[6].pos = CP_Vector_Set(450, 450);
    walls[6].height = 20.f;
    walls[6].width = 300.f;
    walls[6].active = 1;

    walls[7].pos = CP_Vector_Set(250, 250);
    walls[7].height = 20.f;
    walls[7].width = 200.f;
    walls[7].active = 1;

    walls[8].pos = CP_Vector_Set(650, 250);
    walls[8].height = 20.f;
    walls[8].width = 200.f;
    walls[8].active = 1;

    g_scaledDt = 0.f;
    g_messenger.messages[MSG_SPAWN_BULLET] = MessageSpawnBullet;
    playerCount = 1;
    CP_Settings_TextSize(50.0f);
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

    //Bullet count for UI stuff
    current_bullet_count = total_bullet_count = 15;
}

void game_update(void)
{
    // Clean Render
    CP_Graphics_ClearBackground(CP_Color_Create(150, 150, 150, 255));
    // Update scaled dt
    g_scaledDt = CP_System_GetDt();
    // Update players
    for (int i = 0; i < playerCount; ++i) {
        if(player[i].go.active)
            player[i].Update[player[i].state](&player[i], g_scaledDt);
        // Update position
    }
    // Update bullets
    for (int i = 0; i < MAX_BULLETS; ++i) {
        // DO NOT KEEP THIS. VEL SHOULD BE UPDATED IN BULLET UPDATE
        if (bullets[i].go.active)
            bullets[i].go.pos = CP_Vector_Add(bullets[i].go.pos, CP_Vector_Scale(bullets[i].go.vel, g_scaledDt));
    }
    if (CP_Input_KeyTriggered(KEY_Q))
        CP_Engine_Terminate();

    // Collision Loops
    // Player - x
    for (int i = 0; i < playerCount; ++i)
    {
        if (!player[i].go.active)
            continue;
        CP_BOOL player_grounded_flag = 0;

        // Player - Wall
        for (int j = 0; j < MAX_WALLS; ++j)
        {
            if (!walls[j].active)
                continue;
            if (AABB(player[i].go, walls[j]))
            {
                COLLISION_DIRECTION collision_dir = AABB_Direction(player[i].go, walls[j]);
                if (collision_dir == COLLISION_TOP )
                {
                    player[i].go.pos.y = walls[j].pos.y - walls[j].height / 2.f - player[i].go.height / 2.f;
                    player->grounded = 1;
                    player->go.vel.y = 0;
                }
                else if (collision_dir == COLLISION_BOTTOM)
                {
                    player[i].go.pos.y = walls[j].pos.y + walls[j].height / 2.f + player[i].go.height / 2.f;
                    player->go.vel.y = 0;
                }
                else if (collision_dir == COLLISION_LEFT)
                {
                    player[i].go.pos.x = walls[j].pos.x - walls[j].width / 2.f - player[i].go.width / 2.f;
                    player[i].state = STATE_PLAYER_ACTIVE;
                }
                else
                {
                    player[i].go.pos.x = walls[j].pos.x + walls[j].width / 2.f + player[i].go.width / 2.f;
                    player[i].state = STATE_PLAYER_ACTIVE;
                }
            }

            // Grounded check (Walking off platforms)
            if (!player[i].grounded)
                continue;
            // This creates a point right below the player and check if there is a wall there
            if (Point_AABB(CP_Vector_Set(player[i].go.pos.x + player[i].go.width / 2.f, player[i].go.pos.y + player[i].go.height / 2.f + 0.1f), walls[j])||
                Point_AABB(CP_Vector_Set(player[i].go.pos.x - player[i].go.width / 2.f, player[i].go.pos.y + player[i].go.height / 2.f + 0.1f), walls[j]))
                player_grounded_flag = 1;
        }

        if (!player_grounded_flag)
            player[i].grounded = 0;
    }

    // Bullet - x
    for (int i = 0; i < MAX_BULLETS; ++i)
    {
        if (!bullets[i].go.active)
            continue;

        // Bullet - Wall
        for (int j = 0; j < MAX_WALLS; ++j)
        {
            if (!walls[j].active)
                continue;
            if (AABB(bullets[i].go, walls[j]))
            {
                bullets[i].go.active = 0;
            }
        }
    }


    // Render stuff here
    // Player 
    const CP_Color playerColor = CP_Color_Create(100, 0, 0, 255);
    for (int i = 0; i < playerCount; ++i) {
        if (player[i].go.active)
        {
            CP_Settings_Fill(playerColor);
            CP_Graphics_DrawCircle(player[i].go.pos.x, player[i].go.pos.y, player[i].go.height);
            if (player[i].state == STATE_PLAYER_ROLLING)
            {
                for (int j = 0; j < PLAYER_ROLL_AFTERIMAGE; j++)
                    if (!CP_Vector_Equal(player->rollPrevPos[j], CP_Vector_Zero()))
                    {
                        CP_Settings_StrokeWeight(0.f);
                        CP_Settings_Fill(CP_Color_Create(100, 0, 0, player[i].rollAlpha[j]));
                        CP_Graphics_DrawCircle(player[i].rollPrevPos[j].x, player[i].rollPrevPos[j].y, player[i].go.height);
                        CP_Settings_StrokeWeight(3.f);
                    }
            }
        }
    }
    // Bullets
    const CP_Color bulletColor = CP_Color_Create(0, 100, 0, 255);
    CP_Settings_Fill(bulletColor);
    for (int i = 0; i < MAX_BULLETS; ++i) {
        const float BULLET_SIZE_TEMP = 20.f;
        if (bullets[i].go.active)
            CP_Graphics_DrawCircle(bullets[i].go.pos.x, bullets[i].go.pos.y, BULLET_SIZE_TEMP);
    }
    // Walls
    const CP_Color wallColor = CP_Color_Create(120, 120, 120, 255);
    CP_Settings_Fill(wallColor);
    for (int i = 0; i < MAX_WALLS; ++i) {
        if (walls[i].active)
            CP_Graphics_DrawRect(walls[i].pos.x, walls[i].pos.y, walls[i].width, walls[i].height);
    }

    // UI ELEMENTS
    update_timer();
    if (CP_Input_KeyTriggered(KEY_SPACE))
    {
        current_bullet_count -= 1;
    }
    update_bullet_bar(current_bullet_count, total_bullet_count);
}

void game_exit(void)
{
}
