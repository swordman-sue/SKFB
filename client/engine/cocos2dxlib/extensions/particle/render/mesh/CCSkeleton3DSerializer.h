#ifndef __SkeletonSerializer_H__
#define __SkeletonSerializer_H__

#include "CCSerializer.h"

NS_CC_BEGIN

class CCMySkeleton;
class ByteBuffer;
struct CCBone;
struct CCTransformKeyFrame;

class CCSkeletonSerializer : public Serializer
{
public:
    CCSkeletonSerializer();
    virtual ~CCSkeletonSerializer();

    void importSkeleton(ByteBuffer* stream, CCMySkeleton* pDest);

protected:
    void readBone(ByteBuffer* stream, CCMySkeleton* pSkel);
    void readAnimation(ByteBuffer* stream, CCMySkeleton* pSkel);
    void readAnimationTrack(ByteBuffer* stream, CCMeshSkeletonAnimation* anim, CCMySkeleton* pSkel);
    void readKeyFrame(ByteBuffer* stream, NodeAnimationTrack* track, CCMySkeleton* pSkel);
};

NS_CC_END

#endif
