#include "collision.h"
#include <math.h>

CP_BOOL AABB(GameObject gameObjectA, GameObject gameObjectB)
{
    if (gameObjectA.pos.x + gameObjectA.width / 2.f < gameObjectB.pos.x - gameObjectB.width / 2.f ||
        gameObjectB.pos.x + gameObjectB.width / 2.f < gameObjectA.pos.x - gameObjectA.width / 2.f ||
        gameObjectA.pos.y + gameObjectA.height / 2.f < gameObjectB.pos.y - gameObjectB.height / 2.f ||
        gameObjectB.pos.y + gameObjectB.height / 2.f < gameObjectA.pos.y - gameObjectA.height / 2.f)
        return 0;
    return 1;
}

COLLISION_DIRECTION AABB_Direction(GameObject gameObjectA, GameObject gameObjectB)
{
    float top = fabs((gameObjectA.pos.y + gameObjectA.height / 2.f) - (gameObjectB.pos.y - gameObjectB.height / 2.f));
    float bottom = fabs((gameObjectA.pos.y - gameObjectA.height / 2.f) - (gameObjectB.pos.y + gameObjectB.height / 2.f));
    float left = fabs((gameObjectA.pos.x + gameObjectA.width / 2.f) - (gameObjectB.pos.x - gameObjectB.width / 2.f));
    float right = fabs((gameObjectA.pos.x - gameObjectA.width / 2.f) - (gameObjectB.pos.x + gameObjectB.width / 2.f));

    if (top < bottom && top < left && top < right)
        return COLLISION_TOP;
    if (bottom < top && bottom < left && bottom < right)
        return COLLISION_BOTTOM;
    if (left < top && left < bottom && left < right)
        return COLLISION_LEFT;
    return COLLISION_RIGHT;
}

CP_BOOL Point_AABB(CP_Vector point, GameObject gameObject)
{
    if (point.x < gameObject.pos.x - gameObject.width / 2.f ||
        point.x > gameObject.pos.x + gameObject.width / 2.f ||
        point.y < gameObject.pos.y - gameObject.height / 2.f ||
        point.y > gameObject.pos.y + gameObject.height / 2.f)
        return 0;
    return 1;
}
