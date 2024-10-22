--[[
%% @module: 组队
%% @author: ldx
%% @created: 2017-09-01
--]]

Modules = Modules or {}

Modules.TeamModule = Modules.TeamModule or BaseClass(BaseModule)

function Modules.TeamModule:__init()
	self.__is_show_gashapon_cost_diamonds_tips = true
end

function Modules.TeamModule:__delete()
	
end

--答题请求
function Modules.TeamModule:TeamDungeonAnswerQuestionReq(req_info)
	CPPGameLib.PrintTable("ldx",req_info,"答题请求: ")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TEAM_DUNGEON_ANSWER_QUESTION_REQ)
	data.is_right = req_info.is_right
	data.answer = req_info.answer
	GlobalNetAdapter:OnSend(data)	
end

--答题响应
function Modules.TeamModule:TeamDungeonAnswerQuestionResp(protocol_data)
	
end

--答题通知
function Modules.TeamModule:TeamDungeonAnswerQuestionNotify(protocol_data)
	CPPGameLib.PrintTable("ldx",protocol_data,"答题通知 ")
	--答题响应通知
	if protocol_data.answer ~= Macros.Global.FALSE then
		--如果为0则是超时未答题，不进行显示
		self:FireNextFrame(Macros.Event.TeamModule.TEAM_DUNGEON_ANSWER_QUESTION_NOTIFY,protocol_data)
	end
end

--对话结束请求
function Modules.TeamModule:TeamDungeonDialogOverReq()
	-- print("skfb", "对话结束请求")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TEAM_DUNGEON_DIALOG_OVER_REQ)
	GlobalNetAdapter:OnSend(data)	
end

--对话结束响应
function Modules.TeamModule:TeamDungeonDialogOverResp(protocol_data)
end

--对话结束通知
function Modules.TeamModule:TeamDungeonDialogOverNotify(protocol_data)
	-- print("skfb", "对话结束通知")
	--结束剧情
	CallModuleFunc(ModuleType.STORY, "Stop")
	--对话结束响应
	self:FireNextFrame(Macros.Event.TeamModule.TEAM_DUNGEON_DIALOG_OVER_RESP)
end

--下一个NPC请求
function Modules.TeamModule:TeamDungeonNextNPCReq(req_info)
	-- print("skfb", "下一个NPC请求")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TEAM_DUNGEON_NEXT_NPC_REQ)
	data.npc = req_info.npc
	GlobalNetAdapter:OnSend(data)	
end

--下一个NPC响应
function Modules.TeamModule:TeamDungeonNextNPCResp(protocol_data)
	
end

--下一个NPC通知
function Modules.TeamModule:TeamDungeonNextNPCNotify(protocol_data)
	-- print("skfb", "下一个NPC通知")
	--删除领奖界面
    if self.__templ_reward_view then
    	self.__templ_reward_view:DeleteMe()
    	self.__templ_reward_view = nil
    end
	self:FireNextFrame(Macros.Event.TeamModule.MOVE_TO_NPC_NOTICE)
end

--Boss任务开始请求
function Modules.TeamModule:TeamDungeonBossTaskStartReq()
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TEAM_DUNGEON_BOSS_TASK_START_REQ)
	GlobalNetAdapter:OnSend(data)	
end

--Boss任务开始响应
function Modules.TeamModule:TeamDungeonBossTaskStartResp(protocol_data)
	-- print("skfb", "Boss任务开始响应 ")

	if self.__task_link_config then
		--打BOSS
	    CallModuleFunc(ModuleType.SCENE, "EnterSceneReq",Macros.Game.SceneType.TEAM_BOSS_ENCOUNTER,self.__task_link_config.scene_id)
	end
end

--Boss任务开始通知
function Modules.TeamModule:TeamDungeonBossTaskStartNotify(protocol_data)
	if self:GetTeamDungeonTeamLeaderRoleId() == CallModuleFunc(ModuleType.ROLE, "GetRoleID") then
		--队长是由BOSS任务开始响应协议，进入BOSS场景的
		return
	end

	-- print("skfb", "Boss任务开始通知")

	if self.__task_link_config then
		--打BOSS
	    CallModuleFunc(ModuleType.SCENE, "EnterSceneReq",Macros.Game.SceneType.TEAM_BOSS_ENCOUNTER,self.__task_link_config.scene_id)
	end
end

--新任务通知
function Modules.TeamModule:TeamDungeonNewTaskNotify(protocol_data)
	-- CPPGameLib.PrintTable("skfb",protocol_data,"新任务通知 ")
	local is_before_question = false
	if self.__new_task_data then
		--任务数据   
    	local config_task = CPPGameLib.GetConfig("TeamDungeonTask",self.__new_task_data.task_id)
    	if config_task.type == Macros.Game.TEAM_TASK_TYPE.QUESTION then
    		is_before_question = true
    	end
	end
	--完成任务数量
	self.__complete_task_num = self.__complete_task_num or 0
	--通知之前判断是否是答题
	local is_question = protocol_data and protocol_data.is_new_task == 0 and true or false
	--新队伍重置任务数量
	self.__complete_task_num = self.__is_first_into_world_map and 0 or self.__complete_task_num
	--判断是否为答题
	self.__complete_task_num = is_question and self.__complete_task_num or (self.__complete_task_num + 1)
	print("ldx","complete_task_numcomplete_task_numcomplete_task_numcomplete_task_num: ",self.__complete_task_num,is_question)
	--任务数据
	self.__new_task_data = protocol_data
	self.__new_task_data.complete_task_num = self.__complete_task_num
	self.__is_have_accepttask = false
	if is_before_question then
		self:FireNextFrame(Macros.Event.TeamModule.TEAM_DUNGEON_SHOW_RIGHT_ANSWER)
	else		
		self:FireNextFrame(Macros.Event.TeamModule.TEAM_DUNGEON_NEW_TASK_NOTIFY)
	end	
	if self.__is_first_into_world_map then
		self.__is_first_into_world_map = false
		self:FireNextFrame(Macros.Event.TeamModule.TEAM_DUNGEON_MATCH_SUCCESS)		
	end
end

--队伍任务数据
function Modules.TeamModule:TeamDungeonTaskDataNotify(protocol_data)	
	-- CPPGameLib.PrintTable("skfb", protocol_data, "队伍任务数据")

	self.__team_dungeon_task_data = protocol_data
	self.__is_finish_task_link = false
	self.__team_dungeon_task_data.energy = self.__team_dungeon_task_data.energy or 0
	self.__team_dungeon_task_data.dungeon_id = self.__cur_team_dungeon_id
	self.__team_dismiss_flag = false
	self:HandleTeamDungeonTaskData()
end

--抽奖请求
function Modules.TeamModule:TeamDungeonLotteryReq(req_info)
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TEAM_DUNGEON_LOTTERY_REQ)
	data.times = req_info.times
	data.is_cost_item = req_info.is_cost_item
	GlobalNetAdapter:OnSend(data)	
end

--抽奖响应
function Modules.TeamModule:TeamDungeonLotteryResp(protocol_data)
	CPPGameLib.PrintTable("ldx",protocol_data,"TeamDungeonLotteryResp")
	local getReward = {}
    for i, rData in ipairs(protocol_data.item_list) do
        table.insert(getReward,{id = rData.item_id, num = rData.item_num})
    end
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , getReward,nil,nil,nil,true)
 	self:FireNextFrame(Macros.Event.TeamModule.TEAM_DUNGEON_LOTTERY_RESP)
end

--组队匹配请求
function Modules.TeamModule:TeamDungeonMatchReq(req_info)	
	CPPGameLib.PrintTable("ldx",req_info,"组队匹配请求")	
	self.__cur_team_dungeon_id = req_info.dungeon_id
	self.__is_team = req_info.is_team
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TEAM_DUNGEON_MATCH_REQ)
	data.dungeon_id = req_info.dungeon_id
	data.is_team = req_info.is_team
	GlobalNetAdapter:OnSend(data)	
end

--组队匹配响应
function Modules.TeamModule:TeamDungeonMatchResp(protocol_data)
	CPPGameLib.PrintTable("ldx",protocol_data,"组队匹配响应: ")
	self:FireNextFrame(Macros.Event.TeamModule.BREAK_THE_TEAM,true)
	local view = self:GetView()
    if view and view:IsOpen() then
	    view:__HandleQuickMatchingMessage(protocol_data)
    end
    self.__is_on_matching = true
end

--组队匹配通知
function Modules.TeamModule:TeamDungeonMatchNotify(protocol_data)
	CPPGameLib.PrintTable("ldx",protocol_data,"组队匹配通知: ")
	self:FireNextFrame(Macros.Event.TeamModule.BREAK_THE_TEAM,true)
	protocol_data.dungeon_id = self.__cur_team_dungeon_id
	protocol_data.is_team = self.__is_team
	local view = self:GetView()
    if view and view:IsOpen() then
    	view:__HandleQuickMatchingMessage(protocol_data)
    end
    self.__is_on_matching = true
end

--创建队伍请求
function Modules.TeamModule:TeamDungeonCreateTeamReq(req_info)	
	print("ldx","创建队伍请求: ",req_info)
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TEAM_DUNGEON_CREATE_TEAM_REQ)
	data.dungeon_id = req_info.dungeon_id
	GlobalNetAdapter:OnSend(data)	
end

--创建队伍响应
function Modules.TeamModule:TeamDungeonCreateTeamResp(protocol_data)
	--当前队伍id
    self.__cur_team_id = protocol_data.team_id
    --当前队伍副本id
    self.__cur_team_dungeon_id = protocol_data.dungeon_id
	local view = self:GetView()
    if view and view:IsOpen() then
    	view:AfterTeamDungeonCreateTeam(protocol_data)
    end
end

--组队邀请请求
function Modules.TeamModule:TeamDungeonInviteReq(req_info)	
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TEAM_DUNGEON_INVITE_REQ)
	data.target_id = req_info.target_id
	GlobalNetAdapter:OnSend(data)	
end

--组队邀请响应
function Modules.TeamModule:TeamDungeonInviteResp(protocol_data)
end

--组队邀请通知
function Modules.TeamModule:TeamDungeonInviteNotify(protocol_data)
	CPPGameLib.PrintTable("ldx",protocol_data,"protocol_data ")
	self.__accept_invite_notify_list = self.__accept_invite_notify_list or {}
	--是否有相同的玩家邀请
	local is_have_same_invite = false
	for i,invite_info in ipairs(self.__accept_invite_notify_list) do
		if invite_info.team_id == protocol_data.team_id then
			is_have_same_invite = true
			break
		end
	end
	--如果不是相同的玩家邀请
	if not is_have_same_invite then
		table.insert(self.__accept_invite_notify_list,protocol_data)
	end
	--新推送
	if #self.__accept_invite_notify_list == 1 then
		self:FireNextFrame(Macros.Event.TeamModule.TEAM_DUNGEON_REJECT_JOIN_TEAM)
		--主界面邀请图标
		CallModuleFunc(ModuleType.MAIN_CITY,"ResetRegionEntrysBySystemID",Macros.Game.SystemID.TEAM_INVITE)
	end
end

--接受组队邀请请求
function Modules.TeamModule:TeamDungeonAcceptInviteReq(req_info)	
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TEAM_DUNGEON_ACCEPT_INVITE_REQ)
	data.team_id = req_info.team_id
	data.dungeon_id = req_info.dungeon_id
	GlobalNetAdapter:OnSend(data)	
end

--接受组队邀请响应
function Modules.TeamModule:TeamDungeonAcceptInviteResp(protocol_data)
	CPPGameLib.PrintTable("ldx",protocol_data,"接受组队邀请响应: ")
	--当前队伍id
    self.__cur_team_id = protocol_data.team_id
    --当前队伍副本id
    self.__cur_team_dungeon_id = protocol_data.dungeon_id
    --邀请信息队列
	self.__accept_invite_notify_list = {}
	if CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsInMainCity") then
		print("ldx","TRYGHSUSUD ")
		--是否在主界面
        local view = self:GetViewByType(BaseViewType.MAIN_CITY)
        if view and view:IsLayout() then
        	self.__invite_from_maincity_data = protocol_data
        	--直接进入组队界面
        	CallModuleFunc(ModuleType.SCENE, "EnterSceneReq",Macros.Game.SceneType.TEAM_HALL,1)
        end
    else
    	self:FireNextFrame(Macros.Event.TeamModule.INVITED_TO_JOIN_TEAM_NOTICE,protocol_data)
    end
end

--接受组队邀请通知
function Modules.TeamModule:TeamDungeonAcceptInviteNotify(protocol_data)
	self:FireNextFrame(Macros.Event.TeamModule.TEAM_DUNGEON_NEW_MEMBER_JOIN_TEAM,protocol_data)	
end

--请求玩家推荐列表请求
function Modules.TeamModule:TeamDungeonRecommendListReq()	
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TEAM_DUNGEON_RECOMMEND_LIST_REQ)
	GlobalNetAdapter:OnSend(data)	
end

--请求玩家推荐列表响应
function Modules.TeamModule:TeamDungeonRecommendListResp(protocol_data)
	self:FireNextFrame(Macros.Event.TeamModule.TEAM_DUNGEON_CREATE_TEAM_VIEW_VISIBLE,false)
	LogicGUI.TemplInviteTeamMateView.New(protocol_data.member_list)
end

--组队取消匹配请求
function Modules.TeamModule:TeamDungeonCancelMatchReq(is_return_city,is_timeout)	
	self.__is_return_city = is_return_city
	self.__is_matching_timeout = is_timeout
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TEAM_DUNGEON_CANCEL_MATCH_REQ)
	GlobalNetAdapter:OnSend(data)	
end

--组队取消匹配响应
function Modules.TeamModule:TeamDungeonCancelMatchResp()
	--返回主城
	if self.__is_return_city then
		CallModuleFunc(ModuleType.SCENE,"LeaveReturnCity")
		self.__is_return_city = false
		return
	end
	--活动结束
	if self.__is_matching_timeout then
		if self.__is_team then
			--退出队伍
             self:TeamDungeonLeaveReq(true)
		end
		--提示
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("team_over_activity_tips"))
		self.__is_matching_timeout = false
	end
	local view = self:GetView()
    if view and view:IsOpen() then
    	view:AfterCancelMatching()
    end
    self.__is_on_matching = false
end

--组队取消匹配通知
function Modules.TeamModule:TeamDungeonCancelMatchNotify()
	local view = self:GetView()
    if view and view:IsOpen() then
    	view:AfterCancelMatching()
    end
    self.__is_on_matching = false
end

--离开队伍请求
function Modules.TeamModule:TeamDungeonLeaveReq(is_timeout)	
	print("kx","TeamDungeonLeaveReqTeamDungeonLeaveReq ",is_timeout)
	self.__is_grouping_timeout = is_timeout
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TEAM_DUNGEON_LEAVE_REQ)
	GlobalNetAdapter:OnSend(data)	
end

--离开队伍响应
function Modules.TeamModule:TeamDungeonLeaveResp(protocol_data)
end

--离开队伍通知
function Modules.TeamModule:TeamDungeonLeaveNotify(protocol_data)
	CPPGameLib.PrintTable("kx",protocol_data,"离开队伍通知 ")	
	if not self.__is_grouping_timeout then
		--模型
		if self.__team_dungeon_task_data and protocol_data.is_task_team == 1 then
		    for i=1,Macros.Game.MAX_TEAMMATE_NUM do   
		        if self.__team_dungeon_task_data.member_list[i] and self.__team_dungeon_task_data.member_list[i].role_id == protocol_data.role_id then
		            --提示
	        		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("team_member_leave_the_team_tips",self.__team_dungeon_task_data.member_list[i].role_name or ""))
		            --数据调整
		            for k=Macros.Game.MAX_TEAMMATE_NUM,i+1,-1 do
		            	if self.__team_dungeon_task_data.member_list[k] and self.__team_dungeon_task_data.member_list[k - 1] then
			            	self.__team_dungeon_task_data.member_list[k].pos_x = self.__team_dungeon_task_data.member_list[k - 1].pos_x
			            	self.__team_dungeon_task_data.member_list[k].pos_y = self.__team_dungeon_task_data.member_list[k - 1].pos_y
			            end
		            end
		            table.remove(self.__team_dungeon_task_data.member_list,i)	            
		            break
		        end
		    end
		end
	end	
	self:FireNextFrame(Macros.Event.TeamModule.LEAVE_THE_TEAM,protocol_data,self.__is_grouping_timeout)
	self.__is_grouping_timeout = false
end

--解散队伍通知
function Modules.TeamModule:TeamDungeonDismissNotify(protocol_data)
	-- print("skfb", "解散队伍通知")

	--队伍解散标记
	self.__team_dismiss_flag = true
	self.__team_leave_flag = protocol_data.is_captain_leave
	--定时器取消
	if self.__task_retain_count_down_timer then
		GlobalTimerQuest:CancelQuest(self.__task_retain_count_down_timer)
		self.__task_retain_count_down_timer = nil
	end
	self:FireNextFrame(Macros.Event.TeamModule.BREAK_THE_TEAM,false,self.__is_grouping_timeout)
	self.__is_grouping_timeout = false
end

--接任务请求
function Modules.TeamModule:TeamDungeonAcceptTaskReq()	
	-- print("skfb", "接任务请求: ")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TEAM_DUNGEON_ACCEPT_TASK_REQ)
	GlobalNetAdapter:OnSend(data)	
end

--接任务响应
function Modules.TeamModule:TeamDungeonAcceptTaskResp()
	
end

--接任务通知
function Modules.TeamModule:TeamDungeonAcceptTaskNotify(protocol_data)
	-- CPPGameLib.PrintTable("skfb",protocol_data,"接任务通知: ")
	self.__is_have_accepttask = true
	--清空组队副本任务奖励通知
	self.__task_reward_list = nil	
	--结束剧情
	CallModuleFunc(ModuleType.STORY, "Stop")
	--对话结束响应
	self:FireNextFrame(Macros.Event.TeamModule.TEAM_DUNGEON_DIALOG_OVER_RESP,true)
end

--任务链完成通知
function Modules.TeamModule:TeamDungeonTaskLinkFinishNotify()
	--判断是否为答题
	self.__complete_task_num = self.__complete_task_num + 1
	self.__new_task_data.complete_task_num = self.__complete_task_num
	self.__is_finish_task_link = true
	--判断是否答题	
	local is_before_question = false
	if self.__new_task_data then
		--任务数据   
    	local config_task = CPPGameLib.GetConfig("TeamDungeonTask",self.__new_task_data.task_id)
    	if config_task.type == Macros.Game.TEAM_TASK_TYPE.QUESTION then
    		is_before_question = true
    	end
	end
	--任务数据
	if is_before_question then
		self:FireNextFrame(Macros.Event.TeamModule.TEAM_DUNGEON_SHOW_RIGHT_ANSWER,true)
	else
		self:FireNextFrame(Macros.Event.TeamModule.TEAM_DUNGEON_NEW_TASK_NOTIFY)
	end
end

--组队副本任务奖励通知
function Modules.TeamModule:TeamDungeonTaskRewardNotify(protocol_data)
	CPPGameLib.PrintTable("ldx",protocol_data,"组队副本任务奖励通知")
	self.__team_dungeon_task_data.energy = self.__team_dungeon_task_data.energy or 0
	self.__team_dungeon_before_energy = self.__team_dungeon_task_data.energy
	self.__team_dungeon_task_data.energy = protocol_data.energy
	self.__is_have_energy_fly = true

	self.__task_reward_list = {}
	for _, rData in ipairs(protocol_data.item_list) do
		table.insert(self.__task_reward_list, {id = rData.item_id , num = rData.item_num})
	end
	--额外奖励
	for _, rData in ipairs(protocol_data.extra_reward_list) do
		table.insert(self.__task_reward_list, {id = rData.item_id , num = rData.item_num,is_extra = true})
	end
	if not CallModuleFunc(ModuleType.SP_LOGIC_MGR, "IsTeamBattleDungeon") then
		--不在小怪/BOSS战斗副本，直接弹出奖励窗口
		self:ShowTaskRewardView()
	end

	--飞能量到法阵
	self:FireNextFrame(Macros.Event.TeamModule.TEAM_DUNGEON_FLY_TO_ENERGY_ARRAY)
end

--任务开始请求
function Modules.TeamModule:TeamDungeonTaskStartReq()	
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TEAM_DUNGEON_TASK_START_REQ)
	GlobalNetAdapter:OnSend(data)	
end

--任务开始响应
function Modules.TeamModule:TeamDungeonTaskStartResp()
end

-- "boss_damage__I",   --Boss伤害
-- "mvp_id__I",    --mvp_id
-- "killer_id__I", --击杀者ID
-- "mvp_reward_list__T__item_id@I##item_num@I",    -- MVP奖励
-- "kill_reward_list__T__item_id@I##item_num@I",   -- 击杀奖励
-- "member_reward_list__T__role_id@I##attack_damage@I##reward_list@D|RerwadList|", -- 成员奖励
--消息说明: 组队副本BOSS奖励通知 消息Id: 25447
function Modules.TeamModule:TeamDungeonBossRewardNontify( info )
	CPPGameLib.PrintTable("ldx" , info , "消息说明: 组队副本BOSS奖励通知 消息Id: 25447")  
	self.__boss_reward_data = {}
	self.__boss_reward_data.boss_damage = info.boss_damage

	self.__boss_reward_data.lst = {}
	for i , v in ipairs( info.member_reward_list ) do
		local extra = {}
		--如果是MVP
		if v.role_id == info.mvp_id then
			extra.mvp = info.mvp_reward_list
		end
		--如果是击杀
		if v.role_id == info.killer_id then
			extra.kill = info.kill_reward_list
		end

		table.insert( self.__boss_reward_data.lst , { kill = extra.kill ,  role_id = v.role_id , attack_damage = v.attack_damage , extra = extra , reward_list = v.reward_list:GetData() } )		
	end

	--定时器取消
	if self.__task_retain_count_down_timer then
		GlobalTimerQuest:CancelQuest(self.__task_retain_count_down_timer)
		self.__task_retain_count_down_timer = nil
	end
	self:FireNextFrame(Macros.Event.TeamModule.TEAM_DUNGEON_BOSS_BATTLE_RESULT)
end

--准备队伍数据通知
function Modules.TeamModule:TeamDungeonReadyTeamDataNotify(protocol_data)
	--当前队伍副本id
    protocol_data.dungeon_id = self.__cur_team_dungeon_id
	self:FireNextFrame(Macros.Event.TeamModule.INVITED_TO_JOIN_TEAM_NOTICE,protocol_data)
end

--副本排行榜请求
function Modules.TeamModule:TeamDungeonRankReq(dungeon_id)	
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TEAM_DUNGEON_RANK_REQ)
	data.dungeon_id = dungeon_id
	GlobalNetAdapter:OnSend(data)	
end

--副本排行榜响应
function Modules.TeamModule:TeamDungeonRankResp(protocol_data)
	for i,rank_info in ipairs(protocol_data.rank_list) do
		rank_info.role_name_list = rank_info.role_name_list:GetData()		
	end
	CPPGameLib.PrintTable("ldx",protocol_data,"protocol_dataprotocol_data ")
	self:FireNextFrame(Macros.Event.TeamModule.TEAM_DUNGEON_PASS_RECORD_DATA,protocol_data)
end

--组队副本数据请求
function Modules.TeamModule:TeamDungeonDataReq()	
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TEAM_DUNGEON_DATA_REQ)
	GlobalNetAdapter:OnSend(data)	
end

--组队副本数据响应
function Modules.TeamModule:TeamDungeonDataResp(protocol_data)
	CPPGameLib.PrintTable("ldx",protocol_data,"TeamDungeonDataResp ")
	self.__team_dungeon_data = protocol_data
	self.__team_dungeon_data.dungeon_fight_times = CallModuleFunc(ModuleType.VIP, "GetPrivilege", Macros.Game.VIPType.TEAM_DUNGEON_FIGHT_TIME)
	self:FireNextFrame(Macros.Event.TeamModule.TEAM_DUNGEON_DATA_RESP,self.__team_dungeon_data)
end

--组队副本抽奖记录请求
function Modules.TeamModule:TeamDungeonLotteryRecordReq()	
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TEAM_DUNGEON_LOTTERY_RECORD_REQ)
	GlobalNetAdapter:OnSend(data)	
end

--组队副本抽奖记录响应
function Modules.TeamModule:TeamDungeonLotteryRecordResp(protocol_data)
	--当前界面
	local view = self:GetView()
    if view and view:IsOpen() then
    	--打开扭蛋机
    	view:__OpenGashaponMachine(protocol_data.lottery_record_list)
    end
end

--组队副本匹配目标数量通知
function Modules.TeamModule:TeamDungeonMatchTargetNumNotify(protocol_data)
	CPPGameLib.PrintTable("ldx",protocol_data,"TeamDungeonMatchTargetNumNotify ")
	--当前界面
	local view = self:GetView()
    if view and view:IsOpen() then
    	--打开扭蛋机
    	view:__HandleMatchTargetNumNotify(protocol_data.target_num)
    end
end

--组队快捷聊天请求
function Modules.TeamModule:TeamDungeonShortCutChatReq(chat_id)	
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TEAM_DUNGEON_SHORTCUT_CHAT_REQ)
	data.chat_id = chat_id
	GlobalNetAdapter:OnSend(data)	
end

--组队快捷聊天响应
function Modules.TeamModule:TeamDungeonShortCutChatResp(protocol_data)
	--当前界面
	--组队快捷聊天响应
	self:FireNextFrame(Macros.Event.TeamModule.TEAM_DUNGEON_QUICK_CHAT_RESP,protocol_data)
end

--获取邀请加入队伍信息队列
function Modules.TeamModule:GetInvitedToJoinTheTeamList()
	self.__accept_invite_notify_list = self.__accept_invite_notify_list or {}	
	return self.__accept_invite_notify_list
end


--移除邀请加入队伍信息
function Modules.TeamModule:RemoveInvitedJoinTeamMessage(is_maincity)
	if self.__accept_invite_notify_list and #self.__accept_invite_notify_list > 0 then
		table.remove(self.__accept_invite_notify_list,1)
		self:FireNextFrame(Macros.Event.TeamModule.TEAM_DUNGEON_REJECT_JOIN_TEAM)
		if #self.__accept_invite_notify_list > 0 and is_maincity then 
			self:FireNextFrame(Macros.Event.MaincityModule.OPEN_TEAM_INVITE_MESSAGE)
		elseif #self.__accept_invite_notify_list == 0 and is_maincity then 
			--主界面邀请图标
			CallModuleFunc(ModuleType.MAIN_CITY,"ResetRegionEntrysBySystemID",Macros.Game.SystemID.TEAM_INVITE)
		end
	end
end

--处理队伍数据
function Modules.TeamModule:HandleTeamDungeonTaskData()
	--定时器取消
	if self.__task_retain_count_down_timer then
		GlobalTimerQuest:CancelQuest(self.__task_retain_count_down_timer)
		self.__task_retain_count_down_timer = nil
	end
	self.__task_link_config = CPPGameLib.GetConfig("TeamDungeonTaskLink",self.__team_dungeon_task_data.task_link_id)
	--是否任务时间到
	self.__is_time_to_boss = false
	--任务剩余时间
	if self.__task_link_config then
		self.__task_retain_count_down_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
		function()
			--倒计时
	        local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	        local remain_time = (self.__team_dungeon_task_data.task_start_time + self.__task_link_config.task_time) - server_time
	        self:FireNextFrame(Macros.Event.TeamModule.TEAM_DUNGEON_REMAIN_TIME,remain_time)
	        if remain_time <= 0 then
	        	--定时器取消
		        GlobalTimerQuest:CancelQuest(self.__task_retain_count_down_timer)
		        self.__task_retain_count_down_timer = nil
		        self.__is_time_to_boss = true
		        --删除领奖界面
		        if self.__templ_reward_view then
		        	self.__templ_reward_view:DeleteMe()
		        	self.__templ_reward_view = nil
		        end
		        --中止任务去打BOSS
		        self:FireNextFrame(Macros.Event.TeamModule.TEAM_DUNGEON_TASK_TIME_TO_BOSS)
	        end
		end), 1, -1)
	end
	self.__is_first_into_world_map = true	
	self.__is_first_enter_scene = true
end

--处理商品的购买记录
function Modules.TeamModule:HandleTeamShopBuyRecord(shop_type,buy_record_list)
	self.__goods_buy_record_list = self.__goods_buy_record_list or {}
	self.__goods_buy_record_list[shop_type] = self.__goods_buy_record_list[shop_type] or {}	
	for i,shopData in ipairs(buy_record_list) do
		self.__goods_buy_record_list[shop_type][shopData.goods_id] = shopData.buy_num		
	end	
	--组队副本商店购买记录响应
	self:FireNextFrame(Macros.Event.TeamModule.TEAM_DUNGEON_SHOP_RECORD_RESP)
end

--处理商品的购买记录
function Modules.TeamModule:TeamShopBuyResp(shop_type,shop_info)
	--配置信息
	local reward_list = {}
	local goods_config = CPPGameLib.GetConfig("RawGemstoneShop",shop_info.param1)
	table.insert(reward_list, {id = goods_config.item_id , num = goods_config.item_num*shop_info.param2})
	self.__goods_buy_record_list[shop_type][shop_info.param1] = self.__goods_buy_record_list[shop_type][shop_info.param1] or 0
	self.__goods_buy_record_list[shop_type][shop_info.param1] = self.__goods_buy_record_list[shop_type][shop_info.param1] + shop_info.param2
	self.__templ_reward_view = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , reward_list )
	--组队副本商店购买记录响应
	self:FireNextFrame(Macros.Event.TeamModule.TEAM_DUNGEON_SHOP_RECORD_RESP)
end

--获取某个商品的购买记录
function Modules.TeamModule:GetBuyRecordByShopTypeAndGoodsId(shop_type,goods_id)	
	return self.__goods_buy_record_list[shop_type][goods_id] or 0
end

--获取某种类型的商品记录列表
function Modules.TeamModule:GetBuyRecordByShopType(shop_type)	
	self.__goods_buy_record_list = self.__goods_buy_record_list or {}
	return self.__goods_buy_record_list[shop_type]
end

--获取是否通关某个副本记录
function Modules.TeamModule:GetIsPassDungeonRecord(need_pass_dungeonid)
	if not need_pass_dungeonid then
		return false
	end
	if self.__team_dungeon_data then
		for i,pass_dungeon_info in ipairs(self.__team_dungeon_data.pass_dungeon_list) do
			if pass_dungeon_info.dungeon_id == need_pass_dungeonid then
				return true
			end
		end
	end
	return false
end

--获取队伍任务数据
function Modules.TeamModule:GetTeamDungeonTaskData()
	return self.__team_dungeon_task_data
end

--获取当前任务id
function Modules.TeamModule:GetCurTaskData()
	return self.__new_task_data
end

--获取当前队伍id和当前队伍副本id
function Modules.TeamModule:GetCurTeamIdAndTeamDungeonId()
	return self.__cur_team_id,self.__cur_team_dungeon_id
end

--获取是否从主界面同意进入组队
function Modules.TeamModule:GetInviteFromMainCityData()
	return self.__invite_from_maincity_data
end

--设置是否从主界面同意进入组队
function Modules.TeamModule:SetInviteFromMainCityDataNil()
	self.__invite_from_maincity_data = nil
end

--获取是否有过队长提示
function Modules.TeamModule:GetIsFirstEnterScene()
	return self.__is_first_enter_scene
end

--设置是否有过队长提示
function Modules.TeamModule:SetIsFirstEnterScene()
	self.__is_first_enter_scene = false
end

--获取是否到Boss时间
function Modules.TeamModule:GetIsTimeToBoss()
	return self.__is_time_to_boss
end

--获取是否到Boss时间
function Modules.TeamModule:GetIsFinishTaskLink()
	return self.__is_finish_task_link 
end

--离开场景并返回世界地图
function Modules.TeamModule:LeaveReturnWorldMap()
    CallModuleFunc(ModuleType.SCENE, "LeaveSceneReq")
    GlobalScene:ChangeScene(Macros.Game.SceneType.TEAM_WORLD_MAP)
end

--设置是否有法阵飞入
function Modules.TeamModule:SetIsHaveEnergyFly()
	self.__is_have_energy_fly = false
end

--获取是否有法阵飞入
function Modules.TeamModule:GetIsHaveEnergyFly()
	return self.__is_have_energy_fly
end

--获取是否有法阵飞入
function Modules.TeamModule:GetTeamDungeonBeforeEnergy()
	return self.__team_dungeon_before_energy or 0
end

--计算打怪数量
function Modules.TeamModule:GetTeamDungeonFightMonsterNum()
	self.__team_dungeon_fight_monster_num = self.__team_dungeon_fight_monster_num or 0
	return self.__team_dungeon_fight_monster_num
end

--获取成员列表
function Modules.TeamModule:GetTeamDungeonTeamMemberList()	
	return self.__team_dungeon_task_data and self.__team_dungeon_task_data.member_list or {}
end

--设置打怪数量
function Modules.TeamModule:SetTeamDungeonFightMonsterNum(is_clear)
	self.__team_dungeon_fight_monster_num = self.__team_dungeon_fight_monster_num or 0
	self.__team_dungeon_fight_monster_num = is_clear and 0 or (self.__team_dungeon_fight_monster_num + 1)
end

--获取队伍里面队长的角色ID
function Modules.TeamModule:GetTeamDungeonTeamLeaderRoleId()
	if self.__team_dungeon_task_data and self.__team_dungeon_task_data.member_list[1] then
		return self.__team_dungeon_task_data.member_list[1].role_id
	end
	return 0
end

--设置Boss战开始时间
function Modules.TeamModule:SetTeamDungeonBossBattleStartData(data)
	CPPGameLib.PrintTable("ldx",data,"SetTeamDungeonBossBattleStartData")
	self.__boss_battle_data = data
end

--获取Boss战开始时间
function Modules.TeamModule:GetTeamDungeonBossBattleStartData()
	CPPGameLib.PrintTable("ldx",self.__boss_battle_data,"GetTeamDungeonBossBattleStartData")
	return self.__boss_battle_data
end

--获取组队BOSS结算数据
function Modules.TeamModule:GetBossRewardData( var )
	if var then
		self.__boss_reward_data = nil
	end
	return self.__boss_reward_data
end

--获取是否在匹配状态
function Modules.TeamModule:GetIsOnMatching()
	return self.__is_on_matching
end

--设置是否在匹配状态
function Modules.TeamModule:SetIsOnMatching()
	self.__is_on_matching = false
end

--获取是否已经借取任务（纠错用）
function Modules.TeamModule:GetIsHaveAcceptTask()
	return self.__is_have_accepttask
end

--显示组队副本任务奖励弹窗
function Modules.TeamModule:ShowTaskRewardView()
	if not self.__task_reward_list then
		return
	end

	if not self.__templ_reward_view then
		self.__templ_reward_view = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , self.__task_reward_list, function()
			CallModuleFunc(ModuleType.SP_LOGIC_MGR, "HandleTeamDungeonTaskRewardViewClosed")
		end,nil,100073,true )
	else
		CallModuleFunc(ModuleType.SP_LOGIC_MGR, "HandleTeamDungeonTaskRewardViewClosed")
	end

	self.__task_reward_list = nil
end

--显示组队BOSS结算界面
function Modules.TeamModule:ShowBossRewardView()
	if self.__boss_reward_data then
		GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_TEAM_TRANSCRIPT_COUNT ,false)
		return true
	end	
end

--显示队伍解散通知弹窗
function Modules.TeamModule:ShowTeamDismissNotifyView()
	if self.__team_dismiss_flag then
		-- print("skfb", "ShowTeamDismissNotifyView")
        LogicGUI.TemplAffirmTips.New({type = LogicGUI.TemplAffirmTipsType.YES,content = self.__team_leave_flag == 1 and CPPGameLib.GetString("team_break_the_team_tips") or CPPGameLib.GetString("team_break_the_team_tips1"),
                                        show_close_btn = false,confirm_text = CPPGameLib.GetString("common_confirm"),
                                        confirm_func = function ()                                        
											self.__team_dismiss_flag = false

    										--结束剧情
									        CallModuleFunc(ModuleType.STORY, "Stop")

                                            --进入组队大厅
                                            CallModuleFunc(ModuleType.SCENE, "EnterSceneReq", Macros.Game.SceneType.TEAM_HALL, 1)
                                        end})
	end
end

--显示扭蛋消耗钻石提示
function Modules.TeamModule:SetIsShowGashaponCostDiamondsTips(is_hide)
	self.__is_show_gashapon_cost_diamonds_tips = not is_hide and true or false
end

--显示扭蛋消耗钻石提示
function Modules.TeamModule:GetIsShowGashaponCostDiamondsTips()
	return self.__is_show_gashapon_cost_diamonds_tips
end

--设置从活动整合界面进入组队大厅
function Modules.TeamModule:SetEnterTeamHallFrmPromptView(var)
	self.__enter_team_hall_frm_prompt_view = var
end

--是否从活动整合界面进入组队大厅
function Modules.TeamModule:IsEnterTeamHallFrmPromptView()
	return self.__enter_team_hall_frm_prompt_view
end

--处理错误码
function Modules.TeamModule:HandleErrorNotify(error_code)
	if error_code == 20015 or error_code == 20016 then
		self:RemoveInvitedJoinTeamMessage(true)
	end
end
