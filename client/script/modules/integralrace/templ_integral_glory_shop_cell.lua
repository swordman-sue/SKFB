GUI = GUI or {}
GUI.TemplIntegralGloryShopCell = GUI.TemplIntegralGloryShopCell or BaseClass(GUI.Template)

function GUI.TemplIntegralGloryShopCell:__init(data)
	self:InitTemplate(GUI.TemplateRegistry.ARENA_REPUTATIONSHOP_CELL)
	self:createMainUI(data)
end

function GUI.TemplIntegralGloryShopCell:__AddToParent()

end

function GUI.TemplIntegralGloryShopCell:createMainUI(data)
	self.__data = data	
	--根节点监听
	WidgetUnity.SetWidgetListener(self:GetNode(), nil, nil,function ()	
		if self.IsCouldNotBuyAgain then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("shop_today_can_notbuy_tip"))
		else
			LogicGUI.TemplIntegralGloryShopBuyCountView.New(self.__data)
		end	
	end)
	--名字
	local name = nil	
	--头像
	local icon = LogicGUI.IconLayer.New(false,false)
	self:GetWidget("Panel_head"):AddChild(icon:GetNode())
	PosTool.Center(icon)
	icon:SetData({id = data.item_id,num = data.item_num})

	local itemType,config = ObjIDTool.GetInfo(data.item_id)
	if data.item_num ~= 1 then			
		if itemType == Macros.Game.ObjType.HERO then
			name = Util:Name(config.name,Macros.TypefaceSize.largish,config.init_quality)
		else
			name = Util:Name(config.name,Macros.TypefaceSize.largish,config.quality)
		end			
	else
		if itemType == Macros.Game.ObjType.HERO then
			name = Util:Name(config.name,Macros.TypefaceSize.largish,config.init_quality)
		else
			name = Util:Name(config.name,Macros.TypefaceSize.largish,config.quality)
		end			
	end
	self:GetWidget("Panel_name"):AddChild(name)
	PosTool.Center(name)

	--可购次数
	self.__data.history_rank = CallModuleFunc(ModuleType.ARENA,"GetHistoryRank")
	self.Text_moreType = self:GetWidget("Text_couldBuyTimes")
	Util:WidgetLabel(self.Text_moreType,"", Macros.TypefaceSize.normal,Macros.Color.red)
	self.Text_moreType:SetVisible(false)
	--已购买的图标
	self.Image_HaveBuy = self:GetWidget("Image_HaveBuy")
	if data.buy_num_max ~= 0 then
		if data.buy_num_max - data.haveBuyNum > 0 then
			self.Image_HaveBuy:SetVisible(false)
		else
			self.IsCouldNotBuyAgain = true
		end
	else
		self.Image_HaveBuy:SetVisible(false)
	end
	--价格
	--需要一种货币
	self:GetWidget("Image_coinBg_1"):SetVisible(false)
	self:GetWidget("Image_coinBg_2"):SetVisible(false)
	if data.cost_type_list then
		for i=1,#data.cost_type_list do
			self:GetWidget("Image_coinBg_"..i):SetVisible(true)
			local sp = Util:GetMoneyIconImgData(data.cost_type_list[i])
			self:GetWidget("Image_coin_"..i):SetImage(sp)

			Util:WidgetLabel(self:GetWidget("Text_coinNum_"..i),data.cost_value_list[i], Macros.TypefaceSize.normal, Macros.Color.content )
		end
		--居中或者不变
		local widget = self:GetWidget("Image_coinBg_1")
		widget:SetPosition(#data.cost_type_list == 1 and 140 or widget:GetPositionX(),widget:GetPositionY())	
	elseif data.cost_item_id_list then
		--消耗物品
		for i=1,#data.cost_item_id_list do
			self:GetWidget("Image_coinBg_"..i):SetVisible(true)
			local icon_path
			local config = CPPGameLib.GetConfig("Item",data.cost_item_id_list[i])
		    if not config then return "" end
		    if  config.sub_type == Macros.Game.NormalItemSubType.HERODEBRIS then
		      icon_path = Resource.PathTool.GetHeroHeadPath(config.res_id) 
		    else
		      icon_path = Resource.PathTool.GetItemPath(config.res_id)       
		    end
			self:GetWidget("Image_coin_"..i):SetImage(icon_path,TextureResType.LOCAL)

			Util:WidgetLabel(self:GetWidget("Text_coinNum_"..i),data.cost_item_num_list[i], Macros.TypefaceSize.normal, Macros.Color.content )
		end
		--居中或者不变
		local widget = self:GetWidget("Image_coinBg_1")
		widget:SetPosition(#data.cost_item_id_list == 1 and 140 or widget:GetPositionX(),widget:GetPositionY())
	end			
end

function GUI.TemplIntegralGloryShopCell:ResetData(times)
	self.__data.haveBuyNum = self.__data.haveBuyNum + times	
	if self.__data.buy_num_max ~= 0 then
		if self.__data.haveBuyNum >= self.__data.buy_num_max then
			self.Image_HaveBuy:SetVisible(true)
			self.IsCouldNotBuyAgain = true
		end
	end
	if self.NeedUpdateBuyTimes then
		if self.__data.type == Macros.Game.ARENA_SHOP.REPUTATION_SHOP then
			self.Text_moreType:SetText(CPPGameLib.GetString("arena_today_buyTimes",self.__data.buy_num_max - self.__data.haveBuyNum))
		else
			self.Text_moreType:SetText(CPPGameLib.GetString("arena_total_buyTimes",self.__data.buy_num_max - self.__data.haveBuyNum))
		end
	elseif self.NeedUpdatePieceNum then		
		local itemType,config = ObjIDTool.GetInfo(self.__data.item_id)
		local hero_composeConfig = CPPGameLib.GetConfig("HeroCompound",config.target_id)
		local ownNum = ObjIDTool.GetOwnNum(self.__data.item_id)
		self.Text_moreType:SetText(ownNum.."/"..hero_composeConfig.item_num)
	end
end

function GUI.TemplIntegralGloryShopCell:ResetPieceNum()
	if self.NeedUpdatePieceNum then		
		local itemType,config = ObjIDTool.GetInfo(self.__data.item_id)
		local hero_composeConfig = CPPGameLib.GetConfig("HeroCompound",config.target_id)
		local ownNum = ObjIDTool.GetOwnNum(self.__data.item_id)
		self.Text_moreType:SetText(ownNum.."/"..hero_composeConfig.item_num)
	end
end

--新手指引特殊操作检测
function GUI.TemplIntegralGloryShopCell:CheckSectionOperateSP()
	CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.ARENA_REPUTATION_SHOP, self:GetNode(), self.__data.goods_id)				
end