LogicGUI = LogicGUI or {}

LogicGUI.TemplArenaReputationShopBuyCountView = LogicGUI.TemplArenaReputationShopBuyCountView or BaseClass(GUI.Template)

function LogicGUI.TemplArenaReputationShopBuyCountView:__init(data)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:__Getwidget()
		self:InitMainUI(data)
	end
	self:InitTemplate(GUI.TemplateRegistry.ARENA_REPUTATIONSHOPBUY_VIEW)
end

function LogicGUI.TemplArenaReputationShopBuyCountView:__Getwidget()
	--数字显示框
	self.img_textBg = self:GetWidget("Image_5_0_0_0_0_0_0_0_0") 
	self.img_textBg:SetContentSize( self.img_textBg:GetContentWidth()/2 , self.img_textBg:GetContentHeight() )

	PosTool.Center( self:GetWidget("Text_chooseNum") )
	--添加按钮背景
	PosTool.MoveBy( self:GetWidget("Button_add")  , -30 , 0 )
	--减少按钮背景
	PosTool.MoveBy( self:GetWidget("Button_sub")  , 30 , 0 )
	--加十背景
	PosTool.MoveBy( self:GetWidget("Button_add10")  , -40 , 0 )
	--减十背景
	PosTool.MoveBy( self:GetWidget("Button_sub10")  , 40 , 0 )

    local btn_add10 = self:GetWidget("Button_add10")
    
	self.Image_3 = self:GetWidget("Image_3")
    local maxSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "bg_sp_buttonBg" )
    local btnBg = Util:Sprite9( maxSp )
    btnBg:SetContentSize( 55 , 45 )
    btnBg:SetAnchorPoint( 0.5,0.5 )
	btnBg:SetPosition( btn_add10:GetPositionX() + 60  , btn_add10:GetPositionY() )
	self.Image_3:AddChild( btnBg )

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_num_max")
    local btn = Util:Button( sp ,  function ( ... )
    	self:OnBtnAdd( ConfigAdapter.Common.GetOnceUserMax() )
    end )
    btnBg:AddChild(btn) 
    PosTool.Center( btn , 0 , 2 )
end


function LogicGUI.TemplArenaReputationShopBuyCountView:InitMainUI(data)
	self.__data = data
	--默认选中数量
	self.chooseNum = 1
	--配置表数据
	local type,item_Config = ObjIDTool.GetInfo(data.item_id)
	--标题
	Util:WidgetLabel(self:GetWidget("Text_tile"),CPPGameLib.GetString("shop_buy_prop_title"), Macros.TypefaceSize.popup, Macros.Color.btn)
	--关闭按钮
	Util:WidgetButton(self:GetWidget("Button_close"),"",function ()
		self:DeleteMe()
	end)
	--头像
	local icon = LogicGUI.IconLayer.New()
	self:GetWidget("Panel_head"):AddChild(icon:GetNode())
	PosTool.Center(icon)
	icon:SetData({id = data.item_id})	
	--描述
	if type ~= Macros.Game.ObjType.HERO then
		Util:WidgetLabel(self:GetWidget("Text_des"),item_Config.description,Macros.TypefaceSize.normal, Macros.Color.content ,325,48)
	elseif type == Macros.Game.ObjType.HERO then
		Util:WidgetLabel(self:GetWidget("Text_des"),item_Config.describe,Macros.TypefaceSize.normal,Macros.Color.content,325,48)
	end	
	--已拥有
	local txt_have = self:GetWidget("Text_have")
	Util:WidgetLabel(txt_have,CPPGameLib.GetString("shop_have_already_Text"),Macros.TypefaceSize.normal,Macros.Color.keypoint)
	local have_num = ObjIDTool.GetOwnNum(data.item_id)
	local txt_havenum = self:GetWidget("Text_haveNum")
	Util:WidgetLabel(txt_havenum,have_num,Macros.TypefaceSize.normal,Macros.Color.content)
	--名字
	local name = nil
	if type ~= Macros.Game.ObjType.HERO then
		name = Util:Name(item_Config.name,Macros.TypefaceSize.largish,item_Config.quality)
	elseif type == Macros.Game.ObjType.HERO then
		name = Util:Name(item_Config.name,Macros.TypefaceSize.largish,item_Config.init_quality)
	end	
	if name then
		name:SetAnchorPoint(0,0.5)
		self:GetWidget("Panel_name"):AddChild(name)
		PosTool.LeftCenter(name)
	end
	--可购买数量
	local Text_couldBuy = self:GetWidget("Text_couldBuy")	
	self:CountTotalNum()
	if data.canBuyNum ~= 0 then
		self.limit_buy_num = data.canBuyNum - data.haveBuyNum
		self.max_num = math.min(self.limit_buy_num,self.max_num)
		Util:WidgetLabel(Text_couldBuy,CPPGameLib.GetString("shop_can_buymax",self.limit_buy_num),Macros.TypefaceSize.normal , Macros.Color.keypoint )
	else
		Text_couldBuy:SetVisible(false)
		self.IsNotNeedLimit = true
	end
	--总价
	local Text_totalPrice = self:GetWidget("Text_totalPrice")
	Util:WidgetLabel(Text_totalPrice,CPPGameLib.GetString("shop_total_price_Text"), Macros.TypefaceSize.normal,  Macros.Color.keypoint  )	
	
	local lst = {}
	for i , v in ipairs( data.cost_type_list ) do
		table.insert( lst , { id = v , value = data.cost_value_list[i] } )
	end

	if data.cost_item_id_list then
		for i , v in ipairs( data.cost_item_id_list ) do
			table.insert( lst , { id = v , value = data.cost_item_num_list[i] } )
		end
	end

	self:GetWidget("Image_coin_2"):SetVisible( #lst >= 2)
	self:GetWidget("Text_totalPriceNum_2"):SetVisible( #lst >= 2)	

	for i , v in ipairs( lst ) do
		local type_ , config = ObjIDTool.GetInfo( v.id )
		local sp = nil
		if type_ == Macros.Game.ObjType.ITEM then
			sp = Resource.PathTool.GetItemPath(config.res_id)  
			self:GetWidget("Image_coin_"..i):SetImage(sp,TextureResType.LOCAL)
		else
			sp = Util:GetMoneyIconImgData( v.id )  
			self:GetWidget("Image_coin_"..i):SetImage(sp)
		end
		self["Text_totalPriceNum"..i] = self:GetWidget("Text_totalPriceNum_"..i)
		Util:WidgetLabel(self["Text_totalPriceNum"..i] , v.value , Macros.TypefaceSize.normal,  Macros.Color.content )
		self["totalPrice"..i] = v.value
	end
	Text_totalPrice:SetPosition(#lst == 1 and 270 or Text_totalPrice:GetPositionX(),Text_totalPrice:GetPositionY())
	local txt_totalpricenum = self:GetWidget("Text_totalPriceNum_1")
	
	--数量按钮
	--加1
	Util:WidgetButton(self:GetWidget("Button_add"),"",function ()
		self:OnBtnAdd(1)
	end,nil,nil,nil,nil,0)
	--加10
	Util:WidgetButton(self:GetWidget("Button_add10"),"",function ()
		self:OnBtnAdd(10)
	end,nil,nil,nil,nil,0)
	--减1
	Util:WidgetButton(self:GetWidget("Button_sub"),"",function ()
		self:OnBtnSub(1)
	end,nil,nil,nil,nil,0)
	--减10
	Util:WidgetButton(self:GetWidget("Button_sub10"),"",function ()
		self:OnBtnSub(10)
	end,nil,nil,nil,nil,0)
	--选中数字
	self.num_Text = self:GetWidget("Text_chooseNum")
	Util:WidgetLabel(self.num_Text,1,Macros.TypefaceSize.normal,Macros.Color.content)
	--取消
	Util:WidgetButton(self:GetWidget("Button_cancel"),CPPGameLib.GetString("shop_cancel_Buy"),function ()
		self:DeleteMe()
	end,Macros.TypefaceSize.popup,Macros.Color.btn,Macros.Color.button_red)
	--确定
	Util:WidgetButton(self:GetWidget("Button_buy"),CPPGameLib.GetString("shop_sure_Buy"),function ()
		--不满足条件
		if self.__data.need_rank ~= 0 and self.__data.history_rank > self.__data.need_rank then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("arena_no_satisfy_rank"))
			return
		end
		local IsCouldBuy = true
		local cost_type = 0

		local lst = {}
		for i , v in ipairs( data.cost_type_list ) do
			table.insert( lst , { id = v , value = data.cost_value_list[i] } )
		end

		if data.cost_item_id_list then
			for i , v in ipairs( data.cost_item_id_list ) do
				table.insert( lst , { id = v , value = data.cost_item_num_list[i] } )
			end
		end

		for i , v in ipairs( lst ) do
			local type_1 , config = ObjIDTool.GetInfo( v.id )
			local moneyNum = nil
			if type_1 == Macros.Game.ObjType.ITEM then
				moneyNum = CallModuleFunc(ModuleType.Pack, "GetNumByItemID", v.id )
			else
				moneyNum = CallModuleFunc(ModuleType.ROLE,"GetPropValue", v.id )
			end
			if moneyNum < self["totalPrice"..i] then
				IsCouldBuy = false
				cost_type = v.id
				break
			end
		end

		if IsCouldBuy then
			local info = {}
			info.type =  Macros.Game.SHOP.ARENA
			info.param1 =  data.goods_id
			info.param2 =  self.chooseNum
			info.param3 = data.type
			CallModuleFunc(ModuleType.SHOP,"ShopBuyRequest",info)
			self:DeleteMe()
		else
			local type_1 , config = ObjIDTool.GetInfo( cost_type )
			if type_1 == Macros.Game.ObjType.ITEM then
				GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("hero_potential_tips2" , config.name ) )
				GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, cost_type)
			else
				GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("hero_potential_tips2" , config.name ) )
				
				local config = CPPGameLib.GetConfig("HuoBi", cost_type )
				GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, config.big_icon)
			end
		end		
	end,Macros.TypefaceSize.popup,Macros.Color.btn,Macros.Color.button_yellow)
end

--增加数量
function LogicGUI.TemplArenaReputationShopBuyCountView:OnBtnAdd(num)
	if self.chooseNum >= self.max_num then
		if self.IsNotNeedLimit then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("shop_max_could_buy_num"))
		else
			if self.limit_buy_num == self.max_num then
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("shop_limit_buy_num"))
			else
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("shop_max_could_buy_num"))
			end
		end
		return
	elseif self.chooseNum + num >= self.max_num then
		self.chooseNum = self.max_num			
	else
		self.chooseNum = self.chooseNum + num
	end
	self.num_Text:SetText(self.chooseNum)
	local num = 0
	if self.__data.cost_type_list then
		for i=1,#self.__data.cost_type_list do
			num = num + 1
			self["totalPrice"..num] = self.chooseNum * self.__data.cost_value_list[i]
			self["Text_totalPriceNum"..num]:SetText(self["totalPrice"..num])
		end	
	end
	if self.__data.cost_item_id_list then
		for i=1,#self.__data.cost_item_id_list do
			num = num + 1
			self["totalPrice"..num] = self.chooseNum * self.__data.cost_item_num_list[i]
			self["Text_totalPriceNum"..num]:SetText(self["totalPrice"..num])
		end	
	end
end

--减少数量
function LogicGUI.TemplArenaReputationShopBuyCountView:OnBtnSub(num)
	if self.chooseNum <= 1 then
		return 
	elseif self.chooseNum - num <= 1  then
		self.chooseNum = 1
	else 
		self.chooseNum = self.chooseNum - num		
	end
	self.num_Text:SetText(self.chooseNum)
	local num = 0
	if self.__data.cost_type_list then
		for i=1,#self.__data.cost_type_list do
			num = num + 1
			self["totalPrice"..num] = self.chooseNum * self.__data.cost_value_list[i]
			self["Text_totalPriceNum"..num]:SetText(self["totalPrice"..num])
		end	
	end
	if self.__data.cost_item_id_list then
		for i=1,#self.__data.cost_item_id_list do
			num = num + 1
			self["totalPrice"..num] = self.chooseNum * self.__data.cost_item_num_list[i]
			self["Text_totalPriceNum"..num]:SetText(self["totalPrice"..num])
		end	
	end
end

--计算总数量
function LogicGUI.TemplArenaReputationShopBuyCountView:CountTotalNum()
	if self.__data.cost_type_list then
		for index,cost_type in ipairs(self.__data.cost_type_list) do
			local moneyNum = CallModuleFunc(ModuleType.ROLE,"GetPropValue",cost_type)
			local max_num = math.floor(moneyNum/self.__data.cost_value_list[index])
			self.max_num = self.max_num or max_num
			self.max_num = math.min(self.max_num,max_num)	
		end
	end
	--
	if self.__data.cost_item_id_list then
		for index,item_id in ipairs(self.__data.cost_item_id_list) do
			local itemNum = ObjIDTool.GetOwnNum(item_id)
			local max_num = math.floor(itemNum/self.__data.cost_item_num_list[index])
			self.max_num = self.max_num or max_num
			self.max_num = math.min(self.max_num,max_num)	
		end
	end
end

