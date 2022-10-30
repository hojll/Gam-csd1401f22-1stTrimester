#include "utilities.h"
#include <stdio.h>
#include <stdlib.h>

CP_BOOL CP_Vector_Equal(CP_Vector vector1, CP_Vector vector2)
{
	return(vector1.x == vector2.x && vector1.y == vector2.y);
}

int returnRange(int min, int max)
{
	return ((int)rand() / (int)RAND_MAX) * (max - min) + min;
}