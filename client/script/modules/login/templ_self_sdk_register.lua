
--
-- @brief 自用登陆界面
-- @author: yjg
-- @date: 2017年5月16日15:09:29
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplSelfSdkRegister = LogicGUI.TemplSelfSdkRegister or BaseClass(GUI.Template)

function LogicGUI.TemplSelfSdkRegister:__init( func )
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__render_queue = Scene.RenderQueue.GRQ_UI_UP	

	self.__is_auto_load = true
	self.func = func
	self.__open_callback = function()
		self.time = 0
		self:__Getwidget()
		self:__ShowUI()
	end
	
	--新信息
	self.register = self:BindGlobalEvent(Macros.Event.GameState.LOGIN_ACCOUNT_SUCCESS, function(_,is_bind_mobile, is_one_key)
		if is_one_key then
			self:OpenPanelCutTips2()

		elseif is_bind_mobile then
			self:OpenBinding()

		else
			self:OnClose()
		end
	end)
	
	self.error_code = self:BindGlobalEvent(Macros.Event.ErrorNotifyModule.ERROR_NOTIFY, function(_,error_code)									
		if error_code == 1000 or error_code == 1008 then
			self:__Concealwidget()
			self:OpenRegister2()
		elseif error_code == 1007 then
			self:__Concealwidget()
			self:OpenCellphoneRegister()
		elseif error_code == 1009 then
			self:OpenBinding()
		end
	end)


	self.account_remove = self:BindGlobalEvent(Macros.Event.GameState.LOGIN_ACCOUNT_REMOVE, function(_,error_code)									
		self:SetUserRegisterMeunLst()
	end)

	self:InitTemplate(GUI.TemplateRegistry.TEMPL_SDKREGISTER)
end

function LogicGUI.TemplSelfSdkRegister:__delete()

end

--获得组件
function LogicGUI.TemplSelfSdkRegister:__Getwidget()
	self.image_little_logo = self:GetWidget("Image_little_logo")
	self.image_little_logoline = self:GetWidget("Image_little_logoline")
	self.image_big_logo = self:GetWidget("Image_big_logo")
	self.image_little_logo:IgnoreContentAdaptWithSize(true)
	self.image_little_logoline:IgnoreContentAdaptWithSize(true)
	self.image_big_logo:IgnoreContentAdaptWithSize(true)

	--白底
	self.Image_white_bg = self:GetWidget("Image_white_bg")
	--登陆界面1
	self.Panel_register1 = self:GetWidget("Panel_register1")
		--当前登陆账号
		self.Text_register1_account = self:GetWidget("Text_register1_account")
		--登陆游戏
		self.Button_register1_register = Util:WidgetButton( 
																self:GetWidget("Button_register1_register") , 
																CPPGameLib.GetString("login_registerGame") , 
																CPPGameLib.Handler(self,self.OnButtonRegister1Register) , 
																Macros.TypefaceSize.headlineII , Macros.Color.lilyWhite
															)
		--切换账号
		self.Button_register1_cut = Util:WidgetButton( 
																self:GetWidget("Button_register1_cut") , 
																CPPGameLib.GetString("login_cutGame") , 
																CPPGameLib.Handler(self,self.OnButtonRegister1Cut) , 
																Macros.TypefaceSize.headlineII , Macros.Color.lilyWhite
															)

	--登陆界面2
	self.Panel_register2 = self:GetWidget("Panel_register2")
		--一键登陆
		self.Button_register2_oneKey = Util:WidgetButton( 
																self:GetWidget("Button_register2_oneKey") , 
																CPPGameLib.GetString("login_oneKeyRegister") , 
																CPPGameLib.Handler(self,self.OnButtonRegister2OneKey) , 
																Macros.TypefaceSize.headlineII , Macros.Color.lilyWhite
															)
		--手机登陆
		self.Button_register2_register = Util:WidgetButton( 
																self:GetWidget("Button_register2_register") , 
																CPPGameLib.GetString("login_handsetRegister") , 
																CPPGameLib.Handler(self,self.OnButtonRegister2Register) , 
																Macros.TypefaceSize.headlineII , Macros.Color.lilyWhite
															)
		--用户登陆
		self.Button_register2_cut = Util:WidgetButton( 
																self:GetWidget("Button_register2_cut") , 
																CPPGameLib.GetString("login_userRegister") , 
																CPPGameLib.Handler(self,self.OnButtonRegister2Cut) , 
																Macros.TypefaceSize.headlineII , Macros.Color.lilyWhite
															)



	--手机登陆
	self.Panel_cellphone_register = self:GetWidget("Panel_cellphone_register")
		--手机号
		self.TextField_cellphone_import = self:GetWidget("TextField_cellphone_import")

		--验证码
		self.TextField_cellphone_verify = self:GetWidget("TextField_cellphone_verify")

		--登陆
		self.Button_cellphone_register = Util:WidgetButton( 
																self:GetWidget("Button_cellphone_register") , 
																CPPGameLib.GetString("login_register") , 
																CPPGameLib.Handler(self,self.OnButtonCellphoneRegister) , 
																Macros.TypefaceSize.headlineII , Macros.Color.lilyWhite
															)	
		--获得验证码
		self.Panel_cellphone_getVerify = self:GetWidget("Panel_cellphone_getVerify")
		self.Panel_cellphone_getVerify:SetTouchEnabled(true)
		WidgetUnity.SetWidgetListener(self.Panel_cellphone_getVerify, nil, nil, function ( ... )
			self:PanelCellphoneGetVerify()
		end)

		--返回
		self.Panel_cellphone_return = self:GetWidget("Panel_cellphone_return")
		self.Panel_cellphone_return:SetTouchEnabled(true)
		WidgetUnity.SetWidgetListener(self.Panel_cellphone_return, nil, nil, function ( ... )
			self:OnPanelCellphoneReturn()
		end)

	--玩家登陆
	self.Panel_user_register = self:GetWidget("Panel_user_register")
		--登陆
		self.Button_user_register_register = Util:WidgetButton( 
																self:GetWidget("Button_user_register_register") , 
																CPPGameLib.GetString("login_register") , 
																CPPGameLib.Handler(self,self.OnButtonUserRegisterRegister) , 
																Macros.TypefaceSize.headlineII , Macros.Color.lilyWhite
															)

		--新用户注册
		self.Button_user_register_enroll = Util:WidgetButton( 
																self:GetWidget("Button_user_register_enroll") , 
																CPPGameLib.GetString("login_enroll") , 
																CPPGameLib.Handler(self,self.OnButtonUserRegisterEnroll) , 
																Macros.TypefaceSize.headlineII , Macros.Color.lilyWhite
															)
		self.menu = false
		--下拉菜单 ——按钮
		self.Button_user_register_menu = Util:WidgetButton( 
																self:GetWidget("Button_user_register_menu") , 
																"↓" , 
																CPPGameLib.Handler(self,self.OnButtonUserRegisterMenu) , 
																Macros.TypefaceSize.headlineII , Macros.Color.lilyWhite
															)
		--下拉菜单 ——遮罩
		self.Panel_user_register_menu = self:GetWidget("Panel_user_register_menu")

		--返回
		self.Panel_egister_register_return = self:GetWidget("Panel_egister_register_return")
		self.Panel_egister_register_return:SetTouchEnabled(true)
		WidgetUnity.SetWidgetListener(self.Panel_egister_register_return, nil, nil, function ( ... )
			self:OnPanelEgisterRegisterReturn()
		end)

	--玩家注册
	self.Panel_user_enroll = self:GetWidget("Panel_user_enroll")
		--新用户注册
		self.Button_user_enroll_enroll = Util:WidgetButton( 
																self:GetWidget("Button_user_enroll_enroll") , 
																CPPGameLib.GetString("login_enroll_") , 
																CPPGameLib.Handler(self,self.OnButtonUserEnrollEnroll) , 
																Macros.TypefaceSize.headlineII , Macros.Color.lilyWhite
															)

		--返回
		self.Panel_user_enroll_return = self:GetWidget("Panel_user_enroll_return")
		self.Panel_user_enroll_return:SetTouchEnabled(true)
		WidgetUnity.SetWidgetListener(self.Panel_user_enroll_return, nil, nil, function ( ... )
			self:OnPanelUserEnrollReturn()
		end)

	--绑定手机
	self.Panel_binding = self:GetWidget("Panel_binding")	
		--绑定
		self.Button_binding_binding = Util:WidgetButton( 
																self:GetWidget("Button_binding_binding") , 
																CPPGameLib.GetString("login_binding") , 
																CPPGameLib.Handler(self,self.OnButtonBindingBinding) , 
																Macros.TypefaceSize.headlineII , Macros.Color.lilyWhite
															)	
		--跳过此步骤
		self.Button_binding_skip = Util:WidgetButton( 
																self:GetWidget("Button_binding_skip") , 
																CPPGameLib.GetString("login_skip") , 
																CPPGameLib.Handler(self,self.OnButtonBindingSkip) , 
																Macros.TypefaceSize.headlineII , Macros.Color.lilyWhite
															)	
		--安全提示
		self.Text_binding_account = self:GetWidget("Text_binding_account")

		--验证码
		self.Panel_binding_getVerify = self:GetWidget("Panel_binding_getVerify")
		self.Panel_binding_getVerify:SetTouchEnabled(true)
		WidgetUnity.SetWidgetListener(self.Panel_binding_getVerify, nil, nil, function ( ... )
			self:OnPanelBindingGetVerify()
		end)

		--返回
		self.Panel_binding_return = self:GetWidget("Panel_binding_return")
		self.Panel_binding_return:SetTouchEnabled(true)
		WidgetUnity.SetWidgetListener(self.Panel_binding_return, nil, nil, function ( ... )
			self:OnPanelBindingReturn()
		end)	

	--切换确认
	self.Panel_cut = self:GetWidget("Panel_cut")
		--提示容器
		self.Panel_cut_tips = self:GetWidget("Panel_cut_tips")

		--确定切换
		self.Button_cut_confirm = Util:WidgetButton( 
																self:GetWidget("Button_cut_confirm") , 
																CPPGameLib.GetString("login_cut_confirm") , 
																CPPGameLib.Handler(self,self.OnButtonCutConfirm) , 
																Macros.TypefaceSize.headlineII , Macros.Color.lilyWhite
				
											)	
		--继续游戏
		self.Button_cut_continue = Util:WidgetButton( 
																self:GetWidget("Button_cut_continue") , 
																CPPGameLib.GetString("login_cut_continue") , 
																CPPGameLib.Handler(self,self.OnButtonCutContinue) , 
																Macros.TypefaceSize.headlineII , Macros.Color.lilyWhite
															)	
	--切换提示
	self.Panel_cut_tips_2 = self:GetWidget("Panel_cut_tips_2")
		--继续游戏
		self.Button_cut_tips_confirm = Util:WidgetButton( 
																self:GetWidget("Button_cut_tips_confirm") , 
																CPPGameLib.GetString("login_cut_continue") , 
																CPPGameLib.Handler(self,self.OnButtonCutTipsConfirm) , 
																Macros.TypefaceSize.headlineII , Macros.Color.lilyWhite
															)
		
		--提示容器
		self.Panel_cut_tips_tips = self:GetWidget("Panel_cut_tips_tips")
end

--创建组件
function LogicGUI.TemplSelfSdkRegister:__ShowUI()
	-----------------------------------------------------------------------------------------------
	--登陆界面1
	-----------------------------------------------------------------------------------------------
	self.login_nowLogin = Util:Label( 
										CPPGameLib.GetString("login_nowLogin" , 0 , 0 ),
										Macros.TypefaceSize.headlineII , 
										Macros.Color.register
									)
	self.login_nowLogin:SetAnchorPoint( 0.5 , 0.5 )
	self.Text_register1_account:AddChild( self.login_nowLogin )
	PosTool.Center( self.login_nowLogin )

	-----------------------------------------------------------------------------------------------
	--登陆界面2
	--屏蔽客服QQ群提示
	-----------------------------------------------------------------------------------------------
	-- local login_cutTips3 = Util:RichText( 
	-- 										string.format( Macros.Color.register_rt , CPPGameLib.GetString("login_cutTips3") ) , 
	-- 										Macros.TypefaceSize.normal 
	-- 									)
	-- login_cutTips3:SetPosition( self.Panel_register2:GetContentWidth()/2 , 20 )
	-- self.Panel_register2:AddChild( login_cutTips3 )

	-----------------------------------------------------------------------------------------------
	--手机登陆
	-----------------------------------------------------------------------------------------------
	
	--手机号
	self.TextField_cellphone_import = Util:WidgetTextField(self:GetWidget("TextField_cellphone_import"))
	self.TextField_cellphone_import:SetPlaceHolderText( CPPGameLib.GetString("login_handset") )
	self.TextField_cellphone_import:SetPlaceHolderColor( unpack(Macros.Color.registerImport) )
	self.TextField_cellphone_import:SetTextColor( unpack(Macros.Color.registerImport) )
	self.TextField_cellphone_import:SetTextHorizontalAlign(UITextFieldEx.ALIGIN_LEFT)
	self.TextField_cellphone_import:SetTextVertiacalAlign(UITextFieldEx.ALIGIN_LEFT)
	self.TextField_cellphone_import:SetTouchAreaEnabled(true)
	self.TextField_cellphone_import:SetTouchSize( 465.00 , 45 )

	--验证码
	self.TextField_cellphone_verify = Util:WidgetTextField(self:GetWidget("TextField_cellphone_verify"))
	self.TextField_cellphone_verify:SetPlaceHolderText( CPPGameLib.GetString("login_seccode") )
	self.TextField_cellphone_verify:SetPlaceHolderColor( unpack(Macros.Color.registerImport) )
	self.TextField_cellphone_verify:SetTextColor( unpack(Macros.Color.registerImport) )
	self.TextField_cellphone_verify:SetTextHorizontalAlign(UITextFieldEx.ALIGIN_LEFT)
	self.TextField_cellphone_verify:SetTextVertiacalAlign(UITextFieldEx.ALIGIN_LEFT)
	self.TextField_cellphone_verify:SetTouchAreaEnabled(true)
	self.TextField_cellphone_verify:SetTouchSize( 273.00 , 45 )

	--获取验证码
	self.cellphone_login_getSeccode = Util:Label( 
											CPPGameLib.GetString("login_getSeccode"),
											Macros.TypefaceSize.headlineII , 
											Macros.Color.register  
										 )
	self.Panel_cellphone_getVerify:AddChild( self.cellphone_login_getSeccode )
	PosTool.Center( self.cellphone_login_getSeccode )

	--返回
	local login_return = Util:Label( 
												CPPGameLib.GetString("login_return") , 
												Macros.TypefaceSize.headlineII , 
												Macros.Color.register  
										)
	self.Panel_cellphone_return:AddChild( login_return )
	PosTool.Center( login_return )

	-----------------------------------------------------------------------------------------------
	--玩家登陆
	-----------------------------------------------------------------------------------------------
	--账号
	self.TextField_user_import = Util:WidgetTextField(self:GetWidget("TextField_user_import"))
	self.TextField_user_import:SetPlaceHolderText( CPPGameLib.GetString("login_accountTips") )
	self.TextField_user_import:SetPlaceHolderColor( unpack(Macros.Color.registerImport) )
	self.TextField_user_import:SetTextColor( unpack(Macros.Color.registerImport) )
	self.TextField_user_import:SetTextHorizontalAlign(UITextFieldEx.ALIGIN_LEFT)
	self.TextField_user_import:SetTextVertiacalAlign(UITextFieldEx.ALIGIN_LEFT)
	self.TextField_user_import:SetTouchAreaEnabled(true)
	self.TextField_user_import:SetTouchSize( 465.00 , 45 )

	--密码
	self.TextField_user_password = Util:WidgetTextField(self:GetWidget("TextField_user_password"))
	self.TextField_user_password:SetPlaceHolderText( CPPGameLib.GetString("login_passwordTips") )
	self.TextField_user_password:SetPlaceHolderColor( unpack(Macros.Color.registerImport) )
	self.TextField_user_password:SetTextColor( unpack(Macros.Color.registerImport) )
	self.TextField_user_password:SetTextHorizontalAlign(UITextFieldEx.ALIGIN_LEFT)
	self.TextField_user_password:SetTextVertiacalAlign(UITextFieldEx.ALIGIN_LEFT)
	self.TextField_user_password:SetTouchAreaEnabled(true)
	self.TextField_user_password:SetTouchSize( 465.00 , 45 )

	--返回
	local login_return = Util:Label( 
												CPPGameLib.GetString("login_return") , 
												Macros.TypefaceSize.headlineII , 
												Macros.Color.register  
										)
	self.Panel_egister_register_return:AddChild( login_return )
	PosTool.Center( login_return )

	-----------------------------------------------------------------------------------------------
	--玩家注册
	-----------------------------------------------------------------------------------------------
	--账号
	self.TextField_user_enroll_import = Util:WidgetTextField(self:GetWidget("TextField_user_enroll_import"))
	self.TextField_user_enroll_import:SetPlaceHolderText( CPPGameLib.GetString("login_accountTips") )
	self.TextField_user_enroll_import:SetPlaceHolderColor( unpack(Macros.Color.registerImport) )
	self.TextField_user_enroll_import:SetTextColor( unpack(Macros.Color.registerImport) )
	self.TextField_user_enroll_import:SetTextHorizontalAlign(UITextFieldEx.ALIGIN_LEFT)
	self.TextField_user_enroll_import:SetTextVertiacalAlign(UITextFieldEx.ALIGIN_LEFT)
	self.TextField_user_enroll_import:SetMaxLengthEnabled(true)
	self.TextField_user_enroll_import:SetMaxLength(16)
	self.TextField_user_enroll_import:SetTouchAreaEnabled(true)
	self.TextField_user_enroll_import:SetTouchSize( 465.00 , 45 )

	--密码
	self.TextField_user_enrollpassword = Util:WidgetTextField(self:GetWidget("TextField_user_enrollpassword"))
	self.TextField_user_enrollpassword:SetPlaceHolderText( CPPGameLib.GetString("login_passwordTips") )
	self.TextField_user_enrollpassword:SetPlaceHolderColor( unpack(Macros.Color.registerImport) )
	self.TextField_user_enrollpassword:SetTextColor( unpack(Macros.Color.registerImport) )
	self.TextField_user_enrollpassword:SetTextHorizontalAlign(UITextFieldEx.ALIGIN_LEFT)
	self.TextField_user_enrollpassword:SetTextVertiacalAlign(UITextFieldEx.ALIGIN_LEFT)
	self.TextField_user_enrollpassword:SetTouchAreaEnabled(true)
	self.TextField_user_enrollpassword:SetTouchSize( 465.00 , 45 )

	--返回
	local login_return = Util:Label( 
												CPPGameLib.GetString("login_return") , 
												Macros.TypefaceSize.headlineII , 
												Macros.Color.register  
										)
	self.Panel_user_enroll_return:AddChild( login_return )
	PosTool.Center( login_return )

	-----------------------------------------------------------------------------------------------
	--绑定手机
	-----------------------------------------------------------------------------------------------
	--手机号
	self.TextField_binding_import = Util:WidgetTextField(self:GetWidget("TextField_binding_import"))
	self.TextField_binding_import:SetPlaceHolderText( CPPGameLib.GetString("login_handset") )
	self.TextField_binding_import:SetPlaceHolderColor( unpack(Macros.Color.registerImport) )
	self.TextField_binding_import:SetTextColor( unpack(Macros.Color.registerImport) )
	self.TextField_binding_import:SetTextHorizontalAlign(UITextFieldEx.ALIGIN_LEFT)
	self.TextField_binding_import:SetTextVertiacalAlign(UITextFieldEx.ALIGIN_LEFT)
	self.TextField_binding_import:SetTouchAreaEnabled(true)
	self.TextField_binding_import:SetTouchSize( 465.00 , 45 )

	--验证码
	self.TextField_binding_password = Util:WidgetTextField(self:GetWidget("TextField_binding_password"))
	self.TextField_binding_password:SetPlaceHolderText( CPPGameLib.GetString("login_seccode") )
	self.TextField_binding_password:SetPlaceHolderColor( unpack(Macros.Color.registerImport) )
	self.TextField_binding_password:SetTextColor( unpack(Macros.Color.registerImport) )
	self.TextField_binding_password:SetTextHorizontalAlign(UITextFieldEx.ALIGIN_LEFT)
	self.TextField_binding_password:SetTextVertiacalAlign(UITextFieldEx.ALIGIN_LEFT)
	self.TextField_binding_password:SetTouchAreaEnabled(true)
	self.TextField_binding_password:SetTouchSize( 273.00 , 45 )

	--安全提示
	local login_handsetSeccodeTips = Util:Label( 
												CPPGameLib.GetString("login_handsetSeccodeTips") , 
												Macros.TypefaceSize.tab , 
												Macros.Color.register  
										)
	self.Text_binding_account:AddChild( login_handsetSeccodeTips )
	PosTool.Center( login_handsetSeccodeTips )

	--获取验证码
	self.binding_login_getSeccode = Util:Label( 
											CPPGameLib.GetString("login_getSeccode"),
											Macros.TypefaceSize.headlineII , 
											Macros.Color.register  
										 )
	self.Panel_binding_getVerify:AddChild( self.binding_login_getSeccode )
	PosTool.Center( self.binding_login_getSeccode )

	--返回
	local login_return = Util:Label( 
												CPPGameLib.GetString("login_return") , 
												Macros.TypefaceSize.headlineII , 
												Macros.Color.register  
										)
	self.Panel_binding_return:AddChild( login_return )
	PosTool.Center( login_return )

	-----------------------------------------------------------------------------------------------
	--切换确认
	-----------------------------------------------------------------------------------------------
	--切换提示
	local login_cutTips = Util:RichText( 
											string.format( Macros.Color.register_rt , CPPGameLib.GetString("login_cutTips") ) , 
											Macros.TypefaceSize.headlineII , 
											self.Panel_cut_tips:GetContentWidth() - 15 
										)
	login_cutTips:SetAnchorPoint( 0 , 1 )
	login_cutTips:SetPosition( 5 , self.Panel_cut_tips:GetContentHeight() - 20 )
	self.Panel_cut_tips:AddChild( login_cutTips )

	-----------------------------------------------------------------------------------------------
	--切换提示
	-----------------------------------------------------------------------------------------------
	local login_cutTips1 = Util:RichText( 
											string.format( Macros.Color.register_rt , CPPGameLib.GetString("login_cutTips1") ) , 
											Macros.TypefaceSize.headlineII , 
											self.Panel_cut_tips_tips:GetContentWidth() - 15 
										)
	login_cutTips1:SetAnchorPoint( 0 , 1 )
	login_cutTips1:SetPosition( 5 , self.Panel_cut_tips_tips:GetContentHeight() - 20 )
	self.Panel_cut_tips_tips:AddChild( login_cutTips1 )


	--隐藏
	self:__Concealwidget()
end

--隐藏组件
function LogicGUI.TemplSelfSdkRegister:__Concealwidget()
	self.image_little_logo:SetVisible(false)
	self.image_little_logoline:SetVisible(false)
	self.image_big_logo:SetVisible(false)

	--登陆界面1
	self.Panel_register1:SetVisible(false)
	--登陆界面2
	self.Panel_register2:SetVisible(false)
	--手机登陆
	self.Panel_cellphone_register:SetVisible(false)
	self.TextField_cellphone_import:SetText("")
	self.TextField_cellphone_verify:SetText("")
	--玩家登陆
	self.Panel_user_register:SetVisible(false)
	self.TextField_user_import:SetText(StartConfig.SDKAccount or "")
	self.TextField_user_password:SetText("")
	--玩家注册
	self.Panel_user_enroll:SetVisible(false)
	self.TextField_user_enroll_import:SetText("")
	self.TextField_user_enrollpassword:SetText("")
	--绑定手机
	self.Panel_binding:SetVisible(false)
	self.TextField_binding_import:SetText("")
	self.TextField_binding_password:SetText("")
	--切换确定
	self.Panel_cut:SetVisible(false)
	--切换确定
	self.Panel_cut_tips_2:SetVisible(false)
end

--判断打开什么鬼
function LogicGUI.TemplSelfSdkRegister:JudgeOpen()
	--是否有上一次的登陆记录
	local id , last_login_password , type_  = CallModuleFunc(ModuleType.LOGIN,"GetLastAccountInfo")

	--如果有上次的记录
	if type_ then
		--打开登陆1
		self.registerTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime") + 3
		self:OpenRegister1()

	--如果没有上次的记录
	else
		self:OpenRegister2()
	end
end


-----------------------------------------------------------------------------------------------
--登陆界面1
-----------------------------------------------------------------------------------------------
function LogicGUI.TemplSelfSdkRegister:OpenRegister1()
	self:ShowLogo(false, true)
	--显示第一个
	self.Panel_register1:SetVisible(true)
	--弹一下
	ActionManager.GetInstance():ViewOrTemplateOpenEffect(self, function ( ... )
	end)
	self:CountdownRegister()
end

--登陆游戏
function LogicGUI.TemplSelfSdkRegister:OnButtonRegister1Register()
	self:StopCountdownRegister()
	local id , last_login_password , type_  = CallModuleFunc(ModuleType.LOGIN,"GetLastAccountInfo")
	CallModuleFunc(ModuleType.LOGIN,"LoginAccountReq", id , last_login_password , type_ )
end

--切换账号
function LogicGUI.TemplSelfSdkRegister:OnButtonRegister1Cut()
	self:StopCountdownRegister()

	--隐藏
	self:__Concealwidget()

	--打开登陆2界面
	self:OpenRegister2()
end


-----------------------------------------------------------------------------------------------
--登陆界面2
-----------------------------------------------------------------------------------------------
function LogicGUI.TemplSelfSdkRegister:OpenRegister2()
	self:ShowLogo(false, true)

	--显示第一个
	self.Panel_register2:SetVisible(true)
	--弹一下
	ActionManager.GetInstance():ViewOrTemplateOpenEffect(self, function ( ... )
	end)
end

--一键登陆
function LogicGUI.TemplSelfSdkRegister:OnButtonRegister2OneKey()
	--密码
	local password = CallModuleFunc(ModuleType.LOGIN,"GetRandomPassword")
	CallModuleFunc(ModuleType.LOGIN,"LoginAccountReq","account" , password , Macros.Game.LoginType.ONEKEY_LOGIN )
end

--手机登陆
function LogicGUI.TemplSelfSdkRegister:OnButtonRegister2Register()
	--隐藏
	self:__Concealwidget()
	self:OpenCellphoneRegister()
end

--用户登陆
function LogicGUI.TemplSelfSdkRegister:OnButtonRegister2Cut()
	--隐藏
	self:__Concealwidget()
	self:OpenUserRegister()
end


-----------------------------------------------------------------------------------------------
--手机登陆
-----------------------------------------------------------------------------------------------
function LogicGUI.TemplSelfSdkRegister:OpenCellphoneRegister()
	self:ShowLogo(true, false)

	self.Panel_cellphone_register:SetVisible(true)
	--弹一下
	ActionManager.GetInstance():ViewOrTemplateOpenEffect(self, function ( ... )
	end)
end

--登陆
function LogicGUI.TemplSelfSdkRegister:OnButtonCellphoneRegister()
	--手机号
	local account = self.TextField_cellphone_import:GetText()
	if account == "" then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_please_input_account"))	
		return
	end

	--验证码
	local verify = self.TextField_cellphone_verify:GetText()
	if verify == "" then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_please_input_verify"))	
		return
	end
	local num , letter , slip = StringTool.Number( account )
	local section = string.sub( account , 0 , 3 )
	if num ~= 11 or letter ~= 0 or slip ~= 0 or not CPPGameLib.GetConfig("Cellphone" , tonumber(section) ) then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_handsetSlip1"))	
		return 
	end
	--密码
	local password = CallModuleFunc(ModuleType.LOGIN,"GetRandomPassword")
	CallModuleFunc(ModuleType.LOGIN,"LoginAccountReq", account , password , Macros.Game.LoginType.HANDSET_REGIST , verify )
end

--发验证码
function LogicGUI.TemplSelfSdkRegister:PanelCellphoneGetVerify()
	--手机号
	local account = self.TextField_cellphone_import:GetText()
	if account == "" then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_please_input_account"))	
		return
	end
	self.time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime") + 60

	self:Countdown()
	CallModuleFunc(ModuleType.LOGIN,"AuthenticationCodeReq",account)
end

--返回
function LogicGUI.TemplSelfSdkRegister:OnPanelCellphoneReturn()
	--隐藏
	self:__Concealwidget()
	--回到第二个登陆口
	self:OpenRegister2()
end

-----------------------------------------------------------------------------------------------
--玩家登陆
-----------------------------------------------------------------------------------------------
function LogicGUI.TemplSelfSdkRegister:OpenUserRegister()
	self:ShowLogo(true, false)
	self.Panel_user_register:SetVisible(true)
	--弹一下
	ActionManager.GetInstance():ViewOrTemplateOpenEffect(self, function ( ... )
	end)
end

--登陆
function LogicGUI.TemplSelfSdkRegister:OnButtonUserRegisterRegister()
	local account = self.TextField_user_import:GetText()
	if account == "" then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_please_input_account"))	
		return
	end

	local password = self.TextField_user_password:GetText()
	if password == "" then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_please_input_password"))	
		return
	end

	CallModuleFunc(ModuleType.LOGIN,"LoginAccountReq",account , password , Macros.Game.LoginType.USER_LOGIN )
end

--新用户注册
function LogicGUI.TemplSelfSdkRegister:OnButtonUserRegisterEnroll()
	--隐藏
	self:__Concealwidget()
	-- 玩家注册
	self:OpenUserEnroll()
end

--下拉按钮
function LogicGUI.TemplSelfSdkRegister:OnButtonUserRegisterMenu()

	local lst = CallModuleFunc(ModuleType.LOGIN,"GetRecentAccountList")

	if not self.userRegisterMenuLayout and #lst ~= 0 then
		--背板
		self.userRegisterMenuLayout = Util:Layout( self.Panel_user_register_menu:GetContentWidth() , self.Panel_user_register_menu:GetContentHeight() )
		self.userRegisterMenuLayout:SetContentSize( self.userRegisterMenuLayout:GetContentWidth() , (#lst + 1) * 50 )
		self.userRegisterMenuLayout:SetPosition( self.Panel_user_register_menu:GetContentWidth()/2 , self.Panel_user_register_menu:GetContentHeight() + self.userRegisterMenuLayout:GetContentHeight() )
		self.userRegisterMenuLayout:SetAnchorPoint( 0.5 , 1 )
		self.userRegisterMenuLayout:SetBackGroundColor(0, 0, 0)
	    self.userRegisterMenuLayout:SetBackGroundColorType(1)
	    self.userRegisterMenuLayout:SetBackGroundColorOpacity(120)
	    self.userRegisterMenuLayout:SetTouchEnabled(true)
	    self.Panel_user_register_menu:AddChild( self.userRegisterMenuLayout )

	    --ui
	    self.userRegisterMenuLayoutUI = Util:Layout( self.Panel_user_register_menu:GetContentWidth() , 50 )
	    self.userRegisterMenuLayoutUI:SetAnchorPoint( 0.5 , 0 )
	    self.userRegisterMenuLayout:AddChild( self.userRegisterMenuLayoutUI )
	    PosTool.CenterBottom( self.userRegisterMenuLayoutUI )

	    --清理全部
	    local left = Util:Layout( self.userRegisterMenuLayoutUI:GetContentWidth()/2 , 50 ,function ( ... )
	    	for i , v in ipairs( CallModuleFunc(ModuleType.LOGIN,"GetRecentAccountList") ) do
	    		CallModuleFunc(ModuleType.LOGIN,"DelRecentAccount",v.account)
	    	end
	    end )
	    left:SetAnchorPoint( 0 , 0.5 )
	    left:SetPosition( 0 , self.userRegisterMenuLayoutUI:GetContentHeight()/2 )
	    self.userRegisterMenuLayoutUI:AddChild( left )

	    local allRemove = Util:Label( "清理全部" )
	    left:AddChild(allRemove)
		PosTool.Center( allRemove )

		--竖线
	    local shu = Util:Label("|")
	    self.userRegisterMenuLayoutUI:AddChild( shu )
	    PosTool.Center( shu )

	    --取消
	    local right = Util:Layout( self.userRegisterMenuLayoutUI:GetContentWidth()/2 , 50 , function ( ... )
	    	self:UserRegisterMeun()
	    end)
	    right:SetAnchorPoint( 1 , 0.5 )
	    right:SetPosition( self.userRegisterMenuLayoutUI:GetContentWidth() , self.userRegisterMenuLayoutUI:GetContentHeight()/2 )
	    self.userRegisterMenuLayoutUI:AddChild( right )

	    local cancel = Util:Label( "取消" )
	    right:AddChild(cancel)
		PosTool.Center( cancel )

	    --条目
		local params =
	    {
	        item_class = LogicGUI.UserRegisterMenuItem,
	        item_width = self.userRegisterMenuLayout:GetContentWidth() ,
	        item_height = 50 ,
	        row = 4,
	        col = 1,
		    touchable = true,
		    touch_callback	= function (item_index,item_data)
		    	--选中
		    	self.TextField_user_import:SetText( item_data.account )
				self.TextField_user_password:SetText( item_data.password )
				self:UserRegisterMeun()
		    end,
	        view_height = self.userRegisterMenuLayout:GetContentHeight() - 50 ,
	    }
	    -- 创建参数
	    self.userRegisterMenuList = GUI.ScrollView.New(params)
		--设置lst
	    self.userRegisterMenuList:SetPosition( 
	    										self.userRegisterMenuLayout:GetContentWidth()/2 , 
	    										self.userRegisterMenuLayout:GetContentHeight() - (self.userRegisterMenuList:GetViewHeight() /2)
	    										)
	    self.userRegisterMenuLayout:AddChild(self.userRegisterMenuList:GetNode())
		self.userRegisterMenuList:SetDataList( lst )
	end

	self:UserRegisterMeun()
end

--返回
function LogicGUI.TemplSelfSdkRegister:OnPanelEgisterRegisterReturn()
	--隐藏
	self:__Concealwidget()
	--回到第二个登陆口
	self:OpenRegister2()
end

--创建菜单
function LogicGUI.TemplSelfSdkRegister:UserRegisterMeun()
	local lst = CallModuleFunc(ModuleType.LOGIN,"GetRecentAccountList")
	if #lst <= 0 then
		return 
	end

	if self.menu == false then
		CPPActionManager.MoveTo( self.userRegisterMenuLayout , 0.2 ,self.Panel_user_register_menu:GetContentWidth()/2 , self.Panel_user_register_menu:GetContentHeight() )
		self.menu = true
	else
		CPPActionManager.MoveTo( self.userRegisterMenuLayout , 0.2 ,self.Panel_user_register_menu:GetContentWidth()/2 , self.Panel_user_register_menu:GetContentHeight() + self.userRegisterMenuLayout:GetContentHeight() )
		self.menu = false
	end
end

--创建菜单
function LogicGUI.TemplSelfSdkRegister:SetUserRegisterMeunLst()
	local lst = CallModuleFunc(ModuleType.LOGIN,"GetRecentAccountList")
	
	self.userRegisterMenuLayout:SetContentSize( self.userRegisterMenuLayout:GetContentWidth() , (#lst + 1) * 50 )
	--设置lst
    self.userRegisterMenuList:SetPosition( 
    										self.userRegisterMenuLayout:GetContentWidth()/2 , 
    										self.userRegisterMenuLayout:GetContentHeight() - (self.userRegisterMenuList:GetViewHeight() /2)
    										)

	self.userRegisterMenuList:SetDataList( lst )

	if #lst <= 0 then
		CPPActionManager.MoveTo( self.userRegisterMenuLayout , 0.2 ,self.Panel_user_register_menu:GetContentWidth()/2 , self.Panel_user_register_menu:GetContentHeight() + self.userRegisterMenuLayout:GetContentHeight() )
		self.menu = false
	end 
end


-----------------------------------------------------------------------------------------------
--玩家注册
-----------------------------------------------------------------------------------------------
function LogicGUI.TemplSelfSdkRegister:OpenUserEnroll()
	self:ShowLogo(true, false)
	self.Panel_user_enroll:SetVisible(true)
	--弹一下
	ActionManager.GetInstance():ViewOrTemplateOpenEffect(self, function ( ... )
	end)
end

--返回
function LogicGUI.TemplSelfSdkRegister:OnPanelUserEnrollReturn()
	--隐藏
	self:__Concealwidget()
	--回到第二个登陆口
	self:OpenRegister2()
end

--注册
function LogicGUI.TemplSelfSdkRegister:OnButtonUserEnrollEnroll()
	--账号
	local account = self.TextField_user_enroll_import:GetText()	
	if account == "" then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_please_input_account"))	
		return
	end

	local num , letter , slip = StringTool.Number( account )
	if slip ~= 0 then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_role_name_illegality"))	
		return
	elseif letter == 0 then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_role_name_illegality_parameter"))	
		return
	end

	--密码
	local password = self.TextField_user_enrollpassword:GetText()	
	local num , letter , slip = StringTool.Number( password )
	if password == "" then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_please_input_password"))	
		return
	elseif slip ~= 0 then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_role_password_illegality"))	
		return
	end

	CallModuleFunc(ModuleType.LOGIN,"LoginAccountReq", account , password , Macros.Game.LoginType.USER_REGIST)
end


-----------------------------------------------------------------------------------------------
--绑定手机
-----------------------------------------------------------------------------------------------
function LogicGUI.TemplSelfSdkRegister:OpenBinding()
	--隐藏
	self:__Concealwidget()

	self:ShowLogo(true, false)
	self.Panel_binding:SetVisible(true)
	--弹一下
	ActionManager.GetInstance():ViewOrTemplateOpenEffect(self, function ( ... )
	end)
end

--绑定按钮
function LogicGUI.TemplSelfSdkRegister:OnButtonBindingBinding()
	local account = self.TextField_binding_import:GetText()	
	if account == "" then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_please_input_account"))	
		return
	end
	--密码
	local password = self.TextField_binding_password:GetText()	
	if password == "" then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_please_input_verify"))	
		return
	end

	local num , letter , slip = StringTool.Number( account )
	local section = string.sub( account , 0 , 3 )
	if num ~= 11 or letter ~= 0 or slip ~= 0 or not CPPGameLib.GetConfig("Cellphone" , tonumber(section) ) then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_handsetSlip1"))	
		return 
	end

	local info = {}
	info.account = CallModuleFunc(ModuleType.LOGIN,"GetAccount")
	info.code = password
	info.mobile = account
	CallModuleFunc(ModuleType.LOGIN,"MobileBindReq",info)
	self:OnClose()
end

--跳过此步骤
function LogicGUI.TemplSelfSdkRegister:OnButtonBindingSkip()
	self:OnClose()
end

--获得验证码
function LogicGUI.TemplSelfSdkRegister:OnPanelBindingGetVerify()
	--手机号
	local account = self.TextField_binding_import:GetText()
	if account == "" then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_please_input_account"))	
		return
	end
	self.time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime") + 60
	self:Countdown()
	CallModuleFunc(ModuleType.LOGIN,"AuthenticationCodeReq",account)
end

--返回
function LogicGUI.TemplSelfSdkRegister:OnPanelBindingReturn()
	self:OnClose()
end

function LogicGUI.TemplSelfSdkRegister:OnClose()
	self:DeleteMe()
	if self.func then
		self.func()
	end
end

--倒计时
function LogicGUI.TemplSelfSdkRegister:Countdown()
 	local func = nil
    local function moveAnimate( ... )
    	local endtime = self.time - CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
        if endtime <= 0 then
       		WidgetUnity.SetWidgetListener(self.Panel_cellphone_getVerify, nil, nil, function ( ... )
				self:PanelCellphoneGetVerify()
			end)
			WidgetUnity.SetWidgetListener(self.Panel_binding_getVerify, nil, nil, function ( ... )
				self:OnPanelBindingGetVerify()
			end)

			self.binding_login_getSeccode:SetString( CPPGameLib.GetString("login_getSeccode") )
			self.binding_login_getSeccode:SetColor(unpack(Macros.Color.register))

			self.cellphone_login_getSeccode:SetString( CPPGameLib.GetString("login_getSeccode") )
			self.cellphone_login_getSeccode:SetColor(unpack(Macros.Color.register))		

            CPPActionManager.StopAllActions(self.Image_white_bg)
        else
			self.binding_login_getSeccode:SetString( endtime.."S" )
			self.binding_login_getSeccode:SetColor(unpack(Macros.Color.GrayBlack))
			WidgetUnity.SetWidgetListener(self.Panel_binding_getVerify, nil, nil, function ( ... )
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_seccodeSell"))	
			end)
			self.cellphone_login_getSeccode:SetString( endtime.."S" )
			self.cellphone_login_getSeccode:SetColor(unpack(Macros.Color.GrayBlack))
			WidgetUnity.SetWidgetListener(self.Panel_cellphone_getVerify, nil, nil, function ( ... )
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_seccodeSell"))	
			end)
            CPPActionManager.DelayTo( self.Image_white_bg , 1 , func )
        end
    end
    func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
    moveAnimate()
end

--倒计时登陆
function LogicGUI.TemplSelfSdkRegister:CountdownRegister()
	if self.callback then
		CPPActionManager.StopAllActions(self.Button_register1_register)
	end
 	self.callback = nil
    local function moveAnimate( ... )
    	local endtime = self.registerTime - CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
        if endtime <= 0 then
            CPPActionManager.StopAllActions(self.Button_register1_register)
            -- self.sp_bg:SetVisible(false)
            self.Button_register1_register:SetText( CPPGameLib.GetString("login_registerGame") )
            self:OnButtonRegister1Register()
        else
    		local id , last_login_password , type_  = CallModuleFunc(ModuleType.LOGIN,"GetLastAccountInfo")
			self.login_nowLogin:SetString( CPPGameLib.GetString( "login_nowLogin" , id , endtime ) )
            self.Button_register1_register:SetText( CPPGameLib.GetString("login_registerGame") .. "(" .. endtime .."s" .. ")" )
            CPPActionManager.DelayTo( self.Button_register1_register , 1 , self.callback )
        end
    end
    self.callback = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
    moveAnimate()
end

--停止倒计时登陆
function LogicGUI.TemplSelfSdkRegister:StopCountdownRegister()
	self.callback = nil
	-- self.sp_bg:SetVisible(false)
	CPPActionManager.StopAllActions(self.Button_register1_register)
end

-----------------------------------------------------------------------------------------------
--切换确认
-----------------------------------------------------------------------------------------------
function LogicGUI.TemplSelfSdkRegister:OpenPanelCut()
	self:ShowLogo(true, false)
	self.Panel_cut:SetVisible(true)
	--弹一下
	ActionManager.GetInstance():ViewOrTemplateOpenEffect(self, function ( ... )
	end)
end

--确定切换
function LogicGUI.TemplSelfSdkRegister:OnButtonCutConfirm()
	--隐藏
	self:__Concealwidget()

	--打开登陆2界面
	self:OpenRegister2()
end

--继续游戏
function LogicGUI.TemplSelfSdkRegister:OnButtonCutContinue()
	--隐藏
	self:__Concealwidget()
	--打开登陆1
	self.registerTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime") + 3
	--打开登陆1界面
	self:OpenRegister1()
end

-----------------------------------------------------------------------------------------------
--切换提示
-----------------------------------------------------------------------------------------------
function LogicGUI.TemplSelfSdkRegister:OpenPanelCutTips2()
	--隐藏
	self:__Concealwidget()
	
	self:ShowLogo(true, false)
	self.Panel_cut_tips_2:SetVisible(true)
	--弹一下
	ActionManager.GetInstance():ViewOrTemplateOpenEffect(self, function ( ... )
	end)
end

--确定
function LogicGUI.TemplSelfSdkRegister:OnButtonCutTipsConfirm()
	self:OnClose()
end

--设置游戏logo显隐
function LogicGUI.TemplSelfSdkRegister:ShowLogo(little_flag, big_flag)
	self.image_little_logo:SetVisible(little_flag)
	self.image_little_logoline:SetVisible(little_flag)
	self.image_big_logo:SetVisible(big_flag)
end

-----------------------------------------------------------------------------------------------------------------------------------
--下拉菜单条目
-----------------------------------------------------------------------------------------------------------------------------------

LogicGUI = LogicGUI or {}
LogicGUI.UserRegisterMenuItem = LogicGUI.UserRegisterMenuItem or BaseClass(GUI.ScrollItem)

function LogicGUI.UserRegisterMenuItem:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function LogicGUI.UserRegisterMenuItem:__CreateWidgetFromLuaCode()
	self.__layout = Util:Layout( 405 , 50 )
    self.__layout:SetBackGroundColor(0, 0, 0)
    self.__layout:SetBackGroundColorType(1)
    self.__layout:SetBackGroundColorOpacity(120)

    local _ = Util:Label("——————————————————————————————————————————————————")
    _:SetPosition( self.__layout:GetContentWidth()/2 , 0 )
    self.__layout:AddChild( _ )

	return self.__layout
end

function LogicGUI.UserRegisterMenuItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	--账号
    local account = Util:Label( data.account )
    account:SetAnchorPoint( 0 , 0.5 )
    account:SetPosition( 10 , self.__layout:GetContentHeight()/2 )
    self.__layout:AddChild( account )

    --删除
    local remove = Util:Layout( 50 , 50 , function ( ... )
    	CallModuleFunc(ModuleType.LOGIN,"DelRecentAccount",data.account)
    end)
    remove:SetAnchorPoint( 1 , 0.5 )
    remove:SetPosition( self.__layout:GetContentWidth() , self.__layout:GetContentHeight()/2 )
    self.__layout:AddChild( remove )

    --|
    local l = Util:Label( "丨" )
    l:SetAnchorPoint( 0 , 0.5 )
    l:SetPosition( 0 , remove:GetContentHeight()/2 )
    remove:AddChild( l )
    --X
    local X = Util:Label( "X" )
    remove:AddChild( X )
    PosTool.Center(X)

    --|
    local l = Util:Label( "丨" )
    l:SetAnchorPoint( 1 , 0.5 )
    l:SetPosition( remove:GetContentWidth() , remove:GetContentHeight()/2 )
    remove:AddChild( l )
end

function LogicGUI.UserRegisterMenuItem:GetTouchNode()
	return self.__layout
end

