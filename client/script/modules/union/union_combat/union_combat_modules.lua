--
-- @brief 公会模块 (工会战)
-- @author: yjg
-- @date: 2016年10月14日10:39:24
--

Modules = Modules or {}

Modules.UnionModule = Modules.UnionModule or BaseClass(BaseModule)
Modules.UnionModule.BASE = 1
Modules.UnionModule.BOOD = 2
Modules.UnionModule.CENTRE = 3
Modules.UnionModule.NEXT = 4

-- 消息说明:请求工会战角色数据 消息Id: 27101
function Modules.UnionModule:GuildWarRoleDataReq( info )
	CPPGameLib.PrintTable("xy" , info,"消息说明: 请求工会战角色数据: 消息Id: 27101")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_WAR_ROLE_DATA_REQ)
	GlobalNetAdapter:OnSend(data)   
end

-- 消息说明:请求工会战角色数据 消息Id: 27102
function Modules.UnionModule:GuildWarRoleDataResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 请求工会战角色数据: 消息Id: 27102")
	self.guildWarRoleData = info
end

-- 消息说明:请求撤离 消息Id: 27113
function Modules.UnionModule:GuildWarEvacuateReq( info )
	CPPGameLib.PrintTable("xy" , info,"消息说明: 请求撤离: 消息Id: 27113")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_WAR_EVACUATE_REQ)
	data.star_id = info.star_id
	GlobalNetAdapter:OnSend(data)  
end

-- 消息说明:撤离响应 消息Id: 27114
function Modules.UnionModule:GuildWarEvacuateResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 撤离响应: 消息Id: 27114")
	GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_combat_evacuateTips") )
	self:GuildWarStarListReq()
end

----------------------------------------------------------------------------------------------------------------------------------------------------
--星球
----------------------------------------------------------------------------------------------------------------------------------------------------
-- 消息说明:请求星球列表 消息Id: 27107
function Modules.UnionModule:GuildWarStarListReq( info )
	CPPGameLib.PrintTable("xy" , info,"消息说明: 请求星球列表: 消息Id: 27107")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_WAR_STAR_LIST_REQ)
	GlobalNetAdapter:OnSend(data)   
end

-- 消息说明:请求星球列表响应 消息Id: 27108
function Modules.UnionModule:GuildWarStarListResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 请求星球列表响应: 消息Id: 27108")
	self.starList = {}
	for i , v in ipairs( info.star_list ) do
		self.starList[ v.star_id ] = v
	end
	GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_STAR_LST)
end

-- 消息说明:求星球详细 消息Id: 27119
function Modules.UnionModule:GuildWarStarDetailReq( info )
	CPPGameLib.PrintTable("xy" , info,"消息说明: 求星球详细: 消息Id: 27119")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_WAR_STAR_DETAIL_REQ)
	data.star_id = info.star_id
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明:星球详细响应(todo::响应的内容待完善,得根据客户的表现需要) 消息Id: 27120
function Modules.UnionModule:GuildWarStarDetailResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 星球详细响应: 消息Id: 27120")
	self.starDetail = info
	GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_STAR_INFO)
end

-- 消息说明: 星球数据更新通知 消息Id: 27123
-- "star_id__H",       --星球ID
-- "tower_id__H",      --塔ID
function Modules.UnionModule:GuildWarStarUpdateNotify( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:星球数据更新通知 消息Id:27123 ")
	local viewInfo = GlobalViewMgr:GetCurViewInfo()
	if viewInfo then
		if viewInfo.view_type == BaseViewType.UNION_COMBAT then
			self:GuildWarStarDetailReq( info )
		elseif viewInfo.view_type == BaseViewType.UNION_COMBAT_BATTLEGROUND then
			self:GuildWarStarDetailReq( info )
			self:GuildWarTowerListReq( info )
		end
	end
end

--获得星球信息
function Modules.UnionModule:GetUnionCombatStarDetail( )
	return self.starDetail
end

-- 用id获取是不是我的公会
function Modules.UnionModule:GuildWarIsMyUnion( id )
	if self.starList[ id ] then
		return self.starList[ id ]
	end
	return {}
end

----------------------------------------------------------------------------------------------------------------------------------------------------
--占领
----------------------------------------------------------------------------------------------------------------------------------------------------
-- 消息说明:请求本军团已占领的星球 消息Id: 27103
function Modules.UnionModule:GuildWarOccupyStarListReq( info )
	CPPGameLib.PrintTable("xy" , info,"消息说明: 请求本军团已占领的星球: 消息Id: 27103")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_WAR_OCCUPY_STAR_LIST_REQ)
	GlobalNetAdapter:OnSend(data)   
end

-- 消息说明:请求本军团已占领的星球响应 消息Id: 27104
function Modules.UnionModule:GuildWarOccupyStarListResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 请求本军团已占领的星球响应: 消息Id: 27104")
	self.occupyLst = {}
	for i , v in ipairs( info.occupy_star_list ) do
		table.insert( self.occupyLst , { id = v.star_id , is_have_reward = v.is_have_reward } )
	end

	GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_OCCUPY)
end

-- 消息说明:请求领取星球奖励 消息Id: 27117
-- 1.低级星球领取 2.高级星球领取 3.一键领取
function Modules.UnionModule:GuildWarTakeStarRewardReq( info )
	CPPGameLib.PrintTable("xy" , info,"消息说明: 请求领取星球奖励: 消息Id: 27117")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_WAR_TAKE_STAR_REWARD_REQ)
	data.take_type = info.take_type
	data.take_star_list = info.take_star_list
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明:领取星球奖励响应 消息Id: 27118
-- 1.低级星球领取 2.高级星球领取 3.一键领取
-- "take_type__H",     --领取类型
-- "take_star_list__T__star_id@H", -- 要领取的星球列表
function Modules.UnionModule:GuildWarTakeStarRewardResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 领取星球奖励响应: 消息Id: 27118")

	--获得弹窗
	local t1 = {}
	local t2 = {}
	for i , v in ipairs( info.take_star_list ) do
		t2[ v.star_id ] = true
		local starCnf = self:GetGuildWarStarCnf( v.star_id )

    	local lst = starCnf.local_product_id_list or starCnf.daily_reward_id_list
    	local num = starCnf.local_product_num_list or starCnf.daily_reward_num_list

		for j , vv in ipairs( lst ) do
			table.insert( t1 , { id = vv , num = num[j] } )
		end
	end
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

    --数据变更
    for i , v in ipairs( self.occupyLst ) do
    	if t2[ v.id ] then
    		v.is_have_reward = 0
    	end
    end
    GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_OCCUPY_AWARD)
end

--已占有列表
function Modules.UnionModule:GetUnionCombatOccupy()
	local t2 = {}
	for i , v in ipairs( self.occupyLst ) do
		local cnf = self:GetGuildWarStarCnf( v.id )
		table.insert( t2 , { id = v.id , is_have_reward = v.is_have_reward , cnf = cnf } )
	end
	table.sort( t2 , function( a , b )
 		return a.id > b.id
	end )	
	return t2
end

--获取占有数据
function Modules.UnionModule:ToIdGetUnionCombatOccupyInfo( id )
	if not self.occupyLst then
		self.occupyLst = {}
	end

	local t2 = nil
	for i , v in ipairs( self.occupyLst ) do
		if v.id == id then
			t2 = v
			break
		end
	end
	return t2
end

--获取占有数据
function Modules.UnionModule:GetUnionCombatOccupyMax( )
	local normal = self:GetGuildWarCommonCnf( 13 )
	return normal
end

----------------------------------------------------------------------------------------------------------------------------------------------------
--布防
----------------------------------------------------------------------------------------------------------------------------------------------------
-- 消息说明:请求布防 消息Id: 27111
function Modules.UnionModule:GuildWarOrganizeDefenseReq( info )
	CPPGameLib.PrintTable("xy" , info,"消息说明: 请求布防: 消息Id: 27111")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_WAR_ORGANIZE_DEFENSE_REQ)
	data.star_id = info.star_id
	data.tower_type = info.tower_type
	data.role_id = info.role_id
	data.is_to_battle = info.is_to_battle
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明:布防响应 消息Id: 27112
function Modules.UnionModule:GuildWarOrganizeDefenseResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 布防响应: 消息Id: 27112")

	if info.is_to_battle == 1 then
		local memberInfo = self:ToIdGetUnionMemberInfo( info.role_id )
		local defenseInfo = self:ToTypeGetTowerDefenseInfo( info.tower_type ) 
		local towerInfo = self:GetGuildWarTowerCnf( defenseInfo.tower_id )

		self.tower_roleLst[info.role_id] = {}
		self.tower_roleLst[info.role_id].role_id = info.role_id
		self.tower_roleLst[info.role_id].be_attack_times = towerInfo.role_defense_times
		
		self.tower_roleLst[info.role_id].head_img_id = memberInfo.head_icon
		self.tower_roleLst[info.role_id].power = memberInfo.battle_value
		self.tower_roleLst[info.role_id].role_name = memberInfo.name

		self.tower_roleLst[info.role_id].tower_id = defenseInfo.tower_id

		self:UpSetTowerList( info , true )
	else
		self.tower_roleLst[info.role_id] = nil

		self:UpSetTowerList( info , false )
	end

	GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_combat_deploy") )
	
	GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_DEFENSE , info.role_id ) 
end

--本地刷新防御塔信息
function Modules.UnionModule:UpSetTowerList( info , type_ )
	local memberInfo = self:ToIdGetUnionMemberInfo( info.role_id )
	local defenseInfo = self:ToTypeGetTowerDefenseInfo( info.tower_type ) 
	local towerInfo = self:GetGuildWarTowerCnf( defenseInfo.tower_id )
	if type_ == true then
		if self.tower_list[ defenseInfo.tower_id ] then
			table.insert( self.tower_list[ defenseInfo.tower_id ].defender_list.__list , { 
			        role_id = info.role_id ,
			        be_attack_times = 0 ,
			        role_name = memberInfo.name ,
			        power = memberInfo.battle_value ,
			        head_img_id = memberInfo.head_icon ,
			        tower_id = defenseInfo.tower_id ,
				})
		end
	else
		for i , v in ipairs( self.tower_list[ defenseInfo.tower_id ].defender_list.__list ) do
			if v.role_id == info.role_id then
				table.remove( self.tower_list[ defenseInfo.tower_id ].defender_list.__list , i )
				break
			end
		end
	end
end

--按照防御塔类型获得塔信息
function Modules.UnionModule:ToTypeGetTowerDefenseInfo( type_ )
	for k , v in pairs( self.tower_list ) do
		local towerInfo = self:GetGuildWarTowerCnf( v.tower_id )
		if towerInfo.tower_type == type_ then
			return v
		end
	end
end

----------------------------------------------------------------------------------------------------------------------------------------------------
--攻打
----------------------------------------------------------------------------------------------------------------------------------------------------
-- 消息说明: 请求塔列表 消息Id: 27121
function Modules.UnionModule:GuildWarTowerListReq( info )
	CPPGameLib.PrintTable("xy" , info,"消息说明: 请求塔列表: 消息Id: 27121")
	local starInfo = self:GetUnionCombatStarDetail()
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_WAR_TOWER_LIST_REQ)
	data.star_id = starInfo.star_id
	GlobalNetAdapter:OnSend(data)   
end

-- 消息说明: 工会战塔列表响应 消息Id: 27122
function Modules.UnionModule:GuildWarTowerListResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 工会战塔列表响应: 消息Id: 27122")
	self.tower_list = {}
	self.tower_roleLst = {}
	for i , v in ipairs( info.tower_list ) do
		self.tower_list[v.tower_id] = v
		for _ , defenderInfo in ipairs( v.defender_list.__list ) do
			defenderInfo.tower_id = v.tower_id
			self.tower_roleLst[ defenderInfo.role_id ] = defenderInfo
		end
	end

	GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_TOWER)
end

--按ID获得该星球战斗数据
function Modules.UnionModule:ToIdGetUnionCombatStarInfo( id )
	local t1 = {}
	local function NotGuildId( ... )
		local cnf = self:GetGuildWarStarCnf( id )
		table.insert( t1 , { 
								guild_id = 0 , 
								union_name = cnf.name , 
								union_icon = cnf.sp[1] , 
								isPerson = false 
							} )
	end

	local info = self:GetUnionCombatStarDetail()
	if info.attack_guild_id == 0 and info.is_over == 1 then
		NotGuildId()
	else
		table.insert( t1 , { 
								star_id = info.star_id ,
								guild_id = info.attack_guild_id , 
								union_name = info.attack_guild_name , 
								union_icon = info.attack_guild_icon_id , 
								server_id = info.attack_guild_server_id ,
								isPerson = true ,
							} )
	end

	if info.belong_guild_id == 0 then
		NotGuildId()
	else
		table.insert( t1 , { 
								star_id = info.star_id ,
								guild_id = info.belong_guild_id , 
								union_name = info.belong_guild_name , 
								union_icon = info.belong_guild_icon_id , 
								server_id = info.belong_guild_server_id ,
								isPerson = true 
							} )
	end

	return t1
end

--获得该星球防御塔列表
function Modules.UnionModule:ToIdGetUnionCombatTowerInfo( )
	local starInfo = self:GetUnionCombatStarDetail()
	local warStarInfo = self:GetGuildWarStarCnf(starInfo.star_id)

	local towLst = {}
	for i , v in ipairs( warStarInfo.tower_list ) do
		local towerInfo = self:GetGuildWarTowerCnf( v )
		table.insert( towLst , towerInfo )
	end

	table.sort( towLst , function( a , b )
		return a.tower_type < b.tower_type
	end )

	return towLst
end

--获得防御塔防御信息
function Modules.UnionModule:ToIdGetTowerDefenseInfo( tower_id )
	return self.tower_list[ tower_id ]
end

--获得防御塔防御信息/防御人数
function Modules.UnionModule:ToIdGetTowerRoleInfo( role_id , tpwerType )

	local index = 0
	if tpwerType then
		for k , v in pairs( self.tower_roleLst ) do
			local towerCnf = self:GetGuildWarTowerCnf( v.tower_id )
			if towerCnf.tower_type == tpwerType then
				index = index + 1
			end
		end
	end

	return self.tower_roleLst[ role_id ] , index
end

--防守列表
function Modules.UnionModule:ToIdGetGuardLst( id )
	local starInfo = self:GetUnionCombatStarDetail()
	local occupyInfo = self:ToIdGetUnionCombatOccupyInfo( starInfo.star_id )
	starInfo = self:ToIdGetUnionCombatStarInfo( starInfo.star_id )
	local towerCnf = self:GetGuildWarTowerCnf( id )
	local defenseInfo = self:ToIdGetTowerDefenseInfo( id )

	local lst = {}
	--由我占领
	if occupyInfo then
		local memberLst = self:GetUnionMemberLst()
		for i , v in ipairs( memberLst ) do
			local towerId = nil
			if self:ToIdGetTowerRoleInfo(v.role_id) then
				towerId = self:ToIdGetTowerRoleInfo(v.role_id).tower_id
			end

			table.insert( lst , { 
									role_id = v.role_id ,
									position = v.position ,
									server = starInfo[2].server_id,
									star_id = starInfo[2].star_id ,
									tower_id = id , 
									head_icon = v.head_icon , 
									name = v.name , 
									battle_value = v.battle_value , 
									isPerson = true ,
									towerId = towerId , 
									beattack_times = towerCnf.role_defense_times - ( v.be_attack_times or 0 )
								} )
		end

		table.sort( lst, function( a , b )
 			if a.battle_value > b.battle_value then
	 			return a.battle_value > b.battle_value
	 		elseif a.battle_value == b.battle_value then
	 			if a.position > b.position then
		 			return a.position > b.position
		 		elseif a.position == b.position then
		 			return a.role_id < a.role_id
		 		end
	 		end
		end )	

	--有人占领
	elseif starInfo[2].isPerson == true then
		local defenderList = defenseInfo.defender_list.__list
		for i , v in ipairs( defenderList ) do
			table.insert( lst , { 
									role_id = v.role_id ,
									server = starInfo[2].server_id,
									star_id = starInfo[2].star_id ,
									tower_id = id , 
									head_icon = v.head_img_id , 
									name = v.role_name , 
									battle_value = v.power , 
									isPerson = true ,
									beattack_times = towerCnf.role_defense_times - v.be_attack_times
								} )
		end

		table.sort( lst, function( a , b )
			if a.beattack_times > b.beattack_times then
	 			return a.beattack_times > b.beattack_times
	 		elseif a.beattack_times == b.beattack_times then
	 			if a.battle_value > b.battle_value then
		 			return a.battle_value > b.battle_value
		 		elseif a.battle_value == b.battle_value then
		 			return a.role_id < a.role_id
		 		end
	 		end
		end )	
	--没人占领
	else
		local gameInfo = CPPGameLib.GetConfig("GameLevel", towerCnf.map_id )
		local monsterInfo = CPPGameLib.GetConfig("Monster", gameInfo.icon_id )
		local beattack_times = 0
		if defenseInfo then
			beattack_times = defenseInfo.monster_beattack_times
		end
		table.insert( lst , { 
								star_id = starInfo.star_id ,
								tower_id = id , 
								head_icon = monsterInfo.head_icon or monsterInfo.hero_id , 
								name = gameInfo.name , 
								battle_value = gameInfo.recommended_power or 0 , 
								isPerson = false ,
								beattack_times = towerCnf.monster_defense_times - beattack_times
							} )
	end
	return lst
end

--设置攻击对象
function Modules.UnionModule:SetAttackObject( info )
	self.attackObjectInfo = info
end

--获得攻击对象
function Modules.UnionModule:GetAttackObject( )
	return self.attackObjectInfo
end

--判断能不能攻击
function Modules.UnionModule:JudgeCanItAttack( tower_id )
	local isBase = false
	local isAttack = false
	local isOver = false

	local towerCnf = self:GetGuildWarTowerCnf( tower_id )
	--是基地
	if towerCnf and towerCnf.tower_type == Modules.UnionModule.BASE then
		isBase = true
	end
	--是基地
	if isBase == true then
		for i , v in pairs( self.tower_list ) do
			if v.tower_id ~= tower_id then
				local _ ,isOver_ = self:ToIdGetTowerType( v.tower_id )
				if isOver_ == true then
					--获得基地的状态
					isAttack ,isOver = self:ToIdGetTowerType( tower_id )
					break
				end
			end
		end
	else
		isAttack ,isOver = self:ToIdGetTowerType( tower_id )
	end

	return isBase , isAttack , isOver 
end

--按照id获得其状态
function Modules.UnionModule:ToIdGetTowerType( tower_id )
	local isAttack = false
	local isOver = nil
	local towerInfo = self:ToIdGetTowerDefenseInfo( tower_id )
	local towerCnf = self:GetGuildWarTowerCnf( tower_id )
	--攻击次数
	local time = 0
	local person = 0
	for i , v in ipairs( towerInfo.defender_list.__list ) do
		time = time + v.be_attack_times
		person = person + 1
	end

	--耐久度为空
	if towerInfo.cur_durability <= 0 then
		isAttack = false
		isOver = true
	--对面是人
	elseif person ~= 0 and time >= ( towerCnf.role_defense_times * person ) then
		isAttack = false
		isOver = true
	--对面是怪
	elseif towerInfo.monster_beattack_times >= towerCnf.monster_defense_times then
		isAttack = false
		isOver = true
	else
		isAttack = true
		isOver = false
	end

	local starInfo = self:GetUnionCombatStarDetail()
	local starLst = self:ToIdGetUnionCombatStarInfo( starInfo.star_id )

	if starLst[1].guild_id == 0 then
		isAttack = true
		isOver = false
	elseif starLst[2].guild_id ~= 0 and person == 0 then
		isAttack = false
		isOver = true
	end

	return isAttack , isOver
end

--获得我是不是战场的主角
function Modules.UnionModule:GetBattlegroundProtagonist( )
	local starInfo = self:GetUnionCombatStarDetail()
	local starLst = self:ToIdGetUnionCombatStarInfo( starInfo.star_id )
	local t1 = {}
	for i , v in ipairs( starLst ) do
		t1[v.guild_id] = true
	end

	local unionData = CallModuleFunc(ModuleType.UNION , "UnionData")
	if unionData and t1[unionData.guild_id] then
		return true
	end
end

--获得该角色所在的防御塔
function Modules.UnionModule:GetRolePlaceTower( roleId )

end

----------------------------------------------------------------------------------------------------------------------------------------------------
--宣战
----------------------------------------------------------------------------------------------------------------------------------------------------

-- 消息说明:请求宣战 消息Id: 27109
function Modules.UnionModule:GuildWarDeclareWarReq( info )
	CPPGameLib.PrintTable("xy" , info,"消息说明: 请求宣战: 消息Id: 27109")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_WAR_DECLARE_WAR_REQ)
	data.star_id = info.star_id
	GlobalNetAdapter:OnSend(data)   
end

-- 消息说明:宣战响应 消息Id: 27110
function Modules.UnionModule:GuildWarDeclareWarResp( info )
	CPPGameLib.PrintTable("xy" , info,"消息说明: 宣战响应: 消息Id: 27110")

	self:GuildWarStarDetailReq( info )
	
	local info = self:GetUnionCombatStarDetail()

	self.declare_war_star_list[ info.star_id ] = {}
	self.declare_war_star_list[ info.star_id ].server_id = info.belong_guild_server_id
	self.declare_war_star_list[ info.star_id ].active = info.belong_guild_active
	self.declare_war_star_list[ info.star_id ].star_id = info.star_id
	self.declare_war_star_list[ info.star_id ].guild_name = info.belong_guild_name

	GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_combat_declareWarTips") )
	
	GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_WAR_UP)
end

-- 消息说明:请求本军团已宣战的星球列表 消息Id: 27105
function Modules.UnionModule:GuildWarDeclareWarStarListReq( info )
	CPPGameLib.PrintTable("xy" , info,"消息说明: 请求本军团已宣战的星球列表: 消息Id: 27105")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_WAR_DECLARE_WAR_STAR_LIST_REQ)
	GlobalNetAdapter:OnSend(data)   
end

-- 消息说明:请求本军团已宣战的星球列表响应 消息Id: 27106
function Modules.UnionModule:GuildWarDeclareWarStarListResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 请求本军团已宣战的星球列表响应: 消息Id: 27106")
	self.declare_war_star_list = {}
	for i , v in pairs( info.declare_war_star_list ) do
		self.declare_war_star_list[v.star_id] = v
	end
end

--获得我宣战的对象的列表
function Modules.UnionModule:GetUnionCombatMyDeclareWarLst( id )
	if id then
		return self.declare_war_star_list[ id ] 
	else
		return self.declare_war_star_list
	end
end

--获得宣战上限次数
function Modules.UnionModule:GetUnionCombatMyDeclareWarMax()
	local normal = self:GetGuildWarCommonCnf( 2 )
	return normal
end

--获得宣战列表
function Modules.UnionModule:GetUnionCombatALLDeclareWarLst()
	local t1 = {}
	if self:GetUnionCombatStarDetail() then
		for i , v in ipairs( self:GetUnionCombatStarDetail().declare_war_guild_list ) do
			table.insert( t1 , { union_name = v.guild_name , union_icon = v.icon_id , server = v.server_id , active = v.active } ) 
		end
	end
	return t1
end

--宣战时间处理
function Modules.UnionModule:UnionCombatDeclareWarTimeHandle( data )
	local n_nian, n_yue, n_ri, n_shi, n_fen, n_miao, n_week = TimeTool.GetDateFromTimeStamp( CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime") )
	local n_time = tonumber(n_week .. n_shi .. n_fen)
	local type_ = nil
	if n_time >= data.begin_time and n_time < data.end_time  then
		type_ = "xuanZhan"
	end
	return type_ , data.end_time
end

--对战时间处理
function Modules.UnionModule:UnionCombatFightTimeHandle( data )
	local n_nian, n_yue, n_ri, n_shi, n_fen, n_miao, n_week = TimeTool.GetDateFromTimeStamp( CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime") )
	local n_time = tonumber(n_week .. n_shi .. n_fen)
	local type_ = nil
	if n_time >= data.begin_time and n_time < data.end_time  then
		type_ = "duiZhan"
	end
	return type_ , data.end_time
end

--决定进攻处理
function Modules.UnionModule:UnionCombatDecideTimeHandle( data )
	local week = string.sub(data.decide_attacker_time , 0 , 1 )
   	local time = string.sub(data.decide_attacker_time , 2 , 5 )
   	local t1 = { 
	   				["week"] = week , 
	   				["time"] = time 
	   			}
	return t1
end

--开采许可处理
function Modules.UnionModule:UnionCombatExploitTimeHandle( data )
	local n_nian, n_yue, n_ri, n_shi, n_fen, n_miao, n_week = TimeTool.GetDateFromTimeStamp( CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime") )
	local type_ = nil
	for i , v in ipairs( data.take_reward_time ) do
		if n_week == v then
			type_ = "kaiCai"
			break
		end
	end
	return type_
end

--------------------------------------------
--积分
--------------------------------------------
-- 消息说明:请求购买对高级星球挑战次数 消息Id: 27115
function Modules.UnionModule:GuildWarBuyHighStarBattleTimesReq( info )
	CPPGameLib.PrintTable("xy" , info,"消息说明: 请求购买对高级星球挑战次数: 消息Id: 27115")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_WAR_BUY_HIGH_STAR_BATTLE_TIMES_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明:购买对高级星球挑战次数响应 消息Id: 27116
function Modules.UnionModule:GuildWarBuyHighStarBattleTimesResp( info ) 
	CPPGameLib.PrintTable("xy" , info , "消息说明: 购买对高级星球挑战次数响应: 消息Id: 27116")
	self.guildWarRoleData.buy_high_star_battle_times = self.guildWarRoleData.buy_high_star_battle_times + 1
	GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("shop_success"))
	GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_BUY_TIME)
end

--获得今日剩余次数(低级)
function Modules.UnionModule:GetUnionCombatLowTime()
	local normal = self:GetGuildWarCommonCnf( 1 )
	local time = normal - self.guildWarRoleData.used_low_star_battle_times
	return time
end

--获得今日剩余次数(高级)
function Modules.UnionModule:GetUnionCombatHighTime()
	local normal = self:GetGuildWarCommonCnf( 1 )
	local time = (normal - self.guildWarRoleData.used_high_star_battle_times) + self.guildWarRoleData.buy_high_star_battle_times
	return time
end

--获得今日剩余可购买次数
function Modules.UnionModule:GetUnionBuyTime()
	local normal = self:GetGuildWarCommonCnf( 12 )
	local time = normal - self.guildWarRoleData.buy_high_star_battle_times
	return time
end

----------------------------------------------------------------------------------------------------------------------------------------------------
--杂项
----------------------------------------------------------------------------------------------------------------------------------------------------
--杂项
function Modules.UnionModule:GetGuildWarCommonCnf( id )
	local cnf = CPPGameLib.GetConfig("GuildWarCommon", id )
	return cnf.int_value
end

--星球塔信息
function Modules.UnionModule:GetGuildWarTowerCnf( id )
	local cnf = CPPGameLib.GetConfig("GuildWarTower", id )
	return cnf
end

--获得该星球等级
function Modules.UnionModule:GetGuildWarStarLevel( id )
	local cnf = CPPGameLib.GetConfig("GuildWarStar", id )
	local type_ = nil
	if cnf then
		if cnf.star_type <= 2 then 
			type_ = "low"
		else
			type_ = "tall"
		end
	end
	return type_
end

--星球信息
function Modules.UnionModule:GetGuildWarStarCnf( id , all )
	local cnf = CPPGameLib.GetConfig("GuildWarStar", id , false )
	if all then
		cnf = CPPGameLib.GetConfig("GuildWarStar", nil , nil , true )
	end
	return cnf
end

--时间处理
function Modules.UnionModule:GetGuildWarVSActivityTimeCnf(  )
	local cnf = CPPGameLib.GetConfig("GuildWarVSActivityTime", nil , nil , true )
	local t1 = {}
	local endTimeCnf = nil
	for i , v in ipairs( cnf ) do
		--宣战时间
		if v.time_type == 1 then
			local type_ , time = self:UnionCombatDeclareWarTimeHandle( v )
			table.insert( t1 , type_ )
			if type_ and not endTimeCnf then
				endTimeCnf = time
			end
		--对战时间
		elseif v.time_type == 2 then
			local type_ , time = self:UnionCombatFightTimeHandle( v )
			table.insert( t1 , type_ )
			if type_ and not endTimeCnf then
				endTimeCnf = time
			end
		--决定进攻	
		elseif v.time_type == 3 then
			local type_ = self:UnionCombatDecideTimeHandle( v )
			-- table.insert( t1 , type_ )
		--开采时间	
		elseif v.time_type == 4 then	
			local type_ = self:UnionCombatExploitTimeHandle( v )
			table.insert( t1 , type_ )
		end	
	end

	if not endTimeCnf then
		return t1 , 0
	end

	--结束时间
	local e_xingqi = string.sub( endTimeCnf , 0 , 1 )
	local e_shi = string.sub( endTimeCnf , 2 , 3 )
	local e_fen = string.sub( endTimeCnf , 4 , 5 )
	local time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	local n_nian, n_yue, n_ri, n_shi, n_fen, n_miao, n_xingqi = TimeTool.GetDateFromTimeStamp( time )
	local dayGap = ( (e_xingqi - n_xingqi)*24*60*60 ) 
	local x_nian , x_yue, x_ri , x_shi , x_fen , x_miao , x_xingqi = TimeTool.GetDateFromTimeStamp( time + dayGap )
	local endTime = time + ( dayGap - ( x_shi * 60 * 60 ) - ( x_fen * 60 ) - x_miao )
	endTime = endTime + ( e_shi * 60 * 60 ) + ( e_fen * 60 )
	local e_nian , e_yue , e_ri , e_shi , e_fen , e_miao , e_xingqi = TimeTool.GetDateFromTimeStamp( endTime )

	return t1 , endTime
end

--积分星球时间处理
function Modules.UnionModule:UnionCombatIntegralStarTime( )
	local endTime = self:GetGuildWarCommonCnf( 4 )
	local beginTime = self:GetGuildWarCommonCnf( 5 )
	local finishTime = self:GetGuildWarCommonCnf( 6 )
	local awardBeginTime = self:GetGuildWarCommonCnf( 7 )
	local awardfinishTime = self:GetGuildWarCommonCnf( 8 )

	local n_nian, n_yue, n_ri, n_shi, n_fen, n_miao, n_xingqi = TimeTool.GetDateFromTimeStamp( CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime") )

	local shi = string.format( "%02d" , n_shi )
	local fen = string.format( "%02d" , n_fen )
	local n_time = shi .. fen

	local type_ = "end"
	local time = 0

	n_time = tonumber(n_time)
	beginTime = tonumber(beginTime)
	finishTime = tonumber(finishTime)
	awardBeginTime = tonumber(awardBeginTime)
	awardfinishTime = tonumber(awardfinishTime)

	if n_time >= beginTime and n_time <= finishTime then
		type_ = "begin"
		time = TimeTool.CalcTimeStampFrmDate( n_nian..n_yue..n_ri..finishTime )
	elseif n_time >= awardBeginTime and n_time <= awardfinishTime then
		type_ = "award"
		time = TimeTool.CalcTimeStampFrmDate( n_nian..n_yue..n_ri..awardfinishTime )
	end

	return type_ , time , endTime
end

--设置能不能触摸星球
function Modules.UnionModule:SetIsCanTouchStar( type_ )
	self.isCanTouchStar = type_
end

--获得能不能触摸星球
function Modules.UnionModule:GetIsCanTouchStar(  )
	return self.isCanTouchStar
end

--保存当前所选星球位置
function Modules.UnionModule:SetSaveLetGoPos( pos , offset , touch )
	self.letGoPos = pos
	self.offset = offset
	self.touch = touch
end

--保存当前所选星球位置
function Modules.UnionModule:GetSaveLetGoPos( pos )
	return self.letGoPos , self.offset , self.touch  
end