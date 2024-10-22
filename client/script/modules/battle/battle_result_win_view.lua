
--[[
%% @module: 战斗胜利界面
%% @author: swordman sue
%% @created: 2016-11-16
--]]

Modules = Modules or {}

Modules.BattleResultWinView = Modules.BattleResultWinView or BaseClass(BaseView)

function Modules.BattleResultWinView:__init()
	self.__layout_name = "battle_result_win"	
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_loading = false

    self.__open_callback = function()
    	self:__OpenCallback()
    end

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.BATTLE_RESULT_COMMON)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.BATTLE_RESULT_WIN)    
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.BATTLE_RESULT_WIN, true)   
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.BATTLE, true)    
end

function Modules.BattleResultWinView:__OpenCallback()	
	--指引容器
	self.__panel_continue = self:GetWidget("Panel_Continue")
	self.__panel_continue:SetTouchEnabled(false)
	--点击屏幕继续
	WidgetUnity.SetWidgetListener(self.__root_widget, nil, nil, function()
		if not self.__is_finished then
			return
		end		
		self:CloseManual(true)
		self:CallModuleFunc("BattleEndReturnRelevantView")
	end)
	self.__txt_continue = Util:WidgetLabel(
											self:GetWidget("Text_Continue"), 
											CPPGameLib.GetString("battle_result_continue"), 
											Macros.TypefaceSize.tab,Macros.Color.btn,nil,nil,
											Macros.Color.white_stroke )
	self.__txt_continue:SetVisible(false)

	--重新挑战
	self.__btn_restartchallenge = self:GetWidget("Button_RestartChallenge")
	Util:WidgetButton(self.__btn_restartchallenge,"",function ()
		if not self.__is_finished then
			return
		end

		self:CloseManual(true)
		self:CallModuleFunc("BattleEndReturnRelevantView",Macros.Global.FALSE)
	end)
	Util:WidgetLabel(self:GetWidget("Text_RestartChallenge"),CPPGameLib.GetString("battle_result_restart_challenge"), Macros.TypefaceSize.normal, Macros.Color.btn, 0, 0, Macros.Color.button_yellow)
	
	--下一关
	self.__btn_nextgamelevel = self:GetWidget("Button_NextGameLevel")
	Util:WidgetButton(self.__btn_nextgamelevel,"",function ()
		if not self.__is_finished then
			return
		end

		self:CloseManual(true)
		self:CallModuleFunc("BattleEndReturnRelevantView",Macros.Global.TRUE)
	end)
	Util:WidgetLabel(self:GetWidget("Text_NextGameLevel"),CPPGameLib.GetString("battle_result_next_gameLlevel"), Macros.TypefaceSize.normal, Macros.Color.btn, 0, 0, Macros.Color.button_yellow)

	for i=1,3 do
		self:GetWidget( "Image_Star_bg"..i ):SetGray(true)
	end

	self.Image_1 = self:GetWidget("Image_1")
	self.Image_1:SetVisible(false)

	self.Text_CommonRewardTitle = self:GetWidget("Text_CommonRewardTitle")
	self.Text_CommonRewardTitle:SetVisible(false)
	self.Text_CommonRewardTitle:IgnoreContentAdaptWithSize(true)

	self.Image_3 = self:GetWidget("Image_3")
	self.Image_3:SetVisible(false)

	self.Text_DropRewardTitle = self:GetWidget("Text_DropRewardTitle")
	self.Text_DropRewardTitle:SetVisible(false)
	self.Text_DropRewardTitle:IgnoreContentAdaptWithSize(true)
	self.Text_DropRewardTitle:SetAnchorPoint( 0.5 , 0.5 )
	self.Text_DropRewardTitle:SetPosition( self.Text_CommonRewardTitle:GetPositionX() , self.Text_DropRewardTitle:GetPositionY() )
	
	self.Image_Top = self:GetWidget("Image_Top")
	self.Image_Top:IgnoreContentAdaptWithSize(true)

	self.Image_4_0 = self:GetWidget("Image_4_0")
	self.Image_4_0:IgnoreContentAdaptWithSize(true)

	self.Panel_2 = self:GetWidget("Panel_2")

	--显示特效
	self:__ShowEffect()

	--星级评价
	self:__ShowStarDesc()
end

function Modules.BattleResultWinView:__Dispose()
	CPPGameLib.ClearObjList(self.__drop_icon_list)
	self.__drop_icon_list = nil

	if self.__templ_common_reward then
		self.__templ_common_reward:DeleteMe()
		self.__templ_common_reward = nil
	end

	if self.__drop_rewards_timer then
		GlobalTimerQuest:CancelQuest(self.__drop_rewards_timer)
		self.__drop_rewards_timer = nil
	end
end

--显示特效
function Modules.BattleResultWinView:__ShowEffect()
end

--星级评价
function Modules.BattleResultWinView:__ShowStarDesc()
	local battle_info = self:CallModuleFunc("GetCurInfo")

	--星级评价
	local Text_WinDescTitle = Util:WidgetLabel(
												self:GetWidget("Text_WinDescTitle"), 
												CPPGameLib.GetString("battle_result_star_desc_title"), 
												Macros.TypefaceSize.popup, 
												Macros.Color.golden )

	CPPActionManager.FadeOut(Text_WinDescTitle, 0 ,GlobalCallbackMgr:AddCallBackFunc(function ( ... )
		CPPActionManager.FadeIn(Text_WinDescTitle, 1.8 )
	end) )
	Text_WinDescTitle:SetPosition( Text_WinDescTitle:GetPositionX() , Text_WinDescTitle:GetPositionY() - 20 )

	--星级描述
	local Text_WinDesc = Util:WidgetLabel(
											self:GetWidget("Text_WinDesc"), 
											CPPGameLib.GetString("battle_star_des_"..battle_info.star),
											Macros.TypefaceSize.popup
										)
	Text_WinDesc:SetPosition( Text_WinDesc:GetPositionX() , Text_WinDesc:GetPositionY() - 22 )

	CPPActionManager.FadeOut(Text_WinDesc, 0 ,GlobalCallbackMgr:AddCallBackFunc(function ( ... )

		CPPActionManager.FadeIn(Text_WinDesc, 1.8 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
			self.Image_1:SetVisible(true)

			self.Text_CommonRewardTitle:SetVisible(true)

			local Panel_CommonReward = self:GetWidget("Panel_CommonReward")

			local drop_list = self:CallModuleFunc("GetDropList")
			if #drop_list <= 0 then	
				self.Image_1:SetPosition( self.Image_1:GetPositionX() , self.Image_1:GetPositionY() - 80 )
				self.Text_CommonRewardTitle:SetPosition( self.Text_CommonRewardTitle:GetPositionX() , self.Text_CommonRewardTitle:GetPositionY() - 80 )
				Panel_CommonReward:SetPosition( Panel_CommonReward:GetPositionX() , Panel_CommonReward:GetPositionY() - 80 )
			end

			self:__ShowCommonRewards()

			CPPActionManager.DelayTo(self.Image_3, 0.7, GlobalCallbackMgr:AddCallBackFunc(function()
				self.Image_3:SetVisible(true)

				self.Text_DropRewardTitle:SetVisible(true)

				self:__ShowDropRewards()
			end))

		end) )

	end) )

	local image_star_list = {}
	local effect_star_list = {
					[1] = 100008,
					[2] = 100006,
					[3] = 100007,
	}

	for i = 1, Macros.Game.FBMaxStarPerMap do
		image_star = self:GetWidget("Image_Star"..i)
		image_star:SetVisible(false)
		table.insert(image_star_list, image_star)
	end

	local function Star( ... )
		--星级特效
		local star_index = 0
		local _, star_effect
		local function show_star_effect()
			star_index = star_index + 1
			if star_index > battle_info.star then
				self.__star_desc_finished = true
				self:__CheckFinished()
				return
			end

			_, star_effect = self:__CreateEffect({id = effect_star_list[star_index], loop = false}, self:GetWidget("Image_Star_bg"..star_index))
			PosTool.Center(star_effect)

	        CPPActionManager.DelayTo( image_star_list[star_index] , 0.4 ,GlobalCallbackMgr:AddCallBackFunc( function ( ... )
				image_star_list[star_index]:SetVisible(true)
				show_star_effect()
	        end ) )
		end
		show_star_effect()
	end

	--彩带
	local Image_Top = self:GetWidget("Image_Top")

	Image_Top:SetScaleFactor(1.8)

			CPPActionManager.DelayTo(Image_Top, 0.8 , GlobalCallbackMgr:AddCallBackFunc(function()
				Star()
	  		end))

	CPPActionManager.ScaleTo(Image_Top , 0.16 , 0.9 ,GlobalCallbackMgr:AddCallBackFunc(function ( ... )

		CPPActionManager.ScaleTo(Image_Top , 0.15 , 1)

		local Panel_BGEffect = self:GetWidget("Panel_BGEffect")
		local effectid,effect = self:__CreateEffect({res_name = "UI_zhanduishengji_xuanzhuanguang"}, Panel_BGEffect )
		PosTool.Center(effect)

	end))
end

--通关奖励
function Modules.BattleResultWinView:__ShowCommonRewards( endFun )
	local battle_info = self:CallModuleFunc("GetCurInfo")

	self.__templ_common_reward = Modules.TemplBattleCommonReward.New(
	{
		parent_node = self:GetWidget("Panel_CommonReward"),
		add_gold = battle_info.reward_currency_map[Macros.Game.RolePropType.GOLD] ,
		add_exp = battle_info.reward_currency_map[Macros.Game.RolePropType.EXP] ,
		time = 0.5,
		callback = function()
			self.__common_reward_finished = true
			self:__CheckFinished()

			if endFun then
				endFun()
			end

		end
	})	
	self.__templ_common_reward:SetPosition(-9, 0)
end

--掉落奖励
function Modules.BattleResultWinView:__ShowDropRewards()
	--掉落奖励	
	local icon_width
	local icon_space = 15
	local panel_drop_reward = self:GetWidget("Panel_DropReward")
	local Image_3 = self:GetWidget("Image_3")
	local Text_DropRewardTitle = self:GetWidget("Text_DropRewardTitle")

	local drop_index = 1
	local drop_list = self:CallModuleFunc("GetDropList")

	if #drop_list <= 0 then

		panel_drop_reward:SetVisible(false)
		Image_3:SetVisible(false)
		Text_DropRewardTitle:SetVisible(false)

		local Text_WinDescTitle = self:GetWidget("Text_WinDescTitle"):SetText(" ")
		local Text_WinDesc = self:GetWidget("Text_WinDesc"):SetText(" ")
		
		self.__drop_rewards_timer = nil
		self.__drop_reward_finished = true
		self:__CheckFinished()
				
		return 
	end

	--居中用容器
	local layou = Util:Layout(0,0)
	layou:SetAnchorPoint(0.5,0.5)
	panel_drop_reward:AddChild(layou)
	PosTool.Center( layou )

	--按模板取得宽度
	local sky = LogicGUI.IconLayer.New(false, false)
	sky:SetData(drop_list[1])
	layou:SetContentSize( #drop_list * (sky:GetWidth() + icon_space) , 90 )
	local interval = layou:GetContentWidth() / #drop_list

	self.icon_layer = {}
	local function create_drop_reward_icon()
		if not drop_list then
			return
		end
		local drop_info = drop_list[drop_index]
		if not drop_info then
			return
		end

		self.icon_layer[drop_index] = LogicGUI.IconLayer.New(false, false)
		self.icon_layer[drop_index]:SetAnchorPoint(0.5, 0.5)
		self.icon_layer[drop_index]:SetData(drop_info)

		self.__drop_icon_list = self.__drop_icon_list or {}
		table.insert(self.__drop_icon_list, self.icon_layer[drop_index])
		layou:AddChild(self.icon_layer[drop_index]:GetNode())
		icon_width = self.icon_layer[drop_index]:GetWidth()

		self.icon_layer[drop_index]:SetPosition( 5 + ( (icon_width + icon_space) /2 ) + interval * (drop_index - 1) , layou:GetContentHeight()/2 )

		local num = CallModuleFunc(ModuleType.ACTIVITY,"GetTimedOutput" , Macros.Game.FBType.MAIN_DUNGEON )
		if num ~= 1 then
			local double_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "double" , true )
			local double = Util:Sprite( double_sp )
			double:SetAnchorPoint( 0 , 1 )
			double:SetPosition( 0 , self.icon_layer[drop_index]:GetNode():GetContentHeight() )
			self.icon_layer[drop_index]:AddChild( double )
		end

		self.icon_layer[drop_index]:PlayAppearEffect()
		drop_index = drop_index + 1
	end

	self.__drop_rewards_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
		function()
			create_drop_reward_icon()

			if not drop_list or drop_index > #drop_list then
				GlobalTimerQuest:CancelQuest(self.__drop_rewards_timer)
				self.__drop_rewards_timer = nil

				self.__drop_reward_finished = true
				self:__CheckFinished()
			end		
		end), 0.2 , -1)	
end

--完成检测
function Modules.BattleResultWinView:__CheckFinished()
	if self.__star_desc_finished and self.__common_reward_finished and self.__drop_reward_finished then
		self.__is_finished = true
		self.__txt_continue:SetVisible(true)		
		local battle_info = self:CallModuleFunc("GetCurInfo")
		if battle_info.type == Macros.Game.FBType.MAIN_DUNGEON or
			battle_info.type == Macros.Game.FBType.HERO_DUNGEON  or
			battle_info.type == Macros.Game.FBType.ELITE_DUNGEON then
			local chapter_info = CallModuleFunc(ModuleType.DUNGEON, "GetChapterInfo", battle_info.chapter_index,battle_info.type)
			self.__btn_nextgamelevel:SetVisible(true)
			if chapter_info and chapter_info.is_history_passed_all and battle_info.is_first_win ~= Macros.Global.FALSE then 
				self.__btn_restartchallenge:SetVisible(false)
				if battle_info.type == Macros.Game.FBType.HERO_DUNGEON then
					self.__btn_nextgamelevel:SetVisible(false)
				end
			elseif chapter_info and chapter_info.is_history_passed_all then
				self.__btn_restartchallenge:SetVisible(battle_info.type ~= Macros.Game.FBType.HERO_DUNGEON)
				self.__btn_nextgamelevel:SetVisible(battle_info.type ~= Macros.Game.FBType.HERO_DUNGEON)
			else
				self.__btn_restartchallenge:SetVisible(battle_info.type ~= Macros.Game.FBType.HERO_DUNGEON)
			end						
		end	
		--新手指引特殊操作检测(结算界面-点击继续)
    	CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.BATTLE_RESULT_CONTINUE, self.__panel_continue)
	end
end
