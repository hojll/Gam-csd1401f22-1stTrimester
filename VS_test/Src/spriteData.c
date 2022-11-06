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
	//printf("%d, %d\n", anim->currFrame , anim->animData->numFrames);
	// If it reaches the end
	// This is for looping animations
	if (anim->animData->loop) {
		anim->currFrame = 0;
		return;
	}
	// This is not for looping animations
	anim->currFrame = (anim->animData->numFrames - 1);	// To read from left to right
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
	retVal.flip = 0;
	return retVal;
}

void RenderSpriteAnim(SpriteAnimInstance* anim, CP_Image image, float x, float y, float width, float height, int alpha)
{
	//CP_Image_DrawSubImage(image, x, y, width, height, anim->animData->frameDim[0], 0.f, anim->animData->frameDim[0] * 2,
	//	anim->animData->frameDim[1], 255);
	//return; //FOR DEBUGGING
	// LEFT
	float u0 = (float)anim->animData->imageStart[0] + (anim->currFrame) * anim->animData->frameDim[0];
	// TOP 
	float v0 = (float)anim->animData->imageStart[1];
	// RIGHT
	float u1 = u0 + anim->animData->frameDim[0];
	// BOTTOM
	float v1 = (float)anim->animData->imageStart[1] + anim->animData->frameDim[1];
	//printf("ORIGINAL:\nLEFT: %f | TOP: %f | RIGHT: %f | BOTTOM: %f\n=========\n", (float)anim->animData->frameDim[0], 0.f, (float)anim->animData->frameDim[0] * 2,
	//	(float)anim->animData->frameDim[1]);
	CP_Image_DrawSubImage(image, x, y, width, height,
		!anim->flip * u0 + anim->flip * u1,// u0
		v0,// v0
		!anim->flip * u1 + anim->flip * u0,// u1
		v1, alpha);// v1
	/*	anim->animData->imageStart[0] + anim->currFrame * anim->animData->imageDim[0],
		anim->animData->imageStart[1] + anim->currFrame * anim->animData->imageDim[1], 255);*/
}

void RenderSpriteAnimOffset(SpriteAnimInstance* anim, CP_Image image, float x, float y, float width, float height, int alpha, int offset)
{
	SpriteAnimInstance offsetAnim = *anim;
	anim->currFrame = (anim->currFrame + offset) % anim->animData->numFrames;
	RenderSpriteAnim(&offsetAnim, image, x, y, width, height, alpha);
}
