
Modules = Modules or {}

--主线副本关卡挑战模板
Modules.TemplDungeonGameLevelChallenge = Modules.TemplDungeonGameLevelChallenge or BaseClass(GUI.Template)

function Modules.TemplDungeonGameLevelChallenge:__init(data,func)	
	self.__bg_params = {type = BaseView.BGType.SOLID}	

	self:SetData(data)

	self.__is_auto_load = true
	self.__open_callback = function()
		local config_gamelevel = CPPGameLib.GetConfig("GameLevel", self.__data.gamelevel_id)

		--挑战容器
		self.__panel_challenge = self:GetWidget("Panel_challenge")

		--关闭按钮
		Util:WidgetButton(self:GetWidget("Button_close"), "",function ()
			--取消屏蔽关卡界面点击
			--self:FireNextFrame(Macros.Event.DungeonModule.SHIELD_GAMELEVEL_TOUCH,false)
			self:DeleteMe()
		end)

		--关闭界面事件
		self.__DeleteApproachViewEvent = self:BindGlobalEvent(Macros.Event.DungeonModule.DUNGEON_CHALLANGE_CLOSE, function()
	        --取消屏蔽关卡界面点击
			--self:FireNextFrame(Macros.Event.DungeonModule.SHIELD_GAMELEVEL_TOUCH,false)
	        self:DeleteMe()
	    end)

		--关卡名
		Util:WidgetLabel(self:GetWidget("Text_GameLevelName"), config_gamelevel.name or CPPGameLib.GetString("dungeon_normal_gamelevel"), Macros.TypefaceSize.headline, Macros.Color.headline )

		--背景图
		if config_gamelevel.challenge_bg then
			local panel_challenge_bg = self:GetWidget("Panel_ChallengeBG")
			local image_challenge_bg = Util:Sprite(Resource.PathTool.GetUIResPath("dungeon_challenge_bg/"..config_gamelevel.challenge_bg))
			panel_challenge_bg:AddChild(image_challenge_bg)
			PosTool.Center(image_challenge_bg)
		end

		--挑战次数
		local str_battle_times = string.format("%d/%d", config_gamelevel.battle_times_max - self.__data.gamelevel_info.battle_times, config_gamelevel.battle_times_max)
		Util:WidgetLabel(self:GetWidget("Text_ChallengeTimesTitle"), CPPGameLib.GetString("dungeon_challenge_times_title"), Macros.TypefaceSize.largish, Macros.Color.keypoint)
		Util:WidgetLabel(self:GetWidget("Text_ChallengeTimes"), str_battle_times, Macros.TypefaceSize.largish, Macros.Color.content)

		--体力、金币、经验
		Util:WidgetLabel(self:GetWidget("Text_GoldTitle"), CPPGameLib.GetString("dungeon_challenge_gold_title"), Macros.TypefaceSize.largish, Macros.Color.keypoint)		

		local cost_currency_map = ConfigAdapter.GameLevel.GetCostCurrencyMap(self.__data.gamelevel_id)
		Util:WidgetLabel(self:GetWidget("Text_Stamina"), cost_currency_map[Macros.Game.RolePropType.STAMINA] or 0, Macros.TypefaceSize.headline,Macros.Color.btn,0,0,Macros.Color.button_red)

		local reward_currency_map = Modules.BattleFormula:CalcRewardCurrencyMap(self.__data.dungeon_type, self.__data.gamelevel_id)	
		Util:WidgetLabel(self:GetWidget("Text_Gold"), reward_currency_map[Macros.Game.RolePropType.GOLD] or 0, Macros.TypefaceSize.largish,Macros.Color.content)
		Util:WidgetLabel(self:GetWidget("Text_Exp"), reward_currency_map[Macros.Game.RolePropType.EXP] or 0, Macros.TypefaceSize.largish,Macros.Color.content)

		--首胜
		self:GetWidget("Panel_firstWin"):SetVisible(false)

		--星级
		for i = 1, Macros.Game.FBMaxStarPerMap do
			widget = self:GetWidget("Image_Star"..i)
			widget:SetGray(self.__data.gamelevel_info.star < i)
		end

		local imagebg_battlepower = self:GetWidget("Image_bgBattlePower")		 		
		--推荐战力
		if self.__data.dungeon_type == Macros.Game.FBType.MAIN_DUNGEON and config_gamelevel.icon_type ~= 1 then
			--推荐战力文字
			local text_recommended = self:GetWidget("Text_recommended")
			Util:WidgetLabel(text_recommended,CPPGameLib.GetString("dungeon_challenge_recommended_power"), Macros.TypefaceSize.normal,Macros.Color.Special)
			--推荐战力数值
			local text_recommendedpower = self:GetWidget("Text_recommendedPower")
			local recommended_power = config_gamelevel.recommended_power or 0
			local role_battle_value = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.BATTLE_VALUE)
			Util:WidgetLabel(text_recommendedpower,Util:NumTransform(recommended_power), Macros.TypefaceSize.normal,recommended_power > role_battle_value and Macros.Color.red or Macros.Color.green,0,0,recommended_power > role_battle_value and Macros.Color.red_stroke or Macros.Color.green_stroke)
		else
			imagebg_battlepower:SetVisible(false)
		end
		--怪物模型
		local panel_monstermodel = self:GetWidget("Panel_monsterModel")
		local monster_info = CPPGameLib.GetConfig("Monster",config_gamelevel.icon_id) 
		local cur_id 
		local cur_model_type = LogicGUI.ModelType.HERO
		if monster_info.hero_id then
			cur_id = monster_info.hero_id			
		elseif monster_info.model_id then
			cur_model_type = monster_info.model_type == "monster" and LogicGUI.ModelType.MONSTER or LogicGUI.ModelType.HERO
			cur_id = cur_model_type == LogicGUI.ModelType.MONSTER and config_gamelevel.icon_id or monster_info.model_id
		else
		end
		self.__monster_module = LogicGUI.Model.New({info_id = cur_id, parent = panel_monstermodel,model_type = cur_model_type})
		self.__monster_module:SetPosition(5,5)
		local config_hero = CPPGameLib.GetConfig(cur_model_type == LogicGUI.ModelType.MONSTER and "Monster" or "Hero",cur_id) 
		local show_scale = config_hero and config_hero.show_scale or 1
		self.__monster_module:SetScaleFactor(show_scale)

		self:__InitDropList()
		self:__InitBottoms()
		--回调
		if func then
			func()
		end
	end
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_COMMON)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_CHALLENGE)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_CHALLENGE,true)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.DUNGEON_MESSAGE_CELL)	
	self:InitTemplate(GUI.TemplateRegistry.DUNGEON_GAMELEVEL_CHALLENGE, parent_node)	
end

function Modules.TemplDungeonGameLevelChallenge:__delete()	
	if self.__drop_rewards_timer then
		GlobalTimerQuest:CancelQuest(self.__drop_rewards_timer)
		self.__drop_rewards_timer = nil
	end
	if self.__monster_module then
		self.__monster_module:DeleteMe()
		self.__monster_module = nil
	end
end

function Modules.TemplDungeonGameLevelChallenge:__RegistAllEvents()    
	-- 重置
    self:BindGlobalEvent(Macros.Event.DungeonModule.DUNGEON_MAINLINE_RESET,function (__,data)
    	GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("dungeon_resetSucceed") )
    	--取消屏蔽关卡界面点击
		--self:FireNextFrame(Macros.Event.DungeonModule.SHIELD_GAMELEVEL_TOUCH,false)
    	self:DeleteMe()
    end)
end

function Modules.TemplDungeonGameLevelChallenge:__InitDropList()
	Util:WidgetLabel(self:GetWidget("Text_DropTitle"), CPPGameLib.GetString("dungeon_challenge_drop_title"), Macros.TypefaceSize.largish, Macros.Color.keypoint)

	local scroll_node = self:GetWidget("ScrollView_DropList")
	local item_w = 80
	local item_space = 8
	local offset_x = item_w / 2
	local drop_list = ConfigAdapter.GameLevel.GetDropList(self.__data.gamelevel_id)
	--遍历是否有限时掉落
	for _,drop_info in ipairs(CPPGameLib.GetConfig("LimitDrop",nil,nil,true)) do
		--符合副本类型
		if drop_info.type == self.__data.dungeon_type then
			CPPGameLib.PrintTable("ldx",drop_info,"drop_info55666  ")
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

function Modules.TemplDungeonGameLevelChallenge:__InitBottoms()
	--挑战文字
	local txt_challenge = self:GetWidget("Text_Challenge")
	Util:WidgetLabel(txt_challenge,CPPGameLib.GetString("dungeon_challenge"), Macros.TypefaceSize.headline,Macros.Color.btn,0,0,Macros.Color.button_red)
	--挑战
	local btn_challenge = Util:WidgetButton(self:GetWidget("Button_Challenge"),"", function()	
		local is_open_spirit
		local could_use_spirit_list = ConfigAdapter.Common.GetCouldUseSpiritDungeonTypeList()
		for i,fb_type in ipairs(could_use_spirit_list) do
			if fb_type == self.__data.dungeon_type then
				is_open_spirit = true
				break
			end
		end	
		if is_open_spirit then
			if GlobalModulesMgr:IsOpen(Macros.Game.SystemID.FRIEND_ASSIST) then
				--请求助战目标
				CallModuleFunc(ModuleType.BATTLE, "BattleAssistTargetReq", self.__data.chapter_index, self.__data.gamelevel_index,self.__data.dungeon_type)
				self:DeleteMe()
			else
				CallModuleFunc(ModuleType.DUNGEON, "HandleGameLevelChallenge", self.__data.chapter_index, self.__data.gamelevel_index,self.__data.dungeon_type)
			end
		else
			CallModuleFunc(ModuleType.DUNGEON, "HandleGameLevelChallenge", self.__data.chapter_index, self.__data.gamelevel_index,self.__data.dungeon_type)
		end		
	end)	

	--扫荡
	local btn_clean = Util:WidgetButton( self:GetWidget("Button_Brush") , " " , function( ... )
	end,Macros.TypefaceSize.largish,Macros.Color.btn,Macros.Color.button_yellow) 

	local offset_y = 12

	--扫荡条件
	local is_open = CallModuleFunc(ModuleType.DUNGEON, "CheckGameLevelCleanOpenCondition", self.__data.chapter_index, self.__data.gamelevel_index,self.__data.dungeon_type)
	btn_clean:SetVisible(is_open)
	btn_clean:SetBright(is_open)

	if not is_open then
		local system_info = CPPGameLib.GetConfig("SystemEntry",Macros.Game.SystemID.DUNGEON_CLEAN_UP)
		local is_open,is_pre_open = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.DUNGEON_CLEAN_UP)
		btn_clean:SetVisible(is_open or is_pre_open)
		btn_clean:SetText(CPPGameLib.GetString("dungeon_clean_title"))
		WidgetUnity.SetWidgetListener(btn_clean, nil, nil, function()
			local vip_level = CallModuleFunc(ModuleType.VIP,"GetToVipOpen",Macros.Game.VIPType.OPEN_MAIN_DUNGEON_CLEAN)
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("dungeon_open_condition",system_info.level,vip_level))
		end)
	else
		--扫荡次数显示 (章节 关卡)
		-- 扫荡次数 扫荡需要的精力（？） 可以挑战的战斗次数
		local clean_times, times_by_stamina, times_by_battle_times = CallModuleFunc(ModuleType.DUNGEON, "GetGameLevelCanCleanTimes", self.__data.chapter_index, self.__data.gamelevel_index,self.__data.dungeon_type)
		if clean_times > 0 and is_open then
			btn_clean:SetText(CPPGameLib.GetString("dungeon_clean_times", clean_times))
			WidgetUnity.SetWidgetListener(btn_clean, nil, nil, function()
				-- if role_level < system_info.level then
				-- 	GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("common_open_on_role_level",system_info.level))
				-- 	return 
				-- end
				if CallModuleFunc(ModuleType.DUNGEON, "HandleGameLevelClean", self.__data.chapter_index, self.__data.gamelevel_index,self.__data.dungeon_type) then
					--取消屏蔽关卡界面点击
					--self:FireNextFrame(Macros.Event.DungeonModule.SHIELD_GAMELEVEL_TOUCH,false)
					self:DeleteMe()
				end
			end)
		else
			--挑战次数不足，显示“重置”
			if times_by_battle_times <= 0 then
				btn_clean:SetText(CPPGameLib.GetString("dungeon_reset"))
				btn_clean:SetVisible(true)
				WidgetUnity.SetWidgetListener(btn_clean, nil, nil, function()
					local gold , may = CallModuleFunc(ModuleType.DUNGEON, "GetRestTimes",self.__data.gamelevel_info,self.__data.dungeon_type)
					if may ~= 0 then
						--重置次数
						self:Feasible()
					else
						--没有次数
						self:NoFeasible()	
					end
				end)
			--体力不足，显示可挑战次数
			else
				clean_times = math.min(times_by_battle_times, ConfigAdapter.Common.GetMainDungeonCleanDefaultTimes())
				btn_clean:SetText(CPPGameLib.GetString("dungeon_clean_times", clean_times))
				WidgetUnity.SetWidgetListener(btn_clean, nil, nil, function()
					if CallModuleFunc(ModuleType.DUNGEON, "HandleGameLevelClean", self.__data.chapter_index, self.__data.gamelevel_index,self.__data.dungeon_type) then
						--取消屏蔽关卡界面点击
						--self:FireNextFrame(Macros.Event.DungeonModule.SHIELD_GAMELEVEL_TOUCH,false)
						self:DeleteMe()
					end
				end)
			end
		end
	end	
end

function Modules.TemplDungeonGameLevelChallenge:Feasible()
	local gold , may = CallModuleFunc(ModuleType.DUNGEON, "GetRestTimes",self.__data.gamelevel_info,self.__data.dungeon_type)
	local userGold = CallModuleFunc(ModuleType.ROLE , "GetPropValue", Macros.Game.RolePropType.DIAMOND )
	local tips = LogicGUI.TemplAffirmTips.New( { 
													type = LogicGUI.TemplAffirmTipsType.YES_NO,
													content	= " ",
													cancel_text = CPPGameLib.GetString("common_cancel") ,
													confirm_func = function ( ... )
														if gold <= userGold then
															local info = {}
															info.map_type = self.__data.dungeon_type
															info.param1 = self.__data.chapter_index
															info.param2 = self.__data.gamelevel_index
															CallModuleFunc(ModuleType.DUNGEON, "FBResetReq",info)
														else
															GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("currency_insufficient_" .. Macros.Game.RolePropType.DIAMOND) )
														end
													end,
													cancel_func = function ( ... )
														print("----2")
													end,
												} )
	tips:SetLoadedCallback( function ( ... )
		local text = 
			{
				{ zi = CPPGameLib.GetString("dungeon_resetTips",gold), ys = Macros.Color.content , dx = Macros.TypefaceSize.largish,jg = -70},
				{ tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.DIAMOND),dx = 0.8,jg = -140},
			}
		local Affirm = Util:LabCenter(text)
		Affirm:SetPosition(tips:GetNode():GetContentWidth()/2 , tips:GetNode():GetContentHeight()/2 + 40 )
		tips:GetNode():AddChild(Affirm)

		local arena_today_buyTimes = Util:Label( CPPGameLib.GetString("tower_buyTimes",may) , Macros.TypefaceSize.normal , Macros.Color.red )
		arena_today_buyTimes:SetAnchorPoint(0.5,0.5)
		arena_today_buyTimes:SetPosition(tips:GetNode():GetContentWidth()/2 , tips:GetNode():GetContentHeight()/2 - 40 )
		tips:GetNode():AddChild(arena_today_buyTimes)
	end)
end

function Modules.TemplDungeonGameLevelChallenge:NoFeasible()
	local userGold = CallModuleFunc(ModuleType.ROLE , "GetPropValue", Macros.Game.RolePropType.DIAMOND )

	local tips = LogicGUI.TemplAffirmTips.New( { 
													type = LogicGUI.TemplAffirmTipsType.YES_NO,
													content	= " ",
													confirm_text = CPPGameLib.GetString("common_pay") ,
													confirm_func = function ( ... )
														print("----1")
													end,
													cancel_text = CPPGameLib.GetString("common_noPay") ,
													cancel_func = function ( ... )
														print("----2")
													end,
												} )
	tips:SetLoadedCallback(function()

		local dungeon_resetMax = Util:Label( CPPGameLib.GetString("dungeon_resetMax") , Macros.TypefaceSize.largish , Macros.Color.content )
		dungeon_resetMax:SetPosition( tips:GetNode():GetContentWidth()/2 , (tips:GetNode():GetContentHeight()/2) + (dungeon_resetMax:GetContentHeight()/2 ) + 2 )
		tips:GetNode():AddChild(dungeon_resetMax)

		local dungeon_resetVipTips = Util:Label( CallModuleFunc(ModuleType.DUNGEON, "GetRestNewVipInfo",self.__data.dungeon_type) , Macros.TypefaceSize.largish , Macros.Color.content )
		dungeon_resetVipTips:SetPosition(tips:GetNode():GetContentWidth()/2 , (tips:GetNode():GetContentHeight()/2) - (dungeon_resetVipTips:GetContentHeight()/2) - 2 )
		tips:GetNode():AddChild(dungeon_resetVipTips)
	end)

end