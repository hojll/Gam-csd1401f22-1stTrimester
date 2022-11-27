//All content © 2021 DigiPen (SINGAPORE) Corporation, all rights reserved.
//Alvin Yeo	    a.yeo@digipen.edu
//Alonzo Nalpon	a.nalpon@digipen.edu
//Joel Loh	    loh.j@digipen.edu
//Ryan Ho		r.ho@digipen.edu

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
