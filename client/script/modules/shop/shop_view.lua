--[[
%% @module: 商城界面(普通商店、礼包商店、GM商店)
%% @author: swordman sue
%% @created: 2019-3-21
--]]

Modules = Modules or {} 

Modules.ShopView = Modules.ShopView or BaseClass(BaseView)

--选项卡类
local ShopTabItem

--商品页面类
local ShopGoodsPage

function Modules.ShopView:__init()
	self.__layout_name = "store"	
	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
	self.__title_bar_params = 
	{
		title = CPPGameLib.GetString("shop_title"),
		show_diamond = true,
		show_gold = true,
		show_stamina = true,
		show_energy = true,
		close_func = function()
			self:CloseManual()
		end,
	}

    self.__open_callback = function()   	
    	self:__OpenCallback()
    end

    self.__loaded_callback = function()
    	self:__LoadedCallback()
    end

    self:__AddPreloadTemplate(GUI.TemplateRegistry.SHOP_COMMON_TOOLS)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.STORE)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.STORE, true)
end

function Modules.ShopView:__LoadedCallback()
	--充值按钮
	local btn_money = self:GetWidget("Button_money")
	Util:WidgetButton(btn_money,CPPGameLib.GetString("shop_pay_money"),function ()
			CallModuleFunc(ModuleType.VIP, "OpenView", BaseViewType.VIP)
	end,Macros.TypefaceSize.popup,Macros.Color.btn, Macros.Color.button_yellow )

	--后退
	self.__img_back = self:GetWidget("Image_back")
	WidgetUnity.SetWidgetListener(self.__img_back, nil, nil,function ()
		local page_view = self.__goods_page_view_map[self.__cur_type]
		local page_index = page_view:GetCurPageIndex()
		page_view:ScrollToPageByTime(page_index - 1, 0.5)
	end)

	local effect = nil
	self.effectid_1 ,effect = self:__CreateEffect({res_name = "UI_shangdianjiantou" } , self.__img_back , 999 )
	effect:SetAnchorPoint(0.5,0.5)
	effect:SetPosition( self.__img_back:GetContentWidth()/2 , self.__img_back:GetContentHeight()/2 )
    effect:SetScaleXFactor( -1 )

	--前进
	self.__img_front = self:GetWidget("Image_front")
	WidgetUnity.SetWidgetListener(self.__img_front, nil, nil,function ()
		local page_view = self.__goods_page_view_map[self.__cur_type]
		local page_index = page_view:GetCurPageIndex()
		page_view:ScrollToPageByTime(page_index + 1, 0.5)
	end)

    local effect = nil
    self.effectid_2 ,effect = self:__CreateEffect({res_name = "UI_shangdianjiantou" } , self.__img_front , 999 )
    effect:SetAnchorPoint(0.5,0.5)
    effect:SetPosition( self.__img_front:GetContentWidth()/2 , self.__img_front:GetContentHeight()/2 )

    self.__goods_page_view_container_map = {
    	[Macros.Game.SHOP.STOREPROP] = self:GetWidget("Panel_tool"),
    	[Macros.Game.SHOP.STOREOEM] = self:GetWidget("Panel_gift"),
    	[Macros.Game.SHOP.GM_SHOP] = self:GetWidget("Panel_gm"),
	}
end

function Modules.ShopView:__OpenCallback()  
	local tab_index = unpack(self.__open_data)

   --商品PageView列表(普通、礼包、GM)
   self.__goods_page_view_map = {}

   --商品条目表
   self.__goods_item_map = {}

   --初始化选项卡
   self:__InitTab(tab_index)

   self:__ShowGiftTabReddot()
end

function Modules.ShopView:__RegistAllEvents()
	--购买记录反馈
    self:BindGlobalEvent(Macros.Event.ShopModule.RECORD_LIST, function(_, type, sub_type)
		if type == self.__cur_type then
	    	self:__InitGoodsPageView(type, sub_type)
	    end
    end)

	--购买成功
    self:BindGlobalEvent(Macros.Event.ShopModule.BUY_SUCCESS, function( _ , info )
    	--更新商品
	    local page_view = self.__goods_page_view_map[info.type]
	    if page_view then
	    	local page = page_view:GetCurContentPage()
	    	if page then
	    		page:HandleBuySuccess(info)
	    	end
	    end

	    --更新礼包选项卡红点
    	if info.type == Macros.Game.SHOP.STOREOEM then
	    	self:__ShowGiftTabReddot()
	    end	    
    end)

	--零点刷新
    self:BindGlobalEvent(Macros.Event.SundryModule.NEW_DAY_NOTIFY, function()
   		self:CallModuleFunc("ShopBuyRecordRequest", self.__cur_type, self:__GetSubTypeByTabIndex(self.__tab_index))
   end)    
end

function Modules.ShopView:__Dispose()
	if self.__tab_scv then
		self.__tab_scv:DeleteMe()
		self.__tab_scv = nil
	end

	if self.__goods_page_view_map then
		for _, page_view in pairs(self.__goods_page_view_map) do
			page_view:DeleteMe()
		end
		self.__goods_page_view_map = nil
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--初始化选项卡
function Modules.ShopView:__InitTab(tab_index)
    local panel_tab = self:GetWidget("Panel_tab")

	if not self.__tab_scv then    
		local params =  
		{
		    item_class = ShopTabItem,
		    item_width = 151 ,
		    item_height = 56 ,
		    item_space_c = 10,
		    row = 1,
		    col = 5,		    
		    horizon = true,
			view_width = panel_tab:GetContentWidth(),
		    selectable = true,
		    select_callback = function(index, data)
		    	self:__TabChange(index, data)
		    end,
		}

		local tab_data_list
		if GlobalCustomizedLogicAdapterMgr:Call("CheckGMShopEnabled") then
			tab_data_list = GlobalCustomizedLogicAdapterMgr:Call("CreateGMShopTabDataList")

		else
			tab_data_list = {	
				{name = CPPGameLib.GetString("shop_tools")},
				{name = CPPGameLib.GetString("shop_gifts")},
			}
		end

		self.__tab_scv = GUI.ScrollView.New(params)
		self.__tab_scv:SetDataList(tab_data_list)
		self.__tab_scv:SetScissorSize(panel_tab:GetContentWidth(), panel_tab:GetContentHeight() + 40)

		panel_tab:AddChild(self.__tab_scv:GetNode())
		PosTool.Center(self.__tab_scv, 0, -5)
	end

    self.__tab_scv:SelectItem(tab_index or 1) 
end

--选项卡选中回调
function Modules.ShopView:__TabChange(index, data)
	self.__cur_type = self:__GetTypeByTabIndex(index)
	
	--设置商品PageView显隐
	for k, page_view_container in pairs(self.__goods_page_view_container_map) do
		page_view_container:SetVisible(k == self.__cur_type)
	end

	if self.__cur_type == Macros.Game.SHOP.GM_SHOP or not self.__goods_page_view_map[self.__cur_type] then
		self:CallModuleFunc("ShopBuyRecordRequest", self.__cur_type, nil, self:__GetSubTypeByTabIndex(index))			
	else
		self:__ShowTurnPageArrow()	

		local page_view = self.__goods_page_view_map[self.__cur_type]
		local page = page_view:GetCurContentPage()
		if page then
			page:ShowFadeinEffect()
		end		
	end
end

--初始化商品页面
function Modules.ShopView:__InitGoodsPageView(type, sub_type)
	sub_type = sub_type or 1

	--构造页面数据
	local goods_list = self:CallModuleFunc("GetGoodsListByType", type, sub_type)
	local goods_num_per_page = 6
	local cur_page_data
	local pages_data_list = {}
	for i, good_data in ipairs(goods_list) do
		if not cur_page_data or #cur_page_data.goods_list >= goods_num_per_page then
			cur_page_data = {}
			cur_page_data.page_index = math.floor(i / goods_num_per_page) + 1
			cur_page_data.goods_list = {}
			table.insert(pages_data_list, cur_page_data)
		end
		if good_data.canBuyNum == 0 or good_data.canBuyNum > good_data.haveBuyNum then
			table.insert(cur_page_data.goods_list, good_data)
		end
	end

	--创建商品页面
	local page_view = self.__goods_page_view_map[type]
	if not page_view then
		page_view = GUI.PageView.New({
			view_width = 848, 
			view_height = 400, 
			page_class = ShopGoodsPage,
			turn_page_func = function ()
				self:__ShowTurnPageArrow()
			end})
		self.__goods_page_view_container_map[type]:AddChild(page_view:GetNode())
		PosTool.Center(page_view)	

		self.__goods_page_view_map[type] = page_view
	end
	page_view:SetDataList(pages_data_list)
	page_view:SetTouchEnabled(not CallModuleFunc(ModuleType.LEAD, "IsInLead"))

	self:__ShowTurnPageArrow()	

	--当前页淡入效果
	local page_view = self.__goods_page_view_map[type]
	local page = page_view:GetCurContentPage()
	if page then
		page:ShowFadeinEffect()
	end			
end

--显示翻页箭头
function Modules.ShopView:__ShowTurnPageArrow()
	local page_view = self.__goods_page_view_map[self.__cur_type]
	local page_index = page_view:GetCurPageIndex()
   	self.__img_back:SetVisible(page_index ~= 0)
   	self.__img_front:SetVisible(page_index ~= page_view:GetPageNum() - 1)
end

--更新礼包选项卡红点
function Modules.ShopView:__ShowGiftTabReddot()
	if GlobalCustomizedLogicAdapterMgr:Call("CheckGMShopEnabled") then
		return
	end

	local tab_item = self.__tab_scv:GetItem(Macros.Game.SHOP.GM_SHOP)
	if tab_item then
		tab_item:ShowReddot()
	end
end

--根据选项卡索引获取类型
function Modules.ShopView:__GetTypeByTabIndex(tab_index)
	if not GlobalCustomizedLogicAdapterMgr:Call("CheckGMShopEnabled") then
		return tab_index
	end

	if tab_index ~= Macros.Game.SHOP.STOREPROP then
		return Macros.Game.SHOP.GM_SHOP
	end
	return Macros.Game.SHOP.STOREPROP
end

--根据选项卡索引获取子类型
function Modules.ShopView:__GetSubTypeByTabIndex(tab_index)
	if not GlobalCustomizedLogicAdapterMgr:Call("CheckGMShopEnabled") then
		return 1
	end

	return tab_index - 1
end

--------------------------------------------------------------------------------------------------------------------------------
--选项卡类
--------------------------------------------------------------------------------------------------------------------------------
ShopTabItem = ShopTabItem or BaseClass(GUI.ScrollItem)

function ShopTabItem:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)
end

function ShopTabItem:__delete()
end

function ShopTabItem:__CreateWidgetFromLuaCode()
	local layout = Util:Layout(151, 56)
	return layout
end

function ShopTabItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)

	if not self.__btn then
		self.__btn = Util:Button(
			Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no"), 
			nil, self.__data.name, Macros.TypefaceSize.tab, Macros.Color.btn, Macros.Color.button_yellow)
		self.__btn:SetAnchorPoint(0.5, 0.5)
		self:AddChild(self.__btn)
		PosTool.Center(self.__btn)
	end

	if not self.__btn_line then
		self.__btn_line = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp_cbox_wire"))
		self.__btn_line:SetAnchorPoint(0.5, 1)
		self.__btn_line:SetVisible(false)
		self:AddChild(self.__btn_line)
		PosTool.CenterBottom(self.__btn_line, 0, 5)
	end
end

function ShopTabItem:GetTouchNode()
	return self.__btn
end

function ShopTabItem:SetSelected(var)
	self.__btn:SetNormalImage(var and 
		Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse") or 
		Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no"))

	self.__btn_line:SetVisible(var)
end

function ShopTabItem:ShowReddot()
	if not self.__reddot then
		local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
		self.__reddot = Util:Sprite(icon_red_point)
		self.__reddot:SetAnchorPoint(0.5 , 0.5)
		self.__reddot:SetVisible(false)
		self:AddChild(self.__reddot)
		PosTool.RightTop(self.__reddot, -10, -10)
	end

	local list = self:CallModuleFunc("GetPurchasableGiftList")
	self.__reddot:SetVisible(#list > 0)
end

--------------------------------------------------------------------------------------------------------------------------------
--商品页面类
--------------------------------------------------------------------------------------------------------------------------------
ShopGoodsPage = ShopGoodsPage or BaseClass(UILayer)

function ShopGoodsPage:__init()
	self.__goods_item_map = {}
	self.__fadein_goods_id_list = {}
	self:SetContentSize(848, 400)
end

function ShopGoodsPage:__delete()
	if self.__fadein_effect_timer then
		GlobalTimerQuest:CancelQuest(self.__fadein_effect_timer)
		self.__fadein_effect_timer = nil
	end

	for _, item in pairs(self.__goods_item_map) do
		item:DeleteMe()
	end
	self.__goods_item_map = nil
end

function ShopGoodsPage:SetData(data)
	self.__data = data

	local page = nil
	local item_index = 0
	for i, good_data in ipairs(data.goods_list) do
		item_index = item_index + 1
		
		local good_item = GUI.TemplShopAllCell.New(good_data)
		self.__goods_item_map[good_data.goods_id] = good_item
		table.insert(self.__fadein_goods_id_list, good_data.goods_id)
		self:AddChild(good_item)

		if item_index <= 3 then
			good_item:SetPosition((item_index-1)*284,202)
		else
			good_item:SetPosition((item_index-4)*284,0)
		end
	end
end

function ShopGoodsPage:ShowFadeinEffect()
	for _, item in pairs(self.__goods_item_map) do
		item:SetOpacity(0)
		CPPActionManager.StopAllActions(item:GetNode())
	end

	local fadein_index = 0
	local function DoFadein()
		fadein_index = fadein_index + 1
		local goods_id = self.__fadein_goods_id_list[fadein_index] or -1
		local item = self.__goods_item_map[goods_id]
		if item then
			CPPActionManager.FadeIn(item:GetNode(), 0.2)
		else
			if self.__fadein_effect_timer then
				GlobalTimerQuest:CancelQuest(self.__fadein_effect_timer)
				self.__fadein_effect_timer = nil
			end
		end
	end

	if self.__fadein_effect_timer then
		GlobalTimerQuest:CancelQuest(self.__fadein_effect_timer)
	end
	self.__fadein_effect_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
	function()
		DoFadein()
	end), 0.1, -1)

	DoFadein()
end

function ShopGoodsPage:HandleBuySuccess(info)
	local item = self.__goods_item_map[info.param1]
	if item then
		item:ResetData()
	end
end