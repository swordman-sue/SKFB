#ifndef __SkeletonAnimation_H__
#define __SkeletonAnimation_H__

#include <map>
#include <set>
using namespace std;

NS_CC_BEGIN

class CCMySkeleton;
class NodeAnimationTrack;
struct CCBone;

class CCMeshSkeletonAnimation
{
public:
    CCMeshSkeletonAnimation(const std::string& name, float length);
	
    virtual ~CCMeshSkeletonAnimation();

    const std::string& getName(void) const;

    float getLength(void) const;

	void setLength(float len);

    NodeAnimationTrack* createNodeTrack(unsigned short handle);

    NodeAnimationTrack* createNodeTrack(unsigned short handle, CCBone* bone);

    unsigned short getNumNodeTracks(void) const;

    /** Gets a node track by it's handle. */
    NodeAnimationTrack* getNodeTrack(unsigned short handle) const;

	/** Does a track exist with the given handle? */
	bool hasNodeTrack(unsigned short handle) const;

    void destroyNodeTrack(unsigned short handle);

	void destroyAllNodeTracks(void);

    void apply(CCMySkeleton* skeleton, float timePos, float weight = 1.0);

    void apply(CCMySkeleton* skeleton, float timePos, float weight, const CCAnimationState::BoneBlendMask* blendMask);

	typedef std::map<unsigned short, NodeAnimationTrack*> NodeTrackList;

    const NodeTrackList& _getNodeTrackList(void) const;

	void optimise(bool discardIdentityNodeTracks = true);

	typedef std::set<unsigned short> TrackHandleList;

    void _destroyNodeTracks(const TrackHandleList& tracks);

    void _keyFrameListChanged(void) { mKeyFrameTimesDirty = true; }

    TimeIndex _getTimeIndex(float timePos) const;

protected:
    NodeTrackList mNodeTrackList;
    std::string mName;
    float mLength;			//¶¯»­³¤¶È

	typedef std::vector<float> KeyFrameTimeList;
    mutable KeyFrameTimeList mKeyFrameTimes;
    mutable bool mKeyFrameTimesDirty;

	void optimiseNodeTracks(bool discardIdentityTracks);

    void buildKeyFrameTimeList(void) const;
};

NS_CC_END

#endif

