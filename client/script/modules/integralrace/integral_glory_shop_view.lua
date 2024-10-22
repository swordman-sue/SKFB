Modules = Modules or {}

Modules.IntegralGloryShopView = Modules.IntegralGloryShopView or BaseClass(BaseView)

function Modules.IntegralGloryShopView:__init(data)
	self.__layout_name = "arena_reputationshopview"
	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
	self.__title_bar_params = {
		close_func = function ()
			self:CloseManual()	
		end,
		title = CPPGameLib.GetString("integral_glory_shop_title"),
		show_power = true,
		show_energy	= true,
		--TODO 货币类型
		show_score_match_honor	= true,
		show_diamond = true,	
	}

   local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
   local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no")
   self.__tab_params = {
	    {sp_2,sp_1},
	    {	
	   		CPPGameLib.GetString("arena_shopTip"),
		},
		10,
		CPPGameLib.Handler(self, self.changeTag),
		false
	}

	self.__open_callback = function ()
		self:__OpenCallback()
	end
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.STORE)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.STORE,true)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.ARENA_REPUTATIONSHOP_CELL)
end

function Modules.IntegralGloryShopView:__OpenCallback() 
	--货币
   local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_MONEYICON, "type_"..Macros.Game.RolePropType.SCORE_MATCH_HONOR)
   self:GetWidget("Image_8"):SetImage(sp)
   --拥有声望  
   Util:WidgetLabel(self:GetWidget("Text_reputation"),CPPGameLib.GetString("integral_glory_shop_have_integral"), Macros.TypefaceSize.largish, Macros.Color.keypoint )
   self.__reputation_num = self:GetWidget("Text_reputationNum")
   local reputationNum = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.SCORE_MATCH_HONOR)
   Util:WidgetLabel(self.__reputation_num,reputationNum, Macros.TypefaceSize.largish, Macros.Color.content )
   --获取精华id
   for _,score_info in ipairs(CPPGameLib.GetConfig("ScoreMatchShop",nil,nil,true)) do
   		if score_info.cost_item_id_list then
   			self.__cur_essence_id = score_info.cost_item_id_list[1]
   			break
   		end
   end
   --精华
   local icon_path
   local config = CPPGameLib.GetConfig("Item",self.__cur_essence_id)
   if not config then return "" end
   if  config.sub_type == Macros.Game.NormalItemSubType.HERODEBRIS then
	icon_path = Resource.PathTool.GetHeroHeadPath(config.res_id) 
   else
	icon_path = Resource.PathTool.GetItemPath(config.res_id)       
   end
   self:GetWidget("Image_8_0"):SetVisible(true)
   self:GetWidget("Image_8_0"):SetImage(icon_path,TextureResType.LOCAL)
   --拥有精华  
   self:GetWidget("Text_SpThingHave"):SetVisible(true)
   Util:WidgetLabel(self:GetWidget("Text_SpThingHave"),CPPGameLib.GetString("integral_glory_shop_have_essence"), Macros.TypefaceSize.largish, Macros.Color.keypoint )
   self.__txt_spthing_havenum = self:GetWidget("Text_SpThingHaveNum")
   self.__txt_spthing_havenum:SetVisible(true)
   local essence_num = ObjIDTool.GetOwnNum(self.__cur_essence_id)
   Util:WidgetLabel(self.__txt_spthing_havenum,essence_num, Macros.TypefaceSize.largish, Macros.Color.content )
   --后退
   WidgetUnity.SetWidgetListener(self:GetWidget("Image_back"), nil, nil,function ()
   		local page = self.__pageview_reputationshop:GetCurPageIndex()
   		self.__pageview_reputationshop:ScrollToPageByTime(page -1,0.5)
   end)

  	local effect = nil
	self.effectid_1 ,effect = self:__CreateEffect({res_name = "UI_shangdianjiantou" } , self:GetWidget("Image_back") , 999 )
	effect:SetAnchorPoint(0.5,0.5)
	effect:SetPosition( self:GetWidget("Image_back"):GetContentWidth()/2 , self:GetWidget("Image_back"):GetContentHeight()/2 )
    effect:SetScaleXFactor( -1 )

   --前进
   WidgetUnity.SetWidgetListener(self:GetWidget("Image_front"), nil, nil,function ()
   		local page = self.__pageview_reputationshop:GetCurPageIndex()
   		self.__pageview_reputationshop:ScrollToPageByTime(page+1,0.5)
   end)

	local effect = nil
	self.effectid_2 ,effect = self:__CreateEffect({res_name = "UI_shangdianjiantou" } , self:GetWidget("Image_front") , 999 )
	effect:SetAnchorPoint(0.5,0.5)
	effect:SetPosition( self:GetWidget("Image_front"):GetContentWidth()/2 , self:GetWidget("Image_front"):GetContentHeight()/2 )

   --商店
   self.__panel_reputationshop = self:GetWidget("Panel_reputationShop") 
   self.__pageview_reputationshop = self:GetWidget("PageView_reputationShop") 
   WidgetUnity.SetPageViewListener(self.__pageview_reputationshop,CPPGameLib.Handler(self, self.PageviewScrollListener))
   --排名奖励
   self.__panel_rankencourage = self:GetWidget("Panel_rankEncourage")  
   self.__pageview_rankencourage = self:GetWidget("PageView_rankEncourage")
   WidgetUnity.SetPageViewListener(self.__pageview_rankencourage,CPPGameLib.Handler(self, self.PageviewScrollListener))

   --页签
   --选中页签标记
   self.__img_selectedtab1 = self:GetWidget("Image_selectedtab1")
   self.__img_selectedtab2 = self:GetWidget("Image_selectedtab2")
   local Panel_tag = self:GetWidget("Panel_tab")
   self.__tab:On(1)      --默认显示第一个选项卡的
   Panel_tag:AddChild(self.__tab:GetNode())
   --打开第一个PageView
   self.__shop_data = CallModuleFunc(ModuleType.INTEGRAL_RACE,"GetShopDataList")
   self:__InitPageView()
end

function Modules.IntegralGloryShopView:__RegistAllEvents()    
    --更新商店金钱
    self:BindGlobalEvent(Macros.Event.IntegralModule.GLORY_SHOP_BUY_ITEM_RESP, function()
    	for _,layout in ipairs(self.__pageview_layout_list) do
    		layout:ResetPieceNum()
    	end   	
   		local tag,times = CallModuleFunc(ModuleType.INTEGRAL_RACE,"GetNeedData")	
   		self.__pageview_layout_list[tag]:ResetData(times)            
	    self:ResetReputationNum()  
    end)
end

function Modules.IntegralGloryShopView:__Dispose()
	if self.__shop_runAction_Calltimer then
		GlobalTimerQuest:CancelQuest(self.__shop_runAction_Calltimer)
		self.__shop_runAction_Calltimer = nil
	end
end

function Modules.IntegralGloryShopView:__InitPageView()
	if not self.__shop_data then
		return
	end
	--清空所有页面
	self.__pageview_reputationshop:RemoveAllPages()
	self.__pageview_reputationshop:SetTouchEnabled(not CallModuleFunc(ModuleType.LEAD, "IsInLead"))
	self.__pageview_layout_list = {}
	local layout = nil
	local numberTag = 0
	local could_buy_num = 0
	for i,itemData in ipairs(self.__shop_data) do
		numberTag = numberTag + 1
		if i%6 == 1 then
			layout = Util:Layout(848,400)
			layout:SetBackGroundColor(0, 0, 0)
			numberTag = 1
		end
		local itemCell = GUI.TemplIntegralGloryShopCell.New(itemData)
		itemCell:GetNode():SetTag(i)
		layout:AddChild(itemCell:GetNode())
		table.insert(self.__pageview_layout_list,itemCell)
		if numberTag <= 3 then
			itemCell:SetPosition((numberTag-1)*284,202)
		else
			itemCell:SetPosition((numberTag-4)*284,0)
		end
		itemCell:CheckSectionOperateSP()

		if i%6 == 0 or i == #self.__shop_data then
			self.__pageview_reputationshop:AddPage(layout)
		end
	end

	if CallModuleFunc(ModuleType.LEAD, "IsInLead") then
		self.__pageview_reputationshop:ForceDoLayout()
	end

	self:PageviewScrollListener()
	self:ShopItemRunAction()
end

function Modules.IntegralGloryShopView:PageviewScrollListener()
	local page = self.__pageview_reputationshop:GetCurPageIndex()
   	self:GetWidget("Image_back"):SetVisible(true)
   	self:GetWidget("Image_front"):SetVisible(true)
   	if page == 0 then
   		self:GetWidget("Image_back"):SetVisible(false)
   	end
   	if page == self.__pageview_reputationshop:GetPageNum() - 1 then
   		self:GetWidget("Image_front"):SetVisible(false)
   	end
end

function Modules.IntegralGloryShopView:changeTag(index)
	self.__panel_reputationshop:SetVisible(index == 1) 
	self.__panel_rankencourage:SetVisible(index == 2) 
	self.__img_selectedtab1:SetVisible(index == 1)
	self.__img_selectedtab2:SetVisible(index == 2)
	self.__cur_index = index	
	if self.__pageview_reputationshop:GetPageNum() > 0 then
		self:PageviewScrollListener()
		self:ShopItemRunAction()
	end
end

--动态效果
function Modules.IntegralGloryShopView:ShopItemRunAction()
	if self.__shop_runAction_Calltimer then
		GlobalTimerQuest:CancelQuest(self.__shop_runAction_Calltimer)
		self.__shop_runAction_Calltimer = nil
	end
	self.StartTime = 0
	self.EndTime = 0
	local curPageTag = 0
	local page = self.__pageview_reputationshop:GetCurPageIndex()
	curPageTag = page * 6
	self.StartTime = curPageTag
	for i=1,6 do
		if self.__pageview_layout_list[curPageTag + i] then
			self.__pageview_layout_list[curPageTag + i]:GetNode():SetOpacity(0)
			self.EndTime = curPageTag + i
		end
	end
	if not self.__shop_runAction_Calltimer then
		self.__shop_runAction_Calltimer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
		function()
		self:ShowAction()
		end), 0.1, -1)
	end
	self:ShowAction()
end

function  Modules.IntegralGloryShopView:ShowAction()
	self.StartTime = self.StartTime + 1
	CPPActionManager.FadeIn(self.__pageview_layout_list[self.StartTime]:GetNode(),0.2)
	if self.StartTime == self.EndTime then		
		if self.__shop_runAction_Calltimer then
			GlobalTimerQuest:CancelQuest(self.__shop_runAction_Calltimer)
			self.__shop_runAction_Calltimer = nil
		end
	end
end

function Modules.IntegralGloryShopView:ResetReputationNum()
	local reputationNum = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.SCORE_MATCH_HONOR)
	self.__reputation_num:SetText(reputationNum)  
	--拥有精华  
   local essence_num = ObjIDTool.GetOwnNum(self.__cur_essence_id)
   self.__txt_spthing_havenum:SetText(essence_num)
end
