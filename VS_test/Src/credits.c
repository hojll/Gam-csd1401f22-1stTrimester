//All content © 2021 DigiPen (SINGAPORE) Corporation, all rights reserved.
//Joel Loh	    loh.j@digipen.edu
#include "CProcessing.h"

void main_menu_init(void);
void main_menu_update(void);
void main_menu_exit(void);

static float ypos[7];
static float xpos[6];
static char next;
static float delay = 1000.0f;
static char selector;
static float ui_xpos[2];
static float ui_offset;
static float executive_name_starter = 0; 
static float executive_offset = 45.0f;
static int back_alpha = 255;
static int next_alpha = 255;
static CP_Sound SoundFx[3];

void title_format(void) {
	// FONT COLOR
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));

	// FONT ALIGNMENT
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

	// FONT SIZE
	CP_Settings_TextSize(180.0f);
}

// Moves the texts left.
void next_page(void)
{
	if (delay > 0)
	{
		delay -= 7.5f;
	}
	
	if (xpos[1] > CP_System_GetWindowWidth() * 0.5f)
	{
		for (int i = 0; i < 6; ++i)
		{
			xpos[i] -= 20.0f + delay;
		}
	}
}

// Moves the texts right.
void previous_page(void)
{
	if (delay < 1000)
	{
		delay += 7.5f;
	}

	if (xpos[0] < CP_System_GetWindowWidth() * 0.5f)
	{
		for (int i = 0; i < 6; ++i)
		{
			xpos[i] += 20.0f + delay;
		}
	}
}

// Initialize the scene.
void credits_init(void)
{
	ypos[0] = CP_System_GetWindowHeight() * 0.15f; // CREDITS [ Title]
	ypos[1] = CP_System_GetWindowHeight() * 0.325f; // TEAM MEMBERS 
	ypos[2] = CP_System_GetWindowHeight() * 0.425f; // NAME
	ypos[3] = CP_System_GetWindowHeight() * 0.50f; // NAME
	ypos[4] = CP_System_GetWindowHeight() * 0.65f; // INSTRUCTORS
	ypos[5] = CP_System_GetWindowHeight() * 0.75f; // NAME
	ypos[6] = CP_System_GetWindowHeight() * 0.9f; // Copyright

	xpos[0] =  CP_System_GetWindowWidth() * 0.5f;
	xpos[1] =  CP_System_GetWindowWidth() * 1.5f;

	xpos[2] = CP_System_GetWindowWidth() * 0.49f;
	xpos[3] = CP_System_GetWindowWidth() * 0.51f;
	xpos[4] = CP_System_GetWindowWidth() * 1.49f;
	xpos[5] = CP_System_GetWindowWidth() * 1.51f;

	next = 0;
	delay = 150.0f;
	selector = 0;
	ui_offset = CP_System_GetWindowWidth() * 0.02f;
	ui_xpos[0] = CP_System_GetWindowWidth() * 0.05f + ui_offset;
	ui_xpos[1] = CP_System_GetWindowWidth() * 0.05f;
	executive_name_starter = CP_System_GetWindowHeight() * 0.57f;

	SoundFx[0] = CP_Sound_Load("./Assets/SFX/Hello(Menu_Welcome).mp3");
	SoundFx[1] = CP_Sound_Load("./Assets/SFX/Swish(Credit_Next).mp3");
	SoundFx[2] = CP_Sound_Load("./Assets/SFX/Swoosh(Credit_Previous).mp3");

	CP_Sound_Play(SoundFx[0]);
}

// Updates the scene.
void credits_update(void)
{
	// Quits the scene when Q is pressed.
	if (CP_Input_KeyTriggered(KEY_Q))
		CP_Engine_Terminate();

#pragma region SELECTOR_LOOPBACK
	if (selector > 1)
	{
		selector = 0;
	}
	if (selector < 0)
	{
		selector = 1;
	}
#pragma endregion

#pragma region POSITION_SETTING
	if (xpos[1] < CP_System_GetWindowWidth() * 0.501f && next == 2)
	{
		next = 1;
		//delay = 0.0f;
		xpos[0] = CP_System_GetWindowWidth() * -0.5f;
		xpos[1] = CP_System_GetWindowWidth() * 0.5f;

		xpos[2] = CP_System_GetWindowWidth() * -0.49f;
		xpos[3] = CP_System_GetWindowWidth() * -0.51f;
		xpos[4] = CP_System_GetWindowWidth() * 0.49f;
		xpos[5] = CP_System_GetWindowWidth() * 0.51f;
	}

	if (xpos[0] > CP_System_GetWindowWidth() * 0.499f && next == -1)
	{
		next = 0;
		//delay = 1000.0f;
		xpos[0] = CP_System_GetWindowWidth() * 0.5f;
		xpos[1] = CP_System_GetWindowWidth() * 1.5f;

		xpos[2] = CP_System_GetWindowWidth() * 0.49f;
		xpos[3] = CP_System_GetWindowWidth() * 0.51f;
		xpos[4] = CP_System_GetWindowWidth() * 1.49f;
		xpos[5] = CP_System_GetWindowWidth() * 1.51f;
	}
#pragma endregion

#pragma region FUNCTION_CALL
	if (next == 2)
	{
		next_page();
	}

	if (next == -1)
	{
		previous_page();
	}
#pragma endregion

#pragma region BUTTON_SELECTED 
	if (selector == 0)
	{
		back_alpha = 255;
		next_alpha = 120;
		CP_Graphics_DrawTriangle(CP_System_GetWindowWidth() * 0.05f, ypos[3] + 3.0f, CP_System_GetWindowWidth() * 0.05f - 30.0f, ypos[3] - 10.0f + 3.0f, CP_System_GetWindowWidth() * 0.05f - 30.0f, ypos[3] + 10.0f + 3.0f);
		ui_xpos[0] = CP_System_GetWindowWidth() * 0.05f + ui_offset;
		ui_xpos[1] = CP_System_GetWindowWidth() * 0.05f;

		if (CP_Input_KeyTriggered(KEY_SPACE) || CP_Input_KeyTriggered(KEY_ENTER))
		{
			CP_Engine_SetNextGameState(main_menu_init, main_menu_update, main_menu_exit);
		}
	}
	if (selector == 1)
	{
		back_alpha = 120;
		next_alpha = 255;
		CP_Graphics_DrawTriangle(CP_System_GetWindowWidth() * 0.05f, ypos[3] + 100.f + 3.0f, CP_System_GetWindowWidth() * 0.05f - 30.0f, ypos[3] + 100.f - 10.0f + 3.0f, CP_System_GetWindowWidth() * 0.05f - 30.0f, ypos[3] + 100.f + 10.0f + 3.0f);
		ui_xpos[0] = CP_System_GetWindowWidth() * 0.05f;
		ui_xpos[1] = CP_System_GetWindowWidth() * 0.05f + ui_offset;
		
		if (CP_Input_KeyTriggered(KEY_SPACE) || CP_Input_KeyTriggered(KEY_ENTER))
		{
			if (next == -1 || next == 0)
			{
				CP_Sound_Play(SoundFx[1]);
				next = 2;
			}	
			else if (next == 1 || next == 2)
			{
				CP_Sound_Play(SoundFx[2]);
				next = -1;
			}
		}
	}
#pragma endregion

#pragma region USER_INPUT
	if (CP_Input_KeyTriggered(KEY_W) || CP_Input_KeyTriggered(KEY_UP))
	{
		--selector;
	}
	if (CP_Input_KeyTriggered(KEY_S) || CP_Input_KeyTriggered(KEY_DOWN))
	{
		++selector;
	}
#pragma endregion

#pragma region DRAWING_OF_TEXT
	CP_Graphics_ClearBackground(CP_Color_Create(20, 5, 5, 255));
	// draw texts n stuff
	title_format();
	CP_Font_DrawText("CREDITS", CP_System_GetWindowWidth() * 0.5f, ypos[0]);

	CP_Settings_TextSize(100.0f);
	CP_Font_DrawText("TEAM  MEMBERS", xpos[0], ypos[1]);
	
	CP_Settings_TextSize(60.0f);

	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_RIGHT, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Font_DrawText("RYAN HO", xpos[2], ypos[2]);
	CP_Font_DrawText("JOEL LOH", xpos[2], ypos[3]);

	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Font_DrawText("ALVIN YEO", xpos[3], ypos[2]);
	CP_Font_DrawText("ALONZO NALPON", xpos[3], ypos[3]);

	CP_Settings_Fill(CP_Color_Create(255, 255, 255, back_alpha));
	CP_Font_DrawText("BACK", ui_xpos[0], ypos[3]);
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, next_alpha));
	if (next < 1)
	{
		CP_Font_DrawText("NEXT", ui_xpos[1], ypos[3] + 100.f);
	}
	if (next > 0)
	{
		CP_Font_DrawText("PREVIOUS", ui_xpos[1], ypos[3] + 100.f);
	}
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));


	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Settings_TextSize(80.0f);
	CP_Font_DrawText("INSTRUCTORS", xpos[0], ypos[4]);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_RIGHT, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Settings_TextSize(50.0f);
	CP_Font_DrawText("CHEN DING XIANG", xpos[2], ypos[5]);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Font_DrawText("GERALD WONG", xpos[3], ypos[5]);

	//NEXT TAB
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Settings_TextSize(80.0f);
	CP_Font_DrawText("PRESIDENT", xpos[1], ypos[1]);
	
	CP_Font_DrawText("EXECUTIVES", xpos[1], ypos[3] + 10.0f);

	CP_Settings_TextSize(50.0f);
	CP_Font_DrawText("CLAUDE COMAIR", xpos[1], ypos[2] - 25.0f);

	CP_Settings_TextSize(50.0f);
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_RIGHT, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Font_DrawText("JASON CHU", xpos[4], executive_name_starter);
	CP_Font_DrawText("MICHAEL GATS", xpos[4], executive_name_starter + 1 * executive_offset);
	CP_Font_DrawText("RAYMOND YAN", xpos[4], executive_name_starter + 2 * executive_offset);
	CP_Font_DrawText("PRASANNA GHALI", xpos[4], executive_name_starter + 3 * executive_offset);
	CP_Font_DrawText("DR ERIK MOHRMANN", xpos[4], executive_name_starter + 4 * executive_offset);
	CP_Font_DrawText("ANGELA KUGLER", xpos[4], executive_name_starter + 5 * executive_offset);
	CP_Font_DrawText("BEN ELLINGER", xpos[4], executive_name_starter + 6 * executive_offset);

	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Font_DrawText("CHRISTOPHER COMAIR", xpos[5], executive_name_starter);
	CP_Font_DrawText("MICHELE COMAIR", xpos[5], executive_name_starter + 1 * executive_offset);
	CP_Font_DrawText("SAMIR ABOU SAMRA", xpos[5], executive_name_starter + 2 * executive_offset);
	CP_Font_DrawText("JOHN BAUER", xpos[5], executive_name_starter + 3 * executive_offset);
	CP_Font_DrawText("MELVIN GONSALVEZ", xpos[5], executive_name_starter + 4 * executive_offset);
	CP_Font_DrawText("DR CHARLSE DUBA", xpos[5], executive_name_starter + 5 * executive_offset);
	CP_Font_DrawText("JOHNNY DEEK", xpos[5], executive_name_starter + 6 * executive_offset);
	
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Settings_TextSize(20.0f);
	CP_Font_DrawText("All content copyrighted 2022 DigiPen Institute of Technology Singapore. All Rights Reserved", CP_System_GetWindowWidth() * 0.5f, ypos[6]);
	CP_Settings_TextSize(30.0f);
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 150));
	CP_Font_DrawText("PRESS W OR S TO MOVE SELECTION", CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.93f);
	CP_Font_DrawText("PRESS SPACE OR ENTER TO SELECT", CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.96f);
#pragma endregion
}

// Exits the scene.
void credits_exit(void)
{
	for (int i = 0; i < 3; i++) {
		CP_Sound_Free(&SoundFx[i]);
	}
}