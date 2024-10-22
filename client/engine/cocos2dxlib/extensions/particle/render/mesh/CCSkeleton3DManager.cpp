
#include "CCSkeleton3DManager.h"
#include "CCMySkeleton3D.h"

#include "platform/CCFileUtils.h"
#include "cocos/base/ccMacros.h"

NS_CC_BEGIN

CCSkeleton3DManager::CCSkeleton3DManager()
{
}

CCSkeleton3DManager::~CCSkeleton3DManager()
{
}

CCSkeleton3DManager* CCSkeleton3DManager::sharedCCSkeletonManger()
{
	static CCSkeleton3DManager instance;
	return &instance;
}

BaseObj* CCSkeleton3DManager::Create(const std::string& res_name)
{
	BaseObj* skeleton_3d = GetByName(res_name);
	if(!skeleton_3d)
	{
		Data data = FileUtils::getInstance()->getDataFromFile(res_name);
		unsigned char* data_ptr=data.getBytes();
		skeleton_3d = new CCMySkeleton(res_name);
		skeleton_3d->autorelease();

		skeleton_3d->Load(data_ptr, data.getSize());
		skeleton_3d->OnCreated();


		AddResource(skeleton_3d);
	}
	return skeleton_3d;
}

NS_CC_END

