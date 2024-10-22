--[[
%% @module: 精英副本关卡界面
%% @author: ldx
%% @created: 2017-05-06
--]]

Modules = Modules or {}

Modules.EliteDungeonGameLevelView = Modules.EliteDungeonGameLevelView or BaseClass(Modules.DungeonGameLevelView)

function Modules.EliteDungeonGameLevelView:__init()
	self.__layout_name = "dungeon_gamelevel"	
	self.__dungeon_type = Macros.Game.FBType.ELITE_DUNGEON
	self.__title_bar_params = 
	{
		title = CPPGameLib.GetString("dungeon_elite_gamelevel"),
		show_stamina = true,
		show_diamond = true,
		show_power = true,
		show_level = true,
		close_func = function()
			if self.__is_first_passed_all or self.__is_shield_touching or self.__is_first_open then
				return
			end
			local top_stack_info = GlobalViewMgr:GetTopStackInfo()
			if not top_stack_info then
				self:CallModuleFunc("OpenView", BaseViewType.DUNGEON, BaseViewSubType.DUNGEON.ELITE, function()
					GlobalViewMgr:ClearStackInfo()
					self:CloseManual(true)
				end)
			else
				self:CloseManual()
			end
		end,
	}

    self.__open_callback = function()
    	self:__OpenCallback()
    end
    self:__ClearPreLoadTemplateList()
    self:__ClearPreLoadTextureAtlas()
    self:__AddPreloadTemplate(GUI.TemplateRegistry.ELITE_DUNGEON_GAMELEVEL)
    self:__AddPreloadTemplate(GUI.TemplateRegistry.DUNGEON_PASSEDALL_BOXTIP)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_COMMON)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DUNGEON_GAMELEVEL)    
end

function Modules.EliteDungeonGameLevelView:__RegistAllEvents()

	self:BindGlobalEvent(Macros.Event.RebelModule.REBEL_DATA_RESP, function()
			self:__InitRebelInfo()
		end)

	self.__init_rebel = true
	local rebel_find_list = CallModuleFunc(ModuleType.REBEL, "GetRebelFindList")
	if not rebel_find_list then						
		--请求叛军数据
		CallModuleFunc(ModuleType.REBEL, "RebelDataReq")
		
	else
		self:__InitRebelInfo()
	end

	--监听宝箱领取响应事件	
	self:BindGlobalEvent(Macros.Event.DungeonModule.DUNGEON_AWARD_GET_RESP, function(_, award_type, index)
		if award_type == Macros.Game.FBRewardType.STAR then
			self:__SetStarboxImg(index)
		end
	end)	

	--监听宝箱领取响应事件	
	self:BindGlobalEvent(Macros.Event.DungeonModule.DUNGEON_ONEKEY_AWARD_GET_RESP, function(_,is_get_award)
		if self.__templ_dungeon_passedall_tip then
			self.__templ_dungeon_passedall_tip:DeleteMe()
			self.__templ_dungeon_passedall_tip = nil			
		end
		if self.__close_back then
			self.__close_back()
			self.__close_back = nil
		end
		if is_get_award then
			self:__OneKeyTakeAward()
		else
			local max_chapter_num = ConfigAdapter.Dungeon.GetChapterNum(self.__dungeon_type)
			if (self.__chapter_index + 1) > max_chapter_num then
				self.__is_first_passed_all = false
				self.__panel_shield:SetVisible(false) 
				if self.__battle_end_func then
					self.__battle_end_func()
				end
			else
				CallModuleFunc(ModuleType.DUNGEON, "OpenView", self.__type, self.__chapter_index + 1,1,nil,nil,true,self.__open_status,self.__battle_end_func)
			end			
		end		
	end)	

	--战队升级事件	
	self:BindGlobalEvent(Macros.Event.RoleModule.LEVEL_CHANGE, function(_,old_level,level)
		self:__InitLineUpAndRankBtn()
	end)	

	--阵容红点
	self:BindGlobalEvent(Macros.Event.RedModule.RED_UPDATE, function()
		self:__UpdateLineupBtnRedDot()
	end)

	--更新副本关卡数据
	self:BindGlobalEvent(Macros.Event.DungeonModule.UPDATE_DUNGEON_GAMELEVEL_DATA, function(_,gamelevel_index)
		self:__UpdateGameLevelData(gamelevel_index)
	end)
	
	self:__InitLevelGiftInfo()
	
	--阵容和排行榜按钮
	self:__InitLineUpAndRankBtn()
end


--初始化等级礼包通知
function Modules.EliteDungeonGameLevelView:__InitLevelGiftInfo()
	local Panel_Levelgift = self:GetWidget("Panel_Levelgift")
	local Button_Levelgift = self:GetWidget("Button_Levelgift")
	Panel_Levelgift:SetVisible(false)
	Util:WidgetButton(Button_Levelgift, nil, function( ... )
		CallModuleFunc(ModuleType.ACTIVITY, "OpenView", BaseViewType.ACTIVITY, Macros.Game.SystemID.LEVEL_GIFTBAG )
	end)
	local num = CallModuleFunc(ModuleType.ACTIVITY, "GetLevelGiftRedDotNum")
	if num ~= 0 then
		ActionManager.GetInstance():AboutRotateAnimation( self:GetWidget("Button_Levelgift")  , 0.1 , 15)
		Panel_Levelgift:SetVisible(true)
	end

	--名字
	local xtName = Util:Label( CPPGameLib.GetString("activity_levelGiftBag"),Macros.TypefaceSize.largish, Macros.Color.btn, 0, 0, Macros.Color.cityblack_stroke)
	xtName:SetAnchorPoint(0.5,0.5)
	xtName:SetPosition( Panel_Levelgift:GetContentWidth()/2 , 10 )
	Panel_Levelgift:AddChild( xtName )
end