// lua_tinker.h
//
// LuaTinker - Simple and light C++ wrapper for Lua.
//
// Copyright (c) 2005-2007 Kwon-il Lee (zupet@hitel.net)
// 
// please check Licence.txt file for licence and legal issues. 

#if !defined(_LUA_TINKER_H_)
#define _LUA_TINKER_H_

extern "C" 
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

#include <new>
#include <string.h>
#include <string>
#include "zqcommon/logutil.h"
#include "math/Vec2.h"

namespace lua_tinker
{
	// init LuaTinker
	void	init(lua_State *L);

	void	init_s64(lua_State *L);
	void	init_u64(lua_State *L);

	// string-buffer excution
	void	dofile(lua_State *L, const char *filename);
	void	dostring(lua_State *L, const char* buff);
	void	dobuffer(lua_State *L, const char* buff, size_t sz);
	
	// debug helpers
	void	enum_stack(lua_State *L);
	int		on_error(lua_State *L);
	void	print_error(lua_State *L, const char* fmt, ...);

	// class helper
	int meta_get(lua_State *L);
	int meta_set(lua_State *L);
	int dispatcher(lua_State *L);
	int static_meta_get(lua_State *L);
	int static_dispatcher(lua_State *L);
	void push_meta(lua_State *L, const char* name);
	void push_register(lua_State *L, const char* keys, const char* values);
	void get_register(lua_State *L, const char* keys);

	void push_dispatcher(lua_State* L);
	void push_staticdispatcher(lua_State* L);
	void push_currfuncname(lua_State* L, const char* func_name);
	void get_currfuncname(lua_State *L);
	void push_currtable(lua_State* L);
	void get_currtable(lua_State* L);
	void invoke_child(lua_State* L, const char* name);
	void to_dispatcher();
	// dynamic type extention
	struct lua_value
	{
		virtual void to_lua(lua_State *L) = 0;
	};

	/*---------------------------------------------------------------------------*/ 
	/* 添加命名空间结构体，保存当前命名空间名称									 */ 
	/*---------------------------------------------------------------------------*/ 
	struct space_name
	{
		// global name
		static const char* name(const char* n = NULL)
		{
			static char temp[256] = "";
			if(n) strcpy(temp, n);
			return temp;
		}
	};

	// type trait
	template<typename T> struct class_name;
	struct table;

	template<bool C, typename A, typename B> struct if_ {};
	template<typename A, typename B>		struct if_<true, A, B> { typedef A type; };
	template<typename A, typename B>		struct if_<false, A, B> { typedef B type; };

	template<typename A>
	struct is_ptr { static const bool value = false; };
	template<typename A>
	struct is_ptr<A*> { static const bool value = true; };

	template<typename A>
	struct is_ref { static const bool value = false; };
	template<typename A>
	struct is_ref<A&> { static const bool value = true; };

	template<typename A>
	struct remove_const { typedef A type; };
	template<typename A>
	struct remove_const<const A> { typedef A type; };

	template<typename A>
	struct base_type { typedef A type; };
	template<typename A>
	struct base_type<A*> { typedef A type; };
	template<typename A>
	struct base_type<A&> { typedef A type; };

	template<typename A>
	struct class_type { typedef typename remove_const<typename base_type<A>::type>::type type; };
	
	template<typename A>
	struct is_obj { static const bool value = true; };
	template<> struct is_obj<char>					{ static const bool value = false; };
	template<> struct is_obj<unsigned char>			{ static const bool value = false; };
	template<> struct is_obj<short>					{ static const bool value = false; };
	template<> struct is_obj<unsigned short>		{ static const bool value = false; };
	template<> struct is_obj<long>					{ static const bool value = false; };
	template<> struct is_obj<unsigned long>			{ static const bool value = false; };
	template<> struct is_obj<int>					{ static const bool value = false; };
	template<> struct is_obj<unsigned int>			{ static const bool value = false; };
	template<> struct is_obj<float>					{ static const bool value = false; };
	template<> struct is_obj<double>				{ static const bool value = false; };
	template<> struct is_obj<char*>					{ static const bool value = false; };
	template<> struct is_obj<const char*>			{ static const bool value = false; };
	template<> struct is_obj<bool>					{ static const bool value = false; };
	template<> struct is_obj<lua_value*>			{ static const bool value = false; };
	template<> struct is_obj<long long>				{ static const bool value = false; };
	template<> struct is_obj<unsigned long long>	{ static const bool value = false; };
	template<> struct is_obj<std::string>			{ static const bool value = false; };
	template<> struct is_obj<const std::string&>	{ static const bool value = false; };
	template<> struct is_obj<table>					{ static const bool value = false; };

	/////////////////////////////////
	enum { no = 1, yes = 2 }; 
	typedef char (& no_type )[no]; 
	typedef char (& yes_type)[yes]; 

	struct int_conv_type { int_conv_type(int); }; 

	no_type int_conv_tester (...); 
	yes_type int_conv_tester (int_conv_type); 

	no_type vfnd_ptr_tester (const volatile char *); 
	no_type vfnd_ptr_tester (const volatile short *); 
	no_type vfnd_ptr_tester (const volatile int *); 
	no_type vfnd_ptr_tester (const volatile long *); 
	no_type vfnd_ptr_tester (const volatile double *); 
	no_type vfnd_ptr_tester (const volatile float *); 
	no_type vfnd_ptr_tester (const volatile bool *); 
	yes_type vfnd_ptr_tester (const volatile void *); 

	template <typename T> T* add_ptr(T&); 

	template <bool C> struct bool_to_yesno { typedef no_type type; }; 
	template <> struct bool_to_yesno<true> { typedef yes_type type; }; 

	template <typename T> 
	struct is_enum 
	{ 
		static T arg; 
		static const bool value = ( (sizeof(int_conv_tester(arg)) == sizeof(yes_type)) && (sizeof(vfnd_ptr_tester(add_ptr(arg))) == sizeof(yes_type)) ); 
	}; 
 
	/*---------------------------------------------------------------------------*/ 
	/* 根据模板判断类型							 			 */ 
	/*---------------------------------------------------------------------------*/ 
	template<typename A>
	struct which_type	
	{ 
		static const char* getValue()
		{
			if (is_enum<A>::value){ return "n"; }
			else{ return "u"; }
		}
	};
	template<> struct which_type<char>					{ static const char* getValue(){ return "s"; } };
	template<> struct which_type<unsigned char>			{ static const char* getValue(){ return "n"; } };
	template<> struct which_type<short>					{ static const char* getValue(){ return "n"; } };
	template<> struct which_type<unsigned short>		{ static const char* getValue(){ return "n"; } };
	template<> struct which_type<long>					{ static const char* getValue(){ return "n"; } };
	template<> struct which_type<unsigned long>			{ static const char* getValue(){ return "n"; } };
	template<> struct which_type<int>					{ static const char* getValue(){ return "n"; } };
	template<> struct which_type<unsigned int>			{ static const char* getValue(){ return "n"; } };
	template<> struct which_type<float>					{ static const char* getValue(){ return "n"; } };
	template<> struct which_type<double>				{ static const char* getValue(){ return "n"; } };
	template<> struct which_type<char*>					{ static const char* getValue(){ return "s"; } };
	template<> struct which_type<const char*>			{ static const char* getValue(){ return "s"; } };
	template<> struct which_type<bool>					{ static const char* getValue(){ return "b"; } };
	/*	template<> struct which_type<lua_value*>			{ static const bool value = ''; };*/
	template<> struct which_type<long long>				{ static const char* getValue(){ return "n"; } };
	template<> struct which_type<unsigned long long>	{ static const char* getValue(){ return "n"; } };
	template<> struct which_type<std::string>			{ static const char* getValue(){ return "s"; } };
	template<> struct which_type<const std::string&>	{ static const char* getValue(){ return "s"; } };
	template<> struct which_type<table>					{ static const char* getValue(){ return "t"; } };
	
	/*---------------------------------------------------------------------------*/ 
	/////////////////////////////////

	// from lua
	template<typename T>
	struct void2val { static T invoke(void* input){ return *(T*)input; } };
	template<typename T>
	struct void2ptr { static T* invoke(void* input){ 
		return (T*)input; 
	} };
	template<typename T>
	struct void2ref { static T& invoke(void* input){ return *(T*)input; } };

	template<typename T>  
	struct void2type
	{
		static T invoke(void* ptr)
		{
			return	if_<is_ptr<T>::value
						,void2ptr<typename base_type<T>::type>
						,typename if_<is_ref<T>::value
							,void2ref<typename base_type<T>::type>
							,void2val<typename base_type<T>::type>
						>::type
					>::type::invoke(ptr);
		}
	};

	struct user
	{
		user(void* p) : m_p(p) {}
		virtual ~user() {}
		void* m_p;
	};

	template<typename T>  
	struct user2type { static T invoke(lua_State *L, int index) { return void2type<T>::invoke(lua_touserdata(L, index)); } };

	template<typename T>
	struct lua2enum { static T invoke(lua_State *L, int index) { return (T)(int)lua_tonumber(L, index); } };

	template<typename T>
	struct lua2object
	{ 
		static T invoke(lua_State *L, int index) 
		{ 
			if(!lua_isuserdata(L,index))
			{
				lua_pushstring(L, "no class at first argument. (forgot ':' expression ?)");
				lua_error(L);
			}
			return void2type<T>::invoke(user2type<user*>::invoke(L,index)->m_p); 
		} 
	};

	template<typename T>
	T lua2type(lua_State *L, int index)
	{
		return	if_<is_enum<T>::value
					,lua2enum<T>
					,lua2object<T> 
				>::type::invoke(L, index);
	}

	/*-----------------------------------------------------------------------------------------*/ 
	/* 添加将实例转换为userdata的结构体，注册单例时使用，此时不用释放内存					  */ 
	/*-----------------------------------------------------------------------------------------*/ 
	template<typename T>
	struct val2user2 : user
	{
		val2user2(){}

		template<typename T1>
		val2user2(T1 t1) : user(t1) {}

		~val2user2(){}
	};

	template<typename T>
	struct val2user : user
	{
		val2user() : user(new T) {}

		template<typename T1>
		val2user(T1 t1) : user(new T(t1)) {}

		template<typename T1, typename T2>
		val2user(T1 t1, T2 t2) : user(new T(t1, t2)) {}

		template<typename T1, typename T2, typename T3>
		val2user(T1 t1, T2 t2, T3 t3) : user(new T(t1, t2, t3)) {}

		template<typename T1, typename T2, typename T3, typename T4>
		val2user(T1 t1, T2 t2, T3 t3, T4 t4) : user(new T(t1, t2, t3,t4)) {}

		template<typename T1, typename T2, typename T3, typename T4, typename T5>
		val2user(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5) : user(new T(t1, t2, t3,t4,t5)) {}

		template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
		val2user(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8) : user(new T(t1, t2, t3, t4, t5, t6, t7, t8)) {}

		~val2user() { delete ((T*)m_p); }
	};

	template<typename T>
	struct ptr2user : user
	{
		ptr2user(T* t) : user((void*)t) {}
	};

	template<typename T>
	struct ref2user : user
	{
		ref2user(T& t) : user(&t) {}
	};

	// to lua
	template<typename T>
	struct val2lua { static void invoke(lua_State *L, T& input){ new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(input); } };
	template<typename T>
	struct ptr2lua { static void invoke(lua_State *L, T* input){ if(input) new(lua_newuserdata(L, sizeof(ptr2user<T>))) ptr2user<T>(input); else lua_pushnil(L); } };
	template<typename T>
	struct ref2lua 
	{ 
		static void invoke(lua_State *L, T& input)
		{ 
			new(lua_newuserdata(L, sizeof(ref2user<T>))) ref2user<T>(input); 
		} 
	};

	template<typename T>
	struct enum2lua { static void invoke(lua_State *L, T val) { lua_pushnumber(L, (int)val); } };

	template<typename T>
	struct object2lua 
	{ 
		static void invoke(lua_State *L, T val) 
		{ 
			if_<is_ptr<T>::value
				,ptr2lua<typename base_type<T>::type>
				,typename if_<is_ref<T>::value
					,ref2lua<typename base_type<T>::type>
					,val2lua<typename base_type<T>::type>
				>::type
			>::type::invoke(L, val);

			if (lua_type(L, -1))
			{
				push_meta(L, class_name<typename class_type<T>::type>::name());
				if (0 == lua_type(L, -1))
				{
					lua_remove(L, -1);
					push_meta(L, space_name::name());
					if(lua_istable(L, -1))
					{
						lua_pushstring(L, class_name<typename class_type<T>::type>::name());
						lua_gettable(L, -2);
						if (!lua_istable(L, -1))
						{
							LogUtil::LogInfo("can't find '%s' class variable. (forgot registering class variable ?)", class_name<typename class_type<T>::type>::name());
							lua_pop(L, 2);
							return;
						}
					}
					lua_remove(L, -2);
				}
				lua_setmetatable(L, -2);
			}
		} 
	};

	template<typename T>
	void type2lua(lua_State *L, T val)
	{
		if_<is_enum<T>::value
			,enum2lua<T>
			,object2lua<T>
		>::type::invoke(L, val);
	}

	// get value from cclosure
	template<typename T>  
	T upvalue_(lua_State *L)
	{
		return user2type<T>::invoke(L, lua_upvalueindex(1));
	}

	// read a value from lua stack 
	template<typename T>  
	T read(lua_State *L, int index)				{ return lua2type<T>(L, index); }

	template<>	char*				read(lua_State *L, int index);
	template<>	const char*			read(lua_State *L, int index);
	template<>	char				read(lua_State *L, int index);
	template<>	unsigned char		read(lua_State *L, int index);
	template<>	unsigned char*		read(lua_State *L, int index);
	template<>	short				read(lua_State *L, int index);
	template<>	unsigned short		read(lua_State *L, int index);
	template<>	long				read(lua_State *L, int index);
	template<>	unsigned long		read(lua_State *L, int index);
	template<>	int					read(lua_State *L, int index);
	template<>	unsigned int		read(lua_State *L, int index);
	template<>	float				read(lua_State *L, int index);
	template<>	double				read(lua_State *L, int index);
	template<>	bool				read(lua_State *L, int index);
	template<>	void				read(lua_State *L, int index);
	template<>	long long			read(lua_State *L, int index);
	template<>	unsigned long long	read(lua_State *L, int index);
	template<>	std::string			read(lua_State *L, int index);
	template<>  const std::string&  read(lua_State *L, int index);
	template<>	table				read(lua_State *L, int index);

	// push a value to lua stack 
	template<typename T>  
	int push(lua_State *L, T ret)					{ type2lua<T>(L, ret);  return 1;}
	
	template<>	int push(lua_State *L, char ret);
	template<>	int push(lua_State *L, unsigned char ret);
	template<>	int push(lua_State *L, short ret);
	template<>	int push(lua_State *L, unsigned short ret);
	template<>	int push(lua_State *L, long ret);
	template<>	int push(lua_State *L, unsigned long ret);
	template<>	int push(lua_State *L, int ret);
	template<>	int push(lua_State *L, unsigned int ret);
	template<>	int push(lua_State *L, float ret);
	template<>	int push(lua_State *L, double ret);
	template<>	int push(lua_State *L, char* ret);
	template<>	int push(lua_State *L, const char* ret);
	template<>	int push(lua_State *L, bool ret);
	template<>	int push(lua_State *L, lua_value* ret);
	template<>	int push(lua_State *L, long long ret);
	template<>	int push(lua_State *L, unsigned long long ret);
	template<>	int push(lua_State *L, std::string ret);
	template<>	int push(lua_State *L, const std::string& ret);
	template<>	int push(lua_State *L, table ret);
	template<>	int push(lua_State *L, cocos2d::Vec2 ret);

	// pop a value from lua stack
	template<typename T>  
	T pop(lua_State *L) { T t = read<T>(L, -1); lua_pop(L, 1); return t; }
	
	template<>	void	pop(lua_State *L);
	template<>	table	pop(lua_State *L);

	// functor (with return value)
	template<typename RVal, typename T1=void, typename T2=void, typename T3=void, typename T4=void, typename T5=void, typename T6=void, typename T7=void, typename T8=void, typename T9=void, typename T10=void>
	struct functor
	{
		static int invoke(lua_State *L) {return push(L,upvalue_<RVal(*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6),read<T7>(L,7),read<T8>(L,8),read<T9>(L,9),read<T10>(L,10)));}
	};

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
	struct functor<RVal,T1,T2,T3,T4,T5,T6,T7,T8,T9> 
	{
		static int invoke(lua_State *L) { return push(L,upvalue_<RVal(*)(T1,T2,T3,T4,T5,T6,T7,T8,T9)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6),read<T7>(L,7),read<T8>(L,8),read<T9>(L,9)));}
	};

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	struct functor<RVal,T1,T2,T3,T4,T5,T6,T7,T8> 
	{
		static int invoke(lua_State *L) { return push(L,upvalue_<RVal(*)(T1,T2,T3,T4,T5,T6,T7,T8)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6),read<T7>(L,7),read<T8>(L,8)));}
	};

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	struct functor<RVal,T1,T2,T3,T4,T5,T6,T7> 
	{
		static int invoke(lua_State *L) { return push(L,upvalue_<RVal(*)(T1,T2,T3,T4,T5,T6,T7)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6),read<T7>(L,7)));}
	};

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	struct functor<RVal,T1,T2,T3,T4,T5,T6> 
	{
		static int invoke(lua_State *L) { return push(L,upvalue_<RVal(*)(T1,T2,T3,T4,T5,T6)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6)));}
	};

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5>
	struct functor<RVal,T1,T2,T3,T4,T5> 
	{
		static int invoke(lua_State *L) { return push(L,upvalue_<RVal(*)(T1,T2,T3,T4,T5)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5)));}
	};

	template<typename RVal, typename T1, typename T2, typename T3, typename T4>
	struct functor<RVal,T1,T2,T3,T4> 
	{
		static int invoke(lua_State *L) { return push(L,upvalue_<RVal(*)(T1,T2,T3,T4)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4)));}
	};

	template<typename RVal, typename T1, typename T2, typename T3>
	struct functor<RVal,T1,T2,T3> 
	{
		static int invoke(lua_State *L) { return push(L,upvalue_<RVal(*)(T1,T2,T3)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3)));}
	};

	template<typename RVal, typename T1, typename T2>
	struct functor<RVal,T1,T2> 
	{
		static int invoke(lua_State *L) { return push(L,upvalue_<RVal(*)(T1,T2)>(L)(read<T1>(L,1),read<T2>(L,2)));}
	};

	template<typename RVal, typename T1>
	struct functor<RVal,T1> 
	{
		static int invoke(lua_State *L) { return push(L,upvalue_<RVal(*)(T1)>(L)(read<T1>(L,1)));}
	};

	template<typename RVal>
	struct functor<RVal>
	{
		static int invoke(lua_State *L) { return push(L,upvalue_<RVal(*)()>(L)());}
	};

	// functor (without return value)
	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
	struct functor<void, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6),read<T7>(L,7),read<T8>(L,8),read<T9>(L,9),read<T10>(L,10)); return 0; }
	};

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
	struct functor<void, T1, T2, T3, T4, T5, T6, T7, T8, T9>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4,T5,T6,T7,T8,T9)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6),read<T7>(L,7),read<T8>(L,8),read<T9>(L,9)); return 0; }
	};

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	struct functor<void, T1, T2, T3, T4, T5, T6, T7, T8>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4,T5,T6,T7,T8)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6),read<T7>(L,7),read<T8>(L,8)); return 0; }
	};

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	struct functor<void, T1, T2, T3, T4, T5, T6, T7>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4,T5,T6,T7)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6),read<T7>(L,7)); return 0; }
	};

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	struct functor<void, T1, T2, T3, T4, T5, T6>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4,T5,T6)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6)); return 0; }
	};

	template<typename T1, typename T2, typename T3, typename T4, typename T5>
	struct functor<void, T1, T2, T3, T4, T5>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4,T5)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5)); return 0; }
	};

	template<typename T1, typename T2, typename T3, typename T4>
	struct functor<void, T1, T2, T3, T4>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4)); return 0; }
	};

	template<typename T1, typename T2, typename T3>
	struct functor<void, T1, T2, T3>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2,T3)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3)); return 0; }
	};

	template<typename T1, typename T2>
	struct functor<void, T1, T2>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2)>(L)(read<T1>(L,1),read<T2>(L,2)); return 0; }
	};

	template<typename T1>
	struct functor<void, T1>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)(T1)>(L)(read<T1>(L,1)); return 0; }
	};

	template<>
	struct functor<void>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)()>(L)(); return 0; }
	};

	// functor (non-managed)
	template<typename T1>
	struct functor<int, lua_State*, T1>
	{
		static int invoke(lua_State *L) { return upvalue_<int(*)(lua_State*,T1)>(L)(L,read<T1>(L,1)); }
	};

	template<>
	struct functor<int,lua_State*>
	{
		static int invoke(lua_State *L) { return upvalue_<int(*)(lua_State*)>(L)(L); }
	};

	// push_functor
	template<typename RVal> 
	void push_functor(lua_State *L, RVal (*func)())
	{
		lua_pushcclosure(L, functor<RVal>::invoke, 1);
	}

	template<typename RVal, typename T1> 
	void push_functor(lua_State *L, RVal (*func)(T1))
	{ 
		lua_pushcclosure(L, functor<RVal,T1>::invoke, 1);
	}

	template<typename RVal, typename T1, typename T2> 
	void push_functor(lua_State *L, RVal (*func)(T1,T2))
	{ 
		lua_pushcclosure(L, functor<RVal,T1,T2>::invoke, 1);
	}

	template<typename RVal, typename T1, typename T2, typename T3> 
	void push_functor(lua_State *L, RVal (*func)(T1,T2,T3))
	{ 
		lua_pushcclosure(L, functor<RVal,T1,T2,T3>::invoke, 1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4> 
	void push_functor(lua_State *L, RVal (*func)(T1,T2,T3,T4))
	{ 
		lua_pushcclosure(L, functor<RVal,T1,T2,T3,T4>::invoke, 1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5> 
	void push_functor(lua_State *L, RVal (*func)(T1,T2,T3,T4,T5))
	{ 
		lua_pushcclosure(L, functor<RVal,T1,T2,T3,T4,T5>::invoke, 1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> 
	void push_functor(lua_State *L, RVal (*func)(T1,T2,T3,T4,T5,T6))
	{ 
		lua_pushcclosure(L, functor<RVal,T1,T2,T3,T4,T5,T6>::invoke, 1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7> 
	void push_functor(lua_State *L, RVal (*func)(T1,T2,T3,T4,T5,T6,T7))
	{ 
		lua_pushcclosure(L, functor<RVal,T1,T2,T3,T4,T5,T6,T7>::invoke, 1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8> 
	void push_functor(lua_State *L, RVal (*func)(T1,T2,T3,T4,T5,T6,T7,T8))
	{ 
		lua_pushcclosure(L, functor<RVal,T1,T2,T3,T4,T5,T6,T7,T8>::invoke, 1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9> 
	void push_functor(lua_State *L, RVal (*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9))
	{ 
		lua_pushcclosure(L, functor<RVal,T1,T2,T3,T4,T5,T6,T7,T8,T9>::invoke, 1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10> 
	void push_functor(lua_State *L, RVal (*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10))
	{ 
		lua_pushcclosure(L, functor<RVal,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>::invoke, 1);
	}

	// member variable
	struct var_base
	{
		virtual ~var_base(){};
		virtual void get(lua_State *L) = 0;
		virtual void set(lua_State *L) = 0;
	};

	template<typename T, typename V>
	struct mem_var : var_base
	{
		V T::*_var;
		mem_var(V T::*val) : _var(val) {}
		void get(lua_State *L)
		{
			//push<if_<is_obj<V>::value,V&,V>::type>(L, read<T*>(L,1)->*(_var));
			if (is_obj<V>::value){ push<V&>(L, read<T*>(L,1)->*(_var)); }
			else{ push<V>(L, read<T*>(L,1)->*(_var)); }
		}
		void set(lua_State *L)	{ read<T*>(L,1)->*(_var) = read<V>(L, 3);	}
	};

	// class member functor (with return value)
	template<typename RVal, typename T, typename T1=void, typename T2=void, typename T3=void, typename T4=void, typename T5=void, typename T6=void, typename T7=void, typename T8=void, typename T9=void, typename T10=void>
	struct mem_functor
	{
		static int invoke(lua_State *L) 
		{
			return push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7),read<T7>(L,8),read<T8>(L,9),read<T9>(L,10),read<T10>(L,11))); 
		}
	};

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
	struct mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7,T8,T9>
	{
		static int invoke(lua_State *L) 
		{
			return push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7),read<T7>(L,8),read<T8>(L,9),read<T9>(L,10))); 
		}
	};

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	struct mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7,T8>
	{
		static int invoke(lua_State *L) 
		{
			return push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4,T5,T6,T7,T8)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7),read<T7>(L,8),read<T8>(L,9))); 
		}
	};

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	struct mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7>
	{
		static int invoke(lua_State *L) 
		{
			return push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4,T5,T6,T7)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7),read<T7>(L,8))); 
		}
	};

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	struct mem_functor<RVal,T,T1,T2,T3,T4,T5,T6>
	{
		static int invoke(lua_State *L) 
		{
			return push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4,T5,T6)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7))); 
		}
	};

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
	struct mem_functor<RVal,T,T1,T2,T3,T4,T5>
	{
		static int invoke(lua_State *L) 
		{
			return push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4,T5)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6)));
		}
	};

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4> 
	struct mem_functor<RVal,T,T1,T2,T3,T4>
	{
		static int invoke(lua_State *L) 
		{
			return push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5))); 
		}
	};

	template<typename RVal, typename T, typename T1, typename T2, typename T3> 
	struct mem_functor<RVal,T,T1,T2,T3>
	{
		static int invoke(lua_State *L) 
		{
			return push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4)));
		}
	};

	template<typename RVal, typename T, typename T1, typename T2> 
	struct mem_functor<RVal,T,T1, T2>
	{
		static int invoke(lua_State *L) 
		{
			return push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2)>(L))(read<T1>(L,2),read<T2>(L,3))); 
		}
	};

	template<typename RVal, typename T, typename T1> 
	struct mem_functor<RVal,T,T1>
	{
		static int invoke(lua_State *L) 
		{ 
			return push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1)>(L))(read<T1>(L,2))); 
		}
	};

	template<typename RVal, typename T> 
	struct mem_functor<RVal,T>
	{
		static int invoke(lua_State *L) 
		{ 
			return push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)()>(L))());
		}
	};

	// class member functor (without return value)
	template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
	struct mem_functor<void,T,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7),read<T7>(L,8),read<T8>(L,9),read<T9>(L,10),read<T10>(L,11)); return 0; }
	};

	template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
	struct mem_functor<void,T,T1,T2,T3,T4,T5,T6,T7,T8,T9>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7),read<T7>(L,8),read<T8>(L,9),read<T9>(L,10)); return 0; }
	};

	template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	struct mem_functor<void,T,T1,T2,T3,T4,T5,T6,T7,T8>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4,T5,T6,T7,T8)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7),read<T7>(L,8),read<T8>(L,9)); return 0; }
	};

	template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	struct mem_functor<void,T,T1,T2,T3,T4,T5,T6,T7>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4,T5,T6,T7)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7),read<T7>(L,8)); return 0; }
	};

	template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	struct mem_functor<void,T,T1,T2,T3,T4,T5,T6>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4,T5,T6)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7)); return 0; }
	};

	template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
	struct mem_functor<void,T,T1,T2,T3,T4,T5>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4,T5)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6)); return 0; }
	};

	template<typename T, typename T1, typename T2, typename T3, typename T4>
	struct mem_functor<void,T,T1,T2,T3,T4>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5)); return 0; }
	};

	template<typename T, typename T1, typename T2, typename T3>
	struct mem_functor<void,T,T1,T2,T3>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4)); return 0; }
	};

	template<typename T, typename T1, typename T2>
	struct mem_functor<void,T,T1,T2>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2)>(L))(read<T1>(L,2),read<T2>(L,3)); return 0; }
	};

	template<typename T, typename T1>
	struct mem_functor<void,T,T1>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1)>(L))(read<T1>(L,2)); return 0; }
	};

	template<typename T>
	struct mem_functor<void,T>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)()>(L))(); return 0; }
	};

	// class member functor (non-managed)
	template<typename T, typename T1> 
	struct mem_functor<int,T,lua_State*,T1>
	{
		static int invoke(lua_State *L) { return (read<T*>(L,1)->*upvalue_<int(T::*)(lua_State*,T1)>(L))(L, read<T1>(L,2)); }
	};

	template<typename T> 
	struct mem_functor<int,T,lua_State*>
	{
		static int invoke(lua_State *L) { return (read<T*>(L,1)->*upvalue_<int(T::*)(lua_State*)>(L))(L); }
	};

	// push_functor
	template<typename RVal, typename T>
	void push_functor(lua_State *L, RVal (T::*func)()) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T>::invoke, 1); 
	}

	template<typename RVal, typename T>
	void push_functor(lua_State *L, RVal (T::*func)() const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1>
	void push_functor(lua_State *L, RVal (T::*func)(T1)) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1>
	void push_functor(lua_State *L, RVal (T::*func)(T1) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2)) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3)) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4)) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5)) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6)) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7)) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7,T8)) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7,T8>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7,T8) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7,T8>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9)) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7,T8,T9>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7,T8,T9>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10)) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>::invoke, 1); 
	}

	// constructor
	template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
	int constructor(lua_State *L) 
	{ 
		new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6));
		push_meta(L, class_name<typename class_type<T>::type>::name());
		lua_setmetatable(L, -2);

		return 1; 
	}

	template<typename T, typename T1, typename T2, typename T3, typename T4>
	int constructor(lua_State *L) 
	{ 
		new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5));
		push_meta(L, class_name<typename class_type<T>::type>::name());
		lua_setmetatable(L, -2);

		return 1; 
	}

	template<typename T, typename T1, typename T2, typename T3>
	int constructor(lua_State *L) 
	{ 
		new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4));
		push_meta(L, class_name<typename class_type<T>::type>::name());
		lua_setmetatable(L, -2);

		return 1; 
	}

	template<typename T, typename T1, typename T2>
	int constructor(lua_State *L) 
	{ 
		new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2),read<T2>(L,3));
		push_meta(L, class_name<typename class_type<T>::type>::name());
		lua_setmetatable(L, -2);

		return 1; 
	}

	template<typename T, typename T1>
	int constructor(lua_State *L)
	{
		new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2));
		push_meta(L, class_name<typename class_type<T>::type>::name());
		lua_setmetatable(L, -2);

		return 1; 
	}

	template<typename T>
	int constructor(lua_State *L) 
	{ 
		new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>();
		push_meta(L, class_name<typename class_type<T>::type>::name());
		lua_setmetatable(L, -2);

		return 1; 
	}

	/*---------------------------------------------------------------------------*/ 
	/* 改写注册构造函数															 */ 
	/*---------------------------------------------------------------------------*/ 

	template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	int constructorEx(lua_State *L)
	{
		push_meta(L, space_name::name());
		new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7),read<T7>(L,8),read<T8>(L,9));
		lua_pushstring(L, class_name<typename class_type<T>::type>::name());
		lua_gettable(L, -3);
		lua_setmetatable(L, -2);
		lua_remove(L, -2);

		return 1; 
	}

	template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
	int constructorEx(lua_State *L)
	{
		push_meta(L, space_name::name());
		new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6));
		lua_pushstring(L, class_name<typename class_type<T>::type>::name());
		lua_gettable(L, -3);
		lua_setmetatable(L, -2);
		lua_remove(L, -2);

		return 1; 
	}

	template<typename T, typename T1, typename T2, typename T3, typename T4>
	int constructorEx(lua_State *L)
	{
		push_meta(L, space_name::name());
		new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5));
		lua_pushstring(L, class_name<typename class_type<T>::type>::name());
		lua_gettable(L, -3);
		lua_setmetatable(L, -2);
		lua_remove(L, -2);

		return 1; 
	}

	template<typename T, typename T1, typename T2, typename T3>
	int constructorEx(lua_State *L)
	{
		push_meta(L, space_name::name());
		new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4));
		lua_pushstring(L, class_name<typename class_type<T>::type>::name());
		lua_gettable(L, -3);
		lua_setmetatable(L, -2);
		lua_remove(L, -2);

		return 1; 
	}

	template<typename T, typename T1, typename T2>
	int constructorEx(lua_State *L)
	{
		push_meta(L, space_name::name());
		new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2),read<T2>(L,3));
		lua_pushstring(L, class_name<typename class_type<T>::type>::name());
		lua_gettable(L, -3);
		lua_setmetatable(L, -2);
		lua_remove(L, -2);

		return 1; 
	}

	template<typename T, typename T1>
	int constructorEx(lua_State *L)
	{
		push_meta(L, space_name::name());
		new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2));
		lua_pushstring(L, class_name<typename class_type<T>::type>::name());
		lua_gettable(L, -3);
		lua_setmetatable(L, -2);
		lua_remove(L, -2);

		return 1; 
	}

	template<typename T>
	int constructorEx(lua_State *L)
	{
		push_meta(L, space_name::name());
		new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>();
		lua_pushstring(L, class_name<typename class_type<T>::type>::name());
		lua_gettable(L, -3);
		lua_setmetatable(L, -2);
		lua_remove(L, -2);

		return 1; 
	}

	/*---------------------------------------------------------------------------*/ 

	// destroyer
	template<typename T>
	int destroyer(lua_State *L) 
	{ 
		((user*)lua_touserdata(L, 1))->~user();
		return 0;
	}

	/*---------------------------------------------------------------------------*/ 
	/* 改写注册函数 以便将类注册到命名空间中									 */ 
	/*---------------------------------------------------------------------------*/ 
	template<typename F> 
	void defEx(lua_State* L, const char* name, F func)
	{ 
		push_meta(L, space_name::name());
		if(lua_istable(L, -1))
		{
			lua_pushstring(L, name);
			lua_pushlightuserdata(L, (void*)func);
			push_functor(L, func);
			lua_settable(L, -3);
		}
		lua_pop(L, 1);
	}

	// global function
	template<typename F> 
	void def(lua_State* L, const char* name, F func)
	{ 
		lua_pushstring(L, name);
		lua_pushlightuserdata(L, (void*)func);
		push_functor(L, func);
		lua_settable(L, LUA_GLOBALSINDEX);
	}

	// global variable
	template<typename T>
	void set(lua_State* L, const char* name, T object)
	{
		lua_pushstring(L, name);
		push(L, object);
		lua_settable(L, LUA_GLOBALSINDEX);
	}

	template<typename T>
	T get(lua_State* L, const char* name)
	{
		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		return pop<T>(L);
	}

	template<typename T>
	void decl(lua_State* L, const char* name, T object)
	{
		set(L, name, object);
	}

	// call
	template<typename RVal>
	RVal call(lua_State* L, const char* name)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);

		if(lua_isfunction(L,-1))
		{
			lua_pcall(L, 0, 1, errfunc);
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, errfunc);
		return pop<RVal>(L);
	}

	template<typename RVal, typename T1>
	RVal call(lua_State* L, const char* name, T1 arg)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isfunction(L,-1))
		{
			push(L, arg);
			lua_pcall(L, 1, 1, errfunc);
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, errfunc);
		return pop<RVal>(L);
	}

	template<typename RVal, typename T1, typename T2>
	RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isfunction(L,-1))
		{
			push(L, arg1);
			push(L, arg2);
			lua_pcall(L, 2, 1, errfunc);
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, errfunc);
		return pop<RVal>(L);
	}

	template<typename RVal, typename T1, typename T2, typename T3>
	RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isfunction(L,-1))
		{
			push(L, arg1);
			push(L, arg2);
			push(L, arg3);
			lua_pcall(L, 3, 1, errfunc);
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, errfunc);
		return pop<RVal>(L);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isfunction(L,-1))
		{
			push(L, arg1);
			push(L, arg2);
			push(L, arg3);
			push(L, arg4);
			push(L, arg5);
			push(L, arg6);
			lua_pcall(L, 6, 1, errfunc);
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, errfunc);
		return pop<RVal>(L);
	}


	/*---------------------------------------------------------------------------*/ 
	/* 添加命名空间，以便lua调用时知道是C++注册的函数					         */ 
	/*---------------------------------------------------------------------------*/ 
	void module_addEx(lua_State* L, const char* namespaces);

	/*---------------------------------------------------------------------------*/ 
	/* 改写添加注册类，以便将类注册到命名空间中									 */ 
	/*---------------------------------------------------------------------------*/ 
	template<typename T>
	void class_addEx(lua_State* L, const char* name) 
	{
		push_meta(L, space_name::name());
		if(lua_istable(L, -1))
		{
			class_name<T>::name(name);

			lua_pushstring(L, name);
			lua_rawget(L, -2);
			if (lua_istable(L, -1))
			{
#if defined (_WIN32)
				LogUtil::LogInfo("luaregister : class %s is repeat class_add:%s\n", name);
#endif
			}
			else
			{
				lua_pushstring(L, name);
				lua_newtable(L);

				lua_newtable(L);
				lua_pushstring(L, "__index");
				lua_pushcclosure(L, static_meta_get, 0);
				lua_rawset(L, -3);
				lua_setmetatable(L, -2);

				lua_pushstring(L, "__name");
				lua_pushstring(L, name);
				lua_rawset(L, -3);

				lua_pushstring(L, "__index");
				lua_pushcclosure(L, meta_get, 0);
				lua_rawset(L, -3);

				lua_pushstring(L, "__newindex");
				lua_pushcclosure(L, meta_set, 0);
				lua_rawset(L, -3);

				lua_pushstring(L, "__gc");
				lua_pushcclosure(L, destroyer<T>, 0);
				lua_rawset(L, -3);

				lua_rawset(L, -4);
			}
		}
		lua_pop(L, 2);
	}

	// class init
	template<typename T>
	void class_add(lua_State* L, const char* name) 
	{
		class_name<T>::name(name);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if (lua_istable(L, -1))
		{
#if defined (_WIN32)
			LogUtil::LogInfo("luaregister : class %s is repeat add:%s\n", name);
#endif
		}
		else
		{
			lua_pushstring(L, name);
			lua_newtable(L);

			lua_newtable(L);
			lua_pushstring(L, "__index");
			lua_pushcclosure(L, static_meta_get, 0);
			lua_rawset(L, -3);
			lua_setmetatable(L, -2);

			lua_pushstring(L, "__name");
			lua_pushstring(L, name);
			lua_rawset(L, -3);

			lua_pushstring(L, "__index");
			lua_pushcclosure(L, meta_get, 0);
			lua_rawset(L, -3);

			lua_pushstring(L, "__newindex");
			lua_pushcclosure(L, meta_set, 0);
			lua_rawset(L, -3);

			lua_pushstring(L, "__gc");
			lua_pushcclosure(L, destroyer<T>, 0);
			lua_rawset(L, -3);

			lua_settable(L, LUA_GLOBALSINDEX);
		}

		lua_pop(L, 1);
	}

	// Tinker Class Inheritence
	template<typename T, typename P>
	void class_inh(lua_State* L)
	{
		push_meta(L, class_name<T>::name());
		if(lua_istable(L, -1))
		{
			lua_pushstring(L, "__parent");
			push_meta(L, class_name<P>::name());
			lua_rawset(L, -3);
		}
		lua_pop(L, 1);
	}

	/*---------------------------------------------------------------------------*/ 
	/* 改写注册父类，以便将类注册到命名空间中									 */ 
	/*---------------------------------------------------------------------------*/ 
	template<typename T, typename P>
	void class_inhEx(lua_State* L)
	{
		push_meta(L, space_name::name());
		if(lua_istable(L, -1))
		{
			lua_pushstring(L, class_name<T>::name());
			lua_gettable(L, -2);
			if(lua_istable(L, -1))
			{
				lua_pushstring(L, "__parent");
				lua_pushstring(L, class_name<P>::name());
				lua_gettable(L, -4);
				lua_rawset(L, -3);
			}
		}
		lua_pop(L, 2);
	}

	/*---------------------------------------------------------------------------*/ 
	/* 改写注册类构造函数，以便将类注册到命名空间中								 */ 
	/*---------------------------------------------------------------------------*/ 
	template<typename T, typename F>
	void class_conEx(lua_State* L,F func)
	{
		push_meta(L, space_name::name());
		if(lua_istable(L, -1))
		{
			lua_pushstring(L, class_name<T>::name());
			lua_gettable(L, -2);
			if(lua_istable(L, -1))
			{
				lua_getmetatable(L, -1);
				lua_pushstring(L, "__call");
				lua_pushcclosure(L, func, 0);
				lua_rawset(L, -3);
			}
		}
		lua_pop(L, 3);
	}

	// Tinker Class Constructor
	template<typename T, typename F>
	void class_con(lua_State* L,F func)
	{
		push_meta(L, class_name<T>::name());
		if(lua_istable(L, -1))
		{
			lua_getmetatable(L, -1);
			lua_pushstring(L, "__call");
			lua_pushcclosure(L, func, 0);
			lua_rawset(L, -3);
		}
		lua_pop(L, 2);
	}
	/*---------------------------------------------------------------------------*/ 
	/* 根据注册函数获取函数名													 */ 
	/*---------------------------------------------------------------------------*/ 
	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
	std::string getFuncName(lua_State* L, std::string name, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10) const)
	{
		name.append(which_type<T1>::getValue(),1);
		name.append(which_type<T2>::getValue(),1);
		name.append(which_type<T3>::getValue(),1);
		name.append(which_type<T4>::getValue(),1);
		name.append(which_type<T5>::getValue(),1);
		name.append(which_type<T6>::getValue(),1);
		name.append(which_type<T7>::getValue(),1);
		name.append(which_type<T8>::getValue(),1);
		name.append(which_type<T9>::getValue(),1);
		return name.append(which_type<T10>::getValue(),1);
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
	std::string getFuncName(lua_State* L, std::string name, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10))
	{
		name.append(which_type<T1>::getValue(),1);
		name.append(which_type<T2>::getValue(),1);
		name.append(which_type<T3>::getValue(),1);
		name.append(which_type<T4>::getValue(),1);
		name.append(which_type<T5>::getValue(),1);
		name.append(which_type<T6>::getValue(),1);
		name.append(which_type<T7>::getValue(),1);
		name.append(which_type<T8>::getValue(),1);
		name.append(which_type<T9>::getValue(),1);
		return name.append(which_type<T10>::getValue(),1);
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
	std::string getFuncName(lua_State* L, std::string name, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9) const)
	{
		name.append(which_type<T1>::getValue(),1);
		name.append(which_type<T2>::getValue(),1);
		name.append(which_type<T3>::getValue(),1);
		name.append(which_type<T4>::getValue(),1);
		name.append(which_type<T5>::getValue(),1);
		name.append(which_type<T6>::getValue(),1);
		name.append(which_type<T7>::getValue(),1);
		name.append(which_type<T8>::getValue(),1);
		return name.append(which_type<T9>::getValue(),1);
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
	std::string getFuncName(lua_State* L, std::string name, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9))
	{
		name.append(which_type<T1>::getValue(),1);
		name.append(which_type<T2>::getValue(),1);
		name.append(which_type<T3>::getValue(),1);
		name.append(which_type<T4>::getValue(),1);
		name.append(which_type<T5>::getValue(),1);
		name.append(which_type<T6>::getValue(),1);
		name.append(which_type<T7>::getValue(),1);
		name.append(which_type<T8>::getValue(),1);
		return name.append(which_type<T9>::getValue(),1);
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	std::string getFuncName(lua_State* L, std::string name, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7,T8) const)
	{
		name.append(which_type<T1>::getValue(),1);
		name.append(which_type<T2>::getValue(),1);
		name.append(which_type<T3>::getValue(),1);
		name.append(which_type<T4>::getValue(),1);
		name.append(which_type<T5>::getValue(),1);
		name.append(which_type<T6>::getValue(),1);
		name.append(which_type<T7>::getValue(),1);
		return name.append(which_type<T8>::getValue(),1);
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	std::string getFuncName(lua_State* L, std::string name, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7,T8))
	{
		name.append(which_type<T1>::getValue(),1);
		name.append(which_type<T2>::getValue(),1);
		name.append(which_type<T3>::getValue(),1);
		name.append(which_type<T4>::getValue(),1);
		name.append(which_type<T5>::getValue(),1);
		name.append(which_type<T6>::getValue(),1);
		name.append(which_type<T7>::getValue(),1);
		return name.append(which_type<T8>::getValue(),1);
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	std::string getFuncName(lua_State* L, std::string name, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7) const)
	{
		name.append(which_type<T1>::getValue(),1);
		name.append(which_type<T2>::getValue(),1);
		name.append(which_type<T3>::getValue(),1);
		name.append(which_type<T4>::getValue(),1);
		name.append(which_type<T5>::getValue(),1);
		name.append(which_type<T6>::getValue(),1);
		return name.append(which_type<T7>::getValue(),1);
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	std::string getFuncName(lua_State* L, std::string name, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7))
	{
		name.append(which_type<T1>::getValue(),1);
		name.append(which_type<T2>::getValue(),1);
		name.append(which_type<T3>::getValue(),1);
		name.append(which_type<T4>::getValue(),1);
		name.append(which_type<T5>::getValue(),1);
		name.append(which_type<T6>::getValue(),1);
		return name.append(which_type<T7>::getValue(),1);
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	std::string getFuncName(lua_State* L, std::string name, RVal (T::*func)(T1,T2,T3,T4,T5,T6) const)
	{
		name.append(which_type<T1>::getValue(),1);
		name.append(which_type<T2>::getValue(),1);
		name.append(which_type<T3>::getValue(),1);
		name.append(which_type<T4>::getValue(),1);
		name.append(which_type<T5>::getValue(),1);
		return name.append(which_type<T6>::getValue(),1);
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	std::string getFuncName(lua_State* L, std::string name, RVal (T::*func)(T1,T2,T3,T4,T5,T6))
	{
		name.append(which_type<T1>::getValue(),1);
		name.append(which_type<T2>::getValue(),1);
		name.append(which_type<T3>::getValue(),1);
		name.append(which_type<T4>::getValue(),1);
		name.append(which_type<T5>::getValue(),1);
		return name.append(which_type<T6>::getValue(),1);
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
	std::string getFuncName(lua_State* L, std::string name, RVal (T::*func)(T1,T2,T3,T4,T5) const)
	{
		name.append(which_type<T1>::getValue(),1);
		name.append(which_type<T2>::getValue(),1);
		name.append(which_type<T3>::getValue(),1);
		name.append(which_type<T4>::getValue(),1);
		return name.append(which_type<T5>::getValue(),1);
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
	std::string getFuncName(lua_State* L, std::string name, RVal (T::*func)(T1,T2,T3,T4,T5))
	{
		name.append(which_type<T1>::getValue(),1);
		name.append(which_type<T2>::getValue(),1);
		name.append(which_type<T3>::getValue(),1);
		name.append(which_type<T4>::getValue(),1);
		return name.append(which_type<T5>::getValue(),1);
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4>
	std::string getFuncName(lua_State* L, std::string name, RVal (T::*func)(T1,T2,T3,T4) const)
	{
		name.append(which_type<T1>::getValue(),1);
		name.append(which_type<T2>::getValue(),1);
		name.append(which_type<T3>::getValue(),1);
		return name.append(which_type<T4>::getValue(),1);
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4>
	std::string getFuncName(lua_State* L, std::string name, RVal (T::*func)(T1,T2,T3,T4))
	{
		name.append(which_type<T1>::getValue(),1);
		name.append(which_type<T2>::getValue(),1);
		name.append(which_type<T3>::getValue(),1);
		return name.append(which_type<T4>::getValue(),1);
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3>
	std::string getFuncName(lua_State* L, std::string name, RVal (T::*func)(T1,T2,T3) const)
	{
		name.append(which_type<T1>::getValue(),1);
		name.append(which_type<T2>::getValue(),1);
		return name.append(which_type<T3>::getValue(),1);
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3>
	std::string getFuncName(lua_State* L, std::string name, RVal (T::*func)(T1,T2,T3))
	{
		name.append(which_type<T1>::getValue(),1);
		name.append(which_type<T2>::getValue(),1);
		return name.append(which_type<T3>::getValue(),1);
	}

	template<typename RVal, typename T, typename T1, typename T2>
	std::string getFuncName(lua_State* L, std::string name, RVal (T::*func)(T1,T2) const)
	{
		name.append(which_type<T1>::getValue(),1);
		return name.append(which_type<T2>::getValue(),1);
	}

	template<typename RVal, typename T, typename T1, typename T2>
	std::string getFuncName(lua_State* L, std::string name, RVal (T::*func)(T1,T2))
	{
		name.append(which_type<T1>::getValue(),1);
		return name.append(which_type<T2>::getValue(),1);
	}

	template<typename RVal, typename T, typename T1>
	std::string getFuncName(lua_State* L, std::string name, RVal (T::*func)(T1) const)
	{
		return name.append(which_type<T1>::getValue(),1);
	}

	template<typename RVal, typename T, typename T1>
	std::string getFuncName(lua_State* L, std::string name, RVal (T::*func)(T1))
	{
		return name.append(which_type<T1>::getValue(),1);
	}

	template<typename RVal, typename T>
	std::string getFuncName(lua_State* L, std::string name, RVal (T::*func)() const)
	{
		return name;
	}

	template<typename RVal, typename T>
	std::string getFuncName(lua_State* L, std::string name, RVal (T::*func)())
	{
		return name;
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
	std::string getFuncName(lua_State* L, std::string name, RVal (*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10))
	{
		name.append(which_type<T1>::getValue(),1);
		name.append(which_type<T2>::getValue(),1);
		name.append(which_type<T3>::getValue(),1);
		name.append(which_type<T4>::getValue(),1);
		name.append(which_type<T5>::getValue(),1);
		name.append(which_type<T6>::getValue(),1);
		name.append(which_type<T7>::getValue(),1);
		name.append(which_type<T8>::getValue(),1);
		name.append(which_type<T9>::getValue(),1);
		return name.append(which_type<T10>::getValue(),1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
	std::string getFuncName(lua_State* L, std::string name, RVal (*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9))
	{
		name.append(which_type<T1>::getValue(),1);
		name.append(which_type<T2>::getValue(),1);
		name.append(which_type<T3>::getValue(),1);
		name.append(which_type<T4>::getValue(),1);
		name.append(which_type<T5>::getValue(),1);
		name.append(which_type<T6>::getValue(),1);
		name.append(which_type<T7>::getValue(),1);
		name.append(which_type<T8>::getValue(),1);
		return name.append(which_type<T9>::getValue(),1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	std::string getFuncName(lua_State* L, std::string name, RVal (*func)(T1,T2,T3,T4,T5,T6,T7,T8))
	{
		name.append(which_type<T1>::getValue(),1);
		name.append(which_type<T2>::getValue(),1);
		name.append(which_type<T3>::getValue(),1);
		name.append(which_type<T4>::getValue(),1);
		name.append(which_type<T5>::getValue(),1);
		name.append(which_type<T6>::getValue(),1);
		name.append(which_type<T7>::getValue(),1);
		return name.append(which_type<T8>::getValue(),1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	std::string getFuncName(lua_State* L, std::string name, RVal (*func)(T1,T2,T3,T4,T5,T6,T7))
	{
		name.append(which_type<T1>::getValue(),1);
		name.append(which_type<T2>::getValue(),1);
		name.append(which_type<T3>::getValue(),1);
		name.append(which_type<T4>::getValue(),1);
		name.append(which_type<T5>::getValue(),1);
		name.append(which_type<T6>::getValue(),1);
		return name.append(which_type<T7>::getValue(),1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	std::string getFuncName(lua_State* L, std::string name, RVal (*func)(T1,T2,T3,T4,T5,T6))
	{
		name.append(which_type<T1>::getValue(),1);
		name.append(which_type<T2>::getValue(),1);
		name.append(which_type<T3>::getValue(),1);
		name.append(which_type<T4>::getValue(),1);
		name.append(which_type<T5>::getValue(),1);
		return name.append(which_type<T6>::getValue(),1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5>
	std::string getFuncName(lua_State* L, std::string name, RVal (*func)(T1,T2,T3,T4,T5))
	{
		name.append(which_type<T1>::getValue(),1);
		name.append(which_type<T2>::getValue(),1);
		name.append(which_type<T3>::getValue(),1);
		name.append(which_type<T4>::getValue(),1);
		return name.append(which_type<T5>::getValue(),1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4>
	std::string getFuncName(lua_State* L, std::string name, RVal (*func)(T1,T2,T3,T4))
	{
		name.append(which_type<T1>::getValue(),1);
		name.append(which_type<T2>::getValue(),1);
		name.append(which_type<T3>::getValue(),1);
		return name.append(which_type<T4>::getValue(),1);
	}

	template<typename RVal, typename T1, typename T2, typename T3>
	std::string getFuncName(lua_State* L, std::string name, RVal (*func)(T1,T2,T3))
	{
		name.append(which_type<T1>::getValue(),1);
		name.append(which_type<T2>::getValue(),1);
		return name.append(which_type<T3>::getValue(),1);
	}

	template<typename RVal, typename T1, typename T2>
	std::string getFuncName(lua_State* L, std::string name, RVal (*func)(T1,T2))
	{
		name.append(which_type<T1>::getValue(),1);
		return name.append(which_type<T2>::getValue(),1);
	}

	template<typename RVal, typename T1>
	std::string getFuncName(lua_State* L, std::string name, RVal (*func)(T1))
	{
		return name.append(which_type<T1>::getValue(),1);
	}

	template<typename RVal>
	std::string getFuncName(lua_State* L, std::string name, RVal (*func)())
	{
		return name;
	}
	/*---------------------------------------------------------------------------*/ 
	/* 更改注册类函数，将注册的函数加入到namespace								 */ 
	/*---------------------------------------------------------------------------*/ 
	template<typename T, typename F>
	void class_operatorEx(lua_State* L, const char* name, F func) 
	{ 
		push_meta(L, space_name::name());
		if(lua_istable(L, -1))
		{
			lua_pushstring(L, class_name<T>::name());
			lua_gettable(L, -2);
			if(lua_istable(L, -1))
			{
				lua_pushstring(L, name);
				lua_rawget(L, -2);
				if (lua_isnil(L, -1))
				{
					lua_pushstring(L, name);
					new(lua_newuserdata(L,sizeof(F))) F(func);
					push_functor(L, func);
					lua_rawset(L, -4);
				}
#if defined (_WIN32)
				else
				{
					LogUtil::LogInfo("operator overload function:%s\n", name);
				}
#endif
			}
		}
		lua_pop(L, 3); 		
	}

	template<typename T, typename F>
	void class_defEx(lua_State* L, const char* name, F func) 
	{ 
		push_meta(L, space_name::name());
		if(lua_istable(L, -1))
		{
			lua_pushstring(L, class_name<T>::name());
			lua_gettable(L, -2);
			if(lua_istable(L, -1))
			{
				lua_pushstring(L, name);
				lua_rawget(L, -2);
				if (lua_isnil(L, -1))
				{
					lua_pushstring(L, name);
					new(lua_newuserdata(L,sizeof(func))) F(func);
					push_functor(L, func);
					lua_rawset(L, -4);
					push_register(L, name, getFuncName(L, name, func).c_str());
				}
				else if(lua_isfunction(L, -1))
				{
					lua_pushstring(L, name);
					lua_newtable(L);

					get_register(L, name);
					lua_pushvalue(L, -4);
					lua_rawset(L, -3);

					lua_pushstring(L, getFuncName(L, name, func).c_str());
					new(lua_newuserdata(L,sizeof(func))) F(func);
					push_functor(L, func);
					lua_rawset(L, -3);

					lua_rawset(L, -4);
				}
				else if(lua_istable(L, -1))
				{
					lua_pushstring(L, getFuncName(L, name, func).c_str());
					new(lua_newuserdata(L,sizeof(func))) F(func);
					push_functor(L, func);
					lua_rawset(L, -3);
				}
			}
		}
		lua_pop(L, 3); 		
	}

	// Tinker Class Functions
	template<typename T, typename F>
	void class_def(lua_State* L, const char* name, F func) 
	{ 
		push_meta(L, class_name<T>::name());
		if(lua_istable(L, -1))
		{
			lua_pushstring(L, name);
			lua_rawget(L, -2);
			if (lua_isnil(L, -1))
			{
				lua_pushstring(L, name);
				new(lua_newuserdata(L,sizeof(func))) F(func);
				push_functor(L, func);
				lua_rawset(L, -4);
				push_register(L, name, getFuncName(L, name, func).c_str());
			}
			else if(lua_isfunction(L, -1))
			{
				lua_pushstring(L, name);
				lua_newtable(L);

				get_register(L, name);
				lua_pushvalue(L, -4);
				lua_rawset(L, -3);

				lua_pushstring(L, getFuncName(L, name, func).c_str());
				new(lua_newuserdata(L,sizeof(func))) F(func);
				push_functor(L, func);
				lua_rawset(L, -3);

				lua_rawset(L, -4);
			}
			else if(lua_istable(L, -1))
			{
				lua_pushstring(L, getFuncName(L, name, func).c_str());
				new(lua_newuserdata(L,sizeof(func))) F(func);
				push_functor(L, func);
				lua_rawset(L, -3);
			}
		}
		lua_pop(L, 2);
	}

	/*---------------------------------------------------------------------------*/ 
	/* 增加类的枚举类型支持														 */ 
	/*---------------------------------------------------------------------------*/ 
	template<typename T, typename B>
	void class_enumEx(lua_State* L, const char* name,  B val) 
	{
		push_meta(L, space_name::name());
		if(lua_istable(L, -1))
		{
			lua_pushstring(L, class_name<T>::name());
			lua_gettable(L, -2);
			if(lua_istable(L, -1))
			{
				lua_pushstring(L, name);
				push<B>(L, val);
				lua_rawset(L, -3);
			}
		}
		lua_pop(L, 2);
	}

	template<typename T, typename B>
	void class_enum(lua_State* L, const char* name, B val) 
	{
		push_meta(L, class_name<T>::name());
		if(lua_istable(L, -1))
		{
			lua_pushstring(L, name);
			push<B>(L, val);
			lua_rawset(L, -3);
		}
		lua_pop(L, 1);
	}

	/*---------------------------------------------------------------------------*/ 
	/* 增加注册类单例支持														 */ 
	/*---------------------------------------------------------------------------*/ 
	template<typename T, typename RVal>
	void class_instanceEx(lua_State* L, RVal (*func)()) 
	{
		push_meta(L, space_name::name());
		if(lua_istable(L, -1))
		{
			RVal v = func();
			lua_pushstring(L, class_name<T>::name());
			new(lua_newuserdata(L, sizeof(val2user2<T>))) val2user2<T>(v);
			lua_pushstring(L, class_name<T>::name());
			lua_gettable(L, -4);
			lua_setmetatable(L, -2);
			lua_rawset(L, -3);
		}
		lua_pop(L, 1);
	}
	
	template<typename T, typename RVal>
	void class_instance(lua_State* L, RVal (*func)()) 
	{
		RVal v = func();
		new(lua_newuserdata(L, sizeof(val2user2<T>))) val2user2<T>(v);
		lua_getfield(L, LUA_GLOBALSINDEX, class_name<T>::name());
		lua_setmetatable(L, -2);
		lua_setfield(L, LUA_GLOBALSINDEX, class_name<T>::name());
	}

	/*---------------------------------------------------------------------------*/ 
	/* 增加类静态函数的支持														 */ 
	/*---------------------------------------------------------------------------*/ 
	template<typename T, typename F>
	void class_staticEx(lua_State* L, const char* name, F func) 
	{
		push_meta(L, space_name::name());
		if(lua_istable(L, -1))
		{
			lua_pushstring(L, class_name<T>::name());
			lua_gettable(L, -2);
			if(lua_istable(L, -1))		//类的表
			{
				lua_pushstring(L, name);
				lua_rawget(L, -2);
				if (lua_isnil(L, -1))
				{
					lua_remove(L, -1);
					lua_getmetatable(L, -1);

					lua_pushstring(L, name);
					lua_rawget(L, -2);
					if (lua_isnil(L, -1))
					{
						lua_pushstring(L, name);
						lua_pushlightuserdata(L, (void*)func);
						push_functor(L, func);
						lua_rawset(L, -5);
						push_register(L, name, getFuncName(L, name, func).c_str());
					}
					else
					{
						lua_pushstring(L, getFuncName(L, name, func).c_str());
						lua_pushlightuserdata(L, (void*)func);
						push_functor(L, func);
						lua_rawset(L, -3);
					}
				}
				else if(lua_isfunction(L, -1))  //如果重载函数将其注册到元表中
				{
					lua_getmetatable(L, -2);
					
					lua_pushstring(L, name);
					lua_newtable(L);

					get_register(L, name);
					lua_pushvalue(L, -5);
					lua_rawset(L, -3);

					lua_pushstring(L, getFuncName(L, name, func).c_str());
					lua_pushlightuserdata(L, (void*)func);
					push_functor(L, func);
					lua_rawset(L, -3);

					lua_rawset(L, -3);

					lua_pushstring(L, name);
					lua_pushnil(L);
					lua_rawset(L, -5);
				}
			}
		}
		lua_pop(L, 4);
	}

	template<typename T, typename F>
	void class_static(lua_State* L, const char* name, F func) 
	{
		push_meta(L, class_name<T>::name());
		if(lua_istable(L, -1))		//类的表
		{
			lua_pushstring(L, name);
			lua_rawget(L, -2);
			if (lua_isnil(L, -1))
			{
				lua_remove(L, -1);
				lua_getmetatable(L, -1);

				lua_pushstring(L, name);
				lua_rawget(L, -2);
				if (lua_isnil(L, -1))
				{
					lua_pushstring(L, name);
					lua_pushlightuserdata(L, (void*)func);
					push_functor(L, func);
					lua_rawset(L, -5);
					push_register(L, name, getFuncName(L, name, func).c_str());
				}
				else
				{
					lua_pushstring(L, getFuncName(L, name, func).c_str());
					lua_pushlightuserdata(L, (void*)func);
					push_functor(L, func);
					lua_rawset(L, -3);
				}
			}
			else if(lua_isfunction(L, -1))  //如果重载函数将其注册到元表中
			{
				lua_getmetatable(L, -2);

				lua_pushstring(L, name);
				lua_newtable(L);

				get_register(L, name);
				lua_pushvalue(L, -5);
				lua_rawset(L, -3);

				lua_pushstring(L, getFuncName(L, name, func).c_str());
				lua_pushlightuserdata(L, (void*)func);
				push_functor(L, func);
				lua_rawset(L, -3);

				lua_rawset(L, -3);

				lua_pushstring(L, name);
				lua_pushnil(L);
				lua_rawset(L, -5);
			}
		}
		lua_pop(L, 3);
	}

	// Tinker Class Variables
	template<typename T, typename BASE, typename VAR>
	void class_memEx(lua_State* L, const char* name, VAR BASE::*val) 
	{ 
		push_meta(L, space_name::name());
		if(lua_istable(L, -1))
		{
			lua_pushstring(L, class_name<T>::name());
			lua_gettable(L, -2);
			if(lua_istable(L, -1))
			{
				lua_pushstring(L, name);
				new(lua_newuserdata(L,sizeof(mem_var<BASE,VAR>))) mem_var<BASE,VAR>(val);
				lua_rawset(L, -3);
			}
		}
		lua_pop(L, 2);
	}

	// Tinker Class Variables
	template<typename T, typename BASE, typename VAR>
	void class_mem(lua_State* L, const char* name, VAR BASE::*val) 
	{ 
		push_meta(L, class_name<T>::name());
		if(lua_istable(L, -1))
		{
			lua_pushstring(L, name);
			new(lua_newuserdata(L,sizeof(mem_var<BASE,VAR>))) mem_var<BASE,VAR>(val);
			lua_rawset(L, -3);
		}
		lua_pop(L, 1);
	}

	template<typename T>
	struct class_name
	{
		// global name
		static const char* name(const char* name = NULL)
		{
			static char temp[256] = "";
			if(name) strcpy(temp, name);
			return temp;
		}
	};

	// Table Object on Stack
	struct table_obj
	{
		table_obj(lua_State* L, int index, const char* name = nullptr);
		~table_obj();

		void inc_ref();
		void dec_ref();

		bool validate();

		template<typename T>
		void set(const char* name, T object)
		{
			if(validate())
			{
				lua_pushstring(m_L, name);
				push(m_L, object);
				lua_settable(m_L, m_index);
			}
		}

		template<typename T>
		T get(const char* name)
		{
			if(validate())
			{
				lua_pushstring(m_L, name);
				lua_gettable(m_L, m_index);
			}
			else
			{
				lua_pushnil(m_L);
			}

			return pop<T>(m_L);
		}

		lua_State*		m_L;
		int				m_index;
		const void*		m_pointer;
		int				m_ref;
		const char*		m_name;
	};

	// Table Object Holder
	struct table
	{
		table(lua_State* L);
		table(lua_State* L, int index);
		table(lua_State* L, const char* name);
		table(const table& input);
		~table();

		template<typename T>
		void set(const char* name, T object)
		{
			m_obj->set(name, object);
		}

		template<typename T>
		T get(const char* name)
		{
			return m_obj->get<T>(name);
		}

		table_obj*		m_obj;
	};

	bool is_table(table& t);

} // namespace lua_tinker

#endif //_LUA_TINKER_H_
