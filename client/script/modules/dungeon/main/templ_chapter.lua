
Modules = Modules or {}

--副本章节模板
Modules.TemplDungeonChapter = Modules.TemplDungeonChapter or BaseClass(GUI.ScrollItem)

function Modules.TemplDungeonChapter:__init()
	self:InitTemplate(GUI.TemplateRegistry.DUNGEON_CHAPTER)	

	self.__panel_box = self:GetWidget("Panel_Box")
	self.__txt_chapter_name = Util:WidgetLabel(self:GetWidget("Text_ChapterName"), "", nil, Macros.Color.blinkcontent, nil, nil, Macros.Color.common_stroke)
	self.__txt_star = Util:WidgetLabel(self:GetWidget("Text_Star"), "", nil, Macros.Color.blinkcontent, nil, nil, Macros.Color.common_stroke)
	self.__img_star = self:GetWidget("Image_Star")

	local widget = self:GetWidget("Panel_ExtraMap")
	widget:SetVisible(false)
	widget = self:GetWidget("Panel_Challenged")
	widget:SetVisible(false)
	self.__img_passed = self:GetWidget("Image_History_Pass")

	--停止浮动
	self:BindGlobalEvent(Macros.Event.DungeonModule.TAKE_MAIN_FB_CHAPTER_REWARD, function(_,protocol_data)
		local reward_info = CPPGameLib.GetConfig("MainFBChapterReward",protocol_data.reward_level)
		if reward_info and reward_info.need_chapter == self.__data.index then
			if self.__bubble_bg then
				CPPActionManager.StopAllActions(self.__bubble_bg)
				self.__bubble_bg:RemoveSelf(true)
				self.__bubble_bg = nil
			end
		end
	end)	
end

function Modules.TemplDungeonChapter:__delete()
	if self.__fightting_effect_id then
		self:__DelEffect(self.__fightting_effect_id)
		self.__fightting_effect_id = nil
	end
	if self.__chapter_skeleton_effectid then
		self:__DelEffect(self.__chapter_skeleton_effectid)
		self.__chapter_skeleton_effectid = nil
	end
end

function Modules.TemplDungeonChapter:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	if self.__chapter_skeleton_effectid then
		self:__DelEffect(self.__chapter_skeleton_effectid)
		self.__chapter_skeleton_effectid = nil
	end
	local config_chapter = CPPGameLib.GetConfig("MainFuben", data.index)
	local chapter_info = CallModuleFunc(ModuleType.DUNGEON, "GetChapterInfo", data.index)
	local chapter_list = CallModuleFunc(ModuleType.DUNGEON,"GetChapterList")
	if not config_chapter then
		return
	end
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
		--开启条件
		self.__txt_chapter_name:SetVisible(false)

		--章节星级
		self.__txt_star:SetText(chapter_info.cur_star .. "/" .. chapter_info.max_star)

		--章节图标
		self.__chapter_skeleton_root:SetColor(unpack(Macros.Color.lilyWhite))	

		--红点 
		chapter_reddot_num = CallModuleFunc(ModuleType.DUNGEON,"GetChapterRedDotNum",Macros.Game.SystemID.DUNGEON_GAMELEVEL,data.index)
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
		if config_chapter.pre_chapter_id then
			local str
			if config_chapter.need_role_level > 0 then
				str = CPPGameLib.GetString("dungeon_open_after2", config_chapter.pre_chapter_id, config_chapter.need_role_level)
			else
				str = CPPGameLib.GetString("dungeon_open_after", config_chapter.pre_chapter_id)
			end
			self.__txt_chapter_name:SetText(str)		
			PosTool.CenterBottom(self.__txt_chapter_name, 0, 51)
		else
			self.__txt_chapter_name:SetVisible(false)
		end

		--艺术字章节名
		chapter_name:SetVisible(false)
		self.__chapter_skeleton_root:SetColor(unpack(Macros.Color.GrayBlack))	
		--通关标记
		self.__img_passed:SetVisible(false)
	end
	self.__panel_box:SetVisible(chapter_info ~= nil and chapter_reddot_num > 0)	
	self.__txt_star:SetVisible(chapter_info ~= nil)
	self.__img_star:SetVisible(chapter_info ~= nil)	
	--查找配置
	local reward_config = CPPGameLib.GetConfig("DungeonStarReward",self.__data.index,false)
	if not self.__panel_box:IsVisible() and reward_config then
		self:CreateBubble(reward_config,chapter_info)
	end
end

function Modules.TemplDungeonChapter:CreateBubble(reward_config,chapter_info)
	local bubble_bg_sp,bubble_txt_sp
	local could_create
	--判断能否创建
	if chapter_info then	
		if reward_config.is_starbox_reward == Macros.Global.TRUE then
			local config_chapter = ConfigAdapter.Dungeon.GetChapterInfo(self.__data.index, Macros.Game.FBType.MAIN_DUNGEON)
			if config_chapter and chapter_info.star_award_list[#config_chapter.star_list] ~= Modules.DungeonModule.BoxStatus.ALREADY_RECEIVE then
				could_create = true
			end
		else
			local dungeonbox_reward_level = CallModuleFunc(ModuleType.DUNGEON,"GetFBPassedBoxRewardLevel")	
			local reward_info = CPPGameLib.GetConfig("MainFBChapterReward",dungeonbox_reward_level,false)
			if dungeonbox_reward_level == 0 or reward_info and self.__data.index > reward_info.need_chapter then
				could_create = true
			end
		end
	else
		could_create = true
	end
	--根据情况创建资源
	if reward_config.is_starbox_reward == Macros.Global.TRUE and could_create then
		bubble_bg_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON,"bg_reward_short")
		local item_type = ObjIDTool.GetInfo(reward_config.equip_id)
		bubble_txt_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON, item_type == 
						Macros.Game.ObjType.EQUIP and "reward_equip_txt" or "reward_treasure_txt",true)
	elseif could_create then
		bubble_bg_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON,"bg_reward_long")
		bubble_txt_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON, reward_config.is_reward_hero == 
						Macros.Global.TRUE and "reward_hero_txt" or "reward_orange_equip_txt",true)
	end
	--符合条件
	if bubble_bg_sp and bubble_txt_sp then
		--泡泡
		self.__bubble_bg = Util:Sprite(bubble_bg_sp)
		self.__bubble_bg:SetCascadeOpacityEnabled(false)
		self:GetNode():AddChild(self.__bubble_bg)
		self.__bubble_bg:SetAnchorPoint(0.5,1)
		PosTool.CenterTop(self.__bubble_bg,0,0)
		--文字
		local bubble_txt = Util:Sprite(bubble_txt_sp)
		self.__bubble_bg:AddChild(bubble_txt)
		PosTool.Center(bubble_txt,0,5)
		--运动
		if chapter_info then
			self.funId = GlobalCallbackMgr:AddCallBackFunc(function ()
				ActionManager.GetInstance():RunJumpByAction(self.__bubble_bg,0.5,0,-20,0,0,GlobalCallbackMgr:AddCallBackFunc(function()
					self:RunAddAction()
				end))	
			end)
			self:RunAddAction()
		end
	end
end

function Modules.TemplDungeonChapter:RunAddAction()
	ActionManager.GetInstance():RunJumpByAction(self.__bubble_bg,0.5,0,20,0,0,self.funId)
end

function Modules.TemplDungeonChapter:IsTouchEnable()
	if not GUI.ScrollItem.IsTouchEnable(self) then
		return false
	end

	local chapter_info = CallModuleFunc(ModuleType.DUNGEON, "GetChapterInfo", self.__data.index)
	return chapter_info ~= nil
end

--新手指引特殊操作检测(选择主线章节（章节索引，从1开始）)
function Modules.TemplDungeonChapter:CheckSectionOperateSP()
	CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.MAIN_DUNGEON_SELECT_CHAPTER, self:GetNode(), self.__data.index)		
end

--添加红点
function Modules.TemplDungeonChapter:__AddRedDot()
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
function Modules.TemplDungeonChapter:__HideRedDot()
	if self.entry_red_dot then
		self.entry_red_dot:SetVisible(false)
	end	
end

--获取
function Modules.TemplDungeonChapter:GetImgNormal()
	return self.__chapter_skeleton_root
end
