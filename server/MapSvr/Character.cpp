#include "stdafx.h"
#include "Character.h"

Character::Character()
{
}

Character::~Character()
{

}

BOOL Character::Init()
{
	//�����û���ĺ���
	return Object::Init();
}

VOID Character::Release()
{
	//�����û���ĺ���
	Object::Release();
}

VOID Character::Update( DWORD dwDeltaTick )
{

}