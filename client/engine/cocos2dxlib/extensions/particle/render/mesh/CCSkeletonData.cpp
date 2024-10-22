#include "cocos/base/ccTypes.h"
#include "math/CCMath.h"
#include "CCSkeletonData.h"

NS_CC_BEGIN

	CCBone::CCBone(unsigned short handle) 
	: mHandle(handle)
{
	pos.set(0.0f, 0.0f, 0.0f);
	scale.set(1.0f, 1.0f, 1.0f);
	
	quat.setIdentity();
	init_inv.setIdentity();
}

void CCBone::_getOffsetTransform(Mat4& m) const
{
	Mat4 mLocal;
	mLocal.setIdentity();
	Mat4::Mat4MakeMat4(&mLocal, &quat, &pos, &scale);
	m = mLocal * init_inv;
}

CCTransformKeyFrame::CCTransformKeyFrame(float t)
	: time(t)
{
	pos.set(0.0f, 0.0f, 0.0f);
	scale.set(1.0f, 1.0f, 1.0f);
	quat.setIdentity();
}

void CCBone::SetBindingPose()
{
	Mat4::Mat4MakeMat4(&init_inv, &quat, &pos, &scale);
	init_inv.inverse();
}

NS_CC_END
