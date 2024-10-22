--
-- @brief 资源战模块 
-- @author: yjg
-- @date: 2018年8月8日11:00:41
--

Modules = Modules or {}

Modules.ResourceWarModule = Modules.ResourceWarModule or BaseClass(BaseModule)

function Modules.ResourceWarModule:__init()

end

function Modules.ResourceWarModule:__delete()
end

-- 消息说明:请求玩家资源战数据 消息Id: 27001
function Modules.ResourceWarModule:PlayerResourceWarDataReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:请求玩家资源战数据 消息Id: 27001")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_PLAYER_RESOURCE_WAR_DATA_REQ)
	GlobalNetAdapter:OnSend( data ) 
end

-- 消息说明:玩家资源战数据响应 消息Id: 27002

-- "chapter__H",           --章节
-- "resource_id__H",       --资源id
-- "occupy_start_time__I",     --占领开始时间
-- "occupy_over_time__I",      --占领结束时间
-- "keep_occupy_times__H",     --延长的占领次数
-- "last_base_reward_time__I", --最近的基础奖励时间
-- "last_extra_reward_time__I",    --最近的额外奖励时间
-- "cur_extra_reward_need_time__H",--当前额外奖励需要的时间(分钟)

-- "technology_level__H",      --科技等级
-- "guild_accupy_num__H",      --工会占领人数(玩家自己占领的资源所在的章节)
-- "guild_name"

function Modules.ResourceWarModule:PlayerResourceWarDataResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:玩家资源战数据响应 消息Id: 27002")
	self.resourceWarData = info
	GlobalEventSystem:FireNextFrame( Macros.Event.ResourceWarModule.RESOURCE_WAR_DATA )
end

--获得当前章节
function Modules.ResourceWarModule:GetChapter( )
	if self.resourceWarData then
		return self.resourceWarData.chapter
	end
end

---------------------------------------------------------------------------------------------------------
--敌人
---------------------------------------------------------------------------------------------------------

-- 消息说明:请求资源战敌人列表 消息Id: 27003
function Modules.ResourceWarModule:ResourceWarEnemyListReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:请求资源战敌人列表 消息Id: 27003")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_RESOURCE_WAR_ENEMY_LIST_REQ)
	GlobalNetAdapter:OnSend( data ) 
end

-- 消息说明:请求敌人列表响应 消息Id: 27004
function Modules.ResourceWarModule:ResourceWarEnemyListResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:请求敌人列表响应 消息Id: 27004")
	self.enemyList = info.enemy_list
	GlobalEventSystem:FireNextFrame( Macros.Event.ResourceWarModule.RESOURCE_WAR_ENEMY )
end

--敌人列表
function Modules.ResourceWarModule:GetEnemyLst()
	local t1 = {}
	for i , v in ipairs( self.enemyList ) do
		local type_ = nil
		if v.resource_id and v.resource_id ~= 0 then
			local cnf = self:GetResourceWarResourceCnf( v.resource_id )
			type_ = cnf.resource_type 
		end

		table.insert( t1 , { 
								roleIcon = v.head_img , 
								name = v.role_name,
								lv = v.level ,
								power = v.power ,
								union = v.guild_name ,
								time = v.attack_times ,
								type = type_ ,
								id = v.resource_id , 
								chapter = v.chapter,
							} )
	end
	return t1
end

--敌人数据本地处理
function Modules.ResourceWarModule:SetEnemyData( lst )
	--资源点信息
	local resourceCnf = self:GetResourceWarResourceCnf( lst.id )
	if not self.chapterData[ lst.id ] then
		self.chapterData[ lst.id ] = {}
	end
	--资源
	self.chapterData[ lst.id ].role_id = 0 
	self.chapterData[ lst.id ].role_name = lst.name
	self.chapterData[ lst.id ].power = lst.power
	self.chapterData[ lst.id ].guild_name = lst.union
	self.chapterData[ lst.id ].server_id = " "
	self.chapterData[ lst.id ].level = lst.lv
	self.chapterData[ lst.id ].head_img = lst.roleIcon
	self.chapterData[ lst.id ].resource_id = lst.id

	if resourceCnf and resourceCnf.consume_lst then
		--已占领时长
		self.chapterData[ lst.id ].occupyDuration = resourceCnf.consume_lst[1].time * 60 
	end
	return self.chapterData[ lst.id ]
end


---------------------------------------------------------------------------------------------------------
--战报 
---------------------------------------------------------------------------------------------------------
-- 消息说明:请求战报 消息Id: 27005
function Modules.ResourceWarModule:ResourceWarBattleRecordReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:请求战报 消息Id: 27005")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_RESOURCE_WAR_BATTLE_RECORD_REQ)
	GlobalNetAdapter:OnSend( data ) 
end

-- 消息说明:请求战报响应 消息Id: 27006
function Modules.ResourceWarModule:ResourceWarBattleRecordResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:请求战报响应 消息Id: 27006")
	self.battleRecord = info.battle_record_list
	GlobalEventSystem:FireNextFrame( Macros.Event.ResourceWarModule.RESOURCE_BATTLE_RECORD )
end

--战报列表
function Modules.ResourceWarModule:GetSharkLst()
	local t1 = {}
	for i , v in ipairs( self.battleRecord ) do
		local cnf = self:GetResourceWarResourceCnf( v.resource_id )
		table.insert( t1 , { 
								time = v.battle_time , 
								name_1 = v.attacker_name ,
								name_2 = v.defender_name ,
								section = v.chapter ,
								target = cnf.name
							} )
	end
	return t1
end

---------------------------------------------------------------------------------------------------------
--矿场相关
---------------------------------------------------------------------------------------------------------
-- 消息说明:放弃占领的资源 消息Id: 27007
function Modules.ResourceWarModule:ResourceWarGiveUpOccupyReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:放弃占领的资源 消息Id: 27007")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_RESOURCE_WAR_GIVE_UP_OCCUPY_REQ)
	GlobalNetAdapter:OnSend( data ) 
end

-- 消息说明:放弃占领的资源响应 消息Id: 27008
function Modules.ResourceWarModule:ResourceWarGiveUpOccupyResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:放弃占领的资源响应 消息Id: 27008")
	GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("resources_war_abandon_2" ) )
end

---------------------------------------------------------------------------------------------------------
--延长
---------------------------------------------------------------------------------------------------------
-- 消息说明:延长占领的资源 消息Id: 27009
function Modules.ResourceWarModule:ResourceWarKeepOccupyReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:延长占领的资源 消息Id: 27009")
	self.keepOccupy = info
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_RESOURCE_WAR_KEEP_OCCUPY_REQ)
	GlobalNetAdapter:OnSend( data ) 
end

-- 消息说明:延长占领的资源响应 消息Id: 27010
function Modules.ResourceWarModule:ResourceWarKeepOccupyResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:延长占领的资源响应 消息Id: 27010")
	GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("resources_war_abandon_3",math.floor(self.keepOccupy/60) ) )
end


--获得延长的占领次数
function Modules.ResourceWarModule:SetKeepOccupyTimes( num )
	if self.resourceWarData then
		self.resourceWarData.keep_occupy_times = num
	end
end

--获得延长的占领次数
function Modules.ResourceWarModule:GetKeepOccupyTimes( )
	if self.resourceWarData then
		return self.resourceWarData.keep_occupy_times + 1
	end
end

--获得占领开始时间
function Modules.ResourceWarModule:GetOccupyStartTime()
	if self.resourceWarData then
		return self.resourceWarData.occupy_start_time
	end
end

--获得占领结束时间
function Modules.ResourceWarModule:GetOccupyOverTime()
	if self.resourceWarData then
		return self.resourceWarData.occupy_over_time
	end
end


--获得当前额外奖励的档位
function Modules.ResourceWarModule:GetLengthenGears( id )
	local resourceCnf = self:GetResourceWarResourceCnf( id )
	-- GetOccupyStartTime
	local t1 = {}
	for i , v in ipairs( resourceCnf.consume_lst ) do
		local time = self:GetOccupyStartTime() + ( v.time * 60 )
		table.insert( t1 , time )
	end

	local nowtime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")

	local index = 0
	for i , v in ipairs( t1 ) do
		if v > nowtime then
			index = index + 1
			break
		end
	end
	return index
end

--获得我占领着的资源id
function Modules.ResourceWarModule:GetOccupyId()
	if self.resourceWarData then
		return self.resourceWarData.resource_id
	end
end

---------------------------------------------------------------------------------------------------------
--搜索章节
---------------------------------------------------------------------------------------------------------
-- 消息说明:请求资源战章节数据 消息Id: 27011
function Modules.ResourceWarModule:ResourceWarChapterDataReq( info )
	if not info.chapter or info.chapter == 0 then
		info.chapter = 1
	end

	CPPGameLib.PrintTable("xy" , info , "消息说明:请求资源战章节数据 消息Id: 27011")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_RESOURCE_WAR_CHAPTER_DATA_REQ)
	data.chapter = info.chapter
	GlobalNetAdapter:OnSend( data ) 

	--预想：保存当前章节
	self.atPresentChapter = info.chapter
end

-- 消息说明:请求资源战章节数据响应 消息Id: 27012
function Modules.ResourceWarModule:ResourceWarChapterDataResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:请求资源战章节数据响应 消息Id: 27012")
	--数据整理
	self:ChapterDataDispose( info )
    --预想：取得该章节后 刷新界面
	GlobalEventSystem:FireNextFrame( Macros.Event.ResourceWarModule.RESOURCE_CHAPTER )
end

--获得当前章节
function Modules.ResourceWarModule:GetAtPresentChapter( )
	if self.atPresentChapter then
		return self.atPresentChapter
	else
		return 1
	end
end

--获得当前资源信息
function Modules.ResourceWarModule:GetChapterData( id , all )
	if not self.chapterData then
		return 
	end

	for i , v in pairs( self.chapterData ) do
		if v.resource_id == id then
			return v
		end
	end

	if all then
		return self.chapterData
	end
end


--获得当前占领军团
function Modules.ResourceWarModule:GetOccupyUnion( )
	local lst = self:GetChapterData( nil , true )

	if not lst then
		return nil , 0
	end
	local t1 = {}
	for i , v in pairs( lst ) do
		if v.guild_name then
			if not t1[v.guild_name] then
				t1[v.guild_name] = 0
			end
			t1[v.guild_name] = t1[v.guild_name] + 1
		end
	end

	local guild_name = nil
	local index = 0
	for i , v in pairs( t1 ) do
		if v >= ConfigAdapter.Common.GetResourceWarUnionNum() then
			guild_name = i
			index = v
		end
	end

	return guild_name , index
end


--章节资源数据处理
function Modules.ResourceWarModule:ChapterDataDispose( info )
	-- self.chapterData = info.chapter_resource_list
	self.chapterData = {}

	--章节信息
	local chapterCnf = self:GetResourceWarChapterCnf( info.chapter )

	local lst = {}
	for i , v in ipairs( chapterCnf.resource_id_list ) do
		table.insert( lst , v )
	end

	table.insert( lst , self:GetOccupyId() )

	for i , v in ipairs( lst ) do
		--资源点信息
		local resourceCnf = self:GetResourceWarResourceCnf( v )
		if not self.chapterData[ v ] then
			self.chapterData[ v ] = {}
		end

		--军团加成(默认没有)
		self.chapterData[ v ].unionAdd =  nil 
		--科技加成(默认没有)
		self.chapterData[ v ].skillAdd =  nil
		--资源
		self.chapterData[ v ].resource_id = v 

		if resourceCnf and resourceCnf.consume_lst then
			--已占领时长
			self.chapterData[ v ].occupyDuration = resourceCnf.consume_lst[1].time * 60 
		end
	end

	local role_info = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo")
	--有自己
	if self:GetOccupyId() ~= 0 then
		table.insert( info.chapter_resource_list , { 
														resource_id = self:GetOccupyId() ,
														role_id = role_info.role_id , 
														role_name = role_info.role_name , 
														power = role_info.battle_value , 
														level = role_info.level , 
														server_id = role_info.server_id , 
														head_img = role_info.head_icon , 
														guild_name = self.resourceWarData.guild_name , 
													 } )
	end

	--已经占领
	for i , v in ipairs( info.chapter_resource_list ) do
		--对应资源点
		if self.chapterData[ v.resource_id ] then
			self.chapterData[ v.resource_id ].role_id = v.role_id
			self.chapterData[ v.resource_id ].role_name = v.role_name
			self.chapterData[ v.resource_id ].power = v.power
			self.chapterData[ v.resource_id ].server_id = v.server_id
			self.chapterData[ v.resource_id ].level = v.level
			self.chapterData[ v.resource_id ].guild_name = v.guild_name
			self.chapterData[ v.resource_id ].head_img = v.head_img
		end

		local role_id = CallModuleFunc(ModuleType.ROLE, "GetRoleID")
		--是自己
		if v.role_id == role_id then
			local start = self:GetOccupyStartTime()
			local n_Time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")

			--已占领时长
			local n_Time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
			self.chapterData[ v.resource_id ].occupyDuration = n_Time - self:GetOccupyStartTime() 

			--军团加成(默认没有)
			self.chapterData[ v.resource_id ].unionAdd =  nil
			--科技加成(默认没有)
			self.chapterData[ v.resource_id ].skillAdd =  nil
		end
	end
end

--根据id和时间获得当前产出和预计收益
function Modules.ResourceWarModule:ToIDAndTimeGetOutput( id , time , num )
	local cnf = self:GetResourceWarResourceCnf( id )
    local t1 = {}
    --奖励获得次数
    local awardTime_now = math.floor(time / cnf.base_reward_interval_time)
   	--当前产出
   	if cnf.basics_lst then
	    t1.now_lst = {}
		for i , v in ipairs( cnf.basics_lst ) do
			table.insert( t1.now_lst , { item_id = v.item_id , item_num = v.item_num * awardTime_now } )
		end
	end

	--预计产出
	if #cnf.consume_lst ~= 0 then
		num = num or ( ( self:GetKeepOccupyTimes() or 0 ) )
		print("xy" , "num=-============" , num )
	    local maxTime = cnf.consume_lst[ num ]
		local awardTime_max = math.floor( maxTime.time / cnf.base_reward_interval_time)
	   	t1.max_lst = {}
		for i , v in ipairs( cnf.basics_lst ) do
			table.insert( t1.max_lst , { item_id = v.item_id , item_num = v.item_num * awardTime_max } )
		end
		t1.max_time = maxTime
	end

	--额外奖励
    if #cnf.extra_reward ~= 0 then
    	t1.extra_lst = {}
    	for i , v in ipairs( cnf.extra_reward ) do
    		table.insert( t1.extra_lst , { award = v , time = cnf.consume_lst[i] } )
    	end
    	t1.bar_max = cnf.bar_max
    end

	return t1
end

--获取资源点的加成
function Modules.ResourceWarModule:ToIdGetAddition( id )

	local info = self:GetChapterData( id )

	local role_id = CallModuleFunc(ModuleType.ROLE, "GetRoleID")
	local t1 = {}
	if info.role_id and info.role_id == role_id then

		local cnf = self:GetResourceWarGuildProfitCnf( self:GetGuildAccupyNum() )
		local percentage = 0
		if self:GetGuildAccupyNum() ~= 0 and cnf then
			percentage = cnf.base_reward_add_rate
		end
		table.insert( t1 , { type = CPPGameLib.GetString("resources_war_UnionAddition") , value = MathTool.TransProbCoeffPercentage(  percentage  ) } )

		local cnf = self:GetResourceWarTechnology( self:GetTechnologyLevel() )
		local percentage = 0
		if cnf then
			percentage = cnf.base_reward_add_rate
		end
		table.insert( t1 , { type = CPPGameLib.GetString("resources_war_SkillAddition") , value = MathTool.TransProbCoeffPercentage(  cnf.base_reward_add_rate  ) } )
	else
		table.insert( t1 , { type = " " , value = 0 } )
		table.insert( t1 , { type = " " , value = 0 } )
	end

	return t1
end

---------------------------------------------------------------------------------------------------------
--搜索同款 
---------------------------------------------------------------------------------------------------------
-- 消息说明:请求资源战同类资源 消息Id: 27013
function Modules.ResourceWarModule:ResourceWarSameResourceReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:请求资源战同类资源 消息Id: 27013")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_RESOURCE_WAR_SAME_RESOURCE_REQ)
	data.resource_type = info.resource_type
	GlobalNetAdapter:OnSend( data ) 
end

-- 消息说明:请求资源战同类资源响应 消息Id: 27014
-- "resource_type__H",--资源类型
-- "resource_data_list__T__chapter@H##resource_id@H", -- 资源数据列表
function Modules.ResourceWarModule:ResourceWarSameResourceResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:请求资源战同类资源响应 消息Id: 27014")

	local t1 = {}
	for i , v in ipairs( info.resource_data_list ) do
		t1[v.chapter .. v.resource_id ] = true
	end

	local _ , max = self:GetResourceWarChapterCnf( nil , true )

	local index = nil
	for chapter = 1 , max do
		local chapterCnf = self:GetResourceWarChapterCnf( chapter )
		for i , v in ipairs( chapterCnf.resource_id_list ) do
			local resourceCnf = self:GetResourceWarResourceCnf( v )
			if resourceCnf.resource_type == info.resource_type then
				if not t1[ chapter .. v ] then
					index = chapter
					break
				end
			end
		end
		if index ~= nil then
			break
		end
	end

    --请求章节数据
    local info = {}
    info.chapter = index
    self:ResourceWarChapterDataReq( info )
end

---------------------------------------------------------------------------------------------------------
--公会资源 
---------------------------------------------------------------------------------------------------------
-- 消息说明:请求资源战工会资源 消息Id: 27015
function Modules.ResourceWarModule:ResourceWarGuildResourceReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:请求资源战工会资源 消息Id: 27015")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_RESOURCE_WAR_GUILD_RESOURCE_REQ)
	GlobalNetAdapter:OnSend( data ) 
end

-- 消息说明:请求资源战工会资源响应 消息Id: 27016
function Modules.ResourceWarModule:ResourceWarGuildResourceResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:请求资源战工会资源响应 消息Id: 27016")
	local t1 = {}
	for i , v in ipairs( info.resource_data_list ) do
		if not t1[ v.chapter ] then
			t1[ v.chapter ] = {}
			t1[ v.chapter ].chapter = v.chapter
			t1[ v.chapter ].resource_id = v.resource_id
			t1[ v.chapter ].number = 0
		end
		t1[ v.chapter ].number = t1[ v.chapter ].number + 1
	end

	self.guildResource = {}
	for i , v in pairs( t1 ) do
		table.insert( self.guildResource , v )
	end
	table.sort( self.guildResource , function( a , b )
		return a.chapter < b.chapter
	end )

	GlobalEventSystem:FireNextFrame( Macros.Event.ResourceWarModule.RESOURCE_UNION )
end

--军团消息列表
function Modules.ResourceWarModule:GetUnionLst()
	local t1 = {}
	for i , v in ipairs( self.guildResource ) do
		local cnf = self:GetResourceWarGuildProfitCnf( v.number )
		local addition = "0%"
		if cnf then
			addition = MathTool.TransProbCoeffPercentage(cnf.base_reward_add_rate) .. "%"	
		end
		table.insert( t1 , { 
								section = v.chapter ,
								num = v.number , 
								addition = addition			
							} )
	end 

	return t1
end

-- 获得当前公会人数
function Modules.ResourceWarModule:GetGuildAccupyNum( )
	if self.resourceWarData then
		return self.resourceWarData.guild_accupy_num
	else
		return 0
	end
end

---------------------------------------------------------------------------------------------------------
--技能
---------------------------------------------------------------------------------------------------------
-- 消息说明:资源战科技升级请求 消息Id: 27017
function Modules.ResourceWarModule:ResourceWarTechnologyUpgradeReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:资源战科技升级请求 消息Id: 27017")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_RESOURCE_WAR_TECHNOLOGY_UPGRADE_REQ)
	GlobalNetAdapter:OnSend( data ) 
end

-- 消息说明:资源战科技升级响应 消息Id: 27018
function Modules.ResourceWarModule:ResourceWarTechnologyUpgradeResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:资源战科技升级请求 消息Id: 27017")

	GlobalEventSystem:FireNextFrame( Macros.Event.ResourceWarModule.RESOURCE_SKILL )
end

-- 获得当前公会技能等级
function Modules.ResourceWarModule:GetTechnologyLevel( )
	if self.resourceWarData then
		return self.resourceWarData.technology_level
	else
		return 0
	end
end

-- 获得累计时长
function Modules.ResourceWarModule:GetAddUpDuration( )
	if self.resourceWarData then
		return math.floor( self.resourceWarData.total_accupy_time / 60 / 60 )
	else
		return 0
	end
end




---------------------------------------------------------------------------------------------------------
--商店
---------------------------------------------------------------------------------------------------------
--资源购买
function Modules.ResourceWarModule:ShopBuyRequest( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 资源购买 消息Id:")
	-- data.type =  info.type or 1
	-- data.param1 =  info.param1 or 0
	CallModuleFunc(ModuleType.SHOP,"ShopBuyRequest", info )
end

--资源购买
function Modules.ResourceWarModule:ResourceWarBuyResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 资源响应 消息Id:")
	if self.resourceWarRequest[info.param1] then
		self.resourceWarRequest[info.param1] = { goods_id = info.param1 , buy_num = self.resourceWarRequest[info.param1].buy_num +  info.param2 }
	else
		self.resourceWarRequest[info.param1] = { goods_id = info.param1 , buy_num = info.param2 }
	end

	GlobalEventSystem:FireNextFrame(Macros.Event.ResourceWarModule.RESOURCE_SHOP , true )

	local cnf = CPPGameLib.GetConfig("ResourceWarShop", info.param1 )
	local t1 = {}
    table.insert(t1,{id = cnf.item_id , num = cnf.item_num * info.param2 })
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
end

--资源商店记录
function Modules.ResourceWarModule:ResourceWarBuyNotesRequest( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 资源商店记录 消息Id:")
	if not self.resourceWarRequest then
		self.resourceWarRequest = {}
		CallModuleFunc(ModuleType.SHOP,"ShopBuyRecordRequest" , Macros.Game.SHOP.RESOURCE_WAR_SHOP)
	end
end

--深渊商店记录响应
function Modules.ResourceWarModule:ResourceWarBuyNotesResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 深渊商店记录响应 消息Id:")
	local t1 = {}
	for i,v in ipairs(info) do
		self.resourceWarRequest[v.goods_id] = v 
	end

	GlobalEventSystem:FireNextFrame(Macros.Event.ResourceWarModule.RESOURCE_SHOP)
end

--购买记录
function Modules.ResourceWarModule:GetResourceWarRequest( id )
	if not self.resourceWarRequest then
		self.resourceWarRequest = {}
	end
	return self.resourceWarRequest[ id ]
end

--资源商店
function Modules.ResourceWarModule:ShopCnf()
	if self.shopCnf then
		return 
	end
	self.shopCnf = {}
	local cnf = CPPGameLib.GetConfig("ResourceWarShop", nil , nil , true)
	for i , v in pairs( cnf ) do
		table.insert( self.shopCnf , v )
	end

	table.sort( self.shopCnf, function ( a , b )
		return a.sort_id < b.sort_id
	end )
end

--获得资源商店表
function Modules.ResourceWarModule:GetShopCnf()
	local t1 = {}
	local t2 = {}
	for i , v in ipairs( self.shopCnf ) do
		local info = self:GetResourceWarRequest( v.id ) 
		if info then
			if v.buy_num_max ~= 0 and info.buy_num >= v.buy_num_max then
				table.insert( t2 , v )
			else
				table.insert( t1 , v )
			end
		else
			table.insert( t1 , v )
		end
	end
	for i,v in ipairs(t2) do
		table.insert( t1 , v )
	end
	return t1
end

--获得章节数据
function Modules.ResourceWarModule:GetResourceWarChapterCnf( chapter , max )
	local ResourceWarChapterCnf = CPPGameLib.GetConfig("ResourceWarChapter", tonumber(chapter) , false )

	local ended = 0 
	if max then
		local cnf = CPPGameLib.GetConfig("ResourceWarChapter", nil , nil , true )
		for i , v in pairs( cnf ) do
			ended = ended + 1
		end
	end

	return ResourceWarChapterCnf , ended 
end

--获得资源点数据
function Modules.ResourceWarModule:GetResourceWarResourceCnf( id )

	if not self.resourceWarResource then
		self.resourceWarResource = {}
	end

	if not self.resourceWarResource[id] then

		local ResourceWarResource = CPPGameLib.GetConfig("ResourceWarResource", id , false )

		self.resourceWarResource[id] = {}

		if ResourceWarResource then
			self.resourceWarResource[id] = CPPGameLib.CopyTbl(ResourceWarResource)
			--延长占领
			self.resourceWarResource[id].consume_lst = {}
			if self.resourceWarResource[id].keep_occupy_cost_diamond_list then
				local time = 0
				for i , v in ipairs(self.resourceWarResource[id].keep_occupy_cost_diamond_list ) do
					time = time + self.resourceWarResource[id].extra_reward_need_time_list[i]
					table.insert( self.resourceWarResource[id].consume_lst , { item = v , time = time , need_time = self.resourceWarResource[id].extra_reward_need_time_list[i] } )
				end
				self.resourceWarResource[id].keep_occupy_cost_diamond_list = nil
				self.resourceWarResource[id].extra_reward_need_time_list = nil
			end

			--基础奖励
			self.resourceWarResource[id].basics_lst = {}
			if self.resourceWarResource[id].base_reward_currency then
				table.insert( self.resourceWarResource[id].basics_lst , { item_id = Macros.Game.RolePropType.RESOURCE_WAR , item_num = self.resourceWarResource[id].base_reward_currency } )
				self.resourceWarResource[id].base_reward_currency = nil
			end

			--额外奖励
			self.resourceWarResource[id].extra_reward = {}
			if self.resourceWarResource[id].extra_reward_item_id_list then
				for i , v in ipairs(self.resourceWarResource[id].extra_reward_item_id_list ) do
					local t1 = {}
					for j , vv  in ipairs( v ) do
						table.insert( t1 , { item_id = vv , item_num = self.resourceWarResource[id].extra_reward_item_num_list[i][j] } )
					end
					table.insert( self.resourceWarResource[id].extra_reward , t1 )
				end
				self.resourceWarResource[id].extra_reward_item_id_list = nil
				self.resourceWarResource[id].extra_reward_item_num_list = nil
			end
		end
	end

	return self.resourceWarResource[id]
end

--获得资源造型数据
function Modules.ResourceWarModule:GetResourceWarModellingCnf( id )
	local ResourceWarModelling = CPPGameLib.GetConfig("ResourceWarModelling", id , false )
	return ResourceWarModelling
end

--获得资源点数据
function Modules.ResourceWarModule:GetResourceWarGuildProfitCnf( num , all)
	local ResourceWarGuildProfit = CPPGameLib.GetConfig("ResourceWarGuildProfit", num , false )
	if all then
		local cnf = CPPGameLib.GetConfig("ResourceWarGuildProfit" , nil , nil , true )
		local index = 0
		for i , v in pairs(cnf) do
			index = index + 1
		end
		ResourceWarGuildProfit = CPPGameLib.GetConfig("ResourceWarGuildProfit", index , false )
	end
	return ResourceWarGuildProfit
end

--获得科技数据
function Modules.ResourceWarModule:GetResourceWarTechnology( id )
	local ResourceWarTechnology = nil
	if not id then
		ResourceWarTechnology = CPPGameLib.GetConfig("ResourceWarTechnology" , nil , nil , true )
	else
		ResourceWarTechnology = CPPGameLib.GetConfig("ResourceWarTechnology" , id )
		if not ResourceWarTechnology then
			ResourceWarTechnology = {}
			ResourceWarTechnology.base_reward_add_rate = 0
		end
	end
	return ResourceWarTechnology
end

--设置保存选中者信息
function Modules.ResourceWarModule:SetPitchOnData( info )
	self.pitchOnData = info
end

--设置保存选中者信息
function Modules.ResourceWarModule:GetPitchOnData( )
	return self.pitchOnData
end

--------------------------------------------------------------------------------------------------------------------------------
--战斗判断
--------------------------------------------------------------------------------------------------------------------------------
function Modules.ResourceWarModule:CheckBattleCondition( resourceId )
        local resourceCnf = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetResourceWarResourceCnf" , resourceId )
        local value = CallModuleFunc( ModuleType.ROLE , "GetPropValue" , Macros.Game.RolePropType.DIAMOND )
        if resourceCnf.cost_diamond <= value then
            local occupyId = self:GetOccupyId()
            if not occupyId or occupyId == 0 then

                --保存选中
                local chapterData = self:GetChapterData( resourceId )
                self:SetPitchOnData( chapterData )

                
            	return true
            else
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString( "resources_war_abandon_4" ) )
            end
        else
            local txt = "currency_insufficient_" .. Macros.Game.RolePropType.DIAMOND
            GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString(txt) )
        end
end