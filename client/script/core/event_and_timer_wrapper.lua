
EventAndTimerWrapper = EventAndTimerWrapper or BaseClass()

function EventAndTimerWrapper:__init()
    self:RegistAllEvents()
end

function EventAndTimerWrapper:__delete()
	self:UnRegistAllEvents()
	self:CancelAllTimers()
end

---------------------------------------------------------------------------
--事件相关
---------------------------------------------------------------------------
function EventAndTimerWrapper:Bind(event_id, event_func)
	return GlobalEventSystem:Bind(event_id, event_func)
end

function EventAndTimerWrapper:UnBind(event_handle)
	GlobalEventSystem:UnBind(event_handle)
end

function EventAndTimerWrapper:Fire(event_id, ...)
	GlobalEventSystem:FireNextFrame(event_id, ...)
end

function EventAndTimerWrapper:FireNextFrame(event_id, ...)
	GlobalEventSystem:FireNextFrame(event_id, ...)
end

--注册全局事件，对象销毁时自动注销事件
function EventAndTimerWrapper:BindGlobalEvent(event_name, callback, interval)
	self.__event_info_map = self.__event_info_map or {}
	if self.__event_info_map[event_name] then
	    LogTool.LogError("[EventAndTimerWrapper:BindGlobalEvent] 该事件已注册(event_name:%s)", event_name)
	    return
	end

	local event_info = {}
	event_info.interval = interval or -1
	event_info.callback = callback
	event_info.event_handle = GlobalEventSystem:Bind(event_name, function(...)
		if self.__is_deleted or not self.__event_info_map then
			GlobalEventSystem:UnBind(event_info.event_handle)
			return
		end
		local cur_event_info = self.__event_info_map[event_name]
		if cur_event_info then		
			if not cur_event_info.last_time then
				cur_event_info.last_time = Macros.Global.NowTime
				cur_event_info.callback(event_name, unpack({...}))
			elseif Macros.Global.NowTime - cur_event_info.last_time > cur_event_info.interval then
				cur_event_info.last_time = Macros.Global.NowTime
				cur_event_info.callback(event_name, unpack({...}))
			end
		end
	end)
	self.__event_info_map[event_name] = event_info
end

function EventAndTimerWrapper:RegistAllEvents()
end

function EventAndTimerWrapper:UnRegistAllEvents()
	if self.__event_info_map then
		for _, event_info in pairs(self.__event_info_map) do
			GlobalEventSystem:UnBind(event_info.event_handle)
		end
		self.__event_info_map = nil
	end
end

---------------------------------------------------------------------------
--计时器相关
---------------------------------------------------------------------------
--[[
	延时执行一个任务
	@para1 func 任务对应的脚本函数 in
	@para2 delay_time 延时时间 in
	@Return 返回生命周期持有ID
]]
function EventAndTimerWrapper:AddDelayQuest(func, delay_time)
	if not func then
		LogTool.LogError("[EventAndTimerWrapper:AddDelayQuest] invalid param1(func)")
		return
	end

	if not delay_time or delay_time <= 0 then
		LogTool.LogError("[EventAndTimerWrapper:AddDelayQuest] invalid param2(delay_time)")
		return
	end

	self.__timer_id_map = self.__timer_id_map or {}
    local timer_id = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(
        function() 
			if self.__is_deleted or not self.__timer_id_map then
				return
			end

			self.__timer_id_map[timer_id] = nil
        	func()
        end), delay_time)

    return timer_id
end

--[[
	周期性执行一个任务
	@para1 func 任务对应的脚本函数 in
	@para2 period 执行周期 in
	@para3 last_time 任务持续时间(为负表示任务永不过期) in
	@Return 返回生命周期持有ID
]]
function EventAndTimerWrapper:AddPeriodQuest(func, period, last_time)
	if not func then
		LogTool.LogError("[EventAndTimerWrapper:AddPeriodQuest] invalid param1(func)")
		return
	end

	if not period then
		LogTool.LogError("[EventAndTimerWrapper:AddPeriodQuest] invalid param2(period)")
		return
	end

	if not last_time then
		LogTool.LogError("[EventAndTimerWrapper:AddPeriodQuest] invalid param3(last_time)")
		return
	end

	self.__timer_id_map = self.__timer_id_map or {}
    local timer_id = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
        function() 
			if self.__is_deleted or not self.__timer_id_map then
				GlobalTimerQuest:CancelQuest(timer_id)
				return
			end

        	func()
        end), period, last_time)

    return timer_id    
end

--[[
	周期性执行任务指定次数
	@para1 func 任务对应的脚本函数 in
	@para2 period 执行周期 in
	@para3 run_count 任务执行的次数 in
	@Return 返回生命周期持有ID
]]
function EventAndTimerWrapper:AddRunTimesQuest(func, period, run_count)
	if not func then
		LogTool.LogError("[EventAndTimerWrapper:AddRunTimesQuest] invalid param1(func)")
		return
	end

	if not period then
		LogTool.LogError("[EventAndTimerWrapper:AddRunTimesQuest] invalid param2(period)")
		return
	end

	if not run_count then
		LogTool.LogError("[EventAndTimerWrapper:AddRunTimesQuest] invalid param3(run_count)")
		return
	end

	self.__timer_id_map = self.__timer_id_map or {}
    local timer_id = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(
        function() 
			if self.__is_deleted or not self.__timer_id_map then
				GlobalTimerQuest:CancelQuest(timer_id)
				return
			end

        	func()
        end), period, run_count)	

    return timer_id        
end

function EventAndTimerWrapper:CancelTimer(timer_id)
	if self.__timer_id_map and self.__timer_id_map[timer_id] then
		self.__timer_id_map[timer_id] = nil
		GlobalTimerQuest:CancelQuest(timer_id)
	end
end

function EventAndTimerWrapper:CancelAllTimers()
	if self.__timer_id_map then
		for timer_id, _ in pairs(self.__timer_id_map) do
			GlobalTimerQuest:CancelQuest(timer_id)
		end
		self.__timer_id_map = nil
	end
end