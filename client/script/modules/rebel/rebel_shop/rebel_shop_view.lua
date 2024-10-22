--[[
%% @module: 战功商店界面
%% @author: yjg
%% @created: 2016年10月26日17:43:53
--]]


Modules = Modules or {} 

Modules.RebelShopView = Modules.RebelShopView or BaseClass(BaseView)

function Modules.RebelShopView:__init()
	self.__layout_name = "store2"	
	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
	self.__title_bar_params = 
	{
		title = CPPGameLib.GetString("rebel_shop"),
		show_diamond = true,
		show_gold = true,
		show_stamina = true,
		show_energy = true,
		close_func = function()
			self:CloseManual()
		end,
	}

	local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
  	local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no")
  	self.__tab_params = {
  		{sp_2,sp_1},
  		{	
			CPPGameLib.GetString("rebel_shop"),
		},
		10,
		CPPGameLib.Handler(self, self.OnYeMei),
		false	
	}

    self.__open_callback = function()   	
    	self:__OpenCallback()
    end
    self:__AddPreloadTemplate(GUI.TemplateRegistry.SHOP_COMMON_TOOLS)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.STORE)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.STORE,true)
end

function Modules.RebelShopView:__OpenCallback()  
	self:__Getwidget()
	self:__ShowUI()
end

function Modules.RebelShopView:__RegistAllEvents()
	self:BindGlobalEvent(Macros.Event.RoleModule.PROP_CHANGE,function (_, data)
		self:UpPossess()
	end)
	self:BindGlobalEvent(Macros.Event.PackModule.UPDATE_GOODS,function (_, data)
		self:UpPossess()
	end)	
end

function Modules.RebelShopView:__Dispose()

end

function Modules.RebelShopView:__Getwidget()
	-- --下框
	-- self.Image_3 = self:GetWidget("Image_3") 
	--居中层
	self.Panel_store = self:GetWidget("Panel_store") 
	--背景层
	self.Image_smallBg = self:GetWidget("Image_smallBg") 
	--左边按钮
	self.Image_back = self:GetWidget("Image_back") 
	local effect = nil
	self.effectid_1 ,effect = self:__CreateEffect({res_name = "UI_shangdianjiantou" } , self.Image_back , 999 )
	effect:SetAnchorPoint(0.5,0.5)
	effect:SetPosition( self.Image_back:GetContentWidth()/2 , self.Image_back:GetContentHeight()/2 )
	effect:SetScaleXFactor( -1 )
	
	--右边按钮
	self.Image_front = self:GetWidget("Image_front") 
    local effect = nil
    self.effectid_2 ,effect = self:__CreateEffect({res_name = "UI_shangdianjiantou" } , self.Image_front , 999 )
    effect:SetAnchorPoint(0.5,0.5)
    effect:SetPosition( self.Image_front:GetContentWidth()/2 , self.Image_front:GetContentHeight()/2 )
    
	--按钮下面的光
	self.Image_selectedtab2 = self:GetWidget("Image_selectedtab2")
	self.Image_selectedtab2:SetVisible(false)
	--充值按钮
	self.Button_money = self:GetWidget("Button_money") 
	self.Button_money:SetVisible(false)
	--列表容器
	self.Panel_tool = self:GetWidget("Panel_tool") 
	--翻页容器
	self.PageView_tool = self:GetWidget("PageView_tool") 
	--列表容器——2
	self.Panel_gift = self:GetWidget("Panel_gift") 
	self.Panel_gift:SetVisible(false)
	--列表容器——2
	self.PageView_gift = self:GetWidget("PageView_gift") 
	self.PageView_gift:SetVisible(false)
	--页签容器
	self.Panel_tag = self:GetWidget("Panel_tab") 
end

function Modules.RebelShopView:__ShowUI()
	--页签
  	self.__tab:On(1)
  	self.__tab:SetPosition(0 , 0)
  	self.__tab.btn_list[1]:SetTouchEnabled(false)
  	self.Panel_tag:AddChild(self.__tab:GetNode())

	--表  	
	local rebel_value = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo").rebel_value
	local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
	local t1 = {}

	local lst = {}
	for k , v in pairs( CPPGameLib.GetConfig("RebelShop", nil, nil , true ) ) do
		table.insert( lst , v )
	end

	table.sort( lst, function( a , b )
 		return a.sort_id < b.sort_id
	end )

	for i,v in ipairs( lst ) do
		local t2 = {}
		for i,vv in ipairs(v.cost_type_list) do
			table.insert( t2 , { type = vv , num = v.cost_value_list[i] } )
		end

		if v.cost_item_id_list then
			for j , vvv in ipairs( v.cost_item_id_list ) do
				table.insert( t2 , { type = vvv , num = v.cost_item_num_list[j] } )
			end
		end

		if v.need_role_level <= userDate.level then
			local info = {}
			info.item_id = v.item_id
			info.goods_id = i
			info.item_num = v.item_num
			info.cost = t2
			info.type = Macros.Game.SHOP.REBEL
			info.buy_num_max = math.floor(rebel_value / v.cost_value_list[1] )
			info.buy_type = 0
			table.insert(t1,info)
		end
	end
	-- 2016年10月27日16:20:53 大硕硕说不到等级不给看
	self:SetCell( Util:SegmentationTab(t1 , 6) )

	local possessTxt = Util:Label( CPPGameLib.GetString("rebel_own_work") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
	possessTxt:SetPosition( 10 , 30 )
	possessTxt:SetAnchorPoint(0,0.5)
	self.Image_smallBg:AddChild(possessTxt)

	--货币图标
	local diamond = Util:MoneyControl( Macros.Game.RolePropType.REBEL_VALUE , 0.8)
	PosTool.RightTo(possessTxt,diamond)
	self.Image_smallBg:AddChild(diamond)

	--拥有货币
	self.possess = Util:Label(userDate.rebel_value , Macros.TypefaceSize.normal , Macros.Color.content )
	PosTool.RightTo(diamond,self.possess)
	self.Image_smallBg:AddChild(self.possess)
end

function Modules.RebelShopView:UpPossess( ... )
	local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
	if self.possess then
		self.possess:SetString(userDate.rebel_value)
	end

	for i=1 , 100 do
		if self.packIcon[i] then
			local data = self.packIcon[i]:GetData()
		    local type,info = ObjIDTool.GetInfo(data.item_id)
			self.packIcon[i]:PackNum(type,info,data.item_id)
		end
	end
end

function Modules.RebelShopView:OnYeMei()

end

function Modules.RebelShopView:SetCell( data )
	self.packIcon = {}
	local index = 0
	for i,v in ipairs(data) do
		local layer = Util:Layout( self.PageView_tool:GetContentWidth() , self.PageView_tool:GetContentHeight() )

		local interval = layer:GetContentWidth() / 3 
		local xIndex = 0
		local y = layer:GetContentHeight() - 197 

		for i,vv in ipairs(v) do
			xIndex = xIndex + 1
			if xIndex > 3 then
				xIndex = 1
				y = 0
			end
			index = index + 1
			self.packIcon[index] = GUI.TemplRebelShopCell.New(vv)
			self.packIcon[index]:SetPosition( (xIndex-1) * interval , y)
			self.packIcon[index]:GetNode():SetOpacity(0)
			layer:AddChild( self.packIcon[index]:GetNode() )
		end
		self.PageView_tool:AddPage(layer)
	end
	self:ShowAction()
end

function  Modules.RebelShopView:ShowAction()
	local index = 0
	local func = nil

	local function moveAnimate( ... )
		index = index + 1
		if self.packIcon[index] then
			CPPActionManager.FadeIn(self.packIcon[index]:GetNode(),0.05,func)
		end
	end
	func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
	moveAnimate()
end


GUI = GUI or {}
GUI.TemplRebelShopCell = GUI.TemplRebelShopCell or BaseClass(GUI.Template)
function GUI.TemplRebelShopCell:__init(data)
	self:InitTemplate(GUI.TemplateRegistry.SHOP_COMMON_TOOLS)

	self.data = data
	self:__Getwidget()
	self:__ShowUI(data)
end

function GUI.TemplRebelShopCell:__AddToParent()

end

function GUI.TemplRebelShopCell:__Getwidget()
	--背景
    self.Image_2 = self:GetWidget("Image_2") 
    --名字
    self.Panel_name = self:GetWidget("Panel_name") 
    
    --图标
    self.Panel_head = self:GetWidget("Panel_head") 
    --原价
    self.Text_couldBuyTimes = self:GetWidget("Text_couldBuyTimes") 
    self.Text_couldBuyTimes:SetText(" ")
    --价格背景
    self.Image_coinBg = self:GetWidget("Image_coinBg") 
    self.Image_coinBg:SetVisible(false)

    --货币图标
    self.Image_coin = self:GetWidget("Image_coin") 
    self.Image_coin:SetVisible(false)

    --价钱
    self.Text_coinNum = self:GetWidget("Text_coinNum") 
    self.Text_coinNum:SetVisible(false)

    --VIP图片
    self.Image_vip = self:GetWidget("Image_vip") 
    self.Image_vip:SetVisible(false)
	    --vip字样
	    self.Text_vip = self:GetWidget("Text_vip") 
    --原价容器X2
    self.Panel_before = self:GetWidget("Panel_before") 
    self.Panel_before:SetVisible(false)
	    self.Text_before = self:GetWidget("Text_before") 
	    self.Text_beforeNum = self:GetWidget("Text_beforeNum") 
	    self.Image_beforeCoin = self:GetWidget("Image_beforeCoin") 
	    self.Image_55_0 = self:GetWidget("Image_55_0") 
    --价格容器X2
    self.Panel_after = self:GetWidget("Panel_after") 
    self.Panel_after:SetVisible(false)
	    self.Text_after = self:GetWidget("Text_after") 
	    self.Text_afterNum = self:GetWidget("Text_afterNum") 
	    self.Image_afterCoin = self:GetWidget("Image_afterCoin") 
	--已购买
    self.Image_HaveBuy = self:GetWidget("Image_HaveBuy") 
    self.Image_HaveBuy:SetVisible(false)
end

function GUI.TemplRebelShopCell:__ShowUI(data)
	--头像
	local icon = LogicGUI.IconLayer.New( false )
	self:GetWidget("Panel_head"):AddChild(icon:GetNode())
	PosTool.Center(icon)
	icon:SetData({id = data.item_id , num = data.item_num })

	--价格
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "short_price_bg")
	if #data.cost == 1 then	
		local layer = Util:Sprite(sp)
		layer:SetPosition( self.Image_2:GetContentWidth()/2 , self.Image_coinBg:GetPositionY() )
		self.Image_2:AddChild(layer)

		--图标
	    local consume = Util:MoneyControl(data.cost[1].type , 0.8)
	    consume:SetPosition(0 ,layer:GetContentHeight()/2)
	    consume:SetAnchorPoint(0,0.5)
	    layer:AddChild(consume)

		--数量
	    local consumeNum = Util:Label(data.cost[1].num , Macros.TypefaceSize.normal)
	    consumeNum:SetPosition( layer:GetContentWidth() - 10  ,layer:GetContentHeight()/2)
	    consumeNum:SetAnchorPoint(1,0.5)
	    layer:AddChild(consumeNum)
	else
		local interval = self.Image_2:GetContentWidth() / #data.cost
		local tp = Util:Sprite(sp)
		local layer = Util:Layout( tp:GetContentWidth() * #data.cost + 25  , tp:GetContentHeight() )
		layer:SetAnchorPoint( 0.5 , 0.5 )
		layer:SetPosition( self.Image_2:GetContentWidth()/2 , self.Image_coinBg:GetPositionY() )
		self.Image_2:AddChild(layer)

		local layout = {}
		for i,v in ipairs(data.cost) do
			layout[i] = Util:Sprite(sp)
			local x = 0
			if layout[i-1] then
				x = layout[i-1]:GetContentWidth() + 25 
			end

			layout[i]:SetPosition( x ,  layer:GetContentHeight()/2 )
			layout[i]:SetAnchorPoint( 0 , 0.5 )
			layer:AddChild(layout[i])

			local type_ , config = ObjIDTool.GetInfo( v.type )
			local sp = nil
			local consume = nil
			if type_ == Macros.Game.ObjType.ITEM then
				sp = Resource.PathTool.GetItemPath( config.res_id )
				consume = Util:Sprite( sp )
				consume:SetScaleFactor( 0.6 )
			else
				sp = Util:GetMoneyIconImgData( v.type )  
				consume = Util:Sprite( sp )
			end
		    consume:SetPosition(0  ,layout[i]:GetContentHeight()/2)
		    consume:SetAnchorPoint(0,0.5)
		    layout[i]:AddChild(consume)

			--数量
		    local consumeNum = Util:Label( v.num , Macros.TypefaceSize.normal )
		    consumeNum:SetPosition( layout[i]:GetContentWidth() -10 ,layout[i]:GetContentHeight()/2)
		    consumeNum:SetAnchorPoint(1,0.5)
		    layout[i]:AddChild(consumeNum)

		end
	end

    local type,info = ObjIDTool.GetInfo(data.item_id)
	self:PackNum(type,info,data.item_id)
	--名字
	local name = Util:Name(info.name , Macros.TypefaceSize.largish , info.quality or info.init_quality)
	name:SetPosition(self.Panel_name:GetContentWidth()/2  , self.Panel_name:GetContentHeight() /2 )
	name:SetAnchorPoint(0.5,0.5)
	self.Panel_name:AddChild(name)

	local btn = Util:Layout( self.Image_2:GetContentWidth() , self.Image_2:GetContentHeight() , function ( ... )
		self:Transition(data)
	end)
	btn:SetPosition(self.Image_2:GetContentWidth()/2 , self.Image_2:GetContentHeight()/2 )
	btn:SetAnchorPoint(0.5,0.5)
	self.Image_2:AddChild(btn)
end

function GUI.TemplRebelShopCell:Transition(data)
	GUITemplateMgr:CreateTemplateInstance( GUI.TemplateRegistry.SHOP_BUY_TIPVIEW , false , data )
end

--物品数量
function GUI.TemplRebelShopCell:PackNum(type,info,id)
    local num = 0
    local max = 0

    if type == Macros.Game.ObjType.ITEM then
      if info.sub_type == Macros.Game.NormalItemSubType.EQUIPDEBRIS then
   		
        num = CallModuleFunc(ModuleType.Pack, "GetNumByItemID", id )
        max = CPPGameLib.GetConfig("EquipmentCompose", info.target_id).item_num

      elseif info.sub_type == Macros.Game.NormalItemSubType.HERODEBRIS then

        num = CallModuleFunc(ModuleType.Pack, "GetNumByItemID", id )
        max = CPPGameLib.GetConfig("HeroCompound", info.target_id).item_num

      end
    end

    if self.tips then
    	self.tips:RemoveSelf(true)
    	self.tips = nil
    end

    if max ~= 0 then 
      self.tips = Util:Label( CPPGameLib.GetString("tower_amount", num,max) , Macros.TypefaceSize.normal , Macros.Color.content )
      self.tips:SetPosition(self.Text_couldBuyTimes:GetContentWidth()/2 , self.Text_couldBuyTimes:GetContentHeight()/2  )
      self.Text_couldBuyTimes:AddChild(self.tips)
    end

end

function GUI.TemplRebelShopCell:GetData( ... )
	return self.data
end