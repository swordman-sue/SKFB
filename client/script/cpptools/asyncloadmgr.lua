
AsyncLoadMgr = AsyncLoadMgr or BaseClass()

AsyncLoadMgr.PRIORITY = 
{
	NORMAL = 1,		--普通
	PERFER = 2,		--优先
}

AsyncLoadMgr.LoadingMax = 8
AsyncLoadMgr.LoadSkeletonTimeout = 8

function AsyncLoadMgr.GetInstance()
	if not g_asyncload_mgr then
		g_asyncload_mgr = AsyncLoadMgr.New()
	end
	return g_asyncload_mgr
end

function AsyncLoadMgr.DelInstance()
	if g_asyncload_mgr then
		g_asyncload_mgr:DeleteMe()
		g_asyncload_mgr = nil
	end
end

function AsyncLoadMgr:__init()
	self.loading_num = 0

	self.wait_image_list = {}
	self.loading_image_map = {}

	self.wait_skeleton_list = {}	
	self.loading_skeleton_map = {}

	GlobalRunner:AddRunObj(self, Runner.Priority.RP_ASYNC_LOAD_MGR)
end

function AsyncLoadMgr:__delete()
	self:Dump()

	GlobalRunner:RemoveRunObj(self)
end

function AsyncLoadMgr:Dump()
	self:__ClearLoadingImage()
	self:__ClearLoadingSkeleton()
	self.loading_num = 0
end

function AsyncLoadMgr:Update(now_time, elapse_time)
	self:__TryLoadImageResource()
	self:__TryLoadSkeletonFile()
end

--预加载图片
function AsyncLoadMgr:LoadImageResource(file_name, callback_fun, cancel_func, priority)
	if CPPViewHelper.IsImageExistInCache(file_name) then
		callback_fun(file_name)
		return
	end

	local func_data = {}
	func_data.callback_fun = callback_fun
	func_data.cancel_func = cancel_func
	
	--加载中，更新加载回调列表
	if self.loading_image_map[file_name] then
		table.insert(self.loading_image_map[file_name], func_data)
		return
	end

	local wait_info
	for k, v in ipairs(self.wait_image_list) do
		if v.file_name == file_name  then
			wait_info = v
			break
		end
	end

	if not wait_info then
		wait_info = {}
		wait_info.file_name = file_name
		wait_info.priority = priority or AsyncLoadMgr.PRIORITY.NORMAL
		wait_info.callback_list = {}

		local queue_len = #self.wait_image_list
		local insert_pos = 1
		for k=queue_len, 1, -1 do
			local v = self.wait_image_list[k]
			if v.priority >= wait_info.priority then
				insert_pos = k
				break
			end
		end
		--插入到等待列表
		table.insert(self.wait_image_list, insert_pos, wait_info)
	end

	--更新加载回调
	table.insert(wait_info.callback_list, func_data)
end

function AsyncLoadMgr:LoadSkeleton(path, callback_fun)
	if CPPSkeletonDataCahce:IsSkeletonDataInCache(path.json) then
		callback_fun(path)
		return
	end

	--加载中，更新加载回调列表
	local loading_info = self.loading_skeleton_map[path.json]
	if loading_info then
		loading_info.callback_list = loading_info.callback_list or {}
		table.insert(loading_info.callback_list, callback_fun)
		return
	end	

	local wait_info
	for k, v in ipairs(self.wait_skeleton_list) do
		if v.path.json == path.json  then
			wait_info = v
			break
		end
	end

	if not wait_info then
		wait_info = {}
		wait_info.path = path
		wait_info.callback_list = {}

		--插入到等待列表
		table.insert(self.wait_skeleton_list, wait_info)
	end

	--更新加载回调列表
	table.insert(wait_info.callback_list, callback_fun)
end

function AsyncLoadMgr:LoadSsdzSkeleton(path, callback_fun)
	if CPPSkeletonDataCahce:IsSSdzDataInCache(path) then
		callback_fun(path)
		return
	end

	--加载中，更新加载回调列表
	local loading_info = self.loading_skeleton_map[path]
	if loading_info then
		loading_info.callback_list = loading_info.callback_list or {}
		table.insert(loading_info.callback_list, callback_fun)
		return
	end	

	local wait_info
	for k, v in ipairs(self.wait_skeleton_list) do
		if v.path == path then
			wait_info = v
			break
		end
	end

	if not wait_info then
		wait_info = {}
		wait_info.path = path
		wait_info.callback_list = {}
		--插入到等待列表
		table.insert(self.wait_skeleton_list, wait_info)
	end

	--更新加载回调列表
	table.insert(wait_info.callback_list, callback_fun)	
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function AsyncLoadMgr:__TryLoadImageResource()
	if self.loading_num >= AsyncLoadMgr.LoadingMax then
		return
	end
		
	local file_name_list = {}
	for k, v in ipairs(self.wait_image_list) do
		table.insert(file_name_list, v.file_name)

		self.loading_image_map[v.file_name] = v.callback_list
		self.loading_num = self.loading_num + 1

		if self.loading_num >= AsyncLoadMgr.LoadingMax then
			break
		end		
	end
		
	for _, file_name in ipairs(file_name_list) do 
		---预加载图片到内存
		local LoadImageFinish = function()
			if self.loading_image_map[file_name] then
				self.loading_image_map[file_name] = nil
				self.loading_num = self.loading_num - 1

				for _, func_data in pairs(self.loading_image_map[file_name]) do
					if func_data.callback_fun then
						func_data.callback_fun(file_name)
					end
				end
			end
		end
		
		local finish_callback = GlobalCallbackMgr:AddCallBackFunc(LoadImageFinish)
		CPPViewHelper.LoadAsyncImageResource(file_name, finish_callback)
		table.remove(self.wait_image_list, 1)	
	end
end

function AsyncLoadMgr:__TryLoadSkeletonFile()
	if self.loading_num >= AsyncLoadMgr.LoadingMax then
		return
	end

	local function get_path(data)
		local path
		if type(data.path) == "table" then
			path = data.path.json
		else
			path = data.path
		end		
		return path
	end
		
	local path, loading_info, load_skeleton_callback, del_list
	for index, skeleton_data in ipairs(self.wait_skeleton_list) do
		path = get_path(skeleton_data)
		loading_info = self.loading_skeleton_map[path]
		if not loading_info then
			self.loading_skeleton_map[path] = {}
			self.loading_skeleton_map[path].elapse_time = 0
			self.loading_skeleton_map[path].callback_list = skeleton_data.callback_list
		else
			loading_info.callback_list = CPPGameLib.MergeTable2List(loading_info.callback_list, skeleton_data.callback_list)
			print("skfb", "[AsyncLoadMgr:__TryLoadSkeletonFile] fucking ass hole", debug.traceback())
		end

		--加载回调
		local LoadSkeletonDataAsync = function(ssdz_file_name, success)	
			local path = get_path(skeleton_data)
			local loading_info = self.loading_skeleton_map[path]
			if loading_info then
				--加载完成回调
				for _, func in ipairs(loading_info.callback_list) do
					func(path)
				end
				self.loading_skeleton_map[path] = nil
				self.loading_num = self.loading_num - 1
			end
		end
		load_skeleton_callback = GlobalCallbackMgr:AddCallBackFunc(LoadSkeletonDataAsync)

		--加载骨骼
		if type(skeleton_data.path) == "table" then
			CPPSkeletonDataCahce:LoadSkeletonDataAsync(skeleton_data.path.json, skeleton_data.path.atlas, skeleton_data.path.image, load_skeleton_callback)

		elseif type(skeleton_data.path) == "string" then
			CPPSkeletonDataCahce:LoadSsdzSkeletonDataAsync(skeleton_data.path, load_skeleton_callback)
		end

		del_list = del_list or {}
		table.insert(del_list, index)

		self.loading_num = self.loading_num + 1
		if self.loading_num >= AsyncLoadMgr.LoadingMax then
			break
		end		
	end

	if del_list then
		for i = #del_list, 1, -1 do
			table.remove(self.wait_skeleton_list, del_list[i])
		end
	end
end

function AsyncLoadMgr:__ClearLoadingImage()
	---等待队列
	for file_name, wait_info in ipairs(self.wait_image_list) do
		for _, func_data in ipairs(wait_info.callback_list) do
			if func_data.cancel_func then
				func_data.cancel_func()
			end
		end
		LogTool.LogInfo("[AsyncLoadMgr:__ClearLoadingImage] 停止异步加载图片:%s", file_name)
	end
	self.wait_image_list = {}
	
	---进行队列
	for file_name, callback_list in pairs(self.loading_image_map) do
		for _, func_data in ipairs(callback_list) do
			if func_data.cancel_func then
				func_data.cancel_func()
			end
		end
		LogTool.LogInfo("[AsyncLoadMgr:__ClearLoadingImage] 停止异步加载图片:%s", file_name)
	end
	self.loading_image_map = {}
end

function AsyncLoadMgr:__ClearLoadingSkeleton()
	self.wait_skeleton_list = {}
	self.loading_skeleton_map = {}
end
