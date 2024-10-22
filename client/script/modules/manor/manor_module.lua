--[[
%% @module: 领地模块
%% @author: yjg
%% @created: 2016年10月8日09:52:19
--]]

Modules = Modules or {}

Modules.ManorModule = Modules.ManorModule or BaseClass(BaseModule)

function Modules.ManorModule:__init()
	--领地列表
	self.ManorLst = {}
	--奖励列表
	self.awardLst = {}
	--好友
	self.friendLst = {}
	--我的领地信息
	self.MyManor = {}
end

function Modules.ManorModule:__delete()
end

--------------------------------------------------------------------------------------------------------------------------------
--协议
--------------------------------------------------------------------------------------------------------------------------------
--玩家领地数据请求
function Modules.ManorModule:PlayerLandDataReq(info)
	if not info then
		info = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo").role_id
	end

	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_PLAYER_LAND_DATA_REQ)
	data.target_id = info
	GlobalNetAdapter:OnSend(data)
end

--玩家领地数据请求响应
function Modules.ManorModule:PlayerLandDataResp(info)
	self.ManorLst = info
	if info.target_id == CallModuleFunc(ModuleType.ROLE , "GetRoleInfo").role_id or info.target_id == 0 then
		info.target_id = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo").role_id
		self.MyManor = info
	end
	
	--设置城市状态
	self:SetCityType( info.city_list )

	--更新系统推送(领地的巡逻时间完成)
	MsgPushHandle.UpdateLocalPushFun(MsgPushHandle.PushKey.MANOR_PATROL_FINISHED)

	GlobalEventSystem:FireNextFrame(Macros.Event.ManorModule.MANOR_UP)
end

--巡逻请求
function Modules.ManorModule:LandPatrolReq(info)
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_LAND_PATROL_REQ)
	data.city_id = info.city_id
	data.patrol_type = info.patrol_type
	data.time_type = info.time_type
	data.patrol_hero_uid = info.patrol_hero_uid
	GlobalNetAdapter:OnSend(data)

	self.patrolType = true
end

--巡逻响应
function Modules.ManorModule:LandPatrolResp(info)
end

--领取巡逻奖励
function Modules.ManorModule:LandGetPatrolAwardReq(info)
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_LAND_GET_PATROL_AWARD_REQ)
	data.city_id = info.city_id
	GlobalNetAdapter:OnSend(data)
end

--领取巡逻奖励响应
function Modules.ManorModule:LandGetPatrolAwardResp(info)
	local t1 = {}
	for i , v in ipairs( self.awardLst[info.city_id].all ) do
		table.insert( t1 , v )
	end
 
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
    self.awardLst[info.city_id] = nil
	self:PlayerLandDataReq()
end

--领地技能升级(开启,提升)
function Modules.ManorModule:LandSkillLevelupReq(info)
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_LAND_SKILL_LEVELUP_REQ)
	data.city_id = info.id
	GlobalNetAdapter:OnSend(data)
end

--领地技能升级响应(开启,提升)
function Modules.ManorModule:LandSkillLevelupResp(info)
	self:UpSkill(info)
end

--领地镇压暴动
function Modules.ManorModule:LandSuppressRiotReq(info)
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_LAND_SUPPRESS_RIOT_REQ)
	data.city_id = info.city_id
	data.friend_id = info.friend_id
	GlobalNetAdapter:OnSend(data)
end

--领地镇压暴动响应
function Modules.ManorModule:LandSuppressRiotResp(info)
	self.MyManor.riot_suppress_times = self.MyManor.riot_suppress_times + 1

	local cnf = CPPGameLib.GetConfig( "LandCity" , info.city_id )
	local t1 = {}
	table.insert(t1,{type = Macros.Game.RolePropType.DIAMOND , num = cnf.help_suppress_diamond})
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
    
    GlobalEventSystem:FireNextFrame( Macros.Event.ManorModule.MANOR_FRIEND )

end

--请求好友领地列表
function Modules.ManorModule:FriendLandListReq(info)
	print("请求好友领地列表")
	GlobalNetAdapter:OnSend(GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_FRIEND_LAND_LIST_REQ))
end

--请求好友领地列表响应
function Modules.ManorModule:FriendLandListResp(info)
	self:UpFriend(info.friend_land_list)
end

--领地城池详情请求
function Modules.ManorModule:LandCityDetailsReq(info)
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_LAND_CITY_DETAILS_REQ)
	data.role_id = info.role_id
	data.city_id = info.city_id
	GlobalNetAdapter:OnSend(data)
end

--领地城池详情请求响应
function Modules.ManorModule:LandCityDetailsResp(info)
	self:UpPatrolDetails(info)
end

--------------------------------------------------------------------------------------------------------------------------------
--接口
--------------------------------------------------------------------------------------------------------------------------------
--是不是刚从巡逻界面出来
function Modules.ManorModule:SetPatrolType(type)
	self.patrolType = type
end

--是不是刚从巡逻界面出来
function Modules.ManorModule:GetPatrolType(info)
	return self.patrolType
end

--设置城市状态
function Modules.ManorModule:SetCityType(info)
	self.cityType = {}
	for i,v in ipairs( info ) do
		local type_ = "without"
		local value = 0
		self.cityType[v.city_id] = {}
		--没有在巡逻
		if v.start_patrol_time == 0 then
			type_ = "NotPatrol"
		--在巡逻
		else
			type_ = "Patrol"

			local cnf = CPPGameLib.GetConfig( "LandPatrol" , v.patrol_type )
			--总时间(分钟)
			local time = cnf["time_type_data_"..v.time_type][1] * 60 
			--结束时间
			value = v.start_patrol_time + time
		end
		self.cityType[v.city_id].patrol = { type = type_ , value = value }

		--有没有暴动
		if v.is_riot == 0 then
			type_ = "NotRiot"
		else
			type_ = "Riot"
		end
		self.cityType[v.city_id].riot = { type = type_ }

		--有没有等级
		value = v.skill_level
		self.cityType[v.city_id].skill_level = { value = value }

		--没有可以领取的奖励
		if v.is_can_get_reward == 0 then
			type_ = "NotAward"
		else
			type_ = "Award"
		end 
		self.cityType[v.city_id].award = { type = type_ }

		--前置
		local cnf = CPPGameLib.GetConfig( "LandCity" , v.city_id )
		self.cityType[v.city_id].preCity = { value = cnf.pre_city_id }
	end
end

--获得
function Modules.ManorModule:GetCityType(id)
	if not self.cityType then
	else
		return self.cityType[id]
	end
end

--获得信息
function Modules.ManorModule:GetRoleInfo()
	return self.ManorLst
end

--获取这个城会掉什么 （城市id）
function Modules.ManorModule:GetCityPack(city_id)
	local cnf = CPPGameLib.GetConfig("LandPatrolEvent", city_id)
	local t1 = {}
	local t2 = {}
	local t3 = {}
	for i,v in ipairs(cnf.id_list) do
			local type_ = ObjIDTool.GetInfo(v.object_id)
			print( "v.object_id",v.object_id )
			local m_type_ = ConfigAdapter.Item.GetCurrencyInfo(v.object_id)
			if type_ == Macros.Game.ObjType.HERO then
				table.insert( t1 , { id = v.object_id , num = v.object_num , is_only_this_city_create = v.is_only_this_city_create } )
			elseif m_type_ then
				table.insert( t2 , { id = v.object_id , num = v.object_num , is_only_this_city_create = v.is_only_this_city_create } )
			else
				table.insert( t3 , { id = v.object_id , num = v.object_num , is_only_this_city_create = v.is_only_this_city_create } )
			end
	end

	table.sort( t1, function( a , b )
		return a.id < b.id
	end )

	table.sort( t2, function( a , b )
		return a.id < b.id
	end )

	table.sort( t3, function( a , b )
		return a.id < b.id
	end )

	for i,v in ipairs(t2) do
		table.insert( t1 , v )
	end

	for i,v in ipairs(t3) do
		table.insert( t1 , v )
	end

	return t1
end

--获得技能
function Modules.ManorModule:GetSkillLst()
	local citySkillLst = {}
	--获得各个城市的状态
	for i,v in ipairs(self.ManorLst.city_list) do
		citySkillLst[v.city_id] = v.skill_level
	end

	local t1 = {}
	--按表全下
	for i,v in ipairs(CPPGameLib.GetConfig("LandCity", nil, nil , true )) do
		table.insert( t1 , {
								id = i ,
								--名字
								name = v.name , 
								--技能名字
								ruins_skill_name = v.ruins_skill_name,
								--背景
								build_bg = v.build_bg , 
								--当前等级
								nowLevel = citySkillLst[i] or 0 ,
								--当前加成
								nowAddition = v.double_award_rate_list[ citySkillLst[i] or 0 ] or  0,
								--下级加成
								nexAddition = v.double_award_rate_list[ (citySkillLst[i] or 0) + 1 ] or  0,
								--下次所需巡逻时间
								nexTime = v.need_time_list[ (citySkillLst[i] or 0) + 1 ] or 0,
								--下次所需的消耗
								diamond = v.cost_diamond_list[ (citySkillLst[i] or 0) + 1 ] or 0 ,
								--是否开启
								open = citySkillLst[i] or "Not",
							})

	end

	return t1
end

--领地镇压次数
function Modules.ManorModule:RiotSuppressTimes()
	local num = (ConfigAdapter.Common.GetRiotSuppressTimes() - self.MyManor.riot_suppress_times )
	if (ConfigAdapter.Common.GetRiotSuppressTimes() - self.MyManor.riot_suppress_times ) <= 0 then
		num = 0
	end
	return num
end

--获取累计时间
function Modules.ManorModule:AccumulativeTimes()
	return self.ManorLst.patrol_total_time
end

--过滤已经在巡逻的英雄
function Modules.ManorModule:FiltrationHero()
	local lst = {}
	local ManorLst = self.ManorLst.city_list
	for i,v in ipairs(ManorLst) do
		lst[v.hero_info_id] = true
	end

	local lst_ = {}
	local heroLst = CallModuleFunc(ModuleType.HERO, "GetHeroInfoList")
	for i,v in ipairs(heroLst) do
		if not lst[v.info_id] then
			local type_ , info_ = ObjIDTool.GetInfo(v.info_id)
			if info_.type ~= Macros.Game.HeroType.EXP and 
				info_.init_quality > Macros.Game.QualityType.BLUE then
				table.insert(lst_,v)
			end
		end
	end

	return lst_
end

--刷新巡逻城池数据
function Modules.ManorModule:UpPatrolDetails( info )

	local type_ , hero = ObjIDTool.GetInfo(info.patrol_hero_info_id)
	
	--基础信息
	local t1 = {}
	t1.hero_id = info.patrol_hero_info_id
	t1.desc = {}
	t1.time_type = info.time_type
	t1.finally_time = info.patrol_event_list[ #info.patrol_event_list ].happen_time
	t1.start_patrol_time = info.start_patrol_time

	--所有物品
	local t2 = {}
	--所有事件
	local t3 = {}

	--事件表
	local cnf_ = CPPGameLib.GetConfig("LandPatrolEvent", info.city_id)
	for i,v in ipairs(info.patrol_event_list) do
		--时间
		local time = "["..TimeTool.GetTimeFormat(v.happen_time,TimeTool.defaul_format[8]).."] "
		local cnf = cnf_.id_list[v.event_id]
		--事件
		local desc = cnf.desc
		--道具
		local type_ , item = ObjIDTool.GetInfo(cnf.object_id)
		--道具数量
		local itemNum = cnf.object_num 

		local double =  ""
		if v.is_double_award ~= 0 then
			itemNum = itemNum * 2 
			double = string.format( Macros.Color.front_color_rt , CPPGameLib.GetString("manor_double") )
		end

		--组合
		local text = Util:EchoText( time .. desc,{ 
									{ zi = hero.name , ys = Macros.Quality[hero.init_quality] ,dx = Macros.TypefaceSize.slightly},
									{ zi = item.name.."X".. itemNum .. double ,ys = Macros.Quality[item.quality or item.init_quality] ,dx = Macros.TypefaceSize.slightly} 
								} ,Macros.Color.keypoint )
		--整合事件
		table.insert(t3 , { text = text , sort = v.happen_time } )

		--储存所有的道具
		table.insert(t2 , { id = cnf.object_id , num = cnf.object_num , double = v.is_double_award } )
	end

	for i,v in ipairs(info.riot_event_list) do
		--时间
		local time = "["..TimeTool.GetTimeFormat(v.happen_time,TimeTool.defaul_format[8]).."] "
		--事件表
		local cnf = CPPGameLib.GetConfig("LandCity", info.city_id)
		--事件
		local desc = cnf.riot_desc
		--组合
		local text = Util:EchoText( time .. desc , {} ,Macros.Color.red_color )

		--整合事件
		table.insert(t3 , { text = text , sort = v.happen_time } )

		--好友镇压
		if v.suppress_time ~= 0 then
			--时间
			local time = "["..TimeTool.GetTimeFormat(v.suppress_time,TimeTool.defaul_format[8]).."] "
			--事件
			local desc = cnf.suppress_desc

			local cnf_ = CPPGameLib.GetConfig("HuoBi", Macros.Game.RolePropType.ENERGY)
			--道具
			local type_ , item = ObjIDTool.GetInfo(cnf_.big_icon)

			--组合
			local text = Util:EchoText( time .. desc , { 
										{ zi = v.friend_name , ys = Macros.Color.front_color ,dx = Macros.TypefaceSize.slightly},
										-- { zi = item.name .. "X" .. cnf.suppress_energy , ys = Macros.Quality[item.quality or item.init_quality] ,dx = Macros.TypefaceSize.slightly},
									} ,Macros.Color.front_color )
			table.insert(t3 , { text = text , sort = v.suppress_time } )
		end
	end

	table.sort( t3, function ( a , b )
		return a.sort < b.sort
	end )

	for i,v in ipairs(t3) do
		table.insert(t1.desc , v.text )
	end

	--单倍
	local t2_1 = {}
	--双倍
	local t2_2 = {}
	for i , v in ipairs( t2 ) do
		if v.double == 1 then
			if t2_2[ v.id ] then
				t2_2[ v.id ] = t2_2[ v.id ] + v.num
			else
				t2_2[ v.id ] = v.num
			end
			v.num = v.num * 2
		end

		if t2_1[ v.id ] then
			t2_1[ v.id ] = t2_1[ v.id ] + v.num
		else
			t2_1[ v.id ] = v.num
		end
	end

	local t2_3 = {}
	for i,v in pairs(t2_2) do
		table.insert( t2_3 , { id = i , num = v } )
	end
	table.sort( t2_3 , function ( a , b )
		return a.id < b.id
	end )


	--保存整合后的道具
	local t4 = {}
	local t5 = {}
	local t6 = {}
	for k , v in pairs(t2_1) do
		local type_ = ObjIDTool.GetInfo(k)
		local m_type_ = ConfigAdapter.Item.GetCurrencyInfo(k)
		if type_ == Macros.Game.ObjType.HERO then
			table.insert( t4 , {id = k , num = v } )
		elseif m_type_ then
			table.insert( t5 , {id = k , num = v } )
		else
			table.insert( t6 , {id = k , num = v } )
		end
	end

	--道具排序
	table.sort( t4, function( a , b )
		return a.id < b.id
	end )

	table.sort( t5, function( a , b )
		return a.id < b.id
	end )

	table.sort( t6, function( a , b )
		return a.id < b.id
	end )

	for i,v in ipairs(t5) do
		table.insert( t4 , v )
	end

	for i,v in ipairs(t6) do
		table.insert( t4 , v )
	end

	if info.reward_item_num ~= 0 then
		local pieceInfo = CPPGameLib.GetConfig("HeroCompound",info.patrol_hero_info_id)
		table.insert( t4 , 1 , {id = pieceInfo.item_id , num = info.reward_item_num ,  double = 0 } )
	end

	t1.item = { all = t4 , double = t2_3 }
	t1.patrol_type = info.patrol_type

	self.awardLst[info.city_id] = t1.item

	GlobalEventSystem:FireNextFrame(Macros.Event.ManorModule.MANOR_EVENT_UP , t1 ) 
end

--刷新好友列表
function Modules.ManorModule:UpFriend( info )
	self.friendLst = {}
	local cnf = CPPGameLib.GetConfig("SystemEntry", Macros.Game.SystemID.MANOR)
	local t1 = {}
	for i,v in ipairs(info) do
		if v.level >= cnf.level then
			self.friendLst[v.role_id] = v
			table.insert( t1 , v )
		end
	end

	if #t1 ~= 0 then
		table.sort( t1, function ( a , b )
	 		return a.riot_city_num > b.riot_city_num
		end )
	end

	GlobalEventSystem:FireNextFrame(Macros.Event.ManorModule.MANOR_PATROL_FRIEND , t1 )
end

--刷新技能
function Modules.ManorModule:UpSkill( info )
	GlobalEventSystem:FireNextFrame(Macros.Event.ManorModule.MANOR_SKILL,info)
end

--获取奖励列表
function Modules.ManorModule:GetAwardLst( city_id )
	return self.awardLst[city_id]
end

--获得好友信息
function Modules.ManorModule:GetFriend( id )
	return self.friendLst[id]
end

--获取首个巡逻结束的时间戳
function Modules.ManorModule:GetFirstPatrolFinishedTime()
	if not self.cityType then
		return
	end

	local finished_time
	for _, city_info in pairs(self.cityType) do
		if city_info.patrol and city_info.patrol.value > 0 then
			finished_time = finished_time or MathTool.MAX_NUM
			if city_info.patrol.value < finished_time then
				finished_time = city_info.patrol.value
			end
		end
	end
	return finished_time
end

--获得有没有可以领的奖励
function Modules.ManorModule:GetIsAward()
	local type_ = false
	for i , v in pairs( self.cityType ) do
		if v.award.type == "Award" then
			type_ = true
			break
		end
	end
	return type_
end

--------------------------------------------------------------------------------------------------------------------------------
--战斗判断
--------------------------------------------------------------------------------------------------------------------------------
function Modules.ManorModule:CheckBattleCondition( type_ )
	return true
end