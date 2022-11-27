//All content © 2021 DigiPen (SINGAPORE) Corporation, all rights reserved.
//Joel Loh	    loh.j@digipen.edu
#include <stdio.h>
#include <math.h>
#include "cprocessing.h"
#include "spriteData.h"

#define TIME 1
#define FULLSCREEN 0

static CP_Image logo;
static CP_Image game_logo;
static CP_Color red;
static CP_Color blank;

static int alpha;
static int inc_amt;
static char inc;
static float current_x_pos;
static float current_y_pos;
static float timer = 0.0f;
static int frame = 0;
static char game_flag = 0;
static char sound_bool = 0;
static CP_Sound SoundFx[2];

void main_menu_init(void);
void main_menu_update(void);
void main_menu_exit(void);

void splash_screen_init(void)
{
	CP_System_SetFrameRate(60.0f);
	if (FULLSCREEN)
	{
		CP_System_FullscreenAdvanced(1920, 1080);
	}
	else {
		CP_System_SetWindowSize(1920, 1080);
	}
	logo = CP_Image_Load("./Assets/Digipen_Singapore_Logo.jpg");
	game_logo = CP_Image_Load("./Assets/GAMELOGO.png");
	CP_Settings_ImageMode(CP_POSITION_CENTER);
	CP_Settings_ImageWrapMode(CP_IMAGE_WRAP_CLAMP);

	red = CP_Color_Create(255, 0, 0, 255);
	blank = CP_Color_Create(0, 0, 0, 0);

	CP_Settings_Stroke(blank);
	CP_Settings_Fill(red);

	alpha = 1;
	inc = 1;
	inc_amt = (int)(255 / (60 * TIME));
	current_x_pos = 0;
	current_y_pos = 0;

	SoundFx[0] = CP_Sound_Load("./Assets/SFX/Digipen(Opening).mp3");
	SoundFx[1] = CP_Sound_Load("./Assets/SFX/Oznola(Logo).mp3");

	CP_Sound_Play(SoundFx[0]);
}

void splash_screen_update(void)
{
	//INPUT
	if (CP_Input_KeyDown(KEY_Q))
	{
		CP_Engine_Terminate();
	}

	if (CP_Input_KeyTriggered(KEY_SPACE))
	{
		CP_Engine_SetNextGameState(main_menu_init, main_menu_update, main_menu_exit);	
	}

	//LOGIC
	if (alpha >= 255)
	{
		alpha = 255;
		inc = 0;
	}
	if (alpha <= 0)
	{
		alpha = 0;
		
		if (game_flag)
		{
			CP_Engine_SetNextGameState(main_menu_init, main_menu_update, main_menu_exit);
		}
		game_flag = 1;
		inc = 1;
	}

	if (inc == 1)
	{
		alpha += inc_amt;
	}
	else
	{
		alpha -= inc_amt;
	}

	//RENDER
	CP_Graphics_ClearBackground(CP_Color_Create(0, 0, 0, 255));
	if (!game_flag)
	CP_Image_Draw(logo, CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.5f, (float)CP_Image_GetWidth(logo), (float)CP_Image_GetHeight(logo), alpha);

	if (game_flag)
	{
		if (!sound_bool)
		{
			CP_Sound_Play(SoundFx[1]);
			sound_bool = 1;
		}
		
		CP_Image_DrawSubImage(game_logo,
			CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.37f,
			720.0f, 720.0f,
			0.0f + (480.0f * (float)frame), 0.0f,
			480.0f + (480.0f * (float)frame), 480.0f,
			alpha);
		CP_Settings_TextSize(200.0f);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Settings_Fill(CP_Color_Create(255, 255, 255, alpha));
		CP_Font_DrawText("OZNOLA", CP_System_GetWindowWidth() * 0.5f, CP_System_GetWindowHeight() * 0.8f);
	}
	

	if (timer > 0.1f)
	{
		frame += 1;
		if (frame > 3)
		{
			frame = 0;
		}
		timer = 0.0f;
	}
	timer += CP_System_GetDt();

}

void splash_screen_exit(void)
{
	CP_Image_Free(&logo);
	for (int i = 0; i < 2; i++) {
		CP_Sound_Free(&SoundFx[i]);
	}
}