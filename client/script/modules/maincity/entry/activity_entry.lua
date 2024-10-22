--[[
%% @module: 扩展背包入口
%% @author: yjg
%% @created: 2019年1月5日15:46:45
--]]

Modules = Modules or {}

Modules.ActivityEntry = Modules.ActivityEntry or BaseClass(Modules.BaseMainCityEntry)

function Modules.ActivityEntry:__init()
end

function Modules.ActivityEntry:__HandleEntryTouch()
	--检查等级开放
	local is_open = GlobalModulesMgr:IsOpen(self.__entry_info.system_id, true)  
	if not is_open then
		return
	end
		
	--超值折扣
	if self.__system_id == Macros.Game.SystemID.CUSTOM_PURCHASE then
		local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.CUSTOM_PURCHASE)
	    CallModuleFunc(ModuleType.CUSTOM_PURCHASE, "OpenView", BaseViewType.CUSTOM_PURCHASE, info.subtype )
	end
end
