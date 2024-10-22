
local error_box_show
local error_fun_list = {}
local auto_retry_list = {}

--请求失败处理
function NetWorkErrorFun(retry_fun, cancel_retry_fun, ret_code, http_ret_code, op_code, url)
	error_fun_list[#error_fun_list + 1] = {}
	error_fun_list[#error_fun_list].retry_fun = retry_fun
	error_fun_list[#error_fun_list].cancel_retry_fun = cancel_retry_fun	
	
	if error_box_show then
		return
	end

	local RetryCallFun = function()
		for _, v in pairs(error_fun_list) do
			if v.retry_fun then
				v.retry_fun()
			end
		end
		
		error_fun_list = {}
		error_box_show = false
	end

	local CancelCallFun = function()
		for _, v in pairs(error_fun_list) do
			if v.cancel_retry_fun then
				v.cancel_retry_fun()
			end
		end
		
		error_fun_list = {}
		error_box_show = false		
	end

	local error_string
	if not url then
		error_string = CPPGameLib.GetString("common_network_busy")
	else
		error_string = string.format(CPPGameLib.GetString("common_network_error"), ret_code, http_ret_code, op_code)
		error_string = string.format("%s\r(%s)", error_string, url or "")
		error_string = string.gsub(error_string, ".php", "")
	end

	if GlobalGame:CanEnterSceneChangeState() then
	    LogicGUI.TemplAffirmTips.New({
	    	type = LogicGUI.TemplAffirmTipsType.YES,
	        content = error_string,
	        confirm_text = CPPGameLib.GetString("common_retry"),
	        confirm_func = RetryCallFun,
	       	show_close_btn = false,
	    })
    else
    	GlobalSysDlg.GetInstance():Show(error_string, CPPGameLib.GetString("common_retry"), RetryCallFun, false)
    end

	error_box_show = true
end

function HttpAutoRetryFunc(now_time, elapse_time)
	if auto_retry_list then
		for _, urlmsg_data in ipairs(auto_retry_list) do
			SendUrlMsg(urlmsg_data)					
		end
		auto_retry_list = nil
	end
end

--[[
	请求接口
	post_param		[table]post参数
	get_param		[table]get参数
	url             url参数
	handle_func		
	error_notify_func
	forbid_retry
	cancel_retry_func
	timeout
	open_loading
	show_error 
	auto_retry
	auto_retry_max_count
	callback_with_json_str
--]]
function SendUrlMsg(urlmsg_data)
	if not urlmsg_data then
		return
	end

	urlmsg_data.post_param = urlmsg_data.post_param or {}
	urlmsg_data.get_param = urlmsg_data.get_param or {}
	urlmsg_data.open_loading = urlmsg_data.open_loading == nil and true or urlmsg_data.open_loading 
	if not urlmsg_data.auto_retry_count then
		urlmsg_data.auto_retry_count = 0
	end

	-- 重试处理
	local retry_fun = function(ret_code, http_ret_code, op_code)
		if not urlmsg_data.forbid_retry then
			--重试操作
			if not urlmsg_data.auto_retry or 
				(urlmsg_data.auto_retry_max_count and urlmsg_data.auto_retry_count >= urlmsg_data.auto_retry_max_count) then
				urlmsg_data.auto_retry_count = 0

				--提示重试
				NetWorkErrorFun(function()
					SendUrlMsg(urlmsg_data)
				end, urlmsg_data.cancel_retry_func, ret_code, http_ret_code, op_code, url)

				--错误通知
				if urlmsg_data.error_notify_func then
					urlmsg_data.error_notify_func(error_msg)
				end

			else
				--自动重试
				urlmsg_data.auto_retry_count = urlmsg_data.auto_retry_count + 1
				auto_retry_list = auto_retry_list or {}
				table.insert(auto_retry_list, urlmsg_data)
			end
		end
	end

	-- 回调处理函数
	local callback_func = function (httpid, funcid, is_abort, result, ret_code, url, url_data, url_len, http_ret_code, op_code)
		http_ret_code = http_ret_code or 8888
		op_code = op_code or 8888
	
		GlobalCallbackMgr:DelCallBackFunc(funcid)

		if urlmsg_data.open_loading then
			--关闭loading界面
			GlobalViewMgr:CloseViews({[BaseViewType.UI_LOADING_II] = true}, nil, true, true)
		end

		if 0 ~= ret_code or not url_data then			
			local error_msg = string.format("[SendUrlMsg] 请求失败(url:%s ret_code:%d http_ret_code:%d op_code:%d)", url, ret_code, http_ret_code, op_code)
			LogTool.LogError(error_msg)

			retry_fun(ret_code, http_ret_code, op_code)
			return
		end

		if urlmsg_data.callback_with_json_str then
			if urlmsg_data.handle_func then
				urlmsg_data.handle_func(url_data)
			end
			return
		end
				
		local data, pos, err = CPPGameLib.JSON.decode(url_data)
		if err then
			local error_msg = string.format("[SendUrlMsg] json解析错误 err:%s url:%s url_data:%s", err, url, url_data)
			if urlmsg_data.show_error ~= false then
				LogTool.LogError(error_msg)
			end

			--错误通知
			if urlmsg_data.error_notify_func then
				urlmsg_data.error_notify_func(error_msg)
			end

			retry_fun(ret_code, http_ret_code, op_code)			
		else
			if data.code and tonumber(data.code) ~= Macros.Global.HttpSuccessCode then
				local msg_error = data.str
				LogTool.LogError("[SendUrlMsg] 请求失败 msg_error:%s url:%s", msg_error, url)
			else
				if urlmsg_data.handle_func then
					urlmsg_data.handle_func(data.msg, data)
				end
			end
		end
	end
	
	local callback_func_id = GlobalCallbackMgr:AddCallBackFunc(callback_func)

	local call_api_success
	if urlmsg_data.call_plat_api then
		call_api_success = CPPResManager:CallPlatApi(urlmsg_data.url, CPPGameLib.ToStringTable(urlmsg_data.post_param), CPPGameLib.ToStringTable(urlmsg_data.get_param), callback_func_id, urlmsg_data.timeout or Macros.Global.HttpOutTimeII)
	else
		call_api_success = CPPResManager:CallDirectApi(urlmsg_data.url, CPPGameLib.ToStringTable(urlmsg_data.post_param), CPPGameLib.ToStringTable(urlmsg_data.get_param), callback_func_id, urlmsg_data.timeout or Macros.Global.HttpOutTimeII)
	end
	if call_api_success then
		if urlmsg_data.open_loading then
			--打开loading界面
			CallModuleFunc(ModuleType.LOADING, "OpenView", BaseViewType.UI_LOADING_II)		
		end
		return true
	end
	return false	
end

--[[
	请求接口
	header_list     [table]头信息列表
	post_param		[table]post参数
	get_param		[table]get参数
	url
	handle_func
	forbid_retry
	cancel_retry_func
	timeout
	open_loading
	show_error
--]]
function SendUrlMsgEx(urlmsgex_data)
	if not urlmsgex_data then
		return
	end
	urlmsgex_data.header_list = urlmsgex_data.header_list or {}
	urlmsgex_data.post_param = urlmsgex_data.post_param and CPPGameLib.JSON.encode(urlmsgex_data.post_param) or ""
	urlmsgex_data.get_param = urlmsgex_data.get_param and CPPGameLib.JSON.encode(urlmsgex_data.get_param) or ""
	urlmsgex_data.open_loading = urlmsgex_data.open_loading == nil and true or urlmsgex_data.open_loading

	-- 回调处理函数
	local callback_func = function (httpid, funcid, is_abort, result, ret_code, url, url_data, url_len, http_ret_code, op_code)
		http_ret_code = http_ret_code or 8888
		op_code = op_code or 8888

		GlobalCallbackMgr:DelCallBackFunc(funcid)

		if urlmsgex_data.open_loading then
			--关闭loading界面
			GlobalViewMgr:CloseViews({[BaseViewType.UI_LOADING_II] = true}, nil, true, true)
		end

		if 0 ~= ret_code or not url_data then			
			LogTool.LogError("[SendUrlMsgEx] 请求失败(url:%s, ret_code:%s)", url, tostring(ret_code))
			
			if not urlmsgex_data.forbid_retry then
				NetWorkErrorFun(function()
					SendUrlMsgEx(urlmsgex_data)
				end, urlmsgex_data.cancel_retry_func, ret_code, http_ret_code, op_code, url)
				return
			end
			return
		end
				
		local data, pos, err = CPPGameLib.JSON.decode(url_data)
		if err then
			if urlmsgex_data.show_error ~= false then
				LogTool.LogError("[SendUrlMsgEx] json解析错误 err:%s url:%s url_data:%s", err, url, url_data)
			end
		else
			if tonumber(data.status) ~= 0 then
				LogTool.LogError("[SendUrlMsgEx] 请求数据有误 url:%s url_data:%s", url, url_data)
			else
				if urlmsgex_data.handle_func then
					urlmsgex_data.handle_func(data)
				end
			end
		end
	end
	
	local callback_func_id = GlobalCallbackMgr:AddCallBackFunc(callback_func)
	if CPPResManager:CallDirectApiEx(urlmsgex_data.url, urlmsgex_data.header_list, urlmsgex_data.post_param, urlmsgex_data.get_param, callback_func_id, urlmsgex_data.timeout or Macros.Global.HttpOutTimeII) then
		if urlmsgex_data.open_loading then
			--打开loading界面
			CallModuleFunc(ModuleType.LOADING, "OpenView", BaseViewType.UI_LOADING_II)		
		end
		return true
	end	
	return false	
end

--[[
	请求接口
	url
	handle_func
	forbid_retry
	cancel_retry_func
	timeout
	open_loading
	show_error

	file_path
	file_data
	file_size
	mime_type
--]]
function SendFile2Server(urlmsg_data)
	if not urlmsg_data then
		return
	end

	urlmsg_data.open_loading = urlmsg_data.open_loading == nil and false or urlmsg_data.open_loading 

	-- 回调处理函数
	local callback_func = function (httpid, funcid, is_abort, result, ret_code, url, url_data, url_len, http_ret_code, op_code)
		http_ret_code = http_ret_code or 8888
		op_code = op_code or 8888	

		GlobalCallbackMgr:DelCallBackFunc(funcid)

		if urlmsg_data.open_loading then
			--关闭loading界面
			GlobalViewMgr:CloseViews({[BaseViewType.UI_LOADING_II] = true}, nil, true, true)
		end

		if 0 ~= ret_code or not url_data then			
			LogTool.LogError("[SendFile2Server] 请求失败(ret_code:%s, url:%s)", tostring(ret_code), url)
			
			if not urlmsg_data.forbid_retry then
				--重试操作
				if not urlmsg_data.auto_retry then
					--提示重试
					NetWorkErrorFun(function()
						SendFile2Server(urlmsg_data)
					end, urlmsg_data.cancel_retry_func, ret_code, http_ret_code, op_code, url)
				else
					--自动重试
					SendFile2Server(urlmsg_data)					
				end
				return
			end
			return
		end

		if urlmsg_data.callback_with_json_str then
			if urlmsg_data.handle_func then
				urlmsg_data.handle_func(url_data)
			end
			return
		end

		if url_data then
	        LogTool.LogInfo("[SendFile2Server] url_data:%s", url_data)		
	    end

		local data, pos, err = CPPGameLib.JSON.decode(url_data)
		if err then
			if urlmsg_data.show_error ~= false then
				LogTool.LogError("[SendFile2Server] json解析错误 err:%s url:%s url_data:%s", err, url, url_data)
			end
		else
			if data.code and tonumber(data.code) ~= Macros.Global.HttpSuccessCode then
				local msg_error = data.str
				LogTool.LogError("[SendFile2Server] 发送文件失败 msg_error:%s url:%s", msg_error, url)
			else
				if urlmsg_data.handle_func then
					urlmsg_data.handle_func(data.msg, data)
				end
			end
		end
	end
	
	local callback_func_id = GlobalCallbackMgr:AddCallBackFunc(callback_func)

	if urlmsg_data.file_path then
		--发送文件
		CPPResManager:SendFile2Server(urlmsg_data.url, urlmsg_data.file_path, urlmsg_data.mime_type, callback_func_id, urlmsg_data.timeout or Macros.Global.HttpOutTime)		
	else
		--发送文件数据
		CPPResManager:SendFile2Server(urlmsg_data.url, urlmsg_data.file_data, urlmsg_data.file_size, urlmsg_data.mime_type, callback_func_id, urlmsg_data.timeout or Macros.Global.HttpOutTime)		
	end

	if urlmsg_data.open_loading then
		--打开loading界面
		CallModuleFunc(ModuleType.LOADING, "OpenView", BaseViewType.UI_LOADING_II)		
	end	

	return true
end
