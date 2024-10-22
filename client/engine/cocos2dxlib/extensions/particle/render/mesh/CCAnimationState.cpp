#include "cocos/base/ccTypes.h"
#include "math/CCMath.h"
#include "CCAnimationState.h"
#include <limits>
#include <assert.h>

NS_CC_BEGIN

	//---------------------------------------------------------------------
CCAnimationState::CCAnimationState(CCAnimationStateSet* parent, const CCAnimationState &rhs)
    : mBlendMask(0)
    , mAnimationName(rhs.mAnimationName)
    , mParent(parent)
    , mTimePos(rhs.mTimePos)
    , mLength(rhs.mLength)
    , mWeight(rhs.mWeight)
    , mEnabled(rhs.mEnabled)
    , mLoop(rhs.mLoop)
{
	mParent->_notifyDirty();
}
//---------------------------------------------------------------------
CCAnimationState::~CCAnimationState()
{
}
//---------------------------------------------------------------------
CCAnimationState::CCAnimationState(const std::string& animName, 
	CCAnimationStateSet *parent, float timePos, float length, float weight, 
	bool enabled)
    : mBlendMask(0)
    , mAnimationName(animName)
    , mParent(parent)
    , mTimePos(timePos)
    , mLength(length)
    , mWeight(weight)
    , mEnabled(enabled)
    , mLoop(false)
{
	mParent->_notifyDirty();
}
//---------------------------------------------------------------------
const std::string& CCAnimationState::getAnimationName() const
{
    return mAnimationName;
}
//---------------------------------------------------------------------
float CCAnimationState::getTimePosition(void) const
{
    return mTimePos;
}
//---------------------------------------------------------------------
void CCAnimationState::setTimePosition(float timePos)
{
	if (timePos != mTimePos)
	{
		mTimePos = timePos;
		if (mLoop)
		{
			// Wrap
			mTimePos = fmod(mTimePos, mLength);
			if(mTimePos < 0)
				mTimePos += mLength;     
		}
		else
		{
			// Clamp
			if(mTimePos < 0)
				mTimePos = 0;
			else if (mTimePos > mLength)
				mTimePos = mLength;
		}

        if (mEnabled)
            mParent->_notifyDirty();
    }

}
//---------------------------------------------------------------------
float CCAnimationState::getLength() const
{
    return mLength;
}
//---------------------------------------------------------------------
void CCAnimationState::setLength(float len)
{
    mLength = len;
}
//---------------------------------------------------------------------
float CCAnimationState::getWeight(void) const
{
    return mWeight;
}
//---------------------------------------------------------------------
void CCAnimationState::setWeight(float weight)
{
    mWeight = weight;

    if (mEnabled)
        mParent->_notifyDirty();
}
//---------------------------------------------------------------------
void CCAnimationState::addTime(float offset)
{
    setTimePosition(mTimePos + offset);
}
//---------------------------------------------------------------------
bool CCAnimationState::hasEnded(void) const
{
	return (mTimePos >= mLength && !mLoop);
}
//---------------------------------------------------------------------
bool CCAnimationState::getEnabled(void) const
{
    return mEnabled;
}
//---------------------------------------------------------------------
void CCAnimationState::setEnabled(bool enabled)
{
    mEnabled = enabled;
    mParent->_notifyAnimationStateEnabled(this, enabled);
}
//---------------------------------------------------------------------
bool CCAnimationState::operator==(const CCAnimationState& rhs) const
{
    if (mAnimationName == rhs.mAnimationName &&
        mEnabled == rhs.mEnabled &&
        mTimePos == rhs.mTimePos &&
        mWeight == rhs.mWeight &&
        mLength == rhs.mLength && 
        mLoop == rhs.mLoop)
    {
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------
bool CCAnimationState::operator!=(const CCAnimationState& rhs) const
{
    return !(*this == rhs);
}
//---------------------------------------------------------------------
void CCAnimationState::copyStateFrom(const CCAnimationState& animState)
{
    mTimePos = animState.mTimePos;
    mLength = animState.mLength;
    mWeight = animState.mWeight;
    mEnabled = animState.mEnabled;
    mLoop = animState.mLoop;
	mParent->_notifyDirty();

}
//---------------------------------------------------------------------
void CCAnimationState::setBlendMaskEntry(size_t boneHandle, float weight)
{
    assert(mBlendMask && mBlendMask->size() > boneHandle);
    (*mBlendMask)[boneHandle] = weight;
    if (mEnabled)
    mParent->_notifyDirty();
}
//---------------------------------------------------------------------
void CCAnimationState::_setBlendMaskData(const float* blendMaskData) 
{
    assert(mBlendMask && "No BlendMask set!");
    // input 0?
    if(!blendMaskData)
    {
    destroyBlendMask();
    return;
    }
    // dangerous memcpy
    memcpy(&((*mBlendMask)[0]), blendMaskData, sizeof(float) * mBlendMask->size());
    if (mEnabled)
    mParent->_notifyDirty();
}
//---------------------------------------------------------------------
void CCAnimationState::_setBlendMask(const BoneBlendMask* blendMask) 
{
    if(!mBlendMask)
    {
    createBlendMask(blendMask->size(), false);
    }
    _setBlendMaskData(&(*blendMask)[0]);
}
//---------------------------------------------------------------------
void CCAnimationState::createBlendMask(size_t blendMaskSizeHint, float initialWeight)
{
	if(!mBlendMask)
	{
		if(initialWeight >= 0)
		{
			mBlendMask = new BoneBlendMask();
			mBlendMask->resize( blendMaskSizeHint, initialWeight);
		}
		else
		{
			mBlendMask = new BoneBlendMask();
			mBlendMask->resize( blendMaskSizeHint);
		}
	}
}
//---------------------------------------------------------------------
void CCAnimationState::destroyBlendMask()
{
	if (mBlendMask != 0)
	{
		delete mBlendMask;
		mBlendMask = 0;
	}
}

//---------------------------------------------------------------------
CCAnimationStateSet::CCAnimationStateSet()
	: mDirtyFrameNumber(std::numeric_limits<unsigned long>::max())
{
}
//---------------------------------------------------------------------
CCAnimationStateSet::CCAnimationStateSet(const CCAnimationStateSet& rhs)
	: mDirtyFrameNumber(std::numeric_limits<unsigned long>::max())
{
	for (AnimationStateMap::const_iterator i = rhs.mAnimationStates.begin();
		i != rhs.mAnimationStates.end(); ++i)
	{
		CCAnimationState* src = i->second;
		mAnimationStates[src->getAnimationName()] = new CCAnimationState(this, *src);
	}

    // Clone enabled animation state list
    for (EnabledAnimationStateList::const_iterator it = rhs.mEnabledAnimationStates.begin();
        it != rhs.mEnabledAnimationStates.end(); ++it)
    {
        const CCAnimationState* src = *it;
        mEnabledAnimationStates.push_back(getAnimationState(src->getAnimationName()));
    }
}
//---------------------------------------------------------------------
CCAnimationStateSet::~CCAnimationStateSet()
{
	// Destroy
	removeAllAnimationStates();
}
//---------------------------------------------------------------------
void CCAnimationStateSet::removeAnimationState(const std::string& name)
{
	AnimationStateMap::iterator i = mAnimationStates.find(name);
	if (i != mAnimationStates.end())
	{
        mEnabledAnimationStates.remove(i->second);
		if (i->second != 0)
		{
			delete i->second;
			i->second = 0;
		}
		mAnimationStates.erase(i);
	}
}
//---------------------------------------------------------------------
void CCAnimationStateSet::removeAllAnimationStates(void)
{
	for (AnimationStateMap::iterator i = mAnimationStates.begin();
		i != mAnimationStates.end(); ++i)
	{
		if (i->second != 0)
		{
			delete i->second;
			i->second = 0;
		}
	}
	mAnimationStates.clear();
    mEnabledAnimationStates.clear();
}
//---------------------------------------------------------------------
CCAnimationState* CCAnimationStateSet::createAnimationState(const std::string& name,  
	float timePos, float length, float weight, bool enabled)
{
	AnimationStateMap::iterator i = mAnimationStates.find(name);
	if (i != mAnimationStates.end())
	{
		assert(false && "State for animation named AnimationStateSet::createAnimationState already exists.");
	}

	CCAnimationState* newState = new CCAnimationState(name, this, timePos, length, weight, enabled);
	mAnimationStates[name] = newState;

	return newState;
}
//---------------------------------------------------------------------
CCAnimationState* CCAnimationStateSet::getAnimationState(const std::string& name) const
{
	AnimationStateMap::const_iterator i = mAnimationStates.find(name);
	if (i == mAnimationStates.end())
	{
		assert(false && "No state found for animation named AnimationStateSet::getAnimationState");
	}
	return i->second;
}
//---------------------------------------------------------------------
bool CCAnimationStateSet::hasAnimationState(const std::string& name) const
{
	//OGRE_LOCK_AUTO_MUTEX

	return mAnimationStates.find(name) != mAnimationStates.end();
}

//---------------------------------------------------------------------
void CCAnimationStateSet::_notifyDirty(void)
{
	//OGRE_LOCK_AUTO_MUTEX
    ++mDirtyFrameNumber;
}
//---------------------------------------------------------------------
void CCAnimationStateSet::_notifyAnimationStateEnabled(CCAnimationState* target, bool enabled)
{
    mEnabledAnimationStates.remove(target);

    // Add to enabled animation state list if need
    if (enabled)
    {
        mEnabledAnimationStates.push_back(target);
    }

    // Set the dirty frame number
    _notifyDirty();
}

NS_CC_END

