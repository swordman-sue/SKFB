
--[[
%% @module: 模块基类
%% @author: swordman sue
%% @created: 2016-07-08
--]]

BaseModule = BaseModule or BaseClass(EventAndTimerWrapper)

function BaseModule:__init(module_type)
	self.__can_update = nil
	self.__module_type = module_type
end

function BaseModule:__delete()
	self:CloseView()
end

function BaseModule:GetType()
	return self.__module_type
end

function BaseModule:OpenView(view_type, ...)
	--界面打开前的协议请求
	if not self.__ignore_protocol_for_open_view and self:__RequestProtocolsForOpenView(view_type, {...}) then
		return
	end

	local cur_view = self:GetView()
	if cur_view then
		if cur_view:GetViewType() == view_type then
			--处理重复打开
			cur_view:Open(...)
			return
		else
			if not GlobalViewMgr:IsInViewStackFilter(view_type) then
				--解绑当前界面
				cur_view:SetUnbindModuleType(true)
			end
		end
	end

	GlobalViewMgr:OpenView(view_type, self.__module_type, ...)
end

function BaseModule:CloseView(view_id, ignore_view_stack, ignore_view_sound)
	if GlobalViewMgr then
		GlobalViewMgr:CloseView(view_id or self.__view_id, ignore_view_stack, ignore_view_sound)
	end
end

function BaseModule:GetView(view_id)
	return GlobalViewMgr:GetView(view_id or self.__view_id, false)
end

function BaseModule:GetViewByType(view_type)
	return GlobalViewMgr:GetViewByType(view_type)
end

function BaseModule:IsViewOpen(view_id)
	local view = self:GetView(view_id or self.__view_id)
	return view and view:IsOpen()
end

function BaseModule:SetViewID(view_id)
	self.__view_id = view_id
end

function BaseModule:GetViewID()
	return self.__view_id
end

function BaseModule:CanUpdate(elapse_time)
	if not self.__can_update then
		return false
	end

	--更新间隔处理
	self.__update_interval = self.__update_interval or 0

	--每帧更新
	if self.__update_interval <= 0 then
		return true
	end

	--间隔更新
	if not self.__update_elapse_time then
		self.__update_elapse_time = self.__update_interval
	else
		self.__update_elapse_time = self.__update_elapse_time + elapse_time
	end
	if self.__update_elapse_time >= self.__update_interval then
		self.__update_elapse_time = self.__update_elapse_time - self.__update_interval
		return true
	end
	
	return false
end

function BaseModule:Update(now_time, elapse_time)
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--[[
注册界面打开所需协议
@ params：
    view_type			[number]界面类型
    protocol_no			[number]协议号
    request_once		[bool]一次性请求(默认true)
--]]
function BaseModule:__RegistProtocolForOpenView(view_type, protocol_no, request_once)
	self.__protocol_for_open_view = self.__protocol_for_open_view or {}
	self.__protocol_for_open_view[view_type] = self.__protocol_for_open_view[view_type] or {}

	if self.__protocol_for_open_view[view_type][protocol_no] then
		LogTool.LogError("[BaseModule:__RegistProtocolForOpenView] 界面(view_type:%d)打开所需协议已注册")
		return
	end

	local info = {}
	info.protocol_no = protocol_no
	info.protocol_no_resp = Net.Protocol4OpenViewMap[protocol_no]
	info.request_once = request_once == nil and true or request_once
	info.has_request = false
	self.__protocol_for_open_view[view_type][protocol_no] = info

	--注册协议回调
	GlobalModulesMgr:RegistProtocolToModule(info.protocol_no_resp, self.__module_type, "__HandleProtocolsForOpenView")
end

--界面打开前的协议请求
function BaseModule:__RequestProtocolsForOpenView(view_type, args_list)
	local ret = false

	if self.__protocol_for_open_view then
		local count = 0
		local info_map = self.__protocol_for_open_view[view_type]
		if info_map then
			for _, info in pairs(info_map) do
				if not info.has_request then
					info.has_request = true
					local protocol = GlobalNetAdapter:GetProtocol(info.protocol_no)
					GlobalNetAdapter:OnSend(protocol)

					count = count + 1
					ret = true
				end
			end
		end
		self.__wait_protocol_count = count
		self.__wait_protocol_for_open_view = count > 0
		self.__open_view_args = self.__wait_protocol_for_open_view and args_list or nil
	end

	return ret
end

--界面打开前的协议回调
function BaseModule:__HandleProtocolsForOpenView(protocol_data)
	if not self.__protocol_for_open_view or not self.__wait_protocol_for_open_view then
		return
	end

	for view_type, info_map in pairs(self.__protocol_for_open_view) do
		for _, info in pairs(info_map) do
			if info.protocol_no_resp == protocol_data.protocol_no and info.has_request then
				if not info.request_once then
					info.has_request = false
				end

				--协议接收完毕，打开界面
				self.__wait_protocol_count = self.__wait_protocol_count - 1
				if self.__wait_protocol_count <= 0 then
					self.__ignore_protocol_for_open_view = true
					self:OpenView(view_type, unpack(self.__open_view_args))
					self.__ignore_protocol_for_open_view = nil

					self.__wait_protocol_for_open_view = false
					self.__open_view_args = nil
					return
				end
			end
		end
	end
end
