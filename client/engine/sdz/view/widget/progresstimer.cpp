#include "progresstimer.h"
#include "view/widget/spriteex.h"

ProgressTimer* ProgressTimer::CreateWithImageLabel(SpriteEx* label)
{
	return CreateWithSprite(label);
}

ProgressTimer* ProgressTimer::CreateWithGraphicRender(GraphicRender* render)
{
	return NULL;
}

ProgressTimer* ProgressTimer::CreateWithSprite(cocos2d::CCSprite *sp)
{
	ProgressTimer *progress = new ProgressTimer();

	if(!progress->initWithSprite(sp))
	{
		CC_SAFE_DELETE(progress);
		return NULL;
	}

	progress->autorelease();
	return progress;
}

