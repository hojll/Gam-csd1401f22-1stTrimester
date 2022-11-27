#include "CProcessing.h"
#include "e_banner.h"

void create_banner(Banner *banner, char *text)
{
	banner->go.active = 1;
	banner->go.timer = 1.5f;
	banner->text = text;
	banner->color = CP_Color_Create(0, 0, 0, 255);
}

void update_banner(Banner* banner)
{
	int alpha_decrement = (int)(255 / (60 * banner->go.timer));
	if (banner->go.timer <= 0)
	{
		banner->go.active = 0;
	}

	if (banner->go.timer < 0.5f)
	{
		if (banner->color.a > alpha_decrement)
		{
			banner->color.a -= alpha_decrement;
		}
	}

	if (banner->go.active)
	{
		banner->go.timer -= CP_System_GetDt();
	}
}

void draw_banner(Banner* banner)
{
	
	CP_Settings_TextSize(100.0f);
	CP_Settings_Stroke(CP_Color_Create(0, 0, 0, banner->color.a - 120));
	if (banner->go.active)
	{
		CP_Settings_Fill(CP_Color_Create(255, 255, 255, banner->color.a - 120));
		CP_Graphics_DrawRect(CP_System_GetWindowWidth() * 0.5f, (float)CP_System_GetWindowHeight() * 0.4f, (float)CP_System_GetWindowWidth(), 150.0f);
		CP_Settings_Fill(banner->color);
		CP_Font_DrawText(banner->text, CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.4f);
	}
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Settings_TextSize(50.0f);
}