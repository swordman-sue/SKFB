
--[[
%% @module: 战斗模块
%% @author: swordman sue
%% @created: 2016-07-15
--]]

Modules = Modules or {}

Modules.BattleModule = Modules.BattleModule or BaseClass(BaseModule)

Modules.BattleModule.MaxTeammateNum = 5

local function generate_win(is_win)
    if not is_win then
        return Macros.Global.FALSE
    end
    return MathTool.GetRandom(16978, 78786969)
end

--ass_hole + role_id + "HelloWorld"
local function generate_start_fuck(role_id, ass_hole)
    return CPPGameLib.Md5(string.format("%d%dHelloWorld", ass_hole, role_id))
end

--is_win + role_id + ass_hole + "HelloWorld"
local function generate_end_fuck(is_win, role_id, ass_hole, total_damage)
    if is_win == Macros.Global.FALSE then
        return ""
    end
    return CPPGameLib.Md5(string.format("%d%d%d%dHelloWorld", is_win, role_id, ass_hole, total_damage))
end

function Modules.BattleModule:__init()
end

function Modules.BattleModule:__delete()
end

--------------------------------------------------------------------------------------------------------------------------------
--协议
--------------------------------------------------------------------------------------------------------------------------------
--请求开始战斗
function Modules.BattleModule:BattleStartReq(type, ...)
	type = type or Macros.Game.FBType.MAIN_DUNGEON
	param1 = select(1, ...) or 0
	param2 = select(2, ...) or 0
	param3 = select(3, ...) or 0
	param4 = select(4, ...) or 0

	local arg_list = {...}

	--主线副本战斗
	if type == Macros.Game.FBType.MAIN_DUNGEON then
		CallModuleFunc(ModuleType.DUNGEON,"SetIsMainDungeonFight",true)
	else
		CallModuleFunc(ModuleType.DUNGEON,"SetIsMainDungeonFight",false)
	end
	--参数转换
	param1, param2, param3, param4 = self:__TransformParams(true, type, param1, param2, param3, param4)

    local ass_hole = CallModuleFunc(ModuleType.BATTLE, "GetBattleHeroPower")
    local fuck = generate_start_fuck(CallModuleFunc(ModuleType.ROLE, "GetRoleID"), ass_hole)

	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_BATTLE_START_REQ)
	protocol.type = type
	protocol.param1 = param1
	protocol.param2 = param2
	protocol.param3 = param3
	protocol.param4 = param4
    protocol.ass_hole = ass_hole
    protocol.fuck = fuck
	GlobalNetAdapter:OnSend(protocol)

	--额外参数
	self.__ex_params = #arg_list > 4 and {} or nil
	if self.__ex_params then
		for i = 5, #arg_list do
			table.insert(self.__ex_params, arg_list[i])
		end
	end

	LogTool.LogInfo("[Modules.BattleModule:BattleStartReq] 请求开始战斗 type:%d, param1:%d, param2:%d, param3:%d, param4:%d", type, param1, param2, param3, param4)

	return true
end

--开始战斗反馈
function Modules.BattleModule:BattleStartResp(protocol_data)
	LogTool.LogInfo("[Modules.BattleModule:BattleStartResp] 开始战斗反馈 type:%d, param1:%d, param2:%d, param3:%d, param4:%d", protocol_data.type, protocol_data.param1, protocol_data.param2, protocol_data.param3, protocol_data.param4)

	self.__cur_info = self.__cur_info or {}

	--解析信息
	self:__ParseInfo(protocol_data)

	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", self.__cur_info.gamelevel_id)

	--自动战斗
	if config_gamelevel and config_gamelevel.show_auto_fight_btn ~= Macros.Global.FALSE then
		local auto_info = Cookies.Get(Cookies.LevelType.ROLE, Macros.Setting.BattleAuto)
		auto_info = (auto_info and auto_info ~= "") and tonumber(auto_info) or 0
		self.__cur_info.is_auto_fight = auto_info == 1 and true or false
	end

	--快速挑战
	self.__cur_info.in_quick_battle = self.__to_quick_battle
	self.__to_quick_battle = false
	
	--重置公共怒气
    GlobalVOMgr:SetAnger(0)
   	GlobalVOMgr:SetAnger(0, true)        

	--进入副本场景
	if not GlobalScene:ChangeScene(self.__cur_info.type, self.__cur_info.gamelevel_id) then
		return
	end

	--加速档位
	if config_gamelevel and config_gamelevel.show_acc_btn ~= Macros.Global.FALSE then
		local acc_info = Cookies.Get(Cookies.LevelType.ROLE, Macros.Setting.BattleACCIndex)
		self.__cur_info.acc_index = (acc_info and acc_info ~= "") and tonumber(acc_info) or 1
		local acc_factor = self:GetACCFactor()
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "SetDeltaSpeedTime", acc_factor)
	end
end

--请求战斗结束
function Modules.BattleModule:BattleEndReq(is_win, star, is_manual_exit, context1, context2)
	if not self.__cur_info or self.__cur_info.has_req_end then
		return
	end
    self.__cur_info.has_req_end = true

    is_win = generate_win(is_win)
    star = star or 0
    context1 = context1 or 0
    context2 = context2 or 0
    is_pass = is_pass or 0
    local total_damage = self:GetRealTotalDamage()
    local ass_hole = is_win > 0 and CallModuleFunc(ModuleType.BATTLE, "GetBattleHeroPower") or 0
    local fuck = is_win > 0 and generate_end_fuck(is_win, CallModuleFunc(ModuleType.ROLE, "GetRoleID"), ass_hole, total_damage) or ""

    self:HandleBattleEndReq(is_manual_exit, context1, context2)

	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_BATTLE_END_REQ)
	protocol.type = self.__cur_info.type
	protocol.is_win = is_win
	protocol.star = star
	protocol.context1 = context1
    protocol.context2 = context2
    protocol.ass_hole = ass_hole
    protocol.fuck = fuck
    protocol.damage = total_damage
	protocol.monster_list = self.__cur_info.monster_list
	GlobalNetAdapter:OnSend(protocol)

	LogTool.LogInfo("[Modules.BattleModule:BattleEndReq] 请求战斗结束 is_win:%d, star:%d, context1:%d context2:%d", is_win, star, context1, context2)
end

--战斗结束反馈
function Modules.BattleModule:BattleEndResp(protocol_data)
	CallModuleFunc(ModuleType.DUNGEON,"SetIsMainDungeonFight",false)
    self:HandleBattleEndResp(protocol_data)
end

--请求助战目标
function Modules.BattleModule:BattleAssistTargetReq(chapter_index,gamelevel_index,dungeon_type,callback_func,is_refrash)
	self.__tmp_chapter_index = chapter_index
	self.__tmp_gamelevel_index = gamelevel_index
	self.__tmp_dungeon_type = dungeon_type
	self.__tmp_callback_func = callback_func
	self.__is_refrash_handle = is_refrash
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_BATTLE_ASSIST_TARGET_REQ)
	GlobalNetAdapter:OnSend(protocol)
end

--请求助战目标响应
function Modules.BattleModule:BattleAssistTargetResp(protocol_data)
	CPPGameLib.PrintTable("ldx",protocol_data,"BattleAssistTargetRespBattleAssistTargetRespBattleAssistTargetResp ")
	if self.__is_refrash_handle then
		--是否为刷新操作
		GlobalEventSystem:FireNextFrame(Macros.Event.DungeonModule.REFRASH_ASSIST_LIST,protocol_data.help_battle_target_list)
	else
		CallModuleFunc(ModuleType.DUNGEON,"OpenView",BaseViewType.ASSIST,self.__tmp_dungeon_type,self.__tmp_chapter_index,self.__tmp_gamelevel_index,protocol_data.help_battle_target_list,protocol_data.today_used_help_battle_times,self.__tmp_callback_func)
		--Modules.TemplDungeonAssistInFightingView.New()
	end	
end
--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------

--------------------------------------------------------------------------------------------------------------------------------
--基础接口
--------------------------------------------------------------------------------------------------------------------------------
--获取当前信息
function Modules.BattleModule:GetCurInfo()
    return self.__cur_info
end

--清空当前信息
function Modules.BattleModule:ClearCurInfo()
    if self.__cur_info then
        self:__ClearUseSkillLockTimer_PVP()
        self.__cur_info = nil
    end
end

--获取出生类型
function Modules.BattleModule:GetBirthType(obj_type)
    if self.__cur_info then
        return ConfigAdapter.GameLevel.GetBirthType(self.__cur_info.gamelevel_id, obj_type)
    end
end

--获取出生X偏移
function Modules.BattleModule:GetBirthPosX(obj_type)
    if self.__cur_info then
        return ConfigAdapter.GameLevel.GetBirthPosX(self.__cur_info.gamelevel_id, obj_type)
    end
    return 0
end

--获取出生Y坐标
function Modules.BattleModule:GetBirthPosY(obj_type)
    local config_camera = ConfigAdapter.Common.GetBattleCameraInfo()
    local _, map_height = GlobalScene:GetMapSize()

    --人物层与地形层居中对齐时，上下偏移量的一半
    local delta = (1 - 1 / config_camera.terrain_obj_factor) * map_height / 2

    local birth_y = map_height * 2 / 3
    if self.__cur_info then
        birth_y = ConfigAdapter.GameLevel.GetBirthPosY(self.__cur_info.gamelevel_id, obj_type)
    end

    birth_y = (birth_y - delta) * config_camera.terrain_obj_factor
    return birth_y
end

--获取战役区域
function Modules.BattleModule:GetBattleRegion()
    local region_bound = {}
    local region_bound_w, region_bound_h
    local map_width, map_height = GlobalScene:GetMapSize()
    local camera_visual_width, camera_visual_height = GlobalScene:GetCameraVisualSize()

    if self.__cur_info then
        local region_offset_x, region_offset_y = ConfigAdapter.GameLevel.GetBattleOffset(self.__cur_info.gamelevel_id, self.__cur_info.battle_index)
        region_bound.org = Vector2D.New(region_offset_x, region_offset_y)
        region_bound_w = camera_visual_width + ConfigAdapter.GameLevel.GetBattleMapExtend(self.__cur_info.gamelevel_id)
        region_bound_h = camera_visual_height
    else
        region_bound.org = Vector2D.New(0, 0)
        region_bound_w = camera_visual_width
        region_bound_h = camera_visual_height
    end
    if region_bound.org.x + region_bound_w > map_width then
        region_bound_w = map_width - region_bound.org.x
    end
    if region_bound.org.y + region_bound_h > map_height then
        region_bound_h = map_height - region_bound.org.y
    end
    region_bound.size = COCOSize(region_bound_w, region_bound_h)

    return region_bound
end

--获取战斗伤害总量(不含治疗)
function Modules.BattleModule:GetTotalDamage()
    local total_damage = 0
    if self.__cur_info then
        for _, monster_info in ipairs(self.__cur_info.monster_list) do
            total_damage = total_damage + (monster_info.total_damage or 0)
        end     
    end
    return total_damage
end

--获取战斗实际伤害总量(治疗+伤害)
function Modules.BattleModule:GetRealTotalDamage()
    local total_damage = 0
    if self.__cur_info then
        for _, monster_info in ipairs(self.__cur_info.monster_list) do
            total_damage = total_damage + monster_info.damage
        end     
    end
    return total_damage
end

--处理引导标记
function Modules.BattleModule:HandleLeadTag()
    self.__is_have_lead_tag = true
end

--设置引导标记
function Modules.BattleModule:SetHandleLeadTag()
    self.__is_have_lead_tag = false
end

--获取引导标记
function Modules.BattleModule:GetHandleLeadTag()
    return self.__is_have_lead_tag
end

--是否AI控制镜头中
function Modules.BattleModule:IsInAICameraControl()
    return self.__is_in_camera_control
end

--设置是否在AI控制镜头中
function Modules.BattleModule:SetAICameraControl(is_in_camera_control)
    self.__is_in_camera_control = is_in_camera_control
end

--------------------------------------------------------------------------------------------------------------------------------
--战斗流程
--------------------------------------------------------------------------------------------------------------------------------
--开始战斗
function Modules.BattleModule:StartBattle()
    if self.__cur_info then
        --首场战役
        if self.__cur_info.battle_index <= 1 then
            --开启倒计时
            local config_gamelevel = CPPGameLib.GetConfig("GameLevel", self.__cur_info.gamelevel_id)
            CallModuleFunc(ModuleType.SP_LOGIC_MGR, "SetEndTime", config_gamelevel.time)

            --快速挑战
            if self:IsInQuickBattle() then
                CallModuleFunc(ModuleType.SP_LOGIC_MGR, "HandleQuickBattle")
            else
                --设置自动战斗
                self:SetAutoFight()             
            end
        else
            --重置敌方公共怒气
            GlobalVOMgr:SetAnger(0, true)
        end
    end
end

--开始战役事件
function Modules.BattleModule:StartBattleEvent()
    self:FireNextFrame(Macros.Event.BattleModule.BATTLE_START, self.__cur_info.gamelevel_id, self.__cur_info.battle_index)
end

--下场战役
function Modules.BattleModule:ChangeBattle(max_battle_index)
    if self.__cur_info then
        self.__cur_info.battle_index = self.__cur_info.battle_index + 1
        if max_battle_index and max_battle_index < self.__cur_info.battle_index then
            self.__cur_info.battle_index = max_battle_index
        end
        self.__cur_info.battle_id = ConfigAdapter.GameLevel.GetBattleID(self.__cur_info.gamelevel_id, self.__cur_info.battle_index)
    end
end

--处理结束请求相关
function Modules.BattleModule:HandleBattleEndReq(is_manual_exit, context1, context2)
    self.__cur_info.context1 = context1
    self.__cur_info.context2 = context2
    self.__cur_info.is_manual_exit = is_manual_exit

    --跳过战斗中断
    self:StopSkip()

    --取消加速
    CallModuleFunc(ModuleType.SP_LOGIC_MGR, "SetDeltaSpeedTime", 1)
end

--处理结束反馈相关
function Modules.BattleModule:HandleBattleEndResp(protocol_data, close_view, fire_event)
    self.__cur_info.star = protocol_data.star
    self.__cur_info.is_win = protocol_data.is_win
    self.__cur_info.is_first_win = protocol_data.is_first_win
    self.__cur_info.is_me_kill = protocol_data.is_me_kill

    --奖励列表
    self.__cur_info.item_list = self:__ParseRewardList(protocol_data)

    --结束参数列表
    self.__cur_info.end_params_list = {}
    if protocol_data.param_list then
        for k, v in ipairs(protocol_data.param_list) do
            self.__cur_info.end_params_list[k] = v.param
        end
    end

    if close_view ~= false then
        self:CloseView()
    end
    --退出副本，返回主城
    if self.__cur_info.is_manual_exit then
        CallModuleFunc(ModuleType.SP_LOGIC_MGR, "BattleExitHandle")
        return
    end

    if protocol_data.is_win == Macros.Global.TRUE then
        CallModuleFunc(ModuleType.SP_LOGIC_MGR, "BattleSuccessHandle")
    else
        CallModuleFunc(ModuleType.SP_LOGIC_MGR, "BattleFailedHandle")
    end

    --检测通关记录
    CallModuleFunc(ModuleType.DUNGEON, "CheckPassGameLevel")    

    --战斗结束事件
    if fire_event ~= false then
        self:FireNextFrame(Macros.Event.BattleModule.BATTLE_END, self.__cur_info.gamelevel_id, protocol_data.is_win)
    end
end

--战斗结束，从副本/对应界面返回主城
function Modules.BattleModule:BattleEndReturnCity()	
	self:ClearCurInfo()

    --取消加速
	CallModuleFunc(ModuleType.SP_LOGIC_MGR, "SetDeltaSpeedTime", 1)

	--返回主城
	GlobalScene:ChangeScene(Macros.Game.FBType.MAIN_CITY)			
end

--战斗结束，从结算界面返回对应界面
function Modules.BattleModule:BattleEndReturnRelevantView(...)
	if self.__cur_info then	
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "BattleEndReturnRelevantView",nil,...)
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--战斗加速
--------------------------------------------------------------------------------------------------------------------------------
--获取加速档位
function Modules.BattleModule:GetACCIndex()
    if self.__cur_info then
        return self.__cur_info.acc_index or 1
    end
    return 1
end

--切换加速档位
function Modules.BattleModule:ChangeACCIndex()
    if self.__cur_info then
        local acc_list = ConfigAdapter.Common.GetBattleACCList()
        local open_vip = CallModuleFunc(ModuleType.VIP, "GetToVipOpen", Macros.Game.VIPType.BATTLE_ACC_MAX)
        local is_open_by_vip = CallModuleFunc(ModuleType.VIP, "GetPrivilege", Macros.Game.VIPType.BATTLE_ACC_MAX) == Macros.Global.TRUE
        local open_level = ConfigAdapter.Common.GetBattleAccMaxOpenLev()
        local is_open_by_lev = open_level <= CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
        local is_open = is_open_by_vip or is_open_by_lev

        --开启处理
        self.__cur_info.acc_index = self.__cur_info.acc_index + 1
        if not is_open and self.__cur_info.acc_index >= #acc_list then
            self.__cur_info.acc_index = self.__cur_info.acc_index + 1
            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("battle_acc_open_tips" , open_vip , open_level))
        end

        --循环处理
        if not acc_list[self.__cur_info.acc_index] then
            self.__cur_info.acc_index = 1
        end

        Cookies.Set(Cookies.LevelType.ROLE, Macros.Setting.BattleACCIndex, tostring(self.__cur_info.acc_index))

        CallModuleFunc(ModuleType.SP_LOGIC_MGR, "SetDeltaSpeedTime", self:GetACCFactor(), true)             
    end
end

--获取加速系数
function Modules.BattleModule:GetACCFactor()
    if CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsPVPDungeon") then
        return ConfigAdapter.Common.GetPVPACCFactor()
    end

    if self.__cur_info and self.__cur_info.acc_index then
        local acc_list = ConfigAdapter.Common.GetBattleACCList()
        return acc_list[self.__cur_info.acc_index] or 1
    end
    return 1
end
--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------

--------------------------------------------------------------------------------------------------------------------------------
--自动战斗
--------------------------------------------------------------------------------------------------------------------------------
--是否自动战斗
function Modules.BattleModule:GetAutoFight()
    if self.__cur_info then
        return self.__cur_info.is_auto_fight
    end
end

--设置自动战斗
function Modules.BattleModule:SetAutoFight(var)
    if self.__cur_info then
        if var ~= nil then
            self.__cur_info.is_auto_fight = var
        end

        Cookies.Set(Cookies.LevelType.ROLE, Macros.Setting.BattleAuto, self.__cur_info.is_auto_fight and 1 or 0)

        --更新英雄自动战斗标记
        local hero
        local hero_vo_map = GlobalVOMgr:GetHeroMap()
        for obj_id, _ in pairs(hero_vo_map) do
            hero = GlobalScene:GetObj(obj_id)
            if hero then
                hero:SetAutoFight(self.__cur_info.is_auto_fight)
            end
        end

        --检测怒气/合击技能自动释放
        Skill.SkillLogic:CheckAutoReleaseAngerOrJointSkill()        
    end
end
--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------

--------------------------------------------------------------------------------------------------------------------------------
--战斗跳过
--------------------------------------------------------------------------------------------------------------------------------
--跳过战斗
function Modules.BattleModule:Skip()
    if self.__cur_info and not self.__cur_info.in_skip then
        self.__cur_info.in_skip = true

        --处理战斗跳过逻辑
        CallModuleFunc(ModuleType.SP_LOGIC_MGR, "HandleSkipBattle")

        --发送战斗跳过事件
        self:FireNextFrame(Macros.Event.BattleModule.BATTLE_SKIP)
    end
end

--结束跳过战斗
function Modules.BattleModule:StopSkip()
    if self.__cur_info and self.__cur_info.in_skip then
        self.__cur_info.in_skip = false

        --处理战斗跳过逻辑结束
        CallModuleFunc(ModuleType.SP_LOGIC_MGR, "HandleSkipBattleEnd")

        --发送战斗跳过结束事件
        self:FireNextFrame(Macros.Event.BattleModule.BATTLE_SKIP_ABORT)     
    end
end

--跳过战斗中
function Modules.BattleModule:IsSkipping()
    if self.__cur_info then
        return self.__cur_info.in_skip
    end
end
--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------

--------------------------------------------------------------------------------------------------------------------------------
--快速挑战(客户端通过公式直接计算战斗结果)
--------------------------------------------------------------------------------------------------------------------------------
--开始快速挑战
function Modules.BattleModule:StartQuickBattle(...)
    if self:BattleStartReq(...) then
        self.__to_quick_battle = true
        return true
    end
end

--结束快速挑战
function Modules.BattleModule:StopQuickBattle(message, callback)
    if not self.__cur_info or not self.__cur_info.in_quick_battle then
        return
    end

    if CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetCurStatus") == Modules.SplogicStatus.RUN then
        --快速挑战中，不能中断的相关提示
        GlobalTipMsg.GetInstance():Show(message)
        return 
    end

    CallModuleFunc(ModuleType.SP_LOGIC_MGR, "BattleEndReturnRelevantView", function()
        if callback then
            callback()
        end
    end)
end

--是否战斗中
function Modules.BattleModule:IsInBattle()
    return self.__cur_info
end

--是否快速战斗中
function Modules.BattleModule:IsInQuickBattle()
    return self.__cur_info and self.__cur_info.in_quick_battle
end
--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------

--------------------------------------------------------------------------------------------------------------------------------
--战斗对象
--------------------------------------------------------------------------------------------------------------------------------
--获取战斗英雄信息(我方)
--[[
    基础信息 + 战斗属性(包含其他系统的属性加成)
--]]
function Modules.BattleModule:GetBattleHeroInfo(lineup_index, for_battle, ignore_additional_props)
    local lineup_info = CallModuleFunc(ModuleType.HERO, "GetLineupInfo", lineup_index)
    if not lineup_info then
        return
    end

    local hero_info = CallModuleFunc(ModuleType.HERO, "GetDetailHeroInfo", lineup_info.uid)
    if not hero_info then
        return
    end
    hero_info = CPPGameLib.CopyTblShallow(hero_info)
    hero_info.lineup_index = lineup_index

    local _, lineup_equip_map, lineup_treasure_map = CallModuleFunc(ModuleType.HERO, "GetLineupEquipInfo", lineup_index)

    --处理宿命属性加成
    local plus_prop_map = CallModuleFunc(ModuleType.HERO, "GetHeroReinforcementPlusPropMap", lineup_info.uid)
    Modules.BattleFormula:HandlePlusPropMap(hero_info, plus_prop_map)
    -- CPPGameLib.PrintTable("skfb", plus_prop_map, "宿命属性加成")

    --处理援军助威属性加成 
    local plus_prop_map = CallModuleFunc(ModuleType.HERO, "GetHeroReinforcementCheerPlusPropMap")
    Modules.BattleFormula:HandlePlusPropMap(hero_info, plus_prop_map)
    -- CPPGameLib.PrintTable("skfb", plus_prop_map, "处理援军助威属性加成")

    --获取装备强化、精炼、套装、天赋属性、升星加成
    plus_prop_map = ConfigAdapter.Equip.GetPlusPropMap(lineup_equip_map)
    Modules.BattleFormula:HandlePlusPropMap(hero_info, plus_prop_map)
    -- CPPGameLib.PrintTable("skfb", plus_prop_map, "装备强化、精炼、套装、天赋属性加成")

    --处理圣器强化、精炼属性加成
    plus_prop_map = ConfigAdapter.Treasure.GetPlusPropMap(lineup_treasure_map)
    Modules.BattleFormula:HandlePlusPropMap(hero_info, plus_prop_map)   
    -- CPPGameLib.PrintTable("skfb", plus_prop_map, "圣器强化、精炼属性加成")

    --处理装备锻造大师属性加成
    plus_prop_map = ConfigAdapter.Forge.GetEquipPlusPropMap(lineup_equip_map)
    Modules.BattleFormula:HandlePlusPropMap(hero_info, plus_prop_map)
    -- CPPGameLib.PrintTable("skfb", plus_prop_map, "装备锻造大师")

    --处理圣器锻造大师属性加成
    plus_prop_map = ConfigAdapter.Forge.GetTreasurePlusPropMap(lineup_treasure_map) 
    Modules.BattleFormula:HandlePlusPropMap(hero_info, plus_prop_map)
    -- CPPGameLib.PrintTable("skfb", plus_prop_map, "圣器锻造大师")

    --处理世界之心属性加成
    local world_heart_data = CallModuleFunc(ModuleType.WORLD_HEART, "GetWorldHeartData")
    plus_prop_map = ConfigAdapter.Spirit.GetWorldHeartPlusPropMap(world_heart_data.org_stage, world_heart_data.org_level)
    Modules.BattleFormula:HandlePlusPropMap(hero_info, plus_prop_map)   
    -- CPPGameLib.PrintTable("skfb", plus_prop_map, "世界之心")

    --处理精灵属性加成
    local spirit_data = CallModuleFunc(ModuleType.SPIRIT, "GetSpiritData")
    
    --出战精灵属性加成
    plus_prop_map = ConfigAdapter.Spirit.GetPropMapEx(spirit_data, nil, true)
    Modules.BattleFormula:HandlePlusPropMap(hero_info, plus_prop_map)   
    -- CPPGameLib.PrintTable("skfb", plus_prop_map, "出战精灵属性加成")
    
    --祝福精灵属性加成
    local sprite_info = CallModuleFunc(ModuleType.SPIRIT, "GetSpiritInfoByID", lineup_info.bless_sprite_id)
    if sprite_info then
        hero_info.bless_sprite_info = sprite_info
        plus_prop_map, hero_info.bless_sprite_prop_map = ConfigAdapter.Spirit.GetPropMapEx(spirit_data, sprite_info.id, nil, true, true)
        Modules.BattleFormula:HandlePlusPropMap(hero_info, plus_prop_map)       
        -- CPPGameLib.PrintTable("skfb", plus_prop_map, "祝福精灵属性加成")
        -- CPPGameLib.PrintTable("skfb", hero_info.bless_sprite_prop_map, "祝福精灵总属性")
    end

    --处理公会技能属性加成
    plus_prop_map = CallModuleFunc(ModuleType.UNION, "GetSkillPlusPropMap")
    Modules.BattleFormula:HandlePlusPropMap(hero_info, plus_prop_map)   
    -- CPPGameLib.PrintTable("skfb", plus_prop_map, "公会技能属性加成")

    --处理称号属性加成
    plus_prop_map = CallModuleFunc(ModuleType.TITLE, "GetPlusPropMap")  
    Modules.BattleFormula:HandlePlusPropMap(hero_info, plus_prop_map)   
    -- CPPGameLib.PrintTable("skfb", plus_prop_map, "称号属性加成")

    --处理爵位属性加成
    plus_prop_map = CallModuleFunc(ModuleType.KNIGHTHOOD, "GetPlusPropMap")
    Modules.BattleFormula:HandlePlusPropMap(hero_info, plus_prop_map)
    -- CPPGameLib.PrintTable("skfb", plus_prop_map, "爵位属性加成")

    --处理宝石属性加成
    plus_prop_map = CallModuleFunc(ModuleType.GEM, "GetPlusPropMap", lineup_info.gem_list:GetData())
    Modules.BattleFormula:HandlePlusPropMap(hero_info, plus_prop_map)
    -- CPPGameLib.PrintTable("skfb", plus_prop_map, "宝石属性加成")

    --处理占卜属性加成
    plus_prop_map = CallModuleFunc(ModuleType.DIVINATION, "GetPlusPropMap", lineup_info.divination_list)
    Modules.BattleFormula:HandlePlusPropMap(hero_info, plus_prop_map)
    -- CPPGameLib.PrintTable("skfb", plus_prop_map, "占卜属性加成")   

    if for_battle then
        local config_hero = CPPGameLib.GetConfig("Hero", hero_info.info_id)         
        local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")

        --处理爬塔属性加成
        if battle_info and (battle_info.type == Macros.Game.FBType.TOWER_COMMON or battle_info.type == Macros.Game.FBType.TOWER_ELITE) then
            local plus_prop_list = CallModuleFunc(ModuleType.TOWER, "GetHoldBuff")
            if plus_prop_list then
                plus_prop_map = {}
                for _, v in ipairs(plus_prop_list) do
                    plus_prop_map[v.type] = plus_prop_map[v.type] or 0
                    plus_prop_map[v.type] = plus_prop_map[v.type] + v.value
                end
                Modules.BattleFormula:HandlePlusPropMap(hero_info, plus_prop_map)
            end
        end

        --处理世界BOSS阵营属性加成
        if battle_info and battle_info.type == Macros.Game.FBType.WORLD_BOSS then
            local camp = CallModuleFunc(ModuleType.WORLDBOSS, "GetCamp")            
            if config_hero.camp == camp then
                plus_prop_map = ConfigAdapter.Battle.GetCampPlusPropMap(camp, "world_boss_damage_add")
                Modules.BattleFormula:HandlePlusPropMap(hero_info, plus_prop_map)           
            end
        end

        --处理军团副本阵营属性加成
        if battle_info and battle_info.type == Macros.Game.FBType.UNION then
            if config_hero.camp == battle_info.camp then
                plus_prop_map = ConfigAdapter.Battle.GetCampPlusPropMap(battle_info.camp, "guild_fb_damage_add")
                Modules.BattleFormula:HandlePlusPropMap(hero_info, plus_prop_map)                       
            end
        end

        --处理积分赛阵营属性加成
        if battle_info and battle_info.type == Macros.Game.FBType.INTEGRAL_RACE then
            local camp = CallModuleFunc(ModuleType.INTEGRAL_RACE, "GetCamp")            
            if config_hero.camp == camp then
                plus_prop_map = ConfigAdapter.Battle.GetCampPlusPropMap(camp, "integral_race_damage_add")
                Modules.BattleFormula:HandlePlusPropMap(hero_info, plus_prop_map)           
            end
        end

        --处理怪物攻城鼓舞属性加成
        if battle_info and battle_info.type == Macros.Game.FBType.ATTACK_CITY then
            local player_info = CallModuleFunc(ModuleType.MONSTER_STORMCASTLE, "GetPlayerInfo")
            local config_encourage = CPPGameLib.GetConfig("AttackCityEncourage", player_info.encourage_times, false)
            if config_encourage then
                local plus_prop_map = {}
                plus_prop_map[config_encourage.pro_type] = config_encourage.pro_value
                Modules.BattleFormula:HandlePlusPropMap(hero_info, plus_prop_map)                           
            end
        end
    end

    --处理加成类属性
    if not ignore_additional_props then
        Modules.BattleFormula:CalcAdditionalProps(hero_info)
    end

    if for_battle then
        local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")

        --处理战力压制
        if battle_info then
            Modules.BattleFormula:HandlePlusPropMap(hero_info, battle_info.my_combat_suppression_plus_prop_map)
        end

        --处理王者赛血量加成(最终加成，所以在处理完加成类属性之后执行)
        if battle_info and battle_info.type == Macros.Game.FBType.LADDER then
            hero_info.hp = hero_info.hp * ConfigAdapter.Common.GetLadderBattleObjHpFactor()
        end         

        --处理竞技场血量修正(最终加成，所以在处理完加成类属性之后执行)
        if battle_info and battle_info.type == Macros.Game.FBType.ARENA then
            hero_info.hp = hero_info.hp * ConfigAdapter.Common.GetArenaHPFactor()
        end     

       --处理资源战血量修正
        if battle_info and battle_info.type == Macros.Game.FBType.RESOURCE_WAR then
            hero_info.hp = hero_info.hp * ConfigAdapter.Common.GetResourceWarHPFactor()
        end     

        --处理积分赛血量修正
        if battle_info and battle_info.type == Macros.Game.FBType.INTEGRAL_RACE then
            hero_info.hp = hero_info.hp * ConfigAdapter.Common.GetIntegralRaceHPFactor()
        end             
    end

    return hero_info
end

--获取战斗英雄列表(我方)
function Modules.BattleModule:GetBattleHeroList(sortable)
    local list = {}

    local hero_info
    local hero_lineup_list = CallModuleFunc(ModuleType.HERO, "GetLineupList")
    for lineup_index, lineup_info in ipairs(hero_lineup_list) do
        hero_info = self:GetBattleHeroInfo(lineup_index, true)
        if hero_info then
            table.insert(list, hero_info)
        end
    end

    if sortable ~= false then
        Modules.BattleFormula:SortBattleHeroList(list)
    end

    return list
end

--获取上阵英雄列表(我方)，用于上阵界面和战斗界面
function Modules.BattleModule:GetLineupHeroList(for_battle)
    local list = {}

    local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")
    local hero_info
    local lineup_info
    local lineup_list = CallModuleFunc(ModuleType.HERO, "GetLineupList")
    local lineup_index_to_obj_id_map = GlobalVOMgr:GetHeroLineupIdx2ObjIDMap()
    for i, v in ipairs(lineup_list) do
        hero_info = CallModuleFunc(ModuleType.HERO, "GetHeroInfo", v.uid)
        lineup_info = ConfigAdapter.Hero.GetLineupPosInfo(i)
        table.insert(list, 
        {
            lineup_index = i, 
            info_id = hero_info and hero_info.info_id or 0, 
            obj_id = for_battle and lineup_index_to_obj_id_map[i],
            open_level = lineup_info and lineup_info.need_level,
            is_open = lineup_info and lineup_info.need_level <= role_info.level
        })
    end

    Modules.BattleFormula:SortLineupHeroList(list)

    return list
end

--获取战斗英雄总战力(我方)
function Modules.BattleModule:GetBattleHeroPower(for_battle)
    local power = 0
    local hero_info
    local hero_lineup_list = CallModuleFunc(ModuleType.HERO, "GetLineupList")
    for lineup_index, lineup_info in ipairs(hero_lineup_list) do
        hero_info = self:GetBattleHeroInfo(lineup_index, for_battle, true)
        power = power + CallModuleFunc(ModuleType.POWER, "CalcHeroPower", hero_info) 
    end 
    return power
end

--获取战斗英雄战力(我方)
function Modules.BattleModule:GetSingleBattleHeroPower(lineup_index, for_battle)
    local hero_info = self:GetBattleHeroInfo(lineup_index, for_battle, true)
    return CallModuleFunc(ModuleType.POWER, "CalcHeroPower", hero_info) 
end

--创建助战英雄(我方)
function Modules.BattleModule:CreateHeroAssist()
    if not CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsAllObjsEnterRegion") then
        --有参战对象未进入视野，过滤
        return
    end

    if CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsInAngerOrJointSkill") then
        --有参战对象释放怒气/合击技能中，过滤
        return
    end

    if not self.__cur_info or self.__cur_info.is_hero_assist_released then
        return
    end
    self.__cur_info.is_hero_assist_released = true

    --初始化助战英雄
    local hero_assist, last_hero, last_hero_id
    local vo = Scene.HeroAssistVO.New()
    vo:ParseInfo(self.__cur_info.assist_hero_id, self.__cur_info.help_battle_hero_pro_list)
    vo.dir = MathTool.HORZ_RIGHT
    hero_assist = GlobalScene:CreateHeroAssist(vo, nil, Scene.ObjType.HERO)

    local camera_bounding = GlobalScene:GetCameraBounding()
    last_hero_id = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetLastTeammate", vo.obj_id)
    last_hero = GlobalScene:GetObj(last_hero_id)
    hero_assist:SetWorldPos(camera_bounding.org.x + 128, last_hero and last_hero:GetWorldPosY() or 320)
    hero_assist:SetZOrder(Scene.MaxZOrder)

    --助战英雄入场
    hero_assist:DoBirth()

    return true
end

--创建助战怪物(我方)
function Modules.BattleModule:CreateMonsterAssist(info_id)
    if not CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsAllObjsEnterRegion") then
        --有参战对象未进入视野，过滤
        return
    end

    if CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsInAngerOrJointSkill") then
        --有参战对象释放怒气/合击技能中，过滤
        return
    end

    --初始化助战怪物
    local config_monster = CPPGameLib.GetConfig("Monster", info_id)
    if not config_monster then
        return
    end
    local monster_info = Modules.BattleFormula:BuildBattleMonsterInfo(info_id) 

    local monster_assist, last_hero, last_hero_id
    local vo = Scene.MonsterVO.New()
    vo:ParseInfo(monster_info)
    vo.dir = MathTool.HORZ_RIGHT
    monster_assist = GlobalScene:CreateHeroAssist(vo, nil, Scene.ObjType.HERO)

    local camera_bounding = GlobalScene:GetCameraBounding()
    last_hero_id = CallModuleFunc(ModuleType.SP_LOGIC_MGR, "GetLastTeammate", vo.obj_id)
    last_hero = GlobalScene:GetObj(last_hero_id)
    monster_assist:SetWorldPos(camera_bounding.org.x + 128, last_hero and last_hero:GetWorldPosY() or 320)
    monster_assist:SetZOrder(Scene.MaxZOrder)

    --助战英雄入场
    monster_assist:DoBirth()

    return true
end

--获取战斗胜利的英雄列表(我方)
function Modules.BattleModule:GetWinHeroList()
    local hero_vo_list = {}

    local hero
    local hero_vo_map = GlobalVOMgr:GetHeroMap()
    for _, vo in pairs(hero_vo_map) do
        hero = GlobalScene:GetObj(vo.obj_id)
        if hero and not hero:IsDead() then
            table.insert(hero_vo_list, vo)
        end
    end
    Modules.BattleFormula:SortBattleHeroList(hero_vo_list)

    return hero_vo_list
end

--获取战斗英雄镜像列表(敌方)
function Modules.BattleModule:GetBattleHeroMirrorList(for_battle, ignore_additional_props)
	if self.__cur_info and #self.__cur_info.hero_mirror_list > 0 then
		for _, hero_info in ipairs(self.__cur_info.hero_mirror_list) do
			Modules.BattleFormula:ParseHeroMirrorData(hero_info)

            if for_battle then
                local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")

                --处理战力压制
                Modules.BattleFormula:HandlePlusPropMap(hero_info, self.__cur_info.enemy_combat_suppression_plus_prop_map)

                --处理王者赛血量加成
                if battle_info and battle_info.type == Macros.Game.FBType.LADDER then
                    hero_info.hp = hero_info.hp * ConfigAdapter.Common.GetLadderBattleObjHpFactor()
                end         

                --处理竞技场血量修正
                if battle_info and battle_info.type == Macros.Game.FBType.ARENA then
                    hero_info.hp = hero_info.hp * ConfigAdapter.Common.GetArenaHPFactor()
                end

		        --处理资源战血量修正
	        	if battle_info and battle_info.type == Macros.Game.FBType.RESOURCE_WAR then
		            hero_info.hp = hero_info.hp * ConfigAdapter.Common.GetResourceWarHPFactor()
		        end

		        --处理积分赛血量修正
	        	if battle_info and battle_info.type == Macros.Game.FBType.INTEGRAL_RACE then
		            hero_info.hp = hero_info.hp * ConfigAdapter.Common.GetIntegralRaceHPFactor()
				end

                --构造祝福精灵信息
                Modules.BattleFormula:BuildBattleBlessSpiritInfo({spirit_list = self.__cur_info.target_sprite_list}, hero_info)
            end            
		end

		Modules.BattleFormula:SortBattleHeroList(self.__cur_info.hero_mirror_list)

		return self.__cur_info.hero_mirror_list		
	end
end

--获取战役怪物列表
function Modules.BattleModule:GetBattleMonsterList(for_battle, ignore_additional_props)
	if self.__cur_info and #self.__cur_info.hero_mirror_list == 0 then
		local monster_list = self:__GenBattleMonsterList(for_battle, nil, ignore_additional_props)
		Modules.BattleFormula:SortBattleMonsterList(monster_list)

		return monster_list
	end
end
--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------

--------------------------------------------------------------------------------------------------------------------------------
--掉落物
--------------------------------------------------------------------------------------------------------------------------------
--生成掉落物信息
function Modules.BattleModule:GenerateDropInfo(obj_id)
	if not self.__cur_info or not self.__cur_info.drop_list or #self.__cur_info.drop_list <= 0 then
		return
	end

	if self:IsSkipping() then
		return
	end

    local obj = GlobalScene:GetObj(obj_id)
    if not obj then
        return
    end
    local obj_vo = obj:GetVO()

	--构造掉落表
	if not self.__cur_info.real_drop_list then
		local monster_list, highest_quality_monster_id = ConfigAdapter.GameLevel.GetMonsterList(self.__cur_info.gamelevel_id)

		--普通掉落
		self.__cur_info.real_drop_list = MathTool.SelectRandomItemsFrmList(monster_list, #self.__cur_info.drop_list)

		--世界之心
		if self.__cur_info.type == Macros.Game.FBType.MAIN_DUNGEON then
			local config_chapter = ConfigAdapter.Dungeon.GetChapterInfo(self.__cur_info.chapter_index)
			if config_chapter.show_world_star == Macros.Global.TRUE and 
				self.__cur_info.no_passed_flag and self.__cur_info.gamelevel_index >= #config_chapter.gamelevel_list then
				self.__cur_info.sp_drop_info = {}
				self.__cur_info.sp_drop_info.battle_index = self.__cur_info.battle_num
				self.__cur_info.sp_drop_info.monster_id = highest_quality_monster_id
			end
		end
	end

	--检测世界之心掉落
	local sp_drop_info = self.__cur_info.sp_drop_info
	if sp_drop_info and not sp_drop_info.constructed and
		sp_drop_info.battle_index == self.__cur_info.battle_index and
		sp_drop_info.monster_id == obj_vo.info_id then
		sp_drop_info.is_world_star = true
		sp_drop_info.img_name = "shijiezhixin"
		sp_drop_info.constructed = true
		return sp_drop_info
	end

	--检测普通掉落
	for _, drop_info in ipairs(self.__cur_info.real_drop_list) do
		if not drop_info.constructed and 
			drop_info.battle_index == self.__cur_info.battle_index and
			drop_info.monster_id == obj_vo.info_id then
			drop_info.img_name = "img_dropbox"
			drop_info.constructed = true
			return drop_info
		end
	end
end

--更新掉落物数量
function Modules.BattleModule:UpdateDropNum(obj_num)
	if self.__cur_info then
		self.__cur_info.drop_num = self.__cur_info.drop_num or 0
		self.__cur_info.drop_num = self.__cur_info.drop_num + obj_num
	end
end

--获取掉落物数量
function Modules.BattleModule:GetDropNum()
	if self.__cur_info then
		return self.__cur_info.drop_num
	end
	return 0
end

--获取掉落物品表
function Modules.BattleModule:GetDropList()
	if not self.__cur_info or not self.__cur_info.drop_list then
		return nil
	end

	local drop_list = {}
	local drop_list_1 = {}

	local drop_map = {}
	for _, drop_info in ipairs(self.__cur_info.drop_list) do
		drop_map[drop_info.obj_id] = drop_map[drop_info.obj_id] or 0
		drop_map[drop_info.obj_id] = drop_map[drop_info.obj_id] + drop_info.obj_num
	end

	for obj_id, obj_num in pairs(drop_map) do
		table.insert(drop_list, {id = obj_id, num = obj_num})
	end
	
	for i , v in ipairs( self.__cur_info.item_list ) do
		table.insert(drop_list_1, {id = v.id , num = v.num })
	end

	--主线副本
	if self.__cur_info.type == Macros.Game.FBType.MAIN_DUNGEON then
		local num = CallModuleFunc(ModuleType.ACTIVITY,"GetTimedOutput" , Macros.Game.FBType.MAIN_DUNGEON )
		for i , v in ipairs(drop_list) do
			v.num = v.num * num
		end
		for i , v in ipairs(drop_list_1) do
			v.num = v.num * num
		end
	--英雄副本
	elseif self.__cur_info.type == Macros.Game.FBType.HERO_DUNGEON then
		local num = CallModuleFunc(ModuleType.ACTIVITY,"GetTimedOutput" , Macros.Game.FBType.HERO_DUNGEON )
		for i , v in ipairs(drop_list) do
			v.num = v.num * num
		end
		for i , v in ipairs(drop_list_1) do
			v.num = v.num * num
		end
	--精英副本
	elseif self.__cur_info.type == Macros.Game.FBType.ELITE_DUNGEON then
		local num = CallModuleFunc(ModuleType.ACTIVITY,"GetTimedOutput" , Macros.Game.FBType.ELITE_DUNGEON )
		for i , v in ipairs(drop_list) do
			v.num = v.num * num
		end
		for i , v in ipairs(drop_list_1) do
			v.num = v.num * num
		end
	end

	for i , v in ipairs( drop_list_1 ) do
		table.insert( drop_list , v )
	end
	return drop_list
end
--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------

