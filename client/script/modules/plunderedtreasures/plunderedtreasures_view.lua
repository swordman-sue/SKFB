--
-- @brief 夺宝
-- @auther: ldx
-- @date 2016年10月10日 18:11:43
--

Modules = Modules or {}
Modules.PlunderedTreasuresView = Modules.PlunderedTreasuresView or BaseClass(BaseView)
local PlunderedTreasuresViewType = 
{
	CHOOSETREASURES = 1,
	CHOOSEENEMY = 2,
}

function Modules.PlunderedTreasuresView:__init()
    self.__is_force_stretch = true
    self.__is_ignore_iphone_safe_area = true
	self.__layout_name = "plundered_treasures"
	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_plunderedtreasures"),image_size = COCOSize(1136,640),image_anchor = COCOPoint(1, 0.5), image_pos_func = PosTool.RightCenter, is_fixed_size = true}
	self.__title_bar_params = {
		close_func = function ()
			if self.viewType == PlunderedTreasuresViewType.CHOOSETREASURES then
				if self.__is_playing_effect then
					return
				end
				self:CloseManual()
			else
				--TODO
				self.viewType = PlunderedTreasuresViewType.CHOOSETREASURES
			end
		end,
		title = CPPGameLib.GetString("plunderedtreasures"),
		show_diamond = true,
		show_gold = true,
		show_energy	= true,
		show_level	= true,
	}
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.PLUNDEREDTREASURES)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.PLUNDEREDTREASURES,true)
	self.__open_callback = function ()		
		self:__OpenCallback()
	end
end

function Modules.PlunderedTreasuresView:__OpenCallback()
	local sameViewType,isShowTip,item_type = unpack(self.__open_data)
	self:BindGlobalEvent(Macros.Event.PlunderedTreasuresModule.PLUNDEREDTREASURES_REFRASHVIEW, function(_,data)
        if data then
	        if data.treasure_info_id then
	        	local info = CPPGameLib.GetConfig("Treasure",self.__selected_treasuresId)
	        	GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("plunderedtreasures_composeSucessTips",info.name))	        	
        		GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_COMPOUND_TIPS ,false , self.__selected_treasuresId )

	        	self:__SetButtonVisible(true)
	        	self:__SetCompoundVisible()	
	        	self.__is_playing_effect = false
				self.__panel_shield:SetVisible(false)

	        elseif data.getPiece then
	        	local info = CPPGameLib.GetConfig("Item",self.__selected_treasuresPieceId)	        	
	        	GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("plunderedtreasures_getPieceTips",info.name))
	       	end
        end
       	self:SortTreasuresList()
       	self.__treasures_scrollview:SetDataList(self.__treasureslist)

       	self:resetTreasuresPiece()
       	self:SetItemSelectedStatus()
      	
    end)   
    self:BindGlobalEvent(Macros.Event.PlunderedTreasuresModule.PLUNDEREDTREASURES_REFRASHAVOIDWARTIME, function()
      	if self.TemplPlunderedTreasuresAvoidWarView then
      		self.TemplPlunderedTreasuresAvoidWarView:DeleteMe()
      		self.TemplPlunderedTreasuresAvoidWarView = nil
      	end
      	self:refrashAvoidWarTime()
    end)
 
     --关闭界面
    self:BindGlobalEvent(Macros.Event.View.VIEW_CLOSE, function(_ , view_id,view_type)
      	if view_type == BaseViewType.PLUNDEREDTREASURESCHOOSE_ENEMYVIEW then
      		if self.__shield_icon_pos and self["treasure_icon_pos"..self.__shield_icon_pos] then
				self["treasure_icon_pos"..self.__shield_icon_pos]:SetTouchEnabled(true)	
			end
      	end
    end)

    --角色升级
    self:BindGlobalEvent(Macros.Event.RoleModule.LEVEL_CHANGE, function()
      	--判断是否开放一键抢夺
		self:JudgeIsCouldOpenOneKeyPlundered()
    end)

    --寻常特效
    self.__selected_effect_list = {[3] = "UI_duobao_lan",[4] = "UI_duobao_zi",[5] = "UI_duobao_cheng",[6] = "UI_duobao_hong"}     
    --发光特效
    self.__compose_effect_list = {[3] = 100039,[4] = 100040,[5] = 100041,[6] = 100042}     
    --屏蔽层
    self.__panel_shield = self:GetWidget("Panel_shield")
    self.__panel_shield:SetVisible(false)
    --免战
    self.__avoidwar_button = self:GetWidget("Button_avoidWar")
    WidgetUnity.SetWidgetListener(self.__avoidwar_button,nil,nil,function ()
    	self.TemplPlunderedTreasuresAvoidWarView = LogicGUI.TemplPlunderedTreasuresAvoidWarView.New()
    end, nil)
    --图片适配
    self:GetWidget("Image_4"):IgnoreContentAdaptWithSize(true)
    self:GetWidget("Image_ProgressReward"):IgnoreContentAdaptWithSize(true)
    self:GetWidget("Image_3"):IgnoreContentAdaptWithSize(true)   
    --免战倒计时
    self.Text_remainAvoidWarTime = self:GetWidget("Text_remainAvoidWarTime")
    Util:WidgetLabel(self.Text_remainAvoidWarTime,CPPGameLib.GetString("plunderedtreasures_RetainAvoidWarTime"), Macros.TypefaceSize.normal, Macros.Color.btn, 0, 0, Macros.Color.common_stroke )
    self.Text_avoidWarTime = self:GetWidget("Text_avoidWarTime")
    Util:WidgetLabel(self.Text_avoidWarTime,"", Macros.TypefaceSize.normal, Macros.Color.golden, 0, 0, Macros.Color.common_stroke )
    self:refrashAvoidWarTime()    
    --向上箭头
    self.__image_uparrows = self:GetWidget("Image_upArrows")
    --向下箭头
    self.__image_downarrows = self:GetWidget("Image_downArrows")
    --左边栏缩放   
    local panel_treasurecon = self:GetWidget("Panel_treasureCon")
    local offset_x = Macros.Global.ScreenSize.width >= Macros.Global.DesignSizeDef.width and 0 or -30
    panel_treasurecon:SetPosition(panel_treasurecon:GetPositionX() + offset_x,panel_treasurecon:GetPositionY())
    local Image_5 = self:GetWidget("Image_5")
    Image_5:SetScaleXFactor(Macros.Global.ScreenSize.width >= Macros.Global.DesignSizeDef.width and 1 or 0.8)
    --圣器类型
    self.__image_treasuretype = self:GetWidget("Image_treasureType")
    local act_offsetx = math.max(Macros.Global.ScreenSize.width-1040,-160)
    local offset_x = Macros.Global.ScreenSize.width >= 1040 and 0 or act_offsetx
    self.__image_treasuretype:SetPosition(self.__image_treasuretype:GetPositionX() + offset_x,self.__image_treasuretype:GetPositionY())
    self.__image_treasuretype:IgnoreContentAdaptWithSize(true)
	--更新圣器队列并进行排序
	self:SortTreasuresList(item_type)
	self.viewType = PlunderedTreasuresViewType.CHOOSETREASURES	
	local TreasuresListPanel = self:GetWidget("Panel_treasureList")
	local params = {
		item_class	= Modules.TemplPlunderedTreasureListCell,
	    item_width	= 100,
	    item_height	= 80,
	    row	= 5,
	    col	= 1,
	    item_space_r = 20,
	    view_width = 100,
	    view_height = 450,
	    selectable = true,
   		select_callback = function (index,data)
			self.__selectedIndex = index
   			self.__selected_treasuresId = data.treasureId   		
   			local treasure_info = CPPGameLib.GetConfig("Treasure",self.__selected_treasuresId)	
   			self.__text_composename:SetText(treasure_info.name)
   			self.__text_composename:SetColor(Macros.QualityColor(treasure_info.quality))
   			self.__text_composename:SetStrokeColor(Macros.QualityColorStroke(treasure_info.quality))
   			--圣器类型
   			local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.PLUNDEREDTREASURES, "treasure_type"..treasure_info.type,true)
   			self.__image_treasuretype:SetImage(sp)
   			self:resetTreasuresPiece()  			
   		end,
   		update_move_func = function ()
   			--更新箭头
			self:__UpdateArrows()
   		end
	}
	self.__treasures_scrollview = GUI.ScrollView.New(params)
	TreasuresListPanel:AddChild(self.__treasures_scrollview:GetNode())
	PosTool.Center(self.__treasures_scrollview)
	self.__treasures_scrollview:SetDataList(self.__treasureslist)

	--进度奖励
    self.__btn_progressreward = self:GetWidget("Button_ProgressReward")
    self.__btn_progressreward:IgnoreContentAdaptWithSize(true)
    self.__btn_progressreward:SetScaleFactor(0.8)   
    local img_progressreward = self:GetWidget("Image_ProgressReward")
    PosTool.CenterBottom(img_progressreward,10,0)
    img_progressreward:SetScaleFactor(1.25)   
    Util:WidgetButton(self.__btn_progressreward,"", function ()
    	--打开进度奖励界面
    	LogicGUI.TemplPlunderedTreasuresReward.New()
    end, Macros.TypefaceSize.largish, Macros.Color.btn, Macros.Color.cityblack_stroke,2)
    self.__btn_progressreward:SetNormalImage(Resource.PathTool.GetSystemEntryIconPath("btn_store"),TextureResType.LOCAL) 
	--进度红点
	local entry_red_dot_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
	self.__progress_red_dot = SpriteEx.CreateWithImageFile(entry_red_dot_image_data)
	self.__progress_red_dot:SetAnchorPoint(1, 1)
	self.__btn_progressreward:AddChild(self.__progress_red_dot,1)
	PosTool.RightTop(self.__progress_red_dot)		

	--请求数据
	self:CallModuleFunc("plunderedTreasuresDataReq")

	--更新箭头
	self:__UpdateArrows()

	--更新红点
	self:__UpdateRedDotNum()

	--队列重设	
	if sameViewType and sameViewType == BaseViewSubType.PlunderedTreasures.MAIN_VIEW and isShowTip then
		self.__selected_treasuresId,self.__selected_treasuresPieceId = self:CallModuleFunc("GetCurtreasureInfoId")
	elseif sameViewType and sameViewType == BaseViewSubType.PlunderedTreasures.CHOOSE_ENEMY then
		self.__selected_treasuresId,self.__selected_treasuresPieceId = self:CallModuleFunc("GetCurtreasureInfoId")
	elseif self:CallModuleFunc("GetIsSelectedTag") then
		self:CallModuleFunc("SetIsSelectedTag",false)
		self.__selected_treasuresId,self.__selected_treasuresPieceId = self:CallModuleFunc("GetCurtreasureInfoId")
	end
	--中心合成物品容器
	self.__panel_compose = self:GetWidget("Panel_compose")
	local icon_path = self:__GetTreasureSpriteData(self.__selected_treasuresId)
	self.__compose_itemIcon = Util:Sprite(icon_path)
    --self.__compose_itemIcon:SetScaleFactor(2)
    self.__panel_compose:AddChild(self.__compose_itemIcon,3)
	PosTool.Center(self.__compose_itemIcon)
	WidgetUnity.SetWidgetListener(self.__panel_compose, nil, nil,function ()
		GUI.TemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.PLUNDERED_TREASURES_DETAILMESSAGE, false, self.__selected_treasuresId)
	end)	
	--中心物品名字
	self.__text_composename = self:GetWidget("Text_composename")
	Util:WidgetLabel(self.__text_composename, "", Macros.TypefaceSize.tab)
	--合成容器
	self.__panel_compound = self:GetWidget("Panel_compound")
	--中心点坐标
    self.__panel_center_pointx = self.__panel_compound:GetContentWidth()/2
    self.__panel_center_pointy = self.__panel_compound:GetContentHeight()/2    
	--合成按钮 352,500
	self.__btn_compose = self:GetWidget("Button_compose")	
	self.__btn_compose:SetPosition(is_open and 500 or (is_prepare_open and 500 or 352),self.__btn_compose:GetPositionY())
	Util:WidgetButton(self.__btn_compose,CPPGameLib.GetString("plunderedtreasures_compose"),function ()
        CallModuleFunc(ModuleType.SUNDRY, "PackVacancy", Macros.Game.ObjType.TREASURE , 0 , false , function ( sum )
            if sum > 0 then
				local info = CPPGameLib.GetConfig("TreasureCompose",self.__selected_treasuresId)
				local allHave = true
				local max_num
				for i,item_id in ipairs(info.cost_item_list) do
					local item_info = CallModuleFunc(ModuleType.Pack,"GetCommonGoods",item_id)
					if not item_info then
						allHave = false
						break
					else
						max_num = max_num or item_info.item_num
						max_num = item_info.item_num < max_num and item_info.item_num or max_num
					end				
				end
				if not allHave then						
					GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("plunderedtreasures_pieceNoEnoughTips"))
					return
				end
				max_num = max_num > sum and sum or max_num
				self.__is_onekey_compose = max_num > 1
				--是否一键合成
				if self.__is_onekey_compose then
					LogicGUI.TemplPlunderedTreasuresOneKeyCompose.New({item_id = self.__selected_treasuresId,could_compose_num = max_num,func = function (choose_num)
						self.__choose_num = choose_num
						self:__PlayComposeSpecialEffect()
					end})
				else
					self:__PlayComposeSpecialEffect()
				end
			else				
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("plunderedtreasures_treasure_pack_full_tips"))
            end
        end )
	end,Macros.TypefaceSize.largish,Macros.Color.btn,Macros.Color.button_yellow)
	--一键合成按钮
	self.__btn_onekey_compose = self:GetWidget("Button_oneKeyCompose")	
	Util:WidgetButton(self.__btn_onekey_compose,CPPGameLib.GetString("plunderedtreasures_onekey_plundered"),function ()
		local info = CPPGameLib.GetConfig("TreasureCompose",self.__selected_treasuresId)
		local allHave = true
		for i,item_id in ipairs(info.cost_item_list) do
			local item_info = CallModuleFunc(ModuleType.Pack,"GetCommonGoods",item_id)
			if not item_info then
				allHave = false
				break
			end				
		end
		if allHave then						
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("plunderedtreasures_onekey_plundered_full_tips"))
			return
		end
		--一键抢夺
		LogicGUI.TemplPlunderedTreasuresOneKeyTips.New({treasure_id = self.__selected_treasuresId})
	end,Macros.TypefaceSize.largish,Macros.Color.btn,Macros.Color.button_yellow)
	--判断是否开放一键抢夺
	self:JudgeIsCouldOpenOneKeyPlundered()
	--刷新碎片
	self:resetTreasuresPiece()
	if sameViewType == BaseViewSubType.PlunderedTreasures.MAIN_VIEW and isShowTip then
		local info = CPPGameLib.GetConfig("Item",self.__selected_treasuresPieceId)	 
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("plunderedtreasures_getPieceTips",info.name))
	elseif sameViewType == BaseViewSubType.PlunderedTreasures.CHOOSE_ENEMY then
		self.__is_shield_openview = true
		self:CallModuleFunc("plunderedTreasuresTargetListReq",self.__selected_treasuresId,self.__selected_treasuresPieceId,true)
	else
		self:CallModuleFunc("OpenEnemyView")
	end
	
    --规则按钮
    local Button_rule = Util:WidgetButton( self:GetWidget("Button_rule") , nil , function ( ... )
        print("xy" , "(2)圣器抢夺，大图标：" )
        local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
        help:SetLoadedCallback( function ( ... )
           help:SetCell("Rule_RobPyx")
        end )
    end )
	--设置滚动到指定圣器
	self:SetItemSelectedStatus()

	--新手指引特殊操作检测
	self:__CheckSectionOperateSP()

    --处理IPhoneX刘海遮挡问题
    self:__HandleIPhoneXAdaptation()
end

--判断是否开放一键抢夺
function Modules.PlunderedTreasuresView:__RegistAllEvents()
	--奖励记录
	self:BindGlobalEvent(Macros.Event.PlunderedTreasuresModule.PLUNDEREDTREASURES_PROGRESS_REWARD,function ()
		self:SortTreasuresList()
       	self.__treasures_scrollview:SetDataList(self.__treasureslist)
       	self:resetTreasuresPiece()
       	self:SetItemSelectedStatus()
	end)

	--红点更新
	self:BindGlobalEvent(Macros.Event.RedModule.RED_UPDATE,function (_, system_id)
		if system_id == Macros.Game.SystemID.PLUNDEREDTREASURES then
			self:__UpdateRedDotNum()
		end
	end)
end

--判断是否开放一键抢夺
function Modules.PlunderedTreasuresView:JudgeIsCouldOpenOneKeyPlundered()
	--判断一键抢夺是否开放
    local role_info = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo")
    local condition_list = ConfigAdapter.Common.GetOneKeyPlunderedtreasuresOpenCondition()
    self.__txt_openconditional = self:GetWidget("Text_OpenConditional")
    local is_open,is_prepare_open
    if role_info.level >= condition_list[1] or role_info.vip_level >= condition_list[2] then
    	is_open = true
    	self.__btn_onekey_compose:SetEnabled(true)
    else
    	local is_open,is_pre_open = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.ONE_KEY_PLUNDEREDTREASURES)
    	is_prepare_open = is_open or is_pre_open
    	self.__btn_onekey_compose:SetEnabled(false)
    	Util:WidgetLabel(self.__txt_openconditional,CPPGameLib.GetString("dungeon_open_condition",condition_list[1],condition_list[2]), Macros.TypefaceSize.normal, Macros.Color.red)
    end
    self.__txt_openconditional:SetVisible(not is_open and is_prepare_open and true or false)
    self.__is_txt_openconditional_visible = self.__txt_openconditional:IsVisible()
	self.__btn_compose:SetPosition(is_open and 500 or (is_prepare_open and 500 or 352),self.__btn_compose:GetPositionY())
	self.__btn_onekey_compose:SetVisible(is_open and true or (is_prepare_open and true or false))
	self.__is_btn_onekey_compose_visible = self.__btn_onekey_compose:IsVisible()
end

function Modules.PlunderedTreasuresView:GetOpenParams()
	return {}
end

--更新箭头
function Modules.PlunderedTreasuresView:__UpdateArrows()
	if self.__image_uparrows then
		local is_locate_to_top = self.__treasures_scrollview:IsLocateToTop() 
		self.__image_uparrows:SetVisible(not is_locate_to_top)
		local is_locate_to_down = self.__treasures_scrollview:IsLocateToLast() 
		self.__image_downarrows:SetVisible(not is_locate_to_down)
	end
end

function Modules.PlunderedTreasuresView:__GetTreasureSpriteData(treasureId)
	local config = CPPGameLib.GetConfig("Treasure", treasureId)
	local icon_path = Resource.PathTool.GetBigIconPath(config.res_id)
    return icon_path
end

--设置选中状态
function Modules.PlunderedTreasuresView:SetItemSelectedStatus()
	for i,treasureInfo in ipairs(self.__treasureslist) do
		if treasureInfo.treasureId == self.__selected_treasuresId then
			self.__selectedIndex = i
			self.__treasures_scrollview:SelectItem(i)
   			self.__treasures_scrollview:LocateItem(i)
   			break
		end
	end
end

--刷新免战时间
function Modules.PlunderedTreasuresView:updateAvoidWarTime()
	local overTime = self:CallModuleFunc("GetAvoidWarOverTime")
    local severTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
    if severTime >= overTime then
    	self.Text_avoidWarTime:SetVisible(false)
    	self.Text_remainAvoidWarTime:SetVisible(false)
    	self.__avoidwar_button:SetVisible(true)
		if self.__RefrashAvoidWarTimeEvent then
			GlobalTimerQuest:CancelQuest(self.__RefrashAvoidWarTimeEvent)
			self.__RefrashAvoidWarTimeEvent = nil
		end
    else
    	self.Text_avoidWarTime:SetText(TimeTool.TransTimeStamp("HH:MM:SS",overTime - severTime))
    end
end

--刷新免战时间
function Modules.PlunderedTreasuresView:refrashAvoidWarTime()
	local overTime = self:CallModuleFunc("GetAvoidWarOverTime")
    local severTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
    self.Text_avoidWarTime:SetVisible(not (severTime >= overTime))
    self.Text_remainAvoidWarTime:SetVisible(not (severTime >= overTime))
    self.__avoidwar_button:SetVisible(severTime >= overTime)    	
   if severTime < overTime then
    	self.Text_avoidWarTime:SetText(TimeTool.TransTimeStamp("HH:MM:SS",overTime - severTime))
    	if not self.__RefrashAvoidWarTimeEvent then
			self.__RefrashAvoidWarTimeEvent = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
				function()
				self:updateAvoidWarTime()
				end), 1, -1)
		end
		self:updateAvoidWarTime()
    end
end

--更新圣器队列并进行排序
function Modules.PlunderedTreasuresView:SortTreasuresList(item_type)
	--首先判断刚合成的圣器是否应该从列表中移除
	self.__treasureslist = {}
	local isSelectedIdExist = false
	if self.__selected_treasuresId then
		local SelectedIdInfo = CPPGameLib.GetConfig("TreasureCompose",self.__selected_treasuresId)
		if SelectedIdInfo.is_default_show == 1 then
			isSelectedIdExist = true
		else
			for i,item_id in ipairs(SelectedIdInfo.cost_item_list) do
				if CallModuleFunc(ModuleType.Pack,"GetCommonGoods",item_id) then
					isSelectedIdExist = true
					break
				end				
			end	
		end
	end
	--默认显示的和背包有碎片的加入队列中
	for treasureId,info in pairs(CPPGameLib.GetConfig("TreasureCompose", nil, nil , true )) do
		local treasureData = {}
		treasureData.treasureId = treasureId
		if info.is_default_show == 1 then
			table.insert(self.__treasureslist,treasureData)	
		else
			for i,item_id in ipairs(info.cost_item_list) do
				if CallModuleFunc(ModuleType.Pack,"GetCommonGoods",item_id) then
					table.insert(self.__treasureslist,treasureData)	
					break
				end				
			end	
		end		
	end
	--排序
	local function sortTreasuresList(iteminfo1,iteminfo2)
		local info_1 = CPPGameLib.GetConfig("Treasure",iteminfo1.treasureId) 
		local info_2 = CPPGameLib.GetConfig("Treasure",iteminfo2.treasureId) 
		--经验圣器优先排序
		if info_1.type == Macros.Game.TreasureType.EXP and info_2.type == Macros.Game.TreasureType.EXP then
			if info_1.quality == info_2.quality then
				return iteminfo1.treasureId < iteminfo2.treasureId
			else
				return info_1.quality > info_2.quality
			end			
		elseif info_1.type == Macros.Game.TreasureType.EXP and info_2.type ~= Macros.Game.TreasureType.EXP then
			return true
		elseif info_1.type ~= Macros.Game.TreasureType.EXP and info_2.type == Macros.Game.TreasureType.EXP then
			return false
		else
			if info_1.quality ~= info_2.quality then
				return info_1.quality > info_2.quality
			else
				return iteminfo1.treasureId < iteminfo2.treasureId
			end
		end
	end
	table.sort(self.__treasureslist,sortTreasuresList)
	if not isSelectedIdExist then
		self:__SortComposeTreasure(item_type)		
	end

	--检测圣器首次满足合成条件
	CallModuleFunc(ModuleType.LEAD, "CheckTreasureComposeFirstTime", self.__treasureslist)
end

function Modules.PlunderedTreasuresView:resetTreasuresList()
	self.listView:RemoveAllItems()
	self.curListViewIndex = 1
	
end

function Modules.PlunderedTreasuresView:__SortComposeTreasure(item_type)
	--可合成圣器排序
	local compose_list = {}
	for index,treasureData in ipairs(self.__treasureslist) do
		local info = CPPGameLib.GetConfig("TreasureCompose",treasureData.treasureId)
		local is_could_compose = true
		for i,pieceId in ipairs(info.cost_item_list) do
			local pieceNum = CallModuleFunc(ModuleType.Pack,"GetNumByItemID",pieceId)
			if pieceNum < 1 then
				is_could_compose = false
				break
			end
		end
		if is_could_compose then
			table.insert(compose_list,treasureData.treasureId)
		end
	end
	table.sort(compose_list,function (treasureId_1,treasureId_2)
		local info_1 = CPPGameLib.GetConfig("Treasure",treasureId_1) 
		local info_2 = CPPGameLib.GetConfig("Treasure",treasureId_2) 
		if info_1.quality ~= info_2.quality then
			return info_1.quality > info_2.quality
		else
			if info_1.type ~= info_2.type then
				return info_1.type < info_2.type
			else
				return treasureId_1 > treasureId_2
			end 
		end
	end)
	--
	if #compose_list > 0 then
		self.__selected_treasuresId = compose_list[1]
	elseif item_type then
		local sp_treasureid
		--遍历
		for i,treasure_data in ipairs(self.__treasureslist) do
			local info_1 = CPPGameLib.GetConfig("Treasure",treasure_data.treasureId) 
			if info_1.type == item_type then
				sp_treasureid = treasure_data.treasureId
				break
			end
		end		
		--特定类型
		self.__selected_treasuresId = sp_treasureid and sp_treasureid or self.__treasureslist[1].treasureId
	else
		self.__selected_treasuresId = #compose_list > 0 and compose_list[1] or self.__treasureslist[1].treasureId
	end	
end

function Modules.PlunderedTreasuresView:resetTreasuresPiece()
	local info = CPPGameLib.GetConfig("TreasureCompose",self.__selected_treasuresId)
	local treasureInfo = CPPGameLib.GetConfig("Treasure",self.__selected_treasuresId)
	if self.__selected_effect_id then
		self:__DelEffect(self.__selected_effect_id)
		self.__selected_effect_id = nil
	end	
	local num = #info.cost_item_list
	local effect
	self.__selected_effect_id,effect = self:__CreateEffect({res_name = self.__selected_effect_list[num]},self.__panel_compose,2)
	PosTool.Center(effect)
	self.__compose_num = num
	--中心圣器
	local icon_path = self:__GetTreasureSpriteData(self.__selected_treasuresId)
	self.__compose_itemIcon:SetImage(icon_path)
	--检测是否能合成
	local is_could_compose = true
	--隐藏所有控件
	self:__HideAllPosFrame()
	self:__DeleteCenterSpecialEffect()
	for i,pieceId in ipairs(info.cost_item_list) do				
		local pieceNum = CallModuleFunc(ModuleType.Pack,"GetNumByItemID",pieceId)
		if not self["treasure_icon_pos"..i] then
			if not self["prop_frame_pos"..i] then
				local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.PLUNDEREDTREASURES, "prop_frame")
				self["prop_frame_pos"..i] = Util:Sprite(sp)	
				self.__panel_compound:AddChild(self["prop_frame_pos"..i])	
				PosTool.Center(self["prop_frame_pos"..i])
			end
			self["treasure_icon_pos"..i] = LogicGUI.IconLayer.New(false,true)
			self["prop_frame_pos"..i]:AddChild(self["treasure_icon_pos"..i]:GetNode())	
			self["treasure_icon_pos"..i]:SetData({id = pieceId})				
			PosTool.Center(self["treasure_icon_pos"..i])
			--图片数量
			local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.PLUNDEREDTREASURES, "num_icon")
			local red_num = Util:Sprite(sp)
			red_num:SetAnchorPoint(1,0)	
			self["prop_frame_pos"..i]:AddChild(red_num)	
			PosTool.RightBottom(red_num)
			--数量
			self["treasure_icon_num_pos"..i] = Util:Label(pieceNum, Macros.TypefaceSize.slightly, Macros.Color.white)
			red_num:AddChild(self["treasure_icon_num_pos"..i])
			PosTool.Center(self["treasure_icon_num_pos"..i])
		else	
			self["treasure_icon_pos"..i]:SetData({id = pieceId})
			self["treasure_icon_num_pos"..i]:SetString(pieceNum)
		end	
		self["prop_frame_pos"..i]:SetVisible(true)
		local x = math.floor(math.sin(2*math.pi/self.__compose_num*(i-1))*190 + 226)
		local y = math.floor(math.cos(2*math.pi/self.__compose_num*(i-1))*175 + 216)
		self["prop_frame_pos"..i]:SetPosition(x,y)		
		--碎片少于1就打开挑战界面，否则则是信息界面					
		if pieceNum < 1 then
			is_could_compose = false
			if self.__is_shield_openview and self.__selected_treasuresPieceId and self.__selected_treasuresPieceId == pieceId then
				--屏蔽
				self.__shield_icon_pos = i
				self["treasure_icon_pos"..i]:SetTouchEnabled(false)	
				self.__is_shield_openview = false
			end
			self["treasure_icon_pos"..i]:SetGray(true)
			self["treasure_icon_pos"..i]:SetTouchCallback(function ()				
				--打开挑战对手界面
				self:CallModuleFunc("plunderedTreasuresTargetListReq",self.__selected_treasuresId,pieceId,true)
				self.__selected_treasuresPieceId = pieceId	
				--屏蔽
				self.__shield_icon_pos = i
				self["treasure_icon_pos"..i]:SetTouchEnabled(false)							
			end)
		else
			self["treasure_icon_pos"..i]:SetGray(false)
			self["treasure_icon_pos"..i]:SetTouchCallback(function ()
				--打开装备信息界面
				GUI.TemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.PLUNDERED_TREASURES_DETAILMESSAGE, false, pieceId)
			end)
		end
		--新手指引特殊操作检测(圣器争夺-选择碎片（碎片ID）)
		CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.PLUNDERED_TREASURES_CHOOSE_FRAGMENT, self["treasure_icon_pos"..i]:GetNode(), pieceId)         		
	end
	if is_could_compose then
		local effect
		self.__button_zoubian_effect,effect = self:__CreateEffect({res_name = "ui_anniuzoubian",scale = 1.2},self.__btn_compose)
		PosTool.Center(effect)		
	end
end

--删除特效
function Modules.PlunderedTreasuresView:__DeleteCenterSpecialEffect()
	if self.__center_blink_effect then
		self:__DelEffect(self.__center_blink_effect)
		self.__center_blink_effect = nil
	end
	if self.__button_zoubian_effect then
		self:__DelEffect(self.__button_zoubian_effect)
		self.__button_zoubian_effect = nil
	end
end

--播放特效
function Modules.PlunderedTreasuresView:__PlayComposeSpecialEffect()
	self.__is_playing_effect = true
	self.__panel_shield:SetVisible(true)
	self:__DeleteCenterSpecialEffect()
	self:__SetButtonVisible(false)
	local effectid
	local effect
	effectid,effect = self:__CreateEffect({id = self.__compose_effect_list[self.__compose_num],loop = false,time_scale = 3},self.__panel_compound,3)		
	PosTool.Center(effect,2,-15)
	CPPActionManager.DelayTo(self.prop_frame_pos1,0.5, GlobalCallbackMgr:AddCallBackFunc(function()					
		for i=1,self.__compose_num do
			self["prop_frame_pos"..i]:SetVisible(true)
			self["treasure_icon_pos"..i]:GetNode():SetRotation(30)
			local target_point = NodeUnity.ConvertPosBetweenTwoContainer(self.__panel_compound,self["prop_frame_pos"..i],COCOPoint(self.__panel_center_pointx,self.__panel_center_pointy))
			ActionManager.GetInstance():RunScaleToAndMoveToEffect(self["treasure_icon_pos"..i]:GetNode(),0.3,target_point.x,target_point.y,0,GlobalCallbackMgr:AddCallBackFunc(function()
				-- effectid,effect = self:__CreateEffect({res_name = "UI_duobaohecheng_yan",loop = false,time_scale = 1.1},self["prop_frame_pos"..i],2)
				-- effect:SetRotation(-180 + (360/self.__compose_num)* (i-1))			
				-- PosTool.Center(effect)
				self["treasure_icon_pos"..i]:RemoveSelf(true)
				self["treasure_icon_pos"..i] = nil
				if i == self.__compose_num then
					effectid,effect = self:__CreateEffect({id = 100002,loop = false},self.__panel_compound,3)
					CPPActionManager.DelayTo(self["prop_frame_pos"..i],1.2, GlobalCallbackMgr:AddCallBackFunc(function()	
						--是否一键合成
						if self.__is_onekey_compose then 
							CallModuleFunc(ModuleType.TREASURE,"TreasureComposeReq",self.__selected_treasuresId,self.__choose_num)
						else
							CallModuleFunc(ModuleType.TREASURE,"TreasureComposeReq",self.__selected_treasuresId,1)
						end
					end))	
					PosTool.Center(effect,-25,-20)
				end
			end))			
		end						
	end))	
	
end

function Modules.PlunderedTreasuresView:__SetButtonVisible(isVisible)
	self.__btn_compose:SetVisible(isVisible)
	--一键合成按钮
	if self.__is_btn_onekey_compose_visible then
		self.__btn_onekey_compose:SetVisible(isVisible)
	end
	--一键合成显示条件
	if self.__is_txt_openconditional_visible then
		self.__txt_openconditional:SetVisible(isVisible)
	end
end

function Modules.PlunderedTreasuresView:__HideAllPosFrame()
	for i=1,6 do
		if self["prop_frame_pos"..i] then
			self["prop_frame_pos"..i]:SetVisible(false)
		end
	end
end

function Modules.PlunderedTreasuresView:__SetCompoundVisible()
	for i=1,self.__compose_num do
		self["prop_frame_pos"..i]:SetVisible(true)
	end
end

function Modules.PlunderedTreasuresView:__Dispose()
	if self.__treasures_scrollview then
		self.__treasures_scrollview:DeleteMe()
		self.__treasures_scrollview = nil
	end
end

--新手指引特殊操作检测
function Modules.PlunderedTreasuresView:__CheckSectionOperateSP()
	local operate_id, data_id = CallModuleFunc(ModuleType.LEAD, "GetOperateInfo")
	if operate_id == Macros.Game.LeadOperateSpecialID.PLUNDERED_TREASURES_CHOOSE_TREASURE then
		local locate_index
		self.__treasures_scrollview:ItemDataIter(function(item_index, item_data)
			if item_data.treasureId == data_id then
				locate_index = item_index
				return true
			end
		end)				
		if locate_index then
			self.__treasures_scrollview:LocateItem(locate_index, true)
			local item = self.__treasures_scrollview:GetItem(locate_index)
			if item then
				item:CheckSectionOperateSP()
			end
		end
	end
end

--更新红点
function Modules.PlunderedTreasuresView:__UpdateRedDotNum()
	--计算进度奖励的红点
	local progressreward_reddot = self:CallModuleFunc("CountProgressRewardRedDot")
	if self.__progress_red_dot then
		self.__progress_red_dot:SetVisible(progressreward_reddot > 0)
	end
end

--处理IPhoneX刘海遮挡问题
function Modules.PlunderedTreasuresView:__HandleIPhoneXAdaptation()
    if Macros.Global.IphoneSafeAreaRect then
        local safe_area_origin = Macros.Global.IphoneSafeAreaRect.origin
        local widget = self:GetWidget("Panel_treasureCon")
        widget:SetPosition(safe_area_origin.x, widget:GetPositionY())
    end
end