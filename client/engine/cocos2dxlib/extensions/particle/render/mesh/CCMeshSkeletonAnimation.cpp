#include "cocos/base/ccTypes.h"
#include "math/CCMath.h"
#include "CCSkeletonData.h"
#include "CCAnimationState.h"
#include "CCAnimationTrack.h"
#include "CCMeshSkeletonAnimation.h"
#include "CCMySkeleton3D.h"
#include <assert.h>
#include <algorithm>

NS_CC_BEGIN

//---------------------------------------------------------------------
CCMeshSkeletonAnimation::CCMeshSkeletonAnimation(const std::string& name, float length)
    : mName(name)
    , mLength(length)
    , mKeyFrameTimesDirty(false)
{
}
//---------------------------------------------------------------------
CCMeshSkeletonAnimation::~CCMeshSkeletonAnimation()
{
    destroyAllNodeTracks();
}
//---------------------------------------------------------------------
float CCMeshSkeletonAnimation::getLength(void) const
{
    return mLength;
}
//---------------------------------------------------------------------
void CCMeshSkeletonAnimation::setLength(float len)
{
	mLength = len;
}
//---------------------------------------------------------------------
NodeAnimationTrack* CCMeshSkeletonAnimation::createNodeTrack(unsigned short handle)
{
    if (hasNodeTrack(handle))
    {
        assert(false && "Node track with the handle already exists Animation::createNodeTrack");
    }

    NodeAnimationTrack* ret = new NodeAnimationTrack(this, handle);
    mNodeTrackList[handle] = ret;
    return ret;
}
//---------------------------------------------------------------------
NodeAnimationTrack* CCMeshSkeletonAnimation::createNodeTrack(unsigned short handle, CCBone* bone)
{
    NodeAnimationTrack* ret = createNodeTrack(handle);
    ret->setAssociatedNode(bone);
    return ret;
}
//---------------------------------------------------------------------
unsigned short CCMeshSkeletonAnimation::getNumNodeTracks(void) const
{
    return (unsigned short)mNodeTrackList.size();
}
//---------------------------------------------------------------------
bool CCMeshSkeletonAnimation::hasNodeTrack(unsigned short handle) const
{
	return (mNodeTrackList.find(handle) != mNodeTrackList.end());
}
//---------------------------------------------------------------------
NodeAnimationTrack* CCMeshSkeletonAnimation::getNodeTrack(unsigned short handle) const
{
    NodeTrackList::const_iterator i = mNodeTrackList.find(handle);

    if (i == mNodeTrackList.end())
    {
        assert(false && "Cannot find node track with the specified handle Animation::getNodeTrack");
    }
    return i->second;
}
//---------------------------------------------------------------------
void CCMeshSkeletonAnimation::destroyNodeTrack(unsigned short handle)
{
    NodeTrackList::iterator i = mNodeTrackList.find(handle);

	if (i != mNodeTrackList.end())
	{
		if (i->second != 0)
		{
			delete i->second;
			i->second = 0;
		}
		mNodeTrackList.erase(i);
        _keyFrameListChanged();
	}
}
//---------------------------------------------------------------------
void CCMeshSkeletonAnimation::destroyAllNodeTracks(void)
{
    NodeTrackList::iterator i;
    for (i = mNodeTrackList.begin(); i != mNodeTrackList.end(); ++i)
    {
		if (i->second != 0)
		{
			delete i->second;
			i->second = 0;
		}
    }
    mNodeTrackList.clear();
    _keyFrameListChanged();
}

//---------------------------------------------------------------------
const std::string& CCMeshSkeletonAnimation::getName(void) const
{
    return mName;
}

//---------------------------------------------------------------------
void CCMeshSkeletonAnimation::apply(CCMySkeleton* skel, float timePos, float weight)
{
    TimeIndex timeIndex = _getTimeIndex(timePos);

    NodeTrackList::iterator i;
    for (i = mNodeTrackList.begin(); i != mNodeTrackList.end(); ++i)
    {
        CCBone* b = skel->getBone(i->first);
        i->second->applyToNode(b, timeIndex, weight);
    }
}
//---------------------------------------------------------------------
void CCMeshSkeletonAnimation::apply(CCMySkeleton* skel, float timePos, float weight,
    const CCAnimationState::BoneBlendMask* blendMask)
{
    TimeIndex timeIndex = _getTimeIndex(timePos);

    NodeTrackList::iterator i;
    for (i = mNodeTrackList.begin(); i != mNodeTrackList.end(); ++i)
    {
		CCBone* b = skel->getBone(i->first);
		i->second->applyToNode(b, timeIndex, (*blendMask)[b->mHandle] * weight);
    }
}

//---------------------------------------------------------------------
const CCMeshSkeletonAnimation::NodeTrackList& CCMeshSkeletonAnimation::_getNodeTrackList(void) const
{
    return mNodeTrackList;
}
//---------------------------------------------------------------------
void CCMeshSkeletonAnimation::optimise(bool discardIdentityNodeTracks)
{
	optimiseNodeTracks(discardIdentityNodeTracks);
}

//-----------------------------------------------------------------------
void CCMeshSkeletonAnimation::_destroyNodeTracks(const TrackHandleList& tracks)
{
    TrackHandleList::const_iterator t, tend;
    tend = tracks.end();
	for (t = tracks.begin(); t != tend; ++t)
	{
		destroyNodeTrack(*t);
	}
}
//-----------------------------------------------------------------------
void CCMeshSkeletonAnimation::optimiseNodeTracks(bool discardIdentityTracks)
{
// 	// Iterate over the node tracks and identify those with no useful keyframes
// 	std::list<unsigned short> tracksToDestroy;
// 	NodeTrackList::iterator i;
// 	for (i = mNodeTrackList.begin(); i != mNodeTrackList.end(); ++i)
// 	{
// 		NodeAnimationTrack* track = i->second;
// 		if (discardIdentityTracks && !track->hasNonZeroKeyFrames())
// 		{
// 			// mark the entire track for destruction
// 			tracksToDestroy.push_back(i->first);
// 		}
// 		else
// 		{
// 			track->optimise();
// 		}
// 
// 	}
// 	// Now destroy the tracks we marked for death
// 	for(std::list<unsigned short>::iterator h = tracksToDestroy.begin();
// 		h != tracksToDestroy.end(); ++h)
// 	{
// 		destroyNodeTrack(*h);
// 	}
}

TimeIndex CCMeshSkeletonAnimation::_getTimeIndex(float timePos) const
{
    if (mKeyFrameTimesDirty)
    {
        buildKeyFrameTimeList();
	}
	int a = int(timePos / (mLength+0.00001));
	timePos -= (float)(a * mLength); 

    KeyFrameTimeList::iterator it =
        std::lower_bound(mKeyFrameTimes.begin(), mKeyFrameTimes.end(), timePos);

	return TimeIndex(timePos, std::distance(mKeyFrameTimes.begin(), it));
}
//-----------------------------------------------------------------------
void CCMeshSkeletonAnimation::buildKeyFrameTimeList(void) const
{
    NodeTrackList::const_iterator i;
    mKeyFrameTimes.clear();
    for (i = mNodeTrackList.begin(); i != mNodeTrackList.end(); ++i)
    {
        i->second->_collectKeyFrameTimes(mKeyFrameTimes);
    }
    for (i = mNodeTrackList.begin(); i != mNodeTrackList.end(); ++i)
    {
        i->second->_buildKeyFrameIndexMap(mKeyFrameTimes);
    }
    mKeyFrameTimesDirty = false;
}

NS_CC_END


