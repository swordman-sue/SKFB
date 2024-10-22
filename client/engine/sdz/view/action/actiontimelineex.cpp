#include "actiontimelineex.h"
#include "luaengine/luaengine.h"

//add by swordman sue 2016-12-8
IMPLEMENT_CLASS_GUI_INFO(ActionTimelineEx)

ActionTimelineEx::~ActionTimelineEx()
{
	for (auto info : m_endFuncMap)
	{
		LuaEngine::GetInstance()->DelLuaFuncID(info.second);
	}
	m_endFuncMap.clear();
}

ActionTimelineEx* ActionTimelineEx::create()
{
	ActionTimelineEx* object = new (std::nothrow) ActionTimelineEx();
	if (object && object->init())
	{
		object->autorelease();
		return object;
	}
	CC_SAFE_DELETE(object);
	return nullptr;
}

void ActionTimelineEx::SetAnimationEndListener(const std::string animationName, GameType::UINT32 func_id)
{
	AnimationEndFuncMap::iterator it = m_endFuncMap.find(animationName);
	if (it != m_endFuncMap.end())
	{
		LuaEngine::GetInstance()->DelLuaFuncID(it->second);
	}

	m_endFuncMap[animationName] = func_id;
	setAnimationEndCallFunc(animationName, CC_CALLBACK_0(ActionTimelineEx::AnimationEndListener, this, animationName));
}

void ActionTimelineEx::ClearAnimationEndListener(const std::string animationName)
{
	AnimationEndFuncMap::iterator it = m_endFuncMap.find(animationName);
	if (it != m_endFuncMap.end())
	{
		LuaEngine::GetInstance()->DelLuaFuncID(it->second);
		m_endFuncMap.erase(it);
	}
}

void ActionTimelineEx::AnimationEndListener(const std::string animationName)
{
	AnimationEndFuncMap::iterator it = m_endFuncMap.find(animationName);
	if (it != m_endFuncMap.end())
	{
		LuaEngine::GetInstance()->CallFunction(it->second, "");
	}
}

ActionTimelineEx* ActionTimelineEx::createCloneInstance() const
{
	return ActionTimelineEx::create();
}