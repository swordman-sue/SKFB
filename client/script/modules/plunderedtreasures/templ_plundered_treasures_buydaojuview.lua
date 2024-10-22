LogicGUI = LogicGUI or {}

LogicGUI.TemplPlunderedTreasuresBuyDaoJuView  = LogicGUI.TemplPlunderedTreasuresBuyDaoJuView or BaseClass(GUI.Template)

function LogicGUI.TemplPlunderedTreasuresBuyDaoJuView:__init(info_id)
	self.__is_auto_load = true
	self.__open_callback = function ()
		self:initUI(info_id)
	end
	self:InitTemplate(GUI.TemplateRegistry.PLUNDERED_TREASURES_BUYDAOJUVIEW)	
end

function LogicGUI.TemplPlunderedTreasuresBuyDaoJuView:initUI(info_id)
	--购买成功
	self.__buyShopItemSuccessEvent = self:BindGlobalEvent(Macros.Event.ShopModule.BUY_SUCCESS, function()
      	self.__itemNum = self.__itemNum + 1
		self:GetWidget("Text_have"):SetText(CPPGameLib.GetString("plunderedtreasures_haveAvoidWarItemNum",self.__itemNum))
    end)
    --使用成功
    self.__useShopItemSuccessEvent = self:BindGlobalEvent(Macros.Event.PackModule.USE_ITEM_SUCCESS, function()
      	CallModuleFunc(ModuleType.PLUNDEREDTREASURES,"plunderedTreasuresDataReq")
		self:DeleteMe()
    end)
	local shop_Info
	for i,info in ipairs(CPPGameLib.GetConfig("ItemShop",nil,nil,true)) do
		if info.item_id == info_id then
			shop_Info = info
			break
		end
	end
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("plunderedtreasures_AvoidWarTitle_1"), Macros.TypefaceSize.popup,Macros.Color.btn)
	--关闭按钮
	Util:WidgetButton(self:GetWidget("Button_close"), "",function ()
		self:DeleteMe()
	end)
	--当前拥有
	Util:WidgetLabel(self:GetWidget("Text_haveGone"),CPPGameLib.GetString("plunderedtreasures_AvoidWarText_9"), Macros.TypefaceSize.slightly,Macros.Color.keypoint)
	--当前拥有数量
	self.__itemNum = ObjIDTool.GetOwnNum(shop_Info.item_id)
	local txt_have = self:GetWidget("Text_have")
	Util:WidgetLabel(txt_have,CPPGameLib.GetString("plunderedtreasures_haveAvoidWarItemNum",self.__itemNum),Macros.TypefaceSize.slightly,Macros.Color.content)
	--价格
	Util:WidgetLabel(self:GetWidget("Text_coinNum"),shop_Info.price_list[1],Macros.TypefaceSize.slightly,Macros.Color.content)	
	--消耗
	Util:WidgetLabel(self:GetWidget("Text_cost"),CPPGameLib.GetString("plunderedtreasures_AvoidWarText_8"),Macros.TypefaceSize.slightly,Macros.Color.keypoint)	
	--免战时间
	local itemConfig = CPPGameLib.GetConfig("Item",shop_Info.item_id)
	if info_id == ConfigAdapter.Common.GetAvoidWarCardBigId() then
		Util:WidgetLabel(self:GetWidget("Text_avoidWarTime"),CPPGameLib.GetString("plunderedtreasures_AvoidWarText_5"), Macros.TypefaceSize.normal,Macros.Quality[itemConfig.quality],0,0,Macros.Quality_Stroke[itemConfig.quality])
	else
		Util:WidgetLabel(self:GetWidget("Text_avoidWarTime"),CPPGameLib.GetString("plunderedtreasures_AvoidWarText_6"), Macros.TypefaceSize.normal,Macros.Quality[itemConfig.quality],0,0,Macros.Quality_Stroke[itemConfig.quality])
	end
	--头像	
	local Panel_head = self:GetWidget("Panel_head")
	local icon = LogicGUI.IconLayer.New(true,false)
	Panel_head:AddChild(icon:GetNode())
	PosTool.Center(icon)
	icon:SetData({id = shop_Info.item_id})
	icon:adaptParentContentSize()
	--购买
	Util:WidgetButton(self:GetWidget("Button_buy"),CPPGameLib.GetString("plunderedtreasures_AvoidWarText_10"),function ( )
		local diamondNum = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.DIAMOND)
		if diamondNum < shop_Info.price_list[1] then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("plunderedtreasures_AvoidWarText_14"))
			return
		end
		local info = {}
		info.type = 1
		info.param1 = shop_Info.goods_id
		info.param2 = 1
		CallModuleFunc(ModuleType.SHOP,"ShopBuyRequest",info)		
	end,Macros.TypefaceSize.button,Macros.Color.btn,Macros.Color.button_red)
	--使用
	Util:WidgetButton(self:GetWidget("Button_use"),CPPGameLib.GetString("plunderedtreasures_AvoidWarText_11"),function ( )
		if self.__itemNum > 0 then
			local info = {}
			info.item_id = shop_Info.item_id
			info.item_num = 1
			info.select_gift_id = 0
			CallModuleFunc(ModuleType.Pack,"ItemUseReq",info)			
		else
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("plunderedtreasures_AvoidWarText_12"))
		end
	end,Macros.TypefaceSize.button,Macros.Color.btn,Macros.Color.button_yellow)
end

function LogicGUI.TemplPlunderedTreasuresBuyDaoJuView:__delete()

end