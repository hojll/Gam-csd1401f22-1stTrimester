#include <stdio.h>
#include "CProcessing.h"

float time = 100.0f;
char timer_text[5];


void set_timer(float time_in_seconds)
{
    time = time_in_seconds;
}

void update_timer(void)
{
    sprintf_s(timer_text, sizeof(timer_text), "%d", (int)time);
    CP_Font_DrawText(timer_text, CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.1f);
    time -= CP_System_GetDt();
}

void bar_system(float x, float y, float width, float height, int value, int total_value)
{
    CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
    CP_Graphics_DrawRect(x, y, width, height); //backing of bar
    float padding = 2.0f;
    width -= padding;
    float width_per_cell = (width / total_value) - padding;
    float width_w_pad = width_per_cell + padding;

    x += padding;

    float percentage = (float)value / (float)total_value;
    int amount = (int)(width / width_w_pad);
    width_per_cell = (width / amount) - padding;

    amount = amount * percentage;

    CP_Settings_Fill(CP_Color_Create(255, 255, 0, 255));
    CP_Settings_NoStroke();
    for (int i = 0; i < amount; ++i)
    {
        CP_Graphics_DrawRect(x + (width_w_pad * i), y + padding, width_per_cell, height - (2.5f * padding));
    }
    CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
}

void update_bullet_bar(int current_bullet_count, int total_bullet_count)
{
    CP_Settings_RectMode(CP_POSITION_CORNER);
    
    bar_system(CP_System_GetWindowWidth() * 0.025f, CP_System_GetWindowHeight() * 0.925f, 200.0f, 30.0f, current_bullet_count, total_bullet_count);
    CP_Settings_RectMode(CP_POSITION_CENTER);
    CP_Settings_Stroke(CP_Color_Create(0, 0, 0, 255));
}