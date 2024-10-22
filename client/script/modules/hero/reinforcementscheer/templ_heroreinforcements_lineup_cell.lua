
--[[
%% @module：英雄上阵项模板
%% @author: swordman sue
%% @created: 2016-08-24
--]]

Modules = Modules or {}

Modules.TemplHeroReinforcementsLineupCell = Modules.TemplHeroReinforcementsLineupCell or BaseClass(GUI.ScrollItem)

function Modules.TemplHeroReinforcementsLineupCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.HERO_LINEUP_ITEM)

	self.__panel_icon = self:GetWidget("Panel_Icon")
	self.__txt_name = self:GetWidget("Text_Name")
	self.__img_profession = self:GetWidget("Image_Profession")
	self.__img_lineup = self:GetWidget("Image_Lineup")
	self.__img_lineup:SetZOrder(1)

	self.__btn_lineup = Util:WidgetButton(self:GetWidget("Button_Lineup"), CPPGameLib.GetString("hero_lineup_btn_word"), nil, Macros.TypefaceSize.largish,Macros.Color.btn,Macros.Color.button_yellow)
	
	self.__txt_lev = Util:WidgetLabel(self:GetWidget("Text_Level"), nil, Macros.TypefaceSize.normal,Macros.Color.content)

	self.__text_reinforcement = Util:WidgetLabel(self:GetWidget("Text_reinforcement"), nil, Macros.TypefaceSize.normal, Macros.Color.content)

	Util:WidgetLabel(self:GetWidget("Text_Level_Title"), CPPGameLib.GetString("common_lev1"), Macros.TypefaceSize.normal, Macros.Color.keypoint)
end

function Modules.TemplHeroReinforcementsLineupCell:__delete()
end

function Modules.TemplHeroReinforcementsLineupCell:GetTouchNode()
	return self.__btn_lineup
end

--[[
	hero_info
--]]
function Modules.TemplHeroReinforcementsLineupCell:SetData(data)
	    -- CPPGameLib.PrintTable(data,"物品合成响应")
	GUI.ScrollItem.SetData(self, data)

	local hero_info = self.__data.hero_info
	local config_hero = CPPGameLib.GetConfig("Hero", hero_info.info_id)

	--觉醒等级
	local lab = ""
	if data.hero_info.break_level ~= 0 then
		lab = "+"..data.hero_info.break_level
	end

	--英雄名字、等级
	self.__txt_name:SetText(config_hero.name .. " " .. lab )
	self.__txt_name:SetTextColor(Macros.QualityColor(hero_info.quality_level))
	self.__txt_name:SetStrokeColor(Macros.QualityColorStroke(hero_info.quality_level))
	self.__txt_lev:SetText(hero_info.level)

	--职业
	self.__img_profession:SetImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON, "icon_profession_"..config_hero.profession, true))

	--上阵标记
	self.__img_lineup:SetVisible(hero_info.is_to_battle == Macros.Global.TRUE)

	--英雄图标
	if not self.__icon_layer then
		self.__icon_layer = LogicGUI.IconLayer.New(true)
		-- self.__icon_layer:SetTouchEnabled(false)
		self.__panel_icon:AddChild(self.__icon_layer:GetNode(), 0)
		PosTool.Center(self.__icon_layer)		
	end
	self.__icon_layer:SetData({id = hero_info.info_id, obj_info = hero_info})

	--宿命
	local fate_num = CallModuleFunc(ModuleType.HERO, "GetCanActiavteReinforcementsNum", hero_info.uid, self.__data.lineup_type, self.__data.lineup_index)		
	self.__text_reinforcement:SetVisible(fate_num > 0)	
	--是否有合击
	local is_togetherattack = false
	if self.__data.lineup_type == Macros.Game.HeroLineup.HERO_LINEUP then		
		local hero_name_list,quality_list = CallModuleFunc(ModuleType.HERO, "GetCanActiavteTogetherAttackName", hero_info.info_id,self.__data.lineup_index)
		if #hero_name_list > 0 and #quality_list > 0 then
			is_togetherattack = true
			local lab = ""
			self.__txt_fate = Util:RichText("",Macros.TypefaceSize.normal,245,0)
			self:GetNode():AddChild(self.__txt_fate)
			self.__txt_fate:SetAnchorPoint(0,0.5) 
			self.__txt_fate:SetPosition(105,fate_num > 0 and 44 or 31)
			for i,name in ipairs(hero_name_list) do
				if i ~= #hero_name_list then
					lab = lab..CPPGameLib.GetString("hero_lineup_togetherattack",string.format(Macros.Quality_RT[quality_list[i]],name))..CPPGameLib.GetString("common_Comma")
				else
					lab = lab..CPPGameLib.GetString("hero_lineup_togetherattack",string.format(Macros.Quality_RT[quality_list[i]],name))
				end
			end		
			lab = GUI.RichText.GetColorWord(lab, Macros.Color.content_hex)
			self.__txt_fate:SetText(lab)		
			self.__text_reinforcement:SetText(CPPGameLib.GetString("hero_lineup_active_fate", fate_num))			
		else
			self.__text_reinforcement:SetText(CPPGameLib.GetString("hero_lineup_active_fate", fate_num))	
		end
	else		
		self.__text_reinforcement:SetText(CPPGameLib.GetString("hero_lineup_active_fate", fate_num))	
	end
	--宿命位置
	self.__text_reinforcement:SetPosition(105,is_togetherattack and 22 or 31)
	--上阵按钮
	self.__btn_lineup:SetEnabled(hero_info.is_to_battle == Macros.Global.FALSE)

	--英雄出战按钮（英雄ID）
	CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.HERO_BATTLE_BTN, self.__btn_lineup, hero_info.info_id)			
end

