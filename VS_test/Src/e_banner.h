//All content © 2021 DigiPen (SINGAPORE) Corporation, all rights reserved.
//Joel Loh	    loh.j@digipen.edu

#ifndef EBANNER_H
#define EBANNER_H
#include "entities.h"

/*TO CREATE A BANNER
//////////////////////////////////////////
for (int i = 0; i < MAX_BANNER; ++i)
{
    if (!(banner[i].go.active))
    {
        create_banner(&banner[i], "TEXT HERE");
        break;
    }
}
*///////////////////////////////////////////

typedef struct {
    GameObject go;
    CP_Color color;
    char* text;
}Banner;

void create_banner(Banner* banner, char* text);
void update_banner(Banner* banner);
void draw_banner(Banner* banner);

#endif // !EBANNER_H