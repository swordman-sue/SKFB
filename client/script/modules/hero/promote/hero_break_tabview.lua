
--[[
%% @module: 英雄进阶界面
%% @author: swordman sue
%% @created: 2016-08-26
--]]

Modules = Modules or {}

Modules.HeroBreakTabView = Modules.HeroBreakTabView or BaseClass(BaseTabView)

function Modules.HeroBreakTabView:__init()
	self:__AddPreloadTemplate(GUI.TemplateRegistry.SALE_ITEM)
end

--选项卡界面创建完成回调
function Modules.HeroBreakTabView:__LoadedCallback()
	self.__panel_main = self.__parent_view:GetWidget("Panel_Main")
	
	--材料列表
	self.__listview_material = self:GetWidget("ListView_HeroMaterial")
	self.__src_panel_break_tool = self:GetWidget("Panel_Break_Tool")
	self.__src_panel_break_tool:SetVisible(false)
	self.__material_info_list = {}
	for i = 1, 4 do
		local material_info = {}
		material_info.panel_icon = self.__src_panel_break_tool:Clone()
		material_info.panel_icon:SetVisible(true)
		material_info.panel = Util:Layout(200, 80)
		material_info.panel:AddChild(material_info.panel_icon)
		PosTool.LeftCenter(material_info.panel_icon)
		CPPNodeUnity.Retain(material_info.panel)

		material_info.text_name = WidgetUnity.GetWidgetByName(material_info.panel_icon, "Text_Break_ToolName")
		material_info.text_name:SetColor(unpack(Macros.Color.keypoint))
		local panel_tool_num = WidgetUnity.GetWidgetByName(material_info.panel_icon, "Panel_Break_ToolNum")
		material_info.text_num = Util:RichText("0/0", nil, 0, 0)
		material_info.text_num:SetAnchorPoint(0, 0.5)
		panel_tool_num:AddChild(material_info.text_num)
		PosTool.LeftCenter(material_info.text_num)		
		table.insert(self.__material_info_list, material_info)
	end

	--当前和下阶等级
	local panel_break_level = self:GetWidget("Panel_CurBreakLevel")
	self.__rt_cur_break_level = Util:RichText()
	self.__rt_cur_break_level:SetAnchorPoint(0.5, 0.5)
	panel_break_level:AddChild(self.__rt_cur_break_level)
	PosTool.Center(self.__rt_cur_break_level)

	panel_break_level = self:GetWidget("Panel_NextBreakLevel")
	self.__rt_next_break_level = Util:RichText()
	self.__rt_next_break_level:SetAnchorPoint(0.5, 0.5)
	panel_break_level:AddChild(self.__rt_next_break_level)
	PosTool.Center(self.__rt_next_break_level)

	--当前和下阶属性加成
	self.__cur_prop_map = {}
	self.__next_prop_map = {}
	local panel_prop, rt_prop
	local prop_type_list = {Macros.Game.BattleProp.ATTACK, Macros.Game.BattleProp.HP, Macros.Game.BattleProp.PHYSICAL_DEFENCE, Macros.Game.BattleProp.MAGIC_DEFENCE}
	local function create_rt_prop(prop_type, is_cur)
		panel_prop = self:GetWidget((is_cur and "Panel_CurProp" or "Panel_NextProp")..prop_type)
		rt_prop = Util:RichText("", Macros.TypefaceSize.slightly)
		rt_prop:SetAnchorPoint(0, 0.5)		
		panel_prop:AddChild(rt_prop)
		PosTool.LeftCenter(rt_prop)

		local prop_map = is_cur and self.__cur_prop_map or self.__next_prop_map
		prop_map[prop_type] = rt_prop
	end
	for _, prop_type in ipairs(prop_type_list) do		
		create_rt_prop(prop_type, true)
		create_rt_prop(prop_type)
	end

	--解锁天赋
	self.__unlock_talent_info = Util:WidgetLabel(self:GetWidget("Text_Talent"), nil, Macros.TypefaceSize.slightly, Macros.Color.red)

	--所需等级
	local txt_breakneedlevtitle = self:GetWidget("Text_Break_NeedLevTitle")
	Util:WidgetLabel(txt_breakneedlevtitle, CPPGameLib.GetString("hero_break_need_level_title"), Macros.TypefaceSize.largish, Macros.Color.keypoint)
	self.__text_need_level = Util:WidgetLabel(self:GetWidget("Text_Break_NeedLev"),"",Macros.TypefaceSize.normal, Macros.Color.content)

	--金币消耗
	self.__lab_cost = Util:WidgetLabel(self:GetWidget("Text_Break_Cost"), nil, nil, Macros.Color.btn)

	--已满级标记
	self.__image_full_level = self:GetWidget("Image_FullLevel")

	--进阶
	self.__button_break = self:GetWidget("Button_Break_OK")
	widget = Util:WidgetButton(self.__button_break, CPPGameLib.GetString("hero_promote_tab2"), function()
		self:__HandleBreak()
	end,Macros.TypefaceSize.largish,Macros.Color.btn,Macros.Color.button_yellow)
end

--选项卡界面打开回调
function Modules.HeroBreakTabView:__OpenCallback()
	self:__InitMaterialList()
	
	self:__Reset()

	self.__break_event = self:BindGlobalEvent(Macros.Event.HeroModule.HERO_BREAK_SUCC, function(_,uid)
		if self:IsOpen() and self.__parent_view:GetCurHeroUID() == uid then
			--进阶成功

			--刷新界面
			self:__Reset()
			Modules.TemplHeroBreakSucc.New(uid,function ()
				self.__button_break:SetEnabled(true)
			end)				
		end
	end)
end

--选项卡界面关闭回调
function Modules.HeroBreakTabView:__CloseCallback()
	if self.__material_scv then
		self.__material_scv:DeleteMe()
		self.__material_scv = nil
	end
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.HeroBreakTabView:__Dispose()
	for _, material_info in ipairs(self.__material_info_list) do
		CPPNodeUnity.Release(material_info.panel)
	end
end

--获取英雄信息
function Modules.HeroBreakTabView:__GetHeroData()
	local hero_uid = self.__parent_view:GetCurHeroUID()	
	local hero_info = self:CallModuleFunc("GetHeroInfo", hero_uid)
	local config_hero = CPPGameLib.GetConfig("Hero", hero_info.info_id)
	return hero_info, config_hero
end

--刷新
function Modules.HeroBreakTabView:__Reset()
	local hero_info, config_hero = self:__GetHeroData()
	local need_level, cost_gold, cost_obj_list, cost_same_name_hero_num, cost_same_quality_hero_num, cost_same_profession_hero_num = ConfigAdapter.Hero.GetBreakLevelupInfo(hero_info)
	local next_break_level = need_level < 0 and hero_info.break_level or hero_info.break_level + 1

	--当前和下阶等级
	local lab = GUI.RichText.GetColorWord(config_hero.name.."+"..hero_info.break_level, Macros.Quality_HEX[hero_info.quality_level], Macros.Quality_Hex_Stroke[hero_info.quality_level], 2)
	self.__rt_cur_break_level:SetText(lab)	
	lab = GUI.RichText.GetColorWord(config_hero.name.."+"..(need_level < 0 and hero_info.break_level or next_break_level), Macros.Quality_HEX[hero_info.quality_level], Macros.Quality_Hex_Stroke[hero_info.quality_level], 2)
	self.__rt_next_break_level:SetText(lab)	

	--需要等级
	local str_need_level = need_level < 0 and CPPGameLib.GetString("hero_break_reached_max") or string.format("%d/%d", hero_info.level, need_level)
	self.__text_need_level:SetText(str_need_level)
	self.__text_need_level:SetColor(unpack(hero_info.level >= need_level and Macros.Color.front_color or Macros.Color.red))

	--是否满级
	self.__image_full_level:SetVisible(need_level >= 0)			

	--当前和下阶属性加成
	local str_prop_type, str_prop_value, lab_prop
	local basic_prop_map = ConfigAdapter.Hero.GetBasicPropMap(hero_info.info_id, hero_info.quality_level, hero_info.level, hero_info.quality_stage)
	local plus_prop_map = ConfigAdapter.Hero.GetSelfBreakPlusBasicPropMap(hero_info.info_id, hero_info.break_level)
	local next_plus_prop_map = ConfigAdapter.Hero.GetSelfBreakPlusBasicPropMap(hero_info.info_id, next_break_level)
	local function handle_prop(prop_type, prop_value, is_cur)		
		lab_prop = is_cur and self.__cur_prop_map[prop_type] or self.__next_prop_map[prop_type]
		str_prop_type = CPPGameLib.GetString("common_str_with_colon", Macros.Game.BattlePropName[prop_type])

		local color_0 , color_1 , color_2 = Macros.Color.keypoint_hex , Macros.Color.content_hex , Macros.Color.front_color_hex

		str_prop_type = GUI.RichText.GetColorWord(str_prop_type, color_0 )

		str_prop_value = GUI.RichText.GetColorWord(prop_value, is_cur and color_1 or color_2 )
		lab_prop:SetText(string.format("%s%s", str_prop_type, str_prop_value))
	end
	for prop_type, lab_prop in pairs(self.__cur_prop_map) do
		handle_prop(prop_type, basic_prop_map[prop_type] + (plus_prop_map[prop_type] or 0), true)
	end

	for prop_type, lab_prop in pairs(self.__next_prop_map) do
		handle_prop(prop_type, basic_prop_map[prop_type] + (next_plus_prop_map[prop_type] or 0))
	end

	--解锁天赋
	local unlock_talent_name, unlock_talent_value
	local break_info = ConfigAdapter.Hero.GetBreakInfo(next_break_level, hero_info.info_id)
	if break_info then
		unlock_talent_name = break_info.break_name or ""
		unlock_talent_value = break_info.break_desc or ""
	end
	self.__unlock_talent_info:SetText(CPPGameLib.GetString("hero_break_talent_title2")..CPPGameLib.GetString("common_other1", unlock_talent_name, unlock_talent_value))

	--英雄进阶及潜能等级没有升过,可作为消耗品
	local function filter_func(hero_info)
		if hero_info.uid ~= self.__parent_view:GetCurHeroUID() and Modules.HeroModule.BreakCostHeroFilter(hero_info) then
			return true			
		end
	end

	--重置并显示材料
	self.__listview_material:RemoveAllItems()
	self.__selected_cost_same_quality_hero_map = {}
	self.__selected_cost_same_profession_hero_map = {}
	local cost_obj_type, cost_obj_config, cost_obj_own_num
	if cost_same_name_hero_num > 0 then
		table.insert(cost_obj_list, {id = hero_info.info_id, num = cost_same_name_hero_num})
	end
	if cost_same_quality_hero_num > 0 then
		table.insert(cost_obj_list, {id = hero_info.info_id, num = cost_same_quality_hero_num, name = CPPGameLib.GetString("hero_break_same_quality_hero"), is_same_quality = true, quality = hero_info.quality_level, icon_path = Resource.PathTool.GetHeroHeadPath(30000)})
	end
	if cost_same_profession_hero_num > 0 then
		table.insert(cost_obj_list, {id = hero_info.info_id, num = cost_same_profession_hero_num, name = CPPGameLib.GetString("hero_break_same_profession_hero"),  is_same_profession = true, icon_path = Resource.PathTool.GetHeroHeadPath(30000)})
	end
	for i, material_info in ipairs(self.__material_info_list) do
		local cost_obj_info = cost_obj_list[i]
		if not cost_obj_info then
			material_info.activated = nil
		else
			material_info.activated = true

			self.__listview_material:PushBackCustomItem(material_info.panel)

			--材料图标
			cost_obj_type, cost_obj_config = ObjIDTool.GetInfo(cost_obj_info.id)
			if not material_info.icon_layer then
				material_info.icon_layer = LogicGUI.IconLayer.New(false)
				material_info.panel_icon:AddChild(material_info.icon_layer:GetNode())
				PosTool.Center(material_info.icon_layer)
			end

			material_info.icon_layer:SetData({id = cost_obj_info.id, icon_path = cost_obj_info.icon_path or nil})
			
			material_info.icon_layer:SetTouchCallback(function (data)
				if cost_obj_info.is_same_quality then				
					self:__OpenMaterialsList(hero_info, 0, cost_obj_info.num, i)
					return
				end

				if cost_obj_info.is_same_profession then
					self:__OpenMaterialsList(hero_info, 1, cost_obj_info.num, i)
					return
				end

				GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, data.id)
			end)

			--材料名称
			material_info.text_name:SetText(cost_obj_info.name or cost_obj_config.name)
			material_info.text_name:SetTextColor(Macros.QualityColor(cost_obj_info.quality or cost_obj_config.quality or cost_obj_config.init_quality))
			material_info.text_name:SetStrokeColor(Macros.QualityColorStroke(cost_obj_info.quality or cost_obj_config.quality or cost_obj_config.init_quality))

			--材料数量
			if cost_obj_info.is_same_quality then
				cost_obj_own_num = CPPGameLib.GetTableLength(self.__selected_cost_same_quality_hero_map)
			elseif cost_obj_info.is_same_profession then
				cost_obj_own_num = CPPGameLib.GetTableLength(self.__selected_cost_same_profession_hero_map)
			else
				cost_obj_own_num = ObjIDTool.GetOwnNum(cost_obj_info.id, cost_obj_type == Macros.Game.ObjType.HERO and filter_func or nil)
			end
			if cost_obj_own_num >= cost_obj_info.num then
				--材料充足
				local num_str = string.format("%d/%d", cost_obj_own_num, cost_obj_info.num)
				num_str = GUI.RichText.GetColorWord(num_str, Macros.Color.front_color_hex)
				material_info.text_num:SetText(num_str)
			else
				--材料不足
				local own_num_str = GUI.RichText.GetColorWord(tostring(cost_obj_own_num), Macros.Color.red_hex)
				local num_str = GUI.RichText.GetColorWord(string.format("%s/%d", own_num_str, cost_obj_info.num), Macros.Color.keypoint_hex )
				material_info.text_num:SetText(num_str)
			end	
		end
	end

	--金币消耗
	self.__lab_cost:SetText(cost_gold)	
end

--进阶处理
function Modules.HeroBreakTabView:__HandleBreak()
	if not self:CallModuleFunc("JudgeIsCouldBreak", self.__parent_view:GetCurHeroUID(), true) then
		return
	end

	local hero_info, config_hero = self:__GetHeroData()
	local _, _, _, cost_same_name_hero_num, cost_same_quality_hero_num, cost_same_profession_hero_num = ConfigAdapter.Hero.GetBreakLevelupInfo(hero_info)
	local cost_same_name_hero_list = self:CallModuleFunc("GetBreakCostHeroList", hero_info.uid, cost_same_name_hero_num, 0, 0)	
	local selected_cost_same_quality_hero_list, selected_cost_same_profession_hero_list

	--检测已选同品质英雄、同品质且职业英雄是否达到条件
	for hero_uid, _ in pairs(self.__selected_cost_same_quality_hero_map) do
		selected_cost_same_quality_hero_list = selected_cost_same_quality_hero_list or {}
		table.insert(selected_cost_same_quality_hero_list, {uid = hero_uid})
	end
	for hero_uid, _ in pairs(self.__selected_cost_same_profession_hero_map) do
		selected_cost_same_profession_hero_list = selected_cost_same_profession_hero_list or {}
		table.insert(selected_cost_same_profession_hero_list, {uid = hero_uid})
	end
	if cost_same_quality_hero_num > 0 and (not selected_cost_same_quality_hero_list or #selected_cost_same_quality_hero_list < cost_same_quality_hero_num) then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_break_tips3"))			
		return
	end
	if cost_same_profession_hero_num > 0 and (not selected_cost_same_profession_hero_list or #selected_cost_same_profession_hero_list < cost_same_profession_hero_num) then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_break_tips4"))			
		return
	end

	self.__button_break:SetEnabled(false)
	self:CallModuleFunc("HeroBreakReq", hero_info.uid, cost_same_name_hero_list, selected_cost_same_quality_hero_list, selected_cost_same_profession_hero_list)
end

--切换英雄
function Modules.HeroBreakTabView:RefreshBySelectedHero()
	self.__button_break:SetEnabled(true)
	self:__Reset()
end
