
#ifndef sdz_luaregister_h
#define sdz_luaregister_h

#include <string>
#include "luatinker/lua_tinker.h"

// ���д���ע���lua�������ͺ���

class LuaRegister
{
public:
	LuaRegister(){};
	~LuaRegister(){};

	// ע�����ó�
	static void Regist(lua_State* ls);
};


#endif
