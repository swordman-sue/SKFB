#ifndef __AnimationSet_H__
#define __AnimationSet_H__

#include <list>
#include <map>
#include <vector>
#include <string>

NS_CC_BEGIN

class CCAnimationStateSet;

class CCAnimationState
{
public:
	typedef std::vector<float> BoneBlendMask;

    /// Normal constructor with all params supplied
	CCAnimationState(const std::string& animName, CCAnimationStateSet *parent, 
		float timePos, float length, float weight = 1.0, bool enabled = false);
	/// constructor to copy from an existing state with new parent
	CCAnimationState(CCAnimationStateSet* parent, const CCAnimationState &rhs);
	/** Destructor - is here because class has virtual functions and some compilers 
		would whine if it won't exist.
	*/
	virtual ~CCAnimationState();
        
    /// Gets the name of the animation to which this state applies
    const std::string& getAnimationName() const;
    /// Gets the time position for this animation
    float getTimePosition(void) const;
    /// Sets the time position for this animation
    void setTimePosition(float timePos);
    /// Gets the total length of this animation (may be shorter than whole animation)
    float getLength() const;
    /// Sets the total length of this animation (may be shorter than whole animation)
    void setLength(float len);
    /// Gets the weight (influence) of this animation
    float getWeight(void) const;
    /// Sets the weight (influence) of this animation
    void setWeight(float weight);
    /** Modifies the time position, adjusting for animation length
    @remarks
        This method loops at the edges if animation looping is enabled.
    */
    void addTime(float offset);

	/// Returns true if the animation has reached the end and is not looping
	bool hasEnded(void) const;

    /// Returns true if this animation is currently enabled
    bool getEnabled(void) const;
    /// Sets whether this animation is enabled
    void setEnabled(bool enabled);

    /// Equality operator
    bool operator==(const CCAnimationState& rhs) const;
    // Inequality operator
    bool operator!=(const CCAnimationState& rhs) const;

    /** Sets whether or not an animation loops at the start and end of
        the animation if the time continues to be altered.
    */
    void setLoop(bool loop) { mLoop = loop; }
    /// Gets whether or not this animation loops            
    bool getLoop(void) const { return mLoop; }
     
    /** Copies the states from another animation state, preserving the animation name
    (unlike operator=) but copying everything else.
    @param animState Reference to animation state which will use as source.
    */
    void copyStateFrom(const CCAnimationState& animState);

	/// Get the parent animation state set
	CCAnimationStateSet* getParent(void) const { return mParent; }

    /** @brief create a new blend mask with the given number of entries
    *
    * In addition to assigning a single weight value to a skeletal animation,
    * it may be desirable to assign animation weights per bone using a 'blend mask'.
    *
    * @param blendMaskSizeHint 
    *   The number of bones of the skeleton owning this AnimationState.
	* @param initialWeight
	*   The value all the blend mask entries will be initialised with (negative to skip initialisation)
    */
    void createBlendMask(size_t blendMaskSizeHint, float initialWeight = 1.0f);
    /// destroy the currently set blend mask
    void destroyBlendMask();
    /** @brief set the blend mask data (might be dangerous)
    *
    * @par The size of the array should match the number of entries the
    *      blend mask was created with.
    *
    * @par Stick to the setBlendMaskEntry method if you don't know exactly what you're doing.
    */
    void _setBlendMaskData(const float* blendMaskData);
    /** @brief set the blend mask
    *
    * @par The size of the array should match the number of entries the
    *      blend mask was created with.
    *
    * @par Stick to the setBlendMaskEntry method if you don't know exactly what you're doing.
    */
    void _setBlendMask(const BoneBlendMask* blendMask);
    /// get the current blend mask (const version, may be 0) 
    const BoneBlendMask* getBlendMask() const {return mBlendMask;}
    /// return whether there is currently a valid blend mask set
    bool hasBlendMask() const {return mBlendMask != 0;}
    /// set the weight for the bone identified by the given handle
    void setBlendMaskEntry(size_t boneHandle, float weight);
    /// get the weight for the bone identified by the given handle
    inline float getBlendMaskEntry(size_t boneHandle) const
    {
    return (*mBlendMask)[boneHandle];
    }
protected:
    /// the blend mask (containing per bone weights)
    BoneBlendMask* mBlendMask;

    std::string mAnimationName;
	CCAnimationStateSet* mParent;
    float mTimePos;
    float mLength;
    float mWeight;
    bool mEnabled;
    bool mLoop;
};

// A map of animation states
typedef std::map<std::string, CCAnimationState*> AnimationStateMap;

typedef std::list<CCAnimationState*> EnabledAnimationStateList;

/** Class encapsulating a set of AnimationState objects.
*/
class CCAnimationStateSet
{
public:
	/// Create a blank animation state set
	CCAnimationStateSet();
	/// Create an animation set by copying the contents of another
	CCAnimationStateSet(const CCAnimationStateSet& rhs);

	~CCAnimationStateSet();

	CCAnimationState* createAnimationState(const std::string& animName, float timePos, float length, float weight = 1.0, bool enabled = false);
	/// Get an animation state by the name of the animation
	CCAnimationState* getAnimationState(const std::string& name) const;
	/// Tests if state for the named animation is present
	bool hasAnimationState(const std::string& name) const;
	/// Remove animation state with the given name
	void removeAnimationState(const std::string& name);
	/// Remove all animation states
	void removeAllAnimationStates(void);
	/// Set the dirty flag and dirty frame number on this state set
	void _notifyDirty(void);
	/// Get the latest animation state been altered frame number
	unsigned long getDirtyFrameNumber(void) const { return mDirtyFrameNumber; }

    /// Internal method respond to enable/disable an animation state
    void _notifyAnimationStateEnabled(CCAnimationState* target, bool enabled);
    /// Tests if exists enabled animation state in this set
    bool hasEnabledAnimationState(void) const { return !mEnabledAnimationStates.empty(); }

	const EnabledAnimationStateList* GetEnabledAnimationStateList() const { return &mEnabledAnimationStates; }
protected:
	unsigned long mDirtyFrameNumber;
	AnimationStateMap mAnimationStates;
    EnabledAnimationStateList mEnabledAnimationStates;
};

NS_CC_END

#endif

