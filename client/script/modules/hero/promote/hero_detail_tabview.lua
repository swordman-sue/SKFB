
--[[
%% @module: 英雄详情界面
%% @author: swordman sue
%% @created: 2016-12-9
--]]

Modules = Modules or {}

Modules.HeroDetailTabView = Modules.HeroDetailTabView or BaseClass(BaseTabView)

function Modules.HeroDetailTabView:__init()
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_COMMON)
end

--选项卡界面创建完成回调
function Modules.HeroDetailTabView:__LoadedCallback()
	self.__scv = self:GetWidget("ScrollView_Detail")

	--宿命条目模板
	self.__templ_reinforcement = self:GetWidget("Templ_Reinforcement")
	self.__templ_reinforcement:SetVisible(false)

	--初始化
	local panel_name_list = {"Panel_BasicInfo", "Panel_SkillInfo", "Panel_PotentialInfo", "Panel_TalentInfo","Panel_AwakenTalentInfo","Panel_ReinforcementInfo"}
	for _, panel_name in ipairs(panel_name_list) do
		self[panel_name] = self:GetWidget(panel_name)
	end

	local Button_rule = Util:WidgetButton( self:GetWidget("Button_rule") , nil , function ( ... )
		print("xy" , "英雄提升，小图标" )
        local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
        help:SetLoadedCallback( function ( ... )
           help:SetCell("Rule_Hero_Info")
        end )
	end )
	Button_rule:SetZOrder(100)
	
	self:__InitBasicInfo()
	self:__InitSkillInfo()
	self:__InitPotentialInfo()
	self:__InitBreakTalentInfo()
	self:__InitAwakenTalentInfo()
	self:__InitReinforcementInfo()
end

--选项卡界面打开回调
function Modules.HeroDetailTabView:__OpenCallback()
	self:__Reset()
end

--选项卡界面关闭回调
function Modules.HeroDetailTabView:__CloseCallback()
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.HeroDetailTabView:__Dispose()
end

--获取英雄信息
function Modules.HeroDetailTabView:__GetHeroData()
	local hero_uid = self.__parent_view:GetCurHeroUID()
	local hero_info = self:CallModuleFunc("GetDetailHeroInfo", hero_uid)
	local config_hero = CPPGameLib.GetConfig("Hero", hero_info.info_id)
	return hero_info, config_hero
end

--刷新
function Modules.HeroDetailTabView:__Reset()
	self:__ShowBasicInfo()
	self:__ShowSkillInfo()
	self:__ShowPotentialInfo()
	self:__ShowBreakTalentInfo()
	self:__ShowAwakenTalentInfo()
	self:__ShowReinforcementInfo()

	--重置滚动区域
	local height = 0
	local panel
	local panel_name_list = {"Panel_BasicInfo", "Panel_SkillInfo", "Panel_PotentialInfo", "Panel_TalentInfo","Panel_AwakenTalentInfo", "Panel_ReinforcementInfo"}
	for _, panel_name in ipairs(panel_name_list) do
		panel = self[panel_name]
		PosTool.CenterTop(panel, 0, -height)
		height = height + panel:GetContentHeight()
	end
	self.__scv:SetInnerContainerSize(0, height)
	self.__scv:RefreshLayoutComponent()
	self.__scv:ScrollToTop()
end

--基础信息
function Modules.HeroDetailTabView:__InitBasicInfo()
	--标题
	Util:WidgetLabel(self:GetWidget("Text_BasicTitle"), CPPGameLib.GetString("hero_basic_info_title"), Macros.TypefaceSize.popup, Macros.Color.btn, nil, nil, nil, Macros.Color.shadow_color)	

	--等级
	local txt_basiclevtitle = self:GetWidget("Text_BasicLevTitle")
	Util:WidgetLabel(txt_basiclevtitle, CPPGameLib.GetString("common_lev1"), Macros.TypefaceSize.largish, Macros.Color.keypoint)
	self.__text_basiclev = Util:WidgetLabel(self:GetWidget("Text_BasicLev"), "", Macros.TypefaceSize.largish,Macros.Color.content)

	--进阶等级
	Util:WidgetLabel(self:GetWidget("Text_BasicBreakLevTitle"), CPPGameLib.GetString("hero_break_lev_title"), Macros.TypefaceSize.largish, Macros.Color.keypoint)
	self.__text_basicbreaklev = Util:WidgetLabel(self:GetWidget("Text_BasicBreakLev"), nil, Macros.TypefaceSize.largish,Macros.Color.content)
end

function Modules.HeroDetailTabView:__ShowBasicInfo()
	local hero_info = self:__GetHeroData()
	
	--等级
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	self.__text_basiclev:SetText(string.format("%d/%d", hero_info.level, role_level))

	--进阶等级
	self.__text_basicbreaklev:SetText(CPPGameLib.GetString("common_add4", hero_info.break_level))

	--基础属性
	local hero_uid = self.__parent_view:GetCurHeroUID()
	local prop_value
	local prop_type_list = {
								Macros.Game.BattleProp.ATTACK, 
								Macros.Game.BattleProp.PHYSICAL_DEFENCE, 
								Macros.Game.BattleProp.MAGIC_DEFENCE, 
								Macros.Game.BattleProp.HP,
								Macros.Game.BattleProp.BEAT_BACK_CORRECT,
								Macros.Game.BattleProp.BEAT_BACK_DEFENCE_CORRECT
							}

	local adapter_prop_type_list = {[Macros.Game.BattleProp.ATTACK] = true,[Macros.Game.BattleProp.PHYSICAL_DEFENCE] = true,[Macros.Game.BattleProp.BEAT_BACK_CORRECT] = true}
	


	for _, prop_type in ipairs(prop_type_list) do
		prop_value = hero_info[Macros.Game.BattlePropKey[prop_type]]
		Util:WidgetLabel(self:GetWidget("Text_BasicTitle"..prop_type), CPPGameLib.GetString("common_str_with_colon", Macros.Game.BattlePropName[prop_type]), Macros.TypefaceSize.largish, Macros.Color.keypoint)
		local nature_name,nature_num = Util:Nature(prop_type, prop_value or 0, true, true)
		Util:WidgetLabel(self:GetWidget("Text_Basic"..prop_type), nature_num, Macros.TypefaceSize.largish,Macros.Color.content)
	end



end

--技能信息
function Modules.HeroDetailTabView:__InitSkillInfo()	
	--标题
	local txt_skilltitle = self:GetWidget("Text_SkillTitle")
	Util:WidgetLabel(txt_skilltitle, CPPGameLib.GetString("hero_skiil"), Macros.TypefaceSize.popup, Macros.Color.btn, nil, nil, nil, Macros.Color.shadow_color)

	self.__panel_skill_info = self["Panel_SkillInfo"]
	self.__panel_skill = self:GetWidget("Panel_Skill")

	self.__skill_icon_map = 
	{
		[Macros.Game.SkillType.NORMAL] = "icon_attack",
		[Macros.Game.SkillType.SPECIAL] = "icon_skill",
		[Macros.Game.SkillType.ANGER] = "icon_superskill",
		[Macros.Game.SkillType.JOINT] = "icon_together",
	}

end
function Modules.HeroDetailTabView:__ShowSkillInfo()
	local hero_info, config_hero = self:__GetHeroData()

	LogicGUI.TemplHeroInfo.__ShowSkillInfo(self, hero_info, "Panel_SkillInfo", nil, 4,28)
	local i = 1
	-- local skill_info, icon_x, icon_y, config_skill, config_skill_lev, skill_desc
	-- local rt_test = Util:RichText(CPPGameLib.GetString("common_test"))
	-- local rt_line_height = rt_test:GetContentHeight()
	-- for _, skill_id in ipairs(config_hero.skill_list) do
	-- 	config_skill = CPPGameLib.GetConfig("Skill", skill_id)

	-- 	--普攻、特技、怒气/合击
	-- 	if config_skill and 
	-- 		config_skill.type ~= Macros.Game.SkillType.NEAR and
	-- 		config_skill.type ~= Macros.Game.SkillType.JOINT_ATTACHED then

	-- 		self.__skill_info_list = self.__skill_info_list or {}
	-- 		skill_info = self.__skill_info_list[i]
	-- 		if not skill_info then
	-- 			skill_info = {}
	-- 			self.__skill_info_list[i] = skill_info

	-- 			skill_info.icon = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, self.__skill_icon_map[config_skill.type], true))
	-- 			skill_info.icon:SetAnchorPoint(0, 0.5)
	-- 			self.__panel_skill:AddChild(skill_info.icon)

	-- 			skill_info.rt_desc = Util:RichText(nil, Macros.TypefaceSize.slightly, 380)
	-- 			skill_info.rt_desc:SetAnchorPoint(0, 1)
	-- 			skill_info.rt_desc:SetFontSpacing(4)
	-- 			self.__panel_skill:AddChild(skill_info.rt_desc)
	-- 		end

	-- 		skill_info.icon:SetVisible(true)			
	-- 		PosTool.LeftTop(skill_info.icon, 10, -22 - height)

	-- 		config_skill_lev = ConfigAdapter.Skill.GetSkillLevInfo(skill_id, ConfigAdapter.Hero.GetPotentialSkillLevel(hero_info.potential_level))
	-- 		skill_desc = CPPGameLib.GetString("common_brackets", config_skill.name)
	-- 		skill_desc = skill_desc .. (config_skill_lev and config_skill_lev.des or "")		
	-- 		skill_info.rt_desc:SetVisible(true)
	-- 		skill_info.rt_desc:SetText(GUI.RichText.GetColorWord(skill_desc), Macros.Color.content_hex)

	-- 		icon_x, icon_y = skill_info.icon:GetPosition()
	-- 		skill_info.rt_desc:SetPosition(icon_x + 50, icon_y + rt_line_height / 2 + 2)

	-- 		i = i + 1
	-- 		height = height + skill_info.icon:GetContentHeight() / 2 + math.max(skill_info.icon:GetContentHeight() / 2, skill_info.rt_desc:GetContentHeight() - rt_line_height / 2)
	-- 	end		
	-- end	

	-- for j = i, #self.__skill_info_list do
	-- 	skill_info = self.__skill_info_list[j]
	-- 	skill_info.icon:SetVisible(false)
	-- 	skill_info.rt_desc:SetVisible(false)
	-- end


end

--显示技能Tips
function Modules.HeroDetailTabView:__ShowSkillTips(hero_id, skill_id, skill_lev, skill_icon)
	LogicGUI.TemplHeroInfo.__ShowSkillTips(self, hero_id, skill_id, skill_lev, skill_icon,true)
end

--隐藏技能Tips
function Modules.HeroDetailTabView:__HideSkillTips()
	LogicGUI.TemplHeroInfo.__HideSkillTips(self)
end

--潜能信息
function Modules.HeroDetailTabView:__InitPotentialInfo() 
	--标题
	local txt_potentialtitle = self:GetWidget("Text_PotentialTitle")
	Util:WidgetLabel(txt_potentialtitle, CPPGameLib.GetString("hero_potential_title"), Macros.TypefaceSize.popup, Macros.Color.btn, nil, nil, nil, Macros.Color.shadow_color)

	--潜能等级
	Util:WidgetLabel(self:GetWidget("Text_PotentialLevTitle"), CPPGameLib.GetString("hero_potential_level_title"), Macros.TypefaceSize.largish, Macros.Color.keypoint)
	self.__text_potentiallev = Util:WidgetLabel(self:GetWidget("Text_PotentialLev"), nil, Macros.TypefaceSize.largish,Macros.Color.content)
	--tips
	Util:WidgetLabel(self:GetWidget("Text_PotentialTips"), CPPGameLib.GetString("hero_potential_tips4"), Macros.TypefaceSize.normal,Macros.Color.content)

end


function Modules.HeroDetailTabView:__ShowPotentialInfo()
	local hero_info = self:__GetHeroData()

	--潜能等级
	self.__text_potentiallev:SetText(hero_info.potential_level)
end

--进阶天赋
function Modules.HeroDetailTabView:__InitBreakTalentInfo()
	--标题
	local txt_talenttitle = self:GetWidget("Text_TalentTitle")
	Util:WidgetLabel(txt_talenttitle, CPPGameLib.GetString("hero_break_talent_title"), Macros.TypefaceSize.popup, Macros.Color.btn, nil, nil, nil, Macros.Color.shadow_color)

	self.__panel_talentinfo = self["Panel_TalentInfo"]
	self.__panel_talent = self:GetWidget("Panel_Talent")	
end
function Modules.HeroDetailTabView:__ShowBreakTalentInfo()
	local hero_info = self:__GetHeroData()

	local config = CPPGameLib.GetConfig("HeroBreak", hero_info.info_id)
	if not config then
		self.__panel_talent:RemoveAllChild(true)
		self.__lab_break_talent_list = nil
		return
	end

	self.__lab_break_talent_list = self.__lab_break_talent_list or {}	

	local y = -6
	local row_space = 4
	local level_index
	local show_max_index = math.min(hero_info.break_level + 5,#config.level_list)
	for i, info in ipairs(config.level_list) do
		if i <= show_max_index then
			level_index = i
			if not self.__lab_break_talent_list[i] then
				self.__lab_break_talent_list[i] = {}
				self.__lab_break_talent_list[i].lab1 = Util:Label("", Macros.TypefaceSize.normal, Macros.Color.content)
				self.__lab_break_talent_list[i].lab1:SetAnchorPoint(0, 1)
				self.__panel_talent:AddChild(self.__lab_break_talent_list[i].lab1)
				PosTool.LeftTop(self.__lab_break_talent_list[i].lab1, 14, y)

				self.__lab_break_talent_list[i].lab2 = Util:Label("", Macros.TypefaceSize.normal, Macros.Color.content, 310)
				self.__lab_break_talent_list[i].lab2:SetAnchorPoint(0, 1)
				self.__panel_talent:AddChild(self.__lab_break_talent_list[i].lab2)			
			end

			local talent_str = CPPGameLib.GetString("hero_info_talent1", info.break_name)
			local talent_str2 = CPPGameLib.GetString("hero_info_talent2", info.break_desc, info.level)
			self.__lab_break_talent_list[i].lab1:SetString(talent_str)		
			self.__lab_break_talent_list[i].lab2:SetTextWidthAndHeight(440 - self.__lab_break_talent_list[i].lab1:GetContentWidth(),0)
			PosTool.LeftTop(self.__lab_break_talent_list[i].lab2,self.__lab_break_talent_list[i].lab1:GetContentWidth() + 15, y)
			self.__lab_break_talent_list[i].lab2:SetString(talent_str2)		
			if hero_info.break_level >= info.level then
				self.__lab_break_talent_list[i].lab1:SetColor(unpack(Macros.Color.orange))
				self.__lab_break_talent_list[i].lab2:SetColor(unpack(Macros.Color.orange))
			else
				self.__lab_break_talent_list[i].lab1:SetColor(unpack(Macros.Color.content))
				self.__lab_break_talent_list[i].lab2:SetColor(unpack(Macros.Color.content))
			end

			y = y - self.__lab_break_talent_list[i].lab2:GetContentHeight() - row_space	
		else
			break
		end	
	end

	for i = #self.__lab_break_talent_list, level_index + 1, -1 do
		self.__lab_break_talent_list[i].lab1:RemoveSelf(true)
		self.__lab_break_talent_list[i].lab2:RemoveSelf(true)
		table.remove(self.__lab_break_talent_list, i)
	end

	self.__panel_talentinfo:SetContentHeight(38 - y)
end

--觉醒天赋
function Modules.HeroDetailTabView:__InitAwakenTalentInfo()
	--标题
	local txt_awaken_talenttitle = self:GetWidget("Text_AwakenTalentTitle")
	Util:WidgetLabel(txt_awaken_talenttitle, CPPGameLib.GetString("hero_awaken_talent_title"), Macros.TypefaceSize.popup, Macros.Color.btn, nil, nil, nil, Macros.Color.shadow_color)

	self.__panel_awaken_talentinfo = self["Panel_AwakenTalentInfo"]
	self.__panel_awaken_talent = self:GetWidget("Panel_AwakenTalent")	
end

function Modules.HeroDetailTabView:__ShowAwakenTalentInfo()
	local hero_info = self:__GetHeroData()
	local hero_config = CPPGameLib.GetConfig("Hero", hero_info.info_id)
	self.__panel_awaken_talentinfo:SetVisible(hero_config.init_quality >= Macros.Game.QualityType.ORANGE)
	if hero_config.init_quality < Macros.Game.QualityType.ORANGE then
		self.__panel_awaken_talent:RemoveAllChild(true)
		self.__lab_awaken_talent_list = nil
		self.__panel_awaken_talentinfo:SetContentHeight(0)
		return
	end
	local special_id = hero_config.init_quality * 10 + hero_config.profession
	local awaken_config = CPPGameLib.GetConfig("HeroAwakening", special_id)
	self.__lab_awaken_talent_list = self.__lab_awaken_talent_list or {}	
	local y = -6
	local row_space = 4
	local level_index
	for awaken_star=1,Macros.Game.HERO_AWAKE_STAR_MAX do
		if awaken_config[awaken_star][0].lightup_talent_name then
			level_index = awaken_star
			if not self.__lab_awaken_talent_list[awaken_star] then
				self.__lab_awaken_talent_list[awaken_star] = {}
				self.__lab_awaken_talent_list[awaken_star].lab1 = Util:Label("", Macros.TypefaceSize.normal, Macros.Color.content,150)
				self.__lab_awaken_talent_list[awaken_star].lab1:SetAnchorPoint(0, 1)
				self.__panel_awaken_talent:AddChild(self.__lab_awaken_talent_list[awaken_star].lab1)
				PosTool.LeftTop(self.__lab_awaken_talent_list[awaken_star].lab1, 14, y)

				self.__lab_awaken_talent_list[awaken_star].lab2 = Util:Label("", Macros.TypefaceSize.normal, Macros.Color.content,300)
				self.__lab_awaken_talent_list[awaken_star].lab2:SetAnchorPoint(0, 1)
				self.__panel_awaken_talent:AddChild(self.__lab_awaken_talent_list[awaken_star].lab2)
				PosTool.LeftTop(self.__lab_awaken_talent_list[awaken_star].lab2, 130, y)
			end

			local talent_str = CPPGameLib.GetString("hero_info_talent1", awaken_config[awaken_star][0].lightup_talent_name)
			local talent_str2 = CPPGameLib.GetString("hero_info_awakentalent2", awaken_config[awaken_star][0].lightup_talent_des, awaken_star)
			self.__lab_awaken_talent_list[awaken_star].lab1:SetString(talent_str)
			self.__lab_awaken_talent_list[awaken_star].lab2:SetString(talent_str2)		
			if hero_info.awaken_star >= awaken_star then
				self.__lab_awaken_talent_list[awaken_star].lab1:SetColor(unpack(Macros.Color.red))
				self.__lab_awaken_talent_list[awaken_star].lab2:SetColor(unpack(Macros.Color.red))
			else
				self.__lab_awaken_talent_list[awaken_star].lab1:SetColor(unpack(Macros.Color.content))
				self.__lab_awaken_talent_list[awaken_star].lab2:SetColor(unpack(Macros.Color.content))
			end

			y = y - self.__lab_awaken_talent_list[awaken_star].lab2:GetContentHeight() - row_space
		end		
	end

	for i = #self.__lab_awaken_talent_list, level_index + 1, -1 do
		self.__lab_awaken_talent_list[i].lab1:RemoveSelf(true)
		self.__lab_awaken_talent_list[i].lab2:RemoveSelf(true)
		table.remove(self.__lab_awaken_talent_list, i)
	end

	self.__panel_awaken_talentinfo:SetContentHeight(38 - y)
end

--宿命信息
function Modules.HeroDetailTabView:__InitReinforcementInfo()
	--标题
	local txt_reinforcementtitle = self:GetWidget("Text_ReinforcementTitle")
	Util:WidgetLabel(txt_reinforcementtitle, CPPGameLib.GetString("hero_reinforcement"), Macros.TypefaceSize.popup, Macros.Color.btn, nil, nil, nil, Macros.Color.shadow_color)

	self.__panel_reinforcementinfo = self["Panel_ReinforcementInfo"]
	self.__panel_reinforcementlist = self:GetWidget("Panel_ReinforcementList")
end
function Modules.HeroDetailTabView:__ShowReinforcementInfo()
	local hero_info = self:__GetHeroData()
	--上阵英雄
	local OnFightHeroList = CallModuleFunc(ModuleType.HERO,"GetAllLineupHeroList")	
	local lineup_index = CallModuleFunc(ModuleType.HERO,"GetLineupInfoByUID",hero_info.uid)
	--当前阵位英雄的装备
	local equip_table = CallModuleFunc(ModuleType.HERO,"GetLineupEquipInfo",lineup_index)
	--对应阵位卡牌的穿戴情况
	local divination_type_list = CallModuleFunc(ModuleType.HERO,"GetLineupDivinationInfo",lineup_index)
	self.__panel_reinforcementlist:RemoveAllChild(true)

	local y = 2
	local row_space = 5
	local reinforcement_list = self:CallModuleFunc("GetHeroReinforcementInfoList", hero_info.uid)
	for _, reinforcement_info in ipairs(reinforcement_list) do
		local template = LogicGUI.TemplHeroDetailReinforcementInfo.New(self.__templ_reinforcement, self.__panel_reinforcementlist, hero_info.info_id)
		template:SetData(reinforcement_info,OnFightHeroList,equip_table,divination_type_list)
		PosTool.CenterTop(template, 0, y)
		y = y - template:GetHeight() - row_space
	end
	self.__panel_reinforcementinfo:SetContentHeight(45 - y)
end

--切换英雄
function Modules.HeroDetailTabView:RefreshBySelectedHero()
	self:__Reset()
end

--------------------------------------------------------------------------------------------
--英雄详情的宿命条目
--------------------------------------------------------------------------------------------
LogicGUI = LogicGUI or {}

LogicGUI.TemplHeroDetailReinforcementInfo = LogicGUI.TemplHeroDetailReinforcementInfo or BaseClass(GUI.Template)

function LogicGUI.TemplHeroDetailReinforcementInfo:__init(src_widget, parent_node, hero_id)
	self.__layout_mode = GUI.Template.LayoutMode.FROM_WIDGET

	self:InitTemplate(src_widget, parent_node)
	self:SetAnchorPoint(0.5, 1)

	self.__hero_id = hero_id

	self.__text_reinforcement = Util:WidgetLabel(self:GetWidget("Text_Reinforcement"), nil, Macros.TypefaceSize.largish, Macros.Color.keypoint)
	self.__text_desc = Util:WidgetLabel(self:GetWidget("Text_Des"), nil, Macros.TypefaceSize.normal, nil, 400)
	self.__listview_head = self:GetWidget("ListView_Head")
	self.__listview_head:SetItemsMargin(0)
	self.__listview_head:SetTouchSwallow(false)
end

function LogicGUI.TemplHeroDetailReinforcementInfo:__delete()
end

--[[
	hero_info
--]]
function LogicGUI.TemplHeroDetailReinforcementInfo:SetData(data,OnFightHeroList,equip_table,divination_type_list)
	GUI.Template.SetData(self, data)

	self.__text_reinforcement:SetText(self.__data.reinforcementname)
	self.__text_desc:SetText(self.__data.reinforcementdes)
	if self.__data.IsMakeWork then 
		self.__text_reinforcement:SetColor(unpack(Macros.Color.front_color))
		self.__text_desc:SetColor(unpack(Macros.Color.front_color))
	else
		self.__text_reinforcement:SetColor(unpack(Macros.Color.keypoint))
		self.__text_desc:SetColor(unpack(Macros.Color.content))
	end

	self.__listview_head:RemoveAllItems()	
	for i, obj_id in ipairs(self.__data.relation_obj_list) do
		if obj_id ~= "text" then
			local is_Gray = true
			if OnFightHeroList[obj_id] then
				is_Gray = false
			else
				if equip_table[obj_id] then
					is_Gray = false
				elseif divination_type_list[obj_id] then
					is_Gray = false
				else 
					is_Gray = true
				end
			end
			if self.__data.is_card and self.__hero_id ~= obj_id then
				local card_id = self.__data.recommend_card_list[i-1]
				if card_id then
					obj_id = card_id
				else
					return
				end
			end
			local icon = LogicGUI.IconLayer.New()
			icon:SetData({id = obj_id})		
			icon:SetGray(obj_id ~= self.__hero_id and is_Gray)
			icon:SetScaleFactor(0.8)
			self.__listview_head:PushBackCustomItem(icon:GetNode())
		else
			local layout = Util:Layout(15,64)
			layout:SetClippingEnable(false)
			layout:SetBackGroundColor(0, 0, 0)
			local lable = Util:Label(CPPGameLib.GetString("manor_dispatch3"), Macros.TypefaceSize.normal, Macros.Color.blinkcontent,0,0,Macros.Color.common_stroke)
	   		layout:AddChild(lable)
	   		PosTool.Center(lable)
	   		self.__listview_head:PushBackCustomItem(layout)
		end
	end		
end

