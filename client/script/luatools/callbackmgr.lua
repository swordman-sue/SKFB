CallBackMgr = CallBackMgr or BaseClass()
CallBackMgr.Type = {
	CallBackTypeDefault = 0,
	CallBackTypeBtn = 1,
}
CallBackMgr.DefaultBtnCD = 500

function CallBackMgr.CreateInstance()
	if not GlobalCallbackMgr then
		GlobalCallbackMgr = CallBackMgr.New()
	end
end

function CallBackMgr.DelInstance()
	if GlobalCallbackMgr then
		GlobalCallbackMgr:DeleteMe()
		GlobalCallbackMgr = nil
	end
end

function CallBackMgr:__init()
	self.base_callback_func_id = 10000
	self.callback_func_list = {}
end

function CallBackMgr:__delete()
	self.callback_func_list = nil
end

function CallBackMgr:AddCallBackFunc(func, func_type, data)
	if not func then
		return 0
	end

	local  callback_data = {
	 	callback_func = func,
	 	callback_type = func_type or CallBackMgr.Type.CallBackTypeDefault,
	 	callback_data = data,
	}

	self.base_callback_func_id = self.base_callback_func_id + 1
	self.callback_func_list[self.base_callback_func_id] = callback_data

	-- LogTool.LogInfo("func_id:%d    traceback:%s", self.base_callback_func_id, debug.traceback())

	return self.base_callback_func_id
end

function CallBackMgr:GetCallbackFunInfo(func_id)
	return self.callback_func_list[func_id]
end

--增加按钮类型事件
--cd_time cd时间
--cd_tips cd提示
--cd_control 是否外部控制cd逻辑
--wait_msg 等待的网络事件
--msg_tip 等待网络事件点击的提示
function CallBackMgr:AddBtnCallBackFunc(func, cd_time, cd_tips, cd_control, wait_msg, msg_tip)
	local data = {
		event_time = 0,
		down_event_time = 0,

		btn_cd_time = cd_time or CallBackMgr.DefaultBtnCD,
		cd_tips = cd_tips,
		cd_control = cd_control,

		wait_protocol_no = wait_msg,
		waiting_msg = false,
		msg_tip = msg_tip or CPPGameLib.GetString("common_network_busy"),
	}
	return self:AddCallBackFunc(func, CallBackMgr.Type.CallBackTypeBtn, data)
end

function CallBackMgr:DelCallBackFunc(func_id)
	self.callback_func_list[func_id] = nil
end

function CallBackMgr:RunCallBackFunc(func_id, ...)
	if not self.callback_func_list[func_id] then
		return
	end

	local in_cd
	local callback_type = self.callback_func_list[func_id].callback_type
	local callback_data = self.callback_func_list[func_id].callback_data

	--普通类型
	if callback_type == CallBackMgr.Type.CallBackTypeDefault then
		--do nothing

	--按钮类型
	elseif callback_type == CallBackMgr.Type.CallBackTypeBtn then
		if callback_data.ignore then
			callback_data.ignore = false
			return
		end

		--cd时间判断
		local is_in_lead = CallModuleFunc(ModuleType.LEAD, "IsInLead")
		in_cd = not is_in_lead and (CPPGameEngine:GetNowTimeMilli() - callback_data.event_time < callback_data.btn_cd_time)
		if in_cd then
			if callback_data.cd_tips then
				--cd中提示
				GlobalTipMsg.GetInstance():Show(callback_data.cd_tips)
			end

			if not callback_data.cd_control then
				--cd中
				return
			end
		else
			callback_data.event_time = CPPGameEngine:GetNowTimeMilli()
		end

		--网络消息等待
		if callback_data.waiting_msg then
			GlobalTipMsg.GetInstance():Show(tostring(callback_data.msg_tip))
			return

		elseif(callback_data.wait_protocol_no and callback_data.wait_protocol_no ~= 0) then
			callback_data.waiting_msg = true
		end
	end

	if self.callback_func_list[func_id].callback_func then
		if in_cd then
			local params = {...}
			table.insert(params, in_cd)
			return self.callback_func_list[func_id].callback_func(unpack(params))
		else
			return self.callback_func_list[func_id].callback_func(...)
		end
	end
end

function CallBackMgr:ListenNetWorkMsg(protocol_id, protocol_data)
	local protocol_no = protocol_id
	if(protocol_no == ProtocolCode.SC_NoticeNum or protocol_no == ProtocolCode.SC_NoticeCode or protocol_no == ProtocolCode.SC_NewFuncOpen_Error) then
		protocol_no = protocol_data.request_protocol_no
	end

	for k, v in pairs(self.callback_func_list) do
		if(v.callback_type == CallBackMgr.Type.CallBackTypeBtn and v.callback_data and
			v.callback_data.wait_protocol_no == protocol_no) then
			v.callback_data.waiting_msg = false
		end
	end
end

