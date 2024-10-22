
--[[
%% @module: require工具
%% @author: swordman sue
%% @created: 2016-07-06
--]]

---------------单批次处理---------------
----------------------------------------
local RequireTool = RequireTool or BaseClass()

--[[
@ params：
    type			[number]0加载 1卸载(默认0)
    file_list		[table]待加载文件列表({file1,file2....})
    callback		[function]加载完成回调
    num_per_frame	[number]每帧加载量(默认1)
--]]
function RequireTool:__init(params)
	self.type = params.type or 0
	self.file_list = params.file_list
	self.callback = params.callback
	self.num_per_frame = params.num_per_frame or 1
	self.cur_index = 1
	self.max_index = #self.file_list
end

function RequireTool:Update(now_time, elapse_time)
	if self.is_finished then
		return 0
	end

	local cur_index
	local file_name
	local require_num = 0
	for index = self.cur_index, self.max_index do
		cur_index = index
		file_name = self.file_list[cur_index]
		--加载
		if self.type == 0 then
			if not RequireToolBatch.required_map[file_name] then
				require(file_name)
				RequireToolBatch.required_map[file_name] = 1
				
				if StartConfig.IsTest then
					table.insert(RequireToolBatch.required_list, file_name)
				end
			end
			
			require_num = require_num + 1
			if require_num >= self.num_per_frame then
				break
			end

		--卸载
		else
			package.loaded[file_name] = nil
			RequireToolBatch.required_map[file_name] = nil
		end
	end

	self.cur_index = cur_index + 1
	if self.cur_index > self.max_index then
		self.is_finished = true
		if self.callback then
			self.callback()
		end
	end

	return require_num
end

function RequireTool:IsFinished()
	return self.is_finished
end

---------------多批次处理---------------
----------------------------------------
RequireToolBatch = RequireToolBatch or BaseClass()

function RequireToolBatch.CreateInstance()
	if not GlobalRequireToolBatch then
		GlobalRequireToolBatch = RequireToolBatch.New(1)
		RequireToolBatch.required_map = {}
		if StartConfig.IsTest then
			RequireToolBatch.required_list = {}
		end
	end
end

function RequireToolBatch.DelInstance()
	if GlobalRequireToolBatch then
		GlobalRequireToolBatch:DeleteMe()
		GlobalRequireToolBatch = nil
		RequireToolBatch.required_map = nil
		if StartConfig.IsTest then
			RequireToolBatch.required_list = nil
		end
	end	
end

function RequireToolBatch.UnloadAllFiles()
	for file_name, _ in pairs(RequireToolBatch.required_map) do
		package.loaded[file_name] = nil
	end
	RequireToolBatch.required_map = {}

	if StartConfig.IsTest then
		RequireToolBatch.required_list = {}
	end
end

function RequireToolBatch.ReloadAllFiles()
	if not StartConfig.IsTest then
		return
	end

	for file_name, _ in pairs(RequireToolBatch.required_map) do
		package.loaded[file_name] = nil
	end

	local del_list
	for index, file_name in ipairs(RequireToolBatch.required_list) do
		if RequireToolBatch.required_map[file_name] then
			require(file_name)
		else
			del_list = del_list or {}
			table.insert(del_list, index)
		end
	end

	if del_list then
		for index = #del_list, 1, -1 do
			table.remove(RequireToolBatch.required_list, del_list[index])
		end
	end
end

function RequireToolBatch:__init(num_per_frame, is_pipelined)
	self.num_per_frame = num_per_frame
	self.is_pipelined = is_pipelined
	self.require_tool_list = {}

	GlobalRunner:AddRunObj(self, Runner.Priority.RP_REQUIRE_TOOL)
end

function RequireToolBatch:__delete()
	GlobalRunner:RemoveRunObj(self)
end

--[[
@ params：
    file_list		[table]待加载文件列表({file1,file2....})
    callback        [function]加载完成回调
--]]
function RequireToolBatch:LoadFiles(params)
	params.type = 0
	params.num_per_frame = self.num_per_frame
	local tool = RequireTool.New(params)
	table.insert(self.require_tool_list, tool)
end

--[[
@ params：
    file_list		[table]待加载文件列表({file1,file2....})
--]]
function RequireToolBatch:LoadFilesSync(params)
	params.type = 0
	params.num_per_frame = 8888
	local tool = RequireTool.New(params)
	tool:Update()
	tool:DeleteMe()		
end

--[[
@ params：
    file_list		[table]待卸载文件列表({file1,file2....})
    callback        [function]卸载完成回调
--]]
function RequireToolBatch:UnloadFiles(params)
	params.type = 1
	local tool = RequireTool.New(params)
	table.insert(self.require_tool_list, tool)
end

function RequireToolBatch:Update(now_time, elapse_time)
	local del_list
	local require_num = 0
	for index, tool in ipairs(self.require_tool_list) do
		require_num = require_num + tool:Update(now_time, elapse_time)

		if tool:IsFinished() then
			del_list = del_list or {}
			table.insert(del_list, index)

		elseif self.is_pipelined then
			--流水线模式，确保加载完成的顺序
			break
		end

		if require_num >= self.num_per_frame then
			break
		end
	end

	if del_list then
		for index = #del_list, 1, -1 do
			table.remove(self.require_tool_list, del_list[index])
		end
	end
end

