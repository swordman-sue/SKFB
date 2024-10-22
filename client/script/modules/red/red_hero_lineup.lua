
--[[
%% @module: 英雄阵容红点
%% @author: swordman sue
--]]
Modules.Red.HeroLineup = Modules.Red.HeroLineup or BaseClass(Modules.Red.BaseRed)

function Modules.Red.HeroLineup:__init()
	self.red = nil
	self.red_map = {}
end

function Modules.Red.HeroLineup:DeleteMe()
end

--红点统计逻辑
function Modules.Red.HeroLineup:__Statistics(sub_system_id)
	local red_dot_info_key = Macros.Game.SystemID.HERO_LINEUP
	local red_dot_info_key2

	--英雄阵位
	if sub_system_id == Macros.Game.SystemID.HERO_LINEUP then
		red_dot_info_key2 = 1
		CallModuleFunc(ModuleType.HERO, "CountHeroRedDot")

	--援军阵位
	elseif sub_system_id == Macros.Game.SystemID.REINFORCEMENT then
		red_dot_info_key = Macros.Game.SystemID.REINFORCEMENT
		CallModuleFunc(ModuleType.HERO, "CountReinforcementsRedDot")

	--装备、圣器穿戴
	elseif sub_system_id == Macros.Game.SystemID.EQUIP_DRESS then
		red_dot_info_key2 = 2
		CallModuleFunc(ModuleType.HERO, "CountEquipRedDot")

	--卡牌穿戴
	elseif sub_system_id == Macros.Game.SystemID.DIVINATION_DRESS then
		red_dot_info_key2 = 3
		CallModuleFunc(ModuleType.HERO, "CountDivinationRedDot")
	
	--阵容总入口
	elseif sub_system_id == Macros.Game.SystemID.MAX then
		CallModuleFunc(ModuleType.HERO, "CountHeroRedDot")
		CallModuleFunc(ModuleType.HERO, "CountReinforcementsRedDot")
		CallModuleFunc(ModuleType.HERO, "CountEquipRedDot")
		CallModuleFunc(ModuleType.HERO, "CountDivinationRedDot")

		--记录红点信息
		self:__StatisticsSubSystem(Macros.Game.SystemID.HERO_LINEUP, Macros.Game.SystemID.HERO_LINEUP, 1)
		self:__StatisticsSubSystem(Macros.Game.SystemID.EQUIP_DRESS, Macros.Game.SystemID.HERO_LINEUP, 2)
		self:__StatisticsSubSystem(Macros.Game.SystemID.DIVINATION_DRESS, Macros.Game.SystemID.HERO_LINEUP, 3)
		self:__StatisticsSubSystem(Macros.Game.SystemID.REINFORCEMENT, Macros.Game.SystemID.REINFORCEMENT)
		return

	else
		return
	end

	--记录红点信息
	self:__StatisticsSubSystem(sub_system_id, red_dot_info_key, red_dot_info_key2)
end

--统计阵容子系统红点
function Modules.Red.HeroLineup:__StatisticsSubSystem(sub_system_id, red_dot_info_key, red_dot_info_key2)
	local red_dot_info_list = CallModuleFunc(ModuleType.HERO, "GetRedDotInfoMap")
	local red_dot_info = red_dot_info_list[red_dot_info_key]
	if not red_dot_info then
		return
	end

	if type(red_dot_info) == "table" then
		--援军阵位
		if sub_system_id == Macros.Game.SystemID.REINFORCEMENT then
			local red_num = 0
			for i, var in pairs(red_dot_info) do
				red_num = red_num + var
				if red_num > 0 and not Modules.Red.BaseRed.IS_SHOW_NUM then
					break
				end
			end
			self.red_map[sub_system_id] = red_num

		--英雄阵位、装备穿戴、圣器穿戴、卡牌穿戴
		else
			local red_num = 0
			for lineup_index, lineup_red_dot_map in pairs(red_dot_info) do
				local red_dot_info2 = lineup_red_dot_map[red_dot_info_key2]
				if type(red_dot_info2) == "number" then
					red_num = red_num + tonumber(red_dot_info2)
					if red_num > 0 and not Modules.Red.BaseRed.IS_SHOW_NUM then
						break
					end

				elseif type(red_dot_info2) == "table" then
					for _, var in pairs(red_dot_info2) do
						red_num = red_num + var
						if red_num > 0 and not Modules.Red.BaseRed.IS_SHOW_NUM then
							break
						end
					end
				end
				if red_num > 0 and not Modules.Red.BaseRed.IS_SHOW_NUM then
					break
				end
			end
			self.red_map[sub_system_id] = red_num
		end

	elseif type(red_dot_info) == "number" then
		self.red_map[sub_system_id] = red_dot_info
	end
end
