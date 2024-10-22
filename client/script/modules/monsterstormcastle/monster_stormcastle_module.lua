Modules = Modules or {}

Modules.MonsterStormCastleModule = Modules.MonsterStormCastleModule or BaseClass(BaseModule)

function Modules.MonsterStormCastleModule:__init()
	self.__is_need_req_data = true
	self.__own_role_id = CallModuleFunc(ModuleType.ROLE,"GetRoleID")
end

function Modules.MonsterStormCastleModule:__delete()
end

--------------------------------------------------------------------------------------------------------------------------------
--事件
--------------------------------------------------------------------------------------------------------------------------------
--处理主界面显示的事件
function Modules.MonsterStormCastleModule:HandleMaincityVisibleEvent()
	self:__CheckShowJoinBattleDialog()
end

--处理限时副本刷新事件
function Modules.MonsterStormCastleModule:HandleRefreshActivityTime(system_id, show_txt, is_open)
	if is_open then
		self:__CheckShowJoinBattleDialog()
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--协议
--------------------------------------------------------------------------------------------------------------------------------
--攻城数据请求
function Modules.MonsterStormCastleModule:AttackCityDataReq()
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ATTACK_CITY_DATA_REQ)
	GlobalNetAdapter:OnSend(data)
end

--攻城数据响应
function Modules.MonsterStormCastleModule:AttackCityDataResp(player_info)
	self.__player_info = player_info
	local view = self:GetView()
	if view and view:IsOpen() then
		view:__InitResurgenceMessage(self.__player_info)
	end	
end

--攻城进入请求
function Modules.MonsterStormCastleModule:AttackCityEnterReq()
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ATTACK_CITY_ENTER_REQ)
	GlobalNetAdapter:OnSend(data)
end

--攻城进入响应
function Modules.MonsterStormCastleModule:AttackCityEnterResp(player_info)
	self.__player_list = {}
	self.__monster_list = {}
	self.__treasure_list = {}
	for i,info in ipairs(player_info.player_list) do
		self.__player_list[info.role_id] = info
	end
	self:FireNextFrame(Macros.Event.MonsterStormCastleModule.ATTACKCITY_ENTER_RESP,self.__player_list)
end

--攻城进入通知
function Modules.MonsterStormCastleModule:AttackCityEnterNotice(info)
	if self.__player_list  then
		self.__player_list[info.role_id] = info
		self:FireNextFrame(Macros.Event.MonsterStormCastleModule.ATTACKCITY_ENTER_NOTICE,info)
	end	
end

--攻城离开请求
function Modules.MonsterStormCastleModule:AttackCityLeaveReq()
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ATTACK_CITY_LEAVE_REQ)
	GlobalNetAdapter:OnSend(data)
end

--攻城离开响应
function Modules.MonsterStormCastleModule:AttackCityLeaveResp(info)
	
end

--攻城离开通知
function Modules.MonsterStormCastleModule:AttackCityLeaveNotice(info)
	for role_id,player_info in pairs(self.__player_list) do
		if info.role_id == role_id then			
			self.__player_list[role_id] = nil
			break
		end
	end
	self:FireNextFrame(Macros.Event.MonsterStormCastleModule.ATTACKCITY_LEAVE_NOTICE,info.role_id)
end

--攻城行走请求
function Modules.MonsterStormCastleModule:AttackCityGotoReq(info)
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ATTACK_CITY_GOTO_REQ)
	data.src_x = info.src_x
	data.src_y = info.src_y
	data.dest_x = info.dest_x
	data.dest_y = info.dest_y
	GlobalNetAdapter:OnSend(data)
end

--攻城行走响应
function Modules.MonsterStormCastleModule:AttackCityGotoResp(info)
	
end

--攻城行走通知
function Modules.MonsterStormCastleModule:AttackCityGotoNotice(info)	
	if self.__player_list[info.role_id] and self.__own_role_id ~= info.role_id then
		self.__player_list[info.role_id].map_x = info.dest_x
		self.__player_list[info.role_id].map_y = info.dest_y
		self:FireNextFrame(Macros.Event.MonsterStormCastleModule.ATTACKCITY_GOTO_NOTICE,info)
	end	
end

--怪物列表通知
function Modules.MonsterStormCastleModule:AttackCityMonsterListNotice(monster_info)	
	self.__monster_list = self.__monster_list or {}
	self.__cur_chapter = monster_info.cur_chapter
	local monster_config = CPPGameLib.GetConfig("AttackCityMonster",self.__cur_chapter)
	for i,info in ipairs(monster_info.monster_list) do
		self.__monster_list[info.uid] = info
		self.__monster_list[info.uid].is_boss = monster_config.is_boss
	end
	self:FireNextFrame(Macros.Event.MonsterStormCastleModule.ATTACKCITY_MONTSER_LIST_NOTICE,self.__monster_list,self.__cur_chapter)
end

--怪物数据通知(主要是伤害有变化)
function Modules.MonsterStormCastleModule:AttackCityMonsterDataNotice(info)	
	if self.__monster_list[info.uid] then
		self.__monster_list[info.uid].damage = info.damage
		self.__monster_list[info.uid].start_battle_time = info.start_battle_time
		if info.damage >= info.total_hp then
			self.__monster_list[info.uid] = nil
			local monster_config = CPPGameLib.GetConfig("AttackCityMonster",self.__cur_chapter)
			if monster_config.is_boss == Macros.Global.TRUE then
				self:FireNextFrame(Macros.Event.MonsterStormCastleModule.ATTACKCITY_BOSS_DEAD)
				local cur_server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
				self.__player_info.kill_boss_time = cur_server_time
			end
		end
		self:FireNextFrame(Macros.Event.MonsterStormCastleModule.ATTACKCITY_MONTSER_DATA_NOTICE,self.__monster_list[info.uid],info.damage >= info.total_hp,info.uid)
	end	
end

--宝箱列表通知
function Modules.MonsterStormCastleModule:TreasureBoxListNotice(treasure_info)	
	self.__treasure_list = self.__treasure_list or {}
	for i,info in ipairs(treasure_info.treasure_box_list) do
		self.__treasure_list[info.uid] = info
	end
	self:FireNextFrame(Macros.Event.MonsterStormCastleModule.TREASURE_BOX_LIST_NOTICE,self.__treasure_list)
end

--宝箱消失通知
function Modules.MonsterStormCastleModule:TreasureBoxDisappearNotice(info)	
	if self.__treasure_list[info.uid] then
		self.__treasure_list[info.uid] = nil
	end
	self:FireNextFrame(Macros.Event.MonsterStormCastleModule.TREASURE_BOX_DISAPPEAR_NOTICE,info)
end

--宝箱锁住请求
function Modules.MonsterStormCastleModule:TreasureBoxLockReq(uid)
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TREASURE_BOX_LOCK_REQ)
	data.uid = uid
	GlobalNetAdapter:OnSend(data)
end

--宝箱锁住响应
function Modules.MonsterStormCastleModule:TreasureBoxLockResp(info)
	if info.uid == 0 then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("monster_stormcastle_haved_opened_tips"))
	else
		local view = self:GetView()
		if view and view:IsOpen() then
			view:__MovingHero(self.__treasure_list[info.uid].map_x,self.__treasure_list[info.uid].map_y,nil,function ()
				view:__OpenBoxView(info.uid)
			end)
		end	
	end	
end

--宝箱拾取请求
function Modules.MonsterStormCastleModule:TreasureBoxPickUpReq(uid)
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_TREASURE_BOX_PICK_UP_REQ)
	data.uid = uid
	GlobalNetAdapter:OnSend(data)
end

--宝箱拾取响应
function Modules.MonsterStormCastleModule:TreasureBoxPickUpResp(info)
	--奖励
	local reward_list = {}
	for i,item_info in ipairs(info.item_list) do
		table.insert(reward_list,{id = item_info.id , num = item_info.num})
	end
	GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , reward_list )	
end

--复活请求
function Modules.MonsterStormCastleModule:AttackCityReliveReq(is_free_relive)
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ATTACK_CITY_RELIVE_REQ)
	data.is_free_relive = is_free_relive
	GlobalNetAdapter:OnSend(data)
end

--复活响应
function Modules.MonsterStormCastleModule:AttackCityReliveResp(info)
	local view = self:GetView()
	if view and view:IsOpen() then
		view:VisiblePanelResurgence(false,info.is_free_relive)
	end
end

--鼓舞请求
function Modules.MonsterStormCastleModule:AttackCityEncourageReq()
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ATTACK_CITY_ENCOURAGE_REQ)
	GlobalNetAdapter:OnSend(data)
end

--鼓舞响应
function Modules.MonsterStormCastleModule:AttackCityEncourageResp()
	local view = self:GetView()
	if view and view:IsOpen() then
		view:AddSpireEffect()
	end
end

--奖励领取请求
function Modules.MonsterStormCastleModule:AttackCityTakeBossRewardReq(chapter)
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ATTACK_CITY_TAKE_BOSS_REWARD_REQ)
	data.chapter = chapter
	GlobalNetAdapter:OnSend(data)
end

--奖励领取响应
function Modules.MonsterStormCastleModule:AttackCityTakeBossRewardResp(reward_info)
	self:FireNextFrame(Macros.Event.MonsterStormCastleModule.ATTACKCITY_TAKE_BOSSREWARD_RESP,reward_info.chapter)
end

--高排行列表通知
function Modules.MonsterStormCastleModule:AttackCityTopRankListNotice(rank_info)
	self.__rank_list = rank_info.rank_list
	local view = self:GetView()
	if view and view:IsOpen() then
		view:__RefrashRankList(self.__rank_list)
	end
end

--攻城排名列新通知
function Modules.MonsterStormCastleModule:AttackCityRankUpdateNotice(rank_info)
	self.__own_rank_info = rank_info
	local view = self:GetView()
	if view and view:IsOpen() then
		view:UpdateOwnIntergralAndRank(self.__own_rank_info)
	end
end

--攻城排名列新通知
function Modules.MonsterStormCastleModule:MonsterStormCityResp(rank_info)
	LogicGUI.TemplMonsterStormCastleIntergralRankView.New(rank_info)
end

--攻城活动开始通知
function Modules.MonsterStormCastleModule:ActivityStartNotice()
	
end

--攻城活动结束通知
function Modules.MonsterStormCastleModule:ActivityOverNotice()
	self.__monster_list = {}
	self.__treasure_list = {}
end

--------------------------------------------------------------------------------------------------------------------------------
--公共函数
--------------------------------------------------------------------------------------------------------------------------------
--获取鼓舞次数
function Modules.MonsterStormCastleModule:GetMonsterStormCityEncourageTimes()
	return self.__player_info and self.__player_info.encourage_times or 0
end


--获取是否需要重新获取位置
function Modules.MonsterStormCastleModule:GetIsNeedReqData()
	return self.__is_need_req_data
end

--设置是否需要重新获取位置
function Modules.MonsterStormCastleModule:SetIsNeedReqData(is_need)
	self.__is_need_req_data = is_need
end

--获取玩家信息
function Modules.MonsterStormCastleModule:GetAllList()
	return self.__player_list,self.__monster_list,self.__treasure_list,self.__player_info,self.__cur_chapter,self.__rank_list,self.__own_rank_info
end

--获取当前章节
function Modules.MonsterStormCastleModule:GetCurChapter()
	return self.__cur_chapter
end

--获取玩家信息
function Modules.MonsterStormCastleModule:GetPlayerInfo()
	return self.__player_info
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--怪物攻城参战弹窗
function Modules.MonsterStormCastleModule:__CheckShowJoinBattleDialog()
    if not self.__is_pop_monster_stormcastle and not CallModuleFunc(ModuleType.LEAD,"IsInLead") then
        if CallModuleFunc(ModuleType.LIMIT_DUNGEON,"IsLimitDungeonOpen",Macros.Game.SystemID.MONSTER_STORMCASTLE,true) then
            local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
            local notice_info = CPPGameLib.GetConfig("ActivityAdvanceNotice",Macros.Game.SystemID.MONSTER_STORMCASTLE)
            if role_level >= notice_info.need_role_level then
                self.__is_pop_monster_stormcastle = true
                LogicGUI.TemplAffirmTips.New( { 
                    type = LogicGUI.TemplAffirmTipsType.YES_NO,
                    content = CPPGameLib.GetString("monster_stormcastle_entry_tips"),
                    cancel_text = CPPGameLib.GetString("common_cancel") ,
                    confirm_text = CPPGameLib.GetString("common_confirm"),
                    confirm_func = function ()
                        --系统模块跳转
                        GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.MONSTER_STORMCASTLE, nil, false)
                    end,
                } )
            end
        end
    end    
end
