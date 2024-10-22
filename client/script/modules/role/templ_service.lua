--
-- @brief 客服/提交建议 
-- @author: yjg
-- @date: 2018年3月8日16:30:36
--

Modules = Modules or {}

Modules.TemplService = Modules.TemplService or BaseClass(BaseView)

function Modules.TemplService:__init(id,direct,itemResolveId)
    self.__layout_name = "templ_service"  
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function()
		self:__OpenCallback()
	end

	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.EMAIL)
	-- self:InitTemplate(GUI.TemplateRegistry.TEMPL_SERVICE)	
end

function Modules.TemplService:__delete()

end

function Modules.TemplService:__OpenCallback()
	self:__Getwidget()
	self:__ShowUI()
	self:__Server()
end

function Modules.TemplService:__Server()
	CallModuleFunc(ModuleType.SUNDRY, "ServiceIssueHistory" )	

    self:BindGlobalEvent(Macros.Event.SundryModule.ISSUE_HISTORY , function( _ , lst )
    	self.listView:RemoveAllItems()
		self:SetImport()
    	self:RefreshHistory( lst )
    end )

    self:BindGlobalEvent(Macros.Event.SundryModule.SELECT_PATH , function( _ , lst )
    	self:SetScreenshot()
	end )
end

function Modules.TemplService:__Getwidget()
	--背景
	self.Image_bg = self:GetWidget("Image_bg") 
	--光
	self.Image_1 = self:GetWidget("Image_1") 
	--内容背景
	self.Panel_list = self:GetWidget("Panel_list") 
	--关闭按钮
	self.Button_close = Util:WidgetButton( self:GetWidget("Button_close") ,nil,CPPGameLib.Handler(self,self.OnBtnClose) )
end

function Modules.TemplService:__ShowUI()
	self.listView = UIListViewEx.Create()
	self.listView:SetDirection(UIListViewEx.ScrollDirVertical)
	self.listView:SetContentSize(self.Panel_list:GetContentWidth() - 5 ,self.Panel_list:GetContentHeight() - 5)
	self.listView:SetPosition(2.5,2.5)
	self.Panel_list:AddChild(self.listView)

	--标题
	local light = Util:Label( 
								CPPGameLib.GetString("server_upBug") , 
								Macros.TypefaceSize.popup , Macros.Color.lilyWhite )
	
	light:SetPosition(self.Image_1:GetContentWidth()/2 , self.Image_1:GetContentHeight()/2 )
	self.Image_1:AddChild(light)
end

--输入框
function Modules.TemplService:SetImport()
	--背景
	local layout = Util:Layout(self.listView:GetContentWidth(), self.listView:GetContentHeight() / 1.5 - 60 )
	--标题
	local service_import = Util:Label( 
								CPPGameLib.GetString("service_import") , 
								Macros.TypefaceSize.slightly , Macros.Color.keypoint)
	service_import:SetAnchorPoint( 0 , 1 )
	service_import:SetPosition( 10 , layout:GetContentHeight()  )
	layout:AddChild(service_import)

	--输入框
	local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "sp_divination_bg")
	local sp_divination_bg = Util:Sprite9( imgdata )
	sp_divination_bg:SetContentSize( layout:GetContentWidth() - 20 , 105 )
	sp_divination_bg:SetAnchorPoint( 0.5 , 1 )
	sp_divination_bg:SetPosition( layout:GetContentWidth()/2 , service_import:GetPositionY() - ( service_import:GetContentHeight() + 5 ) )
	layout:AddChild(sp_divination_bg)

	--显示文本
	self.showText = Util:Label( CPPGameLib.GetString("server_maxImport") , nil , Macros.Color.lilyWhite , sp_divination_bg:GetContentWidth() - 10 )	
	self.showText:SetAnchorPoint( 0 , 1 )
	self.showText:SetPosition( 5 , sp_divination_bg:GetContentHeight() - 5 )
	sp_divination_bg:AddChild( self.showText )

	--输入框本体
	local TextField = UITextFieldEx.Create()
    TextField:SetMaxLength(50)
    TextField:SetMaxLengthEnabled(true)
	TextField:SetAnchorPoint(0.5, 0.5)
	TextField:SetTouchSoundInfo(Macros.Game.SystemIDForSound.COMMON, 2)
	TextField:SetContentSize( sp_divination_bg:GetContentWidth() , sp_divination_bg:GetContentHeight() )
	TextField:SetTouchAreaEnabled(true)
	TextField:SetTouchSize( sp_divination_bg:GetContentWidth() , sp_divination_bg:GetContentHeight() )
	sp_divination_bg:AddChild( TextField )
	PosTool.Center( TextField )
	CPPActionManager.FadeOut( TextField , 0 )

    TextField:SetEventListener(
             GlobalCallbackMgr:AddCallBackFunc(function()
             	print("xy" , "==================1111")
             end),
             GlobalCallbackMgr:AddCallBackFunc(function()
             	print("xy" , "==================2222")
             end),
             GlobalCallbackMgr:AddCallBackFunc(function()
             	print("xy" , "==================3333" , TextField:GetText() )
             	self.showText:SetString( TextField:GetText() )
             end),
             GlobalCallbackMgr:AddCallBackFunc(function()
             	print("xy" , "==================4444")
             end)
    )  

	--截图-----------------------------------------------------------------------------------------------------------------------------------------------------------
	self.path = {}
	local service_screenshot = {}
	for i = 1 , 2 do
		local y = sp_divination_bg:GetPositionY() - ( sp_divination_bg:GetContentHeight() + 10 )
		if service_screenshot[ i - 1 ] then
			y = service_screenshot[ i - 1 ]:GetPositionY() - ( service_screenshot[ i - 1 ]:GetContentHeight() + 10 )
		end

		service_screenshot[ i ] = Util:Label( CPPGameLib.GetString("service_screenshot") .. "1" .. CPPGameLib.GetString("common_colon") , Macros.TypefaceSize.slightly , Macros.Color.keypoint)
		service_screenshot[ i ]:SetAnchorPoint( 0 , 1 )
		service_screenshot[ i ]:SetPosition( 60 , y )
		layout:AddChild(service_screenshot[ i ])

		--截图背景
		local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "sp_divination_bg")
		local sp_screenshot = Util:Sprite9( imgdata )
		sp_screenshot:SetContentSize( 340 , service_screenshot[ i ]:GetContentHeight() + 2 )
		sp_screenshot:SetAnchorPoint( 0 , 0.5 )
		sp_screenshot:SetPosition( service_screenshot[ i ]:GetPositionX() + ( service_screenshot[ i ]:GetContentWidth() + 5 ) , y - 10 )
		layout:AddChild(sp_screenshot)

		--选择上传
		local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "select_up")
		local select_up = Util:Sprite9( imgdata )
		select_up:SetAnchorPoint( 1 , 0.5 )
		select_up:SetPosition( sp_screenshot:GetContentWidth() - 2 , sp_screenshot:GetContentHeight()/2 )
		sp_screenshot:AddChild(select_up)

		--路径
		self.path[i] = Util:Label( "" , Macros.TypefaceSize.slightly , Macros.Color.lilyWhite )
		self.path[i]:SetAnchorPoint( 0 , 0.5 )
		self.path[i]:SetPosition( 5 , sp_screenshot:GetContentHeight()/2 )
		sp_screenshot:AddChild( self.path[i] )

		--点击
		local screenshot = Util:Layout( sp_screenshot:GetContentWidth() , sp_screenshot:GetContentHeight() , function ( ... )
			self:Screenshot( self.path[i] , i )
		end)
		screenshot:SetAnchorPoint( 0.5 , 0.5 )
		sp_screenshot:AddChild(screenshot)
		PosTool.Center( screenshot )

		--选择图片
		local service_selectImg = Util:Label( CPPGameLib.GetString("service_selectImg") , Macros.TypefaceSize.minimum , Macros.Color.keypoint)
		select_up:AddChild( service_selectImg )
		PosTool.Center( service_selectImg )
	end

	--确认提交
    local btn_btn_bigred = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bigred")
    local btn  =  Util:Button(
                                    btn_btn_bigred,
                                    CPPGameLib.Handler(self, self.Affirm),
                                    CPPGameLib.GetString("service_affirmBtn"),
                                    Macros.TypefaceSize.button,
                                    Macros.Color.lilyWhite,
                                    Macros.Color.button_red
                                )
    btn:SetAnchorPoint( 1 , 0.5 )
    btn:SetPosition( layout:GetContentWidth() - 75 , service_screenshot[1]:GetPositionY() - ( service_screenshot[1]:GetContentHeight() + 5 ) )
    layout:AddChild(btn) 

    --历史记录
    local service_history = Util:Label( CPPGameLib.GetString("service_history") , Macros.TypefaceSize.slightly , Macros.Color.keypoint)
	service_history:SetAnchorPoint( 0 , 1 )
	service_history:SetPosition( service_import:GetPositionX() , btn:GetPositionY() - ( btn:GetContentHeight()/2  + 15 ) )
	layout:AddChild( service_history )

	self.listView:PushBackCustomItem(layout)
end

--刷新历史
function Modules.TemplService:RefreshHistory( lst )

	table.sort( lst , function( a , b )
		return a.id < b.id
	end )

	local layout = Util:Layout(self.listView:GetContentWidth(), 20 )

	--背景图
	local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "bg_sp9_lst_layer1")
	local sp_divination_bg = Util:Sprite9( imgdata )
	sp_divination_bg:SetContentSize( layout:GetContentWidth() - 20 , layout:GetContentHeight() - 5 )
	sp_divination_bg:SetAnchorPoint( 0.5 , 0.5 )
	sp_divination_bg:SetPosition( layout:GetContentWidth()/2 , layout:GetContentHeight()/2 )
	layout:AddChild(sp_divination_bg)


	local function Stretch ( item )
		--容器
		layout:SetContentSize( layout:GetContentWidth() , layout:GetContentHeight() + item:GetContentHeight() )
		--背景
		sp_divination_bg:SetContentSize( sp_divination_bg:GetContentWidth() , layout:GetContentHeight() - 5 )
		sp_divination_bg:SetPosition( layout:GetContentWidth()/2 , layout:GetContentHeight()/2 )
	end

	local bg_sp9_label_frame = {}

	for i , v in ipairs( lst ) do
		--条目
	    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_label_frame")
	    local bg_sp9_label_frame = Util:Sprite9( sp )

		local layer = Util:Layout( sp_divination_bg:GetContentWidth() , bg_sp9_label_frame:GetContentHeight() + 5  )
		layer:SetAnchorPoint( 0.5 , 1 )
		Stretch( layer )
		layer:SetPosition( sp_divination_bg:GetContentWidth()/2 , sp_divination_bg:GetContentHeight() - 2.5 )
		sp_divination_bg:AddChild( layer )

	    bg_sp9_label_frame:SetContentSize( sp_divination_bg:GetContentWidth() - 10 , bg_sp9_label_frame:GetContentHeight() )
	    bg_sp9_label_frame:SetAnchorPoint( 0.5 , 0.5 )
	    bg_sp9_label_frame:SetPosition( layer:GetContentWidth()/2 , layer:GetContentHeight()/2 )
	    layer:AddChild( bg_sp9_label_frame )

	    --问题描述
	    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp_equip_attribute")
	    local bg_sp_equip_attribute = Util:Sprite9( sp )
	    bg_sp_equip_attribute:SetContentSize( bg_sp9_label_frame:GetContentWidth() - 185 , bg_sp9_label_frame:GetContentHeight() - 10 )
	    bg_sp_equip_attribute:SetAnchorPoint( 0 , 0.5 )
	    bg_sp_equip_attribute:SetPosition( 10 , layer:GetContentHeight()/2 )
	    layer:AddChild( bg_sp_equip_attribute )

	    local text = Util:Label( v.question , Macros.TypefaceSize.normal , Macros.Color.content , bg_sp_equip_attribute:GetContentWidth() - 20 )
	    text:SetAnchorPoint( 0 , 1 )
	    text:SetPosition( 15 , bg_sp_equip_attribute:GetContentHeight()  )
	    layer:AddChild( text )

	    --查看回复
	    local btn_btn_bigred = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen2")
	    local btn  =  Util:Button(
	                                    btn_btn_bigred,
	                                    function( ... )
	                                    	self:Examine( v )
	                                    end,
	                                    CPPGameLib.GetString("service_examine"),
	                                    Macros.TypefaceSize.button,
	                                    Macros.Color.lilyWhite,
	                                    Macros.Color.button_yellow
	                                )
	    btn:SetAnchorPoint( 1 , 0.5 )
	    btn:SetPosition( bg_sp9_label_frame:GetContentWidth() - 15 , bg_sp9_label_frame:GetContentHeight()/2 )
	    bg_sp9_label_frame:AddChild(btn) 

	    -- --未回复
	    -- if v.status == 0 then
	    -- 	Util:GreyButton( btn )
	    -- else
	    	self:SetRed( btn , v )
	    -- end

	end
	self.listView:PushBackCustomItem(layout , 1)
	self:SetScreenshot()
end

--选择路径
function Modules.TemplService:Screenshot( node , index )
	CallModuleFunc( ModuleType.SUNDRY, "SetSelectFrame" , index)
	CPPSdkTools.OpenAlbum( )
end

--选择路径
function Modules.TemplService:SetScreenshot( )
	local lst = CallModuleFunc( ModuleType.SUNDRY, "GetServiceSelectPath" )
	for i , v in ipairs( lst ) do
		local path = CPPGameLib.SplitEx( v , "/")

		local ln = string.len( path[#path] )
		local text = path[#path]
		if ln > 10 then
			text = string.sub (text , 0 , 10 )
			text = text .."..."
		end

		if self.path[i] then
			self.path[i]:SetString( text )
		end
	end
end


--确认提交
function Modules.TemplService:Affirm()
	if self.showText:GetString() == CPPGameLib.GetString("server_maxImport") then
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("service_submitTips") )
		return
	end

	local _ ,num ,__ = StringTool.CountUTF8String( self.showText:GetString() )
	if num < 10 then
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("service_submitTips") )
		return
	end

	local lst = CallModuleFunc( ModuleType.SUNDRY, "GetServiceSelectPath" )
	if not lst then
		lst = {}
	end
	CallModuleFunc(ModuleType.SUNDRY, "ServiceIssueSubmit" , self.showText:GetString() , lst[1] , lst[2] )	
	
	self.showText:SetString( CPPGameLib.GetString("server_maxImport") )
	CallModuleFunc( ModuleType.SUNDRY, "ServiceSelectPath" , nil )
	self.path[1]:SetString( " " )
	self.path[2]:SetString( " " )
end


--查看信息
function Modules.TemplService:Examine( data )
	CPPGameLib.PrintTable("xy" , data , "Examine")  
	LogicGUI.TemplServiceExamine.New( data )

	local t1 = {}
	local lst = CPPGameLib.SplitEx( Cookies.Get( nil ,Macros.Setting.ServiceRed) , "<GH>")
	for i , v in ipairs( lst ) do
		t1[v] = true
	end

	--如果没红点
	local MAX = 9
	if not t1[data.id] then
		local t2 = {}
		for i = 1 , MAX do
			if lst[i] then
				table.insert( t2 , lst[i] )
			end
		end
		table.insert( t2 , 1 , data.id )

		local red = ""
		for i , v in ipairs( t2 ) do
			red = red .. v .. "<GH>"
		end
		Cookies.Set( nil , Macros.Setting.ServiceRed , red )
		self.icon_red_point:SetVisible( false )
	end

end

--关闭按钮
function Modules.TemplService:OnBtnClose( ... )
    -- self:DeleteMe()
    CallModuleFunc(ModuleType.SUNDRY, "ServiceSelectPath" , nil )	
    self:CloseManual()
end


--红点设置
function Modules.TemplService:SetRed( btn , data )
	
	--已经回复的
	local entry_red_dot_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
	self.icon_red_point = Util:Sprite( entry_red_dot_image_data )
	self.icon_red_point:SetPosition( btn:GetContentWidth() - 5 , btn:GetContentHeight() - 10 )
	self.icon_red_point:SetVisible(false)
	btn:AddChild( self.icon_red_point )

	local t1 = {}
	local lst = CPPGameLib.SplitEx( Cookies.Get( nil ,Macros.Setting.ServiceRed) , "<GH>")
	for i , v in ipairs( lst ) do
		t1[tonumber(v)] = true
	end

	if not t1[data.id] then
		self.icon_red_point:SetVisible(true)
	else
		self.icon_red_point:SetVisible(false)
	end

end




LogicGUI = LogicGUI or {}

LogicGUI.TemplServiceExamine = LogicGUI.TemplServiceExamine or BaseClass(GUI.Template)

function LogicGUI.TemplServiceExamine:__init( data )
	self.__layout_mode = GUI.Template.LayoutMode.CODE
	self.__bg_params = {type = BaseView.BGType.SOLID}

    self:InitTemplate(nil)

	self:__Getwidget()
	self:__ShowUI( data )
end

function LogicGUI.TemplServiceExamine:__CreateWidgetFromLuaCode()
	self.__layout = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height)
	return self.__layout
end

function LogicGUI.TemplServiceExamine:__delete()

end

function LogicGUI.TemplServiceExamine:__Getwidget()
	local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.EMAIL, "bg_email_reply")
	self.email = Util:Sprite9( imgdata )
	self.email:SetAnchorPoint( 0.5 , 0.5 )
	self.__layout:AddChild( self.email )
	PosTool.Center( self.email )
	
	local btn_register = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_register")
    local btn  =  Util:Button(
                                    btn_register,
                                    function( ... )
                                    	self:OnBtnClose( v )
                                    end
                                )
    btn:SetAnchorPoint( 0.5 , 0.5 )
    btn:SetPosition( self.email:GetContentWidth() - 40 , self.email:GetContentHeight() - 35 )
    self.email:AddChild(btn) 
end

function LogicGUI.TemplServiceExamine:__ShowUI( data )

	local service_service = Util:Label( CPPGameLib.GetString( "service_service" ) , Macros.TypefaceSize.headline , Macros.Color.keypoint)
	service_service:SetAnchorPoint( 0.5 , 0.5 )
	service_service:SetPosition( self.email:GetContentWidth()/2 , self.email:GetContentHeight() - 35 )
	self.email:AddChild( service_service )

	local text = Util:Label( data.reply , 20 , Macros.Color.keypoint, self.email:GetContentWidth() - 60 )
	text:SetAnchorPoint( 0 , 1 )
	text:SetPosition( 35 , self.email:GetContentHeight() - 75 )
	self.email:AddChild( text )
end

--关闭按钮
function LogicGUI.TemplServiceExamine:OnBtnClose( ... )
    self:DeleteMe()
end
