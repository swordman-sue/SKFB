Modules = Modules or {}

Modules.DaliytaskAndAchievementModule = Modules.DaliytaskAndAchievementModule or BaseClass(BaseModule)

function Modules.DaliytaskAndAchievementModule:__init()
	self:__RegistProtocolForOpenView(BaseViewType.DALIYTASK_AND_ACHIEVEMENT,Net.ProtocolNo.C2S_DAILY_MISSION_LIST_REQ, false)
	self:__RegistProtocolForOpenView(BaseViewType.DALIYTASK_AND_ACHIEVEMENT,Net.ProtocolNo.C2S_ACHIEVEMENT_LIST_REQ, false)
end

--日常任务列表请求
function Modules.DaliytaskAndAchievementModule:DaliyMissionListReq()
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_DAILY_MISSION_LIST_REQ)
	GlobalNetAdapter:OnSend(protocol)
end

--日常任务列表请求响应
function Modules.DaliytaskAndAchievementModule:DaliyMissionListResp(data)
	self.DaliyMissionList = data
	local view = self:GetView()
    if view and view:IsOpen() then
        view:__InitTask()
    end
end

--获取日常任务列表
function Modules.DaliytaskAndAchievementModule:getDaliyMissionList()
	return self.DaliyMissionList
end

--任务数据通知
function Modules.DaliytaskAndAchievementModule:DaliyMissionDataNotifyResp(data)
end

--日常任务奖励领取请求
function Modules.DaliytaskAndAchievementModule:DaliyMissionAwardReq(mission_id,base_exp)
	self.__base_exp = base_exp
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_DAILY_MISSION_AWARD_REQ)
	protocol.mission_id = mission_id
	GlobalNetAdapter:OnSend(protocol)
end

--日常任务奖励领取响应
function Modules.DaliytaskAndAchievementModule:DaliyMissionAwardResp(data)
	local t1 = {}
	local missionInfo = CPPGameLib.GetConfig("DailyMission",data.mission_id)
	local encourage_String = ""
	for i,item_id in ipairs(missionInfo.award_item_id) do
		local type,itemInfo = ObjIDTool.GetInfo(item_id)
		encourage_String = encourage_String..itemInfo.name.." x "..missionInfo.award_item_num[i]..","
		table.insert( t1 , { id = item_id , num = missionInfo.award_item_num[i] })
	end

	--经验
	if missionInfo.base_exp then
		local huo_bi_config = CPPGameLib.GetConfig("HuoBi",Macros.Game.RolePropType.EXP)
		table.insert( t1 , {id = huo_bi_config.big_icon , num = self.__base_exp or 0})
	end

    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_LITTLE_ACQUIRE_TIPS, nil, t1)
end

--日常任务积分奖励领取请求
function Modules.DaliytaskAndAchievementModule:DaliyMissionPointAwardReq(point)
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_DAILY_MISSION_POINT_AWARD_REQ)
	protocol.point = point
	GlobalNetAdapter:OnSend(protocol)
end

--日常任务积分奖励领取响应
function Modules.DaliytaskAndAchievementModule:DaliyMissionPointAwardResp(data)
	local roleLevel = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.DAILY_INIT_LEVEL)
    local missionInfo = nil
    for i,v in pairs(CPPGameLib.GetConfig("TeamLevelDailyMission",nil,nil,true)) do
    	if roleLevel >= v.level_min and roleLevel <= v.level_max then
    		missionInfo = v
    		break
    	end
    end
    local itemId = nil
    for i,v in ipairs(missionInfo.point_list) do
    	if v == data.point then
    		itemId = missionInfo.point_award_list[i]
    		break
    	end
    end
    local t1 = {}
	local type,itemInfo = ObjIDTool.GetInfo(itemId)
    if itemInfo.type == Macros.Game.ItemBigType.GIFT and itemInfo.sub_type == Macros.Game.GiftItemSubType.FIXATION then
    	for i,item_Id in ipairs(itemInfo.item_id_list) do
    		table.insert( t1 , { id = item_Id , num = itemInfo.item_max_num_list[i] }) 		
    	end
    else
    	table.insert( t1 , { id = itemId , num = 1 }) 	
    end

    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_LITTLE_ACQUIRE_TIPS, nil, t1)
end

--成就列表请求
function Modules.DaliytaskAndAchievementModule:AchievementListReq()
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ACHIEVEMENT_LIST_REQ)
	GlobalNetAdapter:OnSend(protocol)
end

--成就列表响应
function Modules.DaliytaskAndAchievementModule:AchievementListResp(data)
	self.__achievement_list = data
end

--获取成就列表
function Modules.DaliytaskAndAchievementModule:GetAchievementList()
	return self.__achievement_list
end

--成就数据通知
function Modules.DaliytaskAndAchievementModule:AchievementDataNotifyResp(data)
end

--成就奖励领取请求
function Modules.DaliytaskAndAchievementModule:AchievementAwardReq(achievement_id,cur_reward_lv,item_index)
	self.cur_reward_lv = cur_reward_lv
	self.__item_index = item_index
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ACHIEVEMENT_AWARD_REQ)
	protocol.achievement_id = achievement_id
	GlobalNetAdapter:OnSend(protocol)
end

--成就奖励领取请求
function Modules.DaliytaskAndAchievementModule:AchievementAwardResp(data)
	local bigData = CPPGameLib.GetConfig("Achievement",data.achievement_id)
	local achievementInfo = bigData.level_list[self.cur_reward_lv]
	local encourage_String = ""
	local type,itemInfo = ObjIDTool.GetInfo(achievementInfo.award_item_id)

	local t1 = {}
	table.insert( t1 , { id = achievementInfo.award_item_id , num = achievementInfo.award_item_num })
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_LITTLE_ACQUIRE_TIPS, nil, t1)
	self:FireNextFrame(Macros.Event.DaliyMissionAndAchievementModule.REFRASH_ACHIEVEMENT_CELL,self.__item_index)
end