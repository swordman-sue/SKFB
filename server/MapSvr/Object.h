#pragma once

#include "../Common/Const.h"

class Object
{
public:
	Object(void);
	virtual ~Object(void);

public:
	virtual BOOL		Init();
	virtual VOID		Release();	
	virtual VOID		Update( DWORD dwDeltaTick );

	DWORD				GetObjectKey() const { return m_dwObjectKey; }
	VOID				SetObjectKey(DWORD dwObjectKey) { m_dwObjectKey = dwObjectKey; }

	OBJECT_TYPE			GetObjectType() const { return m_enObjectType; }
	VOID				SetObjectType(OBJECT_TYPE enType) { m_enObjectType = enType; }

private:
	DWORD				m_dwObjectKey;
	OBJECT_TYPE			m_enObjectType;
};