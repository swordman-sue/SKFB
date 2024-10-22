
--[[
%% @module: 精灵AI(用于多人场景)
%% @author: swordman sue
%% @created: 2017-11-15
--]]

AI = AI or {}

AI.MtSpiritAI = AI.MtSpiritAI or BaseClass(AI.BaseAI)

function AI.MtSpiritAI:__init(params)
	self.__type = AI.Type.MT_SPIRIT
end

function AI.MtSpiritAI:__delete()
end

function AI.MtSpiritAI:Update(now_time, elapse_time)	    
    if not self.__is_run then
        return
    end

    local me = self:__GetMe()
    if not me or me:IsDead() then
        self:StopAI()
        return
    end

    if not me:IsInState("null") and
        not me:IsInState("idle") and 
        not me:IsInState("move") then
        return
    end

    --更新状态机
    self.__state_machine:Update(elapse_time)
end

--每场战役开启时的技能重置
function AI.MtSpiritAI:ResetSkillForBattleStart()
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--更新技能CD
function AI.MtSpiritAI:__UpdateSkillCD(elapse_time)
end

--选择技能
function AI.MtSpiritAI:__SelectSkill()
end

--释放技能
function AI.MtSpiritAI:__ReleaseSkill()
end

--根据技能搜索目标
function AI.MtSpiritAI:__SearchTarget()
end

--攻击检测
function AI.MtSpiritAI:__CheckToAttack()
	return false
end

--[[
	待机状态
--]]

function AI.MtSpiritAI:__UpdateStateIdle(state_name, elapse_time)
    self:__ChangeToChase()
end

--[[
	巡逻状态
--]]

function AI.MtSpiritAI:__EnterStateChase(state_name)
    self.__s_chase_state = {}
    self.__s_chase_state.pass_time = 0
    self.__s_chase_state.next_time = 0
end

function AI.MtSpiritAI:__UpdateStateChase(state_name, elapse_time)
    local me = self:__GetMe()
    local my_vo = self:__GetMyVO()

    local bind_obj_obj_id = me:GetBindObjID()
    local bind_obj = GlobalScene:GetObj(bind_obj_obj_id)
    if not bind_obj then
        return
    end
    local bind_obj_wpos = bind_obj:GetWorldPosVec()

    if not self.__s_chase_state.toggle then
        --检测坐标变化，判断是否进入跟随
        local my_world_pos_x, my_world_pos_y = me:GetWorldPos()
        if math.abs(my_world_pos_x - bind_obj_wpos.x) > my_vo.follow_space or math.abs(my_world_pos_y - (bind_obj_wpos.y + my_vo.offset_frm_hero.y)) > my_vo.follow_space then
            self.__s_chase_state.toggle = true
            self.__s_chase_state.pass_time = 0
            self.__s_chase_state.next_time = MathTool.GetRandom(0.2, 0.5)
        end
    else
        --检测是否到时间进入跟随
        self.__s_chase_state.pass_time = self.__s_chase_state.pass_time + elapse_time
        if self.__s_chase_state.pass_time >= self.__s_chase_state.next_time then
            self.__s_chase_state.toggle = false
            self.__s_chase_state.pass_time = 0
            local tgt_wpos = Vector2D.New(bind_obj_wpos.x + my_vo.offset_frm_hero.x * bind_obj:GetDir(), bind_obj_wpos.y + my_vo.offset_frm_hero.y)
            me:DoMove(tgt_wpos, true)
        end
    end
end

