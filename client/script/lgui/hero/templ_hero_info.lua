
--[[
%% @module：英雄信息模板
%% @author: swordman sue
%% @created: 2016-08-23
--]]

LogicGUI = LogicGUI or {}

LogicGUI.TemplHeroInfo = LogicGUI.TemplHeroInfo or BaseClass(GUI.Template)

function LogicGUI.TemplHeroInfo:__init(parent_node)
	self:InitTemplate(GUI.TemplateRegistry.HERO_INFO, parent_node, true)

	self:__InitUI()
end

function LogicGUI.TemplHeroInfo:__delete()
	--删除Scrollview
	if self.__reinforcement_scrollview then
		self.__reinforcement_scrollview:DeleteMe()
		self.__reinforcement_scrollview = nil
	end
end

--[[
	hero_info
--]]
function LogicGUI.TemplHeroInfo:SetData(data)
	GUI.Template.SetData(self, data)
	if not self.__data then
		return
	end
	if not self.__data.prop_constructed then
		local config_hero = CPPGameLib.GetConfig("Hero", self.__data.info_id)
		if not config_hero then
			return
		end
		local config_hero_prop  = ConfigAdapter.Hero.GetPropConfig(config_hero.profession, config_hero.init_quality, 1)
		self.__data.hp = config_hero_prop.blood_max
		self.__data.level = 1
		self.__data.break_level = 0
		self.__data.attack = config_hero_prop.attack
		self.__data.physical_defence = config_hero_prop.physical_defence
		self.__data.magic_defence = config_hero_prop.magic_defence
		self.__data.beat_back_correct = config_hero_prop.beat_back_correct or 0
		self.__data.beat_back_defence_correct = config_hero_prop.beat_back_defence_correct or 0
	end

	self:__ShowBasicInfo()
	self:__ShowSkillInfo(self.__data, "Panel_BasicProp2", "lab_name2")
	self:__ShowReinforcement()

	self.__list_view:ForceDoLayout()
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
function LogicGUI.TemplHeroInfo:__InitUI()
	self.__list_view = self:GetWidget("ListView_Info")
	self.__list_view:SetBounceEnabled(false)
	WidgetUnity.SetScrollListener(self.__list_view, function()
		self:__HideSkillTips()
	end)
end

--基础信息
function LogicGUI.TemplHeroInfo:__ShowBasicInfo()
	local panel_prop = self:GetWidget("Panel_BasicProp1")
	panel_prop:RemoveAllChild(true)

	--基础信息
	local offset_y = -20
	local offset_step = -33

	--标题
	Util:WidgetLabel(self:GetWidget("lab_name1"), CPPGameLib.GetString("hero_basic_info_title"), Macros.TypefaceSize.largish, Macros.Color.btn, nil, nil, nil, Macros.Color.shadow_color)

	-- --等级
	-- local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
 --   	local lab_prop = Util:LabCenter({
 --   		{zi = CPPGameLib.GetString("common_lev1"), dx = Macros.TypefaceSize.normal, ys = Macros.Color.keypoint},
 --   		{zi = string.format("%d/%d", self.__data.level, role_level), dx = Macros.TypefaceSize.normal, jg = 6, ys = Macros.Color.content},
	-- })		
	-- lab_prop:SetAnchorPoint(0, 0.5)
	-- panel_prop:AddChild(lab_prop)
	-- PosTool.LeftTop(lab_prop, 5, offset_y)
	-- offset_y = offset_y + offset_step

	--攻击
	local lab_info = {
   		{zi = Macros.Game.BattlePropName[Macros.Game.BattleProp.ATTACK] .. CPPGameLib.GetString("common_colon"), dx = Macros.TypefaceSize.normal, ys = Macros.Color.keypoint},
   		{zi = tostring(self.__data.attack), dx = Macros.TypefaceSize.normal, jg = 8, ys = Macros.Color.content},
	}
   	lab_prop,widget_list = Util:LabCenter(lab_info)		
	lab_prop:SetAnchorPoint(0, 0.5)
	panel_prop:AddChild(lab_prop)
	self["lab_prop_"..Macros.Game.BattleProp.ATTACK] = widget_list[2]
	PosTool.LeftTop(lab_prop, 5, offset_y)
	--生命
	lab_info = {
   		{zi = Macros.Game.BattlePropName[Macros.Game.BattleProp.HP] .. CPPGameLib.GetString("common_colon"), dx = Macros.TypefaceSize.normal, ys = Macros.Color.keypoint},
   		{zi = tostring(self.__data.hp), dx = Macros.TypefaceSize.normal, jg = 8, ys = Macros.Color.content},
	}
   	lab_prop,widget_list = Util:LabCenter(lab_info)		
	lab_prop:SetAnchorPoint(0, 0.5)
	panel_prop:AddChild(lab_prop)
	self["lab_prop_"..Macros.Game.BattleProp.HP] = widget_list[2]
	PosTool.LeftTop(lab_prop, 147, offset_y)
	offset_y = offset_y + offset_step

	--物防
	lab_info = {
   		{zi = Macros.Game.BattlePropName[Macros.Game.BattleProp.PHYSICAL_DEFENCE] .. CPPGameLib.GetString("common_colon") , dx = Macros.TypefaceSize.normal, ys = Macros.Color.keypoint},
   		{zi = tostring(self.__data.physical_defence), dx = Macros.TypefaceSize.normal, jg = 8, ys = Macros.Color.content},
	}
   	lab_prop,widget_list = Util:LabCenter(lab_info)		
	lab_prop:SetAnchorPoint(0, 0.5)
	panel_prop:AddChild(lab_prop)
	self["lab_prop_"..Macros.Game.BattleProp.PHYSICAL_DEFENCE] = widget_list[2]
	PosTool.LeftTop(lab_prop, 5, offset_y)
	--法防
	lab_info = {
   		{zi = Macros.Game.BattlePropName[Macros.Game.BattleProp.MAGIC_DEFENCE] .. CPPGameLib.GetString("common_colon"), dx = Macros.TypefaceSize.normal, ys = Macros.Color.keypoint},
   		{zi = tostring(self.__data.magic_defence), dx = Macros.TypefaceSize.normal, jg = 8, ys = Macros.Color.content},
	}
   	lab_prop,widget_list = Util:LabCenter(lab_info)		
	lab_prop:SetAnchorPoint(0, 0.5)
	panel_prop:AddChild(lab_prop)
	self["lab_prop_"..Macros.Game.BattleProp.MAGIC_DEFENCE] = widget_list[2]
	PosTool.LeftTop(lab_prop, 147, offset_y)
	offset_y = offset_y + offset_step

	--击退
	local nature_name,nature_num = Util:Nature(Macros.Game.BattleProp.BEAT_BACK_CORRECT, self.__data.beat_back_correct, true, true)
   	lab_info = {
   		{zi = Macros.Game.BattlePropName[Macros.Game.BattleProp.BEAT_BACK_CORRECT] .. CPPGameLib.GetString("common_colon"), dx = Macros.TypefaceSize.normal, ys = Macros.Color.keypoint},
   		{zi = tostring(nature_num), dx = Macros.TypefaceSize.normal, jg = 8, ys = Macros.Color.content},
	}
   	lab_prop,widget_list = Util:LabCenter(lab_info)		
	lab_prop:SetAnchorPoint(0, 0.5)
	panel_prop:AddChild(lab_prop)
	self["lab_prop_"..Macros.Game.BattleProp.BEAT_BACK_CORRECT] = widget_list[2]
	PosTool.LeftTop(lab_prop, 5, offset_y)
	--抗击退
	nature_name,nature_num = Util:Nature(Macros.Game.BattleProp.BEAT_BACK_DEFENCE_CORRECT, self.__data.beat_back_defence_correct, true, true)
   	lab_info = {
   		{zi = Macros.Game.BattlePropName[Macros.Game.BattleProp.BEAT_BACK_DEFENCE_CORRECT] .. CPPGameLib.GetString("common_colon"), dx = Macros.TypefaceSize.normal, ys = Macros.Color.keypoint},
   		{zi = tostring(nature_num), dx = Macros.TypefaceSize.normal, jg = 8, ys = Macros.Color.content},
	}
   	lab_prop,widget_list = Util:LabCenter(lab_info)		
	lab_prop:SetAnchorPoint(0, 0.5)
	panel_prop:AddChild(lab_prop)
	self["lab_prop_"..Macros.Game.BattleProp.BEAT_BACK_DEFENCE_CORRECT] = widget_list[2]
	PosTool.LeftTop(lab_prop, 147, offset_y)

	--设置监听跳到详情
	local panel = self:GetWidget("Panel_1")
	panel:SetTouchEnabled(true)
	WidgetUnity.SetWidgetListener(panel, nil, nil,function ()
		CallModuleFunc(ModuleType.HERO, "OpenView", BaseViewType.HERO_PROMOTE, nil, self.__data.uid)
	end)
end

--技能
function LogicGUI.TemplHeroInfo:__ShowSkillInfo(hero_info, panel_widget_name,title_widget_name,col_num,left_dis)
	-- CPPGameLib.PrintTable("skfb",hero_info,"__ShowSkillInfo__ShowSkillInfo __ShowSkillInfo ")
	hero_id = hero_info.info_id
	local config_hero = CPPGameLib.GetConfig("Hero", hero_id)
	if not config_hero then
		return
	end

	local col_num = col_num or 3
	local left_dis = left_dis or 0

	--阵容是否激活合击
	local is_lightup_joint_skill = false
	--背包是否拥有合击英雄
	local is_have_joint_hero = false
	if config_hero.joint_hero_show_list then
		--阵容是否激活
		for index=1,5 do
			local lineup_info = CallModuleFunc(ModuleType.HERO,"GetLineupInfo",index)
			if lineup_info.uid ~= 0 then
				local hero_info = CallModuleFunc(ModuleType.HERO,"GetHeroInfo",lineup_info.uid)
				if hero_info.info_id == config_hero.joint_hero_show_list[1] then
					is_lightup_joint_skill = true
					break
				end
			end
		end
		--英雄背包是否有这英雄
		local count_num = CallModuleFunc(ModuleType.HERO,"GetNumByInfoID",config_hero.joint_hero_show_list[1])
		is_have_joint_hero = count_num > 0		
	end
	--标题
	if title_widget_name then
		Util:WidgetLabel(self:GetWidget(title_widget_name), CPPGameLib.GetString("hero_skill_title"), Macros.TypefaceSize.largish, Macros.Color.btn, nil, nil, nil, Macros.Color.shadow_color)
	end
	--创建技能图标
	local function create_skill_icon(skill_id)
		local config_skill = CPPGameLib.GetConfig("Skill", skill_id)
		if not config_skill then
			return
		end
		local sp_border = Util:Layout(74, 74)
		sp_border:SetBackGroundImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.HERO_COMMON, "bg_skill_borde"))
		sp_border:SetTouchEnabled(true)
		sp_border:SetTouchSwallow(false)
		sp_border:SetAnchorPoint(0, 1)
		WidgetUnity.SetWidgetListener(sp_border, 
			function()
				local skill_lev = ConfigAdapter.Hero.GetPotentialSkillLevel(hero_info.potential_level)
				self:__ShowSkillTips(hero_id, skill_id, skill_lev, sp_border)
			end, nil,
			function()
				self:__HideSkillTips()
			end,
			function()
				self:__HideSkillTips()
			end)

		local sp_icon = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.HERO_COMMON, "skill_icon"..config_skill.type))		
		sp_border:AddChild(sp_icon, -1)
		PosTool.Center(sp_icon)

		local lab_name = Util:Label(CPPGameLib.GetString("hero_skill_type"..config_skill.type), Macros.TypefaceSize.normal,Macros.Color.blinkcontent)
		lab_name:SetAnchorPoint(0.5, 0)
		sp_border:AddChild(lab_name)
		PosTool.CenterBottom(lab_name, 0, -2)

		--合击技能是否变灰
		if config_skill.type == Macros.Game.SkillType.JOINT then
			if not is_lightup_joint_skill then
				sp_icon:SetGray(true)
				lab_name:SetColor(unpack(Macros.Color.Gray))
			end
		end

		return sp_border
	end

	local panel_prop = self:GetWidget(panel_widget_name)
	if not self.__layout_skill then		
		self.__layout_skill = Util:Layout()
		self.__layout_skill:SetAnchorPoint(0.5, col_num == 3 and 0.5 or 1)
		panel_prop:AddChild(self.__layout_skill)			
	end
	self.__layout_skill:RemoveAllChild(true)

	--品阶是否改变
	local is_quality_change = hero_info.quality_level > config_hero.init_quality
	local cur_skill_list = is_quality_change and config_hero.skill_ex_list or config_hero.skill_list
	if cur_skill_list then
		local row_space = 8
		local col_space = 24

		local skill_type
		local show_skill_list = {}
		for i, v in ipairs(cur_skill_list) do
			--普攻、特技、怒气/合击
			skill_type = ConfigAdapter.Skill.GetSkillType(v)
			if skill_type == Macros.Game.SkillType.NORMAL or 
				skill_type == Macros.Game.SkillType.SPECIAL or 
				skill_type == Macros.Game.SkillType.ANGER or 
				skill_type == Macros.Game.SkillType.JOINT then				
				table.insert(show_skill_list, v)
			end
		end
		local skill_height = #show_skill_list > col_num and 160 or 74			
		self.__layout_skill:SetContentSize(col_num == 3 and 272 or 450, skill_height)
		if col_num == 4 then
			panel_prop:SetContentHeight(skill_height + 45)
			PosTool.CenterTop(self.__layout_skill, 0,-37)
		else
			PosTool.Center(self.__layout_skill, 0,-2)
		end	
		for i, v in ipairs(show_skill_list) do
			--普攻、特技、怒气/合击
			local sp_skill_icon = create_skill_icon(v)
			if not sp_skill_icon then
				return 
			end
			self.__layout_skill:AddChild(sp_skill_icon)
			PosTool.LeftTop(sp_skill_icon, ((i - 1) % col_num) * (sp_skill_icon:GetContentWidth() + col_space) + left_dis, -math.floor((i - 1) / col_num) * (sp_skill_icon:GetContentHeight() + row_space))	
			--合击英雄获取途径
			if config_hero.joint_hero_show_list and i == #show_skill_list and (not is_have_joint_hero) and col_num == 3 then
				local hero_compose_info = CPPGameLib.GetConfig("HeroCompound",config_hero.joint_hero_show_list[1])
				if not hero_compose_info then
					return
				end
				local img_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.HERO_LINEUP, "approach_hero",true)
				local app_btn = Util:Button(img_data,function ()					
					CallModuleFunc(ModuleType.HERO,"OpenView",BaseViewType.HERO_HANDBOOK_DETAIL,hero_compose_info.item_id)
				end)
				app_btn:SetAnchorPoint(0,0.5)
				self.__layout_skill:AddChild(app_btn)
				PosTool.LeftTop(app_btn, (i % col_num) * (sp_skill_icon:GetContentWidth() + col_space), -math.floor((i - 1) / col_num) * (sp_skill_icon:GetContentHeight()*1.5 + row_space))				
			end
		end
	end
end

--显示技能Tips
function LogicGUI.TemplHeroInfo:__ShowSkillTips(hero_id, skill_id, skill_lev, skill_icon,is_special)
	if not self.__skill_tips then
		self.__skill_tips = LogicGUI.TemplHeroSkillTips.New()
		self.__skill_tips:SetAnchorPoint(1, 0.5)
	end

	self.__skill_tips:Retain()
	self.__skill_tips:RemoveSelf(false)
	if is_special then
		self:GetParent():GetWidget("Panel_10"):AddChild(self.__skill_tips:GetNode(),3)
	else
		self:AddChild(self.__skill_tips:GetNode())
	end
	self.__skill_tips:Release()
	local pos
	if is_special then
		pos = NodeUnity.ConvertPosBetweenTwoContainer(skill_icon, self:GetParent():GetWidget("Panel_10"), COCOPoint(-8, skill_icon:GetContentHeight() / 2))
	else
		pos = NodeUnity.ConvertPosBetweenTwoContainer(skill_icon, self.__root_widget, COCOPoint(-8, skill_icon:GetContentHeight() / 2))
	end	
	self.__skill_tips:SetPosition(pos.x, pos.y)
	
	self.__skill_tips:SetData(hero_id, skill_id, skill_lev)
end

--隐藏技能Tips
function LogicGUI.TemplHeroInfo:__HideSkillTips()
	if self.__skill_tips then
		self.__skill_tips:DeleteMe()			
		self.__skill_tips = nil
	end
end

--宿命(援军)
function LogicGUI.TemplHeroInfo:__ShowReinforcement()
	local panel = self:GetWidget("Panel_3")
	local panel_prop = self:GetWidget("Panel_BasicProp3")	
	--先删除Scrollview
	if self.__reinforcement_scrollview then
		self.__reinforcement_scrollview:DeleteMe()
		self.__reinforcement_scrollview = nil
	end
	panel_prop:RemoveAllChild(true)
	
	--激活宿命数量
	local index = 0
	local reinforcement_list = CallModuleFunc(ModuleType.HERO, "GetHeroReinforcementInfoList", self.__data.uid)
	for i,reinforcement_info in ipairs(reinforcement_list) do
		if reinforcement_info.IsMakeWork then
			index = index + 1
		end
	end
	Util:WidgetLabel(self:GetWidget("lab_name3"), CPPGameLib.GetString("hero_reinforcement1",index,#reinforcement_list), Macros.TypefaceSize.largish, Macros.Color.btn, nil, nil, nil, Macros.Color.shadow_color)
	--构建ScrollView
	local cur_item_height = 20
	local cur_row_space = 10
	local params = 
	{
		item_class = Modules.TemplReinforcementInfoCell,
	    item_width = 140,
	    item_height	= cur_item_height,
	    row	= 4,
	    col	= 2,
	    item_space_r = cur_row_space,
	    item_space_c = 4,
	    view_height = 116,
	    is_dynamic_create = false,
	}
	self.__reinforcement_scrollview = GUI.ScrollView.New(params)
	panel_prop:AddChild(self.__reinforcement_scrollview:GetNode())
	PosTool.Center(self.__reinforcement_scrollview)

	self.__reinforcement_scrollview:SetDataList(reinforcement_list)
	panel:SetContentHeight(45 + 4 * (cur_item_height + cur_row_space))
	panel:RefreshLayoutComponent()

	--设置监听跳到宿命界面
	panel:SetTouchEnabled(true)
	WidgetUnity.SetWidgetListener(panel, nil, nil,function ()
		CallModuleFunc(ModuleType.HERO, "OpenView", BaseViewType.HERO_HANDBOOK_DETAIL, self.__data.uid,3,true)
	end)	
end

--天赋
function LogicGUI.TemplHeroInfo:__ShowTalentInfo()
	local panel = self:GetWidget("Panel_3")
	local panel_prop = self:GetWidget("Panel_BasicProp3")

	Util:WidgetLabel(self:GetWidget("lab_name3"), CPPGameLib.GetString("hero_talent"), Macros.TypefaceSize.largish, Macros.Color.headline, nil, nil, nil, Macros.Color.shadow_color)

	local config = CPPGameLib.GetConfig("HeroBreak", self.__data.info_id)
	if not config then
		panel_prop:RemoveAllChild(true)
		return
	end

	self.__lab_talent = self.__lab_talent or {}	

	local y = -10
	local row_space = 2
	local level_index
	for i, info in ipairs(config.level_list) do
		level_index = i
		if not self.__lab_talent[i] then
			self.__lab_talent[i] = {}
			self.__lab_talent[i].lab1 = Util:Label("", Macros.TypefaceSize.slightly, Macros.Color.content, 128)
			self.__lab_talent[i].lab1:SetAnchorPoint(0, 1)
			panel_prop:AddChild(self.__lab_talent[i].lab1)
			PosTool.LeftTop(self.__lab_talent[i].lab1, 2, y)

			self.__lab_talent[i].lab2 = Util:Label("", Macros.TypefaceSize.slightly, Macros.Color.content, 262)
			self.__lab_talent[i].lab2:SetAnchorPoint(0, 1)
			panel_prop:AddChild(self.__lab_talent[i].lab2)
			PosTool.LeftTop(self.__lab_talent[i].lab2, 74, y)
		end

		local talent_str = CPPGameLib.GetString("hero_info_talent1", info.break_name)
		local talent_str2 = CPPGameLib.GetString("hero_info_talent2", info.break_desc, info.level)
		self.__lab_talent[i].lab1:SetString(talent_str)
		self.__lab_talent[i].lab2:SetString(talent_str2)		
		if self.__data.break_level >= info.level then
			self.__lab_talent[i].lab1:SetColor(unpack(Macros.Color.orange))
			self.__lab_talent[i].lab2:SetColor(unpack(Macros.Color.orange))
		else
			self.__lab_talent[i].lab1:SetColor(unpack(Macros.Color.content))
			self.__lab_talent[i].lab2:SetColor(unpack(Macros.Color.content))
		end

		y = y - self.__lab_talent[i].lab2:GetContentHeight() - row_space		
	end

	for i = #self.__lab_talent, level_index + 1, -1 do
		self.__lab_talent[i].lab1:RemoveSelf(true)
		self.__lab_talent[i].lab2:RemoveSelf(true)
		table.remove(self.__lab_talent, i)
	end

	panel:SetContentHeight(36 - y)
	panel:RefreshLayoutComponent()
end


--------------------------------------------------------------------------------------------
--[[
	头像条目
]]
Modules = Modules or {}
Modules.TemplReinforcementInfoCell = Modules.TemplReinforcementInfoCell or BaseClass(GUI.ScrollItem)

function Modules.TemplReinforcementInfoCell:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplReinforcementInfoCell:__CreateWidgetFromLuaCode()
  self.__layout = Util:Layout(140,20)
  self.__layout:SetTouchEnabled(false)
  self.__layout:SetClippingEnable(false)
  self.__layout:SetBackGroundColor(0, 0, 0)
  return self.__layout
end

function Modules.TemplReinforcementInfoCell:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	--创建单条宿命
   	local lab_reinforcement,widget_list = Util:LabCenter({
   		{tp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, self.__data.IsMakeWork and "btn_select_yes_orange_small" or "btn_select_no_small")},
   		{zi = self.__data.reinforcementname, ys = self.__data.IsMakeWork and Macros.Color.red or Macros.Color.content, dx = Macros.TypefaceSize.normal, jg = 4},
	})				
	lab_reinforcement:SetAnchorPoint(0, 0.5)
	self.__lab_reinforcement = widget_list[2]
	if lab_reinforcement then
		self.__layout:AddChild(lab_reinforcement)
		PosTool.LeftCenter(lab_reinforcement,0,0)
	end
end

function Modules.TemplReinforcementInfoCell:GetLabReinforcementInfo()
	return self.__lab_reinforcement
end