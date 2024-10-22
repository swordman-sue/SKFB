LogicGUI = LogicGUI or {}

LogicGUI.TemplEmailReply = LogicGUI.TemplEmailReply or BaseClass(GUI.Template)

function LogicGUI.TemplEmailReply:__init(txt)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID,color = {0, 0, 0, 0}}
	self.__open_callback = function ()
		self:InitMainUI(txt)
	end
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_EMAIL_REPLY)
end

function LogicGUI.TemplEmailReply:InitMainUI(txt)
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("email_reply_title"), Macros.TypefaceSize.popup, Macros.Color.headline)
	--关闭按钮
	Util:WidgetButton(self:GetWidget("Button_close"),"",function ()
		self:DeleteMe()
	end)
	--描述
	local des_txt = Util:RichText("",Macros.TypefaceSize.normal, 325, 0,Macros.Color.content_hex)
	des_txt:SetAnchorPoint(0,1)
	des_txt:SetFontSpacing(6)
	self:GetWidget("Panel_EmailReply"):AddChild(des_txt)
	des_txt:SetText(GUI.RichText.GetColorWord(txt or "", Macros.Color.content_hex))
	des_txt:SetPosition(309,419)	
end

