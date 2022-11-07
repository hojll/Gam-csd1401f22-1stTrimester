#include "combo_counter_ui.h"
#include "easing.h"

void printComboCounter(CP_Vector pos, int scale, int r, int g, int b, int alpha)
{
	// FONT COLOR
	CP_Settings_Fill(CP_Color_Create(r, g, b, alpha));
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Settings_TextSize(scale);
	CP_Font_DrawText("Oznometer", pos.x, pos.y);
}

void printComboCountdownTimer(CP_Vector pos, CP_Vector wh, int r, int g, int b, int alpha)
{
	CP_Settings_Fill(CP_Color_Create(r, g, b, alpha));
	CP_Settings_RectMode(CP_POSITION_CORNER);
	CP_Graphics_DrawRect(pos.x, pos.y, wh.x, wh.y);
}


void addcombotime(double* timer, double addedtime)
{
	*timer += addedtime;
	if (*timer >= addedtime)
	{
		*timer = addedtime;
	}
}

void updateComboCounterTimer(double *timer, double deduct, double combotime)
{
	if (*timer > 0)
	{
		*timer -= deduct * CP_System_GetDt();
	}
}