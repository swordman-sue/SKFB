--
-- @brief 活动模块(福利)
-- @author: yjg
-- @date: 2019年1月2日11:23:34
--

--------------------------------------------------------------------------------------------------------------------------------
--事件
--------------------------------------------------------------------------------------------------------------------------------
--处理角色属性变化
function Modules.ActivityModule:HandleRolePropChange(type, old_value, new_value)
	--VIP等级变化
	if type == Macros.Game.RolePropType.VIP_LEVEL and old_value ~= new_value then
		--VIP礼包数据请求
		self:VipGiftDataReq()
	end
end

--处理角色等级变化
function Modules.ActivityModule:HandleRoleLevelChange(old_level, new_level)
	--统计红点-等级礼包	
	local level_gift_list = CPPGameLib.GetConfig("LevelGift", nil, nil, true)	
	for level, _ in pairs(level_gift_list) do
		if old_level < level and new_level >= level then
			self:StatisticsLevelGiftBag()
			break
		end
	end

	--统计红点-等级基金
	local open_server_fund_list = CPPGameLib.GetConfig("OpenServerFund", nil, nil, true)
	for _, v in pairs(open_server_fund_list) do
		if old_level < v.need_role_level and new_level >= v.need_role_level then
			self:StatisticsRebate()
			break
		end
	end

	--等级商店数据请求
	local level_shop_list = CPPGameLib.GetConfig("LevelShop", nil, nil, true)
	for level, _ in pairs(level_shop_list) do
		if old_level < level and new_level >= level then
			self:LevelShopDataReq()
			break
		end
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------

--开始游戏前，需要请求的活动相关协议
function Modules.ActivityModule:RequestProtocolsForStartGame()
	--登录奖励
	self:LoginRewardDataReq()
	--等级商店
	self:LevelShopDataReq()
	self:LevelShopBuyNotesReq()
	--等级基金
	self:OpenServerFundDataReq()
	--首充
	self:FirstRechargeDataReq()

	--七日登录
	CallModuleFunc(ModuleType.PUSH_SEVEN , "SevenDayLoginDataReq")
	--豪华签到
	CallModuleFunc(ModuleType.CRYSTAL_SHOP, "LuxurySignInDataReq")
	--充值回馈
	CallModuleFunc(ModuleType.FEEDBACK , "RechargeRewardDataReq")
	--女王数据
	CallModuleFunc(ModuleType.QUEEN , "TotalRechargeRewardDataReq")
	--在线奖励
	CallModuleFunc(ModuleType.ONLINE_AWARD , "OnlineRewardDataReq")
	--阵营招募
	CallModuleFunc(ModuleType.LIMIT_ACTIVITY, "CampRecruitDataReq")
end

--------------------------------------------------------------------------------------------------------------------------------
--签到
--------------------------------------------------------------------------------------------------------------------------------
-- 消息说明: 签到数据请求 消息Id: 22701
function Modules.ActivityModule:SignInDataReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SIGN_IN_DATA_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 签到数据响应 消息Id: 22702
function Modules.ActivityModule:SignInDataResp( info )
    if info.last_sign_in_time == 0 then
    	info.last_sign_in_time =  CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime") - (24*60*60)
    end
    self.SignInfo = info
    self:StatisticsRegistration()

    self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_REGISTRATION ,info )
end

-- 消息说明: 签到请求 消息Id: 22703
function Modules.ActivityModule:SignInReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SIGN_IN_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 签到响应 消息Id: 22704
function Modules.ActivityModule:SignInResp( info )
	local cnf = nil
	if self.SignInfo.cur_sign_in_day ~= 0 then
		cnf = CPPGameLib.GetConfig("SignIn", self.SignInfo.cur_sign_in_day)
	else
		local lst = CPPGameLib.GetConfig("SignIn", nil , nil , true )
		cnf = CPPGameLib.GetConfig("SignIn", #lst )
	end

	if not cnf then
		return
	end

    local num = cnf.reward_item_num
    if GlobalCustomizedLogicAdapterMgr:Call("CheckDailySignDoubleReward", cnf.double_reward_need_vip_level) then
    	num = cnf.reward_item_num * 2
    end
    local t1 = {}
    table.insert(t1,{id = cnf.reward_item_id , num = num })
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

    self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_REGISTRATION ,info )
end

--上一次签到的天数是否与今天不一样
function Modules.ActivityModule:IsLastSignInDayDiffToday( )	
	if not self.SignInfo then
		return 
	end

	if self.SignInfo.last_sign_in_time and self.SignInfo.last_sign_in_time ~= 0 then
		local year, month, day, hour, minute, second, week = TimeTool.GetDateFromTimeStamp( self.SignInfo.last_sign_in_time )
		local time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
		local serverYear, serverMonth, serverDay, serverHour, serverMinute, serverSecond, serverWeek = TimeTool.GetDateFromTimeStamp( time )
		if year ~= serverYear or month ~= serverMonth or day ~= serverDay then
			return true
		end
	end
end

--获取当前签到的天数
function Modules.ActivityModule:GetCurSignInDay( )
	if self.SignInfo then
		return self.SignInfo.cur_sign_in_day
	else
		return nil
	end
end

--检测某天是否已签到
function Modules.ActivityModule:CheckDayHasSignIn( day , num )
	num = num or 0
	if self.SignInfo and ( day <= ( self.SignInfo.cur_sign_in_day + num ) ) then
		return true
	end
end

--获得距离下次大奖时间
function Modules.ActivityModule:GetSignInNexBigPrize(  )
	local cnf = CPPGameLib.GetConfig("SignIn", nil , nil , true )
	if not cnf then
		return
	end
	local t1 = {}
	for i = 1 , 2 do
		for _,v in ipairs(cnf) do
			table.insert( t1 , v )
		end
	end

	local num = 0
	for i , v in ipairs( t1 ) do
		if i > self.SignInfo.cur_sign_in_day then
			num = num + 1
			if v.big_prize then
				break
			end
		end
	end
	return num
end

--获得是否可领取
function Modules.ActivityModule:GetSignInIsGet( id )
	--下次领取的id
	local may_id = self.SignInfo.cur_sign_in_day + 1 
	if id == may_id and self:IsLastSignInDayDiffToday() == true then
		return true
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--领取体力（嗑药 or 铜雀台）
--------------------------------------------------------------------------------------------------------------------------------
-- 消息说明: 铜雀台数据请求 消息Id: 22901
function Modules.ActivityModule:TqtDdataReq( info )	
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TQT_DATA_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 铜雀台数据响应 消息Id: 22902
function Modules.ActivityModule:TqtDataResp( info )	
    self.__tgt_last_get_time = info.last_get_time
    self:TQTRecordTidy( info.take_record_list )
    self:StatisticsTongQueTai()

    self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_MEDICINE )
end

-- 消息说明: 铜雀台奖励请求 消息Id: 22903
function Modules.ActivityModule:TqtRewardReq( info )	
   	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TQT_REWARD_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 铜雀台奖励响应 消息Id: 22904
function Modules.ActivityModule:TqtRewardResp( info )	
    local t1 = {}
    local huobi  = CPPGameLib.GetConfig("HuoBi",Macros.Game.RolePropType.STAMINA)
    if not huobi then
    	return
    end
	table.insert( t1 , { id = huobi.item_id , num = ConfigAdapter.Common.GetManual() })
	GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

	self:StatisticsTongQueTai()

    self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_MEDICINE , true )
end

--获得这个时间能不能嗑药
function Modules.ActivityModule:GetTQTIsTakeTime( start_time , over_time )

	--开始时
	local sHour = string.sub( string.format( "%04d" , start_time ) , 0 , 2)
	--开始分
	local sMinute = string.sub( string.format( "%04d" , start_time ) , -2)

	--结束时
	local oHour = string.sub( string.format( "%04d" , over_time ) , 0 , 2)
	--结束分
	local oMinute = string.sub( string.format( "%04d" , over_time ) , -2)	

	--服务器
	local serverTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	--当前服务器时间
	local year, month, day, hour, minute, second, week = TimeTool.GetDateFromTimeStamp(serverTime)

	sTime = tonumber( sHour .. sMinute )
	oTime = tonumber( oHour .. oMinute )
	Time = tonumber( hour .. minute )

	local text = ""
	--如果进行中
	if Time >= sTime and Time <= oTime then
		text = "kaizhe"
	elseif Time > oTime then
		text = "guole"	
	elseif Time < sTime then
		text = "meidao"	
	end

	return text 
end

--获取上次体力领取时间
function Modules.ActivityModule:GetTQTLastGetTime(  )
	return self.__tgt_last_get_time
end

--领取了的整理
function Modules.ActivityModule:TQTRecordTidy( data )
	self.tqtRecord = {}
	for i,v in ipairs(data) do
		self.tqtRecord[v.id] = true
	end
end

--领取了的体力
function Modules.ActivityModule:GetTQTRecordTidy( id )
	if self.tqtRecord then
		return self.tqtRecord[id]
	end
end

--检测体力领取
function Modules.ActivityModule:CheckTQT(calc_red_num)
	local red_num = 0
	local cnf = self:GetTQTcnf()
	if cnf and self.tqtRecord then
		for i, v in ipairs(cnf) do
			local test = self:GetTQTIsTakeTime(v.start_time , v.over_time)
			local has_get = self:GetTQTRecordTidy(i)
			if test == "kaizhe" and not has_get then
				if calc_red_num then
					red_num = red_num + 1
					break
		       	else
		       		return true
		       	end
			end
		end
	end

	if calc_red_num then
		return red_num
	end		
end

--------------------------------------------------------------------------------------------------------------------------------
--摇钱树
--------------------------------------------------------------------------------------------------------------------------------
-- 消息说明: 摇钱树数据请求 消息Id: 22801
function Modules.ActivityModule:YcsDataReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_YCS_DATA_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 摇钱树数据响应 消息Id: 22802
function Modules.ActivityModule:YcsDataResp( info )
   	self.ycsData = info
   	self:StatisticsYingCaiShen()

   	self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_MONEY )
end

-- 消息说明: 摇钱树奖励请求 消息Id: 22803
function Modules.ActivityModule:YcsRewardReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_YCS_REWARD_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 摇钱树奖励响应 消息Id: 22804
function Modules.ActivityModule:YcsRewardResp( info )
	local t1 = {}
	table.insert( t1 , { id = info.type , num = info.value })
	GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
	self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_GET_MONEY )
end

--获得累计的钱
function Modules.ActivityModule:GetYcsAggregateMoney( )
	return self.ycsData.total_reward_value
end

--距离大奖还剩下的次数
function Modules.ActivityModule:GetYcsBigAggregateMoney( )
	return ConfigAdapter.Common.GetAggregateMoneyNum() - self.ycsData.total_get_times
end

--获得今天摇了的次数
function Modules.ActivityModule:GetYcsUsedNum( )
	return self.ycsData.today_get_times .. "/" .. ConfigAdapter.Common.GetAggregateMoneyMax()
end

--获得今天摇了的次数
function Modules.ActivityModule:GetYcsTodayGetTimes( )
	return self.ycsData.today_get_times 
end

--获得领取的时间
function Modules.ActivityModule:GetYcsBeforeTime( )
	if self.ycsData then
		return self.ycsData.last_get_time
	end
end

--获得摇钱树数据
function Modules.ActivityModule:GetYcsData()
	return self.ycsData
end

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--等级商店
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
-- 消息说明: 等级商店数据请求 消息Id: 23701
function Modules.ActivityModule:LevelShopDataReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_LEVEL_SHOP_ACTIVITY_DATA_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 等级商店数据响应 消息Id: 23702
function Modules.ActivityModule:LevelShopDataResp( info )
	self.__level_shop_list = {}

	for i,v in ipairs(info.gift_list) do
		self.__level_shop_list[v.level] = v.activate_time
	end

	self:StatisticsLevelShop()

	self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_SHOP )
end

--按照id获得结束时间
function Modules.ActivityModule:LevelShopToLevelGetEndTime( lv )
	if not self.__level_shop_list then return end
	if self.__level_shop_list[lv] then
		local cnf = CPPGameLib.GetConfig("LevelShop", lv )
		if not cnf then
			return
		end
		local endTime = self.__level_shop_list[lv] + (cnf.valid_time * 60 )
		return endTime
	else
		return
	end
end

-- 消息说明: 等级商店购买 消息Id: 
function Modules.ActivityModule:LevelShopBuyReq( info )
	CallModuleFunc(ModuleType.SHOP, "ShopBuyRequest" , info )
end

-- 消息说明: 等级商店购买返回 消息Id: 
function Modules.ActivityModule:LevelShopBuyResp( info )
	local t1 = {}
	table.insert( t1, { id = info.param1 , num = info.param2 ,type = "buy" } )
	self:LevelShopNotesDispose( t1 )

	self:StatisticsLevelShop()

	self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_SHOP_RECORD)

	--显示奖励
	local cnf = CPPGameLib.GetConfig("LevelShopGoods", info.param1 )
	if not cnf then
		return cnf
	end
	local t1 = {}
    table.insert(t1,{id = cnf.item_id , num = info.param2 * cnf.item_num })
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
end

-- 消息说明: 等级商店购买记录购买 消息Id: 
function Modules.ActivityModule:LevelShopBuyNotesReq(  )
	CallModuleFunc(ModuleType.SHOP, "ShopBuyRecordRequest" , Macros.Game.SHOP.LEVEL_GIFTBAG )
end

-- 消息说明: 等级商店购买记录响应 消息Id: 
function Modules.ActivityModule:LevelShopBuyNotesResp( info )
	local t1 = {}
	for i,v in ipairs(info.buy_record_list) do
		table.insert( t1 , { id = v.goods_id , num = v.buy_num ,type = "notes" } )
	end

	self:LevelShopNotesDispose(t1)
	self:StatisticsLevelShop()

	self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_SHOP_RECORD)
end

--记录处理
function Modules.ActivityModule:LevelShopNotesDispose( info )
	if not self.LevelShopBuyNotes then
		self.LevelShopBuyNotes = {}
	end

	for i,v in ipairs(info) do
		if not self.LevelShopBuyNotes[v.id] then
			self.LevelShopBuyNotes[v.id] = v.num
		else
			if v.type == "buy" then
				self.LevelShopBuyNotes[v.id] = self.LevelShopBuyNotes[v.id] + v.num
			else
				self.LevelShopBuyNotes[v.id] = v.num
			end
		end
	end
end

function Modules.ActivityModule:LevelShopToidGetNum( id )
	if self.LevelShopBuyNotes then
		return self.LevelShopBuyNotes[id] or 0
	else
		return 0
	end
end

function Modules.ActivityModule:LevelShopGetLst( )
	return self.__level_shop_list
end

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--等级礼包
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
-- 消息说明: 等级礼包数据请求 消息Id: 24001
function Modules.ActivityModule:LevelGiftDataReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_LEVEL_GIFT_DATA_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 等级礼包数据响应 消息Id: 24002
function Modules.ActivityModule:LevelGiftDataResp( info )
	self.levelGiftLst = {}
	
	for k,v in pairs(info.level_gift_take_record) do
		self:LevelGiftDispose( v.level )
	end

	self:StatisticsLevelGiftBag()

	self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_LEVEL_GIFTBAG)
end

-- 消息说明: 领取等级礼包请求 消息Id: 24003
function Modules.ActivityModule:TakeLevelGiftRewardReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TAKE_LEVEL_GIFT_REWARD_REQ)
	data.level = info.level
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 领取等级礼包请求响应 消息Id: 24004
function Modules.ActivityModule:TakeLevelGiftRewardResp( info )
	self:LevelGiftDispose( info.level )

	local t1 = {}
	local cnf = CPPGameLib.GetConfig( "LevelGift" , nil , nil ,true )

	for i,v in ipairs( cnf[info.level].item_id_list ) do
		table.insert( t1 , { id = v , num = cnf[info.level].item_num_list[i] } )
	end
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

	self:StatisticsLevelGiftBag()

	self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_LEVEL_GIFTBAG)
end

function Modules.ActivityModule:LevelGiftDispose( level )
	self.levelGiftLst[level] = true
end

function Modules.ActivityModule:GetLevelGiftType( )
	return self.levelGiftLst
end

function Modules.ActivityModule:ToLevelGetLevelGiftType(level)
	if not self.levelGiftLst then
		return
	end
	return self.levelGiftLst[level]
end

function Modules.ActivityModule:GetLevelGiftRedDotNum( )
	local num = 0
	if not self.levelGiftLst then
		return num
	end

	local roleLevel = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.LEVEL)
	for k, v in pairs( CPPGameLib.GetConfig("LevelGift" ,nil , nil,true) ) do
		local is_received_reward = self:ToLevelGetLevelGiftType(v.level)
		if not is_received_reward and v.level <= roleLevel then
			num = num + 1
			if not Modules.Red.BaseRed.IS_SHOW_NUM then
				break
			end
		end
	end

	return num
end


--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--cdk
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
-- 消息说明: 领取活动礼包 消息Id: 20014
function Modules.ActivityModule:TakeActivityGiftReq ( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TAKE_ACTIVITY_GIFT_REQ)
	data.code = info
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 领取活动礼包响应 消息Id: 20015
function Modules.ActivityModule:TakeActivityGiftResp ( info )
    local t1 = {}
    table.insert(t1,{ id = info.gift_id , num = 1 })
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
end


--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--vip福利
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
-- 消息说明: VIP礼包数据请求 消息Id: 23601
function Modules.ActivityModule:VipGiftDataReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_VIP_GIFT_DATA_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: VIP礼包数据响应 消息Id: 23602
function Modules.ActivityModule:VipGiftDataResp( info )
	self.vipGiftData = info
	self:StatisticsVipWelfare()

	self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_VIP_GIFT)
end

-- 消息说明: VIP每日礼包领取 消息Id: 23603
function Modules.ActivityModule:VipDailyGiftGetReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_VIP_DAILY_GIFT_GET_REQ)
	data.vip_level = info.vip_level
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: VIP每日礼包领取响应 消息Id: 23604
function Modules.ActivityModule:VipDailyGiftGetResp( info )
	self.vipGiftData.vip_level_reward_record = info.vip_level
	self:StatisticsVipWelfare()

	--显示奖励
	local now = CPPGameLib.GetConfig("VipDailyGift", info.vip_level , false )
	if not now then
		return
	end
	local t1 = {}
	for i , v in ipairs( now.reward_item_id_list ) do
		table.insert( t1 , { id = v , num = now.reward_item_num_list[i] } )
	end
	GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

	self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_VIP_GIFT)
end

-- 消息说明: VIP周礼包购买 消息Id: 23605
function Modules.ActivityModule:VipWeekGiftBuyReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_VIP_WEEK_GIFT_BUY_REQ)
	data.gift_id = info.gift_id
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: VIP周礼包购买响应 消息Id: 23606
function Modules.ActivityModule:VipWeekGiftBuyResp( info )
	for i , v in ipairs( self.vipGiftData.week_gift_list ) do
		if v.id == info.gift_id then
			v.num = v.num + 1
		end
	end

	self:StatisticsVipWelfare()

	--显示奖励
	local now = CPPGameLib.GetConfig("VipWeekGift", info.gift_id , false )
	if not now then
		return
	end
	local t1 = {}
	for i , v in ipairs( now.item_id_list ) do
		table.insert( t1 , { id = v , num = now.item_num_list[i] } )
	end
	GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

	self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_VIP_GIFT)
end

-- 消息说明: 领取今日推荐礼包 消息Id: 23607
function Modules.ActivityModule:TakeTodayRecommendGiftReq( )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TAKE_TODAY_RECOMMEND_GIFT_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 领取今日推荐礼包响应 消息Id: 23608
function Modules.ActivityModule:TakeTodayRecommendGiftResp( )
	self.vipGiftData.today_recommend_gift_status = 2

	local t1 = {}
	local lst = self:GetVipWeekRecommend()
    for i,v in ipairs(lst) do
        table.insert(t1,{id = v.info_id , num = v.item_num})
    end
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

	self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_RECOMMEND)
end

-- 获得今天的福利
function Modules.ActivityModule:GetVipTodayWelfare( )

	local level = self:GetVipDrawLevel()
	local vipLevel = CallModuleFunc(ModuleType.VIP , "GetVipLV")

	local now = CPPGameLib.GetConfig("VipDailyGift", level + 1 , false )
	if not now then
		now = CPPGameLib.GetConfig("VipDailyGift", level , false )
	end
	
	if vipLevel < now.level then
		now = CPPGameLib.GetConfig("VipDailyGift", vipLevel , false )
	end

	local nex = CPPGameLib.GetConfig("VipDailyGift", ( vipLevel or 0 ) + 1 , false )

	if now then
		local t1 = {}
		t1.lst = {}
		t1.level = now.level
		for i , v in ipairs( now.reward_item_id_list ) do
			table.insert( t1.lst , { info_id = v , item_num = now.reward_item_num_list[i] } )
		end
		now = t1
	end

	if nex then
		local t1 = {}
		t1.lst = {}
		t1.level = nex.level
		for i , v in ipairs( nex.reward_item_id_list ) do
			table.insert( t1.lst , { info_id = v , item_num = nex.reward_item_num_list[i] } )
		end
		nex = t1
	end

	return 	now , nex
end

--获取VIP礼包数据
function Modules.ActivityModule:GetVipGiftData()
	return self.vipGiftData
end

-- 获得领取状态
function Modules.ActivityModule:GetVipDrawLevel( )
	if self.vipGiftData then
		return self.vipGiftData.vip_level_reward_record
	end
end

-- 获得每周的购买列表
function Modules.ActivityModule:GetVipWeekGiftList( )
	local t1 = {}
	if not self.vipGiftData then
		return t1
	end

	local t4 = {}
	for i , v in ipairs( self.vipGiftData.week_gift_list ) do
		local weekGiftCnf = CPPGameLib.GetConfig("VipWeekGift", v.id )

		if weekGiftCnf then
			local t2 = {}
			--物品
			for ii , vv in ipairs( weekGiftCnf.item_id_list ) do
				table.insert( t2 , { info_id = vv , item_num = weekGiftCnf.item_num_list[ii] } )
			end

			--价格
			local t3 = {}
			for ii , vv in ipairs( weekGiftCnf.cost_type_list ) do
				table.insert( t3 , { type = vv , num = weekGiftCnf.cost_value_list[ii] } )
			end

			if weekGiftCnf.buy_num_max - v.num <= 0 then
				table.insert( t4 , { 
										itemLst = t2 , 
										consumeLst = t3 ,  
										max = weekGiftCnf.buy_num_max , 
										num = v.num , 
										buyLevel = weekGiftCnf.need_vip_level ,
										name = weekGiftCnf.name ,
										discount = weekGiftCnf.discount ,
										id = v.id
										} )
			else
				table.insert( t1 , { 
											itemLst = t2 , 
											consumeLst = t3 ,  
											max = weekGiftCnf.buy_num_max , 
											num = v.num , 
											buyLevel = weekGiftCnf.need_vip_level ,
											name = weekGiftCnf.name ,
											discount = weekGiftCnf.discount ,
											id = v.id
											} )

			end
		end
		
	end

	table.sort( t4, function ( a , b )
		return a.buyLevel < b.buyLevel
	end )

	table.sort( t1, function ( a , b  )
		return a.buyLevel < b.buyLevel
	end )

	for i , v in ipairs( t4 ) do
		table.insert( t1 , v )
	end

	return t1
end

function Modules.ActivityModule:GetVipWeekGiftNum( )
	local num_1 = 0
	local num_2 = 0

	--是否VIP等级礼包可以领取
	local drawLevel = self:GetVipDrawLevel()
	local vipLevel = CallModuleFunc(ModuleType.VIP , "GetVipLV")
	if drawLevel and drawLevel < vipLevel then
		num_1 = num_1 + 1
	end

	--是否周礼包可购买
	local weekGiftList = self:GetVipWeekGiftList()
	if weekGiftList then
		for i , v in ipairs( weekGiftList ) do
			if v.buyLevel <= vipLevel and (v.max - v.num) > 0 then
				num_2 = num_2 + 1
				if not Modules.Red.BaseRed.IS_SHOW_NUM then
					break
				end
			end
		end
	end

	return num_1 + num_2 , num_1 , num_2
end

--获得推荐表
function Modules.ActivityModule:GetVipWeekRecommend( )
	local cnf = CPPGameLib.GetConfig("TodayRecommendRecharge", self.vipGiftData.today_recommend_gift_id )
	if not cnf then
		return
	end
	local t1 = {}
	for i , v in ipairs( cnf.reward_item_id_list ) do
		table.insert( t1 , { info_id = v , item_num = cnf.reward_item_num_list[i] } )
	end

	local money = CPPGameLib.GetConfig("Recharge", cnf.recharge_id ).frecharge_diamond
	return t1 , money , cnf.recharge_id 
end

--获得推荐表
function Modules.ActivityModule:GetVipWeekRecommendBuyType( )
	--今日推荐礼包状态(0=不能领取,1=可领取,2=已领取)
	return self.vipGiftData.today_recommend_gift_status
end


--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--基金
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
-- 消息说明: 开服基金数据请求 消息Id: 23801
function Modules.ActivityModule:OpenServerFundDataReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_OPEN_SERVER_FUND_DATA_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 开服基金数据响应 消息Id: 23802
function Modules.ActivityModule:OpenServerFundDataResp( info )
	self.openServerData = info

	-- 基金奖励领取记录
	local t1 = {}
	for i , v in ipairs( self.openServerData.fund_reward_take_record ) do
		t1[ v.reward_id ] = true
	end
	self.openServerData.fund_reward_take_record = t1

	-- 全民福利领取记录
	local t1 = {}
	for i , v in ipairs( self.openServerData.all_people_welfare_take_record ) do
		t1[ v.welfare_id ] = true
	end
	self.openServerData.all_people_welfare_take_record = t1

	self:StatisticsFund()
	self:StatisticsRebate()

	--刷新信息
	self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_FUND)
	--刷新记录
	self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_FUND_RECORD)
end

-- 消息说明: 开服基金购买请求 消息Id: 23803
function Modules.ActivityModule:OpenServerFundBuyReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_OPEN_SERVER_FUND_BUY_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 开服基金购买响应 消息Id: 23804
function Modules.ActivityModule:OpenServerFundBuyResp( info )
	self.openServerData.fund_buy_tims = self.openServerData.fund_buy_tims + 1
	self.openServerData.is_already_buy_fund = 1
	GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("shop_success"))
	self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_FUND)
end

-- 消息说明: 开服基金奖励领取请求 消息Id: 23805
function Modules.ActivityModule:OpenServerFundRewardTakeReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_OPEN_SERVER_FUND_REWARD_TAKE_REQ)
	data.reward_id = info.reward_id
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 开服基金奖励领取响应 消息Id: 23806
function Modules.ActivityModule:OpenServerFundRewardTakeResp( info )
	local cnf = CPPGameLib.GetConfig("OpenServerFund", info.reward_id )
	if not cnf then
		return
	end
	local t1 = {}
    table.insert(t1,{ type = Macros.Game.RolePropType.DIAMOND , num = cnf.diamond })
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

    self.openServerData.fund_reward_take_record[ info.reward_id ] = true

	self:StatisticsFund()
	self:StatisticsRebate()

	--刷新记录
	self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_FUND_RECORD)
end

-- 获得基金表
function Modules.ActivityModule:GetOpenServerFundCnf( )
	local t1 = {}
	local t2 = {}
	for i , v in ipairs( CPPGameLib.GetConfig("OpenServerFund", nil, nil , true ) ) do
		local record = self:GetOpenServerAwardRecord( i )
		if record then
			table.insert( t1 , {id = i , need_role_level = v.need_role_level , diamond = v.diamond } )
		else
			table.insert( t2 , {id = i , need_role_level = v.need_role_level , diamond = v.diamond } )
		end
	end

	for i , v in ipairs( t1 ) do
		table.insert( t2 , v )
	end
	return t2
end

--获得基金购买信息
function Modules.ActivityModule:GetOpenServerData( )
	return self.openServerData
end

--获得基金购买人数
function Modules.ActivityModule:GetOpenServerNum( )
	local nowLab = string.format( "%04d" , self.openServerData.fund_buy_tims )
	--把初始搞成tab
	local nowTab = {}
	for i = 1 , string.len(nowLab) do
		local t = string.sub(nowLab, i , i+1 )
		table.insert(nowTab , string.sub(t,0,1) )
	end
	return nowTab , self.openServerData.fund_buy_tims 
end

--获得基金奖励记录
function Modules.ActivityModule:GetOpenServerAwardRecord( id )
	return self.openServerData.fund_reward_take_record[ id ]
end

function Modules.ActivityModule:GetIsAlreadyBuyFund( )
	return self.openServerData.is_already_buy_fund
end


--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--全民福利
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
-- 消息说明: 全民福利领取请求 消息Id: 23807
function Modules.ActivityModule:AllPeopleWelfareTakeReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ALL_PEOPLE_WELFARE_TAKE_REQ)
	data.welfare_id = info.welfare_id
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 全民福利领取响应 消息Id: 23808
function Modules.ActivityModule:AllPeopleWelfareTakeResp( info )
	local cnf = CPPGameLib.GetConfig("AllThePeopleWelfare", info.welfare_id )
	if not cnf then
		return
	end
	local t1 = {}
    for i , v in ipairs( cnf.item_id_list ) do
		table.insert(t1,{ id = v , num = cnf.item_num_list[i] })
    end
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

	self.openServerData.all_people_welfare_take_record[ info.welfare_id ] = true

	self:StatisticsFund()
	self:StatisticsRebate()

	--刷新记录
	self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_FUND_RECORD)
end

-- 获得福利表
function Modules.ActivityModule:GetAllThePeopleWelfareCnf( )
	local t1 = {}
	local t2 = {}
	for i , v in ipairs( CPPGameLib.GetConfig("AllThePeopleWelfare", nil, nil , true ) ) do
		v.id = i
		local record = self:GetAllPeopleWelfareTakeRecord( v.id )
		if record then
			table.insert( t1 , v )
		else
			table.insert( t2 , v )
		end
	end

	for i , v in ipairs( t1 ) do
		table.insert( t2 , v )
	end
	return t2
end

--获得基金奖励记录
function Modules.ActivityModule:GetAllPeopleWelfareTakeRecord( id )
	return self.openServerData.all_people_welfare_take_record[ id ]
end

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--登录奖励 
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
-- 消息说明: 登录奖励数据请求 消息Id: 25501
function Modules.ActivityModule:LoginRewardDataReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_LOGIN_REWARD_DATA_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 登录奖励数据响应 消息Id: 25502
function Modules.ActivityModule:LoginRewardDataResp( info )
	self.loginReward = {}

	self.login_days = info.login_days
	for i,v in ipairs( info.reward_record_list ) do
		self.loginReward[v.days] = true
	end

	self:StatisticsLoginReward()

	self:FireNextFrame(Macros.Event.ActivityModule.REGISTER_AWARD)
end

-- 消息说明: 领取登录奖励请求 消息Id: 25503
function Modules.ActivityModule:TakeLoginRewardReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TAKE_LOGIN_REWARD_REQ)
	data.days = info.days
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 登录奖励数据响应 消息Id: 25502
function Modules.ActivityModule:TakeLoginRewardResp( info )
	local lst = CPPGameLib.GetConfig("LoginReward", info.days )
	local t1 = {}
    for i,v in ipairs(lst.reward_item_id) do
        table.insert(t1,{id = v , num = lst.reward_item_num[i] })
    end
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

    self.loginReward[ info.days ] = true
    self:StatisticsLoginReward()

    self:FireNextFrame(Macros.Event.ActivityModule.REGISTER_AWARD)
end

function Modules.ActivityModule:GetLoginRewardLst( )
	return self.loginReward
end

function Modules.ActivityModule:ToDaysGetLoginReward( days )
	if self.loginReward then
		return self.loginReward[days]
	end
end

--可领取的天数
function Modules.ActivityModule:GetLoginRewardRegisterDay( )
	return self.login_days or 0
end

--检测登录奖励是否可领奖
function Modules.ActivityModule:CheckLoginReward(calc_red_num)
	local red_num = 0
	local login_days = self:GetLoginRewardRegisterDay()
	for days, v in pairs(CPPGameLib.GetConfig("LoginReward", nil , nil , true )) do
		local is_received_reward = self:ToDaysGetLoginReward(days)
		if login_days >= days and not is_received_reward then
			if calc_red_num then
				red_num = red_num + 1
			    if not Modules.Red.BaseRed.IS_SHOW_NUM then
	       			break
	       		end			
			else
				return true
			end
		end
	end

	if calc_red_num then
		return red_num
	end
end

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--单充 
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
-- 消息说明: 单充数据请求 消息Id: 24503
function Modules.ActivityModule:SingleRechargeDataReq( info )
	if not CallModuleFunc(ModuleType.CHARGE , "GetRechargeData") then
		self:AccumulateRechargeDataReq()
		CallModuleFunc(ModuleType.CHARGE , "RechargeDataReq")
	end
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SINGLE_RECHARGE_DATA_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 单充数据响应 消息Id: 24504
function Modules.ActivityModule:SingleRechargeDataResp( info )	
	local t1 = {}
	for i , v in ipairs( info.reward_record_list ) do
		local cnf = CPPGameLib.GetConfig("LimitActivity", v.data_id )
		if not cnf then
			return 
		end
		cnf = CPPGameLib.GetConfig("Recharge", cnf.context )
		t1[ cnf.money ] = v
	end

	local recharge_list = info.recharge_list
	self.singleRechargeData = {}
	if recharge_list then
		for i,v in pairs( recharge_list ) do
			local cnf = CPPGameLib.GetConfig("Recharge", v.id )
				
			if not cnf then return end

			if not t1[ cnf.money ] then

				self.singleRechargeData[ cnf.money  ] = {}
				--可以购买的次数
				self.singleRechargeData[ cnf.money  ].may = v.times
				--充值的次数
				self.singleRechargeData[ cnf.money  ].recharge = v.times
				--已经领取的次数
				self.singleRechargeData[ cnf.money  ].already = 0

			else

				if not self.singleRechargeData[ cnf.money  ] then
					self.singleRechargeData[ cnf.money  ] = {}
				end
				self.singleRechargeData[ cnf.money  ].may = v.times - t1[ cnf.money  ].num
				self.singleRechargeData[ cnf.money  ].recharge = v.times
				self.singleRechargeData[ cnf.money  ].already = t1[ cnf.money  ].num

			end
		end
	end

	self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_ONE_RECHARGE)
end

-- 消息说明: 领取限时活动奖励请求 消息Id: 24509
function Modules.ActivityModule:TakeLimitActivityRewardReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TAKE_LIMIT_ACTIVITY_REWARD_REQ)
	data.data_id = info.data_id
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 领取限时活动奖励响应 消息Id: 24510
function Modules.ActivityModule:TakeLimitActivityRewardResp( info )
	--单充
	local cnf = CPPGameLib.GetConfig("LimitActivity", info.data_id )
	if not cnf then return end

	if cnf.type == Macros.Game.LIMIT_ACTIVITY_TYPE.ONE_RECHARGE then
		local rechargeCnf = CPPGameLib.GetConfig("Recharge", cnf.context )
		if not rechargeCnf then return end
		self.singleRechargeData[ rechargeCnf.money  ].may = self.singleRechargeData[ rechargeCnf.money ].may - 1
		self.singleRechargeData[ rechargeCnf.money  ].already = self.singleRechargeData[ rechargeCnf.money  ].already + 1
	--累充
	elseif cnf.type == Macros.Game.LIMIT_ACTIVITY_TYPE.GRAND_TOTAL_RECHARGE then
		table.insert( self.accumulateRechargeData.reward_record_list , { data_id = info.data_id } )
	
	--限时招募
	elseif cnf.type == Macros.Game.LIMIT_ACTIVITY_TYPE.RECRUIT then
		table.insert( self.limitRecruitData.reward_record_list , { data_id = info.data_id } )

	--首充
	elseif cnf.type == Macros.Game.LIMIT_ACTIVITY_TYPE.INITIAL then
		table.insert( self.firstRechargeData.reward_record_list , { data_id = info.data_id } )
	end

	local t1 = {}
	for i , v in ipairs( cnf.reward_item_id_list ) do
		table.insert(t1,{id = v , num = cnf.reward_item_num_list[i] })
	end
	GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

	self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_ONE_RECHARGE)
	self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_RECRUIT)
end

-- 2017年6月28日10:39:18
-- 设计是 对应价钱领取对应奖励 不支持最高 确认次数 3
--获得目前单充列表
function Modules.ActivityModule:GetOneRechargeMax( id )
	if self.singleRechargeData then
		return self.singleRechargeData[id]
	end
end

--获得单充记录
function Modules.ActivityModule:GetOneRechargeRecord( id )
	local cnf = CPPGameLib.GetConfig("LimitActivity", id )
	if not cnf then return end
	local rechargeCnf = CPPGameLib.GetConfig("Recharge", cnf.context )
	if not rechargeCnf then return end

	if self.singleRechargeData and self.singleRechargeData[rechargeCnf.money] then
		if self.singleRechargeData[rechargeCnf.money].already >= cnf.reward_times_max then
			return true
		end
	end
end

--获得单充表
function Modules.ActivityModule:GetOneRechargeCnf( ... )
	if not self.limitActivity then
		self:SettleLimitActivity()
	end

	local lst = self.limitActivity[ Macros.Game.LIMIT_ACTIVITY_TYPE.ONE_RECHARGE ]

	table.sort( lst, function ( a , b )
		return a.id < b.id
	end )

	local t2 = {}
	local t3 = {}

	for _ , v in ipairs( lst ) do
		local cnf = CPPGameLib.GetConfig("Recharge", v.context )
		if not cnf then return end
		v.money = cnf.money

		local t1 = {}
		for i , vv in ipairs( v.reward_item_id_list ) do
			table.insert( t1 , { id = vv , num = v.reward_item_num_list[i] } )
		end
		v.item = t1

		if self:GetOneRechargeRecord( v.id ) then
			table.insert( t3 , v )
		else
			table.insert( t2 , v )
		end
	end

	for i , v in ipairs(t3) do
		table.insert( t2 , v )
	end

	return t2
end

function Modules.ActivityModule:GetOneRechargeNum( )
	local num = 0
	local t1 = self:GetOneRechargeCnf()
	for i , v in ipairs( t1 ) do
		local info = self:GetOneRechargeMax( v.money )
	    local record = self:GetOneRechargeRecord( v.id )
	    if info and v.reward_times_max >= info.already then
	        if not record then
			    if not info or v.reward_times_max < info.already or info.may == 0 then
				else
					num = num + 1
				end
	        end
	    end
	end
	return num
end


--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--累充 
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
-- 消息说明: 累充数据请求 消息Id: 24505
function Modules.ActivityModule:AccumulateRechargeDataReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ACCUMULATE_RECHARGE_DATA_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 累充数据响应 消息Id: 24506
function Modules.ActivityModule:AccumulateRechargeDataResp( info )
	self.accumulateRechargeData = info
	self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_ONE_RECHARGE)
end

--获得目前累充最高
function Modules.ActivityModule:GetGrandTotalRechargeMax( )
	if self.accumulateRechargeData then
		return self.accumulateRechargeData.accumulate_recharge
	end
end

--充值了
function Modules.ActivityModule:SetGrandTotalRechargeMax( num )
	if self.accumulateRechargeData then
		self.accumulateRechargeData.accumulate_recharge = self.accumulateRechargeData.accumulate_recharge + ( num or 0 )
	end
end


--获得累充记录
function Modules.ActivityModule:GetGrandTotalRechargeRecord( id )
	if self.accumulateRechargeData then
		for i,v in ipairs( self.accumulateRechargeData.reward_record_list ) do
			if v.data_id == id then
				return true
			end
		end
	end
end

--获得累充表
function Modules.ActivityModule:GetGrandTotalRechargeCnf( ... )
	if not self.limitActivity then
		self:SettleLimitActivity()
	end

	local lst = self.limitActivity[ Macros.Game.LIMIT_ACTIVITY_TYPE.GRAND_TOTAL_RECHARGE ]

	table.sort( lst, function ( a , b )
		return a.id < b.id
	end )

	local t2 = {}
	local t3 = {}

	for _ , v in ipairs( lst ) do
		local t1 = {}
		for i , vv in ipairs( v.reward_item_id_list ) do
			table.insert( t1 , { id = vv , num = v.reward_item_num_list[i] } )
		end
		v.item = t1

		if self:GetGrandTotalRechargeRecord( v.id ) then
			table.insert( t3 , v )
		else
			table.insert( t2 , v )
		end
	end

	for i , v in ipairs(t3) do
		table.insert( t2 , v )
	end

	return t2
end

function Modules.ActivityModule:GetGrandTotalRechargeNum( )
    local t1 = self:GetGrandTotalRechargeCnf()
	local max = self:GetGrandTotalRechargeMax()

	local num = 0
    for i , v in ipairs( t1 ) do
	    local record = self:GetGrandTotalRechargeRecord( v.id )
	    --充值够了
	    if max and v.context <= max then
	    	--有记录
	    	if not record then
	    		num = num + 1
	    	end
	    end
    end

    return num
end


--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--限时召唤 
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
-- 消息说明: 限时招募数据请求 消息Id: 24507
function Modules.ActivityModule:LimitRecruitDataReq( info )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_LIMIT_RECRUIT_DATA_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 限时招募数据响应 消息Id: 24508
function Modules.ActivityModule:LimitRecruitDataResp( info )
	self.limitRecruitData = info
	self:FireNextFrame(Macros.Event.ActivityModule.ACTIVITY_RECRUIT)
end

--获得目前招募最高
function Modules.ActivityModule:GetLimitRecruitMax( )
	if self.limitRecruitData then
		return self.limitRecruitData.recruit_times
	end
end

--获得招募记录
function Modules.ActivityModule:GetLimitRecruitRecord( id )
	if self.limitRecruitData then
		for i,v in ipairs( self.limitRecruitData.reward_record_list ) do
			if v.data_id == id then
				return true
			end
		end
	end
end

--获得招募表
function Modules.ActivityModule:GetLimitRecruitCnf( ... )
	if not self.limitActivity then
		self:SettleLimitActivity()
	end

	local lst = self.limitActivity[ Macros.Game.LIMIT_ACTIVITY_TYPE.RECRUIT ]

	table.sort( lst, function ( a , b )
		return a.id < b.id
	end )

	local t2 = {}
	local t3 = {}

	for _ , v in ipairs( lst ) do
		local t1 = {}
		for i , vv in ipairs( v.reward_item_id_list ) do
			table.insert( t1 , { id = vv , num = v.reward_item_num_list[i] } )
		end
		v.item = t1

		if self:GetLimitRecruitRecord( v.id ) then
			table.insert( t3 , v )
		else
			table.insert( t2 , v )
		end
	end

	for i , v in ipairs(t3) do
		table.insert( t2 , v )
	end

	return t2
end

function Modules.ActivityModule:GetLimitRecruitNum( )
    local t1 = self:GetLimitRecruitCnf()
	local max = self:GetLimitRecruitMax()

	local num = 0
    for i , v in ipairs( t1 ) do
	    local record = self:GetLimitRecruitRecord( v.id )
	    --充值够了
	    if max and v.context <= max then
	    	--有记录
	    	if not record then
	    		num = num + 1
	    	end
	    end
    end
    return num
end

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--统计红点 
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
function Modules.ActivityModule:StatisticsRegistration( )
	CallModuleFunc( ModuleType.RED_MGR , "StatisticsRed" , Macros.Game.SystemID.ACTIVITY , nil , Macros.Game.SystemID.DAILY_SIGNIN)
end

function Modules.ActivityModule:StatisticsTongQueTai( )
	CallModuleFunc( ModuleType.RED_MGR , "StatisticsRed" , Macros.Game.SystemID.ACTIVITY , nil , Macros.Game.SystemID.TQT)
end

function Modules.ActivityModule:StatisticsYingCaiShen( )
	CallModuleFunc( ModuleType.RED_MGR , "StatisticsRed" , Macros.Game.SystemID.ACTIVITY , nil , Macros.Game.SystemID.YCS)
end

function Modules.ActivityModule:StatisticsLevelShop( )
	CallModuleFunc( ModuleType.RED_MGR , "StatisticsRed" , Macros.Game.SystemID.ACTIVITY , nil , Macros.Game.SystemID.LEVEL_SHOP)
end

function Modules.ActivityModule:StatisticsLevelGiftBag( )
	CallModuleFunc( ModuleType.RED_MGR , "StatisticsRed" , Macros.Game.SystemID.ACTIVITY , nil , Macros.Game.SystemID.LEVEL_GIFTBAG)
end

function Modules.ActivityModule:StatisticsVipWelfare( )
	CallModuleFunc( ModuleType.RED_MGR , "StatisticsRed" , Macros.Game.SystemID.ACTIVITY , nil , Macros.Game.SystemID.VIP_WELFARE)
end

function Modules.ActivityModule:StatisticsLunaWelfare( )
	CallModuleFunc( ModuleType.RED_MGR , "StatisticsRed" , Macros.Game.SystemID.ACTIVITY , nil , Macros.Game.SystemID.LUNA_WELFARE)
end

function Modules.ActivityModule:StatisticsFund( )
	CallModuleFunc( ModuleType.RED_MGR , "StatisticsRed" , Macros.Game.SystemID.ACTIVITY , nil , Macros.Game.SystemID.FUND)
end

function Modules.ActivityModule:StatisticsRebate( )
	CallModuleFunc( ModuleType.RED_MGR , "StatisticsRed" , Macros.Game.SystemID.ACTIVITY , nil , Macros.Game.SystemID.REBATE)
end

function Modules.ActivityModule:StatisticsLoginReward( )
	CallModuleFunc( ModuleType.RED_MGR , "StatisticsRed" , Macros.Game.SystemID.ACTIVITY , nil, Macros.Game.SystemID.REGISTER_AWARD)
end
