--[[
%% @module: 战功商店购买界面
%% @author: yjg
%% @created: 2016年11月28日20:12:39
--]]

  -- local t1 = {}
	-- table.insert(t1,{ 
	-- 					type_id = 代替货币的id
	-- 					type = 货币类型 
	-- 					price_list = 价格列表 不同个数不同价格那种
	-- 					haveBuyNum = 已经买了的个数
	--					num = 单价
	-- 				})

  -- local info = {}
  	--道具id
  -- info.item_id = data.object_id 
  	--商店类型
  -- info.type = Macros.Game.SHOP.TOWER
  	--商品购买id
  -- info.goods_id = data.id
  	--价格列表
  -- info.cost = t1
  	--最大
	-- info.max = 1
	--单个数量
	-- info.num = 1
	-- --标题
	-- info.title = CPPGameLib.GetString("special_exchange")
	-- --func
	-- info.func = function ( ... )
	--     -- body
	-- end

LogicGUI = LogicGUI or {}

LogicGUI.TemplShopBuy = LogicGUI.TemplShopBuy or BaseClass(GUI.Template)

function LogicGUI.TemplShopBuy:__init(data , func)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self.func = func
		self.data = data
		self:__Getwidget()
		self:__Settle(data)
		self:__ShowUI(data)
	end
	self:InitTemplate(GUI.TemplateRegistry.SHOP_BUY_TIPVIEW)
end

function LogicGUI.TemplShopBuy:__delete()
end

function LogicGUI.TemplShopBuy:__Getwidget()
	--大容器
    self.Panel_storeBuy = self:GetWidget("Panel_storeBuy") 
    --背景
    self.Image_1 = self:GetWidget("Image_1") 
    --标题
    self.Text_tile = self:GetWidget("Text_tile") 
    --关闭按钮
    self.Button_close = Util:WidgetButton(self:GetWidget("Button_close"),"", CPPGameLib.Handler(self,self.OnButtonClose))
    --描述
    self.Text_des = self:GetWidget("Text_des") 
    --已经拥有
    self.Text_have = self:GetWidget("Text_have") 
    self.Text_have:SetVisible(false)
    --已拥有数量
    self.Text_haveNum = self:GetWidget("Text_haveNum") 
    self.Text_haveNum:SetText(" ")
    --名字
    self.Panel_name = self:GetWidget("Panel_name") 
    --控制面板
    self.Image_3 = self:GetWidget("Image_3") 
    --可购买
    self.Text_couldBuy = self:GetWidget("Text_couldBuy") 
    self.Text_couldBuy:SetVisible(false)

    --总价
    self.Text_totalPrice = self:GetWidget("Text_totalPrice") 
    self.Text_totalPrice:SetVisible(false)

    --价钱图标
    self.Image_coin = self:GetWidget("Image_coin") 
    self.Image_coin:SetVisible(false)

    --价钱
    self.Text_totalPriceNum = self:GetWidget("Text_totalPriceNum") 
    self.Text_totalPriceNum:SetVisible(false)

    --添加按钮1
    self.Button_add = Util:WidgetButton(self:GetWidget("Button_add"),"",function ()
		self:OnBtnAdd(1)
	end,nil,nil,nil,nil,0)
	PosTool.MoveBy( self.Button_add  , -30 , 0 )
    self.Image_20 = self:GetWidget("Image_20") 

    --减少按钮1
    self.Button_sub = Util:WidgetButton(self:GetWidget("Button_sub"),"",function ()
		self:OnBtnSub(1)
	end,nil,nil,nil,nil,0)
	PosTool.MoveBy( self.Button_sub  , 30 , 0 )
    self.Image_21 = self:GetWidget("Image_21") 

    --添加按钮10
    self.Button_add10 = Util:WidgetButton(self:GetWidget("Button_add10"),"",function ()
		self:OnBtnAdd(10)
	end,nil,nil,nil,nil,0)
	PosTool.MoveBy( self.Button_add10  , -40 , 0 )
    self.Image_22 = self:GetWidget("Image_22") 

    --减少按钮10
    self.Button_sub10 = Util:WidgetButton(self:GetWidget("Button_sub10"),"",function ()
		self:OnBtnSub(10)
	end,nil,nil,nil,nil,0)
	PosTool.MoveBy( self.Button_sub10  , 40 , 0 )
    self.Image_23 = self:GetWidget("Image_23") 

    --选中背景
    self.img_textBg = self:GetWidget("Image_5_0_0_0_0_0_0_0_0") 
	self.img_textBg:SetContentSize( self.img_textBg:GetContentWidth()/2 , self.img_textBg:GetContentHeight() )

    --选中
    self.Text_chooseNum = self:GetWidget("Text_chooseNum") 
	PosTool.Center( self.Text_chooseNum )

    --取消
    self.Button_cancel = Util:WidgetButton(
    											self:GetWidget("Button_cancel"), 
    											CPPGameLib.GetString("common_cancel") , 
    											CPPGameLib.Handler(self,self.OnButtonClose),
    											nil,nil,Macros.Color.button_yellow )
    --购买
    self.Button_buy =  Util:WidgetButton(
    										self:GetWidget("Button_buy"), 
    										CPPGameLib.GetString("common_confirm") ,
    										function ()
												self:OnButtonBuy()
											end,nil,nil,Macros.Color.button_red )
    --头像
    self.Panel_head = self:GetWidget("Panel_head") 
end

function LogicGUI.TemplShopBuy:__ShowUI(data)
	--标题
	Util:WidgetLabel(self.Text_tile , data.title or CPPGameLib.GetString("shop_buy_prop_title") , Macros.TypefaceSize.popup ,Macros.Color.white )

	--头像
	local icon = LogicGUI.IconLayer.New()
	self.Panel_head:AddChild(icon:GetNode())
	PosTool.Center(icon)
	icon:SetData({ id = data.item_id , num = data.num or 1 })	

	local type_ , item_Config = ObjIDTool.GetInfo(data.item_id)
	--名字
	local name = Util:Name(item_Config.name , Macros.TypefaceSize.largish , item_Config.quality or item_Config.init_quality) 
	name:SetAnchorPoint(0,0.5)
	name:SetPosition(0 , self.Panel_name:GetContentHeight()/2)
	self.Panel_name:AddChild(name)

	--已拥有数量
	local have_num = ObjIDTool.GetOwnNum(data.item_id)
	local haveNum = Util:Label(have_num, Macros.TypefaceSize.normal , Macros.Color.content )
	haveNum:SetPosition(self.Text_haveNum:GetContentWidth()/2 + 20 , self.Text_haveNum:GetContentHeight()/2 )
	haveNum:SetAnchorPoint( 1 , 0.5 )
	self.Text_haveNum :AddChild(haveNum)

	local have_text = Util:Label(CPPGameLib.GetString("shop_have_already_Text"), Macros.TypefaceSize.normal , Macros.Color.keypoint )
	have_text:SetAnchorPoint(1,0.5)
	have_text:SetPosition( haveNum:GetPositionX() - haveNum:GetContentWidth() - 5 , haveNum:GetPositionY() )
	self.Text_haveNum:AddChild(have_text)

	--描述
	Util:WidgetLabel(self.Text_des , item_Config.description or item_Config.describe , Macros.TypefaceSize.normal ,  Macros.Color.content , 291 , 50 )

	self.pitchNum = 1
	self.lab_setnum = Util:WidgetLabel(self.Text_chooseNum , self.pitchNum , Macros.TypefaceSize.normal , Macros.Color.content )
	self:UpShowUI(data , 1)

	if data.max then
		local shop_can_buymax = Util:Label( CPPGameLib.GetString("shop_can_buymax" , data.max ) , Macros.TypefaceSize.normal , Macros.Color.keypoint )
		shop_can_buymax:SetAnchorPoint(0.5 , 0.5)
		shop_can_buymax:SetPosition(self.Button_sub10:GetPositionX() + 20  , self.Image_coin:GetPositionY() )
		self.Image_3:AddChild(shop_can_buymax)
	end

    local maxSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "bg_sp_buttonBg" )
    local btnBg = Util:Sprite9( maxSp )
    btnBg:SetContentSize( 55 , 45 )
    btnBg:SetAnchorPoint( 0.5,0.5 )
	btnBg:SetPosition( self.Button_add10:GetPositionX() + 60  , self.Button_add10:GetPositionY() )
	self.Image_3:AddChild( btnBg )

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_num_max")
    local btn = Util:Button( sp ,  function ( ... )
    	CPPGameLib.PrintTable("xy" , self.data , "self.data-----------") 
    	self:OnBtnAdd( 9999  )
    end )
    btnBg:AddChild(btn) 
    PosTool.Center( btn , 0 , 2 )
end

--刷新UI
function LogicGUI.TemplShopBuy:UpShowUI(data,index)
	if self.priceLayer then
		self.priceLayer:RemoveSelf(true)
		self.priceLayer = nil
		if self.tips then
			self.tips:RemoveSelf(true)
			self.tips = nil
		end
	end

	self.priceLayer = Util:Layout(0,0)
	self.priceLayer:SetAnchorPoint(0.5 , 0.5)
	self.priceLayer:SetPosition(self.Button_add10:GetPositionX() - 20  , self.Image_coin:GetPositionY() )
	self.Image_3:AddChild(self.priceLayer)
	local is_need_tips = true
	local layer = {}
	local width = 0
	for i,v in ipairs(data.cost) do
		local num = 0
		if v.price_list then

			if v.haveBuyNum then
				if (v.haveBuyNum + 1) >= #v.price_list then
					num = v.price_list[#v.price_list] * math.floor(index)
				elseif (v.haveBuyNum + math.floor(index)) >= #v.price_list then 
					local pri = 0
					for i=v.haveBuyNum + 1,#v.price_list do
						pri = pri + v.price_list[i]
					end
					for i=#v.price_list + 1,v.haveBuyNum + math.floor(index) do
						pri = pri + v.price_list[#v.price_list]
					end
					num = pri
				elseif (v.haveBuyNum + math.floor(index)) < #v.price_list then 
					local pri = 0
					for i=v.haveBuyNum + 1,v.haveBuyNum + math.floor(index) do
						pri = pri + v.price_list[i]
					end
					num = pri
				end
			elseif v.price_list[ math.floor(index) ] then
				for i = 1 , math.floor(index) do
					num = num + v.price_list[i]
				end
			else
				num = num + v.price_list[#v.price_list]
			end

		else
			num = v.num * math.floor(index) 
		end

		if v.type or v.type_id then
			local icon = nil
			local dx = 0.8
			if v.type then
				local type_ , config = ObjIDTool.GetInfo( v.type )
				if type_ == Macros.Game.ObjType.ITEM then
					icon = Resource.PathTool.GetItemPath(config.res_id)  
					dx = 0.55
				else
					icon = Util:GetMoneyIconImgData(v.type)
				end

			elseif v.type_id then
				icon = Resource.PathTool.GetItemPath( v.type_id )
			end

			local tab = { 
							{ tp = icon , dx = dx },
							{ zi = num , dx = Macros.TypefaceSize.normal ,ys = Macros.Color.content , jg = 5},
						}
			local itemLst = {}
			layer[i] , itemLst = Util:LabCenter( tab )	

			local type_ , config = ObjIDTool.GetInfo( v.type or v.type_id ) 
	        if config and config.sub_type == Macros.Game.NormalItemSubType.EQUIPDEBRIS or 
	            config.sub_type == Macros.Game.NormalItemSubType.HERODEBRIS then
				if itemLst[1] then
		            local piece_icon_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON, "piece_icon")
		            local piece_icon = Util:Sprite( piece_icon_sp )
		            piece_icon:SetAnchorPoint(0,1)
		            piece_icon:SetPosition( 6.5 , itemLst[1]:GetContentHeight() - 6.5 )
		            itemLst[1]:AddChild(piece_icon, LogicGUI.IconLayer.InnerZOrder.ICON)
				end
	        end

		elseif v.sp_txt then
			is_need_tips = false
			layer[i] = Util:RichText(CPPGameLib.GetString(v.sp_txt,tostring(num)), Macros.TypefaceSize.normal, 0, 0, Macros.Color.content_hex)
		end	
		width = width + layer[i]:GetContentWidth()
		self.priceLayer:AddChild(layer[i])
	end

	self.priceLayer:SetContentSize( width , layer[1]:GetContentHeight() + 20 )
    local x = 0
    for i=1,#layer do
    	if layer[i-1] then
    		x = layer[i-1]:GetContentWidth() + layer[i-1]:GetPositionX() + 10
    	end
    	local dis_x = is_need_tips and 0 or -40
    	layer[i]:SetPosition( x+dis_x , self.priceLayer:GetContentHeight()/2 )
    	layer[i]:SetAnchorPoint( 0 , 0.5 )
    end
    if is_need_tips then
	    self.tips = Util:Label( CPPGameLib.GetString("shop_total_price_Text") , Macros.TypefaceSize.normal ,Macros.Color.keypoint)
	    self.tips:SetAnchorPoint(1,0.5)
	    self.tips:SetPosition( self.priceLayer:GetPositionX() - self.priceLayer:GetContentWidth()/2 , self.priceLayer:GetPositionY() )
	    self.Image_3:AddChild( self.tips )
	end
end


function LogicGUI.TemplShopBuy:__Settle(data)
	if self.data.max then
		self.itemNum = self.data.max
		return
	end
	table.sort( data.cost, function ( a , b )
		return a.num > b.num
	end )

    local itemLst = {}
    for i , v in ipairs( data.cost ) do
    	local type_ , info = ObjIDTool.GetInfo( v.type )
    	local num = 0
    	if type_ == Macros.Game.ObjType.ITEM then
    		num = CallModuleFunc( ModuleType.Pack , "GetNumByItemID" , v.type )
    	else
    		num = CallModuleFunc( ModuleType.ROLE , "GetPropValue" , v.type )
    	end
		num = num / v.num
		table.insert( itemLst , math.floor(num) )
    end

	table.sort( itemLst , function( a , b )
		return a < b
	end )
	
	self.itemNum = self.data.max or itemLst[1]
end

function LogicGUI.TemplShopBuy:OnBtnAdd(index)
	if self.pitchNum >= self.itemNum then
		return
	elseif self.pitchNum + index >= self.itemNum then
		self.pitchNum = self.itemNum
		self.lab_setnum:SetText(math.floor( self.pitchNum ) )
	else
		self.pitchNum = self.pitchNum + index
		self.lab_setnum:SetText(math.floor( self.pitchNum ) )
	end

	self:UpShowUI( self.data , self.pitchNum )
end

function LogicGUI.TemplShopBuy:OnBtnSub(index)
	if self.pitchNum <= 1 then
		return 
	elseif self.pitchNum - index <= 1  then
		self.pitchNum = 1
		self.lab_setnum:SetText(1)
	else 
		self.pitchNum = self.pitchNum - index
		self.lab_setnum:SetText(math.floor( self.pitchNum ) )
	end

	self:UpShowUI( self.data , self.pitchNum )
end

function LogicGUI.TemplShopBuy:OnButtonBuy(index)

	self.pitchNum = math.floor(self.pitchNum)
	--外部回调
	if self.data.func then
		self.data.func( self.pitchNum )
		self:OnButtonClose()
		return 
	end

	local data = {}
	data.type = self.data.type
	data.param1 = self.data.goods_id
	data.param2 = self.pitchNum

    local type_ = nil
	for i,v in ipairs(self.data.cost) do
		local num = 0
		if v.price_list then
			if v.price_list[self.pitchNum] then
				for j=1,self.pitchNum do
					num = num + v.price_list[j]
				end
			else
				for j=1,self.pitchNum do
					num = num + v.price_list[#v.price_list]
				end
			end
		else
			num = v.num * self.pitchNum 
		end

		local type_1 , config = ObjIDTool.GetInfo( v.type )
		if type_1 == Macros.Game.ObjType.ITEM then
			local item_num = CallModuleFunc(ModuleType.Pack, "GetNumByItemID", v.type )
			if num > item_num then
				type_ = v.type
				break
			end
		elseif v.type and num > CallModuleFunc(ModuleType.ROLE,"GetPropValue",v.type) then
			type_ = v.type
			break
		end
	end

	if self.data.max == 0 then
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("shop_today_can_notbuy") )
	elseif not type_ then
		CallModuleFunc(ModuleType.SHOP,"ShopBuyRequest",data)
	else
		local type_1 , config = ObjIDTool.GetInfo( type_ )
		if type_1 == Macros.Game.ObjType.ITEM then
			GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("hero_potential_tips2" , config.name ) )
			GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, type_)
		else
			local txt = "currency_insufficient_"..type_
			GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString(txt) )

			local config = CPPGameLib.GetConfig("HuoBi", type_ )
			GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, config.big_icon)
		end


	end
	self:OnButtonClose()
end

function LogicGUI.TemplShopBuy:OnButtonClose(index)
	if self.func then
		self.func()	
	end
	self:DeleteMe()
end
