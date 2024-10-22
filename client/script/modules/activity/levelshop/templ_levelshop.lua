
--[[
%% @module: 等级商店
%% @author: yjg
%% @created: 2017年3月20日11:17:34
--]]

--不用

Modules = Modules or {}

Modules.TemplLevelShop = Modules.TemplLevelShop or BaseClass(BaseTabView)

function Modules.TemplLevelShop:__init()
end

--选项卡界面创建完成回调
function Modules.TemplLevelShop:__LoadedCallback()
	self:__Getwidget()
end

--选项卡界面打开回调
function Modules.TemplLevelShop:__OpenCallback()
	self:__SetShowUI()

	local lst = CallModuleFunc(ModuleType.ACTIVITY, "LevelShopGetLst")
	if not lst or CPPGameLib.GetTableLength(lst) <= 0 then
		CallModuleFunc(ModuleType.ACTIVITY, "LevelShopDataReq")
		CallModuleFunc(ModuleType.ACTIVITY, "LevelShopBuyNotesReq")	
	else
        self:OnPitch( self.data )
        self:OnShow()
	end

    --礼包刷新
    self:BindGlobalEvent(Macros.Event.ActivityModule.ACTIVITY_SHOP,function (_, data)
        self:OnPitch( self.data )
        self:OnShow()
    end)

    self:BindGlobalEvent(Macros.Event.ActivityModule.ACTIVITY_SHOP_RECORD,function (_, data)
		self.__chapters_scrollview:ItemDataIter(function(item_index, item_data)
													local item = self.__chapters_scrollview:GetItem(item_index)
													if item then
														item:SetRed(item_data)
													end
												end)
    end)

end

--选项卡界面关闭回调
function Modules.TemplLevelShop:__CloseCallback()
	if self.__chapters_scrollview then
		self.__chapters_scrollview:DeleteMe()
		self.__chapters_scrollview = nil
	end

	if self.pitch then
		self.pitch:DeleteMe()
		self.pitch = nil
	end

end

--选项卡界面销毁回调(释放非托管资源)
function Modules.TemplLevelShop:__Dispose()
end

--获得所有组件
function Modules.TemplLevelShop:__Getwidget()
	--内容层
	self.Image_contentLst = self:GetWidget("Image_contentLst")
	--滑动层
	self.Panel_2 = self:GetWidget("Panel_2")
	--左按钮
	self.Button_left = self:GetWidget("Button_left")
	--右按钮
	self.Button_right = self:GetWidget("Button_right")
end

function Modules.TemplLevelShop:__SetShowUI()

	self.pitch = Modules.TemplCommodity.New(self.Image_contentLst)

    local params = 
    {
        item_class = Modules.TemplLevelShopItem,
        item_width = 300,
        item_height = 215,
        row = 1,
        col = 3,
        item_space_c = -80,
        horizon = true,
        view_width = self.Panel_2:GetContentWidth(), 
        selectable = true,
        select_callback = function(item_index, item_data)
			self:OnPitch( item_data )
        end,
    }
    self.__chapters_scrollview = GUI.SpecialScrollView.New(params)    
    self.Panel_2:AddChild(self.__chapters_scrollview:GetNode())
    PosTool.Center(self.__chapters_scrollview)	   
    local cnf = {}
    local levelshopnum = 0
    for k,v in pairs( CPPGameLib.GetConfig("LevelShop",nil,nil,true) ) do
    	levelshopnum = levelshopnum + 1
    	table.insert( cnf , v )
    end
   
    table.sort( cnf, function ( a , b )
    	return a.level < b.level
    end )
    self.__chapters_scrollview:SetIsNotUnconditionalSelected(true)
    self.__chapters_scrollview:SetChangeData(1,0.38,0.4)
    self.__chapters_scrollview:SetNecessaryData(levelshopnum,3,true,true)
    self.__chapters_scrollview:SetDataList(cnf)    
    -- self.__chapters_scrollview:MoveItemToScreenCenterByIndex(1)
    -- self.__chapters_scrollview:SelectItem(1)

    self.data = cnf[1]
end

function Modules.TemplLevelShop:OnButtonOk()

end

--设置下半部分界面内容
function Modules.TemplLevelShop:OnPitch( data )
	print("xy" , "TemplLevelShop:OnPitch" )
	self.data = data
	self.pitch:__SetShowUI(self.data)
	self.__chapters_scrollview:ItemDataIter(function(item_index, item_data)
												local item = self.__chapters_scrollview:GetItem(item_index)
												if item then
													item:SetRed(item_data)
												end
											end)
end

function Modules.TemplLevelShop:OnShow( data )
	local index = 1
	local data = self.data
	local level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)

	self.__chapters_scrollview:ItemDataIter(function(item_index, item_data)
											    if level >= item_data.level then
											    	index = item_index
											    	data = item_data
											    end
											end)

	self.__chapters_scrollview:MoveItemToScreenCenterByIndex(index)
	self.__chapters_scrollview:SelectItem(index)

	self.pitch:__SetShowUI(data)
end



Modules = Modules or {}

--等级礼包组件
Modules.TemplLevelShopItem = Modules.TemplLevelShopItem or BaseClass(GUI.ScrollItem)

function Modules.TemplLevelShopItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplLevelShopItem:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(300,215)
	self.layout:SetAnchorPoint(0,0)
    return self.layout
end

function Modules.TemplLevelShopItem:__delete()
end

function Modules.TemplLevelShopItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)

	--背景图
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY, "sp_activity_function_open")
	local bg = Util:Sprite( sp )
	bg:SetAnchorPoint( 0.5 , 0.5 )
	self.layout:AddChild( bg )
	PosTool.Center( bg )

	--战队等级
	local activity_squadronLevel = Util:Label( CPPGameLib.GetString( "activity_squadronLevel" , data.level ) )
	activity_squadronLevel:SetAnchorPoint( 0.5 , 1 )
	activity_squadronLevel:SetPosition( bg:GetContentWidth()/2 , bg:GetContentHeight() - 10 )
	bg:AddChild( activity_squadronLevel )
	
	local xtcnf = CPPGameLib.GetConfig("SystemEntry", data.system_type )
	if not xtcnf then return end
	local team_icon = nil
	if xtcnf.team_icon then
		team_icon = Resource.PathTool.GetSystemEntryIconPath(xtcnf.team_icon)
	else
		team_icon = Resource.PathTool.GetSystemEntryIconPath("btn_guild")
	end

	--图标
	local systemIcon = Util:Sprite( team_icon )
	systemIcon:SetAnchorPoint(0.5,0.5)
	systemIcon:SetScaleFactor(data.icon_zoom)
	bg:AddChild( systemIcon )
	PosTool.Center( systemIcon ,0,20)

	--名字
	local xtName = Util:Label( data.titlename , Macros.TypefaceSize.normal , Macros.Color.golden , nil , nil , Macros.Color.common_stroke )
	xtName:SetAnchorPoint(0.5,0.5)
	xtName:SetPosition( bg:GetContentWidth()/2 , bg:GetContentHeight()/2 - 20 )
	bg:AddChild( xtName )

	--描述
	local describe = Util:Name( data.text , Macros.TypefaceSize.normal )
	describe:SetAnchorPoint(0.5,0.5)
	describe:SetPosition( bg:GetContentWidth()/2 , 63 )
	bg:AddChild( describe )

	if data.level <= CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL) then
		--前往
		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY, "btn_activity_go",true)
		local btn_activity_go = Util:Button( sp , function ( ... )
			CallModuleFunc(xtcnf.module_type, "OpenView", xtcnf.view_type or xtcnf.module_type )
		end)
		btn_activity_go:SetAnchorPoint( 0.5,0.5 )
		btn_activity_go:SetPosition( bg:GetContentWidth()/2 , 30 )
		bg:AddChild(btn_activity_go)
	else
		local btn_activity_go = Util:Label(CPPGameLib.GetString("activity_insufficient") , Macros.TypefaceSize.normal , Macros.Color.red )
		btn_activity_go:SetAnchorPoint( 0.5,0.5 )
		btn_activity_go:SetPosition( bg:GetContentWidth()/2 , 30 )
		bg:AddChild(btn_activity_go)	
	end

	--遮罩容器
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY, "sp_activity_function_shield")
	self.__shield_sp = Util:Sprite( sp )
	self.__shield_sp:SetAnchorPoint( 0.5 , 0.5 )
	self.layout:AddChild(self.__shield_sp,1)
	PosTool.Center(self.__shield_sp)
end

function Modules.TemplLevelShopItem:IsTouchEnable()
	return true
end

function Modules.TemplLevelShopItem:GetChangeWidget()
	return self.__shield_sp
end

function Modules.TemplLevelShopItem:SetRed( data )
	if self.icon_red_point then
		self.icon_red_point:RemoveSelf(true)
		self.icon_red_point = nil
	end

	local endTime = CallModuleFunc(ModuleType.ACTIVITY, "LevelShopToLevelGetEndTime" , data.level )
	local serverTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")

    local buy = false
    for i,v in ipairs(data.goods_id_list) do
		local num = CallModuleFunc(ModuleType.ACTIVITY, "LevelShopToidGetNum" , v )
		local con = CPPGameLib.GetConfig("LevelShopGoods", v )
		if not con then return end
		if con.buy_num_max - num ~= 0 or not num then
			buy = true
		end
    end

	if endTime and serverTime < endTime and buy == true then
      local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
      self.icon_red_point = Util:Sprite(icon_red_point)
      self.icon_red_point:SetPosition( self.layout:GetContentWidth() - 112  , self.layout:GetContentHeight() - 67 )
      self.layout:AddChild( self.icon_red_point , 20 )
	end
end

--礼包商品内容
Modules = Modules or {}

--等级礼包组件
Modules.TemplCommodity = Modules.TemplCommodity or BaseClass(GUI.Template)

function Modules.TemplCommodity:__init( parent_node )
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self.parent_node = parent_node
    self:InitTemplate( nil , parent_node )  

    self:BindGlobalEvent(Macros.Event.ActivityModule.ACTIVITY_SHOP_RECORD,function (_, data)
        self:UpBuyLst( )
    end)
    
    self:__ShowUI()
end

function Modules.TemplCommodity:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout( self.parent_node:GetContentWidth() , self.parent_node:GetContentHeight() )
    return self.layout
end

function Modules.TemplCommodity:__delete()
    if self.lst_list then
        self.lst_list:DeleteMe()
        self.lst_list = nil
    end

    if self.insufficient then
    	self.insufficient:RemoveSelf(true)
    	self.insufficient = nil 
    end
end

function Modules.TemplCommodity:__ShowUI()
	local params =  
	{
	    item_class = Modules.TemplCommodityItem,
	    item_width = 220.00,
	    item_height = 264.00 ,
	    horizon = true ,
	    row = 1,
	    col = 3,
	    view_width = self.layout:GetContentWidth() - 5  ,
	}
	-- 创建参数
	self.lst_list = GUI.ScrollView.New(params)
	self:AddChild(self.lst_list:GetNode())
	PosTool.Center(self.lst_list:GetNode() , 0 , -15 )

	--升级提示
	self.insufficient = Util:Layout( self.parent_node:GetContentWidth() , self.parent_node:GetContentHeight() )
	self.insufficient:SetAnchorPoint(0.5,0.5)
	self.parent_node:AddChild( self.insufficient )
	PosTool.Center( self.insufficient )

	local panel_heroMode = LogicGUI.Model.New(	{
												info_id = 31341 , 
												parent = self.insufficient ,
												dir = MathTool.HORZ_RIGHT
											})
	panel_heroMode:SetScaleFactor(1.1)
	panel_heroMode:SetPosition( self.insufficient:GetContentWidth()/2 - 45  , self.insufficient:GetContentHeight()/2 - 115 )

	local dialogue = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY, "dialogue") )
	dialogue:SetFlipX(true)
	dialogue:SetAnchorPoint(0.5,0.5)
	dialogue:SetScaleFactor(0.6)
	dialogue:SetPosition( panel_heroMode:GetNode():GetPositionX() + 155  , panel_heroMode:GetNode():GetPositionY() + 180 )
	self.insufficient:AddChild( dialogue )

	self.tips = Util:Label( 
								CPPGameLib.GetString("activity_upTips", " " , " " ) , 
								Macros.TypefaceSize.normal , Macros.Color.content , 
								(dialogue:GetContentWidth()*0.6) - 20 
							)
	self.tips:SetPosition( dialogue:GetPositionX() , dialogue:GetPositionY() )
	self.insufficient:AddChild(self.tips)

	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY, "btn_activity_goUp",true)
	self.activity_goUp = Util:Button( sp , function ( ... )
		local chapter_index,gamelevel_index = CallModuleFunc(ModuleType.DUNGEON,"GetCurDungeonLatestChapterAndGameLevelByType",Macros.Game.FBType.MAIN_DUNGEON) 
		GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.DUNGEON_GAMELEVEL,{chapter_index,gamelevel_index})
	end )
	self.activity_goUp:SetAnchorPoint(0.5,0.5)
	self.activity_goUp:SetPosition( dialogue:GetPositionX() , dialogue:GetPositionY() - 100  )
	self.insufficient:AddChild( self.activity_goUp )

	--当前等级礼包已过期
	self.activity_pastDue = Util:Name( CPPGameLib.GetString("activity_pastDue") )
	self.activity_pastDue:SetAnchorPoint(0.5 , 0.5)
	self.layout:AddChild( self.activity_pastDue )
	PosTool.Center( self.activity_pastDue )

	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_page_turn")

	self.Button_left = Util:Sprite(sp)
	self.Button_left:SetAnchorPoint(0.5,0.5)
	self.Button_left:SetPosition( 0 , self.layout:GetContentHeight()/2 )
	self.layout:AddChild(self.Button_left)

	self.Button_right = Util:Sprite(sp)
	self.Button_right:SetFlipX(true)
	self.Button_right:SetAnchorPoint(0.5,0.5)
	self.Button_right:SetPosition( self.layout:GetContentWidth() , self.layout:GetContentHeight()/2 )
	self.layout:AddChild(self.Button_right)
end

function Modules.TemplCommodity:__SetShowUI(data)

	self.lst_list:SetVisible(false)
	self.insufficient:SetVisible(false)
	self.activity_pastDue:SetVisible(false)
	self.Button_left:SetVisible(false)
	self.Button_right:SetVisible(false)
	self.activity_goUp:SetVisible(false)

	if not data then return end 

	local level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	local endTime = CallModuleFunc(ModuleType.ACTIVITY, "LevelShopToLevelGetEndTime" , data.level  )
	local serverTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")

	if data.level > level then

		self.insufficient:SetVisible(true)
		self.activity_goUp:SetVisible(true)
		local xtcnf = CPPGameLib.GetConfig("SystemEntry", data.system_type )
		if not xtcnf then return end
		self.tips:SetString( CPPGameLib.GetString("activity_upTips" , data.level , xtcnf.name ) )

	elseif endTime and serverTime >= endTime then

		self.activity_pastDue:SetVisible(true)

	else
		self:CountDown( data )
		self.lst_list:SetVisible(true)

		local lst = {}
		for i,v in ipairs(data.goods_id_list ) do
			local cnf = CPPGameLib.GetConfig("LevelShopGoods", v )
			if not cnf then return end
			table.insert( lst , { lst = cnf , level = data.level } )
		end

		if #lst > 3 then
			self.Button_left:SetVisible(true)
			self.Button_right:SetVisible(true)
		end
		self.lst_list:SetDataList( lst )
	end
end

--刷新购买
function Modules.TemplCommodity:UpBuyLst()
	self.lst_list:ItemDataIter(function(i, item_data)
		local item = self.lst_list:GetItem(i)
		if item then
			item:NumDispose(item_data)
		end
	end)
end

--倒计时
function Modules.TemplCommodity:CountDown(data)
	local endTime = CallModuleFunc(ModuleType.ACTIVITY, "LevelShopToLevelGetEndTime" , data.level )
	local serverTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	CPPActionManager.StopAllActions(self.layout)

	if self.countDownLayer then
		self.countDownLayer:RemoveSelf(true)
		self.countDownLayer = nil
	end

	if endTime and endTime > serverTime then

	    local func = nil
	    local function moveAnimate( ... )
	    	local serverTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
			if endTime - serverTime <= 0 then
				CPPActionManager.StopAllActions(self.layout)
				CallModuleFunc(ModuleType.ACTIVITY, "LevelShopDataReq")
			else
				if self.countDownLayer then
					self.countDownLayer:RemoveSelf(true)
					self.countDownLayer = nil
				end
				self.countDownLayer = Util:Layout( 0 , 0 )
				self.countDownLayer:SetAnchorPoint(0.5,0.5)
				self.countDownLayer:SetPosition( self.lst_list:GetNode():GetContentWidth()/2 , 150 )
				self.lst_list:AddChild( self.countDownLayer )

				--倒计时
				local countDown = Util:Label( 
												TimeTool.TransTimeStamp("HH:MM:SS", endTime - serverTime ) , 
												Macros.TypefaceSize.normal , 
												Macros.Color.golden , nil , nil , 
												Macros.Color.common_stroke 
											)
				countDown:SetAnchorPoint( 0 , 0.5 )
				self.countDownLayer:AddChild( countDown )

				--倒计时
				local countDownText = Util:Name( CPPGameLib.GetString("activity_countDownTips") , Macros.TypefaceSize.normal )
				self.countDownLayer:AddChild( countDownText )
				self.countDownLayer:SetContentSize( countDown:GetContentWidth() + countDownText:GetContentWidth() , countDown:GetContentHeight()  )

				countDown:SetPosition( 0 , self.countDownLayer:GetContentHeight()/2 )
				PosTool.RightTo( countDown , countDownText , 5 )
	        	CPPActionManager.DelayTo(self.layout,1,func)
			end

	    end
	    func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
	    moveAnimate()
	end
end


--等级礼包商品
Modules = Modules or {}
Modules.TemplCommodityItem = Modules.TemplCommodityItem or BaseClass(GUI.ScrollItem)

function Modules.TemplCommodityItem:__init()
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_ACTIVITY_LEVEL_ITEM)   
	self:__Getwidget()
end

function Modules.TemplCommodityItem:__delete()
end

function Modules.TemplCommodityItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	self:__SetShowUI( data.lst )
end

function Modules.TemplCommodityItem:__Getwidget()
	--居中层
	self.Image_item = self:GetWidget("Image_item")
	--名字
	self.Text_name = self:GetWidget("Text_name")
	--图标
	self.Panel_rankIcon = self:GetWidget("Panel_rankIcon")
	--价格背景
	self.Image_numBg = self:GetWidget("Image_numBg")
	--购买按钮
	self.Button_buy = Util:WidgetButton( 
											self:GetWidget("Button_buy") , 
											CPPGameLib.GetString("shop_sure_Buy") , 
											CPPGameLib.Handler(self,self.OnButtonBuy), 
											nil, nil , Macros.Color.button_yellow )
	--购买次数
	self.Text_num = self:GetWidget("Text_num")
end

function Modules.TemplCommodityItem:__SetShowUI(data)
	local type_ , cnf = ObjIDTool.GetInfo(data.item_id)

	--名字
	local name = Util:Name( cnf.name , Macros.TypefaceSize.normal , cnf.init_quality or  cnf.quality  )
	self.Text_name:AddChild(name)
	PosTool.Center( name )

	--图标
	local icon = LogicGUI.IconLayer.New(true)
	icon:SetData( { id = data.item_id , num = data.item_num } )
	icon:SetTouchSwallow(false)
	self.Panel_rankIcon:AddChild( icon:GetNode() )
	PosTool.Center( icon )

	--货币图标
	local currency = Util:MoneyControl( data.cost_type_list[1] , 0.85 )
	currency:SetAnchorPoint(0.5,0.5)
	currency:SetPosition( 0 , self.Image_numBg:GetContentHeight()/2 )
	self.Image_numBg:AddChild(currency)

	--价格
	local price = Util:Label( data.cost_value_list[1] , Macros.TypefaceSize.normal , Macros.Color.lilyWhite )
	price:SetAnchorPoint(0.5,0.5)
	self.Image_numBg:AddChild(price)
	PosTool.Center( price )

    local discount = LogicGUI.LconDiscount.New( data.sale )
    discount:SetAnchorPoint( 0.5 , 0.5 )
    discount:SetPosition( 20 , icon:GetNode():GetContentHeight() )
    icon:AddChild( discount:GetNode() , 50 )


	self:NumDispose( data )
end

function Modules.TemplCommodityItem:OnButtonBuy(data)
	if self.__data.lst.cost_value_list[1] > CallModuleFunc(ModuleType.ROLE,"GetPropValue" , self.__data.lst.cost_type_list[1] ) then
		local txt = "currency_insufficient_"..self.__data.lst.cost_type_list[1]
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString(txt) )
	else
		local info = {}
		info.type = Macros.Game.SHOP.LEVEL_GIFTBAG
		info.param1 = self.__data.lst.goods_id
		info.param2 = 1
		info.param3 = self.__data.level
		CallModuleFunc(ModuleType.ACTIVITY, "LevelShopBuyReq" , info )
	end
end

function Modules.TemplCommodityItem:NumDispose( data )
	data = data.lst or data

	if data and data.buy_num_max == 0 then
		return 
	end

	local num = CallModuleFunc(ModuleType.ACTIVITY, "LevelShopToidGetNum" , data.goods_id )

	if data.buy_num_max - num <= 0 then

		if not self.buy then
			self.buy = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY, "buy" ,true) )
			self.buy:SetAnchorPoint(0.5,0.5)
			self.Image_item :AddChild(self.buy)
			PosTool.Center( self.buy , 50 , -80 )
		end
		if self.num then
			self.num:SetString(" ")
		end
		self.Button_buy:SetEnabled(false)

	else

		if not self.num then
			self.num = Util:Label( " "  , Macros.TypefaceSize.normal , Macros.Color.red )
			self.num:SetAnchorPoint(0.5,0.5)
			self.Text_num:AddChild(self.num)
			PosTool.Center( self.num )
		end
		self.num:SetString( CPPGameLib.GetString("shop_can_buymax" , data.buy_num_max - num ) )
	end

end