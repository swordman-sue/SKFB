--
-- @brief 爬塔主界面 设置篇 (主界面)
-- @author: yjg
-- @date: 2016年9月26日11:00:01
--

Modules = Modules or {}

Modules.TowerView = Modules.TowerView or BaseClass(BaseView)

function Modules.TowerView:SetShowUI()
	self:SetHideAlterUI()

	--设置怪物
	self:SetMonster()

	--是否需要重置
	local resetType = CallModuleFunc(ModuleType.TOWER,"IsNeedReset")
	if resetType == true then

		--如果是一键扫荡出来的
		if CallModuleFunc(ModuleType.TOWER,"GetIsOneKeyClean") == "oneKey" then

			self.modes:TerraceAscheStabilize()

			ActionManager.GetInstance():StopMapBlackingWithLock()

			CallModuleFunc(ModuleType.TOWER,"SetIsOneKeyClean" , false)
			--一键扫荡奖励
			self:OnekeyResetAward()

		else
			--输了
			self:SetLoseShowUI()
		end

	else
		--如果是扫荡出来的
		if CallModuleFunc(ModuleType.TOWER,"GetIsOneKeyClean") == "common" then
			CallModuleFunc(ModuleType.TOWER,"SetIsOneKeyClean" , false)

		--如果是一键扫荡种
		elseif CallModuleFunc(ModuleType.TOWER,"GetIsOneKeyClean") == "oneKeyMarch" then
			-- CallModuleFunc(ModuleType.TOWER,"SetIsOneKeyClean" , false)

		--如果是一键扫荡出来的
		elseif CallModuleFunc(ModuleType.TOWER,"GetIsOneKeyClean") == "oneKey" then
			self.modes:TerraceAscheStabilize()
			ActionManager.GetInstance():StopMapBlackingWithLock()
			CallModuleFunc(ModuleType.TOWER,"SetIsOneKeyClean" , false)
			--一键扫荡奖励
			self:OnekeyResetAward()

		-- 如果有buff可以领取 （通关了
		elseif #CallModuleFunc(ModuleType.TOWER, "GetRandProList") ~= 0 then
			-- 通关
			self:TowerSection()

		    --设置当前信息
			self:SetCurrentInfo()
			self:SetCurrentNature()
		else
			--正常
			self:SetNormalShowUI()
		end
	end

	--设置扫荡
	self:SetSweepBtn()

	--按钮位置设置
	self:SetBtnPos()

	--精英按钮设置
	self:SetEliteBtn()

	--设置重置次数
	self:SetResetText()

	--设置红点
	self:SetRed()
end

--设置怪物
function Modules.TowerView:SetMonster()
	local type_ = CallModuleFunc(ModuleType.TOWER,"GetOneKeyType")
	if type_ == true then
		return
	end

	if self.modes then
		self.modes:DeleteMe()
		self.modes = nil
	end

	local data = nil
	local info = CallModuleFunc(ModuleType.TOWER,"GetbattleData")
	if info and info[2] then
		data = info[2]
	end
	self.modes = LogicGUI.TempTowerMonster.New(self.Panel_terrace1,self.Panel_terrace2,self.Panel_terrace3 , data )

	self.modes:TerraceStabilize()
end

--隐藏掉变动控件
function Modules.TowerView:SetHideAlterUI()
	--失败
	--提示组件
	self.Image_lose:SetVisible(false)

	--按钮
	--隐藏扫荡
	self.Button_sweep:SetVisible( false )
	--隐藏一键扫荡
	self.Button_onekey_reset:SetVisible( false )
	--隐藏重置
	self.Button_reset:SetVisible( false )
	--隐藏精英
	self.Button_elite:SetVisible( false )
	--隐藏奖励
	self.Button_award:SetVisible(false)
	--隐藏密藏
	self.Button_trial:SetVisible(false)

	--锁
	self.Image_lock:SetVisible( false )
end

--按钮位置设置
function Modules.TowerView:SetBtnPos()
	--隐藏扫荡
	self.Button_sweep:SetVisible( false )
	--隐藏一键扫荡
	self.Button_onekey_reset:SetVisible( false )
	--隐藏重置
	self.Button_reset:SetVisible( false )

	--需要重置
	local resetType = CallModuleFunc(ModuleType.TOWER,"IsNeedReset")
	if resetType == true then
		self.Button_reset:SetVisible(true)
		--重置位置修改
		self.Button_reset:SetPosition( self.Panel_1:GetContentWidth()/2 , self.Button_reset:GetPositionY() )
	--不需要重置
	else
		self.Button_reset:SetVisible(true)
		self.Button_sweep:SetVisible(true)

		self.Button_reset:SetPosition( self.Panel_1:GetContentWidth()*0.35 ,  self.Button_reset:GetPositionY() )
		self.Button_sweep:SetPosition( self.Panel_1:GetContentWidth()*0.5 ,  self.Button_sweep:GetPositionY() )

	    local IsOpen = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.ONE_KEY_TOWER )
	    self.Button_onekey_reset:SetVisible(IsOpen)
	end

    self.Button_award:SetPosition( self.Button_awardPos.x , self.Button_awardPos.y )
end

--精英按钮设置
function Modules.TowerView:SetEliteBtn()
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	local cnf = CPPGameLib.GetConfig("SystemEntry" , Macros.Game.SystemID.TOWER_ELITE )

	--小于预告
	if role_level < cnf.prepare_level then
		self.Button_elite:SetVisible( false )
	--大于等于预告 而且 小于开启
	elseif role_level >= cnf.prepare_level and role_level < cnf.level then

		--灰按钮
		self.Button_elite:SetVisible( true )
		Util:GreyButton( self.Button_elite )
		--灰图
		self.Image_elite_1:SetGray(true)
		--显锁
		self.Image_lock:SetVisible(true)

		local onTou = Util:Layout( self.Button_elite:GetContentWidth() , self.Button_elite:GetContentHeight() , function ( ... )
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("tower_eliteOpenLevel" , cnf.level ))
		end)
		onTou:SetAnchorPoint( 0.5 , 0.5 )
		self.Panel_1:AddChild( onTou , 2 )
		onTou:SetPosition( self.Button_elite:GetPositionX() , self.Button_elite:GetPositionY() )
	else
		--彩按钮
		self.Button_elite:SetVisible( true )
		self.Button_elite:SetEnabled( true )
		--彩图
		self.Image_elite_1:SetGray(false)
		--隐锁
		self.Image_lock:SetVisible(false)
	end
end

--扫荡按钮设置
function Modules.TowerView:SetSweepBtn()

	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)

	--扫荡开启判断
	local systemCnf = CPPGameLib.GetConfig("SystemEntry" , Macros.Game.SystemID.TOWER_SWEEP )
	if role_level <= systemCnf.prepare_level and role_level < systemCnf.level  then
		self.Button_sweep:SetVisible( true )
		WidgetUnity.SetWidgetListener( self.Button_sweep , nil , nil , function ( ... )
			local vipLevel = CallModuleFunc(ModuleType.VIP, "GetToVipOpen", Macros.Game.VIPType.KRUUNU_SWEEP )
			GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("hero_train_times_conditional" , vipLevel , systemCnf.level ) )
		end , nil , 0.5*1000 )
	elseif role_level >= systemCnf.level then
		self.Button_sweep:SetVisible( true )
		WidgetUnity.SetWidgetListener( self.Button_sweep , nil , nil , function ( ... ) 
			self:OnButtonSweep()
		end , nil , 0.5*1000 )
	end

	--一键扫荡开启判断
    local system_info = CPPGameLib.GetConfig("SystemEntry",Macros.Game.SystemID.ONE_KEY_TOWER)
    if system_info.prepare_level <= role_level then
    	self.Button_onekey_reset:SetVisible( true )
    end

	local resetType = CallModuleFunc(ModuleType.TOWER,"IsNeedReset")
	if resetType == true then
	    self.Button_onekey_reset:SetVisible( false )
    end

	--等级开启 或者vip够
    local vipType = CallModuleFunc(ModuleType.VIP, "GetPrivilege", Macros.Game.VIPType.ONE_KEY_TOWER)
    if system_info.level <= role_level or vipType == 1 then
    	WidgetUnity.SetWidgetListener( self.Button_onekey_reset , nil, nil, function ( ... )
    		self:OnButtonOnekeyReset()
    	end, nil, 0.5*1000)
	--未开启
    else
    	Util:GreyButton( self.Button_onekey_reset )
    	if self.Button_onekey_reset:IsVisible() ~= false then
	    	local layout = Util:Layout( 
	    								self.Button_onekey_reset:GetContentWidth() , 
	    								self.Button_onekey_reset:GetContentHeight() ,
	    								CPPGameLib.Handler(self,self.NoButtonOnekeyReset)
	    								)
	    	layout:SetAnchorPoint( 0.5 , 0.5 )
	    	self.Panel_1:AddChild( layout )
	    	layout:SetPosition( self.Button_onekey_reset:GetPositionX() ,  self.Button_onekey_reset:GetPositionY() )
    	end
    end
end

--设置重置次数
function Modules.TowerView:SetResetText()
	local _ , items = CallModuleFunc(ModuleType.TOWER, "GetResetTime")
	self.reset_text:SetString( CPPGameLib.GetString("tower_residueResetTimes" , items ) )
end

--设置红点
function Modules.TowerView:SetRed()
	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	local red_num = CallModuleFunc(ModuleType.TOWER , "GetEliteTime") 
	local level = CallModuleFunc(ModuleType.TOWER , "GetLevelRecord")
	local cnf = CPPGameLib.GetConfig("TowerEliteMap", 1 )

	local IsOpen = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.TOWER_ELITE )

	if red_num ~= 0 and level >= cnf.normal_map_level and not IsOpen then
		self.elite_red:SetVisible(true)
	else
		self.elite_red:SetVisible(false)
	end

	local index = CallModuleFunc(ModuleType.TOWER,"GetNowBuyNum")
	if self.red_shop then
    	if index ~= 0 then
    		self.red_shop:SetVisible(true)
    	else
    		self.red_shop:SetVisible(false)
    	end
    end
end



----------------------------------------------------------------------------------------------------------------------------
--正常
----------------------------------------------------------------------------------------------------------------------------
--正常显示
function Modules.TowerView:SetNormalShowUI()
	--设置通关条件
	self:SetPlayerUI()

	--设置当前信息
	self:SetCurrentInfo()

	--设置当前属性
	self:SetCurrentNature()

	--设置奖励
	self:SetTowerBtn()
end

--设置通关条件
function Modules.TowerView:SetPlayerUI()
	self.Image_conditionBg:SetVisible(true)

	--通关条件
	local str_condition = CPPGameLib.GetString("tower_cond_tips_4")
	local config_tower_game_level = CPPGameLib.GetConfig("TowerGameLevel", CallModuleFunc(ModuleType.TOWER , "GetCurMapLevel") + 1 , false)
	local zi = {}
	if config_tower_game_level and config_tower_game_level.win_condition then
		local c_type = config_tower_game_level.win_condition[1]
		local c_param = config_tower_game_level.win_condition[2]
		str_condition = CPPGameLib.GetString("tower_cond_tips_"..c_type)	

		zi = { { zi = tostring(c_param) , ys = Macros.Color.front_color } }
	end	

	if self.minute then
		self.minute:RemoveSelf(true)
		self.minute = nil
	end
	self.minute = Util:Echo( str_condition , zi )
	self.minute:SetAnchorPoint( 0.5,0.5 )
	self.minute:SetPosition( self.Image_conditionBg:GetContentWidth()/2 , self.Image_conditionBg:GetContentHeight()/2 - 15 )
	self.Image_conditionBg:AddChild(self.minute)
end
	
--设置当前信息
function Modules.TowerView:SetCurrentInfo()
	--威望
	self.prestige:SetString( CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.TOWER_PRESTIGE) )

	--可用
	self.tower_curve_nature:SetString( CallModuleFunc(ModuleType.TOWER,"GetCurStar") )

	--历史最高
	self.tower_history:SetString( CallModuleFunc(ModuleType.TOWER,"GetSumStar") or 0 )
	PosTool.RightTo(self.tower_history, self.tower_historySp )

	--本次挑战
	self.tower_the:SetString( CallModuleFunc(ModuleType.TOWER,"GetTotalStar") )
	PosTool.RightTo(self.tower_the, self.tower_theSp )
end

--设置当前属性
function Modules.TowerView:SetCurrentNature()
	for i=1,#self.nature do
		self.nature[i]:SetString( " " )
		self.natureValue[i]:SetString( " " )
	end

	--累计属性
	local lst = CallModuleFunc(ModuleType.TOWER,"GetHoldBuff")
	for i,v in ipairs(lst) do
		local type_ , value = Util:Nature( v.type , v.value ,true ) 
		if self.nature[i] then
			self.nature[i]:SetString( type_ )
			self.natureValue[i]:SetString( value )
		end
	end
end

--通关奖励
function Modules.TowerView:SetTowerBtn( ... )
	self.Button_award:SetVisible( true )
end




----------------------------------------------------------------------------------------------------------------------------
--特殊 （buff 奖励 
----------------------------------------------------------------------------------------------------------------------------
--通关BUFF
function Modules.TowerView:TowerBuff( ... )
	LogicGUI.TemplTowerBuff.New( function ( ... )
		local effectid,effect = self:__CreateEffect({res_name = "UI_pata_miwu"}, self.Panel_1 , 2)
		PosTool.Center(effect)

		if CallModuleFunc(ModuleType.TOWER,"GetOneKeyType") then
			CallModuleFunc(ModuleType.TOWER,"SetOneKeyType",false)
			-- self:SetMonster()
		end

		CPPActionManager.DelayTo(self.Panel_1 , 0.3 ,GlobalCallbackMgr:AddCallBackFunc(function ( ... )
			--正常显示
			self:SetNormalShowUI()
			--按钮位置设置
			self:SetBtnPos()
			-- self.Image_conditionBg:SetVisible(true)
			self.modes:TerraceReset()

			self.Button_sweep:SetTouchEnabled(true)
			self.Button_onekey_reset:SetTouchEnabled(true)

			-- self.Button_award:SetPosition( self.Button_awardPos.x , self.Button_awardPos.y )
			-- self.Button_award:SetVisible(true)
		end))
	end )
end

--通关奖励
function Modules.TowerView:TowerSection( ... )
	self.modes:TerraceAscheStabilize()

	local info = CallModuleFunc(ModuleType.TOWER,"GetbattleData")

	--如果刚打完普通爬塔
	if info and info[3] and info[3].type == Macros.Game.FBType.TOWER_COMMON then
		self.Button_award:SetVisible(true)

		self.layout = Util:Layout( Macros.Global.DesignSize.width, Macros.Global.DesignSize.height , function ( ... )
		end )
		self.layout:SetAnchorPoint( 0.5 , 0.5 )
		self:AddChild( self.layout )
		PosTool.Center( self.layout )

		CPPActionManager.MoveTo( self.Button_award  , 0.5 , self.Panel_1:GetContentWidth()/2 , self.Panel_1:GetContentHeight()/4 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
			self.Button_award:SetVisible(false)
			LogicGUI.TempTowerSection.New( function ( ... )
				self:TowerBuff()
				if self.layout then
					self.layout:RemoveSelf(true)
					self.layout = nil
				end
			end )
		end) )
	--如果没有记录
	elseif not info then
		self:TowerBuff()
	end
end

----------------------------------------------------------------------------------------------------------------------------
--失败
----------------------------------------------------------------------------------------------------------------------------
--失败显示
function Modules.TowerView:SetLoseShowUI()
	--是否需要重置
	local resetType = CallModuleFunc(ModuleType.TOWER,"IsNeedReset")
	if resetType == true then
		--显示人物
		self.Image_lose:SetVisible(true)
		--隐藏条件
		self.Image_conditionBg:SetVisible(false)
		--隐藏所有怪物
		self.modes:TerraceEmptyingStabilize()
	else
		--隐藏人物
		self.Image_lose:SetVisible(false)
	end

	--设置失败文字提示
	local num = CallModuleFunc(ModuleType.TOWER,"GetCurMapLevel")
	self.tower_encourageTips:SetString( CPPGameLib.GetString( "tower_encourageTips" , num or 0 ) )

	self:SetCurrentInfo()
	self:SetCurrentNature()

	self:SetBtnTrialPos()
end

--无双密藏奖励按钮设置
function Modules.TowerView:SetBtnTrialPos()
	--有没有奖励给我
	local type_ , info = CallModuleFunc(ModuleType.TOWER,"GetStarGoods")
	self.Button_trial:SetVisible( type_ )

	--本次打开界面是否弹出打脸
	local isShow = CallModuleFunc(ModuleType.TOWER,"GetShowAwardType")
	if isShow == true and type_ == true then
		--设置显示
		CallModuleFunc(ModuleType.TOWER,"SetShowAwardType" , nil )
		self:OnButtonTrial()
	end
end

function Modules.TowerView:CheckSectionOperateSP()
	if self.modes then
		self.modes:CheckSectionOperateSP()
	end	
end
