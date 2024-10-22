
--[[
%% @module: 战斗界面
%% @author: swordman sue
%% @created: 2016-08-15
--]]

Modules = Modules or {}

Modules.BattleChasmView = Modules.BattleChasmView or BaseClass( Modules.BattleView )

function Modules.BattleChasmView:__init(params)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.CHASM)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.CHASM,true)
end

function Modules.BattleChasmView:__OpenCallback()
	Modules.BattleView.__OpenCallback(self)

	self.killKey = true
	self.kill = 0
	self.effect_name = ""
	self.reciprocal = 0
	self.stay_time = 9999
	self.now_kill = nil

	self.grade = CPPGameLib.GetConfig("EndlessGrade", nil , nil , true )

	self:__ShowUI()
	self:__SetShowUi()
	self:ObtainAward()
end

function Modules.BattleChasmView:__RegistAllEvents()
	Modules.BattleView.__RegistAllEvents(self)

	--怪物死亡
	self:BindGlobalEvent(Macros.Event.SceneObj.MONSTER_DEAD, function(_, obj_id)
		self:__SetShowUi(obj_id)
	end)

	--领取连杀奖励
	self:BindGlobalEvent(Macros.Event.ChasmModule.CHASM_AWARD, function(_, obj_id)
		self.isRewardReq = nil
		self:ChasmAward(obj_id)
	end)

	--下一波
	self:BindGlobalEvent(Macros.Event.ChasmModule.CHASM_WAVE, function(_, obj_id)
		local wave = CallModuleFunc(ModuleType.CHASM, "GetCurWave")
		local monster = CPPGameLib.GetConfig("EndlessLandWave", wave)
		if monster then
			local monster_id = monster.monster_id_list[5]
			--怪物数据
		    local config_monster = CPPGameLib.GetConfig("Monster", monster_id)
			--怪物名称
			Modules.BattleView.__ChangeEnemyName( self , config_monster.name )
		end
		self:ObtainAward()
	end)
end

function Modules.BattleChasmView:__ShowUI()
	--击杀背景
	local sp_shasm_kill_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHASM, "sp_chasm_kill")
	self.sp_shasm_kill = Util:Sprite( sp_shasm_kill_sp )
	self.sp_shasm_kill:SetAnchorPoint( 0.5 , 0.5 )
	self:AddChild(self.sp_shasm_kill)	
	PosTool.Center( self.sp_shasm_kill , 170 , 160 )
	self.sp_shasm_kill:SetVisible( false )

	--击杀1
	local sp_shasm_kill_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHASM, "sp_chasm_kill2" , true )
	local sp_chasm_kill2 = Util:Sprite( sp_shasm_kill_sp )
	sp_chasm_kill2:SetAnchorPoint( 0.5 , 0.5 )
	sp_chasm_kill2:SetPosition(  self.sp_shasm_kill:GetContentWidth()/2 + 75 , self.sp_shasm_kill:GetContentHeight()/2 - 40 )
	self.sp_shasm_kill:AddChild(sp_chasm_kill2)	
	CPPActionManager.RotateBy(sp_chasm_kill2, 0 , -10 )

	--击杀数
    local battle_num = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "chasm_kill", true)
    self.kill_num = GUI.ImgFont.New(battle_num)
    self.kill_num:SetText(tostring(0))
    self.kill_num:SetAnchorPoint( 0.5 , 0.5 )
    self.sp_shasm_kill:AddChild(self.kill_num:GetNode()) 
	PosTool.Center( self.kill_num  ,-8,-10)
	CPPActionManager.RotateBy(self.kill_num:GetNode(), 0 , -10 )

	--领取
	local sp_shasm_target_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHASM, "sp_chasm_target")
	self.sp_shasm_target = Util:Sprite( sp_shasm_target_sp )
	self.sp_shasm_target:SetAnchorPoint( 0 , 0.5 )
	self.sp_shasm_target:SetPosition( 0 , (Macros.Global.DesignSize.height/2) + 120 )
	self:AddChild(self.sp_shasm_target)	
	self.sp_shasm_target:SetVisible(false)

	--奖励
	self.diamond = Util:Sprite( Util:GetMoneyIconImgData( Macros.Game.RolePropType.DIAMOND ) )
	self.sp_shasm_target:AddChild(self.diamond)	
	PosTool.Center( self.diamond , 0 , 10 )	

	self.chasm_KillAward = Util:Label( CPPGameLib.GetString("chasm_killAward" , 0) , Macros.TypefaceSize.normal , Macros.Color.Special )
	self.chasm_KillAward:SetAnchorPoint( 0.5 , 0.5 )
	self.chasm_KillAward:SetPosition( self.sp_shasm_target:GetContentWidth()/2 , 15 )
	self.sp_shasm_target:AddChild( self.chasm_KillAward )

	local mubiao , diamond = CallModuleFunc(ModuleType.CHASM, "GetKillRecord")
	if mubiao ~= 0 then
		self.sp_shasm_target:SetVisible(true)
		self.chasm_KillAward:SetString( CPPGameLib.GetString("chasm_killAward" , mubiao ) )
	end

	--奖励
	self.Image_kill_bg = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHASM, "sp_chasm_kill_bg") )
	self.Image_kill_bg:SetAnchorPoint( 1 , 0.5 )
	self.Image_kill_bg:SetPosition( self.__root_widget:GetContentWidth() , self.__root_widget:GetContentHeight() - 200 )
	self.__root_widget:AddChild(self.Image_kill_bg)	

    local chasm_max_kill = Util:Label( CPPGameLib.GetString("chasm_max_kill") , Macros.TypefaceSize.normal , Macros.Color.Special  )
    chasm_max_kill:SetAnchorPoint( 0 , 0.5 )
    chasm_max_kill:SetPosition( 0 , self.Image_kill_bg:GetContentHeight()/2 )
    self.Image_kill_bg:AddChild( chasm_max_kill )

    self.chasm_max_kill = Util:Label( "0000" , Macros.TypefaceSize.normal , Macros.Color.white  )
    self.Image_kill_bg:AddChild( self.chasm_max_kill )
    PosTool.RightTo( chasm_max_kill , self.chasm_max_kill )

    --无尽石
	self.Image_acquireBg = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_coin") )
	self.Image_acquireBg:SetAnchorPoint( 1 , 0.5 )
	self.Image_acquireBg:SetPosition( self.__root_widget:GetContentWidth() , self.__root_widget:GetContentHeight() - 250 )
	self.__root_widget:AddChild(self.Image_acquireBg)	
	-- self.Image_acquireBg:SetVisible(false)

    local sp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.ENDLESS_STORE)
    local icon = Util:Sprite( sp )
    icon:SetAnchorPoint( 1 , 0.5 )
    icon:SetPosition( self.Image_acquireBg:GetContentWidth()- 5 , self.Image_acquireBg:GetContentHeight()/2 )
    self.Image_acquireBg:AddChild( icon )
    icon:SetScaleFactor( 0.9 )

    self.acquire = Util:Label( " 0 " , Macros.TypefaceSize.normal , Macros.Color.white )
    self.acquire:SetAnchorPoint( 1 , 0.5 )
    self.acquire:SetPosition( self.Image_acquireBg:GetContentWidth() - 42 , self.Image_acquireBg:GetContentHeight()/2 )
    self.Image_acquireBg:AddChild( self.acquire )

    self.text = Util:Label( CPPGameLib.GetString("chasm_acquire") , Macros.TypefaceSize.normal , Macros.Color.Special )
    self.text:SetAnchorPoint( 1 , 0.5 )
    self.text:SetPosition( self.acquire:GetPositionX() - self.acquire:GetContentWidth() - 5 , self.Image_acquireBg:GetContentHeight()/2 )
    self.Image_acquireBg:AddChild( self.text )

	--倒计时
	self.__txt_countdown:SetVisible(false)
end

function Modules.BattleChasmView:__SetShowUi( ... )

	-- 2249 深渊 每次重开 杀敌重新开始算
	-- 2301 无尽深渊 碾压时杀多少，在副本就要从多少开始算起
	local kill = CallModuleFunc(ModuleType.CHASM, "GetCurKeepKillNum")

	if self.now_kill ~= kill then
		-- CallModuleFunc(ModuleType.CHASM, "SetCurKeepKillNum" , 1 )

		self.now_kill = kill
		if kill ~= 0 then
			self.sp_shasm_kill:SetVisible( true )
		end
		ActionManager.GetInstance():SpreadAnimation(self.kill_num:GetNode() , 1.8 , 0.16 )
		local kill = CallModuleFunc(ModuleType.CHASM, "GetCurKeepKillNum")
		self.kill_num:SetText( tostring( kill ) )
		-- self.kill_ = self.kill_ + 1
	end

	local num = CallModuleFunc(ModuleType.CHASM, "GetKeepKillNum")
	self.chasm_max_kill:SetString( num )

	local mubiao , diamond = CallModuleFunc(ModuleType.CHASM, "GetKillRecord")
	if mubiao ~= 0 then
		self.sp_shasm_target:SetVisible(true)
		self.chasm_KillAward:SetString( CPPGameLib.GetString("chasm_killAward" , mubiao ) )
	end

	if mubiao ~= 0 and num >= mubiao then
		local info = {}
		info.keep_kill_num = mubiao
		if self.isRewardReq ~= true then
			self.isRewardReq = true
			CallModuleFunc(ModuleType.CHASM, "EndlessLandKeepKillRewardReq" , info )
		end
		self.chasm_KillAward:SetString( CPPGameLib.GetString("chasm_killAward" , mubiao ) )
	end

	self.kill = self.kill + 1
	CPPActionManager.StopAllActions(self.sp_shasm_kill)

    local func
    local moveAnimate = function () 
    	if self.stay_time <= 0 then
	    	self.kill = 0
			if self.Effect_Id then
				self:__DelEffect(self.Effect_Id)
				self.Effect_Id = nil
			end	
    	end    	
    	self.stay_time = self.stay_time - 1
        CPPActionManager.DelayTo( self.sp_shasm_kill , 1 , func)
    end
    func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
	moveAnimate()

	local grade_1 = CPPGameLib.GetConfig("EndlessGrade", self.kill , false )
	if grade_1 and self.effect_name ~= grade_1.efecct_icon then
		if self.Effect_Id then
			self:__DelEffect(self.Effect_Id)
			self.Effect_Id = nil
		end	

		if grade_1 then 
			self.effect_name = grade_1.efecct_icon
			self.stay_time = grade_1.stay_time

			local Effect_con = nil
		    self.Effect_Id , Effect_con = self:__CreateEffect( { res_name = grade_1.efecct_icon } , self.sp_shasm_kill )
		    Effect_con:SetPosition( self.sp_shasm_kill:GetContentWidth()/2 + 36 , self.sp_shasm_kill:GetContentHeight()/2 + 61 )
		
		elseif self.kill >= self.grade [ #self.grade ].kill_monster then

			self.effect_name = self.grade [ #self.grade ].efecct_icon
			self.stay_time = self.grade [ #self.grade ].stay_time

			local Effect_con = nil
		    self.Effect_Id , Effect_con = self:__CreateEffect( { res_name =  self.grade [ #self.grade ].efecct_icon } , self.sp_shasm_kill )
		    Effect_con:SetPosition( self.sp_shasm_kill:GetContentWidth()/2 + 36 , self.sp_shasm_kill:GetContentHeight()/2 + 61 )
		end	

	else
		self.stay_time = self.grade [ #self.grade ].stay_time
	end

	local wave = CallModuleFunc(ModuleType.CHASM, "GetCurWave")
	local monster = CPPGameLib.GetConfig("EndlessLandWave", wave)
	if monster then
		local monster_id = monster.monster_id_list[5]
		--怪物数据
	    local config_monster = CPPGameLib.GetConfig("Monster", monster_id)
		--怪物名称
		Modules.BattleView.__ChangeEnemyName( self , config_monster.name )
	end

end

function Modules.BattleChasmView:ChasmAward( obj_id )

	--结算特效
	--延迟 
	CPPActionManager.DelayTo( self.__root_widget , 1.3 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
		local Effect_Id , Effect_con = self:__CreateEffect( { res_name = "UI_wujinjiesuan1_language_chs" } , self )
		PosTool.Center( Effect_con , 0 , 210  )
	end) )

	CPPActionManager.DelayTo( self.Image_kill_bg , 1.38, GlobalCallbackMgr:AddCallBackFunc(function ( ... )
		--底
		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHASM, "sp_chasm_target_conclude")
		local sp_shasm_target_conclude = Util:Sprite( sp )
		sp_shasm_target_conclude:SetAnchorPoint( 0.5 , 0.5 )

		-- 杀敌
		local chasm_killAward = Util:Label( CPPGameLib.GetString("chasm_killAward" , obj_id ) , Macros.TypefaceSize.popup , Macros.Color.white )
		chasm_killAward:SetAnchorPoint( 0.5 , 1 )
		chasm_killAward:SetPosition( sp_shasm_target_conclude:GetContentWidth()/2 , sp_shasm_target_conclude:GetContentHeight() - 12 )
		sp_shasm_target_conclude:AddChild( chasm_killAward )

		--奖励
		local cnf = CPPGameLib.GetConfig("EndlessLandKeepKillReward", nil , nil , true )
		local diamond = 0
		for i , v in ipairs( cnf ) do
			if v.keep_kill_num == tonumber(obj_id) then
				diamond = v.diamond
			end
		end

		local rewaed = Util:MoneyControl( Macros.Game.RolePropType.DIAMOND , 0.9 , "X" .. diamond , Macros.TypefaceSize.popup , Macros.Color.green , 5 )
		rewaed:SetAnchorPoint( 0.5 , 1 )
		rewaed:SetPosition( sp_shasm_target_conclude:GetContentWidth()/2 , chasm_killAward:GetPositionY() - 40 )
		sp_shasm_target_conclude:AddChild( rewaed )

		local layer = GUI.TipMarginLayer.New( sp_shasm_target_conclude , false , false , false , 130)
		layer:Show()
		--UI
		--延迟 
		CPPActionManager.DelayTo( layer:GetNode() , 1.8, GlobalCallbackMgr:AddCallBackFunc(function ( ... )
			--渐显时
			-- CPPActionManager.FadeIn( layer:GetNode() , 2.5 , GlobalCallbackMgr:AddCallBackFunc( function ( ... )
				layer:DeleteMe()
				layer = nil
			-- end ) ) 
		end) )

		--更新字样
		--延迟 
		CPPActionManager.DelayTo( self.chasm_max_kill , 1 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
				--刷光
				local Effect_Id , Effect_con = self:__CreateEffect( { res_name = "UI_wujinmubiao" } , self.sp_shasm_target )
				--刷光坐标XY
				Effect_con:SetPosition( self.sp_shasm_target:GetContentWidth()/2 , 13 )

				local mubiao , diamond = CallModuleFunc(ModuleType.CHASM, "GetKillRecord")
				if mubiao ~= 0 then
					self.chasm_KillAward:SetString( CPPGameLib.GetString("chasm_killAward" , mubiao ) )
				end
		end) )

	end) )
end

--更新倒计时
function Modules.BattleChasmView:__UpdateRemainTime(remain_time)
	-- 标题
	if not self.sp_chasm_endless then
		local Image_9 = self:GetWidget("Image_9")
		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHASM, "sp_chasm_endless" , true )
		self.sp_chasm_endless = Util:Sprite( sp )
		self.sp_chasm_endless:SetAnchorPoint( 0.5 , 0.5 )
		self.sp_chasm_endless:SetPosition( Image_9:GetContentWidth()/2 , Image_9:GetContentHeight()/2 + 10 )
		Image_9:AddChild( self.sp_chasm_endless )
	end
end

--更新波数
function Modules.BattleChasmView:__UpdateBattleIndex()
	local battle_info = self:CallModuleFunc("GetCurInfo")
	self.__txt_battle_times:SetText(CPPGameLib.GetString("battle_bout" , CallModuleFunc(ModuleType.CHASM, "GetCurWave") ))
end

--处理掉落物刷新
function Modules.BattleChasmView:__HandleDropBoxRefresh(drop_info)
	Modules.BattleView.__HandleDropBoxRefresh(self, drop_info)

	local drop_list = {}
	local drop_map = {}
	for _, drop_info in ipairs(drop_info.drop_list) do
		drop_map[drop_info.obj_id] = drop_map[drop_info.obj_id] or 0
		drop_map[drop_info.obj_id] = drop_map[drop_info.obj_id] + drop_info.obj_num
	end
	for obj_id, obj_num in pairs(drop_map) do
		table.insert(drop_list, {id = obj_id, num = obj_num})
	end

	local text = ""
	local cnf = CPPGameLib.GetConfig("EndlessLandCollision", self.collisionData.reward_id )
	for i , v in ipairs( drop_list ) do
		local itemCnf = CPPGameLib.GetConfig("Item", v.id )
		text = text .. itemCnf.name .." X ".. v.num
	end

    if text ~= "" then
        GlobalTipMsg.GetInstance():ShowGetLayer( text , nil , 1 , "sp_shasm_get" , 200 )
    end
end

--初始化怪物血量
function Modules.BattleChasmView:__InitMonsterHp(cur_hp, total_hp)
	local total_hp = CallModuleFunc(ModuleType.CHASM, "GetBattleMonsterTotalHP")
	Modules.BattleView.__InitMonsterHp(self, total_hp, total_hp)
end

--处理怪物血量变化 
function Modules.BattleChasmView:__HandleMonsterHpChange(change_hp, cur_hp)
	local monster_cur_hp = self.__monster_cur_hp + change_hp
	monster_cur_hp = math.max(0, monster_cur_hp)
	monster_cur_hp = math.min(monster_cur_hp, self.__monster_total_hp)
	Modules.BattleView.__HandleMonsterHpChange(self, change_hp, monster_cur_hp)
end

function Modules.BattleChasmView:ObtainAward()

	local historyWaveRecord = CallModuleFunc(ModuleType.CHASM, "GetRewardWave" )
	--能有奖励
	if historyWaveRecord and historyWaveRecord ~= 0 then
		local _ , award = CallModuleFunc(ModuleType.CHASM, "GetCurWaveAward" )
		CallModuleFunc(ModuleType.CHASM, "SetTotalObtain" , award )
	end

	local totalObtain = CallModuleFunc(ModuleType.CHASM, "GetTotalObtain" ) 
	if totalObtain and totalObtain ~= 0 then
		self.Image_acquireBg:SetVisible( true )
	end

    self.acquire:SetString( totalObtain or " " )
    self.text:SetPosition( self.acquire:GetPositionX() - self.acquire:GetContentWidth() - 8 , self.Image_acquireBg:GetContentHeight()/2 )

end
