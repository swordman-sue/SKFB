--[[
%% @module: 精灵详情界面
%% @author: ldx
%% @created: 2018年4月13日20:13:29
--]]

LogicGUI = LogicGUI or {}

LogicGUI.TemplSpriteDetailView = LogicGUI.TemplSpriteDetailView or BaseClass(GUI.Template)

function LogicGUI.TemplSpriteDetailView:__init(lineup_data)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__open_callback = function ()
	    self:__InitWidget(lineup_data)
	end	
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.SPIRIT_DETAIL)
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_SPRITE_DETAIL) 
end

function LogicGUI.TemplSpriteDetailView:__delete()
	
end

function LogicGUI.TemplSpriteDetailView:__InitWidget(lineup_data)
	self.__data = lineup_data
	local panel_big = self:GetWidget("Panel_Big")
	--当前的精灵id 
	local cur_spirit_id = self.__data.lineup_info.bless_sprite_id
	--精灵配置
	self.__config_spirit = CPPGameLib.GetConfig("Spirit",cur_spirit_id)
	local config_train = CPPGameLib.GetConfig("SpriteTrain",cur_spirit_id)
	self.__spirit_info = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritInfoByID",cur_spirit_id)
	self.__selected_spirit_skill_lv = config_train.stage_list[self.__spirit_info.train_stage].skill_level
	--关闭按钮
	WidgetUnity.SetWidgetListener(self:GetWidget("Button_close"), nil, nil,function ()
		self:DeleteMe()
	end)
	--精灵名字
	self.__txt_spiritname = self:GetWidget("Text_spiritName")
	Util:WidgetLabel(self.__txt_spiritname, "", Macros.TypefaceSize.tab)
	--精灵名
	self.__txt_spiritname:SetText(self.__config_spirit.name..CPPGameLib.GetString("spirit_sublimate_level",self.__selected_spirit_skill_lv))
	self.__txt_spiritname:SetColor(unpack(Macros.Quality[self.__config_spirit.quality]))
	self.__txt_spiritname:SetStrokeColor(unpack(Macros.Quality_Stroke[self.__config_spirit.quality]))
	--精灵模型
	local panel_spiritmodule = self:GetWidget("Panel_spiritModule")
	self.__spirit_bless_model = LogicGUI.Model.New({info_id = cur_spirit_id,model_type = LogicGUI.ModelType.SPRITE,parent = panel_spiritmodule})
	self.__spirit_bless_model:SetScaleFactor(2)
	self.__spirit_bless_model:SetPosition(5,cur_spirit_id == 50012 and -30 or 5)
	--标题
	local txt_title = self:GetWidget("Text_title")
	Util:WidgetLabel(txt_title,CPPGameLib.GetString("spirit_detail_tab"), Macros.TypefaceSize.tab, Macros.Color.Special)
	--祝福英雄
	local hero_info = CallModuleFunc(ModuleType.HERO,"GetHeroInfo",self.__data.lineup_info.uid)
	local config_hero = CPPGameLib.GetConfig("Hero",hero_info.info_id)
	local rt_lab
	if hero_info.break_level > 0 then
		rt_lab = string.format(Macros.Quality_Stroke_RT[config_hero.init_quality],CPPGameLib.GetString("common_add6", config_hero.name, hero_info.break_level))
	else
		rt_lab = string.format(Macros.Quality_Stroke_RT[config_hero.init_quality],config_hero.name)
	end		
	local bless_hero_rt = Util:RichText(CPPGameLib.GetString("spirit_bless_heroname",rt_lab), Macros.TypefaceSize.normal)
	panel_big:AddChild(bless_hero_rt)
	bless_hero_rt:SetPosition(266.5,385)
	--星数容器
	local panel_starupnum = self:GetWidget("Panel_StarUpNum")
	--星数
	local speciality_info = CPPGameLib.GetConfig("SpriteSpecialProperty",cur_spirit_id)
	local cur_star = self.__spirit_info and self.__spirit_info.star or 1
	if speciality_info then
		local max_star = table.maxn(speciality_info.star_list)		
		local org_pos = -(max_star - 1) * 35/2 + 75
		local org_dis = 35
		for i=1,max_star do
			local star_img = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.SPIRIT_DETAIL,cur_star >= i and "bigstar" or "bigstar_bg")
			if not self["__spirit_star"..i] then
				self["__spirit_star"..i] = Util:Sprite(star_img)
				self["__spirit_star"..i]:SetAnchorPoint(0.5, 0.5)
				panel_starupnum:AddChild(self["__spirit_star"..i],2)
			else
				self["__spirit_star"..i]:SetImage(star_img)
			end
			self["__spirit_star"..i]:SetPosition((i-1)*org_dis+org_pos,15)
		end
	end
	--替换
	local btn_replace = self:GetWidget("Button_Replace")
	Util:WidgetButton(btn_replace,"",function ()
		self:DeleteMe()
		--打开祝福界面
		GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_SPRITE_BLESS, false, 1, {lineup_index = self.__data.lineup_index,uid = self.__data.lineup_info.uid})
	end)
	Util:WidgetLabel(self:GetWidget("Text_Replace"), CPPGameLib.GetString("spirit_detail_replace"), Macros.TypefaceSize.button, Macros.Color.btn, 0, 0, Macros.Color.button_yellow)
	--解除
	local btn_relieve = self:GetWidget("Button_Relieve")
	Util:WidgetButton(btn_relieve,"",function ()
		self:DeleteMe()
		CallModuleFunc(ModuleType.SPIRIT,"SpiritBlessReq",{sprite_id = 0,bless_type = Macros.Game.BLESS_TYPE.CANCEL_BLESS,formation_pos = (self.__data.lineup_index - 1)})
	end)
	Util:WidgetLabel(self:GetWidget("Text_Relieve"), CPPGameLib.GetString("spirit_detail_relieve"), Macros.TypefaceSize.button, Macros.Color.btn, 0, 0, Macros.Color.button_yellow)
	--培养
	local btn_train = self:GetWidget("Button_Train")
	Util:WidgetButton(btn_train,"",function ()
		self:DeleteMe()
		GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.SPIRIT,{self.__data.lineup_info.bless_sprite_id})
	end)
	Util:WidgetLabel(self:GetWidget("Text_Train"), CPPGameLib.GetString("spirit_detail_train"), Macros.TypefaceSize.button, Macros.Color.btn, 0, 0, Macros.Color.button_yellow)
	--滚动容器
	self.__scv = self:GetWidget("ScrollView_Detail")
	--初始化控件
	self.__panel_name_list = {"Panel_BasicInfo","Panel_SpecialityInfo","Panel_BlessOfSkillInfo"}
	for _, panel_name in ipairs(self.__panel_name_list) do
		self[panel_name] = self:GetWidget(panel_name)
	end
	--基础属性
	Util:WidgetLabel(self:GetWidget("Text_BasicTitle"), CPPGameLib.GetString("spirit_blessingofprop_title"), Macros.TypefaceSize.popup, Macros.Color.btn, nil, nil, nil, Macros.Color.shadow_color)
	--精灵特性
	Util:WidgetLabel(self:GetWidget("Text_SpecialityTitle"), CPPGameLib.GetString("spirit_speciality_title"), Macros.TypefaceSize.popup, Macros.Color.btn, nil, nil, nil, Macros.Color.shadow_color)
	--祝福技能
	Util:WidgetLabel(self:GetWidget("Text_BlessingOfSkillTitle"), CPPGameLib.GetString("spirit_blessingofskill_title"), Macros.TypefaceSize.popup, Macros.Color.btn, nil, nil, nil, Macros.Color.shadow_color)
	--
	self:__Reset()
end

--刷新
function LogicGUI.TemplSpriteDetailView:__Reset()
	self:__ShowBasicInfo()
	self:__ShowSpecialityInfo()
	self:__ShowBlessOfSkillInfo()

	--重置滚动区域
	local height = 0
	local panel
	for _, panel_name in ipairs(self.__panel_name_list) do
		panel = self[panel_name]
		PosTool.CenterTop(panel, 0, -height)
		height = height + panel:GetContentHeight()
	end
	self.__scv:SetInnerContainerSize(0, height)
	self.__scv:RefreshLayoutComponent()
	self.__scv:ScrollToTop()
end

--基础属性
function LogicGUI.TemplSpriteDetailView:__ShowBasicInfo()
	--处理精灵属性加成
	local spirit_data = CallModuleFunc(ModuleType.SPIRIT, "GetSpiritData")
	self.__old_plus_prop_map = ConfigAdapter.Spirit.GetPropMapEx(spirit_data,self.__data.lineup_info.bless_sprite_id,nil,true)
	--属性
	for pro_type, pro_value in ipairs(self.__old_plus_prop_map) do
		local pro_str = Util:Nature(pro_type,math.floor(pro_value),nil,true)
		if not self["__txt_property"..pro_type] and self:GetWidget("Text_property"..pro_type) then
			self["__txt_property"..pro_type] = self:GetWidget("Text_property"..pro_type)
			Util:WidgetLabel(self["__txt_property"..pro_type],"", Macros.TypefaceSize.normal, Macros.Color.btn)
		end
		self["__txt_property"..pro_type]:SetText(pro_str or "")
	end
	--等级
	if not self.__txt_property5 then
		self.__txt_property5 = self:GetWidget("Text_property5")
		Util:WidgetLabel(self.__txt_property5,"", Macros.TypefaceSize.normal, Macros.Color.btn)
	end
	self.__txt_property5:SetText(CPPGameLib.GetString("common_lev2",self.__old_plus_prop_map.enhance_level or 0))
	--资质
	local cur_star = self.__spirit_info and self.__spirit_info.star or 0
	if not self.__txt_property6 then
		self.__txt_property6 = self:GetWidget("Text_property6")
		Util:WidgetLabel(self.__txt_property6,"", Macros.TypefaceSize.normal, Macros.Color.btn)
	end
	self.__txt_property6:SetText(CPPGameLib.GetString("spirit_qulaity_txt",self.__config_spirit.qualifications[cur_star+1]))
end

--精灵特性
function LogicGUI.TemplSpriteDetailView:__ShowSpecialityInfo()
	if not self.__panel_speciality then
		self.__panel_speciality = self:GetWidget("Panel_Speciality")
	end
	local cur_spirit_id = self.__data.lineup_info.bless_sprite_id
	local config_speciality = CPPGameLib.GetConfig("SpriteSpecialProperty", cur_spirit_id)
	if not config_speciality then
		self.__panel_speciality:RemoveAllChild(true)
		self.__lab_speciality_list = nil
		return
	end
	self.__lab_speciality_list = self.__lab_speciality_list or {}

	local y = -0.5
	local row_space = 12
	local level_index
	local cur_star = self.__spirit_info and self.__spirit_info.star or 1
	for i, speciality_info in ipairs(config_speciality.star_list) do
		level_index = i
		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, cur_star >= i and "sp_bright_star" or "sp_sky_star")
		if not self.__lab_speciality_list[i] then
			--星
			self.__lab_speciality_list[i] = {}
			self.__lab_speciality_list[i].star = Util:Sprite(sp)
			self.__lab_speciality_list[i].star:SetAnchorPoint(0.5, 1)
			self.__panel_speciality:AddChild(self.__lab_speciality_list[i].star)
			PosTool.LeftTop(self.__lab_speciality_list[i].star, 30.5, y)
			--描述
			self.__lab_speciality_list[i].rt = Util:RichText("", Macros.TypefaceSize.normal,320,0)
			self.__lab_speciality_list[i].rt:SetAnchorPoint(0, 1)
			self.__panel_speciality:AddChild(self.__lab_speciality_list[i].rt)
			PosTool.LeftTop(self.__lab_speciality_list[i].rt, 53.5, y - 3)
		end

		self.__lab_speciality_list[i].star:SetImage(sp)

		local lab = GUI.RichText.GetColorWord(speciality_info and speciality_info.speciality_des or "", Macros.Color.btn_hex)
		self.__lab_speciality_list[i].rt:SetText(lab)	

		y = y - self.__lab_speciality_list[i].rt:GetContentHeight() - row_space		
	end

	self.Panel_SpecialityInfo:SetContentHeight(43 - y)
	PosTool.CenterTop(self.__panel_speciality,0,-43)
end

--祝福技能
function LogicGUI.TemplSpriteDetailView:__ShowBlessOfSkillInfo()
	if not self.__init_blessofskill_info then
		--技能图标容器
		self.__panel_skillicon = self:GetWidget("Panel_SkillIcon")
		--祝福技能名字
		self.__txt_blessofskillname = self:GetWidget("Text_BlessOfSkillName")
		Util:WidgetLabel(self.__txt_blessofskillname,"", Macros.TypefaceSize.largish, Macros.Color.Special)
		--祝福技能描述
		self.__txt_blessofskilldes = Util:RichText("", Macros.TypefaceSize.normal,265,0)
		self.__txt_blessofskilldes:SetAnchorPoint(0, 1)
		self.Panel_BlessOfSkillInfo:AddChild(self.__txt_blessofskilldes)
		PosTool.LeftTop(self.__txt_blessofskilldes, 109.50, 72.5)
		self.__init_blessofskill_info = true
	end
	local cur_spirit_id = self.__data.lineup_info.bless_sprite_id
	--特性配置
	local config_speciality = CPPGameLib.GetConfig("SpriteSpecialProperty",cur_spirit_id)
	--祝福技能名字
	self.__txt_blessofskillname:SetText(config_speciality.star_list and config_speciality.star_list[1].bless_skill_name or "")
	--当前星级
	local cur_star = self.__spirit_info and self.__spirit_info.star or 0
	--祝福技能描述
	local lab = GUI.RichText.GetColorWord(config_speciality.star_list and config_speciality.star_list[cur_star].bless_skill_des or "", Macros.Color.btn_hex)
	self.__txt_blessofskilldes:SetText(lab)
	--祝福技能图标
	local path = Resource.PathTool.GetSpiritSkillIconPath(self.__config_spirit.bless_skill_icon)
	if not self.__bless_skill_icon then						
		self.__bless_skill_icon = Util:Sprite(path)
		self.__panel_skillicon:AddChild(self.__bless_skill_icon,2)
		PosTool.Center(self.__bless_skill_icon)
	else
		self.__bless_skill_icon:SetImage(path)
	end	
	--大小
	local height = self.__txt_blessofskilldes:GetContentHeight() >= 50 and self.__txt_blessofskilldes:GetContentHeight() or 50
	self.Panel_BlessOfSkillInfo:SetContentHeight(72.5 + height + 5)
	PosTool.LeftTop(self.__txt_blessofskilldes,109.5,-72.5)
	--self.Panel_BlessOfSkillInfo:SetVisible(false)
end