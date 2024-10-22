
--[[
%% @module: AI管理器
%% @author: swordman sue
%% @created: 2016-07-13
%% @description: 创建管理AI对象
--]]

AI = AI or {}

AI.AIManager = AI.AIManager or BaseClass(EventAndTimerWrapper)

local get_ai_key

function AI.AIManager.CreateInstance()
    if not GlobalAIMgr then
        GlobalAIMgr = AI.AIManager.New()
    end
end

function AI.AIManager.DelInstance()
    if GlobalAIMgr then
        GlobalAIMgr:DeleteMe()
        GlobalAIMgr = nil
    end
end

function AI.AIManager:__init()
    self.__ai_key = 0
    self.__ai_map = {}
    self.__delete_list = List.New()
    self.__is_pause = false

    GlobalRunner:AddRunObj(self, Runner.Priority.RP_AI_MGR)
end

function AI.AIManager:__delete()
    self:Dump()
    self.__ai_map = nil
    self.__delete_list = nil

    GlobalRunner:RemoveRunObj(self)
end

--根据类型创建AI
function AI.AIManager:CreateAI(params)
    local ai
    CPPGameLib.Switch(params.type)
    {
        [AI.Type.HERO] = function()
            ai = AI.HeroAI.New(params)
        end,
        [AI.Type.MONSTER] = function()
            ai = AI.MonsterAI.New(params)
        end,
        [AI.Type.SPIRIT] = function()
            ai = AI.SpiritAI.New(params)
        end,
        [AI.Type.HERO_PVP] = function()
            ai = AI.HeroPVPAI.New(params)
        end,
        [AI.Type.MT_MONSTER] = function()
            ai = AI.MtMonsterAI.New(params)
        end,
        [AI.Type.MT_SPIRIT] = function()
            ai = AI.MtSpiritAI.New(params)
        end,
    }

    if ai and ai:IsInitSuccess() then
        local key = get_ai_key(self)
        self.__ai_map[key] = ai
        return key
    end    
end

function AI.AIManager:GetAI(key)
    if not key then
        return
    end
    return self.__ai_map[key]
end

function AI.AIManager:GetAIByObjId(obj_id)
    for k, v in pairs(self.__ai_map) do
        if v:GetObjID() == obj_id then
            return v
        end
    end
end

function AI.AIManager:DeleteAI(key)
    local ai = self.__ai_map[key]
    if ai ~= nil then
        ai:StopAI()
        List.PushBack(self.__delete_list, key)
    end
end

function AI.AIManager:Update(now_time, elapse_time)    
    if self.__is_pause then
        return
    end

    if self.__is_pause_screen then
        return
    end

    if CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsAllHeroDead") or 
        CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsAllMonsterDead") then
        return
    end

    --先处理删除列表
    while not List.Empty(self.__delete_list) do
        local key = List.GetFront(self.__delete_list)
        local ai = self.__ai_map[key]
        if ai then
            ai:DeleteMe()
            self.__ai_map[key] = nil
        end
        List.PopFront(self.__delete_list)
    end

    for k, ai in pairs(self.__ai_map) do
        ai:Update(now_time, elapse_time)
    end
end

--暂停
function AI.AIManager:Pause()
    if self.__is_pause then
        return
    end
    self.__is_pause = true
    
    --将管理器和单个对象的暂停，区分开来
    -- self:__StopAllAI()
end

--恢复
function AI.AIManager:Resume()
    if not self.__is_pause then
        return
    end
    self.__is_pause = false

    --将管理器和单个对象的恢复，区分开来
    -- self:__StartAllAI()
end

--怒气/合击技能暂停相关
function AI.AIManager:PauseByScreen()
    if self.__is_pause_screen then
        return
    end
    self.__is_pause_screen = true
end

--怒气/合击技能恢复相关
function AI.AIManager:ResumeByScreen()
    if not self.__is_pause_screen then
        return
    end
    self.__is_pause_screen = false
end

--暂停某AI
function AI.AIManager:PauseAIByKey(key)
    local ai = self.__ai_map[key]
    if ai then
        ai:StopAI()
    end
end

--恢复某AI
function AI.AIManager:ResumeAIByKey(key)
    local ai = self.__ai_map[key]
    if ai then
        ai:StartAI()
    end
end

function AI.AIManager:RegistAllEvents()
    --对象血量变化
    self:BindGlobalEvent(Macros.Event.SceneObj.OBJ_HP_CHANGE, function(_, obj_id, cur_hp, change_hp)
        local obj = GlobalScene:GetObj(obj_id)
        if not obj or not obj:IsMonster() then
            return
        end
        local obj_vo = obj:GetVO()

        for k, ai in pairs(self.__ai_map) do
            if ai:GetType() == AI.Type.MONSTER then
                ai:HpChange(obj_vo.info_id, (cur_hp - change_hp) / obj_vo.max_hp * 100, cur_hp / obj_vo.max_hp * 100)
            end
        end
    end)    

    --战斗倒计时
    self:BindGlobalEvent(Macros.Event.BattleModule.BATTLE_TIME_STEP, function(_, _, _, pass_time)
        for k, ai in pairs(self.__ai_map) do
            if ai:GetType() == AI.Type.MONSTER then
                ai:BattleTimeStep(pass_time - 1, pass_time)
            end
        end        
    end)

    --英雄死亡
    self:BindGlobalEvent(Macros.Event.SceneObj.HERO_DEAD, function(_, obj_id)
        local obj = GlobalScene:GetObj(obj_id)
        if not obj then
            return
        end
        local obj_vo = obj:GetVO()

        for k, ai in pairs(self.__ai_map) do
            if ai:GetType() == AI.Type.MONSTER then
                ai:HeroDead(obj_vo.info_id)
            end
        end            
    end)
    
    --怪物死亡
    self:BindGlobalEvent(Macros.Event.SceneObj.MONSTER_DEAD, function(_, obj_id)
        local obj = GlobalScene:GetObj(obj_id)
        if not obj then
            return
        end
        local obj_vo = obj:GetVO()

        for k, ai in pairs(self.__ai_map) do
            if ai:GetType() == AI.Type.MONSTER then
                ai:MonsterDead(obj_vo.info_id)
            end
        end            
    end)

    --怒气变化
    self:BindGlobalEvent(Macros.Event.SceneObj.COMMON_ANGER_CHANGE_OF_ENEMY, function(_, old_anger, new_anger)
        for k, ai in pairs(self.__ai_map) do
            if ai:GetType() == AI.Type.MONSTER then
                ai:AngerChange(old_anger, new_anger)
            end
        end            
    end)

    --任意单位释放技能
    self:BindGlobalEvent(Macros.Event.SceneObj.OBJ_SKILL_RELEASE, function(_, skill_id)
        for k, ai in pairs(self.__ai_map) do
            if ai:GetType() == AI.Type.MONSTER then
                ai:ObjReleaseSkill(skill_id)
            end
        end            
    end)

    --怪物出生
    self:BindGlobalEvent(Macros.Event.SceneObj.MONSTER_DO_BIRTH, function(_, obj_id)
        local obj = GlobalScene:GetObj(obj_id)
        if not obj then
            return
        end
        local obj_vo = obj:GetVO()

        for k, ai in pairs(self.__ai_map) do
            if ai:GetType() == AI.Type.MONSTER then
                ai:MonsterDoBirth(obj_vo.info_id)
            end
        end            
    end)
end

--[[
切场景相关销毁
--]]
function AI.AIManager:Dump()
    self.__is_pause = false
    self.__is_pause_screen = false
    
    self.__delete_list = List.New()

    for k, v in pairs(self.__ai_map) do
        v:DeleteMe()
    end
    self.__ai_map = {}
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function AI.AIManager:__StartAllAI()
    for _, v in pairs(self.__ai_map) do
        v:StartAI()
    end
end

function AI.AIManager:__StopAllAI()
    for _, v in pairs(self.__ai_map) do
        v:StopAI()
    end
end

--------------------------------------------------------------------------------------------------------------------------------
--私有函数
--------------------------------------------------------------------------------------------------------------------------------
get_ai_key = function(obj_self)
    obj_self.__ai_key = obj_self.__ai_key + 1
    return obj_self.__ai_key
end


