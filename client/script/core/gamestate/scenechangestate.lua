
--[[
%% @module: 切场景状态
%% @author: swordman sue
%% @created: 2016-07-14
%% @description: 预加载场景、切换场景
--]]

GameState = GameState or {}

GameState.SceneChangeState = GameState.SceneChangeState or BaseClass(GameState.BaseState)

function GameState.SceneChangeState:__init()
	self.__name = "scene_change"
end

function GameState.SceneChangeState:StateEnter(state_name)
	self.__load_finished = false
	self.__load_finished_ex = false
	self.__elapse_time = 0
	self.__change_state_time = nil

	CallModuleFunc(ModuleType.SP_LOGIC_MGR, "HandleEnterSceneChangeState", function(dump_login_views)
		--销毁登陆相关界面
		if dump_login_views then
			CallModuleFunc(ModuleType.LOGIN, "DumpViews")	
		end

		self:FireNextFrame(Macros.Event.GameState.SCENE_CHANGE_STATE_ENTER)		
	end)
end

function GameState.SceneChangeState:StateUpdate(state_name, elapse_time)
	self.__elapse_time = self.__elapse_time + elapse_time
	if self.__elapse_time < Scene.SceneChangeTimeout then

		if not self.__load_finished then
			if (GlobalScene:IsMapLoaded() and 
				CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsResLoaded")) then

				--加载完成/超时
				self.__load_finished = true

				--加载完成/超时通知
				CallModuleFunc(ModuleType.SP_LOGIC_MGR, "ResLoadedNotify")
			end

		elseif not self.__load_finished_ex and CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsResLoadedEx") then
			--加载完成II
			self.__load_finished_ex = true
			self.__change_state_time = Macros.Global.NowTime + (CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsInMainCity") and 0 or 0.1)
		end

	else
		if not self.__load_finished or not self.__load_finished_ex then
			self.__load_finished = true
			self.__load_finished_ex = true			
			self.__change_state_time = Macros.Global.NowTime + (CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsInMainCity") and 0 or 0.1)
		end
	end

	if self.__change_state_time and Macros.Global.NowTime >= self.__change_state_time then
		GlobalStateMgr:ToGamePlayState()
	end
end

function GameState.SceneChangeState:StateQuit(state_name)
	CallModuleFunc(ModuleType.SP_LOGIC_MGR, "HandleQuitSceneChangeState", function()
		self:FireNextFrame(Macros.Event.GameState.SCENE_CHANGE_STATE_QUIT)
	end)
end

