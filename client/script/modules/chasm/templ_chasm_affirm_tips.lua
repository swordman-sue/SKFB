
--
-- @brief 确认弹窗
-- @author: yjg
-- @date: 2016年10月28日17:51:53
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplChasmAffirmTips = LogicGUI.TemplChasmAffirmTips or BaseClass(GUI.Template)

--[[
@ params：
	必须：
	type 			[类型]默认为LogicGUI.TemplChasmAffirmTipsType.YES_NO
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
--]]
function LogicGUI.TemplChasmAffirmTips:__init(params)
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__is_auto_load = true
	self.currencyKey = false
	self.__open_callback = function()
		self:__InitWidget()
	end

	self:InitTemplate(GUI.TemplateRegistry.TEMPL_AFFIRM_TIPS)	
end

function LogicGUI.TemplChasmAffirmTips:__InitWidget()
	--标题
	self.txt_headline = Util:WidgetLabel(self:GetWidget("Text_headline"), CPPGameLib.GetString("plunderedtreasures_tipsTitle") , Macros.TypefaceSize.largish, Macros.Color.lilyWhite)
	--内容
	local img_content = self:GetWidget("Image_lstBg")
	self.txt_content = Util:WidgetLabel(self:GetWidget("Text_content"), "" , nil , Macros.Color.content , img_content:GetContentWidth() - 12)
	PosTool.MoveBy( self.txt_content , 0 , 15 )

	self.txt_content1 = Util:Label( CPPGameLib.GetString("chasm_vipAccess" , "" , "" ) )
	self.txt_content:AddChild( self.txt_content1 )
 	PosTool.Center( self.txt_content1 , 0 , -30 )

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
	self:SetShowUI()
end

function LogicGUI.TemplChasmAffirmTips:SetShowUI()
	--持有道具数量    所需道具数量
    local item_num , consume , item_type = CallModuleFunc(ModuleType.CHASM, "GetConsume" )
    --信息
   	local type_ , info_ = ObjIDTool.GetInfo(item_type)
   	if type_ == Macros.Game.ObjType.CURRENCY then
	   	--货币满足
	    -- if item_num >= consume then
	    	self.txt_content:SetText( CPPGameLib.GetString("chasm_accessExpend" , info_.name , consume ) )
	    -- end
   	else
	   	--道具满足
	    -- if item_num >= consume then
	    	self.txt_content:SetText( CPPGameLib.GetString("chasm_accessExpend" , info_.name , consume ) )
	    -- end
   	end

   	local today_battle_times , vipTimes , freeTimes = CallModuleFunc(ModuleType.CHASM, "GetEntranceTime" )
   	self.txt_content1:SetString( CPPGameLib.GetString("chasm_vipAccess" , vipTimes - today_battle_times , vipTimes - freeTimes ) )

    PosTool.Center( self.txt_content1 , 0 , -30 )
end

function LogicGUI.TemplChasmAffirmTips:OnButtonYes()

	--持有道具数量    所需道具数量
    local item_num , consume , item_type = CallModuleFunc(ModuleType.CHASM, "GetConsume" )
    --信息
   	local type_ , info_ = ObjIDTool.GetInfo(item_type)
   	if type_ == Macros.Game.ObjType.CURRENCY then
	   	--货币不满足
	    if item_num < consume then
	    	if self.currencyKey == true then
	    		self.currencyKey = false
	    		self.txt_content:SetString( CPPGameLib.GetString("fund_noDiamond") )
				self.txt_content1:SetString("")
	    	else
		    	GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("hero_potential_tips2" , info_.name ) )
		    	CallModuleFunc(ModuleType.CHARGE, "OpenView", BaseViewType.CHARGE )
		    	self:DeleteMe()
	    	end
	    	return
	    end
   	else
	   	-- 道具不满足
	    if item_num < consume then
	    	GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, item_type)
	    	GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("hero_potential_tips2" , info_.name ) )
	    	self:DeleteMe()
	    	return
	    end
   	end

   	CallModuleFunc(ModuleType.CHASM, "EndlessLandEnterReq" )
   	
	self:DeleteMe()
end

function LogicGUI.TemplChasmAffirmTips:OnBtnClose()
	self:DeleteMe()
end

function LogicGUI.TemplChasmAffirmTips:OnButtonNo()
	self:DeleteMe()
end