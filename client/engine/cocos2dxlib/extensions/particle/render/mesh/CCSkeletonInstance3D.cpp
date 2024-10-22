#include "cocos/base/ccTypes.h"
#include "math/CCMath.h"
#include "particle/render/mesh/bytebuffer.h"
#include "CCSkeletonData.h"
#include "CCAnimationState.h"
#include "CCAnimationTrack.h"
#include "CCMeshSkeletonAnimation.h"
#include "CCMySkeleton3D.h"

#include "CCSkeletonInstance3D.h"
#include "CCMyMesh.h"
#include <assert.h>


NS_CC_BEGIN

SkeletonInstance::SkeletonInstance( CCMySkeleton* masterCopy, CCMyMesh* mesh) 
    : CCMySkeleton()
    , mSkeleton(masterCopy)
	, m_buffer_size(0)
	, m_vertex_buffer(0)
{
	loadImpl(mesh);
}
//-------------------------------------------------------------------------
SkeletonInstance::~SkeletonInstance()
{
	if (m_vertex_buffer != 0)
	{
		free(m_vertex_buffer);
	}
}
//-------------------------------------------------------------------------
unsigned short SkeletonInstance::getNumAnimations(void) const
{
    return mSkeleton->getNumAnimations();
}
//-------------------------------------------------------------------------
CCMeshSkeletonAnimation* SkeletonInstance::getAnimation(unsigned short index) const
{
    return mSkeleton->getAnimation(index);
}
//-------------------------------------------------------------------------
CCMeshSkeletonAnimation* SkeletonInstance::createAnimation(const std::string& name, float length)
{
    return mSkeleton->createAnimation(name, length);
}
//-------------------------------------------------------------------------
void SkeletonInstance::removeAnimation(const std::string& name)
{
    mSkeleton->removeAnimation(name);
}
//-------------------------------------------------------------------------
void SkeletonInstance::_initAnimationState(CCAnimationStateSet* animSet)
{
	mSkeleton->_initAnimationState(animSet);
}
//-------------------------------------------------------------------------
void SkeletonInstance::_refreshAnimationState(CCAnimationStateSet* animSet)
{
	mSkeleton->_refreshAnimationState(animSet);
}
//-------------------------------------------------------------------------
void SkeletonInstance::cloneBone(CCBone* source)
{
	CCBone* bone = createBone();
	bone->pos.set(source->pos);
	bone->scale.set(source->scale);
	bone->quat.set(source->quat);
}
//-------------------------------------------------------------------------
void SkeletonInstance::loadImpl(CCMyMesh* mesh)
{
    const BoneList* bone_list = mSkeleton->GetBoneList();
	BoneList::const_iterator it = bone_list->begin(), ie = bone_list->end();
	for (; it < ie; ++it)
	{
		cloneBone(*it);
	}

	SetBindingPose();

	m_buffer_size = mesh->GetVertexBufferSize();
	m_vertex_buffer = (unsigned char*)malloc(m_buffer_size);
	memcpy(m_vertex_buffer, mesh->GetVertexBuffer(), m_buffer_size);
}

CCMeshSkeletonAnimation* SkeletonInstance::_getAnimationImpl( const std::string& name ) const
{
	return mSkeleton->_getAnimationImpl(name);
}

NS_CC_END

