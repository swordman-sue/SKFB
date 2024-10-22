
--[[
%% @module: 世界之心红点
%% @author: ldx
%% @created: 2019年1月05日
--]]
Modules.Red.WorldHeart = Modules.Red.WorldHeart or BaseClass( Modules.Red.BaseRed )

function Modules.Red.WorldHeart:__init( ... )
end

--红点统计逻辑
function Modules.Red.WorldHeart:__Statistics()
	local worldheart_data = CallModuleFunc(ModuleType.WORLD_HEART, "GetWorldHeartData")

	local elves_info = CPPGameLib.GetConfig("HeartOfWorld",worldheart_data.__stage)
	if worldheart_data.org_level >= #elves_info.elves_list and worldheart_data.org_stage >= #CPPGameLib.GetConfig("HeartOfWorld",nil,nil,true) then
		self.red = 0
	else
		local info = CPPGameLib.GetConfig("HeartOfWorld",worldheart_data.__stage)
		local num_1 = CallModuleFunc(ModuleType.Pack,"GetNumByItemID",info.elves_list[worldheart_data.__level].cost_item[1])
		local num_2 = info.elves_list[worldheart_data.__level].cost_item[2]
		self.red = num_1 >= num_2 and 1 or 0
	end	
end