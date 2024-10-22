#ifndef DreamMeshManager_h__
#define DreamMeshManager_h__

#include "CCResObjManager.h"
#include <string>

NS_CC_BEGIN

class CCMeshManager: public ResObjManager
{
public:
	static CCMeshManager* sharedCCMeshManger();
	virtual ~CCMeshManager();
	virtual BaseObj* Create(const std::string& res_name);

private:
	CCMeshManager();

};

NS_CC_END

#endif