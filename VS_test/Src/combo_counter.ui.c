#include "combo_counter_ui.h"
#include "easing.h"

void printComboCounter(CP_Vector pos, int scale, int r, int g, int b, int alpha)
{
	// FONT COLOR
	CP_Settings_Fill(CP_Color_Create(r, g, b, alpha));
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Settings_TextSize((float)scale);
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
	*timer = addedtime;
	if (*timer >= addedtime)
	{
		*timer = addedtime;
	}
}

void updateComboCounterTimer(int* count, double *timer, double deduct, double combotime)
{
	//printf("timer %lf \n", *timer);

	if (*timer > 0)
	{
		*timer -= deduct * CP_System_GetDt();
		if (*timer <= 0)
		{
			*count = 0;
		}
	}
}

float updateOznometerFade(float alpha, double* timer, double combotime)
{
	float ratio = 0;
	if (*timer > 0)
	{	
		ratio = (float)(*timer / combotime);
		return alpha * ratio;
	}
	return 0;
}
