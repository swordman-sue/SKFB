#include "clippingnodeex.h"

using namespace cocos2d;

ClippingNodeEx* ClippingNodeEx::Node()
{
	ClippingNodeEx *stencil_container = new ClippingNodeEx();
	if(!stencil_container->init())
	{
		CC_SAFE_DELETE(stencil_container);
		return NULL;
	}

	stencil_container->autorelease();
	return stencil_container;
}

void ClippingNodeEx::SetStencilMask(cocos2d::Node* mask_node)
{
	if(!mask_node)
		return;

	setStencil(mask_node);
}

bool ClippingNodeEx::IsInverted() const
{
	return isInverted();
}

void ClippingNodeEx::SetInverted(bool invert)
{
	setInverted(invert);
}

void ClippingNodeEx::SetAlphaTestValue(GLubyte alpha_value)
{
	float m_alpha_test_value = float(alpha_value) / 255;
	setAlphaThreshold(m_alpha_test_value);
}

GLubyte ClippingNodeEx::GetAlphaTestValue()
{
	return GLubyte(getAlphaThreshold() * 255);
}