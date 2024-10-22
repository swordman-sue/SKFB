--
-- @brief 次日登陆
-- @author: yjg
-- @date: 2017年7月15日16:05:38
--


Modules = Modules or {}

Modules.MorrowModule = Modules.MorrowModule or BaseClass(BaseModule)

function Modules.MorrowModule:__init()
	self.is_Initial = true
end

function Modules.MorrowModule:__delete()
end

-- 消息说明: 次日奖励数据请求 消息Id: 24801
function Modules.MorrowModule:NextDayRewardDataReq(info)
	CPPGameLib.PrintTable("xy" , info ,"消息说明: 次日奖励数据请求 消息Id: 24801")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_NEXT_DAY_REWARD_DATA_REQ)
	GlobalNetAdapter:OnSend(data)
end

-- 消息说明: 次日奖励数据响应 消息Id: 24802
function Modules.MorrowModule:NextDayRewardDataResp(info)
	CPPGameLib.PrintTable("xy" , info ,"消息说明: 次日奖励数据响应 消息Id: 24802")
	self.is_take_reward = info.is_take_reward
	self.openKey = false
	if info.is_take_reward == 0 and self.is_Initial == true then
		self.is_Initial = false
		self.openKey = true
		-- LogicGUI.TemplMorrow.New()
		-- CallModuleFunc(ModuleType.MORROW, "OpenView", BaseViewType.MORROW )
	end
end


-- 消息说明: 次日奖励领取请求 消息Id: 24803
function Modules.MorrowModule:NextDayRewardTakeReq(info)
	CPPGameLib.PrintTable("xy" , info ,"消息说明: 次日奖励领取请求 消息Id: 24803")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_NEXT_DAY_REWARD_TAKE_REQ)
	GlobalNetAdapter:OnSend(data)
end

-- 消息说明: 次日奖励领取响应 消息Id: 24804
function Modules.MorrowModule:NextDayRewardTakeResp(info)
	CPPGameLib.PrintTable("xy" , info ,"消息说明: 次日奖励领取响应 消息Id: 24804")
    local t1 = {}
    table.insert(t1,{id = ConfigAdapter.Common.GetMorrowAward() , num = 1 })
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )
    self.is_take_reward = 1
    GlobalEventSystem:FireNextFrame(Macros.Event.MorrowModule.MORROW_AWARD)
end

function Modules.MorrowModule:GetEndTime()
	if not self.__end_time then
	    local role_create_time = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo").role_create_time
	    local year, month, day, hour, minute, second, week = TimeTool.GetDateFromTimeStamp( role_create_time )
	    role_create_time = role_create_time - ( hour * 60 * 60 ) - ( minute * 60 ) - second
		local year, month, day, hour, minute, second, week = TimeTool.GetDateFromTimeStamp( role_create_time )
	    self.__end_time = role_create_time + ( 24 * 60 * 60  )  
    end   
    return self.__end_time
end

--获得领取状态
function Modules.MorrowModule:GetIsTakeReward()
	return self.is_take_reward
end 

--获得是否开启
function Modules.MorrowModule:GetOpenKey()
	return self.openKey
end
