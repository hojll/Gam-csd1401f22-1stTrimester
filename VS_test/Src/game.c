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
#include "e_basicenemy_2.h"
#include "spriteData.h"
#include "e_bullet.h"
#include "e_weaponBox.h"
#include "combo_counter_ui.h"
#include "game_over.h"

enum {
    MAX_BULLETS = 100,
    MAX_ENEMIES = 100,
    MAX_PLAYERS = 2,
    MAX_WALLS = 12,
    MAX_TEXT_POPUP = 20,
    MAX_WEAPON_BOX = 5,
    MAX_GAMESTART_TEXT = 9,
};

static const float WEAPON_BOX_SPAWN_TIME = 3;

static const float DEFAULT_FONT_SIZE = 150.0f;
#define DEFAULT_FONT_COLOR CP_Color_Create(0, 0, 0, 255)

char GAMEOVER = 0;

enum {
    OZNOLA_METER_MAX = 6,
    OZNOLA_BASE_DIFFICULTY_MAX = 3
};
    //o z n o l a
static const float BASE_SPAWN_FREQUENCY = 1.5f; 
static const float COMBO_TIME_DEDUCTION = 1.0f; // speed where the combo end
static const float COMBO_TIME = 3.f; // time given before combo counter end
static const float DIFFICULT_INCREMENT = 10.f; // difficulty increment
static const float MAX_DT_SCALE = 1.3f;

/*
In �Configuration Properties->Debugging- Working Directory�
$(SolutionDir)bin\$(Configuration)-$(Platform)\
Use this in  release mode settings if no work.
*/

Messenger g_messenger;
float g_scaledDt;
/*-------*/
// MEDIA //
/*-------*/
CP_Image backgroundSprite;
CP_Image sprites[NUM_SPRITES];
CP_Image spriteData[NUM_SPRITES]; // TODO: REMOVE THIS SHIT IF NOT USING
/*----------------------*/
// All the entities here//
/*----------------------*/
E_Player player[MAX_PLAYERS];
int playerCount;

E_Bullet bullets[MAX_BULLETS];
GameObject walls[MAX_WALLS];

// Enemy stuff by Ryan
E_Basic_Enemy* enemies;
CP_Vector e_spawnPos1, e_spawnPos2; // Basic Enemy spawn locations
GameObject e2_spawnPos[4];

GameObject ai_nodes[MAX_PATHFINDING_NODES];
CP_BOOL shownodes = 1;
GameObject *playerPrevPlatform;
// Oznola Game Loop stuff
CP_Vector cameraPos;
float scalar = 0;
double difficulty_timer = 0;
float spawntimer[3];
int combocounter = 0;
double combocounter_timer = 0;
double show_oznometer_fade = 0;
static int highest_combo = 0;
int gamestart = 0;
static float gamestart_timer = 1.0f;
static char* gamestart_text[] = {   "Press Spacebar to START!", "Come on already press Spacebar", "Hurry up and press Spacebar!",
                                    "Are you not gonna play?", "Bruh SPACEBAR!!", "I'm just gonna start the game for you!",
                                    "I'm really gonna start the game!", "Why are you doing this to me?", "OK I give up..."};
static int game_start_text_counter = 0;

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
        curr->collide_pos = CP_Vector_Zero();
        break;
    }
}

void MessageSpawnEnemy(void* messageInfo) {
    SpawnEnemyMessage* enemyMsg = (SpawnEnemyMessage*)messageInfo;
    // Spawn enemy here
    for (int j = 0; j < MAX_ENEMIES; ++j)
    {
        E_Basic_Enemy* curr = &enemies[j];
        if (curr->go.active)
            continue;
        curr->go.active = 1;
        curr->go.pos = enemyMsg->position;
        curr->tracking = enemyMsg->tracking;
        if (curr->tracking == 1)
            curr->go.dir.x = 0;
        curr->enemytype = enemyMsg->type;
        switch (enemyMsg->type) {
        case ENEMY_TYPE_1:
            curr->HP = HEALTH_ENEMY_1;
            break;
        case ENEMY_TYPE_2:
            curr->HP = HEALTH_ENEMY_2;
            break;
        case ENEMY_TYPE_3:
            curr->HP = HEALTH_ENEMY_3;
        }
        curr->enemy_shortestNode = NULL;
        curr->redTintVal = 0.f;
        //curr->enemytype = enemyMsg->type;
       
        break;
    }
}

void MessageToPlayerDir(void* messageInfo) {
    ToPlayerDirMessage* msg = (ToPlayerDirMessage*)messageInfo;
    if (msg->entityPos.y > player[0].go.pos.y - player[0].go.height / 2.f)
        msg->direction = player[0].go.pos.x - msg->entityPos.x;
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
    sprites[SPRITE_ENEMY_1] = CP_Image_Load("./Assets/ENEMY1_Spritesheet.png");
    sprites[SPRITE_ENEMY_2] = CP_Image_Load("./Assets/ENEMY2_Spritesheet.png");
    sprites[SPRITE_ENEMY_3] = CP_Image_Load("./Assets/ENEMY3_Spritesheet.png");

    backgroundSprite = CP_Image_Load("./Assets/background.png");
    CP_System_SetFrameRate(60.f);
    printf("Image: %-15s|  dims: %d, %d\n","player", CP_Image_GetWidth(sprites[SPRITE_PLAYER]), CP_Image_GetHeight(sprites[SPRITE_PLAYER]));
    printf("Image: %-15s| dims: %d, %d\n", "background", CP_Image_GetWidth(backgroundSprite), CP_Image_GetHeight(backgroundSprite));
    /////////////////////////
    CP_System_SetWindowSize(1920, 1080);
    CP_Settings_RectMode(CP_POSITION_CENTER);
    cameraPos = CP_Vector_Zero();// Camera
    //Assets/DigiPen_Singapore_WEB_RED.png
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        InitializePlayer(&player[i]);
    }
    player[0].go.active = 1;
    for (int i = 0; i < MAX_BULLETS; ++i) {
        bullets[i] = InitializeBullet();
    }

    e2_spawnPos[0].pos = CP_Vector_Set(150, 150);
    e2_spawnPos[1].pos = CP_Vector_Set(1750, 150);
    e2_spawnPos[2].pos = CP_Vector_Set(150, 800);
    e2_spawnPos[3].pos = CP_Vector_Set(1750, 800);

    // AI / ENEMY
    InitAnimdata_E1();
    enemies = (E_Basic_Enemy*)malloc(sizeof(E_Basic_Enemy) * MAX_ENEMIES);
    InitEnemyList(enemies, MAX_ENEMIES, ai_nodes);
    e_spawnPos1 = CP_Vector_Set(960, 110);
    e_spawnPos2 = CP_Vector_Set(960, 110);

    

    for (int k = 0; k < 3; k++)
    {
        spawntimer[k] = BASE_SPAWN_FREQUENCY;
    }
    
    scalar = 1;
    difficulty_timer = 0;
    gamestart = 0;

    // AI NODES
    {
        // btm platform
        //ai_nodes[0].pos = CP_Vector_Set(540, 1005);
        //ai_nodes[1].pos = CP_Vector_Set(1380, 1005);

        // 2nd btm platform
        //ai_nodes[2].pos = CP_Vector_Set(370, 955);
        //ai_nodes[3].pos = CP_Vector_Set(1550, 955);
        ai_nodes[0].pos = CP_Vector_Set(370, 955);
        ai_nodes[1].pos = CP_Vector_Set(1550, 955);

        // middle
        ai_nodes[2].pos = CP_Vector_Set(480, 725);
        ai_nodes[3].pos = CP_Vector_Set(1440, 725);

        ai_nodes[4].pos = CP_Vector_Set(330, 540);
        ai_nodes[5].pos = CP_Vector_Set(1590, 540);

        ai_nodes[6].pos = CP_Vector_Set(480, 325);
        ai_nodes[7].pos = CP_Vector_Set(1440, 325);

        // bottom 2
        //ai_nodes[8].pos = CP_Vector_Set(960, 1000);
        //ai_nodes[9].pos = CP_Vector_Set(1060, 955);

        for (int i = 0; i < MAX_PATHFINDING_NODES; i++)
        {
            ai_nodes[i].active = 1;
        }
    }
    

    // Walls
    // Bottom
    {
        walls[0].pos = CP_Vector_Set(475, 955);
        walls[0].height = 50.f;
        walls[0].width = 800.f;
        walls[0].active = 1;
        // Bottom 2
        walls[1].pos = CP_Vector_Set(1445, 955);
        walls[1].height = 50.f;
        walls[1].width = 800.f;
        walls[1].active = 1;
        // Top
        walls[2].pos = CP_Vector_Set(475, 75);
        walls[2].height = 50.f;
        walls[2].width = 800.f;
        walls[2].active = 1;
        // Top2
        walls[3].pos = CP_Vector_Set(1445, 75);
        walls[3].height = 50.f;
        walls[3].width = 800.f;
        walls[3].active = 1;
        // Left
        walls[4].pos = CP_Vector_Set(75, 540);
        walls[4].height = 980.f;
        walls[4].width = 50.f;
        walls[4].active = 1;
        // Right
        walls[5].pos = CP_Vector_Set(1845, 540);
        walls[5].height = 980.f;
        walls[5].width = 50.f;
        walls[5].active = 1;

        // Walls in the scalar
        

        walls[6].pos = CP_Vector_Set(225, 540);
        walls[6].height = 50.f;
        walls[6].width = 250.f;
        walls[6].active = 1;

        walls[7].pos = CP_Vector_Set(1695, 540);
        walls[7].height = 50.f;
        walls[7].width = 250.f;
        walls[7].active = 1;

        walls[8].pos = CP_Vector_Set(960, 725);
        walls[8].height = 50.f;
        walls[8].width = 1000.f;
        walls[8].active = 1;

        /*walls[9].pos = CP_Vector_Set(275, 955);
        walls[9].height = 50.f;
        walls[9].width = 350.f;
        walls[9].active = 1;

        walls[10].pos = CP_Vector_Set(1645, 955);
        walls[10].height = 50.f;
        walls[10].width = 350.f;
        walls[10].active = 1;*/

        // top
        walls[11].pos = CP_Vector_Set(960, 325);
        walls[11].height = 50.f;
        walls[11].width = 1000.f;
        walls[11].active = 1;
        
    }

    player->go.faceDir = 1;
    g_scaledDt = 0.f;
    
    // MESSAGE FUNC POINTER INIT
    {
        g_messenger.messages[MSG_SPAWN_BULLET] = MessageSpawnBullet;
        g_messenger.messages[MSG_SPAWN_ENEMY1] = MessageSpawnEnemy;
    }

    playerCount = 1;
    CP_Settings_TextSize(50.0f);
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

    //Bullet count for UI stuff
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
    GAMEOVER = 0;
    game_start_text_counter = 0;
    combocounter = 0;
    combocounter_timer = 0.f;
    reset_timer(60.0f); // reset timer
    highest_combo = 0;
}

void game_update(void)
{
    // Clean Render
    CP_Graphics_ClearBackground(CP_Color_Create(150, 150, 150, 255));
    // Update scaled dt
    g_scaledDt = CP_System_GetDt();
    if (scalar + (combocounter / 50.f) > MAX_DT_SCALE)
        g_scaledDt *= MAX_DT_SCALE;
    else
        g_scaledDt *= scalar + (combocounter / 50.f);

    //printf("play pos %.2f,%.2f\n", player->go.pos.x, player->go.pos.y);
#pragma region UPDATE
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
    {
    for (int i = 0; i < MAX_WEAPON_BOX; ++i) {
        if (weapon_boxes[i].go.active)
            weapon_boxes[i].Update(&weapon_boxes[i]);
    }
    // Spawn weapon box
    if (gamestart)
    {
        spawnWeaponBoxTimer -= g_scaledDt;
    }

    if (spawnWeaponBoxTimer <= 0)
    {
        spawnWeaponBoxTimer = WEAPON_BOX_SPAWN_TIME;

        CP_BOOL lessThanMaxBox = 0;
        // Check if there is alr 3 boxes
        for (int i = 0; i < MAX_WEAPON_BOX; ++i)
        {
            if (!weapon_boxes[i].go.active)
                lessThanMaxBox = 1;
        }

        if (lessThanMaxBox)
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
                curr->grounded = 0;
                break;
            }
        }
    }
    }

    // Update Enemy Lists
    UpdateEnemyList(enemies, MAX_ENEMIES);

    if (CP_Input_KeyTriggered(KEY_Q))
        CP_Engine_Terminate();

    
#pragma region ENEMY_SPAWN_STUFF
     // GameLoop stuff
    if (gamestart == 1)
    {
        difficulty_timer += 1.0f * CP_System_GetDt();
        if (difficulty_timer >= DIFFICULT_INCREMENT){
            difficulty_timer = 0.0f;
            scalar += 0.01f;
        }
        // Basic Dumb enemy spawning
        spawntimer[0] += 1.0f * CP_System_GetDt();
        if (spawntimer[0] > BASE_SPAWN_FREQUENCY) {
            spawntimer[0] = 0.0f; // spawn
            int random_pos = returnRange(1, 50);
            if (random_pos <= 25)
                SpawnEnemy(0, e_spawnPos1);
            else
                SpawnEnemy(0, e_spawnPos2);
        }
        if (scalar > 1){
            spawntimer[1] += 1.0f * CP_System_GetDt();
            if(spawntimer[1] > BASE_SPAWN_FREQUENCY + 3){
                spawntimer[1] = 0.0f;
                int random_pos = returnRange(1, 100);
                if (random_pos <= 25)
                    SpawnEnemy(1, e2_spawnPos[0].pos);
                else if (random_pos <= 50)
                    SpawnEnemy(1, e2_spawnPos[1].pos);
                else if (random_pos <= 75)
                    SpawnEnemy(1, e2_spawnPos[2].pos);
                else if (random_pos <= 100)
                    SpawnEnemy(1, e2_spawnPos[3].pos);
            }
        }
        if (scalar > 1.05f) {
            spawntimer[2] += 1.0f * CP_System_GetDt();
            if (spawntimer[2] > BASE_SPAWN_FREQUENCY + 3) {
                spawntimer[2] = 0.0f;
                int random_pos = returnRange(1, 100);
                if (random_pos <= 25)
                    SpawnEnemy(2, e2_spawnPos[0].pos);
                else if (random_pos <= 50)
                    SpawnEnemy(2, e2_spawnPos[1].pos);
                else if (random_pos <= 75)
                    SpawnEnemy(2, e2_spawnPos[2].pos);
                else if (random_pos <= 100)
                    SpawnEnemy(2, e2_spawnPos[3].pos);
            }
        }
    }
    else
    {
        if (gamestart == 0){
            if (CP_Input_KeyTriggered(KEY_SPACE)){
                gamestart = 1;
                for (int i = 0; i < MAX_TEXT_POPUP; ++i){
                    if (!(popUp[i].go.active)){
                        set_popup(&popUp[i], CP_System_GetDisplayWidth() / 2,
                            CP_System_GetDisplayHeight() / 2,
                            CP_Color_Create(255, 0, 0, 255), 
                            35, 1.0f, "START");
                        break;
                    }
                }

            }
        }
    }
#pragma endregion

    // Debug Spawn Enemy
    {
        // Debug Spawn Enemy
        if (CP_Input_KeyTriggered(KEY_EQUAL)) {
            SpawnEnemyMessage enemy;
            int random_pos = returnRange(1, 50);
            if (random_pos <= 25)
                enemy.position = e_spawnPos1;
            else
                enemy.position = e_spawnPos2;

            enemy.tracking = 0;
            g_messenger.messages[MSG_SPAWN_ENEMY1](&enemy);
        }

        if (CP_Input_KeyTriggered(KEY_MINUS)) {
            SpawnEnemy(1, e2_spawnPos[0].pos);
        }
        if (CP_Input_KeyTriggered(KEY_0)) {
            SpawnEnemy(1, e2_spawnPos[1].pos);
        }
        if (CP_Input_KeyTriggered(KEY_9)) {
            SpawnEnemy(1, e2_spawnPos[2].pos);
        }
        if (CP_Input_KeyTriggered(KEY_8)) {
            SpawnEnemy(1, e2_spawnPos[3].pos);
        }

        if (CP_Input_MouseTriggered(MOUSE_BUTTON_RIGHT))
        {
            SpawnEnemy(1, CP_Vector_Set(CP_Input_GetMouseWorldX(), CP_Input_GetMouseWorldY()));
        }
        if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT))
        {
            SpawnEnemy(2, e2_spawnPos[3].pos);
        }
    }
    
#pragma endregion
#pragma region COLLISION LOOPS
    // Collision Loops
    // Player - x
    for (int i = 0; i < playerCount; ++i)
    {
        if (!player[i].go.active)
            continue;

        // GAME OVER SET
        if ((combocounter_timer / COMBO_TIME) < 0)
        {
            GAMEOVER = 1;
            player[i].go.active = 0;
        }

        CP_BOOL player_grounded_flag = 0;

        //player out of bounds
        if (player[i].go.pos.x > CP_System_GetWindowWidth() - 100)
            player[i].go.pos.x = 100;
        else if (player[i].go.pos.x < 100)
            player[i].go.pos.x = CP_System_GetWindowWidth() - 100;

        if (player[i].go.pos.y > CP_System_GetWindowHeight() - 100)
            player[i].go.pos.y = 100;
        else if (player[i].go.pos.y < 100)
            player[i].go.pos.y = CP_System_GetWindowHeight() - 100;

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
                    //printf("WALL INTERRUPED ROLL\n");
                    player[i].currAnim = SetSpriteAnim(&player[i].animations[ANIM_PLAYER_ACTIVE_IDLE], PLAYER_ACTIVE_ANIM_SPEED);
                    player[i].state = STATE_PLAYER_ACTIVE;
                }
                else
                {
                    player[i].go.pos.x = walls[j].pos.x + walls[j].width / 2.f + player[i].go.width / 2.f;
                    //printf("WALL INTERRUPED ROLL\n");
                    player[i].currAnim = SetSpriteAnim(&player[i].animations[ANIM_PLAYER_ACTIVE_IDLE], PLAYER_ACTIVE_ANIM_SPEED);
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

        // Player - Weapon Box
        for (int j = 0; j < MAX_WEAPON_BOX; j++)
        {
            if (!weapon_boxes[j].go.active)
                continue;
            if (AABB(player[i].go, weapon_boxes[j].go))
            {
                weapon_boxes[j].go.active = 0;
                char* weapon_text = Player_RandomWeapon(&player[i]);

                for (int p = 0; p < MAX_TEXT_POPUP; ++p)
                {
                    if (!(popUp[p].go.active))
                    {
                        set_popup(&popUp[p], 
                            player->go.pos.x,
                            player->go.pos.y - player->go.height / 2.f - 10.f,
                            CP_Color_Create(255, 0, 0, 255),
                            35,
                            3.0f,
                            weapon_text);
                        break;
                    }
                }
            }
        }

        // Player - Enemy
        for (int j = 0; j < MAX_ENEMIES; j++)
        {
            if (!enemies[j].go.active)
                continue;
            if (AABB(player[i].go, enemies[j].go) && player[i].state != STATE_PLAYER_ROLLING)
            {
                enemies[j].go.active = 0;
                player[i].go.active = 0;
                GAMEOVER = 1;

                for (int p = 0; p < MAX_TEXT_POPUP; ++p)
                {
                    if (!(popUp[p].go.active))
                    {
                        set_popup(&popUp[p],
                        player->go.pos.x,
                        player->go.pos.y - player->go.height / 2.f - 10.f,
                        CP_Color_Create(255, 0, 0, 255),
                        (int)DEFAULT_FONT_SIZE,
                        3.0f,
                        "DIED");
                        break;
                    }
                }
            }
        }
    }

    // Bullet - x
    for (int i = 0; i < MAX_BULLETS; ++i)
    {
        if (!bullets[i].go.active)
            continue;

        if (bullets[i].friendly == 0){
            if (AABB(player[0].go, bullets[i].go)) {
                bullets[i].go.active = 0;
                player[0].go.active = 0;
                GAMEOVER = 1;

                for (int p = 0; p < MAX_TEXT_POPUP; ++p)
                {
                    if (!(popUp[p].go.active))
                    {
                        set_popup(&popUp[p],
                            player->go.pos.x,
                            player->go.pos.y - player->go.height / 2.f - 10.f,
                            CP_Color_Create(255, 0, 0, 255),
                            (int)DEFAULT_FONT_SIZE,
                            3.0f,
                            "DIED");
                        break;
                    }
                }
            }
        }

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

        // Bullet - Enemy
        for (int j = 0; j < MAX_ENEMIES; j++)
        {
            if (!enemies[j].go.active)
                continue;
            if (AABB(bullets[i].go, enemies[j].go))
            {
                bullets[i].collide_pos = bullets[i].go.pos;
                COLLISION_DIRECTION collision_dir = AABB_Direction(bullets[i].go, enemies[j].go);
                if (collision_dir == COLLISION_TOP)
                    bullets[i].collide_pos.y = enemies[j].go.pos.y - enemies[j].go.height / 2.f - bullets[i].go.height / 2.f;
                else if (collision_dir == COLLISION_BOTTOM)
                    bullets[i].collide_pos.y = enemies[j].go.pos.y + enemies[j].go.height / 2.f + bullets[i].go.height / 2.f;
                else if (collision_dir == COLLISION_LEFT)
                    bullets[i].collide_pos.x = enemies[j].go.pos.x - enemies[j].go.width / 2.f - bullets[i].go.width / 2.f;
                else
                    bullets[i].collide_pos.x = enemies[j].go.pos.x + enemies[j].go.width / 2.f + bullets[i].go.width / 2.f;

                bullets->Destroy(&bullets[i]);

                if (!EnemyTakeDamage(&enemies[j], 1))
                    killconfirmed();
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
    // ENEMY 1
    for (int j = 0; j < MAX_ENEMIES; ++j)
    {
        if (!enemies[j].go.active)
            continue;
        EnemytoWallCollision(&enemies[j], walls);
        if (playerPrevPlatform != NULL)
        {
            //EnemyPathing(&enemies[j], walls, &player, playerPrevPlatform, MAX_WALLS);
            //EnemyPathing(&enemies[j], ai_nodes, &player, playerPrevPlatform, MAX_PATHFINDING_NODES);
            //EnemyPathing3(&enemies[j], ai_nodes, &player, playerPrevPlatform, MAX_PATHFINDING_NODES, walls, e2_spawnPos);
            EnemyPathing4(&enemies[j], &player, playerPrevPlatform, MAX_PATHFINDING_NODES, walls, ai_nodes, e2_spawnPos);
        }

    }

    if (highest_combo < combocounter)
    {
        highest_combo = combocounter;
    }
    

#pragma endregion

    //----------------------------------------------------------------------------------------------------------------------
    // Render stuff here
    //----------------------------------------------------------------------------------------------------------------------
#pragma region RENDER
    

    CP_Settings_Stroke(CP_Color_Create(0, 0, 0, 255));
    // Background
    //CP_Image_Draw(backgroundSprite, 0.f, 0.f, 1920, 1800, 255);

    CP_Matrix camTransform = CP_Matrix_Translate(cameraPos);
    CP_Settings_ApplyMatrix(camTransform);
    // Bullets
    for (int i = 0; i < MAX_BULLETS; ++i) {
        if (bullets[i].go.active)
        {
            CP_Settings_Fill(bullets[i].color);
            CP_Graphics_DrawCircle(bullets[i].go.pos.x, bullets[i].go.pos.y, bullets[i].go.height);
        }
    }

    // Player 
    const CP_Color playerColor = CP_Color_Create(100, 0, 0, 255);
    for (int i = 0; i < playerCount; ++i) {
        if (player[i].go.active)
        {
            CP_Settings_Fill(playerColor);
            // Drawing image instead
            RenderSpriteAnim(&player[i].currAnim, sprites[SPRITE_PLAYER], player[i].go.pos.x, 
                player[i].go.pos.y, player[i].go.width, player[i].go.height, 255);
             
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

    // Walls
    const CP_Color wallColor = CP_Color_Create(120, 120, 120,  255);
    CP_Settings_Fill(wallColor);
    for (int i = 0; i < MAX_WALLS; ++i) {
        if (walls[i].active)
            CP_Graphics_DrawRect(walls[i].pos.x, walls[i].pos.y, walls[i].width, walls[i].height);
    }

    /*---------*/
    // Enemies //
    /*---------*/
    // Enemy_1
    {
        const CP_Color enemyColor = CP_Color_Create(125, 181, 130, 255);
        const CP_Color enemyColor2 = CP_Color_Create(20, 227, 199, 255);
        const CP_Color enemyColor3 = CP_Color_Create(20, 105, 55, 255);

        for (int i = 0; i < MAX_ENEMIES; ++i)
        {
            if (!enemies[i].go.active)
                continue;
            /*if (enemies[i].tracking)
            {
                CP_Settings_Fill(enemyColor2);
            }
            else
            {
                CP_Settings_Fill(enemyColor);
            }*/
            switch (enemies[i].enemytype)
            {
            case 0:
                CP_Settings_Fill(enemyColor);
                CP_Settings_Tint(CP_Color_Create(255, 0, 0, (int)enemies[i].redTintVal));
                RenderSpriteAnim(&enemies[i].currAnim, sprites[SPRITE_ENEMY_1 + enemies[i].type], enemies[i].go.pos.x,
                    enemies[i].go.pos.y, enemies[i].go.width, enemies[i].go.height, 255);
                break;
            case 1:
                CP_Settings_Fill(enemyColor2);
                CP_Settings_Tint(CP_Color_Create(255, 0, 0, (int)enemies[i].redTintVal));
                RenderSpriteAnim(&enemies[i].currAnim, sprites[SPRITE_ENEMY_2 + enemies[i].type], enemies[i].go.pos.x,
                    enemies[i].go.pos.y, enemies[i].go.width, enemies[i].go.height, 255);
                break;
            case 2:
                CP_Settings_Fill(enemyColor3);
                CP_Settings_Tint(CP_Color_Create(255, 0, 0, (int)enemies[i].redTintVal));
                RenderSpriteAnim(&enemies[i].currAnim, sprites[SPRITE_ENEMY_3 + enemies[i].type], enemies[i].go.pos.x,
                    enemies[i].go.pos.y, enemies[i].go.width, enemies[i].go.height, 255);
                break;
            default:
                break;
            }


            //CP_Graphics_DrawCircle(enemies[i].go.pos.x, enemies[i].go.pos.y, enemies[i].go.height);
            //CP_Graphics_DrawCircle(enemies[i].go.pos.x, enemies[i].go.pos.y + enemies[i].go.height + 20, 5);
            //CP_Settings_Tint(CP_Color_Create(255, 0, 0, (int)enemies[i].redTintVal));
            //RenderSpriteAnim(&enemies[i].currAnim, sprites[SPRITE_ENEMY_1 + enemies[i].type], enemies[i].go.pos.x,
            //    enemies[i].go.pos.y, enemies[i].go.width, enemies[i].go.height, 255);
            //CP_Settings_Stroke(CP_Color_Create(255, 0, 0, 255));
            //if (enemies[i].enemy_shortestNode)
                //CP_Graphics_DrawLine(enemies[i].go.pos.x, enemies[i].go.pos.y + enemies[i].go.height * 0.5f ,
                    //enemies[i].enemy_shortestNode->pos.x, enemies[i].enemy_shortestNode->pos.y);

            // Remove tint from enemy
            CP_Settings_NoTint();
        }
    }

    

    // Weapon Boxes
    for (int i = 0; i < MAX_WEAPON_BOX; ++i) {
        if (weapon_boxes[i].go.active)
        {
            CP_Settings_Fill(weapon_boxes[i].color);
            CP_Graphics_DrawRect(weapon_boxes[i].go.pos.x, weapon_boxes[i].go.pos.y, weapon_boxes[i].go.width, weapon_boxes[i].go.height);
        }
    }

    // Render AI Pathfinding nodes
    // Render enemy spawn location
    /*if (shownodes)
    {
        const CP_Color nodeColor = CP_Color_Create(118, 78, 191, 255);
        CP_Settings_Fill(nodeColor);
        for (int i = 0; i < MAX_PATHFINDING_NODES; ++i)
        {
            CP_Graphics_DrawRect(ai_nodes[i].pos.x, ai_nodes[i].pos.y, 20, 20);
        }
    
        const CP_Color spawnloc = CP_Color_Create(33, 255, 185, 255);
        CP_Settings_Fill(spawnloc);
        for (int i = 0; i < 4; ++i)
        {
            CP_Graphics_DrawRect(e2_spawnPos[i].pos.x,
                e2_spawnPos[i].pos.y, 20, 20);
        }
    }
    */
    CP_Settings_ResetMatrix();
    // UI ELEMENTS
    
    //CP_Settings_TextSize(DEFAULT_FONT_SIZE);
    //CP_Settings_Fill(DEFAULT_FONT_COLOR);
    //if (gamestart)
    //{
    //    update_timer();
    //}
 

    float oznoalpha = updateOznometerFade(255, &combocounter_timer, COMBO_TIME);
    updateComboCounterTimer(&combocounter,&combocounter_timer, COMBO_TIME_DEDUCTION, COMBO_TIME);

    printComboCounter(CP_Vector_Set(CP_System_GetWindowWidth() / 2.f - 0.2f, 40), 52, 0, 0, 0, oznoalpha);
    printComboCounter(CP_Vector_Set(CP_System_GetWindowWidth() / 2.f, 40), 50, 194, 46, 19, oznoalpha);
    
    CP_Vector barpos = CP_Vector_Set(CP_System_GetWindowWidth() * 0.20F, 
        CP_System_GetWindowHeight() * 0.015F);
    if ((combocounter_timer / COMBO_TIME) <= 0)
        CP_Settings_StrokeWeight(0.f);
    printComboCountdownTimer(CP_Vector_Set(CP_System_GetWindowWidth() / 2.f - barpos.x / 2.f, 64),
        CP_Vector_Set((float)(combocounter_timer / COMBO_TIME) * barpos.x
        , barpos.y), 194, 46, 19, oznoalpha);
    CP_Settings_StrokeWeight(3.f);
    char combocountertxt[5];
    sprintf_s(combocountertxt, sizeof((int)combocounter), "%d", combocounter);
    CP_Settings_TextSize(100.f);
    CP_Font_DrawText(combocountertxt, CP_System_GetWindowWidth() / 2.f, 120);

    // OZNOLA METER
    if (CP_Input_KeyTriggered(KEY_F11)) // debug
    {
        // lowkey got lazy u want add time just call this func ea time the mob die
        killconfirmed();
    }

    
    if (!gamestart)
    {
        gamestart_timer -= CP_System_GetDt();
        if (gamestart_timer <= 0.0f)
        {
            for (int q = 0; q < playerCount; ++q)
            {
                if (!player[q].go.active)
                    continue;

                for (int i = 0; i < MAX_TEXT_POPUP; ++i)
                {
                    if (!(popUp[i].go.active))
                    {
                        printf("%d", game_start_text_counter);
                        set_popup(&popUp[i],
                            player[q].go.pos.x,
                            player[q].go.pos.y - 50.0f,
                            CP_Color_Create(0, 0, 0, 200),
                            50,
                            3.0f,
                            gamestart_text[game_start_text_counter]);
                        ++game_start_text_counter;
                        if (game_start_text_counter >= MAX_GAMESTART_TEXT)
                        {
                            for (int x = 0; x < MAX_GAMESTART_TEXT; ++x)
                            {
                                gamestart_text[x] = "Press Spacebar to START!";
                            }
                            game_start_text_counter = 0;
                        }
                        
                        break;
                    }
                }
            }
            gamestart_timer = 4.0f;
        }
    }

    if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT))
    {
        for (int i = 0; i < MAX_TEXT_POPUP; ++i)
        {
            if (!(popUp[i].go.active))
            {
                set_popup(&popUp[i], CP_Input_GetMouseX(), CP_Input_GetMouseY(), CP_Color_Create(255, 0, 0, 255), 50, 3.0f, "Why U Click?");
                break;
            }
        }
    }

    update_bullet_bar(player[0].currAmmo, player[0].maxAmmo);
    for (int i = 0; i < MAX_TEXT_POPUP; ++i)
    {
        update_popup(&popUp[i]);
        draw_popup(&popUp[i]);
    }

    //GAME OVER
    if (GAMEOVER)
    {
        game_over_popup(highest_combo);
    }
#pragma endregion
}

void game_exit(void)
{
    free(enemies);
}
