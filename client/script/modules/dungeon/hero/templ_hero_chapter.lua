
Modules = Modules or {}

--英雄副本章节模板
Modules.TemplHeroDungeonChapter = Modules.TemplHeroDungeonChapter or BaseClass(GUI.ScrollItem)

function Modules.TemplHeroDungeonChapter:__init()
	self:InitTemplate(GUI.TemplateRegistry.DUNGEON_CHAPTER)	

	local widget = self:GetWidget("Panel_Box")
	widget:SetVisible(false)
	widget = self:GetWidget("Panel_Star")
	widget:SetVisible(false)

	self.__txt_chapter_name = Util:WidgetLabel(self:GetWidget("Text_ChapterName"), "", nil, Macros.Color.blinkcontent, nil, nil, Macros.Color.common_stroke)
	self.__img_passed = self:GetWidget("Image_History_Pass")
	self.__panel_extra_map = self:GetWidget("Panel_ExtraMap")

	local bg_hero_shadow = Util:Sprite( Resource.LoadTool.GetImageData( Resource.UITextureAtlasType.DUNGEON, "bg_hero_shadow" ) )
	
	local Image_7 = self:GetWidget("Image_7")
	Image_7:SetContentSize( bg_hero_shadow:GetContentWidth() , bg_hero_shadow:GetContentHeight() )

	self.__txt_extra_map_name = self:GetWidget("Text_ExtraMapName")
	self.__panel_challenged = self:GetWidget("Panel_Challenged")
	self:GetWidget("Image_3"):SetZOrder(1)
	WidgetUnity.SetWidgetListener(self.__panel_extra_map, nil, nil, function()		
		local chapter_info = CallModuleFunc(ModuleType.DUNGEON, "GetChapterInfo", self.__data.index, Macros.Game.FBType.HERO_DUNGEON)
		if not chapter_info then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("dungeon_chapter_unopened"))
			return 
		end
		GlobalEventSystem:FireNextFrame(Macros.Event.DungeonModule.HERO_DUNGEON_GAMELEVEL_CENTER_CHECK_REQ,self.__data.index)
	end)

	--英雄特殊关卡居中检测回调
	self:BindGlobalEvent(Macros.Event.DungeonModule.HERO_DUNGEON_GAMELEVEL_CENTER_CHECK_RESP, function(_,index)
		if index == self.__data.index then
			local chapter_info = CallModuleFunc(ModuleType.DUNGEON, "GetChapterInfo", self.__data.index, Macros.Game.FBType.HERO_DUNGEON)
			if not chapter_info then
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("dungeon_chapter_unopened"))
				return 
			end
			local config_chapter = CPPGameLib.GetConfig("HeroFBChapter", self.__data.index)
			--打开挑战界面		
			Modules.TemplHeroDungeonSpecialGameLevelChallenge.New({
				chapter_index = self.__data.index, 
				gamelevel_index = 0, gamelevel_id = config_chapter.extra_map_id , is_history_passed_all = chapter_info.is_history_passed_all})	
		end
	end)
end

function Modules.TemplHeroDungeonChapter:__delete()
	if self.__hero_module then
		self.__hero_module:DeleteMe()
		self.__hero_module = nil
	end
	if self.__fightting_effect_id then
		self:__DelEffect(self.__fightting_effect_id)
		self.__fightting_effect_id = nil
	end
	if self.__chapter_skeleton_effectid then
		self:__DelEffect(self.__chapter_skeleton_effectid)
		self.__chapter_skeleton_effectid = nil
	end
end

function Modules.TemplHeroDungeonChapter:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	if self.__chapter_skeleton_effectid then
		self:__DelEffect(self.__chapter_skeleton_effectid)
		self.__chapter_skeleton_effectid = nil
	end

	self.__panel_challenged:RemoveAllChild(true)

	local config_chapter = CPPGameLib.GetConfig("HeroFBChapter", data.index)
	local chapter_info = CallModuleFunc(ModuleType.DUNGEON, "GetChapterInfo", data.index, Macros.Game.FBType.HERO_DUNGEON)
	local chapter_list = CallModuleFunc(ModuleType.DUNGEON,"GetChapterList",Macros.Game.FBType.HERO_DUNGEON)
	self.__chapter_skeleton_effectid, effect = self:__CreateEffect({res_name = "UI_chapter_icon"..config_chapter.chapter_icon_id, type = Effect.EffectType.SKELETON, act_index = RenderObj.ActionIndex.IDLE}, self.__root_widget,-1)
	PosTool.Center(effect)
	self.__chapter_skeleton_root = GlobalEffectMgr:GetInnerEffect(self.__chapter_skeleton_effectid)

	local chapter_num = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON, "chapter_num", true),GUI.ImgFont.ImgType.Num1)
	chapter_num:SetAnchorPoint(0.5,0.5)
	self:GetNode():AddChild(chapter_num:GetNode())
	chapter_num:SetText(tostring(data.index))
	PosTool.Center(chapter_num,0,-65)

	--调整文字位置
	local width = chapter_num:GetNode():GetContentWidth()
	local img_chapter_first = self:GetWidget("Image_chapter_first")
	img_chapter_first:SetAnchorPoint(1,0.5)
	img_chapter_first:IgnoreContentAdaptWithSize(true)
	PosTool.Center(img_chapter_first,-(width/2 + 15),-65)
	local img_chapter_num = self:GetWidget("Image_chapternum")
	img_chapter_num:SetAnchorPoint(0,0.5)
	img_chapter_num:IgnoreContentAdaptWithSize(true)
	PosTool.Center(img_chapter_num,(width/2 + 15),-65)
	
	--战斗特效
	if self.__data.index == #chapter_list then
		self.__fightting_effect_id, effect = self:__CreateEffect({res_name = "UI_xiaodao"}, self.__root_widget,2)
		PosTool.Center(effect,0,50)	
	end

	--章节名
	local chapter_res = Resource.PathTool.GetHeroDungeonNamePath(tostring(2000+data.index))
	local chapter_name = Util:Sprite(chapter_res)
	chapter_name:SetAnchorPoint(0.5, 0.5)
	self:GetNode():AddChild(chapter_name)
	PosTool.CenterBottom(chapter_name,0,98)


	--额外副本
	self.__panel_extra_map:SetVisible(config_chapter.extra_map_id > 0)
	if config_chapter.extra_map_id > 0 then
		local config_gamelevel = CPPGameLib.GetConfig("GameLevel", config_chapter.extra_map_id)
		local monster_info = CPPGameLib.GetConfig("Monster",config_gamelevel.icon_id)
		self.__hero_module = LogicGUI.Model.New({info_id = monster_info.hero_id and monster_info.hero_id or monster_info.head_icon, parent = self.__panel_extra_map})
		self.__hero_module:SetScaleFactor(1)
		self.__hero_module:SetPosition(75,0)
		self.__txt_extra_map_name:SetText(config_gamelevel and config_gamelevel.name or "")
		self.__txt_extra_map_name:SetColor(Macros.QualityColor(monster_info.quality))
		self.__txt_extra_map_name:SetStrokeColor(Macros.QualityColorStroke(monster_info.quality))
		if chapter_info and chapter_info.is_history_passed_all and chapter_info.is_extra_map_pass == Macros.Global.FALSE then			
			self.__hero_module:AddUpgradeTips(CPPGameLib.GetString("dungeon_chapter_challenge"))
		end
	end

	if chapter_info then
		--章节名
		self.__txt_chapter_name:SetVisible(false)

		--章节图标
		self.__chapter_skeleton_root:SetColor(unpack(Macros.Color.lilyWhite))

		--通关标记
		self.__img_passed:SetVisible(chapter_info.is_history_passed_all)

		--已通关的关卡标记
		if not chapter_info.is_history_passed_all then
			local sp_x, sp_y
			local sp, sp_w, sp_h, sp_interval, imgdata
			for i = 1, chapter_info.history_total_gamelevel_num do
				imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON, "littlepeople")
				sp_interval = sp_interval or 4
				sp = Util:Sprite(imgdata)
				sp:SetAnchorPoint(0, 0.5)
				sp:SetGray(not (i <= chapter_info.history_passed_gamelevel_num))
				sp_w = sp_w or 0
				self.__panel_challenged:AddChild(sp)
				PosTool.LeftCenter(sp, sp_w, 0)

				sp_w = sp_w + sp:GetContentWidth() + sp_interval
				sp_h = sp_h or sp:GetContentHeight()			
			end
			self.__panel_challenged:SetContentSize(sp_w and sp_w - sp_interval or 0, sp_h or 0)
			PosTool.CenterBottom(self.__panel_challenged, 0, 30)
		end

		--红点
		local chapter_reddot_num = CallModuleFunc(ModuleType.DUNGEON,"GetChapterRedDotNum",Macros.Game.SystemID.HERO_DUNGEON_GAMELEVEL,data.index)
		if chapter_reddot_num > 0 then
			self:__AddRedDot()
		else
			self:__HideRedDot()
		end

		--新手指引特殊操作检测
		self:CheckSectionOperateSP()
		
	else
		--开启条件
		local condition_str
		local pre_chapter_info = CallModuleFunc(ModuleType.DUNGEON, "GetChapterInfo", data.index - 1, Macros.Game.FBType.HERO_DUNGEON)
		if pre_chapter_info then
			if pre_chapter_info.is_history_passed_all then
				--上一个章节已通关，则显示"需通关前置主线章节"
				local pre_main_config_chapter = CPPGameLib.GetConfig("MainFuben", config_chapter.pre_chapter_id)
				condition_str = string.format(CPPGameLib.GetString("dungeon_open_after"), pre_main_config_chapter.name)
			else
				--上一个章节未通关，则显示"需通关上一个章节"
				local pre_config_chapter = CPPGameLib.GetConfig("HeroFBChapter", data.index - 1)
				condition_str = string.format(CPPGameLib.GetString("dungeon_open_after"), pre_config_chapter.name)
			end
		else
			--上一个章节未通关，则显示"需通关上一个章节"
			local pre_config_chapter = CPPGameLib.GetConfig("HeroFBChapter", data.index - 1)
			condition_str = string.format(CPPGameLib.GetString("dungeon_open_after"), pre_config_chapter.name)
		end
		self.__txt_chapter_name:SetText(condition_str)
		PosTool.CenterBottom(self.__txt_chapter_name, 0, 51)
		--艺术字章节名
		chapter_name:SetVisible(false)
		self.__chapter_skeleton_root:SetColor(unpack(Macros.Color.GrayBlack))
		self.__img_passed:SetVisible(false)
	end
end

function Modules.TemplHeroDungeonChapter:IsTouchEnable()
	if not GUI.ScrollItem.IsTouchEnable(self) then
		return false
	end

	local chapter_info = CallModuleFunc(ModuleType.DUNGEON, "GetChapterInfo", self.__data.index, Macros.Game.FBType.HERO_DUNGEON)
	return chapter_info ~= nil
end

--新手指引特殊操作检测(选择主线章节（章节索引，从1开始）)
function Modules.TemplHeroDungeonChapter:CheckSectionOperateSP()
	CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.HERO_DUNGEON_SELECT_CHAPTER, self:GetNode(), self.__data.index)		
end

--添加红点
function Modules.TemplHeroDungeonChapter:__AddRedDot()
	--红点
	if not self.entry_red_dot then
		local entry_red_dot_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
		self.entry_red_dot = SpriteEx.CreateWithImageFile(entry_red_dot_image_data)
		self.entry_red_dot:SetAnchorPoint(1, 1)
		self:GetNode():AddChild(self.entry_red_dot,2)
		PosTool.Center(self.entry_red_dot,60,-30)
	end
	self.entry_red_dot:SetVisible(true)
end

--隐藏红点
function Modules.TemplHeroDungeonChapter:__HideRedDot()
	if self.entry_red_dot then
		self.entry_red_dot:SetVisible(false)
	end	
end

