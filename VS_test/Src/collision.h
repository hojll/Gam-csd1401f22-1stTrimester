#ifndef COLLISION_H
#define COLLISION_H
#include "cprocessing.h"
#include "entities.h"

typedef enum COLLISION_DIRECTION
{
	COLLISION_TOP,
	COLLISION_BOTTOM,
	COLLISION_LEFT,
	COLLISION_RIGHT
}COLLISION_DIRECTION;

/*
@brief  Tests collision using 2 AABBs
*/
CP_BOOL AABB(GameObject gameObjectA, GameObject gameObjectB);

/*
@brief  Assumes AABB is true, calculates where gameObject A is in relation
		to gameObject B
*/
COLLISION_DIRECTION AABB_Direction(GameObject gameObjectA, GameObject gameObjectB);

/*
@brief Tests collision using a point and an AABB
*/
CP_BOOL Point_AABB(CP_Vector point, GameObject gameObject);
#endif