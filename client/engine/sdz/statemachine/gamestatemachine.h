#ifndef GAMESTATEMACHINE_H
#define GAMESTATEMACHINE_H

#include "gamestatepredependency.h"

class GameStateMachine
{
private:
	struct GameStateSink
	{
		SinkId id;
		std::string sink_name;
		StateId cur_state_id;
		StateId previous_state_id;

		StateIdList contain_state_id_list;
	};

public:
	GameStateMachine();
	~GameStateMachine();

	//调用这个更新FSM
	void Update(float elapse_time);

	//改变到一个新状态(系统会自动识别Sink)
	bool ChangeState(StateId state_id, bool ignore_state_lock = false);
	bool ChangeState(const std::string& state_name, bool ignore_state_lock = false);

	//强制切换状态机至某状态
	bool ChangeStateForce(StateId state_id);
	bool ChangeStateForce(const std::string state_name);

	//能否切换到该状态
	bool ChangeStateTest(StateId state_id, bool ignore_state_lock = false);
	bool ChangeStateTest(const std::string& state_name, bool ignore_state_lock = false);

	bool IsInState(StateId state_id);
	bool IsInState(const std::string& state_name);

	GameState* FindState(StateId state_id);

	GameState* FindState(const std::string& name);

	StateId GetSinkRunState(SinkId sink_id); 

	std::string GetSinkRunState(const std::string& sink_name);

	std::string GetSinkPreviousState(const std::string& sink_name);
		
	unsigned int GetCurActionFlag();

	void SinkToNullState(SinkId sink_id);
	void SinkToNullState(const std::string& sink_name);

	bool CreateSink(SinkId sink_id);
	bool CreateSink(const std::string& sink_name);

	GameState* CreateNormalState(StateId state_id, SinkId sink_id);
	GameState* CreateComposeState(StateId state_id, SinkId sink_id);
	GameState* CreateSubState(StateId state_id, SinkId sink_id, StateId parent_id);

	GameState* CreateNormalState(const std::string& state_name, const std::string& sink_name);
	GameState* CreateComposeState(const std::string& state_name, const std::string& sink_name);
	GameState* CreateSubState(const std::string& state_name, const std::string& sink_name, const std::string& parent_name);

	void DestroyState(StateId state_id);
	void DestroyState(const std::string& state_name);

	std::string GetStateNameFromId(StateId state_id);
	StateId GetStateIdFromName(const std::string& name);

	std::string GetSinkNameFromId(SinkId sink_id);
	SinkId GetSinkIdFromName(const std::string& name);

private:
	GameState* CreateStateImpl( StateId state_id, const std::string& state_name, SinkId sink_id, StateComposeType com_type, StateId parent_id = NULL_STATE_ID);
	bool CreateSinkImpl(SinkId sink_id, const std::string& set_name);
	GameStateSink* FindSink(SinkId sink_id);
	GameStateSink* FindSink(const std::string& name);
	std::string GetDefaultStateNameFromId(StateId id);
	std::string GetDefaultSinkNameFromId(SinkId id);

private:
	typedef std::vector<GameStateSink*> StateSinkArray;
	StateSinkArray m_state_sink_array;
	GameStateArray m_state_array;
	
	StateIdList m_free_state_id_list;
	SinkIdList m_free_sink_id_list;
};

#endif 
