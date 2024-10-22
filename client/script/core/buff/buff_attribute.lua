
--[[
%% @module: 属性BUFF
%% @author: swordman sue
%% @created: 2016-10-21
--]]

Buff.AttributeBuff = Buff.AttributeBuff or BaseClass(Buff.BaseBuff)

function Buff.AttributeBuff:__MyInit(config_buff)
	local sp_params = config_buff.params_att
	if not sp_params then
		return false
	end

	--属性类型
	self.__attr_type = sp_params[1]

	--值类型(0=固定值；1=万分比)
	self.__attr_val_type = sp_params[2]

	--值
	self.__attr_val = sp_params[3]
	self.__attr_val = self.__attr_val_type == 0 and self.__attr_val or MathTool.TransProbCoeff2Decimal(self.__attr_val)

	return true
end

--创建回调
function Buff.AttributeBuff:BuffCreated()
	Buff.BaseBuff.BuffCreated(self)

	local tgt = self:__GetTgtObj()
	if tgt then
		local attr_org_val = tgt:GetAttr(self.__attr_type)
		local attr_dst_val = attr_org_val
		if self.__attr_val_type == 0 then
			--固定值
			attr_dst_val = math.ceil(attr_dst_val + self.__attr_val)
		else
			--比例
			attr_dst_val = math.ceil(attr_dst_val * (1 + self.__attr_val))
		end

		--记录变化值
		self.__attr_val_change = attr_dst_val - attr_org_val

		--修改属性值
		tgt:SetAttr(self.__attr_type, attr_dst_val)
	end
end

--移除回调
function Buff.AttributeBuff:BuffRemoved()
	local tgt = self:__GetTgtObj()
	if tgt then
		local attr_cur_val = tgt:GetAttr(self.__attr_type)

		--还原属性值
		tgt:SetAttr(self.__attr_type, attr_cur_val - self.__attr_val_change)
	end

	Buff.BaseBuff.BuffRemoved(self)		
end
