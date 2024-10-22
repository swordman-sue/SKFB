LogicGUI = LogicGUI or {}

LogicGUI.TemplHeroAwakenPropView = LogicGUI.TemplHeroAwakenPropView or BaseClass(GUI.Template)

local HeroAwakenPropViewType = {
	NO_BUTTON = 1,
	ONLY_APPROACH = 2,
	EQUIP_UP = 3,
	APPROACH_AND_COMPOUND = 4,
	ONLY_SURE = 5,
}

function LogicGUI.TemplHeroAwakenPropView:__init(id,type,uid)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:InitUI(id,type,uid)
	end
	self:InitTemplate(GUI.TemplateRegistry.HERO_AWAKEN_PROP)
end

function LogicGUI.TemplHeroAwakenPropView:InitUI(id,type,uid)
	--配置信息
	local item_config = CPPGameLib.GetConfig("AwakeningItem",id) 
	if not item_config then
		print("ldx","配置表没有这东西啊: ",id)
	end
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("hero_awaken_propTitle"), Macros.TypefaceSize.tab, Macros.Color.btn)
	--关闭按钮
	Util:WidgetButton(self:GetWidget("Button_close"),"",function ()
		self:DeleteMe()
	end)	
	--头像
	local panel_headicon = self:GetWidget("Panel_headIcon")
	local icon = LogicGUI.IconLayer.New(false,false)
	panel_headicon:AddChild(icon:GetNode())
	PosTool.Center(icon)
	icon:SetData({id = id})
	--名字
	Util:WidgetLabel(self:GetWidget("Text_propName"),item_config.name, Macros.TypefaceSize.tab, Macros.Color.headline )
	--拥有
	Util:WidgetLabel(self:GetWidget("Text_have"),CPPGameLib.GetString("hero_awaken_haveText"),Macros.TypefaceSize.largish, Macros.Color.keypoint )
	--拥有数量
	local num = ObjIDTool.GetOwnNum(id)
	Util:WidgetLabel(self:GetWidget("Text_haveNum"),num,Macros.TypefaceSize.largish, Macros.Color.content )
	--描述
	local rich_txt = Util:RichText(item_config.des, Macros.TypefaceSize.normal, 424, 70,Macros.Color.content_hex)
	rich_txt:SetAnchorPoint(0,1)	
	self:GetWidget("Panel_awakenProp"):AddChild(rich_txt) 
	rich_txt:SetPosition(120,316)
	--属性
	local txt_str
	for i,pro_type in ipairs(item_config.pro_type_list) do
		local text_property = self:GetWidget("Text_property"..i)
		local text_propertyNum = self:GetWidget("Text_propertyNum"..i)
		text_property:SetVisible(true)
		text_propertyNum:SetVisible(true)
		txt_str = CPPGameLib.GetString("nature"..pro_type)
		Util:WidgetLabel(text_property,CPPGameLib.GetString("common_str_with_colon",txt_str),Macros.TypefaceSize.largish, Macros.Color.keypoint)
		Util:WidgetLabel(text_propertyNum,item_config.pro_value_list[i],Macros.TypefaceSize.largish, Macros.Color.content)
	end			
	--获取按钮
	local button_gain = self:GetWidget("Button_gain")
	--合成按钮
	local button_compound = self:GetWidget("Button_compound")	
	if type == HeroAwakenPropViewType.NO_BUTTON then
		button_gain:SetVisible(false)
		button_compound:SetVisible(false)
	elseif type == HeroAwakenPropViewType.ONLY_APPROACH then
		--获取途径
		button_compound:SetVisible(false)
		Util:WidgetButton(button_gain,CPPGameLib.GetString("hero_awaken_buttonText1"),function ()
			self:DeleteMe()
			GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, id)
		end,Macros.TypefaceSize.popup,Macros.Color.btn,Macros.Color.button_yellow)
		PosTool.CenterBottom(button_gain,0,51)
	elseif type == HeroAwakenPropViewType.EQUIP_UP then
		--装备
		button_compound:SetVisible(false)
		Util:WidgetButton(button_gain,CPPGameLib.GetString("hero_awaken_buttonText3"),function ()
			CallModuleFunc(ModuleType.HERO,"HeroEquipAwakenItemReq",uid,id)
		end,Macros.TypefaceSize.popup,Macros.Color.btn,Macros.Color.button_yellow)
		PosTool.CenterBottom(button_gain,0,51)
	elseif type == HeroAwakenPropViewType.APPROACH_AND_COMPOUND then
		--获取
		Util:WidgetButton(button_gain,CPPGameLib.GetString("hero_awaken_buttonText1"),function ()
			self:DeleteMe()
			GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, id)
		end,Macros.TypefaceSize.popup,Macros.Color.btn,Macros.Color.button_yellow)
		--合成
		Util:WidgetButton(button_compound,CPPGameLib.GetString("hero_awaken_buttonText2"),function ()
			self:DeleteMe()
			LogicGUI.TemplHeroAwakenPropCompoundView.New(id)
		end,Macros.TypefaceSize.popup,Macros.Color.btn,Macros.Color.button_red)
	elseif type == HeroAwakenPropViewType.ONLY_SURE then
		button_compound:SetVisible(false)
		--确定
		Util:WidgetButton(button_gain,CPPGameLib.GetString("hero_awaken_buttonText4"),function ()
			self:DeleteMe()
		end,Macros.TypefaceSize.popup,Macros.Color.btn,Macros.Color.button_yellow)
		PosTool.CenterBottom(button_gain,0,51)
	end
end