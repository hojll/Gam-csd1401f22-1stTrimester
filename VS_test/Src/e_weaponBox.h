//All content © 2021 DigiPen (SINGAPORE) Corporation, all rights reserved.
//Alvin Yeo	    a.yeo@digipen.edu

#ifndef EWEAPONBOX_H
#define EWEAPONBOX_H

#include "entities.h"

#define WEAPON_BOX_LIFETIME 7.f
#define WEAPON_BOX_COLOR CP_Color_Create(139, 69, 19, 255)

typedef struct E_WeaponBox E_WeaponBox;
typedef struct E_WeaponBox {
	GameObject go;
	// Set lifetime to below -50.f for infinite lifetime
	float lifetime;
	CP_Color color;
	void (*Update)(E_WeaponBox*);
	CP_BOOL grounded;
	float alpha;
}E_WeaponBox;
E_WeaponBox InitializeWeaponBox();

#endif
