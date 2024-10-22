#include "gamestatemachine.h"
#include "gamestate.h"

GameStateMachine::GameStateMachine()
{
	m_state_array.resize(MAX_STATE_NUM);
	m_state_sink_array.resize(MAX_STATE_SINK_NUM);

	for (StateId i = 0; i < MAX_STATE_NUM; i++)
	{
		m_free_state_id_list.insert(i);
	}

	for (SinkId i = 0; i < MAX_STATE_SINK_NUM; i++)
	{
		m_free_sink_id_list.insert(i);
	}
}

GameStateMachine::~GameStateMachine()
{
	for (SinkId i = 0; i < MAX_STATE_SINK_NUM; i++)
	{
		if(m_state_sink_array[i])
		{
			delete m_state_sink_array[i];
		}
	}
	m_state_sink_array.clear();

	for (StateId i = 0; i < MAX_STATE_NUM; i++)
	{
		if(m_state_array[i])
		{
			delete m_state_array[i];
		}
	}
	m_state_array.clear();
}

bool GameStateMachine::ChangeStateTest( StateId state_id, bool ignore_state_lock/*=false*/ )
{
	GameState* state = FindState(state_id);
	if(0 != state)
	{
		//状态不可重入时尝试进入相同状态直接失败
		if(IsInState(state_id))
		{
			return state->IsStateReEnter();
		}

		GameStateSink* sink = FindSink(state->GetSinkId());
		GameState* cur_sink_state = FindState(sink->cur_state_id);

		//当前状态被锁定时没有忽略状态锁则状态切换失败
		if(cur_sink_state && cur_sink_state->IsStateLock() && (!ignore_state_lock))
		{
			return false;
		}

		if(state->GetComposeType()==SCT_SUB)
		{
			GameState* parent_state = FindState(state->m_parent_state_id);
			//主状态处于激活状态
			if(IsInState(parent_state->GetId()))
			{
				if(state->CanChangeFromState(parent_state->m_cur_run_sub))
				{
					return true;
				}
			}
			else
			{
				if(parent_state->CanChangeFromState(sink->cur_state_id))
				{
					return true;
				}
			}
		}
		else
		{
			if(state->CanChangeFromState(sink->cur_state_id))
			{
				return true;
			}
		}

	}
	return false;
}

bool GameStateMachine::ChangeStateTest( const std::string& state_name, bool ignore_state_lock)
{
	return ChangeStateTest(GetStateIdFromName(state_name), ignore_state_lock);
}

bool GameStateMachine::ChangeState(StateId state_id,  bool ignore_state_lock)
{
	GameState* state = FindState(state_id);
	if(state)
	{
		//状态不可重入时尝试进入相同状态直接失败
		if(IsInState(state_id))
		{
			if(state->IsStateReEnter())
			{
				state->Exit();
				state->Enter();
				return true;
			}
			else
			{
				return false;
			}
		}

		GameStateSink* sink=FindSink(state->GetSinkId());
		GameState* cur_sink_state = FindState(sink->cur_state_id);
		
		//当前状态被锁定时没有忽略状态锁则状态切换失败
		if(cur_sink_state && cur_sink_state->IsStateLock() && (!ignore_state_lock))
		{
			return false;
		}
			
		if(state->GetComposeType()==SCT_SUB)
		{
			GameState* parent_state = FindState(state->m_parent_state_id);
			
			//主状态处于激活状态
			if(IsInState(parent_state->GetId()))
			{
				GameState* cur_run_state = FindState(parent_state->m_cur_run_sub);
				if(state->CanChangeFromState(parent_state->m_cur_run_sub))
				{
					if(cur_run_state)
					{
						cur_run_state->Exit();
					}
					
					parent_state->m_previous_sub = parent_state->m_cur_run_sub;
					parent_state->m_cur_run_sub = state_id;

					state->Enter();

					return true;
				}
			}
			else
			{
				if(parent_state->CanChangeFromState(sink->cur_state_id))
				{
					parent_state->m_previous_sub = parent_state->m_cur_run_sub;
					parent_state->m_cur_run_sub = state_id;
					
					if(cur_sink_state)
					{
						cur_sink_state->Exit();
					}

					sink->previous_state_id = sink->cur_state_id;
					sink->cur_state_id = parent_state->GetId();

					if(parent_state)
					{
						parent_state->Enter();
					}

					return true;
				}
			}
		}
		else
		{
			if(state->CanChangeFromState(sink->cur_state_id))
			{
				if(cur_sink_state)
				{
					cur_sink_state->Exit();
				}

				sink->previous_state_id = sink->cur_state_id;
				sink->cur_state_id = state_id;

				state->Enter();

				return true;
			}
		}
	}

	return false;
}

bool GameStateMachine::ChangeState( const std::string& state_name, bool ignore_state_lock )
{
	return ChangeState(GetStateIdFromName(state_name), ignore_state_lock);
}

bool GameStateMachine::ChangeStateForce( StateId state_id )
{
	GameState* state = FindState(state_id);
	if(state)
	{
		//状态不可重入时尝试进入相同状态直接失败
		if(IsInState(state_id))
		{
			if(state->IsStateReEnter())
			{
				state->Exit();
				state->Enter();
				return true;
			}
			else
			{
				return false;
			}
		}

		GameStateSink* sink=FindSink(state->GetSinkId());
		GameState* cur_sink_state = FindState(sink->cur_state_id);
			
		if(state->GetComposeType()==SCT_SUB)
		{
			GameState* parent_state = FindState(state->m_parent_state_id);
			//主状态处于激活状态
			if(IsInState(parent_state->GetId()))
			{
				GameState* cur_run_state = FindState(parent_state->m_cur_run_sub);
				if(cur_run_state)
				{
					cur_run_state->Exit();
				}

				parent_state->m_previous_sub = parent_state->m_cur_run_sub;
				parent_state->m_cur_run_sub = state_id;

				state->Enter();
				return true;
			}
			else
			{
				parent_state->m_previous_sub = parent_state->m_cur_run_sub;
				parent_state->m_cur_run_sub = state_id;

				if(cur_sink_state)
				{
					cur_sink_state->Exit();
				}

				sink->previous_state_id = sink->cur_state_id;
				sink->cur_state_id = parent_state->GetId();

				if(parent_state)
				{
					parent_state->Enter();
				}

				return true;
			}
		}
		else
		{
			if(cur_sink_state)
			{
				cur_sink_state->Exit();
			}

			sink->previous_state_id = sink->cur_state_id;
			sink->cur_state_id = state_id;

			state->Enter();

			return true;
		}
	}
	return false;
}

bool GameStateMachine::ChangeStateForce( const std::string state_name )
{
	return ChangeStateForce(GetStateIdFromName(state_name));
}

void GameStateMachine::Update(float elapse_time)
{
	for (size_t i = 0; i < m_state_sink_array.size(); i++)
	{
		GameStateSink* sink = m_state_sink_array[i];
		if(0 != sink)
		{
			GameState* state = FindState(sink->cur_state_id);
			if(state)
			{
				state->Execute(elapse_time);
			}
		}
	}
}

GameState* GameStateMachine::FindState( StateId state_id )
{
	GameState* state = 0;
	if(state_id != NULL_STATE_ID)
	{
		if(state_id < m_state_array.size())
		{
			state = m_state_array[state_id];
		}
	}

	return state;
}

GameState* GameStateMachine::FindState( const std::string& name )
{
	GameState* state = 0;
	for (size_t i = 0; i < m_state_array.size(); ++i)
	{
		if(m_state_array[i] && m_state_array[i]->GetName() == name)
		{
			state = m_state_array[i];
			break;
		}
	}
	return state;
}


void GameStateMachine::DestroyState( StateId state_id )
{
	GameState* state = FindState(state_id);
	if(state)
	{
		GameStateSink* sink = FindSink(state->GetSinkId());
		StateId cur_run_state_id = sink->cur_state_id;
			
		sink->contain_state_id_list.erase(state_id);
			
		//先退出要删除的状态(此处可能导致Previous状态非法,但不影响正常使用)
		bool need_cancel_cur_state = false;
		if(IsInState(state_id))
		{
			state->Exit();
			need_cancel_cur_state = true;
		}

		//子状态
		if(state->GetComposeType() == SCT_SUB)
		{
			GameState* parent_state = FindState(state->m_parent_state_id);
			if(need_cancel_cur_state)
			{
				parent_state->m_cur_run_sub = NULL_STATE_ID;
				parent_state->m_previous_sub = NULL_STATE_ID;
			}
				
			parent_state->m_sub_state_id_list.erase(state_id);
				
				
		}
		else
		{
			//从Sink上移除对应ID
			sink->contain_state_id_list.erase(state_id);
				
			if(need_cancel_cur_state)
			{
				sink->cur_state_id = NULL_STATE_ID;
				sink->previous_state_id = NULL_STATE_ID;
			}


			if(state->GetComposeType() == SCT_COMPOSE)
			{
				state->DestroyAllSubStates();
			}
		}

			
		delete state;
		m_state_array[state_id] = 0;
		m_free_state_id_list.insert(state_id);
	}
}

void GameStateMachine::DestroyState( const std::string& state_name )
{
	DestroyState(GetStateIdFromName(state_name));
}

GameStateMachine::GameStateSink* GameStateMachine::FindSink( SinkId sink_id )
{
	GameStateSink* sink = 0;
	if(sink_id != NULL_STATE_SINK_ID)
	{
		if(m_state_sink_array.size())
		{
			sink = m_state_sink_array[sink_id];
		}
	}
	return sink;
}

GameStateMachine::GameStateSink* GameStateMachine::FindSink( const std::string& name )
{
	GameStateSink* sink = 0;
	for (size_t i = 0; i < m_state_sink_array.size(); i++)
	{
		if(m_state_sink_array[i] && (m_state_sink_array[i]->sink_name == name))
		{
			sink = m_state_sink_array[i];
			break;
		}
	}
	return sink;
}

bool GameStateMachine::IsInState( StateId state_id )
{
	bool is_in_state = false;
	GameState* state = FindState(state_id);
	if(state)
	{
		GameStateSink* sink = FindSink(state->GetSinkId());
		if(state->GetComposeType() == SCT_SUB)
		{
			GameState* parent_state = FindState(state->m_parent_state_id);
			if(parent_state && (parent_state->GetId() == sink->cur_state_id))
			{
				is_in_state = (parent_state->m_cur_run_sub == state_id);
			}
		}
		else
		{
			is_in_state = (sink->cur_state_id == state_id);
		}
	}
	return is_in_state;
}

bool GameStateMachine::IsInState( const std::string& state_name )
{
	return IsInState(GetStateIdFromName(state_name));
}

bool GameStateMachine::CreateSinkImpl( SinkId sink_id, const std::string& set_name)
{
	//创建一个新的Sink
	if((sink_id < MAX_STATE_SINK_NUM) && (m_state_sink_array[sink_id] == 0))
	{

		GameStateSink* sink = new GameStateSink();
		sink->id = sink_id;
		sink->sink_name = set_name;
		sink->cur_state_id = NULL_STATE_ID;
		sink->previous_state_id = NULL_STATE_ID;
		m_state_sink_array[sink_id]  = sink;
		m_free_sink_id_list.erase(sink_id);
		return true;
	}
	else
	{
		return false;
	}
}

GameState* GameStateMachine::CreateStateImpl( StateId state_id, 
	const std::string& state_name, 
	SinkId sink_id, 
	StateComposeType com_type, 
	StateId parent_id/*=NULL_STATE_ID*/ )
{
	GameState* state = 0;
	GameStateSink* sink = FindSink(sink_id);
	if( (state_id < MAX_STATE_NUM) 
		&& (0 == m_state_array[state_id]) 
		&& (0 != sink))
	{
		if((com_type == SCT_COMPOSE) || (com_type == SCT_NORMAL))
		{
			state = new GameState(this, state_id, state_name);
			state->m_cur_run_sub = NULL_STATE_ID;
			sink->contain_state_id_list.insert(state_id);
		}
		else if(com_type == SCT_SUB)
		{
			GameState* parent_state = FindState(parent_id);
			if(parent_state && (parent_state->GetComposeType() == SCT_COMPOSE) && (parent_state->GetSinkId() == sink_id))
			{
				state = new GameState(this, state_id, state_name);
				state->m_parent_state_id = parent_id;
				parent_state->m_sub_state_id_list.insert(state_id);
			}
		}
		if(state)
		{
			state->m_sink_id = sink_id;
			state->m_compose_type = com_type;
			m_state_array[state_id] = state;
			m_free_state_id_list.erase(state_id);
		}

	}
	return state;
}
	
StateId GameStateMachine::GetSinkRunState(SinkId sink_id)
{
	StateId find_id = NULL_STATE_ID;
	GameStateSink* sink = FindSink(sink_id);
	if(sink)
	{
		find_id = sink->cur_state_id;
	}
	return find_id;
}

std::string GameStateMachine::GetSinkRunState( const std::string& sink_name )
{
	return GetStateNameFromId(GetSinkRunState(GetSinkIdFromName(sink_name)));
}

std::string GameStateMachine::GetSinkPreviousState( const std::string& sink_name)
{
	StateId find_id = NULL_STATE_ID;
	SinkId sink_id = GetSinkIdFromName(sink_name);
	GameStateSink* sink = FindSink(sink_id);
	if(sink)
	{
		find_id = sink->previous_state_id;
	}
	return GetStateNameFromId(find_id);
}

void GameStateMachine::SinkToNullState(SinkId sink_id)
{
	GameStateSink* sink = FindSink(sink_id);
	if(sink)
	{
		sink->previous_state_id = sink->cur_state_id;
		GameState* state = FindState(sink->cur_state_id);
		if(state)
		{
			state->Exit();
		}
		sink->cur_state_id = NULL_STATE_ID;
	}
}

void GameStateMachine::SinkToNullState( const std::string& sink_name )
{
	SinkToNullState(GetSinkIdFromName(sink_name));
}

unsigned int GameStateMachine::GetCurActionFlag()
{
	unsigned int actionFlag=0xffffffffu;
	return actionFlag;
}

std::string GameStateMachine::GetDefaultStateNameFromId( StateId id )
{
	char buf[32];
	memset(buf,0,sizeof(buf));
	sprintf(buf, "_def_state_%d", (int)id);
	return std::string(buf);
}

std::string GameStateMachine::GetDefaultSinkNameFromId( SinkId id )
{
	char buf[32];
	memset(buf,0,sizeof(buf));
	sprintf(buf, "_def_sink_%d", (int)id);
	return std::string(buf);
}

bool GameStateMachine::CreateSink( SinkId sink_id )
{
	std::string sink_name = GetDefaultSinkNameFromId(sink_id);
	return CreateSinkImpl(sink_id, sink_name);
}

bool GameStateMachine::CreateSink( const std::string& sink_name )
{
	if(!m_free_sink_id_list.empty())
	{
		SinkId id = *(m_free_sink_id_list.begin());
		return CreateSinkImpl(id, sink_name);
	}
	return false;
}

GameState*  GameStateMachine::CreateNormalState( StateId state_id, SinkId sink_id )
{
	std::string state_name = GetDefaultStateNameFromId(state_id);
	return CreateStateImpl(state_id, state_name, sink_id, SCT_NORMAL, NULL_STATE_ID);
}

GameState*  GameStateMachine::CreateNormalState( const std::string& state_name, const std::string& sink_name )
{
	GameStateSink* sink = FindSink(sink_name);
	if(!sink)
	{
		return 0;
	}

	if(!m_free_state_id_list.empty())
	{
		StateId id = *(m_free_state_id_list.begin());
		return CreateStateImpl(id, state_name, sink->id, SCT_NORMAL, NULL_STATE_ID);
	}
	return 0;
}

GameState* GameStateMachine::CreateComposeState( StateId state_id, SinkId sink_id )
{
	std::string state_name = GetDefaultStateNameFromId(state_id);
	return CreateStateImpl(state_id, state_name, sink_id, SCT_COMPOSE, NULL_STATE_ID);
}

GameState* GameStateMachine::CreateComposeState( const std::string& state_name, const std::string& sink_name )
{
	GameStateSink* sink = FindSink(sink_name);
	if(!sink)
	{
		return 0;
	}

	if(!m_free_state_id_list.empty())
	{
		StateId id = *(m_free_state_id_list.begin());
		return CreateStateImpl(id, state_name, sink->id, SCT_COMPOSE, NULL_STATE_ID);
	}
	return 0;
}

GameState* GameStateMachine::CreateSubState( StateId state_id, SinkId sink_id, StateId parent_id )
{
	std::string state_name = GetDefaultStateNameFromId(state_id);
	return CreateStateImpl(state_id, state_name, sink_id, SCT_SUB, parent_id);
}

GameState* GameStateMachine::CreateSubState( const std::string& state_name, const std::string& sink_name, const std::string& parent_name )
{
	GameStateSink* sink = FindSink(sink_name);
	if(!sink)
	{
		return 0;
	}

	GameState* parent_state = FindState(parent_name);
	if(!parent_state)
	{
		return 0;
	}

	if(!m_free_state_id_list.empty())
	{
		StateId id = *(m_free_state_id_list.begin());
		return CreateStateImpl(id, state_name, sink->id, SCT_SUB, parent_state->GetId());
	}
	return 0;
}

std::string GameStateMachine::GetStateNameFromId( StateId state_id )
{
	std::string tmp_name;
	GameState* state = FindState(state_id);
	if(state)
	{
		tmp_name = state->GetName();
	}
	return tmp_name;
}

StateId GameStateMachine::GetStateIdFromName( const std::string& name )
{
	GameState* state = FindState(name);
	if(state)
	{
		return state->GetId();
	}
	else
	{
		return NULL_STATE_ID;
	}
}

std::string GameStateMachine::GetSinkNameFromId( SinkId sink_id )
{
	std::string tmp_name;
	GameStateSink* sink = FindSink(sink_id);
	if(sink)
	{
		tmp_name = sink->sink_name;
	}
	return tmp_name;
}

SinkId GameStateMachine::GetSinkIdFromName( const std::string& name )
{
	GameStateSink* sink = FindSink(name);
	if(sink)
	{
		return sink->id;
	}
	else
	{
		return NULL_STATE_SINK_ID;
	}
}
