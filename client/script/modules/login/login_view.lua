
--[[
%% @module: 登陆界面
%% @author: swordman sue
%% @created: 2016-07-15
--]]

Modules = Modules or {}

Modules.LoginView = Modules.LoginView or BaseClass(BaseView)

local CtrlZorder = 
{
	PANEL_NOW = 20,
	PANEL_TIP = 21,
	EFFECT_BEG = 4,
	LOGO = 3,
}

function Modules.LoginView:__init()
	self.__layout_name = "login"	
	self.__close_mode = BaseView.CloseMode.VISIBLE

	local bg = Resource.PathTool.GetUIResPath("bigbg/bg_login", nil, "sdz")
	--是否显示渠道登录页
	if CPPSdkTools.IsSDKLoginBG() then
		bg = Resource.PathTool.GetTrdResPath("bg_login.jpg")
	end

	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = bg , image_size = COCOSize(1136, 640)}

	self.is_auto_close = false

	self.__loaded_callback = function()
		self:__LoadedCallback()
	end

    self.__open_callback = function()
    	self:__OpenCallback()
    end

    self.__close_callback = function()
    	self:__CloseCallback()
    end

   self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.LOGIN)
   self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.LOGIN, true)
end

function Modules.LoginView:__Dispose()
	if self.__panel_hero_model then
		self.__panel_hero_model:DeleteMe()
		self.__panel_hero_model = nil
	end

	if self.self_sdk_view then
		self.self_sdk_view:DeleteMe()
		self.self_sdk_view = nil
	end
end

function Modules.LoginView:LoginAccountResp()
	if self.__sub_type == BaseViewSubType.Login.LOGIN then
		self:__ChangeSubView(BaseViewSubType.Login.LOGIN_II)
	end
end

function Modules.LoginView:__LoadedCallback()
	self.__panel_login = self:GetWidget("Panel_Login")
	self.__panel_login2 = self:GetWidget("Panel_Login2")

	--游戏版号信息
	if StartConfig.IsShowGameVersionInfo ~= false then
		local version_info = ConfigAdapter.Common.GetGameVersionInfo()
		if version_info then
			local lab = Util:Label(version_info, 12, nil, nil, nil, Macros.Color.white_stroke)
			lab:SetAnchorPoint(0.5, 1)
			lab:SetTextHorizontalAlign(LabelEx.ALIGIN_CENTER)
			self:AddChild(lab, 9999)
			PosTool.CenterTop(lab, 0, -20)
		end
	end
end

function Modules.LoginView:__OpenCallback()	
	--客户端事件跟踪(统计流失率)
	StatisticsTool.GetInstance():ReportEventTrackForLossRate(StatisticsTool.EventType4LossRate.OPEN_LOGIN_VIEW)									
	
	self.__panel_login:SetVisible(false)
	self.__panel_login2:SetVisible(false)

	local sub_type, callback = unpack(self.__open_data)
	self:__ChangeSubView(sub_type)
	if callback then
		callback()
	end
end

function Modules.LoginView:__CloseCallback()
end

function Modules.LoginView:__ChangeSubView(type)
	self.__sub_type = type

	if self.__panel_now then
		self.__panel_now:SetVisible(false)
		self.__panel_now = nil
	end

	local img_game_title = self:GetWidget("Image_GameTitle")
	img_game_title:SetZOrder(CtrlZorder.LOGO)
	img_game_title:SetScaleFactor(0.95)
	img_game_title:IgnoreContentAdaptWithSize(true)

	local logo = Resource.PathTool.GetResPath("start/logo.png")
	if CPPSdkTools.IsSDKLogo() then
		logo = Resource.PathTool.GetTrdResPath("logo.png")
	end

	img_game_title:SetImage( logo , TextureResType.LOCAL)
	img_game_title:SetAnchorPoint(0.5, 1)
	PosTool.CenterTop(img_game_title, 7 , -22)

	CPPGameLib.Switch(self.__sub_type)
	{
		[BaseViewSubType.Login.LOGIN] = function()
			self.__panel_now = self.__panel_login
			self:__HandleLogin()
			self:__ShowModelsAndEffects()
		end,
		[BaseViewSubType.Login.LOGIN_II] = function()
			self.__panel_now = self.__panel_login2
			self:__HandleLoginII()
			self:__ShowModelsAndEffects()
		end,
	}

	if self.__panel_now then
		self.__panel_now:SetVisible(true)
		self.__panel_now:SetZOrder(CtrlZorder.PANEL_NOW)
	end
end

function Modules.LoginView:__HandleLogin()
	LogTool.LogInfo("[Modules.LoginView:__HandleLogin] %d", CPPSdkTools.IsSelfLoginSystem() and 1 or 0)

	--SDK登录入口
	local is_facebook_toggle = CPPSdkTools.IsFacebookLoginToggle()
	local btn_login_str = is_facebook_toggle and "" or CPPGameLib.GetString("server_enter")
	local btn_login = Util:WidgetButton(
										self:GetWidget("Button_LoginSDK") , 
										btn_login_str , 
										function()
											if CPPSdkTools.IsFacebookLoginToggle() then
												--facebook登录
												local param = {}
												param.type = "facebook"												
												self:CallModuleFunc("LoginSDK", param)
											else
												self:CallModuleFunc("LoginSDK")
											end
									    end , 
										Macros.TypefaceSize.headline , 
										Macros.Color.btn , 
										Macros.Color.button_red
									)
	btn_login:SetVisible(not CPPSdkTools.IsSelfLoginSystem())
	if is_facebook_toggle then
		btn_login:SetNormalImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LOGIN, "facebook", true) )
		btn_login:SetScale9Enabled(false)
		btn_login:IgnoreContentAdaptWithSize(true)
		PosTool.Center( btn_login , 0 , -180 )
	end

	--游客登录入口
	local btn_login_visitor = Util:WidgetButton(
										self:GetWidget("Button_LoginSDK_Visitor") , 
										CPPGameLib.GetString("login_visitor_str") , 
										function()
											--游客登录
											local param = {}
											param.type = "visitor"												
											self:CallModuleFunc("LoginSDK", param)
									    end , 
										Macros.TypefaceSize.headline , 
										Macros.Color.btn , 
										Macros.Color.button_red
									)
	btn_login_visitor:SetVisible(CPPSdkTools.IsVisitorLoginToggle())	
	btn_login_visitor:SetContentWidth(210)

	if is_facebook_toggle then
		btn_login_visitor:SetScaleFactor(0.8)
		btn_login_visitor:SetPosition( self.__panel_login:GetContentWidth() - 10 - (btn_login_visitor:GetContentWidth()/2) , 40.58 )
	else
		PosTool.CenterBottom(btn_login_visitor, -140, 60)
	end

	if CPPSdkTools.IsSelfLoginSystem() then
		--打开自有登陆界面
		if not self.self_sdk_view then
			self.self_sdk_view = LogicGUI.TemplSelfSdkRegister.New(function ()
				self.self_sdk_view = nil
			end)

			self.self_sdk_view:SetLoadedCallback(function ()
				if self:CallModuleFunc("IsSwitchAccount") then
					self:CallModuleFunc("ResetSwitchAccount")
					self.self_sdk_view:OpenPanelCut()
				else
					self.self_sdk_view:JudgeOpen()
				end
			end)
		end
	else
		--打开SDK登陆界面
		self:CallModuleFunc("LoginSDK")
	end
end

function Modules.LoginView:__HandleLoginII()
	local server_info = self:CallModuleFunc("GetServerInfo") or self:CallModuleFunc("GetRecommendServerInfo")
	if not server_info then
		server_info = self:CallModuleFunc("GetRecommendServerInfo", true)
	end
	local panel_server_info = self:GetWidget("Panel_Server")

	if server_info then
		panel_server_info:SetVisible(true)

		WidgetUnity.SetWidgetListener(panel_server_info, nil, nil, function()
			--切换到服务器列表
			GlobalStateMgr:ToServerListState()
		end)

		local btn_login = Util:WidgetButton(
											self:GetWidget("Button_Login") , 
											CPPGameLib.GetString("server_enter") , 
											function()
												self:CallModuleFunc("HandleSelectServer", server_info.id)
   										    end , 
											Macros.TypefaceSize.headline , 
											Macros.Color.btn , 
											Macros.Color.button_red
										)

		local is_show_switch_account = CPPSdkTools.IsShowSwitchAccountBtn()
		local txt_change_account = self:GetWidget("Text_9")
		txt_change_account:SetText( CPPGameLib.GetString( "login_cutGame" ) )
		txt_change_account:SetFontSize( Macros.TypefaceSize.normal)
		
		local img_change_account = self:GetWidget("Image_ChangeAccount")
		txt_change_account:SetVisible(is_show_switch_account)
		img_change_account:SetVisible(is_show_switch_account)
		WidgetUnity.SetWidgetListener(img_change_account, nil, nil, function()
			--切换账号
			self:CallModuleFunc("SwitchAccount")
		end)

		local img_server_status = self:GetWidget("Image_ServerState")
		local text_server_id = self:GetWidget("Text_ServerID")
		local text_server_name = self:GetWidget("Text_ServerName")

		local server_id = server_info.id
		local server_id_name = string.format(CPPGameLib.GetString("login_server_id_name"), server_id)
		text_server_id:SetText(server_id_name)
		text_server_id:SetFontSize( Macros.TypefaceSize.normal )

		text_server_name:SetText(server_info.name)
		text_server_name:SetFontSize( Macros.TypefaceSize.normal )

		local imgdata = self:CallModuleFunc("GetServerStatusImgData", server_info.status)
		img_server_status:SetImage(imgdata)
	else
		panel_server_info:SetVisible(false)
	end
end

function Modules.LoginView:__HandleCreateRole()
	self:ResetBG({type = BaseView.BGType.SOLID})

	local img_game_title = self:GetWidget("Image_GameTitle")
	img_game_title:SetVisible(false)
	img_game_title:SetScaleFactor(0.95)

	local btn_start = Util:WidgetButton( 
											self:GetWidget("Button_Start") , 
											CPPGameLib.GetString("common_confirm") , 
											CPPGameLib.Handler(self,self.__OnCreateRole) ,
											nil,nil,Macros.Color.button_yellow
										)
	
	local img_dice = Util:WidgetButton( self:GetWidget("Image_Dice") , nil , CPPGameLib.Handler(self,self.__OnImgDice) )

	if not self.__panel_hero_model then
		self.__panel_hero_model = LogicGUI.Model.New({
													info_id = 31341, 
													parent = self:GetWidget("Panel_heroMode"),
													dir = MathTool.HORZ_LEFT
												})
		self.__panel_hero_model:SetScaleFactor(1.5)
		PosTool.CenterBottom(self.__panel_hero_model)
	end

	--设置初始名字
	self.edit_role_name = Util:WidgetTextField(self:GetWidget("TextField_RoleName"))
	self.edit_role_name:SetText( CallModuleFunc(ModuleType.SUNDRY, "GetRandomRoleName") )
	self.edit_role_name:SetTextColor( unpack(Macros.Color.golden) )
	self.edit_role_name:SetTextHorizontalAlign(UITextFieldEx.ALIGIN_CENTER)
	self.edit_role_name:SetTextVertiacalAlign(UITextFieldEx.ALIGIN_CENTER)
    self.edit_role_name:SetMaxLengthEnabled(true)
    self.edit_role_name:SetMaxLength(6)
    
	if not self.__create_role_tips then
		local Image_dialogue = self:GetWidget("Image_dialogue")
		self.__create_role_tips = Util:Label( CPPGameLib.GetString("common_login_tips") , Macros.TypefaceSize.popup , Macros.Color.content , Image_dialogue:GetContentWidth() - 28 )	
		Image_dialogue:AddChild(self.__create_role_tips)
		PosTool.Center(self.__create_role_tips, 5)
	end
end

--确定按钮
function Modules.LoginView:__OnCreateRole()
	local role_name = self.edit_role_name:GetText()
	local tab , type_ = LanguageFilter.NameFilter(role_name)
	if not role_name or role_name == "" then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_please_input_role_name"))	
	elseif not type_ then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_role_name_invalid"))	
	elseif type_ then
		local _ ,num ,__ = StringTool.CountUTF8String(role_name)
		if num > 6 then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_role_name_long"))
		else
			if not CallModuleFunc(ModuleType.SUNDRY, "IsNameConflitWithArenaRobot", role_name) then
				self:CallModuleFunc("ChangeRoleNameReq", role_name)
			else
				local config_error_code = CPPGameLib.GetConfig("ErrorCode", 1003 )
				GlobalTipMsg.GetInstance():Show(config_error_code.err_tips)
			end
		end
	end
end

--确定按钮
function Modules.LoginView:__OnImgDice()
	--设置初始名字
	self.edit_role_name:SetText(CallModuleFunc(ModuleType.SUNDRY, "GetRandomRoleName"))
end

--显示登陆模型、特效
function Modules.LoginView:__ShowModelsAndEffects()
	--是否显示渠道登录页
	if CPPSdkTools.IsSDKLoginBG() then
		return
	end

	if self.__has_show_models_and_effects then
		return
	end
	self.__has_show_models_and_effects = true

	local _, effect_root
	local model_num = 6
	local zorder

	--模型
	for i = 1, model_num do
		if i == 1 or i == 2 then
			zorder = i
		else
			zorder = CtrlZorder.EFFECT_BEG + i - 3
		end
		_, effect_root = self:__CreateEffect({res_name = "UI_loginmodel"..i, type = Effect.EffectType.SKELETON}, self.__root_widget, zorder)
		PosTool.CenterBottom(effect_root, -65)
	end

	--特效
	_, effect_root = self:__CreateEffect({res_name = "UI_denglujiemian"}, self.__root_widget, CtrlZorder.EFFECT_BEG + model_num + 1)
	PosTool.Center(effect_root)
end
