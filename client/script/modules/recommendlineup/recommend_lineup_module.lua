--[[
%% @module: 推荐阵容模块
%% @author: ldx
%% @created: 2017-3-23
%% @description: 推荐阵容模块相关
--]]

Modules = Modules or {}
Modules.RecommendLineupModule = Modules.RecommendLineupModule or BaseClass(BaseModule)

function Modules.RecommendLineupModule:__init()
 	-- body
end 

function Modules.RecommendLineupModule:__delete()
 	-- body
end 

--排序现在的开放列表
function Modules.RecommendLineupModule:SortSystemForeshowList()
	self.__system_foreshow_list = {}

 	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
 	local system_count = 0
 	for i,system_info in ipairs(CPPGameLib.GetConfig("HeroSystemForeshow",nil,nil,true)) do
 		system_info.role_level = role_level 		
 		system_info.is_levelup_visible = false
 		if system_info.open_level > role_level then			
 			system_count = system_count + 1
 			system_info.is_levelup_visible = system_count == 1 and true or false
 			table.insert(self.__system_foreshow_list,system_count,system_info)
 		else
 			table.insert(self.__system_foreshow_list,system_info)
 		end
 	end

 	return self.__system_foreshow_list, system_count == 0
end 