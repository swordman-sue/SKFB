#ifndef STENCIL_CONTAINER_H
#define STENCIL_CONTAINER_H

#include "cocos2d.h"

class ClippingNodeEx : public cocos2d::ClippingNode
{
public:
	static ClippingNodeEx* Node();

	void SetStencilMask(cocos2d::Node* mask_node);
	bool IsInverted() const;
	void SetInverted(bool invert);
	void SetAlphaTestValue(GLubyte alpha_value);
	GLubyte GetAlphaTestValue();
};

#endif