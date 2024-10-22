
--[[
%% @module: 运行时管理
%% @author: swordman sue
%% @created: 2014-04-25
%% @description: 根据优先级在每帧中依次执行受托管的RunObj.Update
--]]

Runner = Runner or BaseClass()

Runner.Priority =
{
	RP_MIN = 1,
	RP_LUA_GC = 5,				--LUAGC
	RP_LUA_ERR = 10,			--LUA错误上传
	RP_REQUIRE_TOOL = 15,		--LUA文件加载 
	RP_ASYNC_LOAD_MGR = 20,		--异步加载管理器
	RP_NET_ADAPTER = 25,		--网络适配器
	RP_TIMER = 30,				--定时器
	RP_EVENT_SYS = 35,			--事件系统
	RP_STATE_MGR = 40,			--主状态管理器
	RP_MODULE_MGR = 45,			--模块管理器
	RP_VIEW_MGR = 50,			--界面管理器
	RP_AI_MGR = 55,				--AI管理器
	RP_BUFF_MGR = 56,			--BUFF管理器
	RP_COOKIES = 60,			--本地缓存
	RP_DEFAULT = 90,			--默认
	RP_LEAD = 100,				--新手指引
	RP_MAX = 100,
}

function Runner.CreateInstance()
	if not GlobalRunner then
		GlobalRunner = Runner.New()
	end
end

function Runner.DelInstance()
	if GlobalRunner then
		GlobalRunner:DeleteMe()
		GlobalRunner = nil
	end
end

function Runner:__init()
	self.id_count = 0

	-- 用于标记某个模块是否已经注册,避免重复性的注册
	self.all_run_obj_list = {}		
	setmetatable(self.all_run_obj_list, {__mode = "k"})

	--支持优先级指定, 1为最先执行, RP_MAX为最后执行
	self.priority_run_obj_list = {}
	for _, priority_level in pairs(Runner.Priority) do
		self.priority_run_obj_list[priority_level] = {}
		setmetatable(self.priority_run_obj_list[priority_level], {__mode = "v"})
	end

	--用于ui的优先级表
	self.priority_ui_run_obj_list = {}
	for _, priority_level in pairs(Runner.Priority) do
		self.priority_ui_run_obj_list[priority_level] = {}
		setmetatable(self.priority_ui_run_obj_list[priority_level], {__mode = "v"})
	end

	self.priority_level_list = {}
	for _, priority_level in pairs(Runner.Priority) do
		table.insert(self.priority_level_list, priority_level)
	end
	table.sort(self.priority_level_list, function(a, b)
		if not a then
			return true
		elseif not b then
			return false
		else
			return a < b
		end 
	end)
end

function Runner:__delete()
	self.all_run_obj_list = nil
	self.priority_run_obj_list = nil
	self.priority_ui_run_obj_list = nil
end

--[[@
功能:	主Update中调用该函数,触发托管对象的Update
]]
function Runner:Update(now_time, elapse_time)
	local run_obj_info, real_elapse_time

	for _, priority_level in ipairs(self.priority_level_list) do		
		if not self.all_run_obj_list then
			return
		end

		local priority_tbl = self.priority_run_obj_list[priority_level]
		for _, run_obj in pairs(priority_tbl) do
			run_obj_info = self.all_run_obj_list[run_obj]
			if run_obj_info.interval <= 0 then
				run_obj:Update(now_time, elapse_time)	

			elseif now_time >= run_obj_info.next_update_time then
				real_elapse_time = not run_obj_info.last_update_time and 0 or (now_time - run_obj_info.last_update_time)
				run_obj:Update(now_time, real_elapse_time)	
				run_obj_info.last_update_time = now_time			
				run_obj_info.next_update_time = now_time + run_obj_info.interval
			end
		end
	end

	for _, priority_level in ipairs(self.priority_level_list) do		
		if not self.all_run_obj_list then
			return
		end
		
		local priority_tbl = self.priority_ui_run_obj_list[priority_level]
		for _, run_obj in pairs(priority_tbl) do
			run_obj_info = self.all_run_obj_list[run_obj]
			if run_obj_info.interval <= 0 then
				run_obj:Update(now_time, elapse_time)	

			elseif now_time >= run_obj_info.next_update_time then
				real_elapse_time = not run_obj_info.last_update_time and 0 or (now_time - run_obj_info.last_update_time)
				run_obj:Update(now_time, real_elapse_time)	
				run_obj_info.last_update_time = now_time			
				run_obj_info.next_update_time = now_time + run_obj_info.interval
			end
		end
	end
end

--[[@
功能:	向Runner添加一个RunObj, RunObj必须存在Update函数
参数:	
		run_obj 要添加的run_obj对象 any 必须实现Update函数
		priority_level Update优先级 1-16,数字越小越先执行
返回值:
		是否添加run_obj成功 bool run_obj如果已经存在于Runner中, 则不能再添加
]]
function Runner:AddRunObj(run_obj , priority_level , interval)
	if not run_obj then
		return
	end

	local run_obj_info = self.all_run_obj_list[run_obj]
	if run_obj_info ~= nil then
		--已经存在该对象, 不重复添加
		return false
	else
		if run_obj["Update"] == nil then
			LogTool.LogError("[Runner:AddRunObj] run_obj需要实现Update(now_time, elapse_time)方法")
			return
		end

		--对象不存在,正常添加
		self.id_count = self.id_count + 1
		priority_level = priority_level or Runner.Priority.RP_DEFAULT
		interval = interval or 0

		run_obj_info = {}
		run_obj_info.priority_level = priority_level
		run_obj_info.id_count = self.id_count
		run_obj_info.interval = interval
		run_obj_info.next_update_time = 0
		self.all_run_obj_list[run_obj] = run_obj_info

		self.priority_run_obj_list[priority_level][self.id_count] = run_obj
	end
end

--[[@
功能:	从Runner中删除一个run_obj
]]
function Runner:RemoveRunObj( run_obj )
	if not run_obj then
		return
	end

	local run_obj_info = self.all_run_obj_list[run_obj]
	if run_obj_info ~= nil then
		self.all_run_obj_list[run_obj] = nil
		self.priority_run_obj_list[run_obj_info.priority_level][run_obj_info.id_count] = nil
	end
end

--[[@
功能:	同AddRunObj，仅用于UI
]]
function Runner:AddUIRunObj( run_obj , priority_level , interval )
	if not run_obj then
		return
	end

	local run_obj_info = self.all_run_obj_list[run_obj]
	if run_obj_info ~= nil then
		--已经存在该对象, 不重复添加
		return false
	else
		if run_obj["Update"] == nil then
			LogTool.LogError("[Runner:AddUIRunObj] run_obj需要实现Update(now_time, elapse_time)方法")
			return
		end

		--对象不存在,正常添加
		self.id_count = self.id_count + 1
		priority_level = priority_level or Runner.Priority.RP_DEFAULT
		interval = interval or 0.2

		run_obj_info = {}
		run_obj_info.priority_level = priority_level
		run_obj_info.id_count = self.id_count
		run_obj_info.interval = interval
		run_obj_info.next_update_time = 0		
		self.all_run_obj_list[run_obj] = run_obj_info

		self.priority_ui_run_obj_list[priority_level][self.id_count] = run_obj
	end
end

--[[@
功能:	同RemoveRunObj，仅用于UI
]]
function Runner:RemoveUIRunObj(run_obj)
	if not run_obj then
		return
	end
	
	local run_obj_info = self.all_run_obj_list[run_obj]
	if run_obj_info ~= nil then
		self.all_run_obj_list[run_obj] = nil
		self.priority_ui_run_obj_list[run_obj_info.priority_level][run_obj_info.id_count] = nil
	end
end

