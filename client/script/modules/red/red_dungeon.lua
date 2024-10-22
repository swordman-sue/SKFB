
--[[
%% @module: 副本红点
%% @author: ldx
%% @created: 2019年1月04日
--]]
Modules.Red.Dungeon = Modules.Red.Dungeon or BaseClass( Modules.Red.BaseRed )

function Modules.Red.Dungeon:__init()
end

--红点统计逻辑
function Modules.Red.Dungeon:__Statistics(sub_system_id)    
    --日常副本
	if self.__system_id == Macros.Game.SystemID.DUNGEON_DAILY then
	    self.red = 0

		local lst = CallModuleFunc(ModuleType.DUNGEON, "GetAllDailyChapterType")		
		for i, v in ipairs(lst) do
			if v == "ok" then
				self.red = self.red + 1
				if not Modules.Red.BaseRed.IS_SHOW_NUM then
					break
				end
			end
		end

	--英雄降临
	elseif self.__system_id == Macros.Game.SystemID.HERO_ADVENT then
	    self.red = 0

		local dungeonbox_reward_level = CallModuleFunc(ModuleType.DUNGEON, "GetFBPassedBoxRewardLevel")	
		if dungeonbox_reward_level < #CPPGameLib.GetConfig("MainFBChapterReward",nil,nil,true) then
			local reward_info = CPPGameLib.GetConfig("MainFBChapterReward",dungeonbox_reward_level + 1)
  			--当前通关章节数
			local passed_chapter_num = CallModuleFunc(ModuleType.DUNGEON, "GetPassedAllChapterNum")
			if passed_chapter_num >= reward_info.need_chapter then
				self.red = self.red + 1
			end
		end

	--副本(主线副本、英雄副本、精英副本)
	elseif self.__system_id == Macros.Game.SystemID.DUNGEON then
	    self.red = nil    
	    self.red_map = self.red_map or {}

		local dungeon_red_dot_map = CallModuleFunc(ModuleType.DUNGEON, "GetDungeonRedDotMap")
		if dungeon_red_dot_map then
			if sub_system_id == Macros.Game.SystemID.MAX then
				for sub_system_id_ex, _ in pairs(dungeon_red_dot_map) do
					self:__StatisticsDungeonSubSystem(sub_system_id_ex)
				end
			else
				self:__StatisticsDungeonSubSystem(sub_system_id)
			end
		end
	end
end

--统计副本子系统红点
function Modules.Red.Dungeon:__StatisticsDungeonSubSystem(sub_system_id)
	local red_num = 0
	local dungeon_red_dot_map = CallModuleFunc(ModuleType.DUNGEON, "GetDungeonRedDotMap")

	--章节红点
	local chapter_reddot_list = dungeon_red_dot_map[sub_system_id]
	if chapter_reddot_list then
		for _, chapter_reddot_num in ipairs(chapter_reddot_list) do
			red_num = red_num + chapter_reddot_num
			if red_num > 0 and not Modules.Red.BaseRed.IS_SHOW_NUM then
				break
			end
		end
	end
	
	--今日可挑战次数
	if red_num <= 0 and 
		sub_system_id == Macros.Game.SystemID.HERO_DUNGEON_GAMELEVEL and 
		GlobalModulesMgr:IsOpen(Macros.Game.SystemID.HERO_DUNGEON_GAMELEVEL) then
		
		local extra_data = CallModuleFunc(ModuleType.DUNGEON, "GetExtraInfo", Macros.Game.FBType.HERO_DUNGEON)
		local max_times = CallModuleFunc(ModuleType.VIP, "GetPrivilege", Macros.Game.VIPType.HERO_TRANSCRIPT)
		local remain_times = math.max(0, max_times - extra_data.battle_times)
		red_num = red_num + remain_times
	end		

	self.red_map[sub_system_id] = red_num
end    
