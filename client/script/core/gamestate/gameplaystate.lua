
--[[
%% @module: 游戏状态
%% @author: swordman sue
%% @created: 2016-07-14
--]]

GameState = GameState or {}

GameState.GamePlayState = GameState.GamePlayState or BaseClass(GameState.BaseState)

function GameState.GamePlayState:__init()
	self.__name = "game_play"
end

function GameState.GamePlayState:StateEnter(state_name)
	self:FireNextFrame(Macros.Event.GameState.GAME_PLAY_STATE_ENTER)

	CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GamePlayingStartHandle")	
end

function GameState.GamePlayState:StateUpdate(state_name, elapse_time)
	GlobalScene:Update(Macros.Global.NowTime, elapse_time)
end

function GameState.GamePlayState:StateQuit(state_name)
	self:FireNextFrame(Macros.Event.GameState.GAME_PLAY_STATE_QUIT)
end

