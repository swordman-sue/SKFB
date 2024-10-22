require "modules/battle/battle_win/sp_battle_result_win_base"

--[[
%% @module: 战斗失败界面
%% @author: swordman sue
%% @created: 2016-12-1
--]]

Modules = Modules or {}

Modules.SpBattleResultLoseBase = Modules.SpBattleResultLoseBase or BaseClass(BaseView)

function Modules.SpBattleResultLoseBase:__init()
	self.__layout_name = "sp_battle_result_lose"	
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_loading = false

    self.__open_callback = function()
    	self:__OpenCallback()
    end

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.BATTLE_RESULT_COMMON)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.BATTLE_RESULT_LOSE)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.BATTLE_RESULT_LOSE,true)
	
end

function Modules.SpBattleResultLoseBase:__OpenCallback()
	self:SetUpdateFunc(CPPGameLib.Handler(self, self.Update))

	self:__InitPowerEntry()

	--指引容器
	self.__panel_continue = self:GetWidget("Panel_Continue")
	self.__panel_continue:SetTouchEnabled(false)
	--点击屏幕继续
	WidgetUnity.SetWidgetListener(self.__root_widget, nil, nil, function()
		self:CloseManual(true)
		self:CallModuleFunc("BattleEndReturnRelevantView")
	end)
	self.__txt_continue = Util:WidgetLabel(
											self:GetWidget("Text_Continue"), 
											CPPGameLib.GetString("battle_result_continue"), 
											Macros.TypefaceSize.tab
											)

	--重新挑战
	self.__btn_restartchallenge = self:GetWidget("Button_RestartChallenge")
	--英雄副本和精英副本可见
	local battle_info = self:CallModuleFunc("GetCurInfo")		
	if battle_info.type == Macros.Game.FBType.HERO_DUNGEON or
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

function Modules.SpBattleResultLoseBase:__Dispose()
	if self.__templ_common_reward then
		self.__templ_common_reward:DeleteMe()
		self.__templ_common_reward = nil
	end

	self.__num_addition_anim_list = nil
end

function Modules.SpBattleResultLoseBase:Update(elapse_time)
	Modules.SpBattleResultWinBase.Update(self, elapse_time)
end

--战斗奖励
function Modules.SpBattleResultLoseBase:__ShowCommonRewards()
	self.panel_common_reward = self:GetWidget("Panel_CommonReward")
end

--添加数值递增动画
function Modules.SpBattleResultLoseBase:__AddNumAdditionAnim(node, org_num, change_num, anim_time, finished_callback)
	Modules.SpBattleResultWinBase.__AddNumAdditionAnim(self, node, org_num, change_num, anim_time, finished_callback)
end

function Modules.SpBattleResultLoseBase:__CheckFinished()
	Modules.SpBattleResultWinBase.__CheckFinished(self)
end


function Modules.SpBattleResultLoseBase:__InitPowerEntry()
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
		local layout = Util:Layout( 150 * #t1 , 114 )
		layout:SetAnchorPoint( 0.5 , 0.5 )
		widget:AddChild( layout )
		PosTool.Center(layout)
		
		local interval = layout:GetContentWidth() / #t1
		for i  , system_id in ipairs( t1 ) do
			entry_layer[i] = Modules.SpPowerPromoteEntryLayer.New()
			entry_layer[i]:SetData(system_id, t2[i])
			layout:AddChild(entry_layer[i]:GetNode())
			entry_layer[i]:SetPosition( (84/2) + (interval * (i-1)) , layout:GetContentHeight()/2 )

			if system_id ==  Macros.Game.SystemID.FIRST_RECHARGE then
				local effect
				self.__first_rfecharge_effect_id,effect = self:__CreateEffect({res_name = "UI_shouchongtubiao"} , entry_layer[i] , 99 )
				PosTool.Center(effect,0,-6)

				local effect
				self.__anliyibo_effect_id,effect = self:__CreateEffect({res_name = "UI_anliyibo_language_chs"} , entry_layer[i] , 99 )
				PosTool.Center(effect , -20 , -6 )
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

Modules.SpPowerPromoteEntryLayer = Modules.SpPowerPromoteEntryLayer or BaseClass(UILayer)

function Modules.SpPowerPromoteEntryLayer:__init()
	self:SetAnchorPoint(0, 0.5)
	self:SetContentSize(84, 114)

	self:SetTouchEnabled(true)
	self:SetTouchFun(nil, nil, function()
		if self.__system_id then
			GlobalModulesMgr:OpenSystemModule(self.__system_id, nil, false)			
		end
	end)
end

function Modules.SpPowerPromoteEntryLayer:__delete()
end

function Modules.SpPowerPromoteEntryLayer:SetData(system_id, system_res)
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

	local icon = Util:Sprite(sp)
	self:AddChild(icon)
	PosTool.Center(icon)

	local icon = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE_RESULT_LOSE, "img_lose_"..self.__system_id .. text ,true))
	icon:SetAnchorPoint(0.5, 0)
	self:AddChild(icon)
	PosTool.CenterBottom(icon,0,10)
end