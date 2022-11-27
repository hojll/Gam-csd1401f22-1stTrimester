//All content © 2021 DigiPen (SINGAPORE) Corporation, all rights reserved.
//Joel Loh	    loh.j@digipen.edu
#include "e_text_popup.h"

/*TO CREATE A POPUP TEXT
//////////////////////////////////////////
for (int i = 0; i < MAX_TEXT_POPUP; ++i)
{
    if (!(popUp[i].go.active))
    {
        set_popup(	&popUp[i],
					Position of X, Position of Y (Subtract your offset),
					CP_Color_Create(255, 0, 0, 255), // COLOR OF THE TEXT
					(int)DEFAULT_FONT_SIZE, // THE FONT SIZE 
					3.0f, // THE DURATION OF THE POPUP
					"Pickup/Damage"); // THE TEXT YOU WANT TO RENDER
        break;
    }
}
//////////////////////////////////////////*/
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
	CP_Settings_TextSize((float)text->font_size);
	if (text->go.active)
	{
		CP_Font_DrawText(text->text, text->go.pos.x, text->go.pos.y);
	}
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Settings_TextSize(100.0f);
}