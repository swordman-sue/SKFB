
--[[
%% @module: 回怒BUFF
%% @author: swordman sue
%% @created: 2017-3-1
--]]

Buff.AngerReplyBuff = Buff.AngerReplyBuff or BaseClass(Buff.BaseBuff)

function Buff.AngerReplyBuff:__MyInit(config_buff, ...)
	local sp_params = config_buff.params_anger_reply
	if not sp_params then
		return false
	end

	--回复量	
	self.__reply_var = sp_params[1] or 0

	return true
end

--作用回调(定时调用)
function Buff.AngerReplyBuff:BuffTakeEffect()
	Buff.BaseBuff.BuffTakeEffect(self)

	local tgt = self:__GetTgtObj()
	if tgt then
		tgt:DoAngerChange(self.__reply_var)
	end
end
