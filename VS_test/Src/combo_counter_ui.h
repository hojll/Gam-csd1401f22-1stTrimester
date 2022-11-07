#ifndef COMBO_COUNTER_UI
#define COMBO_COUNTER_UI
#include "CProcessing.h"


void printComboCounter(CP_Vector pos, int scale, int r, int g, int b, int alpha);
void printComboCountdownTimer(CP_Vector pos, CP_Vector wh, int r, int g, int b, int alpha);
void updateComboCounterTimer(double *timer, double deduct, double combotime);
void addcombotime(double *timer, double addedtime);
#endif