      
--[[
%% @module: 战斗胜利界面
%% @author: swordman sue
%% @created: 2016-12-1
--]]

Modules = Modules or {}

Modules.SpBattleResultWinBase = Modules.SpBattleResultWinBase or BaseClass(BaseView)

function Modules.SpBattleResultWinBase:__init()
	self.__layout_name = "sp_battle_result_win"
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_loading = false

	self.topInfo = true
	self.commonRewards = true
	self.extraRewards = true
	self.dropRewards = true

    self.__open_callback = function()
    	self:__OpenCallback()
    end

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.BATTLE_RESULT_COMMON)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.BATTLE_RESULT_WIN)    
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.BATTLE,true)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.BATTLE_RESULT_WIN, true)    
end

function Modules.SpBattleResultWinBase:__OpenCallback()
	self:SetUpdateFunc(CPPGameLib.Handler(self, self.Update))

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
												Macros.TypefaceSize.popup,
												nil,nil,nil,Macros.Color.white_stroke
												)
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

	self.Image_Top = self:GetWidget("Image_Top")
	self.Image_Top:IgnoreContentAdaptWithSize( true )

	self.Image_TopTitle = self:GetWidget("Image_TopTitle")

	--显示特效
	self:__ShowEffect()

	--上方信息
	self:__ShowTopInfo()
end

function Modules.SpBattleResultWinBase:__Dispose()
	CPPGameLib.ClearObjList(self.__drop_icon_list)
	self.__drop_icon_list = nil

	if self.__templ_common_reward then
		self.__templ_common_reward:DeleteMe()
		self.__templ_common_reward = nil
	end

	if self.__templ_common_reward_1 then
		self.__templ_common_reward_1:DeleteMe()
		self.__templ_common_reward_1 = nil
	end

	if self.__drop_rewards_timer then
		GlobalTimerQuest:CancelQuest(self.__drop_rewards_timer)
		self.__drop_rewards_timer = nil
	end

	self.__num_addition_anim_list = nil
end

function Modules.SpBattleResultWinBase:Update(elapse_time)
	--数值递增动画
	if self.__num_addition_anim_list then
		local factor, new_var, del_list
		for i, info in ipairs(self.__num_addition_anim_list) do
			info.elapse_time = info.elapse_time + elapse_time

			if info.elapse_time >= info.anim_time then
				new_var = info.org_num + info.change_num

				--执行完成回调
				if info.finished_callback then
					info.finished_callback()
					info.finished_callback = nil
				end

				--记录已完成项
				del_list = del_list or {}
				table.insert(del_list, i)
			else
				factor = info.elapse_time / info.anim_time
				new_var = info.org_num + info.change_num * factor
			end
			info.node:SetString(math.floor(new_var))
		end

		--移除已完成项
		if del_list then
			table.sort(del_list, SortTools.ItemUpperSorter())

			for _, del_index in ipairs(del_list) do
				table.remove(self.__num_addition_anim_list, del_index)
			end
		end
	end
end

--显示特效
function Modules.SpBattleResultWinBase:__ShowEffect()
	if not self.Image_Star then
		self.Image_Star = {}
		self.Image_Star_bg = {}
		for i = 1 , 3 do
			self.Image_Star[i] = self:GetWidget("Image_Star" .. i)
			self.Image_Star[i]:SetVisible(false)

			self.Image_Star_bg[i] = self:GetWidget("Image_Star_bg" .. i)
			self.Image_Star_bg[i]:SetVisible(false)
		end
	end
end

--上方信息
function Modules.SpBattleResultWinBase:__ShowTopInfo()
	self.panel_top_info = self:GetWidget("Panel_TopInfo")

	--彩带
	self.Image_Top = self:GetWidget("Image_Top")
	self.Image_Top:SetScaleFactor(1.8)
	CPPActionManager.ScaleTo(self.Image_Top , 0.16 , 0.9 ,GlobalCallbackMgr:AddCallBackFunc(function ( ... )
		CPPActionManager.ScaleTo(self.Image_Top , 0.15 , 1)
		local Panel_BGEffect = self:GetWidget("Panel_BGEffect")
		local effectid,effect = self:__CreateEffect({res_name = "UI_zhanduishengji_xuanzhuanguang"}, Panel_BGEffect )
		PosTool.Center(effect)
	end))

	--战斗奖励
	self.panel_common_reward = self:GetWidget("Panel_CommonReward")
	self.Text_CommonRewardTitle = self:GetWidget("Text_CommonRewardTitle")
	self.Text_CommonRewardTitle:IgnoreContentAdaptWithSize(true)

	self.Text_SpecialRewardTitle = self:GetWidget("Text_SpecialRewardTitle")
	self.Text_SpecialRewardTitle:IgnoreContentAdaptWithSize(true)
	self.Text_SpecialRewardTitle:SetAnchorPoint( 0.5 , 0.5 )
	self.Text_SpecialRewardTitle:SetPosition( self.Text_CommonRewardTitle:GetPositionX() , self.Text_SpecialRewardTitle:GetPositionY() )

	self.Image_1 = self:GetWidget("Image_1")

	local image_top_title = self:GetWidget("Image_TopTitle")
	image_top_title:IgnoreContentAdaptWithSize(true)
end

function Modules.SpBattleResultWinBase:CommonRewardFinishedCallback( end_callback )
	self.__common_reward_finished = true
	self:__CheckFinished()

	if end_callback then
		end_callback()
	end
end

--战斗奖励
function Modules.SpBattleResultWinBase:__ShowCommonRewards(end_callback)
	self.panel_common_reward = self:GetWidget("Panel_CommonReward")
	self.Text_CommonRewardTitle = self:GetWidget("Text_CommonRewardTitle")
	self.Image_1 = self:GetWidget("Image_1")
end

--额外奖励
function Modules.SpBattleResultWinBase:__ShowExtraRewards()
	self.panel_extra_reward = self:GetWidget("Panel_SpecialReward")
	self.Text_SpecialRewardTitle = self:GetWidget("Text_SpecialRewardTitle")
	self.Image_2 = self:GetWidget("Image_2")
end

--掉落奖励
function Modules.SpBattleResultWinBase:__ShowDropRewards(panel_drop_reward, drop_list)

	if self.dropRewards ~= true then
		return
	end
	self.dropRewards = false

	--掉落奖励	
	local icon_width
	local icon_space = 15

	local drop_index = 1
	drop_list = drop_list or self:CallModuleFunc("GetDropList")

	local layer = Util:Layout( 0, panel_drop_reward:GetContentHeight() )
	layer:SetAnchorPoint(0.5,0.5)
	panel_drop_reward:AddChild(layer)
	PosTool.Center( layer )

	local noe = panel_drop_reward:GetContentWidth()/5
	layer:SetContentSize( noe * #drop_list , panel_drop_reward:GetContentHeight() )
	local interval = layer:GetContentWidth() / #drop_list

	--双倍状态
	local function NewDouble( icon )
		local double_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "double" , true )
		local double = Util:Sprite( double_sp )
		double:SetAnchorPoint( 0 , 1 )
		double:SetPosition( 0 , icon:GetNode():GetContentHeight() )
		icon:AddChild( double )
	end

	local function create_drop_reward_icon()
		if not drop_list or #drop_list <= 0 then
			return
		end
		local drop_info = drop_list[drop_index]
		if not drop_info then
			return
		end

		local icon_layer = LogicGUI.IconLayer.New(false, false)
		icon_layer:SetAnchorPoint( 0.5 , 0.5)
		icon_layer:SetData(drop_info)
		icon_layer:PlayAppearEffect()
		self.__drop_icon_list = self.__drop_icon_list or {}
		table.insert(self.__drop_icon_list, icon_layer)

		layer:AddChild(icon_layer:GetNode())
		icon_layer:SetPosition( (noe/2) + interval * (drop_index - 1) , layer:GetContentHeight()/2 )

		local battle_info = self:CallModuleFunc("GetCurInfo")
		
		--如果竞技场
		if battle_info.type == Macros.Game.FBType.ARENA and CallModuleFunc(ModuleType.ACTIVITY,"GetTimedOutput" , Macros.Game.FBType.ARENA ) ~= 1 then
			NewDouble( icon_layer )
		--如果爬塔
		elseif battle_info.type == Macros.Game.FBType.TOWER_COMMON and CallModuleFunc(ModuleType.ACTIVITY,"GetTimedOutput" , Macros.Game.FBType.TOWER_COMMON ) ~= 1 then
			NewDouble( icon_layer )
		end

		drop_index = drop_index + 1
	end

	self.__drop_rewards_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
		function()
			create_drop_reward_icon()

			if not drop_list or drop_index > #drop_list then
				GlobalTimerQuest:CancelQuest(self.__drop_rewards_timer)
				self.__drop_rewards_timer = nil

				self.__extra_reward_finished = true
				self:__CheckFinished()
			end			
		end), 0.3, -1)	
end

--添加数值递增动画
function Modules.SpBattleResultWinBase:__AddNumAdditionAnim(node, org_num, change_num, anim_time, finished_callback)
	self.__num_addition_anim_list = self.__num_addition_anim_list or {}

	local info = {}
	info.node = node
	info.org_num = org_num
	info.change_num = change_num
	info.anim_time = anim_time
	info.elapse_time = 0
	info.finished_callback = finished_callback
	table.insert(self.__num_addition_anim_list, info)
end

--完成检测
function Modules.SpBattleResultWinBase:__CheckFinished()
	if self.__common_reward_finished and self.__extra_reward_finished then
		self.__is_finished = true
		self.__txt_continue:SetVisible(true)
		local battle_info = self:CallModuleFunc("GetCurInfo")	
		if battle_info.type == Macros.Game.FBType.HERO_DUNGEON or
			battle_info.type == Macros.Game.FBType.ELITE_DUNGEON then
			local chapter_info = CallModuleFunc(ModuleType.DUNGEON, "GetChapterInfo", battle_info.chapter_index,battle_info.type)				
			if battle_info.gamelevel_index ~= Macros.Global.FALSE then
				if chapter_info and chapter_info.is_history_passed_all and battle_info.is_first_win ~= Macros.Global.FALSE then
					self.__btn_restartchallenge:SetVisible(false)
					self.__btn_nextgamelevel:SetVisible(battle_info.type ~= Macros.Game.FBType.HERO_DUNGEON)
				elseif chapter_info and chapter_info.is_history_passed_all then
					self.__btn_restartchallenge:SetVisible(battle_info.type ~= Macros.Game.FBType.HERO_DUNGEON)
					self.__btn_nextgamelevel:SetVisible(battle_info.type ~= Macros.Game.FBType.HERO_DUNGEON)
				else
					self.__btn_restartchallenge:SetVisible(battle_info.type ~= Macros.Game.FBType.HERO_DUNGEON)
					self.__btn_nextgamelevel:SetVisible(true)
				end	
			end		
		end		
	end
end

--星级评价
function Modules.SpBattleResultWinBase:__ShowStarDesc()
	local battle_info = self:CallModuleFunc("GetCurInfo")
	local image_star_list = {}
	local effect_star_list = {
					[1] = 100008,
					[2] = 100006,
					[3] = 100007,
	}
	for i = 1, Macros.Game.FBMaxStarPerMap do
		image_star = self.Image_Star[i]
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
				return
			end
			_, star_effect = self:__CreateEffect({id = effect_star_list[star_index], loop = false}, self.Image_Star_bg[star_index])
			PosTool.Center(star_effect)

	        CPPActionManager.DelayTo( image_star_list[star_index] , 0.4 ,GlobalCallbackMgr:AddCallBackFunc( function ( ... )
				image_star_list[star_index]:SetVisible(true)
				show_star_effect()
	        end ) )
		end
		show_star_effect()
	end

	Star()
end