
--[[
%% @module: 限时副本汇总(组队、世界BOSS、PVP、怪物攻城....)
--]]

Modules = Modules or {}

Modules.LimitDungeonModule = Modules.LimitDungeonModule or BaseClass(BaseModule)

function Modules.LimitDungeonModule:__init()
end

function Modules.LimitDungeonModule:__delete()
end

--------------------------------------------------------------------------------------------------------------------------------
--事件
--------------------------------------------------------------------------------------------------------------------------------
function Modules.LimitDungeonModule:HandleLoginAccountSuccess()
    --活动预告公告
    self:AddPeriodQuest(function()
        self:__CheckLimitDungeonAdvanceNotice()
    end, 1, -1)
end

function Modules.LimitDungeonModule:HandleRoleLevelChange()
    self.__today_open_system_info_list = nil
    self:__CheckLimitDungeonAdvanceNotice()
end

function Modules.LimitDungeonModule:HandleMaincityVisibleEvent()
    if CallModuleFunc(ModuleType.TEAM, "IsEnterTeamHallFrmPromptView") then
        CallModuleFunc(ModuleType.TEAM, "SetEnterTeamHallFrmPromptView", false)
        self:OpenView(BaseViewType.LIMIT_DUNGEON)
    end
end

--------------------------------------------------------------------------------------------------------------------------------
--公共函数
--------------------------------------------------------------------------------------------------------------------------------
function Modules.LimitDungeonModule:GetLimitDungeonList()
	local lst = {}
	local lst_ = {}
    local role_level = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.LEVEL)

	for k , v in pairs( CPPGameLib.CopyTbl( CPPGameLib.GetConfig("Deseno", nil,nil,true ) ) ) do
        local module_info = CPPGameLib.GetConfig("SystemEntry" , k )
		v.id = k
        v.level = module_info.level
        if module_info.prepare_level <= role_level then
            local _ , type_ = self:GetLimitDungeonOpenDesc( v.id )
            if type_ == true then
    			table.insert(lst , v )
            else
            	table.insert(lst_ , v )
            end
        end
	end

    table.sort( lst, function ( a , b )
        if a.level < b.level then
            return a.level < b.level
        elseif a.level == b.level then
            return a.id < b.id
        end
    end )

    table.sort( lst_, function ( a , b )
        if a.level < b.level then
            return a.level < b.level
        elseif a.level == b.level then
            return a.id < b.id
        end
    end )

    for i , v in ipairs(lst_) do
    	table.insert( lst , v )
    end

	return lst
end

--获取开启时间描述
function Modules.LimitDungeonModule:GetLimitDungeonOpenDesc(id)
    local cnf = CPPGameLib.GetConfig("Deseno", id)

    --开始时间
    local open_time = {}
    for i,v in ipairs(cnf.open_time) do
       	local time = string.format( "%04d" , tonumber(v) )
    	local sHour = string.sub(time , 0 , 2)
    	local sMinute = string.sub(time , -2)
        open_time[i] = sHour ..":" .. sMinute
    end

	--结束时间
    local end_time = {}
    for i,v in ipairs(cnf.end_time) do
       	local time = string.format( "%04d" , tonumber(v) )
    	local eHour = string.sub(time , 0 , 2)
    	local eMinute = string.sub(time , -2)
        end_time[i] = eHour ..":" .. eMinute
    end

	--服务器
	local serverTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	--当前服务器时间
	local year, month, day, hour, minute, second, week = TimeTool.GetDateFromTimeStamp(serverTime)
	local open = false

    for i,day in ipairs(cnf.week_day_list) do
        for index , v in ipairs(open_time) do
            if day == tonumber(week) then
    			if ( tonumber( hour..minute ) >= tonumber( cnf.open_time[index] ) ) and 
    				( tonumber( hour..minute ) <= tonumber( cnf.end_time[index]) ) then
    				open = true
                    break
    			end
            end
        end
    end

    local lab = ""
    for index ,v in ipairs(open_time) do
        local h = ""
        h = open_time[index].." ~ "..end_time[index]
        lab = lab..h ..( index == #open_time and " " or "  ".. CPPGameLib.GetString("common_Comma") .."  "  )
    end
    return lab , open
end

--检测限时副本是否开启
function Modules.LimitDungeonModule:IsLimitDungeonOpen(system_id, is_actual_opened)
    if not self.__today_open_system_info_list then
        self:__CheckLimitDungeonAdvanceNotice()
    end

    if self.__today_open_system_info_list and self.__today_open_system_info_list[system_id] then
        if self.__today_open_system_info_list[system_id].is_opened and not self.__today_open_system_info_list[system_id].is_over then
            if is_actual_opened then
                return self.__today_open_system_info_list[system_id].is_onfighting, self.__today_open_system_info_list[system_id].is_over
            else
                return true, self.__today_open_system_info_list[system_id].is_over
            end         
        else
            return false, self.__today_open_system_info_list[system_id].is_over
        end
    else
        return false
    end
end

--获取限时副本的信息
function Modules.LimitDungeonModule:GetLimitDungeonInfo(system_id)
    if not system_id then
        return
    end

    if not self.__today_open_system_info_list then
        self:__CheckLimitDungeonAdvanceNotice()
    end

    return self.__today_open_system_info_list and self.__today_open_system_info_list[system_id]
end

--获取限时副本的描述信息
function Modules.LimitDungeonModule:GetLimitDungeonDesc(system_id)
    if not self.__today_open_system_info_list then
        self:__CheckLimitDungeonAdvanceNotice()
    end

    if self.__today_open_system_info_list and self.__today_open_system_info_list[system_id] then
        if self.__today_open_system_info_list[system_id].is_onfighting then
            return CPPGameLib.GetString("maincity_on_fighting"),false
        else
            local cur_server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
            local show_txt = TimeTool.TransTimeStamp("MM:SS",cur_server_time - self.__today_open_system_info_list[system_id].time_list[1])
            return show_txt,true
        end     
    end

    return "", false
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--检测限时副本预告
function Modules.LimitDungeonModule:__CheckLimitDungeonAdvanceNotice()
    if CPPSdkTools.IsInExamine() then
        --审核中，屏蔽预告
        return
    end 

    local date_info = TimeTool.GetDateFromTimeStamp(CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime"), nil, true)
    local cur_week = date_info.wday

    --判断周几，存储开放的系统
    if not self.__today_open_system_info_list or (self.__mark_cur_week and self.__mark_cur_week ~= cur_week) then
        self.__today_open_system_info_list = {}
        self.__mark_cur_week = cur_week
        --判断周几时间是否开放
        for system_id,system_info in pairs(CPPGameLib.GetConfig("ActivityAdvanceNotice", nil, nil , true )) do          
            local is_opened = false
            for _, week in ipairs(system_info.week_day_list) do
                if week == cur_week then
                    is_opened = true
                    break
                end
            end
            if is_opened then
                local data = CPPGameLib.CopyTbl(system_info)
                self.__today_open_system_info_list[system_id] = data
            end         
        end
    end
    
    local role_level = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.LEVEL)

    --判断是否需要重新刷新
    local is_need_refrash = false
    --如果活动还没结束就判断
    for system_id,system_info in pairs(self.__today_open_system_info_list) do
        system_info.mark_index = system_info.mark_index or 0
       
        local is_final_over
        if not system_info.is_over then
            is_final_over = false
        else
            is_final_over = system_info.mark_index >= #system_info.start_time
            if not is_final_over then
                system_info.time_list = nil
                system_info.is_over = false
                system_info.is_onfighting = false
                system_info.is_opened = false
            end
        end

        if role_level >= system_info.need_role_level and not is_final_over then
            local cur_server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
            if not system_info.time_list then
                system_info.mark_index = system_info.mark_index + 1
                system_info.time_list = {}
                local time_key_word = {"start_begin_notice_time","start_end_notice_time","start_time","over_time","over_begin_notice_time","over_end_notice_time"}              
                local year,month,day = TimeTool.GetDateFromTimeStamp(CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime"))
                for i,key_word in ipairs(time_key_word) do
                    if system_info[key_word] and system_info[key_word][system_info.mark_index] then     
                        local hour = tonumber(string.sub(system_info[key_word][system_info.mark_index],1,string.len(system_info[key_word][system_info.mark_index]) == 4 and 2 or 1))                    
                        local min = tonumber(string.sub(system_info[key_word][system_info.mark_index],string.len(system_info[key_word][system_info.mark_index]) == 4 and 3 or 2,string.len(system_info[key_word][system_info.mark_index])))
                        system_info.time_list[i] = TimeTool.GetTimeStampFromDate(year, month, day, hour, min, 0)
                    end
                end 
            end         

            --活动预告时间
            if system_info.time_list and system_info.time_list[1] then
                system_info.is_opened = cur_server_time >= system_info.time_list[1]
            else
                system_info.is_opened = false
            end

            local show_txt = ""
            if system_info.time_list and system_info.time_list[1] then
                if cur_server_time >= system_info.time_list[1] then
                    if cur_server_time < system_info.time_list[4] and not system_info.is_onfighting then
                        local is_open = false

                        --活动开启预告中
                        if cur_server_time >= system_info.time_list[1] and cur_server_time <= system_info.time_list[2] then
                            show_txt = TimeTool.TransTimeStamp("MM:SS",system_info.time_list[3] - cur_server_time)

                            --发送本地系统公告
                            if (cur_server_time - system_info.time_list[1])%system_info.interval_time == 0 and system_info.is_declare == Macros.Global.TRUE then
                                local advance_str = string.format(system_info.advance_notice or "%s", TimeTool.GetTimeFormat(system_info.time_list[3], "%H:%M", true))
                                CallModuleFunc(ModuleType.NOTICE, "SystemNoticeResp", {text = advance_str, notice_id = 0})
                            end

                        --活动开启预告结束，但未开启
                        elseif cur_server_time < system_info.time_list[3] then
                            show_txt = TimeTool.TransTimeStamp("MM:SS",system_info.time_list[3] - cur_server_time)

                        --活动进行中
                        elseif cur_server_time >= system_info.time_list[3] then
                            is_open = true
                            system_info.is_onfighting = true
                            show_txt = CPPGameLib.GetString("maincity_on_fighting")

                            --发送本地系统公告
                            if system_info.is_declare == Macros.Global.TRUE then
                                CallModuleFunc(ModuleType.NOTICE, "SystemNoticeResp", {text = system_info.start_notice or "",notice_id = 0})
                            end
                        end

                        GlobalEventSystem:FireNextFrame(Macros.Event.LimitActivityModule.REFRASH_ACTIVITY_TIME_RESP, system_id, show_txt, is_open)                      
                    
                    --活动结束预告
                    elseif cur_server_time < system_info.time_list[4] and system_info.is_onfighting and system_info.over_advance_notice then
                        if cur_server_time >= system_info.time_list[5] and cur_server_time <= system_info.time_list[6] then
                            if (cur_server_time - system_info.time_list[5])%system_info.interval_time == 0 and system_info.is_declare == Macros.Global.TRUE then

                                --发送本地系统公告
                                local advance_str = string.format(system_info.over_advance_notice or "%s", TimeTool.GetTimeFormat(system_info.time_list[4], "%H:%M", true))
                                CallModuleFunc(ModuleType.NOTICE, "SystemNoticeResp", {text = advance_str, notice_id = 0})
                            end
                        end
                    
                    --活动结束
                    elseif cur_server_time >= system_info.time_list[4] then
                        system_info.is_over = true

                        --判断是否需要刷新
                        is_need_refrash = is_need_refrash and is_need_refrash or system_info.mark_index < #system_info.start_time

                        --发送本地系统公告
                        if system_info.over_notice and system_info.is_declare == Macros.Global.TRUE then
                            CallModuleFunc(ModuleType.NOTICE, "SystemNoticeResp", {text = system_info.over_notice or "", notice_id = 0})
                        end

                        GlobalEventSystem:FireNextFrame(Macros.Event.LimitActivityModule.REFRASH_ACTIVITY_END, system_id)                      
                    end
                end
            end
        end
    end

    --需要刷新则重新进入
    if is_need_refrash then
        self:__CheckLimitDungeonAdvanceNotice()
    end
end