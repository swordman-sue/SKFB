
#include "CCResObjManager.h"
#include "base/CCDirector.h"

#include "particle/render/mesh/bytebuffer.h"

NS_CC_BEGIN



void BaseObj::Load(const unsigned char* data_ptr, unsigned int data_len)
{
	m_res_stream = new ByteBuffer(data_len);
	m_res_stream->append(data_ptr, data_len);
}
void BaseObj::Unload()
{
	if(m_res_stream!=0)
	{
		delete m_res_stream;
		m_res_stream = 0;
	}
}

ResObjManager::ResObjManager()
{
}

ResObjManager::~ResObjManager()
{
	for (RenderResourceNameMap::iterator iter = m_name_map.begin();iter!=m_name_map.end();iter++)
	{
		iter->second->Unload();
		iter->second->release();
		iter->second = 0;
	}
	m_name_map.clear();
}

BaseObj* ResObjManager::GetByName(const std::string& name )
{
	RenderResourceNameMap::iterator ires = m_name_map.find(name);
	if(ires!=m_name_map.end())
	{
		return ires->second;
	}
	else
	{
		return 0;
	}
}

void ResObjManager::Remove(const std::string& name)
{
	RenderResourceNameMap::iterator iter = m_name_map.find(name);
	if (iter!=m_name_map.end() && (iter->second->getReferenceCount()==1))
	{
		iter->second->Unload();
		iter->second->release();
		iter->second = 0;
		m_name_map.erase(name);
	}
}

void ResObjManager::AddResource(BaseObj* resource)
{
	RenderResourceNameMap::iterator iter = m_name_map.find(resource->GetResName());
	if (iter!=m_name_map.end())
	{
		iter->second->retain();
	}
	else
	{
		resource->retain();
		m_name_map.insert(std::make_pair(resource->GetResName(),resource));
	}
}


NS_CC_END
