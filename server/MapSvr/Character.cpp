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
	//最后调用基类的函数
	return Object::Init();
}

VOID Character::Release()
{
	//最后调用基类的函数
	Object::Release();
}

VOID Character::Update( DWORD dwDeltaTick )
{

}