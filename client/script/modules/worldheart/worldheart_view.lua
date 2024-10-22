--
-- @brief 精灵系统
-- @auther: ldx
-- @date 2016年9月30日 11:58:28
--

Modules = Modules or {}
Modules.WorldHeartView = Modules.WorldHeartView or BaseClass(BaseView)

local OPEN_TYPE = {
	HAVE_ALREADY_OPENED = 1,
	LIGHT_UP = 2,
	NO_OPEN_YET = 3,
}

function Modules.WorldHeartView:__init()
	self.__layout_name = "worldheart"
	self.__bg_params = {type = BaseView.BGType.IMAGE,image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_worldheart")}
	self.__title_bar_params = 
	{
		close_func = function ()
		   if self.__is_on_lightup and not CallModuleFunc(ModuleType.LEAD, "IsInLead") then
		   		return
		   end
		   self:CloseManual()
		end ,
		title = CPPGameLib.GetString("worldheart_system"),
	}
	self.__open_callback = function ()
		local data = self:CallModuleFunc("GetWorldHeartData")
		self:__OpenCallback(data)
	end
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.WORLD_HREAT)
end

function Modules.WorldHeartView:__OpenCallback(data)
	if not data then
		return
	end 
	self.__data = data
	--设置定时器
	self:SetUpdateFunc(function ()
		self:GetWidget("Panel_touch"):SetTouchEnabled(true)
		if self.__update_move then
			if not self.__not_update_property then
				self:GetWidget("Panel_touch"):SetTouchEnabled(false)
			end
			self:__UpdateCrystalCircle()
		end
	end)
	--设置容器监听
	WidgetUnity.SetWidgetListener(self:GetWidget("Panel_touch"),function (x,y)
		--隐藏转圈隐藏控件
		self:__HideCirclePanel()
		self.__began_x = x
		self.__record_x = x
		self.__update_count_degrees = 0
		self.__update_degrees = 0		
		self.__record_change_x = self.__crystal_piece_degrees1
	end,function (x,y)	
		--隐藏转圈隐藏控件
		self:__HideCirclePanel()
		self.__not_update_property = true			
		self.__update_count_degrees = 0
		self.__update_degrees = (x - self.__record_x) * 0.7
		self.__update_fuhao = self.__update_degrees >= 0 and 3 or -3
		self.__record_x = x	
		self.__update_move = true					
	end,function (x,y)		
		self.__not_update_property = false	
		local offset_piece_x = self.__crystal_piece_degrees1 - self.__record_change_x	
		self.__update_count_degrees = 0
		local offset_x = (x - self.__began_x) >=0 and (x - self.__began_x) * 0.7 or (x - self.__began_x) * 0.7	
		if offset_x >= 0 then
			local p_x = offset_x%(360/self.__crystal_piece_num)
			offset_x =  p_x >= (180/self.__crystal_piece_num) and (offset_x + ((360/self.__crystal_piece_num) - p_x)) or (offset_x - p_x)
			offset_x = math.min(offset_x,360)			
		else
			local p_x = offset_x%(-360/self.__crystal_piece_num)
			offset_x =  p_x <= (-180/self.__crystal_piece_num) and (offset_x - (p_x + (360/self.__crystal_piece_num))) or (offset_x - p_x)
		end		
		offset_x = offset_x - offset_piece_x
		self.__update_degrees = offset_x
		self.__update_fuhao = self.__update_degrees >= 0 and 3 or -3
		self.__update_speed = true
		self.__update_move = true
	end,function (x,y)		
		self.__not_update_property = false	
		local offset_piece_x = self.__crystal_piece_degrees1 - self.__record_change_x	
		self.__update_count_degrees = 0
		local offset_x = (x - self.__began_x) >=0 and (x - self.__began_x) * 0.7 or (x - self.__began_x) * 0.7	
		if offset_x >= 0 then
			local p_x = offset_x%(360/self.__crystal_piece_num)
			offset_x =  p_x >= (180/self.__crystal_piece_num) and (offset_x + ((360/self.__crystal_piece_num) - p_x)) or (offset_x - p_x)
			offset_x = math.min(offset_x,360)			
		else			
			local p_x = offset_x%(-360/self.__crystal_piece_num)
			offset_x =  p_x <= (-180/self.__crystal_piece_num) and (offset_x - (p_x + (360/self.__crystal_piece_num))) or (offset_x - p_x)
		end		
		offset_x = offset_x - offset_piece_x
		self.__update_degrees = offset_x
		self.__update_fuhao = self.__update_degrees >= 0 and 3 or -3
		self.__update_speed = true
		self.__update_move = true
	end)
	--获取椭圆中心点
	self.__circle_center_pointx = self:GetWidget("Image_fazhen"):GetPositionX()
	self.__circle_center_pointy = self:GetWidget("Image_fazhen"):GetPositionY()
	--精灵球队列
	local Panel_spirit_list = self:GetWidget("Panel_spirit_list")
	local params = 
	{
		item_class = Modules.TemplSpiritListSpiritBall,
	    item_width = 120,
	    item_height	= 120,
	    row	= 4,
	    col	= 1,
	    view_width = 120,
	    view_height = 402,
	    selectable = true,
	    select_callback	= function (index,data)
	    	self.__selected_index = index
	    	self:__SelectedSpiritBallCallBack()
	    end,
	}
	--构造精灵球队列数据
	local spirit_list_data = {}
	for index,info in ipairs(CPPGameLib.GetConfig("HeartOfWorld",nil,nil,true)) do
		local data = {}
		data.__index = index
		data.__curstage = self.__data.__stage
		table.insert(spirit_list_data,data)
	end
	--精灵容器
	self.__spirit_ball_scrollview = GUI.ScrollView.New(params)
	Panel_spirit_list:AddChild(self.__spirit_ball_scrollview:GetNode())
	PosTool.Center(self.__spirit_ball_scrollview)
	self.__spirit_ball_scrollview:SetDataList(spirit_list_data)
	--精灵属性文字	
	Util:WidgetLabel(self:GetWidget("Text_spirit_property"),CPPGameLib.GetString("spirit_propertyText"), Macros.TypefaceSize.normal , Macros.Color.btn , 0, 0, Macros.Color.button_yellow)	
	--打开精灵属性界面
	WidgetUnity.SetWidgetListener(self:GetWidget("Button_spirit_property"),nil, nil,function ()
		local spirit_data = ConfigAdapter.Spirit.GetWorldHeartPlusPropMap(self.__data.org_stage, self.__data.org_level)
		if not spirit_data then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("spirit_no_Addproperty"))
			return
		end
		LogicGUI.TemplWorldHeartPropertyView.New(spirit_data)
	end, nil)
	--点亮按钮
	self.__button_lighton = self:GetWidget("Button_lightOn")
	Util:WidgetButton(self.__button_lighton,CPPGameLib.GetString("spirit_lighton_open"),function ()
		local is_pass = self:__CheckMaterial()
		if is_pass then
			self.__is_on_lightup = true
			self.__panel_shield:SetVisible(true)
			self:CallModuleFunc("WorldHeartLevelUpDataReq")	
			self:__HideCirclePanel()		
		else
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("spirit_noEnough"))
		end
	end,Macros.TypefaceSize.button,Macros.Color.btn, Macros.Color.button_yellow )
	--添加烟雾特效
	local effect
	self.__smog_effectid,effect = self:__CreateEffect({res_name = "UI_dizuo"},self:GetWidget("Image_fazhen"))
	PosTool.Center(effect)
	--激活水晶增加的属性文字
	self.__txt_base_property = self:GetWidget("Text_base_property")
	Util:WidgetLabel(self.__txt_base_property,CPPGameLib.GetString("spirit_onfighthero_addproperty"),26,Macros.Color.btn)
	--激活水晶增加的四个属性
	self.__txt_base_propertyf4 = self:GetWidget("Text_base_propertyf4")
	Util:WidgetLabel(self.__txt_base_propertyf4,CPPGameLib.GetString("spirit_text_base_propertyf4"),Macros.TypefaceSize.slightly,Macros.Color.Special)
	--激活水晶增加的属性数字
	self.__text_base_propertyper = self:GetWidget("Text_base_propertyper")
	Util:WidgetLabel(self.__text_base_propertyper,"",26,Macros.Color.green)	
	--消耗世界之心	
	self.__text_cost_spiritbook = self:GetWidget("Text_cost_spiritbook")	
	Util:WidgetLabel(self.__text_cost_spiritbook,CPPGameLib.GetString("spirit_costThing"), Macros.TypefaceSize.popup,Macros.Color.Special)
	--当前阶的名字
	local image_48 = self:GetWidget("Image_48")
	self.__text_spirit_crystalname = self:GetWidget("Text_spirit_crystalname")
	Util:WidgetLabel(self.__text_spirit_crystalname,"", Macros.TypefaceSize.headlineII, Macros.Color.btn_1 ,35,150)
	--消耗世界之心数量
	self.__text_cost_spiritbooknum = self:GetWidget("Text_cost_spiritbooknum")
	Util:WidgetLabel(self.__text_cost_spiritbooknum,"", Macros.TypefaceSize.popup)	
	--全点亮文字
	self.__text_all_lighton = self:GetWidget("Text_all_lighton")
	Util:WidgetLabel(self.__text_all_lighton,CPPGameLib.GetString("spirit_text_all_lighton"), Macros.TypefaceSize.popup,Macros.Color.Special)	
	--中心水晶
	self.__image_center_crystal = self:GetWidget("Image_center_crystal")
	self.__image_center_crystal:SetZOrder(2)
	--屏蔽层
	self.__panel_shield = self:GetWidget("Panel_shield")
	self.__panel_shield:SetZOrder(5)
	local function circle_jumpby()
		ActionManager:GetInstance():RunJumpByActionEx(self.__image_center_crystal,3,0,20,nil,nil,function ()
			ActionManager:GetInstance():RunJumpByActionEx(self.__image_center_crystal,3,0,-20,nil,nil,function ()
				circle_jumpby()	
			end)
		end)
	end
	circle_jumpby()
	--激活特效容器
	self.__panel_lightup_effect = self:GetWidget("Panel_lightUpEffect")
	self.__panel_lightup_effect:SetZOrder(3)
	--碎片水晶属性
	self.__panel_propertyline = self:GetWidget("Panel_propertyline")	
	self.__panel_propertyline:SetZOrder(3)
	--碎片线
	self.__image_line = self:GetWidget("Image_line")
	--碎片添加属性
	self.__text_addproperty = self:GetWidget("Text_addproperty")
	Util:WidgetLabel(self.__text_addproperty,"", Macros.TypefaceSize.popup,Macros.Color.green,0,0,Macros.Color.green_stroke)	
	--初始化水晶碎片
	self:__InitCrystalPiece()
	--下面那块
	self.__panel_centerbottom = self:GetWidget("Panel_centerbottom")
	--判断是否播放特效
	--self:JudgeButtonSpecialEffect()
	--初始化位置
	local locate_index = self.__data.__stage > 2 and (self.__data.__stage - 2) or (self.__data.__stage > 1 and (self.__data.__stage - 1) or self.__data.__stage)
	self.__spirit_ball_scrollview:LocateItem(locate_index)
	self.__spirit_ball_scrollview:SelectItem(self.__data.__stage)

	--规则按钮
	local Button_rule = Util:WidgetButton( self:GetWidget("Button_rule") , nil , function ( ... )
		local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
        help:SetLoadedCallback( function ( ... )
           help:SetCell("Rule_World_Heart")
        end )
	end )
	Button_rule:SetZOrder(100)
	
end

function Modules.WorldHeartView:__CheckMaterial()
	local spirit_info = CPPGameLib.GetConfig("HeartOfWorld",self.__data.__stage)
	local num_1 = CallModuleFunc(ModuleType.Pack,"GetNumByItemID",spirit_info.elves_list[self.__data.__level].cost_item[1])
	local num_2 = spirit_info.elves_list[self.__data.__level].cost_item[2]
	return num_1 >= num_2
end

function Modules.WorldHeartView:__RegistAllEvents()
	--精灵升级响应
	self:BindGlobalEvent(Macros.Event.WorldHeartModule.WORLD_HEART_LEVELUP, function(_, data)
		self.__data = data	
		--更新世界之心碎片	
		self:__UpdateSpiritNum()			 
		if self.__data.__level > 1 and self.__data.__level <= #self.__spirit_info.elves_list and (not self.__data.__is_full_level) then
			--升级闪光特效
			local effectid,effect = self:__CreateEffect({id = 100031},self["__crystal_piece_layout"..(self.__data.__level - 1)],2)
			PosTool.CenterBottom(effect)
			--点亮特效
			CPPActionManager.DelayTo(self.__text_addproperty,0.3, GlobalCallbackMgr:AddCallBackFunc(function()
				effectid,effect = self:__CreateEffect({id = 100036},self.__panel_lightup_effect,2)
				PosTool.Center(effect)
				--飘字
				if self.__spirit_info.elves_list[self.__data.__level - 1].add_pro_type[1] ~= 0 then					
					local lab = Util:Label(self.__spirit_info.elves_list[self.__data.__level - 1].award_desc, Macros.TypefaceSize.headline, Macros.Color.green, 0, 0, Macros.Color.green_stroke)
					self:GetWidget("Panel_spirit"):AddChild(lab,5)
					lab:SetPosition(631,300)	
					local windwords_list = {}
					local info = {}
					info.node = lab
					info.anim_type = Macros.Game.WIND_WORDS_TYPE.UPSLOPE		
					info.tgt_pos = {x = 631,y = 350} 
					table.insert(windwords_list,info)
					local windwords_effect = WindWordsEffects.New()
					windwords_effect:Wind(windwords_list,function ()
						if windwords_effect then
							windwords_effect:DeleteMe()
							windwords_effect = nil
						end
					end)			
				else					
					local award_info_arr = self.__spirit_info.elves_list[self.__data.__level - 1].award_item					
					local item_type, item_config = ObjIDTool.GetInfo(award_info_arr[1])
					if item_config.item_id_list then
						local _
						item_type, _ = ObjIDTool.GetInfo(item_config.item_id_list[1])
					end
					if item_type ~= Macros.Game.ObjType.SPIRIT then
						--奖励框
						local getReward = {{id = award_info_arr[1], num = award_info_arr[2]}}
						GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , getReward )

					else
						--精灵预览框
						LogicGUI.TemplSpiritIntroduceView.New(item_config.item_id_list[1],function ()
							self:CloseManual()
						end)
					end						
				end		
			end))	
			CPPActionManager.DelayTo(self.__text_addproperty,1, GlobalCallbackMgr:AddCallBackFunc(function()					
				self["__crystal_piece_image"..(self.__data.__level - 1)]:SetGray(false)
				--待机特效					
				if self.__data.__level < #self.__spirit_info.elves_list then
					if not self["__daiji_effectid"..self.__data.__level] then
						self["__daiji_effectid"..self.__data.__level],effect = self:__CreateEffect({res_name = "UI_shuxingqiudaiji"},self["__crystal_piece_layout"..self.__data.__level],2)
						PosTool.CenterBottom(effect,0,-27)
					end
				end
				self.__is_on_lightup = false
				self.__panel_shield:SetVisible(false)
				self.__update_count_degrees = 0
				self.__update_degrees = -(360/self.__crystal_piece_num)
				self.__update_fuhao = -(5/self.__crystal_piece_num)
				self.__update_move = true
				self.__update_speed = true							
			end))
		else
			local effectid,effect = self:__CreateEffect({id = 100032},self.__panel_lightup_effect,4)
			PosTool.Center(effect)		
			CPPActionManager.DelayTo(self.__text_addproperty,1.2,GlobalCallbackMgr:AddCallBackFunc(function()	
				local stage = self.__data.__is_full_level and self.__data.__stage or (self.__data.__stage - 1)			
				LogicGUI.TemplWorldHeartLightUpView.New(stage,self.__data.__is_full_level)							
			end))	
		end
	end)
	--激活回调
	self:BindGlobalEvent(Macros.Event.WorldHeartModule.WORLD_HEART_LIGHTUP, function(_,is_full_level)
		self.__is_on_lightup = false
		self.__panel_shield:SetVisible(false)
		if is_full_level then
			self.__panel_centerbottom:SetVisible(true)
			local hide_str = "haved_open"
			self:__HidePanel(hide_str)	
			self.__image_center_crystal:SetGray(false)
		else
			local locate_index = self.__data.__stage > 2 and (self.__data.__stage - 2) or (self.__data.__stage > 1 and (self.__data.__stage - 1) or self.__data.__stage)
			self.__spirit_ball_scrollview:LocateItem(locate_index)
			self.__spirit_ball_scrollview:SelectItem(self.__data.__stage)
			local item = self.__spirit_ball_scrollview:GetItem(self.__data.__stage)
			if item then
				item:SetBallLight(self.__data.__stage)	
			end					
		end
	end)
end

--初始化水晶碎片
function Modules.WorldHeartView:__InitCrystalPiece()
	for index=1,5 do
		--容器
		self["__crystal_piece_layout"..index] = Util:Layout(2,2)
		self["__crystal_piece_layout"..index]:SetClippingEnable(false)
  		self["__crystal_piece_layout"..index]:SetBackGroundColor(0, 0, 0)
  		self:GetWidget("Panel_spirit"):AddChild(self["__crystal_piece_layout"..index])
  		--底图
  		local image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.WORLD_HREAT, "pedestal")
  		local pedestal = Util:Sprite(image_data)
  		self["__crystal_piece_layout"..index]:AddChild(pedestal)
  		PosTool.CenterBottom(pedestal)
  		--水晶碎片
  		image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.WORLD_HREAT, "crystal_piece_"..index)
  		self["__crystal_piece_image"..index] = Util:Sprite(image_data)
  		self["__crystal_piece_layout"..index]:AddChild(self["__crystal_piece_image"..index])
  		PosTool.Center(self["__crystal_piece_image"..index],0,60)
  		local x,y = self:__GetPositionByDegrees(-90 + (index - 1) * (72))
  		self["__crystal_piece_layout"..index]:SetAnchorPoint(0.5,0.5)
  		self["__crystal_piece_layout"..index]:SetPosition(x,y)
	end
end

function Modules.WorldHeartView:__GetPositionByDegrees(degrees)
	local x = self.__circle_center_pointx + 355*math.cos(math.rad(degrees))
	local y = self.__circle_center_pointy + 123*math.sin(math.rad(degrees))
	return x,y
end

--更新世界之心数量
function Modules.WorldHeartView:__UpdateSpiritNum()
	if self.__selected_index == self.__data.__stage then
		local spirit_num = CallModuleFunc(ModuleType.Pack,"GetNumByItemID",self.__spirit_info.elves_list[self.__data.__level].cost_item[1])
		self.__text_cost_spiritbooknum:SetText(CPPGameLib.GetString("spirit_numcost",spirit_num,self.__spirit_info.elves_list[self.__data.__level].cost_item[2]))
		self.__text_cost_spiritbooknum:SetColor(unpack(spirit_num >=self.__spirit_info.elves_list[self.__data.__level].cost_item[2] and Macros.Color.green or Macros.Color.red))	
		if self.__text_cost_spiritbook:IsVisible() then
			self.__text_cost_spiritbook:SetVisible(self.__spirit_info.elves_list[self.__data.__level].cost_item[2] > 0)
			self.__text_cost_spiritbooknum:SetVisible(self.__spirit_info.elves_list[self.__data.__level].cost_item[2] > 0)
		end
	end
end

	
function Modules.WorldHeartView:__SelectedSpiritBallCallBack()		--隐藏转圈隐藏控件
	self:__HideCirclePanel()
	self.__spirit_info = CPPGameLib.GetConfig("HeartOfWorld",self.__selected_index)
	if not self.__spirit_info then
		return
	end	
	self.__crystal_piece_num = #self.__spirit_info.elves_list - 1
	--当前阶的名字
	self.__text_spirit_crystalname:SetText(self.__spirit_info.elves_list[1].name)
	--中心水晶
	local hide_str
	if self.__selected_index == #CPPGameLib.GetConfig("HeartOfWorld",nil,nil,true) and self.__data.__is_full_level then
		self.__image_center_crystal:SetGray(false)
		hide_str = "haved_open"
	else
		hide_str = self.__selected_index < self.__data.__stage and "haved_open" or (self.__selected_index > self.__data.__stage and "un_open" or "on_open")
		self.__image_center_crystal:SetGray(not (self.__selected_index < self.__data.__stage))
	end
	--激活水晶增加的属性文字
	self.__txt_base_property:SetText(self.__spirit_info.elves_list[1].sp_show_property and self.__spirit_info.elves_list[1].sp_show_property or CPPGameLib.GetString("spirit_onfighthero_addproperty"))
	--激活水晶增加的四个属性
	self.__txt_base_propertyf4:SetVisible(not self.__spirit_info.elves_list[1].sp_show_property and true or false)
	--添加的属性百分比
	local propery_penceny = MathTool.TransProbCoeffPercentage(self.__spirit_info.elves_list[#self.__spirit_info.elves_list].add_pro_value[1])
	self.__text_base_propertyper:SetText(CPPGameLib.GetString("spirit_addPro",propery_penceny))		
	self.__text_base_propertyper:SetVisible(not self.__spirit_info.elves_list[1].sp_show_property and true or false)
	--隐藏控件	
	self.__button_lighton:SetText(CPPGameLib.GetString("spirit_light"..hide_str))
	self:__HidePanel(hide_str)	
	--世界之心数量
	self:__UpdateSpiritNum()
	--激活文字
	if self.__selected_index == self.__data.__stage and self.__data.__level == #self.__spirit_info.elves_list then
		self.__button_lighton:SetText(CPPGameLib.GetString("spirit_lightOnText"))
	end
	--设置转圈前的前一个位置
	self:__SetCrystalPiecePosition()
	self.__update_count_degrees = 0
	self.__update_degrees = -(360/self.__crystal_piece_num)
	self.__update_fuhao = -(5/self.__crystal_piece_num)
	self.__update_move = true
	self.__update_speed = true
end

--设置转圈前的前一个位置
function Modules.WorldHeartView:__SetCrystalPiecePosition()
	--先设置水晶的位置和可见性
	local degrees = -90 + 360/self.__crystal_piece_num
	local cur_index = self.__selected_index < self.__data.__stage and self.__crystal_piece_num or (self.__selected_index > self.__data.__stage and 1 or self.__data.__level)
	local effect
	for index=1,5 do
		local isGray
		local is_have_effect
		if self.__selected_index < self.__data.__stage and index <= self.__crystal_piece_num then
			isGray = false
			is_have_effect = true
		elseif self.__selected_index > self.__data.__stage or index > self.__crystal_piece_num then
			isGray = true
			is_have_effect = false
		else
			isGray = not (self.__data.__level > index)
			is_have_effect = self.__data.__level >= index
		end
		if is_have_effect then
			if not self["__daiji_effectid"..index] then
				self["__daiji_effectid"..index],effect = self:__CreateEffect({res_name = "UI_shuxingqiudaiji"},self["__crystal_piece_layout"..index],2)
				PosTool.CenterBottom(effect,0,-27)
			end
		else
			if self["__daiji_effectid"..index] then
				self:__DelEffect(self["__daiji_effectid"..index])
				self["__daiji_effectid"..index] = nil
			end
		end
		self["__crystal_piece_image"..index]:SetGray(isGray)
		self["__crystal_piece_degrees"..index] = degrees + (index - cur_index) * (360/self.__crystal_piece_num)
		local x,y = self:__GetPositionByDegrees(self["__crystal_piece_degrees"..index])
		self["__crystal_piece_layout"..index]:SetPosition(x,y)
		self["__crystal_piece_layout"..index]:SetVisible(index <= self.__crystal_piece_num) 
	end
end

--更新转圈位置
function Modules.WorldHeartView:__UpdateCrystalCircle()
	if self.__update_speed then
		self.__update_fuhao = self.__update_fuhao + 0.5*(self.__update_fuhao)	
	end
	if self.__update_fuhao >= 0 then
		if self.__update_count_degrees >= self.__update_degrees then
			self.__update_move = false
			self.__update_speed = false			
			self:__CircleCallBack()
			return		
		end		
		if (self.__update_count_degrees + self.__update_fuhao) > self.__update_degrees then
			self.__update_fuhao = self.__update_degrees - self.__update_count_degrees
			self.__update_count_degrees = self.__update_degrees
		else
			self.__update_count_degrees = self.__update_count_degrees + self.__update_fuhao		
		end	
	else	
		if self.__update_count_degrees <= self.__update_degrees then
			self.__update_move = false
			self.__update_speed = false		
			self:__CircleCallBack()
			return		
		end
		if (self.__update_count_degrees + self.__update_fuhao) < self.__update_degrees then
			self.__update_fuhao = self.__update_degrees - self.__update_count_degrees
			self.__update_count_degrees = self.__update_degrees
		else
			self.__update_count_degrees = self.__update_count_degrees + self.__update_fuhao		
		end
	end		
	for index=1,5 do
		self["__crystal_piece_degrees"..index] = self["__crystal_piece_degrees"..index] + self.__update_fuhao
		local x,y = self:__GetPositionByDegrees(self["__crystal_piece_degrees"..index])
		self["__crystal_piece_layout"..index]:SetPosition(x,y)
	end
end

function Modules.WorldHeartView:__CircleCallBack()	
	if self.__not_update_property then
		return
	end
	local index
	for i=1,5 do
		if self["__crystal_piece_layout"..i]:IsVisible() then
			if math.abs((math.modf(self["__crystal_piece_degrees"..i])+90)%360) <= 3 then
				index = i
				break
			end
		end
	end
	if index then
		self.__panel_centerbottom:SetVisible(true)
		if self.__selected_index == self.__data.__stage then		
			--灰色
			self.__button_lighton:SetEnabled(index == self.__data.__level)
			--按钮特效
			local is_pass = self:__CheckMaterial()
			if (index == self.__data.__level and is_pass ) or (self.__data.__level == #self.__spirit_info.elves_list and is_pass) then
				if not self.__button_effect_id then
					local effect
					self.__button_effect_id,effect = self:__CreateEffect({res_name = "ui_anniuzoubian"},self.__button_lighton)
					PosTool.Center(effect)
				end
			end		
			--文字
			local hide_str = index < self.__data.__level and "haved_open" or (index > self.__data.__level and "un_open" or "on_open")
			self.__button_lighton:SetText(CPPGameLib.GetString("spirit_light"..hide_str))	
			self.__text_cost_spiritbook:SetVisible(hide_str == "on_open")
			self.__text_cost_spiritbooknum:SetVisible(hide_str == "on_open")
			--激活
			if self.__data.__level == #self.__spirit_info.elves_list then
				self.__button_lighton:SetEnabled(true)
				self.__button_lighton:SetText(CPPGameLib.GetString("spirit_lightOnText"))
				CPPActionManager.StopAllActions(self.__text_addproperty)
				self.__image_line:SetVisible(false)
				self.__text_addproperty:SetVisible(false)
				self.__hide_property = true
				local effect
				if not self.__spirit_breath_id then
					self.__spirit_breath_id,effect = self:__CreateEffect({res_name = "UI_jinglinghuxi"},self.__panel_lightup_effect,2)
					PosTool.Center(effect,0,-30)
				end
				if not self.__spirit_bottom_id then
					self.__spirit_bottom_id,effect = self:__CreateEffect({res_name = "UI_jinglinghuxi_dizuo"},self.__panel_lightup_effect,3)
					PosTool.CenterBottom(effect,0,30)
				end
			else
				self.__hide_property = false
				self:__DelLightUpEffect()
			end
		else
			self.__hide_property = false
			self:__DelLightUpEffect()
		end
		--属性文字
		self.__text_addproperty:SetText(self.__spirit_info.elves_list[index].award_desc)
		if not self.__hide_property then
			--播完特效显示
			local effect
			self.__line_effectid,effect = self:__CreateEffect({res_name = "UI_jinglingxian",loop = false},self.__panel_propertyline,3)
			CPPActionManager.DelayTo(self.__text_addproperty,0.6, GlobalCallbackMgr:AddCallBackFunc(function()					
					self.__image_line:SetVisible(true)
					self.__text_addproperty:SetVisible(true)					
				end))
			effect:SetPosition(135,50)
		end
	end
end

--删除激活特效
function Modules.WorldHeartView:__DelLightUpEffect()	
	if self.__spirit_breath_id then
		self:__DelEffect(self.__spirit_breath_id)
		self.__spirit_breath_id = nil
	end
	if self.__spirit_bottom_id then
		self:__DelEffect(self.__spirit_bottom_id)
		self.__spirit_bottom_id = nil
	end
end

function Modules.WorldHeartView:__Dispose()
	if self.__spirit_ball_scrollview then
		self.__spirit_ball_scrollview:DeleteMe()
		self.__spirit_ball_scrollview = nil
	end

    if self.__button_effect_id then
		self:__DelEffect(self.__button_effect_id)
		self.__button_effect_id = nil
	end
	if self.__line_effect_id then
		self:__DelEffect(self.__line_effect_id)
		self.__line_effect_id = nil
	end
	if self.__smog_effectid then
		self:__DelEffect(self.__smog_effectid)
		self.__smog_effectid = nil
	end
	self:__DelLightUpEffect()
end

function Modules.WorldHeartView:__HideCirclePanel()
	self.__panel_centerbottom:SetVisible(false)
	if self.__line_effect_id then
		self:__DelEffect(self.__line_effect_id)
		self.__line_effect_id = nil
	end
	if self.__button_effect_id then
		self:__DelEffect(self.__button_effect_id)
		self.__button_effect_id = nil
	end
	if self.__line_effectid then
		self:__DelEffect(self.__line_effectid)
		self.__line_effectid = nil
	end
	self.__image_line:SetVisible(false)
	self.__text_addproperty:SetVisible(false)
end

function Modules.WorldHeartView:__HidePanel(hide_str)
	self.__button_lighton:SetVisible(hide_str ~= "haved_open")
	self.__text_cost_spiritbook:SetVisible(hide_str == "on_open")
	self.__text_cost_spiritbooknum:SetVisible(hide_str == "on_open")
	self.__text_all_lighton:SetVisible(hide_str == "haved_open")
	self.__button_lighton:SetEnabled(not (hide_str == "un_open"))
end

----------------------------------------------------------------------------------------------------------------------
--精灵队列精灵球
----------------------------------------------------------------------------------------------------------------------
Modules = Modules or {}

Modules.TemplSpiritListSpiritBall = Modules.TemplSpiritListSpiritBall or BaseClass(GUI.ScrollItem)

function Modules.TemplSpiritListSpiritBall:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplSpiritListSpiritBall:__CreateWidgetFromLuaCode()
  self.__layout = Util:Layout(120,120)
  self.__layout:SetClippingEnable(false)
  self.__layout:SetBackGroundColor(0, 0, 0)
  return self.__layout
end

function Modules.TemplSpiritListSpiritBall:SetData(data)
	GUI.ScrollItem.SetData(self,data)

	--精灵球底
	local image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.WORLD_HREAT, "bg_spirit_crystal")
	self.__spirit_bg = Util:Sprite(image_data)
	self.__layout:AddChild(self.__spirit_bg)
	PosTool.Center(self.__spirit_bg)
	self.__spirit_bg:SetGray(self.__data.__curstage < self.__data.__index)

	--精灵球
	local act_index = self.__data.__index%5 == 0 and 5 or self.__data.__index%5
	image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.WORLD_HREAT, "spirit_crystal_"..act_index)
	self.__spirit_ball = Util:Sprite(image_data)
	self.__layout:AddChild(self.__spirit_ball)
	PosTool.Center(self.__spirit_ball)
	self.__spirit_ball:SetGray(self.__data.__curstage < self.__data.__index)

	--精灵球名字
	local spirit_info = CPPGameLib.GetConfig("HeartOfWorld",self.__data.__index)
	self.__spirit_name = Util:Label(spirit_info.elves_list[1].name, Macros.TypefaceSize.largish, Macros.Color.blinkcontent, 0, 0, Macros.Color.common_stroke)
	self.__spirit_name:SetTextHorizontalAlign(LabelEx.ALIGIN_CENTER)
	self.__layout:AddChild(self.__spirit_name)
	PosTool.Center(self.__spirit_name,0,-20)	
	if self.__data.__curstage < self.__data.__index then
		self.__spirit_name:SetColor(unpack(Macros.Color.Gray))
	end		
end

function Modules.TemplSpiritListSpiritBall:SetBallLight(cur_stage)
	self.__data.__curstage = cur_stage
	self.__spirit_bg:SetGray(false)
	self.__spirit_ball:SetGray(false)
	self.__spirit_name:SetColor(unpack(Macros.Color.blinkcontent))
end

function Modules.TemplSpiritListSpiritBall:SetSelected(var)
	if var then
		if not self.__selected_image then
		    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.WORLD_HREAT, "selected_frame")
		    self.__selected_image = Util:Sprite(sp)
		    self.__layout:AddChild(self.__selected_image, 1)
		    PosTool.Center(self.__selected_image)
		end
	else
		if self.__selected_image then
			self.__selected_image:RemoveSelf(true)
			self.__selected_image = nil
		end
	end
end