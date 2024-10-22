
--
-- @brief 确认弹窗
-- @author: yjg
-- @date: 2016年10月28日17:51:53
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplAffirmTips = LogicGUI.TemplAffirmTips or BaseClass(GUI.Template)

LogicGUI.TemplAffirmTipsType = 
{
	YES_NO = 1,
	YES = 2,
}

--[[
@ params：
	必须：
	type 			[类型]默认为LogicGUI.TemplAffirmTipsType.YES_NO
	content			[string]提示内容
	confirm_text	[string]确认按钮文字 
	confirm_func	[function]确认按钮回调

	可选：
	title			[string]标题(默认“提示”)
	cancel_text		[string]取消按钮文字
	cancel_func		[function]取消按钮回调
	close_func 		[function]关闭按钮回调(默认为cancel_func)
	render_queue 	[number]渲染层次
	show_close_btn 	[bool]是否显示关闭按钮(默认true)
	show_checkbox_content 	[string]选项卡显示内容(默认为空)
--]]
function LogicGUI.TemplAffirmTips:__init(params)
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__render_queue = params.render_queue or Scene.RenderQueue.GRQ_UI_UP	

	self.__is_auto_load = true
	self.__open_callback = function()
		self:__InitWidget()

		self:SetType(params.type or LogicGUI.TemplAffirmTipsType.YES_NO)
		self:SetHeadline(params.title or CPPGameLib.GetString("common_tips"))
		self:SetContent(params.content)
		self:SetCheckBox(params.show_checkbox_content) 
		self:SetCloseInfo(params.show_close_btn == nil and true or params.show_close_btn,params.close_func or params.cancel_func)
		self:SetConfirmInfo(params.confirm_text, params.confirm_func)
		self:SetCancelInfo(params.cancel_text, params.cancel_func)
	end

	self:InitTemplate(GUI.TemplateRegistry.TEMPL_AFFIRM_TIPS)	
end

function LogicGUI.TemplAffirmTips:SetType(type)
	self.__type = type

	if self.__type == LogicGUI.TemplAffirmTipsType.YES_NO then

	elseif self.__type == LogicGUI.TemplAffirmTipsType.YES then
		self.btn_yes:SetAnchorPoint(0.5, 0.5)
		self.btn_no:SetVisible(false)
		self.btn_close:SetVisible(false)
		PosTool.Center(self.btn_yes)
	end
end

function LogicGUI.TemplAffirmTips:SetHeadline(tab)
	self.txt_headline:SetText(tab)
end

function LogicGUI.TemplAffirmTips:SetContent(tab)
	self.txt_content:SetText(tab)	
end

function LogicGUI.TemplAffirmTips:GetContent(tab)
	return self.txt_content
end

function LogicGUI.TemplAffirmTips:SetCloseInfo(is_show, close_func)
	self.btn_close:SetVisible(is_show)
	self.close_func = close_func
end

function LogicGUI.TemplAffirmTips:SetConfirmInfo(confirm_text, confirm_func)
	if confirm_text then
		self.btn_yes:SetText(confirm_text)
	end
	self.confirm_func = confirm_func
end

function LogicGUI.TemplAffirmTips:SetCancelInfo(cancel_text, cancel_func)
	if cancel_text then
		self.btn_no:SetText(cancel_text)
	end
	self.cancel_func = cancel_func
end

function LogicGUI.TemplAffirmTips:OnBtnClose()
	if self.close_func then
		self.close_func()

	elseif self.cancel_func then
		self.cancel_func()
	end
	self:DeleteMe()
end

function LogicGUI.TemplAffirmTips:OnButtonYes()
	if self.confirm_func then
		self.confirm_func(self.__check_box:GetSelectedState())
	end
	self:DeleteMe()
end

function LogicGUI.TemplAffirmTips:OnButtonNo()
	if self.cancel_func then
		self.cancel_func()
	end
	self:DeleteMe()
end

function LogicGUI.TemplAffirmTips:SetCheckBox(show_checkbox_content)
	self.__check_box:SetVisible(show_checkbox_content ~= nil)
	self.txt_checkboxtips:SetVisible(show_checkbox_content ~= nil)
	self.txt_checkboxtips:SetText(show_checkbox_content ~= nil and show_checkbox_content or "")	
	if show_checkbox_content ~= nil then
		self.txt_content:SetPosition(self.txt_content:GetPositionX(),self.txt_content:GetPositionY()+25)
	end
end

function LogicGUI.TemplAffirmTips:__InitWidget()
	--标题
	self.txt_headline = Util:WidgetLabel(self:GetWidget("Text_headline"), "", Macros.TypefaceSize.largish, Macros.Color.lilyWhite)
	--内容
	local img_content = self:GetWidget("Image_lstBg")
	self.txt_content = Util:WidgetLabel(self:GetWidget("Text_content"), "" , nil , Macros.Color.content , img_content:GetContentWidth() - 12)
	--关闭按钮
	self.btn_close = Util:WidgetButton( self:GetWidget("Button_close") , " " , CPPGameLib.Handler(self,self.OnBtnClose) )
	--确定按钮
	self.btn_yes = Util:WidgetButton( 
										self:GetWidget("Button_yes") , 
										CPPGameLib.GetString("common_confirm") , 
										CPPGameLib.Handler(self,self.OnButtonYes) , 
										Macros.TypefaceSize.largish , nil , Macros.Color.button_red )
	--取消按钮
	self.btn_no = Util:WidgetButton( 
										self:GetWidget("Button_no") ,
										CPPGameLib.GetString("common_cancel") , 
										CPPGameLib.Handler(self,self.OnButtonNo) , 
										Macros.TypefaceSize.largish , nil , Macros.Color.button_yellow )
	--选项卡
	self.__check_box = self:GetWidget("CheckBox")
	self.__check_box:SetSelectedState(false)
	--选项卡提示
	self.txt_checkboxtips = Util:WidgetLabel(self:GetWidget("Text_CheckBoxTips"), "", Macros.TypefaceSize.largish, Macros.Color.red)
end
