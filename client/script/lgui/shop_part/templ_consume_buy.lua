--[[
%% @module: 消耗品购买
%% @author: yjg
%% @created: 2017年3月23日16:14:39
--]]

--id = 道具id  or 货币类型
--func = 回调

LogicGUI = LogicGUI or {}

LogicGUI.TemplConsumeBuy = LogicGUI.TemplConsumeBuy or BaseClass(EventAndTimerWrapper)

function LogicGUI.TemplConsumeBuy:__init( id ,func )
	self.func = func

	--获得货币配置
	self.__config_currency = CPPGameLib.GetConfig("HuoBi", id , false)
	if not self.__config_currency then
		for _, v in pairs( CPPGameLib.GetConfig( "HuoBi" , nil , nil, true) ) do
			if v.item_id and v.item_id == id then
				self.__config_currency = v
				break
			end
		end
	end
	if not self.__config_currency then
		return
	end

	local item_num = CallModuleFunc(ModuleType.Pack, "GetNumByItemID", self.__config_currency.item_id)
	if item_num and item_num ~= 0 then
		local onuse = GUITemplateMgr:CreateTemplateInstance(
																GUI.TemplateRegistry.PACK_ONEUSE , 
																false , self.__config_currency.item_id , 
																nil,nil, 
																function ( ... )
																	self:DeleteMe()
																end )
	else
		CallModuleFunc(ModuleType.SHOP, "ShopBuyRecordRequest", Macros.Game.SHOP.STOREPROP)
	end

	self:BindGlobalEvent(Macros.Event.ShopModule.RECORD_LIST, function ( _, type_ )
		if type_ == Macros.Game.SHOP.STOREPROP then
			self.__good_data = CallModuleFunc(ModuleType.SHOP, "GetGoodDataByItemID", Macros.Game.SHOP.STOREPROP, nil, self.__config_currency.item_id)
			if not self.__good_data then
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("shop_buy_invalid_good"))
				return
			end

			self:Purchase()
		end
	end)
end

function LogicGUI.TemplConsumeBuy:__delete( )
	if self.func then
		self.func()
	end
end

--购买弹窗
function LogicGUI.TemplConsumeBuy:Purchase()
	local t1 = {}
	table.insert(t1,{ 
						type = self.__good_data.cost_type , 
						price_list = self.__good_data.price_list ,
						haveBuyNum = self.__good_data.haveBuyNum
					})

	local info = {}
		-- 道具id
	info.item_id = self.__config_currency.item_id
		-- 商店类型
	info.type = Macros.Game.SHOP.STOREPROP
		-- 商品购买id
	info.goods_id = self.__good_data.goods_id
		-- 价格列表
	info.cost = t1
		-- 最大
	info.max = self.__good_data.canBuyNum - self.__good_data.haveBuyNum 

	-- 2658 从资源栏进入购买金币显示数量不对，显示1W 实际是10W 10W是对的
	-- 2017年10月13日15:37:45
	info.num = self.__good_data.item_num

	if info.max ~= 0 then
		local shopBuy = GUITemplateMgr:CreateTemplateInstance( GUI.TemplateRegistry.SHOP_BUY_TIPVIEW , false , info , function ( ... )
			self:DeleteMe()
		end )
	else
		local item , viplevel , addnum , exp = CallModuleFunc(ModuleType.VIP ,"ToTypeGetShopBuyResetItem" , self.__good_data.vip_idx )
		if item == true then

			local tips = LogicGUI.TemplAffirmTips.New( { 
															type = LogicGUI.TemplAffirmTipsType.YES_NO,
															content	= " ",
															confirm_text = CPPGameLib.GetString("vip_gorecharge") ,
															confirm_func = function ( ... )
																CallModuleFunc(ModuleType.CHARGE, "OpenView", BaseViewType.CHARGE )
																self:DeleteMe()
															end,
															close_func = function ( ... )
																-- body
																self:DeleteMe()
															end,
														} )
			tips:SetLoadedCallback(function()
				local Affirm = Util:LabCenter( { 
												{ zi = CPPGameLib.GetString("shop_buy_1") , ys = Macros.Color.content },
												{ tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.DIAMOND) , jg = 5 ,dx = 0.8},
												{ zi = exp , ys = Macros.Color.content },
												{ zi = CPPGameLib.GetString("shop_buy_2" , viplevel , addnum ) , ys = Macros.Color.content },
											} )
				Affirm:SetPosition(tips.txt_content:GetContentWidth()/2 , tips.txt_content:GetContentHeight()/2 + 25 )
				tips.txt_content:AddChild(Affirm)

	            local arena_today_buyTimes = Util:Label( CPPGameLib.GetString("shop_buy_3") , Macros.TypefaceSize.normal , Macros.Color.content )
	            arena_today_buyTimes:SetAnchorPoint(0.5,0.5)
	            arena_today_buyTimes:SetPosition(tips:GetNode():GetContentWidth()/2 , tips:GetNode():GetContentHeight()/2 - 25 )
	            tips:GetNode():AddChild(arena_today_buyTimes)
			end)
		else
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("shop_today_can_notbuy_tip"))
		end
	end
end

--[[
%% @module: 上限购买
%% @author: yjg
%% @created: 2017年8月16日11:03:53
--]]

LogicGUI = LogicGUI or {}

LogicGUI.TemplBuyBagMax = LogicGUI.TemplBuyBagMax or BaseClass(GUI.Template)

function LogicGUI.TemplBuyBagMax:__init( type )
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__fb_type = type
	self.__open_callback = function ()
		self:__ShowUI() 
	end	
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_AFFIRM_TIPS)	
end

function LogicGUI.TemplBuyBagMax:__Getwidget()

end

function LogicGUI.TemplBuyBagMax:__ShowUI()
	--标题
	self.txt_headline = Util:WidgetLabel(self:GetWidget("Text_headline"), CPPGameLib.GetString("common_systemTips") , Macros.TypefaceSize.popup, Macros.Color.lilyWhite)
	--内容
	local img_content = self:GetWidget("Image_lstBg")
	self.txt_content = Util:WidgetLabel(self:GetWidget("Text_content"), "" , nil , Macros.Color.content , img_content:GetContentWidth() - 12)
	
	local lv , max = CallModuleFunc(ModuleType.VIP, "ExtendPack", self.__fb_type )
	if lv or max then
		local text = CPPGameLib.GetString("vip_buyBagTips" , lv , max )
		local vip_buyBagTips = Util:RichText( text , 
								  Macros.TypefaceSize.largish , 
								 img_content:GetContentWidth() - 12 , 0 )
		self.txt_content:AddChild( vip_buyBagTips )
		PosTool.Center( vip_buyBagTips )
	else
		local vip_buyBagTips = Util:Label( CPPGameLib.GetString("equip_toMax") , Macros.TypefaceSize.normal , Macros.Color.content )
		self.txt_content:AddChild( vip_buyBagTips )
		PosTool.Center( vip_buyBagTips )
	end

	--关闭按钮
	self.btn_close = Util:WidgetButton( self:GetWidget("Button_close") , " " , CPPGameLib.Handler(self,self.OnBtnClose) )
	
	--确定按钮
	self.btn_yes = Util:WidgetButton( 
										self:GetWidget("Button_yes") , 
										CPPGameLib.GetString("common_pay") , 
										CPPGameLib.Handler(self,self.OnButtonYes) , 
										Macros.TypefaceSize.largish , nil , Macros.Color.button_red )


	--取消按钮
	self.btn_no = Util:WidgetButton( 
										self:GetWidget("Button_no") ,
										CPPGameLib.GetString("common_noPay") , 
										CPPGameLib.Handler(self,self.OnButtonNo) , 
										Macros.TypefaceSize.largish , nil , Macros.Color.button_yellow )
end

function LogicGUI.TemplBuyBagMax:OnButtonYes()
	CallModuleFunc(ModuleType.CHARGE, "OpenView", BaseViewType.CHARGE)
	self:OnBtnClose()
end

function LogicGUI.TemplBuyBagMax:OnButtonNo()
	self:OnBtnClose()
end

function LogicGUI.TemplBuyBagMax:OnBtnClose()
	self:DeleteMe()
end