
--[[
%% @module: 主状态管理器
%% @author: swordman sue
%% @created: 2016-07-07
--]]

GameState = GameState or {}

GameState.StateMgr = GameState.StateMgr or BaseClass()

function GameState.StateMgr.CreateInstance()
	if not GlobalStateMgr then
		GlobalStateMgr = GameState.StateMgr.New()
	end
end

function GameState.StateMgr.DelInstance()
	if GlobalStateMgr then
		GlobalStateMgr:DeleteMe()
		GlobalStateMgr = nil
	end
end

function GameState.StateMgr:__init()
	self.__main_sink_name = "GameLoop"
	self.__state_machine = CPPGameStateMachine()
	self.__state_machine:CreateSink(self.__main_sink_name)

	self.__init_state = GameState.InitState.New()
	self.__resource_update_state = GameState.ResourceUpdateState.New()
	self.__connect_server_state = GameState.ConnectServerState.New()
	self.__login_state = GameState.LoginState.New()
	self.__server_list_state = GameState.ServerListState.New()
	self.__enter_game_state = GameState.EnterGameState.New()
	self.__scene_change_state = GameState.SceneChangeState.New()
	self.__game_play_state = GameState.GamePlayState.New()	

	----------------------------初始化状态----------------------------
	local tmp_state
	tmp_state = self.__state_machine:CreateComposeState(self.__init_state:GetName(), self.__main_sink_name)
	tmp_state:SetCallbackAsMultiFuctions(self.__init_state:GetFuncIDS())
	tmp_state:AddOutStateName(self.__resource_update_state:GetName())
	tmp_state:AddOutStateName(self.__connect_server_state:GetName())
	tmp_state:AddOutStateName(self.__scene_change_state:GetName())			

	--资源更新
	tmp_state = self.__state_machine:CreateSubState(self.__resource_update_state:GetName(), self.__main_sink_name, self.__init_state:GetName())
	tmp_state:SetStateCanReEnter(true)
	tmp_state:SetCallbackAsMultiFuctions(self.__resource_update_state:GetFuncIDS())
	tmp_state:AddOutStateName(self.__connect_server_state:GetName())	

	--连接服务器
	tmp_state = self.__state_machine:CreateSubState(self.__connect_server_state:GetName(), self.__main_sink_name, self.__init_state:GetName())
	tmp_state:SetStateCanReEnter(true)
	tmp_state:SetCallbackAsMultiFuctions(self.__connect_server_state:GetFuncIDS())
	tmp_state:AddOutStateName(self.__login_state:GetName())

	--登陆
	tmp_state = self.__state_machine:CreateSubState(self.__login_state:GetName(), self.__main_sink_name, self.__init_state:GetName())
	tmp_state:SetStateCanReEnter(true)
	tmp_state:SetCallbackAsMultiFuctions(self.__login_state:GetFuncIDS())
	tmp_state:AddOutStateName(self.__connect_server_state:GetName())	
	tmp_state:AddOutStateName(self.__server_list_state:GetName())	
	tmp_state:AddOutStateName(self.__enter_game_state:GetName())	

	--服务器列表
	tmp_state = self.__state_machine:CreateSubState(self.__server_list_state:GetName(), self.__main_sink_name, self.__init_state:GetName())
	tmp_state:SetStateCanReEnter(true)
	tmp_state:SetCallbackAsMultiFuctions(self.__server_list_state:GetFuncIDS())
	tmp_state:AddOutStateName(self.__login_state:GetName())
	tmp_state:AddOutStateName(self.__enter_game_state:GetName())	

	--进入游戏
	tmp_state = self.__state_machine:CreateSubState(self.__enter_game_state:GetName(), self.__main_sink_name, self.__init_state:GetName())
	tmp_state:SetStateCanReEnter(true)
	tmp_state:SetCallbackAsMultiFuctions(self.__enter_game_state:GetFuncIDS())
	-------------------------------------------------------------------

	-----------------------------其他状态------------------------------	
	tmp_state = self.__state_machine:CreateNormalState(self.__scene_change_state:GetName(), self.__main_sink_name)
	tmp_state:SetStateCanReEnter(true)
	tmp_state:SetCallbackAsMultiFuctions(self.__scene_change_state:GetFuncIDS())
	tmp_state:AddOutStateName(self.__game_play_state:GetName())

	tmp_state = self.__state_machine:CreateNormalState(self.__game_play_state:GetName(), self.__main_sink_name)
	tmp_state:SetCallbackAsMultiFuctions(self.__game_play_state:GetFuncIDS())
	tmp_state:AddOutStateName(self.__init_state:GetName())
	tmp_state:AddOutStateName(self.__scene_change_state:GetName())
	-------------------------------------------------------------------

    GlobalRunner:AddRunObj(self, Runner.Priority.RP_STATE_MGR)	
end

function GameState.StateMgr:__delete()
	self.__state_machine = nil

	GlobalRunner:RemoveRunObj(self)
end

function GameState.StateMgr:Update(now_time, elapse_time)
	self.__state_machine:Update(elapse_time)
end

function GameState.StateMgr:Start(start_callback)
	self.__start_callback = start_callback
	self:ToResourceUpdateState()
end

function GameState.StateMgr:SetStartCallback(var)
	self.__start_callback = var
end

function GameState.StateMgr:GetStartCallback()
	return self.__start_callback
end

function GameState.StateMgr:GetLoginStateSubType()
	return self.__login_state_sub_type
end

function GameState.StateMgr:ToResourceUpdateState()
	self.__state_machine:ChangeState(self.__resource_update_state:GetName(), false)
end

function GameState.StateMgr:ToConnectServerState()
	self.__state_machine:ChangeState(self.__connect_server_state:GetName(), false)
end

function GameState.StateMgr:ToLoginState(sub_type)
	self.__login_state_sub_type = sub_type or BaseViewSubType.Login.LOGIN
	self.__state_machine:ChangeState(self.__login_state:GetName(), false)
end

function GameState.StateMgr:ToServerListState()
	self.__state_machine:ChangeState(self.__server_list_state:GetName(), false)
end

function GameState.StateMgr:ToEnterGameState()
	self.__state_machine:ChangeState(self.__enter_game_state:GetName(), false)
end

function GameState.StateMgr:ToSceneChangeState()
	self.__state_machine:ChangeStateForce(self.__scene_change_state:GetName())
end

function GameState.StateMgr:ToGamePlayState()
	self.__state_machine:ChangeState(self.__game_play_state:GetName(), false)
end

function GameState.StateMgr:IsInitState()
	return self.__state_machine:IsInState("init_state")
end

function GameState.StateMgr:IsLoginState()
	return self.__state_machine:IsInState("login")
end

function GameState.StateMgr:IsGamePlayState()
	return self.__state_machine:IsInState("game_play")
end

function GameState.StateMgr:IsSceneChangeState()
	return self.__state_machine:IsInState("scene_change")
end
