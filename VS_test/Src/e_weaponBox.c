#include "e_weaponBox.h"

void WeaponBox_ActiveUpdate(E_WeaponBox* weaponBox)
{
	weaponBox->go.pos = CP_Vector_Add(weaponBox->go.pos, CP_Vector_Scale(weaponBox->go.vel, g_scaledDt));

	weaponBox->lifetime -= g_scaledDt;
	if (weaponBox->lifetime <= 0)
		weaponBox->go.active = 0;

	if (weaponBox->lifetime <= 3.f)
	{
		
		weaponBox->alpha += (float)MAX_TRIGO / (float)FADE_TIME * CP_System_GetDt();
		// Wrap around so it never hits max
		weaponBox->alpha = (float)((int)weaponBox->alpha % MAX_TRIGO);
		weaponBox->color = CP_Color_Create(139, 69, 19, weaponBox->alpha);
	}

	if (!weaponBox->grounded && weaponBox->go.vel.y < MAX_GRAV_VEL)
		weaponBox->go.vel.y += GRAVITY;
}

E_WeaponBox InitializeWeaponBox()
{
	E_WeaponBox weaponBox;
	weaponBox.Update = WeaponBox_ActiveUpdate;
	weaponBox.go.active = 0;
	weaponBox.go.width = 30;
	weaponBox.go.height = 30;
	weaponBox.go.pos = CP_Vector_Zero();
	weaponBox.go.vel = CP_Vector_Zero();
	weaponBox.lifetime = WEAPON_BOX_LIFETIME;
	weaponBox.grounded = 0;
	weaponBox.color = WEAPON_BOX_COLOR;
	weaponBox.alpha = 255;
	return weaponBox;
}
