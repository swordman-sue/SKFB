
#include "CCMeshManager.h"
#include "CCMyMesh.h"

#include "platform/CCFileUtils.h"
#include "math/CCMath.h"
#include "cocos/base/ccMacros.h"

NS_CC_BEGIN

	CCMeshManager::CCMeshManager()
{
}

CCMeshManager::~CCMeshManager()
{
}

CCMeshManager* CCMeshManager::sharedCCMeshManger()
{
	static CCMeshManager instance;
	return &instance;
}

BaseObj* CCMeshManager::Create(const std::string& res_name)
{
	BaseObj* mesh = GetByName(res_name);
	if(!mesh)
	{
		Data data = FileUtils::getInstance()->getDataFromFile(res_name);
		unsigned char* data_ptr=data.getBytes();
		mesh = new CCMyMesh(res_name);
		mesh->autorelease();

		mesh->Load(data_ptr, data.getSize());
		mesh->OnCreated();

		AddResource(mesh);
	}
	return mesh;
}

NS_CC_END