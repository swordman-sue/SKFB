
--[[
%% @module：英雄潜能成功模板
%% @author: swordman sue
%% @created: 2016-12-14
--]]

Modules = Modules or {}

Modules.TemplHeroPotentialSucc = Modules.TemplHeroPotentialSucc or BaseClass(GUI.Template)

function Modules.TemplHeroPotentialSucc:__init(data)
	self.__is_auto_load = true	
	self.__open_callback = function()
		GUI.Template.SetData(self, data)

		self:__OpenCallback()
	end

	self.__bg_params = {type = BaseView.BGType.SOLID}

	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_POTENTIAL, true)
	
	self:InitTemplate(GUI.TemplateRegistry.HERO_POTENTIAL_SUCC)
end

function Modules.TemplHeroPotentialSucc:__delete()
	--战力变化 --潜能
	CallModuleFunc(ModuleType.ROLE, "RecordBattlePowerChanged", Macros.Game.COMBAT_CHANGE.HERO_POTENTIAL, true)
end

function Modules.TemplHeroPotentialSucc:__OpenCallback()
	self:GetWidget("Image_ribbon"):IgnoreContentAdaptWithSize(true)
	
	self:GetWidget("Image_upgrade"):IgnoreContentAdaptWithSize(true)
	
	local hero_info = CallModuleFunc(ModuleType.HERO, "GetHeroInfo", self.__data)
	local config_hero = CPPGameLib.GetConfig("Hero", hero_info.info_id)
	local config_potential = CPPGameLib.GetConfig("HeroDestiny", hero_info.potential_level - 1)
	local next_config_potential = CPPGameLib.GetConfig("HeroDestiny", hero_info.potential_level)	

	--当前和下阶等级
	Util:WidgetLabel(self:GetWidget("Text_Potential_CurLev"), CPPGameLib.GetString("hero_potential_info3", hero_info.potential_level - 1), Macros.TypefaceSize.largish, Macros.Color.btn)
	Util:WidgetLabel(self:GetWidget("Text_Potential_NextLev"), CPPGameLib.GetString("hero_potential_info3", hero_info.potential_level), Macros.TypefaceSize.largish, Macros.Color.green)

	--当前和下阶属性加成
	local prop_name, cur_prop_value, next_prop_value
	local cur_prop_map = ConfigAdapter.Hero.GetPotentialPlusPropMap(hero_info.potential_level - 1 , hero_info.info_id )
	local next_prop_map = ConfigAdapter.Hero.GetPotentialPlusPropMap(hero_info.potential_level , hero_info.info_id )
	local prop_type_list = {Macros.Game.BattleProp.ATTACK_RATE, Macros.Game.BattleProp.HP_RATE, Macros.Game.BattleProp.PHYSICAL_DEFENCE_RATE, Macros.Game.BattleProp.MAGIC_DEFENCE_RATE}
	for i, prop_type in ipairs(prop_type_list) do
		prop_name = CPPGameLib.GetString("common_str_with_colon", Macros.Game.BattlePropName[prop_type])
		cur_prop_value = MathTool.TransProbCoeffPercentage(cur_prop_map and cur_prop_map[prop_type] or 0)
		next_prop_value = MathTool.TransProbCoeffPercentage(next_prop_map[prop_type] or 0)

		Util:WidgetLabel(self:GetWidget("Text_CurPropTitle"..prop_type), prop_name, Macros.TypefaceSize.largish, Macros.Color.golden)
		Util:WidgetLabel(self:GetWidget("Text_NextPropTitle"..prop_type), prop_name, Macros.TypefaceSize.largish, Macros.Color.golden)

		Util:WidgetLabel(self:GetWidget("Text_CurProp"..prop_type), string.format("+%d%%", cur_prop_value), Macros.TypefaceSize.largish, Macros.Color.btn)
		Util:WidgetLabel(self:GetWidget("Text_NextProp"..prop_type), string.format("+%d%%", next_prop_value), Macros.TypefaceSize.largish, Macros.Color.green)
	end

	--当前和下阶技能等级
	local index = 1
	local max_index = 3
	local config_skill
	local icon_list = {[Macros.Game.SkillType.SPECIAL] = "icon_skill",[Macros.Game.SkillType.ANGER] = "icon_superskill",[Macros.Game.SkillType.JOINT] = "icon_together"}
	for _, skill_id in ipairs(config_hero.skill_list) do
		config_skill = CPPGameLib.GetConfig("Skill", skill_id)
		--只影响特殊、怒气/合击技能
		if config_skill and 
			config_skill.type == Macros.Game.SkillType.SPECIAL or 
			config_skill.type == Macros.Game.SkillType.ANGER or
			config_skill.type == Macros.Game.SkillType.JOINT then

			local image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, icon_list[config_skill.type],true)
			self:GetWidget("Image_curSkill"..index):SetImage(image_data)
			self:GetWidget("Image_nextSkill"..index):SetImage(image_data)
			Util:WidgetLabel(self:GetWidget("CurSkill"..index), CPPGameLib.GetString("hero_potential_skill_name", config_skill.name, CPPGameLib.GetString("common_lev5",config_potential.skill_level)), Macros.TypefaceSize.largish, Macros.Color.btn)
			Util:WidgetLabel(self:GetWidget("NextSkill"..index), CPPGameLib.GetString("hero_potential_skill_name", config_skill.name, CPPGameLib.GetString("common_lev5",next_config_potential.skill_level)), Macros.TypefaceSize.largish, Macros.Color.green)
			index = index + 1
		end
	end	
	self:GetWidget("Image_bg"):SetContentSize(798,250 + 43 * (index - 1))
	local widget
	for i = index, max_index do
		self:GetWidget("CurSkill"..i):SetVisible(false)
		self:GetWidget("NextSkill"..i):SetVisible(false)
		self:GetWidget("Image_Skill"..i):SetVisible(false)
		self:GetWidget("Image_curSkill"..index):SetVisible(false)
		self:GetWidget("Image_nextSkill"..index):SetVisible(false)
	end

	--继续
	self.__text_continue = Util:WidgetLabel(self:GetWidget("Text_Continue"), CPPGameLib.GetString("common_continue"), Macros.TypefaceSize.popup, Macros.Color.btn)
	self.__text_continue:SetVisible(false)
	WidgetUnity.SetWidgetListener(self.__root_widget, nil, nil, function()
		if not self.__is_anim_end then
			return
		end

		self:DeleteMe()
	end)

	--播放特效
	self.__is_anim_end = false
	self.__panel_bg = self:GetWidget("Panel_BG")
	self.__panel_bg:SetVisible(false)
	self:__PlayEffect()
end

function Modules.TemplHeroPotentialSucc:__PlayEffect()
	--出场特效
	local effect_id, effect = self:__CreateEffect({id = 100016, loop = false, delay_time = 0.9, delay_callback = function()
		self.__panel_bg:SetVisible(true)

		--皇冠特效
		effect_id, effect = self:__CreateEffect({id = 100017}, self:GetWidget("Panel_CircleEffect"))
		PosTool.Center(effect, 0, -15)	

		--播放CSB动画
		self:PlayActionTimeline("animation", false, function()
			self.__is_anim_end = true
			self.__text_continue:SetVisible(true)
		end)

	end}, self:GetWidget("Panel_Shield"))
	PosTool.Center(effect, -2, -2)	
end