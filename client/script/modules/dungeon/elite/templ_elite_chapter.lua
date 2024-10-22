Modules = Modules or {}

--副本章节模板
Modules.TemplEliteDungeonChapter = Modules.TemplEliteDungeonChapter or BaseClass(Modules.TemplDungeonChapter)

function Modules.TemplEliteDungeonChapter:__init()
	
end

function Modules.TemplEliteDungeonChapter:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	if self.__chapter_skeleton_effectid then
		self:__DelEffect(self.__chapter_skeleton_effectid)
		self.__chapter_skeleton_effectid = nil
	end

	local config_chapter = CPPGameLib.GetConfig("EliteFBChapter", data.index)
	local chapter_info = CallModuleFunc(ModuleType.DUNGEON, "GetChapterInfo", data.index,Macros.Game.FBType.ELITE_DUNGEON)
	local chapter_list = CallModuleFunc(ModuleType.DUNGEON,"GetChapterList",Macros.Game.FBType.ELITE_DUNGEON)
	self.__chapter_skeleton_effectid, effect = self:__CreateEffect({res_name = "UI_chapter_icon"..config_chapter.chapter_icon_id, type = Effect.EffectType.SKELETON, act_index = RenderObj.ActionIndex.IDLE}, self.__root_widget,-1)
	PosTool.Center(effect)
	self.__chapter_skeleton_root = GlobalEffectMgr:GetInnerEffect(self.__chapter_skeleton_effectid)

	local chapter_num = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON, "chapter_num", true),GUI.ImgFont.ImgType.Num1)
	chapter_num:SetAnchorPoint(0.5,0.5)
	self:GetNode():AddChild(chapter_num:GetNode())
	chapter_num:SetText(tostring(data.index))
	PosTool.Center(chapter_num,0,-65)

	--章节名
	local chapter_res = Resource.PathTool.GetMainDungeonNamePath(tostring(1000+data.index))
	local chapter_name = Util:Sprite(chapter_res)
	chapter_name:SetAnchorPoint(0.5, 0.5)
	self:GetNode():AddChild(chapter_name)
	PosTool.CenterBottom(chapter_name,0,98)

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
	local chapter_reddot_num = 0
	if chapter_info then
		--章节名		
		self.__txt_chapter_name:SetVisible(false)

		--章节星级
		self.__txt_star:SetText(chapter_info.cur_star .. "/" .. chapter_info.max_star)

		--章节图标
		self.__chapter_skeleton_root:SetColor(unpack(Macros.Color.lilyWhite))	

		--红点 
		chapter_reddot_num = CallModuleFunc(ModuleType.DUNGEON,"GetChapterRedDotNum",Macros.Game.SystemID.DUNGEON_ELITE,data.index)
		if chapter_reddot_num > 0 then
			self:__AddRedDot()
		else
			self:__HideRedDot()
		end		
		--通关标记
		self.__img_passed:SetVisible(chapter_info.is_history_passed_all)
		--新手指引特殊操作检测
		self:CheckSectionOperateSP()
	else
		--开启条件
		local config_chapter_pre = CPPGameLib.GetConfig("EliteFBChapter", config_chapter.pre_chapter_id)
		local str = string.format(CPPGameLib.GetString("dungeon_open_after"), config_chapter_pre.name)
		self.__txt_chapter_name:SetText(str)		
		PosTool.CenterBottom(self.__txt_chapter_name, 0, 51)
		--艺术字章节名
		chapter_name:SetVisible(false)
		self.__chapter_skeleton_root:SetColor(unpack(Macros.Color.GrayBlack))	
		--通关标记
		self.__img_passed:SetVisible(false)
	end
	self.__panel_box:SetVisible(chapter_info ~= nil and chapter_reddot_num > 0)
	self.__txt_star:SetVisible(chapter_info ~= nil)
	self.__img_star:SetVisible(chapter_info ~= nil)	
end

function Modules.TemplEliteDungeonChapter:IsTouchEnable()
	if not GUI.ScrollItem.IsTouchEnable(self) then
		return false
	end

	local chapter_info = CallModuleFunc(ModuleType.DUNGEON, "GetChapterInfo", self.__data.index,Macros.Game.FBType.ELITE_DUNGEON)
	return chapter_info ~= nil
end