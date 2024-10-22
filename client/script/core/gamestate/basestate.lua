
--[[
%% @module: 状态机基类
%% @author: swordman sue
%% @created: 2016-07-06
--]]

GameState = GameState or {}

GameState.BaseState = GameState.BaseState or BaseClass(EventAndTimerWrapper)

function GameState.BaseState:__init()
	self.__name = "unknown"
	self.__start_time = 0
end

function GameState.BaseState:__delete()
end

function GameState.BaseState:GetName()
	return self.__name
end

function GameState.BaseState:SetData(data)
	self.__data = data
end

function GameState.BaseState:StateEnter(state_name)
end

function GameState.BaseState:StateUpdate(state_name, elapse_time)
end

function GameState.BaseState:StateQuit(state_name)
end

function GameState.BaseState:GetFuncIDS()
	local enter_func = function(state_name)
		self:StateEnter(state_name)
	end
	local update_func = function(state_name, elapse_time)
		self:StateUpdate(state_name, elapse_time)
	end
	local quit_func = function(state_name)
		self:StateQuit(state_name)
	end
	return GlobalCallbackMgr:AddCallBackFunc(enter_func), GlobalCallbackMgr:AddCallBackFunc(update_func), GlobalCallbackMgr:AddCallBackFunc(quit_func)
end

