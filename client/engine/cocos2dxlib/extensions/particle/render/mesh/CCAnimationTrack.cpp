#include "cocos/base/ccTypes.h"
#include "math/CCMath.h"
#include "CCSkeletonData.h"
#include "CCAnimationState.h"
#include "CCAnimationTrack.h"
#include "CCMeshSkeletonAnimation.h"
#include <assert.h>
#include <algorithm>
using namespace std;

NS_CC_BEGIN

struct KeyFrameTimeLess
{
	bool operator() (const CCTransformKeyFrame* kf, const CCTransformKeyFrame* kf2) const
	{
		return kf->time < kf2->time;
	}
};


AnimationTrack::AnimationTrack(CCMeshSkeletonAnimation* parent, unsigned short handle) :
	mParent(parent), mHandle(handle)
{
}
//---------------------------------------------------------------------
AnimationTrack::~AnimationTrack()
{
    removeAllKeyFrames();
}
//---------------------------------------------------------------------
size_t AnimationTrack::getNumKeyFrames(void) const
{
    return mKeyFrames.size();
}
//---------------------------------------------------------------------
CCTransformKeyFrame* AnimationTrack::getKeyFrame(size_t index) const
{
    assert( index < mKeyFrames.size() );
    return mKeyFrames[index];
}
//---------------------------------------------------------------------
float AnimationTrack::getKeyFramesAtTime(const TimeIndex& timeIndex, CCTransformKeyFrame** keyFrame1, CCTransformKeyFrame** keyFrame2,
    unsigned short* firstKeyIndex) const
{
    float t1, t2;
    float timePos = timeIndex.getTimePos();

    // Find first keyframe after or on current time
    KeyFrameList::const_iterator i;
    if (timeIndex.hasKeyIndex())
    {
        // Global keyframe index available, map to local keyframe index directly.
        assert(timeIndex.getKeyIndex() < mKeyFrameIndexMap.size());
        i = mKeyFrames.begin() + mKeyFrameIndexMap[timeIndex.getKeyIndex()];
    }
    else
    {
        float totalAnimationLength = mParent->getLength();
        assert(totalAnimationLength > 0.0f && "Invalid animation length!");

		float a = timePos / totalAnimationLength;
		timePos -= (float)((int)a * totalAnimationLength); 

        // No global keyframe index, need to search with local keyframes.
        CCTransformKeyFrame timeKey(timePos);
        i = std::lower_bound(mKeyFrames.begin(), mKeyFrames.end(), &timeKey, KeyFrameTimeLess());
    }

    if (i == mKeyFrames.end())
    {
        // There is no keyframe after this time, wrap back to first
        *keyFrame2 = mKeyFrames.front();
        t2 = mParent->getLength() + (*keyFrame2)->time;

        // Use last keyframe as previous keyframe
        --i;
    }
    else
    {
        *keyFrame2 = *i;
        t2 = (*keyFrame2)->time;
        // Find last keyframe before or on current time
        if (i != mKeyFrames.begin() && timePos < (*i)->time)
        {
            --i;
        }
    }

    if (firstKeyIndex)
    {
        *firstKeyIndex = static_cast<unsigned short>(std::distance(mKeyFrames.begin(), i));
    }

    *keyFrame1 = *i;
    t1 = (*keyFrame1)->time;

    if (t1 == t2)
    {
        return 0.0;
    }
    else
    {
        return (timePos - t1) / (t2 - t1);
    }
}
//---------------------------------------------------------------------
CCTransformKeyFrame* AnimationTrack::createKeyFrame(float timePos)
{
    CCTransformKeyFrame* kf = new CCTransformKeyFrame(timePos);
    KeyFrameList::iterator i = std::upper_bound(mKeyFrames.begin(), mKeyFrames.end(), kf, KeyFrameTimeLess());
    mKeyFrames.insert(i, kf);
    mParent->_keyFrameListChanged();

    return kf;
}
//---------------------------------------------------------------------
void AnimationTrack::removeKeyFrame(size_t index)
{
	// If you hit this assert, then the keyframe index is out of bounds
    assert( index < mKeyFrames.size() );
    KeyFrameList::iterator i = mKeyFrames.begin();
    i += index;
	if (*i != 0)
	{
		delete *i;
	}

    mKeyFrames.erase(i);
    mParent->_keyFrameListChanged();
}
//---------------------------------------------------------------------
void AnimationTrack::removeAllKeyFrames(void)
{
    KeyFrameList::iterator i = mKeyFrames.begin();
    for (; i != mKeyFrames.end(); ++i)
	{
		if (*i != 0)
		{
			delete *i;
			*i = 0;
		}
    }
    mParent->_keyFrameListChanged();
    mKeyFrames.clear();
}
//---------------------------------------------------------------------
void AnimationTrack::_collectKeyFrameTimes(std::vector<float>& keyFrameTimes)
{
    for (KeyFrameList::const_iterator i = mKeyFrames.begin(); i != mKeyFrames.end(); ++i)
    {
        float timePos = (*i)->time;

		std::vector<float>::iterator it = std::lower_bound(keyFrameTimes.begin(), keyFrameTimes.end(), timePos);
        if (it == keyFrameTimes.end() || *it != timePos)
        {
            keyFrameTimes.insert(it, timePos);
        }
    }
}
//---------------------------------------------------------------------
void AnimationTrack::_buildKeyFrameIndexMap(const std::vector<float>& keyFrameTimes)
{
    mKeyFrameIndexMap.resize(keyFrameTimes.size() + 1);
    size_t i = 0, j = 0;
    while (j <= keyFrameTimes.size())
    {
        mKeyFrameIndexMap[j] = i;
        while (i < mKeyFrames.size() && mKeyFrames[i]->time <= keyFrameTimes[j])
            ++i;
        ++j;
    }
}

// Node specialisations
//---------------------------------------------------------------------
NodeAnimationTrack::NodeAnimationTrack(CCMeshSkeletonAnimation* parent, unsigned short handle)
	: AnimationTrack(parent, handle), m_target_bone(0)
{
}

//---------------------------------------------------------------------
NodeAnimationTrack::~NodeAnimationTrack()
{
}

CCBone* NodeAnimationTrack::getAssociatedNode(void) const
{
	return m_target_bone;
}


void NodeAnimationTrack::setAssociatedNode(CCBone* bone)
{
	m_target_bone = bone;
}
//---------------------------------------------------------------------
void NodeAnimationTrack::getInterpolatedKeyFrame(const TimeIndex& timeIndex, CCTransformKeyFrame* kret) const
{
    CCTransformKeyFrame *k1, *k2;
    unsigned short firstKeyIndex;

    float t = this->getKeyFramesAtTime(timeIndex, &k1, &k2, &firstKeyIndex);

    if (t == 0.0)
    {
		kret->quat.set((k1->quat));
		kret->pos.set(k1->pos);
		kret->scale.set(k1->scale);
    }
    else
    {
		Vec3 base (0, 0, 0);

		Quaternion::lerp(k2->quat, k1->quat, t, &kret->quat);

        Vec3::subtract(k2->pos, k1->pos,&base);
		base.scale(t);
		
		Vec3::add(base, k1->pos,&kret->pos);

		Vec3::subtract(k2->scale, k1->scale, &base);
		base.scale(t);
		Vec3::add(k1->scale,base,&kret->scale);
    }
}

//---------------------------------------------------------------------
void NodeAnimationTrack::applyToNode(CCBone* bone, const TimeIndex& timeIndex, float weight)
{
	if (mKeyFrames.empty() || !weight || !bone)
		return;

    CCTransformKeyFrame kf(timeIndex.getTimePos());
	getInterpolatedKeyFrame(timeIndex, &kf);

	// add to existing. Weights are not relative, but treated as absolute multipliers for the animation
	bone->pos = kf.pos*weight;

	Quaternion ide;
	ide.setIdentity();
	
	Quaternion::slerp(ide, kf.quat, weight, &bone->quat);

	static Vec3 unit_scale (1, 1, 1);
	Vec3 scale;
	scale.set(kf.scale);

	if ((scale==unit_scale))
	{
        if (weight != 1.0f)
		{
			Vec3::subtract(scale,unit_scale,&scale);
			scale.scale(weight);
			Vec3::add(unit_scale,scale,&scale);
		}
	}

	(bone->scale).set(scale);
}

// void NodeAnimationTrack::optimise(void)
// {
// 	Vector3 lasttrans = Vector3::ZERO;
// 	Vector3 lastscale = Vector3::ZERO;
// 	Quaternion lastorientation;
//     KeyFrameList::iterator i = mKeyFrames.begin();
// 	Radian quatTolerance(1e-3f);
// 	std::list<unsigned short> removeList;
// 	unsigned short k = 0;
// 	ushort dupKfCount = 0;
//     for (; i != mKeyFrames.end(); ++i, ++k)
//     {
// 		TransformKeyFrame* kf = static_cast<TransformKeyFrame*>(*i);
// 		Vector3 newtrans = kf->getTranslate();
// 		Vector3 newscale = kf->getScale();
// 		Quaternion neworientation = kf->getRotation();
// 		// Ignore first keyframe; now include the last keyframe as we eliminate
// 		// only k-2 in a group of 5 to ensure we only eliminate middle keys
// 		if (i != mKeyFrames.begin() &&
// 			newtrans.positionEquals(lasttrans) &&
// 			newscale.positionEquals(lastscale) &&
// 			neworientation.equals(lastorientation, quatTolerance))
// 		{
// 			++dupKfCount;
// 
// 			// 4 indicates this is the 5th duplicate keyframe
// 			if (dupKfCount == 4)
// 			{
// 				// remove the 'middle' keyframe
// 				removeList.push_back(k-2);
// 				--dupKfCount;
// 			}
// 		}
// 		else
// 		{
// 			// reset
// 			dupKfCount = 0;
// 			lasttrans = newtrans;
// 			lastscale = newscale;
// 			lastorientation = neworientation;
// 		}
// 	}
// 
// 	// Now remove keyframes, in reverse order to avoid index revocation
// 	std::list<unsigned short>::reverse_iterator r = removeList.rbegin();
// 	for (; r!= removeList.rend(); ++r)
// 	{
// 		removeKeyFrame(*r);
// 	}
// }
	

NS_CC_END

