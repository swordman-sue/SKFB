#ifndef __OgreSkeletonManager_H__
#define __OgreSkeletonManager_H__

#include "CCResObjManager.h"

NS_CC_BEGIN
	
class CCSkeleton3DManager : public ResObjManager
{
public:
	static CCSkeleton3DManager* sharedCCSkeletonManger();
	virtual ~CCSkeleton3DManager();

	virtual BaseObj* Create(const std::string& res_name);
private:
	CCSkeleton3DManager();
};

NS_CC_END

#endif

