#include "cprocessing.h"


/* Forward declarations */
void game_init(void);
void game_update(void);
void game_exit(void);

void main_menu_init(void);
void main_menu_update(void);
void main_menu_exit(void);

/* Entry point */
int main(void)
{
    if (1)
    {
        CP_Engine_SetNextGameState(main_menu_init, main_menu_update, main_menu_exit);
    }
    else 
    {
        CP_Engine_SetNextGameState(game_init, game_update, game_exit);
    }
    CP_Engine_Run();
    return 0;
}
