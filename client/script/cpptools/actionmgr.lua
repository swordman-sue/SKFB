
ActionManager = ActionManager or BaseClass()

local remove_map_blacking_bg
local remove_head_anim_blacking_bg

--关键帧动画类型
local KeyFrameAnimType = 
{
	HEAD_ANIM = 1,
	FLOAT_ANIM = 2,
}

--关键帧动作类型
local KeyFrameActionType = 
{
	POS = 1,
	OPACITY = 2,
}

function ActionManager.GetInstance()
	if not g_actionmgr then
		g_actionmgr = ActionManager.New()
	end
	return g_actionmgr
end

function ActionManager.DelInstance()
	if g_actionmgr then
		g_actionmgr:DeleteMe()
		g_actionmgr = nil
	end
end

function ActionManager:__init()
    GlobalRunner:AddRunObj(self)	
end

function ActionManager:__delete()
	GlobalRunner:RemoveRunObj(self)
end

function ActionManager:Update(now_time, elapse_time)
	--更新头像动画
	self:__UpdateHeadAnim(elapse_time)

	--更新浮空动画
	self:__UpdateFloatAnim(elapse_time)
end

function ActionManager:Pause()
	self:__PauseHeadAnim()
	self:__PauseFloatAnim()
end

function ActionManager:Resume()
	self:__ResumeHeadAnim()
	self:__ResumeFloatAnim()
end

function ActionManager:Dump()
	self.__map_blacking_lock = false
	self:StopMapBlacking()
	self:StopMapBlackingEffectEx()
	self:StopWorldStarEffect()

	self:__DumpHeadAnim()
	self:__DumpFloatAnim()
end

--放大->恢复
function ActionManager:SpreadAnimation(node, scale, time)
	if not node then
		LogTool.LogError("[ActionManager:SpreadAnimation] 参数node无效")
		return 
	end

	CPPActionManager.ScaleTo(node, time / 2, scale, GlobalCallbackMgr:AddCallBackFunc(function()
		CPPActionManager.ScaleTo(node, time / 2, 1)
	end))
end

--小->大->小->恢复
function ActionManager:ElasticAnimation(node , time , func)
	if not node then
		LogTool.LogError("[ActionManager:ElasticAnimation] 参数node无效")
		return 
	end

	node:SetScaleFactor( 0.5 )
	CPPActionManager.ScaleTo(node, time / 3 , 1.1 , GlobalCallbackMgr:AddCallBackFunc(function()
		CPPActionManager.ScaleTo(node, time / 3 , 0.9 , GlobalCallbackMgr:AddCallBackFunc(function()
			CPPActionManager.ScaleTo(node, time / 3 , 1 , GlobalCallbackMgr:AddCallBackFunc(function()
				if func then func() end
			end) )
		end) )
	end))
end

--呼吸
function ActionManager:ZoomAnimation(node, inteval, distence1 , distence2 )
	if not node then
		LogTool.LogError("[ActionManager:LeftRightMoveAnimation] 参数node无效")
		return
	end
	local inteval_time = inteval or  1
	distence1 = distence1 or 1.01
	distence2 = distence2 or 1


	local func
	local is_left
	local zoom = function () 
		is_left = not is_left
		if is_left then
			CPPActionManager.ScaleTo(node, inteval_time , distence1 , func)
		else
			CPPActionManager.ScaleTo(node, inteval_time , distence2 , func)
		end
	end
	func = GlobalCallbackMgr:AddCallBackFunc(zoom)
	zoom()
	return func
end

--动态渐变
function ActionManager:FadeAnimation(node, inteval, once_callback)
	if not node then
		LogTool.LogError("[ActionManager:FadeAnimation] 参数node无效")
		return 
	end
	local inteval_time = inteval or  0.5
	local func
	local is_out 
	local fadeAnimate = function ()
		is_out = not is_out
		if is_out then
			CPPActionManager.FadeOut(node, inteval_time, func)
		else
			CPPActionManager.FadeIn(node, inteval_time, once_callback or func)
		end
	end
	func = GlobalCallbackMgr:AddCallBackFunc(fadeAnimate)
	fadeAnimate()
	return func
end

--左右移动
function ActionManager:LeftRightMoveAnimation(node, inteval, distence, flip)
	if not node then
		LogTool.LogError("[ActionManager:LeftRightMoveAnimation] 参数node无效")
		return
	end
	local inteval_time = inteval or  1
	local distence_value = distence or 5
	local func
	local is_left

	local moveAnimate = function () 
		is_left = not is_left
		if is_left then
			CPPActionManager.MoveBy(node, inteval_time,distence_value,0,func)
			if(flip) then
				node:SetFlipX(true)
			end
		else
			CPPActionManager.MoveBy(node, inteval_time,-distence_value,0,func)
			if(flip) then
				node:SetFlipX(false)
			end
		end
	end
	func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
	moveAnimate()
	return func
end

--上下移动
function ActionManager:UpDownMoveAnimation(node,inteval,distence)
	if not node then
		LogTool.LogError("[ActionManager:UpDownMoveAnimation] 参数node无效")
		return 
	end
	local inteval_time = inteval or  1
	local distence = distence or 5
	local func
	local is_up
	local moveAnimate = function()
		is_up = not is_up
		if is_up then
			CPPActionManager.MoveBy(node, inteval_time,0,distence,func)
		else
			CPPActionManager.MoveBy(node, inteval_time,0,-distence,func)
		end
	end
	func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
	moveAnimate()
end

--转动
function ActionManager:RotateAnimation(node,inteval,digree)
	if not node then
		LogTool.LogError("[ActionManager:RotateAnimation] 参数node无效")
		return 
	end
	local inteval_time = inteval or  0.5
	local digree = digree or 90
	local func
	local roteateAnimate = function()
		CPPActionManager.RotateBy(node, inteval_time,digree,func)
	end
	func = GlobalCallbackMgr:AddCallBackFunc(roteateAnimate)
	roteateAnimate()
	return func
end

---震动对象
function ActionManager:ShockObject(object, x_offset, y_offset, shock_time, shock_count, x_pos, y_pos, callback_func)
	x_offset = x_offset or 5
	y_offset = y_offset or 5
	shock_time = shock_time or 0.025
	shock_count = shock_count or 0
	x_pos = x_pos or 0
	y_pos = y_pos or 0

	local move_pt_list = 
	{
		{x_offset, -y_offset},
		{x_offset, y_offset},
		{0, 0},
	}

	local fun_id
	local cur_stage = 1
	local OnMoveObj = function()
		if(cur_stage > #move_pt_list) then
			if shock_count <= 0 then 
				if callback_func then callback_func() end
				return 
			end
			cur_stage = 1
			shock_count = shock_count - 1
		end
		local pt_x = move_pt_list[cur_stage][1]
		local pt_y = move_pt_list[cur_stage][2]
		CPPActionManager.MoveTo(object, shock_time, pt_x + x_pos, pt_y + y_pos, fun_id)
		cur_stage = cur_stage + 1
	end
	fun_id = GlobalCallbackMgr:AddCallBackFunc(OnMoveObj)

	CPPActionManager.StopAllActions(object)
	OnMoveObj()
end

-- 地图层变黑(开始)
function ActionManager:StartMapBlacking(bg_color, bg_alpha, fadein_time, fadein_finished_callback, render_queue, effect_config_id, effect_params)
	bg_color = bg_color or Macros.Color.lilyBlack
	bg_alpha = bg_alpha or 204
	fadein_time = fadein_time or 0.4
	render_queue = render_queue or Scene.RenderQueue.GRQ_UI_DOWN

	if self.__map_blacking_lock then
		return
	end

	--停止变黑效果
	self:StopMapBlacking()

	--黑色遮罩
	self.__map_blacking_bg = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height)
	self.__map_blacking_bg:SetAnchorPoint(0.5, 0.5)
	self.__map_blacking_bg:SetTouchEnabled(true)
	Util:SetLayoutBGColor(self.__map_blacking_bg, bg_color, bg_alpha)
	GlobalScene:AddToRenderGroup(self.__map_blacking_bg, render_queue)
	PosTool.Center(self.__map_blacking_bg)

	--淡入
	if fadein_time > 0 then
		self.__map_blacking_bg:SetOpacity(0)

		if fadein_finished_callback then
			local fadein_finished_callback_id = GlobalCallbackMgr:AddCallBackFunc(function()
				if fadein_finished_callback_id then
					GlobalCallbackMgr:DelCallBackFunc(fadein_finished_callback_id)			
					fadein_finished_callback_id = nil
				end
		
				--淡入完成回调
				fadein_finished_callback()
			end)
			--淡入效果
			CPPActionManager.FadeIn(self.__map_blacking_bg, fadein_time, fadein_finished_callback_id)
		else
			--淡入效果
			CPPActionManager.FadeIn(self.__map_blacking_bg, fadein_time)
		end
	else
		if fadein_finished_callback then
			fadein_finished_callback()		
		end
	end

	--播放特效
	self:StartMapBlackingEffect(effect_config_id, effect_params)
end

-- 地图层变黑(停止)
function ActionManager:StopMapBlacking(fadeout_time, fadeout_finished_callback)
	if self.__map_blacking_lock then
		return
	end

	local function do_finished_callback()
		if fadeout_finished_callback then
			fadeout_finished_callback()
		end
	end

	if fadeout_time then
		local fadeout_finished_callback_id = GlobalCallbackMgr:AddCallBackFunc(function()
			if fadeout_finished_callback_id then
				GlobalCallbackMgr:DelCallBackFunc(fadeout_finished_callback_id)			
				fadeout_finished_callback_id = nil
			end

			--移除黑色背景
			remove_map_blacking_bg(self)

			--淡出完成回调
			do_finished_callback()
		end)
		
		--淡出效果
		if self.__map_blacking_bg then
			CPPActionManager.FadeOut(self.__map_blacking_bg, fadeout_time, fadeout_finished_callback_id)
		else
			--淡出完成回调
			do_finished_callback()
		end
	else
		--移除黑色背景
		remove_map_blacking_bg(self)
	end

	if self.__map_blacking_bg then
		self.__map_blacking_bg:SetTouchEnabled(false)
	end

	--销毁特效
	self:StopMapBlackingEffect()
end

function ActionManager:StartMapBlackingWithLock(...)
	if not self.__map_blacking_lock then
		ActionManager.GetInstance():StartMapBlacking(...)
		self.__map_blacking_lock = true
		return true
	end
end

function ActionManager:StopMapBlackingWithLock(...)
	if self.__map_blacking_lock then
		self.__map_blacking_lock = false
		ActionManager.GetInstance():StopMapBlacking(...)
		return
	end
end

-- 地图层变黑特效(播放)
function ActionManager:StartMapBlackingEffect(effect_config_id, effect_params)
	self:StopMapBlackingEffect()

	if not effect_config_id then
		return
	end

	effect_params = effect_params or {}
	effect_params[1] = effect_params[1] or Scene.RenderQueue.GRQ_UI_DOWN
	effect_params[2] = effect_params[2] or 1

	if type(effect_config_id) == "number" then
		--特效ID
		local effect_id, effect = GlobalEffectMgr:CreateScenePosEffect({id = effect_config_id, loop = true, time_scale = effect_params[2]}, effect_params[1])
		PosTool.Center(effect)
		self.__map_blacking_effect_id = effect_id

	elseif type(effect_config_id) == "string" then
		--贴图资源
		self.__map_blacking_image = Util:Sprite(Resource.PathTool.GetSkillJointBGPath(effect_config_id))
		GlobalScene:AddToRenderGroup(self.__map_blacking_image, effect_params[1], effect_params[2])
		PosTool.Center(self.__map_blacking_image)

		local scale_x = Macros.Global.DesignSize.width / Macros.Global.BGDefaultSize.width
		local scale_y = Macros.Global.DesignSize.height / Macros.Global.BGDefaultSize.height
		self.__map_blacking_image:SetScaleFactor(math.max(scale_x, scale_y))

	elseif type(effect_config_id) == "table" then
		local anim_info = effect_config_id
		if anim_info.type == "anim_battle_skip" then
			self.__map_blacking_effect_id_list = {}

			local effect_id, effect
			for _, effect_info in ipairs(anim_info.effect_list) do
				local real_event_callback = effect_info.bind_event_callback and function(action_name, evnet_name, int_value, float_value, string_val)
                    if evnet_name == "event_collision_effec" then
                        effect_id, effect = GlobalEffectMgr:CreateScenePosEffect({res_name = "UI_zhandoutiaoguo3", time_scale = effect_info.time_scale}, Scene.RenderQueue.GRQ_UI_UP)
                        PosTool.Center(effect)
                        table.insert(self.__map_blacking_effect_id_list, effect_id)
                    end
                end or nil

				effect_id, effect = GlobalEffectMgr:CreateScenePosEffect({type = effect_info.type, res_name = effect_info.res_name, loop = true, time_scale = effect_info.time_scale, event_callback = real_event_callback}, effect_info.zorder)
				PosTool.Center(effect)		
				table.insert(self.__map_blacking_effect_id_list, effect_id)	
			end
		end
	end
end

-- 地图层变黑特效(停止)
function ActionManager:StopMapBlackingEffect()
	if self.__map_blacking_effect_id then
		GlobalEffectMgr:DelEffect(self.__map_blacking_effect_id)
		self.__map_blacking_effect_id = nil
	end

	if self.__map_blacking_image then
		self.__map_blacking_image:RemoveSelf(true)
		self.__map_blacking_image = nil
	end

	if self.__map_blacking_effect_id_list then
		for _, id in ipairs(self.__map_blacking_effect_id_list) do
			GlobalEffectMgr:DelEffect(id)
		end
		self.__map_blacking_effect_id_list = nil
	end
end

-- 地图层变黑特效(播放)
function ActionManager:StartMapBlackingEffectEx(effect_config_id, effect_params)
	self:StopMapBlackingEffectEx()

	if not effect_config_id then
		return
	end

	effect_params = effect_params or {}
	effect_params[1] = effect_params[1] or Scene.RenderQueue.GRQ_UI_DOWN
	effect_params[2] = effect_params[2] or 1

	local effect_id, effect = GlobalEffectMgr:CreateScenePosEffect({id = effect_config_id, loop = true, time_scale = effect_params[2]}, effect_params[1])
	PosTool.Center(effect)
	self.__map_blacking_effect_id_ex = effect_id
end

-- 地图层变黑特效(停止)
function ActionManager:StopMapBlackingEffectEx()
	if self.__map_blacking_effect_id_ex then
		GlobalEffectMgr:DelEffect(self.__map_blacking_effect_id_ex)
		self.__map_blacking_effect_id_ex = nil
	end	
end

--左右转动
function ActionManager:AboutRotateAnimation(node,inteval,digree)
	if not node then
		LogTool.LogError("[ActionManager:RotateAnimation] 参数node无效")
		return 
	end
	local inteval_time = inteval or  0.5
	local digree = digree or 90
	local is_up
	local func

	local intex = 0
	local roteateAnimate = function()
		intex = intex + 1 
		is_up = not is_up
		if intex == 5 then
			CPPActionManager.RotateTo(node, 0, 0)
			CPPActionManager.MoveBy(node, inteval_time*5, 0, 0, func)
			intex = 0
		else
			if is_up then
				CPPActionManager.RotateTo(node, inteval_time, digree, func)
			else
				CPPActionManager.RotateTo(node, inteval_time, -digree, func)
			end
		end
	end

	func = GlobalCallbackMgr:AddCallBackFunc(roteateAnimate)
	roteateAnimate()

	return func
end

--图标直线位移效果
function ActionManager:ImgMoveToEffect(self, num, img, pos_x, pos_y, target_x, target_y, callback, multiple)
	local multiple = multiple or 1
	local totalnum = num
	local items_tbl = {}
	local offy_tbl = {}
	for i = 1, num do
		offy_tbl[i] = MathTool.GetRandom(-10, 10)
		items_tbl[i] = Util:Sprite(img)
		items_tbl[i]:SetPosition(pos_x, pos_y + offy_tbl[i])
		items_tbl[i]:SetOpacity(0)
		items_tbl[i]:SetVisible(false)
		self:AddChild(items_tbl[i], Macros.Global.z_order_top, Macros.Global.z_order_top)
	end
	local action_func = function(node, index)
		local OnComplete = function()
			self:RemoveChild(node, true)
			totalnum = totalnum - 1
			if callback and totalnum == 0 then 
				callback()
			end
		end
		local on_complete_callback = GlobalCallbackMgr:AddCallBackFunc(OnComplete)
		CPPActionManager.MoveTo(node, 0.9*multiple, target_x, target_y + offy_tbl[index])
		CPPActionManager.RotateTo(node,  0.9*multiple, 720)
		CPPActionManager.ScaleTo(node,  0.9*multiple, 1.1, on_complete_callback)
	end
	local index = 1
	local Complete_callback
	local complete = function(x, y)
		index = index + 1	
		if index <= num then 
			items_tbl[index]:SetVisible(true)
			action_func(items_tbl[index], index)
			CPPActionManager.FadeIn(items_tbl[index], 0.1*multiple, Complete_callback)
		end
	end
	Complete_callback = GlobalCallbackMgr:AddCallBackFunc(complete)
	items_tbl[index]:SetVisible(true)
	CPPActionManager.FadeIn(items_tbl[index], 0.1*multiple, Complete_callback)
	action_func(items_tbl[index], index)
end

--图标直线位移效果(图标添加到render_queue指定渲染层中)
function ActionManager:ImgMoveToEffectEX(num, img, beg_world_pos, tgt_world_pos, callback, multiple, render_queue)
	render_queue = render_queue or Scene.RenderQueue.GRQ_UI_UP
	local render_group = GlobalScene:GetRenderGroup(render_queue)

	local beg_pos = NodeUnity.ConvertToNodePositon(render_group:GetNode(), beg_world_pos)
	local tgt_pos = NodeUnity.ConvertToNodePositon(render_group:GetNode(), tgt_world_pos)

	ActionManager.GetInstance():ImgMoveToEffect(render_group, num, img, beg_pos.x, beg_pos.y, tgt_pos.x, tgt_pos.y, callback, multiple)
end

--图标抛物线位移效果
function ActionManager:ImgFloatToEffect(self, num, img, pos_x, pos_y, target_x, target_y, callback, multiple)
	if num == 0 then 
		return
	end
	local multiple = multiple or 1
	local totalnum = num
	local items_tbl = {}
	for index = 1, num do
		items_tbl[index] = SpriteEx.CreateWithImageFile(img)
		items_tbl[index]:SetPosition(pos_x, pos_y)
		items_tbl[index]:SetOpacity(0)
		items_tbl[index]:SetVisible(false)
		self:AddChild(items_tbl[index], Macros.Global.z_order_top, Macros.Global.z_order_top)
	end
	local action_func = function(node)
		local onComplete = function(x, y)
			self:RemoveChild(node, true)
			totalnum  = totalnum - 1
			if callback and totalnum == 0 then 
				callback()
			end
		end
		local onComplete_callback = GlobalCallbackMgr:AddCallBackFunc(onComplete)
		CPPActionManager.JumpTo(node, 0.9*multiple, target_x, target_y, MathTool.GetRandom(40), 1);
		CPPActionManager.RotateTo(node,  0.9*multiple, 720)
		CPPActionManager.ScaleTo(node,  0.9*multiple, 0.5, onComplete_callback)
	end
	local index = 1
	local Complete_callback
	local complete = function(x, y)
		index = index + 1
		
		if index <= num then 
			items_tbl[index]:SetVisible(true)
			action_func(items_tbl[index])
			CPPActionManager.FadeIn(items_tbl[index], 0.1*multiple, Complete_callback)
		end
	end
	Complete_callback = GlobalCallbackMgr:AddCallBackFunc(complete)
	items_tbl[index]:SetVisible(true)
	CPPActionManager.FadeIn(items_tbl[index], 0.1*multiple, Complete_callback)
	action_func(items_tbl[index])
end

--多次闪烁效果
function ActionManager:BlinkEffect(node, blinktime, totaltime, callback)
	CPPActionManager.StopAllActions(node)

	local times = math.ceil(totaltime/blinktime/2)
	local fadeout_callback
	local fadein = function()
		CPPActionManager.FadeOut(node, blinktime, fadeout_callback)
	end
	local fadein_callback =  GlobalCallbackMgr:AddCallBackFunc(fadein)

	local fadeout = function()
		times = times - 1
		if times < 0 then 
			if callback then
				callback()
			end
			return
		end
		CPPActionManager.FadeIn(node, blinktime, fadein_callback)
	end
	fadeout_callback = GlobalCallbackMgr:AddCallBackFunc(fadeout)
	CPPActionManager.FadeOut(node, blinktime, fadeout_callback)
end

--循环闪烁效果
function ActionManager:CycleBlinkEffect(node, ftime)
	CPPActionManager.StopAllActions(node)

	local fadeout_callback 
	local fadein = function()
		CPPActionManager.FadeOut(node, ftime, fadeout_callback)
	end
	local fadein_callback =  GlobalCallbackMgr:AddCallBackFunc(fadein)

	local fadeout = function()
		CPPActionManager.FadeIn(node, ftime, fadein_callback)
	end
	fadeout_callback = GlobalCallbackMgr:AddCallBackFunc(fadeout)
	CPPActionManager.FadeOut(node, ftime, fadeout_callback)
end

--聚焦效果
function ActionManager:NodeZoom(node, factor, dx, dy, zoom_time, finished_callback)
	if finished_callback then
		local finished_callback_id = GlobalCallbackMgr:AddCallBackFunc(function() 
			finished_callback()
		end)
		CPPActionManager.ScaleTo(node, zoom_time, factor, finished_callback_id)			
	else
		CPPActionManager.ScaleTo(node, zoom_time, factor)
	end
	CPPActionManager.MoveBy(node, zoom_time, dx, dy)			
end

--头像动画
function ActionManager:HeadAnim(effect_config_id, node_head1, node_head2, node_title, head_anim_config_id, finished_callback)
	if self.__head_anim_playing then
		return
	end

	self.__head_anim_finished_callback = finished_callback

	local config_head_anim = CPPGameLib.GetConfig("HeadAnim", head_anim_config_id)
	if not config_head_anim then
		if self.__head_anim_finished_callback then
			self.__head_anim_finished_callback()
			self.__head_anim_finished_callback = nil
		end
		return
	end

	self:__InsertHeadAnimInfo(KeyFrameActionType.POS, node_head1, config_head_anim.head1_pos_list, config_head_anim.head1_pos_time_list)
	self:__InsertHeadAnimInfo(KeyFrameActionType.OPACITY, node_head1, config_head_anim.head1_opacity_list, config_head_anim.head1_opacity_time_list)
	self:__InsertHeadAnimInfo(KeyFrameActionType.POS, node_head2, config_head_anim.head2_pos_list, config_head_anim.head2_pos_time_list)
	self:__InsertHeadAnimInfo(KeyFrameActionType.OPACITY, node_head2, config_head_anim.head2_opacity_list, config_head_anim.head2_opacity_time_list)
	self:__InsertHeadAnimInfo(KeyFrameActionType.POS, node_title, config_head_anim.title_pos_list, config_head_anim.title_pos_time_list)
	self:__InsertHeadAnimInfo(KeyFrameActionType.OPACITY, node_title, config_head_anim.title_opacity_list, config_head_anim.title_opacity_time_list)

	--特效
	local effect_id, effect = GlobalEffectMgr:CreateScenePosEffect({id = effect_config_id, loop = false}, Scene.RenderQueue.GRQ_UI_UP)
	PosTool.Center(effect)
	self.__head_anim_effect_id = effect_id

	self.__head_anim_playing = true
end

--浮空动画
function ActionManager:FloatAnim(node, float_info, float_dir, finished_callback)
	local config_float_anim = CPPGameLib.GetConfig("FloatAnim", float_info[1])
	return self:__InsertFloatAnimInfo(KeyFrameActionType.POS, node, config_float_anim.pos_list, config_float_anim.pos_time_list, float_dir, finished_callback)
end

--停止浮空动画
function ActionManager:StopFloatAnim(float_anim_id)
	if not float_anim_id then
		return
	end

	if self.__float_anim_map then
		local info = self.__float_anim_map[float_anim_id]
		if info then
			if info.finished_callback then
				info.finished_callback()
				info.finished_callback = nil
			end
			CPPActionManager.StopAllActions(info.node)
		end
		self.__float_anim_map[float_anim_id] = nil
	end
end

--跳跃
function ActionManager:RunJumpByAction(node,time,off_x,off_y,height,frequency,funid)
	if not node then
		return
	end

	height = height or 0
	frequency = frequency or 1
	CPPActionManager.JumpBy(node,time,off_x,off_y,height,frequency,funid)
end

--跳跃
function ActionManager:RunJumpByActionEx(node,time,off_x,off_y,height,frequency,func)
	if not node then
		return
	end

	height = height or 0
	frequency = frequency or 1
	CPPActionManager.JumpBy(node,time,off_x,off_y,height,frequency,GlobalCallbackMgr:AddCallBackFunc(func))
end

--移动
function ActionManager:RunMoveBy(node, time, offset_x, offset_y, func)
	if not node then
		return
	end

	time = time or 1
	offset_x = offset_x or 0
	offset_y = offset_y or 0
	CPPActionManager.MoveBy(node, time, offset_x, offset_y, GlobalCallbackMgr:AddCallBackFunc(func))
end
function ActionManager:RunMoveTo(node, time, pos_x, pos_y, func)
	if not node then
		return
	end

	time = time or 1
	pos_x = pos_x or 0
	pos_y = pos_y or 0
	CPPActionManager.MoveTo(node, time, pos_x, pos_y, GlobalCallbackMgr:AddCallBackFunc(func))
end

--移动到某个位置并缩放
function ActionManager:RunScaleToAndMoveToEffect(node,time,pos_x,pos_y,scale,funid)
	if not node then
		return
	end

	scale = scale or 0
	time = time or 1
	CPPActionManager.MoveTo(node,time,pos_x,pos_y,funid)
	CPPActionManager.ScaleTo(node,time,scale)
end

--移动到某个位置并缩放
function ActionManager:RunScaleToAndMoveToEffectEx(node,time,pos_x,pos_y,scale,func)
	if not node then
		return
	end

	scale = scale or 0
	time = time or 1
	CPPActionManager.MoveTo(node,time,pos_x,pos_y,GlobalCallbackMgr:AddCallBackFunc(func))
	CPPActionManager.ScaleTo(node,time,scale)
end

--偏移到某个位置并缩放
function ActionManager:RunScaleToAndMoveByEffectEx(node,time,pos_x,pos_y,scale,func)
	if not node then
		return
	end

	scale = scale or 0
	time = time or 1
	CPPActionManager.MoveBy(node,time,pos_x,pos_y,GlobalCallbackMgr:AddCallBackFunc(func))
	CPPActionManager.ScaleTo(node,time,scale)
end

--淡出
function ActionManager:RunFadeOut(node, time, func)
	time = time or 1
	CPPActionManager.FadeOut(node, time, GlobalCallbackMgr:AddCallBackFunc(func))
end

--淡入
function ActionManager:RunFadeIn(node, time, func)
	CPPActionManager.FadeIn(node, time, GlobalCallbackMgr:AddCallBackFunc(func))
end

function ActionManager:RunFadeTo(node, opacity, time, func)
	CPPActionManager.FadeTo(node, opacity, time, GlobalCallbackMgr:AddCallBackFunc(func))
end

--移动到某个位置并淡出
function ActionManager:RunFadeOutAndMoveToEffect(node,time,pos_x,pos_y,funid)
	if not node then
		return
	end

	time = time or 1
	CPPActionManager.MoveTo(node,time,pos_x,pos_y,funid)
	CPPActionManager.FadeOut(node,time)
end

--移动到某个位置并淡入
function ActionManager:RunFadeInAndMoveToEffect(node,time,pos_x,pos_y,funid)
	if not node then
		return
	end

	time = time or 1
	CPPActionManager.MoveTo(node,time,pos_x,pos_y,funid)
	CPPActionManager.FadeIn(node,time)
end

--移动某偏移并淡出
function ActionManager:RunFadeToAndMoveByEffect(node, time, offset_x, offset_y, opacity, func)
	if not node then
		return
	end

	time = time or 1
	offset_x = offset_x or 0
	offset_y = offset_y or 0
	opacity = opacity or 128
	CPPActionManager.MoveBy(node, time, offset_x, offset_y, GlobalCallbackMgr:AddCallBackFunc(func))
	CPPActionManager.FadeTo(node, opacity, time)
end

--贝塞尔曲线运动
function ActionManager:BezierBy(node, time, end_pos, ctrl_pos1, ctrl_pos2, end_func)
	if not node then
		return
	end

	CPPActionManager.BezierBy(node, time, end_pos, ctrl_pos1, ctrl_pos2, GlobalCallbackMgr:AddCallBackFunc(function()
		if end_func then
			end_func()
		end
	end))	
end

--箭头循环闪烁向上移
function ActionManager:CycleBlinkMoveUpEffect(node,time,off_y)
	if not node then
		return
	end

	off_y = off_y or 6
	local CycleBlinkUpId
	local cycle_one_callbackid = GlobalCallbackMgr:AddCallBackFunc(function ()
		node:SetPosition(node:GetPositionX(),node:GetPositionY() - off_y)
		CPPActionManager.FadeIn(node,0,CycleBlinkUpId)
	end)
	local cycle_two_callback = function ()
		CPPActionManager.JumpBy(node,0.4,0,off_y * 3/4,1,1,GlobalCallbackMgr:AddCallBackFunc(function ()
			CPPActionManager.MoveBy(node,0.3,0,off_y * 1/4,GlobalCallbackMgr:AddCallBackFunc(function ()
				CPPActionManager.FadeOut(node,0.25,cycle_one_callbackid)
			end))
		end))				
	end
	CycleBlinkUpId = GlobalCallbackMgr:AddCallBackFunc(cycle_two_callback)
	cycle_two_callback()
end

--飘字上移
function ActionManager:WindWordsUpslopeEffect(node,pos,funid)
	if not node then
		return
	end	

	CPPActionManager.ScaleTo(node,0.25,1.2,GlobalCallbackMgr:AddCallBackFunc(function ()
		CPPActionManager.ScaleTo(node,0.2,1,GlobalCallbackMgr:AddCallBackFunc(function ()
			CPPActionManager.DelayTo(node,1,GlobalCallbackMgr:AddCallBackFunc(function ()
				self:RunFadeOutAndMoveToEffect(node,1,pos.x,pos.y,funid)
			end))
		end))
	end))
end

--飘字移到制定位置
function ActionManager:WindWordsToLocationEffect(node,pos,funid)
	if not node then
		return
	end
	
	CPPActionManager.ScaleTo(node,0.3,1.1,GlobalCallbackMgr:AddCallBackFunc(function ()
		CPPActionManager.ScaleTo(node,0.2,1,GlobalCallbackMgr:AddCallBackFunc(function ()
			CPPActionManager.DelayTo(node,1,GlobalCallbackMgr:AddCallBackFunc(function ()
				self:RunScaleToAndMoveToEffect(node,0.2,pos.x,pos.y,0,funid)
			end))
		end))
	end))
end

--AI喊话特效
function ActionManager:AISayEffect(node, alive_time, type, end_func)
	if not node then
		return
	end

	alive_time = alive_time or 2.5

   CPPActionManager.DelayTo(node, alive_time, GlobalCallbackMgr:AddCallBackFunc(function()
    	node:RemoveSelf(true)
    	if end_func then
    		end_func()
	    end
    end))
end

--技能喊话
function ActionManager:SkillSayEffect( node , info , alive_time , end_func )
 CPPGameLib.PrintTable("xy" , info , "物品合成响应")

	alive_time = alive_time or 2

	local come = 0.2
	local answer = 0.2
	local deviation = 109
	local posx = node:GetPositionX()

	--是英雄
	local function IsHero( ... )
		-- print("xy" , "IsHero----------------")node:GetPositionX() +  node:GetContentWidth() + deviation
		CPPActionManager.MoveTo(node, come , 0  , node:GetPositionY() )
	end

	--不是英雄
	local function NoHero( ... )
		-- print("xy" , "NoHero----------------" , node:GetPositionX() ,  node:GetContentWidth()  )node:GetPositionX() - node:GetContentWidth() - deviation
		CPPActionManager.MoveTo(node, come , Macros.Global.DesignSize.width , node:GetPositionY() )
	end

	if info.isMonster == true then
		NoHero()
	else
		IsHero()
	end
    CPPActionManager.DelayTo(node, alive_time, GlobalCallbackMgr:AddCallBackFunc(function()
	   	if info.isMonster == true then
			CPPActionManager.MoveTo(node, come , posx , node:GetPositionY() , GlobalCallbackMgr:AddCallBackFunc( function ( ... )
				node:RemoveSelf(true)
				if end_func then
					end_func()
				end
			end ) ) 
		else
			CPPActionManager.MoveTo(node, come , posx , node:GetPositionY() , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
				node:RemoveSelf(true)
				if end_func then
					end_func()
				end
			end) )  
		end	
    end))
end


--战斗飘血特效
function ActionManager:BattleHPEffect(node, finished_callback)
    -- 初始位置：角色坐标向上偏移170像素
    node:SetPosition(node:GetPositionX(),node:GetPositionY()+0)
    -- 初始大小：0.75
    node:SetScaleFactor(0.75)
    -- 初始透明度：0
    CPPActionManager.FadeTo(node, 0, 0 )

	-- 0.2秒大小：1.2
    CPPActionManager.ScaleTo( node , 0.2 , 1.2 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
    	CPPActionManager.ScaleTo( node , 0.3 , 1 )
    end))

    CPPActionManager.FadeIn(node, 0.2)

	CPPActionManager.MoveBy(node, 0.2, 0, 50)
	CPPActionManager.MoveBy(node, 0.5, 0, 0, GlobalCallbackMgr:AddCallBackFunc(function ( ... )
		-- 0.7秒大小：1
	    CPPActionManager.ScaleTo( node , 0.2 , 1)

	    CPPActionManager.FadeOut(node, 0.2)
		-- 0.7秒位置：250
		CPPActionManager.MoveBy(node, 0.2, 0, 30 , GlobalCallbackMgr:AddCallBackFunc( function ( ... )
        	if finished_callback then
        		finished_callback()
        	end
		end) )
	end))
end

--战斗buff特效名字
function ActionManager:BattleSkillNameEffect(node, finished_callback)
    -- 初始位置：
    node:SetPosition(node:GetPositionX(),node:GetPositionY() + 50)
    -- 初始大小：
    node:SetScaleFactor(0.5)
    -- 初始透明度：
    CPPActionManager.FadeTo(node, 0, 0 )

	-- 0.1秒大小：1
    CPPActionManager.FadeIn(node, 0.1 )
    CPPActionManager.ScaleTo( node , 0.1 , 1.5 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )

		CPPActionManager.DelayTo( node , 0.7 , GlobalCallbackMgr:AddCallBackFunc(function ()

			CPPActionManager.MoveBy(node, 0.2 , 0, 20 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )

				CPPActionManager.FadeTo(node , 0.2 , 0 )
				CPPActionManager.MoveBy(node , 0.2 , 0 , 20 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
					if finished_callback then
						finished_callback()
					end
				end) )

			end))

		end))

    end))
end

--战斗队伍中间飘字
function ActionManager:BattleTeamWordsUpslopeEffect(node, render_queue)
	render_queue = render_queue or Scene.RenderQueue.GRQ_UI

	GlobalScene:AddToRenderGroup(node, render_queue)
    PosTool.LeftCenter(node, Macros.Global.DesignSize.width / 4, 0)

    node:SetOpacity(0)	
    CPPActionManager.FadeIn(node, 0.15, GlobalCallbackMgr:AddCallBackFunc(function ()
		CPPActionManager.DelayTo(node, 1, GlobalCallbackMgr:AddCallBackFunc(function ()
			CPPActionManager.FadeOut(node, 0.15, GlobalCallbackMgr:AddCallBackFunc(function ()
				node:RemoveSelf(true)
			end))
		end))
	end))	
end

--战斗QTE评分
function ActionManager:BattleQTEGradeEffect(node, finished_callback)
	if not node then return end
	local x_coe = 1
    node:SetScaleFactor(1.5)
    node:SetOpacity(200)
    CPPActionManager.ScaleTo(node, 0.4 * x_coe, 1.3)
    CPPActionManager.FadeIn(node, 0.4 * x_coe, GlobalCallbackMgr:AddCallBackFunc(function ()
    	CPPActionManager.ScaleTo(node, 1 * x_coe, 1.5)
    	CPPActionManager.DelayTo(node, 1 * x_coe, GlobalCallbackMgr:AddCallBackFunc(function ()
			CPPActionManager.FadeOut(node, 0.5 * x_coe, GlobalCallbackMgr:AddCallBackFunc(function ()
				node:RemoveSelf(true)
			end))
		end))
		if finished_callback then
			finished_callback()
		end
	end))
end

--战斗QT伤害系数
function ActionManager:BattleQTEHarmEffect(node, node1)
	local handle_func = function (handle_node)
		handle_node:SetVisible(true)
		handle_node:SetScaleFactor(0.1)
		CPPActionManager.ScaleTo(handle_node, 0.15, 1.7, GlobalCallbackMgr:AddCallBackFunc(function ()
			CPPActionManager.ScaleTo(handle_node, 0.05, 1, GlobalCallbackMgr:AddCallBackFunc(function ()
				CPPActionManager.DelayTo(handle_node, 0.5, GlobalCallbackMgr:AddCallBackFunc(function ()
					CPPActionManager.FadeOut(handle_node, 0.5, GlobalCallbackMgr:AddCallBackFunc(function ()
						handle_node:RemoveSelf(true)
					end))
				end))
			end))			
		end))
	end
	handle_func(node,true)
	CPPActionManager.DelayTo(node1, 0.2, GlobalCallbackMgr:AddCallBackFunc(function ()
		handle_func(node1)
	end))
end

--界面/模板打开效果
function ActionManager:ViewOrTemplateOpenEffect(obj, finished_callback)
	--全屏遮罩
	local full_screen_layer = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height)
	full_screen_layer:SetTouchEnabled(true)
	full_screen_layer:SetAnchorPoint(0.5, 0.5)
	GlobalScene:AddToRenderGroup(full_screen_layer, Scene.RenderQueue.GRQ_UI_UP)
	PosTool.Center(full_screen_layer)

	CPPActionManager.DelayTo(full_screen_layer, 0.23, GlobalCallbackMgr:AddCallBackFunc(function()
		--移除全屏遮罩
		full_screen_layer:RemoveSelf(true)		
	end))

	--缩放效果
	local node = obj:GetNode()
	node:SetScaleFactor(0)
	CPPActionManager.ScaleTo(node, 0.13, 1.05, GlobalCallbackMgr:AddCallBackFunc(
		function() 
			CPPActionManager.ScaleTo(node, 0.1, 1, GlobalCallbackMgr:AddCallBackFunc(
				function() 
					--恢复背景
					obj:SetBGVisible(true)

					--完成回调
					finished_callback()
				end))
		end))

	--隐藏背景
	obj:SetBGVisible(false)	
end

--主线副本每章节最后的关卡首次通关时，展示世界之心获得效果
function ActionManager:StartWorldStarEffect(story_id, finished_callback)
	if self.__in_world_star_effect then
		return
	end
	self.__in_world_star_effect = true

	local obj_id = GlobalVOMgr:GetDropBoxIDOfWorldStar()
	local obj = GlobalScene:GetObj(obj_id)
	if not obj then
		self:StopWorldStarEffect()
		finished_callback()
		return
	end
	local obj_vo = obj:GetVO()
	local obj_model = obj:GetModel()
	local render_group = GlobalScene:GetRenderGroup(Scene.RenderQueue.GRQ_UI_MAX)
	local beg_world_pos = NodeUnity.ConvertPosBetweenTwoContainer(obj_model:GetNode(), render_group:GetNode(), COCOPoint(obj_vo.world_star_offset.x, obj_vo.world_star_offset.y))
	local end_world_pos = COCOPoint(Macros.Global.DesignSize.width/2, Macros.Global.DesignSize.height/2)
	GlobalScene:DeleteObj(obj_id)

	--全屏遮罩
	self:StopMapBlackingWithLock()
	self:StartMapBlackingWithLock(nil, nil, nil, nil, Scene.RenderQueue.GRQ_UI_UP)

	--世界之心
	local world_star_effect_root
	self.__world_star_effect_id, world_star_effect_root = GlobalEffectMgr:CreateScenePosEffect(
		{res_name = obj_vo.drop_info.img_name, pos_x = beg_world_pos.x, pos_y = beg_world_pos.y, loop = true, scale = 0.2, zorder = 2}, Scene.RenderQueue.GRQ_UI_MAX) 

	--飞到屏幕中央
	self:RunScaleToAndMoveToEffectEx(world_star_effect_root, 1, end_world_pos.x, end_world_pos.y, 1, function()
		--背景光效
		local world_star_bg_effect_root
		self.__world_star_bg_effect_id, world_star_bg_effect_root = GlobalEffectMgr:CreateScenePosEffect({
			id = 100000, pos_x = end_world_pos.x, pos_y = end_world_pos.y, zorder = 1, loop = true}, Scene.RenderQueue.GRQ_UI_MAX)

		--获得字样
		self.__lab_world_star = Util:Label(CPPGameLib.GetString("battle_world_star_get"), Macros.TypefaceSize.headlineII, Macros.Color.orange, nil, nil, Macros.Color.orange_stroke)
		self.__lab_world_star:SetZOrder(3)
		self.__lab_world_star:SetAnchorPoint(0.5, 1)
		GlobalScene:AddToRenderGroup(self.__lab_world_star, Scene.RenderQueue.GRQ_UI_UP)
		PosTool.CenterTop(self.__lab_world_star, 0, -100)

		--点击屏幕继续
		self.__lab_continue = Util:Label(CPPGameLib.GetString("common_continue"), Macros.TypefaceSize.headlineII, Macros.Color.orange, nil, nil, Macros.Color.common_stroke)
		self.__lab_continue:SetZOrder(3)
		self.__lab_continue:SetAnchorPoint(0.5, 1)
		GlobalScene:AddToRenderGroup(self.__lab_continue, Scene.RenderQueue.GRQ_UI_UP)
		PosTool.CenterBottom(self.__lab_continue, 0, 140)

		WidgetUnity.SetWidgetListener(self.__map_blacking_bg, nil, nil, function()
			WidgetUnity.SetWidgetListener(self.__map_blacking_bg)
			if self.__lab_continue then
				self.__lab_continue:RemoveSelf(true)
				self.__lab_continue = nil
			end
			if self.__lab_world_star then
				self.__lab_world_star:RemoveSelf(true)
				self.__lab_world_star = nil
			end			

			world_star_effect_root:SetScaleFactor(0.6)
			world_star_bg_effect_root:SetScaleFactor(0.6)

			local function world_star_story_end_func()
				CPPActionManager.ScaleTo(world_star_bg_effect_root, 0.2, 0, GlobalCallbackMgr:AddCallBackFunc(function()
					GlobalEffectMgr:DelEffect(self.__world_star_bg_effect_id)
					self.__world_star_bg_effect_id = nil

					--飞到指定位置
					local flyto_x = end_world_pos.x + 300
					local flyto_y = 30
					local battle_view = CallModuleFunc(ModuleType.BATTLE, "GetView")
					if battle_view and battle_view:IsOpen() then
						local panel_tgt = battle_view:GetWidget("Panel_Anger")
						if panel_tgt then
							end_world_pos = NodeUnity.ConvertPosBetweenTwoContainer(panel_tgt, render_group:GetNode(), COCOPoint(panel_tgt:GetContentWidth()/2, panel_tgt:GetContentHeight()/2))
							flyto_x = end_world_pos.x
							flyto_y = end_world_pos.y
						end
					end
					self:RunScaleToAndMoveToEffectEx(world_star_effect_root, 0.5, flyto_x, flyto_y, 0, function()
	
						--播放闪光特效
						self.__world_star_bg_effect_id, world_star_bg_effect_root = GlobalEffectMgr:CreateScenePosEffect({
							id = 100028, pos_x = flyto_x, pos_y = flyto_y, scale = 0.2, zorder = 1, loop = true}, Scene.RenderQueue.GRQ_UI_UP)											

						CPPActionManager.ScaleTo(world_star_bg_effect_root, 0.2, 0.4, GlobalCallbackMgr:AddCallBackFunc(function()
							CPPActionManager.ScaleTo(world_star_bg_effect_root, 0.2, 0, GlobalCallbackMgr:AddCallBackFunc(function()
								--结束回调
								self:StopWorldStarEffect()
								finished_callback()								
							end))
						end))
					end)				
				end))
			end

			--播放世界之心剧情
			if story_id then
				CallModuleFunc(ModuleType.STORY, "Start", story_id, nil, function()
					world_star_story_end_func()
				end)
			else
				world_star_story_end_func()
			end
		end)
	end)

	return true
end
function ActionManager:StopWorldStarEffect()
	if not self.__in_world_star_effect then
		return
	end
	self.__in_world_star_effect = false

	self:StopMapBlackingWithLock()

	if self.__world_star_effect_id then
		GlobalEffectMgr:DelEffect(self.__world_star_effect_id)
		self.__world_star_effect_id = nil
	end

	if self.__world_star_bg_effect_id then
		GlobalEffectMgr:DelEffect(self.__world_star_bg_effect_id)
		self.__world_star_bg_effect_id = nil
	end

	if self.__lab_continue then
		self.__lab_continue:RemoveSelf(true)
		self.__lab_continue = nil
	end

	if self.__lab_world_star then
		self.__lab_world_star:RemoveSelf(true)
		self.__lab_world_star = nil
	end	
end
function ActionManager:IsInWorldStarEffect()
	return self.__in_world_star_effect
end

--盖章效果
function ActionManager:Stamp(node, func , SC1 , SC2 , SC3 , notShow )
	node:SetScaleFactor( SC1 or 5 )
	if not notShow then
		node:SetVisible(true)
	end
	CPPActionManager.ScaleTo(node, 0.2, SC2 or 0.9 , GlobalCallbackMgr:AddCallBackFunc(function( ... )
		CPPActionManager.ScaleTo(node, 0.2, SC3 or 1 , GlobalCallbackMgr:AddCallBackFunc(function( ... )
			if func then
				func()
			end
		end) )
	end) )
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--插入头像动画
function ActionManager:__InsertHeadAnimInfo(action_type, node, anim_list, time_list)
	if not node then
		return
	end
	
	local info = self:__CreateKeyframeAnimInfo(KeyFrameAnimType.HEAD_ANIM, action_type, node, anim_list, time_list)
	CPPNodeUnity.Retain(info.node)

	self.__head_anim_list = self.__head_anim_list or {}
	table.insert(self.__head_anim_list, info)
end

--插入浮空动画
function ActionManager:__InsertFloatAnimInfo(action_type, node, anim_list, time_list, float_dir, finished_callback)
	if not node then
		return
	end
	
	local info = self:__CreateKeyframeAnimInfo(KeyFrameAnimType.FLOAT_ANIM, action_type, node, anim_list, time_list, finished_callback)
	info.float_dir = float_dir or info.float_dir

	self.__float_anim_id = self.__float_anim_id or 0
	self.__float_anim_id = self.__float_anim_id + 1
	self.__float_anim_map = self.__float_anim_map or {}
	self.__float_anim_map[self.__float_anim_id] = info

	return self.__float_anim_id
end

--创建关键帧动画信息
function ActionManager:__CreateKeyframeAnimInfo(anim_type, action_type, node, anim_list, time_list, finished_callback)
	local info = {}
	info.anim_type = anim_type
	info.action_type = action_type
	info.node = node
	info.anim_list = anim_list
	info.time_list = time_list
	info.time_index = 1
	info.change_index = true
	info.float_dir = 1
	info.finished_callback = finished_callback
	return info
end

--初始化关键帧动画
function ActionManager:__InitKeyframeAnimInfo(info)
	anim_type = info.anim_type

	if anim_type == KeyFrameAnimType.HEAD_ANIM then
		action_type = info.action_type
		node = info.node
		anim_info = info.anim_list[1]

		if not node:GetParent() then
			node:SetAnchorPoint(0.5, 0.5)
			GlobalScene:AddToRenderGroup(node, Scene.RenderQueue.GRQ_UI_UP)
		end
		if action_type == KeyFrameActionType.POS then
			PosTool.Center(node, anim_info[1], anim_info[2])

		elseif action_type == KeyFrameActionType.OPACITY then
			node:SetOpacity(anim_info)
		end
	end
end

--执行关键帧动作
function ActionManager:__DoKeyframeAction(action_type, node, cur_anim_info, next_anim_info, anim_time, float_dir, func)
	if action_type == KeyFrameActionType.POS then
		CPPActionManager.MoveBy(node, anim_time, float_dir * (next_anim_info[1] - cur_anim_info[1]), next_anim_info[2] - cur_anim_info[2], GlobalCallbackMgr:AddCallBackFunc(func))

	elseif action_type == KeyFrameActionType.OPACITY then
		CPPActionManager.FadeTo(node, next_anim_info, anim_time, GlobalCallbackMgr:AddCallBackFunc(func))
	end
end

--更新关键帧动画
function ActionManager:__UpdateKeyframeAction(info, elapse_time)
	if info.is_finished then
		return
	end

	if info.change_index then
		--初始动画
		if info.time_index == 1 then
			self:__InitKeyframeAnimInfo(info)
		
		--结束动画
		elseif info.time_index >= #info.time_list then
			info.is_finished = true
			return
		end

		local cur_anim_info = info.anim_list[info.time_index]
		local cur_anim_time = info.time_list[info.time_index]
		local next_anim_info = info.anim_list[info.time_index + 1]
		local next_anim_time = info.time_list[info.time_index + 1]
		local float_dir = info.float_dir
		--执行动画
		self:__DoKeyframeAction(info.action_type, info.node, cur_anim_info, next_anim_info, next_anim_time - cur_anim_time, float_dir, function()
			info.change_index = true
		end)		

		--切换到下一个关键帧
		info.time_index = info.time_index + 1
		info.change_index = false
	end
end

--更新头像动画
function ActionManager:__UpdateHeadAnim(elapse_time)
	if self.__head_anim_playing then
		local is_all_finished = true

		--更新动画
		for i, info in ipairs(self.__head_anim_list) do
			self:__UpdateKeyframeAction(info, elapse_time)

			--完成检测
			if not info.is_finished then
				is_all_finished = false
			end
		end

		if is_all_finished then
			self.__head_anim_playing = false

			remove_head_anim_blacking_bg(self)

			--移除节点、解除引用
			for i, info in ipairs(self.__head_anim_list) do
				info.node:RemoveSelf(true)
				CPPNodeUnity.Release(info.node)
			end
			self.__head_anim_list = nil
			
			--完成回调
			if self.__head_anim_finished_callback then
				self.__head_anim_finished_callback()
				self.__head_anim_finished_callback = nil
			end
		end
	end
end

--暂停头像动画
function ActionManager:__PauseHeadAnim()
	if self.__head_anim_playing then
		for _, info in ipairs(self.__head_anim_list) do
			CPPActionManager.PauseAction(info.node)
		end
	end
end

--恢复头像动画
function ActionManager:__ResumeHeadAnim()
	if self.__head_anim_playing then
		for _, info in ipairs(self.__head_anim_list) do
			CPPActionManager.ResumeAction(info.node)
		end
	end
end

--销毁头像动画
function ActionManager:__DumpHeadAnim()
	if self.__head_anim_playing then
		self.__head_anim_playing = false
		self.__head_anim_finished_callback = nil

		remove_head_anim_blacking_bg(self)

		--移除节点、解除引用
		for _, info in ipairs(self.__head_anim_list) do
			info.node:RemoveSelf(true)
			CPPNodeUnity.Release(info.node)			
		end
		self.__head_anim_list = nil

		if self.__head_anim_effect_id then
			GlobalEffectMgr:DelEffect(self.__head_anim_effect_id)
			self.__head_anim_effect_id = nil
		end
	end
end

--更新浮空动画
function ActionManager:__UpdateFloatAnim(elapse_time)
	if self.__float_anim_map then
		local del_list

		--更新动画
		for i, info in pairs(self.__float_anim_map) do
			self:__UpdateKeyframeAction(info, elapse_time)

			--完成检测
			if info.is_finished then
				if info.finished_callback then
					info.finished_callback()
					info.finished_callback = nil
				end
				del_list = del_list or {}
				table.insert(del_list, i)
			end
		end

		--移除已完成项
		if del_list then
			for _, del_key in ipairs(del_list) do
				self.__float_anim_map[del_key] = nil
			end
		end
	end
end

--暂停浮空动画
function ActionManager:__PauseFloatAnim()
	if self.__float_anim_map then
		for _, info in pairs(self.__float_anim_map) do
			CPPActionManager.PauseAction(info.node)
		end
	end
end

--恢复浮空动画
function ActionManager:__ResumeFloatAnim()
	if self.__float_anim_map then
		for _, info in pairs(self.__float_anim_map) do
			CPPActionManager.ResumeAction(info.node)
		end
	end
end

--销毁浮空动画
function ActionManager:__DumpFloatAnim()
	self.__float_anim_id = nil
	self.__float_anim_map = nil
end

--------------------------------------------------------------------------------------------------------------------------------
--私有函数
--------------------------------------------------------------------------------------------------------------------------------
remove_map_blacking_bg = function(obj_self)
	if obj_self.__map_blacking_bg then
		obj_self.__map_blacking_bg:RemoveSelf(true)
		obj_self.__map_blacking_bg = nil
	end
end

remove_head_anim_blacking_bg = function(obj_self)
	if obj_self.__head_anim_blacking_bg then
		obj_self.__head_anim_blacking_bg:RemoveSelf(true)
		obj_self.__head_anim_blacking_bg = nil
	end
end
