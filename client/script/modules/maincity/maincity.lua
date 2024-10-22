
--[[
%% @module: 主界面
%% @author: swordman sue
%% @created: 2018-11-6
--]]

Modules = Modules or {}

Modules.MaincityView = Modules.MaincityView or BaseClass(BaseView)

Modules.MaincityView.SECOND_REGION_PANEL_ID = 8
Modules.MaincityView.EXTEND_REGION_PANEL_ID = 88

function Modules.MaincityView:__init()
	self.__layout_name = "maincity"	
	self.__title_bar_params = {
		show_bg = false,
		show_diamond = true,
		show_gold = true,
		show_stamina = true,
		show_energy	= true,
		offset_x = 95 ,
	}

    self.__open_callback = function()  	
    	self:__OpenCallback()
    end

   self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.CITY)   
   self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.CITY, true) 
end

function Modules.MaincityView:__OpenCallback()
	local callback = unpack(self.__open_data)
	if callback then
		callback()
	end
	self.__open_loading = false

	self:__CreateLayers()	
	self:__InitModulesEntrys()	
	self:__InitRoleInfo()	
	self:__InitLineupHeros()
	self:__InitChat()
	self:__InitGM()
end

function Modules.MaincityView:__Dispose()
	CPPGameLib.ClearObjList(self.__lineup_hero_list)
	self.__lineup_hero_list = nil

	if self.__ui_chat_layer then
		self.__ui_chat_layer:DeleteMe()
		self.__ui_chat_layer = nil
	end

	--删除顶级、二级、扩展入口相关
	for region_type, region_layout_info in pairs(Modules.MaincityModule.RegionLayoutMap) do
		Modules.MaincityView.DeleteEntrysByRegionType(region_type)
	end
	--清空所有二级容器、扩展容器记录
	Modules.MaincityView.extend_modules_entrys_map = nil
	Modules.MaincityView.extend_modules_entrys_containers_map = nil		
	Modules.MaincityView.second_modules_entrys_containers_map = nil

	--容错处理(清空所有入口，解绑相关事件与计时器，避免在主界面销毁后，还存在无效引用)
	GlobalMainCityEntryMgr:ClearAllEntrys()
end

--------------------------------------------------------------------------------------------------------------------------------
--事件相关
--------------------------------------------------------------------------------------------------------------------------------
function Modules.MaincityView:__RegistAllEvents()
	--头像更新
	self:BindGlobalEvent(Macros.Event.RoleModule.HEAD_CHANGE, function(_, data)
		self:__UpdateHead()
	end)

	--刷新属性相关
	self:BindGlobalEvent(Macros.Event.RoleModule.PROP_CHANGE, function(_, type, old_value, new_value)
		self:__UpdateRoleProp(type, old_value, new_value)		
	end)

	--重排图标位置
	self:BindGlobalEvent(Macros.Event.MaincityModule.RESET_REGION_ENTRYS, function(_, system_id)
	    local entry_id = self:CallModuleFunc("TranslateSystemIDToEntryID", system_id)	
		local config_entry = CPPGameLib.GetConfig("CityIcon", entry_id, false)	    	
		if config_entry then
			self:__ResetRegionEntrys(config_entry.region_type)		
		end
	end)

	--零点刷新
	self:BindGlobalEvent(Macros.Event.SundryModule.NEW_DAY_NOTIFY, function()
		--重置所有区域的系统入口
		self:__ResetAllRegionEntrys()
	end)	
	
	--更新主界面效果
	self:BindGlobalEvent(Macros.Event.View.UPDATE_MAINCITY_EFFECT, function()
		self:__UpdateMaincityEffect()
	end)
	
	--爵位激活
	self:BindGlobalEvent(Macros.Event.KnighthoodModule.KNIGHTHOOD, function(_,error_code)
		GlobalEffectMgr:SetEffectVisible(self.UI_jueweitongyong , true)
	end)	
end

--处理只有主界面显示的事件
function Modules.MaincityView:HandleOnlyCityViewOpen()	
    self:SetVisible(true)

    --更新主界面效果
    self:__UpdateMaincityEffect()

    --恢复背景音乐
    self:__RecoveryBGM()

    --重置所有区域的系统入口
    if self.__is_need_reset_all_region_entrys then
    	self:__ResetAllRegionEntrys()
    end

    --重置上阵英雄
    if self.__is_need_reset_lineup_heros then
    	self:__InitLineupHeros()    	

    --更新上阵英雄信息
    else
	    self:__UpdateLineupHerosInfo()
	end

    --检测上阵英雄的新手指引
    self:__ChcekLineupHerosLead()

    --设置主城推送使用控件
    self:__PopUseItemPanel()

    --更新VIP礼包
    self:__UpdateVIPGift()

    --爵位红点检测
    self:__UpdateKnighthoodRed()    
end

--处理不只有主界面显示的事件
function Modules.MaincityView:HandleNotOnlyCityViewOpen()	
    self:SetVisible(false)
    Modules.MaincityView.CloseSecondModulesEntrys()
    Modules.MaincityView.CloseExtendModulesEntrys()
end

--刷新角色等级
function Modules.MaincityView:HandleRoleLevelChange(old_var, new_var)
	self.__role_lev:SetText(CPPGameLib.GetString("common_lev8", new_var))

	--重置所有区域的系统入口
	self:__ResetAllRegionEntrys()

	--更新上阵英雄信息
	self:__UpdateLineupHerosInfo()
end

--刷新英雄阵位
function Modules.MaincityView:HandleHeroLineupUpdate()
	self:__InitLineupHeros()
end

--刷新角色属性
function Modules.MaincityView:HandleRolePropChange(prop_type, old_var, new_var)
	if prop_type == Macros.Game.RolePropType.BATTLE_VALUE then
	    local battle_value = new_var
	    if new_var >= 1000000 then
	    	battle_value = math.floor( new_var/10000 )
			self.myriad:SetVisible(true)
		else
			self.myriad:SetVisible(false)
	    end
	    self.__power_font:SetText(tostring(battle_value))
	   	PosTool.RightTo( self.__power_font , self.myriad )
	end
end

--刷新角色名
function Modules.MaincityView:HandleRoleNameChanged()
	local role_name = CallModuleFunc(ModuleType.ROLE, "GetName")
	self.__role_name:SetText(role_name)
end

--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------

--------------------------------------------------------------------------------------------------------------------------------
--静态函数
--------------------------------------------------------------------------------------------------------------------------------
--创建模块入口
function Modules.MaincityView.CreateModulesEntrys(region_type, region_panel, region_layout_info, entry_list, is_cleanup_region_panel)		
	if not region_type then
		LogTool.LogError("[Modules.MaincityView.CreateModulesEntrys] 无效的区域类型")
		return
	end

	if not region_panel then
		LogTool.LogError("[Modules.MaincityView.CreateModulesEntrys] 无效的区域容器")
		return
	end

	--区域布局信息
	region_layout_info = region_layout_info or Modules.MaincityModule.GetRegionLayoutInfo(Modules.MaincityModule.RegionLayoutMap, region_type)

	if not region_layout_info then
		LogTool.LogError("[Modules.MaincityView.CreateModulesEntrys] 无效的区域布局信息")
		return
	end

	--新的入口列表
	entry_list = entry_list or CallModuleFunc(ModuleType.MAIN_CITY, "GetModuleEntrysByRegion", region_type)

	--旧的入口列表
	local org_entry_list
	if region_layout_info.is_extend_region then
		local extend_id = (entry_list and entry_list[1]) and entry_list[1].extend_systemid or nil
		org_entry_list = GlobalMainCityEntryMgr:GetEntrysOnlyExtendRegion(region_type, extend_id)
	else
		org_entry_list = GlobalMainCityEntryMgr:GetEntrysExceptExtendRegion(region_type)
	end 

	--对比新旧列表，移除无效的旧入口
	if org_entry_list and not is_cleanup_region_panel then
		for _, org_entry_info in ipairs(org_entry_list) do
			local invalid = true
			if entry_list then
				for _, now_entry_info in ipairs(entry_list) do
					if now_entry_info.system_id == org_entry_info.system_id then
						invalid = false
						break
					end
				end
			end
			if invalid then
				GlobalMainCityEntryMgr:DeleteEntry(org_entry_info.id)
			end
		end
	end

	--移除该区域所有子节点
	if is_cleanup_region_panel then
		--删除顶级、二级、扩展入口相关
		Modules.MaincityView.DeleteEntrysByRegionType(region_type)
		--移除所有子节点
		region_panel:RemoveAllChild(true)
	end

	if entry_list then
		local entry_widget_x, entry_widget_y, entry_btn, entry_txt

		--初始化布局信息
		Modules.MaincityModule.InitRegionLayoutInfo(region_layout_info)

		local real_entry_list
		local entry_show_end_index = math.min(region_layout_info.entry_show_end_index, #entry_list)
		for index = region_layout_info.entry_show_beg_index, entry_show_end_index do
			local entry_info = entry_list[index]
			local entry = GlobalMainCityEntryMgr:GetEntryById(entry_info.id)
			local btn_sp = UIButtonEx.CreateWithImage(Resource.PathTool.GetSystemEntryIconPath(entry_info.res_name), TextureResType.LOCAL)
			if not entry and btn_sp then
				--创建入口
				entry = GlobalMainCityEntryMgr:CreateEntry(region_layout_info, entry_info, region_panel)	
			end

			if entry then
				--设置入口位置
				entry:InitPosition(index, entry_list)
				entry:SetInitSuccess(true)
				entry:UpdateEntryRedDot()

				real_entry_list = real_entry_list or {}
				table.insert(real_entry_list, entry_info)
			end
		end
	end
end

--打开模块二级入口弹窗
function Modules.MaincityView.OpenSecondModulesEntrysByRegion(region_type, entry_widget)
	Modules.MaincityView.second_modules_entrys_containers_map = Modules.MaincityView.second_modules_entrys_containers_map or {}
	local second_container = Modules.MaincityView.second_modules_entrys_containers_map[region_type]
	if not second_container then
		--创建二级区域背景
		second_container = Util:Layout(8, 8)
		second_container:SetAnchorPoint(0.5,0)
		second_container:SetTouchEnabled(true)
		second_container:SetVisible(true)		
		second_container:SetBackGroundImageScale9Enabled(true)
		second_container:SetBackGroundImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY, "bg_more"))		
		second_container:SetTag(region_type)
		entry_widget:AddChild(second_container)

		local second_container_arrows = Util:Sprite9(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY, "bg_more_arrows"))
		second_container_arrows:SetVisible(true)
		second_container_arrows:SetAnchorPoint(0.5, 1)
		second_container:AddChild(second_container_arrows)
		PosTool.CenterBottom(second_container_arrows,45,0.5)
		Modules.MaincityView.second_modules_entrys_containers_map[region_type] = second_container

		--初始化模块二级入口
		Modules.MaincityView.InitSecondModulesEntrysByRegion(region_type)
		PosTool.CenterTop(second_container,45,120)
		second_container:SetScaleFactor(0.3)
		second_container:SetVisible(false)
	end

	local entry_btn = GlobalMainCityEntryMgr:GetEntryBtnById(Modules.MaincityModule.BOTTOM_LEFT_MORE_ENERY_ID)
	CPPActionManager.RotateTo(entry_btn,not second_container:IsVisible() and ct or ct_2,not second_container:IsVisible() and 45 or 0)

	local entry_red_dot = GlobalMainCityEntryMgr:GetEntryRedDotById(Modules.MaincityModule.BOTTOM_LEFT_MORE_ENERY_ID)
	local ct = 0.13
	local ct_2 = 0.18
	if not second_container:IsVisible() then
		entry_red_dot:SetVisible(false)
		second_container:SetVisible(true)
		if CallModuleFunc(ModuleType.LEAD, "IsInLead") then
			--指引过程中，直接打开
			second_container:SetScaleFactor(1)
		else
			CPPActionManager.ScaleTo(second_container,ct,1)
		end
	else
		CPPActionManager.ScaleTo(second_container,ct_2,0.3,GlobalCallbackMgr:AddCallBackFunc(function ()
			second_container:SetVisible(false)
			local red_dot_num = Modules.MaincityView.GetEneryMoreRedDotNum()
			entry_red_dot:SetVisible(red_dot_num > 0)
		end))
	end
end

--打开模块扩展弹窗
function Modules.MaincityView.OpenExtendModulesEntrysByRegion(region_type, entry_widget, extend_id)
	Modules.MaincityView.extend_modules_entrys_containers_map = Modules.MaincityView.extend_modules_entrys_containers_map or {}
	local extend_container = Modules.MaincityView.extend_modules_entrys_containers_map[extend_id]
	if not extend_container then
		--创建二级区域背景
		extend_container = Util:Layout(8, 8)
		extend_container:SetTouchEnabled(true)
		extend_container:SetBackGroundImageScale9Enabled(true)
		extend_container:SetBackGroundImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY, "bg_more"))		
		entry_widget:AddChild(extend_container)
	
		Modules.MaincityView.extend_modules_entrys_containers_map[extend_id] = extend_container

		--初始化模块扩展入口
		Modules.MaincityView.InitExtendModulesEntrysByRegion(region_type, extend_id)
			
		extend_container:SetScaleFactor(0.3)
		extend_container:SetVisible(false)
	end

	local ct = 0.13
	local ct_2 = 0.18
	if not extend_container:IsVisible() then
		extend_container:SetVisible(true)
		if CallModuleFunc(ModuleType.LEAD, "IsInLead") then
			--指引过程中，直接打开
			extend_container:SetScaleFactor(1)
		else
			CPPActionManager.ScaleTo(extend_container,ct,1)
		end
	else
		CPPActionManager.ScaleTo(extend_container,ct_2,0.3,GlobalCallbackMgr:AddCallBackFunc(function ()
			extend_container:SetVisible(false)
		end))
	end
end

--初始化模块二级入口
function Modules.MaincityView.InitSecondModulesEntrysByRegion(region_type)
	--二级区域布局信息
	local region_layout_info = Modules.MaincityModule.GetRegionLayoutInfo(Modules.MaincityModule.SecondRegionLayoutMap, region_type, true)
	local second_container = Modules.MaincityView.second_modules_entrys_containers_map and Modules.MaincityView.second_modules_entrys_containers_map[region_type] or nil
	if not second_container then
		return
	end

	local second_region_panel = second_container:GetChildByTag(Modules.MaincityView.SECOND_REGION_PANEL_ID)
	if not second_region_panel then
		second_region_panel = Util:Layout(100, 100)
		second_region_panel:SetTag(Modules.MaincityView.SECOND_REGION_PANEL_ID)
		second_region_panel:SetAnchorPoint(0.5, 0.5)
		second_container:AddChild(second_region_panel)
	end

	--入口列表
	local entry_list = CallModuleFunc(ModuleType.MAIN_CITY, "GetModuleEntrysByRegion", region_type)

	--设置二级区域容器尺寸
	local row = region_layout_info.row
	local col = region_layout_info.col
	local item_margin_x = math.abs(region_layout_info.item_margin_x)
	local item_margin_y = math.abs(region_layout_info.item_margin_y)
	local second_entry_num = #entry_list - region_layout_info.entry_show_beg_index + 1
	if row > col then
		row = math.ceil(second_entry_num / region_layout_info.col)
		col = math.min(col,second_entry_num)
	else
		col = math.ceil(second_entry_num / region_layout_info.row)
	end
	second_region_panel:SetContentSize(col * item_margin_x - (item_margin_x - region_layout_info.item_w), row * item_margin_y - (item_margin_y - region_layout_info.item_h) + 10)

	--设置二级区域背景尺寸
	local second_region_panel_size = second_region_panel:GetContentSize()
	second_container:SetAnchorPoint(0,30/(second_region_panel_size.height + 18))
	second_container:SetContentSize(second_region_panel_size.width + 10, second_region_panel_size.height + 18)
	PosTool.Center(second_region_panel)	

	--创建二级模块入口
	Modules.MaincityView.CreateModulesEntrys(region_type, second_region_panel, region_layout_info, entry_list)
end

--初始化模块扩展入口
function Modules.MaincityView.InitExtendModulesEntrysByRegion(region_type, extend_id)
	local extend_container = Modules.MaincityView.extend_modules_entrys_containers_map and Modules.MaincityView.extend_modules_entrys_containers_map[extend_id] or nil

	--扩展区域布局信息
	local region_layout_info = Modules.MaincityModule.GetRegionLayoutInfo(Modules.MaincityModule.ExtendRegionLayoutMap, region_type, true)

	--方向
	local is_updir = region_layout_info.is_updir
	if not extend_container then
		return
	end
	local extend_region_panel = extend_container:GetChildByTag(Modules.MaincityView.EXTEND_REGION_PANEL_ID)
	if not extend_region_panel then
		extend_region_panel = Util:Layout(100, 100)
		extend_region_panel:SetTag(Modules.MaincityView.EXTEND_REGION_PANEL_ID)
		extend_region_panel:SetAnchorPoint(0.5, 0.5)
		extend_container:AddChild(extend_region_panel)
	end
	
	--入口列表
	local entry_list = CallModuleFunc(ModuleType.MAIN_CITY, "GetModuleEntrysByExtendId", extend_id)
	Modules.MaincityView.extend_modules_entrys_map = Modules.MaincityView.extend_modules_entrys_map or {}
	Modules.MaincityView.extend_modules_entrys_map[extend_id] = entry_list

	--设置扩展区域容器尺寸
	local row = region_layout_info.row
	local col = region_layout_info.col
	local item_margin_x = math.abs(region_layout_info.item_margin_x)
	local item_margin_y = math.abs(region_layout_info.item_margin_y)
	local extend_entry_num = #entry_list
	if row > col then
		row = math.ceil(extend_entry_num / region_layout_info.col)
		col = math.min(col,extend_entry_num)
	else
		col = math.ceil(extend_entry_num / region_layout_info.row)
	end
	extend_region_panel:SetContentSize(col * item_margin_x - (item_margin_x - region_layout_info.item_w), row * item_margin_y - (item_margin_y - region_layout_info.item_h) + 10)

	--设置扩展区域背景尺寸
	local extend_region_panel_size = extend_region_panel:GetContentSize()
	extend_container:SetAnchorPoint(is_updir and 30/(extend_region_panel_size.width + 34) or ((extend_region_panel_size.width + 4)/(extend_region_panel_size.width + 34)), is_updir and 0 or 1)
	extend_container:SetContentSize(extend_region_panel_size.width + 34, extend_region_panel_size.height + 18)		
	PosTool.Center(extend_region_panel)	
	local pos_x = (is_updir and 30 or -55) + (region_layout_info.extend_container_offset_x or 0)
	local pos_y = (is_updir and 100 or -45) + (region_layout_info.extend_container_offset_y or 0)
	extend_container:SetPosition(pos_x,pos_y)

	--箭头
	local extend_container_arrows = Util:Sprite9(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY, "bg_more_arrows"))
	extend_container_arrows:SetRotation(is_updir and -90 or 90)
	extend_container_arrows:SetAnchorPoint(1,is_updir and 1 or 0)
	extend_container:AddChild(extend_container_arrows)		
	extend_container_arrows:SetPosition(is_updir and 30 or (extend_region_panel_size.width + 4),is_updir and 3 or (extend_container:GetContentHeight() - 3))	
	
	--创建扩展模块入口
	Modules.MaincityView.CreateModulesEntrys(region_type, extend_region_panel, region_layout_info, entry_list)
end

--关闭模块二级入口弹窗
function Modules.MaincityView.CloseSecondModulesEntrys(time)
	if not Modules.MaincityView.second_modules_entrys_containers_map then
		return
	end

	local ct = time or 0.13
	local entry_btn = GlobalMainCityEntryMgr:GetEntryBtnById(Modules.MaincityModule.BOTTOM_LEFT_MORE_ENERY_ID)
	CPPActionManager.RotateTo(entry_btn,ct,0,GlobalCallbackMgr:AddCallBackFunc(function ()
		local entry_red_dot = GlobalMainCityEntryMgr:GetEntryRedDotById(Modules.MaincityModule.BOTTOM_LEFT_MORE_ENERY_ID)
		local red_dot_num = Modules.MaincityView.GetEneryMoreRedDotNum()
		if entry_red_dot then
			entry_red_dot:SetVisible(red_dot_num > 0)
		end
	end))
	
	local second_container
	for region_type, region_layout_info in pairs(Modules.MaincityModule.SecondRegionLayoutMap) do
		second_container = Modules.MaincityView.second_modules_entrys_containers_map[region_type]
		if second_container then
			CPPActionManager.ScaleTo(second_container,ct,0.3,GlobalCallbackMgr:AddCallBackFunc(function ()
				second_container:SetVisible(false)
			end))
		end
	end
end

--关闭模块扩展入口弹窗
function Modules.MaincityView.CloseExtendModulesEntrys(cur_extend_id, time)
	if not Modules.MaincityView.extend_modules_entrys_containers_map then
		return
	end
	local ct_2 = time or 0.13
	for extend_id, extend_container in pairs(Modules.MaincityView.extend_modules_entrys_containers_map) do 
		if not cur_extend_id or extend_id ~= cur_extend_id then
			if extend_container:IsVisible() then
				CPPActionManager.ScaleTo(extend_container,ct_2,0.3,GlobalCallbackMgr:AddCallBackFunc(function ()
					extend_container:SetVisible(false)
				end))
			end
		end		
	end
end

--[[
1.扩展容器里面的图标要比扩展容器先删，添加扩展容器的图标要比扩展容器后删。
2.二级容器里面的图标要比二级容器先删，添加二级容器的图标要比二级容器后删。
]]--
--删除某个区域的顶级、二级、扩展入口相关
function Modules.MaincityView.DeleteEntrysByRegionType(region_type, ignore_top_level_entrys)
	local org_entry_list = GlobalMainCityEntryMgr:GetEntrysByRegion(region_type, ignore_top_level_entrys)

	if org_entry_list then
		local tmp_extend_map = {}
		local delay_del_list = {}

		for _, org_entry_info in ipairs(org_entry_list) do		
			if org_entry_info.is_extend then
				tmp_extend_map[org_entry_info.is_extend] = true
				table.insert(delay_del_list, org_entry_info.id)
			
			elseif org_entry_info.is_more then
				table.insert(delay_del_list, org_entry_info.id)
			
			else
				if org_entry_info.extend_systemid then
					tmp_extend_map[org_entry_info.extend_systemid] = true
				end
				--删除部分顶级入口、所有二级容器和扩展容器中的入口
				GlobalMainCityEntryMgr:DeleteEntry(org_entry_info.id)
			end					
		end			

		--清空二级容器记录
		if Modules.MaincityView.second_modules_entrys_containers_map then
			if Modules.MaincityView.second_modules_entrys_containers_map[region_type] then
				Modules.MaincityView.second_modules_entrys_containers_map[region_type]:RemoveSelf(true)
			end
			Modules.MaincityView.second_modules_entrys_containers_map[region_type] = nil
		end			

		--清空扩展容器记录
		for extend_id, _ in pairs(tmp_extend_map) do		
			if Modules.MaincityView.extend_modules_entrys_map then
				Modules.MaincityView.extend_modules_entrys_map[extend_id] = nil			
			end
			if Modules.MaincityView.extend_modules_entrys_containers_map then
				if Modules.MaincityView.extend_modules_entrys_containers_map[extend_id] then
					Modules.MaincityView.extend_modules_entrys_containers_map[extend_id]:RemoveSelf(true)
				end
				Modules.MaincityView.extend_modules_entrys_containers_map[extend_id] = nil		
			end
		end		

		--延迟删除包含二级容器或扩展容器的顶级入口
		for _, entry_id in ipairs(delay_del_list) do
			GlobalMainCityEntryMgr:DeleteEntry(entry_id)
		end
	end
end

--获取左下方二级入口红点数
function Modules.MaincityView.GetEneryMoreRedDotNum()
	local limit_num = CallModuleFunc(ModuleType.MAIN_CITY, "GetBotttomEntryLimitNumByRegion", Modules.MaincityModule.RegionType.BOTTOM_LEFT)
	local entry_list = CallModuleFunc(ModuleType.MAIN_CITY, "GetModuleEntrysByRegion", Modules.MaincityModule.RegionType.BOTTOM_LEFT)
	local entry_info
	local red_dot_num = 0
	for i = limit_num + 2, #entry_list do
		entry_info = entry_list[i]
		if GlobalModulesMgr:IsOpen(entry_info.system_id) then
			red_dot_num = red_dot_num + CallModuleFunc(ModuleType.RED_MGR, "GetRed", entry_info.system_id, Macros.Game.SystemID.MAX)
			if red_dot_num > 0 and not Modules.Red.BaseRed.IS_SHOW_NUM then
				break
			end
		end
	end
	return red_dot_num
end

--获取某扩展区域入口红点数
function Modules.MaincityView.GetEneryExtendRedDotNum(extend_id)
	local entry_list
	if not Modules.MaincityView.extend_modules_entrys_map or not Modules.MaincityView.extend_modules_entrys_map[extend_id] then
		entry_list = CallModuleFunc(ModuleType.MAIN_CITY, "GetModuleEntrysByExtendId", extend_id)
	else
		entry_list = Modules.MaincityView.extend_modules_entrys_map[extend_id]
	end

	local red_dot_num = 0
	if entry_list then
		for i, entry_info in ipairs(entry_list) do
			if GlobalModulesMgr:IsOpen(entry_info.system_id) then
				red_dot_num = red_dot_num + CallModuleFunc(ModuleType.RED_MGR, "GetRed", entry_info.system_id, Macros.Game.SystemID.MAX)
				if red_dot_num > 0 and not Modules.Red.BaseRed.IS_SHOW_NUM then
					break
				end
			end
		end
	end
	return red_dot_num
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数（初始化相关）
--------------------------------------------------------------------------------------------------------------------------------
--创建背景层
function Modules.MaincityView:__CreateLayers()
	self.__bg_params = 
	{
		image_path_list = {"bg_maincity_beijing","bg_maincity_qianjing"},image_size = COCOSize(1136, 640),
		touch_func = function()
			Modules.MaincityView.CloseSecondModulesEntrys()
			Modules.MaincityView.CloseExtendModulesEntrys()
		end,
	}
	self.__bg = Modules.MainCityBG.New(self.__bg_params, self)						
	self:AddChild(self.__bg, Macros.Global.z_order_bottom)
	PosTool.Center(self.__bg)

	--若背景图可点击，则设置根节点可穿透
	self.__root_widget:SetTouchSwallow(false)					
end

--初始化角色相关
function Modules.MaincityView:__InitRoleInfo()
	local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")

	local panel_role = self:GetWidget("Panel_Role")

	--角色名字、等级	
	self.__role_name = Util:WidgetLabel(self:GetWidget("Text_RoleName"), role_info.role_name, Macros.TypefaceSize.slightly,Macros.Color.maincity_rolename,0,0,Macros.Color.maincity_rolename_stroke)
	self.__role_lev = Util:WidgetLabel(self:GetWidget("Text_RoleLev"), CPPGameLib.GetString("common_lev8", role_info.level),Macros.TypefaceSize.slightly,Macros.Color.maincity_rolename,0,0,Macros.Color.maincity_rolename_stroke)

	--VIP
	local panel_vip = self:GetWidget("Panel_VIP")
	local panel_vip_touch = self:GetWidget("Panel_VIP_Touch")
    self.__vip_font = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "poodlenum", true))
    self.__vip_font:SetText(tostring(role_info.vip_level))
    self.__vip_font:SetAnchorPoint(0.5, 0.5)
    panel_vip:AddChild(self.__vip_font:GetNode())	
    PosTool.Center(self.__vip_font, 0, 1)
    WidgetUnity.SetWidgetListener(panel_vip_touch, nil, nil, function()
    	--打开VIP界面
    	CallModuleFunc(ModuleType.VIP, "OpenView", BaseViewType.VIP)
    end)

    --系统未开启或隐藏主界面VIP
	local img_vip = self:GetWidget("Image_VIP")
    if not GlobalModulesMgr:IsOpen(Macros.Game.SystemID.VIP) or GlobalCustomizedLogicAdapterMgr:Call("IsHideMaincityVIP") then
    	img_vip:SetVisible(false)
    	panel_vip_touch:SetVisible(false)
    end

    --初始化主界面GM等级图标
    self.__img_gm_level = GlobalCustomizedLogicAdapterMgr:Call("InitMaincityGMLevel", img_vip)

    --VIP礼包
	self.Image_vip_oem = self:GetWidget("Image_vip_oem")
	self.Image_vip_oem:SetVisible(false)

    --战力
    local panel_rolepower = self:GetWidget("Panel_RolePower")
    self.__power_font = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "maincity_battle_num", true))
    self.__power_font:SetAnchorPoint(0, 0.5)
    self.__power_font:SetScaleFactor(0.9)
    panel_rolepower:AddChild(self.__power_font:GetNode())
    PosTool.LeftCenter(self.__power_font, 5, 0)

    self.myriad = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "myriad", true) )
    panel_rolepower:AddChild(self.myriad)
    self.myriad:SetScaleFactor(0.9)
	self.myriad:SetVisible(false)

    local battle_value = role_info.battle_value
    if role_info.battle_value >= 1000000 then
    	battle_value = math.floor( role_info.battle_value/10000 )
		self.myriad:SetVisible(true)
    end
    self.__power_font:SetText(tostring(battle_value))
   	PosTool.RightTo( self.__power_font , self.myriad )

    --角色头像    
    local panel_role_head = self:GetWidget("Panel_RoleHead")
    self.role_icon = Util:Sprite(Resource.PathTool.GetHeroHeadPath(role_info.head_icon))
    panel_role_head:AddChild(self.role_icon, -1)
    PosTool.Center(self.role_icon)

    --点击头像
    WidgetUnity.SetWidgetListener(panel_role, nil, nil, function()
    	--打开角色信息界面
    	CallModuleFunc(ModuleType.ROLE, "OpenView", BaseViewType.ROLEINFO)
    end)

   --角色爵位    
    local img_medal = self:GetWidget("Image_2")
    local nobility = CallModuleFunc(ModuleType.ROLE,"GetNobility")
	local effect
	self.UI_jueweitongyong , effect = self:__CreateEffect({res_name = "UI_jueweitongyong"} , img_medal )
	PosTool.Center(effect , -10 , -24 )
	GlobalEffectMgr:SetEffectVisible(self.UI_jueweitongyong , false)

    if nobility and nobility >= 1 then
    	GlobalEffectMgr:SetEffectVisible(self.UI_jueweitongyong , true)
	end
    self.nobility = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_level_" .. nobility , true)  )
    img_medal:AddChild(self.nobility)
    PosTool.Center(self.nobility , -20 , -30 )

    WidgetUnity.SetWidgetListener(self:GetWidget("Panel_Knighthood_Touch"), nil, nil, function()
    	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
		local IsOpen = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.KNIGHTHOOD )
		if IsOpen then
	    	--打开爵位界面
	    	CallModuleFunc(ModuleType.KNIGHTHOOD, "OpenView", BaseViewType.KNIGHTHOOD)
	    else
	    	GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("common_system_unopen") )
		end
    end)

    --经验条
    local imgdata_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_little_maincity_bar")
    local imgdata_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_little_maincity_bg")
    self.__bar_exp = GUI.ProgressBar9Layer.New(imgdata_bar, imgdata_bg , 187 , 8 , 191 , 10 , 0, 100)
    self.__bar_exp:SetAnchorPoint(0, 0.5)
    self.__bar_exp:SetPosition( 0 , 5 )
    self.__bar_exp:SetProgress(math.min(1,role_info.exp/role_info.max_exp)* 100)
    panel_role:AddChild(self.__bar_exp:GetNode())

    self.__bar_exp_text = Util:Name( "" , 14 )
    panel_role:AddChild( self.__bar_exp_text )
    self.__bar_exp_text:SetPosition( self.__bar_exp:GetPositionX() + 95.5 , self.__bar_exp:GetPositionY() )    
    self.__bar_exp_text:SetString(math.floor( (role_info.exp / role_info.max_exp) * 100 ) .. "%")  
end

--初始化模块入口
function Modules.MaincityView:__InitModulesEntrys()
	for region_type, region_layout_info in pairs(Modules.MaincityModule.RegionLayoutMap) do
		self:__ResetRegionEntrys(region_type, nil, region_layout_info, nil, false)
	end
end

--初始化上阵英雄
function Modules.MaincityView:__InitLineupHeros()
	if not self:IsOpen() then
		self.__is_need_reset_lineup_heros = true
		return
	end	
	self.__is_need_reset_lineup_heros = nil

	local panel_heros = self:GetWidget("Panel_Heros")
	local offset_l = Vector2D.New(-180, 20)
	local offset_r = Vector2D.New(180, 20)
	local layout_list = 
	{
		{offset = Vector2D.New(), zorder = 5},
		{offset = offset_l, zorder = 4},
		{offset = offset_r, zorder = 3},
		{offset = offset_l:Multi(2), zorder = 2},
		{offset = offset_r:Multi(2), zorder = 1},
	}

	CPPGameLib.ClearObjList(self.__lineup_hero_list)
	self.__lineup_hero_list = {}

	local lineup_pos_list = {}
	local lineup_hero_layer, hero_info, layout_info
	local full_lineup_list = CallModuleFunc(ModuleType.HERO, "GetLineupList")
	
	--创建已上阵英雄
	for i, lineup_info in ipairs(full_lineup_list) do
		hero_info = CallModuleFunc(ModuleType.HERO, "GetHeroInfo", lineup_info.uid)
		if hero_info then			
			layout_info = layout_list[#self.__lineup_hero_list + 1]
			lineup_hero_layer = Modules.MaincityHeroLineupLayer.New()
			lineup_hero_layer:SetData(i, hero_info)
			panel_heros:AddChild(lineup_hero_layer:GetNode(), layout_info.zorder)
			PosTool.CenterBottom(lineup_hero_layer, layout_info.offset.x, layout_info.offset.y)
			table.insert(self.__lineup_hero_list, lineup_hero_layer)
		else
			table.insert(lineup_pos_list, i)
		end
	end

	--创建未上阵条目
	local lineup_num = #self.__lineup_hero_list 
	for i, lineup_pos in ipairs(lineup_pos_list) do
		layout_info = layout_list[lineup_num + i]
		lineup_hero_layer = Modules.MaincityHeroLineupLayer.New(true, lineup_pos)
		panel_heros:AddChild(lineup_hero_layer:GetNode(), layout_info.zorder)		
		PosTool.CenterBottom(lineup_hero_layer, layout_info.offset.x, layout_info.offset.y)

		table.insert(self.__lineup_hero_list, lineup_hero_layer)				
	end
end

--初始化聊天相关
function Modules.MaincityView:__InitChat()
 	local node = self:GetWidget("Panel_Chat")	

	self.__ui_chat_layer = Modules.MainCityChatLayer.New(node)
	self.__ui_chat_layer:SetAnchorPoint( 0.5,0.5 )
	node:AddChild(self.__ui_chat_layer:GetNode())
	PosTool.Center(self.__ui_chat_layer)

    local layer = Util:Layout( node:GetContentWidth() , node:GetContentHeight() ,function ( ... )
        CallModuleFunc(ModuleType.CHAT, "OpenView", BaseViewType.CHAT)
    end )
    layer:SetAnchorPoint(0.5,0.5)
    node:AddChild(layer , 99 )
    PosTool.Center( layer )

    local entry_red_dot_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
	self.__chat_red_point = Util:Sprite(entry_red_dot_image_data)
	self.__chat_red_point:SetPosition( node:GetContentWidth(),node:GetContentHeight())
	self.__chat_red_point:SetVisible(false)
	node:AddChild(self.__chat_red_point)
end

--爵位红点
function Modules.MaincityView:__UpdateKnighthoodRed()
    local entry_red_dot_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
    if not self.knighthood_red_point then
	    --角色爵位    
	    local img_medal = self:GetWidget("Image_2")
		self.knighthood_red_point = Util:Sprite( entry_red_dot_image_data )
		self.knighthood_red_point:SetVisible(false)
		
		self.knighthood_red_point:SetPosition( img_medal:GetContentWidth() - self.knighthood_red_point:GetContentWidth() - 30 , ( img_medal:GetContentHeight()/2 ) - 30 )
		img_medal:AddChild( self.knighthood_red_point )
    end

    --当前打开了的
    local cur_lock_index = CallModuleFunc(ModuleType.KNIGHTHOOD, "GetActivateRecord")
    local my_combat_value , need_power = CallModuleFunc(ModuleType.KNIGHTHOOD, "GetNeedFighting" , cur_lock_index + 1 )

	local IsOpen = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.KNIGHTHOOD )
	local roleLevel = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)

    if need_power and my_combat_value >= need_power and IsOpen then
    	self.knighthood_red_point:SetVisible(true)
    else
    	self.knighthood_red_point:SetVisible(false)
    end
end

--设置主城推送使用控件
function Modules.MaincityView:__PopUseItemPanel()
	--打开主界面前先更新一下弹出使用数据
    self:CallModuleFunc("UpdatePopUseData")
	local pop_new_item_list = self:CallModuleFunc("GetPopNewItemList")
	--队列第一位的数据
	self.__first_pop_data = pop_new_item_list[1]
	if not self.__is_init_pop_panel then
		--初始化弹出控件
		self.__is_init_pop_panel = true
		--统一处理方法
		self.__pop_same_handle = function ()
			--移除主界面弹出使用物品数据
    		self:CallModuleFunc("RemovePopUseData")
    		--更新
    		self:__PopUseItemPanel()
		end
		--弹出新物品信息控件
		self.__image_pop = self:GetWidget("Image_Pop")
		--新道具
		Util:WidgetLabel(self:GetWidget("Text_NewPopProp"), CPPGameLib.GetString("maincity_pop_new_prop"), Macros.TypefaceSize.normal, Macros.Color.btn, 0, 0, Macros.Color.content)
		--关闭信息
		Util:WidgetButton(self:GetWidget("Button_PosClose"),"",function ()
			self.__pop_same_handle()
		end)
		--图标控件
		self.__panel_popicon = self:GetWidget("Panel_PopIcon")
		--点击使用
		Util:WidgetLabel(self:GetWidget("Text_TouchUse"), CPPGameLib.GetString("maincity_touch_to_use"), Macros.TypefaceSize.slightly, Macros.Color.Special)	
		--点击使用控件		
		WidgetUnity.SetWidgetListener(self:GetWidget("Panel_TouchUse"), nil, nil,function ()
			if not self.__first_pop_data then
				return
			end
			--物品信息
	    	local item_config = CPPGameLib.GetConfig("Item",self.__first_pop_data.item_id)
		    --如果类型是普通道具 与 可以在背包使用
		    if item_config.type == 1 and item_config.is_can_use == 1 then
		        -- print("如果类型是普通道具 与 可以在背包使用")		        
		        --如果数量大于1
		        if self.__first_pop_data.item_num == 1 then
		            local data = {}
		            data.item_id = self.__first_pop_data.item_id
		            data.item_num = 1
		            data.select_gift_id = 0
		            CallModuleFunc(ModuleType.Pack, "ItemUseReq",data)   
		            self.__pop_same_handle()
		        else     
		            GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.PACK_ONEUSE , false , self.__first_pop_data.item_id )   
		            self.__pop_same_handle() 
		        end

		    --否则类型是礼包 而且还是多选礼包
		    elseif item_config.type == 2 and item_config.sub_type == 2 then
		        -- print("否则类型是礼包 而且还是多选礼包")		        
		        local item_config = CPPGameLib.GetConfig("Item",self.__first_pop_data.item_id)
		        if item_config then
		        	local gift_data = CPPGameLib.CopyTblShallow(item_config)
		        	gift_data.item_num = self.__first_pop_data.item_num
		        	gift_data.item_id = self.__first_pop_data.item_id
			        local onuse = GUITemplateMgr:CreateTemplateInstance( GUI.TemplateRegistry.PACK_SELECT , false , gift_data )	 
			        self.__pop_same_handle() 
		        end  
		    else
		    	--再否则 只是礼包
		        CallModuleFunc(ModuleType.SUNDRY, "GiftBagPackVacancy", self.__first_pop_data.item_id , nil , true , function (nil_list)
		            if #nil_list == 0 then
		            	if self.__first_pop_data.item_num > 1 then
		            		GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.PACK_ONEUSE , false , self.__first_pop_data.item_id)
		            	else
		            		local data = {}
			                data.item_id = self.__first_pop_data.item_id
			                data.item_num = self.__first_pop_data.item_num
			                data.select_gift_id = 0
			                CallModuleFunc(ModuleType.Pack, "ItemUseReq",data) 
		            	end		                		                
		                self.__pop_same_handle()    
		            end
		        end )
		    end
		end, nil)
	end
	--显隐
	self.__image_pop:SetVisible(#pop_new_item_list > 0)
	if #pop_new_item_list > 0 then
		--设置图标
		if not self.__pop_icon then
			self.__pop_icon = LogicGUI.IconLayer.New(false,false)
			self.__panel_popicon:AddChild(self.__pop_icon:GetNode())
			PosTool.Center(self.__pop_icon)			
		end
		self.__pop_icon:SetData({id = self.__first_pop_data.item_id,num = self.__first_pop_data.item_num})
		self.__pop_icon:ShowItemName(true,0,0,1.25)
   	end
end

--更新VIP礼包
function Modules.MaincityView:__UpdateVIPGift()	
	if GlobalCustomizedLogicAdapterMgr:Call("IsHideMaincityVIP") then
		return
	end

	local t1 = CallModuleFunc(ModuleType.SHOP , "GetPurchasableGiftList" )
	if #t1 ~= 0 then
		local cnf = CPPGameLib.GetConfig("Vip", t1[#t1].need_vip_level , false )
		if cnf then
			local lst = CPPGameLib.SplitEx(cnf.tips , "<GH>")

			if not self.__VipOem_t1 then
				self.__VipOem_t1 = Util:Name( lst[1] , Macros.TypefaceSize.slightly ,100 )
				self.__VipOem_t1:SetPosition( self.Image_vip_oem:GetContentWidth()/2 + 6 , self.Image_vip_oem:GetContentHeight()/2 + 14 )
				self.Image_vip_oem:AddChild( self.__VipOem_t1 )

				self.__VipOem_t2 = Util:Name( lst[2] , Macros.TypefaceSize.slightly ,100 )
				self.__VipOem_t2:SetPosition( self.Image_vip_oem:GetContentWidth()/2 + 6 , self.Image_vip_oem:GetContentHeight()/2 - 14 )
				self.Image_vip_oem:AddChild( self.__VipOem_t2 )
			end

			self.__VipOem_t1:SetString( lst[1] )
			self.__VipOem_t2:SetString( lst[2] )
			ActionManager:ZoomAnimation(self.Image_vip_oem,1, 1.02 , 0.98 )
			self.Image_vip_oem:SetVisible(true)
		end
	else
		self.Image_vip_oem:SetVisible(false)
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数（处理相关）
--------------------------------------------------------------------------------------------------------------------------------
--重置某区域的系统入口
function Modules.MaincityView:__ResetRegionEntrys(region_type, region_panel, region_layout_info, entry_list, cleanup)
	if not region_type then
		return
	end

	--删除二级、扩展入口相关
	if cleanup ~= false then
		Modules.MaincityView.DeleteEntrysByRegionType(region_type, true)
	end

	--区域容器
	region_panel = region_panel or self:GetWidget("Panel_Region"..region_type)

	--重置系统入口
	Modules.MaincityView.CreateModulesEntrys(region_type, region_panel, region_layout_info, entry_list)
end

--重置所有区域的系统入口
function Modules.MaincityView:__ResetAllRegionEntrys()
	if not self:IsOpen() then
		self.__is_need_reset_all_region_entrys = true
		return
	end
	self.__is_need_reset_all_region_entrys = nil

	--入口表对比
	local function compare_entry_list(list1, list2)
		if not list1 and not list2 then
			return true
		end

		if (not list1 and list2) or (list1 and not list2) then
			return false
		end
		
		if #list1 ~= #list2 then
			return false
		end

		for i, entry_info in ipairs(list1) do
			if entry_info.system_id ~= list2[i].system_id then
				return false
			end
		end

		return true
	end

	--刷新入口
	local entry_list, org_entry_list
	local region_panel
	for region_type, region_layout_info in pairs(Modules.MaincityModule.RegionLayoutMap) do
		entry_list = self:CallModuleFunc("GetModuleEntrysByRegion", region_type)	
		org_entry_list = GlobalMainCityEntryMgr:GetEntrysExceptExtendRegion(region_type)			

		--检测区域的系统入口是否发生变化
		if not compare_entry_list(org_entry_list, entry_list) then
			--重置某区域的系统入口
			self:__ResetRegionEntrys(region_type, nil, region_layout_info, entry_list)
		end
	end
end

--恢复背景音乐
function Modules.MaincityView:__RecoveryBGM()
	local sound_info = GlobalViewMgr:GetViewSoundInfo(self.__type)
	if sound_info then
		GlobalSoundMgr:PlayBGM(sound_info.open_bgm, nil, sound_info.volume, true)
	end
end

--检测英雄上阵的新手指引
function Modules.MaincityView:__ChcekLineupHerosLead()
	if self.__lineup_hero_list then
		for _, lineup_layer in ipairs(self.__lineup_hero_list) do
			lineup_layer:CheckSectionOperateSP()
		end
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数（更新相关）
--------------------------------------------------------------------------------------------------------------------------------
--更新头像
function Modules.MaincityView:__UpdateHead()
	--角色头像
    if self.role_icon then
		local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")    	
        self.role_icon:SetImage(Resource.PathTool.GetHeroHeadPath(role_info.head_icon))
    end   
end

--更新属性相关
function Modules.MaincityView:__UpdateRoleProp(type, old_value, new_value)
	--Vip
    if type == Macros.Game.RolePropType.VIP_LEVEL and (new_value > old_value) then
    	if self.__vip_font then
			self.__vip_font:SetText(tostring(new_value))
		end

	--爵位
	elseif type == Macros.Game.RolePropType.NOBILITY then
		local nobility = CallModuleFunc(ModuleType.ROLE,"GetNobility")
		self.nobility:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_level_" .. nobility , true) )    

	--经验
	elseif type == Macros.Game.RolePropType.EXP then
		local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")
	    self.__bar_exp:SetProgress(math.min(1,role_info.exp/role_info.max_exp)* 100)
	    self.__bar_exp_text:SetString(math.floor( (role_info.exp / role_info.max_exp) * 100 ) .. "%")  

	--充值总额度(影响GM等级)
	elseif type == Macros.Game.RolePropType.TOTAL_RECHARGE_AMOUNT then
		if GlobalCustomizedLogicAdapterMgr:Call("IsHideMaincityVIP") then			
			if not self.__img_gm_level then
				local img_vip = self:GetWidget("Image_VIP")
		    	img_vip:SetVisible(false)
		    	local panel_vip_touch = self:GetWidget("Panel_VIP_Touch")
		    	panel_vip_touch:SetVisible(false)			
			    self.__img_gm_level = GlobalCustomizedLogicAdapterMgr:Call("InitMaincityGMLevel", img_vip)
			else
				GlobalCustomizedLogicAdapterMgr:Call("UpdateMaincityGMLevel", self.__img_gm_level)
			end
		end
	end
end

--更新上阵英雄信息
function Modules.MaincityView:__UpdateLineupHerosInfo()
	if not self:IsOpen() then
		return
	end

	if self.__lineup_hero_list then
		local lineup_hero_layer, hero_info, hero_index
		for _, lineup_hero_layer in ipairs(self.__lineup_hero_list) do
			lineup_hero_layer:UpdateAwaken()
			lineup_hero_layer:UpdateEvolution()
			lineup_hero_layer:UpdateUpgradeOrBreak()
			lineup_hero_layer:UpdateEmptyLineupPos()
		end
	end
end

--更新主界面效果
function Modules.MaincityView:__UpdateMaincityEffect()
	local city = CallModuleFunc(ModuleType.MAIN_CITY,"GetMaincityEffectData")
	if city and city.id ~= self.__show_cityid then
		self.__show_cityid = city.id

		if self.__bg then
			--背景
			self.__bg:SetBG(city.city_bg)

			--特效
			self.__bg:SetEffects(city.effects)

			--骨骼
			self.__bg:SetAction(city.action)
		end
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--GM指令
--------------------------------------------------------------------------------------------------------------------------------
--GM指令
function Modules.MaincityView:__InitGM()
	if not StartConfig.IsTest then
		return
	end

	local client_gm_command_map = 
	{	
		--新手指引
		["lead"] = function(lead_id, section_index)
			CallModuleFunc(ModuleType.LEAD, "Start", tonumber(lead_id), tonumber(section_index))
		end,

		--剧情
		["story"] = function(story_id, section_index)
			CallModuleFunc(ModuleType.STORY, "Start", tonumber(story_id), tonumber(section_index or 1))
		end,

		--英雄属性
		["hero"] = function(lineup_index)
			local hero_info = CallModuleFunc(ModuleType.BATTLE, "GetBattleHeroInfo", tonumber(lineup_index), nil, true)
			local prop_key
			for prop_type = 1, table.maxn(Macros.Game.BattlePropKey) do
				prop_key = Macros.Game.BattlePropKey[prop_type]
				if prop_key then
					print(StartConfig.PrintTag, Macros.Game.BattlePropName[prop_type], hero_info[prop_key])
				end
			end
		end,

		--英雄战力
		["power"] = function(lineup_index)
			lineup_index = tonumber(lineup_index)
			if lineup_index == -1 then
				print(StartConfig.PrintTag, CPPGameLib.GetString("dungeon_power", CallModuleFunc(ModuleType.BATTLE, "GetBattleHeroPower")))
			else
				print(StartConfig.PrintTag, CPPGameLib.GetString("dungeon_power", CallModuleFunc(ModuleType.BATTLE, "GetSingleBattleHeroPower", lineup_index)))
			end
		end,
	}

	local panel_gm = self:GetWidget("Panel_GM")	
	local txt_place_holder_list = {
		[1] = "ID",
		[2] = "参数",
		[3] = "参数",
		[4] = "参数",
	}

	edit_gm_list = {}

	local x = 0
	for i = 1, 4 do
		local layouBg = Util:Layout()
		layouBg:SetAnchorPoint(0, 0.5)
		layouBg:SetContentSize(100, 35)
		layouBg:SetBackGroundColor(0, 0, 0)
		layouBg:SetBackGroundColorType(1)
		layouBg:SetTouchEnabled(false)
		panel_gm:AddChild(layouBg)
		PosTool.LeftCenter(layouBg, x)
		x = x + 103 

		edit_gm_list[i] = Util:TextField(txt_place_holder_list[i])
		edit_gm_list[i]:SetContentSize(100, 35)
		layouBg:AddChild(edit_gm_list[i])
		PosTool.Center(edit_gm_list[i])
	end

	local btn_gm = Util:Button(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_green"), function()
		local lab_1 = edit_gm_list[1]:GetText()
		local lab_2 = edit_gm_list[2]:GetText()
		local lab_3 = edit_gm_list[3]:GetText()
		local lab_4 = edit_gm_list[4]:GetText()

		if lab_1 == "" or lab_2 == "" then
			GlobalTipMsg.GetInstance():Show("请输入ID或者参数") 

		elseif lab_1 == "改名" then
			CallModuleFunc(ModuleType.LOGIN , "ChangeRoleNameReq" , lab_2 )
		else
			--客户端GM命令
			if client_gm_command_map[lab_1] then
				client_gm_command_map[lab_1](lab_2, lab_3)
				return
			end

			--服务端GM命令
			local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GM_REQ)
			data.id = lab_1
			data.param1 = lab_2
			data.param2 = lab_3 == "" and 0 or lab_3
			data.param3 = lab_4 == "" and 0 or lab_4
			GlobalNetAdapter:OnSend(data)    
		end
	end, "GM")	
	btn_gm:SetAnchorPoint(0, 0.5)
	btn_gm:SetContentSize(100, 35)
	panel_gm:AddChild(btn_gm)
	PosTool.LeftCenter(btn_gm, x)
end