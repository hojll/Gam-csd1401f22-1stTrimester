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
#include "e_weaponBox.h"
#include "combo_counter_ui.h"

#define MAX_BULLETS 100
#define MAX_ENEMIES 100
#define MAX_PLAYERS 2
#define MAX_WALLS 10
#define MAX_TEXT_POPUP 20
#define MAX_WEAPON_BOX 3

#define WEAPON_BOX_SPAWN_TIME 5

#define DEFAULT_FONT_SIZE 100.0f
#define DEFAULT_FONT_COLOR CP_Color_Create(0, 0, 0, 255)

#define OZNOLA_METER_MAX 6
#define OZNOLA_BASE_DIFFICULTY_MAX 3;
//o z n o l a
#define BASE_SPAWN_FREQUENCY 2.5f
#define COMBO_TIME_DEDUCTION 1.0f
#define COMBO_TIME 2.f

/*
In �Configuration Properties->Debugging- Working Directory�
$(SolutionDir)bin\$(Configuration)-$(Platform)\
Use this in release mode settings if no work.
*/

Messenger g_messenger;
float g_scaledDt;
/*-------*/
// MEDIA //
/*-------*/
CP_Image sprites[NUM_SPRITES];
CP_Image spriteData[NUM_SPRITES];
/*----------------------*/
// All the entities here//
/*----------------------*/
E_Player player[MAX_PLAYERS];
int playerCount;

E_Bullet bullets[MAX_BULLETS];
GameObject walls[MAX_WALLS];

// Enemy stuff by Ryan
E_Basic_Enemy_1 enemies[MAX_ENEMIES];
CP_Vector e_spawnPos1, e_spawnPos2; // Basic Enemy spawn locations
CP_Vector e2_spawnPos[4];

GameObject ai_nodes[MAX_PATHFINDING_NODES];
CP_BOOL shownodes = 1;
GameObject *playerPrevPlatform;
// Oznola Game Loop stuff
int oznola_meter = 0;
int oznola_difficulty[5];
double spawntimer = 0;
int combocounter = 0;
double combocounter_timer = 0;
double show_oznometer_fade = 0;


int total_bullet_count; // For UI by Joel
TextPopUp popUp[MAX_TEXT_POPUP]; // For UI by Joel

E_WeaponBox weapon_boxes[MAX_WEAPON_BOX];
float spawnWeaponBoxTimer;


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
        curr->tracking = enemyMsg->tracking;
        curr->enemytype = enemyMsg->type;        
        break;
    }
}
#pragma endregion

// call this function everytime you kill an enemy
void killconfirmed()
{
    ++combocounter;
    addcombotime(&combocounter_timer, COMBO_TIME);
}

void game_init(void)
{
    /*---------------------*/
    // MEDIA INITIALIZAION //
    /*---------------------*/
    // Initialization of spriteData should be done within entity Initialize
    // within their respective file
    sprites[SPRITE_PLAYER] = CP_Image_Load("./Assets/SPRITESHEET.png");
    CP_System_SetFrameRate(60.f);
    printf("Image dims: %d, %d\n", CP_Image_GetWidth(sprites[SPRITE_PLAYER]), CP_Image_GetHeight(sprites[SPRITE_PLAYER]));
    /////////////////////////
    CP_System_SetWindowSize(900, 900);
    CP_Settings_RectMode(CP_POSITION_CENTER);
    //Assets/DigiPen_Singapore_WEB_RED.png
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        InitializePlayer(&player[i]);
    }
    player[0].go.active = 1;
    for (int i = 0; i < MAX_BULLETS; ++i) {
        bullets[i] = InitializeBullet();
    }

    // AI / ENEMY
    InitEnemyList(enemies, (int)MAX_ENEMIES, ai_nodes);
    e_spawnPos1 = CP_Vector_Set(250, 110);
    e_spawnPos2 = CP_Vector_Set(650, 110);
    e2_spawnPos[0] = CP_Vector_Set(200, 800);
    e2_spawnPos[1] = CP_Vector_Set(700, 800);
    e2_spawnPos[2] = CP_Vector_Set(200, 200);
    e2_spawnPos[3] = CP_Vector_Set(700, 200);
    
    oznola_difficulty[0] = 2;
    oznola_difficulty[1] = 4;
    oznola_difficulty[2] = 8;
    oznola_difficulty[3] = 16;
    oznola_difficulty[4] = 32;

    
    ai_nodes[5].pos = CP_Vector_Set(200, 250); // top left
    ai_nodes[4].pos = CP_Vector_Set(700, 250); // top right
    ai_nodes[3].pos = CP_Vector_Set(450, 450); // middle
    ai_nodes[2].pos = CP_Vector_Set(200, 650); // left
    ai_nodes[1].pos = CP_Vector_Set(700, 650); // right
    ai_nodes[0].pos = CP_Vector_Set(450, 850); // btm

    for (int i = 0; i < MAX_PATHFINDING_NODES; i++)
        ai_nodes[i].active = 1; 

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
    total_bullet_count = player[0].currAmmo;
    for (int i = 0; i < MAX_TEXT_POPUP; ++i)
    {
        set_popup(&popUp[i], 0.0f, 0.0f, DEFAULT_FONT_COLOR, (int)DEFAULT_FONT_SIZE, 0, "initializing");
    }

    //Weapon Boxes
    for (int i = 0; i < MAX_WEAPON_BOX; i++)
    {
        weapon_boxes[i] = InitializeWeaponBox();
    }
    // So the first box spawns faster
    spawnWeaponBoxTimer = 2;
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
            player[i].Update[player[i].state](&player[i]);
        // Update position
    }
    // Update bullets
    for (int i = 0; i < MAX_BULLETS; ++i) {
        if (bullets[i].go.active)
            bullets[i].Update(&bullets[i]);
    }
    // Update weapon box and spawning
    for (int i = 0; i < MAX_WEAPON_BOX; ++i) {
        if (weapon_boxes[i].go.active)
            weapon_boxes[i].Update(&weapon_boxes[i]);
    }
    // Spawn weapon box
    spawnWeaponBoxTimer -= g_scaledDt;
    if (spawnWeaponBoxTimer <= 0)
    {
        spawnWeaponBoxTimer = WEAPON_BOX_SPAWN_TIME;

        CP_BOOL lessThan3Box = 0;
        // Check if there is alr 3 boxes
        for (int i = 0; i < MAX_WEAPON_BOX; ++i)
        {
            if (!weapon_boxes[i].go.active)
                lessThan3Box = 1;
        }

        if (lessThan3Box)
        {
            for (int i = 0; i < MAX_WEAPON_BOX; ++i)
            {
                E_WeaponBox* curr = &weapon_boxes[i];
                if (curr->go.active)
                    continue;
                curr->go.active = 1;
                curr->lifetime = WEAPON_BOX_LIFETIME;
                curr->color = WEAPON_BOX_COLOR;
                curr->go.pos.x = CP_Random_RangeFloat(100.f, 800.f);
                curr->go.pos.y = CP_Random_RangeFloat(100.f, 800.f);
                break;
            }
        }
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

        enemy.tracking = 0;
        g_messenger.messages[MSG_SPAWN_ENEMY](&enemy);
    }

    if (CP_Input_KeyTriggered(KEY_MINUS)) {
        SpawnEnemy(1, e2_spawnPos[0]);
    }
    if (CP_Input_KeyTriggered(KEY_0)) {
        SpawnEnemy(1, e2_spawnPos[1]);
    }
    if (CP_Input_KeyTriggered(KEY_9)) {
        SpawnEnemy(1, e2_spawnPos[2]);
    }
    if (CP_Input_KeyTriggered(KEY_8)) {
        SpawnEnemy(1, e2_spawnPos[3]);
    }


     // spawning frequency
    spawntimer += 1.0f * CP_System_GetDt();
    if (spawntimer > BASE_SPAWN_FREQUENCY)
    {
        spawntimer = 0.0f; // spawn
        int random_pos = returnRange(1, 50);
        if (random_pos <= 25)
            SpawnEnemy(0, e_spawnPos1);
        else
            SpawnEnemy(0, e_spawnPos2);

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
                    playerPrevPlatform = &walls[j];                    
                }
                else if (collision_dir == COLLISION_BOTTOM)
                {
                    player[i].go.pos.y = walls[j].pos.y + walls[j].height / 2.f + player[i].go.height / 2.f;
                    player->go.vel.y = 0;
                }
                else if (collision_dir == COLLISION_LEFT)
                {
                    player[i].go.pos.x = walls[j].pos.x - walls[j].width / 2.f - player[i].go.width / 2.f;
                    printf("WALL INTERRUPED ROLL\n");
                    player[i].currAnim = SetSpriteAnim(&player[i].animations[ANIM_PLAYER_ACTIVE], PLAYER_ACTIVE_ANIM_SPEED);
                    player[i].state = STATE_PLAYER_ACTIVE;
                }
                else
                {
                    player[i].go.pos.x = walls[j].pos.x + walls[j].width / 2.f + player[i].go.width / 2.f;
                    printf("WALL INTERRUPED ROLL\n");
                    player[i].currAnim = SetSpriteAnim(&player[i].animations[ANIM_PLAYER_ACTIVE], PLAYER_ACTIVE_ANIM_SPEED);
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

    //Weapon Box - x
    for (int i = 0; i < MAX_WEAPON_BOX; ++i)
    {
        if (!weapon_boxes[i].go.active)
            continue;
        CP_BOOL weapon_box_grounded_flag = 0;

        // Weapon Box - Wall
        for (int j = 0; j < MAX_WALLS; ++j)
        {
            if (!walls[j].active)
                continue;
            if (AABB(weapon_boxes[i].go, walls[j]))
            {
                COLLISION_DIRECTION collision_dir = AABB_Direction(weapon_boxes[i].go, walls[j]);
                if (collision_dir == COLLISION_TOP)
                {
                    weapon_boxes[i].go.pos.y = walls[j].pos.y - walls[j].height / 2.f - weapon_boxes[i].go.height / 2.f;
                    weapon_boxes->grounded = 1;
                    weapon_boxes->go.vel.y = 0;
                }
                else if (collision_dir == COLLISION_BOTTOM)
                {
                    weapon_boxes[i].go.pos.y = walls[j].pos.y + walls[j].height / 2.f + weapon_boxes[i].go.height / 2.f;
                    weapon_boxes->go.vel.y = 0;
                }
                else if (collision_dir == COLLISION_LEFT)
                {
                    weapon_boxes[i].go.pos.x = walls[j].pos.x - walls[j].width / 2.f - weapon_boxes[i].go.width / 2.f;
                }
                else
                {
                    weapon_boxes[i].go.pos.x = walls[j].pos.x + walls[j].width / 2.f + weapon_boxes[i].go.width / 2.f;
                }
            }

            // Grounded check (Walking off platforms)
            if (!weapon_boxes[i].grounded)
                continue;
            // This creates a point right below the player and check if there is a wall there
            if (Point_AABB(CP_Vector_Set(weapon_boxes[i].go.pos.x + weapon_boxes[i].go.width / 2.f, weapon_boxes[i].go.pos.y + weapon_boxes[i].go.height / 2.f + 0.1f), walls[j]) ||
                Point_AABB(CP_Vector_Set(weapon_boxes[i].go.pos.x - weapon_boxes[i].go.width / 2.f, weapon_boxes[i].go.pos.y + weapon_boxes[i].go.height / 2.f + 0.1f), walls[j]))
                weapon_box_grounded_flag = 1;
        }



        if (!weapon_box_grounded_flag)
            weapon_boxes[i].grounded = 0;
    }

    // Enemies 
    for (int j = 0; j < MAX_ENEMIES; ++j)
    {
        if (!enemies[j].go.active)
            continue;
        EnemytoWallCollision(&enemies[j], walls);
        if (playerPrevPlatform != NULL)
        {
            //EnemyPathing(&enemies[j], walls, &player, playerPrevPlatform, MAX_WALLS);
            EnemyPathing(&enemies[j], ai_nodes, &player, playerPrevPlatform, MAX_PATHFINDING_NODES);

        }

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
            // Drawing image instead
            RenderSpriteAnim(&player[i].currAnim, sprites[SPRITE_PLAYER], player[i].go.pos.x, 
                player[i].go.pos.y, player[i].go.height, player[i].go.height, 255);
             
            //CP_Image_DrawSubImage(sprites[0], player[i].go.pos.x, player[i].go.pos.y, player[i].go.height, player[i].go.height,
            //    0, 0, 32, 32, 255);

            //CP_Image_Draw(sprites[SPRITE_PLAYER], player[i].go.pos.x, player[i].go.pos.y, player[i].go.height, player[i].go.height, 255);
            //CP_Graphics_DrawCircle(player[i].go.pos.x, player[i].go.pos.y, player[i].go.height);
            if (player[i].state == STATE_PLAYER_ROLLING)
            {
                for (int j = 0; j < PLAYER_ROLL_AFTERIMAGE; j++)
                    if (!CP_Vector_Equal(player->rollPrevPos[j], CP_Vector_Zero()))
                    {
                        CP_Settings_StrokeWeight(0.f);
                        
                        //CP_Settings_Fill(CP_Color_Create(100, 0, 0, (int)player[i].rollAlpha[j]));
                        RenderSpriteAnimOffset(&player[i].currAnim, sprites[SPRITE_PLAYER], player[i].rollPrevPos[j].x,
                            player[i].rollPrevPos[j].y, player[i].go.height, player[i].go.height, (int)player[i].rollAlpha[j], j);
                        //CP_Graphics_DrawCircle(player[i].rollPrevPos[j].x, player[i].rollPrevPos[j].y, player[i].go.height);
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
    const CP_Color enemyColor2 = CP_Color_Create(20, 227, 199, 255);
    for (int i = 0; i < MAX_ENEMIES; ++i)
    {
        if (!enemies[i].go.active)
            continue;
        if (enemies[i].tracking)
        {
            CP_Settings_Fill(enemyColor2);
        }
        else
        {
            CP_Settings_Fill(enemyColor);
        }
        CP_Graphics_DrawCircle(enemies[i].go.pos.x, enemies[i].go.pos.y, enemies[i].go.height);
        
    }

    // Walls
    const CP_Color wallColor = CP_Color_Create(120, 120, 120, 255);
    CP_Settings_Fill(wallColor);
    for (int i = 0; i < MAX_WALLS; ++i) {
        if (walls[i].active)
            CP_Graphics_DrawRect(walls[i].pos.x, walls[i].pos.y, walls[i].width, walls[i].height);
    }

    // Weapon Boxes
    for (int i = 0; i < MAX_WEAPON_BOX; ++i) {
        if (weapon_boxes[i].go.active)
        {
            CP_Settings_Fill(weapon_boxes[i].color);
            CP_Settings_StrokeWeight(0.f);
            CP_Graphics_DrawRect(weapon_boxes[i].go.pos.x, weapon_boxes[i].go.pos.y, weapon_boxes[i].go.width, weapon_boxes[i].go.height);
            CP_Settings_StrokeWeight(3.f);
        }
    }

    // Render AI Pathfinding nodes
    if (shownodes)
    {
        const CP_Color nodeColor = CP_Color_Create(118, 78, 191, 255);
        CP_Settings_Fill(nodeColor);
        for (int i = 0; i < MAX_PATHFINDING_NODES; ++i)
        {
            CP_Graphics_DrawRect(ai_nodes[i].pos.x, ai_nodes[i].pos.y, 20, 20);
        }
    }

    // UI ELEMENTS
    CP_Settings_TextSize(DEFAULT_FONT_SIZE);
    CP_Settings_Fill(DEFAULT_FONT_COLOR);
    update_timer();


 

    float oznoalpha = updateOznometerFade(255, &combocounter_timer, COMBO_TIME);
    updateComboCounterTimer(&combocounter,&combocounter_timer, COMBO_TIME_DEDUCTION, COMBO_TIME);

    printComboCounter(CP_Vector_Set(129.8f, 40), 52, 0, 0, 0, oznoalpha);
    printComboCounter(CP_Vector_Set(130, 40), 50, 194, 46, 19, oznoalpha);
    
    CP_Vector barpos = CP_Vector_Set(CP_System_GetWindowWidth() * 0.20F, 
        CP_System_GetWindowHeight() * 0.015F);
    printComboCountdownTimer(CP_Vector_Set(35, 64), 
        CP_Vector_Set((float)(combocounter_timer / COMBO_TIME) * barpos.x
        , barpos.y), 194, 46, 19, oznoalpha);
    char combocountertxt[5];
    sprintf_s(combocountertxt, sizeof((int)combocounter), "%d", combocounter);
    CP_Font_DrawText(combocountertxt, barpos.x + 78, barpos.y + 35);

    // OZNOLA METER
    if (CP_Input_KeyTriggered(KEY_F11)) // debug
    {
        // lowkey got lazy u want add time just call this func ea time the mob die
        killconfirmed();
    }


    if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT))
    {
        for (int i = 0; i < MAX_TEXT_POPUP; ++i)
        {
            if (!(popUp[i].go.active))
            {
                set_popup(&popUp[i], CP_Input_GetMouseX(), CP_Input_GetMouseY(), CP_Color_Create(255, 0, 0, 255), (int)DEFAULT_FONT_SIZE, 3.0f, "Pickup/Damage");
                break;
            }
        }
    }

    update_bullet_bar(player[0].currAmmo, total_bullet_count);
    for (int i = 0; i < MAX_TEXT_POPUP; ++i)
    {
        update_popup(&popUp[i]);
        draw_popup(&popUp[i]);
    }
}

void game_exit(void)
{
}
