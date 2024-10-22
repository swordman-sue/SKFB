#ifndef __AnimationTrack_H__
#define __AnimationTrack_H__

#include <vector>
using namespace std;

NS_CC_BEGIN

class CCMeshSkeletonAnimation;
class Node;
struct CCTransformKeyFrame;
struct CCBone;

class TimeIndex
{
protected:
    /** The time position (in relation to the whole animation sequence)
    */
    float mTimePos;
    size_t mKeyIndex;

    /** Indicate it's an invalid global keyframe index.
    */
    static const size_t INVALID_KEY_INDEX = (size_t)-1;

public:
    /** Construct time index object by the given time position.
    */
    TimeIndex(float timePos)
        : mTimePos(timePos)
        , mKeyIndex(INVALID_KEY_INDEX)
    {
    }

    TimeIndex(float timePos, size_t keyIndex)
        : mTimePos(timePos)
        , mKeyIndex(keyIndex)
    {
    }

    bool hasKeyIndex(void) const
    {
        return mKeyIndex != INVALID_KEY_INDEX;
    }

    float getTimePos(void) const
    {
        return mTimePos;
    }

    size_t getKeyIndex(void) const
    {
        return mKeyIndex;
    }
};

class AnimationTrack
{
public:
    /// Constructor
    AnimationTrack(CCMeshSkeletonAnimation* parent, unsigned short handle);

    virtual ~AnimationTrack();

	/** Get the handle associated with this track. */
	unsigned short getHandle(void) const { return mHandle; }

    /** Returns the number of keyframes in this animation. */
    virtual size_t getNumKeyFrames(void) const;

    /** Returns the KeyFrame at the specified index. */
    virtual CCTransformKeyFrame* getKeyFrame(size_t index) const;

    /** Gets the 2 KeyFrame objects which are active at the time given, and the blend value between them.
    @remarks
        At any point in time  in an animation, there are either 1 or 2 keyframes which are 'active',
        1 if the time index is exactly on a keyframe, 2 at all other times i.e. the keyframe before
        and the keyframe after.
    @par
        This method returns those keyframes given a time index, and also returns a parametric
        value indicating the value of 't' representing where the time index falls between them.
        E.g. if it returns 0, the time index is exactly on keyFrame1, if it returns 0.5 it is
        half way between keyFrame1 and keyFrame2 etc.
    @param timeIndex The time index.
    @param keyFrame1 Pointer to a KeyFrame pointer which will receive the pointer to the 
        keyframe just before or at this time index.
    @param keyFrame2 Pointer to a KeyFrame pointer which will receive the pointer to the 
        keyframe just after this time index. 
    @param firstKeyIndex Pointer to an unsigned short which, if supplied, will receive the 
        index of the 'from' keyframe in case the caller needs it.
    @returns Parametric value indicating how far along the gap between the 2 keyframes the timeIndex
        value is, e.g. 0.0 for exactly at 1, 0.25 for a quarter etc. By definition the range of this 
        value is:  0.0 <= returnValue < 1.0 .
    */
    virtual float getKeyFramesAtTime(const TimeIndex& timeIndex, CCTransformKeyFrame** keyFrame1, CCTransformKeyFrame** keyFrame2,
        unsigned short* firstKeyIndex = 0) const;

    /** Creates a new KeyFrame and adds it to this animation at the given time index.
    @remarks
        It is better to create KeyFrames in time order. Creating them out of order can result 
        in expensive reordering processing. Note that a KeyFrame at time index 0.0 is always created
        for you, so you don't need to create this one, just access it using getKeyFrame(0);
    @param timePos The time from which this KeyFrame will apply.
    */
    virtual CCTransformKeyFrame* createKeyFrame(float timePos);

    /** Removes a KeyFrame by it's index. */
    virtual void removeKeyFrame(size_t index);

    /** Removes all the KeyFrames from this track. */
    virtual void removeAllKeyFrames(void);

    virtual void getInterpolatedKeyFrame(const TimeIndex& timeIndex, CCTransformKeyFrame* kf) const = 0;

    /** Internal method to collect keyframe times, in unique, ordered format. */
	virtual void _collectKeyFrameTimes(std::vector<float>& keyFrameTimes);

    /** Internal method to build keyframe time index map to translate global lower
        bound index to local lower bound index. */
	virtual void _buildKeyFrameIndexMap(const std::vector<float>& keyFrameTimes);

	/** Returns the parent Animation object for this track. */
	CCMeshSkeletonAnimation *getParent() const { return mParent; }
protected:
	typedef std::vector<CCTransformKeyFrame*> KeyFrameList;
    KeyFrameList mKeyFrames;
    CCMeshSkeletonAnimation* mParent;
	unsigned short mHandle;

	typedef std::vector<size_t> KeyFrameIndexMap;
    KeyFrameIndexMap mKeyFrameIndexMap;
};

/** Specialised AnimationTrack for dealing with node transforms.
*/
class NodeAnimationTrack : public AnimationTrack
{
public:
	/// Constructor
	NodeAnimationTrack(CCMeshSkeletonAnimation* parent, unsigned short handle);
    /// Destructor
    virtual ~NodeAnimationTrack();

	/** Returns a pointer to the associated Node object (if any). */
	virtual CCBone* getAssociatedNode(void) const;

	/** Sets the associated Node object which will be automatically affected by calls to 'apply'. */
	virtual void setAssociatedNode(CCBone* node);

	/** As the 'apply' method but applies to a specified Node instead of associated node. */
	virtual void applyToNode(CCBone* bone, const TimeIndex& timeIndex, float weight = 1.0);

	/// @copydoc AnimationTrack::getInterpolatedKeyFrame
	virtual void getInterpolatedKeyFrame(const TimeIndex& timeIndex, CCTransformKeyFrame* kf) const;

protected:
	CCBone* m_target_bone;
};

NS_CC_END

#endif