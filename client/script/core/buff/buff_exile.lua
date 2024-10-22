
--[[
%% @module: 放逐BUFF
%% @author: swordman sue
%% @created: 2016-09-18
--]]

Buff.ExileBuff = Buff.ExileBuff or BaseClass(Buff.BaseBuff)

--创建回调
function Buff.ExileBuff:BuffCreated()
	Buff.BaseBuff.BuffCreated(self)

	local tgt = self:__GetTgtObj()
	if tgt then
		tgt:SetForbidMove(true)
		tgt:SetForbidAttack(true)
		tgt:SetForbidBeAttacked(true)

		tgt:PushOpacity(128)
		tgt:PushAnimTimeScale(0)

		tgt:DoNull(self.__total_time, nil, true)

		--计算目标点
		local src_world_pos = tgt:GetWorldPosVec()
		local battle_region = CallModuleFunc(ModuleType.BATTLE, "GetBattleRegion")
		if tgt:GetDir() == MathTool.HORZ_LEFT then
			self.__exile_dst_world_pos = Vector2D.New(battle_region.org.x + battle_region.size.width - 1, src_world_pos.y)
		else
			self.__exile_dst_world_pos = Vector2D.New(battle_region.org.x, src_world_pos.y)
		end
	    --边界处理
	    CallModuleFunc(ModuleType.SP_LOGIC_MGR, "BoundTest", self.__exile_dst_world_pos, true, self.__tgt_obj_id)

	    --临时目标点
	    local dst_world_pos = Vector2D.New(self.__exile_dst_world_pos.x, self.__exile_dst_world_pos.y - 150)

	    --计算时间
	    local move_time
	    local move_speed = 512
	    local delta_world_vec = dst_world_pos - src_world_pos
	    move_time = delta_world_vec:Length() / move_speed

	    --漂移动作
	    local model = tgt:GetModel()
	    local dst_pixel_pos = GlobalScene:WorldToPixelVec(dst_world_pos)
	    CPPActionManager.MoveTo(model:GetNode(), move_time, dst_pixel_pos.x, dst_pixel_pos.y)
	end
end

--移除回调
function Buff.ExileBuff:BuffRemoved()
	local tgt = self:__GetTgtObj()
	if tgt then
		tgt:SetForbidMove(false)
		tgt:SetForbidAttack(false)
		tgt:SetForbidBeAttacked(false)

		tgt:PopOpacity()
		tgt:PopAnimTimeScale()	

		tgt:SetWorldPos(self.__exile_dst_world_pos.x, self.__exile_dst_world_pos.y)
	end	

	Buff.BaseBuff.BuffRemoved(self)		
end
