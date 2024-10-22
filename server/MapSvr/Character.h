#pragma once
#include "Object.h"

class NetMsgBody;

class Character : public Object
{
public:
	Character();
	~Character();

	virtual BOOL	Init();
	virtual VOID	Release();
	virtual VOID	Update( DWORD dwDeltaTick );
	virtual BOOL	SendPacket(NetMsgBody * pMsg) { return TRUE; }

	virtual DWORD	GetCharID() const { return 0; }
	virtual BYTE	GetRace() const { return 0; }
	virtual BYTE	GetGender() const { return 0; }
	virtual DWORD	GetMonsterID() const { return 0; }
	virtual USHORT	GetLevel() const { return 0; }
	virtual const char*	GetRoleName() = 0;
};