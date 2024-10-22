
--[[
%% @module: 净化BUFF
%% @author: swordman sue
%% @created: 2017-05-25
--]]

Buff.PurgeBuff = Buff.PurgeBuff or BaseClass(Buff.BaseBuff)

--创建回调
function Buff.PurgeBuff:BuffCreated()
	Buff.BaseBuff.BuffCreated(self)

	local tgt_buff_map = GlobalBuffMgr:GetBuffsByTargetID(self.__tgt_obj_id)
	tgt_buff_map = CPPGameLib.CopyTbl(tgt_buff_map)
	
	local buff, config_buff
	if tgt_buff_map then
		for buff_type, buff_id_map in pairs(tgt_buff_map) do
			--控制类BUFF，直接移除
			if GlobalBuffMgr:IsCtrlBuff(buff_type) then			
				for buff_id, _ in pairs(buff_id_map) do
					GlobalBuffMgr:DeleteBuff(buff_id)
				end

			--治疗BUFF，如果第一个参数为负数，则去除
			elseif buff_type == Buff.BuffType.TREATMENT then
				for buff_id, _ in pairs(buff_id_map) do
					buff = GlobalBuffMgr:GetBuff(buff_id)
					config_buff = buff:GetConfig()
					if config_buff and config_buff.params_treatment and config_buff.params_treatment[2] < 0 then
						GlobalBuffMgr:DeleteBuff(buff_id)
					end
				end

			--属性BUFF，如果第三个参数为负数，则去除
			elseif buff_type == Buff.BuffType.ATTRIBUTE then
				for buff_id, _ in pairs(buff_id_map) do
					buff = GlobalBuffMgr:GetBuff(buff_id)
					config_buff = buff:GetConfig()
					if config_buff and config_buff.params_att and config_buff.params_att[3] < 0 then
						GlobalBuffMgr:DeleteBuff(buff_id)
					end
				end
			end
		end
	end
end
