#include <stdio.h>
#include "cprocessing.h"
#include "bullet_go.h"
#include "player_go.h"
#include "gameObject.h"
#include <stdlib.h>

#define MAX_GAMEOBJECTS 500

GameObject *allGameObjects[MAX_GAMEOBJECTS];
Player* player;

void AddToGameObjectList(GameObject* gameObject)
{
    for (int i = 0; i < MAX_GAMEOBJECTS; i++)
    {
        if (allGameObjects[i] == NULL || !allGameObjects[i]->active)
        {
            allGameObjects[i] = gameObject;
            return;
        }
    }
    // If no space available, cannot be added to the list. This is because idk how to dynamically allocate more slots in c, might not be possible
}

void game_init(void)
{
    CP_System_SetWindowSize(900, 900);
    CP_Settings_RectMode(CP_POSITION_CENTER);
    //Assets/DigiPen_Singapore_WEB_RED.png

    CP_Settings_TextSize(50.0f);
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

    player = malloc(sizeof(Player));
    player->base = malloc(sizeof(GameObject));
    CreatePlayer(
        player,
        CP_Vector_Set(CP_System_GetWindowWidth() / 2.f, CP_System_GetWindowHeight() / 2.f),
        CP_Vector_Set(1, 0),
        NULL,
        30.f
    );
    player->speed = 200.f;
    AddToGameObjectList(player->base);
}

void game_update(void)
{
    // Clean Render
    CP_Graphics_ClearBackground(CP_Color_Create(150, 150, 150, 255));

    //PLAYER INPUTS
    if (CP_Input_KeyTriggered(KEY_SPACE))
    {
        Bullet* new_bullet = malloc(sizeof(Bullet));
        new_bullet->base = malloc(sizeof(GameObject));
        CreateBullet(
            new_bullet,
            player->base->pos,
            CP_Vector_Set(1, 0),
            NULL,
            10.f,
            500.f,
            1.f);
        AddToGameObjectList(new_bullet->base);
    }
    if (CP_Input_KeyDown(KEY_W))
        player->base->pos = CP_Vector_Add(player->base->pos, CP_Vector_Scale(CP_Vector_Set(0, -1), player->speed * CP_System_GetDt()));
    if (CP_Input_KeyDown(KEY_A))
        player->base->pos = CP_Vector_Add(player->base->pos, CP_Vector_Scale(CP_Vector_Set(-1, 0), player->speed * CP_System_GetDt()));
    if (CP_Input_KeyDown(KEY_S))
        player->base->pos = CP_Vector_Add(player->base->pos, CP_Vector_Scale(CP_Vector_Set(0, 1), player->speed * CP_System_GetDt()));
    if (CP_Input_KeyDown(KEY_D))
        player->base->pos = CP_Vector_Add(player->base->pos, CP_Vector_Scale(CP_Vector_Set(1, 0), player->speed * CP_System_GetDt()));

    if (CP_Input_KeyTriggered(KEY_Q))
        CP_Engine_Terminate();

    //UPDATE LOOP
    for (int i = 0; i < MAX_GAMEOBJECTS; i++)
    {
        if (allGameObjects[i] != NULL && allGameObjects[i]->active)
        {
            switch (allGameObjects[i]->type)
            {
                case PLAYER:
                    ((Player*)allGameObjects[i]->parent)->update(allGameObjects[i]->parent);
                case BULLET:
                    ((Bullet*)allGameObjects[i]->parent)->update(allGameObjects[i]->parent);
                    break;
                default:
                    break;
            }
        }
    }

    //RENDER LOOP
    for (int i = 0; i < MAX_GAMEOBJECTS; i++)
    {
        if (allGameObjects[i] != NULL && allGameObjects[i]->active)
        {
            switch (allGameObjects[i]->type)
            {
                case PLAYER:
                    CP_Settings_Fill(CP_Color_Create(0, 0, 255, 255));
                    CP_Graphics_DrawCircle(allGameObjects[i]->pos.x, allGameObjects[i]->pos.y, allGameObjects[i]->spriteScale);
                    break;
                case BULLET:
                    CP_Settings_Fill(CP_Color_Create(255, 0, 0, 255));
                    CP_Graphics_DrawCircle(allGameObjects[i]->pos.x, allGameObjects[i]->pos.y, allGameObjects[i]->spriteScale);
                    break;
                default:
                    break;
            }
        }
    }
}

void game_exit(void)
{
    for (int i = 0; i < MAX_GAMEOBJECTS; i++)
    {
        if (allGameObjects[i] != NULL && allGameObjects[i]->active)
        {
            switch (allGameObjects[i]->type)
            {
                case PLAYER:
                    ((Player*)allGameObjects[i]->parent)->destroy(allGameObjects[i]->parent);
                case BULLET:
                    ((Bullet*)allGameObjects[i]->parent)->destroy(allGameObjects[i]->parent);
                    break;
                default:
                    break;
            }

        }
    }
}
