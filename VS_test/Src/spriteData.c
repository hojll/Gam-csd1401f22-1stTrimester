#include "spriteData.h"
// I heard inline makes it faster... Does it?
void UpdateSpriteAnim(SpriteAnimInstance* anim, float dt) {
	anim->elapsedTime += dt;
	int numFramesToSkip = (int)(anim->elapsedTime / anim->frameDuration);
	anim->currFrame += numFramesToSkip;
	anim->elapsedTime = fmodf(anim->elapsedTime, anim->frameDuration);

	if (anim->currFrame < anim->animData->numFrames) {
		return;
	}
	printf("%d, %d\n", anim->currFrame , anim->animData->numFrames);
	// If it reaches the end
	// This is for looping animations
	if (anim->animData->loop) {
		anim->currFrame = 0;
		return;
	}
	// This is not for looping animations
	anim->currFrame = anim->animData->numFrames;
	anim->state = 1;
}
// Function for Initializing a sprite animation
SpriteAnimInstance SetSpriteAnim(SpriteAnimData const* animData, float frameDuration) {
	SpriteAnimInstance retVal;
	retVal.animData = animData;
	retVal.currFrame = 0;
	retVal.elapsedTime = 0.f;
	retVal.frameDuration = frameDuration;
	retVal.state = 0;
	retVal.animData = animData;
	return retVal;
}

void RenderSpriteAnim(SpriteAnimInstance* anim, CP_Image image, float x, float y, float width, float height)
{
	//CP_Image_DrawSubImage(image, x, y, width, height, anim->animData->imageStart[0], anim->animData->imageStart[1],
	CP_Image_DrawSubImage(image, x, y, width, height,
		anim->currFrame * 32,
		0,
		(anim->currFrame + 1) * 32,
		32, 255);
	/*	anim->animData->imageStart[0] + anim->currFrame * anim->animData->imageDim[0],
		anim->animData->imageStart[1] + anim->currFrame * anim->animData->imageDim[1], 255);*/
}
