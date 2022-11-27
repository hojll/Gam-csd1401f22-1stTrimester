//All content © 2021 DigiPen (SINGAPORE) Corporation, all rights reserved.
//Joel Loh	    loh.j@digipen.edu
#include "CProcessing.h"

void game_init(void);
void game_update(void);
void game_exit(void);

void restart_init(void)
{

}

void restart_update(void)
{
	CP_Engine_SetNextGameState(game_init, game_update, game_exit);
}

void restart_exit(void)
{

}