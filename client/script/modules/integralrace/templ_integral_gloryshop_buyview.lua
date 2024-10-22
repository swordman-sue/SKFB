LogicGUI = LogicGUI or {}

LogicGUI.TemplIntegralGloryShopBuyCountView = LogicGUI.TemplIntegralGloryShopBuyCountView or BaseClass(GUI.Template)

function LogicGUI.TemplIntegralGloryShopBuyCountView:__init(data)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:InitMainUI(data)
	end
	self:InitTemplate(GUI.TemplateRegistry.ARENA_REPUTATIONSHOPBUY_VIEW)
end

function LogicGUI.TemplIntegralGloryShopBuyCountView:InitMainUI(data)
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
		Util:WidgetLabel(self:GetWidget("Text_des"),item_Config.describe,Macros.TypefaceSize.normal, Macros.Color.content ,325,48)
	end	
	--已拥有
	Util:WidgetLabel(self:GetWidget("Text_have"),CPPGameLib.GetString("shop_have_already_Text"),Macros.TypefaceSize.normal, Macros.Color.keypoint )
	local have_num = ObjIDTool.GetOwnNum(data.item_id)
	Util:WidgetLabel(self:GetWidget("Text_haveNum"),have_num,Macros.TypefaceSize.normal, Macros.Color.content )
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
	if data.buy_num_max ~= 0 then
		self.limit_buy_num = data.buy_num_max - data.haveBuyNum
		self.max_Num = math.min(self.limit_buy_num,self.max_Num)
		Util:WidgetLabel(Text_couldBuy,CPPGameLib.GetString("shop_can_buymax",self.limit_buy_num),Macros.TypefaceSize.normal , Macros.Color.keypoint )
	else
		Text_couldBuy:SetVisible(false)
		self.IsNotNeedLimit = true
	end
	--总价
	local Text_totalPrice = self:GetWidget("Text_totalPrice")
	Util:WidgetLabel(Text_totalPrice,CPPGameLib.GetString("shop_total_price_Text"), Macros.TypefaceSize.normal, Macros.Color.keypoint  )	
	if data.cost_type_list then
		self:GetWidget("Image_coin_2"):SetVisible(#data.cost_type_list >= 2)
		self:GetWidget("Text_totalPriceNum_2"):SetVisible(#data.cost_type_list >= 2)
		for i=1,#data.cost_type_list do
			local sp = Util:GetMoneyIconImgData(data.cost_type_list[i])
			self:GetWidget("Image_coin_"..i):SetImage(sp)
			self["Text_totalPriceNum"..i] = self:GetWidget("Text_totalPriceNum_"..i)
			Util:WidgetLabel(self["Text_totalPriceNum"..i],data.cost_value_list[i], Macros.TypefaceSize.normal, Macros.Color.content )
			self["totalPrice"..i] = self.__data.cost_value_list[i]
		end
		Text_totalPrice:SetPosition(#data.cost_type_list == 1 and 270 or Text_totalPrice:GetPositionX(),Text_totalPrice:GetPositionY())
	elseif data.cost_item_id_list then
		self:GetWidget("Image_coin_2"):SetVisible(#data.cost_item_id_list >= 2)
		self:GetWidget("Text_totalPriceNum_2"):SetVisible(#data.cost_item_id_list >= 2)
		for i=1,#data.cost_item_id_list do
			local icon_path
			local config = CPPGameLib.GetConfig("Item",data.cost_item_id_list[i])
		    if not config then return "" end
		    if  config.sub_type == Macros.Game.NormalItemSubType.HERODEBRIS then
		      icon_path = Resource.PathTool.GetHeroHeadPath(config.res_id) 
		    else
		      icon_path = Resource.PathTool.GetItemPath(config.res_id)       
		    end
			self:GetWidget("Image_coin_"..i):SetImage(icon_path,TextureResType.LOCAL)
			self["Text_totalPriceNum"..i] = self:GetWidget("Text_totalPriceNum_"..i)
			Util:WidgetLabel(self["Text_totalPriceNum"..i],data.cost_item_num_list[i], Macros.TypefaceSize.normal, Macros.Color.content )
			self["totalPrice"..i] = self.__data.cost_item_num_list[i]
		end
		Text_totalPrice:SetPosition(#data.cost_item_id_list == 1 and 270 or Text_totalPrice:GetPositionX(),Text_totalPrice:GetPositionY())
	end
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
		local IsCouldBuy = true
		local cost_type = 0
		if data.cost_type_list then
			for i=1,#data.cost_type_list do
				local moneyNum = CallModuleFunc(ModuleType.ROLE,"GetPropValue",self.__data.cost_type_list[i])
				if moneyNum < self["totalPrice"..i] then
					IsCouldBuy = false
					cost_type = self.__data.cost_type_list[i]
					break
				end
			end	
		elseif data.cost_item_id_list then
			for i=1,#data.cost_item_id_list do
				local moneyNum = ObjIDTool.GetOwnNum(self.__data.cost_item_id_list[i])
				if moneyNum < self["totalPrice"..i] then
					IsCouldBuy = false
					cost_type = self.__data.cost_item_id_list[i]
					break
				end
			end	
		end
		if IsCouldBuy then
			local info = {}
			info.type = Macros.Game.SHOP.SHOP_SCORE_MATCH
			info.param1 =  data.goods_id
			info.param2 =  self.chooseNum
			CallModuleFunc(ModuleType.SHOP,"ShopBuyRequest",info)
			CallModuleFunc(ModuleType.INTEGRAL_RACE,"SaveNeedData",data.tag,self.chooseNum)
			--self:FireNextFrame(Macros.Event.ArenaModule.ARENA_UPDTAE_SHOPLIST,self.__data.type,data.tag,self.chooseNum)
			self:DeleteMe()
		else
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("shop_no_enough_money"))

			local config = CPPGameLib.GetConfig("HuoBi",cost_type)
			GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, data.cost_type_list and config.big_icon or cost_type)
		end		
	end,Macros.TypefaceSize.popup,Macros.Color.btn,Macros.Color.button_yellow)
end

--增加数量
function LogicGUI.TemplIntegralGloryShopBuyCountView:OnBtnAdd(num)
	if self.chooseNum >= self.max_Num then
		if self.IsNotNeedLimit then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("shop_max_could_buy_num"))
		else
			if self.limit_buy_num == self.max_Num then
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("shop_limit_buy_num"))
			else
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("shop_max_could_buy_num"))
			end
		end
		return
	elseif self.chooseNum + num >= self.max_Num then
		self.chooseNum = self.max_Num			
	else
		self.chooseNum = self.chooseNum + num
	end
	self.num_Text:SetText(self.chooseNum)
	if self.__data.cost_type_list then
		for i=1,#self.__data.cost_type_list do
			self["totalPrice"..i] = self.chooseNum * self.__data.cost_value_list[i]
			self["Text_totalPriceNum"..i]:SetText(self["totalPrice"..i])
		end
	elseif self.__data.cost_item_id_list then
		for i=1,#self.__data.cost_item_id_list do
			self["totalPrice"..i] = self.chooseNum * self.__data.cost_item_num_list[i]
			self["Text_totalPriceNum"..i]:SetText(self["totalPrice"..i])
		end
	end
end

--减少数量
function LogicGUI.TemplIntegralGloryShopBuyCountView:OnBtnSub(num)
	if self.chooseNum <= 1 then
		return 
	elseif self.chooseNum - num <= 1  then
		self.chooseNum = 1
	else 
		self.chooseNum = self.chooseNum - num		
	end
	self.num_Text:SetText(self.chooseNum)
	if self.__data.cost_type_list then
		for i=1,#self.__data.cost_type_list do
			self["totalPrice"..i] = self.chooseNum * self.__data.cost_value_list[i]
			self["Text_totalPriceNum"..i]:SetText(self["totalPrice"..i])
		end
	elseif self.__data.cost_item_id_list then
		for i=1,#self.__data.cost_item_id_list do
			self["totalPrice"..i] = self.chooseNum * self.__data.cost_item_num_list[i]
			self["Text_totalPriceNum"..i]:SetText(self["totalPrice"..i])
		end
	end
end

--计算总数量
function LogicGUI.TemplIntegralGloryShopBuyCountView:CountTotalNum()
	if self.__data.cost_type_list then
		if #self.__data.cost_type_list == 1 then
			local moneyNum = CallModuleFunc(ModuleType.ROLE,"GetPropValue",self.__data.cost_type_list[1])
			self.max_Num = math.floor(moneyNum/self.__data.cost_value_list[1])	
		else
			local moneyNum = CallModuleFunc(ModuleType.ROLE,"GetPropValue",self.__data.cost_type_list[1])
			local moneyNum_1 = CallModuleFunc(ModuleType.ROLE,"GetPropValue",self.__data.cost_type_list[2])
			local max_num_1 = math.floor(moneyNum/self.__data.cost_value_list[1])	
			local max_num_2 = math.floor(moneyNum_1/self.__data.cost_value_list[2])	
			self.max_Num = math.min(max_num_1,max_num_2)
		end
	elseif self.__data.cost_item_id_list then
		if #self.__data.cost_item_id_list == 1 then
			local moneyNum = ObjIDTool.GetOwnNum(self.__data.cost_item_id_list[1])
			self.max_Num = math.floor(moneyNum/self.__data.cost_item_num_list[1])	
		else
			local moneyNum = ObjIDTool.GetOwnNum(self.__data.cost_item_id_list[1])
			local moneyNum_1 = ObjIDTool.GetOwnNum(self.__data.cost_item_id_list[2])
			local max_num_1 = math.floor(moneyNum/self.__data.cost_item_num_list[1])	
			local max_num_2 = math.floor(moneyNum_1/self.__data.cost_item_num_list[2])	
			self.max_Num = math.min(max_num_1,max_num_2)
		end
	end
end

