--[[
%% @module: 称号模块
%% @author: yjg
%% @created: 2017年7月8日15:12:04
--]]


Modules = Modules or {}

Modules.TitleModule = Modules.TitleModule or BaseClass(BaseModule)

function Modules.TitleModule:__init()
	self.title_list = {}
	self:__TitleSettle()
end

function Modules.TitleModule:__delete()
	if self.chenghao then
		GlobalTimerQuest:CancelQuest( self.chenghao )
		self.chenghao = nil
	end
	if self.zhuangshi then
		GlobalTimerQuest:CancelQuest( self.zhuangshi )
		self.zhuangshi = nil
	end
end

-- 消息说明: 请求角色称号信息 消息Id: 23401
function Modules.TitleModule:RoleTitleInfoReq( info )
	CPPGameLib.PrintTable("xy" , info,"消息说明: 请求角色称号信息 消息Id: 23401")  
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_ROLE_TITLE_INFO_REQ)
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 请求角色称号信息响应 消息Id: 23402
function Modules.TitleModule:RoleTitleInfoResp( info )
	CPPGameLib.PrintTable("xy" , info,"消息说明: 请求角色称号信息响应 消息Id: 23402")  
	self.title_id = info.cur_use_title_id
	self.title_id_ = info.cur_use_nickname_id

	self.title_list = {}
	for i , v in ipairs( info.title_list ) do
		self.title_list[v.id] = v
	end

	self:Countdown()
	GlobalEventSystem:FireNextFrame(Macros.Event.TitleModule.TITLE)
end

-- 消息说明: 使用称号请求 消息Id: 23403
function Modules.TitleModule:UseTitleReq( info )
	CPPGameLib.PrintTable("xy" , info,"消息说明: 使用称号请求 消息Id: 23403") 
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_USE_TITLE_REQ)
	data.title_id = info.title_id
	GlobalNetAdapter:OnSend(data) 
end

-- 消息说明: 使用称号响应 消息Id: 23404
function Modules.TitleModule:UseTitleResp(info)
	CPPGameLib.PrintTable("xy" , info,"消息说明: 使用称号响应 消息Id: 23404")

	local cnf = CPPGameLib.GetConfig("Title", info.title_id, nil , true )
	if cnf.type == Macros.Game.TITLE_TYPE.TITLE then
		self.title_id = info.title_id
	elseif cnf.type == Macros.Game.TITLE_TYPE.PREFIX then
		self.title_id_ = info.title_id
	end

	GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("title_succeed") )
	-- GlobalEventSystem:FireNextFrame(Macros.Event.TitleModule.TITLE) 
end

--获得记录
function Modules.TitleModule:GetRecord( id )
	return self.title_list[id] 
end

--获得佩戴
function Modules.TitleModule:GetTitleId( ... )
	return self.title_id , self.title_id_ 
end

--获得称号表
-- Macros.Game.TITLE_TYPE.TITLE
-- Macros.Game.TITLE_TYPE.PREFIX
function Modules.TitleModule:GetTitleCnf( type )
	return self.titleCnf[type]
end

--用ID获得称号的详细属性
function Modules.TitleModule:ToIdGetTitleInfo( id )
	local cnf = CPPGameLib.CopyTbl( CPPGameLib.GetConfig("Title", id, nil , true ) )
	cnf.property = {}
	for i , v in ipairs( cnf.pro_type_list ) do
		table.insert( cnf.property , { type = v , value = cnf.pro_value_list[i] } )
	end
	cnf.pro_type_list = nil
	cnf.pro_value_list = nil
	return cnf
end

--称号表整理
function Modules.TitleModule:__TitleSettle()
	local cnf = CPPGameLib.GetConfig("Title", nil, nil , true )
	local t1 = {}
	for i , v in pairs( CPPGameLib.CopyTbl(cnf) ) do
		if not t1[v.type] then
			t1[v.type] = {}
		end
		v.id = i
		table.insert( t1[v.type] , v )
	end

	self.titleCnf = {}
	for i , v in ipairs( t1 ) do
		local t2 = {}
		for __ , vv in ipairs( v ) do
			if not t2[vv.system_type] then
				t2[vv.system_type] = {}
			end
			vv.property = {}
			for ii , vvv in ipairs( vv.pro_type_list ) do
				table.insert( vv.property , { type = vvv , value = vv.pro_value_list[ii] } )
			end
			vv.pro_type_list = nil
			vv.pro_value_list = nil
			table.insert( t2[vv.system_type] , vv )
		end

		table.insert( self.titleCnf , t2 )
	end
end

--获取称号属性加成
function Modules.TitleModule:GetPlusPropMap()
	local plus_prop_map
	local config_title
	local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
	for title_id, title_info in pairs(self.title_list) do
		if server_time < title_info.over_time then
			config_title = CPPGameLib.GetConfig("Title", title_id)
			if config_title and config_title.pro_type_list then
				for i, pro_type in ipairs(config_title.pro_type_list) do
					plus_prop_map = plus_prop_map or {}
					plus_prop_map[pro_type] = plus_prop_map[pro_type] or 0
					plus_prop_map[pro_type] = plus_prop_map[pro_type] + config_title.pro_value_list[i]
				end
			end		
		end
	end
	
	return plus_prop_map
end

--当前使用倒计时
function Modules.TitleModule:Countdown()
	if self.chenghao then
		GlobalTimerQuest:CancelQuest( self.chenghao )
		self.chenghao = nil
	end
	local chenghao = self:GetRecord( self.title_id )
	if chenghao and chenghao.over_time ~= 0 then
		--定时器
		self.chenghao = GlobalTimerQuest:AddDelayQuest(
		GlobalCallbackMgr:AddCallBackFunc(function()
			local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
			if chenghao.over_time - server_time <= 0 then
				self:RoleTitleInfoReq()
			end
		end) , 1)
	end

	if self.zhuangshi then
		GlobalTimerQuest:CancelQuest( self.zhuangshi )
		self.zhuangshi = nil
	end
	local zhuangshi = self:GetRecord( self.title_id_ )
	if zhuangshi and zhuangshi.over_time ~= 0 then
		--定时器
		self.zhuangshi = GlobalTimerQuest:AddDelayQuest(
		GlobalCallbackMgr:AddCallBackFunc(function()
			local server_time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
			if zhuangshi.over_time - server_time <= 0 then
				self:RoleTitleInfoReq()
			end
		end) , 1)
	end
end
