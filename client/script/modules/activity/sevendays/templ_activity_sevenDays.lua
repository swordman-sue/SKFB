--
-- @brief 七天活动子界面
-- @author: yjg
-- @date: 2017年1月17日15:27:17
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplActivitySevenDays = LogicGUI.TemplActivitySevenDays or BaseClass(GUI.Template)

function LogicGUI.TemplActivitySevenDays:__init( lstBg , bg )

    self.bg = bg
    self.lstBg = lstBg
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate( nil , self.bg )

	self:__ShowUI()

    self:BindGlobalEvent(Macros.Event.ActivityModule.ACTIVITY_SEVENDAY_GET, function ( _ , id )
   		self:UpLst(id)
    end)

    self:BindGlobalEvent(Macros.Event.ActivityModule.ACTIVITY_SEVENDAY_BUY, function ( _ , id , id_ )
    	self:UpLst(id,id_)
    end)

    self:BindGlobalEvent(Macros.Event.ActivityModule.ACTIVITY_SEVENDAY_CHOOSE_BUY, function ( _ , id , id_ )
    	self.semivalence:__SetShowUI( self.lst[self.index][1] )
    end)

	--红点更新事件
	self:BindGlobalEvent(Macros.Event.RedModule.RED_UPDATE, function(_, system_id)
		--更新阵容红点
		if system_id == Macros.Game.SystemID.HERO_LINEUP then
			self:SetRed()
		end
	end)    
end
  
function LogicGUI.TemplActivitySevenDays:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(self.bg:GetContentWidth() , self.bg:GetContentHeight() )
	self.layout:SetAnchorPoint(0,0)
    return self.layout
end

function LogicGUI.TemplActivitySevenDays:__delete()
  	if self.lst_list then
  		self.lst_list:DeleteMe()
  		self.lst_list = nil
  	end
  	if self.catalog then
  		self.catalog:DeleteMe()
  		self.catalog = nil
  	end

  	if self.semivalence then
  		self.semivalence:DeleteMe()
  		self.semivalence = nil
  	end
end


function LogicGUI.TemplActivitySevenDays:__ShowUI()
    -- --半价
    self.semivalence = Modules.TemplSemivalenceItem.New( self.bg )
    self.semivalence:SetVisible(false)

   --图标列表
    local params =  
    {
        item_class = Modules.TemplActivitySevenDaysCatalogItem,
        item_width = 118 ,
        item_height = 100 ,
        row = 1,
        col = 5,
        view_width = self.lstBg:GetContentWidth() ,
        horizon = true ,
        touchable = true,
        touch_callback = function ( index , item )
		    self.catalog:ItemDataIter(function(i, item_data)
		        local item = self.catalog:GetItem(i)
		        if item then
		        	item:SetNoType()
		        end
		    end)
	        local item = self.catalog:GetItem(index)
	        if item then
	        	item:SetOnType()
	        end
	        self:OnYeMei(index)
        end
    }
    -- 创建参数
    self.catalog = GUI.ScrollView.New(params)
    self.catalog:SetAnchorPoint( 0 , 1 )
    self.catalog:SetPosition( 0 , self.lstBg:GetContentHeight()/2 )
    self.lstBg:AddChild(self.catalog:GetNode())
    self:OnYeMei(1) 

   --图标列表
    local params =  
    {
        item_class = Modules.TemplActivitySevenDaysItem,
        item_width = self.bg:GetContentWidth() ,
        item_height = 135 ,
        row = 3,
        col = 1,
        item_space_r = 5 ,
        -- view_width = self.Image_ItemBg:GetContentWidth(),
        view_height = self.bg:GetContentHeight() - 10 ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.lst_list:SetPosition(self.bg:GetContentWidth()/2 , self.bg:GetContentHeight()/2 )
    self.bg:AddChild(self.lst_list:GetNode())

end

function LogicGUI.TemplActivitySevenDays:OnYeMei(index)
	self.index = index
	self.semivalence:SetVisible(false)

	if self.lst then
		self.lst_list:SetVisible(false)
		if self.lst[self.index][1].logic_type == Macros.Game.ACTIVITY_SEVEN_DAYS_CHARS_ADDEMBLY.HALF_PAY then
			self.semivalence:SetVisible(true)
			self.semivalence:__SetShowUI( self.lst[self.index][1] )
		else
			local t1 = {}
			if self.lst[self.index][1].show_type == Macros.Game.ACTIVITY_SEVEN_DAYS_SHOP.DISCOUNT or
				self.lst[self.index][1].show_type == Macros.Game.ACTIVITY_SEVEN_DAYS_SHOP.ROD then
				--购买次数
				local t2 = {}
				local t3 = {}
				for i , v in ipairs( self.lst[self.index] ) do
					local lst = CallModuleFunc(ModuleType.SEVENDAYS, "SevenDayShopData" , v.x )
					local time = CallModuleFunc(ModuleType.SEVENDAYS, "GetShopRecord" , lst[1].goods_id )
					if lst[1].buy_num_max - time <= 0 then
						table.insert( t3 , v )
					else
						table.insert( t2 , v )
					end
				end
				for i,v in ipairs(t3) do
					table.insert( t2 , v )
				end
				t1 = t2
			else
				t1 = self.lst[self.index]
			end

			self.lst_list:SetDataList( t1 )
			self.lst_list:SetVisible(true)
		end	
	end
end

function LogicGUI.TemplActivitySevenDays:__SetShowUI( date , index )

	self.red_index = index 
	self.target_list = date.target_list 
	
	self.lst = CallModuleFunc(ModuleType.SEVENDAYS, "SevenDayTargetDataCnf" , date.target_list )

    local t1 = {}
	for i,v in ipairs(self.lst) do
		table.insert( t1 , {name = CPPGameLib.GetString( "activity_dateValueType" .. v[1].show_type ) } )
	end
	
	self.catalog:SetDataList( t1 )
	self.catalog:SetPosition( (self.lstBg:GetContentWidth()/2) - (self.catalog:GetNode():GetContentWidth()/2) , self.lstBg:GetContentHeight()/2 )

	self.semivalence:SetVisible(false)
	self.lst_list:SetVisible(false)

	if self.lst[self.index][1].logic_type == Macros.Game.ACTIVITY_SEVEN_DAYS_CHARS_ADDEMBLY.HALF_PAY then
		self.semivalence:SetVisible(true)
		self.semivalence:__SetShowUI( self.lst[self.index][1] )
	else
		local t1 = {}
		if self.lst[self.index][1].show_type == Macros.Game.ACTIVITY_SEVEN_DAYS_SHOP.DISCOUNT or
			self.lst[self.index][1].show_type == Macros.Game.ACTIVITY_SEVEN_DAYS_SHOP.ROD then
			--购买次数
			local t2 = {}
			local t3 = {}
			for i , v in ipairs( self.lst[self.index] ) do
				local lst = CallModuleFunc(ModuleType.SEVENDAYS, "SevenDayShopData" , v.x )
				local time = CallModuleFunc(ModuleType.SEVENDAYS, "GetShopRecord" , lst[1].goods_id )
				if lst[1].buy_num_max - time <= 0 then
					table.insert( t3 , v )
				else
					table.insert( t2 , v )
				end
			end
			for i,v in ipairs(t3) do
				table.insert( t2 , v )
			end
			t1 = t2
		else
			t1 = self.lst[self.index]
		end

		self.lst_list:SetDataList( t1 )
		self.lst_list:SetVisible(true)
	end	 

	local num = self.index or 1
    self.catalog:ItemDataIter(function(i, item_data)
		local item = self.catalog:GetItem(i)
	    if item then
	    	if i == num then
		    	item:SetOnType()
		    else
		    	item:SetNoType()
	    	end
	    end
    end)
end

function LogicGUI.TemplActivitySevenDays:UpLst( id ,id_ )
	self.lst = CallModuleFunc(ModuleType.SEVENDAYS, "SevenDayTargetDataCnf" , self.target_list )
	self.lst_list:SetDataList( self.lst[self.index] )
end

function LogicGUI.TemplActivitySevenDays:SetRed()
	local cnf = CallModuleFunc(ModuleType.SEVENDAYS, "GetRedDotInfoByDay" , self.red_index )
	print("skfb", "fffffffffffffffffffffffffffffssssssssssssssssssssssssssssssssssssss")

    self.catalog:ItemIter(function(i, item, item_data)
		local num = 0
		local lst = cnf and cnf[i] or nil
		if lst then
			for _, v in ipairs(lst) do
				if v == "true" then
					num = num + 1
					break
				end
			end
		end
       	item:SetRed(num)		
    end)

end


Modules = Modules or {}
Modules.TemplActivitySevenDaysCatalogItem = Modules.TemplActivitySevenDaysCatalogItem or BaseClass(GUI.ScrollItem)

function Modules.TemplActivitySevenDaysCatalogItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplActivitySevenDaysCatalogItem:__CreateWidgetFromLuaCode()	
    self.layout = Util:Layout(118 , 75)
    self.layout:SetTouchEnabled(false)
    return self.layout
end

function Modules.TemplActivitySevenDaysCatalogItem:__delete()
end

function Modules.TemplActivitySevenDaysCatalogItem:SetData( data )
    GUI.ScrollItem.SetData(self, data)

    self:__ShowUI(data)
end

function Modules.TemplActivitySevenDaysCatalogItem:__ShowUI( data )
    local btn_cbox_seven_no = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.SEVEN_DAYS, "btn_no")
    
    self.btn = Util:Button( btn_cbox_seven_no )
    self.btn:SetScaleFactor( 0.95 )
    self.btn:SetAnchorPoint( 0 , 1 )
    self.btn:SetPosition( 0 , self.layout :GetContentHeight())
    self.layout :AddChild( self.btn )

    self.text = Util:Label( " " , Macros.TypefaceSize.popup , Macros.Color.btn , nil , nil , Macros.Color.button_red )
    self.text:SetAnchorPoint(0.5,0.5)
    self.btn:AddChild( self.text )
    PosTool.Center( self.text , 0 , 10 )

    local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
    self.icon_red_point = Util:Sprite(icon_red_point)
    self.icon_red_point:SetPosition(self.btn:GetContentWidth() - 10 , self.btn:GetContentHeight() - 10 )
    self.btn:AddChild(self.icon_red_point , 20)	
    self.icon_red_point:SetVisible(false)

    self:SetLabText( data.name )

end

function Modules.TemplActivitySevenDaysCatalogItem:SetNoType(  )
	local btn_cbox_seven_no = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.SEVEN_DAYS, "btn_no")
	self.btn:SetNormalImage( btn_cbox_seven_no )
	self.text:SetStrokeColor(unpack(Macros.Color.button_red))
end

function Modules.TemplActivitySevenDaysCatalogItem:SetOnType(  )
	local btn_btn_activity_on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.SEVEN_DAYS, "btn_on")
	self.btn:SetNormalImage( btn_btn_activity_on )
	self.text:SetStrokeColor(unpack(Macros.Color.button_yellow))
end

function Modules.TemplActivitySevenDaysCatalogItem:SetLabText( text )
	self.text:SetString( text )
end

function Modules.TemplActivitySevenDaysCatalogItem:SetRed( num )
	if num ~= 0 then
		self.icon_red_point:SetVisible(true)
	else
		self.icon_red_point:SetVisible(false)
	end
	self.icon_red_point:SetPosition(self.btn:GetContentWidth() - 10 , self.btn:GetContentHeight() - 10)
end

function Modules.TemplActivitySevenDaysCatalogItem:GetTouchNode()
    return self.btn
end


Modules = Modules or {}
Modules.TemplActivitySevenDaysItem = Modules.TemplActivitySevenDaysItem or BaseClass(GUI.ScrollItem)

function Modules.TemplActivitySevenDaysItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplActivitySevenDaysItem:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(677 , 134)
    self.layout:SetTouchEnabled(false)
    return self.layout
end

function Modules.TemplActivitySevenDaysItem:__delete()
end

function Modules.TemplActivitySevenDaysItem:SetData( data )
    GUI.ScrollItem.SetData(self, data)

    self:__ShowUI(data)
end

function Modules.TemplActivitySevenDaysItem:__ShowUI(data)
	--背景
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.SEVEN_DAYS, "sp_pack_bg")
	self.item = Util:Sprite9( sp )
	self.item:SetContentSize( self.layout:GetContentWidth() - 10 , self.layout:GetContentHeight() )
	self.item:SetPosition( self.layout:GetContentWidth()/2 , self.layout:GetContentHeight()/2 )
	self.layout:AddChild(self.item)

	--名字
	self.name = Util:Label( data.text , nil , Macros.Color.keypoint )
	self.name:SetAnchorPoint( 0 , 0.5)
	self.name:SetPosition( 10 , self.item:GetContentHeight() - (self.name:GetContentHeight()/2) - 10 )
	self.item:AddChild(self.name)

	--icon背景
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.SEVEN_DAYS, "sp_pack_frame")
	self.iconBg = Util:Sprite9( sp )
	self.iconBg:SetContentSize( 420 , self.layout:GetContentHeight() - 50 )
	self.iconBg:SetAnchorPoint( 0 , 0 )
	self.iconBg:SetPosition( 10 , 10 )
	self.layout:AddChild( self.iconBg )	

	--如果是购买的
	if data.show_type == Macros.Game.ACTIVITY_SEVEN_DAYS_SHOP.DISCOUNT or 
		data.show_type == Macros.Game.ACTIVITY_SEVEN_DAYS_SHOP.ROD then
		self:SetShop(data)
	else
		self:SetDraw(data)
	end
end

--购买条目
function Modules.TemplActivitySevenDaysItem:SetShop(data)
	local lst = CallModuleFunc(ModuleType.SEVENDAYS, "SevenDayShopData" , data.x )
	local x = 5
	local icon_layer = {}

	--消费
	local consume = 0
	if lst[1].cost_value_list then
		consume = lst[1].cost_value_list[1]
	end

	for i,v in ipairs( lst ) do
		if icon_layer[i - 1] then
			x = icon_layer[i - 1] :GetPositionX() + icon_layer[ i-1 ]:GetNode():GetContentWidth() + 15
		end
	    icon_layer[i] = LogicGUI.IconLayer.New(true)
	    icon_layer[i]:SetData({id = v.item_id , num = v.item_num })
	    icon_layer[i]:SetAnchorPoint(0 , 0.5)
	    icon_layer[i]:SetPosition( x , self.iconBg:GetContentHeight()/2 )
	    self.iconBg:AddChild(icon_layer[i]:GetNode())
	    icon_layer[i]:SetTouchSwallow(false)

    	local effect_id , effect_con = self:__CreateEffect({res_name = "UI_banjia"}, icon_layer[i]:GetNode() ,999 )
		PosTool.Center(effect_con,0,2)
	end

	--原价
	local shop_before_price = Util:LabCenter({
   		{zi = CPPGameLib.GetString("shop_before_price"), dx = Macros.TypefaceSize.minimum , ys = Macros.Color.keypoint },
   		{zi = lst[1].par or "0" , dx = Macros.TypefaceSize.minimum , mb = true },
   		{tp = Util:GetMoneyIconImgData(lst[1].cost_type_list[1]) , dx = 0.8 , jg = 5 },
	})		
	shop_before_price:SetAnchorPoint(0.5 , 0.5)
	shop_before_price:SetPosition( self.iconBg:GetContentWidth()/2 , self.name:GetPositionY() - 11 )
	self.iconBg:AddChild(shop_before_price)

	--红线
	local node = Util:Sprite9(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "delete_line"))
	node:SetContentSize( shop_before_price:GetContentWidth() + 5 , 3 )
    node:SetAnchorPoint( 0.5,0.5 )
    shop_before_price:AddChild(node)	
    PosTool.Center(node)

	--现价
	local shop_after_price = Util:LabCenter({
   		{zi = CPPGameLib.GetString("shop_after_price"), dx = Macros.TypefaceSize.minimum , ys = Macros.Color.keypoint },
   		{zi = consume , dx = Macros.TypefaceSize.minimum , mb = true },
   		{tp = Util:GetMoneyIconImgData(lst[1].cost_type_list[1]) , dx = 0.8 , jg = 5 },
	})		
	shop_after_price:SetAnchorPoint(1 , 0.5)
	shop_after_price:SetPosition( self.iconBg:GetContentWidth() , self.name:GetPositionY() - 11 )
	self.iconBg:AddChild(shop_after_price)

	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen2")
	self.ensure  =  Util:Button( sp , 
								 CPPGameLib.Handler(self, self.OnBtnBuy) , 
								 CPPGameLib.GetString("shop_sure_Buy") , 
								 Macros.TypefaceSize.tab , 
								 nil , Macros.Color.button_yellow
								 )
	self.ensure:SetPosition( self.item:GetContentWidth()/2 + 220  , self.item:GetContentHeight()/2 )
	self.layout:AddChild(self.ensure) 

	--购买次数
	local time = CallModuleFunc(ModuleType.SEVENDAYS, "GetShopRecord" , lst[1].goods_id )
-- TemplActivitySevenDays
	--限购
	self.activity_astrict = Util:LabCenter({
   		{zi = CPPGameLib.GetString("activity_astrict"), dx = Macros.TypefaceSize.minimum , ys = Macros.Color.keypoint },
   		{zi = lst[1].buy_num_max - time .. "/" .. lst[1].buy_num_max , dx = Macros.TypefaceSize.minimum , mb = true },
	})		
	self.activity_astrict:SetAnchorPoint(0.5 , 0.5)
	self.activity_astrict:SetPosition( self.ensure:GetContentWidth()/2 , self.ensure:GetContentHeight() + 10 )
	self.ensure:AddChild(self.activity_astrict)

	if lst[1].buy_num_max - time == 0 then
		self.activity_astrict:SetVisible(false)

		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "buy_1" , true)
		self.ensure:SetNormalImage(sp)
		self.ensure:SetText(" ")
		Util:GreyButton( self.ensure )
	end

end


--领取条目
function Modules.TemplActivitySevenDaysItem:SetDraw(data)

	local x = 5
	local icon_layer = {}
	
	local sevenDayCnf = CPPGameLib.GetConfig("SevenDayTargetData", data.id)
	if not sevenDayCnf then return end
	for i,v in ipairs(sevenDayCnf.reward_item_id_list) do
		if icon_layer[i - 1] then
			x = icon_layer[i - 1] :GetPositionX() + icon_layer[ i-1 ]:GetNode():GetContentWidth() + 15
		end

	    icon_layer[i] = LogicGUI.IconLayer.New(true)
	    icon_layer[i]:SetData({id = v , num = sevenDayCnf.reward_item_num_list[i] })
	    icon_layer[i]:SetAnchorPoint( 0 , 0.5)
	    icon_layer[i]:SetPosition( x , self.iconBg:GetContentHeight()/2 )
	    icon_layer[i]:SetTouchSwallow(false)
	    self.iconBg:AddChild(icon_layer[i]:GetNode())
	end

	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen2")
	self.ensure =  Util:Button(
									sp,
									CPPGameLib.Handler(self, self.OnBtnDraw),
									CPPGameLib.GetString("manor_get"),
									Macros.TypefaceSize.tab, nil , 
									Macros.Color.button_yellow
									)
	self.ensure:SetPosition( self.item:GetContentWidth()/2 + 220 , self.item:GetContentHeight()/2 )
	self.layout:AddChild(self.ensure) 

	if data.is_get_reward ~= 0 then
		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "receive_icon" , true)
		self.ensure:SetNormalImage(sp)
		self.ensure:SetText(" ")
		self.ensure:SetTouchEnabled(false)
		if self.btnEffect_id then
			self:__DelEffect(self.btnEffect_id)
			self.btnEffect_id = nil
			self.BtnEffect_con = nil
		end
	else
		self.ensure:SetEnabled(true)
		self.btnEffect_id , self.BtnEffect_con = self:__CreateEffect({res_name = "ui_anniuzoubian2"},self.ensure,999)
		self.BtnEffect_con:SetPosition(self.ensure:GetContentWidth()/2 , self.ensure:GetContentHeight()/2 )
	end

	if data.is_asc == 0 then 
		--竞技场
		if data.logic_type == Macros.Game.ACTIVITY_SEVEN_DAYS_CHARS_ADDEMBLY.ATHLETICS then
			if data.value > data.x or data.value == 0 then
				if self.btnEffect_id then
					self:__DelEffect(self.btnEffect_id)
					self.btnEffect_id = nil
					self.BtnEffect_con = nil
				end
				Util:GreyButton( self.ensure )
			end
		end
	else
		if data.value < data.x then
			if self.btnEffect_id then
				self:__DelEffect(self.btnEffect_id)
				self.btnEffect_id = nil
				self.BtnEffect_con = nil
			end
			Util:GreyButton( self.ensure )
		end
	end

end

--购买回调
function Modules.TemplActivitySevenDaysItem:OnBtnBuy()
	local lst = CallModuleFunc(ModuleType.SEVENDAYS, "SevenDayShopData" , self.__data.x )
	local diamonds = CallModuleFunc(ModuleType.ROLE,"GetPropValue" , lst[1].cost_type_list[1] )
	if diamonds >= lst[1].cost_value_list[1] then
	    local info = {}
	    info.param1 = lst[1].goods_id
	    info.itemId = self.__data.x
	    info.itemId_ = self.__data.id
		CallModuleFunc(ModuleType.SEVENDAYS, "ShopBuyRequest" , info )
	else
		local txt = "currency_insufficient_"..lst[1].cost_type_list[1]
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString(txt) )
	end
end

--领取回调
function Modules.TemplActivitySevenDaysItem:OnBtnDraw()
	local info = {}
	info.target_id = self.__data.id
	CallModuleFunc(ModuleType.SEVENDAYS, "SevenDayGetRewardReq",info)
end


--单独
Modules = Modules or {}

Modules.TemplSemivalenceItem = Modules.TemplSemivalenceItem or BaseClass(GUI.Template)

function Modules.TemplSemivalenceItem:__init( node )
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil , node)  
    self:SetData()
end

function Modules.TemplSemivalenceItem:__delete()
end

function Modules.TemplSemivalenceItem:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(677.00 , 452.00)
    return self.layout
end

function Modules.TemplSemivalenceItem:SetData()
  	self.semivalence = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.SEVEN_DAYS, "semivalence",true ))
  	self.semivalence:SetAnchorPoint(0.5,0.5)
  	self.semivalence:SetPosition(self.layout:GetContentWidth()/2 , self.layout:GetContentHeight() /2 )
  	self.layout:AddChild(self.semivalence)

  	self.semivalence_ = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.SEVEN_DAYS, "semivalence" ))
  	self.semivalence_:SetAnchorPoint(0.5,0.5)
  	self.semivalence_:SetPosition(self.layout:GetContentWidth()/2 , self.layout:GetContentHeight() /2 )
  	self.layout:AddChild(self.semivalence_)

  	--图标
	self.icon_layer = LogicGUI.IconLayer.New(true)
	self.icon_layer:SetData({id = 10000 , num = 0 })
	self.icon_layer:SetAnchorPoint( 0.5 , 0.5)
	self.icon_layer:SetPosition( self.semivalence_:GetContentWidth()/2 , self.semivalence_:GetContentHeight()/2 + 20 )
	self.icon_layer:SetTouchSwallow(false)
	self.semivalence_:AddChild(self.icon_layer:GetNode())  

	local effect_id , effect_con = self:__CreateEffect({res_name = "UI_banjia"}, self.icon_layer:GetNode() ,999 )
	PosTool.Center(effect_con,0,2)

	self.describe = Util:Label("" , nil , Macros.Color.content )
	self.describe:SetAnchorPoint( 0.5 , 0.5 )
	self.describe:SetPosition(  self.semivalence_:GetContentWidth()/2 , self.semivalence_:GetContentHeight()/2 - 40 )
	self.semivalence_:AddChild( self.describe )

	--名字
	self.name = Util:Name("0" , Macros.TypefaceSize.largish  )
	self.name:SetPosition( self.icon_layer:GetNode():GetPositionX() , self.icon_layer:GetNode():GetPositionY() + 85  )
	self.semivalence_:AddChild(self.name)  

	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen2")
	self.ensure =  Util:Button(sp,CPPGameLib.Handler(self, self.OnBtnDraw),CPPGameLib.GetString("shop_sure_Buy"),Macros.TypefaceSize.popup , nil , Macros.Color.button_yellow)
	self.ensure:SetPosition( self.semivalence_:GetContentWidth()/2 , self.semivalence_:GetContentHeight()/2 - 170)
	self.semivalence_:AddChild(self.ensure) 
end

function Modules.TemplSemivalenceItem:__SetShowUI( data ,type_ )
	self.data = data

    local lst = CallModuleFunc(ModuleType.SEVENDAYS, "SevenDayShopData" , data.x )
	-- 图标
	self.icon_layer:SetData( {id = lst[1].item_id , num = lst[1].item_num } )

	--名字
	local type_ , info = ObjIDTool.GetInfo(lst[1].item_id)
	self.name:SetString(info.name)
	self.name:SetColor(unpack(Macros.Quality[info.quality]))
	self.name:SetStrokeColor(unpack(Macros.Quality_Stroke[info.quality]))

	local lst = CallModuleFunc(ModuleType.SEVENDAYS, "SevenDayShopData" , data.x )

	if data.sale_text then
		self.describe:SetString( data.sale_text )
	else
		self.describe:SetString( " " )
	end

	--消费
	local consume = 0
	for i,v in ipairs( lst ) do
	    consume = consume + v.cost_value_list[1]
	end

	if self.shop_before_price then
		self.shop_before_price:RemoveSelf(true)
		self.shop_before_price = nil
	end

	if self.shop_after_price then
		self.shop_after_price:RemoveSelf(true)
		self.shop_after_price = nil
	end

	--原价
	self.shop_before_price = Util:LabCenter({
   		{zi = CPPGameLib.GetString("shop_before_price"), dx = Macros.TypefaceSize.largish , ys = Macros.Color.lilyWhite },
   		{zi = consume*2 , dx = Macros.TypefaceSize.largish },
   		{tp = Util:GetMoneyIconImgData(lst[1].cost_type_list[1]) , dx = 0.8 },
	})		
	self.shop_before_price:SetAnchorPoint( 1 , 0.5)
	self.shop_before_price:SetPosition( self.semivalence_:GetContentWidth()/2 - 20 , self.name:GetPositionY() - 185 )
	self.semivalence_:AddChild(self.shop_before_price)

	-- 红线
	local delete_line = Util:Sprite9(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "delete_line"))
	delete_line:SetContentSize( self.shop_before_price:GetContentWidth() + 5 , 3 )
    delete_line:SetAnchorPoint( 0.5,0.5 )
    self.shop_before_price:AddChild(delete_line)	
    PosTool.Center(delete_line)

	--现价
	self.shop_after_price = Util:LabCenter({
   		{zi = CPPGameLib.GetString("shop_after_price"), dx = Macros.TypefaceSize.largish , ys = Macros.Color.lilyWhite },
   		{zi = consume , dx = Macros.TypefaceSize.largish },
   		{tp = Util:GetMoneyIconImgData(lst[1].cost_type_list[1]) , dx = 0.8 },
	})		
	self.shop_after_price:SetAnchorPoint(0 , 0.5)
	self.shop_after_price:SetPosition( self.semivalence_:GetContentWidth()/2 + 20 , self.name:GetPositionY() - 185 )
	self.semivalence_:AddChild(self.shop_after_price)


	local lst = CallModuleFunc(ModuleType.SEVENDAYS, "SevenDayShopData" , self.data.x )

	--购买次数
	local time = CallModuleFunc(ModuleType.SEVENDAYS, "GetShopRecord" , lst[1].goods_id )
	if lst[1].buy_num_max == 0 then

	elseif time >= lst[1].buy_num_max then
		self.ensure:SetText( CPPGameLib.GetString("shop_albuy") )
		self.ensure:SetEnabled(false)
		Util:GreyButton( self.ensure )
	else
		self.ensure:SetText( CPPGameLib.GetString("shop_sure_Buy") )
		self.ensure:SetEnabled(true)	

		self.ensure:SetTextColor(unpack(Macros.Color.blinkcontent))
		local r, g, b, a = unpack(Macros.Color.button_yellow)
		self.ensure:SetOutline(r, g, b, a, 2)
	end
end

function Modules.TemplSemivalenceItem:OnBtnDraw( data )
	local lst = CallModuleFunc(ModuleType.SEVENDAYS, "SevenDayShopData" , self.data.x )
	local diamonds = CallModuleFunc(ModuleType.ROLE,"GetPropValue" , lst[1].cost_type_list[1] )
	if diamonds >= lst[1].cost_value_list[1] then
		local tips = LogicGUI.TemplAffirmTips.New( { 
													type = LogicGUI.TemplAffirmTipsType.YES_NO,
													content	= CPPGameLib.GetString("activity_affirmBuy"),
													confirm_text = CPPGameLib.GetString("common_confirm") ,
													confirm_func = function ( ... )
														local info = {}
														info.param1 = lst[1].goods_id
														info.itemId_ = self.data.id
														CallModuleFunc(ModuleType.SEVENDAYS, "ShopBuyRequest" , info )
													end,
													cancel_text = CPPGameLib.GetString("common_cancel") ,
													cancel_func = function ( ... )
														print("----2")
													end,
												} )
	else
		local txt = "currency_insufficient_"..lst[1].cost_type_list[1]
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString(txt) )
	end
end