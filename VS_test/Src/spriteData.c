//All content © 2021 DigiPen (SINGAPORE) Corporation, all rights reserved.
//Alonzo Nalpon	a.nalpon@digipen.edu [Framework]
//Joel Loh	    loh.j@digipen.edu [Art]
#include "spriteData.h"

/// <summary>
/// Updates supplied sprite anim instance 
/// based on dt
/// </summary>
/// <param name="anim">the anim instance to update</param>
/// <param name="dt">delta time</param>
void UpdateSpriteAnim(SpriteAnimInstance* anim, float dt) {
	anim->elapsedTime += dt;
	int numFramesToSkip = (int)(anim->elapsedTime / anim->frameDuration);
	anim->currFrame += numFramesToSkip;
	anim->elapsedTime = fmodf(anim->elapsedTime, anim->frameDuration);

	if (anim->currFrame < anim->animData->numFrames) {
		return;
	}
	// If it reaches the end
	// This is for looping animations
	if (anim->animData->loop) {
		anim->currFrame = 0;
		return;
	}
	// This is for non-looping animations
	anim->currFrame = (anim->animData->numFrames - 1);	// To read from left to right
	anim->state = 1;
}

/// <summary>
/// Initializes a new sprite anim instance based on supplied anim data
/// </summary>
/// <param name="animData">the anim data</param>
/// <param name="frameDuration">duration of each frame</param>
/// <returns>a new sprite anim instance</returns>
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

/// <summary>
/// Renders a sprite animation from any anim instance
/// and supplied image
/// </summary>
/// <param name="anim">the anim instance</param>
/// <param name="image">the source image containing spritesheet</param>
/// <param name="x">x coordinate to render sprite</param>
/// <param name="y">y coordinate to render sprite</param>
/// <param name="width">width of the sprite</param>
/// <param name="height">height of the sprite</param>
/// <param name="alpha">alpha value of sprite</param>
void RenderSpriteAnim(SpriteAnimInstance* anim, CP_Image image, float x, float y, float width, float height, int alpha)
{
	// LEFT
	float u0 = (float)anim->animData->imageStart[0] + (anim->currFrame) * anim->animData->frameDim[0];
	// TOP 
	float v0 = (float)anim->animData->imageStart[1];
	// RIGHT
	float u1 = u0 + anim->animData->frameDim[0];
	// BOTTOM
	float v1 = (float)anim->animData->imageStart[1] + anim->animData->frameDim[1];
	CP_Image_DrawSubImage(image, x, y, width, height,
		!anim->flip * u0 + anim->flip * u1,// u0
		v0,// v0
		!anim->flip * u1 + anim->flip * u0,// u1
		v1, alpha);// v1
}

/// <summary>
/// Same as rendering sprite anim.
/// Only difference is offset value to offset curr frame. Will wrap
/// automatically
/// </summary>
/// <param name="anim">the anim instance</param>
/// <param name="image">the source image containing spritesheet</param>
/// <param name="x">x coordinate to render sprite</param>
/// <param name="y">y coordinate to render sprite</param>
/// <param name="width">width of the sprite</param>
/// <param name="height">height of the sprite</param>
/// <param name="alpha">alpha value of sprite</param>
/// <param name="offset">offset of frames</param>
void RenderSpriteAnimOffset(SpriteAnimInstance* anim, CP_Image image, float x, float y, float width, float height, int alpha, int offset)
{
	SpriteAnimInstance offsetAnim = *anim;
	anim->currFrame = (anim->currFrame + offset) % anim->animData->numFrames;
	RenderSpriteAnim(&offsetAnim, image, x, y, width, height, alpha);
}
