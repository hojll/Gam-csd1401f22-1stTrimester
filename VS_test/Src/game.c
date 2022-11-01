#include <stdio.h>
#include "cprocessing.h"
#include <stdlib.h>
#include "entities.h"
#include "messenger.h"
#include "collision.h"
#include "e_player.h"
#include "utilities.h"
#include "ui.h"
#include "e_text_popup.h"
#include "e_basicenemy_1.h"
#include "spriteData.h"
#include "e_bullet.h"

#define MAX_BULLETS 100
#define MAX_ENEMIES 100
#define MAX_PLAYERS 2
#define MAX_WALLS 10
#define MAX_TEXT_POPUP 20

#define DEFAULT_FONT_SIZE 100.0f
#define DEFAULT_FONT_COLOR CP_Color_Create(0, 0, 0, 255)
#define MAX_PATHFINDING_NODES 6
/*
In “Configuration Properties->Debugging- Working Directory”
$(SolutionDir)bin\$(Configuration)-$(Platform)\
Use this in release mode settings if no work.
*/

Messenger g_messenger;
float g_scaledDt;
/*-------*/
// MEDIA //
/*-------*/
CP_Image sprites[NUM_SPRITES];
/*----------------------*/
// All the entities here//
/*----------------------*/
E_Player player[MAX_PLAYERS];
int playerCount;

E_Bullet bullets[MAX_BULLETS];
GameObject walls[MAX_WALLS];

E_Basic_Enemy_1 enemies[MAX_ENEMIES];
CP_Vector e_spawnPos1, e_spawnPos2; // Enemy spawn locations
GameObject ai_nodes[MAX_PATHFINDING_NODES];
CP_BOOL shownodes = 1;

int current_bullet_count, total_bullet_count; // For UI by Joel
TextPopUp popUp[MAX_TEXT_POPUP]; // For UI by Joel



#pragma region MESSAGES
void MessageSpawnBullet(void* messageInfo) {
    SpawnBulletMessage* bulletMSG = (SpawnBulletMessage*)messageInfo;
    // Spawn bullet here
    for (int i = 0; i < MAX_BULLETS; ++i) {
        E_Bullet* curr = &bullets[i];
        if (curr->go.active)
            continue;
        curr->go = bulletMSG->go;
        curr->go.active = 1;
        curr->bullet_type = bulletMSG->type;
        curr->lifetime = bulletMSG->lifetime;
        curr->color = bulletMSG->color;
        break;
    }
}
#pragma endregion

// Idk why ur pragma region no rike me 
void MessageSpawnEnemy(void* messageInfo) {
    SpawnEnemyMessage* enemyMsg = (SpawnEnemyMessage*)messageInfo;
    // Spawn enemy here
    for (int j = 0; j < MAX_ENEMIES; ++j)
    {
        E_Basic_Enemy_1* curr = &enemies[j];
        if (curr->go.active)
            continue;
        curr->go.active = 1;
        curr->go.pos = enemyMsg->position;
        break;
    }
}


void game_init(void)
{
    CP_System_SetWindowSize(900, 900);
    CP_Settings_RectMode(CP_POSITION_CENTER);
    //Assets/DigiPen_Singapore_WEB_RED.png
    //player[1] = InitializePlayer();
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        player[i] = InitializePlayer();
    }
    for (int i = 0; i < MAX_BULLETS; ++i) {
        bullets[i] = InitializeBullet();
    }

    InitEnemyList(enemies, (int)MAX_ENEMIES);
    e_spawnPos1 = CP_Vector_Set(250, 110);
    e_spawnPos2 = CP_Vector_Set(650, 110);
    ai_nodes[0].pos = CP_Vector_Set(250, 620);
    ai_nodes[1].pos = CP_Vector_Set(650, 620);
    ai_nodes[2].pos = CP_Vector_Set(450, 420);
    ai_nodes[3].pos = CP_Vector_Set(250, 220);
    ai_nodes[4].pos = CP_Vector_Set(650, 220);
    ai_nodes[5].pos = CP_Vector_Set(450, 820);

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
    g_messenger.messages[MSG_SPAWN_ENEMY] = MessageSpawnEnemy;


    playerCount = 1;
    CP_Settings_TextSize(50.0f);
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

    //Bullet count for UI stuff
    current_bullet_count = total_bullet_count = 10;
    for (int i = 0; i < MAX_TEXT_POPUP; ++i)
    {
        set_popup(&popUp[i], 0.0f, 0.0f, DEFAULT_FONT_COLOR, DEFAULT_FONT_SIZE, 0, "initializing");
    }
}

void game_update(void)
{
    // Clean Render
    CP_Graphics_ClearBackground(CP_Color_Create(150, 150, 150, 255));
    // Update scaled dt
    g_scaledDt = CP_System_GetDt();

    //printf("play pos %.2f,%.2f\n", player->go.pos.x, player->go.pos.y);

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
            bullets[i].Update(&bullets[i]);
    }
    
    UpdateEnemyList(enemies, MAX_ENEMIES);

    if (CP_Input_KeyTriggered(KEY_Q))
        CP_Engine_Terminate();

    // Debug Spawn Enemy
    if (CP_Input_KeyTriggered(KEY_EQUAL)) {
        SpawnEnemyMessage enemy;
        int random_pos = returnRange(1, 50);
        if (random_pos <= 25)
            enemy.position = e_spawnPos1;
        else
            enemy.position = e_spawnPos2;


        g_messenger.messages[MSG_SPAWN_ENEMY](&enemy);
    }

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
                bullets[i].collide_pos = bullets[i].go.pos;
                COLLISION_DIRECTION collision_dir = AABB_Direction(bullets[i].go, walls[j]);
                if (collision_dir == COLLISION_TOP)
                    bullets[i].collide_pos.y = walls[j].pos.y - walls[j].height / 2.f - bullets[i].go.height / 2.f;
                else if (collision_dir == COLLISION_BOTTOM)
                    bullets[i].collide_pos.y = walls[j].pos.y + walls[j].height / 2.f + bullets[i].go.height / 2.f;
                else if (collision_dir == COLLISION_LEFT)
                    bullets[i].collide_pos.x = walls[j].pos.x - walls[j].width / 2.f - bullets[i].go.width / 2.f;
                else
                    bullets[i].collide_pos.x = walls[j].pos.x + walls[j].width / 2.f + bullets[i].go.width / 2.f;

                bullets->Destroy(&bullets[i]);
            }
        }
    }

    // Enemies 
    for (int j = 0; j < MAX_ENEMIES; ++j)
    {
        if (!enemies[j].go.active)
            continue;
        EnemytoWallCollision(&enemies[j], walls);
    }
    

    //----------------------------------------------------------------------------------------------------------------------
    // Render stuff here
    //----------------------------------------------------------------------------------------------------------------------

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
    for (int i = 0; i < MAX_BULLETS; ++i) {
        if (bullets[i].go.active)
        {
            CP_Settings_Fill(bullets[i].color);
            CP_Graphics_DrawCircle(bullets[i].go.pos.x, bullets[i].go.pos.y, bullets[i].go.height);
        }
    }

    // Enemies 
    const CP_Color enemyColor = CP_Color_Create(125, 181, 130, 255);
    CP_Settings_Fill(enemyColor);
    for (int i = 0; i < MAX_ENEMIES; ++i)
    {
        if(enemies[i].go.active)
            CP_Graphics_DrawCircle(enemies[i].go.pos.x, enemies[i].go.pos.y, enemies[i].go.height);
    }

    // Walls
    const CP_Color wallColor = CP_Color_Create(120, 120, 120, 255);
    CP_Settings_Fill(wallColor);
    for (int i = 0; i < MAX_WALLS; ++i) {
        if (walls[i].active)
            CP_Graphics_DrawRect(walls[i].pos.x, walls[i].pos.y, walls[i].width, walls[i].height);
    }

    // UI ELEMENTS
    CP_Settings_TextSize(DEFAULT_FONT_SIZE);
    CP_Settings_Fill(DEFAULT_FONT_COLOR);
    update_timer();
    if (CP_Input_KeyTriggered(KEY_SPACE))
    {
        current_bullet_count -= 1;
        for (int i = 0; i < MAX_TEXT_POPUP; ++i)
        {
            if (!(popUp[i].go.active))
            {
                set_popup(&popUp[i], CP_Input_GetMouseX(), CP_Input_GetMouseY(), CP_Color_Create(255, 0, 0, 255), DEFAULT_FONT_SIZE, 3.0f, "CLICK");
                break;
            }
        }
    }

    update_bullet_bar(current_bullet_count, total_bullet_count);
    for (int i = 0; i < MAX_TEXT_POPUP; ++i)
    {
        update_popup(&popUp[i]);
        draw_popup(&popUp[i]);
    }
}



void game_exit(void)
{
}
