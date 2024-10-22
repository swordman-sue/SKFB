
--[[
%% @module: 深渊
%% @author: swordman sue
%% @created: 2017-7-17
--]]

Modules = Modules or {}

Modules.SpChasmModule = Modules.SpChasmModule or BaseClass(Modules.SpMainDungeonModule)

function Modules.SpChasmModule:__init()
end

function Modules.SpChasmModule:__delete()
end

--初始化
function Modules.SpChasmModule:Initialize()
	Modules.SpMainDungeonModule.Initialize(self)
end

--释放
function Modules.SpChasmModule:Dispose()
	Modules.SpMainDungeonModule.Dispose(self)
end

--注册事件
function Modules.SpChasmModule:RegistAllEvents()
    Modules.SpMainDungeonModule.RegistAllEvents(self)

    --怪物VO添加
    self:BindGlobalEvent(Macros.Event.SceneObj.MONSTER_VO_ADD, function(_, obj_id, vo)
        self:__HandleMonsterVOAdd(vo)
    end)
end

--更新剩余时间
function Modules.SpChasmModule:SetRemainTime(var)
    Modules.SplogicModule.SetRemainTime(self, var)
end

--战斗结束，从结算界面返回对应界面
function Modules.SpChasmModule:BattleEndReturnRelevantView()
    local cur_keep_kill_num = CallModuleFunc(ModuleType.CHASM, "GetCurKeepKillNum")
    local is_first_battle = CallModuleFunc(ModuleType.CHASM, "GetIsFirstBattle")
    CallModuleFunc(ModuleType.CHASM, "OpenView", BaseViewType.CHASM)

    --首次战斗
    if is_first_battle then
        GlobalEventSystem:FireNextFrame(Macros.Event.ChasmModule.CHASM_FIRST_LOSE_LEAD,cur_keep_kill_num)  
    end

    Modules.SplogicModule.BattleEndReturnRelevantView(self)   
end

--战斗成功处理
function Modules.SpChasmModule:BattleSuccessHandle(callback)
    Modules.SpMainDungeonModule.BattleSuccessHandle(self, callback)

    --通关恢复血量
    local cur_wave = CallModuleFunc(ModuleType.CHASM, "GetCurWave")
    local config_wave = CPPGameLib.GetConfig("EndlessLandWave", cur_wave)
    local hero_vo_map = GlobalVOMgr:GetHeroMap()
    local obj, hp_change
    for obj_id, vo in pairs(hero_vo_map) do
        hp_change = vo.max_hp * MathTool.TransProbCoeff2Decimal(config_wave.reward_hp_factor)
        CallModuleFunc(ModuleType.SP_LOGIC_MGR, "DoHpChange", obj_id, hp_change)
    end
end

--打开胜利界面
function Modules.SpChasmModule:OpenBattleSuccessView()
    CallModuleFunc(ModuleType.CHASM, "OpenView", BaseViewType.CHASM_WIN, function()
        local cur_wave = CallModuleFunc(ModuleType.CHASM, "GetCurWave")
        local max_wave = CallModuleFunc(ModuleType.CHASM, "GetMaxWave")
        if cur_wave >= max_wave then
            --最后一波，结束
            CallModuleFunc(ModuleType.SP_LOGIC_MGR, "BattleEndReturnRelevantView")
            return
        end

        --切换到下一波
        self.__splogic_status = Modules.SplogicStatus.RUN
        self:PlayBGM()
        CallModuleFunc(ModuleType.SP_LOGIC_MGR, "Resume")

        local acc_factor = CallModuleFunc(ModuleType.BATTLE, "GetACCFactor")
        CallModuleFunc(ModuleType.SP_LOGIC_MGR, "SetDeltaSpeedTime", acc_factor)

        CallModuleFunc(ModuleType.CHASM, "ChangeWave")
        CallModuleFunc(ModuleType.BATTLE, "ChangeBattle", max_wave)
        self:__StartBattle(true)
    end)
end

--战斗失败处理
function Modules.SpChasmModule:BattleFailedHandle(callback)
    Modules.SpMainDungeonModule.BattleFailedHandle(self, callback)   
end

--对象零血通知(客户端)
function Modules.SpChasmModule:ObjDeadNotify(obj_id, frm_protocol)
    local obj = GlobalScene:GetObj(obj_id)
    if not frm_protocol and obj and obj:IsMonster() then
        --杀怪请求
        CallModuleFunc(ModuleType.CHASM, "EndlessLandKillMonsterReq", obj_id)
        return
    end

    --杀怪通知
    Modules.SpMainDungeonModule.ObjDeadNotify(self, obj_id, frm_protocol)
end

--请求战斗结束
function Modules.SpChasmModule:BattleEndReq(is_win, star, is_manual_exit)
    CallModuleFunc(ModuleType.CHASM, "EndlessLandBattleOverReq", is_win, is_manual_exit)
end

--动态创建怪物
function Modules.SpChasmModule:CreateMonsterDynamic(data)
    local cur_wave = CallModuleFunc(ModuleType.CHASM, "GetCurWave")
    local config_wave = CPPGameLib.GetConfig("EndlessLandWave", cur_wave)
    if not config_wave then
        return
    end

    local monster_info = Modules.BattleFormula:BuildBattleMonsterInfo(data.info_id, nil, nil, config_wave.add_pro_type_list, config_wave.add_pro_value_list)
    monster_info.index = data.index

    local vo = Scene.MonsterVO.New()
    vo:ParseInfo(monster_info)
    vo.dir = MathTool.HORZ_LEFT

    local obj = GlobalScene:CreateMonster(vo, true)
    obj:PauseAI()
    obj:SetVisible(true)

    local battle_region = CallModuleFunc(ModuleType.BATTLE, "GetBattleRegion")
    local horz_space = ConfigAdapter.Common.GetBattleBirthPosBoundaryMargin()    
    local monster_birth_pos_y = CallModuleFunc(ModuleType.BATTLE, "GetBirthPosY", Scene.ObjType.MONSTER)
    local birth_src_pos = Vector2D.New(battle_region.org.x + battle_region.size.width - 1, monster_birth_pos_y)
    local brith_tgt_pos = Vector2D.New(birth_src_pos.x - horz_space, birth_src_pos.y)
    obj:DoBirth(Macros.Game.FBBirthType.RUN_INTO, birth_src_pos, brith_tgt_pos)
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function Modules.SpChasmModule:__OpenBattleView()
    CallModuleFunc(ModuleType.BATTLE, "OpenView", BaseViewType.BATTLE_CHASM)
end

--是否所有怪物死亡
function Modules.SpChasmModule:IsAllMonsterDead()
    if not Modules.SpMainDungeonModule.IsAllMonsterDead(self) then
        return
    end

    local monster_vo_map = GlobalVOMgr:GetMonsterMap()
    if monster_vo_map then
        local monster
        for _, vo in pairs(monster_vo_map) do
            monster = GlobalScene:GetObj(vo.obj_id)
            if monster and not monster.is_real_dead then
                return false
            end
        end
    end

    return CallModuleFunc(ModuleType.CHASM, "IsAllMonsterCreatedOfCurWave")
end

--检测怪物死亡
function Modules.SpChasmModule:__CheckMonsterDead(obj_id)
    Modules.SpMainDungeonModule.__CheckMonsterDead(self, obj_id)
end

--获取战斗怪物列表
function Modules.SpChasmModule:__GetBattleMonsterList()
    if not self.__battle_monster_list then
        self.__battle_monster_list = CallModuleFunc(ModuleType.CHASM, "GetBattleMonsterList")
    end
    return self.__battle_monster_list
end

function Modules.SpChasmModule:__HandleMonsterVOAdd(vo)
    CallModuleFunc(ModuleType.CHASM, "HandleMonsterVOAdd", vo)
end

function Modules.SpChasmModule:__HandleMonsterVORemove(vo)
    Modules.SpMainDungeonModule.__HandleMonsterVORemove(self, vo)

    CallModuleFunc(ModuleType.CHASM, "HandleMonsterVORemove", vo)
end


--生成掉落信息
function Modules.SpChasmModule:__GenerateDropInfo(obj_id)
    return CallModuleFunc(ModuleType.CHASM, "GenerateDropInfo", obj_id)
end

--处理结束(击杀所有怪物)
function Modules.SpChasmModule:__HandleEndForAllMonsterDeadReal()
    self:BattleEndReq(true)
end
