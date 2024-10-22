#ifndef __SkeletonData_H__
#define __SkeletonData_H__

NS_CC_BEGIN

struct CCBone
{
	unsigned short mHandle;
	Vec3 pos;
	Vec3 scale;
	Quaternion quat;

	Mat4 init_inv;			//¹Ç÷À³õÊ¼¾ØÕóµÄÄæ

	CCBone(unsigned short handle);
	void SetBindingPose();
	void _getOffsetTransform(Mat4& m) const;
};

struct CCTransformKeyFrame
{
	float time;
	Vec3 pos;
	Vec3 scale;
	Quaternion quat;

	CCTransformKeyFrame(float t);
};

NS_CC_END

#endif