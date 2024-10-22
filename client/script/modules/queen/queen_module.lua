--[[
%% @module: 女王
%% @author: yjg
%% @created: 2018年2月5日16:59:20
--]]

Modules = Modules or {}

Modules.QueenModule = Modules.QueenModule or BaseClass(BaseModule)

function Modules.QueenModule:__init()
end

function Modules.QueenModule:__delete()
    if self.highRenewTime then
        GlobalTimerQuest:CancelQuest( self.highRenewTime )
        self.highRenewTime = nil
    end
end

-- 消息说明:请求累充奖励数据 消息Id: 26201
function Modules.QueenModule:TotalRechargeRewardDataReq( info )
    CPPGameLib.PrintTable("xy" , info , "消息说明:请求累充奖励数据 消息Id: 26201")  
    local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TOTAL_RECHARGE_REWARD_DATA_REQ)
    GlobalNetAdapter:OnSend(data) 
end

-- 消息说明:请求累充奖励数据响应 消息Id: 26202
-- "cur_data_id__H", --当前活动数据ID
-- "total_amount__I", --总的充值额度
-- "reward_record_list__T__reward_id@H", --奖励记录列表(奖励ID)
function Modules.QueenModule:TotalRechargeRewardDataResp( info )
    CPPGameLib.PrintTable("xy" , info , "消息说明:请求累充奖励数据响应 消息Id: 26202")  
    local Time = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.QUEEN )
    self.cur_data_id = Time.subtype
    
    self.total_amount = info.total_amount
    self.reward_record_list = {}
    for i , v in ipairs( info.reward_record_list ) do
        self.reward_record_list[ v.reward_id ] = true
    end
    self:Countdown()

    --统计红点
    CallModuleFunc(ModuleType.RED_MGR , "StatisticsRed" , Macros.Game.SystemID.QUEEN )
end

-- 消息说明:领取累充奖励请求 消息Id: 26203
-- "reward_id__H", --奖励ID
function Modules.QueenModule:TakeTotalRechargeRewardReq( info )
    CPPGameLib.PrintTable("xy" , info , "消息说明:领取累充奖励请求 消息Id: 26203")  
    local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TAKE_TOTAL_RECHARGE_REWARD_REQ)
    data.reward_id = info.reward_id
    GlobalNetAdapter:OnSend(data) 
end

-- 消息说明:领取累充奖励响应 消息Id: 26204
-- "reward_id__H", --奖励ID
function Modules.QueenModule:TakeTotalRechargeRewardResp( info )
    CPPGameLib.PrintTable("xy" , info , "消息说明:领取累充奖励响应 消息Id: 26204")

    local cnf = self:GetTotalRechargeRewardCnf()
    local lst = nil
    local t1 = {}
    if info.reward_id < #cnf then
        lst = self:GetAwardLst( info.reward_id )
        if #lst ~= 0 then
            t1 = lst
        else
            table.insert( t1 , { id = lst.id , num = lst.num } )
        end
    else
        lst = self:GetAwardPackage()
        t1 = lst
    end
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
    self.reward_record_list[ info.reward_id ] = true
    GlobalEventSystem:FireNextFrame(Macros.Event.QueenModule.QUEEN_DATA)

    --统计红点
    CallModuleFunc(ModuleType.RED_MGR , "StatisticsRed" , Macros.Game.SystemID.QUEEN )
end

function Modules.QueenModule:GetTotalRechargeRewardCnf( )
    local cnf = CPPGameLib.GetConfig("TotalRechargeReward", self.cur_data_id , false )
    if not cnf then
        return 
    end
    return cnf.reward_id_list
end

--获得累计充值数据
function Modules.QueenModule:GetGrandTotalPay()
    local cnf = self:GetTotalRechargeRewardCnf( )
    if not cnf then
        return 0 , 0
    end
    local max = cnf[#cnf].need_recharge_amount
    local num = self.total_amount or 0

    local addUpAddition = CallModuleFunc(ModuleType.CHARGE , "GetAccumulatedChargeFactor")
    return num * addUpAddition , ( max or 0 ) * addUpAddition
end


--获得奖励礼包领取状态
function Modules.QueenModule:GetAwardPackageType()
    local cnf = self:GetTotalRechargeRewardCnf( )
    if not cnf then
        return ""
    end
    local num , max = self:GetGrandTotalPay()
    local maxInfo = cnf[#cnf]
    local text = ""

    local addUpAddition = CallModuleFunc(ModuleType.CHARGE , "GetAccumulatedChargeFactor")
    if num >= (maxInfo.need_recharge_amount * addUpAddition ) then
        if self:GetRewardRecordList( #cnf ) then
            --领过了
            text = "Already"
        else
            --可领取
            text = "May"
        end
    else
        --不可领取
        text = "Nein"
    end

    return text
end

--奖励礼包
function Modules.QueenModule:GetAwardPackage()
    if not self.awardPackage then
        self.awardPackage = {}
        local cnf = self:GetTotalRechargeRewardCnf( )
        if not cnf then
            return self.awardPackage 
        end
        -- --非礼包
        -- for i , v in ipairs( cnf[#cnf].reward_id_list ) do
        --     table.insert( self.awardPackage , { id = v , num = cnf[#cnf].reward_num_list[i] } )
        -- end
        --礼包
        local item = CPPGameLib.GetConfig("Item", cnf[#cnf].reward_id_list[1] )
        for i , v in ipairs( item.item_id_list ) do
            table.insert( self.awardPackage , { id = v , num = item.item_min_num_list[i] * cnf[#cnf].reward_num_list[1]  } )
        end
    end
    return self.awardPackage
end

--奖励列表
function Modules.QueenModule:GetAwardLst( index )
    if not self.awardLst then
        self.awardLst = {}
        local cnf = self:GetTotalRechargeRewardCnf( )
        if not cnf then
            return self.awardLst
        end
        for i , v in ipairs( cnf ) do
            if i < #cnf then
                --非礼包
                -- for ii , vv in ipairs( v.reward_id_list ) do
                --     table.insert( self.awardLst , { id = vv , num = v.reward_num_list[ii] , recharge = v.need_recharge_amount * 10 } )
                -- end
                --礼包
                 local addUpAddition = CallModuleFunc(ModuleType.CHARGE , "GetAccumulatedChargeFactor")
                table.insert( self.awardLst , { id = v.reward_id_list[1] , num = v.reward_num_list[1] , recharge = v.need_recharge_amount * addUpAddition } )
            end
        end
    end
    if index then
        return self.awardLst[index]
    end
    return self.awardLst
end

--获得奖励领取记录
function Modules.QueenModule:GetRewardRecordList( index )
    if not self.reward_record_list then
        self.reward_record_list = {}
    end
    return self.reward_record_list[ index ]
end

--界面保留条件
function Modules.QueenModule:GetRetainTerm( )
    local num , max = self:GetGrandTotalPay()
    local cnf = self:GetTotalRechargeRewardCnf()
    if not cnf then
        return false
    end

    --是否有未领取的奖励
    if CallModuleFunc(ModuleType.QUEEN, "CheckCanReward") then
        return true
    end

    --活动是否结束
    local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.QUEEN )
    local n_Time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
    if not info then
        return false
    end

    if not info.over or info.over - n_Time <= 0 then
        --结束了
        --不保留
        return false        
    else
        --未结束
        --是否买完了而且领完了
        local cnf = self:GetTotalRechargeRewardCnf()
        local index = 0
        for i , v in ipairs( cnf ) do
            local type_ = self:GetRewardRecordList( i )
            if type_ then
                index = index + 1
            end
        end
        if index == #cnf then
            --领完了 
            return false
        else
            return true
        end
    end
end

--倒计时
function Modules.QueenModule:Countdown( )
    --倒计时
    local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.QUEEN )
    CPPGameLib.PrintTable("xy" , info , "ToIdGetActivityTime")  
    local n_Time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
    if not info.over or info.over - n_Time <= 0 then
        GlobalEventSystem:FireNextFrame(Macros.Event.QueenModule.QUEEN_COUNTDOWN , 0 )
        return 
    end

    if self.highRenewTime then
        GlobalTimerQuest:CancelQuest( self.highRenewTime )
        self.highRenewTime = nil
    end

    self.highRenewTime = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(function()
        local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.QUEEN )
        local n_Time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
        if info.over - n_Time <= 0 then
            if self.highRenewTime then
                GlobalTimerQuest:CancelQuest( self.highRenewTime )
                self.highRenewTime = nil
            end
        end
        GlobalEventSystem:FireNextFrame(Macros.Event.QueenModule.QUEEN_COUNTDOWN , info.over - n_Time )
    end),1 ,-1)
end

--检测是否有未领取奖励
function Modules.QueenModule:CheckCanReward(calc_red_num)
    local red_num = 0

    local cnf = CallModuleFunc(ModuleType.QUEEN , "GetTotalRechargeRewardCnf")
    if cnf then
        local num , max = CallModuleFunc(ModuleType.QUEEN , "GetGrandTotalPay") 

        --是否有未领取的奖励
        for i , v in ipairs( cnf ) do
            local type_ = CallModuleFunc(ModuleType.QUEEN , "GetRewardRecordList" , i )
            local addUpAddition = CallModuleFunc(ModuleType.CHARGE , "GetAccumulatedChargeFactor")
            if num >= (v.need_recharge_amount * addUpAddition ) and not type_ then
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
    end

    if calc_red_num then
        return red_num
    end
end