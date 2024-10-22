LogicGUI = LogicGUI or {}

LogicGUI.TemplSpiritIntroduceView = LogicGUI.TemplSpiritIntroduceView or BaseClass(GUI.Template)

function LogicGUI.TemplSpiritIntroduceView:__init(spirit_id,fun)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:initUI(spirit_id,fun)
	end
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.SPIRIT)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.SPIRIT,true)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.WORLD_HREAT,true)
	self:InitTemplate(GUI.TemplateRegistry.SPIRIT_INTRODUCE)	
end

function LogicGUI.TemplSpiritIntroduceView:__delete()
	if self.__spirit_model then
		self.__spirit_model:DeleteMe()
		self.__spirit_model = nil
	end
end

function LogicGUI.TemplSpiritIntroduceView:initUI(spirit_id,fun)
	--关闭按钮
	WidgetUnity.SetWidgetListener(self:GetWidget("Button_Close"),nil,nil,function ()
		self:DeleteMe()
	end)

	--配置
	local spirit_config = CPPGameLib.GetConfig("Spirit",spirit_id)
	--精灵名字
	self.__text_spiritname = self:GetWidget("Text_SpiritName")
	Util:WidgetLabel(self.__text_spiritname, "", Macros.TypefaceSize.tab)
	--精灵名
	self.__text_spiritname:SetText(spirit_config.name)
	self.__text_spiritname:SetColor(unpack(Macros.Quality[spirit_config.quality]))
	self.__text_spiritname:SetStrokeColor(unpack(Macros.Quality_Stroke[spirit_config.quality]))

	--星数
	for i=1,5 do
		self:GetWidget("Image_Star"..i):SetVisible(spirit_config.init_star >= i)
	end

	--精灵资质
	self.__txt_quality = self:GetWidget("Text_Quality")
	Util:WidgetLabel(self.__txt_quality,"", Macros.TypefaceSize.normal,Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
	--资质
	self.__txt_quality:SetText(CPPGameLib.GetString("spirit_quality",spirit_config.qualifications[spirit_config.init_star+1]))
	
	--精灵模型容器
	self.__panel_spiritmodule = self:GetWidget("Panel_SpiritModule")
	--精灵模型
	self.__spirit_model = LogicGUI.Model.New({info_id = spirit_config.model_id,
							parent = self.__panel_spiritmodule,model_type = LogicGUI.ModelType.SPRITE})		
		self.__spirit_model:SetScaleFactor(2)
		self.__spirit_model:SetPosition(10,spirit_config.model_id == 50012 and -30 or 40)

	--详情
	local btn_detail = self:GetWidget("Button_Detail")
	Util:WidgetButton(btn_detail, CPPGameLib.GetString("email_reply_btn"),function ()
		if fun then
			fun()
		end
		self:DeleteMe()	
		--跳转去精灵系统
		CallModuleFunc(ModuleType.SPIRIT, "OpenView", BaseViewType.SPIRIT, unpack({spirit_id}))
	end,Macros.TypefaceSize.button,Macros.Color.btn,Macros.Color.button_red)	

	--精灵技能
	Util:WidgetLabel(self:GetWidget("Text_SkillTitle"), CPPGameLib.GetString("spirit_spiritskill"), Macros.TypefaceSize.popup, Macros.Color.btn, nil, nil, nil, Macros.Color.shadow_color)	
	--技能列表
	local skill_id_list = {spirit_config.auto_skill_id,spirit_config.special_skill_id,spirit_config.common_skill_id}
	--技能图标
	for i=1,3 do
		local path = Resource.PathTool.GetSpiritSkillIconPath(skill_id_list[i])
		if not self["__spirit_icon"..i] then						
			self["__spirit_icon"..i] = Util:Sprite(path)
			self:GetWidget("Image_skillIcon"..i):AddChild(self["__spirit_icon"..i],0)
			PosTool.Center(self["__spirit_icon"..i])
		else
			self["__spirit_icon"..i]:SetImage(path)
		end	
	end
	--技能被动文字层级
	self:GetWidget("Image_passivityTxt"):SetZOrder(2) 
	--技能通用文字层级
	self:GetWidget("Image_commonTxt"):SetZOrder(2) 
	--技能专属文字层级
	self:GetWidget("Image_specialTxt"):SetZOrder(2)

	--特性配置
	local config_speciality = CPPGameLib.GetConfig("SpriteSpecialProperty",spirit_id)
	--祝福技能
	Util:WidgetLabel(self:GetWidget("Text_BlessingOfSkillTitle"), CPPGameLib.GetString("spirit_blessingofskill_title"), Macros.TypefaceSize.popup, Macros.Color.btn, nil, nil, nil, Macros.Color.shadow_color)
	--技能图标容器
	self.__panel_skillicon = self:GetWidget("Panel_SkillIcon")
	--祝福技能名字
	self.__txt_blessofskillname = self:GetWidget("Text_BlessOfSkillName")
	Util:WidgetLabel(self.__txt_blessofskillname,config_speciality.star_list and config_speciality.star_list[1].bless_skill_name or "", Macros.TypefaceSize.largish, Macros.Color.golden,0,0,Macros.Color.common_stroke)
	--祝福技能描述
	self.__txt_blessofskilldes = Util:RichText("", Macros.TypefaceSize.normal,250,0,Macros.Color.btn_hex,Macros.Color.common_stroke_hex)
	self.__txt_blessofskilldes:SetAnchorPoint(0, 1)
	self:GetWidget("Panel_BlessOfSkillInfo"):AddChild(self.__txt_blessofskilldes)
	PosTool.LeftTop(self.__txt_blessofskilldes, 105, -75.5)

	--祝福技能描述
	local lab = GUI.RichText.GetColorWord(config_speciality.star_list and config_speciality.star_list[spirit_config.init_star].bless_skill_des or "", Macros.Color.btn_hex,Macros.Color.common_stroke_hex,2)
	self.__txt_blessofskilldes:SetText(lab)
	--祝福技能图标
	local path = Resource.PathTool.GetSpiritSkillIconPath(spirit_config.bless_skill_icon)
	if not self.__bless_skill_icon then						
		self.__bless_skill_icon = Util:Sprite(path)
		self.__panel_skillicon:AddChild(self.__bless_skill_icon,2)
		PosTool.Center(self.__bless_skill_icon)
	else
		self.__bless_skill_icon:SetImage(path)
	end	

	--转换率
	local config_train = CPPGameLib.GetConfig("SpriteTrain", spirit_id)	
	local pro_conversion_rate = MathTool.TransProbCoeffPercentage(config_train.stage_list[1].pro_conversion_rate) or 0
	local txt_tips = self:GetWidget("Text_tips")
	Util:WidgetLabel(txt_tips,CPPGameLib.GetString("spirit_transformtips_txt",pro_conversion_rate), 12, Macros.Color.btn,0,0,Macros.Color.common_stroke)
end
