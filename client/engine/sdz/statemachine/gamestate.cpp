#include "gamestate.h"
#include "gamestatemachine.h"
#include "luaengine/luaengine.h"
#include "platform/CCPlatformMacros.h"

GameState::GameState(GameStateMachine* parent_machine,StateId state_id, const std::string& name) : 
	m_parent_machine(parent_machine),
	m_compose_type(SCT_NORMAL),
	m_state_id(state_id),
	m_action_flag(0xffffffffu),
	m_can_re_enter(false),
	m_is_state_lock(false),
	m_listener(0),
	m_parent_state_id(NULL_STATE_ID),
	m_state_name(name),
	m_callback_type(CALLT_NONE),
	m_callback_table_id(0),
	m_enter_func_id(0),
	m_execute_func_id(0),
	m_exit_func_id(0)
{
}

GameState::~GameState()
{
	ClearCallbackInfo();
}

bool GameState::CanChangeFromState(StateId old_state_id)
{
	bool can_change = false;
	GameState* old_state = m_parent_machine->FindState(old_state_id);
	if(old_state)
	{
		if(old_state->GetSinkId()==GetSinkId())
		{
			if((old_state->m_out_state_id_list.find(m_state_id) != old_state->m_out_state_id_list.end())||
				(old_state->m_out_state_name_list.find(m_state_name) != old_state->m_out_state_name_list.end()))
			{
				can_change = true;
			}
		}
	}
	else	//前置状态为空的情况
	{
		can_change = true;
	}
	return can_change;
}

void GameState::Enter()
{
	RunEnterFunction();

	if(m_compose_type==SCT_COMPOSE)
	{
		GameState* cur_sub_state = m_parent_machine->FindState(m_cur_run_sub);
		if(cur_sub_state)
		{
			cur_sub_state->Enter();
		}
	}
}

void GameState::Execute( float elapsedTime )
{
	RunExecuteFunction(elapsedTime);
	
	if(m_compose_type==SCT_COMPOSE)
	{
		GameState* cur_sub_state = m_parent_machine->FindState(m_cur_run_sub);
		if(cur_sub_state)
		{
			cur_sub_state->Execute(elapsedTime);
		}
	}
}

void GameState::Exit()
{
	if(m_compose_type==SCT_COMPOSE)
	{
		GameState* cur_sub_state = m_parent_machine->FindState(m_cur_run_sub);
		if(cur_sub_state)
		{
			cur_sub_state->Exit();
		}
	}
		
	RunExitFunction();
}

void GameState::RunEnterFunction()
{
	if(m_callback_type == CALLT_MULIT_FUNC_OBJ)
	{
		if(m_enter_func_id != 0)
		{
			LuaEngine::GetInstance()->CallFunction(m_enter_func_id, "s", m_state_name.c_str());
		}		
	}
	else if(m_callback_type == CALLT_FUNC_IN_TABLE)
	{		
	}
	else if(m_callback_type == CALLT_C_LISTNER)
	{
		if(m_listener)
		{
			m_listener->OnStateEnter(this);
		}
	}
}

void GameState::RunExitFunction()
{
	if(m_callback_type == CALLT_MULIT_FUNC_OBJ)
	{
		if(m_exit_func_id != 0)
		{
			LuaEngine::GetInstance()->CallFunction(m_exit_func_id, "s", m_state_name.c_str());
		}
	}
	else if(m_callback_type == CALLT_FUNC_IN_TABLE)
	{
	}
	else if(m_callback_type == CALLT_C_LISTNER)
	{
		if(m_listener)
		{
			m_listener->OnStateQuit(this);
		}
	}
}

void GameState::RunExecuteFunction(float elapse_time)
{
	if(m_callback_type == CALLT_MULIT_FUNC_OBJ)
	{
		if(m_execute_func_id != 0)
		{
			LuaEngine::GetInstance()->CallFunction(m_execute_func_id, "sd", m_state_name.c_str(), (double)elapse_time);
		}
	}
	else if(m_callback_type == CALLT_FUNC_IN_TABLE)
	{
	}
	else if(m_callback_type == CALLT_C_LISTNER)
	{
		if(m_listener)
		{
			m_listener->OnStateUpdate(this, elapse_time);
		}
	}
}

void GameState::AddOutStateId( StateId stateId )
{
	m_out_state_id_list.insert(stateId);
}

void GameState::RemoveOutStateId( StateId state_id )
{
	m_out_state_id_list.erase(state_id);	
}

unsigned int GameState::GetActionFlag()
{
	unsigned int tmp_act_flag = m_action_flag;
	if(m_compose_type == SCT_COMPOSE)
	{
		GameState* cur_sub_state = m_parent_machine->FindState(m_cur_run_sub);
		if(cur_sub_state)
		{
			tmp_act_flag = cur_sub_state->GetActionFlag();
		}
	}

	return tmp_act_flag;
}

bool GameState::IsStateReEnter() const
{
	//父状态始终可重入
	if(m_compose_type==SCT_COMPOSE)
	{
		GameState* cur_sub_state = m_parent_machine->FindState(m_cur_run_sub);
		if(cur_sub_state)
		{
			return cur_sub_state->IsStateReEnter();
		}
		else
		{
			return true;
		}
	}
	else
	{
		return m_can_re_enter;
	}
}

bool GameState::IsStateLock() const
{
	if(m_compose_type==SCT_COMPOSE)
	{
		GameState* cur_sub_state = m_parent_machine->FindState(m_cur_run_sub);
		if(cur_sub_state)
		{
			return cur_sub_state->IsStateLock();
		}
		else
		{
			return false;
		}
	}
	else
	{
		return m_is_state_lock;
	}
}

void GameState::SetSingleActionFlag( unsigned int aFlag, bool bSet/*=true*/ )
{
	if(bSet)
	{
		m_action_flag |=aFlag;
	}
	else
	{
		m_action_flag &= (~aFlag);
	}
}

void GameState::DestroyAllSubStates()
{
	if(m_compose_type==SCT_COMPOSE)
	{
		for (StateIdList::iterator i = m_sub_state_id_list.begin(); i != m_sub_state_id_list.end(); ++i)
		{
			m_parent_machine->DestroyState(*i);
		}
		m_sub_state_id_list.clear();
	}
}

void GameState::SetCallbackAsCListener( IGameStateListener* listener )
{
	ClearCallbackInfo();
	m_callback_type = CALLT_C_LISTNER;
	m_listener = listener;
}

void GameState::AddOutStateName( const std::string& state_name )
{
	m_out_state_name_list.insert(state_name);
}

void GameState::RemoveOutStateName( const std::string& state_name )
{
	m_out_state_name_list.erase(state_name);
}

void GameState::SetCallbackAsMultiFuctions( int enter_func_id, int execute_func_id, int exit_func_id )
{
	ClearCallbackInfo();
	m_callback_type = CALLT_MULIT_FUNC_OBJ;
	m_enter_func_id = enter_func_id;
	m_execute_func_id = execute_func_id;
	m_exit_func_id = exit_func_id;
}

//void GameState::SetCallbackAsTable( int table_id )
//{
//	ClearCallbackInfo();
//	m_callback_type = CALLT_FUNC_IN_TABLE;
//	m_callback_table_id = table_id;
//}

void GameState::ClearCallbackInfo()
{
	if(m_callback_type == CALLT_FUNC_IN_TABLE)
	{
		//m_callback_table_id = 0;
	}
	else if(m_callback_type == CALLT_MULIT_FUNC_OBJ)
	{
		LuaEngine::GetInstance()->DelLuaFuncID(m_enter_func_id);
		LuaEngine::GetInstance()->DelLuaFuncID(m_execute_func_id);
		LuaEngine::GetInstance()->DelLuaFuncID(m_exit_func_id);
		m_enter_func_id = 0;
		m_execute_func_id = 0;
		m_exit_func_id = 0;
	}
	else if(m_callback_type == CALLT_C_LISTNER)
	{
		if(m_listener)
		{
			m_listener->Free();
		}
		m_listener = 0;
	}

	m_callback_type = CALLT_NONE;
}
