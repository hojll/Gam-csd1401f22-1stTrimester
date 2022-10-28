#ifndef ETEXTPOPUP_H
#define ETEXTPOPUP_H
#include "entities.h"

typedef struct {
    GameObject go;
    CP_Color color;
    int font_size;
    char *text;
}TextPopUp;

void set_popup(TextPopUp* text, float x, float y, CP_Color color, int font_size, float duration, char* string);

void update_popup(TextPopUp* text);

void draw_popup(TextPopUp* text);

#endif // !ETEXTPOPUP_H

