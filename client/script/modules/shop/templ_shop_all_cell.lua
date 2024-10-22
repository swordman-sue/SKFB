--[[
%% @module: 商店界面商品条目
%% @author: 
%% @created: 
--]]

GUI = GUI or {}
GUI.TemplShopAllCell = GUI.TemplShopAllCell or BaseClass(GUI.Template)

function GUI.TemplShopAllCell:__init(data)
	self:InitTemplate(GUI.TemplateRegistry.SHOP_COMMON_TOOLS)
	self:SetData(data)
end

function GUI.TemplShopAllCell:__AddToParent()
end

function GUI.TemplShopAllCell:SetData(data)
	self.__data = data

	WidgetUnity.SetWidgetListener(self:GetNode(), nil, nil,function ()
		--已达购买上限
		if self.__data.canBuyNum > 0 and self.__data.canBuyNum <= self.__data.haveBuyNum then
			--普通商店
			if self.__data.shop_type == Macros.Game.SHOP.STOREPROP then
				local item , viplevel , addnum , exp = CallModuleFunc(ModuleType.VIP ,"ToTypeGetShopBuyResetItem" , data.vip_idx )
				if item == true then
					local tips = LogicGUI.TemplAffirmTips.New( { 
																	type = LogicGUI.TemplAffirmTipsType.YES_NO,
																	content	= " ",
																	confirm_text = CPPGameLib.GetString("vip_gorecharge") ,
																	confirm_func = function ( ... )
																		CallModuleFunc(ModuleType.CHARGE, "OpenView", BaseViewType.CHARGE )
																	end,
																	close_func = function ( ... )
																		-- body
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

			--礼包商店
			elseif self.__data.shop_type == Macros.Game.SHOP.STOREOEM then
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("shop_gift_only_buy_once"))
			
			--GM商店
			elseif self.__data.shop_type == Macros.Game.SHOP.GM_SHOP then
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("shop_today_can_notbuy_tip"))
			end

		--可购买
		else
			LogicGUI.TemplShopBuyCountView.New(self.__data)
		end
	end)
	
	self.Panel_head = self:GetWidget("Panel_head")

	--商品名称
	local name
	local icon = LogicGUI.IconLayer.New(false,false)
	self.Panel_head:AddChild(icon:GetNode())
	PosTool.Center(icon)

	local obj_type, config_item = ObjIDTool.GetInfo(self.__data.item_id)

	--普通商店、GM商店
	if self.__data.shop_type == Macros.Game.SHOP.STOREPROP or self.__data.shop_type == Macros.Game.SHOP.GM_SHOP then
		icon:SetData({id = self.__data.item_id, num = self.__data.item_num ~= 1 and self.__data.item_num or nil})

		--商品名称
		name = Util:Name(config_item.name, Macros.TypefaceSize.largish, config_item.quality or config_item.init_quality)

		--商品价格
		local img_currency = self:GetWidget("Image_coin")
		self.__text_cost_value = self:GetWidget("Text_coinNum")
		img_currency:SetImage(Util:GetMoneyIconImgData(self.__data.cost_type))
		Util:WidgetLabel(self.__text_cost_value, self.__data.cost_value, Macros.TypefaceSize.normal, Macros.Color.btn)

		--可购次数
		self.__text_couldbuytimes = self:GetWidget("Text_couldBuyTimes")	
		Util:WidgetLabel(self.__text_couldbuytimes,"",Macros.TypefaceSize.normal, Macros.Color.red )	
		if self.__data.canBuyNum > 0 then
			if self.__data.canBuyNum <= self.__data.haveBuyNum then
				self.__text_couldbuytimes:SetColor(unpack( Macros.Color.red ))
				self.__text_couldbuytimes:SetText(CPPGameLib.GetString("shop_today_can_notbuy"))
			else
				self.__text_couldbuytimes:SetColor(unpack( Macros.Color.content ))
				self.__text_couldbuytimes:SetText(CPPGameLib.GetString("shop_today_couldBuyTimes",self.__data.canBuyNum - self.__data.haveBuyNum))
			end					
		else
			self.__text_couldbuytimes:SetVisible(false)
		end

		--隐藏其他控件
		self:GetWidget("Image_vip"):SetVisible(false)
		self:GetWidget("Image_HaveBuy"):SetVisible(false)		
		self:GetWidget("Panel_before"):SetVisible(false)
		self:GetWidget("Panel_after"):SetVisible(false)
	
	--礼包商店
	elseif self.__data.shop_type == Macros.Game.SHOP.STOREOEM then
		icon:SetData({id = self.__data.item_id})

		--商品名称
		name = Util:Name(config_item.name,Macros.TypefaceSize.largish,config_item.quality)

		--原价
		Util:WidgetLabel(self:GetWidget("Text_before"),CPPGameLib.GetString("shop_before_price"), Macros.TypefaceSize.normal, Macros.Color.keypoint )
		Util:WidgetLabel(self:GetWidget("Text_beforeNum"),self.__data.pur_price, Macros.TypefaceSize.normal, Macros.Color.content )
		local sp = Util:GetMoneyIconImgData(self.__data.cost_type)
		self:GetWidget("Image_beforeCoin"):SetImage(sp)
		self:GetWidget("Image_beforeCoin"):SetPosition(self:GetWidget("Text_beforeNum"):GetContentWidth() + self:GetWidget("Text_beforeNum"):GetPositionX() + 8,self:GetWidget("Image_beforeCoin"):GetPositionY())
		
		--现价
		Util:WidgetLabel(self:GetWidget("Text_after"),CPPGameLib.GetString("shop_after_price"), Macros.TypefaceSize.normal, Macros.Color.keypoint )
		Util:WidgetLabel(self:GetWidget("Text_afterNum"),self.__data.cost_value, Macros.TypefaceSize.normal, Macros.Color.content )
		local sp = Util:GetMoneyIconImgData(self.__data.cost_type)
		self:GetWidget("Image_afterCoin"):SetImage(sp)
		self:GetWidget("Image_afterCoin"):SetPosition(self:GetWidget("Text_afterNum"):GetContentWidth() + self:GetWidget("Text_afterNum"):GetPositionX() + 8,self:GetWidget("Image_afterCoin"):GetPositionY())

        self.__image_vip = self:GetWidget("Image_vip")

		if self.__data.canBuyNum <= 0 or self.__data.canBuyNum > self.__data.haveBuyNum then
			--VIP购买条件
			self.__text_vip = self:GetWidget("Text_vip")
			local vipLevel = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.VIP_LEVEL)
			if vipLevel >= self.__data.need_vip_level then
		        self.__vip_effect_id , effect = self:__CreateEffect({res_name = "UI_taozhuang" }, icon:GetNode() )
		        effect:SetPosition( icon:GetNode():GetContentWidth()/2 - 1 , icon:GetNode():GetContentHeight()/2 ) 
		        effect:SetScaleFactor(1.05)
		        effect:SetZOrder(999)

	    		--VIP
				Util:WidgetLabel(self.__text_vip, "" , Macros.TypefaceSize.slightly, Macros.Color.golden)
		        self.__vip_libao_effectid , effect = self:__CreateEffect({res_name = "UI_viplibao_language_chs" }, self:GetWidget("Text_vip") )
		        effect:SetAnchorPoint(0.5,0.5)
		        effect:SetPosition(self:GetWidget("Text_vip"):GetContentWidth()/2-45, self:GetWidget("Text_vip"):GetContentHeight()/2+3) 
		        effect:SetRotation(45)
		        effect:SetZOrder(999)
			else
				--VIP
				Util:WidgetLabel(self.__text_vip, CPPGameLib.GetString("shop_vip_limit_buy", self.__data.need_vip_level), Macros.TypefaceSize.slightly, Macros.Color.golden)
		    end

		    --装饰
		    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY, "vip_oem_1" , true)
		    self.__image_vip_oem = Util:Sprite( sp )
		    self.__image_vip_oem:SetAnchorPoint( 0.5 , 0.5 )
		    self.__image_vip_oem:SetPosition( icon:GetNode():GetPositionX() + 80 , icon:GetNode():GetPositionY() - 20 )
		    self.Panel_head:AddChild( self.__image_vip_oem )
			local cnf = CPPGameLib.GetConfig("Vip", self.__data.need_vip_level , false )
			if cnf then
				local lst = CPPGameLib.SplitEx(cnf.tips , "<GH>")

				local __VipOem_t1 = Util:Name( lst[1] , Macros.TypefaceSize.slightly ,100 )
				__VipOem_t1:SetPosition( self.__image_vip_oem:GetContentWidth()/2 + 6 , self.__image_vip_oem:GetContentHeight()/2 + 14 )
				self.__image_vip_oem:AddChild( __VipOem_t1 )

				local __VipOem_t2 = Util:Name( lst[2] , Macros.TypefaceSize.slightly ,100 )
				__VipOem_t2:SetPosition( self.__image_vip_oem:GetContentWidth()/2 + 6 , self.__image_vip_oem:GetContentHeight()/2 - 14 )
				self.__image_vip_oem:AddChild( __VipOem_t2 )

				ActionManager:ZoomAnimation(self.__image_vip_oem,1, 1.02 , 0.98 )
				self.__image_vip_oem:SetVisible(true)
			end		

			self:GetWidget("Image_HaveBuy"):SetVisible(false)	

		else
			self.__image_vip:SetVisible(false)
			self:GetWidget("Image_HaveBuy"):SetVisible(true)
		end
		
		--隐藏其他控件
		self:GetWidget("Image_coinBg"):SetVisible(false)
		self:GetWidget("Text_couldBuyTimes"):SetVisible(false)
	end

	--商品名称
	self:GetWidget("Panel_name"):AddChild(name)
	PosTool.Center(name)

	--折扣图标	
	self:__ShowDiscount()
end

function GUI.TemplShopAllCell:ResetData()
	--普通商店、GM商店
	if self.__data.shop_type == Macros.Game.SHOP.STOREPROP or self.__data.shop_type == Macros.Game.SHOP.GM_SHOP then
		if self.__data.shop_type == Macros.Game.SHOP.STOREPROP then
			self.__text_cost_value:SetText(self.__data.cost_value)
		end

		if self.__data.canBuyNum > 0 then
			if self.__data.canBuyNum <= self.__data.haveBuyNum then
				self.__text_couldbuytimes:SetColor(unpack( Macros.Color.red))
				self.__text_couldbuytimes:SetText(CPPGameLib.GetString("shop_today_can_notbuy"))
			else
				self.__text_couldbuytimes:SetColor(unpack( Macros.Color.content ))
				self.__text_couldbuytimes:SetText(CPPGameLib.GetString("shop_today_couldBuyTimes",self.__data.canBuyNum - self.__data.haveBuyNum))
			end					
		else
			self.__text_couldbuytimes:SetVisible(false)
		end

	--礼包商店
	else
		self.__image_vip:SetVisible(false)
		self.__image_vip_oem:SetVisible(false)
		self:GetWidget("Image_HaveBuy"):SetVisible(true)

		if self.__vip_libao_effectid then
			self:__DelEffect(self.__vip_libao_effectid)
			self.__vip_libao_effectid = nil
		end
		if self.__vip_effect_id then
			self:__DelEffect(self.__vip_effect_id)
			self.__vip_effect_id = nil
		end
	end

	--折扣图标	
	self:__ShowDiscount()
end

--显示折扣图标
function GUI.TemplShopAllCell:__ShowDiscount()
	if self.__img_discount then
		self.__img_discount:RemoveSelf(true)
		self.__img_discount = nil
	end

    if self.__data.discount then
    	local num = math.ceil( (self.__data.cost_value / self.__data.discount) * 10 )
    	if num < 10 then
            self.__img_discount = LogicGUI.LconDiscount.New( num )
            self.__img_discount:SetAnchorPoint(0.5, 0.5)
            self.Panel_head:AddChild(self.__img_discount:GetNode())
            PosTool.LeftTop(self.__img_discount , - 10 , -20)
        end
    end
end