LogicGUI = LogicGUI or {}

LogicGUI.TemplDungeonAddAssistRoleView = LogicGUI.TemplDungeonAddAssistRoleView or BaseClass(GUI.Template)

function LogicGUI.TemplDungeonAddAssistRoleView:__init(role_id,role_name,fun)
	self.__is_auto_load = true	
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:initUI(role_id,role_name,fun)
	end
	self:InitTemplate(GUI.TemplateRegistry.PLUNDERED_TREASURES_RELIEVESTATETIPVIEW)
end

function LogicGUI.TemplDungeonAddAssistRoleView:initUI(role_id,role_name,fun)
	--标题	
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("plunderedtreasures_tipsTitle"), Macros.TypefaceSize.popup, Macros.Color.btn)
	--内容	
	local txt_tip = self:GetWidget("Text_tip")
	Util:WidgetLabel(txt_tip,CPPGameLib.GetString("dungeon_add_assist_target_tips",role_name), Macros.TypefaceSize.normal, Macros.Color.content,350,50)
	txt_tip:SetPosition(txt_tip:GetPositionX(),txt_tip:GetPositionY()+25)
	--关闭按钮
	Util:WidgetButton(self:GetWidget("Button_close"),"",function ()
		if fun then
			fun(self.__checkbox:GetSelectedState())
		end
		self:DeleteMe()
	end)
	--取消按钮
	Util:WidgetButton(self:GetWidget("Button_cancel"),CPPGameLib.GetString("common_cancel"),function ()
		if fun then
			fun(self.__checkbox:GetSelectedState())
		end
		self:DeleteMe()
	end,Macros.TypefaceSize.largish,Macros.Color.btn,Macros.Color.button_yellow)
	--确定按钮
	Util:WidgetButton(self:GetWidget("Button_sure"),CPPGameLib.GetString("common_confirm"),function ()
		if fun then
			fun(self.__checkbox:GetSelectedState())
		end
		CallModuleFunc(ModuleType.FRIEND,"FriendAddRequest",{target_id = role_id})
		self:DeleteMe()
	end,Macros.TypefaceSize.largish,Macros.Color.btn,Macros.Color.button_red)
	--提示
	local txt_checkbox_tips = self:GetWidget("Text_CheckBoxTips")
	txt_checkbox_tips:SetVisible(true)
	Util:WidgetLabel(txt_checkbox_tips,CPPGameLib.GetString("common_no_show"), Macros.TypefaceSize.normal, Macros.Color.red)
	--选项卡
	self.__checkbox = self:GetWidget("CheckBox")
	self.__checkbox:SetVisible(true)
	self.__checkbox:SetSelectedState(false)
end

