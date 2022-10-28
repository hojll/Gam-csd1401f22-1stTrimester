#include "e_text_popup.h"

void set_popup(TextPopUp *text, float x, float y, CP_Color color, int font_size, float duration, char *string)
{
	text->go.active = 1;
	text->go.pos.x = x;
	text->go.pos.y = y;
	text->color = color;
	text->font_size = font_size;
	text->go.timer = duration;
	text->text = string;
	text->go.vel.y = 3;
}

void update_popup(TextPopUp *text)
{
	int alpha_decrement = (int)(255 / (60 * text->go.timer));
	if (text->go.timer <= 0)
	{
		text->go.active = 0;
	}

	if (text->go.active)
	{
		if (text->color.a > alpha_decrement)
		{
			text->color.a -= alpha_decrement;
		}

		text->go.pos.y -= text->go.vel.y;

		if (text->go.vel.y >= 0.5f)
		{
			text->go.vel.y -= 0.1f;
		}
		
		text->go.timer -= CP_System_GetDt();
	}
}

void draw_popup(TextPopUp* text)
{
	CP_Settings_Fill(text->color);
	CP_Settings_TextSize(35.0f);
	if (text->go.active)
	{
		CP_Font_DrawText(text->text, text->go.pos.x, text->go.pos.y);
	}
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Settings_TextSize(100.0f);
}