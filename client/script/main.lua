
--[[
%% @module: 逻辑层入口
%% @author: swordman sue
%% @created: 2016-07-07
--]]

require "core/gc"
require "core/baseclass"
require "core/runner"
require "macros/global"
require "startup"

local update_error_info

--游戏开始
function Start()
	LogTool.LogInfo("[MainScript::Start] 脚本启动")

	LuaGCHandle.StartCustomGC()
	Runner.CreateInstance()
	Startup.CreateInstance()
end

--每帧更新
function Update(now_time, now_time_milli, elapse_time)
	Macros.Global.NowTime = now_time
	Macros.Global.NowTimeMilli = now_time_milli
	Macros.Global.ElapseTime = elapse_time

	LuaGCHandle.GCCollectGarbage(elapse_time)
	if GlobalStartup then
		GlobalStartup:Update(now_time, elapse_time)
	end
	if GlobalRunner then
		GlobalRunner:Update(now_time, elapse_time)
	end
end

--每帧更新(错误回调)
function UpdateErr()
	-- 主循环中出现报错，通知玩家并上传报错日志
	local last_lua_err = LogTool:GetLastLuaError()
	if last_lua_err then
		LogTool:CommitLuaError()
		LogTool:ClearLastLuaError()

		update_error_info = update_error_info or {}
		if update_error_info.content ~= last_lua_err then
			update_error_info.content = last_lua_err
			update_error_info.catch_time = Macros.Global.NowTime
			update_error_info.count = 1
		else
			update_error_info.count = update_error_info.count + 1
		end
		-- 相同报错在2秒内出现超过256次，通知玩家重启游戏
		if update_error_info.count >= 256 and update_error_info.catch_time + 2 >= Macros.Global.NowTime then
			update_error_info.catch_time = 0

			--崩溃提示
			ShowGameDialog(
				CPPGameLib.GetString("common_error"),
				CPPGameLib.GetString("common_script_crash"),
				function()
				 	CPPGameEngine:CloseApp()
				end)
		end
	end
end

--游戏重启
function Restart()
	CPPSdkTools.Logout()
	GlobalNetAdapter:Disconnect(true)
end

--游戏停止
function Stop()
	LogTool.LogInfo("[MainScript::Stop] 脚本结束")

	Startup.DelInstance()
	Runner.DelInstance()
end

--网络连接
function OnNetConnected(result, handle, net_id)
	if GlobalNetAdapter then
		GlobalNetAdapter:OnConnected(result, handle, net_id)
	end
end

--网络消息接收
function OnNetRecv(protocol_no, net_id)
	if GlobalNetAdapter then
		GlobalNetAdapter:OnRecv(protocol_no, net_id)
	end
end

--网络消息接收错误
function OnNetRecvErr(protocol_no, net_id)
	if CoreProtocolMap and CoreProtocolMap[protocol_no] then
		local last_lua_err = LogTool:GetLastLuaError()
		if last_lua_err then
			LogTool:CommitLuaError()
			LogTool:ClearLastLuaError()			

			--崩溃提示			
			ShowGameDialog(
				CPPGameLib.GetString("common_error"), 
				CPPGameLib.GetString("common_script_crash"),
				function()
					CPPGameEngine:CloseApp()
				end)
		end
	end
end

--网络断开
function OnNetDisconnect(net_id)
	if GlobalNetAdapter then
		GlobalNetAdapter:OnDisconnect(net_id)
	end
end

--控制台命令
function OnConsoleCommand(command_str)
	local command_list = CPPGameLib.Split(command_str, '@')
	local command_type = command_list[1]
	if not command_type then
		return
	end

	local param_list
	if command_list[2] then
		param_list = CPPGameLib.Split(command_list[2], '#')
	end

	CPPGameLib.Switch(command_type)
	{		
		["test"] = function()		
		end,
		["startprofile"] = function()
			CPPLuaProfileAgent:Start(CPPLuaProfileAgent.LPT_LATEST_CALLFUNC_MONITOR)
		end,
		["stopprofile"] = function()
			CPPLuaProfileAgent:Stop()
		end,
		["reload"] = function()
			RequireToolBatch.ReloadAllFiles()
		end,
		["onekeywin"] = function()
			CallModuleFunc(ModuleType.BATTLE, "OneKeyWin")
		end,	
	}	
end

--进入前台
function OnEnterForeground()
	LogTool.LogInfo("[MainScript::OnEnterForeground]")

	Macros.Global.NowTime = CPPGameEngine:GetNowTime()
	Macros.Global.NowTimeMilli = CPPGameEngine:GetNowTimeMilli()

	if GlobalStartup then
		GlobalStartup:OnEnterForeground()
	end

	if GlobalNetAdapter then
		GlobalNetAdapter:OnEnterForeground()
	end

	if GlobalGame then
		GlobalGame:OnEnterForeground()
	end
end

--进入后台
function OnEnterBackground()
	LogTool.LogInfo("[MainScript::OnEnterBackground]")
	
	if GlobalStartup then
		GlobalStartup:OnEnterBackground()
	end

	if GlobalNetAdapter then
		GlobalNetAdapter:OnEnterBackground()
	end

	if GlobalGame then
		GlobalGame:OnEnterBackground()
	end
end

--缓存LUA报错
function CacheLuaError(key, error)
	LogTool:CacheLuaError(key, error)

	if StatisticsTool then
		--客户端事件跟踪过程中，上报导致卡死的错误信息
		StatisticsTool.GetInstance():ReportErrorInfoForEventTrack(error)										
	end
end

--播放系统操作音效
function PlaySystemOperEffect(sys_id, tag_id)
	if not sys_id or not tag_id then
		return
	end
	if sys_id == 0 or tag_id == 0 then
		return
	end

	if GlobalGame and GlobalGame:CanEnterSceneChangeState() then
		GlobalSoundMgr:PlaySystemOperEffect(sys_id, tag_id)
	end
end

--游戏消息弹窗
function ShowGameDialog(title, content, callback)
	if LogicGUI and LogicGUI.TemplAffirmTips then
	    LogicGUI.TemplAffirmTips.New({
	    	type = LogicGUI.TemplAffirmTipsType.YES,
	    	title = title, 
	    	content = content,
	        confirm_func = function()
	        	if callback then
	        		callback()
	        	end
	        end,
	       	show_close_btn = false,
	        render_queue = Scene.RenderQueue.GRQ_UI_MAX_II,		       	
	    })						
	end
end

