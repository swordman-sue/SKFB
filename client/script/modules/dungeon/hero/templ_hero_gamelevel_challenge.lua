
Modules = Modules or {}
							
--英雄副本关卡挑战模板
Modules.TemplHeroDungeonGameLevelChallenge = Modules.TemplHeroDungeonGameLevelChallenge or BaseClass(GUI.Template)

function Modules.TemplHeroDungeonGameLevelChallenge:__init(data,func)	
	self.__bg_params = {type = BaseView.BGType.SOLID}	

	self:SetData(data)

	self.__is_auto_load = true
	self.__open_callback = function()
		local config_gamelevel = CPPGameLib.GetConfig("GameLevel", self.__data.gamelevel_id)
		print("self.__data.gamelevel_id: ",self.__data.gamelevel_id)
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

		--推荐战力
		self:GetWidget("Image_bgBattlePower"):SetVisible(false)

		--体力、金币、经验
		Util:WidgetLabel(self:GetWidget("Text_GoldTitle"), CPPGameLib.GetString("dungeon_challenge_gold_title"), Macros.TypefaceSize.largish, Macros.Color.keypoint)	
		
		--首胜
		local panel_firstwin = self:GetWidget("Panel_firstWin")
		panel_firstwin:SetVisible(config_gamelevel.first_reward_type_list ~= nil and true or false)
		if config_gamelevel.first_reward_type_list ~= nil then
			self:GetWidget("Image_firstWin"):SetZOrder(1)
			self:GetWidget("Image_firstWin"):IgnoreContentAdaptWithSize(true)
			local icon = LogicGUI.IconLayer.New()
			local icon_info = CPPGameLib.GetConfig("HuoBi",config_gamelevel.first_reward_type_list[1])
			panel_firstwin:AddChild(icon:GetNode())
			PosTool.Center(icon)
			icon:SetData({id = icon_info.big_icon,num = config_gamelevel.first_reward_value_list[1]})
		end

		--已领取首胜标记
		local label = Util:Label(CPPGameLib.GetString("dungeon_chapter_have_getfirstreward"), Macros.TypefaceSize.largish, Macros.Color.btn, 0, 0, Macros.Color.button_red)
		panel_firstwin:AddChild(label,1)
		PosTool.Center(label)
		label:SetVisible(not self.__data.gamelevel_info.no_passed_flag)

		local cost_currency_map = ConfigAdapter.GameLevel.GetCostCurrencyMap(self.__data.gamelevel_id)
		Util:WidgetLabel(self:GetWidget("Text_Stamina"), cost_currency_map[Macros.Game.RolePropType.STAMINA] or 0, Macros.TypefaceSize.headline,Macros.Color.btn,0,0,Macros.Color.button_red)

		local reward_currency_map = Modules.BattleFormula:CalcRewardCurrencyMap(Macros.Game.FBType.HERO_DUNGEON, self.__data.gamelevel_id)	
		Util:WidgetLabel(self:GetWidget("Text_Gold"), reward_currency_map[Macros.Game.RolePropType.GOLD], Macros.TypefaceSize.largish,Macros.Color.content)
		Util:WidgetLabel(self:GetWidget("Text_Exp"), reward_currency_map[Macros.Game.RolePropType.EXP], Macros.TypefaceSize.largish,Macros.Color.content)

		--星级
		for i = 1, Macros.Game.FBMaxStarPerMap do
			self:GetWidget("Image_Star"..i):SetVisible(false)
		end

		--怪物模型
		local panel_monstermodel = self:GetWidget("Panel_monsterModel")
		local monster_info = CPPGameLib.GetConfig("Monster",config_gamelevel.icon_id) 	
		self.__monster_module = LogicGUI.Model.New({info_id = monster_info.hero_id and monster_info.hero_id or monster_info.head_icon, parent = panel_monstermodel})
		self.__monster_module:SetPosition(5,5)
		self.__monster_module:SetScaleFactor(1.2)

		self:__InitDropList()
		self:__InitBottoms()	
		--功能
		if func then
			func()
		end
	end
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_COMMON)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_CHALLENGE)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_CHALLENGE,true)
	self:InitTemplate(GUI.TemplateRegistry.DUNGEON_GAMELEVEL_CHALLENGE, parent_node)	
end

function Modules.TemplHeroDungeonGameLevelChallenge:__delete()
	if self.__drop_rewards_timer then
		GlobalTimerQuest:CancelQuest(self.__drop_rewards_timer)
		self.__drop_rewards_timer = nil
	end
	if self.__monster_module then
		self.__monster_module:DeleteMe()
		self.__monster_module = nil
	end
end

function Modules.TemplHeroDungeonGameLevelChallenge:__InitDropList()
	Util:WidgetLabel(self:GetWidget("Text_DropTitle"), CPPGameLib.GetString("dungeon_challenge_drop_title"), Macros.TypefaceSize.largish, Macros.Color.keypoint)

	local scroll_node = self:GetWidget("ScrollView_DropList")
	local item_w = 80
	local item_space = 8
	local offset_x = item_w / 2
	local drop_list = ConfigAdapter.GameLevel.GetHeroDungeonDropList(self.__data.gamelevel_id)
	--遍历是否有限时掉落
	for _,drop_info in ipairs(CPPGameLib.GetConfig("LimitDrop",nil,nil,true)) do
		--符合副本类型
		if drop_info.type == Macros.Game.FBType.HERO_DUNGEON then
			local data = {}
			data.star_date = drop_info.star_date
			data.over_date = drop_info.over_date
			data.reward_over_date = drop_info.over_date
			data.reward_over_time = 0
			data.star_time = 0
			data.over_time = 0
			data.time_type = 1
			data.need_role_level = 1
			data.days = 1
			local activityCnf = CallModuleFunc(ModuleType.ACTIVITY,"ToTypeSettleActivityType",1,data)
			if activityCnf.judge == "open" then
				local cur_drop_info = {}
				cur_drop_info.id = drop_info.drop_item_id
				if drop_info.drop_num_min ~= drop_info.drop_num_max then
					cur_drop_info.min_num = drop_info.drop_num_min
					cur_drop_info.max_num = drop_info.drop_num_max
				else
					cur_drop_info.num = drop_info.drop_num_min
				end				
				table.insert(drop_list,cur_drop_info)	
			end
		end
	end
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

function Modules.TemplHeroDungeonGameLevelChallenge:__InitBottoms()
	--挑战文字
	local text_challenge = self:GetWidget("Text_Challenge")
	Util:WidgetLabel(text_challenge,CPPGameLib.GetString("dungeon_challenge"), Macros.TypefaceSize.headline,Macros.Color.btn,0,0,Macros.Color.button_red)
	text_challenge:SetPosition(text_challenge:GetPositionX()+5,text_challenge:GetPositionY())
	--挑战
	local btn_challenge = Util:WidgetButton(self:GetWidget("Button_Challenge"), "", function()
		local is_open_spirit
		local could_use_spirit_list = ConfigAdapter.Common.GetCouldUseSpiritDungeonTypeList()
		for i,fb_type in ipairs(could_use_spirit_list) do
			if fb_type == Macros.Game.FBType.HERO_DUNGEON then
				is_open_spirit = true
				break
			end
		end	
		if is_open_spirit and GlobalModulesMgr:IsOpen(Macros.Game.SystemID.FRIEND_ASSIST) then
			CallModuleFunc(ModuleType.BATTLE, "BattleAssistTargetReq", self.__data.chapter_index,self.__data.gamelevel_index,Macros.Game.FBType.HERO_DUNGEON)
			self:DeleteMe()
		else
			CallModuleFunc(ModuleType.DUNGEON, "HandleGameLevelChallenge", self.__data.chapter_index,self.__data.gamelevel_index,Macros.Game.FBType.HERO_DUNGEON)
		end		
	end)	
	
end


