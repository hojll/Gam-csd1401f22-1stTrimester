#include "CProcessing.h"
#include "restart.h"
#define ALPHA_DECREMENT 50

static float y_position[3], x_position[2];
static int alpha[3] = { 255 , 255, 255 };

static int selector_count;
static char selected;
static float timer;
static float button_movement;
static char game_over_initialized = 0;

void main_menu_init(void);
void main_menu_update(void);
void main_menu_exit(void);
void game_init(void);
void game_update(void);
void game_exit(void);

void initialize_value(void)
{
	if (!game_over_initialized)
	{
		x_position[0] = 0;
		x_position[1] = (float)CP_System_GetWindowWidth() * 0.5f;
		y_position[0] = (float)CP_System_GetWindowHeight() * 0.3f;
		y_position[1] = (float)CP_System_GetWindowHeight() * 0.6f;
		y_position[2] = (float)CP_System_GetWindowHeight() * 0.75f;
		selector_count = 0;
		button_movement = 0.0f;
		selected = 0;
		timer = 0.5f;
		game_over_initialized = 1;
		alpha[0] = 255, alpha[1] = 255, alpha[2] = 255;
	}
}

int arrow_offset(void)
{
	if (selector_count == 0)
	{
		return 190;
	}
	if (selector_count == 1)
	{
		return 250;
	}
	return 0;
}

void game_over_popup(void)
{
	initialize_value();
	if (CP_Input_KeyTriggered(KEY_DOWN) || CP_Input_KeyTriggered(KEY_S))
	{
		if (!selected)
		{
			selector_count += 1;
			if (selector_count == 2)
			{
				selector_count = 0;
			}
		}
	}

	if (CP_Input_KeyTriggered(KEY_UP) || CP_Input_KeyTriggered(KEY_W))
	{
		if (!selected)
		{
			selector_count -= 1;
			if (selector_count == -1)
			{
				selector_count = 1;
			}
		}
	}

	if ((CP_Input_KeyTriggered(KEY_ENTER) || CP_Input_KeyTriggered(KEY_D) || CP_Input_KeyTriggered(KEY_RIGHT) || CP_Input_KeyTriggered(KEY_SPACE)))
	{
		// restart
		if (selector_count == 0)
		{
			selected = 1;
			// Go to restart
		}
		// main menu
		if (selector_count == 1)
		{
			selected = 1;
			// Go to Main Menu
		}
	}

	if(selected)
	{
		timer -= CP_System_GetDt();
		if (selector_count == 0)
		{
			alpha[2] -= ALPHA_DECREMENT;
			if (timer <= 0.0f)
			{
				game_over_initialized = 0;
				CP_Engine_SetNextGameState(restart_init, restart_update, restart_exit);
				
			}
		}
		if (selector_count == 1)
		{
			alpha[1] -= ALPHA_DECREMENT;
			if (timer <= 0.0f)
			{
				game_over_initialized = 0;
				CP_Engine_SetNextGameState(main_menu_init, main_menu_update, main_menu_exit);
				//exit(0);
			}
		}
	}

	CP_Settings_RectMode(CP_POSITION_CENTER);
	
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Settings_TextSize(150.0f);

	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 150));
	CP_Graphics_DrawRect((float)CP_System_GetWindowWidth() * 0.5f, (float)CP_System_GetWindowHeight() * 0.5f, (float)CP_System_GetWindowWidth(), (float)CP_System_GetWindowHeight());

	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
	CP_Font_DrawText("GAME OVER", x_position[1], y_position[0]);

	CP_Settings_TextSize(100.0f);
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, alpha[1]));
	CP_Font_DrawText("RESTART", x_position[1], y_position[1]);

	CP_Settings_Fill(CP_Color_Create(255, 255, 255, alpha[2]));
	CP_Font_DrawText("MAIN MENU", x_position[1], y_position[2]);

	// Triangle Selector
	CP_Graphics_DrawTriangle(x_position[1] - arrow_offset(), y_position[selector_count + 1] + 3.0f, x_position[1] - 30.0f - arrow_offset(), y_position[selector_count + 1] - 10.0f + 3.0f, x_position[1] - 30.0f - arrow_offset(), y_position[selector_count + 1] + 10.0f + 3.0f);
	CP_Settings_Stroke(CP_Color_Create(255, 255, 255, 255));
}