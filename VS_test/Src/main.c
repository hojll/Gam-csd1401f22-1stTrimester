#include "cprocessing.h"
#include "stdio.h"

CP_Image logo;
CP_Vector position;
float moveSpeed;
CP_Vector prevPos;

void game_init(void)
{
	logo = CP_Image_Load("Assets/DigiPen_Singapore_WEB_RED.png");
	CP_Settings_ImageMode(CP_POSITION_CORNER);
	CP_Settings_ImageWrapMode(CP_IMAGE_WRAP_CLAMP);

	CP_System_SetWindowSize(1920, 1080);
	CP_System_Fullscreen();
	CP_System_ShowCursor(0);

	position = CP_Vector_Set(CP_System_GetWindowWidth() / 2 - CP_Image_GetWidth(logo) / 2,
		CP_System_GetWindowHeight() / 2 - CP_Image_GetHeight(logo) / 2);

	moveSpeed = 10.0f;
}

void game_update(void)
{
	CP_Graphics_ClearBackground(CP_Color_Create(0,0,0,255));
	CP_Image_Draw(logo,
		position.x,
		position.y,
		CP_Image_GetWidth(logo),
		CP_Image_GetHeight(logo),
		255);

	prevPos = position;
	if (CP_Input_KeyDown(KEY_Q))
	{
		CP_Engine_Terminate();
	}
	if (CP_Input_KeyDown(KEY_LEFT))
	{
		position.x -= moveSpeed;
	}
	if (CP_Input_KeyDown(KEY_RIGHT))
	{
		position.x += moveSpeed;
	}
	if (CP_Input_KeyDown(KEY_UP))
	{
		position.y -= moveSpeed;
	}
	if (CP_Input_KeyDown(KEY_DOWN))
	{
		position.y += moveSpeed;
	}
	//printf("X:%f, Y:%f", position.x, position.y);

	/*CP_Vector middlePos = position;
	middlePos.x += CP_Image_GetWidth(logo) / 2;
	middlePos.y += CP_Image_GetHeight(logo) / 2;
	if (bound_collision_check(middlePos,
		CP_Image_GetWidth(logo),
		CP_Image_GetHeight(logo),
		0,
		CP_System_GetWindowWidth(),
		0,
		CP_System_GetWindowHeight()))
	{
		position = prevPos;
	}*/
}

CP_BOOL bound_collision_check(CP_Vector position, float width, float height, float leftHoriBound, float rightHoriBound, float topVertiBound, float bottomVertiBound)
{
	//Checks collision for an object at position with width and height against the outer bounds of the game
	//position - Middle position of object
	//width - Width of object
	//height - Height of object
	//leftHoriBound - Most left an object can go
	//rightHoriBound - Most right an object can go
	//topVertiBound - Most up an object can go
	//bottomVertiBound - Most down an object can go

	if ((position.x - (width / 2)) < leftHoriBound ||
		(position.x + (width / 2)) > rightHoriBound ||
		(position.y - (height / 2)) < topVertiBound ||
		(position.y + (height / 2)) > bottomVertiBound)
	{
		return 1;
	}
	return 0;
}

void game_exit(void)
{
	CP_Image_Free(&logo);
}




int main(void)
{
	//CP_Engine_SetNextGameState(splash_screen_init, splash_screen_update, splash_screen_exit);
	CP_Engine_SetNextGameState(game_init, game_update, game_exit);

	CP_Engine_Run();
}