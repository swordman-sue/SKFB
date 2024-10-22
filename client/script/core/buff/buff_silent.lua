
--[[
%% @module: 沉默BUFF
%% @author: swordman sue
%% @created: 2016-09-18
--]]

Buff.SilentBuff = Buff.SilentBuff or BaseClass(Buff.BaseBuff)

--创建回调
function Buff.SilentBuff:BuffCreated()
	Buff.BaseBuff.BuffCreated(self)
	
	local tgt = self:__GetTgtObj()
	if tgt then
		tgt:SetForbidSpecialSkill(true)
		tgt:SetForbidAngerOrJointSkill(true)
	end
end

--移除回调
function Buff.SilentBuff:BuffRemoved()
	local tgt = self:__GetTgtObj()
	if tgt then
		tgt:SetForbidSpecialSkill(false)
		tgt:SetForbidAngerOrJointSkill(false)
	end

	Buff.BaseBuff.BuffRemoved(self)	
end
