--[[
%% @module: 精灵祝福界面
%% @author: ldx
%% @created: 2018年4月12日18:26:26
--]]

LogicGUI = LogicGUI or {}

LogicGUI.TemplSpiritSublimationView = LogicGUI.TemplSpiritSublimationView or BaseClass(GUI.Template)

function LogicGUI.TemplSpiritSublimationView:__init(cur_spirit_id, auto_skill_id)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__open_callback = function ()
	    self:__InitWidget(cur_spirit_id,auto_skill_id)
	end	
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_SPIRIT_SUBLIMATION_VIEW) 
end

function LogicGUI.TemplSpiritSublimationView:__delete()
	if self.__spirit_bless_model then
		self.__spirit_bless_model:DeleteMe()
		self.__spirit_bless_model = nil
	end
end

function LogicGUI.TemplSpiritSublimationView:__RegistAllEvents()
	--精灵培养回调
	self:BindGlobalEvent(Macros.Event.SpiritModule.SPIRIT_TRAIN_RESP, function(_,cur_spirit_id)		
		self.__spirit_data = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritData")
		self:__CreateSublimateDataBySpiritId(cur_spirit_id)
		self:RunSpiritSublimation(true)
		self.__on_sending = false
	end)
end

function LogicGUI.TemplSpiritSublimationView:__InitWidget(cur_spirit_id,auto_skill_id)
	self.__cur_spirit_id = cur_spirit_id
	self.__cur_auto_skill_id = auto_skill_id
	self.__sublimate_data = {}
	self.__spirit_data = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritData")
	--颜色队列
	local color_list = {{0x37,0xc7,0x27},{0xf1,0x46,0x70},{0xeb,0x8a,0x07},{0x14,0x89,0xd9}}
	--精灵配置
	local spirit_config = CPPGameLib.GetConfig("SpriteTrain",self.__cur_spirit_id)
	for i=1,4 do			
		--精灵培养属性
		--材料名字
		local item_config = CPPGameLib.GetConfig("Item",spirit_config.stage_list[1].cost_item_id_list[i])			
		Util:WidgetLabel(self:GetWidget("Text_materialName"..i),item_config.name, Macros.TypefaceSize.minimum, color_list[i])
		--材料数量
		self["__text_materialnum"..i] = self:GetWidget("Text_materialNum"..i)
		Util:WidgetLabel(self["__text_materialnum"..i],"", Macros.TypefaceSize.slightly, Macros.Color.btn,0,0,Macros.Color.common_stroke)
		--当前添加属性值
		self["__text_curaddproperty"..i] = self:GetWidget("Text_curAddProperty"..i)
		Util:WidgetLabel(self["__text_curaddproperty"..i],"", Macros.TypefaceSize.slightly, Macros.Color.btn,0,0,Macros.Color.common_stroke)
		--升华提升容器
		self["__panel_lightup"..i] = self:GetWidget("Panel_lightUp"..i)
		--升华提升属性名字
		self["__text_lightup_pro"..i] = self:GetWidget("Text_lightUpPro"..i)
		Util:WidgetLabel(self["__text_lightup_pro"..i],"", Macros.TypefaceSize.slightly, Macros.Color.Special)
		--升华提升属性当前值
		self["__text_lightup_curpro"..i] = self:GetWidget("Text_lightUpCurPro"..i)
		Util:WidgetLabel(self["__text_lightup_curpro"..i],"", Macros.TypefaceSize.slightly, Macros.Color.btn)
		--升华提升下一级属性名字
		self["__text_lightup_nextpro"..i] = self:GetWidget("Text_lightUpNextPro"..i)
		Util:WidgetLabel(self["__text_lightup_nextpro"..i],"", Macros.TypefaceSize.slightly, Macros.Color.Special)
		--升华提升属性下一级值
		self["__text_lightup_nextcurpro"..i] = self:GetWidget("Text_lightUpNextCurPro"..i)
		Util:WidgetLabel(self["__text_lightup_nextcurpro"..i],"", Macros.TypefaceSize.slightly, Macros.Color.green)
		--培养材料获取途径
		WidgetUnity.SetWidgetListener(self:GetWidget("Panel_approachTouch"..i),nil, nil,function ()
			GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, spirit_config.stage_list[1].cost_item_id_list[i])
		end)						
	end
	--大背景
	self.__panel_big = self:GetWidget("Panel_Big")
	--精灵升华
	self.__panel_spiritsublimate = self:GetWidget("Panel_spiritSublimate")
	--精灵培养
	self.__panel_spirittrain = self:GetWidget("Panel_spiritTrain")

	--水晶位置
	self.__crystal_pos_list = {[1] = {[1] = 369,[2] = 393},[2] = {[1] = 325,[2] = 395},[3] = {[1] = 363,[2] = 394},[4] = {[1] = 343,[2] = 390}}
	--阵法位置
	self.__image_spirit_sublimate_pos_list = {[1] = 267,[2] = 288}

	--标题
	local txt_title = self:GetWidget("Text_title")
	Util:WidgetLabel(txt_title,CPPGameLib.GetString("spirit_sublimate_tab"), Macros.TypefaceSize.tab, Macros.Color.Special)

	--一键培养开放条件
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	local is_open = role_level >= ConfigAdapter.Common.GetOneKeySpiritTrainOpenCondition()
	--培养按钮
	self.__button_train = self:GetWidget("Button_train")
	self.__button_train:SetPosition(is_open and 320.5 or 204,self.__button_train:GetPositionY())
	Util:WidgetButton(self.__button_train, CPPGameLib.GetString("spirit_train"),function ()
		if self.__spirit_train_tips ~= "" then
			GlobalTipMsg.GetInstance():Show(self.__spirit_train_tips)
			return
		end
		if self.__on_sending then
			return
		end
		self.__on_sending = true
		--特效
		local effectid,effect = self:__CreateEffect({id = 100057,loop = false},self.__panel_big,3)
		effect:SetAnchorPoint(0.5,0.5)
		effect:SetPosition(362,240)
		--延时
		CPPActionManager.DelayTo(self:GetNode(), 1.3, GlobalCallbackMgr:AddCallBackFunc(function()
			CallModuleFunc(ModuleType.SPIRIT,"SpiritTrainReq",self.__cur_spirit_id,1)
		end))
	end, Macros.TypefaceSize.button, Macros.Color.btn, Macros.Color.button_yellow)
	--一键培养按钮
	self.__button_onekey_train = self:GetWidget("Button_onekey_train")
	self.__button_onekey_train:SetVisible(is_open)
	Util:WidgetButton(self.__button_onekey_train, CPPGameLib.GetString("spirit_onekey_train"),function ()
		if self.__spirit_train_tips ~= "" then
			GlobalTipMsg.GetInstance():Show(self.__spirit_train_tips)
			return
		end
		if self.__on_sending then
			return
		end
		--一键培养
		LogicGUI.TemplSpiritOneKeyTrain.New({cost_num = self.__onekey_cost_num,func = function ()
			self.__on_sending = true
			--特效
			local effectid,effect = self:__CreateEffect({id = 100057,loop = false},self.__panel_big,3)
			effect:SetAnchorPoint(0.5,0.5)
			effect:SetPosition(362,240)
			--延时
			CPPActionManager.DelayTo(self:GetNode(), 1.3, GlobalCallbackMgr:AddCallBackFunc(function()
				CallModuleFunc(ModuleType.SPIRIT,"SpiritTrainReq",self.__cur_spirit_id,self.__could_train_times)
			end))
		end})		
	end, Macros.TypefaceSize.button, Macros.Color.btn, Macros.Color.button_yellow)
	--材料不足提示
	self.__text_materialtips = self:GetWidget("Text_materialTips")
	Util:WidgetLabel(self.__text_materialtips,"", Macros.TypefaceSize.slightly, Macros.Color.red)
	--精灵培养
	self.__panel_sublimate = self:GetWidget("Panel_sublimate")
	--阵法
	self.__image_spiritsublimate = self:GetWidget("Image_spiritSublimate")
	--升华图腾
	self.__image_spiritsublimate_icon = self:GetWidget("Image_spiritSublimateIcon")
	self.__image_spiritsublimate_icon:IgnoreContentAdaptWithSize(true)
	--图腾名字
	self.__text_sublimateitem_name = self:GetWidget("Text_sublimateItemName")
	Util:WidgetLabel(self.__text_sublimateitem_name,"", Macros.TypefaceSize.largish, Macros.Color.red,0,0,Macros.Color.orange_stroke)
	--图腾数量
	self.__text_sublimateitem_num = self:GetWidget("Text_sublimateItemNum")	
	Util:WidgetLabel(self.__text_sublimateitem_num,"", Macros.TypefaceSize.normal, Macros.Color.red,0,0,Macros.Color.orange_stroke)
	--升华按钮
	self.__button_sublimate = self:GetWidget("Button_sublimate")
	Util:WidgetButton(self.__button_sublimate, CPPGameLib.GetString("spirit_btn_sublimate2"),function ()
		if self.__spirit_sublimate_tips ~= "" then
			GlobalTipMsg.GetInstance():Show(self.__spirit_sublimate_tips)
			return
		end
		if self.__on_sending then
			return
		end
		self.__on_sending = true
		local effectid,effect = self:__CreateEffect({id = 100058,loop = false},self.__panel_big,3)
		effect:SetAnchorPoint(0.5,0.5)
		effect:SetPosition(362,240)
		--延时
		CPPActionManager.DelayTo(self:GetNode(),2.75,GlobalCallbackMgr:AddCallBackFunc(function()
			CallModuleFunc(ModuleType.SPIRIT,"SpiritTrainReq",self.__cur_spirit_id,1)
		end))	
	end, Macros.TypefaceSize.button, Macros.Color.btn, Macros.Color.button_yellow)
	--升华条件不足提示	
	self.__text_sublimatetips = self:GetWidget("Text_sublimateTips")
	Util:WidgetLabel(self.__text_sublimatetips,"", Macros.TypefaceSize.slightly, Macros.Color.red)
	--升华材料获取途径
	self.__panel_sublimateapproachtouch = self:GetWidget("Panel_sublimateApproachTouch")
	WidgetUnity.SetWidgetListener(self.__panel_sublimateapproachtouch, nil, nil,function ()
		if self.__spirit_sublimate_materical_id then
			GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, self.__spirit_sublimate_materical_id)
		end
	end)

	--关闭按钮
	WidgetUnity.SetWidgetListener(self:GetWidget("Button_close"), nil, nil,function ()
		self:DeleteMe()
	end)
	
	--正常形象
	self.__panel_status1 = self:GetWidget("Panel_Status1")
	--精灵名字
	self.__txt_spiritname = self:GetWidget("Text_spiritName")
	Util:WidgetLabel(self.__txt_spiritname, "", Macros.TypefaceSize.tab)
	--精灵模型
	local panel_spiritmodule = self:GetWidget("Panel_spiritModule")
	self.__spirit_bless_model = LogicGUI.Model.New({info_id = self.__cur_spirit_id,model_type = LogicGUI.ModelType.SPRITE,parent = panel_spiritmodule})
	self.__spirit_bless_model:SetScaleFactor(2)
	self.__spirit_bless_model:SetPosition(5,self.__cur_spirit_id == 50012 and -30 or 5)

	--进阶
	self.__panel_status2 = self:GetWidget("Panel_Status2")
	self.__panel_status2:SetVisible(false)

	--有新形象的容器
	self.__panel_statusnewfigure = self:GetWidget("Panel_StatusNewFigure")
	self.__panel_statusnewfigure:SetVisible(false)

	--打开升华界面
	self:RunSpiritSublimation()
end

--创建升华数据
function LogicGUI.TemplSpiritSublimationView:RunSpiritSublimation(is_special)
	local cur_spirit_id = self.__cur_spirit_id
	local spirit_config = CPPGameLib.GetConfig("Spirit",cur_spirit_id)
	local spirit_train_config = CPPGameLib.GetConfig("SpriteTrain",cur_spirit_id)
	local spirit_info = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritInfoByID",cur_spirit_id)
	local max_train_stage = #spirit_train_config.stage_list
	local next_train_stage = math.min(spirit_info.train_stage+1,max_train_stage)
	local is_full_stage = spirit_info.train_stage >= max_train_stage
	self.__selected_spirit_skill_lv = spirit_train_config.stage_list[spirit_info.train_stage].skill_level
	self.__selected_enhance_level = spirit_info.enhance_level
	--精灵名
	self.__txt_spiritname:SetText(spirit_config.name..CPPGameLib.GetString("spirit_sublimate_level",self.__selected_spirit_skill_lv))
	self.__txt_spiritname:SetColor(unpack(Macros.Quality[spirit_config.quality]))
	self.__txt_spiritname:SetStrokeColor(unpack(Macros.Quality_Stroke[spirit_config.quality]))

	--满级
	self.__button_train:SetVisible(not is_full_stage)		
	self.__button_onekey_train:SetVisible(not is_full_stage)

	if not self.__spirit_sublimate_init then
		--升华数据
		if not self.__sublimate_data[cur_spirit_id] and CallModuleFunc(ModuleType.SPIRIT,"GetSpiritInfoByID",cur_spirit_id) then
			self:__CreateSublimateDataBySpiritId(cur_spirit_id)
		end
		--self.__spirit_sublimate_init = true
		local cur_stage = self.__spirit_data.spirit_info_map[cur_spirit_id].train_stage
		--是否是升华
		local is_on_sublimate = not is_full_stage and self.__spirit_data.spirit_info_map[cur_spirit_id].train_times >= spirit_train_config.stage_list[cur_stage].need_train_times or false
		--self:PlayActionTimeline("spirit_train", false)
		self.__panel_status2:SetVisible(is_on_sublimate)
		--动画
		local deline = self:GetActionTimeLine()
		if deline then
			deline:Pause()
		end			
		local num = is_on_sublimate and 2 or 1
		self.__image_spiritsublimate_icon:SetOpacity(255)
		if not is_special then				
			for i=1,4 do
				self:GetWidget("Image_crystal"..i):SetPosition(self:GetWidget("Image_crystal"..i):GetPositionX(),self.__crystal_pos_list[i][num])
			end
			--self.__image_spiritsublimate:SetPosition(self.__image_spiritsublimate:GetPositionX(),self.__image_spirit_sublimate_pos_list[num])				
			self.__panel_spirittrain:SetOpacity(255)
			self.__button_train:SetOpacity(255)		
			self.__button_onekey_train:SetOpacity(255)			
			self.__panel_spirittrain:SetVisible(not is_on_sublimate)
			self.__panel_sublimate:SetVisible(is_on_sublimate)	
		else
			if is_on_sublimate then
				self.__panel_spirittrain:SetVisible(true)
				self.__panel_sublimate:SetVisible(false)	
			elseif not is_on_sublimate and self.__spirit_data.spirit_info_map[cur_spirit_id].train_times == 0 then	
				--打开升华成功界面
				Modules.TemplSpiritSublimationSucc.New({spirit_id = cur_spirit_id})
				for i=1,4 do
					self:GetWidget("Image_crystal"..i):SetPosition(self:GetWidget("Image_crystal"..i):GetPositionX(),self.__crystal_pos_list[i][num])
				end
				--self.__image_spiritsublimate:SetPosition(self.__image_spiritsublimate:GetPositionX(),self.__image_spirit_sublimate_pos_list[num])				
				self.__panel_spirittrain:SetOpacity(255)
				self.__button_train:SetOpacity(255)		
				self.__button_onekey_train:SetOpacity(255)	
				self.__panel_spirittrain:SetVisible(not is_on_sublimate)
				self.__panel_sublimate:SetVisible(is_on_sublimate)
			else
				self.__panel_spirittrain:SetVisible(not is_on_sublimate)
				self.__panel_sublimate:SetVisible(is_on_sublimate)
			end
		end					
		local skill_config = CPPGameLib.GetConfig("Skill",self.__cur_auto_skill_id)			
		--判断当前的界面
		if is_on_sublimate then	
			--培养特效
			if self.__spirit_cup_effectid then
				self:__DelEffect(self.__spirit_cup_effectid)
				self.__spirit_cup_effectid = nil
			end		
			--升华
			local icon_path = Resource.PathTool.GetBigIconPath(spirit_train_config.stage_list[cur_stage].stage_up_cost_item[1]) 
			self.__image_spiritsublimate_icon:SetImage(icon_path,TextureResType.LOCAL)  
			local type,config_info = ObjIDTool.GetInfo(spirit_train_config.stage_list[cur_stage].stage_up_cost_item[1])
			local item_num = ObjIDTool.GetOwnNum(spirit_train_config.stage_list[cur_stage].stage_up_cost_item[1])
			self.__spirit_sublimate_materical_id = spirit_train_config.stage_list[cur_stage].stage_up_cost_item[1]
			--提示
			self.__spirit_sublimate_tips = self.__selected_enhance_level < spirit_train_config.stage_list[cur_stage].need_enhance_level and CPPGameLib.GetString("spirit_sublimate_dissatisfy_tips_1",spirit_train_config.stage_list[cur_stage].need_enhance_level)
											or (item_num < spirit_train_config.stage_list[cur_stage].stage_up_cost_item[2] and CPPGameLib.GetString("spirit_sublimate_dissatisfy_tips_2") or "")
			self.__text_sublimatetips:SetText(self.__spirit_sublimate_tips)				
			--图腾名字
			self.__text_sublimateitem_name:SetText(config_info.name)
			self.__text_sublimateitem_name:SetColor(Macros.QualityColor(config_info.quality))
			self.__text_sublimateitem_name:SetStrokeColor(Macros.QualityColorStroke(config_info.quality))
			--图腾数量
			local labcolor = item_num >= spirit_train_config.stage_list[cur_stage].stage_up_cost_item[2] and Macros.Color.green or Macros.Color.red 
			local stroke_color = item_num >= spirit_train_config.stage_list[cur_stage].stage_up_cost_item[2] and Macros.Color.green_stroke or Macros.Color.red_stroke
			self.__text_sublimateitem_num:SetText(CPPGameLib.GetString("spirit_numcost",item_num,spirit_train_config.stage_list[cur_stage].stage_up_cost_item[2]))
			self.__text_sublimateitem_num:SetColor(unpack(labcolor))
			self.__text_sublimateitem_num:SetStrokeColor(unpack(stroke_color))
			--显示队列				
			local front_str_list = {CPPGameLib.GetString("spirit_sublimate_txt"),CPPGameLib.GetString("spirit_property_transform_txt"),CPPGameLib.GetString("common_str_with_colon",skill_config.name),CPPGameLib.GetString("spirit_image")}
			local show_count_num = 0
			for i,front_str in ipairs(front_str_list) do
				self["__panel_lightup"..i]:SetVisible(false)				
				if i == 1 and spirit_train_config.stage_list[cur_stage].stage ~= spirit_train_config.stage_list[next_train_stage].stage then
					--阶段
					show_count_num = show_count_num + 1		
					self["__text_lightup_pro"..show_count_num]:SetText(front_str)	
					self["__text_lightup_nextpro"..show_count_num]:SetText(front_str)		
					self["__text_lightup_curpro"..show_count_num]:SetText(CPPGameLib.GetString("spirit_sublimate_lv",spirit_train_config.stage_list[cur_stage].stage))
					self["__text_lightup_nextcurpro"..show_count_num]:SetText(CPPGameLib.GetString("spirit_sublimate_lv",spirit_train_config.stage_list[next_train_stage].stage))
				elseif i == 2 and spirit_train_config.stage_list[cur_stage].pro_conversion_rate ~= spirit_train_config.stage_list[next_train_stage].pro_conversion_rate then
					show_count_num = show_count_num + 1	
					self["__text_lightup_pro"..show_count_num]:SetText(front_str)
					self["__text_lightup_nextpro"..show_count_num]:SetText(front_str)	
					--转换率					
					self["__text_lightup_curpro"..show_count_num]:SetText(CPPGameLib.GetString("spirit_transform_probability_txt",MathTool.TransProbCoeffPercentage(spirit_train_config.stage_list[cur_stage].pro_conversion_rate or 0)))
					self["__text_lightup_nextcurpro"..show_count_num]:SetText(CPPGameLib.GetString("spirit_transform_probability_txt",MathTool.TransProbCoeffPercentage(spirit_train_config.stage_list[next_train_stage].pro_conversion_rate or 0)))
				elseif i == 3 and spirit_train_config.stage_list[cur_stage].skill_level ~= spirit_train_config.stage_list[next_train_stage].skill_level then
					show_count_num = show_count_num + 1	
					self["__text_lightup_pro"..show_count_num]:SetText(front_str)
					self["__text_lightup_nextpro"..show_count_num]:SetText(front_str)	
					--技能等级
					self["__text_lightup_curpro"..show_count_num]:SetText(CPPGameLib.GetString("common_lev5",spirit_train_config.stage_list[cur_stage].skill_level))
					self["__text_lightup_nextcurpro"..show_count_num]:SetText(CPPGameLib.GetString("common_lev5",spirit_train_config.stage_list[next_train_stage].skill_level))
				elseif i == 4 and spirit_train_config.stage_list[cur_stage].model_id ~= spirit_train_config.stage_list[next_train_stage].model_id then
					show_count_num = show_count_num + 1	
					self["__text_lightup_pro"..show_count_num]:SetText(front_str)
					--精灵形象
				end							
				self["__panel_lightup"..show_count_num]:SetVisible(true)
			end
			if not is_special then
				self:PlayActionTimeline("spirit_sublimate_loop", true)
			end
		else
			--培养特效
			if not self.__spirit_cup_effectid then
				self.__spirit_cup_effectid,effect = self:__CreateEffect({res_name = "UI_jinglingbeizi",loop = true},self.__panel_big,2)
				effect:SetAnchorPoint(0.5,0.5)
				effect:SetPosition(362,240)
			end
			--计算材料数量
			local count_list = self:__CountMatericalDataBySpiritId(cur_spirit_id)
			--材料不足提示文字
			self.__spirit_train_tips = spirit_train_config.stage_list[cur_stage].need_train_times == 0 and CPPGameLib.GetString("spirit_train_full_level_tips") or (not self.__could_spirit_train and CPPGameLib.GetString("common_material_not_enough") or "")
			self.__text_materialtips:SetText(self.__spirit_train_tips)
			--显示变化属性
			for i,pro_type in ipairs(spirit_train_config.stage_list[cur_stage].pro_type_list) do					
				self["__text_materialnum"..pro_type]:SetText(CPPGameLib.GetString("spirit_numcost",count_list[pro_type][1] or 0,count_list[pro_type][2] or 0))
				local labcolor = count_list[pro_type][1] >= count_list[pro_type][2] and Macros.Color.btn or Macros.Color.red 
				local stroke_color = count_list[pro_type][1] >= count_list[pro_type][2] and Macros.Color.common_stroke or Macros.Color.red_stroke
				self["__text_materialnum"..pro_type]:SetColor(unpack(labcolor))
				self["__text_materialnum"..pro_type]:SetStrokeColor(unpack(stroke_color))
				self["__text_curaddproperty"..pro_type]:SetText(CPPGameLib.GetString("spirit_numcost",self.__sublimate_data[cur_spirit_id][pro_type][1] or 0,self.__sublimate_data[cur_spirit_id][pro_type][2] or 0))
			end				
		end
		--动画
		if is_on_sublimate and is_special then
			self.__image_spiritsublimate_icon:SetOpacity(0)
			self.__button_train:SetEnabled(false)
			self.__button_onekey_train:SetEnabled(false)
			self:PlayActionTimeline("spirit_sublimate", false, function ()
				self.__button_train:SetEnabled(true)
				self.__button_onekey_train:SetEnabled(true)
				self.__button_train:SetOpacity(0)
				self.__button_onekey_train:SetOpacity(0)
				self.__panel_sublimate:SetVisible(true)	
				self.__panel_spirittrain:SetVisible(false)
				CPPActionManager.FadeIn(self.__image_spiritsublimate_icon,0.5)
				local effectid,effect = self:__CreateEffect({id = 100059,loop = false},self.__panel_big,4)
				effect:SetAnchorPoint(0.5,0.5)
				effect:SetPosition(362,220)
				self:PlayActionTimeline("spirit_sublimate_loop", true,function ()
					self.__button_train:SetOpacity(255)
					self.__button_onekey_train:SetOpacity(255)
				end)
			end)
		elseif not is_on_sublimate and is_special then
		end
	end
end

--创建升华数据
function LogicGUI.TemplSpiritSublimationView:__CreateSublimateDataBySpiritId(spirit_id)
	if not spirit_id then
		return
	end
	self.__sublimate_data[spirit_id] = {}	
	--构造数据
	local spirit_train_config = CPPGameLib.GetConfig("SpriteTrain",spirit_id)
	for i,pro_type in ipairs(spirit_train_config.stage_list[1].pro_type_list) do
		self.__sublimate_data[spirit_id][pro_type] = {}
		self.__sublimate_data[spirit_id][pro_type][1] = 0
		self.__sublimate_data[spirit_id][pro_type][2] = 0
	end
	--分层
	for stage=1,self.__spirit_data.spirit_info_map[spirit_id].train_stage do
		local is_cur_stage = stage == self.__spirit_data.spirit_info_map[spirit_id].train_stage
		--算属性
		if not is_cur_stage then
			for count=1,spirit_train_config.stage_list[stage].need_train_times do
				for i,pro_type in ipairs(spirit_train_config.stage_list[stage].pro_type_list) do
					self.__sublimate_data[spirit_id][pro_type][1] = self.__sublimate_data[spirit_id][pro_type][1] + spirit_train_config.stage_list[stage].pro_value_list[i]
					self.__sublimate_data[spirit_id][pro_type][2] = self.__sublimate_data[spirit_id][pro_type][2] + spirit_train_config.stage_list[stage].pro_value_list[i]
				end
			end
		else
			for k=1,2 do
				for count=1,k == 1 and self.__spirit_data.spirit_info_map[spirit_id].train_times or spirit_train_config.stage_list[stage].need_train_times do
					for i,pro_type in ipairs(spirit_train_config.stage_list[stage].pro_type_list) do
						self.__sublimate_data[spirit_id][pro_type][k] = self.__sublimate_data[spirit_id][pro_type][k] + spirit_train_config.stage_list[stage].pro_value_list[i]
					end
				end
			end
		end
	end
end

--计算材料数量
function LogicGUI.TemplSpiritSublimationView:__CountMatericalDataBySpiritId(spirit_id)
	if not spirit_id then
		return
	end
	local count_list = {}
	self.__could_spirit_train = true
	--构造数据
	local spirit_train_config = CPPGameLib.GetConfig("SpriteTrain",spirit_id)
	local cur_stage = self.__spirit_data.spirit_info_map[spirit_id].train_stage
	local train_times = self.__spirit_data.spirit_info_map[spirit_id].train_times
	--可以培养的次数，消耗的数量
	local could_train_times,cost_num
	for i,pro_type in ipairs(spirit_train_config.stage_list[cur_stage].pro_type_list) do
		count_list[pro_type] = {}
		count_list[pro_type][1] = ObjIDTool.GetOwnNum(spirit_train_config.stage_list[cur_stage].cost_item_id_list[i])
		count_list[pro_type][2] = spirit_train_config.stage_list[cur_stage].cost_item_num_list[i]
		cost_num = spirit_train_config.stage_list[cur_stage].cost_item_num_list[i]
		could_train_times = could_train_times or math.floor(count_list[pro_type][1]/count_list[pro_type][2])
		could_train_times = math.floor(count_list[pro_type][1]/count_list[pro_type][2]) < could_train_times and math.floor(count_list[pro_type][1]/count_list[pro_type][2]) or could_train_times
		if count_list[pro_type][1] < count_list[pro_type][2] then
			self.__could_spirit_train = false
		end
	end
	local cur_need_train_times = spirit_train_config.stage_list[cur_stage].need_train_times
	self.__could_train_times = could_train_times < (cur_need_train_times - train_times) and could_train_times or (cur_need_train_times - train_times)
	self.__onekey_cost_num = cost_num * self.__could_train_times
	return count_list
end
