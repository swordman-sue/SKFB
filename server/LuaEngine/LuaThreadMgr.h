#pragma once

#include <map>
#include "LuaErrorProc.h"
#include "Config.h"
#include "../ToolKit/Common.h"

struct lua_State;
class LuaThreadMgr
{
public:
	LuaThreadMgr(lua_State* L, int initNum = 100);
	~LuaThreadMgr();
	void SetTimeout(unsigned int timeout);
	int GetThreadNum() const;
	lua_State* Alloc();
	void Free(lua_State* state);
	bool IsUsed(lua_State* state) const;
#ifdef _DEBUG_API
	void DumpList() const;
#endif
private:
	void Collect();
	void AppendThread(int num);
	struct Node;
	void AddToList(Node*& listTail, Node* node);
	void RemoveFromList(Node*& listTail, Node* node);
	void ClearThread(lua_State* state);
	void FreeNode(Node* node);
private:
	// thread pool
	lua_State* L;
	int m_threadPool;

	// used and unused lists
	struct Node
	{
		Node() { m_prev = 0; m_next = 0; m_thread = 0; m_allocTime = 0; }
		Node* m_prev;
		Node* m_next;
		lua_State* m_thread;
		unsigned int m_allocTime;
	};
	Node* m_tail;
	Node* m_usedTail;
	typedef std::map<lua_State*, Node*> NODE_MAP;
	NODE_MAP m_nodes;

	// collect config
	unsigned int m_threadTimeout;
private:
	void operator=(const LuaThreadMgr&);
	LuaThreadMgr(const LuaThreadMgr&);
};