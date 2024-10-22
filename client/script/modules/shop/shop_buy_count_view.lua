
--[[
%% @module: (普通商店、礼包商店、GM商店)
--]]

LogicGUI = LogicGUI or {}

LogicGUI.TemplShopBuyCountView = LogicGUI.TemplShopBuyCountView or BaseClass(GUI.Template)

function LogicGUI.TemplShopBuyCountView:__init(data)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:InitUI()
		self:SetData(data)
	end
	self:InitTemplate(GUI.TemplateRegistry.SHOP_BUY_TIPVIEW)
end

function LogicGUI.TemplShopBuyCountView:InitUI()
	--标题
	Util:WidgetLabel(self:GetWidget("Text_tile"),CPPGameLib.GetString("shop_buy_prop_title"), Macros.TypefaceSize.popup, Macros.Color.btn)

	--关闭按钮
	Util:WidgetButton(self:GetWidget("Button_close"),"",function ()
		self:DeleteMe()
	end)

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

	--数量按钮
	--加1
	Util:WidgetButton(self:GetWidget("Button_add"),"",function ()
		self:__OnBtnAdd(1)
	end,nil,nil,nil,nil,0)
	--加10
	Util:WidgetButton(self:GetWidget("Button_add10"),"",function ()
		self:__OnBtnAdd(10)
	end,nil,nil,nil,nil,0)
	--加MAX
	self.Image_3 = self:GetWidget("Image_3")
    local maxSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "bg_sp_buttonBg" )
    local btnBg = Util:Sprite9( maxSp )
    btnBg:SetContentSize( 55 , 45 )
    btnBg:SetAnchorPoint( 0.5,0.5 )
	btnBg:SetPosition( btn_add10:GetPositionX() + 60  , btn_add10:GetPositionY() )
	self.Image_3:AddChild( btnBg )
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_num_max")
    local btn = Util:Button( sp ,  function ( ... )
    	self:__OnBtnAdd( ConfigAdapter.Common.GetOnceUserMax() )
    end )
    btnBg:AddChild(btn) 
    PosTool.Center( btn , 0 , 2 )

	--减1
	Util:WidgetButton(self:GetWidget("Button_sub"),"",function ()
		self:__OnBtnSub(1)
	end,nil,nil,nil,nil,0)
	--减10
	Util:WidgetButton(self:GetWidget("Button_sub10"),"",function ()
		self:__OnBtnSub(10)
	end,nil,nil,nil,nil,0)

	--选中数字
	self.num_Text = self:GetWidget("Text_chooseNum")
	Util:WidgetLabel(self.num_Text,1,Macros.TypefaceSize.normal,Macros.Color.content)
	
	--取消
	Util:WidgetButton(self:GetWidget("Button_cancel"),CPPGameLib.GetString("shop_cancel_Buy"),function ()
		self:DeleteMe()
	end,Macros.TypefaceSize.popup,Macros.Color.btn, Macros.Color.button_red )
	
	--确定
	Util:WidgetButton(self:GetWidget("Button_buy"),CPPGameLib.GetString("shop_sure_Buy"),function ()
		self:__OnBtnBuy()
	end,Macros.TypefaceSize.popup,Macros.Color.btn, Macros.Color.button_yellow )    
end

function LogicGUI.TemplShopBuyCountView:SetData(data)
	self.__data = data

	--默认选中数量
	self.chosen_num = 1

	--商品对应的物品信息
	local obj_type, config_good_item = ObjIDTool.GetInfo(data.item_id)

	--商品图标
	local icon = LogicGUI.IconLayer.New(false,false)
	self:GetWidget("Panel_head"):AddChild(icon:GetNode())
	PosTool.Center(icon)
	if data.shop_type == Macros.Game.SHOP.STOREPROP or data.shop_type == Macros.Game.SHOP.GM_SHOP then
		icon:SetData({id = data.item_id, num = data.item_num})
	else
		icon:SetData({id = data.item_id})	
	end

	--商品名称
	local name =  Util:Name(config_good_item.name, Macros.TypefaceSize.largish, config_good_item.quality or config_good_item.init_quality)
	name:SetAnchorPoint(0,0.5)
	self:GetWidget("Panel_name"):AddChild(name)
	PosTool.LeftCenter(name)

	--商品描述
	Util:WidgetLabel(self:GetWidget("Text_des"), config_good_item.description or config_good_item.describe, Macros.TypefaceSize.normal, Macros.Color.content, 325, 48)

	--消耗类型、消耗数量、限购数量
	local text_couldbuy = self:GetWidget("Text_couldBuy")
	if data.shop_type == Macros.Game.SHOP.STOREPROP then
		self.price_list = data.price_list
		self.cost_type = data.cost_type
		self.cost_value = data.cost_value
		self:__CalcMaxBuyNumForCommonGood()		
	else
		self.cost_type = data.cost_type
		self.cost_value = data.cost_value
		self.max_buynum = data.canBuyNum ~= 0 and data.canBuyNum or ConfigAdapter.Common.GetOnceUserMax()
	end	

	if data.canBuyNum == 0 then
		text_couldbuy:SetVisible(false)
	else	
		self.max_buynum = math.min(data.canBuyNum - data.haveBuyNum, self.max_buynum) 
	end

	--限购数量
	Util:WidgetLabel(text_couldbuy,CPPGameLib.GetString("shop_can_buymax", self.max_buynum),Macros.TypefaceSize.normal,Macros.Color.keypoint)

	--已拥有数量
	local txt_have = self:GetWidget("Text_have")
	Util:WidgetLabel(txt_have,CPPGameLib.GetString("shop_have_already_Text"),Macros.TypefaceSize.normal, Macros.Color.keypoint )
	local have_num = ObjIDTool.GetOwnNum(data.item_id)
	local txt_havenum = self:GetWidget("Text_haveNum")
	Util:WidgetLabel(txt_havenum,have_num,Macros.TypefaceSize.normal , Macros.Color.content)

	--总价
	local txt_totalprice = self:GetWidget("Text_totalPrice")
	Util:WidgetLabel(txt_totalprice,CPPGameLib.GetString("shop_total_price_Text"),Macros.TypefaceSize.normal,Macros.Color.keypoint)	

	--价格
	self.Text_totalPriceNum = self:GetWidget("Text_totalPriceNum")
	Util:WidgetLabel(self.Text_totalPriceNum,self.cost_value,Macros.TypefaceSize.normal, Macros.Color.content )

	--货币	 
	self:GetWidget("Image_coin"):SetImage(Util:GetMoneyIconImgData(self.cost_type))
end

--计算总价格
function LogicGUI.TemplShopBuyCountView:__CalcTotalPrice()
	if self.__data.shop_type == Macros.Game.SHOP.STOREPROP then
		local max_price = self.price_list[#self.price_list]
		local have_buy_num = self.__data.haveBuyNum
		if have_buy_num + 1 >= #self.price_list then
			self.cost_value = max_price * self.chosen_num
		else
			local cur_cost_value = 0
			for i = have_buy_num + 1, have_buy_num + self.chosen_num do
				cur_cost_value = cur_cost_value + (self.price_list[i] or max_price)
			end			
			self.cost_value = cur_cost_value
		end	
	else
		self.cost_value = self.cost_value * self.chosen_num
	end
end

--计算普通商品的可购买数
function LogicGUI.TemplShopBuyCountView:__CalcMaxBuyNumForCommonGood()
	local have_buy_num = self.__data.haveBuyNum
	local cur_currency_value = CallModuleFunc(ModuleType.ROLE,"GetPropValue", self.cost_type)			
	local max_price = self.price_list[#self.price_list]

	if have_buy_num + 1 >= #self.price_list then
		self.max_buynum = math.floor(cur_currency_value / max_price)	
	else
		local cur_price = 0
		local buynum = 0
		for i = have_buy_num + 1, #self.price_list do
			cur_price = cur_price + self.price_list[i]
			if cur_price <= cur_currency_value then
				buynum = buynum + 1
			else
				buynum = buynum + math.floor((cur_currency_value - cur_price) / max_price)
				break
			end
		end
		if cur_price < cur_currency_value then
			buynum = buynum + math.floor((cur_currency_value - cur_price) / max_price)
		end

		self.max_buynum = math.max(0, buynum)
	end
end

--增加数量
function LogicGUI.TemplShopBuyCountView:__OnBtnAdd(num)	
	if self.chosen_num >= self.max_buynum then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("shop_limit_buy_num"))
		return
	end

	self.chosen_num = math.min(self.chosen_num + num, self.max_buynum)
	self.num_Text:SetText(self.chosen_num)

	self:__CalcTotalPrice()	
	self.Text_totalPriceNum:SetText(self.cost_value)
end

--减少数量
function LogicGUI.TemplShopBuyCountView:__OnBtnSub(num)
	if self.chosen_num <= 1 then
		return 
	end

	self.chosen_num = math.max(0, self.chosen_num - num)
	self.num_Text:SetText(self.chosen_num)

	self:__CalcTotalPrice()	
	self.Text_totalPriceNum:SetText(self.cost_value)
end

--处理购买相关
function LogicGUI.TemplShopBuyCountView:__OnBtnBuy()
	--检测背包空间是否充足
	local obj_type = ObjIDTool.GetInfo(self.__data.item_id)
	local ismax = false
	CallModuleFunc(ModuleType.SUNDRY, "PackVacancy", obj_type , self.chosen_num ,true,function ( sum , text )
		if sum < self.chosen_num then
			ismax = true
		end
	end)
	if ismax then
		return 
	end

	--检测角色等级
	local role_level = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.LEVEL)
	if self.__data.need_role_level and self.__data.need_role_level > role_level then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("shop_level_tip",self.__data.need_role_level))
		self:DeleteMe()
		return
	end

	--检测VIP等级
	local vip_level = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.VIP_LEVEL)
	if self.__data.need_vip_level and self.__data.need_vip_level > vip_level then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("shop_vip_tip",self.__data.need_vip_level))
		self:DeleteMe()
		return
	end

	--检测GM等级
	if not GlobalCustomizedLogicAdapterMgr:Call("CheckGMLevel", self.__data.need_gm_level) then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("shop_gm_tip",self.__data.need_gm_level))
		self:DeleteMe()
		return
	end

	--检测货币是否充足
	local cur_currency_value = CallModuleFunc(ModuleType.ROLE, "GetPropValue", self.__data.cost_type)
	if cur_currency_value >= self.cost_value then
		local info = {}
		info.type = self.__data.shop_type
		info.param1 = self.__data.goods_id
		info.param2 = self.chosen_num
		info.param3 = self.__data.shop_sub_type
		CallModuleFunc(ModuleType.SHOP, "ShopBuyRequest", info)
		self:DeleteMe()
	else
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("shop_no_enough_money"))
	end		
end