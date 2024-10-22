--[[
%% @module: 称号
%% @author: yjg
%% @created: 2017年7月8日17:12:11
--]]

Modules = Modules or {}

Modules.TitleView = Modules.TitleView or BaseClass(BaseView)

function Modules.TitleView:__init( data )
    self.__layout_name = "temp_title"  
	self.__bg_params = {type = BaseView.BGType.SOLID}

    self.__open_callback = function()
        self:__Getwidget()
        self:__ShowUI()
        self:__Server()
    end
    self.__tab_params_ex = { 6 , true }

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.TITLE)  
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.TITLE,true)  
end

function Modules.TitleView:__Getwidget()
	--背景
	self.Image_setting = self:GetWidget("Image_setting") 
	--光
	self.Image_light = self:GetWidget("Image_light") 
	--关闭按钮
	self.Button_close = Util:WidgetButton( self:GetWidget("Button_close") , nil , CPPGameLib.Handler(self,self.OnButtonClose) ) 
	
	--内容背景
	self.Image_title_bg = self:GetWidget("Image_title_bg") 
	--选择框
	self.Image_frame = self:GetWidget("Image_frame")
	--获取路径
	self.Image_way = self:GetWidget("Image_way")
	--持续时间
	self.Image_time = self:GetWidget("Image_time")
	--属性
	self.Image_property = self:GetWidget("Image_property")
	--页签
	self.Panel_lst = self:GetWidget("Panel_lst")
	--英雄占位
	self.Panel_icon = self:GetWidget("Panel_icon")
	--使用
	self.Button_use = Util:WidgetButton( 
											self:GetWidget("Button_use") , 
											CPPGameLib.GetString("title_adorn") , 
											CPPGameLib.Handler(self,self.OnButtonUse) ,
											Macros.TypefaceSize.popup,nil,Macros.Color.button_yellow
										) 
	--属性
	self.Button_property = Util:WidgetButton( self:GetWidget("Button_property") , nil , CPPGameLib.Handler(self,self.OnButtonProperty) ) 
end

function Modules.TitleView:__ShowUI()
	local name = ""
	if unpack(self.__open_data) == Macros.Game.TITLE_TYPE.TITLE then
		name = CPPGameLib.GetString("title")
	else
		name = CPPGameLib.GetString("title_")
	end

	--标题
	local title = Util:Label( name , Macros.TypefaceSize.popup , Macros.Color.lilyWhite  )
	title:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2)
	self.Image_light:AddChild(title)



	local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
	--创建模型
	local icon = LogicGUI.Model.New({
										info_id = userDate.head_icon , 
										parent = self.Panel_icon ,
										model_type = LogicGUI.ModelType.HERO,
									})
	local config_hero = CPPGameLib.GetConfig("Hero", userDate.head_icon )
	icon:SetScaleFactor(config_hero.ui_scale or 1)

	--名字
	self.name = Util:Name( userDate.role_name , Macros.TypefaceSize.largish )
	self.Panel_icon:AddChild( self.name )
	PosTool.Center( self.name , 0 , 210 )

	self.title = Util:Sprite( Resource.PathTool.GetTitleIconPath( "sp_title_1" , true ) )
	self.title:SetPosition( self.name :GetContentWidth()/2 , self.name:GetContentHeight() + ( self.title:GetContentHeight() ) )
	self.name:AddChild( self.title )
	self.title:SetVisible(false)

	--前缀
	self.prefix = Util:Sprite(Resource.PathTool.GetTitleIconPath( "sp_prefix_26" , true ) )
	self.prefix:SetAnchorPoint( 1 , 0.5 )
	self.prefix:SetPosition( -10 , self.name:GetContentHeight()/2 )
	self.name:AddChild( self.prefix )
	self.prefix:SetVisible(false)

	--后缀 
	self.suffix = Util:Sprite( Resource.PathTool.GetTitleIconPath( "sp_suffix_26" ) )
	self.suffix:SetAnchorPoint( 0 , 0.5 )
	self.suffix:SetPosition( self.name:GetContentWidth() + 10 , self.name:GetContentHeight()/2 )
	self.name:AddChild( self.suffix )
	self.suffix:SetVisible(false)

	--获取途径
	local title_approach = Util:Label( CPPGameLib.GetString("title_approach") , Macros.TypefaceSize.largish , Macros.Color.Special )
	title_approach:SetAnchorPoint( 0 , 0.5 )
	title_approach:SetPosition( 8 , self.Image_way:GetContentHeight() - 20 )
	self.Image_way:AddChild( title_approach )

	self.title_approach = Util:Label( 
										"" , 
										Macros.TypefaceSize.slightly , 
										Macros.Color.white ,
										self.Image_way:GetContentWidth() - 16
									)
	self.title_approach:SetAnchorPoint( 0 , 1 )
	self.title_approach:SetPosition( 8 , self.Image_way:GetContentHeight() - 40 )
	self.Image_way:AddChild( self.title_approach )

	--获取途径触摸
    self.Image_way_Tou = Util:Layout( self.Image_way:GetContentWidth() , self.Image_way:GetContentHeight() - 36 )
    self.Image_way_Tou:SetAnchorPoint(0.5,0)
    self.Image_way_Tou:SetPosition( self.Image_way:GetContentWidth()/2 , 0 )
    self.Image_way:AddChild( self.Image_way_Tou )

	--保质期
	local title_time = Util:Label( CPPGameLib.GetString("title_time") , Macros.TypefaceSize.largish , Macros.Color.Special )
	title_time:SetAnchorPoint( 0 , 0.5 )
	title_time:SetPosition( 8 , self.Image_time:GetContentHeight() - 20 )
	self.Image_time:AddChild( title_time )

	self.title_time = Util:Label( 
										"" , 
										Macros.TypefaceSize.slightly , 
										Macros.Color.white ,
										self.Image_time:GetContentWidth() - 16
									)
	self.title_time:SetAnchorPoint( 0 , 1 )
	self.title_time:SetPosition( 8 , self.Image_time:GetContentHeight() - 40 )
	self.Image_time:AddChild( self.title_time )

	--属性
	local title_property = Util:Label( CPPGameLib.GetString("title_property") , Macros.TypefaceSize.largish , Macros.Color.Special )
	title_property:SetAnchorPoint( 0 , 0.5 )
	title_property:SetPosition( 8 , self.Image_property:GetContentHeight() - 20 )
	self.Image_property:AddChild( title_property )


	self.title_property = {}
	local y = self.Image_property:GetContentHeight() - 43
	for i = 1 , 3 do
		self.title_property[i] = {}
		--类型
		self.title_property[i].type = Util:Label( 
											" " , 
											Macros.TypefaceSize.slightly , 
											Macros.Color.white
										)
		self.title_property[i].type:SetAnchorPoint( 0 , 1 )
		self.title_property[i].type:SetPosition( 8 , y )
		self.Image_property:AddChild( self.title_property[i].type )

		--值
		self.title_property[i].value = Util:Label( 
											" " , 
											Macros.TypefaceSize.slightly , 
											Macros.Color.green
										)
		self.title_property[i].value:SetAnchorPoint( 0 , 1 )
		self.title_property[i].value:SetPosition( self.title_property[i].type:GetContentWidth() + self.title_property[i].type:GetPositionX() + 5 , y )
		self.Image_property:AddChild( self.title_property[i].value )

		y = y - 25 
	end

	--获得表
	local lst = CallModuleFunc( ModuleType.TITLE , "GetTitleCnf", unpack(self.__open_data) )
    --条目
    local params =  
    {
        item_class = Modules.TitleViewItem,
        item_width = self.Image_frame:GetContentWidth() ,
        item_height = 104,
        row = 4,
        col = 1,
        item_space_r = 5,
        view_height = self.Image_frame:GetContentHeight() - 5 ,
	    selectable = true,
	    select_callback = function (index, data)
	    	self:__SetShowUi( data , index )
	    end,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.Image_frame:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode())

	--页签
	self.tab = {}
	for i , v in pairs(lst) do
		local cnf = CPPGameLib.GetConfig("SystemEntry", i )
		table.insert( self.tab , { name = cnf.name , sort = i , data = v } )
	end

	table.sort( self.tab , function ( a , b )
		return a.sort < b.sort
	end )

    self.__tab:SetContentSize( self.Panel_lst:GetContentWidth() , self.Panel_lst:GetContentHeight() )
    self.__tab:SetData( self.tab ) 
    self.__tab:SetItemAlign( 0 )

    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TITLE, "btn_title_ontab")
    local no = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TITLE, "btn_title_notab")
    self.__tab:SetOnCeil( on , no )  

    if #self.tab > 5 then
	    self.__tab:SetTouchEnabled(true)
	else
		self.__tab:SetTouchEnabled(false)
	end

    self.__tab:SetPosition(self.Panel_lst:GetContentWidth()/2 , self.Panel_lst:GetContentHeight()/2 )
    self.Panel_lst:AddChild(self.__tab:GetNode())
end

function Modules.TitleView:__Server()
	CallModuleFunc( ModuleType.TITLE , "RoleTitleInfoReq" )
	self:BindGlobalEvent(Macros.Event.TitleModule.TITLE, function( ... )
		self.__tab:On( self.__open_data[2] or self.index or 1 , self.tab[ self.__open_data[2] or 1 ] or self.lst )
	end)
end

function Modules.TitleView:OnButtonClose()
	self:CloseManual()
end

function Modules.TitleView:Ceil( index , data )
    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TITLE, "btn_title_ontab")
    local sizeSp = Util:Sprite( on )
    local layer = Util:Layout( sizeSp:GetContentWidth() , sizeSp:GetContentHeight() )

    local name = Util:Label( data.name , Macros.TypefaceSize.largish , Macros.Color.white , nil , nil , Macros.Color.button_palm )
    name:SetAnchorPoint(0.5, 0.5)
    layer:AddChild(name)
    PosTool.Center(name)

    return layer
end

function Modules.TitleView:OnCallBack( index , lst )
	self.index = index
	self.lst = lst
	self:SetLst( lst.data )
end

--刷新列表
function Modules.TitleView:SetLst( data )
	local info = CallModuleFunc( ModuleType.TITLE , "GetRecord" , data.id )

	local t1 , t2 , t3 = {} , {} , {} 
	local titleId , titleId_ = CallModuleFunc( ModuleType.TITLE , "GetTitleId" )
	for i , v in ipairs( data ) do
		if v.id == titleId or v.id == titleId_ then
			table.insert( t1 , v )
		elseif CallModuleFunc( ModuleType.TITLE , "GetRecord" , v.id ) then
			table.insert( t2 , v )
		else
			table.insert( t3 , v )
		end
	end

	for i , v in ipairs( t2 ) do
		table.insert( t1 , v )
	end

	for i , v in ipairs( t3 ) do
		table.insert( t1 , v )
	end	

    self.lst_list:SetDataList( t1 )
    self.lst_list:SelectItem( 1 )
end

--显示的UI设置
function Modules.TitleView:__SetShowUi( data ,index )
	--设置称号
	self.title:SetVisible(false)
	self.prefix:SetVisible(false)
	self.suffix:SetVisible(false)

	if data.type == Macros.Game.TITLE_TYPE.TITLE then

		-- self.title:SetImage( Resource.PathTool.GetTitleIconPath( "sp_title_".. data.id , true ) , TextureResType.LOCAL )
		self.title:SetImage( Resource.PathTool.GetTitleIconPath( "sp_title_".. data.id , true ) )
		self.title:SetPosition( self.name :GetContentWidth()/2 , self.name:GetContentHeight() + ( self.title:GetContentHeight()/2 ) )

		self.title:SetVisible(true)

	elseif data.type == Macros.Game.TITLE_TYPE.PREFIX then

		-- 
		-- self.prefix:SetImage( Resource.PathTool.GetTitleIconPath( "sp_prefix_"..data.id ,true ) , TextureResType.LOCAL )
		self.prefix:SetImage( Resource.PathTool.GetTitleIconPath( "sp_prefix_"..data.id ,true ) )
		self.prefix:SetVisible(true)

		-- self.suffix:SetImage( Resource.PathTool.GetTitleIconPath( "sp_suffix_"..data.id ) , TextureResType.LOCAL )
		local cnf = CPPGameLib.GetConfig("Title", data.id )
		if cnf.postfix == 1 then
			self.suffix:SetImage( Resource.PathTool.GetTitleIconPath( "sp_suffix_"..data.id ) )
			self.suffix:SetVisible(true)
		end
	end
	--设置途径
	self.title_approach:SetString( data.approach_text )

	local info = CallModuleFunc( ModuleType.TITLE , "GetRecord" , data.id )

	CPPActionManager.StopAllActions(self.title_time)

	if info then
		-- --保质期 --表里的剩余的时间
		if info.over_time ~= 0 then
			-- self.title_time:SetString( TimeTool.TransTimeStamp( CPPGameLib.GetString("title_residue") , info.over_time ) )
			self:Countdown( info.over_time  )
		else

			self.title_time:SetString( CPPGameLib.GetString("title_always") )
		end
	else
		--保质期 --表里的时间
		if data.valid_time ~= 0 then
			self.title_time:SetString( CPPGameLib.GetString("title_residue" , TimeTool.OfflineTime_1( data.valid_time * 60 ) ) )
		else
			self.title_time:SetString( CPPGameLib.GetString("title_always") )
		end
	end

	--属性
	for i = 1 , #self.title_property do
		local type_ , value_ = nil
		if data.property[i] then
			type_ , value_ =  Util:Nature( data.property[i].type , data.property[i].value , true )
		end
		self.title_property[i].type:SetString( type_ or "" )
		self.title_property[i].value:SetString( value_ or "" )
		self.title_property[i].value:SetPosition( self.title_property[i].type:GetContentWidth() + self.title_property[i].type:GetPositionX() + 5 , self.title_property[i].type:GetPositionY() )
	end

	-- 获取途径
	self.Image_way_Tou:SetTouchEnabled(true)
	WidgetUnity.SetWidgetListener( self.Image_way_Tou , nil, nil, function ( ... )
		local cnf = CPPGameLib.GetConfig("SystemEntry", data.approach )
		CallModuleFunc( cnf.module_type , "OpenView", cnf.view_type or cnf.module_type )
	end)

	self.id = data.id
	
	self.Button_use:SetVisible(true)
	--开启的
	local info = CallModuleFunc( ModuleType.TITLE , "GetRecord" , data.id )
	if info then
        self.Button_use:SetEnabled(true)
        self.Button_use:SetText(CPPGameLib.GetString("title_adorn"))     
        local r, g, b, a = unpack( Macros.Color.button_yellow )
        local stroke_width = 2
        self.Button_use:SetOutline(r, g, b, a, stroke_width) 
    else
        self.Button_use:SetText(CPPGameLib.GetString("title_noOpen"))
        Util:GreyButton( self.Button_use )
	end

	local infoId , titleId_ = CallModuleFunc( ModuleType.TITLE , "GetTitleId" )
	if infoId == data.id or data.id == titleId_ then
        self.Button_use:SetVisible(false)
	end
end

--使用
function Modules.TitleView:OnButtonUse()
	local info = {}
	info.title_id = self.id
	CallModuleFunc( ModuleType.TITLE , "UseTitleReq" , info )
end

--倒计时
function Modules.TitleView:Countdown( time )
	local func = nil
	local function moveAnimate( ... )
		local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
		if time - server_time ~= 0 then
			self.title_time:SetString( CPPGameLib.GetString("title_residue" , TimeTool.OfflineTime_1( time - server_time ) ) )
			CPPActionManager.DelayTo( self.title_time , 1 , func)
		else
			CallModuleFunc( ModuleType.TITLE , "RoleTitleInfoReq" )
		end
	end
	func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
	moveAnimate()
end

--获取打开参数
function Modules.TitleView:GetOpenParams()
    if self.index then
    	local index = unpack(self.__open_data)

        return { index , self.index}
    end
end


Modules = Modules or {}
Modules.TitleViewItem = Modules.TitleViewItem or BaseClass(GUI.ScrollItem)

function Modules.TitleViewItem:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TitleViewItem:__delete()

end

function Modules.TitleViewItem:__CreateWidgetFromLuaCode()
  self.layout = Util:Layout( 244.00 , 104.00 )
  return self.layout
end

function Modules.TitleViewItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self:__ShowUI(data)
end

function Modules.TitleViewItem:__Getwidget()
end

function Modules.TitleViewItem:__ShowUI(data)
	--按钮
	local btn_title_noselect = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TITLE, "btn_title_noselect")
	self.btn = Util:Sprite( btn_title_noselect )
	self.layout:AddChild( self.btn )
	PosTool.Center( self.btn )

	--图标
	if data.type == Macros.Game.TITLE_TYPE.TITLE then
		self:Title( data )
	elseif data.type == Macros.Game.TITLE_TYPE.PREFIX then
		self:Prefix( data )
	end

	--使用状态
	local sp_title_lock = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TITLE, "sp_title_lock" )
	self.sp_title_lock = Util:Sprite( sp_title_lock )
	self.sp_title_lock:SetAnchorPoint( 0 , 1 )
	self.sp_title_lock:SetPosition( 0 , self.btn:GetContentHeight() )
	self.btn:AddChild( self.sp_title_lock )

	local time_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TITLE, "sp_title_always" , true )
	if data.valid_time ~= 0 then
		time_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TITLE, "sp_title_limit" , true )
	end
	local time_type = Util:Sprite( time_sp )
	time_type:SetAnchorPoint( 1 , 1 )
	time_type:SetPosition( self.btn:GetContentWidth() , self.btn:GetContentHeight() - 1 )
	self.btn:AddChild( time_type )

	self:Transfer( data )
end

--称号
function Modules.TitleViewItem:Title( data )
	local sp_title = Resource.PathTool.GetTitleIconPath( "sp_title_"..data.id , true )
	self.icon = Util:Sprite( sp_title )
	self.btn:AddChild( self.icon )
	PosTool.Center( self.icon )
end

--前缀
function Modules.TitleViewItem:Prefix( data )
	local sp_title = Resource.PathTool.GetTitleIconPath( "sp_prefix_"..data.id , true )
	self.icon = Util:Sprite( sp_title )
	self.btn:AddChild( self.icon )
	PosTool.Center( self.icon )
end

--判断
function Modules.TitleViewItem:Transfer( data )

	local info = CallModuleFunc( ModuleType.TITLE , "GetRecord" , data.id )

	local titleId , titleId_ = CallModuleFunc( ModuleType.TITLE , "GetTitleId" )
	--穿着的
	if info and ( data.id == titleId or data.id == titleId_ ) then
		local sp_title = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TITLE, "sp_title_enabled" , true )
		self.sp_title_lock:SetImage( sp_title )
		self.sp_title_lock:SetVisible(true)
	--没开启的
	elseif not info then
		local sp_title = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TITLE, "sp_title_lock" )
		self.sp_title_lock:SetImage( sp_title )
		self.sp_title_lock:SetVisible(true)
	--开启的
	elseif info then
		self.sp_title_lock:SetVisible(false)
	end
end

function Modules.TitleViewItem:SetSelected(var)
	if var == true then
		local btn_title_onselect = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TITLE, "btn_title_onselect")
		self.btn:SetImage( btn_title_onselect )
	else
		local btn_title_noselect = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TITLE, "btn_title_noselect")
		self.btn:SetImage( btn_title_noselect )
	end
end