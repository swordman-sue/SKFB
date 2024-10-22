
Modules = Modules or {}
							
--精英副本外域入侵挑战模板
Modules.TemplExternalAreaChallengeView = Modules.TemplExternalAreaChallengeView or BaseClass(GUI.Template)

function Modules.TemplExternalAreaChallengeView:__init(data)	
	self.__bg_params = {type = BaseView.BGType.SOLID}	

	self:SetData(data)

	self.__is_auto_load = true
	self.__open_callback = function()
		local config_gamelevel = CPPGameLib.GetConfig("GameLevel",self.__data.map_id)
		
		--推荐战力
		local imagebg_battlepower = self:GetWidget("Image_bgBattlePower")
		imagebg_battlepower:SetVisible(false)

		--关闭按钮
		Util:WidgetButton(self:GetWidget("Button_close"), "",function ()
			self:DeleteMe()
		end)
		--关卡名
		Util:WidgetLabel(self:GetWidget("Text_GameLevelName"), config_gamelevel.name, Macros.TypefaceSize.headline, Macros.Color.headline)

		--背景图
		if config_gamelevel.challenge_bg then
			local panel_challenge_bg = self:GetWidget("Panel_ChallengeBG")
			local image_challenge_bg = Util:Sprite(Resource.PathTool.GetUIResPath("dungeon_challenge_bg/"..config_gamelevel.challenge_bg))
			panel_challenge_bg:AddChild(image_challenge_bg)
			PosTool.Center(image_challenge_bg)
		end

		--挑战次数
		self:GetWidget("Text_ChallengeTimesTitle"):SetVisible(false)
		self:GetWidget("Text_ChallengeTimes"):SetVisible(false)
		self:GetWidget("Button_Brush"):SetVisible(false)
		
		--体力、金币、经验
		Util:WidgetLabel(self:GetWidget("Text_GoldTitle"), CPPGameLib.GetString("dungeon_challenge_gold_title"), Macros.TypefaceSize.largish, Macros.Color.keypoint)	
		
		--首胜
		local panel_firstwin = self:GetWidget("Panel_firstWin")
		panel_firstwin:SetVisible(false)

		local cost_currency_map = ConfigAdapter.GameLevel.GetCostCurrencyMap(self.__data.map_id)
		Util:WidgetLabel(self:GetWidget("Text_Stamina"), cost_currency_map[Macros.Game.RolePropType.STAMINA] or 0, Macros.TypefaceSize.headline,Macros.Color.btn,0,0,Macros.Color.button_red)

		local reward_currency_map = Modules.BattleFormula:CalcRewardCurrencyMap(Macros.Game.FBType.ELITE_DUNGEON, self.__data.map_id)	
		Util:WidgetLabel(self:GetWidget("Text_Gold"), reward_currency_map[Macros.Game.RolePropType.GOLD], Macros.TypefaceSize.largish,Macros.Color.content)
		Util:WidgetLabel(self:GetWidget("Text_Exp"), reward_currency_map[Macros.Game.RolePropType.EXP], Macros.TypefaceSize.largish,Macros.Color.content)

		--星级
		for i = 1, Macros.Game.FBMaxStarPerMap do
			self:GetWidget("Image_Star"..i):SetVisible(false)
		end

		--怪物模型
		local panel_monstermodel = self:GetWidget("Panel_monsterModel")
		local monster_info = CPPGameLib.GetConfig("Monster",config_gamelevel.icon_id)
		local icon_id = monster_info.model_type and monster_info.model_id or monster_info.hero_id	
		self.__monster_module = LogicGUI.Model.New({info_id = icon_id, parent = panel_monstermodel})
		self.__monster_module:SetPosition(5,5)
		self.__monster_module:SetScaleFactor(1.2)

		self:__InitDropList()
		self:__InitBottoms()
	end
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_COMMON)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_CHALLENGE)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_CHALLENGE,true)
	self:InitTemplate(GUI.TemplateRegistry.DUNGEON_GAMELEVEL_CHALLENGE, parent_node)	
end

function Modules.TemplExternalAreaChallengeView:__delete()
	if self.__drop_rewards_timer then
		GlobalTimerQuest:CancelQuest(self.__drop_rewards_timer)
		self.__drop_rewards_timer = nil
	end
	if self.__monster_module then
		self.__monster_module:DeleteMe()
		self.__monster_module = nil
	end
end

function Modules.TemplExternalAreaChallengeView:__InitDropList()
	Util:WidgetLabel(self:GetWidget("Text_DropTitle"), CPPGameLib.GetString("dungeon_challenge_drop_title"), Macros.TypefaceSize.largish, Macros.Color.keypoint)

	local scroll_node = self:GetWidget("ScrollView_DropList")
	local item_w = 80
	local item_space = 8
	local offset_x = item_w / 2
	local drop_list = ConfigAdapter.GameLevel.GetHeroDungeonDropList(self.__data.map_id)

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

function Modules.TemplExternalAreaChallengeView:__InitBottoms()
	--挑战文字
	local text_challenge = self:GetWidget("Text_Challenge")
	Util:WidgetLabel(text_challenge,CPPGameLib.GetString("dungeon_challenge"), Macros.TypefaceSize.headline,Macros.Color.btn,0,0,Macros.Color.button_red)
	text_challenge:SetPosition(text_challenge:GetPositionX()+5,text_challenge:GetPositionY())
	--挑战
	local btn_challenge = Util:WidgetButton(self:GetWidget("Button_Challenge"), "", function()
		CallModuleFunc(ModuleType.DUNGEON, "SetEliteExternalDungeonMapId",self.__data.map_id)
		CallModuleFunc(ModuleType.DUNGEON, "HandleGameLevelChallenge", self.__data.chapter_index, 0, Macros.Game.FBType.ELITE_DUNGEON)
	end)		
end


