#include "cocos/base/ccTypes.h"
#include "math/CCMath.h"
#include "bytebuffer.h"
#include "CCSkeletonData.h"
#include "CCAnimationState.h"
#include "CCAnimationTrack.h"
#include "CCMeshSkeletonAnimation.h"
#include "CCMySkeleton3D.h"
#include "CCSkeleton3DSerializer.h"
#include <assert.h>

#define DREAM_MAX_NUM_BONES 256

using namespace std;

NS_CC_BEGIN

CCMySkeleton::CCMySkeleton(): BaseObj(""), mNextAutoHandle(0)
{
}
//---------------------------------------------------------------------
CCMySkeleton::CCMySkeleton( const std::string& name)
	:BaseObj(name),
	 mNextAutoHandle(0)
{
}
//---------------------------------------------------------------------
CCMySkeleton::~CCMySkeleton()
{
	Clear();
	Unload();
}

//---------------------------------------------------------------------
CCBone* CCMySkeleton::createBone(void)
{
    return createBone(mNextAutoHandle++);
}

//---------------------------------------------------------------------
CCBone* CCMySkeleton::createBone(unsigned short handle)
{
    if (handle >= DREAM_MAX_NUM_BONES)
    {
        assert(false && "Exceeded the maximum number of bones per skeleton Skeleton::createBone");
    }
    if (handle < mBoneList.size() && mBoneList[handle] != NULL)
    {
        assert(false && "A bone with the handle already exists Skeleton::createBone" );
    }
    CCBone* ret = new CCBone(handle);

    if (mBoneList.size() <= handle)
    {
        mBoneList.resize(handle+1);
    }
    mBoneList[handle] = ret;
    return ret;
}


//---------------------------------------------------------------------
void CCMySkeleton::setAnimationState(const CCAnimationStateSet& animSet)
{
	float weightFactor = 1.0f;
	const EnabledAnimationStateList* anim_list = animSet.GetEnabledAnimationStateList();

	//if (mBlendState == ANIMBLEND_AVERAGE)
	{
		float totalWeights = 0.0f;
		EnabledAnimationStateList::const_iterator stateIt = anim_list->begin(), anim_end = anim_list->end();
		while (stateIt != anim_end)
		{
			const CCAnimationState* animState = *stateIt;
			if (_getAnimationImpl(animState->getAnimationName()))
			{
				totalWeights += animState->getWeight();
			}
			++stateIt;
		}

		if (totalWeights > 1.0f)
		{
			weightFactor = 1.0f / totalWeights;
		}
	}

    EnabledAnimationStateList::const_iterator stateIt = anim_list->begin(), anim_end = anim_list->end();
    while (stateIt != anim_end)
    {
        const CCAnimationState* animState = *stateIt;
        CCMeshSkeletonAnimation* anim = _getAnimationImpl(animState->getAnimationName());
        if (anim)
        {
            if(animState->hasBlendMask())
            {
				anim->apply(this, animState->getTimePosition(), animState->getWeight() * weightFactor, animState->getBlendMask());
            }
            else
            {
				anim->apply(this, animState->getTimePosition(), animState->getWeight() * weightFactor);
            }
        }
		++stateIt;
    }
}

//---------------------------------------------------------------------
CCMeshSkeletonAnimation* CCMySkeleton::createAnimation(const std::string& name, float length)
{
    if (mAnimationsList.find(name) != mAnimationsList.end())
    {
        assert(false && "An animation with the name already exists Skeleton::createAnimation");
    }

    CCMeshSkeletonAnimation* ret = new CCMeshSkeletonAnimation(name, length);
    mAnimationsList[name] = ret;
    return ret;
}
//---------------------------------------------------------------------
CCMeshSkeletonAnimation* CCMySkeleton::getAnimation(const std::string& name) const
{
	CCMeshSkeletonAnimation* ret = _getAnimationImpl(name);
	if (!ret)
	{
		assert(false && "No animation entry found named Skeleton::getAnimation");
	}
	return ret;
}
//---------------------------------------------------------------------
bool CCMySkeleton::hasAnimation(const std::string& name)
{
	return _getAnimationImpl(name) != 0;
}
//---------------------------------------------------------------------
CCMeshSkeletonAnimation* CCMySkeleton::_getAnimationImpl(const std::string& name) const
{
	CCMeshSkeletonAnimation* ret = 0;
    AnimationList::const_iterator i = mAnimationsList.find(name);
    if (i != mAnimationsList.end())
	{
		ret = i->second;
	}
	return ret;
}
//---------------------------------------------------------------------
void CCMySkeleton::removeAnimation(const std::string& name)
{
    AnimationList::iterator i = mAnimationsList.find(name);
    if (i == mAnimationsList.end())
    {
        assert(false && "No animation entry found named Skeleton::getAnimation");
    }
	if (i->second != 0)
	{
		delete i->second;
		i->second = 0;
	}
    mAnimationsList.erase(i);
}
//-----------------------------------------------------------------------
void CCMySkeleton::_initAnimationState(CCAnimationStateSet* animSet)
{
    animSet->removeAllAnimationStates();   
    AnimationList::iterator i;
    for (i = mAnimationsList.begin(); i != mAnimationsList.end(); ++i)
    {
        CCMeshSkeletonAnimation* anim = i->second;
        const std::string& animName = anim->getName();
        animSet->createAnimationState(animName, 0.0, anim->getLength());
    }
}
//-----------------------------------------------------------------------
void CCMySkeleton::_refreshAnimationState(CCAnimationStateSet* animSet)
{
	AnimationList::iterator i;
	for (i = mAnimationsList.begin(); i != mAnimationsList.end(); ++i)
	{
		CCMeshSkeletonAnimation* anim = i->second;
		const std::string& animName = anim->getName();
		if (!animSet->hasAnimationState(animName))
		{
			animSet->createAnimationState(animName, 0.0, anim->getLength());
		}
		else
		{
			CCAnimationState* animState = animSet->getAnimationState(animName);
			animState->setLength(anim->getLength());
			float t = (anim->getLength() < animState->getTimePosition()) ? anim->getLength() : animState->getTimePosition();
			animState->setTimePosition(t);
		}
	}
}
//-----------------------------------------------------------------------
unsigned short CCMySkeleton::getNumBones(void) const
{
    return (unsigned short)mBoneList.size();
}
//-----------------------------------------------------------------------
void CCMySkeleton::_getBoneMatrices(Mat4* pMatrices)
{
    BoneList::const_iterator i, boneend;
    boneend = mBoneList.end();
    for (i = mBoneList.begin();i != boneend; ++i)
    {
        CCBone* pBone = *i;
        pBone->_getOffsetTransform(*pMatrices);
        pMatrices++;
    }
}
//---------------------------------------------------------------------
unsigned short CCMySkeleton::getNumAnimations(void) const
{
    return (unsigned short)mAnimationsList.size();
}
//---------------------------------------------------------------------
CCMeshSkeletonAnimation* CCMySkeleton::getAnimation(unsigned short index) const
{
    assert( index < mAnimationsList.size() );
    AnimationList::const_iterator i = mAnimationsList.begin();
	std::advance(i, index);
    return i->second;
}
//---------------------------------------------------------------------
CCBone* CCMySkeleton::getBone(unsigned short handle) const
{
    assert(handle < mBoneList.size() && "Index out of bounds");
    return mBoneList[handle];
}

void CCMySkeleton::SetBindingPose()
{
	BoneList::iterator i;
	for (i = mBoneList.begin(); i != mBoneList.end(); ++i)
	{            
		(*i)->SetBindingPose();
	}
}

//---------------------------------------------------------------------
// void Skeleton::optimiseAllAnimations(bool preservingIdentityNodeTracks)
// {
//     AnimationList::iterator ai, aiend;
//     aiend = mAnimationsList.end();
// 
//     if (!preservingIdentityNodeTracks)
//     {
//         Animation::TrackHandleList tracksToDestroy;
// 
//         // Assume all node tracks are identity
//         ushort numBones = getNumBones();
//         for (ushort h = 0; h < numBones; ++h)
//         {
//             tracksToDestroy.insert(h);
//         }
// 
//         // Collect identity node tracks for all animations
//         for (ai = mAnimationsList.begin(); ai != aiend; ++ai)
//         {
//             ai->second->_collectIdentityNodeTracks(tracksToDestroy);
//         }
// 
//         // Destroy identity node tracks
//         for (ai = mAnimationsList.begin(); ai != aiend; ++ai)
//         {
//             ai->second->_destroyNodeTracks(tracksToDestroy);
//         }
//     }
// 
//     for (ai = mAnimationsList.begin(); ai != aiend; ++ai)
//     {
//         // Don't discard identity node tracks here
// 		ai->second->optimise(false);
// 	}
// }
//---------------------------------------------------------------------

void CCMySkeleton::Clear()
{
	BoneList::iterator i;
	for (i = mBoneList.begin(); i != mBoneList.end(); ++i)
	{
		if (*i != 0)
		{
			delete *i;
			*i = 0;
		}
	}
	mBoneList.clear();

	AnimationList::iterator ai;
	for (ai = mAnimationsList.begin(); ai != mAnimationsList.end(); ++ai)
	{
		if (ai->second != 0)
		{
			delete ai->second;
			ai->second = 0;
		}
	}
	mAnimationsList.clear();
}


void CCMySkeleton::OnCreated()
{
	if( m_res_stream->size() > 0)
	{
		CCSkeletonSerializer serilizer;
		serilizer.importSkeleton(m_res_stream, this);
	}
}

NS_CC_END