
--[[
%% @module: 战斗模块
%% @author: swordman sue
%% @created: 2016-07-15
--]]

Modules = Modules or {}

Modules.BattleModule = Modules.BattleModule or BaseClass(BaseModule)

function Modules.BattleModule:OneKeyWin()
    if CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsPVPDungeon") then
        return
    end

    if CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsChasmDungeon") then
        return
    end
    
    if self.__cur_info then
        self.__cur_info.battle_index = self.__cur_info.battle_num

        CallModuleFunc(ModuleType.STORY, "Stop", true, true)
        CallModuleFunc(ModuleType.LEAD, "Stop")
        GlobalScene:DeleteAllMonsters()
    end
end

function Modules.BattleModule:OneKeyLose()
    if CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsPVPDungeon") then
        return
    end

    if CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsChasmDungeon") then
        return
    end
    
    if self.__cur_info then
        self.__cur_info.battle_index = self.__cur_info.battle_num

        CallModuleFunc(ModuleType.STORY, "Stop", true, true)
        CallModuleFunc(ModuleType.LEAD, "Stop")
        GlobalScene:DeleteAllHeros()
    end
end

--------------------------------------------------------------------------------------------------------------------------------
--事件
--------------------------------------------------------------------------------------------------------------------------------
function Modules.BattleModule:HandleRolePropChange(type, old_value, new_value)
    --VIP等级变化
    if type == Macros.Game.RolePropType.VIP_LEVEL and old_value ~= new_value then
        --更新战斗加速系数
        local open_vip = CallModuleFunc(ModuleType.VIP, "GetToVipOpen", Macros.Game.VIPType.BATTLE_ACC_MAX)
        local open_level = ConfigAdapter.Common.GetBattleAccMaxOpenLev()
        local role_level = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.LEVEL)
        if old_value < open_vip and new_value >= open_vip and role_level >= open_level then
            local acc_list = ConfigAdapter.Common.GetBattleACCList()
            Cookies.Set(Cookies.LevelType.ROLE, Macros.Setting.BattleACCIndex, tostring(#acc_list))
        end
    end
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--参数转换
function Modules.BattleModule:__TransformParams(is_c2s, type, param1, param2, param3, param4)
    return param1, param2, param3, param4
end

--解析战斗开始协议数据
function Modules.BattleModule:__ParseInfo(protocol_data)
    if not self.__cur_info then
        return
    end

    --副本类型
    self.__cur_info.type = protocol_data.type

    --参数转换
    self.__cur_info.param1, self.__cur_info.param2, self.__cur_info.param3, self.__cur_info.param4 = self:__TransformParams(
        false, self.__cur_info.type, protocol_data.param1, protocol_data.param2, protocol_data.param3, protocol_data.param4)    

    self:__ParseNormalDungeonInfo()
    self:__ParseTowerInfo()
    self:__ParseRebelInfo()
    self:__ParseManorDareInfo()
    self:__ParsePlunderedTreasuresInfo()
    self:__ParseArenaInfo()
    self:__ParseWorldBossInfo()
    self:__ParseUnionInfo()
    self:__ParseAttackCityInfo()
    self:__ParseChasmInfo()
    self:__ParseTeamNormalBattleInfo()
    self:__ParseTeamBossBattleInfo()
    self:__ParseLadderInfo()
    self:__ParseDivinationInfo()
    self:__ParseIntegralRaceInfo()
    self:__ParseResourceWarsInfo()
    self:__ParseUnionWarsInfo()
    self:__ParseLeadDungeonInfo()
    self:__ParseResourceWarInfo()

    if not self.__cur_info.gamelevel_id then
        return
    end

    --是否已请求退出
    self.__cur_info.has_req_end = false

    --战役索引
    self.__cur_info.battle_index = 1
    --战役ID
    self.__cur_info.battle_id = ConfigAdapter.GameLevel.GetFirstBattleID(self.__cur_info.gamelevel_id)
    --战役数量
    self.__cur_info.battle_num = ConfigAdapter.GameLevel.GetBattleNum(self.__cur_info.gamelevel_id)

    --战斗前的角色信息
    self.__cur_info.pre_role_info = CPPGameLib.CopyTbl(CallModuleFunc(ModuleType.ROLE, "GetRoleInfo"))

    --奖励列表
    self.__cur_info.reward_currency_map = Modules.BattleFormula:CalcRewardCurrencyMap(self.__cur_info.type, self.__cur_info.gamelevel_id)

    --掉落列表
    self.__cur_info.drop_list = protocol_data.drop_list

    --怪物列表
    self.__cur_info.monster_list = protocol_data.monster_list

    --英雄镜像列表
    self.__cur_info.hero_mirror_list = protocol_data.hero_list

    --目标精灵列表
    self.__cur_info.target_sprite_list = protocol_data.target_sprite_list

    --助战英雄ID
    self.__cur_info.assist_hero_id = protocol_data.help_battle_hero_info_id

    --助战英雄属性列表
    self.__cur_info.help_battle_hero_pro_list = protocol_data.help_battle_hero_pro_list

    --敌方战力
    self.__cur_info.enemy_battle_power = protocol_data.target_fighting_power

    --初始化助战精灵
    self:__InitAssistSpriteInfo()

    --计算战力压制属性加成表
    self:__CalcPowerSuppressionPlusPropMap()
end

--解析新手副本
function Modules.BattleModule:__ParseLeadDungeonInfo()
    if CPPSdkTools.IsInExamine() then
        return
    end

    --进入未通关的新手副本关联副本时，将type和gamelevel_id重定向为新手副本
    local associated_gamelevel_id = ConfigAdapter.Dungeon.GetLeadAssociatedGameLevelID()
    if self.__cur_info.type == Macros.Game.FBType.MAIN_DUNGEON and 
        self.__cur_info.gamelevel_id == associated_gamelevel_id and self.__cur_info.no_passed_flag then
        
        self.__cur_info.type = Macros.Game.FBType.LEAD
        self.__cur_info.gamelevel_id = ConfigAdapter.Dungeon.GetLeadGameLevelID()
    end
end

--解析普通副本
function Modules.BattleModule:__ParseNormalDungeonInfo()
    --缓存未通关标记
    local function record_no_passed_flag()
        local gamelevel_info = CallModuleFunc(ModuleType.DUNGEON, "GetGameLevelInfo", self.__cur_info.chapter_index, self.__cur_info.gamelevel_index, self.__cur_info.type)
        self.__cur_info.no_passed_flag = gamelevel_info and gamelevel_info.no_passed_flag or nil
    end

    --初始化精灵信息
    local function init_sprite_info()
        self.__cur_info.sprite_id = self.__cur_info.param4
        self.__cur_info.sprite_common_skill_id = self.__ex_params and self.__ex_params[1] or 0
    end

    if self.__cur_info.type == Macros.Game.FBType.MAIN_DUNGEON or 
        self.__cur_info.type == Macros.Game.FBType.ELITE_DUNGEON or
        self.__cur_info.type == Macros.Game.FBType.DAILY_DUNGEON then
        --章节索引
        self.__cur_info.chapter_index = self.__cur_info.param1
        --关卡索引
        self.__cur_info.gamelevel_index = self.__cur_info.param2
        --关卡ID      
        self.__cur_info.gamelevel_id = ConfigAdapter.Dungeon.GetGameLevelID(self.__cur_info.chapter_index, self.__cur_info.gamelevel_index, self.__cur_info.type)

        --缓存未通关标记
        record_no_passed_flag()

        if self.__cur_info.type == Macros.Game.FBType.MAIN_DUNGEON then
            --初始化精灵信息
            init_sprite_info()
        end

        --精英副本外域入侵
        if self.__cur_info.type == Macros.Game.FBType.ELITE_DUNGEON and self.__cur_info.gamelevel_index == 0 then
            local chpater_info = CallModuleFunc(ModuleType.DUNGEON, "GetChapterInfo", self.__cur_info.chapter_index, self.__cur_info.type)
            if chpater_info then
                self.__cur_info.gamelevel_id = chpater_info.enemy_map_id
                self.__cur_info.no_passed_flag = chpater_info.is_enemy_dead == Macros.Global.FALSE
            end         
        end

    elseif self.__cur_info.type == Macros.Game.FBType.HERO_DUNGEON then
        --章节索引
        self.__cur_info.chapter_index = self.__cur_info.param1
        --关卡索引
        self.__cur_info.gamelevel_index = self.__cur_info.param2
        --关卡ID
        if self.__cur_info.gamelevel_index > 0 then
            self.__cur_info.gamelevel_id = ConfigAdapter.Dungeon.GetGameLevelID(self.__cur_info.chapter_index, self.__cur_info.gamelevel_index, self.__cur_info.type)       
        else
            local config_chapter = CPPGameLib.GetConfig("HeroFBChapter", self.__cur_info.chapter_index)
            self.__cur_info.gamelevel_id = config_chapter and config_chapter.extra_map_id or nil
        end

        --缓存未通关标记
        record_no_passed_flag()

        --初始化精灵信息
        init_sprite_info()
    end    
end

--解析爬塔副本
function Modules.BattleModule:__ParseTowerInfo()
    --普通爬塔副本
    if self.__cur_info.type == Macros.Game.FBType.TOWER_COMMON then
        --关卡ID
        local tower_game_level = CallModuleFunc(ModuleType.TOWER, "GetBattleMapLevel")
        local config_tower_game_level = CPPGameLib.GetConfig("TowerGameLevel", tower_game_level)
        if not config_tower_game_level then
            return
        end
        self.__cur_info.gamelevel_id = config_tower_game_level.map_list[self.__cur_info.param1 + 1]

    --精英爬塔副本
    elseif self.__cur_info.type == Macros.Game.FBType.TOWER_ELITE then
        --关卡ID
        local config_tower_elite = CPPGameLib.GetConfig("TowerEliteMap", self.__cur_info.param1)
        if not config_tower_elite then
            return
        end
        self.__cur_info.gamelevel_id = config_tower_elite.map_id
    end    
end

--解析叛军副本信息
function Modules.BattleModule:__ParseRebelInfo()
    --叛军副本
    if self.__cur_info.type == Macros.Game.FBType.REBEL then
        --关卡ID
        self.__cur_info.gamelevel_id = self.__cur_info.param4
    end
end

--解析领地副本信息
function Modules.BattleModule:__ParseManorDareInfo()
    if self.__cur_info.type == Macros.Game.FBType.MANOR_DARE then
        local config_land_city = CPPGameLib.GetConfig("LandCity", self.__cur_info.param1)
        if not config_land_city then
            return
        end     
        --关卡ID
        self.__cur_info.gamelevel_id = config_land_city.map_id
    end
end

--解析夺宝副本信息
function Modules.BattleModule:__ParsePlunderedTreasuresInfo()
    if self.__cur_info.type == Macros.Game.FBType.PLUNDERED_TREASURES then
        --打玩家镜像
        if self.__cur_info.param2 == Macros.Global.TRUE then
            --关卡ID
            self.__cur_info.gamelevel_id = ConfigAdapter.Common.GetPlunderedTreasuresGameLevelID()
        --打怪物
        else
            --关卡ID
            self.__cur_info.gamelevel_id = self.__cur_info.param1
        end
    end
end

--解析竞技场信息
function Modules.BattleModule:__ParseArenaInfo()
    if self.__cur_info.type == Macros.Game.FBType.ARENA then
        --关卡ID
        self.__cur_info.gamelevel_id = ConfigAdapter.Common.GetArenaGameLevelID()
    end
end

--解析天梯赛信息
function Modules.BattleModule:__ParseLadderInfo()
    if self.__cur_info.type == Macros.Game.FBType.LADDER then
        --关卡ID
        self.__cur_info.gamelevel_id = ConfigAdapter.Common.GetLadderGameLevelID()
    end    
end

--解析占卜战斗信息
function Modules.BattleModule:__ParseDivinationInfo()
    if self.__cur_info.type == Macros.Game.FBType.DIVINATION then
        --关卡ID
        self.__cur_info.gamelevel_id = self.__cur_info.param1
    end    
end

--解析世界BOSS信息
function Modules.BattleModule:__ParseWorldBossInfo()
    if self.__cur_info.type == Macros.Game.FBType.WORLD_BOSS then
        --关卡ID
        self.__cur_info.gamelevel_id = CallModuleFunc(ModuleType.WORLDBOSS, "GetGameLevelID")
    end
end

--解析公会副本信息
function Modules.BattleModule:__ParseUnionInfo()
    if self.__cur_info.type == Macros.Game.FBType.UNION then
        --关卡ID
        local section = CallModuleFunc(ModuleType.UNION, "GetNowAttackSection") 
        local fbcnf = CPPGameLib.GetConfig("GuildFB", section )
        self.__cur_info.gamelevel_id = fbcnf.gamelevel_list[ self.__cur_info.param1 + 1 ]
        self.__cur_info.camp = self.__cur_info.param1
    end
end

--解析怪物攻城信息
function Modules.BattleModule:__ParseAttackCityInfo()
    if self.__cur_info.type == Macros.Game.FBType.ATTACK_CITY then
        self.__cur_info.gamelevel_id = self.__cur_info.param2       
    end
end

--解析无尽之地信息
function Modules.BattleModule:__ParseChasmInfo()
    if self.__cur_info.type == Macros.Game.FBType.CHASM then
        self.__cur_info.gamelevel_id = ConfigAdapter.Common.GetChasmGameLevelID()
    end
end

--解析积分赛信息
function Modules.BattleModule:__ParseIntegralRaceInfo()
    if self.__cur_info.type == Macros.Game.FBType.INTEGRAL_RACE then
        self.__cur_info.gamelevel_id = ConfigAdapter.Common.GetIntegralRaceGameLevelID()
    end
end

--解析资源战信息
function Modules.BattleModule:__ParseResourceWarsInfo()
    if self.__cur_info.type == Macros.Game.FBType.RESOURCE_WAR then
        local config_resource = CPPGameLib.GetConfig("ResourceWarResource",self.__cur_info.param2,false)
        if config_resource then
            self.__cur_info.gamelevel_id = config_resource.map_id
        end
    end
end

--解析军团战信息
function Modules.BattleModule:__ParseUnionWarsInfo()
    if self.__cur_info.type == Macros.Game.FBType.UNION_LOW then
        local config_union = CPPGameLib.GetConfig("GuildWarStar",self.__cur_info.param1)
        if config_union then
            self.__cur_info.gamelevel_id = config_union.map_id
        end
    elseif self.__cur_info.type == Macros.Game.FBType.UNION_TALL then
        local config_union = CPPGameLib.GetConfig("GuildWarTower",self.__cur_info.param2)
        if config_union then
            self.__cur_info.gamelevel_id = config_union.map_id
        end
    end
end

--解析组队普通战斗信息
function Modules.BattleModule:__ParseTeamNormalBattleInfo()
    if self.__cur_info.type == Macros.Game.FBType.TEAM_NORMAL_BATTLE then
        self.__cur_info.gamelevel_id = self.__cur_info.param2       
    end
end

--解析组队BOSS战斗信息
function Modules.BattleModule:__ParseTeamBossBattleInfo()
    if self.__cur_info.type == Macros.Game.FBType.TEAM_BOSS_BATTLE then
        self.__cur_info.gamelevel_id = self.__cur_info.param2       
    end
end

--解析资源战战斗信息
function Modules.BattleModule:__ParseResourceWarInfo()
    if self.__cur_info.type == Macros.Game.FBType.RESOURCE_WAR then
        local config_resource = CPPGameLib.GetConfig("ResourceWarResource",self.__cur_info.param2 )
        self.__cur_info.gamelevel_id = config_resource.map_id     
    end
end

--生成战役怪物列表
function Modules.BattleModule:__GenBattleMonsterList(for_battle, monster_list, ignore_additional_props)
    local monster_list = monster_list or ConfigAdapter.Battle.GetBattleMonsterList(self.__cur_info)

    --读取服务端返回的怪物数据(已受伤害、血量上限)
    if #self.__cur_info.monster_list > 0 then
        local monster_info
        for _, s_monster_info in ipairs(self.__cur_info.monster_list) do
            monster_info = monster_list[s_monster_info.index + 1]
            if monster_info then
                monster_info.damage = s_monster_info.damage
                monster_info.blood_max = s_monster_info.total_hp
            end
        end
        if for_battle then
            self.__cur_info.monster_list = monster_list
        end
    end

    local add_pro_type_list, add_pro_value_list = self:__GetBattleMonsterPlusPropList()

    --读取怪物属性
    local config_monster, blood_max
    for _, monster_info in ipairs(monster_list) do
        config_monster = CPPGameLib.GetConfig("Monster", monster_info.info_id)

        --读取基础属性
        Modules.BattleFormula:BuildBattleMonsterInfo(nil, monster_info, true)

        --读取血量
        blood_max = monster_info.blood_max
        monster_info.hp = (blood_max and blood_max > 0) and blood_max or config_monster.blood_max

        --若读取服务器返回的血量，则忽略血量相关属性计算
        local filter_map = (blood_max and blood_max > 0) and {
            [Macros.Game.BattleProp.HP] = true, 
            [Macros.Game.BattleProp.HP_RATE] = true, 
            [Macros.Game.BattleProp.HP_RATE_FINAL] = true,
        } or nil

        --读取怪物增幅属性
        if add_pro_type_list and add_pro_value_list then
            local prop_key, prop_value
            for i, add_pro_type in ipairs(add_pro_type_list) do
                if not filter_map or not filter_map[add_pro_type] then
                    prop_key = Macros.Game.BattlePropKey[add_pro_type]
                    monster_info[prop_key] = monster_info[prop_key] or 0
                    monster_info[prop_key] = monster_info[prop_key] + (add_pro_value_list[i] or 0)
                end
            end
        end

        --处理加成类属性
        if not ignore_additional_props then
            Modules.BattleFormula:CalcAdditionalProps(monster_info, filter_map)
        end

        if for_battle then
            --处理战力压制
            Modules.BattleFormula:HandlePlusPropMap(monster_info, self.__cur_info.enemy_combat_suppression_plus_prop_map)

            --处理竞技场血量修正
            if self.__cur_info.type == Macros.Game.FBType.ARENA then
                monster_info.hp = monster_info.hp * ConfigAdapter.Common.GetArenaHPFactor()

            --处理资源战血量修正
            elseif self.__cur_info.type == Macros.Game.FBType.RESOURCE_WAR then
                monster_info.hp = monster_info.hp * ConfigAdapter.Common.GetResourceWarHPFactor()

            --处理积分赛血量修正
            elseif self.__cur_info.type == Macros.Game.FBType.INTEGRAL_RACE then
                monster_info.hp = monster_info.hp * ConfigAdapter.Common.GetIntegralRaceHPFactor()
            end 

        end        
    end

    return monster_list
end

--获取战役怪物增幅属性
function Modules.BattleModule:__GetBattleMonsterPlusPropList()
    local add_pro_type_list, add_pro_value_list

    if self.__cur_info.type == Macros.Game.FBType.REBEL then
        --叛军
        local config_rebel_battle = CPPGameLib.GetConfig("RebelBattle", self.__cur_info.param3)
        if config_rebel_battle then
            add_pro_type_list = config_rebel_battle.add_pro_type_list
            add_pro_value_list = config_rebel_battle.add_pro_value_list
        end

        local rebel_data = CallModuleFunc(ModuleType.REBEL, "GetCanHitRebelFindData")
        local config_rebel_quality = CPPGameLib.GetConfig("RebelQuality", rebel_data.quality)
        if config_rebel_quality then
            add_pro_type_list = CPPGameLib.MergeTable2List(add_pro_type_list, config_rebel_quality.pro_type_list)
            add_pro_value_list = CPPGameLib.MergeTable2List(add_pro_value_list, config_rebel_quality.pro_ratio_list)
        end

    elseif self.__cur_info.type == Macros.Game.FBType.ARENA then
        --竞技场
        local config_arena_robot = CPPGameLib.GetConfig("ArenaRobot", self.__cur_info.param1, false)
        if config_arena_robot then
            add_pro_type_list = config_arena_robot.add_pro_type_list
            add_pro_value_list = config_arena_robot.add_pro_value_list
        end

    elseif self.__cur_info.type == Macros.Game.FBType.PLUNDERED_TREASURES then
        --夺宝
        local config_duobao_map = CPPGameLib.GetConfig("DuobaoMap", CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL), false)
        if config_duobao_map then
            add_pro_type_list = config_duobao_map.add_pro_type_list
            add_pro_value_list = config_duobao_map.add_pro_value_list
        end

    elseif self.__cur_info.type == Macros.Game.FBType.ATTACK_CITY then
        --怪物攻城
        local cur_chapter = CallModuleFunc(ModuleType.MONSTER_STORMCASTLE, "GetCurChapter")
        local config_attack_city_monster = CPPGameLib.GetConfig("AttackCityMonster", cur_chapter, false)
        if config_attack_city_monster then
            add_pro_type_list = config_attack_city_monster.add_pro_type_list
            add_pro_value_list = config_attack_city_monster.add_pro_value_list
        end
    end

    return add_pro_type_list, add_pro_value_list
end

--初始化助战精灵
function Modules.BattleModule:__InitAssistSpriteInfo()
    local config_battle_sprite = CPPGameLib.GetConfig("BattleSpriteToggle", self.__cur_info.type, false)
    if not config_battle_sprite or config_battle_sprite.trigger_type ~= Macros.Global.TRUE then
        return 
    end

    local sprite_data = CallModuleFunc(ModuleType.SPIRIT, "GetSpiritData")
    if sprite_data then
        local config_sprite = CPPGameLib.GetConfig("Spirit", sprite_data.cur_use_spirit, false)
        if config_sprite then
            self.__cur_info.sprite_id = sprite_data.cur_use_spirit

            local common_skill_id = tonumber(Cookies.Get(Cookies.LevelType.ROLE, Macros.Setting.BattleSpriteCommonSkillID)) or 0
            self.__cur_info.sprite_common_skill_id = common_skill_id ~= 0 and common_skill_id or config_sprite.common_skill_id
        end
    end
end

--计算战力压制属性加成表
function Modules.BattleModule:__CalcPowerSuppressionPlusPropMap()
    --对战怪物(统一由客户端计算战力)
    local monster_list = self:GetBattleMonsterList(true, true)
    if monster_list then
        --计算我方战力
        self.__cur_info.my_battle_power = self:GetBattleHeroPower(true)

        --计算敌方战力        
        self.__cur_info.enemy_battle_power = 0
        for _, monster_info in pairs(monster_list) do
            self.__cur_info.enemy_battle_power = self.__cur_info.enemy_battle_power + CallModuleFunc(ModuleType.POWER, "CalcMonsterPowerEx", monster_info)
        end

    --对战英雄镜像(统一由服务端计算战力)
    else
        --读取我方战力
        self.__cur_info.my_battle_power = CallModuleFunc(ModuleType.ROLE,"GetPropValue", Macros.Game.RolePropType.BATTLE_VALUE)
    end

    --计算战力压制加成
    self.__cur_info.my_combat_suppression_plus_prop_map, 
    self.__cur_info.enemy_combat_suppression_plus_prop_map = Modules.BattleFormula:CalcCombatSuppressionPlusPropMap(self.__cur_info.type, self.__cur_info.my_battle_power, self.__cur_info.enemy_battle_power)  

    -- CPPGameLib.PrintTable(StartConfig.PrintTag, self.__cur_info.my_combat_suppression_plus_prop_map, "我方战力压制属性加成")
    -- CPPGameLib.PrintTable(StartConfig.PrintTag, self.__cur_info.enemy_combat_suppression_plus_prop_map, "敌方战力压制属性加成")
end

--解析奖励列表
function Modules.BattleModule:__ParseRewardList( data )
    local t1 = {}
    --爬塔精英
    if self.__cur_info.type == Macros.Game.FBType.TOWER_ELITE then
        local cnf = CPPGameLib.GetConfig("TowerEliteMap", self.__cur_info.param1 )
        --拿到关卡表的奖励
        local award = CPPGameLib.GetConfig("GameLevel", cnf.map_id)
        table.insert( t1 , {id = award.reward_item_id_list[1] , num = award.reward_item_num_min_list[1] } )

    --日常副本
    elseif self.__cur_info.type == Macros.Game.FBType.DAILY_DUNGEON then
        local cnf_1 = CPPGameLib.GetConfig("DailyFBReward", self.__cur_info.param1, false)
        if not cnf_1 then
            return data.item_list
        end

        local cnf_2 = {}
        if cnf_1 then
            cnf_2 = cnf_1[self.__cur_info.param2]
        end

        local cnf_3 = {}
        if cnf_2 then
            cnf_3 = cnf_2[ self.__cur_info.context1 ]
        end
        
        if cnf_3 then
            if cnf_3.item_id_list and #cnf_3.item_id_list ~= 0 then
                for i,v in ipairs(cnf_3.item_id_list) do
                    local type_ , info_ = ObjIDTool.GetInfo( v )
                    if info_.sub_type == Macros.Game.NormalItemSubType.CURRENCY then
                        table.insert( t1 , {id = v , num = cnf_3.item_num_list[i] , type = info_.param1 } ) 
                    else
                        table.insert( t1 , {id = v , num = cnf_3.item_num_list[i] } )   
                    end
                end
            end
        end

        local t2 = {}
        local t4 = {}
        for i,v in ipairs(t1) do
            local type , info_ = ObjIDTool.GetInfo( v.id )
            if v.type then
                if t4[v.type] then
                    t4[v.type].num = t4[v.type].num + (v.num*info_.param2)
                else
                    t4[v.type] = {}
                    t4[v.type].num = v.num*info_.param2
                end

                local config = CPPGameLib.GetConfig("HuoBi", v.type)
                t4[v.type].id = config.big_icon

                table.insert( t2 , { id = t4[v.type].id , num = t4[v.type].num } )
            else
                table.insert( t2 , v )
            end
        end

        local t3 = {}
        for i,v in pairs(t2) do
            table.insert( t3 , { id = v.id , num = v.num }  )
        end
        t1 = t3

    else
        t1 = data.item_list
    end

    return t1
end

--首胜奖励
function Modules.BattleModule:__FirstWinAwardConvert( data )
    --如果类型是
    if data.type == Macros.Game.FBType.TOWER_ELITE then
        local cnf = CPPGameLib.GetConfig("TowerEliteMap", data.param1 )
        if data.is_first_win ~= 0 then
            GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.DUNGEON_FIRSTWIN, false, cnf.map_id)
        end
    end
end

