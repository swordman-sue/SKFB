
--[[
%% @module: 设置界面
%% @author: swordman sue
%% @created: 2016-11-2
--]]

Modules = Modules or {}

Modules.SettingTabView = Modules.SettingTabView or BaseClass(BaseTabView)

function Modules.SettingTabView:__init()
end

--选项卡界面创建完成回调
function Modules.SettingTabView:__LoadedCallback()
	self:__Getwidget()
	self:__ShowUI()
end

--选项卡界面打开回调
function Modules.SettingTabView:__OpenCallback()
end

--选项卡界面关闭回调
function Modules.SettingTabView:__CloseCallback()
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.SettingTabView:__Dispose()
end

--获得模板组件
function Modules.SettingTabView:__Getwidget()
	--上线
	local Image_3 = self:GetWidget("Image_3")
	--下线
	local Image_3_0 = self:GetWidget("Image_3_0")

	self.Text_open_1 = Util:WidgetLabel( self:GetWidget("Text_open_1") , CPPGameLib.GetString("server_open") , Macros.TypefaceSize.normal , Macros.Color.lilyWhite )
	self.Text_close_1 = Util:WidgetLabel( self:GetWidget("Text_close_1") , CPPGameLib.GetString("server_close") , Macros.TypefaceSize.normal , Macros.Color.lilyWhite )
	
	--音乐
	self.CheckBox_music = self:GetWidget("CheckBox_music")
	WidgetUnity.SetCheckboxListener(self.CheckBox_music, 
		function()
			self:OnCheckBoxMusic( true , true )
		end, 
		function()
			self:OnCheckBoxMusic( false , true )
		end
	)
	self.CheckBox_music:SetSelectedState( GlobalSoundMgr:GetBGMEnabled() )
	self:OnCheckBoxMusic( GlobalSoundMgr:GetBGMEnabled() )

	self.Text_open_2 = Util:WidgetLabel( self:GetWidget("Text_open_2") , CPPGameLib.GetString("server_open") , Macros.TypefaceSize.normal , Macros.Color.lilyWhite)
	self.Text_close_2 = Util:WidgetLabel( self:GetWidget("Text_close_2") , CPPGameLib.GetString("server_close") , Macros.TypefaceSize.normal , Macros.Color.lilyWhite)
	--音效
	self.CheckBox_sound = self:GetWidget("CheckBox_sound")
	WidgetUnity.SetCheckboxListener(self.CheckBox_sound, 
		function()
			self:OnCheckBoxSound( true , true )
		end, 
		function()
			self:OnCheckBoxSound( false , true )
		end
	) 
	self.CheckBox_sound:SetSelectedState( GlobalSoundMgr:GetEffectEnabled() )
	self:OnCheckBoxSound( GlobalSoundMgr:GetEffectEnabled() )

	--退出登陆
	local Button_exit = Util:WidgetButton( 
											self:GetWidget("Button_exit"), 
											CPPGameLib.GetString("server_quit"), 
											CPPGameLib.Handler(self,self.OnButtonExit), 
											nil, nil , Macros.Color.button_yellow
										)

	--联系GM
	local Button_GM = Util:WidgetButton( 
											self:GetWidget("Button_GM"), 
											CPPGameLib.GetString("server_gm"), 
											CPPGameLib.Handler(self,self.OnButton_GM), 
											nil , nil, Macros.Color.button_yellow
										)

	--审核状态下，屏蔽GM按钮
	Button_GM:SetVisible(not CPPSdkTools.IsInExamine())
	if ConfigAdapter.Common.GetVisibleGM() == 1 then
		Button_GM:SetVisible( false )
	end
	
	
	--游戏公告
	local Button_Notice = Util:WidgetButton( 
											self:GetWidget("Button_Notice"), 
											CPPGameLib.GetString("server_notice"), 
											CPPGameLib.Handler(self,self.OnButtonNotice), 
											nil, nil, Macros.Color.button_yellow
										)
	Button_Notice:SetVisible(CPPSdkTools.IsShowActivityNoticeBtn())
	if ConfigAdapter.Common.GetTemplNoticeShowType() == 0 then
		Button_Notice:SetVisible(false)
	end

	--提交建议
	local Button_upBug = Util:WidgetButton( 
											self:GetWidget("Button_upBug"), 
											CPPGameLib.GetString("server_upBug"), 
											CPPGameLib.Handler(self,self.OnButtonUpBug), 
											nil, nil, Macros.Color.button_yellow
										)
	Button_upBug:SetVisible( CPPSdkTools.IsShowSuggestionBtn() )

	--服务器
	self.Text_server = Util:WidgetLabel( 
											self:GetWidget("Text_server") , 
											CPPGameLib.GetString("server_server") , 
											Macros.TypefaceSize.popup , 
											Macros.Color.headline
										)

	--玩家ID
	self.Text_player = Util:WidgetLabel( 
											self:GetWidget("Text_player") , 
											CPPGameLib.GetString("server_playerId") , 
											Macros.TypefaceSize.popup , 
											Macros.Color.headline
										)
end

--创建显示组件
function Modules.SettingTabView:__ShowUI()
	local server_music = Util:Label( 
										CPPGameLib.GetString("server_music") , 
										Macros.TypefaceSize.popup ,
										 Macros.Color.keypoint 
									  )
	server_music:SetAnchorPoint( 1 , 0.5 )
	server_music:SetPosition( self.CheckBox_music:GetPositionX() - (self.CheckBox_music:GetContentWidth()/2) - 10 , self.CheckBox_music:GetPositionY() )
	self.__root_widget:AddChild( server_music )

	local server_sound = Util:Label( 
										CPPGameLib.GetString("server_sound") , 
										Macros.TypefaceSize.popup ,
										Macros.Color.keypoint
									  )
	server_sound:SetAnchorPoint( 1 , 0.5 )
	server_sound:SetPosition( self.CheckBox_sound:GetPositionX() - (self.CheckBox_sound:GetContentWidth()/2) - 10 , self.CheckBox_sound:GetPositionY() )
	self.__root_widget:AddChild( server_sound )

	local roleInfo = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
	local lst = CallModuleFunc(ModuleType.LOGIN, "GetServerInfo", roleInfo.server_id)

	local text = ""
	if lst then
		text =  CPPGameLib.GetString("common_district" , lst.id ) .. " - " .. lst.name
	end

	local server_id = Util:Label( text  , Macros.TypefaceSize.popup , Macros.Color.keypoint)
	server_id:SetAnchorPoint( 0 , 0.5 )
	server_id:SetPosition( self.Text_server:GetPositionX() + 2 , self.Text_server:GetPositionY() )
	self.__root_widget:AddChild( server_id )

	local server_id = Util:Label( roleInfo.role_id or 12580 , Macros.TypefaceSize.popup , Macros.Color.keypoint)
	server_id:SetAnchorPoint( 0 , 0.5 )
	server_id:SetPosition( self.Text_player:GetPositionX() + 2 , self.Text_player:GetPositionY() )
	self.__root_widget:AddChild( server_id )
end

--音乐
function Modules.SettingTabView:OnCheckBoxMusic( type_ , show ) 
	self.Text_open_1 :SetVisible( type_ == true )
	self.Text_close_1 :SetVisible( type_ ~= true )

	if show then
		local text = ""
		if type_ then
			text = CPPGameLib.GetString("server_bgmOp")
		else
			text = CPPGameLib.GetString("server_bgmEd")
		end
		GlobalTipMsg.GetInstance():Show( text )
	end

	GlobalSoundMgr:SetBGMEnabled(type_)
end

--音效
function Modules.SettingTabView:OnCheckBoxSound( type_ , show )
	self.Text_open_2 :SetVisible( type_ == true )
	self.Text_close_2 :SetVisible( type_ ~= true )

	if show then
		local text = ""
		if type_ then
			text = CPPGameLib.GetString("server_soundOp")
		else
			text = CPPGameLib.GetString("server_soundEd")
		end
		GlobalTipMsg.GetInstance():Show( text )
	end

	GlobalSoundMgr:SetEffectEnabled(type_)
end

--公告
function Modules.SettingTabView:OnButtonNotice( type_ , show )
    if ConfigAdapter.Common.GetTemplNoticeShowType() == 1 then
    	local tabLst = CallModuleFunc( ModuleType.NOTICE, "GetActivityNoticeData" )
    	if tabLst and #tabLst ~= 0 then
	        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACTIVITY_NOTICE)
		else
			GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("Notice_not") )
	    end
    end
end

--退出按钮
function Modules.SettingTabView:OnButtonExit(type_)
	CallModuleFunc(ModuleType.LOGIN, "Logout", true)
end

--联系GM
function Modules.SettingTabView:OnButton_GM(type_)
	CallModuleFunc(ModuleType.SUNDRY, "ConnectGM")
end

--联系GM
function Modules.SettingTabView:OnButtonUpBug(type_)
	LogicGUI.TemplUpBug.New()
end


--
-- @brief 提交建议
-- @author: yjg
-- @date: 2018年3月5日14:19:09
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplUpBug = LogicGUI.TemplUpBug or BaseClass(GUI.Template)

function LogicGUI.TemplUpBug:__init( fun )
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID} 
    self.__open_callback = function ()

    	self.func = fun

        self:__Getwidget()
        self:__ShowUI()

    end 
	self:InitTemplate(GUI.TemplateRegistry.UNION_IMPORT)	
end


function LogicGUI.TemplUpBug:__delete()
end

function LogicGUI.TemplUpBug:__Getwidget( ... )
	--背景
	self.Image_Bg = self:GetWidget("Image_Bg")
	--光
	self.Image_light = self:GetWidget("Image_light")	
	--关闭
	self.Button_close = Util:WidgetButton( 
                                            self:GetWidget("Button_close") , 
                                            nil , CPPGameLib.Handler(self,self.OnButtonClose)
                                        )
	--内容框
	self.Image_content = self:GetWidget("Image_content")	
	--输入框
    self.node_TextField = Util:WidgetTextField(self:GetWidget("node_TextField"))
	--确定	
	self.Button_establish = Util:WidgetButton( 
                                        self:GetWidget("Button_establish") , 
                                        CPPGameLib.GetString("common_confirm") , 
                                        CPPGameLib.Handler(self,self.OnButtonEstablish),
                                        nil,nil,Macros.Color.button_yellow 
                                    )



	--取消	
	self.Button_cancel = Util:WidgetButton( 
                                        self:GetWidget("Button_cancel") , 
                                        CPPGameLib.GetString("common_cancel") , 
                                        CPPGameLib.Handler(self,self.OnButtonCancel),
                                        nil,nil,Macros.Color.button_red 
                                    )

end

function LogicGUI.TemplUpBug:__ShowUI( ... )
	self.server_upBug = Util:Label( CPPGameLib.GetString("server_upBug") , Macros.TypefaceSize.popup , Macros.Color.white )	
	self.server_upBug:SetAnchorPoint( 0.5 , 0.5 )
	self.Image_light:AddChild( self.server_upBug )
	PosTool.Center( self.server_upBug )

	self.showText = Util:Label( CPPGameLib.GetString("server_maxImport") , nil , Macros.Color.lilyBlack , 400 )	
	self.showText:SetAnchorPoint( 0 , 1 )
	self.showText:SetPosition( 5 , self.node_TextField:GetPositionY()  )
	self.Image_content:AddChild( self.showText )

    --输入框
    self.node_TextField:SetMaxLength(50)
    self.node_TextField:SetMaxLengthEnabled(true)
    self.node_TextField:SetTextColor(Macros.Color.content)
    self.node_TextField:SetFontSize( self.Image_content:GetContentWidth() , self.Image_content:GetContentHeight() )
	self.node_TextField:SetTextHorizontalAlign(UITextFieldEx.ALIGIN_TOP)
	
	CPPActionManager.FadeOut( self.node_TextField , 0 )

    self.node_TextField:SetEventListener(
             GlobalCallbackMgr:AddCallBackFunc(function()
             	print("xy" , "==================1111")
             end),
             GlobalCallbackMgr:AddCallBackFunc(function()
             	print("xy" , "==================2222")
             end),
             GlobalCallbackMgr:AddCallBackFunc(function()
             	print("xy" , "==================3333")
             	self.showText:SetString( self.node_TextField:GetText() )
             end),
             GlobalCallbackMgr:AddCallBackFunc(function()
             	print("xy" , "==================4444")
             end)
    )  

	local server_importTips = Util:Label( CPPGameLib.GetString("server_importTips") , Macros.TypefaceSize.slightly , Macros.Color.keypoint )	
	server_importTips:SetAnchorPoint( 0.5 , 0 )
	self.Image_Bg:AddChild( server_importTips )
	PosTool.CenterBottom( server_importTips , 0 , 70 )


end

-- 关闭按钮
function LogicGUI.TemplUpBug:OnButtonClose( ... )	
	self:DeleteMe()
end

-- 取消
function LogicGUI.TemplUpBug:OnButtonCancel( ... )	
	self:DeleteMe()
end

-- 确定按钮
function LogicGUI.TemplUpBug:OnButtonEstablish( ... )	
	local name , isop = LanguageFilter.NameFilter( self.node_TextField:GetText() )
	if isop then
		local info = {}
		info.contack = "?"
		info.text = self.node_TextField:GetText()
		CallModuleFunc(ModuleType.ROLE, "PlayerLevelMessageReq", info )
		self:DeleteMe()
	else
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_inconformity") )
	end
end