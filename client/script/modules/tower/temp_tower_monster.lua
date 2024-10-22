
--
-- @brief 爬塔怪物
-- @author: yjg
-- @date: 2016年12月1日09:43:32
--

LogicGUI = LogicGUI or {}

LogicGUI.TempTowerMonster = LogicGUI.TempTowerMonster or BaseClass()

function LogicGUI.TempTowerMonster:__init( node1 , node2 , node3 ,type_)

	self.nodeLst = {
			[1] = node1,
			[2] = node2,
			[3] = node3,
	}
	self.type = type_

	self:__ShowUI(  )
end

function LogicGUI.TempTowerMonster:__delete( )
	for i=1,#self.monster.mode do
		if self.monster.mode[i] then
			self.monster.mode[i]:DeleteMe()
			self.monster.mode[i] = nil
		end

		if self.monster.mu[i] then
			self.monster.mu[i]:RemoveSelf(true)
			self.monster.mu[i] = nil
		end

		if self.monster.effectId[i] then
			BaseView.__DelEffect(self, self.monster.effectId[i] )
			self.monster.effectId[i] = nil
			self.monster.effectCon[i] = nil
		end

		if self.monster.name[i] then
			self.monster.name[i]:RemoveSelf(true)
			self.monster.name[i] = nil
		end

		if self.monster.cut[i] then
			self.monster.cut[i]:RemoveSelf(true)
			self.monster.cut[i] = nil
		end

		if self.monster.kailBg[i] then
			self.monster.kailBg[i]:RemoveSelf(true)
			self.monster.kailBg[i] = nil
		end

		if self.monster.kail[i] then
			self.monster.kail[i]:RemoveSelf(true)
			self.monster.kail[i] = nil
		end		

	end
end

function LogicGUI.TempTowerMonster:__ShowUI( )

	local lst = CallModuleFunc(ModuleType.TOWER, "TowerMonsterStar")

	self.monster = {}
	self.monster.mode = {}
	self.monster.mu = {}

	self.monster.cut = {}
	self.monster.starScene = {}
	self.monster.grey = {}

	self.monster.name = {}

	self.monster.effectId = {}
	self.monster.effectCon = {}
	self.monster.layout_lead = {}

	self.monster.kailBg = {}
	self.monster.kail = {}

	local deadSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TOWER, "dead")
	local starSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "star")
	local talkSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_talk")

	local star = Util:Sprite( starSp )

	for i,v in ipairs(lst) do
		local cnf = CPPGameLib.GetConfig("GameLevel", v.map_list[1])

		local tower_game_level = CallModuleFunc(ModuleType.TOWER, "GetCurMapLevel")
		local towerGameCnf = CPPGameLib.GetConfig("TowerGameLevel", tower_game_level+1 , false )
		if not towerGameCnf then
			towerGameCnf = CPPGameLib.GetConfig("TowerGameLevel", tower_game_level , false )
		end

		local monter = CPPGameLib.GetConfig( "Monster" , cnf.icon_id )
	
		--创建模型
		self.monster.mode[i] = LogicGUI.Model.New(	{
														info_id = cnf.icon_id , 
														parent = self.nodeLst[i] ,
														model_type = LogicGUI.ModelType.MONSTER,
														dir = MathTool.HORZ_LEFT
													})

		self.monster.mode[i]:SetTouchRect(-80 , 0 , 150 , 150 )
		self.monster.mode[i]:SetScaleFactor( monter.show_scale or 1 )
		self.monster.mode[i]:SetTouchFun(nil,nil,function ( ... )

			if v.game_level > (tower_game_level+1) then
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("tower_Tips"))
			else
				local type_ = CallModuleFunc(ModuleType.TOWER, "IsNeedReset")
				if type_ == true then
					GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("tower_beat"))
				else
					self:OnModes(v)
				end
			end
		end)
		self.monster.mode[i]:SetPosition( self.nodeLst[i]:GetContentWidth()/2 , self.nodeLst[i]:GetContentHeight() - 28 )

		--指引触碰容器
		local layout_lead = Util:Layout(100, 100)
		layout_lead:SetAnchorPoint(0.5, 0)
		self.nodeLst[i]:AddChild(layout_lead)
		layout_lead:SetPosition( self.nodeLst[i]:GetContentWidth()/2 , self.nodeLst[i]:GetContentHeight() - 13 )
		self.monster.layout_lead[i] = layout_lead

		--墓碑
		self.monster.mu[i] = Util:Sprite(deadSp)
		self.monster.mu[i]:SetVisible(false)
		self.monster.mu[i]:SetAnchorPoint(0.5,0)
		self.monster.mu[i]:SetPosition( self.nodeLst[i]:GetContentWidth()/2+ 5 , self.nodeLst[i]:GetContentHeight() - 30 )
		self.nodeLst[i]:AddChild(self.monster.mu[i])

		--星星层
		self.monster.starScene[i] = Util:Layout( (star:GetContentWidth()+5) * 3  , star:GetContentHeight() )
		self.monster.starScene[i]:SetAnchorPoint(0.5,0.5)
		self.monster.mu[i]:AddChild(self.monster.starScene[i])
		PosTool.Center(self.monster.starScene[i])

		--灰色星星
		self.monster.grey[i] = {}
		local interval = self.monster.starScene[i]:GetContentWidth() / 3
		for j = 1 , 3 do
			self.monster.grey[i][j] = Util:Sprite( starSp )
			self.monster.grey[i][j]:SetGray(true)
			self.monster.grey[i][j]:SetAnchorPoint( 0.5 , 0.5 )
			self.monster.grey[i][j]:SetPosition( (star:GetContentWidth()/2) + interval * (j - 1) , self.monster.starScene[i]:GetContentHeight()/2 - 50 )
			self.monster.starScene[i]:AddChild( self.monster.grey[i][j] )
		end

		--名字
		self.monster.name[i] = Util:Name(v.name , nil )
		self.monster.name[i]:SetAnchorPoint( 0.5,0.5 )
		self.monster.name[i]:SetPosition(  self.nodeLst[i]:GetContentWidth()/2 , self.nodeLst[i]:GetContentHeight() - 60 )
		self.nodeLst[i]:AddChild(self.monster.name[i])

		--关卡
		self.monster.cut[i] = Util:Name( CPPGameLib.GetString( "tower_screening" , v.game_level ) , nil , Macros.Game.QualityType.ORANGE)
		self.monster.cut[i]:SetAnchorPoint(0.5,0.5)
		self.monster.cut[i]:SetPosition(  self.nodeLst[i]:GetContentWidth()/2 , self.nodeLst[i]:GetContentHeight() - 110 )
		self.nodeLst[i]:AddChild(self.monster.cut[i])

		--喊话
		self.monster.kailBg[i] = Util:Sprite9( talkSp )
																						
		self.monster.kailBg[i]:SetContentSize( 
												( self.monster.kailBg[i]:GetContentWidth() * 0.48 + 10) ,  --按原图缩放百分之40 + 10像素
												( self.monster.kailBg[i]:GetContentHeight() * 0.48 + 10 )   --按原图缩放百分之40 + 10像素
											)
		self.monster.kailBg[i]:SetFlipX(true)
		self.monster.kailBg[i]:SetPosition( -32 , 285 )
		self.nodeLst[i]:AddChild(self.monster.kailBg[i] , 5 )

-- towerGameCnf.chat
		self.monster.kail[i] = Util:Label(towerGameCnf.chat , Macros.TypefaceSize.slightly , Macros.Color.common_stroke , self.monster.kailBg[i]:GetContentWidth() - 45 )
		self.monster.kail[i]:SetAnchorPoint(0.5 , 0.5)
		self.monster.kail[i]:SetPosition( self.monster.kailBg[i]:GetPositionX() + 8 , self.monster.kailBg[i]:GetPositionY() )
		self.nodeLst[i]:AddChild( self.monster.kail[i] , 6)

		--按钮
	    self.monster.effectId[i] , self.monster.effectCon[i] = BaseView.__CreateEffect(self, {res_name = "UI_xiaodao"},self.nodeLst[i])
	    self.monster.effectCon[i]:SetScaleFactor(0.65)
	    PosTool.Center(self.monster.effectCon[i],0,230)

	end

end

-- 平台移动
function LogicGUI.TempTowerMonster:TerraceMove()

	local time = 0
	local posy = 0

	--第一个浮台的定点位置
	local onePosy = 145
	--起伏的时间间隔
	local timeJg = 0.28
	--平台间的高度间隔
	local posyJg = 80
	--回弹幅度
	local posyPy = 20

	local lst = CallModuleFunc(ModuleType.TOWER, "TowerMonsterStar")

	local tower_game_level = CallModuleFunc(ModuleType.TOWER, "GetCurMapLevel")

	for i,v in ipairs(self.nodeLst) do
		time = time + timeJg
		posy = posy + posyJg
		if i == 1 then
			posy =  onePosy
		end

		CPPActionManager.MoveTo( v , time, v:GetPositionX() , posy + posyPy , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
			CPPActionManager.MoveTo( v , time + 0.1 , v:GetPositionX() , v:GetPositionY() - posyPy )
		end) )


		if tower_game_level == 0 then

			if lst[i].game_level > 1 then
				if self.monster.mode[i] then
					self.monster.mode[i]:SetGray(true)
					self.monster.mode[i]:SetToPlayTime(0)
					self.monster.effectCon[i]:SetVisible(false)
				end
				self:CloseCall(i)
			else

				self:OpenCall(i)
				CPPActionManager.DelayTo( v , time + 0.5 , GlobalCallbackMgr:AddCallBackFunc(function()
					ActionManager.GetInstance():UpDownMoveAnimation(v , MathTool.GetRandom(1, 5) , MathTool.GetRandom(1, 20) )
				end))	
			end

		elseif lst[i].game_level > (tower_game_level+1) then
			if self.monster.mode[i] then
				self.monster.mode[i]:SetGray(true)
				self.monster.mode[i]:SetToPlayTime(0)
				self.monster.effectCon[i]:SetVisible(false)
			end
			self:CloseCall(i)

		elseif lst[i].game_level <= tower_game_level then
			if self.monster.mode[i] then
				self.monster.mode[i]:GetNode():SetVisible(false)
				self.monster.mu[i]:SetVisible(true)
				self.monster.effectCon[i]:SetVisible(false)
			end
			self:CloseCall(i)

		else
			self:OpenCall(i , true)

			CPPActionManager.DelayTo( v , time + 2 , GlobalCallbackMgr:AddCallBackFunc(function()
				ActionManager.GetInstance():UpDownMoveAnimation(v , MathTool.GetRandom(1, 5) , MathTool.GetRandom(1, 20) )
			end))
		end
	end
end

-- 平台稳定
function LogicGUI.TempTowerMonster:TerraceStabilize()

	local time = 0
	local posy = 0

	local lst = CallModuleFunc(ModuleType.TOWER, "TowerMonsterStar")

	local tower_game_level = CallModuleFunc(ModuleType.TOWER, "GetCurMapLevel")

	for i,v in ipairs(self.nodeLst) do
		time = time + 1
		posy = posy + 80
		if i == 1 then
			posy =  135
		end
		v:SetPosition( v:GetPositionX() , posy )

		if tower_game_level == 0 then
			if lst[i].game_level > 1 then

				self.monster.mode[i]:SetGray(true)
				self.monster.mode[i]:SetToPlayTime(0)
				self.monster.effectCon[i]:SetVisible(false)

				self:CloseCall(i)
			else
				self:OpenCall( i )
				CPPActionManager.DelayTo( v , time + 0.5 , GlobalCallbackMgr:AddCallBackFunc(function()
					ActionManager.GetInstance():UpDownMoveAnimation(v , MathTool.GetRandom(1, 5) , MathTool.GetRandom(1, 20) )
				end))
			end
		elseif lst[i].game_level > (tower_game_level+1) then

			self.monster.mode[i]:SetGray(true)
			self.monster.mode[i]:SetToPlayTime(0)
			self.monster.effectCon[i]:SetVisible(false)

			self:CloseCall(i)

		elseif lst[i].game_level <= tower_game_level then

			self.monster.mode[i]:GetNode():SetVisible(false)
			self.monster.mu[i]:SetVisible(true)
			self.monster.effectCon[i]:SetVisible(false)

		    local tollgateStar = CallModuleFunc(ModuleType.TOWER, "GetTollgateStar" , lst[i].game_level)

		    for j=1 , tollgateStar.star do
				if lst[i].game_level > (tower_game_level - 1) and self.type then
					--按钮
					CPPActionManager.DelayTo( self.monster.grey[i][j] , 0.2 * j , GlobalCallbackMgr:AddCallBackFunc(function()
					    local effectId, effectCon = BaseView.__CreateEffect(self, {res_name = "UI_pataxingxing",loop = false , end_callback = function ( ... )
							self.monster.grey[i][j]:SetGray(false)
					    end }, self.monster.grey[i][j] )
					    PosTool.Center(effectCon)
					end))
				else
					self.monster.grey[i][j]:SetGray(false)
				end
		    end

		    self:CloseCall(i)
		
		else
			self:OpenCall(i)

			CPPActionManager.DelayTo( v , time + 0.5 , GlobalCallbackMgr:AddCallBackFunc(function()
				ActionManager.GetInstance():UpDownMoveAnimation(v , MathTool.GetRandom(1, 5) , MathTool.GetRandom(1, 20) )
			end))
		end
	end
end

-- 稳定置灰假装还没切
function LogicGUI.TempTowerMonster:TerraceAscheStabilize()

	local posy = 0

	local lst = CallModuleFunc(ModuleType.TOWER, "TowerMonsterStar",true)

	local tower_game_level = CallModuleFunc(ModuleType.TOWER, "GetCurMapLevel")

	for i,v in ipairs(self.nodeLst) do
		posy = posy + 80
		if i == 1 then
			posy =  135
		end
		v:SetPosition( v:GetPositionX() , posy )

		local cnf = CPPGameLib.GetConfig("GameLevel", lst[i].map_list[1])

		self.monster.mode[i]:GetNode():SetVisible(false)
		self.monster.effectCon[i]:SetVisible(false)
		self.monster.name[i]:SetVisible(false)
		self.monster.cut[i]:SetVisible(false)
		self.monster.mu[i]:SetVisible(true)

	    local tollgateStar = CallModuleFunc(ModuleType.TOWER, "GetTollgateStar" , lst[i].game_level)

	    if tollgateStar then
		    for j=1 , tollgateStar.star do
				if lst[i].game_level > (tower_game_level - 1) and self.type then
					--按钮
					CPPActionManager.DelayTo( self.monster.grey[i][j] , 0.2 * j , GlobalCallbackMgr:AddCallBackFunc(function()
					    local effectId, effectCon = BaseView.__CreateEffect(self, {res_name = "UI_pataxingxing",loop = false , end_callback = function ( ... )
							self.monster.grey[i][j]:SetGray(false)
					    end }, self.monster.grey[i][j] )
					    PosTool.Center(effectCon)
					end))
				else
					self.monster.grey[i][j]:SetGray(false)
				end
		    end
		end

	    self:CloseCall(i)
	end
end

-- 重置一下位置
function LogicGUI.TempTowerMonster:TerraceReset()

	local lst = CallModuleFunc(ModuleType.TOWER, "TowerMonsterStar")

	for i,v in ipairs(self.nodeLst) do

		local cnf = CPPGameLib.GetConfig("GameLevel", lst[i].map_list[1])
		self.monster.mode[i]:SetInfoID( cnf.icon_id )
		self.monster.mode[i]:GetNode():SetVisible(true)
		self.monster.mu[i]:SetVisible(false)
		self.monster.effectCon[i]:SetVisible(true)
		self.monster.name[i]:SetVisible(true)
		self.monster.cut[i]:SetVisible(true)
		self.monster.kailBg[i]:SetVisible(false)
		self.monster.kail[i]:SetVisible(false)
		v:SetPosition( v:GetPositionX() , -310.47 )

		self:CloseCall(i)
	end

	CPPActionManager.DelayTo( self.nodeLst[1] , 0.5 , GlobalCallbackMgr:AddCallBackFunc(function()
		self:TerraceMove()
	end))
end

-- 稳定隐藏假装被掏空
function LogicGUI.TempTowerMonster:TerraceEmptyingStabilize()

	local time = 0
	local posy = 0

	local lst = CallModuleFunc(ModuleType.TOWER, "TowerMonsterStar")

	local tower_game_level = CallModuleFunc(ModuleType.TOWER, "GetCurMapLevel")

	for i,v in ipairs(self.nodeLst) do
		time = time + 1
		posy = posy + 80
		if i == 1 then
			posy =  135
		end
		v:SetPosition( v:GetPositionX() , posy )

		self.monster.mode[i]:GetNode():SetVisible(false)
		self.monster.effectCon[i]:SetVisible(false)
		self.monster.name[i]:SetVisible(false)
		self.monster.cut[i]:SetVisible(false)
		self:CloseCall(i)
	end
end

--开启喊话(弹出)
function LogicGUI.TempTowerMonster:OpenCall( i , key )
	self:CloseCall(i)
	if self.monster.kailBg[i] then
		CPPActionManager.DelayTo( self.monster.kailBg[i] , 0.55 , GlobalCallbackMgr:AddCallBackFunc( function ( ... )
			self.monster.kailBg[i]:SetVisible(true)
			self.monster.kail[i]:SetVisible(true)	
			ActionManager.GetInstance():ElasticAnimation( self.monster.kailBg[i] , 0.6 )
			ActionManager.GetInstance():ElasticAnimation( self.monster.kail[i] , 0.6 )
		end ) )
	end
end

--关闭喊话
function LogicGUI.TempTowerMonster:CloseCall(i)
	if self.monster.kailBg[i] then
		CPPActionManager.StopAllActions( self.monster.kailBg[i] )
		CPPActionManager.StopAllActions( self.monster.kail[i] )
		self.monster.kailBg[i]:SetVisible(false)
		self.monster.kail[i]:SetVisible(false)
	end
end


--攻打选择难度
function LogicGUI.TempTowerMonster:OnModes( tab )
	LogicGUI.TemplTowerDifficultySelect.New(tab)
end

--新手指引特殊操作检测(爬塔界面-选择关卡（爬塔关卡索引）)
function LogicGUI.TempTowerMonster:CheckSectionOperateSP()
	local lst = CallModuleFunc(ModuleType.TOWER, "TowerMonsterStar")
	for i, v in ipairs(lst) do
		CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.TOWER_CHOOSE_GAMELEVEL, self.monster.layout_lead[i], v.game_level) 	
	end
end
