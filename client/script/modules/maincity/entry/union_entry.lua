--[[
%% @module: 公会入口
%% @author: yjg
%% @created: 2019年1月5日15:55:52
--]]

Modules = Modules or {}

Modules.UnionEntry = Modules.UnionEntry or BaseClass(Modules.BaseMainCityEntry)

function Modules.UnionEntry:__init()
end

function Modules.UnionEntry:__HandleEntryTouch()
	--检查等级开放
	local is_open = GlobalModulesMgr:IsOpen(self.__entry_info.system_id, true)  
	if not is_open then
		return
	end
		
	--军团
	local role_info = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo")
	if role_info.guild_id == 0 then
		CallModuleFunc(ModuleType.UNION , "GuildDateReq" , "union" )
	else
		Modules.BaseMainCityEntry.__HandleEntryTouch(self)
	end		
end
