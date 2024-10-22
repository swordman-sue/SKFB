
--[[
%% @module: 冰冻/石化BUFF
%% @author: swordman sue
%% @created: 2016-09-18
--]]

Buff.FrozenBuff = Buff.FrozenBuff or BaseClass(Buff.BaseBuff)

--创建回调
function Buff.FrozenBuff:BuffCreated()
	Buff.BaseBuff.BuffCreated(self)
		
	local tgt = self:__GetTgtObj()
	if tgt then
		tgt:SetForbidMove(true)
		tgt:SetForbidAttack(true)
		tgt:SetForbidRepel(true)
		tgt:SetForbidBeAttackedFloat(true)

		tgt:PushAnimTimeScale(0)

		if self.__buff_type == Buff.BuffType.FROZEN then
			--冰冻
			tgt:PushColor(Macros.Color.blue)
		else
			--石化
			tgt:SetGrayEx(true)
		end

		tgt:DoNull(self.__total_time, nil, true)	
	end
end

--移除回调
function Buff.FrozenBuff:BuffRemoved()
	local tgt = self:__GetTgtObj()
	if tgt then
		tgt:SetForbidMove(false)
		tgt:SetForbidAttack(false)
		tgt:SetForbidRepel(false)
		tgt:SetForbidBeAttackedFloat(false)

		tgt:PopAnimTimeScale()		

		if self.__buff_type == Buff.BuffType.FROZEN then
			--冰冻
			tgt:PopColor()
		else
			--石化
			tgt:SetGrayEx(false)
		end
	end

	Buff.BaseBuff.BuffRemoved(self)		
end
