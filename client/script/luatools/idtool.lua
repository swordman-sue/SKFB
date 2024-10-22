
ObjIDTool = ObjIDTool or {}

--根据配置ID获取对象类型和对象配置
function ObjIDTool.GetInfo(id, show_error)
	if not ObjIDTool.ConfigNameMap then
		ObjIDTool.ConfigNameMap = 
		{
			[Macros.Game.ObjType.CURRENCY] = "HuoBi",
			[Macros.Game.ObjType.ITEM] = "Item",
			[Macros.Game.ObjType.EQUIP] = "Equipment",
			[Macros.Game.ObjType.HERO] = "Hero",
			[Macros.Game.ObjType.TREASURE] = "Treasure",
			[Macros.Game.ObjType.SPIRIT] = "Spirit",
			[Macros.Game.ObjType.AWAKEN_TOOL] = "AwakeningItem",
			[Macros.Game.ObjType.GEM] = "Gem",
			[Macros.Game.ObjType.FAIRY] = "Card",
		}
	end

	if not id or id <= 0 then
		return
	end

	local type = math.floor(id * 0.0001)
	local config_info
	local config_name = ObjIDTool.ConfigNameMap[type]
	if config_name then
		config_info = CPPGameLib.GetConfig(config_name, id, show_error)
	end
	return type, config_info
end

--根据配置ID获取对象拥有数量
function ObjIDTool.GetOwnNum(id, filter_func)
	if not ObjIDTool.GetOwnNumFuncMap then
		ObjIDTool.GetOwnNumFuncMap = 
		{
			[Macros.Game.ObjType.CURRENCY] = function(var)
				local type_ , info_ = ObjIDTool.GetInfo( var )
				local config_currency = {}
				if type_ == Macros.Game.ObjType.CURRENCY then
					config_currency.type = var
				else
					config_currency = CPPGameLib.GetConfig(var)
				end

				return config_currency and CallModuleFunc(ModuleType.ROLE, "GetPropValue", config_currency.type) or 0
			end,
			[Macros.Game.ObjType.ITEM] = function(var, filter)
				return CallModuleFunc(ModuleType.Pack, "GetNumByItemID", var)
			end,
			[Macros.Game.ObjType.EQUIP] = function(var, filter)
				return CallModuleFunc(ModuleType.EQUIP, "GetNumByInfoID", var, filter)			
			end,
			[Macros.Game.ObjType.HERO] = function(var, filter)
				return CallModuleFunc(ModuleType.HERO, "GetNumByInfoID", var, filter)			
			end,
			[Macros.Game.ObjType.TREASURE] = function(var, filter)
				return CallModuleFunc(ModuleType.TREASURE, "GetNumByInfoID", var, filter)			
			end,
			[Macros.Game.ObjType.SPIRIT] = function(var, filter)
				return CallModuleFunc(ModuleType.SPIRIT, "GetNumByInfoID", var, filter)			
			end,
			[Macros.Game.ObjType.AWAKEN_TOOL] = function(var, filter)
				return CallModuleFunc(ModuleType.Pack, "GetNumByItemID", var, filter)			
			end,
			[Macros.Game.ObjType.FAIRY] = function(var, filter)
				return CallModuleFunc(ModuleType.DIVINATION, "ToInfoIdGetDivinationNum", var, filter)			
			end,
		}	
	end

	local type, _ = ObjIDTool.GetInfo(id)
	if ObjIDTool.GetOwnNumFuncMap[type] then
		return ObjIDTool.GetOwnNumFuncMap[type](id, filter_func)
	end
	return 0
end

--根据配置ID获取竞技场机器人数据
function ObjIDTool.GetArenaRobotData(id)
	local isRotbot = false
	local config_info = {}
	if id > 100000000 then
		isRotbot = true
		local robotData = {}
		robotData = CPPGameLib.GetConfig("ArenaRobot", id)
		config_info.name = robotData.name
		robotData = CPPGameLib.GetConfig("Monster", robotData.monster_list[1])
		config_info.id = robotData.hero_id
	end	
	return isRotbot,config_info
end

--根据关卡ID获取战役代表形象数据
function ObjIDTool.GetBattleRepresentativeMonsterConfig(id)
	local battle_Config = CPPGameLib.GetConfig("Battle",id)
	return CPPGameLib.GetConfig("Monster", battle_Config.representativeId)
end

--获取战斗对象ID
function ObjIDTool.GetBattleInfoID(attacker_obj_id, show_error)
	local attacker_obj = GlobalScene:GetObj(attacker_obj_id)
	if not attacker_obj then
		return 0
	end
	local attacker_vo = attacker_obj:GetVO()

	if attacker_obj:IsHeroRelevant() then
		return attacker_vo.info_id
	end

	local config_monster = CPPGameLib.GetConfig("Monster", attacker_vo.info_id, show_error)
	if config_monster then
		return config_monster.hero_id or attacker_vo.info_id
	end	
	return 0
end

--获取怪物对应的英雄ID
function ObjIDTool.GetHeroIDFromMonster(info_id, show_error)
	local config_monster = CPPGameLib.GetConfig("Monster", info_id, show_error)
	if config_monster then
		return config_monster.hero_id or 0
	end
	return info_id
end