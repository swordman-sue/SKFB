
--[[
%% @module: 竞技场声望商店
%% @author: swordman sue
%% @created: 2019-3-21
--]]

Modules = Modules or {} 

Modules.ArenaReputationShopView = Modules.ArenaReputationShopView or BaseClass(BaseView)

--选项卡类
local ShopTabItem

--商品页面类
local ShopGoodsPage

function Modules.ArenaReputationShopView:__init()
	self.__layout_name = "arena_reputationshopview"
	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
	self.__title_bar_params = {
		close_func = function ()
			self:CloseManual()	
		end,
		title = CPPGameLib.GetString("arena_ReputationShopText"),
		show_power = true,
		show_energy	= true,
		show_arena_prestige	= true,
		show_diamond = true,	
	}

    self.__loaded_callback = function()
    	self:__LoadedCallback()
    end

	self.__open_callback = function ()
		self:__OpenCallback()
	end

	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.STORE)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.STORE,true)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.ARENA_REPUTATIONSHOP_CELL)
end

function Modules.ArenaReputationShopView:__LoadedCallback()
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
    	[Macros.Game.ARENA_SHOP.REPUTATION_SHOP] = self:GetWidget("Panel_reputationShop"),
    	[Macros.Game.ARENA_SHOP.RANK_ENCOURAGE] = self:GetWidget("Panel_rankEncourage"),
	}
end

function Modules.ArenaReputationShopView:__OpenCallback()  
	local tab_index = unpack(self.__open_data)

   --商品PageView列表
   self.__goods_page_view_map = {}

   --商品条目表
   self.__goods_item_map = {}

   --初始化选项卡
   self:__InitTab(tab_index)

   --设置声望数量  
   Util:WidgetLabel(self:GetWidget("Text_reputation"), CPPGameLib.GetString("arena_haveReputation"), Macros.TypefaceSize.largish, Macros.Color.keypoint)
   self.__txt_reputation_num = Util:WidgetLabel(self:GetWidget("Text_reputationNum"), reputationNum, Macros.TypefaceSize.largish, Macros.Color.content )   
   self:__UpdateReputationNum()

   --设置排名奖励选项卡红点
   self:__ShowRankEncourageTabReddot()

   --新手指引特殊操作检测
   self:__CheckSectionOperateSP()
end

function Modules.ArenaReputationShopView:__RegistAllEvents()
	--购买记录反馈
    self:BindGlobalEvent(Macros.Event.ShopModule.RECORD_LIST, function(_, type)
		if type == Macros.Game.SHOP.ARENA then
	    	self:__InitGoodsPageView(Macros.Game.SHOP.ARENA, self.__cur_type)
	    end
    end)

	--购买成功
    self:BindGlobalEvent(Macros.Event.ShopModule.BUY_SUCCESS, function( _ , info )
    	if info.type == Macros.Game.SHOP.ARENA then
		    --更新声望数量
		    self:__UpdateReputationNum()

		    local sub_type = info.param3 

		    --更新商品
		    local page_view = self.__goods_page_view_map[sub_type]
		    if page_view then
		    	page_view:PageIter(function(index, page)
		    		page:HandleBuySuccess(info)
		    	end)
		    end		    

			--更新排名奖励选项卡红点
    		if sub_type == Macros.Game.ARENA_SHOP.RANK_ENCOURAGE then
		    	self:__ShowRankEncourageTabReddot()
		    end
    	end
    end)
end

function Modules.ArenaReputationShopView:__Dispose()
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
function Modules.ArenaReputationShopView:__InitTab(tab_index)
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

		local tab_data_list = {	
				{name = CPPGameLib.GetString("arena_shopTip")},
				{name = CPPGameLib.GetString("arena_rankEncourage")},
		}

		self.__tab_scv = GUI.ScrollView.New(params)
		self.__tab_scv:SetDataList(tab_data_list)
		self.__tab_scv:SetScissorSize(panel_tab:GetContentWidth(), panel_tab:GetContentHeight() + 40)

		panel_tab:AddChild(self.__tab_scv:GetNode())
		PosTool.Center(self.__tab_scv, 0, -5)
	end

    self.__tab_scv:SelectItem(tab_index or 1) 
end

--选项卡选中回调
function Modules.ArenaReputationShopView:__TabChange(index, data)
	self.__cur_type = self:__GetTypeByTabIndex(index)

	--设置商品PageView显隐
	for k, page_view_container in pairs(self.__goods_page_view_container_map) do
		page_view_container:SetVisible(k == self.__cur_type)
	end

	--初始化商品PageView
	if not self.__goods_page_view_map[self.__cur_type] then
		CallModuleFunc(ModuleType.SHOP, "ShopBuyRecordRequest", Macros.Game.SHOP.ARENA)			
	else
		--翻页箭头效果
		self:__ShowTurnPageArrow()	

		--当前页淡入效果
		local page_view = self.__goods_page_view_map[self.__cur_type]
		local page = page_view:GetCurContentPage()
		if page then
			page:ShowFadeinEffect()
		end		
	end
end

--初始化商品页面
function Modules.ArenaReputationShopView:__InitGoodsPageView(type, sub_type)
	sub_type = sub_type or 1

	--构造页面数据
	local goods_list = CallModuleFunc(ModuleType.SHOP, "GetGoodsListByType", type, sub_type)
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
	local page_view = self.__goods_page_view_map[sub_type]
	if not page_view then
		page_view = GUI.PageView.New({
			view_width = 848, 
			view_height = 400, 
			page_class = ShopGoodsPage,
			turn_page_func = function ()
				self:__ShowTurnPageArrow()
			end})
		self.__goods_page_view_container_map[sub_type]:AddChild(page_view:GetNode())
		PosTool.Center(page_view)	

		self.__goods_page_view_map[sub_type] = page_view
	end
	page_view:SetDataList(pages_data_list)
	local is_in_lead = CallModuleFunc(ModuleType.LEAD, "IsInLead")
	page_view:SetTouchEnabled(not is_in_lead)
	if is_in_lead then
		page_view:ForceDoLayout()
	end

	--翻页箭头效果
	self:__ShowTurnPageArrow()	

	--当前页淡入效果
	local page_view = self.__goods_page_view_map[sub_type]
	local page = page_view:GetCurContentPage()
	if page then
		page:ShowFadeinEffect()
	end			
end

--显示翻页箭头
function Modules.ArenaReputationShopView:__ShowTurnPageArrow()
	local page_view = self.__goods_page_view_map[self.__cur_type]
	local page_index = page_view:GetCurPageIndex()
   	self.__img_back:SetVisible(page_index ~= 0)
   	self.__img_front:SetVisible(page_index ~= page_view:GetPageNum() - 1)
end

--更新礼包选项卡红点
function Modules.ArenaReputationShopView:__ShowRankEncourageTabReddot()
	local tab_item = self.__tab_scv:GetItem(Macros.Game.ARENA_SHOP.RANK_ENCOURAGE)
	if tab_item then
		tab_item:ShowReddot()
	end
end

--新手指引特殊操作检测
function Modules.ArenaReputationShopView:__CheckSectionOperateSP()
	local tab_item = self.__tab_scv:GetItem(Macros.Game.ARENA_SHOP.RANK_ENCOURAGE)
	if tab_item then
	   	CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.ARENA_SHOP_TAB_RANK_ENCOURAGE, tab_item:GetNode())				    
	end
end

--根据选项卡索引获取类型
function Modules.ArenaReputationShopView:__GetTypeByTabIndex(tab_index)
	return tab_index
end

--更新声望数量
function Modules.ArenaReputationShopView:__UpdateReputationNum()
   local var = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.ARENA_PRESTIGE)
   self.__txt_reputation_num:SetText(var)  
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
	local is_show_reddot
	local rank = CallModuleFunc(ModuleType.ARENA, "GetRank")
	local reputation = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.ARENA_PRESTIGE)
	local goods_list = CallModuleFunc(ModuleType.SHOP, "GetGoodsListByType", Macros.Game.SHOP.ARENA, Macros.Game.ARENA_SHOP.RANK_ENCOURAGE)
	if goods_list then
		for _, good_data in ipairs(goods_list) do
			if (good_data.need_rank == 0 or good_data.need_rank >= rank) and reputation >= good_data.cost_value then
				is_show_reddot = true
				break
			end
		end
	end

	if is_show_reddot and not self.__reddot then
		local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
		self.__reddot = Util:Sprite(icon_red_point)
		self.__reddot:SetAnchorPoint(0.5 , 0.5)
		self.__reddot:SetVisible(false)
		self:AddChild(self.__reddot)
		PosTool.RightTop(self.__reddot, -10, -10)
	end
	if self.__reddot then
		self.__reddot:SetVisible(is_show_reddot)
	end
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
		
		local good_item = GUI.TemplArenaShopAllCell.New(good_data)
		self.__goods_item_map[good_data.goods_id] = good_item
		table.insert(self.__fadein_goods_id_list, good_data.goods_id)
		self:AddChild(good_item)

		if item_index <= 3 then
			good_item:SetPosition((item_index-1)*284,202)
		else
			good_item:SetPosition((item_index-4)*284,0)
		end

		good_item:CheckSectionOperateSP()
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
	for goods_id, item in pairs(self.__goods_item_map) do
		item:ResetData(info)
	end
end
