
--[[
%% @module: 特殊逻辑基类
%% @author: swordman sue
%% @created: 2016-07-15
%% @description: 实现特殊逻辑
--]]

Modules = Modules or {}

Modules.SplogicModule = Modules.SplogicModule or BaseClass(EventAndTimerWrapper)

function Modules.SplogicModule:__init(type, scene_id)
	self.__type = type
    self.__scene_id = scene_id

    self.__protocol_regist_list = {}    
end

function Modules.SplogicModule:__delete()
end

--初始化
function Modules.SplogicModule:Initialize()
    self.__splogic_status = Modules.SplogicStatus.RUN
end

--释放
function Modules.SplogicModule:Dispose()
end

function Modules.SplogicModule:Update(now_time, elapse_time)
end

function Modules.SplogicModule:RegistAllEvents()
    --self:BindGlobalEvent(event_name, callback)
    --....
end

function Modules.SplogicModule:RegistAllProtocols()
    --self:__RegisterProtocol(protocol_no, func_name)
    --....
end

function Modules.SplogicModule:UnRegistAllProtocols()
    if self.__protocol_regist_list then
        for _, protocol_regist_id in ipairs(self.__protocol_regist_list) do
            GlobalNetAdapter:UnRegisterProtocol(protocol_regist_id)
        end
        self.__protocol_regist_list = nil
    end
end

function Modules.SplogicModule:__RegisterProtocol(protocol_no, func_name)
    local protocol_callback = function(protocol_data)
        if self[func_name] then
            self[func_name](self, protocol_data)
        end
    end
    local protocol_regist_id = GlobalNetAdapter:RegisterProtocol(protocol_no, protocol_callback)
    table.insert(self.__protocol_regist_list, protocol_regist_id)
end

function Modules.SplogicModule:GetType()
    return self.__type
end

function Modules.SplogicModule:GetStatus()
    return self.__splogic_status
end

--------------------------------------------------------------------------------------------------------------------------------
--特殊逻辑
--------------------------------------------------------------------------------------------------------------------------------
--加载资源
function Modules.SplogicModule:LoadRes()
    self.__is_res_loaded = true
end

--卸载资源
function Modules.SplogicModule:UnloadRes()
    Modules.SplogicModulesMgr.UnloadRes(self)
end

function Modules.SplogicModule:IsResLoaded()
    return self.__is_res_loaded
end

function Modules.SplogicModule:IsResLoadedEx()
    return true
end

function Modules.SplogicModule:ResLoadedNotify()
end

function Modules.SplogicModule:GetResLoadInfo()
    return (self.__loaded_tex_count or 0) + (self.__loaded_template_count or 0), 
    (self.__need_load_tex_count or 0) + (self.__need_load_template_count or 0)
end

--获取常驻内存的资源列表
function Modules.SplogicModule:GetKeepRes()
    local keep_list
    if self.__splogic_loaded_texinfo_list then
        for tex_atlas_info, _ in pairs(self.__splogic_loaded_texinfo_list) do
            keep_list = keep_list or {}
            table.insert(keep_list, tex_atlas_info.texture_path)
        end
    end
    return keep_list
end

--是否需要加载地图
function Modules.SplogicModule:IsNeedLoadMap()
    return true
end

--进入后台是否暂停
function Modules.SplogicModule:IsPauseWhenDeActive()
    if self.__splogic_status ~= Modules.SplogicStatus.RUN then
        return false
    end

    if self.__in_boss_show then
        return false
    end

    if CallModuleFunc(ModuleType.LEAD, "IsInLead") or 
        CallModuleFunc(ModuleType.STORY, "IsInStory") then
        return false
    end

    if ActionManager:GetInstance():IsInWorldStarEffect() then
        return false
    end

    if CallModuleFunc(ModuleType.BATTLE, "IsSkipping") then
        return false
    end

    return GlobalStateMgr:IsGamePlayState()
end

function Modules.SplogicModule:IsShowPauseDialog()
    if not self:IsPauseWhenDeActive() then
        return false
    end

    local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
    if battle_info then
        local config_gamelevel = CPPGameLib.GetConfig("GameLevel", battle_info.gamelevel_id)
        if config_gamelevel.show_pause_btn == Macros.Global.FALSE then
            --隐藏暂停按钮的关卡，不显示暂停弹窗
            return false
        end
    end

    if not GlobalNetAdapter:IsConnected() then
        --已弹出断线重连弹窗，不显示暂停弹窗
        return false
    end

    return true    
end

function Modules.SplogicModule:Pause(for_end, for_world_star)
    if for_end then
        GlobalAIMgr:Pause()
        GlobalBuffMgr:Dump()
        ActionManager.GetInstance():Dump()
        return
    end

    GlobalScene:Pause()
    GlobalAIMgr:Pause()
    GlobalBuffMgr:Pause()
    GlobalSoundMgr:Pause()    
    ActionManager.GetInstance():Pause()

    if for_world_star ~= true then
        GlobalEffectMgr:Pause() 
    end
end

function Modules.SplogicModule:Resume()
    GlobalScene:Resume()
    GlobalAIMgr:Resume()
    GlobalBuffMgr:Resume()
    GlobalEffectMgr:Resume()
    ActionManager.GetInstance():Resume()

    GlobalSoundMgr:Resume()    
end

--进入GamePlaying状态
function Modules.SplogicModule:GamePlayingStartHandle()
    --居中镜头
    GlobalScene:SetCameraCenter()

    --设置运行帧率
    -- CPPSceneManager:SetGameFrame(Macros.Global.FPS_Default)

    --销毁阵容界面
    CallModuleFunc(ModuleType.HERO, "DumpViews")

    self:FireNextFrame(Macros.Event.SpLogicModule.GAME_PLAYING_START_HANDLE, self.__type)
end

--离开GamePlaying状态
function Modules.SplogicModule:GamePlayingStopHandle()
    if not self.__splogic_status then
        return
    end

    --居中镜头
    GlobalScene:SetCameraCenter()

    --销毁AI
    GlobalAIMgr:Dump()

    --销毁BUFF
    GlobalBuffMgr:Dump()

    --销毁VO
    GlobalVOMgr:Dump()

    --销毁指引相关
    CallModuleFunc(ModuleType.LEAD, "Dump")

    --销毁剧情相关
    CallModuleFunc(ModuleType.STORY, "Dump")

    --快速挑战中，不进行以下处理
    if not CallModuleFunc(ModuleType.BATTLE, "IsInQuickBattle") then
        --销毁界面
        GlobalViewMgr:Dump()

        --销毁场景模板
        GUITemplateMgr:Dump()

        --销毁特效
        GlobalEffectMgr:Dump()

        --销毁动作
        ActionManager.GetInstance():Dump()       

        --销毁异步加载数据
        AsyncLoadMgr.GetInstance():Dump()

        --销毁音效
        GlobalSoundMgr:Dump()

        --销毁场景
        GlobalScene:Dump()            

        --释放资源
        GlobalGame:ReleaseResource(true)
    else
        --销毁场景
        GlobalScene:Dump()    
    end
end

function Modules.SplogicModule:BattleEndHandle(ret)
    self.__splogic_status = ret
end

--战斗退出处理
function Modules.SplogicModule:BattleExitHandle()
    return false
end

--战斗成功处理
function Modules.SplogicModule:BattleSuccessHandle(callback)
    if callback then
        callback()
    end
end

--打开胜利界面
function Modules.SplogicModule:OpenBattleSuccessView()
end

--战斗平局处理
function Modules.SplogicModule:BattleTieHandle(callback)
    if callback then
        callback()
    end
end

--打开平局界面
function Modules.SplogicModule:OpenBattleTieView()
end

--战斗失败处理
function Modules.SplogicModule:BattleFailedHandle(callback)
    GlobalScene:SetObjGray(true)
    GlobalScene:SetMapGray(true)

    if callback then
        callback()
    end
end

--打开失败界面
function Modules.SplogicModule:OpenBattleFailedView()
end

--战斗结束，从结算界面返回对应界面之前的预加载
function Modules.SplogicModule:BattleEndReturnRelevantViewPreload(finished_callback)
    if finished_callback then
        finished_callback()
    end

    -- CallModuleFunc(ModuleType.LOADING, "OpenView", BaseViewType.UI_LOADING_II, function()
    --     CallModuleFunc(ModuleType.SP_LOGIC_MGR, "LoadRes", Game.CityCommonTexture, Game.CityCommonTempl, function()
    --         GlobalViewMgr:CloseViews({[BaseViewType.UI_LOADING_II] = true}, nil, true, true)
    --         if finished_callback then
    --             finished_callback()
    --         end
    --     end)    
    -- end)
end

--战斗结束，从结算界面返回对应界面
function Modules.SplogicModule:BattleEndReturnRelevantView(fire_event)
    self.__splogic_status = Modules.SplogicStatus.EXIT
    
    CallModuleFunc(ModuleType.BATTLE, "ClearCurInfo")    

    if fire_event ~= false then
        --派发结束事件
        self:FireNextFrame(Macros.Event.BattleModule.BATTLE_END_RETURN_RELEVANT_VIEW)
    end
end

--边界处理(以屏幕可视区域为边界)
function Modules.SplogicModule:BoundTest(pos, is_world_pos)
    local horz_space = ConfigAdapter.Common.GetBattleBirthPosBoundaryMargin()
    local map_width, map_height = GlobalScene:GetMapSize()
    local min_x = horz_space
    local max_x = map_width - 1 - horz_space
    local min_y = 0
    local max_y = map_height - 1

    local world_pos_x, world_pos_y
    if is_world_pos then
        world_pos_x = pos.x
        world_pos_y = pos.y
    else
        world_pos_x, world_pos_y = GlobalScene:LogicToWorld(pos.x, pos.y)
    end

    world_pos_x = math.max(min_x, world_pos_x)
    world_pos_x = math.min(world_pos_x, max_x)
    world_pos_y = math.max(min_y, world_pos_y)
    world_pos_y = math.min(world_pos_y, max_y)

    if is_world_pos then
        pos.x = world_pos_x
        pos.y = world_pos_y
    else
        pos.x, pos.y = GlobalScene:WorldToLogic(world_pos_x, world_pos_y)
    end
end

--血量处理
function Modules.SplogicModule:DoHpChange(obj_id, change_hp, damage_type, frm_protocol)
    local obj = GlobalScene:GetObj(obj_id)
    if not obj then
        return
    end

    if frm_protocol then
        return
    end

    damage_type = damage_type or Modules.BattleFormula.DamageType.NORMAL

    local obj_vo = obj:GetVO()
    local old_hp = obj_vo.hp
    obj:SetHp(obj_vo.hp + change_hp)

    if change_hp > 0 then
        --检测精灵祝福BUFF
        Skill.SkillLogic:CheckSpriteBlessBuff(Macros.Game.SpiritBlessSkillTriggerMode.BE_TREATED, nil, obj_vo)        
    end

    --飘血特效
    obj:ShowHpEffect(change_hp, damage_type)

    if old_hp ~= obj_vo.hp then
        change_hp = obj_vo.hp - old_hp
        
        --派发血量改变事件
        GlobalEventSystem:FireNextFrame(Macros.Event.SceneObj.OBJ_HP_CHANGE, obj_id, obj_vo.hp, change_hp)    
    end

    if old_hp > 0 and obj_vo.hp <= 0 then
        --对象零血通知
        self:ObjDeadNotify(obj_id)
    end    
end

function Modules.SplogicModule:HPChangeSyncReq(type, src_obj_id, tgt_obj_id, change_hp, damage_type)
end

--对象零血通知(客户端)
function Modules.SplogicModule:ObjDeadNotify(obj_id)
    local obj = GlobalScene:GetObj(obj_id)
    if not obj then
        return
    end

    obj.is_real_dead = true
    if obj:CanDoDead() then
        obj:DoDead()
    end
end

--怒气处理
function Modules.SplogicModule:DoAngerChange(change_anger, is_enemy)
    local old_anger = GlobalVOMgr:GetAnger(is_enemy)
    GlobalVOMgr:SetAnger(old_anger + change_anger, is_enemy)
    local new_anger = GlobalVOMgr:GetAnger(is_enemy)
    local is_change = old_anger ~= new_anger
    local is_full = new_anger >= GlobalVOMgr:GetMaxAnger()

    --派发英雄公共怒气改变事件
    if is_change and not is_enemy then
        GlobalEventSystem:FireNextFrame(Macros.Event.SceneObj.COMMON_ANGER_CHANGE, old_anger, new_anger)
    end

    if is_change and is_enemy then
        GlobalEventSystem:FireNextFrame(Macros.Event.SceneObj.COMMON_ANGER_CHANGE_OF_ENEMY, old_anger, new_anger)
    end

    --检测怒气/合击技能自动释放
    if is_change or is_full then 
        Skill.SkillLogic:CheckAutoReleaseAngerOrJointSkill(is_enemy)
    end
end

--修改剩余时间
function Modules.SplogicModule:SetRemainTime(var)
    local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
    local config_gamelevel = CPPGameLib.GetConfig("GameLevel", battle_info.gamelevel_id)

    self.__remain_time = var

    --战斗开始多久    
    local pass_time = config_gamelevel.time - self.__remain_time
    --战役开始多久
    local battle_pass_time = pass_time - (self.__battle_pass_time_beg or 0)

    self:FireNextFrame(Macros.Event.BattleModule.BATTLE_TIME_STEP, self.__remain_time, pass_time, battle_pass_time)      
end

--停止BGM
function Modules.SplogicModule:StopBGM()
    if CallModuleFunc(ModuleType.BATTLE, "IsInQuickBattle") then
        return
    end    

    if not self.__stop_bgm then
        GlobalSoundMgr:StopBGM(true)
        self.__stop_bgm = true
    end
end

--是否自动战斗
function Modules.SplogicModule:IsAutoFight()
    return CallModuleFunc(ModuleType.BATTLE, "GetAutoFight")
end

--是否所有英雄阵亡
function Modules.SplogicModule:IsAllHeroDead()
end

--是否所有怪物阵亡
function Modules.SplogicModule:IsAllMonsterDead()
end

--处理进入切场景状态
function Modules.SplogicModule:HandleEnterSceneChangeState(callback)
    if callback then callback() end
end

--处理离开切场景状态
function Modules.SplogicModule:HandleQuitSceneChangeState(callback)
    if callback then callback() end
end

--处理场景进入通知(用于多人同屏)
function Modules.SplogicModule:HandleEnterSceneNotice(protocol_data)
end

--处理场景离开通知(用于多人同屏)
function Modules.SplogicModule:HandleLeaveSceneNotice(protocol_data)
end

--处理英雄行走通知(用于多人同屏)
function Modules.SplogicModule:HandleHeroGotoNotice(protocol_data)
end

--处理怪物数据通知
function Modules.SplogicModule:HandleMonsterDataNotice(protocol_data)
end

--处理怪物新增通知
function Modules.SplogicModule:HandleMonsterAddNotice(monster_info)
end

--处理采集物新增通知
function Modules.SplogicModule:HandleItemAddNotice(item_info)
end

--处理采集物锁定响应
function Modules.SplogicModule:HandleItemLockResp(protocol_data)
end

--处理采集物锁定通知
function Modules.SplogicModule:HandleItemLockNotify(protocol_data)
end

--处理采集物拾取响应
function Modules.SplogicModule:HandleItemPickupResp(protocol_data)
end

--处理采集物拾取通知
function Modules.SplogicModule:HandleItemPickupNotify(protocol_data)
end

--处理采集物消失响应
function Modules.SplogicModule:HandleItemDisappearNotice(protocol_data)
end

--处理组队副本任务奖励界面关闭
function Modules.SplogicModule:HandleTeamDungeonTaskRewardViewClosed()
end

--添加待攻击近战碰撞对记录
function Modules.SplogicModule:AddToAttackPNObjsRecord(obj_id, attacker_obj_id, skill_id, skill_lev)
end

--移除待攻击近战碰撞对记录
function Modules.SplogicModule:DelToAttackPNObjsRecord(obj_id, force)
end

--当指引和剧情时，是否暂停
function Modules.SplogicModule:IsPauseWhenLeadOrStory()
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function Modules.SplogicModule:__CheckACC(limit_factor)
    if not limit_factor then
        local acc_list = ConfigAdapter.Common.GetBattleACCList()
        limit_factor = acc_list[#acc_list]
    end

    local is_skipping = CallModuleFunc(ModuleType.BATTLE, "IsSkipping")
    local factor = CPPSceneManager:GetDeltaSpeedTime()
    if not is_skipping and factor - limit_factor >= 0.1 then
        CPPSceneManager:SetDeltaSpeedTime(1)
        GlobalNetAdapter:Disconnect(nil, CPPGameLib.GetString("common_acc_warnning"))   
    end
end
