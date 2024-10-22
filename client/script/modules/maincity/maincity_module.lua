
--[[
%% @module: 主界面模块
%% @author: swordman sue
%% @created: 2018-11-6
--]]

Modules = Modules or {}

Modules.MaincityModule = Modules.MaincityModule or BaseClass(BaseModule)

--系统入口图标列表
local sys_icon_list

function Modules.MaincityModule:__init()
    --创建入口控件管理器
    Modules.MainCityEntryMgr.CreateInstance()       
end

function Modules.MaincityModule:__delete()
    if self.__city_show_timer then
        GlobalTimerQuest:CancelQuest( self.__city_show_timer )
        self.__city_show_timer = nil
    end

    --销毁入口控件管理器
    Modules.MainCityEntryMgr.DelInstance()    
end

--------------------------------------------------------------------------------------------------------------------------------
--事件相关
--------------------------------------------------------------------------------------------------------------------------------
--处理仅有主界面显示的事件
function Modules.MaincityModule:HandleOnlyCityViewOpen()
    local view = self:GetView()
    if view and view:IsLayout() then
        if not self.__is_only_maincity_view_open then
            view:HandleOnlyCityViewOpen()
            self:FireNextFrame(Macros.Event.View.MAINCITY_VISIBLE)
            self.__is_only_maincity_view_open = true
        end
    end
end

--处理不仅有主界面显示的事件
function Modules.MaincityModule:HandleNotOnlyCityViewOpen()
    local view = self:GetView()
    if view and view:IsLayout() then
        if self.__is_only_maincity_view_open then
            view:HandleNotOnlyCityViewOpen()
            self:FireNextFrame(Macros.Event.View.MAINCITY_HIDE)
            self.__is_only_maincity_view_open = false
        end
    end        
end

--角色升级更新
function Modules.MaincityModule:HandleRoleLevelChange(old_var, new_var)
    local view = self:GetView()
    if view and view:IsLayout() then
        view:HandleRoleLevelChange(old_var, new_var)
    end     
end

--阵位信息更新
function Modules.MaincityModule:HandleHeroLineupUpdate()
    local view = self:GetView()
    if view and view:IsLayout() then
        view:HandleHeroLineupUpdate()
    end
end

--角色属性改变
function Modules.MaincityModule:HandleRolePropChange(prop_type, old_var, new_var)
    local view = self:GetView()
    if view and view:IsLayout() then
        view:HandleRolePropChange(prop_type, old_var, new_var)
    end     
end

--角色名改变
function Modules.MaincityModule:HandleRoleNameChanged()
    local view = self:GetView()
    if view and view:IsLayout() then
        view:HandleRoleNameChanged()
    end         
end

--处理限时活动刷新时间
function Modules.MaincityModule:HandleLimitActivityRefreshTime(system_id, show_txt, is_open)
    if is_open then
        --活动开启，刷新区域入口
        self:ResetRegionEntrysBySystemID(system_id)
    end
end

--处理限时活动结束
function Modules.MaincityModule:HandleLimitActivityEnd(system_id)
    --活动结束，刷新区域入口
    self:ResetRegionEntrysBySystemID(system_id)
end

--处理界面关闭
function Modules.MaincityModule:HandleViewCloseEvent(view_id, view_type)
    --刷新遗迹奖励入口
    if view_type == BaseViewType.MANOR then
        self:ResetRegionEntrysBySystemID(Macros.Game.SystemID.MANOR)
    end
end

--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------
--获取某区域底部入口数量上限
function Modules.MaincityModule:GetBotttomEntryLimitNumByRegion(region_type)
    region_type = region_type or 0

    local max_botttom_iconnum = 10
    local ret_entry_list = self:__GetModuleEntrysByRegion(Modules.MaincityModule.RegionType.BOTTOM_RIGHT)
    if region_type == Modules.MaincityModule.RegionType.BOTTOM_LEFT then
        return max_botttom_iconnum - #ret_entry_list

    elseif region_type == Modules.MaincityModule.RegionType.BOTTOM_RIGHT then
        return #ret_entry_list
    end    
end


--根据扩展id获取模块入口列表
function Modules.MaincityModule:GetModuleEntrysByExtendId(extend_id, iter_func)
    local ret_entry_list = {}

    local is_open, is_pre_open
    for entry_id, entry_info in pairs(CPPGameLib.GetConfig("CityIcon",nil,nil,true)) do       
        if entry_info.extend_systemid and entry_info.extend_systemid == extend_id then
            entry_info.id = entry_id

            --系统开启检测
            is_open, is_pre_open = GlobalModulesMgr:IsOpen(entry_info.system_id)   
            if is_open or is_pre_open then
                local is_activity_open = CallModuleFunc(ModuleType.ACTIVITY, "CheckActivityOpened", entry_info.system_id)
                if is_activity_open then
                    table.insert(ret_entry_list, entry_info)

                    --处理迭代逻辑
                    if iter_func and iter_func(entry_info) then
                        return ret_entry_list
                    end
                end
            end
        end
    end
    
    if #ret_entry_list > 0 then
        table.sort(ret_entry_list, SortTools.KeyLowerSorter("sort_id"))
    end

    return ret_entry_list
end

--根据区域类型获取模块入口列表
function Modules.MaincityModule:GetModuleEntrysByRegion(region_type)
    local ret_entry_list = {}

    local is_open, is_pre_open
    local entry_list = self:__GetModuleEntrysByRegion(region_type)
    for _, entry_info in ipairs(entry_list) do
        --系统开启检测
        is_open, is_pre_open = GlobalModulesMgr:IsOpen(entry_info.system_id)   
        if is_open or (is_pre_open and entry_info.region_type ~= Modules.MaincityModule.RegionType.CENTER) then
            local is_activity_open = CallModuleFunc(ModuleType.ACTIVITY, "CheckActivityOpened", entry_info.system_id)
            if is_activity_open then
                table.insert(ret_entry_list, entry_info)
            end
        end
    end

    --区域一级入口上限处理(超出则显示“更多”按钮，点击后展开二级入口弹窗)
    if region_type == Modules.MaincityModule.RegionType.BOTTOM_LEFT then
        local limit_num = self:GetBotttomEntryLimitNumByRegion(region_type)
        if limit_num and #ret_entry_list > limit_num then
            local entry_info = 
            {
                id = 20000 + region_type,
                is_more = true,
                region_type = region_type,
                res_name = [[btn_more]],
                module_txt = CPPGameLib.GetString("maincity_more_word"),
            }
            table.insert(ret_entry_list, limit_num + 1, entry_info)
        end

    elseif region_type == Modules.MaincityModule.RegionType.CENTER then
        if #ret_entry_list <= 2 then
            return ret_entry_list
        end
        
        --中心排序
        local new_ret_entry_list = {}
        --中心位置
        local key_pos = math.floor((#ret_entry_list - 1)/2) + 1
        for i,entry_info in ipairs(ret_entry_list) do
            local new_entry_info = CPPGameLib.CopyTbl(entry_info)
            local new_pos = key_pos + (i%2 == 0 and -1 or 1)* math.ceil((i-1)/2) 
            if not new_ret_entry_list[new_pos] and new_pos > 0 then
                new_ret_entry_list[new_pos] = new_entry_info
            else
                for count=1,#ret_entry_list do
                    new_pos = new_pos + 1
                    if not new_ret_entry_list[new_pos] and new_pos > 0 then
                        new_ret_entry_list[new_pos] = new_entry_info
                        break
                    end
                end
            end
        end

        return new_ret_entry_list
    end
    
    return ret_entry_list
end

--处理系统入口点击逻辑
function Modules.MaincityModule:HandleEntryTouch(entry_info, entry_widget)
    if not entry_info or not entry_widget then
        return
    end

    if entry_info.is_more then                  
        Modules.MaincityView.CloseExtendModulesEntrys()

        --打开二级入口弹窗
        Modules.MaincityView.OpenSecondModulesEntrysByRegion(entry_info.region_type, entry_widget)

    elseif entry_info.is_extend then
        Modules.MaincityView.CloseSecondModulesEntrys()
        Modules.MaincityView.CloseExtendModulesEntrys(entry_info.is_extend)

        --打开扩展弹窗
        Modules.MaincityView.OpenExtendModulesEntrysByRegion(entry_info.region_type, entry_widget, entry_info.is_extend)
    
    else        
        Modules.MaincityView.CloseSecondModulesEntrys(0)
        Modules.MaincityView.CloseExtendModulesEntrys(nil, 0)

        --系统模块跳转
        GlobalModulesMgr:OpenSystemModule(entry_info.system_id)
    end
end

--重置对应区域的系统入口
function Modules.MaincityModule:ResetRegionEntrysBySystemID(system_id)
    if not system_id then
        return
    end

    self:FireNextFrame(Macros.Event.MaincityModule.RESET_REGION_ENTRYS, system_id)
end

--系统入口Id转换为主界面入口Id
function Modules.MaincityModule:TranslateSystemIDToEntryID(system_id)
    if not self.__translate_table then
        self.__translate_table = {}
        for entry_id,entry_info in pairs(CPPGameLib.GetConfig("CityIcon",nil,nil,true)) do
            if entry_info.system_id then
                self.__translate_table[entry_info.system_id] = entry_id
            end
        end
    end
    return self.__translate_table[system_id]
end

--获取主城推送使用物品表
function Modules.MaincityModule:GetPopNewItemList()
    return self.__pop_new_item_list or {}
end

--获取更多入口的红点数量
function Modules.MaincityModule:GetEneryMoreRedDotNum()
    return Modules.MaincityView.GetEneryMoreRedDotNum()
end

--获取扩展入口的红点数量
function Modules.MaincityModule:GetEneryExtendRedDotNum(extend_id)
    return Modules.MaincityView.GetEneryExtendRedDotNum(extend_id)
end

--获取facebook分享数据
function Modules.MaincityModule:GetFacebookShareData()
    return self.__record_facebook_share_data
end

--获取五星好评数据
function Modules.MaincityModule:GetFiveStarHighPraiseData()
    return self.__record_fivestar_highpraise_data
end

--插入主界面弹出使用物品数据
function Modules.MaincityModule:InsertPopUseData(insert_data)
    if not insert_data then return end
    local new_insert_data = CPPGameLib.CopyTblShallow(insert_data)
    self.__pop_new_item_list = self.__pop_new_item_list or {}
    local is_same
    for i,old_insert_data in ipairs(self.__pop_new_item_list) do
        if old_insert_data.item_id == new_insert_data.item_id then
            is_same = true
            old_insert_data.item_num = old_insert_data.item_num + new_insert_data.item_num
            break
        end
    end
    --是否是相同的数据
    if not is_same then
        table.insert(self.__pop_new_item_list,new_insert_data)
    end
end

--更新主界面弹出使用物品数据
function Modules.MaincityModule:UpdatePopUseData()
    self.__pop_new_item_list = self.__pop_new_item_list or {}
    local remove_list
    for i,pop_data in ipairs(self.__pop_new_item_list) do
        local item_num = ObjIDTool.GetOwnNum(pop_data.item_id)
        pop_data.item_num = math.min(pop_data.item_num, item_num)
        if item_num <= 0 then
            remove_list = remove_list or {}
            table.insert(remove_list,i)
        end
    end
    if remove_list then
        table.sort(remove_list,function (a,b)
            return a > b
        end)
        for _,remove_index in ipairs(remove_list) do
            table.remove(self.__pop_new_item_list,remove_index)
        end
    end
end

--移除主界面弹出使用物品数据
function Modules.MaincityModule:RemovePopUseData()
    if self.__pop_new_item_list then
        table.remove(self.__pop_new_item_list,1)
    end
end

--获取入口图标
function Modules.MaincityModule:GetSystemEntryIconPath(res_name)
    return Resource.PathTool.GetSystemEntryIconPath(res_name)
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--根据区域类型获取模块入口列表(不含扩展区域中的入口)
function Modules.MaincityModule:__GetModuleEntrysByRegion(region_type)
    if not self.__module_entrys_map then
        self.__module_entrys_map = {}
        for id, entry_info in pairs(CPPGameLib.GetConfig("CityIcon",nil,nil,true)) do
            if not entry_info.extend_systemid then
                entry_info.id = id
                self.__module_entrys_map[entry_info.region_type] = self.__module_entrys_map[entry_info.region_type] or {}
                table.insert(self.__module_entrys_map[entry_info.region_type], entry_info)
            end
        end

        --排序
        for _, region_type in pairs(Modules.MaincityModule.RegionType) do
            if self.__module_entrys_map[region_type] then
                table.sort(self.__module_entrys_map[region_type], SortTools.KeyLowerSorter("sort_id"))
            end
        end
    end

    return self.__module_entrys_map[region_type]
end

--设置主城
function Modules.MaincityModule:GetMaincityEffectData()

    local cnf = CPPGameLib.GetConfig("ConfigSetCity", nil , nil , true )

    --时间存储
    local t1 = {}
    for i , v in ipairs( cnf ) do
        --开始时间
        local op_nian = string.sub( v.star_date, 0 , 4 )
        local op_yue  = string.sub( v.star_date, 5 , 6 )
        local op_ri   = string.sub( v.star_date, 7 , 8 )
        local op_shi  = string.sub( v.star_time, 0 , 2 )
        local op_fen  = string.sub( v.star_time, 3 , 4 )

        local openTime = TimeTool.GetTimeStampFromDate( op_nian, op_yue, op_ri, op_shi, op_fen )

        --结束时间
        local en_nian = string.sub( v.over_date, 0 , 4 )
        local en_yue  = string.sub( v.over_date, 5 , 6 )
        local en_ri   = string.sub( v.over_date, 7 , 8 )
        local en_shi  = string.sub( v.over_time, 0 , 2 )
        local en_fen  = string.sub( v.over_time, 3 , 4 )

        local endTime = TimeTool.GetTimeStampFromDate( en_nian, en_yue, en_ri, en_shi, en_fen )

        table.insert( t1 , { openTime = openTime , endTime = endTime , data  = v } )
    end

    table.sort( t1, function ( a , b )
        return a.openTime < b.openTime
    end )

    --设置判断
    local t2 = nil
    local openTime = 0
    local endTime = 0
    local time_sync = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
    for i,v in ipairs(t1) do
        if v.openTime <= time_sync and v.endTime >= time_sync then
            openTime = v.openTime
            endTime = v.endTime
            t2 = v.data
            break
        end
    end

    if not t2 then
        for i , v in ipairs(t1) do
            if v.openTime >= time_sync then
                self:__StartMaincityEffectCountdown( v.openTime , time_sync )
                break
            end
        end

        local t3 = {}
        t3.city_bg = {}
        table.insert( t3.city_bg , { city_bg = "city/bg_maincity_beijing", zorder = 1 , pos = { [1] = 0 , [2] = 0 } } )
        table.insert( t3.city_bg , { city_bg = "city/bg_maincity_qianjing", zorder = 4 , pos = { [1] = 0 , [2] = 0 } } )

        t3.effects = {}
        table.insert( t3.effects , { effects = 100068 , zorder = 2 , pos = { [1] = 0 , [2] = 0 } } )
        table.insert( t3.effects , { effects = 100069 , zorder = 5 , pos = { [1] = 0 , [2] = 0 } } )

        t3.action = {}
        table.insert( t3.action , { action = "UI_maincity" , zorder = 3 , pos = { [1] = 0 , [2] = -320 } } )

        t3.id = 0

        return t3
    else
        local t3 = {}
        --背景
        if t2.city_bg then
            t3.city_bg = {}
            for i , v in ipairs( t2.city_bg ) do
                table.insert( t3.city_bg , { city_bg = v , zorder = t2.city_zorder[i] , pos = t2.city_pos[i] , suffix = t2.city_suffix[i] } )
            end
        end
        
        --特效
        if t2.effects_id then
            t3.effects = {}
            for i , v in ipairs( t2.effects_id ) do
                table.insert( t3.effects , { effects = v , zorder = t2.effects_zorder[i] , pos = t2.effects_pos[i] } )
            end
        end

        --动作
        if t2.action then
            t3.action = {}
            for i , v in ipairs( t2.action ) do
                table.insert( t3.action , { action = v , zorder = t2.action_zorder[i] , pos = t2.action_pos[i] } )
            end
        end
        t3.id = t2.activity_id

        self:__StartMaincityEffectCountdown( endTime , time_sync )

        return t3
    end
end

--启动主界面效果倒计时
function Modules.MaincityModule:__StartMaincityEffectCountdown( openTime , time_sync )

    if self.__city_show_timer then
        GlobalTimerQuest:CancelQuest( self.__city_show_timer )
        self.__city_show_timer = nil
    end

    local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
    if openTime - server_time <= 0 then
        return 
    end

    --定时器
    self.__city_show_timer = GlobalTimerQuest:AddPeriodQuest(
    GlobalCallbackMgr:AddDelayQuest(function()
        if self.__city_show_timer then
            GlobalTimerQuest:CancelQuest( self.__city_show_timer )
            self.__city_show_timer = nil
        end
        self:FireNextFrame(Macros.Event.View.UPDATE_MAINCITY_EFFECT)
    end) , openTime - server_time )
end
