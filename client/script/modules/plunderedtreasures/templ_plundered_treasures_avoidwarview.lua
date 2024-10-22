LogicGUI = LogicGUI or {}

LogicGUI.TemplPlunderedTreasuresAvoidWarView = LogicGUI.TemplPlunderedTreasuresAvoidWarView or BaseClass(GUI.Template)

function LogicGUI.TemplPlunderedTreasuresAvoidWarView:__init()
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:initUI()
	end
	self:InitTemplate(GUI.TemplateRegistry.PLUNDERED_TREASURES_AVOIDVIEW)	
end

function LogicGUI.TemplPlunderedTreasuresAvoidWarView:initUI()
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("plunderedtreasures_AvoidWarTitle"), Macros.TypefaceSize.popup,Macros.Color.btn)
	--关闭按钮
	Util:WidgetButton(self:GetWidget("Button_close"), "",function ()
		self:DeleteMe()
	end)
	--提示1
	local richText = Util:RichText(CPPGameLib.GetString("plunderedtreasures_AvoidWarText_1"), Macros.TypefaceSize.normal,0,0,Macros.Color.keypoint_hex)
	self:GetWidget("Image_6"):AddChild(richText)
	richText:SetPosition(217,237)
	--提示2
	Util:WidgetLabel(self:GetWidget("Text_tip2"),CPPGameLib.GetString("plunderedtreasures_AvoidWarText_4"), Macros.TypefaceSize.normal,Macros.Color.content)
	--免战8小时
	local Panel_eight = self:GetWidget("Panel_eight")
	local icon = LogicGUI.IconLayer.New(false,true)
	Panel_eight:AddChild(icon:GetNode())
	PosTool.Center(icon)
	icon:SetData({id = ConfigAdapter.Common.GetAvoidWarCardBigId()})
	icon:adaptParentContentSize()
	icon:SetTouchCallback(function ()
		LogicGUI.TemplPlunderedTreasuresBuyDaoJuView.New(ConfigAdapter.Common.GetAvoidWarCardBigId())
	end)
	--免战1小时
	local Panel_one = self:GetWidget("Panel_one")
	local icon = LogicGUI.IconLayer.New(false,true)
	Panel_one:AddChild(icon:GetNode())
	PosTool.Center(icon)
	icon:SetData({id = ConfigAdapter.Common.GetAvoidWarCardSmallId()})
	icon:adaptParentContentSize()
	icon:SetTouchCallback(function ()
		LogicGUI.TemplPlunderedTreasuresBuyDaoJuView.New(ConfigAdapter.Common.GetAvoidWarCardSmallId())
	end)
	--免战时间
	local itemConfig = CPPGameLib.GetConfig("Item",ConfigAdapter.Common.GetAvoidWarCardBigId())
	Util:WidgetLabel(self:GetWidget("Text_avoidEightTime"),CPPGameLib.GetString("plunderedtreasures_AvoidWarText_2"), Macros.TypefaceSize.normal,Macros.Quality[itemConfig.quality],0,0,Macros.Quality_Stroke[itemConfig.quality])
	itemConfig = CPPGameLib.GetConfig("Item",ConfigAdapter.Common.GetAvoidWarCardSmallId())
	Util:WidgetLabel(self:GetWidget("Text_avoidOneTime"),CPPGameLib.GetString("plunderedtreasures_AvoidWarText_3"), Macros.TypefaceSize.normal,Macros.Quality[itemConfig.quality],0,0,Macros.Quality_Stroke[itemConfig.quality])
end