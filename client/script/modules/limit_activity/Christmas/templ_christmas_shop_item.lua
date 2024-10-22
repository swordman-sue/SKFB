Modules = Modules or {}

Modules.TemplChristmasShopItem = Modules.TemplChristmasShopItem or BaseClass(GUI.ScrollItem)


function Modules.TemplChristmasShopItem:__init()
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_CHRISTMAS_SHOP_CELL)
end

function Modules.TemplChristmasShopItem:__delete()
	
end

function Modules.TemplChristmasShopItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	--头像
	local panel_icon = self:GetWidget("Panel_icon")
	local icon = LogicGUI.IconLayer.New()
	panel_icon:AddChild(icon:GetNode())
	PosTool.Center(icon)
	icon:SetData({id = self.__data.item_id,num = self.__data.item_num})
	--物品配置
	local type,item_info = ObjIDTool.GetInfo(self.__data.item_id)
	--名字
	local quality = type == Macros.Game.ObjType.HERO and item_info.init_quality or item_info.quality
	Util:WidgetLabel(self:GetWidget("Text_PropName"), item_info.name, Macros.TypefaceSize.largish, Macros.Quality[quality], 0, 0, Macros.Quality_Stroke[quality])
	--购买按钮
	local btn_buy = self:GetWidget("Button_buy")
	Util:WidgetButton(btn_buy,CPPGameLib.GetString("rebel_mai"),function ()
		--判断活动是否已经结束
		if not CallModuleFunc(ModuleType.LIMIT_ACTIVITY,"GetActivityOpen",Macros.Game.SystemID.CHRISTMAS_ACTIVITY) then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("christmas_activity_over"))
			return
		end
		self:BuySomeThing()
	end, Macros.TypefaceSize.button, Macros.Color.btn, Macros.Color.button_yellow)
	--购买条件
	local txt_conditional = self:GetWidget("Text_conditional")
	txt_conditional:SetVisible(not self.__data.is_buy_full)
	--购买货币
	local image_money = self:GetWidget("Image_money")
	image_money:SetVisible(self.__data.shop_type == Macros.Game.SHOP.CHRISTMAS_PROP)	
	--价格
	local txt_price = self:GetWidget("Text_price")
	txt_price:SetVisible(self.__data.shop_type == Macros.Game.SHOP.CHRISTMAS_PROP)
	if self.__data.shop_type == Macros.Game.SHOP.CHRISTMAS_PROP then
		--货币
		local type,config_item = ObjIDTool.GetInfo(self.__data.cost_item_id_list[1])
		local sp = Resource.PathTool.GetItemPath(config_item.res_id)
		image_money:SetImage(sp,TextureResType.LOCAL)
		--价格
		Util:WidgetLabel(txt_price,self.__data.cost_item_num_list[1], Macros.TypefaceSize.slightly,Macros.Color.btn)
		--
		if self.__data.is_satisfy_level then
			image_money:SetVisible(not self.__data.is_buy_full)
			txt_price:SetVisible(not self.__data.is_buy_full)
		else
			image_money:SetVisible(false)
			txt_price:SetVisible(false)
		end	
	end
	--限购的个数
	local txt_conditionaltwo = self:GetWidget("Text_conditionaltwo")
	txt_conditionaltwo:SetVisible(not self.__data.is_buy_full)
	--销磬
	local txt_buyall = self:GetWidget("Text_buyall")
	Util:WidgetLabel(txt_buyall,CPPGameLib.GetString("christmas_shop_sell_out"), Macros.TypefaceSize.slightly,Macros.Color.Special)	
	txt_buyall:SetVisible(self.__data.is_buy_full)
	--
	local is_gray
	--如果未买完
	if not self.__data.is_buy_full then
		--商店类型
		if self.__data.shop_type == Macros.Game.SHOP.CHRISTMAS_INTEGRAL then
			--是否不满足条件
			if not self.__data.is_satisfy_level then
				is_gray = true
				--不满足等级
				if self.__data.total_score >= self.__data.need_history_score then
					Util:WidgetLabel(txt_conditional,CPPGameLib.GetString("christmas_snowintegral_shop",self.__data.cost_score), Macros.TypefaceSize.slightly,Macros.Color.btn)	
					Util:WidgetLabel(txt_conditionaltwo,CPPGameLib.GetString("christmas_shop_need_level",self.__data.need_role_level), Macros.TypefaceSize.slightly,Macros.Color.Special)	
				else
					Util:WidgetLabel(txt_conditional,CPPGameLib.GetString("christmas_shop_need_level",self.__data.need_role_level), Macros.TypefaceSize.slightly,Macros.Color.Special)		
					txt_conditionaltwo:SetVisible(false)
				end							
			else
				is_gray = self.__data.need_history_score > self.__data.total_score 
				--所需积分
				Util:WidgetLabel(txt_conditional,self.__data.total_score >= self.__data.need_history_score and CPPGameLib.GetString("christmas_snowintegral_shop",self.__data.cost_score) or CPPGameLib.GetString("christmas_shop_needintegral",self.__data.need_history_score),
				 Macros.TypefaceSize.slightly,self.__data.total_score >= self.__data.need_history_score and Macros.Color.btn or Macros.Color.Special)
				Util:WidgetLabel(txt_conditionaltwo,self.__data.buy_num_max == 0 and CPPGameLib.GetString("christmas_shop_nolimit_buytimes") or CPPGameLib.GetString("christmas_shop_buytimes",self.__data.buy_num_max - self.__data.buy_record_num), Macros.TypefaceSize.slightly,Macros.Color.btn)
				txt_conditionaltwo:SetVisible(self.__data.total_score >= self.__data.need_history_score)
			end
		else
			--是否不满足条件
			if not self.__data.is_satisfy_level then
				is_gray = true
				--不满足等级
				Util:WidgetLabel(txt_conditional,CPPGameLib.GetString("christmas_shop_need_level",self.__data.need_role_level), Macros.TypefaceSize.slightly,Macros.Color.Special)
				txt_conditionaltwo:SetVisible(false)
			else
				txt_conditional:SetVisible(false)
				Util:WidgetLabel(txt_conditionaltwo,self.__data.buy_num_max == 0 and CPPGameLib.GetString("christmas_shop_nolimit_buytimes") or CPPGameLib.GetString("christmas_shop_buytimes",self.__data.buy_num_max - self.__data.buy_record_num), Macros.TypefaceSize.slightly,Macros.Color.btn)
			end
		end
	end
	--不满足条件
	if is_gray or self.__data.is_buy_full then
		btn_buy:SetEnabled(false)
		btn_buy:SetTextColor(unpack(Macros.Color.forbidden))	
		local r, g, b, a = unpack(Macros.Color.forbidden_stroke)
		stroke_width = stroke_width or 2
		btn_buy:SetOutline(r, g, b, a, stroke_width)	
	end
end

function Modules.TemplChristmasShopItem:BuySomeThing()
	local could_buy_num,use_num
	local t1 = {}
	local cur_data = {}
	--道具兑换
	if self.__data.shop_type == Macros.Game.SHOP.CHRISTMAS_PROP then
		local have_num = ObjIDTool.GetOwnNum(self.__data.cost_item_id_list[1])
		if have_num < self.__data.cost_item_num_list[1] then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("christmas_shop_noenough_prop"))	
			return
		end
		could_buy_num = have_num/self.__data.cost_item_num_list[1]
		use_num = self.__data.cost_item_num_list[1]
		cur_data.type_id = self.__data.cost_item_id_list[1]
	else
		local integral = CallModuleFunc(ModuleType.LIMIT_ACTIVITY,"GetSnowBallIntegral")
		if integral < self.__data.cost_score then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("christmas_shop_noenough_snowball_integral"))			
			return
		end
		could_buy_num = integral/self.__data.cost_score
		use_num = self.__data.cost_score
		cur_data.sp_txt = "christmas_shop_total_need"
	end
    cur_data.haveBuyNum = self.__data.buy_record_num
    cur_data.num = use_num
    table.insert(t1,cur_data)

    --背包容量
    local item_type = ObjIDTool.GetInfo(self.__data.item_id)

    CallModuleFunc(ModuleType.SUNDRY, "PackVacancy", item_type , 0 , true , function ( sum )
    	self.__pack_nil_num = sum
    end)

    if self.__pack_nil_num <= 0 then
    	return
    end

    if could_buy_num > self.__data.buy_num_max and self.__data.buy_num_max ~= 0 then
        could_buy_num = self.__data.buy_num_max - self.__data.buy_record_num
    end
    could_buy_num = could_buy_num > self.__pack_nil_num and self.__pack_nil_num or could_buy_num
    local info = {}

    --道具id
    info.item_id = self.__data.item_id 
    --商店类型
    info.type = self.__data.shop_type 
    --商品购买id
    info.goods_id = self.__data.goods_id
    --价格列表
    info.cost = t1
    -- 最大
    info.max = could_buy_num
    --图标显示的单价
    info.num = self.__data.item_num or 1

    GUITemplateMgr:CreateTemplateInstance( GUI.TemplateRegistry.SHOP_BUY_TIPVIEW , false , info )
end