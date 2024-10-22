#include "cocos/base/ccTypes.h"
#include "math/CCMath.h"
#include "CCSkeletonData.h"
#include "CCAnimationState.h"
#include "CCAnimationTrack.h"
#include "CCMeshSkeletonAnimation.h"
#include "CCMySkeleton3D.h"
#include "CCSkeletonFileFormat.h"
#include "CCSkeleton3DSerializer.h"

NS_CC_BEGIN

CCSkeletonSerializer::CCSkeletonSerializer()
{
    mVersion = "[Serializer_v1.10]";
}
//---------------------------------------------------------------------
CCSkeletonSerializer::~CCSkeletonSerializer()
{
}

//---------------------------------------------------------------------
void CCSkeletonSerializer::importSkeleton(ByteBuffer* stream, CCMySkeleton* pSkel)
{
    readFileHeader(stream);

    unsigned short streamID;
    while(!eof(stream))
    {
        streamID = readChunk(stream);
        switch (streamID)
        {
        case SKELETON_BONE:
            readBone(stream, pSkel);
            break;
        case SKELETON_ANIMATION:
            readAnimation(stream, pSkel);
			break;
        }
    }

	pSkel->SetBindingPose();
}
//---------------------------------------------------------------------
void CCSkeletonSerializer::readBone(ByteBuffer* stream, CCMySkeleton* pSkel)
{
    unsigned short handle;
    readShorts(stream, &handle, 1);
    CCBone* pBone = pSkel->createBone(handle);
    readObject(stream, pBone->pos);
    readObject(stream, pBone->quat);
    readObject(stream, pBone->scale);
}

//---------------------------------------------------------------------
void CCSkeletonSerializer::readAnimation(ByteBuffer* stream, CCMySkeleton* pSkel)
{
    std::string name;
    name = readString(stream);
    float len;
    readFloats(stream, &len, 1);

    CCMeshSkeletonAnimation* pAnim = pSkel->createAnimation(name, len);

    // Read all tracks
    if (!eof(stream))
    {
        unsigned short streamID = readChunk(stream);
        while(streamID == SKELETON_ANIMATION_TRACK && !eof(stream))
        {
            readAnimationTrack(stream, pAnim, pSkel);
            if (!eof(stream))
            {
                streamID = readChunk(stream);
            }
        }
        if (!eof(stream))
        {
            skip(stream, -STREAM_OVERHEAD_SIZE);
        }
    }
}
//---------------------------------------------------------------------
void CCSkeletonSerializer::readAnimationTrack(ByteBuffer* stream, CCMeshSkeletonAnimation* anim, CCMySkeleton* pSkel)
{
    unsigned short boneHandle;
    readShorts(stream, &boneHandle, 1);

    CCBone *targetBone = pSkel->getBone(boneHandle);
    NodeAnimationTrack* pTrack = anim->createNodeTrack(boneHandle, targetBone);

    if (!eof(stream))
    {
        unsigned short streamID = readChunk(stream);
        while(streamID == SKELETON_ANIMATION_TRACK_KEYFRAME && !eof(stream))
        {
            readKeyFrame(stream, pTrack, pSkel);

            if (!eof(stream))
            {
                streamID = readChunk(stream);
            }
        }
        if (!eof(stream))
        {
            skip(stream, -STREAM_OVERHEAD_SIZE);
        }
    }
}
//---------------------------------------------------------------------
void CCSkeletonSerializer::readKeyFrame(ByteBuffer* stream, NodeAnimationTrack* track, CCMySkeleton* pSkel)
{
    float time;
    readFloats(stream, &time, 1);
    CCTransformKeyFrame *kf = track->createKeyFrame(time);
    readObject(stream, kf->quat);
    readObject(stream, kf->pos);
    readObject(stream, kf->scale);
}

NS_CC_END