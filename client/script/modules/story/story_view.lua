
--[[
%% @module: 剧情界面
%% @author: swordman sue
%% @created: 2016-01-7
--]]

Modules = Modules or {}

Modules.StoryView = Modules.StoryView or BaseClass(BaseView)

--背景类型
local BGType = 
{
	ALPHA_BLACK = 0,
	SOLID_BLACK = 1,
}

--位置类型
local PosType = 
{
	LEFT = 0,
	RIGHT = 1,
	CENTER = 2,
}

--模型类型
local ModelType = 
{
	--动画
	ANIM = 0,

	--精灵
	SPRITE = 1,

	--半身像
	HALF_HEAD = 2,
}

--模型子类型
local ModelSubType = 
{
	MONSTER = 0,
	HERO = 1,
}

local ModelWidth = 256
local ModelPositionX = 0
local ModelPositionY = 123
local model_pos_type_list = {PosType.LEFT, PosType.RIGHT, PosType.CENTER}

--字幕间隔
local caption_interval = 5

--字体大小
local font_size = 30

--步骤动画时间
local section_operate_anim_time = 0.4

--处理模型焦点
local handle_model_focus

--对话框入场时间
local dialog_enter_time = 0.4

--对话行数
local max_dialog_content_line_num = 2
--对话内容入场时间
local dialog_content_enter_time = 1
--对话内容行高度
local dialog_content_line_height = 26
--对话内容行间隔
local dialog_content_line_interval = 10

function Modules.StoryView:__init(render_queue)
	self.__layout_name = "story"
	self.__is_force_stretch = true
	self.__ignore_iphone_safe_area = true

	self.__render_queue = render_queue or Scene.RenderQueue.GRQ_UI_UP_II
	self.__loading_view_type = BaseViewType.UI_LOADING

	self.is_auto_close = false

    self.__loaded_callback = function()
    	self:__LoadedCallback()
    end

    self.__open_callback = function()
    	self:__OpenCallback()
    end
end

function Modules.StoryView:__LoadedCallback()
	self.__panel_section_operate = self:GetWidget("Panel_Action")
	--裁剪窗口
	self.__panel_captions = self:GetWidget("Panel_Captions")
	self.__img_skip = self:GetWidget("Image_Skip")
	WidgetUnity.SetWidgetListener(self.__img_skip, nil, nil, function()
		self:__Skip()
	end)

	self.__img_continue = self:GetWidget("Image_Continue")
	ActionManager.GetInstance():UpDownMoveAnimation(self.__img_continue, 0.5)

	--对话英雄名
	self.__text_dialog_name = Util:WidgetLabel(self:GetWidget("Text_CharacterName"), "", Macros.TypefaceSize.headline, Macros.Color.Special, nil, nil, Macros.Color.common_stroke)

	self.__img_dialog = self:GetWidget("Image_Dialog")
	self.__img_dialog:SetVisible(false)

	--特效
	if self:CallModuleFunc("GetOnlineMark") then
		self.__img_dialog:SetZOrder(2)
	    local effect
	    self.__online_reward_push_effectid,effect = self:__CreateEffect({id = 100096},self.__panel_section_operate,1)
	    PosTool.Center(effect)
	end

	--对话遮罩容器
	self.__mask_dialog_container = Node.Create()
	self.__mask_dialog_container:SetContentSize(754, max_dialog_content_line_num * (dialog_content_line_height + dialog_content_line_interval) - dialog_content_line_interval)
	self.__mask_dialog_container:SetAnchorPoint(0.5, 0.5)

	--对话裁剪容器
	local clipping_node_dialog = ClippingNodeEx.Node()
	clipping_node_dialog:SetAlphaTestValue(0)
	clipping_node_dialog:SetStencilMask(self.__mask_dialog_container)
	self.__img_dialog:AddChild(clipping_node_dialog)
	PosTool.Center(clipping_node_dialog, 0, -32)

	--对话文本
	self.__rtext_dialog_content = Util:RichText("", Macros.TypefaceSize.largish, 754)
	self.__rtext_dialog_content:SetFontSize(Macros.TypefaceSize.popup)
	self.__rtext_dialog_content:SetFontSpacing(dialog_content_line_interval)
	self.__rtext_dialog_content:SetAnchorPoint(0, 1)
	clipping_node_dialog:AddChild(self.__rtext_dialog_content)

	WidgetUnity.SetWidgetListener(self.__root_widget, nil, nil, function()
		self:__ChangeCurLabMovingSpeed()
		self:__HandleFullScreenTouch()
	end, nil, nil, 100)
end

function Modules.StoryView:__OpenCallback()
	local story_info = self:CallModuleFunc("GetStoryInfo")
	if story_info.show_world_star_effect == Macros.Global.TRUE then
		self:__StartWorldStarEffect()
	else
		self:__Start()
	end
end

function Modules.StoryView:__Dispose()
	if self.__model_info_list then
		for _, model_info in pairs(self.__model_info_list) do
			CPPGameLib.DeleteObj(model_info.model)
		end
		self.__model_info_list = nil
	end

	if self.__del_model_info_map then
		for _, model_info in pairs(self.__del_model_info_map) do
			CPPGameLib.DeleteObj(model_info.model)			
		end
		self.__del_model_info_map = nil
	end

	if self.__section_operate_timer then
		GlobalTimerQuest:CancelQuest(self.__section_operate_timer)
		self.__section_operate_timer = nil
	end

	if self.__stop_timer then
		GlobalTimerQuest:CancelQuest(self.__stop_timer)
		self.__stop_timer = nil
	end

	if self.__freezon_timer then
		GlobalTimerQuest:CancelQuest(self.__freezon_timer)
		self.__freezon_timer = nil
	end

	if self.__world_star_effect_id then
		GlobalEffectMgr:DelEffect(self.__world_star_effect_id)
		self.__world_star_effect_id = nil
	end

	if self.__world_star_bg_effect_id then
		GlobalEffectMgr:DelEffect(self.__world_star_bg_effect_id)
		self.__world_star_bg_effect_id = nil
	end

	if self.__online_reward_push_effectid then
		self:__DelEffect(self.__online_reward_push_effectid)
		self.__online_reward_push_effectid = nil
	end
end

--改变速度
function Modules.StoryView:__ChangeCurLabMovingSpeed()
	if not self.__is_on_running_caption then
		return
	end
	--是否移动
	if self.__is_cur_lab_layout_moving then
		--是否已经加速
		if not self.__is_have_add_speed then
			self.__is_have_add_speed = true
		else
			return
		end
		--停止之前的动作
		if self.__cur_lab_layout then
			CPPActionManager.StopAllActions(self.__cur_lab_layout)
			self.__cur_lab_layout:SetPosition(self.__panel_captions:GetContentWidth()/2,self.__cur_move_height)
			self.__is_cur_lab_layout_moving = false
			if self.__continute_lab then
				self.__continute_lab:SetVisible(true)
			end
		end
	else
		self:__FadeOutCurCaptions()
	end
end

--结束剧情
function Modules.StoryView:Stop(finished_callback)
	if self.__stop_timer then
		return
	end

	self.__stop_finished_callback = finished_callback
	self.__img_skip:SetVisible(false)
	self.__img_continue:SetVisible(false)

	self.__stop_timer = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
		self:__Stop(true)
	end), section_operate_anim_time)

	--对话框退场
	self:__ExitDialog(section_operate_anim_time)

	--模型退场
	self:__ExitModels(section_operate_anim_time)
end

--剧情步骤
function Modules.StoryView:DoSectionOperate()
	local section_info = self:CallModuleFunc("GetSectionInfo")
	if not section_info then
		return
	end

	if self.__freezon_timer then
		GlobalTimerQuest:CancelQuest(self.__freezon_timer)
		self.__freezon_timer = nil
	end

	--背景
	self:__CreateBG(section_info)

	--模型
	local new_model_info = self:__CreateModel(section_info)
	if new_model_info then
		--模型焦点处理
		for _, model_info in pairs(self.__model_info_list) do
			if new_model_info.pos_type ~= model_info.pos_type then
				handle_model_focus(model_info, false)
			else
				handle_model_focus(model_info, true)
			end
		end

		if new_model_info.pos_type == PosType.CENTER then
			--对话框退场
			self:__ExitDialog(section_operate_anim_time, true)
		end
	end

	--对话框
	local dialog_anim_time = self:__CreateDialog(section_info)

	--字幕
	self:__CreateCaptions(section_info)

	--步骤动画完成，重置位置
    self.__section_operate_timer = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
    	self.__section_operate_timer = nil
    	self:__SkipSectionOperate()
    end), math.max(section_operate_anim_time, dialog_anim_time))
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--开始剧情步骤
function Modules.StoryView:__Start()
	local story_info = self:CallModuleFunc("GetStoryInfo")
	if story_info.can_skip == Macros.Global.TRUE then
		self.__img_skip:SetVisible(true)
		self.__img_skip:SetOpacity(0)
		ActionManager.GetInstance():RunFadeIn(self.__img_skip)
	else
		self.__img_skip:SetVisible(false)
	end

	--模型/精灵列表
	self.__model_info_list = {}

	self.__is_started = true

	--下一个步骤
	self:CallModuleFunc("NextSection")
end

function Modules.StoryView:__Stop(check_world_star_effect)
	if check_world_star_effect then
		local story_info = self:CallModuleFunc("GetStoryInfo")
		if story_info and story_info.show_world_star_effect == Macros.Global.TRUE then
			self:__StopWorldStarEffect()
			return
		end
	end

	self:CloseManual()

	if self.__stop_finished_callback then
		self.__stop_finished_callback()
	end
end

--跳过剧情
function Modules.StoryView:__Skip()
	if not self.__is_started then
		return
	end
	
	self:CallModuleFunc("Stop")
end

--跳过剧情步骤
function Modules.StoryView:__SkipSectionOperate()
	local section_info = self:CallModuleFunc("GetSectionInfo")

	--对话
	if section_info and section_info.dialog_info then
		if self.__mask_dialog_node_list then
			for i, mask_dialog_node in ipairs(self.__mask_dialog_node_list) do
				CPPActionManager.StopAllActions(mask_dialog_node)
				PosTool.LeftTop(mask_dialog_node, 0, -(i - 1) * (dialog_content_line_height + dialog_content_line_interval))
			end	
		end
		self.__img_continue:SetVisible(true)
		self.__rtext_dialog_content:SetVisible(true)
	end

	--模型
	for _, pos_type in ipairs(model_pos_type_list) do
		local model_info = self.__model_info_list[pos_type]
		if model_info then
			CPPActionManager.StopAllActions(model_info.node)
			model_info.node:SetPosition(model_info.tgt_x, model_info.tgt_y)
		end
	end
end

--继续剧情
function Modules.StoryView:__Continue()
	if not self.__is_started then
		return
	end

	self:CallModuleFunc("NextSection")	
end

--全屏点击处理
function Modules.StoryView:__HandleFullScreenTouch()
	if not self.__is_started then
		return
	end

	if self.__stop_timer then
		GlobalTimerQuest:CancelQuest(self.__stop_timer)
		self.__stop_timer = nil		

		--跳过退场动画
		self:__Stop(true)
		return
	end

	if self.__section_operate_timer then
		GlobalTimerQuest:CancelQuest(self.__section_operate_timer)
		self.__section_operate_timer = nil

		--跳过步骤动画
		self:__SkipSectionOperate()
		return
	end

	if not self.__is_on_running_caption then
		self:__Continue()
	end
end

--创建背景
function Modules.StoryView:__CreateBG(section_info)
	local bg_type = section_info.bg_type or BGType.ALPHA_BLACK
	if not self.__layout_bg then
		local size = self.__root_widget:GetContentSize()
		self.__layout_bg = Util:Layout(size.width, size.height)
		self.__layout_bg:SetTouchEnabled(false)
		self.__layout_bg:SetAnchorPoint(0.5, 0.5)
		self.__layout_bg:SetZOrder(-1)
		Util:SetLayoutBGColor(self.__layout_bg, Macros.Color.lilyBlack)
		self.__panel_section_operate:AddChild(self.__layout_bg)
		PosTool.Center(self.__layout_bg)
	end

	if bg_type == BGType.ALPHA_BLACK then
		self.__layout_bg:SetOpacity(204)
	else
		self.__layout_bg:SetOpacity(255)
	end
end

--创建对话框
function Modules.StoryView:__CreateDialog(section_info)
	self.__img_continue:SetVisible(false)
	self.__img_dialog:SetVisible(false)

	local dialog_info = section_info.dialog_info
	if not dialog_info then
		return 0
	end

	self.__img_dialog:SetVisible(true)

	--更新对话信息
	local size = self.__mask_dialog_container:GetContentSize()
	--内容
	self.__rtext_dialog_content:SetText(GUI.RichText.GetColorWord(dialog_info[2], Macros.Color.lead_story_def_hex))	
	self.__rtext_dialog_content:SetVisible(false)
	PosTool.Center(self.__rtext_dialog_content, -size.width / 2, size.height / 2)

	local rtext_size = self.__rtext_dialog_content:GetContentSize()
	local dialog_content_line_num = self.__rtext_dialog_content:GetRowNum()
	local function update_dialog_content()
		--内容
		self.__rtext_dialog_content:SetVisible(true)

		--名字
		self.__text_dialog_name:SetText(dialog_info[1] .. CPPGameLib.GetString("common_colon"))

		--创建遮罩条目
		local mask_dialog_node
		for i = 1, dialog_content_line_num do
			self.__mask_dialog_node_list = self.__mask_dialog_node_list or {}
			mask_dialog_node = self.__mask_dialog_node_list[i]
			if not mask_dialog_node then
				mask_dialog_node = Util:Layout(size.width, dialog_content_line_height)
				mask_dialog_node:SetAnchorPoint(0, 1)
				self.__mask_dialog_container:AddChild(mask_dialog_node)
				Util:SetLayoutBGColor(mask_dialog_node, Macros.Color.lilyBlack)
				table.insert(self.__mask_dialog_node_list, mask_dialog_node)
			end

			PosTool.LeftTop(mask_dialog_node, -size.width, -(i - 1) * (dialog_content_line_height + dialog_content_line_interval))
		end

		--执行遮罩条目动画
		local mdn_index = 1
		local function do_dialog_content_anim()
			if mdn_index > dialog_content_line_num then
				self.__img_continue:SetVisible(true)
				return
			end

			ActionManager.GetInstance():RunMoveBy(self.__mask_dialog_node_list[mdn_index], dialog_content_enter_time, size.width, nil, function()
				do_dialog_content_anim()
			end)
			mdn_index = mdn_index + 1
		end
		do_dialog_content_anim()
	end

	if not self.__is_dialog_entered then
		--对话框入场
		self.__is_dialog_entered = true
		local jump_offset_y = self.__img_dialog:GetContentHeight()
		PosTool.CenterBottom(self.__img_dialog, 0, -jump_offset_y)
		ActionManager.GetInstance():RunJumpByActionEx(self.__img_dialog, dialog_enter_time, 0, jump_offset_y, nil, nil, function()
			update_dialog_content()						
		end)
		return dialog_enter_time + dialog_content_enter_time * dialog_content_line_num
	else
		update_dialog_content()
		return dialog_content_enter_time * dialog_content_line_num
	end
end

--对话框退场
function Modules.StoryView:__ExitDialog(duration, cleanup)
	if self.__is_dialog_entered then
		self.__is_dialog_entered = false
		local jump_offset_y = -self.__img_dialog:GetContentHeight()
		ActionManager.GetInstance():RunJumpByActionEx(self.__img_dialog, duration, 0, jump_offset_y)
	end
end

--对比模型信息
local function compare_model_info(info1, info2)
	if not info1 or not info2 then
		return false
	end
	if info1[1] ~= info2[1] then
		return false
	end

	local compare_num = info1[1] == ModelType.ANIM and 3 or 2
	for i = 1, compare_num do
		if info1[i] ~= info2[i] then
			return false
		end
	end
	return true
end
--创建模型
function Modules.StoryView:__CreateModel(section_info)
	local pos_type = section_info.pos_type or PosType.LEFT
	local model_params = section_info.model_info
	local old_model_info = self.__model_info_list[pos_type]
	local is_same_model_params = compare_model_info(old_model_info and old_model_info.model_params, model_params)
	local do_enter_anim
	local tgt_offset_x = 0
	local tgt_offset_y = 0	

	if not model_params then
		return
	end

	--居中模型退场动画(1、从居中模型切换到居中模型，2、从居中模型切换到其他模型)
	self:__DeleteModel(PosType.CENTER, section_operate_anim_time, true)

	--其他模型退场动画(1、从其他模型切换到居中模型)
	if pos_type == PosType.CENTER then
		self:__ExitModels(section_operate_anim_time)
	end

	--清空旧模型相关
	if old_model_info then
		--模型参数不一致，清空旧模型相关
		if not is_same_model_params then
			CPPGameLib.DeleteObj(old_model_info.model)
			old_model_info.model = nil
		end
	end

	--构造新模型相关
	local new_model_info = old_model_info or {}
	new_model_info.pos_type = pos_type
	new_model_info.model_params = model_params

	--创建模型根节点
	if not new_model_info.node then
		new_model_info.node = Util:Layout()
		self.__panel_section_operate:AddChild(new_model_info.node)
	end
	self.__model_info_list[pos_type] = new_model_info

	--创建模型动画/精灵
	if model_params then
		do_enter_anim = true

		local scale
		local model_type = model_params[1]

		--模型动画
		if model_type == ModelType.ANIM then
			--0|模型类型（0怪物 1英雄）|ID|动作索引|帧数时间|缩放系数|目标点X偏移|目标点Y偏移
			local act_index = model_params[4] or RenderObj.ActionIndex.IDLE
			local frame_time = model_params[5] or 0
			tgt_offset_x = model_params[7] or 0
			tgt_offset_y = model_params[8] or 0
			scale = model_params[6] or 1

			if not new_model_info.model then
				local sub_model_type = model_params[2] == ModelSubType.HERO and LogicGUI.ModelType.HERO or LogicGUI.ModelType.MONSTER
				local dir = pos_type == PosType.LEFT and MathTool.HORZ_RIGHT or MathTool.HORZ_LEFT
				new_model_info.model = LogicGUI.Model.New({info_id = model_params[3], parent = new_model_info.node, model_type = sub_model_type, act_index = act_index, dir = dir, fixed_frame_time = frame_time})					
			else
				new_model_info.model:SetToPlayTime(frame_time, act_index)
			end

		--模型精灵
		elseif model_type == ModelType.SPRITE then
			--1|图片名|目标点X偏移|目标点Y偏移|后缀|特效ID
			local res_name = Resource.PathTool.GetStorySpritePath(model_params[2], model_params[6] or "sdz")
			tgt_offset_x = model_params[4] or 0
			tgt_offset_y = model_params[5] or 0
			scale = model_params[3] or 1

			if not new_model_info.model then
				new_model_info.model = Util:Sprite(res_name)
				new_model_info.model:SetAnchorPoint(0.5, pos_type == PosType.CENTER and 0.5 or 0)
				new_model_info.model:SetFlipX(pos_type == PosType.RIGHT)
				new_model_info.node:AddChild(new_model_info.model)
			end

			if not new_model_info.effect_id and model_params[7] then
				local effect, effect_scale
				effect_scale = 1 / scale
				new_model_info.effect_id, effect = self:__CreateEffect({id = model_params[7], scale = effect_scale}, new_model_info.model)
				PosTool.Center(effect, 0, 0)
			end

			if section_info.freezon_time then	
			    self.__freezon_timer = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(
			    function()
			    	if not new_model_info.node then
			    		return
			    	end
					local lab_freezon_tips = Util:Label(CPPGameLib.GetString("common_continue"), Macros.TypefaceSize.popup, Macros.Color.headline)
					new_model_info.node:AddChild(lab_freezon_tips)
					PosTool.Center(lab_freezon_tips, 0, -290)					
			    end), section_info.freezon_time)
			end

		--半身像
		elseif model_type == ModelType.HALF_HEAD then
			--2|图片名|缩放系数|目标点X偏移|目标点Y偏移|是否显示模型切换动作
			local res_name = Resource.PathTool.GetHalfHeadPath(model_params[2])
			tgt_offset_x = model_params[4] or 0
			tgt_offset_y = model_params[5] or 0
			scale = model_params[3] or 1
			do_enter_anim = not model_params[6] and true or (model_params[6] == Macros.Global.TRUE)

			if not new_model_info.model then
				new_model_info.model = Util:Sprite(res_name)
				new_model_info.model:SetAnchorPoint(pos_type == PosType.RIGHT and 1 or 0, 0)
				new_model_info.model:SetFlipX(pos_type == PosType.RIGHT)
				new_model_info.node:AddChild(new_model_info.model)
			end
		end

		new_model_info.model:SetScaleFactor(scale)
	end

	--计算起止位置、退场偏移
	if pos_type ~= PosType.CENTER then
		--非居中
		if pos_type == PosType.LEFT then
			new_model_info.src_x = -ModelPositionX - ModelWidth
			new_model_info.tgt_x = ModelPositionX + tgt_offset_x
			new_model_info.exit_offset = -new_model_info.tgt_x - ModelPositionX - ModelWidth
		else
			new_model_info.src_x = PosTool.RightBottom(new_model_info.node, ModelPositionX + ModelWidth, nil, nil, true)
			new_model_info.tgt_x = PosTool.RightBottom(new_model_info.node, -ModelPositionX + tgt_offset_x, nil, nil, true)	
			new_model_info.exit_offset = self.__panel_section_operate:GetContentWidth() - new_model_info.tgt_x + ModelPositionX + ModelWidth
		end
		new_model_info.src_y = ModelPositionY + tgt_offset_y
		new_model_info.tgt_y = new_model_info.src_y
	else
		--居中
		new_model_info.src_x, new_model_info.src_y = PosTool.Center(new_model_info.node, -self.__panel_section_operate:GetContentWidth()/2, tgt_offset_y, nil, true)
		new_model_info.tgt_x, new_model_info.tgt_y = PosTool.Center(new_model_info.node, tgt_offset_x, tgt_offset_y, nil, true)
		new_model_info.exit_offset = 1.5 * self.__panel_section_operate:GetContentWidth() - new_model_info.tgt_x
	end

	--模型进场动画
	if do_enter_anim then
		--模型参数不一致，播放进场动画
		if not is_same_model_params then
			new_model_info.node:SetPosition(new_model_info.src_x, new_model_info.src_y)
			ActionManager.GetInstance():RunJumpByAction(new_model_info.node, section_operate_anim_time, new_model_info.tgt_x - new_model_info.src_x, 0)
		end
	end

	return new_model_info
end

--销毁模型
function Modules.StoryView:__DeleteModel(pos_type, action_duration, width_action)
	local model_info = self.__model_info_list and self.__model_info_list[pos_type]
	if model_info then
		if not width_action then
			--不带动作
			CPPGameLib.DeleteObj(model_info.model)
			model_info.node:RemoveSelf(true)
			model_info.node = nil

			if model_info.effect_id then
				self:__DelEffect(model_info.effect_id)
				model_info.effect_id = nil
			end
		else
			self.__del_model_index = self.__del_model_index or 0
			self.__del_model_index = self.__del_model_index + 1
			self.__del_model_info_map = self.__del_model_info_map or {}
			self.__del_model_info_map[self.__del_model_index] = model_info

			--带动作
			local del_model_index = self.__del_model_index
			ActionManager.GetInstance():RunJumpByActionEx(model_info.node, action_duration, model_info.exit_offset, 0, nil, nil, function()
				CPPGameLib.DeleteObj(model_info.model)
				model_info.node:RemoveSelf(true)
				model_info.node = nil

				if model_info.effect_id then
					self:__DelEffect(model_info.effect_id)
					model_info.effect_id = nil
				end

				self.__del_model_info_map[del_model_index] = nil
			end)
		end
		self.__model_info_list[pos_type] = nil
	end
end

--模型退场
function Modules.StoryView:__ExitModels(duration)
	for _, pos_type in ipairs(model_pos_type_list) do
		self:__DeleteModel(pos_type, duration, true)
	end
end

--获取模型节点
function Modules.StoryView:__GetModelNode(pos_type)
	local model_info = self.__model_info_list and self.__model_info_list[pos_type]
	if model_info then
		return model_info.node
	end
end

--创建字幕
function Modules.StoryView:__CreateCaptions(section_info)
	local caption = section_info.caption_list
	if not caption then
		--所有字幕退场
		self:__FadeOutCurCaptions(true)
		return
	end
	
	if section_info.auto_skip_second and not self.__is_have_create then
		self.__panel_captions:SetVisible(true)
		self.__is_have_create = true
		self.__cur_section_index = self:CallModuleFunc("GetSectionIndex")
		self.__org_auto_skip_second = section_info.auto_skip_second
		self.__is_on_running_caption = true
		self.__is_on_last_captions = false
		--点击继续
		self.__continute_lab = Util:Label(CPPGameLib.GetString("story_continue"), Macros.TypefaceSize.normal, Macros.Color.white)
		self.__continute_lab:SetAnchorPoint(1,0)
		self:GetNode():AddChild(self.__continute_lab)
		PosTool.RightBottom(self.__continute_lab,-10,20)	
		self.__continute_lab:SetVisible(false)
	end
	--对话框退场
	self:__ExitDialog(self.__org_auto_skip_second, true)

	--模型退场
	self:__ExitModels(self.__org_auto_skip_second)

	local lab_caption
	local height = 0
	--字幕父容器
	self.__cur_lab_layout = Util:Layout(100,(font_size + caption_interval)*#section_info.caption_list)
	self.__cur_lab_layout:SetAnchorPoint(0.5,1)
	self.__panel_captions:AddChild(self.__cur_lab_layout)
	PosTool.CenterBottom(self.__cur_lab_layout,0,0)
	--创建新字幕		
	for i=1,#section_info.caption_list do
		lab_caption = Util:Label(section_info.caption_list[i], font_size, Macros.Color.white, nil, nil, Macros.Color.white_stroke)
		lab_caption:SetAnchorPoint(0.5,1)
		self.__cur_lab_layout:AddChild(lab_caption)
		PosTool.CenterTop(lab_caption,0,-height)	
		height = height + font_size + caption_interval		
	end	

	self.__is_cur_lab_layout_moving = true
	self.__is_have_add_speed = false
	self.__cur_move_height = self.__panel_captions:GetContentHeight()/2 + height/2
	CPPActionManager.MoveTo(self.__cur_lab_layout,self.__org_auto_skip_second,self.__panel_captions:GetContentWidth()/2,self.__cur_move_height,GlobalCallbackMgr:AddCallBackFunc(function ()
		self.__is_cur_lab_layout_moving = false
		if self.__continute_lab then
			self.__continute_lab:SetVisible(true)
		end
	end))
end

--字幕淡出
function Modules.StoryView:__FadeOutCurCaptions(ignore_countdown)
	if self.__cur_lab_layout then	
		--是否已经点击
		if not self.__is_have_click then
			self.__is_have_click = true
		else
			return
		end
		CPPActionManager.FadeOut(self.__cur_lab_layout,section_operate_anim_time,GlobalCallbackMgr:AddCallBackFunc(function ()
			self.__cur_lab_layout:RemoveSelf(true)
			self.__cur_lab_layout = nil
			if self.__continute_lab then
				self.__continute_lab:SetVisible(false)
			end
			if ignore_countdown ~= true then
				self:__CountDownCaptions()
			else
				self.__is_on_running_caption = false
			end
			self.__is_have_click = false
		end))
	end
end

--移动字幕
function Modules.StoryView:__CountDownCaptions()
	if not self.__is_on_last_captions then
		self.__cur_section_index = self.__cur_section_index  + 1
	end
	local captions_num = self:CallModuleFunc("GetStoryCaptionsNum") 
	--结束剧情
	if self.__cur_section_index > captions_num then
		self.__is_on_last_captions = true	
		self:CallModuleFunc("SetCurSectionIndex",self.__cur_section_index - 1) 
		self.__is_on_running_caption = false
		self:CallModuleFunc("NextSection") 
		self.__panel_captions:SetVisible(false)		
		return
	end
	local sectioninfo = self:CallModuleFunc("GetSectionInfo",self.__cur_section_index)
	self:__CreateCaptions(sectioninfo)
end

--开始世界之心特效
function Modules.StoryView:__StartWorldStarEffect()
	local view = GlobalViewMgr:GetViewByType(BaseViewType.DUNGEON_GAMELEVEL)
	if not view or not view:IsOpen() then
		self:__Start()
		return
	end

	local boss_gamelevel_world_star_effect_id = view:GetBossGameLevelWorldStarEffectID()
	local boss_gamelevel_world_star_effect = GlobalEffectMgr:GetEffect(boss_gamelevel_world_star_effect_id)
	if not boss_gamelevel_world_star_effect or self.__world_star_effect_id or self.__world_star_bg_effect_id then
		self:__Start()
		return
	end
	boss_gamelevel_world_star_effect:SetVisible(false)

	self.__boss_gamelevel_world_star_effect_id = boss_gamelevel_world_star_effect_id

	local render_group = GlobalScene:GetRenderGroup(Scene.RenderQueue.GRQ_UI_MAX)
	local beg_world_pos = NodeUnity.ConvertPosBetweenTwoContainer(boss_gamelevel_world_star_effect, render_group:GetNode(), COCOPoint(0, 0))
	local end_world_pos = COCOPoint(Macros.Global.DesignSize.width/2, Macros.Global.DesignSize.height/2)
	self.__world_star_org_pos = beg_world_pos

	--世界之心
	local world_star_effect_root
	self.__world_star_effect_id, world_star_effect_root = GlobalEffectMgr:CreateScenePosEffect(
		{res_name = "shijiezhixin", pos_x = beg_world_pos.x, pos_y = beg_world_pos.y, loop = true, scale = 0.2, zorder = 2}, Scene.RenderQueue.GRQ_UI_MAX) 

	--飞到屏幕中央
	ActionManager.GetInstance():RunScaleToAndMoveToEffectEx(world_star_effect_root, 1, end_world_pos.x, end_world_pos.y, 1, function()
		self:__Start()

		--背景光效
		local world_star_bg_effect_root
		self.__world_star_bg_effect_id, world_star_bg_effect_root = GlobalEffectMgr:CreateScenePosEffect({
			id = 100000, pos_x = end_world_pos.x, pos_y = end_world_pos.y, zorder = 1, loop = true}, Scene.RenderQueue.GRQ_UI_MAX)
	end)
end

--结束世界之心特效
function Modules.StoryView:__StopWorldStarEffect()
	local world_star_effect_root = GlobalEffectMgr:GetEffect(self.__world_star_effect_id)
	local world_star_bg_effect_root = GlobalEffectMgr:GetEffect(self.__world_star_bg_effect_id)
	if not world_star_effect_root or not world_star_bg_effect_root then
		self:__Stop()
		return
	end

	local boss_gamelevel_world_star_effect = GlobalEffectMgr:GetEffect(self.__boss_gamelevel_world_star_effect_id)
	if boss_gamelevel_world_star_effect then
		boss_gamelevel_world_star_effect:SetVisible(true)
	end

	world_star_effect_root:SetScaleFactor(0.6)
	world_star_bg_effect_root:SetScaleFactor(0.6)

	CPPActionManager.ScaleTo(world_star_bg_effect_root, 0.2, 0, GlobalCallbackMgr:AddCallBackFunc(function()
		GlobalEffectMgr:DelEffect(self.__world_star_bg_effect_id)
		self.__world_star_bg_effect_id = nil

		--飞出屏幕
		local flyto_x = self.__world_star_org_pos.x
		local flyto_y = self.__world_star_org_pos.y
		ActionManager.GetInstance():RunScaleToAndMoveToEffectEx(world_star_effect_root, 0.8, flyto_x, flyto_y, 0, function()

			--播放闪光特效
			self.__world_star_bg_effect_id, world_star_bg_effect_root = GlobalEffectMgr:CreateScenePosEffect({
				id = 100028, pos_x = flyto_x, pos_y = flyto_y, scale = 0.2, zorder = 1, loop = true}, Scene.RenderQueue.GRQ_UI_MAX)											

			CPPActionManager.ScaleTo(world_star_bg_effect_root, 0.2, 0.4, GlobalCallbackMgr:AddCallBackFunc(function()
				CPPActionManager.ScaleTo(world_star_bg_effect_root, 0.2, 0, GlobalCallbackMgr:AddCallBackFunc(function()
					self:__Stop()
				end))
			end))
		end)				
	end))
end

--------------------------------------------------------------------------------------------------------------------------------
--私有函数
--------------------------------------------------------------------------------------------------------------------------------
handle_model_focus = function(model_info, is_focus)
	if model_info.pos_type ~= PosType.LEFT and model_info.pos_type ~= PosType.RIGHT then
		return
	end
	local color = is_focus and Macros.Color.lilyWhite or Macros.Color.Gray
	local zorder = is_focus and 1 or -1
	if model_info.node then
		model_info.node:SetZOrder(zorder)
	end
	if model_info.model then
		model_info.model:SetColor(unpack(color))
	end
end
