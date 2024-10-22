
--[[
%% @module: 眩晕BUFF
%% @author: swordman sue
%% @created: 2016-09-18
--]]

Buff.VertigoBuff = Buff.VertigoBuff or BaseClass(Buff.BaseBuff)

--创建回调
function Buff.VertigoBuff:BuffCreated()
	Buff.BaseBuff.BuffCreated(self)
	
	local tgt = self:__GetTgtObj()
	if tgt then
		tgt:SetForbidMove(true)
		tgt:SetForbidAttack(true)

		if not tgt:IsInBeattackedFloat() then
			tgt:DoNull(self.__total_time, nil, true)
		end
	end
end

--移除回调
function Buff.VertigoBuff:BuffRemoved()
	local tgt = self:__GetTgtObj()
	if tgt then
		tgt:SetForbidMove(false)
		tgt:SetForbidAttack(false)
	end

	Buff.BaseBuff.BuffRemoved(self)		
end
