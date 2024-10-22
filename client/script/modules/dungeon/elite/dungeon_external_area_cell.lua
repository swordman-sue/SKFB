Modules = Modules or {}

--副本章节模板
Modules.TemplDungeonExternalAreaCell = Modules.TemplDungeonExternalAreaCell or BaseClass(GUI.ScrollItem)

function Modules.TemplDungeonExternalAreaCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.DUNGEON_EXTERNAL_AREA_CELL)	
end

function Modules.TemplDungeonExternalAreaCell:__delete()
	if self.__fightting_effect_id then
		self:__DelEffect(self.__fightting_effect_id)
		self.__fightting_effect_id = nil
	end
end

function Modules.TemplDungeonExternalAreaCell:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	--头像
	local gamelevel_info = CPPGameLib.GetConfig("GameLevel",self.__data.map_id)
	local monster_info = CPPGameLib.GetConfig("Monster",gamelevel_info.icon_id)
	local icon_id = monster_info.model_type and monster_info.model_id or monster_info.hero_id
	local icon_path_func = LogicGUI.IconLayer.IconPathFuncMap[Macros.Game.ObjType.HERO]
	local icon_path, icon_scale = icon_path_func(icon_id)
	self:GetWidget("Image_headicon"):IgnoreContentAdaptWithSize(true)
	self:GetWidget("Image_headicon"):SetImage(icon_path,TextureResType.LOCAL)
	self:GetWidget("Image_headicon"):SetScaleFactor(1.2)
	--名字
	Util:WidgetLabel(self:GetWidget("Text_monsterName"), monster_info.name, Macros.TypefaceSize.largish, Macros.Quality[monster_info.quality], 0, 0, Macros.Quality_Stroke[monster_info.quality])
	--章节
	Util:WidgetLabel(self:GetWidget("Text_chapterIndex"),CPPGameLib.GetString("dungeon_chapter_num",self.__data.chapter_index), Macros.TypefaceSize.largish,Macros.Color.btn)
	--关卡名字
	Util:WidgetLabel(self:GetWidget("Text_chapterName"), gamelevel_info.name, Macros.TypefaceSize.largish, Macros.Color.btn)
	--战斗按钮
	self:GetWidget("Image_killed"):SetVisible(self.__data.is_dead == Macros.Global.TRUE)
	local image_fight = self:GetWidget("Image_fight")
	image_fight:SetVisible(self.__data.is_dead == Macros.Global.FALSE)
	WidgetUnity.SetWidgetListener(image_fight, nil, nil,function ()
		Modules.TemplExternalAreaChallengeView.New(self.__data)
	end)
	if self.__data.is_dead == Macros.Global.FALSE then
		local effect
		self.__fightting_effect_id, effect = self:__CreateEffect({res_name = "UI_xiaodao",scale = 0.8}, image_fight,2)
		PosTool.Center(effect,0,20)	
		--战斗文字
		self:GetWidget("Image_fightTxt"):SetZOrder(3)
	end
end