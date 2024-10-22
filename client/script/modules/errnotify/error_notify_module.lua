
--[[
%% @module: 错误通知模块
%% @author: swordman sue
%% @created: 2016-07-28
--]]

Modules = Modules or {}

Modules.ErrorNotifyModule = Modules.ErrorNotifyModule or BaseClass(BaseModule)

function Modules.ErrorNotifyModule:__init()
end

function Modules.ErrorNotifyModule:__delete()
end

function Modules.ErrorNotifyModule:GameErrorNotify(protocol_data)	
	local config_error_code = CPPGameLib.GetConfig("ErrorCode", protocol_data.error_code)
    if config_error_code and config_error_code.err_tips then
        if protocol_data.error_code == 2 then
            if StartConfig.IsTest then
        	   GlobalTipMsg.GetInstance():Show(string.format("%s(%d)", config_error_code.err_tips, protocol_data.from_msg_id))
            end
        else
           GlobalTipMsg.GetInstance():Show(config_error_code.err_tips)
        end
    end

	--重置协议loading
	GlobalNetAdapter:ResetNetLoading()
	
	--派发事件
	GlobalEventSystem:FireNextFrame(Macros.Event.ErrorNotifyModule.ERROR_NOTIFY, protocol_data.error_code)
end
