LogicGUI = LogicGUI or {}

LogicGUI.TemplWorldHeartPropertyView = LogicGUI.TemplWorldHeartPropertyView or BaseClass(GUI.Template)

function LogicGUI.TemplWorldHeartPropertyView:__init(data)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:initUI(data)
	end
	self:InitTemplate(GUI.TemplateRegistry.WORLD_HEART_PROPERTY)	
end

function LogicGUI.TemplWorldHeartPropertyView:initUI(data)
	--关闭按钮
	WidgetUnity.SetWidgetListener(self:GetWidget("Button_close"),nil,nil,function ()
		self:DeleteMe()
	end)
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("spirit_propertyText"),Macros.TypefaceSize.popup,Macros.Color.btn)
	--确定
	Util:WidgetButton(self:GetWidget("Button_sure"), CPPGameLib.GetString("spirit_sure"),function ()
		self:DeleteMe()
	end,Macros.TypefaceSize.button,Macros.Color.btn,Macros.Color.button_yellow)	
	--初始化文字
	local panel_spiritProperty = self:GetWidget("Panel_spiritProperty")
	local count = 0
	local double_count = 0
	for prop_type,prop_value in pairs(data) do
		local str = Macros.Game.BattlePropName[prop_type]
		local label = Util:Label(str, Macros.TypefaceSize.normal, Macros.Color.content)
		panel_spiritProperty:AddChild(label)
		label:SetAnchorPoint(0,0.5)
		label:SetPosition((count%2) == 0 and 54 or 258,244 - double_count*40)
		local is_percent,pro_str = Util:GroupType(prop_type , prop_value)
		str = is_percent and pro_str or prop_value
		local label_value = Util:Label("+"..str, Macros.TypefaceSize.normal, Macros.Color.front_color)
		panel_spiritProperty:AddChild(label_value)
		label_value:SetAnchorPoint(0,0.5)
		label_value:SetPosition(label:GetContentWidth() + label:GetPositionX() + 10,244 - double_count*40)
		count = count + 1
		if count%2 == 0 then
			double_count = double_count + 1
		end
	end
end
