--[[
%% @module: 七天累计
%% @author: yjg
%% @created: 2018年1月26日10:42:08
--]]


Modules = Modules or {}

Modules.PushSevenModule = Modules.PushSevenModule or BaseClass(BaseModule)

function Modules.PushSevenModule:__init()
    self:SetSevenDayLoginCnf()
end

function Modules.PushSevenModule:__delete()
end

-- 消息说明:7天登录数据请求 消息Id: 26001
function Modules.PushSevenModule:SevenDayLoginDataReq( info )
    local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SEVEN_DAY_LOGIN_DATA_REQ)
    GlobalNetAdapter:OnSend( data )     
end

-- 消息说明:7天登录数据响应 消息Id: 26002
-- "daily_data_list__T__day@C##flag@C", --奖励记录列表[days][flag] (1=表示已登录过,2=表示已领取)
function Modules.PushSevenModule:SevenDayLoginDataResp( info )
    if not self.sevenDayLogin then
        self.sevenDayLogin = {}
    end

    for i , v in ipairs( info.daily_data_list ) do
        local text = ""
        --未登录
        if v.flag == 0 then
            text = "NoRegister"
        --登录
        elseif v.flag == 1 then
            text = "Register"
        --已领取
        elseif v.flag == 2 then
            text = "Draw"
        end
        self.sevenDayLogin[ v.day ] = text
    end

    GlobalEventSystem:FireNextFrame(Macros.Event.PushSevenModule.PUSH_SEVEN_DATA)

    --统计红点
    CallModuleFunc(ModuleType.RED_MGR , "StatisticsRed" , Macros.Game.SystemID.PUSH_SEVEN )
end

-- 消息说明:7天登录奖励领取请求 消息Id: 26003
-- "day__H",       --要领取的天数
function Modules.PushSevenModule:TakeSevenDayLoginRewardReq( info )
    local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TAKE_SEVEN_DAY_LOGIN_REWARD_REQ)
    data.day = info.day
    GlobalNetAdapter:OnSend( data ) 
end

-- 消息说明:7天登录奖励领取响应 消息Id: 26004
-- "day__H",       --要领取的天数
function Modules.PushSevenModule:TakeSevenDayLoginRewardResp( info )
    local cnf = self:GetSevenDayLoginCnf( info.day )
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , cnf.reward ,function( ... )
        GlobalEventSystem:FireNextFrame( Macros.Event.PushSevenModule.PUSH_SEVEN_NEX , info.day )
    end )

    GlobalEventSystem:FireNextFrame(Macros.Event.PushSevenModule.PUSH_SEVEN_AWARD , info.day )
    self.sevenDayLogin[ info.day ] = "Draw"

    --重置对应区域的系统入口
    CallModuleFunc(ModuleType.MAIN_CITY,"ResetRegionEntrysBySystemID",Macros.Game.SystemID.PUSH_SEVEN)

    --统计红点
    CallModuleFunc(ModuleType.RED_MGR , "StatisticsRed" , Macros.Game.SystemID.PUSH_SEVEN )
end

--拿到每天的状态信息
function Modules.PushSevenModule:GetSevenDayLogin( day )
    if self.sevenDayLogin and self.sevenDayLogin[ day ] then
        return self.sevenDayLogin[ day ]
    elseif self.sevenDayLogin then
        return self.sevenDayLogin
    end
end

--获得能领取的列表
function Modules.PushSevenModule:GetMayDraw( day )
    local t1 = {}
    for i , v in ipairs( self.sevenDayLogin ) do
        if v == "Register" then
            table.insert( t1 , { id = i } )
        end
    end
    return t1
end

--
function Modules.PushSevenModule:SetSevenDayLoginCnf( )
    
    local cnf = CPPGameLib.GetConfig("SevenDayLogin", nil , nil , true )
    
    if not self.sevenDayLoginCnf then
        self.sevenDayLoginCnf = {} 
    end

    for k , v in pairs(cnf) do
        self.sevenDayLoginCnf[v.day] = {}

        local t1 = {}
        for i , vv in ipairs( v.reward_id_list ) do
            table.insert( t1 , { id = vv , num = v.reward_num_list[i] } )
        end

        --显示的礼包内容
        local type_ , info_ = ObjIDTool.GetInfo( t1[1].id )

        local showLst = {}
        if type_ == Macros.Game.ObjType.ITEM then

            if info_.item_id_list then
                for _ , id in ipairs( info_.item_id_list ) do
                    table.insert( showLst , { id = id , num = info_.item_min_num_list[_] } )
                end
            end
        end

        self.sevenDayLoginCnf[v.day] = { 
                                            reward = t1 , 
                                            best = v.best , 
                                            showLst = showLst
                                        } 
    end    
end

--获得七日登录表
function Modules.PushSevenModule:GetSevenDayLoginCnf( day )
    if self.sevenDayLoginCnf then
        if self.sevenDayLoginCnf[day] then
            return self.sevenDayLoginCnf[day]
        else
            return self.sevenDayLoginCnf
        end
    end
end
