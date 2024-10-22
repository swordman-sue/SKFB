
--[[
%% @module: 场景对象状态机
%% @author: swordman sue
%% @created: 2016-07-13
--]]

Scene = Scene or {}

Scene.Obj = Scene.Obj or BaseClass()

--对象移动结果
Scene.Obj.DoMoveRet =
{
    INVALID = 0,    --无法移动
    NEAR = 1,       --起止点过近，不需移动
    NORMAL = 2,     --可移动
}

function Scene.Obj:__InitStateMachine()
	self.__state_machine = CPPGameStateMachine()
	self.__state_machine:CreateSink("main")

	--空状态
	local function null_enter_func(state_name)
		self["__EnterStateNull"](self, state_name)
	end
	local function null_update_func(state_name, elapse_time)
		self["__UpdateStateNull"](self, state_name, elapse_time)
	end
	local function null_quit_func(state_name)
		self["__QuitStateNull"](self, state_name)
	end
	local tmp_state = self.__state_machine:CreateNormalState("null", "main")
	tmp_state:SetCallbackAsMultiFuctions(
		GlobalCallbackMgr:AddCallBackFunc(null_enter_func), 
		GlobalCallbackMgr:AddCallBackFunc(null_update_func), 
		GlobalCallbackMgr:AddCallBackFunc(null_quit_func))

	--待机状态
	local function idle_enter_func(state_name)
		self["__EnterStateIdle"](self, state_name)
	end
	local function idle_update_func(state_name, elapse_time)
		self["__UpdateStateIdle"](self, state_name, elapse_time)
	end
	local function idle_quit_func(state_name)
		self["__QuitStateIdle"](self, state_name)
	end
	tmp_state = self.__state_machine:CreateNormalState("idle", "main")
	tmp_state:SetCallbackAsMultiFuctions(
		GlobalCallbackMgr:AddCallBackFunc(idle_enter_func), 
		GlobalCallbackMgr:AddCallBackFunc(idle_update_func), 
		GlobalCallbackMgr:AddCallBackFunc(idle_quit_func))
    tmp_state:SetStateCanReEnter(true)

	--移动状态
	local function move_enter_func(state_name)
		self["__EnterStateMove"](self, state_name)
	end
	local function move_update_func(state_name, elapse_time)
		self["__UpdateStateMove"](self, state_name, elapse_time)
	end
	local function move_quit_func(state_name)
		self["__QuitStateMove"](self, state_name)
	end
	tmp_state = self.__state_machine:CreateNormalState("move", "main")
	tmp_state:SetCallbackAsMultiFuctions(
		GlobalCallbackMgr:AddCallBackFunc(move_enter_func), 
		GlobalCallbackMgr:AddCallBackFunc(move_update_func), 
		GlobalCallbackMgr:AddCallBackFunc(move_quit_func))
	tmp_state:SetStateCanReEnter(true)

    --特殊移动状态
    self.__s_sp_move_state = {}
    local function sp_move_enter_func(state_name)
        self["__EnterStateSpecialMove"](self, state_name)
    end
    local function sp_move_update_func(state_name, elapse_time)
        self["__UpdateStateSpecialMove"](self, state_name, elapse_time)
    end
    local function sp_move_quit_func(state_name)
        self["__QuitStateSpecialMove"](self, state_name)
    end
    tmp_state = self.__state_machine:CreateNormalState("sp_move", "main")
    tmp_state:SetCallbackAsMultiFuctions(
        GlobalCallbackMgr:AddCallBackFunc(sp_move_enter_func), 
        GlobalCallbackMgr:AddCallBackFunc(sp_move_update_func), 
        GlobalCallbackMgr:AddCallBackFunc(sp_move_quit_func))
    tmp_state:SetStateCanReEnter(true)

    --死亡状态
    self.__s_dead_state = {}
    --进入死亡状态标记
    self.__is_dead = false
    local function dead_enter_func(state_name)
        self["__EnterStateDead"](self, state_name)
    end
    local function dead_update_func(state_name, elapse_time)
        self["__UpdateStateDead"](self, state_name, elapse_time)
    end
    local function dead_quit_func(state_name)
        self["__QuitStateDead"](self, state_name)
    end
    tmp_state = self.__state_machine:CreateNormalState("dead", "main")
    tmp_state:SetCallbackAsMultiFuctions(
        GlobalCallbackMgr:AddCallBackFunc(dead_enter_func), 
        GlobalCallbackMgr:AddCallBackFunc(dead_update_func), 
        GlobalCallbackMgr:AddCallBackFunc(dead_quit_func))                  
end

function Scene.Obj:__DeleteStateMachine()
    self.__state_machine = nil
end

function Scene.Obj:IsInState(state_name)
	if not self.__state_machine then
		return false
	end

	return self.__state_machine:IsInState(state_name)
end

function Scene.Obj:GetRuningState()
    if not self.__state_machine then
        return "invalid"
    end

    return self.__state_machine:GetSinkRunState("main")
end

--------------------------------------------------------------------------------------------------------------------------------
--空状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.Obj:CanDoNull()
    if self:IsDead() then
        return false
    end

    if self:IsInState("sp_move") then
        return false
    end

    return true
end

function Scene.Obj:DoNull(last_time, is_change_state, force)
    if not force and not self:CanDoNull() then
        return
    end

    self.__s_null_state = self.__s_null_state or {}
    self.__s_null_state.pass_time = 0
    self.__s_null_state.last_time = last_time or 0.5
    self.__s_null_state.change_state = is_change_state
    self:ChangeState("null", true)
end

function Scene.Obj:__EnterStateNull(state_name)
    self:PlayAnimation(RenderObj.ActionIndex.IDLE)    
end

function Scene.Obj:__UpdateStateNull(state_name, elapse_time)
    self.__s_null_state.pass_time = self.__s_null_state.pass_time + elapse_time
    
    if self.__s_null_state.change_state ~= false then
        if self.__s_null_state.pass_time >= self.__s_null_state.last_time then
            self:ChangeState("idle", true)
        end
    end
end

function Scene.Obj:__QuitStateNull(state_name)
    self.__s_null_state = nil
end

--------------------------------------------------------------------------------------------------------------------------------
--待机状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.Obj:CanDoIdle()
    if self:IsDead() then
        return false
    end    

    if self:IsInState("sp_move") then
        return false
    end

    return true
end

function Scene.Obj:DoIdle(force)
    if not force and not self:CanDoIdle() then
        return false
    end
    self:ChangeState("idle", true)
    return true
end

function Scene.Obj:__EnterStateIdle(state_name)
    self:PlayAnimation(RenderObj.ActionIndex.IDLE)
end

function Scene.Obj:__UpdateStateIdle(state_name, elapse_time)
end

function Scene.Obj:__QuitStateIdle(state_name)
end

--------------------------------------------------------------------------------------------------------------------------------
--移动状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.Obj:SetForbidMove(var)
    local delta = var and 1 or -1
    self.__forbid_move = self.__forbid_move or 0
    self.__forbid_move = self.__forbid_move + delta
end

function Scene.Obj:IsForbidMove()
    return self.__forbid_move and self.__forbid_move > 0
end

function Scene.Obj:CanDoMove()
    if self:IsDead() then
        return false
    end

    if not self:IsInState("null") and
        not self:IsInState("idle") and
        not self:IsInState("move") then
        return false
    end

    if self.__forbid_move and self.__forbid_move > 0 then
        return false
    end

    if self.__vo.immune_ctrl_skill or self.__vo.immune_ctrl_skill2 then
        return false
    end

    return true
end

function Scene.Obj:DoMove(end_pos, is_world_pos, speed)
    if not end_pos then
        return
    end
    
    if not self:CanDoMove() then
        return Scene.Obj.DoMoveRet.INVALID
    end

    --坐标转换
    if not is_world_pos then
        end_pos = GlobalScene:LogicToWorldVec(end_pos)
    end

    --边界处理
    CallModuleFunc(ModuleType.SP_LOGIC_MGR, "BoundTest", end_pos, true, self.__vo.obj_id)

    self.__s_move_state = self.__s_move_state or {}

    local next_dir
    next_dir, self.__s_move_state.total_distance, self.__s_move_state.dir_vec = self:__CalcMoveInfo(nil, end_pos)

    --起止点过近
    if self.__world_pos:IsNear(end_pos, 16) then
        self:SetWorldPos(end_pos.x, end_pos.y)
        return Scene.Obj.DoMoveRet.NEAR
    end

    --计算移动信息
    self.__s_move_state.pass_time = 0
    self.__s_move_state.pass_distance = 0
    self.__s_move_state.beg_pos = Vector2D.New(self.__world_pos.x, self.__world_pos.y)
    self.__s_move_state.end_pos = end_pos
    self.__s_move_state.speed = speed or self:GetMoveSpeed()

    if not self:IsInState("move") then
        Scene.Obj.SetDir(self, next_dir)
        self:ChangeState("move", true)
    else                
        --改变移动方向
        self:SetDir(next_dir)
    end

    return Scene.Obj.DoMoveRet.NORMAL
end

function Scene.Obj:__EnterStateMove(state_name)
	self:PlayAnimation(RenderObj.ActionIndex.RUN)
end

function Scene.Obj:__UpdateStateMove(state_name, elapse_time)
    self.__s_move_state.pass_time = self.__s_move_state.pass_time + elapse_time
    self.__s_move_state.pass_distance = self.__s_move_state.pass_time * self.__s_move_state.speed

	if self.__s_move_state.pass_distance >= self.__s_move_state.total_distance then
        self:SetWorldPos(self.__s_move_state.end_pos.x, self.__s_move_state.end_pos.y)
        self:__QuitStateMoveHelper()
	else
		local mov_dis = self.__s_move_state.dir_vec:Multi(self.__s_move_state.pass_distance)
		local now_pos = self.__s_move_state.beg_pos + mov_dis
		self:SetWorldPos(now_pos.x, now_pos.y)
	end
end

function Scene.Obj:__QuitStateMove(state_name)
    self.__s_move_state = nil
end

function Scene.Obj:__QuitStateMoveHelper()
    self:DoIdle()
end

--根据起止点，计算移动信息
function Scene.Obj:__CalcMoveInfo(beg_pos, end_pos)
    if not beg_pos then
        beg_pos = Vector2D.New(self.__world_pos.x, self.__world_pos.y)
    end

    --计算移动基本信息
    local move_vec = end_pos - beg_pos
    local distance = move_vec:Length()
    move_vec:Normalize(distance)

    if move_vec.x == 0 then
        dir = self.__dir
    else
        dir = move_vec.x > 0 and MathTool.HORZ_RIGHT or MathTool.HORZ_LEFT
    end 
    return dir, distance, move_vec
end

--------------------------------------------------------------------------------------------------------------------------------
--特殊移动状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.Obj:CanDoSpecialMove()
    if self:IsDead() then
        return false
    end

    return true
end

--[[
    注：end_callback中，不允许切换本对象的状态
--]]
function Scene.Obj:DoSpecialMove(end_pos, is_world_pos, speed, uninterruptable, end_callback)
    self:PauseAI()
    self:SetSkipBoundTest(true)

    self.__s_sp_move_state = self.__s_sp_move_state or {}
    self.__s_sp_move_state.uninterruptable = uninterruptable
    self.__s_sp_move_state.end_callback = end_callback
    self.__s_sp_move_state.is_finished = false

    if not end_pos then
        self:__QuitStateSpecialMoveHelper()
        return
    end
    
    if not self:CanDoSpecialMove() then
        self:__QuitStateSpecialMoveHelper()
        return Scene.Obj.DoMoveRet.INVALID
    end

    --坐标转换
    if not is_world_pos then
        end_pos = GlobalScene:LogicToWorldVec(end_pos)
    end

    --起止点过近
    if self.__world_pos:IsNear(end_pos, 16) then
        self.__s_sp_move_state.is_finished = true
        self:SetWorldPos(end_pos.x, end_pos.y)
        if not self:IsInState("sp_move") then
            self:__QuitStateSpecialMoveHelper()
        else
            self:__QuitStateSpecialMoveHelper(true)            
        end
        return Scene.Obj.DoMoveRet.NEAR
    end

    --边界处理
    CallModuleFunc(ModuleType.SP_LOGIC_MGR, "BoundTest", end_pos, true, self.__vo.obj_id)

    --计算移动信息
    local next_dir
    next_dir, self.__s_sp_move_state.total_distance, self.__s_sp_move_state.dir_vec = self:__CalcMoveInfo(nil, end_pos)

    self.__s_sp_move_state.pass_time = 0
    self.__s_sp_move_state.pass_distance = 0
    self.__s_sp_move_state.beg_pos = Vector2D.New(self.__world_pos.x, self.__world_pos.y)
    self.__s_sp_move_state.end_pos = end_pos
    self.__s_sp_move_state.speed = speed or self:GetMoveSpeed()

    if not self:IsInState("sp_move") then
        Scene.Obj.SetDir(self, next_dir)
        self:ChangeState("sp_move", true)
    else        
        --改变移动方向
        self:SetDir(next_dir)
    end

    return Scene.Obj.DoMoveRet.NORMAL
end

function Scene.Obj:__EnterStateSpecialMove(state_name)
    self:PlayAnimation(RenderObj.ActionIndex.RUN)
end

function Scene.Obj:__UpdateStateSpecialMove(state_name, elapse_time)  
    self.__s_sp_move_state.pass_time = self.__s_sp_move_state.pass_time + elapse_time
    self.__s_sp_move_state.pass_distance = self.__s_sp_move_state.pass_time * self.__s_sp_move_state.speed

    if self.__s_sp_move_state.pass_distance >= self.__s_sp_move_state.total_distance then
        self:SetWorldPos(self.__s_sp_move_state.end_pos.x, self.__s_sp_move_state.end_pos.y)
        self.__s_sp_move_state.is_finished = true
        self:__QuitStateSpecialMoveHelper(true)
    else
        local mov_dis = self.__s_sp_move_state.dir_vec:Multi(self.__s_sp_move_state.pass_distance)
        local now_pos = self.__s_sp_move_state.beg_pos + mov_dis
        self:SetWorldPos(now_pos.x, now_pos.y)
    end
end

function Scene.Obj:__QuitStateSpecialMove(state_name)
    self:__QuitStateSpecialMoveHelper()
    self.__s_sp_move_state = nil
end

function Scene.Obj:__QuitStateSpecialMoveHelper(change_state)
    if not change_state then
        self:ResumeAI()
        self:SetSkipBoundTest(false)

        --执行结束回调
        if self.__s_sp_move_state.end_callback then
            self.__s_sp_move_state.end_callback(self.__vo.obj_id, self.__s_sp_move_state.is_finished)
            self.__s_sp_move_state.end_callback = nil
        end
    else
        self:DoNull(0.05, nil, true)
    end
end

--------------------------------------------------------------------------------------------------------------------------------
--死亡状态
--------------------------------------------------------------------------------------------------------------------------------
function Scene.Obj:IsDead()
    return self.__is_dead
end

function Scene.Obj:DoDead()
    if self.__is_dead then
        return
    end
    self.__is_dead = true

    self:ChangeState("dead", true)
end

function Scene.Obj:__EnterStateDead(state_name)
    --移除身上所有BUFF
    GlobalBuffMgr:DeleteBuffsByTargetID(self.__vo.obj_id)

    self.__s_dead_state = {}
    self.__s_dead_state.is_anim_end = false

    self.__body_anim_play_info.end_callback = function()
        if not self.__s_dead_state then
            return
        end
        self.__s_dead_state.is_anim_end = true
    end 
    self:PlayAnimation(RenderObj.ActionIndex.DEAD, false)
end

function Scene.Obj:__UpdateStateDead(state_name, elapse_time)
    if self.__s_dead_state.is_anim_end then
        self:__QuitStateDeadHelper()
    end
end

function Scene.Obj:__QuitStateDead(state_name)
    self.__body_anim_play_info.end_callback = nil
    self.__s_dead_state = nil
end

function Scene.Obj:__QuitStateDeadHelper()
    GlobalScene:DeleteObj(self.__vo.obj_id)
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function Scene.Obj:ChangeState(state_name, force)
    if self.__state_machine == nil then
        return
    end

    if not force then
        self.__state_machine:ChangeState(state_name, false)
    else
        self.__state_machine:ChangeStateForce(state_name)
    end
end

