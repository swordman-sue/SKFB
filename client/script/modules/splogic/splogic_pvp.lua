
--[[
%% @module: 1V1
%% @author: swordman sue
%% @created: 2017-6-15
--]]

Modules = Modules or {}

Modules.SpPVPModule = Modules.SpPVPModule or BaseClass(Modules.SpMainDungeonModule)

function Modules.SpPVPModule:__init()
end

function Modules.SpPVPModule:__delete()
end

--初始化
function Modules.SpPVPModule:Initialize()
	Modules.SpMainDungeonModule.Initialize(self)

    local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
    self.__is_pve = battle_info.is_pve
end

--释放
function Modules.SpPVPModule:Dispose()
	Modules.SpMainDungeonModule.Dispose(self)
end

function Modules.SpPVPModule:IsResLoadedEx()
    local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
    return battle_info and battle_info.is_started    
end

function Modules.SpPVPModule:ResLoadedNotify()
    CallModuleFunc(ModuleType.BATTLE, "ReadyFinishReq_PVP")
end

--更新剩余时间
function Modules.SpPVPModule:SetRemainTime(var)
    Modules.SplogicModule.SetRemainTime(self, var)
end

--请求战斗结束
function Modules.SpPVPModule:BattleEndReq(var)
    if self.__is_pve then        
        --打机器人中
        CallModuleFunc(ModuleType.BATTLE, "BattleEndReq_PVP", var)
    end
end

--打开胜利界面
function Modules.SpPVPModule:OpenBattleSuccessView()
    CallModuleFunc(ModuleType.PVP, "OpenView", BaseViewType.PVP_LOSE , "win" )
end

--打开平局界面
function Modules.SpPVPModule:OpenBattleTieView()
    CallModuleFunc(ModuleType.PVP, "OpenView", BaseViewType.PVP_LOSE , "tie" )
end

--打开失败界面
function Modules.SpPVPModule:OpenBattleFailedView()
    CallModuleFunc(ModuleType.PVP, "OpenView", BaseViewType.PVP_LOSE , "lose" )
end

--战斗结束，从结算界面返回对应界面
function Modules.SpPVPModule:BattleEndReturnRelevantView()	
    CallModuleFunc(ModuleType.PVP, "OpenView", BaseViewType.PVP)
    
	Modules.SplogicModule.BattleEndReturnRelevantView(self)
end

--触发怒气/合击技能
function Modules.SpPVPModule:TriggerAngerOrJointSkill(obj_id, is_plus)
    if self:IsInAngerOrJointSkill() then
        --怒气/合击技能释放中，过滤
        return
    end

    local obj = GlobalScene:GetObj(obj_id)
    if not obj or not obj:IsAngerEnough(is_plus) then
        --对象不存在、怒气不足，过滤
        return
    end

    local cd_info = obj:GetAI():GetAngerOrJointCommonCDInfo()
    if cd_info and cd_info.in_cd then
        --CD中，过滤
        return
    end

	local skill_id = Skill.SkillLogic:GetAngerOrJointSkillID(obj_id, is_plus)
	if not skill_id then
        --技能不存在，过滤
		return
	end

    --打机器人中
    if self.__is_pve then
        return obj:TriggerAngerOrJointSkill(skill_id, is_plus)
    end

    --技能释放请求
	CallModuleFunc(ModuleType.BATTLE, "UseSkillReq_PVP", obj_id, skill_id)
end

--触发精灵技能
function Modules.SpPVPModule:TriggerSpriteSkill(obj_id, skill_id, world_pos)
    if self.__is_pve then
        --打机器人中
        Modules.SpMainDungeonModule.TriggerSpriteSkill(self, obj_id, skill_id, world_pos)
        return
    end

    CallModuleFunc(ModuleType.BATTLE, "UseSkillReq_PVP", obj_id, skill_id, world_pos.x, world_pos.y)    
end

--血量处理
function Modules.SpPVPModule:DoHpChange(obj_id, change_hp, damage_type, frm_protocol)
    if self.__is_pve then
        --打机器人中
        Modules.SpMainDungeonModule.DoHpChange(self, obj_id, change_hp, damage_type, frm_protocol)
        return
    end    

    local obj = GlobalScene:GetObj(obj_id)
    if not obj then
        return
    end

    damage_type = damage_type or Modules.BattleFormula.DamageType.NORMAL    

    local obj_vo = obj:GetVO()    
    local old_hp = obj_vo.hp
    if frm_protocol then
        --更新血量
        obj:SetHp(obj_vo.hp + change_hp)

        if change_hp > 0 then
            --检测精灵祝福BUFF
            Skill.SkillLogic:CheckSpriteBlessBuff(Macros.Game.SpiritBlessSkillTriggerMode.BE_TREATED, nil, obj_vo)        
        end

        if old_hp ~= obj_vo.hp then
            change_hp = obj_vo.hp - old_hp
            
            --派发血量改变事件
            GlobalEventSystem:FireNextFrame(Macros.Event.SceneObj.OBJ_HP_CHANGE, obj_id, obj_vo.hp, change_hp)    
        end

        if old_hp > 0 and obj_vo.hp <= 0 then
            --对象零血通知
            self:ObjDeadNotify(obj_id)
        end            
    else
        --飘血特效
        obj:ShowHpEffect(change_hp, damage_type)
    end
end

--血量变化同步请求
function Modules.SpPVPModule:HPChangeSyncReq(type, src_obj_id, tgt_obj_id, change_hp, damage_type)
    if self.__is_pve then
        --打机器人中
        return
    end

    local src_obj = GlobalScene:GetObj(src_obj_id)
    local tgt_obj = GlobalScene:GetObj(tgt_obj_id)

    if type == Modules.BattleFormula.HPChangeSrc.SKILL then
        if not src_obj or not src_obj:IsHeroPVP(true) then
            --只同步己方英雄技能造成的血量变化
            return
        end

    elseif Modules.BattleFormula.HPChangeSrc.BUFF then
        if not tgt_obj or not tgt_obj:IsHeroPVP(true) then
            --只同步己方英雄持有BUFF造成的血量变化
            return
        end
    end

    CallModuleFunc(ModuleType.BATTLE, "HPChangeSyncReq_PVP", src_obj_id, tgt_obj_id, change_hp, damage_type)
end

--获取己方精灵
function Modules.SpPVPModule:GetTeammateSprite()
    local sprite
    local spirit_vo_map = GlobalVOMgr:GetSpiritMap()
    if spirit_vo_map then
        for _, vo in pairs(spirit_vo_map) do
            sprite = GlobalScene:GetObj(vo.obj_id)
            if sprite and sprite:CheckBindObjType(Scene.ObjType.HERO_PVP) then
                return sprite
            end
        end
    end
end

function Modules.SpPVPModule:Update(now_time, elapse_time)
    Modules.SpMainDungeonModule.Update(self, now_time, elapse_time)

    self:__CheckACC(ConfigAdapter.Common.GetPVPACCFactor())
    self:__CheckEndReal(true)
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--获取战斗英雄列表
function Modules.SpPVPModule:__GetBattleHeroList()
    if not self.__battle_hero_list then
        local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
        if battle_info.red_role_id == CallModuleFunc(ModuleType.ROLE, "GetRoleID") then
            self.__battle_hero_list = CallModuleFunc(ModuleType.BATTLE, "GetRedHeroList_PVP")
        else
            self.__battle_hero_list = CallModuleFunc(ModuleType.BATTLE, "GetBlueHeroList_PVP")
        end
    end
    return self.__battle_hero_list
end

--获取战斗怪物列表
function Modules.SpPVPModule:__GetBattleMonsterList()
    if not self.__battle_monster_list then
        local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
        if battle_info.red_role_id == CallModuleFunc(ModuleType.ROLE, "GetRoleID") then
            self.__battle_monster_list = CallModuleFunc(ModuleType.BATTLE, "GetBlueHeroList_PVP")
        else
            self.__battle_monster_list = CallModuleFunc(ModuleType.BATTLE, "GetRedHeroList_PVP")
        end
    end
    return self.__battle_monster_list
end

--获取战斗英雄镜像列表
function Modules.SpPVPModule:__GetBattleHeroMirrorList()
end

--创建英雄
function Modules.SpPVPModule:__CreateHeros()
    Modules.SpMainDungeonModule.__CreateHeros(self, Scene.HeroPVPVO, function(vo, turnon_ai)
        return GlobalScene:CreateHeroPVP(vo, turnon_ai)
    end)
end

--创建怪物
function Modules.SpPVPModule:__CreateMonsters()
    Modules.SpMainDungeonModule.__CreateMonsters(self, Scene.MonsterPVPVO, function(vo, turnon_ai)
        return GlobalScene:CreateMonsterPVP(vo, turnon_ai)
    end)
end

--创建精灵
function Modules.SpPVPModule:__CreateSprites()
    local hero_birth_pos_y = CallModuleFunc(ModuleType.BATTLE, "GetBirthPosY", Scene.ObjType.HERO)      
    local region_bound = CallModuleFunc(ModuleType.BATTLE, "GetBattleRegion")

    local role_id = CallModuleFunc(ModuleType.ROLE, "GetRoleID")
    local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
    local battle_spirit_info, vo, sprite
    
    --红方精灵
    local red_sprite_info = CallModuleFunc(ModuleType.BATTLE, "GetRedSpriteInfo_PVP")
    if red_sprite_info then
        local red_spirit_birth_pos = Vector2D.New(region_bound.org.x + ConfigAdapter.Common.GetBattleBirthPosBoundaryMargin(), hero_birth_pos_y + Scene.SpiritOffset.y)
    
        battle_spirit_info = Modules.BattleFormula:BuildBattleSpiritInfo(red_sprite_info)
        vo = Scene.SpiritVO.New()
        vo.obj_id = red_sprite_info.obj_id
        vo:ParseInfo(battle_spirit_info)
        vo:ParseSkillList(red_sprite_info.skill_id)
        vo.org_pos_x, vo.org_pos_y = GlobalScene:WorldToLogic(red_spirit_birth_pos.x, red_spirit_birth_pos.y)
        sprite = GlobalScene:CreateSpirit(vo, true, role_id == battle_info.red_role_id and Scene.ObjType.HERO_PVP or Scene.ObjType.MONSTER_PVP)
        sprite:SetVisible(false)
    end

    --蓝方精灵
    local blue_sprite_info = CallModuleFunc(ModuleType.BATTLE, "GetBlueSpriteInfo_PVP")
    if blue_sprite_info then
        local blue_spirit_birth_pos = Vector2D.New(region_bound.org.x + region_bound.size.width - 1 - ConfigAdapter.Common.GetBattleBirthPosBoundaryMargin(), hero_birth_pos_y + Scene.SpiritOffset.y)

        battle_spirit_info = Modules.BattleFormula:BuildBattleSpiritInfo(blue_sprite_info)
        vo = Scene.SpiritVO.New()
        vo.obj_id = blue_sprite_info.obj_id
        vo:ParseInfo(battle_spirit_info)
        vo:ParseSkillList(blue_sprite_info.skill_id)
        vo.org_pos_x, vo.org_pos_y = GlobalScene:WorldToLogic(blue_spirit_birth_pos.x, blue_spirit_birth_pos.y)
        sprite = GlobalScene:CreateSpirit(vo, true, role_id == battle_info.blue_role_id and Scene.ObjType.HERO_PVP or Scene.ObjType.MONSTER_PVP)
        sprite:SetVisible(false)
    end
end

--精灵出场
function Modules.SpPVPModule:__DoBirthForSprite()
    local spirit_birth_pos
    local region_bound = CallModuleFunc(ModuleType.BATTLE, "GetBattleRegion")

    local sprite
    local spirit_vo_map = GlobalVOMgr:GetSpiritMap()
    if spirit_vo_map then
        for _, vo in pairs(spirit_vo_map) do
            sprite = GlobalScene:GetObj(vo.obj_id)
            sprite:SetVisible(true)
            sprite:SetSkipBoundTest(true)
            if sprite:CheckBindObjType(Scene.ObjType.HERO_PVP) then
                spirit_birth_pos = Vector2D.New(region_bound.org.x + ConfigAdapter.Common.GetBattleBirthPosBoundaryMargin(), sprite:GetWorldPosY())
                sprite:SetWorldPos(spirit_birth_pos.x + 156, spirit_birth_pos.y)
            else
                spirit_birth_pos = Vector2D.New(region_bound.org.x + region_bound.size.width - 1 - ConfigAdapter.Common.GetBattleBirthPosBoundaryMargin(), sprite:GetWorldPosY())
                sprite:SetWorldPos(spirit_birth_pos.x - 156, spirit_birth_pos.y)
            end
            sprite:DoBirth()
        end
    end
end

--检测所有英雄死亡
function Modules.SpPVPModule:__CheckAllHeroDead()
    if self.__is_pve then
        --打机器人中        
        Modules.SpMainDungeonModule.__CheckAllHeroDead(self)
        return
    end    

    CallModuleFunc(ModuleType.BATTLE, "ClearUseSkillLock_PVP", obj_id)
end

--检测所有怪物死亡
function Modules.SpPVPModule:__CheckAllMonsterDead()
    if self.__is_pve then
        --打机器人中        
        Modules.SpMainDungeonModule.__CheckAllMonsterDead(self)
        return
    end    
end

--检测掉落物
function Modules.SpPVPModule:__CheckDrop(obj_id)
end

--检测结束
function Modules.SpPVPModule:__CheckEnd()
    if self.__is_pve then
        --打机器人中        
        Modules.SpMainDungeonModule.__CheckEnd(self)
    end
end

--检测结束
function Modules.SpPVPModule:__CheckEndReal(for_check)
    if not self.__is_pve then
        return
    end    

    if for_check then
        if self.__check_end_real_time and Macros.Global.NowTime >= self.__check_end_real_time then
            self.__check_end_real_time = nil

            local have_hero = GlobalVOMgr:CalcTotalHeroHP() > 0
            local have_monster = GlobalVOMgr:CalcTotalMonsterHP() > 0
            if not have_hero and not have_monster then
                --平局
                self:BattleEndReq(Modules.BattleModule.ResultType.TIE)
            elseif not have_hero then
                --失败
                self:BattleEndReq(Modules.BattleModule.ResultType.LOSE)
            else
                --胜利
                self:BattleEndReq(Modules.BattleModule.ResultType.WIN)
            end
        end

    elseif not self.__check_end_real_time then
        local have_hero = GlobalVOMgr:CalcTotalHeroHP() > 0
        local have_monster = GlobalVOMgr:CalcTotalMonsterHP() > 0
        if not have_hero or not have_monster then
            self.__is_checked_end = true
    
            --延时检测，处理同归于尽的情况
            self.__check_end_real_time = Macros.Global.NowTime
        end
    end
end

--检测结束
function Modules.SpPVPModule:__CheckEndDelay(elapse_time)
    if self.__is_pve then
        --打机器人中
        Modules.SpMainDungeonModule.__CheckEndDelay(self, elapse_time)
    end
end

--血量变化
function Modules.SpPVPModule:__ObjHPChange(obj_id, cur_hp, change_hp)
end

function Modules.SpPVPModule:__ObjAngerOrJointSkillReleased(obj_id)
    if not self.__is_pve then
        CallModuleFunc(ModuleType.BATTLE, "ClearUseSkillLock_PVP", obj_id)
    end
end

function Modules.SpPVPModule:__GetStartBattleEffect()
    return 100055
end

function Modules.SpPVPModule:__GetStartBattleEffectTime()
    return 1.6
end

function Modules.SpPVPModule:__IsPauseWhenDeActive()
    return false
end