#ifdef LUA_RESETCOROUTINE

#include "LuaThreadMgr.h"
#include <windows.h>

#pragma warning( push, 2 )
extern "C" {
	#include "LuaPlus/src/lua.h"
	#include "LuaPlus/src/lauxlib.h"
};
#pragma warning( pop )

const int DEFAULT_TIMEOUT = 60 * 1000;
const char* CALL_TIMEOUT = "Call timeout";
const char* STACK_EXCEPTION = "Stack exception";

static unsigned int Time()
{
	return GetTickCount();
}

LuaThreadMgr::LuaThreadMgr(lua_State* _L, int initNum /* = 100 */)
{
	ASSERT(initNum != 0);

	L = _L;
	m_tail = NULL;
	m_usedTail = NULL;
	m_threadTimeout = DEFAULT_TIMEOUT;
	lua_createtable(L, initNum, 0);
	m_threadPool = luaL_ref(L, LUA_REGISTRYINDEX);
	this->AppendThread(initNum);
}

void LuaThreadMgr::AppendThread(int num)
{
	int startIndex = this->GetThreadNum() + 1;
	int endIndex = startIndex + num - 1;

	lua_rawgeti(L, LUA_REGISTRYINDEX, m_threadPool);
	for (; startIndex<=endIndex; ++startIndex)
	{
		// store a thread in the registry
		lua_newthread(L);
		lua_State* NL = lua_tothread(L, -1);
		lua_rawseti(L, -2, startIndex);

		// create a node
		Node* currNode = new Node();
		currNode->m_thread = NL;
		currNode->m_allocTime = 0;
		m_nodes[NL] = currNode;

		// make a unused list
		currNode->m_prev = m_tail;
		currNode->m_next = NULL;
		if (m_tail) m_tail->m_next = currNode;
		m_tail = currNode;
	}

	lua_pop(L, 1);
}

LuaThreadMgr::~LuaThreadMgr()
{
	NODE_MAP::iterator it = m_nodes.begin();
	for (; it != m_nodes.end(); ++it)
		delete it->second;

	luaL_unref(L, LUA_REGISTRYINDEX, m_threadPool);
}

int LuaThreadMgr::GetThreadNum() const
{
	return m_nodes.size();
}

void LuaThreadMgr::AddToList(Node*& listTail, Node* node)
{
	IF_NOT (node)
		return;

	if (!listTail)
	{
		listTail = node;
		node->m_prev = NULL;
		node->m_next = NULL;
	}
	else
	{
		listTail->m_next = node;
		node->m_prev = listTail;
		node->m_next = NULL;
		listTail = node;
	}
}

void LuaThreadMgr::RemoveFromList(Node*& listTail, Node* node)
{
	IF_NOT (node && listTail)
		return;

	if (node->m_next)
		node->m_next->m_prev = node->m_prev;
	if (node->m_prev)
		node->m_prev->m_next = node->m_next;
	if (!node->m_next)
		listTail = node->m_prev;
}

lua_State* LuaThreadMgr::Alloc()
{
	if (!m_tail)
		this->Collect();
	if (!m_tail)
		this->AppendThread(this->GetThreadNum() / 2 + 1);

	Node* tail = m_tail;
	tail->m_allocTime = Time();
	this->RemoveFromList(m_tail, tail);
	this->AddToList(m_usedTail, tail);
	return tail->m_thread;
}

void LuaThreadMgr::Free(lua_State* state)
{
	NODE_MAP::iterator it = m_nodes.find(state);
	IF_NOT (it != m_nodes.end())
		return;
	Node* node = it->second;
	IF_NOT (node)
		return;
	this->FreeNode(node);
}

void LuaThreadMgr::FreeNode(Node* node)
{
	IF_NOT (node)
		return;
	node->m_allocTime = 0;
	this->RemoveFromList(m_usedTail, node);
	this->AddToList(m_tail, node);
	this->ClearThread(node->m_thread);
}

bool LuaThreadMgr::IsUsed(lua_State* state) const
{
	IF_NOT (state)
		return false;
	NODE_MAP::const_iterator it = m_nodes.find(state);
	IF_NOT (it != m_nodes.end())
		return false;
	Node* node = it->second;
	IF_NOT (node)
		return false;
	return node->m_allocTime != 0;
}

void LuaThreadMgr::SetTimeout(unsigned int timeout)
{
	m_threadTimeout = timeout;
}

void LuaThreadMgr::ClearThread(lua_State* state)
{
	if (lua_status(state) != 0)
	{
		::LuaGetErrorProc()->BindLastError(state, STACK_EXCEPTION);
		lua_resetcoroutine(state);
	}

	IF_NOT (lua_gettop(state) == 0)
		lua_settop(state, 0);
}

void LuaThreadMgr::Collect()
{
	unsigned int currTime = Time();
	Node* node = m_usedTail;
	while (node)
	{
		Node* theNode = node;
		node = node->m_prev;
		if (currTime - theNode->m_allocTime > m_threadTimeout)
		{
			::LuaGetErrorProc()->BindLastError(theNode->m_thread, CALL_TIMEOUT);
			this->FreeNode(theNode);
		}
	}
}

#ifdef _DEBUG_API
#include <iostream>
void LuaThreadMgr::DumpList() const
{
	Node* node = m_tail;
	std::cout << "Unused list: ";
	while (node) {
		std::cout << node << "  ";
		node = node->m_prev;
	}
	std::cout << std::endl;

	node = m_usedTail;
	std::cout << "Used list: ";
	while (node) {
		std::cout << node << "  ";
		node = node->m_prev;
	}
	std::cout << std::endl << std::endl;
}
#endif

#endif