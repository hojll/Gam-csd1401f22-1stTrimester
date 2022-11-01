#include "CProcessing.h"

float y_position[4], x_position[2], timer;

int selector_count;

void game_init(void);
void game_update(void);
void game_exit(void);

void title_text_format(void)
{
	// FONT COLOR
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));

	// FONT ALIGNMENT
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

	// FONT SIZE
	CP_Settings_TextSize(180.0f);
}

void button_text_format(void)
{
	// FONT COLOR
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));

	// FONT ALIGNMENT
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_MIDDLE);

	// FONT SIZE
	CP_Settings_TextSize(100.0f);
}

float text_offset(int offset)
{
	// 0 = play | 1 = tutorial | 2 = exit
	if (offset == selector_count)
		return x_position[0] + CP_System_GetWindowWidth() * 0.02f;
	return x_position[0];
}

float line_offset(void)
{
	float offset = 0;
	if (selector_count == 0)
	{
		offset = 250.0f;
	}
	if (selector_count == 1)
	{
		offset = 440.0f;
	}
	if (selector_count == 2)
	{
		offset = 230.0f;
	}
	return x_position[0] + offset;
}

void go_to_game(void)
{
	CP_Engine_SetNextGameState(game_init, game_update, game_exit);

}

void main_menu_init(void)
{
	CP_System_SetWindowSize(900, 900);
	CP_Settings_RectMode(CP_POSITION_CENTER);

	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

	x_position[0] = CP_System_GetWindowWidth() * 0.13f;
	x_position[1] = CP_System_GetWindowWidth() * 0.5f;
	y_position[0] = CP_System_GetWindowHeight() * 0.2f;
	y_position[1] = CP_System_GetWindowHeight() * 0.5f;
	y_position[2] = CP_System_GetWindowHeight() * 0.65f;
	y_position[3] = CP_System_GetWindowHeight() * 0.8f;
	selector_count = 0;
	timer = 20.5f;

}

void main_menu_update(void)
{
	if (CP_Input_KeyTriggered(KEY_DOWN) || CP_Input_KeyTriggered(KEY_S))
	{
		selector_count += 1;
		if (selector_count == 3)
		{
			selector_count = 0;
		}
	}

	if (CP_Input_KeyTriggered(KEY_UP) || CP_Input_KeyTriggered(KEY_W))
	{
		selector_count -= 1;
		if (selector_count == -1)
		{
			selector_count = 2;
		}
	}

	if ((CP_Input_KeyTriggered(KEY_ENTER) || CP_Input_KeyTriggered(KEY_D) || CP_Input_KeyTriggered(KEY_RIGHT)) && selector_count == 0)
	{
		go_to_game();
	}

	CP_Graphics_ClearBackground(CP_Color_Create(20, 5, 5, 255));
	// OZNOLAND
	title_text_format();
	CP_Font_DrawText("OZNOLAND", x_position[1], y_position[0]);

	button_text_format();
	// Play
	CP_Font_DrawText("PLAY", text_offset(0), y_position[1]);

	// Tutorial
	CP_Font_DrawText("TUTORIAL", text_offset(1), y_position[2]);

	// Exit
	CP_Font_DrawText("EXIT", text_offset(2), y_position[3]);

	// Triangle Selector
	CP_Graphics_DrawTriangle(x_position[0], y_position[selector_count + 1] + 3.0f, x_position[0] - 30.0f, y_position[selector_count + 1] - 10.0f + 3.0f, x_position[0] - 30.0f, y_position[selector_count + 1] + 10.0f + 3.0f);
	CP_Settings_Stroke(CP_Color_Create(255, 255, 255, 255));
	//CP_Graphics_DrawLine(line_offset(), y_position[selector_count + 1] + 3.0f, line_offset() + 5000.f, y_position[selector_count + 1] + 3.0f);
	//CP_Graphics_DrawTriangle(line_offset(), y_position[selector_count + 1] + 3.0f, line_offset() + 30.0f, y_position[selector_count + 1] - 10.0f + 3.0f, line_offset() + 30.0f, y_position[selector_count + 1] + 10.0f + 3.0f);

}

void main_menu_exit(void)
{

}