--
-- @brief 爬塔主界面 设置篇 (主界面)
-- @author: yjg
-- @date: 2016年9月26日11:00:01
--

Modules = Modules or {}

Modules.TowerView = Modules.TowerView or BaseClass(BaseView)

--神装商店	
function Modules.TowerView:OnButtonShop()
	CallModuleFunc(ModuleType.TOWER, "OpenView", BaseViewType.TOWERSHOP)
end

--精英挑战
function Modules.TowerView:OnButtonElite()
	LogicGUI.TemplTowerElite.New( function ( ... )
		CallModuleFunc(ModuleType.TOWER,"SetbattleData",nil)
	end )
	if self.elite_red then
		local index = CallModuleFunc(ModuleType.TOWER,"GetEliteTime")
		local level = CallModuleFunc(ModuleType.TOWER , "GetLevelRecord")
		local cnf = CPPGameLib.GetConfig("TowerEliteMap", 1 )
		if index ~= 0 and level >= cnf.normal_map_level then
			self.elite_red:SetVisible(true)
		else
			self.elite_red:SetVisible(false)
		end
	end
end

--奖励
function Modules.TowerView:OnButtonAward()
	LogicGUI.TempTowerAward.New()
end

--排行榜
function Modules.TowerView:OnButtonRanking()
	LogicGUI.TempTowerRank.New()
end

--试炼
function Modules.TowerView:OnButtonTrial()
	local InLead = CallModuleFunc(ModuleType.LEAD,"IsInLead")
	if InLead ~= true then
		LogicGUI.TempTowerTreasure.New()
	end
end

--重置按钮
function Modules.TowerView:OnButtonReset()
	--如果需要重置
	local reset = CallModuleFunc(ModuleType.TOWER, "IsNeedReset")

	local tower_game_level = CallModuleFunc(ModuleType.TOWER, "GetCurMapLevel")
	local towerGameCnf = CPPGameLib.GetConfig("TowerGameLevel", tower_game_level+1 , false )
	if reset or not towerGameCnf then
		LogicGUI.TempTowerReset.New()
	else
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("tower_noReset") )
	end
end

--扫荡按钮
function Modules.TowerView:OnButtonSweep()
	local type_ = CallModuleFunc(ModuleType.TOWER, "GetSweep")
	if type_ then
		local info = {}
		-- info.is_to_history_record = 0
		CallModuleFunc(ModuleType.TOWER , "TowerOneKeyCleanReq" , info )
		self.Button_sweep:SetTouchEnabled(false)
		self.Button_onekey_reset:SetTouchEnabled(false)
	else
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("tower_noSweep") )
	end
end

--重置回调
function Modules.TowerView:OnReset()
	local effectid,effect = self:__CreateEffect({res_name = "UI_pata_miwu"}, self.Panel_1 , 2)
	PosTool.Center(effect)

	CPPActionManager.DelayTo(self.Panel_1 , 0.3 ,GlobalCallbackMgr:AddCallBackFunc(function ( ... )
		self.modes:TerraceReset()
	end))
end

--更多属性
function Modules.TowerView:OnMoreNature()
	LogicGUI.TempTowerProperty.New()
end

--扫荡
function Modules.TowerView:OnSweep()
	local type_ = CallModuleFunc(ModuleType.TOWER, "GetIsOneKeyClean")
	if type_ and type_ == "oneKey" then
		-- self:TowerSection()
	else
		LogicGUI.TemplTowerSweep.New(function ( ... )
			self:TowerSection()
		end)	
	end
end

--一键扫荡开启
function Modules.TowerView:OnButtonOnekeyReset()
	print("xy" , "=====一键扫荡开启")
	local type_ = CallModuleFunc(ModuleType.TOWER, "GetSweep")
	if type_ then


		local acc_factor = 1
        local anim_info = {}
        anim_info.type = "anim_battle_skip"
        anim_info.effect_list = 
        {
            {res_name = "UI_zhandoutiaoguo2", time_scale = 1 / acc_factor, zorder = Scene.RenderQueue.GRQ_UI_UP},
            {res_name = "UI_battleskip_left", type = Effect.EffectType.SKELETON, time_scale = 1 / acc_factor, zorder = Scene.RenderQueue.GRQ_UI_UP, bind_event_callback = true},
            {res_name = "UI_battleskip_right", type = Effect.EffectType.SKELETON, time_scale = 1 / acc_factor, zorder = Scene.RenderQueue.GRQ_UI_UP}
        }
		ActionManager.GetInstance():StartMapBlackingWithLock(nil, 127, nil, nil, Scene.RenderQueue.GRQ_UI_UP, anim_info)

		-- self.__loading_effect_id, effect_root = self:__CreateEffect({res_name = "UI_loading"}, self.__root_widget)
		-- PosTool.Center(effect_root)	

		CallModuleFunc(ModuleType.TOWER, "SetOneKeyNum")

		local info = {}
		-- info.is_to_history_record = 1
		CallModuleFunc(ModuleType.TOWER , "TowerOneKeyAllCleanReq" , info )
	else
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("tower_noSweep") )
	end
end

--一键扫荡没开
function Modules.TowerView:NoButtonOnekeyReset()
	print("xy" , "=====一键扫荡没开")

	local system_info = CPPGameLib.GetConfig("SystemEntry",Macros.Game.SystemID.ONE_KEY_TOWER)
	local vipLevel = CallModuleFunc(ModuleType.VIP, "GetToVipOpen", Macros.Game.VIPType.ONE_KEY_TOWER )

    local tips = LogicGUI.TemplAffirmTips.New( { 
                                                    type = LogicGUI.TemplAffirmTipsType.YES_NO,
                                                    content = " ",
                                                    cancel_text = CPPGameLib.GetString("common_cancel") ,
                                                    confirm_text = CPPGameLib.GetString("vip_gorecharge") ,
                                                    confirm_func = function ( ... )
                                                        CallModuleFunc(ModuleType.CHARGE, "OpenView", BaseViewType.CHARGE)
                                                    end
                                                } )

    tips:SetLoadedCallback( function ( ... )
        local text = 
            {
                { zi = CPPGameLib.GetString("tower_reset_openTips" , system_info.level , vipLevel ),dx = Macros.TypefaceSize.normal , ys = Macros.Color.content },
            }
        local Affirm = Util:LabCenter(text)
        Affirm:SetPosition(tips:GetNode():GetContentWidth()/2 , tips:GetNode():GetContentHeight()/2 + 25 )
        tips:GetNode():AddChild(Affirm)

        local arena_today_buyTimes = Util:Label( CPPGameLib.GetString("vip_recharge_1") , Macros.TypefaceSize.normal , Macros.Color.content )
        arena_today_buyTimes:SetAnchorPoint(0.5,0.5)
        arena_today_buyTimes:SetPosition(tips:GetNode():GetContentWidth()/2 , tips:GetNode():GetContentHeight()/2 - 25 )
        tips:GetNode():AddChild(arena_today_buyTimes)
    end )
end

--一键扫荡奖励
function Modules.TowerView:OnekeyResetAward()

	local num = CallModuleFunc(ModuleType.TOWER, "GetPassedRecord")
	local index = CallModuleFunc(ModuleType.TOWER, "GetOneKeyNum") + 1
	
	local t1 = {} 
	for i = index , num do
		local chapterCnf = CPPGameLib.GetConfig("TowerChapter" , i)
		
		for ii , vv in ipairs( chapterCnf.star_award_id_list[3] ) do
			table.insert( t1 , { item_id = vv , item_num = chapterCnf.star_award_num_list[3][ii] } )
		end
	end

	local tips = GUITemplateMgr:CreateTemplateInstance( 
														GUI.TemplateRegistry.TEMPL_GIFTBAG_GAIN , 
														true , t1 , 0 , function ( ... )
																			local effectid,effect = self:__CreateEffect({res_name = "UI_pata_miwu"}, self.Panel_1 , 2)
																			PosTool.Center(effect)

																			if CallModuleFunc(ModuleType.TOWER,"GetOneKeyType") then
																				CallModuleFunc(ModuleType.TOWER,"SetOneKeyType",false)
																				self:SetMonster()
																			end

																			CPPActionManager.DelayTo(self.Panel_1 , 0.3 ,GlobalCallbackMgr:AddCallBackFunc(function ( ... )
																				self.Image_conditionBg:SetVisible(true)
																				self.modes:TerraceReset()

																				self.Button_sweep:SetTouchEnabled(true)
																				self.Button_onekey_reset:SetTouchEnabled(true)

																				self.Button_award:SetPosition( self.Button_awardPos.x , self.Button_awardPos.y )
																				self.Button_award:SetVisible(true)
																			end))
																		end)
    tips:SetLoadedCallback( function ( ... )
    	tips:SetBtnText( CPPGameLib.GetString("common_confirm") )
    	tips:SetHeadlineText( CPPGameLib.GetString("tower_onekey_reset_award") )
    	tips:SetTipsText( CPPGameLib.GetString("tower_now_reset" , CallModuleFunc(ModuleType.TOWER , "GetCurMapLevel") + 1 ) )
    end )

    --设置当前信息
	self:SetCurrentInfo()
	self:SetCurrentNature()
end