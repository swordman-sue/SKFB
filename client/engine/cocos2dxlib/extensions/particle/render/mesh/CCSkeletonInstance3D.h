#ifndef __SkeletonInstance_H__
#define __SkeletonInstance_H__

#include <string>

NS_CC_BEGIN

class CCMySkeleton;
struct CCBone;
class CCMyMesh;
class CCMeshSkeletonAnimation;
class CCAnimationStateSet;

class SkeletonInstance : public CCMySkeleton
{
public:

    SkeletonInstance( CCMySkeleton* masterCopy, CCMyMesh* mesh);

    ~SkeletonInstance();

    unsigned short getNumAnimations(void) const;

    CCMeshSkeletonAnimation* getAnimation(unsigned short index) const;

    CCMeshSkeletonAnimation* createAnimation(const std::string& name, float length);

    void removeAnimation(const std::string& name);

	void _initAnimationState(CCAnimationStateSet* animSet);

	/// @copydoc Skeleton::_refreshAnimationState
	void _refreshAnimationState(CCAnimationStateSet* animSet);

	/// Internal accessor for animations (returns null if animation does not exist)
	CCMeshSkeletonAnimation* _getAnimationImpl(const std::string& name) const;

	unsigned int GetBufferSize() { return m_buffer_size; }

	unsigned char* GetBuffer() { return m_vertex_buffer; }

protected:

    CCMySkeleton* mSkeleton;

    void cloneBone(CCBone* source);

    void loadImpl(CCMyMesh* mesh);

private:
	unsigned char* m_vertex_buffer;
	unsigned int m_buffer_size;
};

NS_CC_END


#endif

