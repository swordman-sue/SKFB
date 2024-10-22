
--[[
%% @module: 场景
%% @author: swordman sue
%% @created: 2016-07-11
%% @description: 负责创建和管理地图、场景对象....
--]]

Scene = Scene or {}

Scene.VOMgr = Scene.VOMgr or BaseClass(EventAndTimerWrapper)

function Scene.VOMgr.CreateInstance()
	if not GlobalVOMgr then
		GlobalVOMgr = Scene.VOMgr.New()
	end
end

function Scene.VOMgr.DelInstance()
	if GlobalVOMgr then
		GlobalVOMgr:DeleteMe()
		GlobalVOMgr = nil
	end
end

function Scene.VOMgr:__init()
	--怪物列表
	self.__monster_vo_map = {}

	--英雄列表
	self.__hero_vo_map = {}
	self.__hero_lineup_index_to_obj_id_map = {}

	--弹道列表
	self.__bullet_vo_map = {}	

	--掉落宝箱列表
	self.__dropbox_vo_map = {}

	--精灵列表
	self.__spirit_vo_map = {}

	--公共怒气
	self.__anger = 0
	self.__max_anger = ConfigAdapter.Common.GetMaxCommonAnger()
	self.__enemy_anger = 0
	self.__enemy_max_anger = ConfigAdapter.Common.GetMaxCommonAnger()

	--QTE连击次数
	self.__qte_combo_hit_times = 0
end

function Scene.VOMgr:__delete()
	self.__scene_info = nil
	
	self:Dump(true)
end

------------------------------------------------------------------------------------------
------------------------------------------ 场景 ------------------------------------------
------------------------------------------------------------------------------------------
function Scene.VOMgr:SetSceneInfo(type, id)
	self.__scene_info = {}
	self.__scene_info.type = type
	self.__scene_info.id = id

	CPPGameLib.Switch(type)
	{
		--主城
		[Macros.Game.FBType.MAIN_CITY] = function()
			self.__scene_info.map_id = 0
		end,

		[Macros.Game.SceneType.TEAM_WORLD_MAP] = function()
			self.__scene_info.map_id = 0
		end,

		[Default] = function()
			local config_name
			if type < Macros.Game.SceneType.BEG_TAG then
				--关卡
				config_name = "GameLevel"
			else
				--场景地图
				config_name = "Scene"
			end
			local config = CPPGameLib.GetConfig(config_name, id)
			if not config then
				return
			end
			self.__scene_info.map_id = config.map_id
		end
	}
	
	return true	
end

function Scene.VOMgr:GetSceneInfo()
	return self.__scene_info
end

function Scene.VOMgr:GetSceneConfig()
	if self.__scene_info and self.__scene_info.type >= Macros.Game.SceneType.BEG_TAG then
		return CPPGameLib.GetConfig("Scene", self.__scene_info.id)
	end
end

function Scene.VOMgr:GetMapID()
	return self.__scene_info and self.__scene_info.map_id or 0
end

------------------------------------------------------------------------------------------
------------------------------------------ 英雄 ------------------------------------------
------------------------------------------------------------------------------------------
function Scene.VOMgr:AddHeroVO(obj_id, vo)
	self:DeleteHeroVO(obj_id)
	
	self.__hero_vo_map[obj_id] = vo
	if vo.lineup_index and vo.lineup_index ~= -1 then
		self.__hero_lineup_index_to_obj_id_map[vo.lineup_index] = vo.obj_id
	end

	self:FireNextFrame(Macros.Event.SceneObj.HERO_VO_ADD, obj_id, vo)
end

function Scene.VOMgr:DeleteHeroVO(obj_id, cleanup)
	local vo = self.__hero_vo_map[obj_id]
	if vo then
		if cleanup ~= false then
			self.__hero_vo_map[obj_id] = nil
			if vo.lineup_index and vo.lineup_index ~= -1 then
				self.__hero_lineup_index_to_obj_id_map[vo.lineup_index] = nil
			end			

			self:FireNextFrame(Macros.Event.SceneObj.HERO_VO_REMOVE, obj_id, vo)
		end
	end
end

function Scene.VOMgr:ClearAllHeroVO(cleanup)
	for obj_id, vo in pairs(self.__hero_vo_map) do
		self:DeleteHeroVO(obj_id, false)
	end
	self.__hero_vo_map = not cleanup and {} or nil
	self.__hero_lineup_index_to_obj_id_map = not cleanup and {} or nil
end

function Scene.VOMgr:GetHeroMap()
	return self.__hero_vo_map
end

function Scene.VOMgr:GetHeroLineupIdx2ObjIDMap()
	return self.__hero_lineup_index_to_obj_id_map
end

function Scene.VOMgr:GetHeroList(do_sort, check_can_dobeattacked, skill_id)
	local hero
	local hero_list = {}
	for _, vo in pairs(self.__hero_vo_map) do
		if check_can_dobeattacked then
			hero = GlobalScene:GetObj(vo.obj_id)
			if hero and hero:CanDoBeAttacked(skill_id) then
				table.insert(hero_list, vo)
			end
		else
			table.insert(hero_list, vo)
		end
	end	
	if do_sort then
		table.sort(hero_list, function(vo1, vo2)
			if not vo1 then
				return false
			end
			if not vo2 then
				return true
			end
			return vo1.obj_id < vo2.obj_id
		end)
	end
	return hero_list
end

function Scene.VOMgr:GetHeroNum()
	local num = 0
	for _, vo in pairs(self.__hero_vo_map) do
		if vo.hp > 0 then
			num = num + 1
		end
	end	
	return num
end

function Scene.VOMgr:CalcTotalHeroHP()
	local var = 0
	for _, vo in pairs(self.__hero_vo_map) do
		var = var + vo.hp
	end
	return math.floor(var)
end

function Scene.VOMgr:CalcTotalHeroMaxHP()
	local var = 0
	for _, vo in pairs(self.__hero_vo_map) do
		var = var + vo.max_hp
	end
	return math.floor(var)
end

--获取当前英雄的血量与总血量比值
function Scene.VOMgr:CalcHeroCurHPRatio()
	if self:CalcTotalHeroMaxHP() > 0 then
		return (self:CalcTotalHeroHP()/self:CalcTotalHeroMaxHP())*100
	else
		return 0
	end
end

------------------------------------------------------------------------------------------
------------------------------------------ 怪物 ------------------------------------------
------------------------------------------------------------------------------------------
function Scene.VOMgr:AddMonsterVO(obj_id, vo)
	self:DeleteMonsterVO(obj_id)
	self.__monster_vo_map[obj_id] = vo
	self:FireNextFrame(Macros.Event.SceneObj.MONSTER_VO_ADD, obj_id, vo)
end

function Scene.VOMgr:DeleteMonsterVO(obj_id, cleanup)
	local vo = self.__monster_vo_map[obj_id]
	if vo then
		if cleanup ~= false then
			self.__monster_vo_map[obj_id] = nil
		end
		self:FireNextFrame(Macros.Event.SceneObj.MONSTER_VO_REMOVE, obj_id, vo)
	end
end

function Scene.VOMgr:ClearAllMonsterVO(cleanup)
	for obj_id, vo in pairs(self.__monster_vo_map) do
		self:DeleteMonsterVO(obj_id, false)
	end
	self.__monster_vo_map = not cleanup and {} or nil
end

function Scene.VOMgr:GetMonsterMap()
	return self.__monster_vo_map
end

function Scene.VOMgr:GetMonsterList(do_sort, check_can_dobeattacked, skill_id)
	local monster
	local monster_list = {}
	for _, vo in pairs(self.__monster_vo_map) do
		if check_can_dobeattacked then
			monster = GlobalScene:GetObj(vo.obj_id)
			if monster and monster:CanDoBeAttacked(skill_id) then
				table.insert(monster_list, vo)
			end
		else
			table.insert(monster_list, vo)
		end
	end
	if do_sort then
		table.sort(monster_list, function(vo1, vo2)
			if not vo1 then
				return false
			end
			if not vo2 then
				return true
			end
			return vo1.obj_id < vo2.obj_id
		end)
	end
	return monster_list
end

function Scene.VOMgr:GetMonsterNum()
	local num = 0
	for _, vo in pairs(self.__monster_vo_map) do
		if vo.hp > 0 then
			num = num + 1
		end
	end	
	return num
end

function Scene.VOMgr:CalcTotalMonsterHP()
	local var = 0
	for _, vo in pairs(self.__monster_vo_map) do
		var = var + vo.hp
	end
	return math.floor(var)
end

function Scene.VOMgr:CalcTotalMonsterMaxHP()
	local var = 0

	for _, vo in pairs(self.__monster_vo_map) do
		var = var + vo.max_hp
	end
	if self.__extra_monster_max_hp then
		var = var + self.__extra_monster_max_hp
	end

	return math.floor(var)
end

function Scene.VOMgr:CalcExtraMonsterMaxHP(max_hp)
	self.__extra_monster_max_hp = self.__extra_monster_max_hp or 0
	self.__extra_monster_max_hp = self.__extra_monster_max_hp + max_hp
end

function Scene.VOMgr:ClearExtraMonsterMaxHP()
	self.__extra_monster_max_hp = nil
end

--获取当前怪物的血量与总血量比值
function Scene.VOMgr:CalcMonsterCurHPRatio()
	if self:CalcTotalMonsterMaxHP() > 0 then
		return (self:CalcTotalMonsterHP()/self:CalcTotalMonsterMaxHP())*100
	else
		return 0
	end
end

------------------------------------------------------------------------------------------
------------------------------------------ 弹道 ------------------------------------------
------------------------------------------------------------------------------------------
function Scene.VOMgr:AddBulletVO(obj_id, vo)
	self:DeleteBulletVO(obj_id)
	self.__bullet_vo_map[obj_id] = vo
	self:FireNextFrame(Macros.Event.SceneObj.BULLET_VO_ADD, obj_id, vo)
end

function Scene.VOMgr:DeleteBulletVO(obj_id, cleanup)
	local vo = self.__bullet_vo_map[obj_id]
	if vo then
		if cleanup ~= false then
			self.__bullet_vo_map[obj_id] = nil
		end
		self:FireNextFrame(Macros.Event.SceneObj.BULLET_VO_REMOVE, obj_id, vo)
	end
end

function Scene.VOMgr:ClearAllBulletVO(cleanup)
	for obj_id, vo in pairs(self.__bullet_vo_map) do
		self:DeleteMonsterVO(obj_id, false)
	end
	self.__bullet_vo_map = not cleanup and {} or nil
end

function Scene.VOMgr:GetBulletMap()
	return self.__bullet_vo_map
end

------------------------------------------------------------------------------------------
------------------------------------------ 掉落宝箱 ------------------------------------------
------------------------------------------------------------------------------------------
function Scene.VOMgr:AddDropBoxVO(obj_id, vo)
	self:DeleteDropBoxVO(obj_id)
	self.__dropbox_vo_map[obj_id] = vo
end

function Scene.VOMgr:DeleteDropBoxVO(obj_id, cleanup)
	local vo = self.__dropbox_vo_map[obj_id]
	if vo then
		if cleanup ~= false then
			self.__dropbox_vo_map[obj_id] = nil
		end
	end
end

function Scene.VOMgr:ClearAllDropBoxVO(cleanup)
	for obj_id, vo in pairs(self.__dropbox_vo_map) do
		self:DeleteDropBoxVO(obj_id, false)
	end
	self.__dropbox_vo_map = not cleanup and {} or nil
end

function Scene.VOMgr:GetDropBoxMap()
	return self.__dropbox_vo_map
end

--获取世界之心的宝箱ID
function Scene.VOMgr:GetDropBoxIDOfWorldStar()
	for obj_id, vo in pairs(self.__dropbox_vo_map) do
		if vo.drop_info.is_world_star then
			return obj_id
		end
	end
end

------------------------------------------------------------------------------------------
------------------------------------------ 精灵 ------------------------------------------
------------------------------------------------------------------------------------------
function Scene.VOMgr:AddSpiritVO(obj_id, vo)
	self:DeleteSpiritVO(obj_id)
	self.__spirit_vo_map[obj_id] = vo
end

function Scene.VOMgr:DeleteSpiritVO(obj_id, cleanup)
	local vo = self.__spirit_vo_map[obj_id]
	if vo then
		if cleanup ~= false then
			self.__spirit_vo_map[obj_id] = nil
		end
	end
end

function Scene.VOMgr:ClearAllSpiritVO(cleanup)
	for obj_id, vo in pairs(self.__spirit_vo_map) do
		self:DeleteSpiritVO(obj_id, false)
	end
	self.__spirit_vo_map = not cleanup and {} or nil
end

function Scene.VOMgr:GetSpiritMap()
	return self.__spirit_vo_map
end

------------------------------------------------------------------------------------------
---------------------------------------- 公共怒气 ----------------------------------------
------------------------------------------------------------------------------------------
function Scene.VOMgr:GetAnger(is_enemy)
	if not is_enemy then
		return self.__anger 
	else
		return self.__enemy_anger
	end
end

function Scene.VOMgr:GetMaxAnger(is_enemy)
	if not is_enemy then
		return self.__max_anger
	else
		return self.__enemy_max_anger
	end
end

function Scene.VOMgr:SetAnger(anger, is_enemy)
    local change_anger
	local max_anger = self:GetMaxAnger(is_enemy)
    anger = math.max(0, anger)
    anger = math.min(anger, max_anger)

    if not is_enemy then
    	change_anger = anger - self.__anger
	    self.__anger = anger

	else
		change_anger = anger - self.__enemy_anger
		self.__enemy_anger = anger
	end
end

function Scene.VOMgr:ChangeAnger(var, is_enemy)
	local anger = self:GetAnger(is_enemy)
	self:SetAnger(anger + var, is_enemy)
end

--统计初始怒气值相关
function Scene.VOMgr:CalcInitAngerRelevant()
	--初始怒气
	self.__init_anger = self:GetAnger() or 0
	self.__enemy_init_anger = self:GetAnger(true) or 0

	--初始连击
	self.__qte_combo_hit_times = 0
end

--获取初始怒气值
function Scene.VOMgr:GetInitAnger(is_enemy)
	if not is_enemy then
		return self.__init_anger or 100
	end
	return self.__enemy_init_anger or 100
end

------------------------------------------------------------------------------------------
------------------------------------------ 其他 ------------------------------------------
------------------------------------------------------------------------------------------
function Scene.VOMgr:Dump(cleanup)
	self:ClearAllHeroVO(cleanup)
	self:ClearAllMonsterVO(cleanup)
	self:ClearAllBulletVO(cleanup)
	self:ClearAllDropBoxVO(cleanup)
	self:ClearAllSpiritVO(cleanup)
	self:ClearExtraMonsterMaxHP()
end

--增加连击数
function Scene.VOMgr:AddQTEComboHitTimes(combo_hittimes)
	combo_hittimes = combo_hittimes or 1
	self.__qte_combo_hit_times = self.__qte_combo_hit_times + combo_hittimes
	self:FireNextFrame(Macros.Event.BattleModule.BATTLE_QTE_COMBO, self.__qte_combo_hit_times)
end

--获取连击数
function Scene.VOMgr:GetQTEComboHitTimes()
	return self.__qte_combo_hit_times
end

--重置连击数
function Scene.VOMgr:ResetQTEComboHitTimes()
	--重置连击数
	self.__qte_combo_hit_times = 0
	self:FireNextFrame(Macros.Event.BattleModule.BATTLE_QTE_COMBO, self.__qte_combo_hit_times)
end

