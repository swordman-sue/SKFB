#ifndef CCResObjManager_h__
#define CCResObjManager_h__

#include "cocos/base/ccTypes.h"
#include "cocos/base/CCRef.h"
#include <string>
#include <map>
#include <vector>

NS_CC_BEGIN

class ByteBuffer;

class BaseObj:public Ref
{
public:
	BaseObj(std::string name):m_res_name(name),m_res_stream(0)
	{

	}
	virtual ~BaseObj()
	{

	}

	virtual std::string GetResName()
	{
		return m_res_name;
	}
	virtual void OnCreated() = 0;

	virtual void Load(const unsigned char* data_ptr, unsigned int data_len);
	virtual void Unload();

protected:
	std::string m_res_name;
	ByteBuffer* m_res_stream;
};

class ResObjManager: public Ref
{
public:
	ResObjManager();
	virtual ~ResObjManager();

	virtual BaseObj* Create(const std::string& res_name) = 0;
	virtual BaseObj* GetByName(const std::string& name);
	void Remove(const std::string& name);

protected:
	void AddResource(BaseObj* resource);
		
protected:
	typedef std::map<std::string, BaseObj*> RenderResourceNameMap;
	RenderResourceNameMap m_name_map;
};

NS_CC_END

#endif // CCResObjManager_h__

