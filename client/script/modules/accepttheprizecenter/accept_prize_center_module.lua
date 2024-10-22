Modules = Modules or {}

Modules.AcceptPrizeCenterModule = Modules.AcceptPrizeCenterModule or BaseClass(BaseModule)

function Modules.AcceptPrizeCenterModule:__init()
	-- body
end

function Modules.AcceptPrizeCenterModule:__delete()
	-- body
end

--奖励列表请求
function Modules.AcceptPrizeCenterModule:RewardCenterListReq()
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_REWARD_CENTER_LIST_REQ)
	GlobalNetAdapter:OnSend(data)
end

--奖励列表响应
function Modules.AcceptPrizeCenterModule:RewardCenterListResp(info)
	self.reward_list = info.reward_list
	for i,data in ipairs(self.reward_list) do		
		data.item_list = data.item_list:GetData()
		data.currency_list = data.currency_list:GetData()
	end
	table.sort(self.reward_list,function (a,b)
		if a.time ~= b.time then
			return a.time > b.time
		else
			return a.id < b.id
		end
	end)
end

--奖励领取请求 
function Modules.AcceptPrizeCenterModule:RewardCenterGetReq(reward_list,delete_index,is_onekey_get)
	self.__delete_index = delete_index
	self.__is_onekey_get = is_onekey_get or false
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_REWARD_CENTER_GET_REQ)
	data.reward_list = reward_list
	GlobalNetAdapter:OnSend(data)
end

--奖励领取响应
function Modules.AcceptPrizeCenterModule:RewardCenterGetResp(info)
	for i,infoData in ipairs(info.reward_list) do
		for key,rewardData in ipairs(self.reward_list) do
			if infoData.id == rewardData.id then
				table.remove(self.reward_list,key)
				break
			end
		end
	end
	self:FireNextFrame(Macros.Event.AcceptPrizeCenterModule.REWARD_DELETE_ITEM, self.__delete_index,self.__is_onekey_get)

	--为空则调用消失接口
	if #self.reward_list == 0 then
		CallModuleFunc(ModuleType.MAIN_CITY,"ResetRegionEntrysBySystemID",Macros.Game.SystemID.ACCEPT_PRIZE_CENTER)
	end
end

--是否显示领奖中心
function Modules.AcceptPrizeCenterModule:IsRewardCenterVisible()
	return #self.reward_list > 0
end

--获取领奖中心列表
function Modules.AcceptPrizeCenterModule:GetRewardCenterList()
	return self.reward_list
end

--领奖中心奖励通知
function Modules.AcceptPrizeCenterModule:RewardNoticeNotification(info)
	--空就打开界面
	if #self.reward_list == 0 then
		CallModuleFunc(ModuleType.MAIN_CITY,"ResetRegionEntrysBySystemID",Macros.Game.SystemID.ACCEPT_PRIZE_CENTER)
	end
	table.insert(self.reward_list,1,info)
end


