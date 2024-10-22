
--[[
%% @module: 战斗失败界面
%% @author: swordman sue
%% @created: 2016-11-16
--]]

Modules = Modules or {}

Modules.BattleResultLoseView = Modules.BattleResultLoseView or BaseClass(BaseView)

function Modules.BattleResultLoseView:__init()
	self.__layout_name = "battle_result_lose"	
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_loading = false

    self.__open_callback = function()
    	self:__OpenCallback()
    end
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.BATTLE_RESULT_COMMON)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.BATTLE_RESULT_LOSE)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.BATTLE_RESULT_LOSE,true)
end

function Modules.BattleResultLoseView:__OpenCallback()
	self:__InitTopInfo()

	self:__InitPowerEntry()

	--指引容器
	self.__panel_continue = self:GetWidget("Panel_Continue")
	self.__panel_continue:SetTouchEnabled(false)
	--点击屏幕继续
	WidgetUnity.SetWidgetListener(self.__root_widget, nil, nil, function()
		self:CloseManual(true)
		self:CallModuleFunc("BattleEndReturnRelevantView")
	end)
	
	Util:WidgetLabel( 	self:GetWidget("Text_Continue"), 
						CPPGameLib.GetString("battle_result_continue"), 
						Macros.TypefaceSize.popup,Macros.Color.btn,nil,nil,
						Macros.Color.white_stroke )

	--重新挑战
	self.__btn_restartchallenge = self:GetWidget("Button_RestartChallenge")
	local battle_info = self:CallModuleFunc("GetCurInfo")
	if battle_info.type == Macros.Game.FBType.MAIN_DUNGEON or
			battle_info.type == Macros.Game.FBType.HERO_DUNGEON  or
			battle_info.type == Macros.Game.FBType.ELITE_DUNGEON then
		self.__btn_restartchallenge:SetVisible(true)	
	end	
	Util:WidgetButton(self.__btn_restartchallenge,"",function ()
		self:CloseManual(true)
		self:CallModuleFunc("BattleEndReturnRelevantView",Macros.Global.FALSE)
	end)
	Util:WidgetLabel(self:GetWidget("Text_RestartChallenge"),CPPGameLib.GetString("battle_result_restart_challenge"), Macros.TypefaceSize.normal, Macros.Color.btn, 0, 0, Macros.Color.button_yellow)

	self.Image_1 = self:GetWidget("Image_1")
	self.Image_1:IgnoreContentAdaptWithSize( true )

end

function Modules.BattleResultLoseView:__Dispose()
	if self.__first_rfecharge_effect_id then
		self:__DelEffect(self.__first_rfecharge_effect_id)
		self.__first_rfecharge_effect_id = nil
	end

	if self.__anliyibo_effect_id then
		self:__DelEffect(self.__anliyibo_effect_id)
		self.__anliyibo_effect_id = nil
	end

	if self.__lead_player_attack_effectid then
		self:__DelEffect(self.__lead_player_attack_effectid)
		self.__lead_player_attack_effectid = nil
	end
end

function Modules.BattleResultLoseView:__InitTopInfo()
	local panel_top_info = self:GetWidget("Panel_TopInfo")

	local battle_info = self:CallModuleFunc("GetCurInfo")

	--爬塔
    -- if battle_info.type == Macros.Game.FBType.TOWER_COMMON or battle_info.type == Macros.Game.FBType.TOWER_ELITE then
    if battle_info.type == Macros.Game.FBType.TOWER_COMMON then
		local tower_game_level = CallModuleFunc(ModuleType.TOWER, "GetBattleMapLevel")
		local config_tower_game_level = CPPGameLib.GetConfig("TowerGameLevel", tower_game_level)
		if config_tower_game_level and config_tower_game_level.win_condition then
			local c_type = config_tower_game_level.win_condition[1]
			local c_param = config_tower_game_level.win_condition[2]
			local failed_tips = CPPGameLib.GetString("battle_tower_failed_tip"..c_type, c_param)
			local lab = Util:Label(failed_tips, nil, Macros.Color.golden)
			panel_top_info:AddChild(lab)
			PosTool.Center(lab)
		end
    end
end

function Modules.BattleResultLoseView:__InitPowerEntry()
	--提升战力提示
	Util:WidgetLabel(self:GetWidget("Text_Tips"), CPPGameLib.GetString("battle_result_lose_tips") , Macros.TypefaceSize.popup )

	--提升战力入口
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	local system_id_list, system_res_list = ConfigAdapter.Battle.GetPowerPromoteEntrys(role_level)
	local entry_layer = {}
	if system_id_list and system_res_list then

		local t1 = {}
		local t2 = {}
		for i , system_id in ipairs( system_id_list ) do
			if #t1 < 3 then
				if system_id ~= Macros.Game.SystemID.FIRST_RECHARGE then
					table.insert( t1 , system_id )
					table.insert( t2 , system_res_list[i] )
				else
					local first_recharge_num = CallModuleFunc(ModuleType.ACTIVITY,"GetFirstRechargeOver")
					if first_recharge_num ~= 0 then
						table.insert( t1 , system_id )
						table.insert( t2 , system_res_list[i] )
					end
				end
			end
		end

		local widget = self:GetWidget("Panel_Promote")

		local num = #t1

		local layout = Util:Layout( 150 * num , 114 )
		layout:SetAnchorPoint( 0.5 , 0.5 )
		widget:AddChild( layout )
		PosTool.Center(layout)


		local interval = layout:GetContentWidth() / #t1
		for i  , system_id in ipairs( t1 ) do
			entry_layer[i] = Modules.PowerPromoteEntryLayer.New()
			entry_layer[i]:SetData(system_id, t2[i])
			layout:AddChild(entry_layer[i]:GetNode())
			entry_layer[i]:SetPosition( (interval/2) + (interval * (i-1)) , layout:GetContentHeight()/2 )

			if system_id ==  Macros.Game.SystemID.FIRST_RECHARGE then
				local effect
				self.__first_rfecharge_effect_id,effect = self:__CreateEffect({res_name = "UI_shouchongtubiao"} , entry_layer[i] , 99 )
				PosTool.Center(effect,0,-6)

				local effect
				self.__anliyibo_effect_id,effect = self:__CreateEffect({res_name = "UI_anliyibo_language_chs"} , entry_layer[i] , 99 )
				PosTool.Center(effect , -20 , -6 )

				--弱指引
				self.__lead_player_attack_effectid,effect = self:__CreateEffect({res_name = "UI_zhiyinshouzhi"},entry_layer[i],200)
				PosTool.Center(effect)
			end
		end
	end

	--新手指引特殊操作检测(战力提升入口)
	if entry_layer[2] then
	    CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.BATTLE_FAILED_POWER_PROMOTE_ENTRY, entry_layer[2]:GetNode())
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--战力提升入口
--------------------------------------------------------------------------------------------------------------------------------
Modules = Modules or {}

Modules.PowerPromoteEntryLayer = Modules.PowerPromoteEntryLayer or BaseClass(UILayer)

function Modules.PowerPromoteEntryLayer:__init()
	self:SetAnchorPoint(0, 0.5)
	self:SetContentSize(84, 114)

	self:SetTouchEnabled(true)
	self:SetTouchFun(nil, nil, function()
		if self.__system_id then
			if self.__system_id == Macros.Game.SystemID.STRONGER then
				CallModuleFunc(ModuleType.STRONGER, "OpenView", BaseViewType.STRONGER , 1 , true)
			else
				GlobalModulesMgr:OpenSystemModule(self.__system_id, nil, false)			
			end
		end
	end)
end

function Modules.PowerPromoteEntryLayer:__delete()
end

function Modules.PowerPromoteEntryLayer:SetData(system_id, system_res)
	self.__system_id = system_id


    local sp = Resource.PathTool.GetSystemEntryIconPath(system_res)
    local text = ""
	if system_id == Macros.Game.SystemID.FIRST_RECHARGE then
	    local t1 = CallModuleFunc(ModuleType.ACTIVITY, "GetFirstRechargeCnf")
	    local max = CallModuleFunc(ModuleType.ACTIVITY, "GetFirstRechargeMAX")
	    local index = 0
	    for i , v in ipairs( t1 ) do
		    if max >= v.context then
		    	index = index + 1
		    end
	    end
	    if index == 0 then
	    	sp = Resource.PathTool.GetSystemEntryIconPath("btn_first_recharge_1" )
	    	text = "_1"
	    elseif index == 1 then
	    	sp = Resource.PathTool.GetSystemEntryIconPath("btn_first_recharge_2")
	    	text = "_2"
	    end
	end

	local icon = Util:Sprite( sp )
	self:AddChild(icon)
	PosTool.Center(icon)

	icon = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE_RESULT_LOSE, "img_lose_"..self.__system_id .. text ,true))
	icon:SetAnchorPoint(0.5, 0)
	self:AddChild(icon)
	PosTool.CenterBottom(icon,0,-5)
end