#include "timerquest.h"
#include <vector>
#include "gameengine/gameengine.h"
#include "luaengine/luaengine.h"
#include "zqcommon/logutil.h"

USING_NS_CC;

TimerQuestItem::TimerQuestItem(unsigned int id, TimerQuest* agent, ZQ::UINT32 quest_func)
	:m_id(id)
	,m_parent(agent)
	,m_timer_func(quest_func)
	,m_is_pause(false)
	,m_pause_ref_count(0)
	,m_is_always_run(false)
{

}

TimerQuestItem::~TimerQuestItem()
{
	if(m_timer_func != 0)
	{
		LuaEngine::GetInstance()->DelLuaFuncID(m_timer_func);
		m_timer_func = 0;
	}
}

bool TimerQuestItem::Run()
{
	if (!m_parent->_IsQuestAlive(m_id))
	{
		//外界已经不再执有该任务,删除该任务
		return true;	
	}

	bool need_del = false;

	if (m_is_pause)
	{
		//暂停中
		return need_del;
	}
		
	int ret_val = LuaEngine::GetInstance()->CallFunction(m_timer_func, "");
	need_del = (ret_val != 0);
	if (need_del)
	{
		//脚本运行返回需要删除指令
		return true; 
	}

	m_run_count++;
	m_next_run_time += m_run_period;

	if ((!m_is_always_run) && (m_next_run_time > m_last_time))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void TimerQuestItem::Pause()
{
	++m_pause_ref_count;
	if (m_pause_ref_count == 1)
	{
		m_is_pause = true;
		m_pause_time = GameEngine::GetInstance()->GetNowTime();
	}
}

void TimerQuestItem::Resume()
{
	if (m_pause_ref_count <= 0)
	{
		return;
	}

	--m_pause_ref_count;
	if (m_pause_ref_count == 0)
	{
		m_is_pause = false;
		double elapse_time = GameEngine::GetInstance()->GetNowTime() - m_pause_time;
		m_next_run_time += elapse_time;
		m_last_time += elapse_time;
	}
}

//-------------------------------------------------------------------------
TimerQuest::TimerQuest()
	: m_id_count(0)
	, m_now_time(0.0)
	, m_is_pause(false)
{

}

TimerQuest::~TimerQuest()
{
	Stop();
}

void TimerQuest::Stop()
{
	for (TimerQuestItemMap::iterator ihandle = m_timer_quest_map.begin(); ihandle != m_timer_quest_map.end(); ihandle++)
	{
		delete ihandle->second;
	}
	m_timer_quest_map.clear();
}

void TimerQuest::Update(double now_time, double elapse_time)
{
	if (m_is_pause)
	{
		return;
	}

	m_now_time = now_time;
	TimerQuestItemMap::iterator iquest = m_timer_quest_map.upper_bound(m_now_time);
	if(iquest == m_timer_quest_map.begin())
	{
		return;
	}

	typedef std::vector<TimerQuestItem*> TimerQuestItemVector;
	TimerQuestItemVector tmp_quest_array;

	for (TimerQuestItemMap::iterator ihandle = m_timer_quest_map.begin(); ihandle != iquest; ihandle++)
	{
		tmp_quest_array.push_back(ihandle->second);
	}
	m_timer_quest_map.erase(m_timer_quest_map.begin(), iquest);

	//逐项对到时间的任务进行处理
	for (size_t i = 0; i < tmp_quest_array.size(); i++)
	{
		TimerQuestItem* quest = tmp_quest_array[i];
		bool need_del = quest->Run();
		if(!need_del)
		{
			m_timer_quest_map.insert(std::make_pair(quest->m_next_run_time, quest));
		}
		else
		{
			CancelQuest(quest->m_id);
			delete quest;
		}
	}
}

bool TimerQuest::_IsQuestAlive( unsigned int id )
{
	if(m_keep_id_set.find(id) != m_keep_id_set.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

unsigned int TimerQuest::AddDelayQuest(ZQ::UINT32 quest_func, double delay_time)
{
	TimerQuestItem* quest = new TimerQuestItem(++m_id_count, this, quest_func);
	quest->m_is_always_run = false;
	quest->m_next_run_time = m_now_time + delay_time;
	quest->m_run_period = 10.0;
	quest->m_last_time = quest->m_next_run_time + 1.0;
	quest->m_run_count = 0;
	if (m_is_pause)
	{
		quest->Pause();
	}

	m_timer_quest_map.insert(std::make_pair(quest->m_next_run_time, quest));

	m_keep_id_set.insert(quest->m_id);
	return quest->m_id;
}

unsigned int TimerQuest::AddPeriodQuest(ZQ::UINT32 quest_func, double period, double last_time)
{
	TimerQuestItem* quest = new TimerQuestItem(++m_id_count, this, quest_func);
	quest->m_is_always_run = (last_time < 0.0);
	quest->m_next_run_time = m_now_time + period;
	quest->m_run_period = period;
	quest->m_last_time = m_now_time + last_time;
	quest->m_run_count = 0;
	if (m_is_pause)
	{
		quest->Pause();
	}

	m_timer_quest_map.insert(std::make_pair(quest->m_next_run_time, quest));

	m_keep_id_set.insert(quest->m_id);
	return quest->m_id;
}

unsigned int TimerQuest::AddRunTimesQuest(ZQ::UINT32 quest_func, double period, unsigned int run_count)
{
	TimerQuestItem* quest = new TimerQuestItem(++m_id_count, this, quest_func);
	quest->m_is_always_run = false;
	quest->m_next_run_time = m_now_time + period;
	quest->m_run_period = period;
	quest->m_last_time = m_now_time + period*run_count + 0.1*period; //额外追加一点时间,防止浮点误差
	quest->m_run_count = 0;
	if (m_is_pause)
	{
		quest->Pause();
	}

	m_timer_quest_map.insert(std::make_pair(quest->m_next_run_time, quest));	

	m_keep_id_set.insert(quest->m_id);

	return quest->m_id;
}

void TimerQuest::CancelQuest( unsigned int quest_id )
{
	m_keep_id_set.erase(quest_id);
}

void TimerQuest::PauseQuest( unsigned int quest_id )
{
	if (_IsQuestAlive(quest_id))
	{
		TimerQuestItem* pQuestItem = NULL;
		for (TimerQuestItemMap::iterator ihandle = m_timer_quest_map.begin(); ihandle != m_timer_quest_map.end(); ihandle++)
		{
			pQuestItem = ihandle->second;
			if (pQuestItem->m_id == quest_id)
			{
				pQuestItem->Pause();
			}
		}
	}
}

void TimerQuest::ResumeQuest( unsigned int quest_id )
{
	if (_IsQuestAlive(quest_id))
	{
		TimerQuestItem* pQuestItem = NULL;
		for (TimerQuestItemMap::iterator ihandle = m_timer_quest_map.begin(); ihandle != m_timer_quest_map.end(); ihandle++)
		{
			pQuestItem = ihandle->second;
			if (pQuestItem->m_id == quest_id)
			{
				pQuestItem->Resume();
			}
		}
	}
}

// 暂停所有任务
void TimerQuest::Pause()
{
	if (m_is_pause)
	{
		return;
	}
	m_is_pause = true;

	TimerQuestItem* pQuestItem = NULL;
	for (TimerQuestItemMap::iterator ihandle = m_timer_quest_map.begin(); ihandle != m_timer_quest_map.end(); ihandle++)
	{
		pQuestItem = ihandle->second;
		if (_IsQuestAlive(pQuestItem->m_id))
		{
			pQuestItem->Pause();
		}
	}
}

// 恢复所有任务
void TimerQuest::Resume()
{
	if (!m_is_pause)
	{
		return;
	}
	m_is_pause = false;
	m_now_time = GameEngine::GetInstance()->GetNowTime();

	TimerQuestItemMap tmp_map;
	tmp_map.swap(m_timer_quest_map);
	TimerQuestItem* pQuestItem = NULL;
	for (TimerQuestItemMap::iterator ihandle = tmp_map.begin(); ihandle != tmp_map.end(); ihandle++)
	{
		pQuestItem = ihandle->second;
		if (_IsQuestAlive(pQuestItem->m_id))
		{
			pQuestItem->Resume();
		}
		m_timer_quest_map.insert(std::make_pair(pQuestItem->m_next_run_time, pQuestItem));
	}
}
