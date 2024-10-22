--
-- @brief 推送处理
-- @author: yjg
-- @date: 2017年8月2日09:58:59
--

Modules = Modules or {}

Modules.PushModule = Modules.PushModule or BaseClass(BaseModule)

function Modules.PushModule:__init()
    self:__Server()

	self.openKey = false
    self.system_id = nil

	self.lst = self:GetPushContent()

	table.sort( self.lst, function( a , b )
    	return a.sort < b.sort
	end )

	self.systemLst = {}
	for i , v in ipairs( self.lst ) do
		self.systemLst[v.system_id] = v
	end

    --当前时间 
    local n_Time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
    local n_nian, n_yue, n_ri, n_shi, n_fen, n_miao, n_xingqi = TimeTool.GetDateFromTimeStamp(n_Time)
    local t_time = string.format( "%04d" , n_nian )..string.format( "%02d" , n_yue )..string.format( "%02d" , n_ri )

    --清空旧数据
    local t2 = CPPGameLib.SplitEx( Cookies.Get( nil ,Macros.Setting.PushRecord) , "<GH>")
    local first = ""
    for i , v in ipairs( t2 ) do
        local time = string.sub( v , 0 , 8 )
        if t_time == time then
            first = first .. v .."<GH>"
        end
    end
    Cookies.Set( nil , Macros.Setting.PushRecord , first )
end

function Modules.PushModule:__delete()
	if self.time then
		GlobalTimerQuest:CancelQuest(self.time)
	    self.time = nil
	end
end

--数据现行
function Modules.PushModule:__Server()
	CallModuleFunc(ModuleType.MORROW,"NextDayRewardDataReq")
    CallModuleFunc(ModuleType.SUPERDISCOUNTVIEW , "SuperDiscountDataReq")
end

--中转
function Modules.PushModule:Transfer( systemid , jump )   

	self.openKey = true

	if StartConfig.ClosePush and StartConfig.ClosePush == true then
		return 
	end

    local viewInfo = GlobalViewMgr:IsOnlyCityViewOpen()

	--界面开着
    if viewInfo then
		--如果没有传进来id
		if not systemid then
			--预留时间等数据回来？
			self.time = GlobalTimerQuest:AddPeriodQuest(
			    GlobalCallbackMgr:AddCallBackFunc(function()

				GlobalTimerQuest:CancelQuest(self.time)
			    self.time = nil

				for i , v in ipairs( self.lst ) do
					local type_ = self:Valve( v.system_id )
                    if type_ == true then
						break
					end
				end

			end),0.5,-1)

		--有id进来
		else
			--拿到顺位
			local sort = self.systemLst[ systemid ].sort
            if jump then
                sort = sort + 1
            end            
			local id = nil
			for i,v in ipairs( self.lst ) do
				if v.sort > sort then
					id = v.system_id
                    local type_ = self:Valve( id )  
                    if type_ == true then
						break
					end
                elseif i == #self.lst then
                    self.system_id = nil
				end
			end
		end
    end
end

--数据判断
function Modules.PushModule:Valve( id )

    --是否首次
    if not self:IsFirst( id ) then
        return 
    end

	--次日--90
	if id == Macros.Game.SystemID.MORROW then  
		if CallModuleFunc(ModuleType.MORROW,"GetOpenKey") == true then
			CallModuleFunc(ModuleType.MORROW, "OpenView", BaseViewType.MORROW , "push" )
			return true
		end

	--首充--76
	elseif id == Macros.Game.SystemID.FIRST_RECHARGE then 
        --首充
        local first_recharge_num = CallModuleFunc(ModuleType.ACTIVITY,"GetFirstRechargeOver")
        if first_recharge_num ~= 0 then
			CallModuleFunc(ModuleType.ACTIVITY, "OpenView", BaseViewType.FIRST_RECHARGE , "push" )
            return true
        end

    --天梯--108
    elseif id == Macros.Game.SystemID.LADDER then 
        CallModuleFunc(ModuleType.PUSH, "OpenView", BaseViewType.PUSH_LADDER , "push" )
        return true

    --超值购--94
    elseif id == Macros.Game.SystemID.SUPERDISCOUNTVIEW then 
        --按钮状态(0=还没购买, 1=已购买(可领取), 2=已领取)
        local openType = GlobalModulesMgr:IsOpen( Macros.Game.SystemID.SUPERDISCOUNTVIEW )
        if openType then
            local status = CallModuleFunc(ModuleType.SUPERDISCOUNTVIEW , "GetStatus") 
            local lst = CallModuleFunc(ModuleType.SUPERDISCOUNTVIEW,"GetNowTimeAward")
            if status == 0 and lst then
                CallModuleFunc(ModuleType.SUPERDISCOUNTVIEW, "OpenView", BaseViewType.SUPERDISCOUNTVIEW , "push" )
                return true
            end
        end

    --七日登录--126
    elseif id == Macros.Game.SystemID.PUSH_SEVEN then 

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
            CallModuleFunc(ModuleType.PUSH_SEVEN, "OpenView", BaseViewType.PUSH_SEVEN , "push" ) 
            return true
        end
    --等级基金
    elseif id == Macros.Game.SystemID.REBATE then 
        local is_buy_fund = CallModuleFunc(ModuleType.ACTIVITY,"GetIsAlreadyBuyFund")
        if is_buy_fund == Macros.Global.FALSE then
            local cnf = CPPGameLib.GetConfig("NewPush", nil, nil, true)
            CallModuleFunc(ModuleType.PUSH, "OpenView", BaseViewType.CUSTOM , cnf[id] , id )
            return true
        end      

    --世界等级任务
    elseif id == Macros.Game.SystemID.PUSH_WORLDLEVEL then
        local server_opTime = CallModuleFunc(ModuleType.ROLE,"GetOpenClothing")
        if server_opTime <= 1 then
            return false
        end
        local num , max = CallModuleFunc(ModuleType.PUSH_WORLDLEVEL , "GetTaskSchedule" )
        if num < max then
            local level = CallModuleFunc(ModuleType.PUSH_WORLDLEVEL , "GetWorldLevel" )
            if level ~= 0 then
                return true
            end
        end
    --不需任何条件91，96，107，109，102，113
    else
        local cnf = CPPGameLib.GetConfig("NewPush", nil, nil, true)
        CallModuleFunc(ModuleType.PUSH, "OpenView", BaseViewType.CUSTOM , cnf[id] , id )  
        return true

    end
end

--打开状态
function Modules.PushModule:GetOpenKey()
	return self.openKey
end

--获取推送内容(检测时间)
function Modules.PushModule:GetPushContent()
    local channel_type = CPPSdkTools.GetCPChannelType()
    local config = CPPGameLib.GetConfig("Push", channel_type , false )
    if not config then
        config = CPPGameLib.GetConfig("Push", 0 )
    end
    if not config then
        return {}
    end

    local lst = {}
    local pushLst = config.sort_list
    table.sort( pushLst, function ( a , b )
        return a.sort < b.sort
    end )

    for i , v in pairs( pushLst ) do
        if not v.time_type then
            table.insert( lst , v )
            break
        --开服时间
        elseif v.time_type == 1 then
            if self:OpenServerTime( v ) then
                table.insert( lst , v )
                break
            end
        --指定时间
        elseif v.time_type == 2 then
            if self:AssignServerTime( v ) then
                table.insert( lst , v )
                break
            end
        end
    end

    local content_list = {}
    if lst[1] then
        local sort_id
        for i, system_id in ipairs( lst[1].system_id_list ) do
            sort_id = lst[1].sort_list[i] or i
            table.insert(content_list, {system_id = system_id , sort = sort_id})
        end
    end
    return content_list
end

--是否首次
function Modules.PushModule:IsFirst( id )
    local cnf = CPPGameLib.GetConfig("FirstPush", id, false )
    --每次都弹
    if not cnf then
        return true
    else
        local t1 = {}
        local t2 = CPPGameLib.SplitEx( Cookies.Get( nil ,Macros.Setting.PushRecord) , "<GH>")
        for i , v in ipairs( t2 ) do
            t1[v] = true
        end
        --弹过了
        local n_Time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
        local n_nian, n_yue, n_ri, n_shi, n_fen, n_miao, n_xingqi = TimeTool.GetDateFromTimeStamp(n_Time)
        local t_time = string.format( "%04d" , n_nian )..string.format( "%02d" , n_yue )..string.format( "%02d" , n_ri )
        if t1[t_time..id ] then
            return nil
        else
            local first = t_time..id .. "<GH>"
            for i , v in ipairs( t2 ) do
                first = first .. v .. "<GH>"
            end
            Cookies.Set( nil , Macros.Setting.PushRecord , first )
            return true
        end
    end
end

--开服时间计算
function Modules.PushModule:OpenServerTime( data )
    local _ , server_time = CallModuleFunc(ModuleType.ROLE,"GetOpenClothing")
    local year, month, day, hour, min, sec , wday = TimeTool.GetDateFromTimeStamp( server_time )


    local star_time_H = string.sub( string.format( "%04d" , data.star_time ) , 0 , 2)
    local star_time_M = string.sub( string.format( "%04d" , data.star_time ) , -2)
    local dataOpenTime = server_time + ( ( data.star_date - 1 ) * 24 * 60 * 60 ) + ( star_time_H * 60 * 60  ) + ( star_time_M * 60 )

    local over_time_H = string.sub( string.format( "%04d" , data.over_time ) , 0 , 2)
    local over_time_M = string.sub( string.format( "%04d" , data.over_time ) , -2)
    local dataOverTime = server_time + ( ( data.over_date - 1 ) * 24 * 60 * 60 ) + ( over_time_H * 60 * 60  ) + ( over_time_M * 60 )

    local serverNowTime = year..month..string.format( "%02d" , day )..string.format( "%02d" , hour )..string.format( "%02d" , min )

    if tonumber(serverNowTime) >= tonumber(dataOpenTime) and tonumber(serverNowTime) < tonumber(dataOverTime) then
        return true
    end
end

--指定时间计算
function Modules.PushModule:AssignServerTime( data )
    local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
    local year, month, day, hour, min, sec , wday = TimeTool.GetDateFromTimeStamp( server_time )

    local dataOpenTime = data.star_date .. string.format( "%04d" , data.star_time )
    local dataOverTime = data.over_date .. string.format( "%04d" , data.over_time )
    local serverNowTime = year..month..string.format( "%02d" , day )..string.format( "%02d" , hour )..string.format( "%02d" , min )

    if serverNowTime >= dataOpenTime and serverNowTime < dataOverTime then
        return true
    end
end