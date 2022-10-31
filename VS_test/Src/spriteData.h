#ifndef SPRITE_DATA_H
#define SPRITE_DATA_H
#include <math.h>
//#include "cprocessing.h"
// Edit this enum as you add
// more sprites
typedef enum SPRITE {
	SPRITE_PLAYER,
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
}SpriteAnimData;

// This is what keep tracks of the current animation
typedef struct SpriteAnimInstance {
	SpriteAnimData const* animData;
	float frameDuration;
	float elapsedTime;
	int currFrame;
}SpriteAnimInstance;

inline void UpdateSpriteAnim(SpriteAnimInstance *anim, float dt) {
	anim->elapsedTime += dt;
	int numFramesToSkip = (int)(anim->elapsedTime / anim->frameDuration);
	anim->elapsedTime = fmodf(anim->elapsedTime, anim->frameDuration);
	
	anim->currFrame += numFramesToSkip;
	if (anim->currFrame > anim->animData->numFrames)
		anim->currFrame = anim->animData->numFrames;
}
#endif