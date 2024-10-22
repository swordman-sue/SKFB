
--
-- @brief 获取途径条目
-- @author: ldx
-- @date: 2016年10月31日15:17:54
--

Modules = Modules or {}

Modules.TemplApproachOfAchievingCell = Modules.TemplApproachOfAchievingCell or BaseClass(GUI.ScrollItem)

function Modules.TemplApproachOfAchievingCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGCELL)
end

function Modules.TemplApproachOfAchievingCell:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	--图标
	if self.__data.approach_icon_res_name then
		local Panel_itemIcon = self:GetWidget("Panel_itemIcon")
		local icon = UIImageViewEx.CreateWithImage(Resource.PathTool.GetSystemEntryIconPath(self.__data.approach_icon_res_name),TextureResType.LOCAL)
		icon:IgnoreContentAdaptWithSize(false)
		icon:SetContentSize(70,70)
		Panel_itemIcon:AddChild(icon)
		PosTool.Center(icon)
	end
	--名字
	Util:WidgetLabel(self:GetWidget("Text_name"), self.__data.approach_name, Macros.TypefaceSize.normal, Macros.Color.keypoint )
	--描述
	Util:WidgetLabel(self:GetWidget("Text_des"), self.__data.approach_des, Macros.TypefaceSize.slightly , Macros.Color.content )
	--限制次数
	local limitTimes = self:GetWidget("Text_times")
	limitTimes:SetVisible(self.__data.limit_times_str ~= nil)
	Util:WidgetLabel(limitTimes,self.__data.limit_times_str or "", Macros.TypefaceSize.slightly, Macros.Color.keypoint )
	--前往按钮
	local Button_achieving = self:GetWidget("Button_achieving")
	Button_achieving:SetVisible(self.__data.isOpen)
	WidgetUnity.SetWidgetListener(Button_achieving, nil, nil, function ()
		self:FireNextFrame(Macros.Event.ApproachOfAchievingModule.DELETE_APPROACHVIEW)
		
		--声望商店
		if self.__data.approach_systemid == Macros.Game.SystemID.ARENA_REPUTATIONSHOP then
			GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.ARENA_REPUTATIONSHOP)

		elseif self.__data.approach_systemid == Macros.Game.SystemID.UNION_SHOP then
			local role_info = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo")
			if role_info.guild_id == 0 then
				GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.UNION)
			else
				--军团商店
				GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.UNION_SHOP)
			end		
		elseif self.__data.approach_systemid == Macros.Game.APPROACH_SPECIAL_TYPE.MAIN_DUNGEON_ALL_GAMELEVEL then
			--主线副本所有关卡
			local chapter_index,gamelevel_index = CallModuleFunc(ModuleType.DUNGEON,"GetCurDungeonLatestChapterAndGameLevelByType",Macros.Game.FBType.MAIN_DUNGEON) 
			GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.DUNGEON_GAMELEVEL,{chapter_index,gamelevel_index})
		elseif self.__data.approach_systemid == Macros.Game.APPROACH_SPECIAL_TYPE.HERO_DUNGEON_ALL_GAMELEVEL then
			--英雄副本所有关卡
			local chapter_index,gamelevel_index = CallModuleFunc(ModuleType.DUNGEON,"GetCurDungeonLatestChapterAndGameLevelByType",Macros.Game.FBType.HERO_DUNGEON) 
			GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.HERO_DUNGEON_GAMELEVEL,{chapter_index})
		elseif self.__data.approach_systemid == Macros.Game.APPROACH_SPECIAL_TYPE.ELITE_DUNGEON_ALL_GAMELEVEL then
			--精英副本所有关卡
			local chapter_index,gamelevel_index = CallModuleFunc(ModuleType.DUNGEON,"GetCurDungeonLatestChapterAndGameLevelByType",Macros.Game.FBType.ELITE_DUNGEON) 
			GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.DUNGEON_ELITE,{chapter_index,gamelevel_index})	
		else
			GlobalModulesMgr:OpenSystemModule(self.__data.approach_systemid,self.__data.extra_view_params)
		end
			
	end, nil)

	--未开启文字
	local Text_unlocked = self:GetWidget("Text_unlocked")
	Text_unlocked:SetText(CPPGameLib.GetString("spirit_haveNotOpen"))
	Text_unlocked:SetVisible(not self.__data.isOpen and self.__data.approach_id ~= 1002)
end

function Modules.TemplApproachOfAchievingCell:__delete()
	
end