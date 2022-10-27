#include "collision.h"
#pragma once

CP_BOOL AABB(GameObject gameObjectA, GameObject gameObjectB)
{
    if (gameObjectA.pos.x + gameObjectA.width / 2.f < gameObjectB.pos.x - gameObjectB.width / 2.f ||
        gameObjectB.pos.x + gameObjectB.width / 2.f < gameObjectA.pos.x - gameObjectA.width / 2.f ||
        gameObjectA.pos.y + gameObjectA.height / 2.f < gameObjectB.pos.y - gameObjectB.height / 2.f ||
        gameObjectB.pos.y + gameObjectB.height / 2.f < gameObjectA.pos.y - gameObjectA.height / 2.f)
        return 0;
    return 1;
}
