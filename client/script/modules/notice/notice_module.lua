--[[
%% @module: 公告
%% @author: yjg
%% @created: 2017年5月11日15:29:21
--]]


Modules = Modules or {}

Modules.NoticeModule = Modules.NoticeModule or BaseClass(BaseModule)

function Modules.NoticeModule:__init()
    --系统公告数据列表
    self.__sys_notice_data_list = {}
end

function Modules.NoticeModule:__delete()
    if self.__templ_sys_notice then
        self.__templ_sys_notice:DeleteMe()
        self.__templ_sys_notice = nil
    end
end

--------------------------------------------------------------------------------------------------------------------------------
--事件
--------------------------------------------------------------------------------------------------------------------------------
function Modules.NoticeModule:HandleLoginAccountSuccess()
    --游戏帮助公告
    self:AddPeriodQuest(function()
        self:__CheckGameHelpNotice()
    end, ConfigAdapter.Common.GetGameHelpNoticeCD(), -1)
end

function Modules.NoticeModule:HandleHeroRecruitNotice(data)
    local cnf = CPPGameLib.GetConfig("SystemNotice" , 2 )
    local user = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo")
    local text = string.format( cnf.text , user.role_name , data ) 
    local tab = { text = cnf.text , tab = { 
                                                [1] = { zi = user.role_name },
                                                [2] = { zi = string.format( Macros.Quality_RT[ data[2] ] , data[1] ) }
                                            }}      
    if #self.__sys_notice_data_list >=1 then
        table.insert( self.__sys_notice_data_list , 2 , tab )
    else
        table.insert( self.__sys_notice_data_list , tab )
    end
    self:__CheckShowSystemNotice()
end

--------------------------------------------------------------------------------------------------------------------------------
--协议
--------------------------------------------------------------------------------------------------------------------------------
--消息说明: 请求活动公告 消息Id: 
function Modules.NoticeModule:ActivityNoticeReq( info )
    local url = Macros.Global.NOTICEURL
    if not url then
        return
    end

    local key = "bab3817fbca54db2"
    local post_data = {}
    post_data.userId = 0
    post_data.sid = 0
    post_data.gameId = "4"
    post_data.sign = CPPGameLib.Md5( post_data.sid .. post_data.userId .. key )

    local urlmsg_data = {}
    urlmsg_data.url = url
    urlmsg_data.post_param = post_data
    urlmsg_data.handle_func = function (_,resp_post_data)
        self:ActivityNoticeResp( resp_post_data )
    end
    urlmsg_data.forbid_retry = true
    SendUrlMsg(urlmsg_data)
end

--消息说明: 请求活动公告响应 消息Id: 20017
function Modules.NoticeModule:ActivityNoticeResp( info )
    if #info.notice_list <= 0 then
        return 
    end 

    self.__activity_notice_data_list = info.notice_list
    self.__activity_notice_icon_list = {}
    local t1 = {}
    for i , v in ipairs( self.__activity_notice_data_list ) do
        if not t1[v.icon] then
            t1[v.icon] = true
            table.insert( self.__activity_notice_icon_list , v.icon )
        end
    end

    --活动公告
    if ConfigAdapter.Common.GetTemplNoticeShowType() == 1 then
        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACTIVITY_NOTICE)
    end
end 

--系统通知
function Modules.NoticeModule:SystemNoticeResp( info )
    if not GlobalGame:CanEnterSceneChangeState() then
        return
    end

    local qualityType = {
                [Macros.Game.NOTICE_PARAM_TYPE.PACK] = "quality" ,
                [Macros.Game.NOTICE_PARAM_TYPE.EQUIP] = "quality",
                [Macros.Game.NOTICE_PARAM_TYPE.HERO]  = "init_quality",
    }

    local text = ""
    if info.notice_id == 0 then
        text = info.text
        local time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
        table.insert( self.__sys_notice_data_list , {text = text , tab = {} , time = time , sort = 9999 } )
    else
        local user = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo")
        if info.notice_id == 2 and info.param_list[1].param == user.role_name then
            return
        end

        local cnf = CPPGameLib.GetConfig("SystemNotice" , info.notice_id )
        if not cnf then return end
        
        --次数
        for i = 1 , cnf.times do
            local text = cnf.text

            local tab = {}
            if cnf.param_type_list then
                if info.param_list[i] then

                    for i,v in ipairs(cnf.param_type_list) do
                        --如果是数量或字符串
                        if v == Macros.Game.NOTICE_PARAM_TYPE.NUM or 
                           v == Macros.Game.NOTICE_PARAM_TYPE.STRING then

                                table.insert( tab , {zi = info.param_list[i].param } )

                        --如果是道具或装备或圣器
                        elseif v == Macros.Game.NOTICE_PARAM_TYPE.PACK or 
                               v == Macros.Game.NOTICE_PARAM_TYPE.EQUIP or 
                               v == Macros.Game.NOTICE_PARAM_TYPE.HERO then 

                            local type_ , info_ = ObjIDTool.GetInfo( tonumber( info.param_list[i].param ) )
                            local lab = string.format( Macros.Quality_RT[ info_[ qualityType[v] ] ] , info_.name )
                            table.insert( tab , {zi = lab} )

                        --如果是卡牌
                        elseif v == Macros.Game.NOTICE_PARAM_TYPE.CARD then

                        --如果是世界boss                         
                        elseif v == Macros.Game.NOTICE_PARAM_TYPE.WORLDBOSS then

                            --boss信息
                            local bossCnf = CPPGameLib.GetConfig("WorldBoss", tonumber(info.param_list[1].param) )
                            --关卡信息
                            local levelCnf = CPPGameLib.GetConfig("GameLevel", tonumber(bossCnf.map_id) )
                            --怪物信息
                            local MonsterCnf = CPPGameLib.GetConfig("Monster", tonumber(levelCnf.icon_id) )

                            table.insert( tab , {zi = MonsterCnf.name} )

                        --如果是怪物攻城
                        elseif v == Macros.Game.NOTICE_PARAM_TYPE.MONSTER_STROMCASTLE then
                            
                            local AttackCnf = CPPGameLib.GetConfig("AttackCityMonster", tonumber(info.param_list[i].param) )
                            table.insert( tab , {zi = AttackCnf.boss_name} )

                        --天梯
                        elseif v == Macros.Game.NOTICE_PARAM_TYPE.LADDER then

                            local AttackCnf = CPPGameLib.GetConfig("LadderDan", tonumber(info.param_list[i].param) )
                            table.insert( tab , {zi = AttackCnf.name} )

                        --如果是圣诞boss
                        elseif v == Macros.Game.NOTICE_PARAM_TYPE.CHRISTMAS_BOOS then
                            
                            local AttackCnf = CPPGameLib.GetConfig("ChristmasBoss", tonumber(info.param_list[1].param) )
                            table.insert( tab , {zi = AttackCnf.name} )

                        end
                    end
                end
            end

            local time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
            table.insert( self.__sys_notice_data_list , { text = text , tab = tab , time = time , sort = (cnf.sort or 9999) } )
        end
    end

    table.sort( self.__sys_notice_data_list , function ( a , b )
        if a and b and a.sort and b.sort then
            if a.sort < b.sort then
                return a.sort < b.sort
            elseif a.sort == b.sort then
                if a.time < b.time then
                    return a.time < b.time
                end
            end
        else
            return false
        end
    end )

    self:__CheckShowSystemNotice()
end

--------------------------------------------------------------------------------------------------------------------------------
--公共函数
--------------------------------------------------------------------------------------------------------------------------------
--获取活动公告数据
function Modules.NoticeModule:GetActivityNoticeData( )
    return self.__activity_notice_data_list , self.__activity_notice_icon_list 
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--检测限时系统公告
function Modules.NoticeModule:__CheckShowSystemNotice()
    if #self.__sys_notice_data_list > 0 and not self.__templ_sys_notice then
        local sys_notice_data = table.remove(self.__sys_notice_data_list, 1)
        self.__templ_sys_notice = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_SYSTEM_NOTICE, true, sys_notice_data, function()
            if self.__templ_sys_notice then
                self.__templ_sys_notice:DeleteMe()
                self.__templ_sys_notice = nil
            end
            self:__CheckShowSystemNotice()
        end)
    end
end

--检测游戏帮助公告
function Modules.NoticeModule:__CheckGameHelpNotice()
    local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)

    --根据等级区间去随机一个提示
    local tips_detail_info = {}
    local is_tips = false
    for _, info in pairs(CPPGameLib.GetConfig("RoleLevelTips",nil,nil,true)) do
        if role_level >= info.level_min and role_level <= info.level_max then
            tips_detail_info = info
            is_tips = true
            break
        end
    end
    --随机一个提示
    if is_tips then
        local random_index = MathTool.GetRandom(#tips_detail_info.tips_list,1) 
        local tips_info = CPPGameLib.GetConfig("GameTipsList",tips_detail_info.tips_list[random_index])
        if tips_info then
            self:SystemNoticeResp({text = tips_info.text or "",notice_id = 0})
        end
    end
end

