--
-- @brief 活动模块 
-- @author: yjg
-- @date: 2017年1月14日11:43:44
--

Modules = Modules or {}

Modules.ActivityModule = Modules.ActivityModule or BaseClass(BaseModule)

function Modules.ActivityModule:__init()
	self.ALL_CNF = {}

	--每日签到
	self:__RegistProtocolForOpenView(BaseViewType.ACTIVITY, Net.ProtocolNo.C2S_SIGN_IN_DATA_REQ)
	--等级礼包
	self:__RegistProtocolForOpenView(BaseViewType.ACTIVITY, Net.ProtocolNo.C2S_LEVEL_GIFT_DATA_REQ)
	--VIP礼包
	self:__RegistProtocolForOpenView(BaseViewType.ACTIVITY, Net.ProtocolNo.C2S_VIP_GIFT_DATA_REQ)
	--摇钱树
	self:__RegistProtocolForOpenView(BaseViewType.ACTIVITY, Net.ProtocolNo.C2S_YCS_DATA_REQ)
end

function Modules.ActivityModule:__delete()
end

--------------------------------------------------------------------------------------------------------------------------------
--事件
--------------------------------------------------------------------------------------------------------------------------------
--处理新一天的通知
function Modules.ActivityModule:HandleNewDayNotify()
	self:InitActivitys()
end

--------------------------------------------------------------------------------------------------------------------------------
--签到
--------------------------------------------------------------------------------------------------------------------------------

--签到表
function Modules.ActivityModule:GetSignInCnf()
	if not self.ALL_CNF[ Macros.Game.SystemID.DAILY_SIGNIN ] then
		self:AddActivityCnf( Macros.Game.SystemID.DAILY_SIGNIN )
	end
	return self.ALL_CNF[ Macros.Game.SystemID.DAILY_SIGNIN ]
end


--------------------------------------------------------------------------------------------------------------------------------
--领取体力（嗑药 or 铜雀台）
--------------------------------------------------------------------------------------------------------------------------------

--获得药的表
function Modules.ActivityModule:GetTQTcnf( )	
	if not self.ALL_CNF[ Macros.Game.SystemID.TQT ] then
		self:AddActivityCnf( Macros.Game.SystemID.TQT )
	end
	return self.ALL_CNF[ Macros.Game.SystemID.TQT ]
end


--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--首充 
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

-- 消息说明: 首充数据请求 消息Id: 24501
function Modules.ActivityModule:FirstRechargeDataReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 首充数据请求 消息Id: 24501")
	self:AccumulateRechargeDataReq()
	
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_FIRST_RECHARGE_DATA_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 首充数据响应 消息Id: 24502
function Modules.ActivityModule:FirstRechargeDataResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 首充数据响应 消息Id: 24502")
	self.firstRechargeData = info
	self.total_recharge = info.total_recharge
	GlobalEventSystem:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_FIRST_RECHARGE)
end

--获得首充记录
function Modules.ActivityModule:GetFirstRechargeDataRecord( id )
	if self.firstRechargeData then
		for i,v in ipairs( self.firstRechargeData.reward_record_list ) do
			if v.data_id == id then
				return true
			end
		end
	end
end

--获得首充表
function Modules.ActivityModule:GetFirstRechargeCnf( ... )
	if not self.limitActivity then
		self:SettleLimitActivity()
	end

	local lst = self.limitActivity[ Macros.Game.LIMIT_ACTIVITY_TYPE.INITIAL ]

	table.sort( lst, function ( a , b )
		return a.context < b.context
	end )

	local t2 = {}
	local t3 = {}

	for _ , v in ipairs( lst ) do
		local t1 = {}
		for i , vv in ipairs( v.reward_item_id_list ) do
			table.insert( t1 , { id = vv , num = v.reward_item_num_list[i] } )
		end
		v.item = t1
		if self:GetFirstRechargeDataRecord( v.id ) then
			table.insert( t3 , v )
		else
			table.insert( t2 , v )
		end
	end

	for i , v in ipairs(t3) do
		table.insert( t2 , v )
	end


	local t4 = CPPGameLib.CopyTbl(t2)
	table.sort( t4 , function ( a , b )
		return a.context < b.context
	end )

	local num = 0
	for i , v in ipairs(t4) do
		if not self:GetFirstRechargeDataRecord( v.id ) then
			num = v.context
			break
		end
	end

	if num == 0 then
		num = t4[#t4].context
	end

	return t2 , num
end

--获得还有没有首充
function Modules.ActivityModule:GetFirstRechargeOver( )
	local num = 0
	local lst = self:GetFirstRechargeCnf()
	for i , v in ipairs( lst ) do
		local type_ = self:GetFirstRechargeDataRecord(v.id)
		if type_ ~= true then
			num = num + 1
		end
	end
	return num
end

--获得首充最高
function Modules.ActivityModule:GetFirstRechargeMAX( ... )
	return self.total_recharge
end

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--节日活动 
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

--获得节日活动表
function Modules.ActivityModule:GetFestivalActivityCnf()
	local cnf = CPPGameLib.GetConfig("FestivalActivity", nil,nil, true )
	if not cnf then return end
	local t1 = {}
	for i , data in pairs( cnf ) do
		
		local lst = self:ToTypeSettleActivityType( data.time_type , data )
		if lst and lst.show == "open" and lst.judge == "open" then
			-- data.activity_type = i
			table.insert( t1 , data )
		end
	end
	return t1
end

--获得节日活动条目内容
function Modules.ActivityModule:GetFestivalActivityItem( id ,type )
	local cnf = CPPGameLib.GetConfig("FestivalActivityParam", id )
	if not cnf then return end
	local info = nil
	if self.festivalActivityData then
		info = self.festivalActivityData[type .. id]
	end

	print("xy" , "====type========" , type )
	CPPGameLib.PrintTable("xy" , info , "GetFestivalActivityItem")  

	return cnf , info
end

-- 消息说明:节日活动数据请求 消息Id: 25101
function Modules.ActivityModule:FestivalActivityDataReq( info )
	CPPGameLib.PrintTable("xy" , info.activity_type , "消息说明: 节日活动数据请求 消息Id: 25101")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_FESTIVAL_ACTIVITY_DATA_REQ)
	data.activity_type = info.activity_type
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明:节日活动数据响应 消息Id: 25102
-- id: 			// 目标ID
-- context: 		// 上下文(阶段值,完成次数)
-- take_reward_times:  	// 领取奖励次数
function Modules.ActivityModule:FestivalActivityDataResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 节日活动数据响应 消息Id: 25102")
	if not self.festivalActivityData then
		self.festivalActivityData = {}
	end
	local addUpAddition = CallModuleFunc(ModuleType.CHARGE , "GetAccumulatedChargeFactor")
	for i , v in ipairs( info.target_list ) do
		if info.activity_type == Macros.Game.ACTIVITY_FESTIVAL_TAB.ADDUP_PAY then
			v.context = v.context * addUpAddition
		elseif info.activity_type == Macros.Game.ACTIVITY_FESTIVAL_TAB.ADDUP_GIFT then
			v.context = v.context * addUpAddition
		elseif info.activity_type == Macros.Game.ACTIVITY_FESTIVAL_TAB.OPEN_ADD_UP then
			v.context = v.context * addUpAddition
		end
		self.festivalActivityData[ info.activity_type .. v.id ] = v	
	end
	GlobalEventSystem:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_FESTIVAL)
end

-- 消息说明:节日活动奖励领取请求 消息Id: 25103
function Modules.ActivityModule:TakeFestivalActivityRewardReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 节日活动奖励领取请求 消息Id: 25103")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TAKE_FESTIVAL_ACTIVITY_REWARD_REQ)
	data.activity_type = info.activity_type
	data.target_id = info.target_id
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明:节日活动奖励领取响应 消息Id: 25104
function Modules.ActivityModule:TakeFestivalActivityRewardResp( info )
	-- CPPGameLib.PrintTable("xy" , info , "消息说明: 节日活动奖励领取响应 消息Id: 25104")
	if self.festivalActivityData[ info.activity_type ..  info.target_id ] then
		self.festivalActivityData[  info.activity_type ..  info.target_id  ].take_reward_times = self.festivalActivityData[  info.activity_type ..  info.target_id  ].take_reward_times + 1 
	end
	local cnf = self:GetFestivalActivityItem( info.target_id , info.activity_type )
    local t1 = {}
    for i,v in ipairs(cnf.reward_item_id_list) do
        table.insert(t1,{id = v , num = cnf.reward_item_num_list[i] })
    end
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
	GlobalEventSystem:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_FESTIVAL)

	
    self:FestivalActivityRedPointReq()
end

-- 消息说明: 节日商品兑换请求 消息Id: 25105
function Modules.ActivityModule:FestivalActivityGoodsExchangeReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 节日商品兑换请求 消息Id: 25105")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_FESTIVAL_ACTIVITY_GOODS_EXCHANGE_REQ)
	data.activity_type = info.activity_type
	data.target_id = info.target_id
	data.exchange_num = info.exchange_num
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 节日商品兑换响应 消息Id: 25106
function Modules.ActivityModule:FestivalActivityGoodsExchangeResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 节日商品兑换响应 消息Id: 25106")
	if self.festivalActivityData[ info.activity_type .. info.target_id ] then
		self.festivalActivityData[ info.activity_type .. info.target_id ].take_reward_times = self.festivalActivityData[ info.activity_type .. info.target_id  ].take_reward_times + info.exchange_num
	else
		self.festivalActivityData[ info.activity_type .. info.target_id ] = {}
		self.festivalActivityData[ info.activity_type .. info.target_id ].take_reward_times = info.exchange_num
	end
	local cnf = self:GetFestivalActivityItem( info.target_id , info.activity_type )
    local t1 = {}
    for i,v in ipairs(cnf.reward_item_id_list) do
        table.insert(t1,{id = v , num = info.exchange_num * cnf.reward_item_num_list[i]})
    end
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
	GlobalEventSystem:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_FESTIVAL)

    self:FestivalActivityRedPointReq()
end


-- 消息说明: 节日活动红点请求 消息Id: 25107
function Modules.ActivityModule:FestivalActivityRedPointReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 节日活动红点请求 消息Id: 25107")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_FESTIVAL_ACTIVITY_RED_POINT_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 节日活动红点响应 消息Id: 25108
function Modules.ActivityModule:FestivalActivityRedPointResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 节日活动红点响应 消息Id: 25108")
	self.festivalActivityRed = {}
	for i , v in ipairs( info.red_piont_list ) do
		self.festivalActivityRed[v.activity_type] = true
	end

	GlobalEventSystem:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_FESTIVAL_RED)
end

-- 获得红点
function Modules.ActivityModule:GetTypeFestivalActivityRed( type )
	return self.festivalActivityRed[type]
end

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--登录奖励 
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--活动相关数据计算 
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

--获得已经开启了的活动
function Modules.ActivityModule:InitActivitys( )
	--玩家信息
	local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
	if not userDate then
		return
	end

	self.activity = {}
	local activityCnf = CPPGameLib.GetConfig("Activity", nil, nil , true )
	for k , v in pairs( activityCnf ) do
		local t1 = {}
		for k_ , data in pairs( v.sort_list ) do
			local lst = self:ToTypeSettleActivityType( data.time_type , data )
			table.insert( t1 , lst )
		end
		self.activity[ k ] = t1
	end
end

-- data.star_date
-- data.star_time
-- data.over_date
-- data.over_time
-- data.reward_over_date
-- data.reward_over_time

--用类型整理活动状态
function Modules.ActivityModule:ToTypeSettleActivityType( time_type , data )
	--玩家信息
	local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
	if not userDate then
		return
	end

	--玩家等级
	local userLv = userDate.level
	--开服开了几天
	local server_opTime = CallModuleFunc(ModuleType.ROLE,"GetOpenClothing")

	--开服时间
	local o_Time = userDate.open_server_time
	local o_nian, o_yue, o_ri , o_shi, o_fen, o_miao, o_xingqi = TimeTool.GetDateFromTimeStamp(o_Time)
	
	--当前时间 
	local n_Time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	local n_nian, n_yue, n_ri, n_shi, n_fen, n_miao, n_xingqi = TimeTool.GetDateFromTimeStamp(n_Time)

	--创号时间 
	local c_Time = userDate.role_create_time
	local c_nian, c_yue, c_ri, c_shi, c_fen, c_miao, c_xingqi = TimeTool.GetDateFromTimeStamp(c_Time)

	local function Compound( judgeLst , starLst , overLst , rewardLst )

		for k , v in pairs( judgeLst ) do
			judgeLst[k] = tonumber(v)
		end

		local judge = "open"
		if judgeLst.nowTime < judgeLst.star_tiem then
			judge = "noOpen"
		elseif judgeLst.nowTime > judgeLst.over_tiem and judgeLst.nowTime <= judgeLst.reward_over_time then
			judge = "award"
		elseif judgeLst.nowTime > judgeLst.reward_over_time then
			judge = "over"
		end

		local t1 = { 
						judge = judge ,

						nowTime = judgeLst.nowTime , 

						star = starLst ,

						over = overLst ,

						reward = rewardLst ,
					} 		
		return t1
	end

	local function Change( star_time )
		--表里的时间
		local cnfStar = string.format( "%04d" , star_time ) 
		local t = tonumber( string.sub(cnfStar, 0 , 2 ) ) * 60 * 60
		local t1 = tonumber( string.sub(cnfStar, 3 , 4 ) ) * 60
		return t + t1
	end

	local lst = {}
	--指定时间
	if time_type == Macros.Game.ACTIVITY_TYPE.ASSIGN then

		--判断形态
		local judgeLst = {}
		judgeLst.nowTime = n_Time
		judgeLst.star_tiem = TimeTool.CalcTimeStampFrmDate( string.format( "%04d" , data.star_date ) ..  string.format( "%04d" , data.star_time ) )
		judgeLst.over_tiem = TimeTool.CalcTimeStampFrmDate( string.format( "%04d" , data.over_date ) ..  string.format( "%04d" , data.over_time ) )
		judgeLst.reward_over_time = TimeTool.CalcTimeStampFrmDate( data.reward_over_date ..  string.format( "%04d" , data.reward_over_time ) )

		--开始时间
		local starLst = TimeTool.CalcTimeStampFrmDate( string.format( "%04d" , data.star_date ) ..  string.format( "%04d" , data.star_time ) )

		--结束时间
		local overLst = TimeTool.CalcTimeStampFrmDate( string.format( "%04d" , data.over_date ) ..  string.format( "%04d" , data.over_time ) )

		--奖励
		local reward_overLst = TimeTool.CalcTimeStampFrmDate( data.reward_over_date ..  string.format( "%04d" , data.reward_over_time ) )

		lst = Compound( judgeLst , starLst , overLst , reward_overLst )
	
	-- 每日事件
	elseif time_type == Macros.Game.ACTIVITY_TYPE.DAYS then

		-- 现在的时间
		local nowTime = n_shi .. n_fen

		--判断形态
		local judgeLst = {}
		judgeLst.nowTime = TimeTool.CalcTimeStampFrmDate( string.format( "%04d" , n_nian ) .. string.format( "%02d" , n_yue ) .. string.format( "%02d" , n_ri ) .. string.format( "%04d" , nowTime ) )
		judgeLst.star_tiem = TimeTool.CalcTimeStampFrmDate( string.format( "%04d" , n_nian ) .. string.format( "%02d" , n_yue ) .. string.format( "%02d" , n_ri ) .. string.format( "%04d" , data.star_time ) )
		judgeLst.over_tiem = TimeTool.CalcTimeStampFrmDate( string.format( "%04d" , n_nian ) .. string.format( "%02d" , n_yue ) .. string.format( "%02d" , n_ri ) .. string.format( "%04d" , data.over_time ) )
		judgeLst.reward_over_time = TimeTool.CalcTimeStampFrmDate( string.format( "%04d" , n_nian ) .. string.format( "%02d" , n_yue ) .. string.format( "%02d" , n_ri ) .. string.format( "%04d" , data.reward_over_time ) )

		--开始时间
		local starLst = TimeTool.CalcTimeStampFrmDate( string.format( "%04d" , n_nian ) .. string.format( "%02d" , n_yue ) .. string.format( "%02d" , n_ri ) .. string.format( "%04d" , data.star_time ) )

		--结束时间
		local overLst = TimeTool.CalcTimeStampFrmDate( string.format( "%04d" , n_nian ) .. string.format( "%02d" , n_yue ) .. string.format( "%02d" , n_ri ) .. string.format( "%04d" , data.over_time ) )

		--奖励
		local reward_overLst 	= TimeTool.CalcTimeStampFrmDate( string.format( "%04d" , n_nian ) .. string.format( "%02d" , n_yue ) .. string.format( "%02d" , n_ri ) .. string.format( "%04d" , data.reward_over_time ) )

		lst = Compound( judgeLst , starLst , overLst , reward_overLst )

	-- 每周事件
	elseif time_type == Macros.Game.ACTIVITY_TYPE.WEEKLY then

		if data.star_date == 0 then
			data.star_date = 7
		end

		if data.over_date == 0 then
			data.over_date = 7
		end

		if data.reward_over_date == 0 then
			data.reward_over_date = 7
		end

		if o_xingqi == "0" or o_xingqi == 0 then
			o_xingqi = 7 
		end

		if n_xingqi == "0" or n_xingqi == 0 then
			n_xingqi = 7 
		end

		local start = 0
		local over = 0
		local draw = 0

		--开启的时间
		local star_time = 0
		--距离的天数
		local day_gap = math.abs( n_xingqi - data.star_date ) * 24 * 60 * 60 
		
		--如果今天大于等于开启
		if tonumber( n_xingqi ) > tonumber( data.star_date ) and tonumber( n_xingqi ) < tonumber( data.over_date ) then
			-- print("xy", "如果今天大于开启 " , n_xingqi , data.star_date )
			star_time = n_Time - day_gap
		elseif tonumber( n_xingqi ) < tonumber( data.star_date ) then
			-- print("xy", "如果今天小于开启 " , n_xingqi , data.star_date )
			star_time = n_Time + day_gap
		elseif tonumber( n_xingqi ) == tonumber( data.star_date ) then
			star_time = n_Time
			-- print("xy", "如果今天等于开启 " , n_xingqi , data.star_date )
		elseif tonumber( n_xingqi ) == tonumber( data.over_date ) then
			star_time = n_Time - day_gap
			-- print("xy", "如果今天等于结束 " , n_xingqi , data.over_date )
		else
			-- print("xy", "如果今天大于开启")
			star_time = n_Time - day_gap
		end
		star_time = star_time - ( n_shi * 60 * 60 ) - (n_fen * 60) - n_miao
		start = star_time + Change( data.star_time )

		--结束的时间
		local over_time = 0
		--距离的天数
		local day_gap = math.abs( data.star_date - data.over_date ) * 24 * 60 * 60 
		--今天大于结束
 		if tonumber(data.star_date) > tonumber(data.over_date) then
 			over_time = star_time - day_gap
		--今天小于结束
 		elseif tonumber(data.star_date) < tonumber(data.over_date) then
 			over_time = star_time + day_gap
 		else
 			over_time = star_time
 		end

		--最终的结束时间
		over = over_time + Change( data.over_time )

		--领奖的时间
		local reward_over_time = 0
		--距离的天数
		local day_gap = math.abs( data.star_date - data.reward_over_date ) * 24 * 60 * 60 

		--今天大于结束
 		if tonumber(data.star_date) > tonumber(data.reward_over_date) then
 			reward_over_time = star_time - day_gap
		--今天小于结束
 		elseif tonumber(data.star_date) < tonumber(data.reward_over_date) then
 			reward_over_time = star_time + day_gap
 		else
 			reward_over_time = star_time
 		end

		--最终的结束时间
		draw = reward_over_time + Change( data.reward_over_time )

		--判断形态
		local judgeLst = {}
		judgeLst.nowTime = n_Time
		judgeLst.star_tiem = start
		judgeLst.over_tiem = over
		judgeLst.reward_over_time = draw

		--开始时间
		local year, month, day, hour, minute, second, week = TimeTool.GetDateFromTimeStamp(start)
		local starLst = start

		--结束时间
		local overLst = over

		--奖励
		local reward_overLst 	= draw

		lst = Compound( judgeLst , starLst , overLst , reward_overLst )

	-- 每月事件
	elseif time_type == Macros.Game.ACTIVITY_TYPE.MONTHLY then

		--开始的时间
		local star_tiem = string.format( "%04d" , data.star_date ) ..  string.format( "%04d" , data.star_time )
		--结束的时间
		local over_tiem = string.format( "%04d" , data.over_date ) ..  string.format( "%04d" , data.over_time )
		--奖励的时间
		local reward_over_time = string.format( "%04d" , data.reward_over_date ) ..  string.format( "%04d" , data.reward_over_time )					

		--判断形态
		local judgeLst = {}
		judgeLst.nowTime = n_Time
		judgeLst.star_tiem = TimeTool.CalcTimeStampFrmDate( string.format( "%04d" , n_nian ) .. string.format( "%02d" , n_yue ) .. string.format( "%04d" , data.star_date ) .. string.format( "%04d" , data.star_time ) )
		judgeLst.over_tiem = TimeTool.CalcTimeStampFrmDate( string.format( "%04d" , n_nian ) .. string.format( "%02d" , n_yue ) .. string.format( "%04d" , data.over_date ) .. string.format( "%04d" , data.over_time ) )
		judgeLst.reward_over_time = TimeTool.CalcTimeStampFrmDate( string.format( "%04d" , n_nian ) .. string.format( "%02d" , n_yue ) .. string.format( "%04d" , data.reward_over_date ) .. string.format( "%04d" , data.reward_over_time ) )

		--开始时间
		local starLst = TimeTool.CalcTimeStampFrmDate( string.format( "%04d" , n_nian ) .. string.format( "%02d" , n_yue ) .. string.format( "%04d" , data.star_date ) .. string.format( "%04d" , data.star_time ) )

		--结束时间
		local overLst = TimeTool.CalcTimeStampFrmDate( string.format( "%04d" , n_nian ) .. string.format( "%02d" , n_yue ) .. string.format( "%04d" , data.over_date ) .. string.format( "%04d" , data.over_time ) )

		--奖励
		local reward_overLst = TimeTool.CalcTimeStampFrmDate( string.format( "%04d" , n_nian ) .. string.format( "%02d" , n_yue ) .. string.format( "%04d" , data.reward_over_date ) .. string.format( "%04d" , data.reward_over_time ) )

		lst = Compound( judgeLst , starLst , overLst , reward_overLst )

	-- 七日登陆
	elseif time_type == Macros.Game.ACTIVITY_TYPE.SEVEN then

		if data.over_date == 0 then
			data.over_date = 7
		end

		if data.reward_over_date == 0 then
			data.reward_over_date = 7
		end

		--开始的时间
		local star_tiem = o_Time - (o_shi * 60 * 60) - (o_fen*60) - o_miao
		star_tiem = star_tiem + ( ( data.star_date - 1 )  * 24 * 60 *60 )
		star_tiem = star_tiem + Change( data.star_time )

		--结束的时间
		local over_tiem = o_Time - (o_shi * 60 * 60) - (o_fen*60) - o_miao
		over_tiem = over_tiem + ( ( data.over_date - 1 )  * 24 * 60 *60 )
		over_tiem = over_tiem + Change( data.over_time )

		--奖励的时间
		local reward_over_time = o_Time - (o_shi * 60 * 60) - (o_fen*60) - o_miao
		reward_over_time = reward_over_time + ( ( data.reward_over_date - 1 )  * 24 * 60 *60 )
		reward_over_time = reward_over_time + Change( data.reward_over_time )

		--判断形态
		local judgeLst = {}
		judgeLst.nowTime = n_Time
		judgeLst.star_tiem = star_tiem
		judgeLst.over_tiem = over_tiem
		judgeLst.reward_over_time = reward_over_time

		--开始时间
		local starLst = star_tiem

		--结束时间
		local overLst = over_tiem

		--奖励
		local reward_overLst = reward_over_time

		lst = Compound( judgeLst , starLst , overLst , reward_overLst )


	-- 充值回馈
	elseif time_type == Macros.Game.ACTIVITY_TYPE.SEVEN_2 then

		if data.over_date == 0 then
			data.over_date = 7
		end

		if data.reward_over_date == 0 then
			data.reward_over_date = 7
		end

		--开始的时间
		local star_tiem = o_Time - (o_shi * 60 * 60) - (o_fen*60) - o_miao
		star_tiem = star_tiem + ( ( data.star_date - 1 )  * 24 * 60 *60 )
		star_tiem = star_tiem + Change( data.star_time )

		--结束的时间
		local over_tiem = o_Time - (o_shi * 60 * 60) - (o_fen*60) - o_miao
		over_tiem = over_tiem + ( ( data.over_date - 1 )  * 24 * 60 *60 )
		over_tiem = over_tiem + Change( data.over_time )

		--奖励的时间
		local reward_over_time = o_Time - (o_shi * 60 * 60) - (o_fen*60) - o_miao
		reward_over_time = reward_over_time + ( ( data.reward_over_date - 1 )  * 24 * 60 *60 )
		reward_over_time = reward_over_time + Change( data.reward_over_time )

		--判断形态
		local judgeLst = {}
		judgeLst.nowTime = n_Time
		judgeLst.star_tiem = star_tiem
		judgeLst.over_tiem = over_tiem
		judgeLst.reward_over_time = reward_over_time

		--开始时间
		local starLst = star_tiem

		--结束时间
		local overLst = over_tiem

		--奖励
		local reward_overLst = reward_over_time

		lst = Compound( judgeLst , starLst , overLst , reward_overLst )

	--开服
	elseif time_type == Macros.Game.ACTIVITY_TYPE.OPEN then

		--开始的时间
		local star_tiem = c_Time - (c_shi * 60 * 60) - (c_fen*60) - c_miao
		star_tiem = star_tiem + ( ( data.star_date - 1 )  * 24 * 60 *60 )
		star_tiem = star_tiem + Change( data.star_time )

		--结束的时间
		local over_tiem = c_Time - (c_shi * 60 * 60) - (c_fen*60) - c_miao
		over_tiem = over_tiem + ( ( data.over_date - 1 )  * 24 * 60 *60 )
		over_tiem = over_tiem + Change( data.over_time )

		--奖励的时间
		local reward_over_time = c_Time - (c_shi * 60 * 60) - (c_fen*60) - c_miao
		reward_over_time = reward_over_time + ( ( data.reward_over_date - 1 )  * 24 * 60 *60 )
		reward_over_time = reward_over_time + Change( data.reward_over_time )

		--判断形态
		local judgeLst = {}
		judgeLst.nowTime = n_Time
		judgeLst.star_tiem = star_tiem
		judgeLst.over_tiem = over_tiem
		judgeLst.reward_over_time = reward_over_time

		--开始时间
		local starLst = star_tiem

		--结束时间
		local overLst = over_tiem

		--奖励
		local reward_overLst = reward_over_time

		lst = Compound( judgeLst , starLst , overLst , reward_overLst )

	else
		--打印堆栈
		-- print("xy" , debug.traceback())
	end
		-- CPPGameLib.PrintTable("xy" , reward_overLst , "reward_overLst")  
	local show = "open"

	if server_opTime < data.days then
		show = "noDays"
	elseif data.need_role_level and userLv < data.need_role_level then
		show = "noNeed_role_level"
	end

	lst.days = data.days
	lst.need_role_level = data.need_role_level or 0
	lst.show = show
	lst.sort = data.sort
	--用于同模块不同界面
	lst.subtype = data.data_id

	--获得我的服务器ID
	local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")
	local server_id = role_info and role_info.server_id or 0

	if data.server_list then
		local t1 = {}
		for i , v in ipairs( data.server_list ) do
			t1[v] = true
		end

		if t1[server_id] and lst.judge == "open" then
			lst.judge = "open"
		else
			lst.judge = "noOpen"
		end
	end

	return lst
end

--用id获得活动开启状态
function Modules.ActivityModule:ToIdGetActivityTime( id )
	local t1 = {judge = "open" , show = "open" , sort = 0 }

	if not self.activity[ id ] then 
		return t1 
	end
	local t1 = self.activity[ id ]
	table.sort( t1, function ( a , b )
		return a.sort < b.sort
	end )

	local t2 = {}
	for i,v in ipairs( t1 ) do
		if (v.judge == "open" or v.judge == "award") and v.show == "open" then
			t2 = v
			break
		end
	end
	return t2 , t1
end

-- 整理限时活动表
function Modules.ActivityModule:SettleLimitActivity( )
	self.limitActivity = {}
	local cnf = CPPGameLib.GetConfig("LimitActivity", nil, nil , true )
	if not cnf then return end

	for i , v in pairs( cnf ) do
		if not self.limitActivity[v.type] then
			self.limitActivity[v.type] = {}
		end
		table.insert( self.limitActivity[v.type] , { 
														id = i , 
														type = v.type , 
														context = v.context , 
														reward_times_max = v.reward_times_max , 
														reward_item_id_list = v.reward_item_id_list , 
														reward_item_num_list = v.reward_item_num_list 
													} 
					)
	end
end

--活动表加载
function Modules.ActivityModule:AddActivityCnf( type_ )
	--签到
	if type_ == Macros.Game.SystemID.DAILY_SIGNIN then
		self.ALL_CNF[ Macros.Game.SystemID.DAILY_SIGNIN ] = CPPGameLib.GetConfig("SignIn", nil, nil , true )
	--铜雀台
	elseif type_ == Macros.Game.SystemID.TQT then
		self.ALL_CNF[ Macros.Game.SystemID.TQT ] = CPPGameLib.GetConfig("TQT", nil, nil , true )
	end
end

--限时产出
function Modules.ActivityModule:GetTimedOutput( system )
	local cnf = CPPGameLib.GetConfig("LimitProduct", system, false)
	if not cnf then
		return 1
	end
	--当前日期
	local n_Time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	local n_nian, n_yue, n_ri, n_shi, n_fen, n_miao, n_xingqi = TimeTool.GetDateFromTimeStamp(n_Time)
	local n_riqi = n_nian .. string.format( "%02d" , n_yue )  .. string.format( "%02d" , n_ri )
	n_riqi = tonumber( n_riqi )

	--是否在日期里
	if n_riqi >= cnf.start_date and n_riqi <= cnf.over_date then
		print("xy" , "在日期里")
		local n_shijian = string.format( "%02d" , n_shi ) .. string.format( "%02d" , n_fen )
		n_shijian = tonumber(n_shijian)
		for i = 1  , #cnf.start_time_list  do 
			--是否在时间里
			if n_shijian >= cnf.start_time_list[i] and n_shijian <= cnf.over_time_list[i] then
				print("xy" , "在时间里" , cnf.multiple )
				return cnf.multiple
			else
				print("xy" , "不在时间里")
				return 1
			end
		end
	end

	return 1
end    

--检测活动开放
function Modules.ActivityModule:CheckActivityOpened( entrySystemId )
    local start = 0
    local over = 0
    local draw = 0
    local time_sync = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")

    --判断是否限时活动如果是就判断是否开启
    for system_id,system_info in pairs(CPPGameLib.GetConfig("ActivityAdvanceNotice", nil, nil , true )) do
        if system_id == entrySystemId then
            return CallModuleFunc(ModuleType.LIMIT_DUNGEON,"IsLimitDungeonOpen",system_id)
        end
    end

    --如果是七天活动
    if entrySystemId == Macros.Game.SystemID.SEVEN_DEYS then
        local server_opTime = CallModuleFunc(ModuleType.ROLE,"GetOpenClothing")
        local cnf = CPPGameLib.GetConfig("Activity", Macros.Game.SystemID.SEVEN_DEYS , false )
        if server_opTime > cnf.sort_list[1].reward_over_date then
        else
            return true
        end

    --如果是七天活动II
    elseif entrySystemId == Macros.Game.SystemID.SEVEN_DEYS_II then
        local server_opTime = CallModuleFunc(ModuleType.ROLE,"GetOpenClothing")
        local cnf = CPPGameLib.GetConfig("Activity", Macros.Game.SystemID.SEVEN_DEYS_II , false )
        if server_opTime > cnf.sort_list[1].reward_over_date or server_opTime < cnf.sort_list[1].star_date then
        else
            return true
        end

	--等级商店
    elseif entrySystemId == Macros.Game.SystemID.LEVEL_SHOP then
		local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
		local level_shop_data_list = CallModuleFunc(ModuleType.ACTIVITY, "LevelShopGetLst")
		for lv, activate_time in pairs(level_shop_data_list) do
			--检测时效性
			local config = CPPGameLib.GetConfig("LevelShop", lv)
			if config and config.valid_time * 60 + activate_time > server_time then
				return true
			end
		end
    
    --英雄降临
    elseif entrySystemId == Macros.Game.SystemID.HERO_ADVENT then
        local dungeonbox_reward_level = CallModuleFunc(ModuleType.DUNGEON,"GetFBPassedBoxRewardLevel")
        return dungeonbox_reward_level < #CPPGameLib.GetConfig("MainFBChapterReward",nil,nil,true)

    --首充
    elseif entrySystemId == Macros.Game.SystemID.FIRST_RECHARGE then
        local first_recharge_num = CallModuleFunc(ModuleType.ACTIVITY,"GetFirstRechargeOver")
        if first_recharge_num ~= 0 then
            return true
        else
            return false
        end

    --功能预告
    elseif entrySystemId == Macros.Game.SystemID.SYSTEM_FORESHOW then
        local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
        local fore_show_info = CPPGameLib.GetConfig("HeroSystemForeshow",#CPPGameLib.GetConfig("HeroSystemForeshow",nil,nil,true))
        return role_level < fore_show_info.open_level

    --次日登录
    elseif entrySystemId == Macros.Game.SystemID.MORROW then
        local isTakeReward = CallModuleFunc(ModuleType.MORROW,"GetIsTakeReward")
        if isTakeReward ~= 1 then
            return true
        else
            return false
        end

    --超值折扣
    elseif entrySystemId == Macros.Game.SystemID.SUPERDISCOUNTVIEW then
        local lst = CallModuleFunc(ModuleType.SUPERDISCOUNTVIEW,"GetNowTimeAward")
        if lst then
            return true
        else
            return false
        end

    --领奖中心
    elseif entrySystemId == Macros.Game.SystemID.ACCEPT_PRIZE_CENTER then
        --领奖中心
        return CallModuleFunc(ModuleType.ACCEPT_PRIZE_CENTER,"IsRewardCenterVisible")

    --冲榜活动
    elseif entrySystemId == Macros.Game.SystemID.RANK_ACTIVITY then
        --冲榜
        local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.RANK_ACTIVITY )
        if info.show == "open" then
            return true 
        end

    --等级基金
    elseif entrySystemId == Macros.Game.SystemID.REBATE then
        --基金
        --是否已经购买过
        local is_buy_fund = CallModuleFunc(ModuleType.ACTIVITY,"GetIsAlreadyBuyFund")
        if is_buy_fund == Macros.Global.TRUE then
            return false
        end

        local date_info = TimeTool.GetDateFromTimeStamp(CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime"), nil, true)
        local cur_week = date_info.wday

        --先设置之前的为未推送
        for cur_day=0,6 do
            if cur_day ~= cur_week then
                Cookies.Set(Cookies.LevelType.ROLE, string.format("%s_%d", Macros.Setting.PushFund, cur_day), "0")
            end            
        end
        --是否已经推送过       
        local flag = Cookies.Get(Cookies.LevelType.ROLE, string.format("%s_%d", Macros.Setting.PushFund, cur_week)) ~= "1"
        if flag then
            --没购买过
            return true
        else
            --购买过
            return false           
        end    

    --直购特惠
    elseif entrySystemId == Macros.Game.SystemID.DIRECT_BUY then
        local num = CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"GetDirectBuyActivity" )
        if num ~= 0 then
            return true
        else
            return false
        end

    --登录奖励
    elseif entrySystemId == Macros.Game.SystemID.REGISTER_AWARD then
    	return self:CheckLoginReward()

    --组队邀请
    elseif entrySystemId == Macros.Game.SystemID.TEAM_INVITE then
        --组队邀请
        local team_list = CallModuleFunc(ModuleType.TEAM, "GetInvitedToJoinTheTeamList")
        return #team_list > 0

    --精英副本外域入侵
    elseif entrySystemId == Macros.Game.SystemID.ELITE_DUNGEON_BREAK then
        --外域入侵
        local extra_data = CallModuleFunc(ModuleType.DUNGEON,"GetExtraInfo",Macros.Game.FBType.ELITE_DUNGEON)
        if #extra_data > 0 then
            for i,extra_info in ipairs(extra_data) do
                if extra_info.is_dead == Macros.Global.FALSE then
                    return true
                end
            end
        end
        return false

    --寻访遗迹
    elseif entrySystemId == Macros.Game.SystemID.MANOR then
        --遗迹可领取
        local type_ = CallModuleFunc(ModuleType.MANOR, "GetIsAward")
        return type_

    --春节、端午、圣诞活动
    elseif entrySystemId == Macros.Game.SystemID.CHRISTMAS_ACTIVITY or entrySystemId == Macros.Game.SystemID.SPRING_FESTIVAL 
        or entrySystemId == Macros.Game.SystemID.DRAGONBOAT_FESTIVAL then
        --圣诞活动或者春节活动
        local type_ = CallModuleFunc(ModuleType.LIMIT_ACTIVITY, "GetActivityOpen",Macros.Game.SystemID.CHRISTMAS_ACTIVITY)
        return type_

    --好友召唤
    elseif entrySystemId == Macros.Game.SystemID.SUMMON_FRIENDS then
        if CPPSdkTools.IsFacebookActivityToggle() then
            return true
        end

    --好友邀请
    elseif entrySystemId == Macros.Game.SystemID.INVITE_FRIENDS then
        if CPPSdkTools.IsFacebookActivityToggle() then
            return true
        end

    --facebook分享获取好礼
    elseif entrySystemId == Macros.Game.SystemID.FACEBOOK_SHARE_REWARD then
        return false

    --勇者试炼主城推送
    elseif entrySystemId == Macros.Game.SystemID.TOWER then
        local _ , times = CallModuleFunc(ModuleType.TOWER, "GetResetTime")
        return times > 0

    --七日登录
    elseif entrySystemId == Macros.Game.SystemID.PUSH_SEVEN then
        local noRegister = {}
        local register = {}
        local draw = {}
        local overdue = {}

        for i = 1 , 7 do
            local type_ = CallModuleFunc(ModuleType.PUSH_SEVEN , "GetSevenDayLogin" , i )

            --未登录           
            if type_ == "NoRegister" then
                table.insert( noRegister , i )
            --可领取
            elseif type_ == "Register" then
                table.insert( register , i )
            --已领取
            elseif type_ == "Draw" then
                table.insert( draw , i )
            --已过期
            elseif type_ == "Overdue" then
                table.insert( overdue , i )
            end
        end

        --如果还有未登录的 或者 还有可领取的
        if #noRegister ~= 0 or #register ~= 0 then
            return true
        end

    --在线奖励
    elseif entrySystemId == Macros.Game.SystemID.ONLINE_AWARD then
        local cnf = CallModuleFunc(ModuleType.ONLINE_AWARD , "GetOnlineawardCnf")
        local openClothing = CallModuleFunc(ModuleType.ROLE,"GetRoleCreateTime")

        if openClothing <= #cnf then
            return true
        end

    --充值回馈
    elseif entrySystemId == Macros.Game.SystemID.FEEDBACK then
        local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.FEEDBACK )
        if info.show == "open" then
            return true
        end
        return CallModuleFunc(ModuleType.FEEDBACK, "CheckReward")

    --女王
    elseif entrySystemId == Macros.Game.SystemID.QUEEN then
        local type_ = CallModuleFunc(ModuleType.QUEEN , "GetRetainTerm")
        return type_

    --自定义超值购
    elseif entrySystemId == Macros.Game.SystemID.CUSTOM_PURCHASE then
        local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.CUSTOM_PURCHASE )
        if info.show == "open" then
            return true
        end

    --活动热卖
    elseif entrySystemId == Macros.Game.SystemID.HOT_SALE then

        local cur_info,oepn_info = CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"GetDirectBuyShopOpen3" )
        if oepn_info and oepn_info.isOpen == "yes" then
            return true
        else
            local num = CallModuleFunc(ModuleType.DIRECT_BUY_SHOP,"GetHotSaleUnclaimed" )
            if num > 0 then
                return true
            end
        end

    --扭蛋机
    elseif entrySystemId == Macros.Game.SystemID.GASHAPONMACHINE then
        local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.GASHAPONMACHINE )
        if info.show == "open" then
            return true
        end

    --五星好评
    elseif entrySystemId == Macros.Game.SystemID.FIVESTAR_HIGHPRAISE then
         return false

    --转盘
    elseif entrySystemId == Macros.Game.SystemID.TURNTABLE then

        local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.TURNTABLE )
        if info.show == "open" then
            return true
        end

    --豪华签到
    elseif entrySystemId == Macros.Game.SystemID.LUXURY then
        local sign_data = CallModuleFunc(ModuleType.CRYSTAL_SHOP,"GetLuxurySignInType")
        if sign_data then
            for _,sign_info in pairs(sign_data) do
                --有未充值的档位
                if sign_info.status == 0 then
                    return true
                end
            end
        end
        
    --世界等级任务
    elseif entrySystemId == Macros.Game.SystemID.PUSH_WORLDLEVEL then
        local server_opTime = CallModuleFunc(ModuleType.ROLE,"GetOpenClothing")
        if server_opTime <= 1 then
            return false
        end

        local num , max = CallModuleFunc(ModuleType.PUSH_WORLDLEVEL , "GetTaskSchedule" )
            if CallModuleFunc(ModuleType.PUSH_WORLDLEVEL , "GetTodayAwardType" ) then
            end
        if num < max then
            local level = CallModuleFunc(ModuleType.PUSH_WORLDLEVEL , "GetWorldLevel" )
            if level ~= 0 then
                return true
            end
        elseif CallModuleFunc(ModuleType.PUSH_WORLDLEVEL , "GetTodayAwardType" ) ~= true then
            return true
        end
 
    --阵营招募
    elseif entrySystemId == Macros.Game.SystemID.CAMP_RECRUIT then
        local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.CAMP_RECRUIT )
        if info.show == "open" then
            return true
        end
 
    --排行榜
    elseif entrySystemId == Macros.Game.SystemID.RECHARGE_RANK then    
        local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.RECHARGE_RANK )
        if info.show == "open" then
            return true
        end
 
    else 
        return true
    end
end