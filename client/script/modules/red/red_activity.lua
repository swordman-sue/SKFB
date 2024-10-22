
--[[
%% @module: 活动红点红点
%% @author: yjg
%% @created: 2018年12月24日20:20:16
--]]
Modules.Red.Activity = Modules.Red.Activity or BaseClass( Modules.Red.BaseRed )

function Modules.Red.Activity:__init( ... )
	self.red = nil
	self.red_map = {}
end

--红点统计逻辑
function Modules.Red.Activity:__Statistics( sub_system_id )
    -- 签到
	if sub_system_id == Macros.Game.SystemID.MAX or sub_system_id == Macros.Game.SystemID.DAILY_SIGNIN then
		self:DailySignin( )
	end

    -- 体力
	if sub_system_id == Macros.Game.SystemID.MAX or sub_system_id == Macros.Game.SystemID.TQT then
		self:TongQueTai( )
	end

    -- 摇钱树
	if sub_system_id == Macros.Game.SystemID.MAX or sub_system_id == Macros.Game.SystemID.YCS then
		self:YingCaiShen( )
	end
    
    -- 等级特惠
	if sub_system_id == Macros.Game.SystemID.MAX or sub_system_id == Macros.Game.SystemID.LEVEL_SHOP then
		self:LevelShop( )
	end

    -- 等级礼包
	if sub_system_id == Macros.Game.SystemID.MAX or sub_system_id == Macros.Game.SystemID.LEVEL_GIFTBAG then
		self:LevelGiftBag( )
	end

    -- VIP礼包
	if sub_system_id == Macros.Game.SystemID.MAX or sub_system_id == Macros.Game.SystemID.VIP_WELFARE then
		self:VipWelfare( )
	end

    -- 月卡
	if sub_system_id == Macros.Game.SystemID.MAX or sub_system_id == Macros.Game.SystemID.LUNA_WELFARE then
		self:LunaWelfare( )
	end

    -- 全民福利
	if sub_system_id == Macros.Game.SystemID.MAX or sub_system_id == Macros.Game.SystemID.FUND then
		self:Fund( )
	end

    -- 等级基金
	if sub_system_id == Macros.Game.SystemID.MAX or sub_system_id == Macros.Game.SystemID.REBATE then   
		self:Rebate( )
	end

	--登录奖励    
	if sub_system_id == Macros.Game.SystemID.MAX or sub_system_id == Macros.Game.SystemID.REGISTER_AWARD then
		self:LoginReward( )
	end
end

--每日签到
function Modules.Red.Activity:DailySignin()
	self.red_map[Macros.Game.SystemID.DAILY_SIGNIN] = CallModuleFunc(ModuleType.ACTIVITY, "IsLastSignInDayDiffToday") and 1 or 0
end

-- 体力
function Modules.Red.Activity:TongQueTai( ... )
	self.red_map[Macros.Game.SystemID.TQT] = CallModuleFunc(ModuleType.ACTIVITY, "CheckTQT", true)
end

-- 摇钱树
function Modules.Red.Activity:YingCaiShen( ... )
	local red_num = 0

	local ycsData = CallModuleFunc(ModuleType.ACTIVITY, "GetYcsData" )
	if ycsData then
		--检测领取大奖状态
		if CallModuleFunc(ModuleType.ACTIVITY, "GetYcsBigAggregateMoney") <= 0 then
			red_num = 1

		--检测今日可领取状态
		elseif ycsData.last_get_time + ConfigAdapter.Common.GetAggregateMoneyTime()	<= CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime") and 
			ycsData.today_get_times < ConfigAdapter.Common.GetAggregateMoneyMax() then
				red_num = 1
		end
	end

	self.red_map[Macros.Game.SystemID.YCS] = red_num
end

-- 等级特惠
function Modules.Red.Activity:LevelShop()
	local red_num = 0
	local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	local level_shop_data_list = CallModuleFunc(ModuleType.ACTIVITY, "LevelShopGetLst")
	for lv, activate_time in pairs(level_shop_data_list) do
		local config = CPPGameLib.GetConfig("LevelShop", lv)

		--检测时效性
		if config and config.valid_time * 60 + activate_time > server_time then
		    for i, good_id in ipairs(config.goods_id_list) do
		    	--检测可购买次数
				local buy_num = CallModuleFunc(ModuleType.ACTIVITY, "LevelShopToidGetNum", good_id)
				local config_level_shop_good = CPPGameLib.GetConfig("LevelShopGoods", good_id)
				if config_level_shop_good and config_level_shop_good.buy_num_max - (buy_num or 0) > 0 then
					red_num = red_num + 1
				    if not Modules.Red.BaseRed.IS_SHOW_NUM then
		       			break
		       		end			
				end
		    end			
		end

	    if red_num > 0 and not Modules.Red.BaseRed.IS_SHOW_NUM then
   			break
   		end					
	end

    self.red_map[Macros.Game.SystemID.LEVEL_SHOP] = red_num
end

-- 等级礼包
function Modules.Red.Activity:LevelGiftBag( ... )
	self.red_map[Macros.Game.SystemID.LEVEL_GIFTBAG] = CallModuleFunc(ModuleType.ACTIVITY, "GetLevelGiftRedDotNum")
end

-- VIP礼包
function Modules.Red.Activity:VipWelfare( ... )
	self.red_map[Macros.Game.SystemID.VIP_WELFARE] = CallModuleFunc(ModuleType.ACTIVITY, "GetVipWeekGiftNum")
end

-- 月卡
function Modules.Red.Activity:LunaWelfare( ... )
    local lst = CallModuleFunc(ModuleType.CHARGE, "GetMonthCard")
    local num = 0
    if lst and lst.month_card_list then
        for i , v in ipairs( lst.month_card_list ) do
            if v.card_id == 1001 then
                local cnf = CPPGameLib.GetConfig( "MonthCard", v.card_id )
                --结束时间
                local endTime = v.buy_time + ( cnf.valid_time * 24 * 60 * 60 ) 
                local serverTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
                local __, __, day1, __, __, __, __ = TimeTool.GetDateFromTimeStamp( v.last_take_reward_time )
                --今天
                local __, __, day2, __, __, __, __ = TimeTool.GetDateFromTimeStamp( serverTime )
                --剩余时间
                local residueTime = endTime - serverTime
                if residueTime <= 0 then
                else
                    if v.last_take_reward_time ~= 0 and day1 == day2 then
                    else
                        num = num + 1
					    if not Modules.Red.BaseRed.IS_SHOW_NUM then
			       			break
			       		end			
                    end
                end

            elseif v.card_id == 1002 then
                --结束时间
                local serverTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
                --剩余时间
                if v.buy_time == 0 then
                else
                    --上次使用的时间
                    local __, __, day1, __, __, __, __ = TimeTool.GetDateFromTimeStamp( v.last_take_reward_time )
                    --今天
                    local __, __, day2, __, __, __, __ = TimeTool.GetDateFromTimeStamp( serverTime )
                    if v.last_take_reward_time ~= 0  and day1 == day2 then
                    else
                        num = num + 1
					    if not Modules.Red.BaseRed.IS_SHOW_NUM then
			       			break
			       		end			
                    end
                end
            end
        end
    end

	self.red_map[Macros.Game.SystemID.LUNA_WELFARE] = num
end

-- 全民福利
function Modules.Red.Activity:Fund( ... )
	local t1 = CallModuleFunc(ModuleType.ACTIVITY, "GetAllThePeopleWelfareCnf")
	local _ , now = CallModuleFunc(ModuleType.ACTIVITY, "GetOpenServerNum")
    local openServerData = CallModuleFunc(ModuleType.ACTIVITY, "GetOpenServerData")

    local num = 0
    for i , v in ipairs( t1 ) do
	    local record = CallModuleFunc(ModuleType.ACTIVITY, "GetAllPeopleWelfareTakeRecord" , v.id )
	    --等级够了
	    if v.need_buy_num <= now then
	    	--有记录
	    	if not record then
	    		num = num + 1
			    if not Modules.Red.BaseRed.IS_SHOW_NUM then
	       			break
	       		end
	    	end
	    end
    end

    self.red_map[Macros.Game.SystemID.FUND] = num
end

-- 基金
function Modules.Red.Activity:Rebate( ... )
	local roleLevel = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	local openServerData = CallModuleFunc(ModuleType.ACTIVITY, "GetOpenServerData")

	local num = 0
	local t1 = CallModuleFunc(ModuleType.ACTIVITY, "GetOpenServerFundCnf")
	for i , v in ipairs( t1 ) do
		local record = CallModuleFunc(ModuleType.ACTIVITY, "GetOpenServerAwardRecord" , v.id )
	    --等级够了
	    if v.need_role_level <= roleLevel and openServerData.is_already_buy_fund ~= 0 then
	    	--有记录
	    	if not record then
	    		num = num + 1
	    		if not Modules.Red.BaseRed.IS_SHOW_NUM then
	       			break
	       		end
	    	end
		end
	end
	self.red_map[Macros.Game.SystemID.REBATE] = num
end

--登录奖励  
function Modules.Red.Activity:LoginReward()
	local red_num = CallModuleFunc(ModuleType.ACTIVITY, "CheckLoginReward", true)
	self.red_map[Macros.Game.SystemID.REGISTER_AWARD] = red_num
end

