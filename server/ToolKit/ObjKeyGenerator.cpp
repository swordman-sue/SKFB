#include "StdAfx.h"
#include "ObjKeyGenerator.h"

ObjKeyGenerator::ObjKeyGenerator(void)
{
}

ObjKeyGenerator::~ObjKeyGenerator(void)
{
}

void ObjKeyGenerator::Create(DWORD dwStartKey, DWORD dwEndKey, DWORD dwStep /* = 1 */)
{
	for (DWORD i=dwStartKey; i<dwEndKey; i+=dwStep)
		m_dequeKey.push_back(i);
}

DWORD ObjKeyGenerator::Getkey()
{
	m_cs.Lock();

	if(m_dequeKey.empty())
	{
		m_cs.Unlock();
		return NULL;
	}

	DWORD dwKey;
	dwKey = m_dequeKey.front();
	m_dequeKey.pop_front();

	m_cs.Unlock();

	return dwKey;
}

void ObjKeyGenerator::RestoreKey(DWORD dwKey)
{
	m_cs.Lock();

	m_dequeKey.push_back(dwKey);

	m_cs.Unlock();
}

size_t ObjKeyGenerator::GetKeyNum() const
{
	return m_dequeKey.size();
}
