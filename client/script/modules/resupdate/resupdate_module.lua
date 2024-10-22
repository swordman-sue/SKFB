
--[[
%% @module: 资源更新模块
%% @author: swordman sue
%% @created: 2016-09-30
--]]

Modules = Modules or {}

Modules.ResUpdateModule = Modules.ResUpdateModule or BaseClass(BaseModule)

--同时更新文件数上限
local MAX_FILE_NUM = 10

--新增更新文件数上限(单批次)
local MAX_INCREMENT_NUM_PER_BATCH = 3

--自动重试次数上限
local MAX_AUTO_RETRY_NUM = 2

--进游戏前必须解压的资源列表
local NecessaryUnzipMap = 
{
	--协议文件
	["resource/pb"] = true,

	["resource/default"] = true,
	["resource/effects/particle/mesh"] = true,

	--TODO：处理音频文件的动态解压
	["resource/sound"] = true,

	--公共资源
	["resource/ui_res/common"] = true,
	["resource/ui_res/citycommon"] = true,
	[string.format("resource/ui_res/language/%s/common", LanguageHelper.language)] = true,
	[string.format("resource/ui_res/language/%s/citycommon", LanguageHelper.language)] = true,

	--配置文件
	[string.format("resource/language/%s", LanguageHelper.district_language)] = true,
}

--进游戏前非必须下载的资源列表(文件下载超过自动重试次数后，可忽略)
local UnnecessaryDownloadMap = 
{
	["resource/bigicon"] = true,
	["resource/half_heads"] = true,
	["resource/heads"] = true,
	["resource/items"] = true,
	["resource/effects"] = true,
}

function Modules.ResUpdateModule:__init()
	self.__can_update = true
end

function Modules.ResUpdateModule:__delete()
end

function Modules.ResUpdateModule:RegistAllEvents()
	local function cancel_cur_download_task()
		if self.__downloading_file_map then
			local progress_info, abort_file_name_list, abort_http_id_list
			for file_name, http_id in pairs(self.__downloading_file_map) do
				progress_info = CPPHttpManager:GetProgress(http_id)
				if progress_info.ret and (progress_info.total_len <= 0 or progress_info.cur_len < progress_info.total_len) then
					abort_file_name_list = abort_file_name_list or {}
					abort_http_id_list = abort_http_id_list or {}
					table.insert(abort_file_name_list, file_name)
					table.insert(abort_http_id_list, http_id)
				end
			end		

			--中断未完成的请求
			if abort_http_id_list then
				CPPResManager:CancelUpdateFileList(abort_http_id_list, nil)
			end
			--重新下载未完成的请求
			if abort_file_name_list then
				CPPResManager:UpdateFileList(abort_file_name_list, self.__on_update_callback_id, Macros.Global.HttpOutTime, self.__downloading_file_map, self.__unziping_file_map)
			end
		end
	end

	--网络无效，重新下载未完成的请求
	self:Bind(Macros.Event.Net.NETWORK_INVALID_NOTIFY, cancel_cur_download_task)

	--网络状态变化，重新下载未完成的请求
	self:Bind(Macros.Event.Net.NETWORK_STATE_CHANGED_NOTIFY, cancel_cur_download_task)
end

function Modules.ResUpdateModule:Start()
	--需要解压/下载的文件信息
	self.__need_unzip_file_map = {}		--key:文件名 value:文件大小
	self.__need_unzip_file_size = 0
	self.__need_download_file_map = {}	--key:文件名 value:文件大小
	self.__need_download_file_size = 0

	--必须解压的文件列表
	self.__must_unzip_file_map = {}

	--需要更新的文件信息
	self.__need_update_file_list = {}

	--解压/下载中的文件信息
	self.__unziping_file_map = {}		--key:文件名 value:http_id/async_id
	self.__downloading_file_map = {}	--key:文件名 value:http_id/async_id
	self.__updating_file_num = 0
	self.__to_update_file_index = 1

	--解压/下载完的文件信息
	self.__unzipped_file_size = 0
	self.__downloaded_file_size = 0
	self.__updated_file_num = 0

	--资源解压/下载完成
	self.__is_res_load_finished = false
	--脚本加载完成
	self.__is_script_load_finished = false
	--公共资源加载完成
	self.__is_common_load_finished = false

	--文件已自动重试次数记录表
	self.__retried_file_map = {}

	--准备自动重试的文件列表
	self.__to_retry_file_list = {}

	local is_skip_update = tonumber(CPPResManager:GetSetting(Macros.Setting.IsSkipUpdate)) or 0
	if is_skip_update ~= Macros.Global.TRUE and (StartConfig.IsLocalRes ~= "true" or CPPSystemTools.system ~= CPPSystemTools.win32) then
		--获取需解压/下载的文件列表
		CPPResManager:GetUpdateFileList(self.__need_download_file_map, self.__need_unzip_file_map, self.__must_unzip_file_map)
	end

	--统计需解压/下载的信息
	for file_name, file_size in pairs(self.__need_unzip_file_map) do
		if self:__CheckNecessaryUnzip(file_name) then	
			self.__need_unzip_file_size = self.__need_unzip_file_size + file_size
			table.insert(self.__need_update_file_list, file_name)
		end
	end
	for file_name, file_size in pairs(self.__need_download_file_map) do
		self.__need_download_file_size = self.__need_download_file_size + file_size
		table.insert(self.__need_update_file_list, file_name)
	end

	--初始化资源更新回调
	self:__SetupResUpdateCallback()

	self.__is_started = true
end

function Modules.ResUpdateModule:Update(now_time, elapse_time)
	if not self.__is_started then
		return
	end

	if self:IsFinished() then
		return
	end

	--检测解压/下载完成
	self:__HandleResUpdateFinished()

	--尝试重试
	self:__TryAutoRetry(elapse_time)

	--尝试解压/更新资源
	self:__TryResUpdate()

	local view = self:GetView()
	if view and view:IsOpen() then
		view:UpdateProcess(elapse_time)
	end
end

function Modules.ResUpdateModule:IsFinished()
	return self.__is_res_load_finished and self.__is_script_load_finished and self.__is_common_load_finished
end

function Modules.ResUpdateModule:IsResLoadFinished()
	return self.__is_res_load_finished
end

function Modules.ResUpdateModule:IsCommonLoadFinished()
	return self.__is_common_load_finished
end

function Modules.ResUpdateModule:IsUnzipping()
	return CPPGameLib.GetTableLength(self.__unziping_file_map) > CPPGameLib.GetTableLength(self.__downloading_file_map)
end

--获取更新信息
function Modules.ResUpdateModule:GetResUpdateInfo()	
	local updated_size = self.__unzipped_file_size + self.__downloaded_file_size
	local need_update_size = self.__need_unzip_file_size + self.__need_download_file_size
	local downloaded_size = self.__downloaded_file_size

	for _, http_id in pairs(self.__downloading_file_map) do
		local progress_info = CPPHttpManager:GetProgress(http_id)
		if progress_info.ret then
			downloaded_size = downloaded_size + progress_info.cur_len
			updated_size = updated_size + progress_info.cur_len
		end
	end

	return updated_size, need_update_size, 
	self.__unzipped_file_size, self.__need_unzip_file_size, 
	downloaded_size, self.__need_download_file_size
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--尝试解压/更新资源
function Modules.ResUpdateModule:__TryResUpdate()
	if self.__is_res_load_finished then
		return
	end

	if not GlobalNetAdapter:IsNetworkValid() then 
		return
	end

	--计算新增更新文件数上限(单批次)
	local max_increment_num_per_batch = MAX_FILE_NUM - self.__updating_file_num - #self.__to_retry_file_list
	if max_increment_num_per_batch <= 0 then
		return
	end
	max_increment_num_per_batch = math.min(max_increment_num_per_batch, MAX_INCREMENT_NUM_PER_BATCH)

	--生成本次要更新的文件列表
	local to_update_file_list
	for index = self.__to_update_file_index, #self.__need_update_file_list do
		to_update_file_list = to_update_file_list or {}
		table.insert(to_update_file_list, self.__need_update_file_list[index])

		if #to_update_file_list >= max_increment_num_per_batch then
			break
		end
	end

	if to_update_file_list then
		self.__updating_file_num = self.__updating_file_num + #to_update_file_list
		self.__to_update_file_index = self.__to_update_file_index + #to_update_file_list
	
		--更新文件
		CPPResManager:UpdateFileList(to_update_file_list, self.__on_update_callback_id, Macros.Global.HttpOutTime, self.__downloading_file_map, self.__unziping_file_map)
	end
end

--初始化资源更新回调
function Modules.ResUpdateModule:__SetupResUpdateCallback()	
	--资源更新回调
	local function on_update_callback(async_or_http_id, func_id, is_abort, result, ret_code, file_name, file_len, http_ret_code, op_code, custom_ret_msg)
		if self.__is_res_load_finished then
			return
		end

		http_ret_code = http_ret_code or 8888
		op_code = op_code or 8888
		custom_ret_msg = custom_ret_msg or ""

		--文件下载/解压超过自动重试次数后，能否忽略
		local is_ignorable_file_4_failure

		--失败
		if not result or ret_code ~= 0 or is_abort then
			--自动重试超过上限
			if self.__retried_file_map[file_name] and self.__retried_file_map[file_name] >= MAX_AUTO_RETRY_NUM then
				self.__retried_file_map[file_name] = 0

				is_ignorable_file_4_failure = self:__CheckIgnorableFile4Failure(file_name)
				if not is_ignorable_file_4_failure then
					--不可忽略，提示网络异常，让玩家重试
					NetWorkErrorFun(function()
						self:__DoAutoRetry(file_name, true)
					end, nil, ret_code, http_ret_code, op_code, file_name)

					--客户端事件跟踪过程中，上报导致卡死的错误信息
					local op_desc = self.__downloading_file_map[file_name] and "download" or "unzip"
					local error_msg = string.format([[[Modules.ResUpdateModule:on_update_callback] 
						%s file(%s) failed is_abort:%d ret_code:%d http_ret_code:%d op_code:%d custom_ret_msg:%s]], 
						op_desc, file_name, is_abort and Macros.Global.TRUE or Macros.Global.FALSE, ret_code, http_ret_code, op_code, custom_ret_msg)
					StatisticsTool.GetInstance():ReportErrorInfoForEventTrack(error_msg)																				
					return
				end
			end
			
			if not is_ignorable_file_4_failure then
				--不可忽略，插入到自动重试列表
				table.insert(self.__to_retry_file_list, {file_name = file_name})
				return
			end
		end

		--移除重试信息
		if self.__retried_file_map then
			self.__retried_file_map[file_name] = nil
		end

		--更新解压/下载信息
		file_len = self.__need_unzip_file_map[file_name] or file_len

		self.__updating_file_num = self.__updating_file_num - 1
		self.__updated_file_num = self.__updated_file_num + 1
		if self.__unziping_file_map[file_name] then
			self.__unziping_file_map[file_name] = nil
			self.__unzipped_file_size = self.__unzipped_file_size + file_len
		end
		if self.__downloading_file_map[file_name] then
			self.__downloading_file_map[file_name] = nil
			self.__downloaded_file_size = self.__downloaded_file_size + file_len
		end
	end
	self.__on_update_callback_id = GlobalCallbackMgr:AddCallBackFunc(on_update_callback)	
end

--检测自动重试
function Modules.ResUpdateModule:__TryAutoRetry(elapse_time)
	local retry_file_name, ignore_record, retryed_index_list
	for i, retry_info in ipairs(self.__to_retry_file_list) do
		--延时进行自动重试
		retry_info.pass_time = retry_info.pass_time or 0
		retry_info.pass_time = retry_info.pass_time + elapse_time
		if retry_info.pass_time >= 0.5 then			
			if self:__DoAutoRetry(retry_info.file_name, retry_info.ignore_record) then
				retryed_index_list = retryed_index_list or {}
				table.insert(retryed_index_list, i)
			end
		end
	end

	--移除已自动重试项
	if retryed_index_list then
		table.sort(retryed_index_list,  SortTools.ItemUpperSorter())
		for _, del_index in ipairs(retryed_index_list) do
			table.remove(self.__to_retry_file_list, del_index)
		end
	end
end

--执行自动重试
function Modules.ResUpdateModule:__DoAutoRetry(retry_file_name, ignore_record)
	self.__retried_file_map[retry_file_name] = self.__retried_file_map[retry_file_name] or 0
	local cur_retried_num = self.__retried_file_map[retry_file_name]
	if cur_retried_num >= MAX_AUTO_RETRY_NUM then
		return
	end

	if not ignore_record then
		self.__retried_file_map[retry_file_name] = cur_retried_num + 1
	end

	if CPPResManager:UpdateFileList({retry_file_name}, self.__on_update_callback_id, Macros.Global.HttpOutTime, self.__downloading_file_map, self.__unziping_file_map) then
		if not ignore_record then
			LogTool.LogInfo("[on_update_callback] 资源更新重试处理，file_name:%s retry_count:%s", retry_file_name, self.__retried_file_map[retry_file_name])
		end
		return true
	end
end

--检测资源解压
function Modules.ResUpdateModule:__CheckNecessaryUnzip(file_name)
	if self.__must_unzip_file_map[file_name] then
		return true
	end

	if CPPSystemTools.IsIOS() then
		--在IOS平台上，从bundle读取资源无效率问题，所以没必要拷贝到下载目录
		return false
	end

	if string.find(file_name, ".cnf") then
		return true
	end

	for necessary_file_key, _ in pairs(NecessaryUnzipMap) do
		if string.find(file_name, necessary_file_key) then
			return true
		end
	end
end

--检测文件可忽略性(部分文件下载/解压超过自动重试次数后，选择性忽略)
function Modules.ResUpdateModule:__CheckIgnorableFile4Failure(file_name)
	if self.__unziping_file_map[file_name] then
		return not self:__CheckNecessaryUnzip(file_name)
	end

	if not GlobalNetAdapter:IsNetworkValid() then
		--网络不连通，不可忽略
		return
	end

	for unnecessary_file_key, _ in pairs(UnnecessaryDownloadMap) do
		if string.find(file_name, unnecessary_file_key) then
			return true
		end
	end
end

--资源解压、更新完成处理
function Modules.ResUpdateModule:__HandleResUpdateFinished()
	if self.__updated_file_num < #self.__need_update_file_list then
		return
	end

	if not self.__is_res_load_finished then
		self.__is_res_load_finished = true

		--客户端事件跟踪(统计流失率)
		StatisticsTool.GetInstance():ReportEventTrackForLossRate(StatisticsTool.EventType4LossRate.LOAD_COMMON_RES)				

		--加载公共资源
		GlobalGame:LoadBaseKeepRes(function()
			self:__HandleCommonLoadFinished()
		end)
	end
end

--公共资源加载完成处理
function Modules.ResUpdateModule:__HandleCommonLoadFinished()
	self.__is_common_load_finished = true

	--客户端事件跟踪(统计流失率)
	StatisticsTool.GetInstance():ReportEventTrackForLossRate(StatisticsTool.EventType4LossRate.LOAD_INIT_SCRIPT_II)			

	--加载第二批初始脚本
	GlobalGame:LoadLuaFilesForEnterSceneChangeState(function()
		self:__HandleScriptLoadFinished()
	end)
end

--第二批LUA初始文件加载完成处理
function Modules.ResUpdateModule:__HandleScriptLoadFinished()
	self.__is_script_load_finished = true

	GlobalGame:LoadLuaFilesForEnterSceneChangeStateII()
end

