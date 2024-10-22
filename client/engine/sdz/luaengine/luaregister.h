
#ifndef sdz_luaregister_h
#define sdz_luaregister_h

#include <string>
#include "luatinker/lua_tinker.h"

// 集中处理注册给lua引擎的类和函数

class LuaRegister
{
public:
	LuaRegister(){};
	~LuaRegister(){};

	// 注册配置池
	static void Regist(lua_State* ls);
};


#endif
