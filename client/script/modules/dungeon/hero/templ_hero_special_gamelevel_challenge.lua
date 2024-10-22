
Modules = Modules or {}
							
--英雄副本关卡挑战模板
Modules.TemplHeroDungeonSpecialGameLevelChallenge = Modules.TemplHeroDungeonSpecialGameLevelChallenge or BaseClass(GUI.Template)

function Modules.TemplHeroDungeonSpecialGameLevelChallenge:__init(data)	
	self.__bg_params = {type = BaseView.BGType.SOLID}	

	self:SetData(data)

	self.__is_auto_load = true
	self.__open_callback = function()
		local config_gamelevel = CPPGameLib.GetConfig("GameLevel", self.__data.gamelevel_id)
		--章节信息
		local chapter_info = CallModuleFunc(ModuleType.DUNGEON,"GetChapterInfo",self.__data.chapter_index,Macros.Game.FBType.HERO_DUNGEON)

		--关闭按钮
		Util:WidgetButton(self:GetWidget("Button_close"), "",function ()
			self:DeleteMe()
		end)

		--关卡名
		Util:WidgetLabel(self:GetWidget("Text_GameLevelName"), config_gamelevel.name, Macros.TypefaceSize.popup, Macros.Color.headline )
		--怪物模型
		local panel_heroModule = self:GetWidget("Panel_heroModule")
		local monster_info = CPPGameLib.GetConfig("Monster",config_gamelevel.icon_id) 	
		self.__hero_module = LogicGUI.Model.New({info_id = monster_info.hero_id and monster_info.hero_id or monster_info.head_icon, parent = panel_heroModule})
		self.__hero_module:SetScaleFactor(1)
		self.__hero_module:SetPosition(2.5,0)

		--描述
		Util:WidgetLabel(self:GetWidget("Text_des"),config_gamelevel.describe and config_gamelevel.describe or "", Macros.TypefaceSize.normal, Macros.Color.content ,385,90)
		
		--气泡文字
		Util:WidgetLabel(self:GetWidget("Text_talk"),config_gamelevel.chat and config_gamelevel.chat or "", Macros.TypefaceSize.normal, Macros.Color.content ,163,64)
		
		self:__InitDropList()
		self:__InitBottoms()

		--是否已通关
		local text_pass = self:GetWidget("Text_pass")
		Util:WidgetLabel(text_pass,"", Macros.TypefaceSize.normal, Macros.Color.content)
		if chapter_info.is_history_passed_all and chapter_info.is_extra_map_pass == Macros.Global.TRUE then
			self:GetWidget("Button_Challenge"):SetVisible(false)
			text_pass:SetText(CPPGameLib.GetString("dungeon_chapter_passed"))
		elseif chapter_info.is_history_passed_all and chapter_info.is_extra_map_pass == Macros.Global.FALSE then
			text_pass:SetVisible(false)
		else
			self:GetWidget("Button_Challenge"):SetVisible(false)
			local chapter_data = CPPGameLib.GetConfig("HeroFBChapter",self.__data.chapter_index)
			local config_gamelevel = CPPGameLib.GetConfig("GameLevel", chapter_data.gamelevel_list[#chapter_data.gamelevel_list])
			print("ldx",config_gamelevel.name)
			text_pass:SetText(CPPGameLib.GetString("dungeon_chapter_condition",config_gamelevel.name))
		end
	end
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_CHALLENGE)
	self:InitTemplate(GUI.TemplateRegistry.HERO_DUNGEON_SPECIAL_GAMELEVEL_CHALLENGE, parent_node)	
end

function Modules.TemplHeroDungeonSpecialGameLevelChallenge:__delete()
	if self.__drop_rewards_timer then
		GlobalTimerQuest:CancelQuest(self.__drop_rewards_timer)
		self.__drop_rewards_timer = nil
	end
	if self.__hero_module then
		self.__hero_module:DeleteMe()
		self.__hero_module = nil
	end
end

function Modules.TemplHeroDungeonSpecialGameLevelChallenge:__InitDropList()
	--标题		
	Util:WidgetLabel(self:GetWidget("Text_ChallengeTitle"),CPPGameLib.GetString("dungeon_challenge_special_title"), Macros.TypefaceSize.largish, Macros.Color.keypoint)

	local scroll_node = self:GetWidget("ScrollView_DropList")
	local item_w = 80
	local item_space = 12
	local offset_x = item_w / 2
	local drop_list = ConfigAdapter.GameLevel.GetHeroDungeonDropList(self.__data.gamelevel_id)

	local index = 1
	local function create_drop_reward_icon()
		local drop_info = drop_list[index]
		if not drop_info then
			GlobalTimerQuest:CancelQuest(self.__drop_rewards_timer)
			self.__drop_rewards_timer = nil

			offset_x = offset_x - item_w / 2 - item_space
			scroll_node:SetInnerContainerSize(offset_x + 4, 0)
			return
		end
		index = index + 1

		local icon_layer = LogicGUI.IconLayer.New()
		icon_layer:SetData(drop_info)
		icon_layer:SetScaleFactor(0.95)
		scroll_node:AddChild(icon_layer:GetNode())
		PosTool.LeftCenter(icon_layer, offset_x)
		
		offset_x = offset_x + item_w + item_space
	end

	self.__drop_rewards_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
	function()
		create_drop_reward_icon()
	end), 0.08, -1)
end

function Modules.TemplHeroDungeonSpecialGameLevelChallenge:__InitBottoms()
	--挑战
	local btn_challenge = Util:WidgetButton(self:GetWidget("Button_Challenge"),CPPGameLib.GetString("dungeon_challenge"), function()
		CallModuleFunc(ModuleType.DUNGEON, "HandleGameLevelChallenge", self.__data.chapter_index, self.__data.gamelevel_index,Macros.Game.FBType.HERO_DUNGEON)
	end,Macros.TypefaceSize.largish,Macros.Color.btn,Macros.Color.button_yellow)	
	
end


