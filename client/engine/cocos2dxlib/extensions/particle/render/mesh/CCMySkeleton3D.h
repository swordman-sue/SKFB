#ifndef __Skeleton3D_H__
#define __Skeleton3D_H__

#include "CCResObjManager.h"
#include "math/CCMath.h"

#include <map>
#include <vector>
#include <string>

NS_CC_BEGIN

struct CCBone;
class CCMeshSkeletonAnimation;
class CCAnimationStateSet;

class CCMySkeleton: public BaseObj
{
protected:
	CCMySkeleton();

public:
    CCMySkeleton(const std::string& name);
    virtual ~CCMySkeleton();

    virtual CCBone* createBone(void);

    virtual CCBone* createBone(unsigned short handle);

    virtual unsigned short getNumBones(void) const;

	typedef std::vector<CCBone*> BoneList;

    virtual CCBone* getBone(unsigned short handle) const;

    virtual CCMeshSkeletonAnimation* createAnimation(const std::string& name, float length);

    virtual CCMeshSkeletonAnimation* getAnimation(const std::string& name) const;

	virtual CCMeshSkeletonAnimation* _getAnimationImpl(const std::string& name) const;

	virtual bool hasAnimation(const std::string& name);

    virtual void removeAnimation(const std::string& name);

    virtual void setAnimationState(const CCAnimationStateSet& animSet);

    virtual void _initAnimationState(CCAnimationStateSet* animSet);

	virtual void _refreshAnimationState(CCAnimationStateSet* animSet);

    virtual void _getBoneMatrices(Mat4* pMatrices);

    virtual unsigned short getNumAnimations(void) const;

    virtual CCMeshSkeletonAnimation* getAnimation(unsigned short index) const;

	const BoneList* GetBoneList() { return &mBoneList; }

	void SetBindingPose();

	//virtual void optimiseAllAnimations(bool preservingIdentityNodeTracks = false);

	typedef std::vector<unsigned short> BoneHandleMap;

public:
	virtual void OnCreated();

protected:
    BoneList mBoneList;

    unsigned short mNextAutoHandle;

	typedef std::map<std::string, CCMeshSkeletonAnimation*> AnimationList;
    AnimationList mAnimationsList;

	void Clear();
};

NS_CC_END

#endif

