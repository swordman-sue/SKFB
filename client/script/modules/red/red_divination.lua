
--[[
%% @module: 占卜红点
%% @author: yjg
%% @created: 2018年12月17日17:10:10
--]]
Modules.Red.Divination = Modules.Red.Divination or BaseClass( Modules.Red.BaseRed )

function Modules.Red.Divination:__init( ... )
	self.red = nil
	self.red_map = {}
end

--红点统计逻辑
function Modules.Red.Divination:__Statistics( sub_system_id )
	--成就
	if sub_system_id == Macros.Game.SystemID.MAX or sub_system_id == Modules.RedMgr.SystemID.DIVINATION_ACHIEVEMENT then 
		self.red_map[Modules.RedMgr.SystemID.DIVINATION_ACHIEVEMENT] = self:GetAchievementRed()
	end

	--收藏书
	if sub_system_id == Macros.Game.SystemID.MAX or sub_system_id == Modules.RedMgr.SystemID.DIVINATION_BOOK then
		self.red_map[Modules.RedMgr.SystemID.DIVINATION_BOOK] = self:GetRookRed()
	end
end

--有没有可以升星或者收藏的书
function Modules.Red.Divination:GetRookRed( )
	local cnf = CallModuleFunc(ModuleType.DIVINATION , "GetCollectionBookCnf" )
	local red_num  = 0
	for i , v in ipairs( cnf ) do
		for ii , vv in ipairs( v.collection_group_list ) do
			local type_ = CallModuleFunc(ModuleType.DIVINATION , "ToGroupIdGetUpStarType" , vv )
			if type_ then
				red_num = red_num  + 1
				if not Modules.Red.BaseRed.IS_SHOW_NUM then
					break
				end
			end
		end
		if red_num > 0 and not Modules.Red.BaseRed.IS_SHOW_NUM then 
			break
		end
	end
	return red_num 
end

--有没有可以激活的成就
function Modules.Red.Divination:GetAchievementRed( )
	local cnf = CallModuleFunc(ModuleType.DIVINATION , "GetAchievementCnf" )
	local level = CallModuleFunc(ModuleType.DIVINATION , "GetNowOpenAchievement" )
	local max = CallModuleFunc(ModuleType.DIVINATION , "GetHistoryCollectionValue" )
	local red_num  = 0
	for i,v in ipairs( cnf ) do
		if level < v.level and v.need_collection_value <= max then
			red_num = red_num + 1
			if not Modules.Red.BaseRed.IS_SHOW_NUM then
				break
			end
		end
	end
	return red_num 
end