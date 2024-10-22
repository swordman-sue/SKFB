#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "zqcommon/typedef.h"
#include "gamestatepredependency.h"

class GameStateMachine;

class GameState
{
	friend class GameStateMachine;

public:
	GameState(GameStateMachine* parent_machine, StateId state_id, const std::string& name);
	virtual ~GameState();

	void Enter();
	void Execute(float elapsedTime);
	void Exit();

	SinkId GetSinkId() const {return m_sink_id;}
	StateId GetId() const {return m_state_id;}
	const std::string& GetName() const{return m_state_name;}

	StateComposeType GetComposeType() const {return m_compose_type;}

	bool CanChangeFromState(StateId old_state_id);

	void SetCallbackAsCListener(IGameStateListener* listener);

	IGameStateListener* GetListener(){return m_listener;}

	void AddOutStateId(StateId state_id);

	void RemoveOutStateId(StateId state_id);

	void AddOutStateName(const std::string& state_name);

	void RemoveOutStateName(const std::string& state_name);

	void SetActionFlag(unsigned int actionFlag){m_action_flag=actionFlag;}

	unsigned int GetActionFlag();

	void SetSingleActionFlag(unsigned int aFlag, bool bSet=true);
	
	bool IsStateLock() const;
		
	void SetIsStateLock(bool isLock) {m_is_state_lock=isLock;}

	bool IsStateReEnter() const;
	void SetStateCanReEnter(bool canReEnter){m_can_re_enter=canReEnter;}

	void DestroyAllSubStates();

	void SetCallbackAsMultiFuctions(int enter_func_id, int execute_func_id, int exit_func_id);

	//void SetCallbackAsTable(int table_id);

protected:
	void ClearCallbackInfo();
	void RunEnterFunction();
	void RunExitFunction();
	void RunExecuteFunction(float elapse_time);

protected:
	StateId m_state_id;
	std::string m_state_name;

	GameStateMachine* m_parent_machine;
	SinkId m_sink_id;
	StateComposeType m_compose_type;

	StateIdList m_out_state_id_list;
	StateNameList m_out_state_name_list;

	//仅当mComposType 为 SCT_COMPOSE时下列各项有效
	StateIdList m_sub_state_id_list;
	StateId m_cur_run_sub;
	StateId m_previous_sub;
		
	StateId m_parent_state_id;
		
	IGameStateListener* m_listener;

	unsigned int m_action_flag;

	bool m_can_re_enter;
	bool m_is_state_lock;

	StateCallbackType m_callback_type;

	int m_callback_table_id;
		
	GameType::UINT32 m_enter_func_id;
	GameType::UINT32 m_execute_func_id;
	GameType::UINT32 m_exit_func_id;
};

#endif
