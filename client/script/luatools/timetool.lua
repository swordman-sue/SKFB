
TimeTool = TimeTool or {}

--一天的秒数
TimeTool.SINGLE_DAY_SECOND = 86400

--TODO: 放到stringtable.lua
TimeTool.defaul_format = {
    [1]  = CPPGameLib.GetString("time_1") ,
    [2]  = CPPGameLib.GetString("time_2") ,
    [3]  = CPPGameLib.GetString("time_3") ,
    [4]  = CPPGameLib.GetString("time_4") ,
    [5]  = CPPGameLib.GetString("time_5") ,
    [6]  = CPPGameLib.GetString("time_6") ,
    [7]  = CPPGameLib.GetString("time_7") ,
    [8]  = CPPGameLib.GetString("time_8") ,
    [9]  = CPPGameLib.GetString("time_9") ,
    [10] = CPPGameLib.GetString("time_10") ,
    [11] = CPPGameLib.GetString("time_11") ,
    [12] = CPPGameLib.GetString("time_12") ,
    [13] = CPPGameLib.GetString("time_13") ,
    [14] = CPPGameLib.GetString("time_14") ,
    [15] = CPPGameLib.GetString("time_15") ,
    [16] = CPPGameLib.GetString("time_16") ,
    [17] = CPPGameLib.GetString("time_17") ,
    [18] = CPPGameLib.GetString("time_18") ,
    [19] = CPPGameLib.GetString("time_19") , 
    [20] = CPPGameLib.GetString("time_20") ,
    [21] = CPPGameLib.GetString("time_21") ,
    [22] = CPPGameLib.GetString("time_22") ,
    [23] = CPPGameLib.GetString("time_23") ,
    [24] = CPPGameLib.GetString("time_24") ,
}

local trans_timestamp_for_date_frm_timestamp 
local trasn_timestamp_for_timestamp_frm_date

--获取客户端时间戳
function TimeTool.GetTime()
    return os.time()
end

--[[
    时间戳转为日期
    @time：服务端时间戳
    @ignore_time_zone：是否忽略时区差值转换
    @ret_tbl：是否返回table
--]]
function TimeTool.GetDateFromTimeStamp(time, ignore_time_zone, ret_tbl)
    --根据客户端与服务端的时间差，进行转换
    time = trans_timestamp_for_date_frm_timestamp(time, ignore_time_zone)

    local date_info = os.date("*t", time)
    date_info.wday = date_info.wday - 1
    date_info.month = string.format("%02d", date_info.month)
    date_info.day = string.format("%02d", date_info.day)
    date_info.hour = string.format("%02d", date_info.hour)
    date_info.min = string.format("%02d", date_info.min)
    date_info.sec = string.format("%02d", date_info.sec)
   
    if ret_tbl then
        return date_info
    else
        return date_info.year, date_info.month, date_info.day, date_info.hour, date_info.min, date_info.sec, date_info.wday
    end
end

--[[
    日期转为时间戳
    @year、month、day、h、m、s：服务端日期
    @ignore_time_zone：是否忽略时区差值转换
--]]
function TimeTool.GetTimeStampFromDate(_year, _month, _day, h, m, s, ignore_time_zone)
    _year = tonumber(_year)
    _month = tonumber(_month) or 1
    _day = tonumber(_day) or 1
    h = tonumber(h) or 0
    m = tonumber(m) or 0
    s = tonumber(s) or 0

    local tbl = {year = _year, month = _month, day = _day, hour = h, min = m, sec = s}
    local time = os.time(tbl) or 0

    --根据客户端与服务端的时间差，进行转换
    time = trasn_timestamp_for_timestamp_frm_date(time, ignore_time_zone)

    return time
end

--[[
    获取时间格式
    @time：服务端时间戳
    @time_format：详见TimeTool.defaul_format
    @ignore_time_zone：是否忽略时区差值转换
 --]]
function TimeTool.GetTimeFormat(time, time_format, ignore_time_zone)
    local my_format = time_format or TimeTool.defaul_format[1]
    if type(time_format) == "number"  then
        my_format = TimeTool.defaul_format[time_format]
    end

    --根据客户端与服务端的时间差，进行转换
    time = trans_timestamp_for_date_frm_timestamp(time, ignore_time_zone)

    return os.date(my_format, time)
end

--[[
    将秒转化为特定的格式
    format: { "HH:MM:SS", "MM:SS", "DD天HH时MM分" }
    t：剩余时间
]]
function TimeTool.TransTimeStamp(format, t)
    -- day
    local format, count = string.gsub(format, "DD", "%%d")
    if count == 1 then
        format = string.format(format, math.floor(t / 86400))
        t = t - math.floor(t / 86400) * 86400 -- 先减去天数
    end
    -- hour
    format, count = string.gsub(format, "HH", "%%02d")
    if count == 1 then
        format = string.format(format, math.floor(t / 3600))
        t = t - math.floor(t / 3600) * 3600 -- 再减去小时数
    end
    format, count = string.gsub(format, "H", "%%2d")
    if count == 1 then
        format = string.format(format, math.floor(t / 3600))
        t = t - math.floor(t / 3600) * 3600 -- 再减去小时数
    end
    -- minute
    format, count = string.gsub(format, "MM", "%%02d")
    if count == 1 then
        format = string.format(format, math.floor(t / 60))
        t = t - math.floor(t / 60) * 60 -- 再减少分钟数
    end
    format, count = string.gsub(format, "M", "%%2d")
    if count == 1 then
        format = string.format(format, math.floor(t / 60))
        t = t - math.floor(t / 60) * 60 -- 再减少分钟数
    end
    -- second
    format, count = string.gsub(format, "SS", "%%02d")
    if count == 1 then
        format = string.format(format, t)
    end
    format, count = string.gsub(format, "S", "%%2d")
    if count == 1 then
        format = string.format(format, t)
    end

    return format
end

--离线时间转换
function TimeTool.OfflineTime(nowTime)
    --服务器时间
    local serverTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
    --过去了多久
    local time = serverTime - nowTime

    local num = 0
    if nowTime <= 0 then
        text = CPPGameLib.GetString("union_online")
    --秒
    elseif time > 0 and time < 60 then
        text = CPPGameLib.GetString("time_second", time )
    --分钟
    elseif time >= 60 and time < 3600 then
        text = CPPGameLib.GetString("time_Minute", math.floor(time/60) )
    --小时
    elseif time >= 3600 and time < 86400 then
        text = CPPGameLib.GetString("time_hour", math.floor(time/3600) )
        num = math.floor(time/3600)
    --天
    elseif time >= 86400 then
        text = CPPGameLib.GetString("time_sky", math.floor(time/86400) )
        num = math.floor(time/3600)
    end

    return text , num
end

--时间转换
function TimeTool.OfflineTime_1(time)
    local text = ""
    if time >= ( 24 * 60 * 60  ) then
        local sky = math.floor( time/( 24 * 60 * 60 ) )
        local hour = ( time - ( sky * ( 24 * 60 * 60 ) ) ) / 60 / 60 
        text = CPPGameLib.GetString("time_reciprocal_day", sky , math.floor( hour) ) 

    elseif time >= ( 60 * 60 ) then
        local hour = math.floor( time/( 60 * 60 ) )
        local minute = ( time - ( hour * ( 60 * 60 ) ) ) / 60 
        text = CPPGameLib.GetString("time_reciprocal_hour", hour , math.floor( minute ) ) 

    elseif time >= ( 60 ) then
        local minute = math.floor( time/( 60 ) )
        local second = ( time - ( minute * ( 60 ) ) ) 
        text = CPPGameLib.GetString("time_reciprocal_minute", minute , math.floor( second ) ) 
    end
    return text
end

--以起始时间戳为基准，计算第X天Y小时后的时间戳(起始时间戳算第一天)
--服务端时间戳
function TimeTool.CalcTimeStampFrmBasicTimeStamp(time_stamp, tday, thour)
    local _, _, _, hour, min, sec = TimeTool.GetDateFromTimeStamp(time_stamp)
    local day_pass_time = hour * 60 * 60 + min * 60 + sec
    
    time_stamp = time_stamp - day_pass_time + (tday - 1) * 86400 + thour * 60 * 60
    return time_stamp
end

--获取日期对应的时间戳(日期格式：201710311800)
function TimeTool.CalcTimeStampFrmDate(date)
    if not date or string.len(date) < 12 then
        date = date or "null"
        LogTool.LogError("[TimeTool.CalcTimeStampFrmDate] 无效的日期格式 %s", date)
    end

    local year = string.sub(date, 1, 4)
    local month = string.sub(date, 5, 6)
    local day = string.sub(date, 7, 8)
    local hour = string.sub(date, 9, 10)
    local minute = string.sub(date, 11, 12)
    return TimeTool.GetTimeStampFromDate(year, month, day, hour, minute, 0)
end

--获取HHMMSS对应的秒数
function TimeTool.CalcSecondFrmHHMMSS(var)
    local start_hour = math.floor(var / 100)
    local start_minute = var % 100
    return start_hour * 60 * 60 + start_minute * 60   
end

--------------------------------------------------------------------------------------------------------------------------------
--私有函数
--------------------------------------------------------------------------------------------------------------------------------
--根据客户端与服务端的时间差，进行转换(同样的时间戳，快时区的日期比慢时区的日期要大)
trans_timestamp_for_date_frm_timestamp = function(time, ignore_time_zone)
    if not time then
        return
    end

    if TimeTool.delta_between_local_and_server and math.abs(TimeTool.delta_between_local_and_server) > 1 and not ignore_time_zone then
        --执行转换(为了显示服务端日期)
        time = time - TimeTool.delta_between_local_and_server
    end
    return math.max(0, time)
end

--根据客户端与服务端的时间差，进行转换(同样的日期，快时区的时间戳比慢时区的时间戳小)
trasn_timestamp_for_timestamp_frm_date = function(time, ignore_time_zone)
    if TimeTool.delta_between_local_and_server and math.abs(TimeTool.delta_between_local_and_server) > 1 and not ignore_time_zone then
        --进行转换(为了返回服务端时间戳)
        time = time + TimeTool.delta_between_local_and_server
    end
    return math.max(0, time)
end