--
-- @brief 公会模块 
-- @author: yjg
-- @date: 2016年10月14日10:39:24
--

Modules = Modules or {}

Modules.UnionModule = Modules.UnionModule or BaseClass(BaseModule)
--职位列表
Modules.UnionModule.CHAIRMAN = 1
Modules.UnionModule.VICE_CHAIRMAN = 2
Modules.UnionModule.MEMBER = 3

function Modules.UnionModule:__init()
	--我的工会信息
	self.myUnionData = {}
	--申请列表
	self.apply_list = {}
	--成员列表
	self.member_list = {}
	--成员列表
	self.member_list_ = {}	
	--购买记录
	self.buyRecord = {}
	--领取记录
	self.awardRecord = {}
	--留言
	self.leaveWord_list = {}
end

function Modules.UnionModule:__delete()
	if self.replyTime then
    	GlobalTimerQuest:CancelQuest(self.replyTime)
        self.replyTime = nil
    end
end

--消息说明: 工会数据请求: 消息Id: 22101
function Modules.UnionModule:GuildDateReq(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 工会数据请求: 消息Id: 22101")
	self.reqType = info

	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_DATE_REQ)
	GlobalNetAdapter:OnSend(data)   
end

-- 消息说明: 工会数据响应 消息Id: 22102
function Modules.UnionModule:GuildDateResp(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 工会数据响应 消息Id: 22102")
	self.myUnionData = info

	for i,v in ipairs( self.myUnionData.qs_reward_record_list ) do
		self:SettleBoxType( v.quan_shui )
	end

	if self.reqType == "union" then 
		self.reqType = ""
	    local view = self:GetView()
	    if not view then
	    	if info.guild_id ~= 0 then
	    		GlobalModulesMgr:OpenSystemModule( Macros.Game.SystemID.UNION , nil, false)
	    	else
				self.unionJoin = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.UNION_JOIN , false , function( ... )
			    	if self.unionJoin then
			    		self.unionJoin:DeleteMe()
			    		self.unionJoin = nil
			    	end
				end )
	    	end
	    end   
	end

	GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_DATE)
end

-- 消息说明: 创建工会 消息Id: 22103
function Modules.UnionModule:GuildCreateReq(info)
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_CREATE_REQ)
	data.icon_id = info.icon_id
	data.guild_name = info.guild_name
	GlobalNetAdapter:OnSend(data)  
end

-- 消息说明: 创建工会响应 消息Id: 22104
function Modules.UnionModule:GuildCreateResp(info)
	self:GuildDateReq()
	GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_ESTABLISH)
end

-- 消息说明: 工会列表请求 消息Id: 22133
function Modules.UnionModule:GuildListReq(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 工会列表请求 消息Id: 22133")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_LIST_REQ)
	GlobalNetAdapter:OnSend(data)  
end

-- 消息说明: 工会列表请求响应 消息Id: 22134
function Modules.UnionModule:GuildListResp(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 工会列表请求响应 消息Id: 22134")
	GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_JOIN , info.guild_list)
end

-- 消息说明: 成员列表请求 消息Id: 22131
function Modules.UnionModule:GuildMemberListReq(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 成员列表请求 消息Id: 22131")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_MEMBER_LIST_REQ)
	GlobalNetAdapter:OnSend(data)  
end

-- 消息说明: 成员列表响应 消息Id: 22132
function Modules.UnionModule:GuildMemberListResp(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 成员列表响应 消息Id: 22132")
	self.member_list = info.member_list
	for _ , v in ipairs( info.member_list ) do
		self.member_list_[ v.role_id ] = v
	end

	GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_MEMBER,info.member_list)
end

-- 消息说明: 申请列表请求 消息Id: 22127
function Modules.UnionModule:GuildApplyListReq(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 申请列表请求 消息Id: 22127")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_APPLY_LIST_REQ)
	GlobalNetAdapter:OnSend(data)  
end

-- 消息说明: 申请列表请求响应 消息Id: 22128
function Modules.UnionModule:GuildApplyListResp(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 申请列表请求响应 消息Id: 22128")
	self.apply_list = info.apply_list
	GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_APPLY)
end

-- 消息说明: 留言列表请求 消息Id: 22129
function Modules.UnionModule:GuildLeavemsgListReq(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 留言列表请求 消息Id: 22129")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_LEAVEMSG_LIST_REQ)
	GlobalNetAdapter:OnSend(data) 	
end

-- 消息说明: 留言列表响应 消息Id: 22130
function Modules.UnionModule:GuildLeavemsgListResp(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 留言列表响应 消息Id: 22130")
	self.leaveWord_list = info.leave_msg_list
	GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_GUESTBOOK)
end

-- 消息说明: 解散工会 消息Id: 22105
function Modules.UnionModule:GuildDismissReq(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 解散工会 消息Id: 22105")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_DISMISS_REQ)
	GlobalNetAdapter:OnSend(data) 	
end

-- 消息说明: 解散工会响应 消息Id: 22106
function Modules.UnionModule:GuildDismissResp(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 解散工会响应 消息Id: 22106")
	self.myUnionData = {}
	GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_dissolveTips___") )
end

-- 消息说明: 申请加入工会 消息Id: 22107
function Modules.UnionModule:GuildApplyReq(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 申请加入工会 消息Id: 22107")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_APPLY_REQ)
	data.guild_id = info.guild_id
	GlobalNetAdapter:OnSend(data) 	
end

-- 消息说明: 申请加入工会响应 消息Id: 22108
function Modules.UnionModule:GuildApplyResp(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 申请加入工会响应 消息Id: 22108")
	GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_JOIN , info.guild_id , true)
	GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_applyForWin") )
end

-- 消息说明: 审核申请 消息Id: 22109
function Modules.UnionModule:GuildApplyCheckReq(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 审核申请 消息Id: 22109")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_APPLY_CHECK_REQ)
	data.target_id = info.target_id
	data.is_accept = info.is_accept
	GlobalNetAdapter:OnSend(data) 	
end

-- 消息说明: 审核申请响应 消息Id: 22110
function Modules.UnionModule:GuildApplyCheckResp(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 审核申请响应 消息Id: 22110")
	local t1 = self.apply_list
	for i,v in ipairs(t1) do
		if v.role_id == info.target_id then
			table.remove(self.apply_list,i)
		end
	end
	
	if #self.apply_list == 0 then
		for i , v in ipairs( self.myUnionData.red_point_list ) do
			if v.type == Macros.Game.UNION_RED.AUDIT then
				table.remove( self.myUnionData.red_point_list , i )
				break
			end
		end
	end
	
	self.myUnionData.member_num = self.myUnionData.member_num + 1

	GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_APPLY)
	--刷新主界面
	GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_HALL_DATE_UP)
end

-- 消息说明: 取消申请 消息Id: 22111
function Modules.UnionModule:GuildApplyCancelReq(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 取消申请 消息Id: 22111")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_APPLY_CANCEL_REQ)
	data.guild_id = info.guild_id
	GlobalNetAdapter:OnSend(data) 	
end

-- 消息说明: 取消申请响应 消息Id: 22112
function Modules.UnionModule:GuildApplyCancelResp(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 取消申请响应 消息Id: 22112")
	GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_APPLY, info.guild_id , true )
end

-- 消息说明: 踢出公会 消息Id: 22113
function Modules.UnionModule:GuildKickoutReq(info)
	CPPGameLib.PrintTable("xy" , info,"踢出公会: 任命 消息Id: 22113")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_KICKOUT_REQ)
	data.guild_id = info.guild_id
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 踢出公会响应 消息Id: 22114
function Modules.UnionModule:GuildKickoutResp(info)
	CPPGameLib.PrintTable("xy" , info,"踢出公会响应: 任命 消息Id: 22114")
	local t1 = self.member_list
	for i,v in ipairs(t1) do
		if info.guild_id == v.role_id then
			table.remove( self.member_list , i )
		end
	end
	self.myUnionData.member_num = self.myUnionData.member_num - 1
	GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_MEMBER,self.member_list)
end

-- 消息说明: 退出工会 消息Id: 22115
function Modules.UnionModule:GuildLeaveReq(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 退出工会 消息Id: 22115")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_LEAVE_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 退出工会响应 消息Id: 22116
function Modules.UnionModule:GuildLeaveResp(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 退出工会响应 消息Id: 22116")
	GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_quitTips_") )
end

-- 消息说明: 留言 消息Id: 22119
function Modules.UnionModule:GuildLeaveEssageReq(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 留言 消息Id: 22119")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_LEAVEM_ESSAGE_REQ)
	data.leave_msg = info.leave_msg
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 留言响应 消息Id: 22120
function Modules.UnionModule:GuildLeaveEssageResp(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 留言响应 消息Id: 22120")
	table.insert(self.leaveWord_list, { time = info.time , text = info.text , name = info.name , pos = info.pos } )
	GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_GUESTBOOK)
end

-- 消息说明: 搜索工会 消息Id: 22121
function Modules.UnionModule:GuildSearchReq(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 搜索工会 消息Id: 22121")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_SEARCH_REQ)
	data.guild_id = info.guild_id or 0
	data.guild_name = info.guild_name
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 搜索工会响应 消息Id: 22122
function Modules.UnionModule:GuildSearchResp(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 搜索工会响应 消息Id: 22122")
	GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_JOIN , info.guild_list)

end

-- 消息说明: 泉涌 消息Id: 22123
function Modules.UnionModule:GuildQyReq(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 泉涌 消息Id: 22123")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_QY_REQ)
	data.type = info.type
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 泉涌响应 消息Id: 22124
function Modules.UnionModule:GuildQyResp(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 泉涌响应 消息Id: 22124")
	self.myUnionData.qy_type = info.type
	self.myUnionData.qy_exp = info.exp
	self.myUnionData.qy_contribution = info.contribution
	
	GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_MYUNION , info.type )
	GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_DATE)
end

-- 消息说明: 领取泉水奖励 消息Id: 22125
function Modules.UnionModule:GuildQsAwardReq(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 领取泉水奖励 消息Id: 22123")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_QS_AWARD_REQ)
	data.qs = info.qs
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 领取泉水奖励响应 消息Id: 22126
function Modules.UnionModule:GuildQsAwardResp(info)
	CPPGameLib.PrintTable("xy" , info , "消息说明: 领取泉水奖励响应 消息Id: 22124")
	self:SettleBoxType( info.qs )

	
	-- CPPGameLib.PrintTable("xy" , GameConfig.GuildQSReward , "GuildQSReward: 领取泉水奖励响应 消息Id: 22124")
	-- CPPGameLib.PrintTable("xy" , info , "消息说明: 领取泉水奖励响应 消息Id: 22124")
	local cnf = CPPGameLib.GetConfig("GuildQSReward", info.qs)
	CPPGameLib.PrintTable("xy" , cnf , "消息说明: 领取泉水奖励响应 消息Id: 22124------------------")

	local t1 = {}

	for _ , vv in ipairs(cnf.skill_list) do
		if vv.guild_level == self:GetUnionLevel() then
		    for i , v in ipairs( vv.item_id_list ) do
		        table.insert( t1 , { id = v , num = vv.item_num_list[i] } )
		    end
		end
	end
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

	GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_MYUNION)
end

-- 消息说明: 任命 消息Id: 22117
function Modules.UnionModule:GuildAppointReq(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 任命 消息Id: 22117")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_APPOINT_REQ)
	data.pos = info.pos
	data.target_id = info.target_id
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 任命响应 消息Id: 22118
function Modules.UnionModule:GuildAppointResp(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 任命响应 消息Id: 22118")
	if info.pos == Modules.UnionModule.CHAIRMAN then
		self:GuildDateReq()
		self:GuildMemberListReq()
	else
		for i,v in ipairs(self.member_list) do
			if info.target_id == v.role_id then
				v.position = info.pos
			end
		end
		GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_MEMBER,self.member_list)
	end
end

-- 消息说明: 工会弹劾请求 消息Id: 22155
function Modules.UnionModule:GuildImpeachReq( info )
	CPPGameLib.PrintTable("xy" , info,"工会弹劾请求 消息Id: 22155")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_IMPEACH_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 工会弹劾响应 消息Id: 22156
function Modules.UnionModule:GuildImpeachResp()
	CPPGameLib.PrintTable("xy" , info,"消息说明: 工会弹劾响应 消息Id: 22156")
	--弹劾
	GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_ACCUSE)
end

-- 消息说明: 修改工会信息请求 消息Id: 22135
-- "type__C",      --类型(1=图标,2=公告,3=宣言)
-- "icon_id__H",   --图标ID
-- "text__s",      --文本
function Modules.UnionModule:GuildInfoModifyReq(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 修改工会信息请求 消息Id: 22135")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_INFO_MODIFY_REQ)
	data.type = info.type
	data.icon_id = info.icon_id or 0
	data.text = info.text or "0"
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 修改工会信息响应 消息Id: 22136
function Modules.UnionModule:GuildInfoModifyResp(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 修改工会信息响应 消息Id: 22136")
	if info.type == Macros.Game.UNION.NOTICE then
		self.myUnionData.notice = info.text
	elseif info.type == Macros.Game.UNION.MANIFESTO then
		self.myUnionData.declaration = info.text
	elseif info.type == Macros.Game.UNION.ICON then
		self.myUnionData.icon_id = info.icon_id
	end
	
	GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_HALL_DATE_UP)
end

-- 消息说明: 工会事件列表请求 消息Id: 22137
function Modules.UnionModule:GuildEventListReq(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 工会事件列表请求 消息Id: 22137")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_EVENT_LIST_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 工会事件列表响应 消息Id: 22138
function Modules.UnionModule:GuildEventListResp(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 工会事件列表响应 消息Id: 22138")
	GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_TRENDS,info.event_list)
end

--消息说明：公会购买记录请求 消息id
function Modules.UnionModule:GetMyBuyRecordReq( type )
	CPPGameLib.PrintTable("xy" , type,"消息说明：公会购买记录请求")
	CallModuleFunc(ModuleType.SHOP,"ShopBuyRecordRequest",type)
end

--消息说明：公会购买记录响应 消息id
function Modules.UnionModule:HandleBuyResponse(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明：公会购买记录响应")
	local lst = {}
	lst.type = info.type
	for i,v in ipairs(info.buy_record_list) do
		lst.goods_id = v.goods_id
		lst.buy_num = v.buy_num
		self:BuyRecordDispose( lst )
	end

	self.buyRecordType = true
	if info.type == Macros.Game.SHOP.UNIONAWARDPROP then
		GlobalEventSystem:FireNextFrame( Macros.Event.UnionModule.UNION_BUY_RECORD )
	end
	
end

-- 消息说明: 限时商品列表请求 消息Id: 20907
function Modules.UnionModule:ShopLimitGoodsListReq(type)
	-- CPPGameLib.PrintTable("xy" , type,"限时商品列表请求")	
	local info = {}
	info.type = type
	CallModuleFunc(ModuleType.SHOP,"ShopLimitGoodsListReq",info)
end

-- 消息说明: 限时商品列表响应 消息Id: 20908
function Modules.UnionModule:ShopLimitGoodsListResp(info)
	CPPGameLib.PrintTable("xy" , info,"限时商品列表响应")
    self.shopLimit = info

    GlobalEventSystem:FireNextFrame( Macros.Event.UnionModule.UNION_DESENO )
end

-- 消息说明: 商店购买响应 消息Id: 
function Modules.UnionModule:UnionBuyResp(info)
	CPPGameLib.PrintTable("xy" , info,"商店购买响应")

    local lst = {}
    lst.type = info.type
	lst.goods_id = info.param1
	lst.buy_num = info.param2
	self:BuyRecordDispose( lst )

	local cnf = nil
	if info.type == Macros.Game.SHOP.UNIONPROP then
		cnf = CPPGameLib.GetConfig( "GuildItemShop", info.param1 )
	elseif info.type == Macros.Game.SHOP.UNIONTIMEPROP then
		cnf = CPPGameLib.GetConfig( "GuildLimitShop", info.param1 )
		
		for i , v in ipairs( self.shopLimit.goods_list ) do
			if v.goods_id == info.param1 then
				v.buy_num = v.buy_num + info.param2
			end
		end

	elseif info.type == Macros.Game.SHOP.UNIONAWARDPROP then	
		cnf = CPPGameLib.GetConfig( "GuildAwardShop", info.param1 )
	end

    local t1 = {}
    table.insert(t1,{id = cnf.item_id , num = cnf.item_num * info.param2 })
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

    GlobalEventSystem:FireNextFrame( Macros.Event.UnionModule.UNION_BUY , info.type )
end

-- 消息说明: 技能升级请求 消息Id: 22141
function Modules.UnionModule:GuildSkillLevelUpReq(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 技能升级请求 消息Id: 22141")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_SKILL_LEVEL_UP_REQ)
	-- 升级的类型(1=工会,2=个人)
	data.type = info.type
	data.skill_id = info.skill_id
	data.skill_level = info.skill_level
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 技能升级响应 消息Id: 22142
function Modules.UnionModule:GuildSkillLevelUpResp(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 技能升级响应 消息Id: 22142")
	local exp = 0
	if info.type == Macros.Game.UNION_SKILL.GUILD then

		self.guild_skill_list[ info.skill_id ] = info.skill_level
		local cnf = CPPGameLib.GetConfig("GuildSkill", info.skill_id )
		exp = cnf.skill_list[ info.skill_level].cost_guild_exp

	elseif info.type == Macros.Game.UNION_SKILL.PERSON then
		self.my_skill_list[ info.skill_id ] = info.skill_level

	end

	if info.skill_level == 1 then
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_killStudy") )
	else
		local cnf = CPPGameLib.GetConfig("GuildSkill", info.skill_id )
		cnf = cnf.skill_list[ info.skill_level]
		if info.type == Macros.Game.UNION_SKILL.GUILD then 
			GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_killUp_" , cnf.name ) )
		else
			GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_killUp" , cnf.name ) )
		end
	end

	self.myUnionData.exp = self.myUnionData.exp - exp
	GlobalEventSystem:FireNextFrame( Macros.Event.UnionModule.UNION_KILL , info.type )
end

-- 消息说明: 工会技能列表请求 消息Id: 22139
function Modules.UnionModule:GuildSkillListReq(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 工会技能列表请求 消息Id: 22139")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_SKILL_LIST_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 工会技能列表响应 消息Id: 22140
function Modules.UnionModule:GuildSkillListResp(info)
	self.my_skill_list = {}
	self.guild_skill_list = {}

	for i,v in ipairs(info.my_skill_list) do
		self.my_skill_list[v.skill_id] = v.skill_level
	end

	for i,v in ipairs(info.guild_skill_list) do
		self.guild_skill_list[v.skill_id] = v.skill_level
	end

	GlobalEventSystem:FireNextFrame( Macros.Event.UnionModule.UNION_KILL )
end

-- 消息说明: 请求工会排行榜列表 消息Id: 20809
function Modules.UnionModule:GuildRankListReq(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 请求工会排行榜列表 消息Id: 20809")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_RANK_LIST_REQ)
	data.rank_type = info.rank_type
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 请求工会排行榜列表响应 消息Id: 20810
function Modules.UnionModule:GuildRankListResp(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 请求工会排行榜列表响应 消息Id: 20810")
	self.unionLevelRankLst = {}
	self.myRank = {}
	if info.rank_type == 0 then
		self.unionLevelRankLst = info.rank_list
	end
	self.myRank = { rank_value = info.rank_value , rank = info.rank }
	
	GlobalEventSystem:FireNextFrame( Macros.Event.UnionModule.UNION_RANK )
end

-- 消息说明: 军团副本数据请求 消息Id: 22143
function Modules.UnionModule:GuildFbDataReq(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 军团副本数据请求 消息Id: 22143")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_FB_DATA_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 军团副本数据响应 消息Id: 22144
function Modules.UnionModule:GuildFbDataResp(info)
	CPPGameLib.PrintTable("xy" , info ,"消息说明: 军团副本数据响应 消息Id: 22144")
	self.fbData = info
	self:UnionReplyTime()

	GlobalEventSystem:FireNextFrame( Macros.Event.UnionModule.UNION_FB )
end

-- 消息说明: 工会关卡怪物数据请求 消息Id: 22157
function Modules.UnionModule:GuildMonsterDataReq(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 工会关卡怪物数据请求 消息Id: 22157")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_MONSTER_DATA_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 工会关卡怪物数据响应 消息Id: 22158
function Modules.UnionModule:GuildMonsterDataResp(info)
	-- CPPGameLib.PrintTable("xy" , info,"消息说明: 工会关卡怪物数据响应 消息Id: 22158")
	if not self.monsterData then
		self.monsterData = {}
		self.monsterData.camp_list = {}
	end
	self.monsterData.camp_list = info.camp_list
	self:MonsterHPSettle()
	GlobalEventSystem:FireNextFrame( Macros.Event.UnionModule.UNION_FB_DATA )
end

-- 消息说明: 军团副本章节奖励请求 消息Id: 22145
function Modules.UnionModule:GuildFbChapterRewardReq(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 军团副本章节奖励请求 消息Id: 22145")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_FB_CHAPTER_REWARD_REQ)
	data.chapter = info.chapter
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 军团副本章节奖励响应 消息Id: 22146
function Modules.UnionModule:GuildFbChapterRewardResp(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 军团副本章节奖励响应 消息Id: 22146")

	local fbCnf = CPPGameLib.GetConfig("GuildFB", info.chapter )
	local t1 = {}
    for i,v in ipairs(fbCnf.chapter_reward_list) do
        table.insert(t1,{id = v[1] , num = v[2]})
    end
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

	table.insert( self.fbData.take_chapter_reward_record , { chapter = info.chapter } )  
	
	GlobalEventSystem:FireNextFrame( Macros.Event.UnionModule.UNION_AWARD_GET )
end

-- 消息说明: 军团副本挑战次数购买 消息Id: 22153 
function Modules.UnionModule:GuildFbBattleTimesBuyReq(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 军团副本挑战次数购买 消息Id: 22153")
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_FB_BATTLE_TIMES_BUY_REQ)
	data.buy_times = info.buy_times
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 军团副本挑战次数购买响应 消息Id: 22154 
function Modules.UnionModule:GuildFbBattleTimesBuyResp(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 军团副本挑战次数购买响应 消息Id: 22154")
	-- self.fbData.buy_battle_times = self.fbData.buy_battle_times + info.buy_times 
	GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("shop_success") )
	GlobalEventSystem:FireNextFrame( Macros.Event.UnionModule.UNION_FB )
end

-- 消息说明: 军团副本关卡奖励请求 消息Id: 22147
function Modules.UnionModule:GuildFbMapRewardReq(info)
	CPPGameLib.PrintTable("xy" , info , "消息说明: 军团副本关卡奖励请求 消息Id: 22147" )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_FB_MAP_REWARD_REQ)
	data.chapter = info.chapter
	data.camp = info.camp - 1 
	data.reward_idx = info.reward_idx - 1 
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 军团副本关卡奖励响应 消息Id: 22148
-- "chapter__H",		--请求的章节
-- "camp__C",		--请求的阵营(从0开始,在界面的位置)
-- "box_idx__C",		--领取的宝箱索引(从0开始,在界面的位置)
-- "reward_idx__C",	--领取的奖励索引(从0开始,在表里的位置)
function Modules.UnionModule:GuildFbMapRewardResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 军团副本关卡奖励响应 消息Id: 22148" )
	local fbCnf = CPPGameLib.GetConfig("GuildFB", info.chapter )
    local name = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo").role_name

	local data = {}
	data.chapter = info.chapter
	data.camp = info.camp
	data.take_box_record_list = { box_idx = info.box_idx , reward_idx = info.reward_idx , role_name = name }
	self:SettleRecordList( data , true )

	local t1 = {}
    table.insert(t1,{id = fbCnf.map_reward_id_list[ info.reward_idx + 1 ] , num = fbCnf.map_reward_num_list[ info.reward_idx + 1 ] })
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1 )

    info.box_idx = info.box_idx + 1
    info.role_name = name

    if not self.myMapReward[ info.chapter ] then
    	self.myMapReward[ info.chapter ] = {}
    	for i = 1 , 3 do
    		self.myMapReward[ info.chapter ][i] = {}
    		self.myMapReward[ info.chapter ][i] = { camp = i - 1 , is_take_reward = 0 }
    	end
    end
	self.myMapReward[ info.chapter ][info.camp + 1] = { camp = info.camp , is_take_reward = 1 }

	GlobalEventSystem:FireNextFrame( Macros.Event.UnionModule.UNION_GET_RECORD , info )
end

-- 消息说明: 军团副本关卡奖励记录请求 消息Id: 22149
function Modules.UnionModule:GuildFbMapRewardRecordReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 军团副本关卡奖励记录请求 消息Id: 22149" )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_FB_MAP_REWARD_RECORD_REQ)
	data.chapter = info.chapter
	data.camp = info.camp
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 军团副本关卡奖励记录响应 消息Id: 22150
-- "chapter__H",		--请求的章节
-- "camp__C",		--请求的阵营
-- box_idx@H: 宝箱索引(在界面上的位置)
-- reward_idx@H : 奖励索引(在配置表中的位置)
-- ##role_name@s : 领取者名称
function Modules.UnionModule:GuildFbMapRewardRecordResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 军团副本关卡奖励记录响应 消息Id: 22150" )
	-- self.record_list = info.take_box_record_list
	-- self.record_camp = info.camp
	self.begItemBoxType = true
	self:SettleRecordList( info )
	self:SettleItemBoxRed()
	GlobalEventSystem:FireNextFrame( Macros.Event.UnionModule.UNION_REWARD_RECORD , info )
end

-- 消息说明: 军团副本伤害记录请求 消息Id: 22151
function Modules.UnionModule:GuildFbDamageRecordReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 军团副本伤害记录请求 消息Id: 22151" )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_FB_DAMAGE_RECORD_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 军团副本伤害记录响应 消息Id: 22152
-- 列表字段说明:
-- head_icon@H: 头像
-- damage@I: 伤害
-- battle_times@C: 战斗次数
-- role_name@s: 角色名称
function Modules.UnionModule:GuildFbDamageRecordResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 军团副本伤害记录响应 消息Id: 22152" )
	local lst = {}
	table.sort( info.damage_list, function ( a , b )
		return a.damage > b.damage
	end )

	for i,v in ipairs(info.damage_list) do
		v.rank = i
		table.insert( lst , v )
	end
	GlobalEventSystem:FireNextFrame( Macros.Event.UnionModule.UNION_HARM_RECORD , lst )
end


-- 消息说明: 工会副本章节重置 消息Id: 22159
function Modules.UnionModule:GuildFbChapterResetReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 工会副本章节重置 消息Id: 22159" )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_FB_CHAPTER_RESET_REQ)
	data.is_history_record = info.is_history_record
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 工会副本章节重置响应 消息Id: 22160
function Modules.UnionModule:GuildFbChapterResetResp( info )
	-- CPPGameLib.PrintTable("xy" , info , "消息说明: 工会副本章节重置响应 消息Id: 22160" )
	self.fbData.is_from_history_chapter = info.is_history_record
	GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_historyChangeSucceed") )
end

-- 消息说明: 工会怪物伤害通知 消息Id: 22161
            -- "chapter__H",       --所属的章节
            -- "camp__C",      --所属的阵营
            -- "damage__I",        --伤害
            -- "cur_total_hp__I",  --当前HP
            -- "attack_name__s",   --攻击者名称
function Modules.UnionModule:GuildMonsnterDamageNotify( info )
	-- CPPGameLib.PrintTable("xy" , info , "消息说明: 工会怪物伤害通知 消息Id: 22161" )
	if self.monsterData then
		local n1 = self.monsterData.nowSchedule
		--更新现在吃了的伤害（章节）
		self.monsterData.nowSchedule = self.monsterData.nowSchedule + info.damage
		--更新关卡吃了的伤害（关卡）
		CPPGameLib.PrintTable("xy" , self.monsterData.levelData , "self.monsterData.levelData" )
		self.monsterData.levelData[info.camp + 1].damage = self.monsterData.levelData[info.camp + 1].damage + info.damage 

		if info.cur_total_hp <= 0 then
			self.monsterData.levelData[info.camp + 1].killer_name = info.attack_name
		end
		GlobalEventSystem:FireNextFrame( Macros.Event.UnionModule.UNION_MURBER )
	end
end


-- 消息说明: 我的关卡奖励记录请求 消息Id: 22162
function Modules.UnionModule:GuildMyMapRewardRecordReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 我的关卡奖励记录请求 消息Id: 22162" )
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_GUILD_MY_MAP_REWARD_RECORD_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 我的关卡奖励记录响应 消息Id: 22163
function Modules.UnionModule:GuildMyMapRewardRecordResp( info )
	-- CPPGameLib.PrintTable("xy" , info , "消息说明: 我的关卡奖励记录响应 消息Id: 22163" )
	self.myMapReward = {}	

	for i,v in ipairs( info.reward_record_list ) do
		local t1 = {}
		for ii,vv in ipairs( v.camp_list.__list ) do
			t1[ii] = vv
		end
		self.myMapReward[ v.chapter ] = t1
	end

	GlobalEventSystem:FireNextFrame( Macros.Event.UnionModule.UNION_FB_AWARD )
end















-- 消息说明: 工会弹劾通知 消息Id: 22164
function Modules.UnionModule:GuildImpeachNotice( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明: 工会弹劾通知 消息Id: 22164" )
	--刷新一次公会数据 优化方式是不是可以把id改成名字然后由我本地发事件刷新页面
	self:GuildDateReq()
	--刷新一次成员列表
	self:GuildMemberListReq()

	GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_accuse_") )

	GlobalEventSystem:FireNextFrame(Macros.Event.UnionModule.UNION_HALL_DATE_UP)
end



---------------------------------------------------------------------------------------------------------------------------
--公会副本奖励
---------------------------------------------------------------------------------------------------------------------------
--按照章节拿到该章节的奖励状态
function Modules.UnionModule:GetMyMapReward( id )
	if self.myMapReward then
		return self.myMapReward[ id ]
	end
end





---------------------------------------------------------------------------------------------------------------------------
--公会大厅
---------------------------------------------------------------------------------------------------------------------------

--获得公会数据
function Modules.UnionModule:UnionData()
	if self.myUnionData.guild_id and self.myUnionData.guild_id ~= 0 then
		return self.myUnionData
	end
end

--获得公会名称
function Modules.UnionModule:GetUnionName()
	return self.myUnionData.guild_name
end

--获得公会图标
function Modules.UnionModule:GetUnionIcon()
	return self.myUnionData.icon_id
end

--获得公会公告
function Modules.UnionModule:GetUnionNotice()
	return self.myUnionData.notice
end

--获得公会宣言
function Modules.UnionModule:GetUnionDeclaration()
	return self.myUnionData.declaration
end

--获得公会等级
function Modules.UnionModule:GetUnionLevel()
	return self.myUnionData.level
end

--获得公会经验
function Modules.UnionModule:GetUnionExp()
	local cnf = CPPGameLib.GetConfig("Guild", self:GetUnionLevel() , false )
	if not cnf then
		cnf = CPPGameLib.GetConfig("Guild",   #CPPGameLib.GetConfig("Guild", nil , nil , true ) , false )
	end
	return self.myUnionData.exp , cnf.need_exp
end

--获得公会会长
function Modules.UnionModule:GetUnionMaster()
	return self.myUnionData.leader_name
end

--获得公会成员数量
function Modules.UnionModule:GetUnionMemberNum( ... )
	local cnf = CPPGameLib.GetConfig("Guild", self:GetUnionLevel() , false )
	if not cnf then
		cnf = CPPGameLib.GetConfig("Guild",   #CPPGameLib.GetConfig("Guild", nil , nil , true ) , false )
	end
	return self.myUnionData.member_num , cnf.member_max
end

--获得我的阶级
function Modules.UnionModule:GetMyPosition()
	return self.myUnionData.position
end

--获得我的周活跃
function Modules.UnionModule:GetMyActive()
	return self.myUnionData.active
end

---------------------------------------------------------------------------------------------------------------------------
--公会排行榜
---------------------------------------------------------------------------------------------------------------------------

--获得等级排行榜
function Modules.UnionModule:GetUnionLevelRank( ... )

	for i,v in ipairs(self.unionLevelRankLst) do
		v.rank = i
	end
	return self.unionLevelRankLst
end

--获得我的排行榜消息
function Modules.UnionModule:GetUnionMyRankInfo( ... )
	return self.myRank
end

---------------------------------------------------------------------------------------------------------------------------
--公会商店
---------------------------------------------------------------------------------------------------------------------------

--获得道具商店
function Modules.UnionModule:GetPropShop()
	local cnf = CPPGameLib.GetConfig("GuildItemShop", nil, nil , true )
	return self:ShoplLstSort( cnf , Macros.Game.SHOP.UNIONPROP )
end

--获得限时商店
function Modules.UnionModule:GetDesenoShop()
	if self.shopLimit then
		local t1 = {}
		for i,v in ipairs(self.shopLimit.goods_list) do
			local cnf = CPPGameLib.CopyTbl( CPPGameLib.GetConfig("GuildLimitShop", v.goods_id ) )
			if cnf then
				cnf.buy_num = v.buy_num
				t1[cnf.goods_id] = cnf
			end
		end
		return self:ShoplLstSort( t1 , Macros.Game.SHOP.UNIONTIMEPROP )
	end
end

--获得奖励商店
function Modules.UnionModule:GetAwardShop()
	local cnf = CPPGameLib.GetConfig("GuildAwardShop", nil, nil , true )
	return self:ShoplLstSort( cnf , Macros.Game.SHOP.UNIONAWARDPROP )
end

--获得限时倒计时
function Modules.UnionModule:GetDesenoTime()
	local serverTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	-- --当前服务器时间
	local year, month, day, hour, minute, second, week = TimeTool.GetDateFromTimeStamp(serverTime)
	local time = 0

	local lst = ConfigAdapter.Common.GetUnionLimitTimeUp()
	for i , v in ipairs( lst ) do
		v = v .. "00"
		local nTime = hour..minute..second
		if tonumber(v) >= tonumber(nTime) then

			local h = string.sub( string.format( "%04d" , v ) , 0 , 2 )
			local m = string.sub( string.format( "%04d" , v ) , 3 , 4 )

			h = h * 60 * 60
			m = m * 60

			time = h + m
			time = time - ( ( hour * 60 * 60 ) + ( minute * 60 ) + second )
			break
		end
	end

	if time == 0 then
		for i , v in ipairs( lst ) do
			v = v .. "00"
			local nTime = hour..minute..second
			if tonumber(v) <= tonumber(nTime) then

				local h = string.sub( string.format( "%04d" , v ) , 0 , 2 )
				local m = string.sub( string.format( "%04d" , v ) , 3 , 4 )

				h = h * 60 * 60
				m = m * 60

				time = h + m + ( 24 * 60 * 60 )

				time = time - ( ( hour * 60 * 60 ) + ( minute * 60 ) + second )

				break
			end
		end	
	end

	return time + serverTime
end

--获得全部的购买记录
function Modules.UnionModule:GetBuyRecord()
	return self.buyRecord
end

--获得我购买记录请求状态
function Modules.UnionModule:GetBuyRecordType()
	return self.buyRecordType
end

---------------------------------------------------------------------------------------------------------------------------
--公会建设
---------------------------------------------------------------------------------------------------------------------------

--获得最近是谁建设的类型
function Modules.UnionModule:GetUnionLastType( ... )
	return self.myUnionData.last_qy_type
end

--获得最近是谁建设的值
function Modules.UnionModule:GetUnionLastValue( ... )
	return self.myUnionData.last_qy_value
end

--获得最近是谁建设的名字
function Modules.UnionModule:GetUnionLastName( ... )
	return self.myUnionData.last_qy_target_name
end

--获得今天选择的建设类型
function Modules.UnionModule:GetUnionType()
	return self.myUnionData.qy_type
end

--获得今天获得的获得工会经验
function Modules.UnionModule:GetUnionQyExp()
	return self.myUnionData.qy_exp
end

--获得今天获得的获得工会贡献值
function Modules.UnionModule:GetUnionQyContribution()
	return self.myUnionData.qy_contribution
end

--获得今天建设的进度
function Modules.UnionModule:GetUnionSchedule()
	return self.myUnionData.quan_shui
end

--获得今天建设了的人数
function Modules.UnionModule:GetUnionScheduleNum()
	return self.myUnionData.qy_total_times
end

--获得今天领取了的箱子
function Modules.UnionModule:GetBoxType( id )
	return self.awardRecord[ id ]
end

--整理领取记录
function Modules.UnionModule:SettleBoxType( data )
	self.awardRecord[ data ] = true
end


---------------------------------------------------------------------------------------------------------------------------
--公会留言
---------------------------------------------------------------------------------------------------------------------------

--获得留言列表
function Modules.UnionModule:GetLeaveWordList( )
	table.sort( self.leaveWord_list, function ( a , b )
		return a.time > b.time
	end )
	return self.leaveWord_list
end

---------------------------------------------------------------------------------------------------------------------------
--公会成员
---------------------------------------------------------------------------------------------------------------------------

--获得申请成员
function Modules.UnionModule:GetUnionApplyList( ... )
	return self.apply_list
end

--获得公会成员列表
function Modules.UnionModule:GetUnionMemberLst( ... )
	table.sort( self.member_list, function ( a , b )
		return a.position < b.position
	end )

	return self.member_list
end

--获得公会成员信息
function Modules.UnionModule:ToIdGetUnionMemberInfo( id )
	return self.member_list_[id]
end

--获得我对目标的权限
function Modules.UnionModule:GetMyJurisdiction( data )
	if data.position > 3 then
		data.position = 3
	end 
	local cnf = CPPGameLib.CopyTbl( CPPGameLib.GetConfig( "GuildPrivilege", self:GetMyPosition() ) )
	
	local role_id = CallModuleFunc(ModuleType.ROLE, "GetRoleID") 
	if data.position > self:GetMyPosition() then
		if data.role_id == role_id and data.position ~= Modules.UnionModule.CHAIRMAN then
			cnf.leave = 1
		else
			cnf.leave = 0
		end
	else
		for i,v in pairs(cnf) do
			cnf[i] = 0
		end
		if data.role_id == role_id and data.position ~= Modules.UnionModule.CHAIRMAN then
			cnf.leave = 1
		else
			cnf.leave = 0
		end	
	end


	local _ , num = TimeTool.OfflineTime( data.offline_time )
	if data.position == 1 then
		if num > ConfigAdapter.Common.GetUnionTreasonTime() then
			cnf.accuse = 1
		end
	else
		cnf.accuse = 0
	end

	return cnf
end

--获得我对目标的任命类型
function Modules.UnionModule:GetMyAppointType(data)

	local appoint = {}
	appoint.accuse = false
	appoint.recall = false
	appoint.appoint_assist = false
	appoint.transfer = false

	if data.position > 3 then
		data.position = 3
	end 

	local role_id = CallModuleFunc(ModuleType.ROLE, "GetRoleID") 
	if data.position > self:GetMyPosition() then
		local paCnf = CPPGameLib.GetConfig( "GuildPrivilege", data.position)

		--可被弹劾
		if paCnf.accuse == 1 then
			appoint.accuse = true
		end

		--可被罢免
		if paCnf.recall == 1 then
			appoint.recall = true
		end

		--可被任命
		if paCnf.appoint_assist == 0 and data.position ~= Modules.UnionModule.VICE_CHAIRMAN  then
			appoint.appoint_assist = true
		end

		--可被转移会长
		if data.position == Modules.UnionModule.VICE_CHAIRMAN then 
			appoint.transfer = true
		end

	end

	return appoint
end

--获得我的权限
function Modules.UnionModule:GetMyPermission( type )
	local cnf = CPPGameLib.GetConfig( "GuildPrivilege", self:GetMyPosition() )
	if Macros.Game.UNION.ICON == type then
		return cnf.level

	elseif Macros.Game.UNION.NOTICE == type then
		return cnf.level

	elseif Macros.Game.UNION.MANIFESTO == type then
		return cnf.inside_msg

	elseif Macros.Game.UNION.DISSOLVE == type then
		return cnf.dismiss

	elseif Macros.Game.UNION.APPLY == type then
		return cnf.apply_check

	elseif Macros.Game.UNION.SKILL == type then
		return cnf.skill

	elseif Macros.Game.UNION.RESET == type then
		return cnf.reset

	elseif Macros.Game.UNION.DECLARE_WAR == type then
		return cnf.declare_war

	end
end


---------------------------------------------------------------------------------------------------------------------------
--公会技能
---------------------------------------------------------------------------------------------------------------------------

--获得公会技能列表
function Modules.UnionModule:GetUnionSkillLst( ... )
	if not self.guild_skill_list then
		self.guild_skill_list = {}
	end
	return self.guild_skill_list
end

function Modules.UnionModule:GetSkillLst( id , level )
	local cnf =  CPPGameLib.GetConfig( "GuildSkill" , id , false ) 
	return cnf.skill_list[level] , cnf.skill_list[level + 1]
end

function Modules.UnionModule:GetPersonSkill( id )
	if not self.my_skill_list or not self.my_skill_list[ id ] then
		return 0
	end
	return self.my_skill_list[ id ]
end

function Modules.UnionModule:GetGuildSkill( id )
	if not self.guild_skill_list or not self.guild_skill_list[ id ] then
		return 0
	end
	return self.guild_skill_list[ id ]
end

--公会技能次数
function Modules.UnionModule:GetSkillLstTime()
	local lst = self:GetUnionKillCnf()
	local index = 0
	for i,v in ipairs(lst) do
		local skillLevel = self:GetGuildSkill( v.id ) 
		local nowCnf , nexCnf = self:GetSkillLst( v.id , skillLevel )
		local level = self:GetUnionLevel()

		--公会经验
		local exp = self:GetUnionExp()
		if exp >= nexCnf.cost_guild_exp and level >= nexCnf.need_guild_level then
			index = index + 1
		end
	end
	print("xy" ,  "index=========,1" , index)
	return index
end

--个人技能次数
function Modules.UnionModule:GetPersonSkillTime()
	local lst = self:GetUnionPersonKillCnf()
	local index = 0
	for i,v in ipairs(lst) do
		local personLevel = self:GetPersonSkill( v.id )
		local guildLevel = self:GetGuildSkill( v.id )  
		local nowCnf , nexCnf = self:GetSkillLst( v.id , personLevel )
		local num = CallModuleFunc(ModuleType.ROLE,"GetPropValue", nexCnf.cost_type_list[1] )
		if num >= nexCnf.cost_value_list[1] and personLevel < guildLevel then
			index = index + 1
		end
	end
	return index
end

---------------------------------------------------------------------------------------------------------------------------
--公会副本
---------------------------------------------------------------------------------------------------------------------------
--获得当前可攻打的章节
function Modules.UnionModule:GetNowAttackSection()
	return self.fbData.curr_pass_chapter + 1
end

--今天开始的章节
function Modules.UnionModule:GetTodayStartSection()
	return self.fbData.today_start_chapter + 1 
end

--获得外面的血量
function Modules.UnionModule:GetOutsideHP()
	if self.fbData.damage ~= 0 or self.fbData.total_hp ~= 0 then
		local hp = self.fbData.damage / self.fbData.total_hp * 100
		return  math.floor( hp )  , 100
	else
		return 100 , 100
	end
end

--获得怪物总血量
function Modules.UnionModule:GetMonsterSumHP()
	return self.monsterData.nowSchedule , self.monsterData.maxSchedule
end

--获得怪物阵营信息
function Modules.UnionModule:GetMonsterData( id )
	if not self.monsterData then
		return
	end
	return self.monsterData.levelData[id]
end

--获得可领取奖励
function Modules.UnionModule:GetMayGetAwardRed( )
	local cnf = CallModuleFunc(ModuleType.UNION , "GetUnionTranscriptAwardLst")
	local section = CallModuleFunc(ModuleType.UNION , "GetNowAttackSection")
	local num = 0
	for i , v in ipairs( cnf ) do
		local awardlst = CallModuleFunc(ModuleType.UNION , "GetAwardGetRecord" , v.id )
		if v.id >= section then

		elseif awardlst then

		else
			num = num + 1
		end
	end
	return num
end




--获得今日挑战次数
function Modules.UnionModule:GetNowChallengeTime()
	return ConfigAdapter.Common.GetUnionDefaultChallengeTime() + self.fbData.buy_battle_times - self.fbData.battle_times
end

--挑战次数回复时间(分钟)
function Modules.UnionModule:GetNowChallengeReplyTime()
	return self.fbData.battle_times_last_recover_time + (ConfigAdapter.Common.GetUnionDefaultChallengeCD() * 60 )
end

--公会购买次数
function Modules.UnionModule:GetUnionUsedUpBuyTime()
	return self.fbData.buy_battle_times
end

--获得领取记录
function Modules.UnionModule:GetAwardGetRecord( id )
	local t1 = {}
	for k,v in pairs( self.fbData.take_chapter_reward_record ) do
		t1[v.chapter] = true
	end
	return t1[id]
end

--获得开启状态
function Modules.UnionModule:GetStartTime()
	local time = ConfigAdapter.Common.GetUnionFbStartAndEndTime()
	local startTime = time[1]
	local endTime = time[2]

	--开始时
	local sHour = string.sub(startTime , 0 , 2)
	--开始分
	local sMinute = string.sub(startTime , -2)

	--结束时
	local oHour = string.sub(endTime , 0 , 2)
	--结束分
	local oMinute = string.sub(endTime , -2)	

	local nowTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	local nYear, nMonth, nDay, nHour, nMinute, nSecond, nWeek = TimeTool.GetDateFromTimeStamp(nowTime)

	--进行中
	local type_ = false 
	--如果开始时少于等于现在的时 而且结束时大于现在 
	if sHour < nHour and oHour > nHour then
		type_ = true
	--如果开始时等于现在时
	elseif sHour == nHour then
		if sMinute <= nMinute then
			type_ = true
		end
	--如果结束时等于现在时
	elseif oHour == nHour then
		if oMinute > nMinute then
			type_ = true
		end
	end
	return type_ , sHour..":"..sMinute , oHour ..":"..oMinute
end

--当前可购买次数
function Modules.UnionModule:GetUnionTranscriptBuyTime()
	local default = CallModuleFunc(ModuleType.VIP, "GetPrivilege",Macros.Game.VIPType.UNION_TRANSCRIPT_BUY_TIME)
	default = default - self.fbData.buy_battle_times
	return default
end

--选中章节有没有领取过奖励
function Modules.UnionModule:GetUnionTranscriptSectionDraw( chapter , camp )

	local t1 = {}
	if self.record_list and self.record_list[chapter] and self.record_list[chapter][camp] then
		for i,v in ipairs(self.record_list[chapter][camp]) do
			t1[v.role_name] = v
		end
	end

	local isGet = ""
	--现在可以攻打的章节
	local section = self:GetNowAttackSection()
	--如果可以攻打的章节大于现在传进来的章节 证明是已经通的
	if section == chapter then
		--有记录
		local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")

		if t1[role_info.role_name] then
			isGet = "linquguo"
		elseif not self:GetMonsterData(camp) or (  self:GetMonsterData(camp) and  (self:GetMonsterData(camp).max - self:GetMonsterData(camp).damage) ) == 0 then
			isGet = "xing"
		elseif self:GetMonsterData(camp) and ( self:GetMonsterData(camp).max - self:GetMonsterData(camp).damage ) ~= 0 then
			isGet = "buxing"
		end
	elseif section > chapter then
		local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")
		if t1[role_info.role_name] then
			isGet = "linquguo"
		else
			isGet = "xing"
		end
	end

	return isGet
end

--重置的选中类型
function Modules.UnionModule:GetUnionTranscriptResettingType()
	return self.fbData.is_from_history_chapter
end




---------------------------------------------------------------------------------------------------------------------------
--数据处理
---------------------------------------------------------------------------------------------------------------------------

--购买记录处理
function Modules.UnionModule:BuyRecordDispose( info )
	if self.buyRecord[ info.type ] then 
		if self.buyRecord[ info.type ][ info.goods_id ] then
			self.buyRecord[ info.type ][ info.goods_id ] = self.buyRecord[ info.type ][ info.goods_id ] + info.buy_num 
		else
			self.buyRecord[ info.type ][ info.goods_id ] = info.buy_num
		end
	else
		self.buyRecord[ info.type ] = {}
		self.buyRecord[ info.type ][ info.goods_id ] = info.buy_num
	end	
end

--商店列表排序
function Modules.UnionModule:ShoplLstSort( lst , type_ )
	local t1 = {}
	local t2 = {}
	local t3 = {}
	for id , v in pairs( lst ) do
		v.buyRecord = 0
		v.type = type_

		if self.buyRecord[ type_ ] and self.buyRecord[ type_ ][ id ] then
			v.buyRecord = self.buyRecord[ type_ ][ id ]
			if v.buy_num_max ~= 0 and v.buy_num_max <= self.buyRecord[ type_ ][ id ] then
				table.insert( t1 , v )
			else
				table.insert( t2 , v )
			end
		else
			local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)

			if v.need_role_level > role_level or v.need_guild_level > self:GetUnionLevel() then 
				table.insert( t1 , v )
			-- elseif v.need_guild_level > self:GetUnionLevel() then
				-- table.insert( t3 , v )
			else
				table.insert( t2 , v )
			end
		end

	end

	table.sort( t1, function ( a , b )
		return a.goods_id < b.goods_id
	end )
	table.sort( t2, function ( a , b )
		return a.goods_id < b.goods_id
	end )
	-- table.sort( t3, function ( a , b )
	-- 	return a.goods_id < b.goods_id
	-- end )

	for i,v in ipairs(t1) do
		table.insert( t2 , v )
	end

	-- for i,v in ipairs(t3) do
	-- 	table.insert( t2 , v )
	-- end	

	return t2
end


--获得公会奖励可领取
function Modules.UnionModule:GetAwardTime( lst , type_ )
	local lst = self:GetAwardShop()

    local level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
    local unionLevel = CallModuleFunc(ModuleType.UNION,"GetUnionLevel")
    local index = 0
    for i,v in ipairs(lst) do
	    if v.need_role_level <= level and v.need_guild_level <= unionLevel and v.buy_num_max > v.buyRecord then
	    	index = index + 1 
	    end
    end
    return index
end

--获得建设表
function Modules.UnionModule:GetUnionScheduleCnf()
    local t1 = {}
	for i,v in pairs( CPPGameLib.CopyTbl( CPPGameLib.GetConfig("GuildQSReward", nil, nil , true) ) ) do
		for _ , vv in ipairs( v.skill_list ) do
			if vv.guild_level == self:GetUnionLevel() then
				vv.qs = i
				table.insert( t1 , vv )
			end
		end
	end
	table.sort( t1, function ( a , b )
		return a.qs < b.qs
	end )
	return t1
end

--获得可以学习的技能表
function Modules.UnionModule:GetUnionKillCnf( ... )
	local t1 = {}
	for i = 1 , 10 do
		local A = CPPGameLib.GetConfig( "GuildSkill" , i , false )
		if A then
			local cnf = A.skill_list[1]
			cnf.id = i
			cnf.type = Macros.Game.UNION_SKILL.GUILD
			table.insert( t1 , cnf )
		end
	end
	return t1
end

--个人的技能表
function Modules.UnionModule:GetUnionPersonKillCnf( ... )
	local t1 = {}
	local lst = CallModuleFunc(ModuleType.UNION , "GetUnionSkillLst") 
	for k,v in pairs(CPPGameLib.GetConfig("GuildSkill", nil, nil , true )) do
		local cnf_ = nil
		if lst[k] then
			cnf_ = v.skill_list[ lst[k] ]
			cnf_.show = true
		else
			cnf_ = v.skill_list[ 1 ]
			cnf_.show = false
		end
		cnf_.id = k
		cnf_.type = Macros.Game.UNION_SKILL.GUILD

		table.insert( t1 , cnf_ )
	end
    table.sort( t1, function ( a , b )
    	return a.id < b.id
    end )
    return t1
end


--获得红点列表
function Modules.UnionModule:GetUnionRedLst( ... )
	local t1 = {}
	--红点记录
	for k , v in pairs(self.myUnionData.red_point_list) do
		t1[v.type] = {}
		t1[v.type].type = 1
	end

	--建设的特殊的处理
	if self:GetUnionType() == 0 then
		t1[ Macros.Game.UNION_RED.CONSTRUCTION ] = {}
		t1[ Macros.Game.UNION_RED.CONSTRUCTION ].type = 1
	else
		t1[ Macros.Game.UNION_RED.CONSTRUCTION ] = nil
	end

	--奖励的特殊处理
	local index = self:GetAwardTime()
	local type_ = CallModuleFunc(ModuleType.UNION, "GetBuyRecordType" )
	if type_ then
		if index ~= 0 then
			t1[ Macros.Game.UNION_RED.AWARD ] = {}
			t1[ Macros.Game.UNION_RED.AWARD ].type = 1
		else
			t1[ Macros.Game.UNION_RED.AWARD ] = nil
		end
	end

	return t1 , self.myUnionData.red_point_list
end

--公会副本
function Modules.UnionModule:GetUnionTranscriptCnf( ... )
	local t1 = {}
	for i,v in ipairs( CPPGameLib.GetConfig("GuildFB", nil, nil , true ) ) do
		table.insert( t1 , {id = i} )
	end

	return t1
end

--公会奖励
function Modules.UnionModule:GetUnionTranscriptAwardLst( ... )
	local lst = self:GetUnionTranscriptCnf()
	local t1 = {}
	local t2 = {}
	for i,v in ipairs(lst) do

		local fb_cnf = CPPGameLib.GetConfig("GuildFB", v.id)

		local info = {}
		info.id = v.id
		info.name = fb_cnf.name
		info.chapter_reward_list = fb_cnf.chapter_reward_list 
		if self:GetAwardGetRecord(i) then
			table.insert( t1 , info )
		else
			table.insert( t2 , info )
		end
	end

	for i,v in ipairs(t1) do
		table.insert( t2 , v )
	end
	return t2
end

--公会副本回复时间计算
function Modules.UnionModule:UnionReplyTime( ... )
	
	local endTime = self:GetNowChallengeReplyTime()
	local nowNext = self:GetNowChallengeTime() 
	local maxNeXT = ConfigAdapter.Common.GetUnionDefaultChallengeTime()
	if nowNext >= maxNeXT then
		print( "xy" , "满的"  )
	else
		if self.replyTime then
			GlobalTimerQuest:CancelQuest(self.replyTime)
			self.replyTime = nil
		end
		self.replyTime = GlobalTimerQuest:AddPeriodQuest(
		    GlobalCallbackMgr:AddCallBackFunc(function()
		    	--回复时间
		    	local staminaTime = endTime - CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
		        if staminaTime == 0 then
		        	GlobalTimerQuest:CancelQuest(self.replyTime)
		            self.replyTime = nil

		            self:GuildFbDataReq()
		        else
		        	-- print("xy" , "GuildFbDataReq" , staminaTime )
		        end
		end),1,-1)
	end
end

--获得本地表的关卡怪物血量
function Modules.UnionModule:GetToSectionHp( id )
	local fbCnf = CPPGameLib.GetConfig("GuildFB", id)
	local max = 0
	for i,v in ipairs( fbCnf.gamelevel_list ) do
		local lst = ConfigAdapter.GameLevel.GetMonsterList( v )
		for i,v in ipairs(lst) do
			local monsterCnf = CPPGameLib.GetConfig("Monster", v.monster_id)
			max = max + monsterCnf.blood_max
		end
	end
	return max
end

--整理本地宝箱
function Modules.UnionModule:SettleRecordList( info , follow )
	if not self.record_list then
		self.record_list = {}
	end
	if not self.record_list[info.chapter] then
		self.record_list[info.chapter] = {}
	end
	if not self.record_list[info.chapter][ info.camp + 1] then
		self.record_list[info.chapter][ info.camp + 1 ] = {}
	end
	if follow then
		table.insert( self.record_list[info.chapter][ info.camp + 1 ] , info.take_box_record_list )
	else
		self.record_list[info.chapter][ info.camp + 1 ] = info.take_box_record_list
	end

	CPPGameLib.PrintTable("xy" , self.record_list[info.chapter] ,"self.record_list[info.chapter][ info.camp ]")
end

--获得本地宝箱
function Modules.UnionModule:GetSettleRecordList( chapter , camp )
	local t1 = {}
	if self.record_list and self.record_list[chapter] and self.record_list[chapter][camp] then
		for i,v in ipairs( self.record_list[chapter][camp] ) do
			t1[v.box_idx + 1 ] = v
		end
	end
	CPPGameLib.PrintTable("xy" , chapter ,"A: 工会技能列表响应 消息Id: 22140")

	local fbCnf = CPPGameLib.GetConfig("GuildFB", chapter )
	local lst = {}
	for i,v in ipairs( fbCnf.map_reward_id_list ) do
		if t1[i] then
			table.insert( lst , { 
									id = fbCnf.map_reward_id_list[t1[i].reward_idx + 1 ] , 
									num = fbCnf.map_reward_num_list[t1[i].reward_idx + 1 ] , 
									box_idx = t1[i].box_idx + 1 , 
									reward_idx = t1[i].reward_idx+ 1 , 
									role_name = t1[i].role_name ,
									section = chapter , 
									level = camp , 
									index = i ,
									} )
		else
			table.insert( lst , { id = v , num = fbCnf.map_reward_num_list[i] , section = chapter , level = camp , index = i } )
		end
	end


	return lst
end

--获得初次请求宝箱的标志
function Modules.UnionModule:GetInitialBegItemBox()
	return self.begItemBoxType
end

--整理宝箱的红点
function Modules.UnionModule:SettleItemBoxRed()

	if self:GetInitialBegItemBox() == true then
		self.begItemBoxType = "end"
	else
		return 
	end

	local start = self:GetTodayStartSection()
	local now = self:GetNowAttackSection()
	self.itemBoxRed = {}
	local index = 1
	for i = start , now do
		if not self.itemBoxRed[i] then
			self.itemBoxRed[i] = {}
		end
		for j = 1 , 3 do
			local draw = self:GetUnionTranscriptSectionDraw( i , j )
			if not self.itemBoxRed[i][j] then
				self.itemBoxRed[i][j] = {}
			end
			self.itemBoxRed[i][j] = draw
		end
	end
end

--更新宝箱的红点
function Modules.UnionModule:UpItemBoxRed( chapter , camp  )  
	self.itemBoxRed[chapter][camp] = self:GetUnionTranscriptSectionDraw( chapter , camp )
end

--获取宝箱的红点
function Modules.UnionModule:GetItemBoxRed( chapter , camp  )  
	return self.itemBoxRed[chapter][camp]
end

--获取重置信息章节
function Modules.UnionModule:GetResetLst()  
	--章节
	local section = self:GetNowAttackSection()
	local fbCnf = CPPGameLib.GetConfig("GuildFB", section )
	
	local section1 = section - 1
	if section1 == 0 then
		section1 = section
	end
	local fbCnf_ = CPPGameLib.GetConfig("GuildFB", section1 )

	local info = {}
	info[1] = { type = 1 , name = fbCnf.name , section = section }
	info[2] = { type = 1 , name = fbCnf_.name , section = section1}
	
	return info
end

--怪物血量整理
function Modules.UnionModule:MonsterHPSettle()  

	local t1 = {}
	local damage = 0
	local max = 0
	for i,v in pairs( self.monsterData.camp_list ) do
		local damage_ = 0
		local max_ = 0
		for _ , vv in ipairs( v.monster_list.__list ) do

			if vv.damage >= vv.total_hp then
				vv.damage = vv.total_hp
			end
			damage = damage + vv.damage
			damage_ = damage_ + vv.damage

			max = max + vv.total_hp
			max_ = max_ + vv.total_hp

		end
		table.insert( t1 , { damage = damage_ , max = max_ , killer_name = v.killer_name } )
	end

	--当前吃了这么多伤害
	self.monsterData.nowSchedule = damage
	self.monsterData.maxSchedule = max

	--每个关卡的信息
	self.monsterData.levelData = t1
end

--获取技能属性加成
function Modules.UnionModule:GetSkillPlusPropMap()
	if not self.my_skill_list then
		return
	end

	local config_skill, config_skill_lev
	local plus_prop_map
	for skill_id, skill_level in pairs(self.my_skill_list) do				
		config_skill = CPPGameLib.GetConfig("GuildSkill", skill_id, false)
		if config_skill and config_skill.skill_list then
			config_skill_lev = config_skill.skill_list[skill_level]
			if config_skill_lev then
				for i, prop_type in ipairs(config_skill_lev.add_pro_type) do
					plus_prop_map = plus_prop_map or {}
					plus_prop_map[prop_type] = plus_prop_map[prop_type] or 0
					plus_prop_map[prop_type] = plus_prop_map[prop_type] + config_skill_lev.add_pro_value[i] or 0
				end
			end
		end
	end

	return plus_prop_map
end

--------------------------------------------------------------------------------------------------------------------------------
--战斗判断
--------------------------------------------------------------------------------------------------------------------------------
function Modules.UnionModule:CheckBattleCondition( type_ , data )
	--公会副本
	if type_ == Macros.Game.FBType.UNION then
		local time = CallModuleFunc(ModuleType.UNION , "GetNowChallengeTime" )
		if time ~= 0 then
			return true
		end
	--低级星球
	elseif type_ == Macros.Game.FBType.UNION_LOW then
   		local num = CallModuleFunc(ModuleType.UNION , "GetUnionCombatLowTime" )
   		if num ~= 0 then
   			return true
		end
	--高级星球
	elseif type_ == Macros.Game.FBType.UNION_TALL then
		local isBase , isAttack , isOver = CallModuleFunc(ModuleType.UNION , "JudgeCanItAttack" , data.tower_id )
		if isAttack == true and isOver == false then
			return true
		--如果是基地 而且还不能打
		elseif isBase == true and isAttack ~= true  then
				
		end
	end
end