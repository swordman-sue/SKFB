
--
-- @brief 爬塔重置界面
-- @author: yjg
-- @date: 2017年3月3日17:26:16
--

LogicGUI = LogicGUI or {}

LogicGUI.TempTowerReset = LogicGUI.TempTowerReset or BaseClass(GUI.Template)

function LogicGUI.TempTowerReset:__init(params)
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__render_queue = Scene.RenderQueue.GRQ_UI_UP	
	self.__is_auto_load = true
	self.__open_callback = function()
		self:__InitWidget()
	end

	self:InitTemplate(GUI.TemplateRegistry.TEMPL_AFFIRM_TIPS)	
end

function LogicGUI.TempTowerReset:__InitWidget()
	--标题
	self.txt_headline = Util:WidgetLabel(self:GetWidget("Text_headline"), CPPGameLib.GetString("common_tips") , Macros.TypefaceSize.largish, Macros.Color.lilyWhite)
	
	--关闭按钮
	self.btn_close = Util:WidgetButton( self:GetWidget("Button_close") , " " , CPPGameLib.Handler(self,self.OnBtnClose) )

	--确定按钮
	self.btn_yes = Util:WidgetButton( 
										self:GetWidget("Button_yes") , 
										CPPGameLib.GetString("common_confirm") , 
										CPPGameLib.Handler(self,self.OnButtonYes) , 
										Macros.TypefaceSize.largish , nil , Macros.Color.button_red)

	--取消按钮
	self.btn_no = Util:WidgetButton( 
										self:GetWidget("Button_no") ,
										CPPGameLib.GetString("common_cancel") , 
										CPPGameLib.Handler(self,self.OnButtonNo) , 
										Macros.TypefaceSize.largish , nil , Macros.Color.button_yellow)


	--内容
	local img_content = self:GetWidget("Image_lstBg")
	local lst = CallModuleFunc(ModuleType.TOWER, "GetResetTime")

	self.value = lst[1].value

	if lst[1].type == "free" then
		self.txt_content = Util:WidgetLabel(
											self:GetWidget("Text_content"), 
											CPPGameLib.GetString("tower_freeTips") , nil , 
											Macros.Color.content , img_content:GetContentWidth() - 12
										)
	
	else
		if lst[1].value ~= 0 then
			self.txt_content = Util:LabCenter_3( { 
											{ zi = CPPGameLib.GetString("tower_expenditureAffirm_1") , ys = Macros.Color.content , dx = Macros.TypefaceSize.normal },
											{ tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.DIAMOND) ,dx = 1.3 , gs = lst[1].value , gsdx = Macros.TypefaceSize.normal },
											{ zi = CPPGameLib.GetString("tower_purchaseAffirm") , ys = Macros.Color.content , dx = Macros.TypefaceSize.normal },
										} , Macros.TypefaceSize.normal , img_content:GetContentWidth() - 12 )

			self.txt_content:SetPosition(self:GetWidget("Text_content"):GetPositionX() , self:GetWidget("Text_content"):GetPositionY() + 40 )
			self:GetWidget("Image_bg"):AddChild(self.txt_content)
		else

			local type_ , value_ = CallModuleFunc(ModuleType.VIP , "ToTypeGetResetItem" , Macros.Game.VIPType.KRUUNI_RESET )
			--没有更高级
			if type_ == false then
				self.txt_content = Util:Label( CPPGameLib.GetString("tower_resetMax") , Macros.TypefaceSize.normal , Macros.Color.content  )
				self.txt_content:SetPosition(self:GetWidget("Text_content"):GetPositionX() , self:GetWidget("Text_content"):GetPositionY() + 40 )
				self:GetWidget("Image_bg"):AddChild(self.txt_content)
			else
				self.btn_yes:SetText( CPPGameLib.GetString("vip_Toimprove") )
				self.btn_no:SetText( CPPGameLib.GetString("vip_Isee") )

				self.txt_content = Util:Label( CPPGameLib.GetString("tower_noResetTime") , Macros.TypefaceSize.normal , Macros.Color.content , self:GetWidget("Image_bg"):GetContentWidth() - 70  )
				self.txt_content:SetPosition(self:GetWidget("Text_content"):GetPositionX() , self:GetWidget("Text_content"):GetPositionY() + 20 )
				self:GetWidget("Image_bg"):AddChild(self.txt_content)
				self.txt_content:SetTextHorizontalAlign(LabelEx.ALIGIN_CENTER)
			end
		end

		local num_ = CallModuleFunc(ModuleType.TOWER , "GetTowerResetTimes")
		if num_ ~= 0 then
			local arena_today_buyTimes = Util:Label( CPPGameLib.GetString("tower_buyTimes" , num_) , Macros.TypefaceSize.normal , Macros.Color.golden )
			arena_today_buyTimes:SetAnchorPoint(0.5,0.5)
			arena_today_buyTimes:SetPosition(self:GetWidget("Text_content"):GetPositionX() , self:GetWidget("Text_content"):GetPositionY() - 40 )
			self:GetWidget("Image_bg"):AddChild(arena_today_buyTimes)
		end
	end












































end


function LogicGUI.TempTowerReset:OnBtnClose()
	self:DeleteMe()
end

function LogicGUI.TempTowerReset:OnButtonYes()

	local num_ = CallModuleFunc(ModuleType.TOWER , "GetTowerResetTimes")
	if num_ <= 0 then
		local type_ , value_ = CallModuleFunc(ModuleType.VIP , "ToTypeGetResetItem" , Macros.Game.VIPType.KRUUNI_RESET )
		if type_ ==  true then
			CallModuleFunc(ModuleType.CHARGE, "OpenView", BaseViewType.CHARGE)
		else
			GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("tower_resetMax") )
		end
		self:OnButtonNo()
		return
	end

	local diamond = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.DIAMOND)

	if diamond < self.value then
		local txt = "currency_insufficient_" .. Macros.Game.RolePropType.DIAMOND
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString(txt) )
	else
		CallModuleFunc(ModuleType.TOWER , "TowerResetReq")
	end

	self:OnButtonNo()
end

function LogicGUI.TempTowerReset:OnButtonNo()
	self:DeleteMe()
end