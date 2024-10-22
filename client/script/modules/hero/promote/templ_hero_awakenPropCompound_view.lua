LogicGUI = LogicGUI or {}

LogicGUI.TemplHeroAwakenPropCompoundView = LogicGUI.TemplHeroAwakenPropCompoundView or BaseClass(GUI.Template)

function LogicGUI.TemplHeroAwakenPropCompoundView:__init(id)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:InitUI(id)
	end
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_AWAKEN_PROP_COMPOSE)	
	self:__AddPreloadTemplate(GUI.TemplateRegistry.HERO_AWAKEN_SPECIAL_APPROACH_CELL)
	self:InitTemplate(GUI.TemplateRegistry.HERO_AWAKEN_PROP_COMPOUND)
end

function LogicGUI.TemplHeroAwakenPropCompoundView:InitUI(id)
	--关闭按钮
	Util:WidgetButton(self:GetWidget("Button_close"),"",function ()
		self:DeleteMe()
	end)
	--关闭界面事件
	self.__DeleteApproachViewEvent = self:BindGlobalEvent(Macros.Event.ApproachOfAchievingModule.DELETE_APPROACHVIEW, function()
	    self:DeleteMe()
	end)
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("hero_awaken_CompoundTitle"),Macros.TypefaceSize.tab, Macros.Color.btn)
	--上面选中栏
	local panel_topscrollview = self:GetWidget("Panel_topScrollview")
	local params = {
		item_class	= Modules.TemplHeroAwakenTopIconCell,
		item_width	= 80,
		item_height	= 94,
		horizon = true,
		row	= 1,
		col	= 5,
		item_space_c = 47,
		view_width = 588,
		view_height = 94,
		selectable = true,
   		select_callback	= function (index,data)
   			self:CloseBeforeThisView(index)
   		end,
	}
	self.__top_scrollview = GUI.ScrollView.New(params)
	panel_topscrollview:AddChild(self.__top_scrollview:GetNode())
	PosTool.Center(self.__top_scrollview)
	self.__selected_index = 1
	self.__id_table = {}
	for i=1,6 do
		local cur_id = i == 1 and id or 0
		table.insert(self.__id_table,cur_id)		
	end
	--途径
	self.__panel_approach = self:GetWidget("Panel_approach")
	--物品名字
	self.__text_itemname = self:GetWidget("Text_itemName")
	Util:WidgetLabel(self.__text_itemname,"",Macros.TypefaceSize.popup,Macros.Color.white,0,0,Macros.Color.white_stroke)
	--获取途径文字
	Util:WidgetLabel(self:GetWidget("Text_approach"),CPPGameLib.GetString("hero_awaken_CompoundApproach"),Macros.TypefaceSize.largish,Macros.Color.content)
	--物品头像
	self.__panel_itemicon = self:GetWidget("Panel_itemIcon")

	--合成
	self.__panel_compounddetail = self:GetWidget("Panel_compoundDetail")
	--头像
	self.__panel_compounditemicon = self:GetWidget("Panel_compoundItemIcon")
	--中心点坐标
    self.__panel_center_pointx = self.__panel_compounditemicon:GetContentWidth()/2
    self.__panel_center_pointy = self.__panel_compounditemicon:GetContentHeight()/2
	--名字
	self.__text_titleitem = self:GetWidget("Text_titleItem")
	Util:WidgetLabel(self.__text_titleitem,"",Macros.TypefaceSize.popup,Macros.Color.headline,0,0,Macros.Color.common_stroke)
	--合成按钮 
	self.__btn_compound = self:GetWidget("Button_compound")
	Util:WidgetButton(self.__btn_compound, CPPGameLib.GetString("hero_awaken_buttonText2"),function ( )
		if self.__compound_materialpass then
			self:__PlayComposeSpecialEffect()			
		else
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_awaken_CompoundTips"))
		end
	end,Macros.TypefaceSize.popup,Macros.Color.btn,Macros.Color.button_yellow)
	--一键合成按钮 
	Util:WidgetButton(self:GetWidget("Button_onekeycompound"), CPPGameLib.GetString("hero_awaken_buttonText5"),function ( )
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("arena_lookForwardTo"))
	end,Macros.TypefaceSize.popup,Macros.Color.btn,Macros.Color.button_yellow)

	--装备觉醒道具合成事件
	self.__awaken_prop_compose_event = self:BindGlobalEvent(Macros.Event.HeroModule.HERO_AWAKEN_PROP_COMPOSE_RESP, function(_,item_id)
		self.__btn_compound:SetEnabled(true)
		local awaken_config = CPPGameLib.GetConfig("AwakeningItem",item_id)
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_awaken_CompoundSuccessTip",awaken_config.name))
		self:__OpenCompoundView(self.__compoundpropid)
	end)

	--货币
	self.__image_coin = self:GetWidget("Image_coin")
	--合成所需货币数量
	self.__text_coinnum = self:GetWidget("Text_coinNum")
	Util:WidgetLabel(self.__text_coinnum,"",Macros.TypefaceSize.largish,Macros.Color.content)

	--需要两个的合成
	self.__panel_two = self:GetWidget("Panel_two")
	--需要三个的合成
	self.__panel_three = self:GetWidget("Panel_three") 
	--需要四个的合成
	self.__panel_four = self:GetWidget("Panel_four")
	--Panel表
	self.__panel_table ={[2] = self.__panel_two,[3] = self.__panel_three,[4] = self.__panel_four}
	--String表
	self.__string_table ={[2] = "__paneltwo",[3] = "__panelthree",[4] = "__panelfour"}
	--物品容器初始化
	for i=1,4 do
		if i <=2 then
			self["__paneltwo_itempos"..i] = WidgetUnity.GetWidgetByName(self.__panel_two,"Panel_itemPos"..i)
		end
		if i <=3 then
			self["__panelthree_itempos"..i] = WidgetUnity.GetWidgetByName(self.__panel_three,"Panel_itemPos"..i)
		end
		self["__panelfour_itempos"..i] = WidgetUnity.GetWidgetByName(self.__panel_four,"Panel_itemPos"..i)
	end
	self.__open_index = 0
	self.__top_scrollview:SetDataList({{info_id = id,cur_index = 1}})
	self.__top_scrollview:SelectItem(1)
end

--合成前的效果
function LogicGUI.TemplHeroAwakenPropCompoundView:__PlayComposeSpecialEffect()
	self.__btn_compound:SetEnabled(false)
	--觉醒信息
	local awaken_config = CPPGameLib.GetConfig("AwakeningItem",self.__compoundpropid)
	--合成需要的数量
	local compoundNum = #awaken_config.compose_item_id_list
	--飞入效果
	for i=1,compoundNum do
		--飘入图标
		local config = CPPGameLib.GetConfig("AwakeningItem", awaken_config.compose_item_id_list[i])
		local sp = Resource.PathTool.GetItemPath(config.res_id)
		
		self["awaken_icon_pos"..i] = Util:Sprite(sp)
		self[self.__string_table[compoundNum].."_itempos"..i]:AddChild(self["awaken_icon_pos"..i])
		PosTool.Center(self["awaken_icon_pos"..i])
		local x_dgree
		if (2 * i - compoundNum - 1) == 0 then
			x_dgree = 0
		else
			x_dgree = math.deg ( math.atan ( ( compoundNum - 1 ) / ( 2 * i - compoundNum - 1 ) ) )
		end		
		self["awaken_icon_pos"..i]:SetRotation(-x_dgree)

		--
		local effectid,effect
		effectid,effect = self:__CreateEffect({res_name = "UI_hecheng",time_scale = 1,loop = false,scale = 1.5},self.__panel_compounditemicon,3)
		PosTool.Center(effect,-7,2)

		local target_point = NodeUnity.ConvertPosBetweenTwoContainer(self.__panel_compounditemicon,self[self.__string_table[compoundNum].."_itempos"..i],COCOPoint(self.__panel_center_pointx,self.__panel_center_pointy))
		ActionManager.GetInstance():RunScaleToAndMoveToEffect(self["awaken_icon_pos"..i],0.6,target_point.x,target_point.y,1.9)	
		--飞到一半开始变小
		CPPActionManager.DelayTo(self[self.__string_table[compoundNum].."_itempos"..i],0.2, GlobalCallbackMgr:AddCallBackFunc(function()	
			CPPActionManager.StopAllActions(self["awaken_icon_pos"..i])
			ActionManager.GetInstance():RunScaleToAndMoveToEffect(self["awaken_icon_pos"..i],0.4,target_point.x,target_point.y,0.5,GlobalCallbackMgr:AddCallBackFunc(function()
				self["awaken_icon_pos"..i]:RemoveSelf(true)
				self["awaken_icon_pos"..i] = nil
				if i == compoundNum then
					--effectid,effect = self:__CreateEffect({res_name = "UI_hecheng",loop = false},self.__panel_compounditemicon,3)
					CPPActionManager.DelayTo(self[self.__string_table[compoundNum].."_itempos"..i],0.2, GlobalCallbackMgr:AddCallBackFunc(function()	
						CallModuleFunc(ModuleType.Pack,"ItemComposeReq",self.__compoundpropid)
					end))	
					--PosTool.Center(effect,-7,2)
				end
			end))
		end))			
	end	
end

--显示获取途径界面
function LogicGUI.TemplHeroAwakenPropCompoundView:__OpenApproachView(id)
	self.__panel_approach:SetVisible(true)
	self.__panel_compounddetail:SetVisible(false)
	--道具名字
	local awaken_config = CPPGameLib.GetConfig("AwakeningItem",id)
	self.__text_itemname:SetText(awaken_config.name)
	if not self.__approachitemicon then
		self.__approachitemicon = LogicGUI.IconLayer.New(false,false)
		self.__panel_itemicon:AddChild(self.__approachitemicon:GetNode())
		PosTool.Center(self.__approachitemicon)
	end
	local itemNum = ObjIDTool.GetOwnNum(id)
	self.__approachitemicon:SetData({id = id,num = itemNum})

	if not self.__scrollview then
		local params = {
			item_class	= Modules.TemplSpecialApproachOfAchievingCell,
		    item_width	= 578,
		    item_height	= 85,
		    row	= 3,
		    col	= 1,
		    item_space_r = 5,
		    view_width = 578,
		    view_height = 300,
		}
		self.__scrollview = GUI.ScrollView.New(params)
		self:GetWidget("Panel_scrollview"):AddChild(self.__scrollview:GetNode())
		PosTool.Center(self.__scrollview)
	end
	--获取处理后的途径数据
	self.__approach_of_achieving_list = CallModuleFunc(ModuleType.APPROACH_OF_ACHIEVING,"HandleApproachOfAchievingData",id)
	self.__scrollview:SetDataList(self.__approach_of_achieving_list)
end

--显示合成界面
function LogicGUI.TemplHeroAwakenPropCompoundView:__OpenCompoundView(id)
	self.__panel_approach:SetVisible(false)
	self.__panel_compounddetail:SetVisible(true)

	self.__compound_materialpass = false
	self.__compoundpropid = id
	local money_check = false
	local material_check = true	

	local awaken_config = CPPGameLib.GetConfig("AwakeningItem",id)
	--道具名字
	self.__text_titleitem:SetText(awaken_config.name)
	self.__text_titleitem:SetColor(unpack(Macros.Quality[awaken_config.quality]))
	self.__text_titleitem:SetStrokeColor(unpack(Macros.Quality_Stroke[awaken_config.quality]))
	if not self.__compounditemicon then
		self.__compounditemicon = LogicGUI.IconLayer.New(false,false)
		self.__panel_compounditemicon:AddChild(self.__compounditemicon:GetNode())
		PosTool.Center(self.__compounditemicon)
	end
	local itemNum = ObjIDTool.GetOwnNum(id)
	print("ldx","id = id ,num = itemNum: ",itemNum)
	self.__compounditemicon:SetData({id = id ,num = itemNum})

	local sp = Util:GetMoneyIconImgData(awaken_config.cost_money[1])
	local moneyNum = CallModuleFunc(ModuleType.ROLE,"GetPropValue",awaken_config.cost_money[1])
	--货币
	self.__image_coin:SetImage(sp)
	--货币数量
	self.__text_coinnum:SetText(awaken_config.cost_money[2])

	money_check = moneyNum >= awaken_config.cost_money[2]

	local compoundNum = #awaken_config.compose_item_id_list
	--容器Table
	for k,Panel in pairs(self.__panel_table) do
		if k == compoundNum then
			Panel:SetVisible(true)
		else
			Panel:SetVisible(false)
		end
	end
	--图标 String_Table
	local cheack_table = {}
	for i=1,compoundNum do
		if not self[self.__string_table[compoundNum].."_itemicon"..i] then
			self[self.__string_table[compoundNum].."_itemicon"..i] = LogicGUI.IconLayer.New(false,false)
			self[self.__string_table[compoundNum].."_itempos"..i]:AddChild(self[self.__string_table[compoundNum].."_itemicon"..i]:GetNode())
			PosTool.Center(self[self.__string_table[compoundNum].."_itemicon"..i])
			--数量
			self[self.__string_table[compoundNum].."_label"..i] = Util:Label("", Macros.TypefaceSize.minimum)
			self[self.__string_table[compoundNum].."_label"..i]:SetAnchorPoint(1,0)
			self[self.__string_table[compoundNum].."_itemicon"..i]:GetNode():AddChild(self[self.__string_table[compoundNum].."_label"..i],100)
			PosTool.RightBottom(self[self.__string_table[compoundNum].."_label"..i],72,6)
		end
		--self[self.__string_table[compoundNum].."_itempos"..i]:SetVisible(true)
		WidgetUnity.SetWidgetListener(self[self.__string_table[compoundNum].."_itempos"..i],nil,nil,function ()
			self:__ChooseView(awaken_config.compose_item_id_list[i])
		end)
		local itemnum = ObjIDTool.GetOwnNum(awaken_config.compose_item_id_list[i])		
		local str = itemnum.."/"..awaken_config.compose_item_num_list[i]
		self[self.__string_table[compoundNum].."_label"..i]:SetString(str)
		self[self.__string_table[compoundNum].."_label"..i]:SetColor(unpack(itemnum >= awaken_config.compose_item_num_list[i] and Macros.Color.white or Macros.Color.red))
		self[self.__string_table[compoundNum].."_label"..i]:SetStrokeColor(unpack(itemnum >= awaken_config.compose_item_num_list[i] and Macros.Color.button_yellow or Macros.Color.red_stroke))
		self[self.__string_table[compoundNum].."_label"..i]:SetStrokeWidth(2)
		table.insert(cheack_table,itemnum >= awaken_config.compose_item_num_list[i])
		self[self.__string_table[compoundNum].."_itemicon"..i]:SetData({id = awaken_config.compose_item_id_list[i]})
	end
	--材料检测
	for i,IsEnough in ipairs(cheack_table) do
		if not IsEnough then
			material_check = false
			break
		end 
	end
	if material_check and money_check then
		self.__compound_materialpass = true
	end
end

--选择界面
function LogicGUI.TemplHeroAwakenPropCompoundView:__ChooseView(id,isJustReturn)	
	--TODO：列表相关调整
	local awaken_config = CPPGameLib.GetConfig("AwakeningItem",id)
	if not isJustReturn then
		self.__open_index = self.__open_index + 1
		self.__id_table[self.__open_index] = id	
		local topData = {}
		topData.info_id = id
		topData.cur_index = self.__open_index
		self.__top_scrollview:AttachItem(topData)
		self.__top_scrollview:SelectItem(self.__open_index)
		self.__top_scrollview:LocateItem(self.__open_index)
	end
	if awaken_config.quality == Macros.Game.QualityType.WHITE then
		self:__OpenApproachView(id)
	else
		self:__OpenCompoundView(id)
	end
end

--关闭在它之后打开的界面
function LogicGUI.TemplHeroAwakenPropCompoundView:CloseBeforeThisView(pos)
	--TODO：列表相关调整
	if pos == self.__open_index then
		return
	end
	self.__open_index = pos
	local arrowsIndex = self.__open_index - 1
	local times = self.__top_scrollview:GetItemCount() - pos
	for i=1,times do
		self.__top_scrollview:DetachItem(self.__top_scrollview:GetItemCount())
	end
	self:__ChooseView(self.__id_table[self.__open_index],true)
end

function LogicGUI.TemplHeroAwakenPropCompoundView:__delete()
	if self.__top_scrollview then
		self.__top_scrollview:DeleteMe()
		self.__top_scrollview = nil
	end
	if self.__scrollview then
		self.__scrollview:DeleteMe()
		self.__scrollview = nil
	end
	for i=1,5 do
		if self["awaken_icon_pos"..i] then
			CPPActionManager.StopAllActions(self["awaken_icon_pos"..i])
			self["awaken_icon_pos"..i]:RemoveSelf(true)
			self["awaken_icon_pos"..i] = nil
		end
	end
end

--顶部头像栏头像
Modules = Modules or {}

Modules.TemplHeroAwakenTopIconCell = Modules.TemplHeroAwakenTopIconCell or BaseClass(GUI.ScrollItem) 

function Modules.TemplHeroAwakenTopIconCell:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplHeroAwakenTopIconCell:__CreateWidgetFromLuaCode()
  self.__layout = Util:Layout(80,94)
  self.__layout:SetAnchorPoint(0,0)
  self.__layout:SetClippingEnable(false)
  return self.__layout
end

function Modules.TemplHeroAwakenTopIconCell:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	 --头像
	if not self.__icon then
		self.__icon = LogicGUI.IconLayer.New(false,false)
	    self.__layout:AddChild(self.__icon:GetNode())
	    self.__icon:SetAnchorPoint(0.5,1)
	    PosTool.CenterTop(self.__icon)  
	end   
    self.__icon:SetData({id = data.info_id})
    if self.__data.cur_index > 1 then
	    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.HERO_AWAKEN_PROP_COMPOSE, "arrows")
	    local sprite = SpriteEx.CreateWithImageFile(sp)
	    sprite:SetAnchorPoint(0.5,0.5)
	    self.__layout:AddChild(sprite)
	    PosTool.LeftCenter(sprite,-24,0)
	end
end

function Modules.TemplHeroAwakenTopIconCell:SetSelected(var)
	if var then
		if not self.__selected_image then
		    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW,"bg_sp9_selected")
			self.__selected_image = Util:Sprite(sp)
			self.__icon:AddChild(self.__selected_image,3)
			PosTool.Center(self.__selected_image)
			sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.HERO_AWAKEN_PROP_COMPOSE,"downarrows")
			local sprite = Util:Sprite(sp)
			self.__selected_image:AddChild(sprite)
			PosTool.CenterBottom(sprite,0,-4)
		end
	else
		if self.__selected_image then
			self.__selected_image:RemoveSelf(true)
			self.__selected_image = nil
		end
	end	
end


--
-- @brief 获取途径条目
-- @author: ldx
-- @date: 2016年10月31日15:17:54
--

Modules = Modules or {}

Modules.TemplSpecialApproachOfAchievingCell = Modules.TemplSpecialApproachOfAchievingCell or BaseClass(GUI.ScrollItem)

function Modules.TemplSpecialApproachOfAchievingCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.HERO_AWAKEN_SPECIAL_APPROACH_CELL)
end

function Modules.TemplSpecialApproachOfAchievingCell:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	--图标
	local panel_itemicon = self:GetWidget("Panel_itemIcon")
	local icon = UIImageViewEx.CreateWithImage(Resource.PathTool.GetSystemEntryIconPath(data.approach_icon_res_name),TextureResType.LOCAL)
	icon:IgnoreContentAdaptWithSize(false)
	icon:SetContentSize(70,70)
	panel_itemicon:AddChild(icon)
	PosTool.Center(icon)
	--名字
	Util:WidgetLabel(self:GetWidget("Text_name"), data.approach_name, Macros.TypefaceSize.normal, Macros.Color.keypoint)
	--描述
	Util:WidgetLabel(self:GetWidget("Text_des"), data.approach_des, Macros.TypefaceSize.slightly, Macros.Color.content)
	--限制次数
	local limitTimes = self:GetWidget("Text_times")
	limitTimes:SetVisible(data.limit_times_str ~= nil)
	Util:WidgetLabel(limitTimes,data.limit_times_str or "", Macros.TypefaceSize.slightly,Macros.Color.keypoint)
	--前往按钮
	local Button_achieving = self:GetWidget("Button_achieving")
	Button_achieving:SetVisible(data.isOpen)
	WidgetUnity.SetWidgetListener(Button_achieving, nil, nil, function ()
		self:FireNextFrame(Macros.Event.ApproachOfAchievingModule.DELETE_APPROACHVIEW)
		GlobalModulesMgr:OpenSystemModule(data.approach_systemid,data.extra_view_params)	
	end, nil)
	--未开启文字
	local text_unlocked = self:GetWidget("Text_unlocked")
	text_unlocked:SetVisible(not data.isOpen)
end

function Modules.TemplSpecialApproachOfAchievingCell:__delete()
	
end