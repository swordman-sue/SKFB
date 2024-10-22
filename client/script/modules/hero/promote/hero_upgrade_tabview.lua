
--[[
%% @module: 英雄升级界面
%% @author: swordman sue
%% @created: 2016-08-26
--]]

Modules = Modules or {}

Modules.HeroUpgradeTabView = Modules.HeroUpgradeTabView or BaseClass(BaseTabView)

function Modules.HeroUpgradeTabView:__init()
	self:__AddPreloadTemplate(GUI.TemplateRegistry.SALE_ITEM)
end

--切换英雄
function Modules.HeroUpgradeTabView:RefreshBySelectedHero()
	self:__TextDelete()
	self:__Reset()
end

--选项卡界面创建完成回调
function Modules.HeroUpgradeTabView:__LoadedCallback()
	self.__panel_main = self.__parent_view:GetWidget("Panel_Main")

	--英雄模型容器
	self.__panel_hero_commodel = WidgetUnity.GetWidgetByName(self.__panel_main, "Panel_HeroUpgradeSpecialEffect")
	--英雄等级
	local txt_level = self:GetWidget("Text_Level")		
	local txt_levelstring = self:GetWidget("Text_LevelString")
	--维语适配
	self.__lab_cur_lev = txt_level
	Util:WidgetLabel(txt_level, nil, Macros.TypefaceSize.normal, Macros.Color.content)
	Util:WidgetLabel(txt_levelstring,CPPGameLib.GetString("hero_upgrade_level"), Macros.TypefaceSize.normal, Macros.Color.keypoint)
	--等级加
	self.__lab_add_lev = Util:WidgetLabel(self:GetWidget("Text_LevelAdd"), nil, Macros.TypefaceSize.normal, Macros.Color.front_color)	
	self.__lab_cur_lev_pos = NodeUnity.ConvertPosBetweenTwoContainer(self.__lab_cur_lev:GetParent(), self.__root_widget, COCOPoint(self.__lab_cur_lev:GetPositionX(),self.__lab_cur_lev:GetPositionY()))	
	--飘字等级
	self.wind_words_levelup = Util:WidgetLabel(self:GetWidget("Text_levelUp"))
	Util:WidgetLabel(self.wind_words_levelup,"", 26, Macros.Color.golden,0,0,Macros.Color.common_stroke)
	self.__wind_words_levelup_org_pos = {pos_x = self.wind_words_levelup:GetPositionX(),pos_y = self.wind_words_levelup:GetPositionY()}
	--非升级飘
	self.__rich_text = Util:RichText("", Macros.TypefaceSize.popup, 0, 0, Macros.Color.lilyWhite_hex, Macros.Color.common_stroke_hex)
	self.__rich_text:SetAnchorPoint(0.5,0.5)
	self.__rich_text:SetPosition(-15,283)
	self.__root_widget:AddChild(self.__rich_text)
	self.__rich_text:SetVisible(false)
	--英雄属性
	self.__prop_map = {}
	self.__prop_add_map = {}
	self.__windwords_pos_list = {}
	self.__windword_org_pos_list = {}
	local prop_type_list = {Macros.Game.BattleProp.ATTACK, Macros.Game.BattleProp.HP, Macros.Game.BattleProp.PHYSICAL_DEFENCE, Macros.Game.BattleProp.MAGIC_DEFENCE}
	for i, prop_type in ipairs(prop_type_list) do
		--标题
		local txt_basicinfotitle = self:GetWidget("Text_BasicInfoTitle"..prop_type)
		local txt_basicinfo = self:GetWidget("Text_BasicInfo"..prop_type)
		Util:WidgetLabel(txt_basicinfotitle, Macros.Game.BattlePropName[prop_type], Macros.TypefaceSize.normal, Macros.Color.keypoint)

		--属性
		self.__prop_map[prop_type] = Util:WidgetLabel(txt_basicinfo, nil, Macros.TypefaceSize.normal,Macros.Color.content)
		self.__prop_add_map[prop_type] = Util:WidgetLabel(self:GetWidget("Text_BasicInfoAdd"..prop_type), nil, Macros.TypefaceSize.normal, Macros.Color.front_color)		

		--飘字属性
		self["wind_words_levelup_"..prop_type] = Util:WidgetLabel(self:GetWidget("Text_"..i))
		Util:WidgetLabel(self["wind_words_levelup_"..prop_type],"", Macros.TypefaceSize.popup, Macros.Color.green,0,0,Macros.Color.green_stroke)
		self.__windword_org_pos_list[prop_type] = {pos_x = self["wind_words_levelup_"..prop_type]:GetPositionX(),pos_y = self["wind_words_levelup_"..prop_type]:GetPositionY()}
		--飘字属性位置
		self.__windwords_pos_list[prop_type] = NodeUnity.ConvertPosBetweenTwoContainer(txt_basicinfo:GetParent(), self.__root_widget, COCOPoint(txt_basicinfo:GetPositionX(),txt_basicinfo:GetPositionY()))
	end

	--材料列表
	self.__material_info_list = {}
	--获取经验条中心坐标
	self.__exp_progressbar_center_list = {}
	for i = 1, Modules.HeroModule.MaxUpgradeMaterialNum do
		local material_info = {}
		material_info.panel = self:GetWidget("Image_Material"..i)

		local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_num_add_big")
		material_info.btn_add = Util:Button(imgdata, function()
			self:__OpenMaterialsList()
		end)
		material_info.panel:AddChild(material_info.btn_add)
		PosTool.Center(material_info.btn_add)

		material_info.btn_close = self:GetWidget("Button_Material"..i)
		material_info.btn_close:SetZOrder(2)
		WidgetUnity.SetWidgetListener(material_info.btn_close, nil, nil, function()
			--移除材料
			self:__DelMaterial(material_info.hero_uid)
		end)

		table.insert(self.__material_info_list, material_info)
		local new_pos = NodeUnity.ConvertPosBetweenTwoContainer(self.__root_widget, material_info.panel, COCOPoint(220,250))
		table.insert(self.__exp_progressbar_center_list, new_pos)
	end

	--金币消耗
	self.__lab_cost = Util:WidgetLabel(self:GetWidget("Text_Cost"), nil, Macros.TypefaceSize.normal, Macros.Color.btn)

    --获得经验
	self.__lab_exp = Util:WidgetLabel(self:GetWidget("Text_Exp"), nil, Macros.TypefaceSize.normal, Macros.Color.btn)

	--经验条
	local panel = self:GetWidget("Panel_ExpBar")
	local imgdata_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_preview")
    self.__bar_exp = GUI.ProgressBar9Layer.New(imgdata_bar, nil, 412, 20)
    self.__bar_exp:SetAnchorPoint(0, 0.5)
    panel:AddChild(self.__bar_exp:GetNode())
    PosTool.LeftCenter(self.__bar_exp,2,0)    

	--自动添加
	self.__button_auto = self:GetWidget("Button_Auto")
	Util:WidgetButton(self.__button_auto,"",
		function()
			local hero_info,config_hero = self:__GetHeroData()			
			if hero_info.level >= ConfigAdapter.Common.GetHeroCouldOneKeyLevelUpRoleLevel() then
				--一键升级
				self:__OneKeyLevelUp()
			else
				--自动添加
				self:__AutoAddMaterials()
			end			
	end,Macros.TypefaceSize.largish,Macros.Color.btn, Macros.Color.button_yellow)

	--升级
	self.__button_upgrade = self:GetWidget("Button_Upgrade")
	Util:WidgetButton(self.__button_upgrade, CPPGameLib.GetString("hero_promote_tab1"), function()
		self:__HandleUpgrade()
	end,Macros.TypefaceSize.largish,Macros.Color.btn, Macros.Color.button_red )

	--新手指引特殊操作检测
	self:__CheckSectionOperateSP()
end

--选项卡界面打开回调
function Modules.HeroUpgradeTabView:__OpenCallback()
	self:__InitMaterialList()

	self:__Reset()

	self.__upgrade_event = self:BindGlobalEvent(Macros.Event.HeroModule.HERO_UPGRADE_SUCC, function(_,uid)
		if self:IsOpen() and self.__parent_view:GetCurHeroUID() == uid then
			--升级成功，刷新界面
			if self.__is_upgrade then
				local effect 
				self.__upgrade_effect_id,effect = self:__CreateEffect({id = 100013,loop = false}, self.__panel_hero_commodel, 2)
				PosTool.CenterBottom(effect)
				local effect_id,effect = self:__CreateEffect({res_name = "UI_shengjipiaozi_language_chs",loop = false},self.__parent_view:GetWidget("Panel_upgradeEffect"), 2)
				effect:SetAnchorPoint(0.5,0)
				PosTool.Center(effect,0,20)
				self:__Reset(true)
				self:__BounceTextSpecialEffect()
			else
				local lab = GUI.RichText.GetColorWord(CPPGameLib.GetString("hero_upgrade_add_exp_tips",self.__cost_gold), Macros.Color.lilyWhite_hex, Macros.Color.common_stroke_hex,2)
				self.__rich_text:SetText(lab)
				self.__rich_text:SetPosition(-15,283)
				self.__rich_text:SetVisible(false)
				local windwords_list = {}
				local info = {}
				info.node = self.__rich_text
				info.anim_type = Macros.Game.WIND_WORDS_TYPE.UPSLOPE		
				info.tgt_pos = {x = -15,y = 330} 
				table.insert(windwords_list,info)
				local windwords_effect = WindWordsEffects.New()
				windwords_effect:Wind(windwords_list,function ()
					if windwords_effect then
						windwords_effect:DeleteMe()
						windwords_effect = nil
					end
				end)
				self:__Reset()
			end	
			self.__button_upgrade:SetEnabled(true)
			self.__button_auto:SetEnabled(true)		
		end
	end)

	--监听指引下一个步骤事件
	self:BindGlobalEvent(Macros.Event.LeadModule.CHECK_NEXT_SECTION, function(_, pre_section_info)
		self:__CheckSectionOperateSP()
	end)		
end

--选项卡界面关闭回调
function Modules.HeroUpgradeTabView:__CloseCallback()
	--删除英雄材料列表
	if self.__material_scv then
		self.__material_scv:DeleteMe()
		self.__material_scv = nil
	end

	--删除英雄材料条目
	for index, material_info in ipairs(self.__material_info_list) do
		material_info.hero_uid = nil
		if material_info.hero then
			material_info.hero:DeleteMe()
			material_info.hero = nil
		end
	end

    if self.__txt_material_need_exp then
        self.__txt_material_need_exp:RemoveSelf(true)
        self.__txt_material_need_exp = nil
    end
    if self.__txt_material_add_exp then
        self.__txt_material_add_exp:RemoveSelf(true)
        self.__txt_material_add_exp = nil
    end

	self:__TextDelete()
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.HeroUpgradeTabView:__Dispose()
end

--获取英雄信息
function Modules.HeroUpgradeTabView:__GetHeroData()
	local hero_uid = self.__parent_view:GetCurHeroUID()
	local hero_info = self:CallModuleFunc("GetHeroInfo", hero_uid)
	local config_hero = CPPGameLib.GetConfig("Hero", hero_info.info_id)
	return hero_info, config_hero
end

--播放文字特效
function Modules.HeroUpgradeTabView:__BounceTextSpecialEffect()	
	local hero_info, config_hero = self:__GetHeroData()	  
	local windwords_list = {}
	for prop_type,add_value in ipairs(self.__add_list) do
		local info = {}
		self["wind_words_levelup_"..prop_type]:SetPosition(self.__windword_org_pos_list[prop_type].pos_x,self.__windword_org_pos_list[prop_type].pos_y)
		self["wind_words_levelup_"..prop_type]:SetText(Macros.Game.BattlePropName[prop_type].." "..CPPGameLib.GetString("common_add4", add_value))
		info.node = self["wind_words_levelup_"..prop_type]
		info.anim_type = Macros.Game.WIND_WORDS_TYPE.MOVING_TO_LOCATION		
		info.tgt_pos = {x = self.__windwords_pos_list[prop_type].x,y = self.__windwords_pos_list[prop_type].y} 
		table.insert(windwords_list,info)
	end
	self.wind_words_levelup:SetText(CPPGameLib.GetString("hero_upgrade_to_some_level", hero_info.level))
	self.wind_words_levelup:SetPosition(self.__wind_words_levelup_org_pos.pos_x,self.__wind_words_levelup_org_pos.pos_y)
	local info = {}
	info.node = self.wind_words_levelup
	info.anim_type = Macros.Game.WIND_WORDS_TYPE.MOVING_TO_LOCATION		
	info.tgt_pos = {x = self.__lab_cur_lev_pos.x,y = self.__lab_cur_lev_pos.y} 
	table.insert(windwords_list,info)
	local windwords_effect = WindWordsEffects.New()
	windwords_effect:Wind(windwords_list,function ()
		self:__TextScaleToSpecialEffect()
		if windwords_effect then
			windwords_effect:DeleteMe()
			windwords_effect = nil
		end
	end)	
end

--文字滚动效果
function Modules.HeroUpgradeTabView:__TextScaleToSpecialEffect()
	self:__TextDelete()
	local hero_info, config_hero = self:__GetHeroData()
	local basic_prop_map = ConfigAdapter.Hero.GetBasicPropMap(hero_info.info_id, hero_info.quality_level, hero_info.level, hero_info.quality_stage)
	for prop_type, lab_prop in pairs(self.__prop_map) do
		self["rollText_"..prop_type] = TypefaceEffects.New()
		self["rollText_"..prop_type]:Roll(lab_prop,basic_prop_map[prop_type])
	end
	self.rollText_level = TypefaceEffects.New()
	self.rollText_level:Roll(self.__lab_cur_lev,hero_info.level)
end

--删除
function Modules.HeroUpgradeTabView:__TextDelete()
	for prop_type, lab_prop in pairs(self.__prop_map) do
		if self["rollText_"..prop_type] then
			self["rollText_"..prop_type]:DeleteMe()
			self["rollText_"..prop_type] = nil
		end
	end
	if self.rollText_level then
		self.rollText_level:DeleteMe()
		self.rollText_level = nil
	end
end

--刷新
function Modules.HeroUpgradeTabView:__Reset(IsNeedChangeAfter)
	local hero_info, config_hero = self:__GetHeroData()

	self.__lab_add_lev:SetVisible(false)
	CPPActionManager.StopAllActions(self.__lab_add_lev)
	if not IsNeedChangeAfter then
		--英雄等级
		self.__lab_cur_lev:SetText(hero_info.level)
		self.__lab_cur_lev:SetScaleFactor(1)
		--英雄属性
		local basic_prop_map = ConfigAdapter.Hero.GetBasicPropMap(hero_info.info_id, hero_info.quality_level, hero_info.level, hero_info.quality_stage)
		for prop_type, lab_prop in pairs(self.__prop_map) do
			lab_prop:SetText(basic_prop_map[prop_type])
			lab_prop:SetScaleFactor(1)
		end	
	end
	for _, lab_prop_add in pairs(self.__prop_add_map) do
		lab_prop_add:SetVisible(false)
		CPPActionManager.StopAllActions(lab_prop_add)
	end
	--延迟缩小
	CPPActionManager.DelayTo(self.__lab_cur_lev,0.2,GlobalCallbackMgr:AddCallBackFunc(function()
		self.__lab_cur_lev:SetScaleFactor(1)
		for prop_type, lab_prop in pairs(self.__prop_map) do
			lab_prop:SetScaleFactor(1)
		end
	end))
			
	--英雄宝宝	
	for index, material_info in ipairs(self.__material_info_list) do
		material_info.btn_add:SetVisible(true)
		material_info.btn_close:SetVisible(false)

		material_info.hero_uid = nil
		if material_info.hero then
			CPPActionManager.StopAllActions(material_info.hero:GetNode())
			material_info.hero:DeleteMe()
			material_info.hero = nil
		end
	end

	self.__button_upgrade:SetEnabled(true)
	self.__button_auto:SetEnabled(true)		
	--金币消耗
	self.__cost_gold = 0
	self.__lab_cost:SetText("0")

	--经验条
	local upgrade_info = ConfigAdapter.Hero:GetUpgradeInfo(hero_info.quality_level, hero_info.level)
	self.__bar_exp:SetProgressMax(upgrade_info and upgrade_info.exp or 0)
	self.__bar_exp:SetProgress(hero_info.exp)
	self.__bar_exp:StopPreviewEffect()

	--获得经验
	self.__lab_exp:SetText(string.format("%d/%d", hero_info.exp, upgrade_info.exp))

	--按钮文字
	self.__button_auto:SetText(hero_info.level >= ConfigAdapter.Common.GetHeroCouldOneKeyLevelUpRoleLevel()
				and CPPGameLib.GetString("hero_upgrade_onekey_levelup_btn") or CPPGameLib.GetString("hero_upgrade_btn"))

	--升级所需经验
	self.__txt_material_need_exp:SetString(math.max(upgrade_info.exp - hero_info.exp,0))
end

--自动添加材料
function Modules.HeroUpgradeTabView:__AutoAddMaterials()
	local hero_info, config_hero = self:__GetHeroData()

	--检测等级上限
	if self:__CheckUpperLimit() then
		return
	end

	local list = self:CallModuleFunc("GetUpgradeMaterialList", self.__parent_view:GetCurHeroUID(), nil, true)
	if not list then
		--获取途径
		GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, ConfigAdapter.Common.GetPurpleExpHero())
		--材料不足
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_upgrade_tips2"))		
		return
	end

	--自动添加
	self:__AddMaterials(list)
end

--一键升级
function Modules.HeroUpgradeTabView:__OneKeyLevelUp()
	--检测等级上限
	if self:__CheckUpperLimit() then
		return
	end

	local hero_info, config_hero = self:__GetHeroData()

	local list = self:CallModuleFunc("GetUpgradeMaterialList", self.__parent_view:GetCurHeroUID(), nil, true)
	if not list then
		--获取途径
		GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, ConfigAdapter.Common.GetPurpleExpHero())
		--材料不足
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_upgrade_tips2"))		
		return
	end

	--升级所需经验
	local upgrade_info = ConfigAdapter.Hero:GetUpgradeInfo(hero_info.quality_level, hero_info.level)
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)

	--总经验
	local add_exp = 0
	local cost_hero_list = {}
	for _, cost_hero_info in ipairs(list) do
		table.insert(cost_hero_list,{uid = cost_hero_info.uid})
		add_exp = add_exp + Modules.HeroModule.GetHeroExp(cost_hero_info)	
		if add_exp >= (upgrade_info.exp - hero_info.exp) then
			break
		end
	end
	
	if add_exp >= (upgrade_info.exp - hero_info.exp) and role_level > hero_info.level then
		--足够升一级以上
		Modules.TemplHeroOnekeyLevelUpTipsOne.New(list, hero_info, self.__parent_view:GetCurHeroUID(), function (add_level)
			self.__is_upgrade = true
			local cur_prop_map = ConfigAdapter.Hero.GetBasicPropMap(hero_info.info_id, hero_info.quality_level, hero_info.level, hero_info.quality_stage)
			local tgt_prop_map = ConfigAdapter.Hero.GetBasicPropMap(hero_info.info_id, hero_info.quality_level, hero_info.level + add_level, hero_info.quality_stage)
			self.__add_list = {}
			local add_value
			for prop_type, lab_prop_add in pairs(self.__prop_add_map) do
				add_value = tgt_prop_map[prop_type] - cur_prop_map[prop_type]
				self.__add_list[prop_type] = add_value
			end
		end)
	else
		--不够升一级
		self.__cost_gold = add_exp
		Modules.TemplHeroOnekeyLevelUpTipsTwo.New(add_exp, function ()
			self:CallModuleFunc("HeroUpgradeReq", self.__parent_view:GetCurHeroUID(), cost_hero_list)
		end,function ()
			self.__cost_gold = 0
		end,role_level == hero_info.level)		
	end
end

--添加材料
function Modules.HeroUpgradeTabView:__AddMaterials(cost_hero_list, check_upper_limit)
	if not cost_hero_list then
		return
	end
	
	local add_exp = 0
	local limit_exp
	if check_upper_limit ~= false then
		limit_exp = self:__GetUpperLimitExp()
	end

	local cost_hero_info
	for i, material_info in ipairs(self.__material_info_list) do
		cost_hero_info = cost_hero_list[i]

		--经验上限处理
		if check_upper_limit ~= false then
			if (add_exp + Modules.HeroModule.GetHeroExp(cost_hero_info)) >= limit_exp and i ~= 1 then
				cost_hero_info = nil
			else
				add_exp = add_exp + Modules.HeroModule.GetHeroExp(cost_hero_info)
			end
		end

		material_info.hero_uid = cost_hero_info and cost_hero_info.uid or nil
		if not material_info.hero_uid then
			if material_info.hero then
				material_info.hero:DeleteMe()
				material_info.hero = nil
			end
		else
			if not material_info.hero then
				material_info.hero = LogicGUI.IconLayer.New(false, false)
				material_info.panel:AddChild(material_info.hero:GetNode())
				PosTool.Center(material_info.hero)
			end
			material_info.hero:SetData({id = cost_hero_info.info_id})
			material_info.hero:ShowHeroName(true)
		end

		material_info.btn_add:SetVisible(not material_info.hero_uid)
		material_info.btn_close:SetVisible(material_info.hero_uid ~= nil)
	end

	self:__DoPreview()
end

--移除材料
function Modules.HeroUpgradeTabView:__DelMaterial(del_hero_uid)
	if not del_hero_uid then
		return
	end

	for _, material_info in ipairs(self.__material_info_list) do
		if material_info.hero_uid == del_hero_uid then
			material_info.hero_uid = nil
			if material_info.hero then
				material_info.hero:DeleteMe()
				material_info.hero = nil
			end
			material_info.btn_add:SetVisible(true)
			material_info.btn_close:SetVisible(false)
			break			
		end
	end

	self:__DoPreview()
end

--预览
function Modules.HeroUpgradeTabView:__DoPreview()
	local hero_info, config_hero = self:__GetHeroData()

	--经验加成
	local add_exp = 0
	local cost_hero_info, cost_config_hero
	for _, material_info in ipairs(self.__material_info_list) do
		if material_info.hero_uid then
			cost_hero_info = self:CallModuleFunc("GetHeroInfo", material_info.hero_uid)
			add_exp = add_exp + Modules.HeroModule.GetHeroExp(cost_hero_info)
		end
	end

	--经验上限处理
	local limit_exp = self:__GetUpperLimitExp()

	self.__is_overflow = nil
	--经验条
	local imgdata_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_bar_bar_preview")
	if add_exp > 0 then
		self.__bar_exp:StartPreviewEffect(hero_info.exp + add_exp, imgdata_bar)
	else
		self.__bar_exp:StopPreviewEffect()
	end
	local upgrade_info = ConfigAdapter.Hero:GetUpgradeInfo(hero_info.quality_level, hero_info.level)	
	self.__lab_exp:SetText(string.format("%d/%d", hero_info.exp + add_exp, upgrade_info.exp))

	--金币消耗
	self.__cost_gold = add_exp
	self.__lab_cost:SetText(tostring(self.__cost_gold))

	--等级&&属性加成
	local tgt_level = ConfigAdapter.Hero.GetUpgradeTgtLevelByExp(
		hero_info.quality_level, hero_info.level, hero_info.exp, math.min(add_exp, limit_exp), config_hero.limit_level)

	local cur_prop_map = ConfigAdapter.Hero.GetBasicPropMap(hero_info.info_id, hero_info.quality_level, hero_info.level, hero_info.quality_stage)
	local tgt_prop_map = ConfigAdapter.Hero.GetBasicPropMap(hero_info.info_id, hero_info.quality_level, tgt_level, hero_info.quality_stage)

	if tgt_level > hero_info.level and cur_prop_map and tgt_prop_map and add_exp > 0 then
		self.__add_list = {}
		self.__is_upgrade = true		
		self.__lab_add_lev:SetVisible(true)
		self.__lab_add_lev:SetText(CPPGameLib.GetString("common_add4", tgt_level - hero_info.level))
		ActionManager.GetInstance():CycleBlinkEffect(self.__lab_add_lev, 0.5)
		local add_value
		for prop_type, lab_prop_add in pairs(self.__prop_add_map) do
			add_value = tgt_prop_map[prop_type] - cur_prop_map[prop_type]
			lab_prop_add:SetVisible(true)
			self.__add_list[prop_type] = add_value
			lab_prop_add:SetText(CPPGameLib.GetString("common_add4", add_value))
			ActionManager.GetInstance():CycleBlinkEffect(lab_prop_add, 0.5)
		end		
	else
		self.__is_upgrade = false
		self.__lab_add_lev:SetVisible(false)
		CPPActionManager.StopAllActions(self.__lab_add_lev)
		for _, lab_prop_add in pairs(self.__prop_add_map) do
			lab_prop_add:SetVisible(false)
			CPPActionManager.StopAllActions(lab_prop_add)
		end
	end
end

--升级处理
function Modules.HeroUpgradeTabView:__HandleUpgrade()
	--检测等级上限
	if self:__CheckUpperLimit() then
		return
	end

	local cost_hero_list
	for index, material_info in ipairs(self.__material_info_list) do					
		if material_info.hero_uid then
			cost_hero_list = cost_hero_list or {}
			table.insert(cost_hero_list, {uid = material_info.hero_uid})
		end
	end
	if not cost_hero_list then
		--材料不足
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_upgrade_tips3"))
		return
	end

	if self.__cost_gold > CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.GOLD) then
		--金币不足
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("common_gold_not_enough"))
		CallModuleFunc(ModuleType.SUNDRY, "ToIdGetPath", Macros.Game.RolePropType.GOLD )
		return
	end

	--经验是否溢出
	if self.__is_overflow then
		local tips = LogicGUI.TemplAffirmTips.New( { 
													type = LogicGUI.TemplAffirmTipsType.YES_NO,
													content	= CPPGameLib.GetString("hero_upgrade_overflow_tip"),
													cancel_text = CPPGameLib.GetString("common_cancel") ,
													confirm_text = CPPGameLib.GetString("common_confirm"),
													confirm_func = function ()
														self:__UpgradeSpecialEffect(cost_hero_list)
													end,
													cancel_func = function ()
													end,
												} )
	else
		self:__UpgradeSpecialEffect(cost_hero_list)
	end	
end

--升级前的特效
function Modules.HeroUpgradeTabView:__UpgradeSpecialEffect(cost_hero_list)
	--特效
	self.__button_upgrade:SetEnabled(false)
	self.__button_auto:SetEnabled(false)
	local call_once
	for index, material_info in ipairs(self.__material_info_list) do			
		if material_info.hero then
			material_info.btn_close:SetVisible(false)
			local effect_id,effect = self:__CreateEffect({id = 100015,loop = false,time_scale = 2,end_callback = function ()
				if material_info.hero then
					ActionManager.GetInstance():RunScaleToAndMoveToEffect(		
																			material_info.hero:GetNode(),0.3,
																			self.__exp_progressbar_center_list[index].x,
																			self.__exp_progressbar_center_list[index].y,
																			0,GlobalCallbackMgr:AddCallBackFunc(
																				function ( )
																					if not call_once then
																						call_once = true
																						CPPGameLib.PrintTable("ldx",cost_hero_list,"cost_hero_list: ")
																						self:CallModuleFunc("HeroUpgradeReq", self.__parent_view:GetCurHeroUID(), cost_hero_list)
																					end				
																				end)
																		)
				end					
					end},material_info.panel,2)
			PosTool.Center(effect)						
		end
	end
end

--获取可吸收的经验上限
function Modules.HeroUpgradeTabView:__GetUpperLimitExp()
	local hero_info, config_hero = self:__GetHeroData()
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)		
	local tgt_level = math.min(role_level, config_hero.limit_level)
	return ConfigAdapter.Hero.GetUpgradeExpByTgtLevel(hero_info.quality_level, hero_info.level, tgt_level, hero_info.exp)
end

--检测是否超出上限
function Modules.HeroUpgradeTabView:__CheckUpperLimit()
	local hero_info, config_hero = self:__GetHeroData()
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue", Macros.Game.RolePropType.LEVEL)

	--检测是否超过等级上限
	if hero_info.level >= config_hero.limit_level then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_upgrade_limit_tips"))		
		return
	end

	--检测是否超过角色等级
	if hero_info.level < role_level then
		return false
	elseif hero_info.level > role_level then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_upgrade_tips1"))
		return true
	end
	local upgrade_info = ConfigAdapter.Hero:GetUpgradeInfo(hero_info.quality_level, hero_info.level)	
	if hero_info.exp >= upgrade_info.exp - 1 then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_upgrade_tips1"))
		return true
	end		
	return false
end

--新手指引特殊操作检测(英雄升级添加材料)
function Modules.HeroUpgradeTabView:__CheckSectionOperateSP()
	local material_info = self.__material_info_list[1]
	if material_info then
		CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.HERO_UPGRADE_ADD_MATERIAL, material_info.btn_add)		
	end
end
