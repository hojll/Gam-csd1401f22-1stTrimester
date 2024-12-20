//All content � 2021 DigiPen (SINGAPORE) Corporation, all rights reserved.
//Alonzo Nalpon	a.nalpon@digipen.edu


#ifndef SPRITE_DATA_H
#define SPRITE_DATA_H
#include <math.h>
#include "cprocessing.h"
//#include "cprocessing.h"
// Edit this enum as you add
// more sprites
typedef enum SPRITE {
	SPRITE_PLAYER,
	SPRITE_ENEMY_1,
	SPRITE_ENEMY_2,
	SPRITE_ENEMY_3,
	NUM_SPRITES
}SPRITE;
// The data for the extracted strip. 
// Should not be edited once defined
typedef struct SpriteAnimData {
	SPRITE spriteID;
	int frameDim[2];	// X and Y dimensions of each frame
	int imageDim[2];	// Y and Y dimensions of image
	int numFrames;		// number of frames in an animation
	int imageStart[2];	// Start coord of image
	short loop;		// 1 is loop, 0 is no loop
}SpriteAnimData;

// This is what keep tracks of the current animation
typedef struct SpriteAnimInstance {
	SpriteAnimData const* animData;
	float frameDuration;
	float elapsedTime;
	int currFrame;
	short state;	// 1 is complete, 0 is incomplete
	short flip;		// 0 is default, 1 is flip image across y
}SpriteAnimInstance;

SpriteAnimInstance SetSpriteAnim(SpriteAnimData const* animData, float frameDuration);
void UpdateSpriteAnim(SpriteAnimInstance *anim, float dt);
void RenderSpriteAnim(SpriteAnimInstance* anim, CP_Image image, float x, float y, float width, float height, int alpha);
void RenderSpriteAnimOffset(SpriteAnimInstance* anim, CP_Image image, float x, float y, float width, float height, int alpha, int offset);

#endif