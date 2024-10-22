
WidgetUnity = WidgetUnity or {}

function WidgetUnity.GetWidgetByName(root_widget, widget_name, show_error)
	local widget = CPPWidgetUnity.GetWidgetByName(root_widget, widget_name)
	if not widget and show_error ~= false then
		LogTool.LogError("[WidgetUnity.GetWidgetByName] widget %s not exist", widget_name)
	end
	return widget
end

function WidgetUnity.SetWidgetListener(widget, down_func, move_func, up_func, cancel_func, cd_time, down_limit_interval, cd_control)
	cd_time = cd_time or 0
	down_limit_interval = down_limit_interval or 400

	if not widget then
		LogTool.LogError("[WidgetUnity.SetWidgetListener] 参数widget无效")
		return
	end

	local down_func_id, move_func_id, up_func_id, cancel_func_id
	down_func_id = GlobalCallbackMgr:AddBtnCallBackFunc(function(x, y, in_cd)
		--[[
			控件touchBegan后，即使有屏蔽层覆盖在它上面，仍然能触发touchEnd
			暂时的处理方式是，监听两次touchBegan的时间，若间隔过小，则无视第二次对应的touchEnd
		--]]
		local down_callback_func_info = GlobalCallbackMgr:GetCallbackFunInfo(down_func_id)
		local up_callback_func_info = GlobalCallbackMgr:GetCallbackFunInfo(up_func_id)
		if CPPGameEngine:GetNowTimeMilli() - down_callback_func_info.callback_data.down_event_time < down_limit_interval then
			if up_callback_func_info then
				up_callback_func_info.callback_data.ignore = true
			end
		else
			down_callback_func_info.callback_data.down_event_time = CPPGameEngine:GetNowTimeMilli()
		end

		if down_func then
			down_func(x, y, in_cd)
		end
	end, cd_time, nil, cd_control)
	move_func_id = move_func and GlobalCallbackMgr:AddBtnCallBackFunc(move_func, cd_time, nil, cd_control) or 0
	up_func_id = up_func and GlobalCallbackMgr:AddBtnCallBackFunc(up_func, cd_time, nil, cd_control) or 0
	cancel_func_id = cancel_func and GlobalCallbackMgr:AddBtnCallBackFunc(cancel_func, cd_time, nil, cd_control) or 0
	widget:SetEventListener(down_func_id, move_func_id, up_func_id, cancel_func_id)
end

function WidgetUnity.SetCheckboxListener(widget, select_func, unselect_func)
	if not widget then
		LogTool.LogError("[WidgetUnity.SetCheckboxListener] 参数widget无效")		
		return
	end
	
	local select_func_id = select_func and GlobalCallbackMgr:AddCallBackFunc(select_func) or 0
	local unselect_func_id = unselect_func and GlobalCallbackMgr:AddCallBackFunc(unselect_func) or 0
	widget:SetEventListener(select_func_id, unselect_func_id)
end

function WidgetUnity.SetScrollListener(widget, scroll_func)
	if not widget then
		LogTool.LogError("[WidgetUnity.SetScrollListener] 参数widget无效")		
		return
	end
	
	local scroll_func_id = scroll_func and GlobalCallbackMgr:AddCallBackFunc(scroll_func) or 0
	widget:SetEventListener(scroll_func_id)
end

function WidgetUnity.SetEditboxListener(widget, attach_func, detach_func, insert_func, delete_func)
	if not widget then
		LogTool.LogError("[WidgetUnity.SetEditboxListener] 参数widget无效")		
		return
	end
	
	local attach_func_id = attach_func and GlobalCallbackMgr:AddCallBackFunc(attach_func) or 0
	local detach_func_id = detach_func and GlobalCallbackMgr:AddCallBackFunc(detach_func) or 0
	local insert_func_id = insert_func and GlobalCallbackMgr:AddCallBackFunc(insert_func) or 0
	local delete_func_id = delete_func and GlobalCallbackMgr:AddCallBackFunc(delete_func) or 0
	widget:SetEventListener(attach_func_id, detach_func_id, insert_func_id, delete_func_id)
end

function WidgetUnity.SetPageViewListener(widget,func,is_sp)
	if not widget then
		LogTool.LogError("[WidgetUnity.SetEditboxListener] 参数widget无效")		
		return
	end
	local handle_func = function (event_type)
		if is_sp then
        	if func then
        		func(event_type)
        	end
		else
			if event_type == GUI.PageView.EVENT_TYPE.TURN_PAGE_ENDED then
	        	--翻页结束
	        	if func then
	        		func()
	        	end
			end
		end
	end
	local attach_func_id = handle_func and GlobalCallbackMgr:AddCallBackFunc(handle_func) or 0
	widget:SetEventListener(attach_func_id)
end

