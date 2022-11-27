//All content © 2021 DigiPen (SINGAPORE) Corporation, all rights reserved.
//Joel Loh	    loh.j@digipen.edu

#ifndef ETEXTPOPUP_H
#define ETEXTPOPUP_H
#include "entities.h"


typedef struct {
    GameObject go; // Basic struct.
    CP_Color color; // Color for the text.
    int font_size; // Font size for modularity.
    char *text; // String to be printed.
}TextPopUp;

// Assigns a TextPopUp struct with the relevant data.
void set_popup(TextPopUp* text, float x, float y, CP_Color color, int font_size, float duration, char* string);

// Decrements the time and sets the active once its over.
void update_popup(TextPopUp* text);

// Draws the TextPopUp struct.
void draw_popup(TextPopUp* text);

#endif // !ETEXTPOPUP_H

