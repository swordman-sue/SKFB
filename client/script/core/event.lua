
_inner_event_connection_obj = _inner_event_connection_obj or {}

Event = Event or BaseClass()

function Event:__init(event_id)
	self.event_id = event_id
	self.bind_id_count = 0
	self.event_func_list = {}
	self.lock = false
	self.to_add_event_func_list = {}
	self.to_unbind_obj_list = {}
end

function Event:Fire(arg_table)
	self.lock = true	
	for bind_id, func in pairs(self.event_func_list) do
		local flag = true
		for _, obj in pairs(self.to_unbind_obj_list or {}) do
			if bind_id == obj.bind_id then
				flag = false
				break
			end
		end
		if flag then
			func(unpack(arg_table))
		end
	end
	self.lock = false

	for _, obj in pairs(self.to_unbind_obj_list or {}) do
		self:UnBind(obj)
	end
	self.to_unbind_obj_list = {}

	for bind_id, event_func in pairs(self.to_add_event_func_list) do 
		self.event_func_list[bind_id] = event_func
	end
	self.to_add_event_func_list = {}
end

function Event:UnBind(obj)
	--仅当obj符合类型时才作对应操作
	if getmetatable(obj) == _inner_event_connection_obj and obj.event_id == self.event_id then
		if self.lock == true then
			table.insert(self.to_unbind_obj_list, obj)
		else
			self.event_func_list[obj.bind_id] = nil
		end
	end
end

function Event:Bind(event_func)
	self.bind_id_count = self.bind_id_count + 1
	local obj = {}
	setmetatable(obj, _inner_event_connection_obj)
	obj.event_id = self.event_id
	obj.bind_id = self.bind_id_count
	if self.lock then
		self.to_add_event_func_list[obj.bind_id] = event_func
	else
		self.event_func_list[obj.bind_id] = event_func
	end
	return obj
end

