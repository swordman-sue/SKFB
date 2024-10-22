#include "stdafx.h"
#include "Object.h"

Object::Object()
{
	m_dwObjectKey = 0;
	m_enObjectType = OBJECT_NONE;
}

Object::~Object()
{

}

BOOL Object::Init()
{
	return TRUE;
}

VOID Object::Release()
{

}

VOID Object::Update( DWORD dwDeltaTick )
{

}
