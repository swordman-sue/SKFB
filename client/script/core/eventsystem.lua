
--利用ID分离对EventSystem的直接引用
_inner_event_system_list = _inner_event_system_list or {}
_inner_event_system_id_count = _inner_event_system_id_count or 0

EventSystem = EventSystem or BaseClass()

function EventSystem.CreateInstance()
	if not GlobalEventSystem then
		GlobalEventSystem = EventSystem.New()
		GlobalRunner:AddRunObj(GlobalEventSystem, Runner.Priority.RP_EVENT_SYS)
	end
end

function EventSystem.DelInstance()
	if GlobalEventSystem then
		GlobalRunner:RemoveRunObj(GlobalEventSystem)
		GlobalEventSystem:DeleteMe()
		GlobalEventSystem = nil
	end
end

--事件系统(非单键)
function EventSystem:__init()
	_inner_event_system_id_count = _inner_event_system_id_count + 1
	self.system_id = _inner_event_system_id_count
	_inner_event_system_list[self.system_id] = self

	--需要激发的事件(延后调用方式)
	self.need_fire_events = List.New()

	--事件列表
	self.event_list = {}

	self.delay_handle_list = {}

	self.delay_id_count = 0

	self.is_deleted = false

	self.timer_quest = CPPTimerQuest()
end

function EventSystem:__delete()
	if not self.is_deleted then
		self.timer_quest:Stop()
		self.timer_quest = nil

		self.need_fire_events = nil
		self.delay_handle_list = nil
		self.event_list = nil

		_inner_event_system_list[self.system_id] = nil
		self.is_deleted = true
	end
end

--调用已经处于派发队列中的Event
function EventSystem:Update()
	--timer quest
	self.timer_quest:Update(Macros.Global.NowTime, Macros.Global.ElapseTime)

	--依次执行所有需要触发的事件
	while not List.Empty(self.need_fire_events) do
		local fire_info = List.PopFront(self.need_fire_events)
		fire_info.event:Fire(fire_info.arg_list)
	end
end

function EventSystem:Bind(event_id, event_func)
	if event_id == nil then
	    LogTool.LogError("[EventSystem:Bind] 参数event_id无效")		
		return
	end

	if self.is_deleted then
		return
	end

	if self.event_list[event_id] == nil then
		self:CreateEvent(event_id)
	end
	local tmp_event = self.event_list[event_id]

	return tmp_event:Bind(event_func)
end

function EventSystem:UnBind(event_handle)
	if event_handle == nil or event_handle.event_id == nil then
	    LogTool.LogError("[EventSystem:UnBind] 参数event_id无效")
		return
	end

	if self.is_deleted then
		return
	end

	local tmp_event = self.event_list[event_handle.event_id]
	if tmp_event ~= nil then
		tmp_event:UnBind(event_handle)
	end
end

--立即触发
function EventSystem:Fire(event_id, ...)
	if event_id == nil then
	    LogTool.LogError("[EventSystem:Fire] 参数event_id无效")		
		return
	end

	if self.is_deleted then
		return
	end

	if self.__pause_fire_event then
		return
	end

	local tmp_event = self.event_list[event_id]
	if tmp_event ~= nil then
		tmp_event:Fire({...})
	end
end

--下一帧触发
function EventSystem:FireNextFrame(event_id, ...)
	if event_id == nil then
	    LogTool.LogError("[EventSystem:FireNextFrame] 参数event_id无效")		
		return
	end

	if self.is_deleted then
		return
	end

	if self.__pause_fire_event then
		return
	end

	local tmp_event = self.event_list[event_id]
	if tmp_event ~= nil then
		local fire_info = {}
		fire_info.event = tmp_event
		fire_info.arg_list = {...}
		List.PushBack(self.need_fire_events, fire_info)
	end
end

function EventSystem:FireDelay(event_id, delay_time, ...)
	if event_id == nil then
	    LogTool.LogError("[EventSystem:FireDelay] 参数event_id无效")		
		return
	end

	if self.is_deleted then
		return
	end

	if self.__pause_fire_event then
		return
	end

	self.delay_id_count = self.delay_id_count + 1
	local delay_id = self.delay_id_count
	local system_id = self.system_id
	local arg_list = {...}
	local delay_call_func = function()
		local obj = _inner_event_system_list[system_id]
		if obj ~= nil then
			obj:Fire(event_id, unpack(arg_list))	--执行定时任务
			obj.delay_handle_list[delay_id] = nil	--删除该句柄
		end
	end
	local quest_handle = self.timer_quest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(delay_call_func), delay_time)
	self.delay_handle_list[delay_id] = quest_handle
end

function EventSystem:CreateEvent(event_id)
	self.event_list[event_id] = Event.New(event_id)
end

function EventSystem:Clear()
	self.timer_quest:Stop()
	--需要激发的事件(延后调用方式)
	self.need_fire_events = List.New()

	--事件列表
	self.event_list = {}

	self.delay_handle_list = {}

	self.delay_id_count = 0
end

function EventSystem:PauseTimer()
	self.timer_quest:Pause()
end

function EventSystem:ResumeTimer()
	self.timer_quest:Resume()
end

--暂停事件派发
function EventSystem:PauseFireEvent()
	self.__pause_fire_event = true
end

--恢复事件派发
function EventSystem:ResumeFireEvent()
	self.__pause_fire_event = false
end
