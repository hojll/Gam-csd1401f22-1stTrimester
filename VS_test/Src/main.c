//All content © 2021 DigiPen (SINGAPORE) Corporation, all rights reserved.

#include "cprocessing.h"


/* Forward declarations */
void splash_screen_init(void);
void splash_screen_update(void);
void splash_screen_exit(void);

/* Entry point */
int main(void)
{
    CP_Engine_SetNextGameState(splash_screen_init, splash_screen_update, splash_screen_exit);
    CP_Engine_Run();
    return 0;
}
