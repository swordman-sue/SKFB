Modules = Modules or {}

Modules.FriendModule = Modules.FriendModule or BaseClass(BaseModule)

function Modules.FriendModule:__init()
	--true 是只请求一次                 false  是不断请求
	self.__friend_map = {}
    self:__RegistProtocolForOpenView(BaseViewType.FRIEND, Net.ProtocolNo.C2S_FRIEND_LIST_REQ, false)
    self:__RegistProtocolForOpenView(BaseViewType.FRIEND, Net.ProtocolNo.C2S_FRIEND_APPLY_LIST_REQ, false)
end

function Modules.FriendModule:__delete()
	
end

--请求好友列表
function Modules.FriendModule:FriendListRequest()
	CPPGameLib.PrintTable("xy" , info , "请求好友列表")  

	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_FRIEND_LIST_REQ)
	GlobalNetAdapter:OnSend(data)
end

--请求好友列表响应
function Modules.FriendModule:FreindListResp( info )
	CPPGameLib.PrintTable("xy" , info , "请求好友列表响应")  
	--好友列表的
	--排序，按照领了和没领的
	self.__friend_map[Macros.Game.FRIEND_TYPE.FRIEND_LIST]={}	
	self:SortFriendByOfflineTime(info.friend_list)
	self.__get_energy_times = info.get_energy_times
	for k=0,1 do
		for i,friendData in ipairs(info.friend_list) do
			if friendData.is_give_energy == k then       
				table.insert(self.__friend_map[Macros.Game.FRIEND_TYPE.FRIEND_LIST],friendData)
			end
		end
	end

	--领取精力
	self.__friend_map[Macros.Game.FRIEND_TYPE.GET_ENERY]={}
	for i,friendData in ipairs(info.friend_list) do
		if friendData.get_energy_state == 1 then
			local info = CPPGameLib.CopyTbl(friendData)
			table.insert(self.__friend_map[Macros.Game.FRIEND_TYPE.GET_ENERY],info)
		end
	end
	--获取好友数量
	self:FireNextFrame(Macros.Event.TeamModule.TEAM_DUNGEON_GET_FRIEND_LIST,info.friend_list)
end

function Modules.FriendModule:GetFriendDataAndSetScrollViewByIndex(index)
	print("ldx","haodedongyeahaodoeo; ",index)
	if self.__friend_map and self.__friend_map[index] then
		print("ldx","????????????????: ",index)
		self:FireNextFrame(Macros.Event.FriendModule.INIT_CORRESPONDING_PANEL,index,self.__friend_map[index])
	end
end

--获取各个模块数据
function Modules.FriendModule:GetFriendDataByIndex(index)
	return self.__friend_map[index]
end

--请求黑名单列表
function Modules.FriendModule:BlcakListRequest()	
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_BLACK_LIST_REQ)  
	GlobalNetAdapter:OnSend(data)
end

--黑名单列表响应
function Modules.FriendModule:BlcakListResp( info )
	-- CPPGameLib.PrintTable(info,"herimingdananann")
	self.__friend_map[Macros.Game.FRIEND_TYPE.BLACK_LIST] = {}
	self.__friend_map[Macros.Game.FRIEND_TYPE.BLACK_LIST] = info.black_list	
	self:GetFriendDataAndSetScrollViewByIndex(Macros.Game.FRIEND_TYPE.BLACK_LIST)
end

--请求申请列表
function Modules.FriendModule:FriendApplyListRequest()
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_FRIEND_APPLY_LIST_REQ)  
	GlobalNetAdapter:OnSend(data)
end

--请求申请列表响应
function Modules.FriendModule:FriendApplyListResp( info )
	self.__friend_map[Macros.Game.FRIEND_TYPE.APPLY_LIST] = {}
	self.__friend_map[Macros.Game.FRIEND_TYPE.APPLY_LIST] = info.apply_list
end

--请求推荐列表
function Modules.FriendModule:FriendRecommendListRequest()
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_FRIEND_RECOMMEND_LIST_REQ)  
	GlobalNetAdapter:OnSend(data)
end

--请求推荐列表响应
function Modules.FriendModule:FriendRecommendListResp( info )
	-- CPPGameLib.PrintTable(info,"nicaiciaiicaiiaiaiaia")
	self:SortFriendByOfflineTime(info.recommend_list)
	self.__friend_map[Macros.Game.FRIEND_TYPE.ADD_FRIEND] = {}
	self.__friend_map[Macros.Game.FRIEND_TYPE.ADD_FRIEND] = info.recommend_list
	self:FireNextFrame(Macros.Event.FriendModule.INIT_CORRESPONDING_PANEL,Macros.Game.FRIEND_TYPE.ADD_FRIEND,info.recommend_list)				
end

--查找目标
function Modules.FriendModule:FriendTargetFindReq(target_name)
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_FRIEND_TARGET_FIND_REQ) 
	data.target_name = target_name
	GlobalNetAdapter:OnSend(data)
end

--目标查找响应
function Modules.FriendModule:FriendTargetFindResp( info )
	CPPGameLib.PrintTable(info,"serrsrrd")
	if info.role_id == Macros.Global.FALSE then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("friend_player_no_exist"))
		return
	end
	local tableData = {}
	table.insert(tableData,info)
	self:FireNextFrame(Macros.Event.FriendModule.INIT_CORRESPONDING_PANEL,Macros.Game.FRIEND_TYPE.ADD_FRIEND,tableData)
end

--添加好友
function Modules.FriendModule:FriendAddRequest(info,delete_index,friend_Type)
	--CPPGameLib.PrintTable(info,"发送消息")	
	local role_id = CallModuleFunc(ModuleType.ROLE,"GetRoleID")
	if role_id == info.target_id then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("friend_add_own"))
		return
	end
	self.__delete_index = delete_index
	self.__friend_Type = friend_Type
	if self:GetAgainNameState(info.target_id) then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("friend_again_add"))
		return
	end
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_FRIEND_ADD_REQ)  
	data.target_id = info.target_id
	GlobalNetAdapter:OnSend(data)
	
end

--添加好友响应
function Modules.FriendModule:FriendAddResp(info)
	CPPGameLib.PrintTable("ldx",info,"infoinfo:info: ")
	if info.result == 0 then
		self:RemoveDataFromTable(Macros.Game.FRIEND_TYPE.ADD_FRIEND,info.target_id)
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("friend_add"))
	elseif info.result == 1 then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("friend_max_num"))
	elseif info.result == 2 then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("friend_other_max_num"))
	end
	self:FireNextFrame(Macros.Event.FriendModule.DELETE_ITEM, self.__delete_index,self.__friend_Type)
end

--删除好友
function Modules.FriendModule:FriendDelRequest( target_id )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_FRIEND_DEL_REQ)  
	data.target_id = target_id 
	GlobalNetAdapter:OnSend(data)	
end

--删除好友响应
function Modules.FriendModule:FriendDelResp( info )
    GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("friend_delete_friend"))
end

--申请好友回复
function Modules.FriendModule:FriendApplyReplyRequest(info,delete_index,friend_Type)
	self.__delete_index = delete_index
	self.__friend_Type = friend_Type	
	-- CPPGameLib.PrintTable(info,"发送消息")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_FRIEND_APPLY_REPLY_REQ)
	data.is_accept = info.is_accept 
	data.target_id = info.target_id 
	GlobalNetAdapter:OnSend(data)
end

--申请好友回复
function Modules.FriendModule:FriendApplyReplyResp( info )	
	CPPGameLib.PrintTable(info,"info!!!")
	if info.is_accept == Macros.Global.TRUE then     --TODO:这个不确定
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("friend_agree"))               		
	else
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("friend_refused"))
	end
	self:RemoveDataFromTable(self.__friend_Type,info.target_id)
	self:FireNextFrame(Macros.Event.FriendModule.DELETE_ITEM, self.__delete_index,self.__friend_Type)
end

--添加黑名单
function Modules.FriendModule:BlackAddRequest( target_id )
	local role_id = CallModuleFunc(ModuleType.ROLE,"GetRoleID")
	if role_id == target_id then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("friend_black_own"))
		return
	end
	--[[
	if self.__friend_map[Macros.Game.FRIEND_TYPE.BLACK_LIST] then 
		for i,v in ipairs(self.__friend_map[Macros.Game.FRIEND_TYPE.BLACK_LIST]) do
			if target_id == v.role_id then
	       		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("friend_black_again"))
				return 
			end
		end
	end --]]
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_BLACK_ADD_REQ)
	data.target_id = target_id               --目标ID
	GlobalNetAdapter:OnSend(data)
end

--添加黑名单响应
function Modules.FriendModule:BlcakAddResp( info )	
    GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("friend_add_black_success"))
end	

--删除黑名单
function Modules.FriendModule:BlackDelRequest(target_id,delete_index,friend_Type)
	self.__delete_index = delete_index
	self.__friend_Type = friend_Type
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_BLACK_DEL_REQ) 
	data.target_id = target_id           --目标ID
	GlobalNetAdapter:OnSend(data)
end

--删除黑名单响应
function Modules.FriendModule:BlackDelResp(info)
	self:FireNextFrame(Macros.Event.FriendModule.DELETE_ITEM, self.__delete_index,self.__friend_Type)
end

--赠送精力
function Modules.FriendModule:FriendEnergyGiveRequest(target_id,move_index)
	self.__move_index = move_index
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_FRIEND_ENERGY_GIVE_REQ) 
	data.target_id = target_id            --目标ID
	GlobalNetAdapter:OnSend(data)
end

--赠送精力响应
function Modules.FriendModule:FriendEnergyGiveResp(info)
	self:FireNextFrame(Macros.Event.FriendModule.MOVE_ITEM, self.__move_index)
	GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("friend_give_enery_success"))	
end

function Modules.FriendModule:OneUseGetAllEnery()	
	if #self.__friend_map[Macros.Game.FRIEND_TYPE.GET_ENERY] > 0 then
		self.IsOneUseGetAllEnery = true	
        local level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
        local config = CPPGameLib.GetConfig("RoleLevel",level)
        local eneryData = {}
        local get_energy_times = self.__get_energy_times
        for i,friendData in ipairs(self.__friend_map[Macros.Game.FRIEND_TYPE.GET_ENERY]) do
        	if get_energy_times <= config.friend_num_max then       		
        		table.insert(eneryData,{target_id = friendData.role_id})
        		get_energy_times = get_energy_times + 1
        	else
        		break
        	end			
		end
		self:FriendEnergyGetRequest(eneryData)		        
	else
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("friend_nofriend_get_enery_success"))	
	end	
end

--领取精力
function Modules.FriendModule:FriendEnergyGetRequest(target_list,delete_index,friend_Type)
	self.__delete_index = delete_index
	self.__friend_Type = friend_Type
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_FRIEND_ENERGY_GET_REQ) 
	data.target_list = target_list           --目标ID	
	GlobalNetAdapter:OnSend(data)	
end

--领取精力响应
function Modules.FriendModule:FriendEnergyGetResp( info )	
	for i,role_info in ipairs(info.target_list) do
		for index,friendData in ipairs(self.__friend_map[Macros.Game.FRIEND_TYPE.GET_ENERY]) do
			if role_info.target_id == friendData.role_id then
				table.remove(self.__friend_map[Macros.Game.FRIEND_TYPE.GET_ENERY],index)
				self.__get_energy_times = self.__get_energy_times + 1
			end
		end
	end
	if self.IsOneUseGetAllEnery then
		self:FireNextFrame(Macros.Event.FriendModule.INIT_CORRESPONDING_PANEL,Macros.Game.FRIEND_TYPE.GET_ENERY,self.__friend_map[Macros.Game.FRIEND_TYPE.GET_ENERY])
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("friend_OneUse_get_enery_success"))	
		self.IsOneUseGetAllEnery = false
	else
		self:FireNextFrame(Macros.Event.FriendModule.DELETE_ITEM, self.__delete_index,self.__friend_Type)	
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("friend_get_enery_success"))	
	end	
end

function Modules.FriendModule:FriendDataRefresh(info)
	CPPGameLib.PrintTable(info,"uuwuwdjdjdjdjdjdjjjwww")
	if info.flag == 0 then
		self:RemoveDataFromTable(info.type,info.role_id)
		self:FireNextFrame(Macros.Event.FriendModule.DELETE_ITEM,1,info.type,info.role_id)
		if info.type == Macros.Game.FRIEND_TYPE.FRIEND_LIST then	
			self.__friend_map[Macros.Game.FRIEND_TYPE.GET_ENERY] = self.__friend_map[Macros.Game.FRIEND_TYPE.GET_ENERY] or {}
			for i,eneryData in ipairs(self.__friend_map[Macros.Game.FRIEND_TYPE.GET_ENERY]) do
				if eneryData.role_id == info.role_id then
					self:RemoveDataFromTable(Macros.Game.FRIEND_TYPE.GET_ENERY,info.role_id)	
					self:FireNextFrame(Macros.Event.FriendModule.DELETE_ITEM,1,Macros.Game.FRIEND_TYPE.GET_ENERY,info.role_id)
					break
				end
			end		
		end
	else
		self.__friend_map[info.type] = self.__friend_map[info.type] or {}
		if info.type == Macros.Game.FRIEND_TYPE.FRIEND_LIST then
			self.__friend_map[Macros.Game.FRIEND_TYPE.GET_ENERY] = self.__friend_map[Macros.Game.FRIEND_TYPE.GET_ENERY] or {}				
			if not self:GetAgainNameState(info.role_id) then				
				if info.is_give_energy == 0 then
					table.insert(self.__friend_map[Macros.Game.FRIEND_TYPE.FRIEND_LIST],1,info)
					self:FireNextFrame(Macros.Event.FriendModule.ADD_ITEM,1,Macros.Game.FRIEND_TYPE.FRIEND_LIST,info)
				else	
					table.insert(self.__friend_map[Macros.Game.FRIEND_TYPE.FRIEND_LIST],info)				
					self:FireNextFrame(Macros.Event.FriendModule.ADD_ITEM,nil,Macros.Game.FRIEND_TYPE.FRIEND_LIST,info)
				end
				if info.get_energy_state == 1 then		
					local copy_info = CPPGameLib.CopyTbl(info)		
					table.insert(self.__friend_map[Macros.Game.FRIEND_TYPE.GET_ENERY],1,copy_info)
					self:FireNextFrame(Macros.Event.FriendModule.ADD_ITEM,1,Macros.Game.FRIEND_TYPE.GET_ENERY,copy_info)
				end
			end
		else
			self.__friend_map[Macros.Game.FRIEND_TYPE.BLACK_LIST] = self.__friend_map[Macros.Game.FRIEND_TYPE.BLACK_LIST] or {}
			table.insert(self.__friend_map[Macros.Game.FRIEND_TYPE.BLACK_LIST],1,info)
			self:FireNextFrame(Macros.Event.FriendModule.ADD_ITEM,1,Macros.Game.FRIEND_TYPE.BLACK_LIST,info)
		end
	end
end

function Modules.FriendModule:GetFriendNumber()
	return self.__friend_map[Macros.Game.FRIEND_TYPE.FRIEND_LIST] and 
			#self.__friend_map[Macros.Game.FRIEND_TYPE.FRIEND_LIST] or 0
end

--重复添加好友判断
function Modules.FriendModule:GetAgainNameState(roldId)
	if self.__friend_map[Macros.Game.FRIEND_TYPE.FRIEND_LIST] then
		for i,v in ipairs(self.__friend_map[Macros.Game.FRIEND_TYPE.FRIEND_LIST]) do
			if roldId == v.role_id then 
				return true
			end
		end
	end
	return false
end

--判断是否是好友
function Modules.FriendModule:GetIsFriend(name)
	for i,v in ipairs(self.__friend_map[Macros.Game.FRIEND_TYPE.FRIEND_LIST]) do
		if name == v.role_name then 
			return true
		end
	end
	return false
end

function Modules.FriendModule:GetHaveGetEneryTimes()
	return self.__get_energy_times
end

function Modules.FriendModule:RemoveDataFromTable(friend_type,roleId)
	if self.__friend_map[friend_type] then
		for i,friendData in ipairs(self.__friend_map[friend_type]) do
			if friendData.role_id == roleId then
				table.remove(self.__friend_map[friend_type],i)
				break
			end
		end
	end
end

--在线排序
function Modules.FriendModule:SortFriendByOfflineTime(sort_list)
	table.sort(sort_list,function (data_1,data_2)		
		if not data_1 then
			return false
		elseif not data_2 then
			return true		
		elseif data_1.offline_time == 0 and data_2.offline_time == 0 then
			return data_1.role_id < data_2.role_id 
		elseif data_1.offline_time == 0 and data_2.offline_time ~= 0 then
			return true
		elseif data_1.offline_time ~= 0 and data_2.offline_time == 0 then
			return false
		else
			if data_1.offline_time == data_2.offline_time then
				return data_1.role_id < data_2.role_id 
			else
				return data_1.offline_time > data_2.offline_time
			end			
		end
	end)
end

--通过好友类型获取好友列表
function Modules.FriendModule:GetFriendListByFriendType(friend_type)
	friend_type = friend_type or Macros.Game.FRIEND_TYPE.FRIEND_LIST
	return self.__friend_map[friend_type]
end
