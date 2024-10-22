Modules = Modules or {}

Modules.Spirit = Modules.Spirit or BaseClass(BaseModule)

function Modules.Spirit:__init()
	self.__red_dot_info_map = {}
	self:__RegistProtocolForOpenView(BaseViewType.SPIRIT,Net.ProtocolNo.C2S_SPIRIT_DATA_REQ, false)
end

function Modules.Spirit:__delete()
end

--精灵系统数据请求
function Modules.Spirit:SpiritDataReq()
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SPIRIT_DATA_REQ)
	GlobalNetAdapter:OnSend(protocol)
end

--精灵系统数据响应
function Modules.Spirit:SpiritDataResp(protocol_data)
	self.__spirit_data = protocol_data
	self.__spirit_data.spirit_info_map = {}
	for i,spirit_info in ipairs(self.__spirit_data.spirit_list) do
		self.__spirit_data.spirit_info_map[spirit_info.id] = spirit_info
	end
	self:FireNextFrame(Macros.Event.SpiritModule.SPIRIT_DATA_RESP)	

	--统计红点
    CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.SPIRIT, nil, Macros.Game.SystemID.MAX)	
end

--获取精灵系统信息数据
function Modules.Spirit:GetSpiritData()
	return self.__spirit_data, self.__red_dot_info_map
end

--获取某个精灵数据
function Modules.Spirit:GetSpiritInfoByID(spirit_id)
	if self.__spirit_data then
		return self.__spirit_data.spirit_info_map[spirit_id]
	end
end

--获取某个精灵图鉴数据
function Modules.Spirit:GetSpiritHandBookInfoByID(pokedex_id)
	if self.__spirit_data then
		for i,pokedex_info in ipairs(self.__spirit_data.pokedex_list) do
			if pokedex_info.id == pokedex_id then
				return pokedex_info				
			end
		end
	end
end

--精灵强化请求
function Modules.Spirit:SpiritEnhanceReq(spirit_id)
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SPIRIT_ENHANCE_REQ)
	protocol.spirit_id = spirit_id
	GlobalNetAdapter:OnSend(protocol)
end

--精灵强化响应
function Modules.Spirit:SpiritEnhanceResp(protocol_data)
	local spirit_info = self:GetSpiritInfoByID(protocol_data.spirit_id)
	if spirit_info then
		spirit_info.enhance_level = spirit_info.enhance_level + 1
	end

	self:FireNextFrame(Macros.Event.SpiritModule.SPIRIT_ENHANCE_RESP)

	--统计红点
    CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.SPIRIT, nil, Macros.Game.SystemID.SPIRIT_ENHANCE)	
end

--精灵出战请求
function Modules.Spirit:SpiritUseReq(spirit_id)
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SPIRIT_USE_REQ)
	protocol.spirit_id = spirit_id
	GlobalNetAdapter:OnSend(protocol)
end

--精灵出战响应
function Modules.Spirit:SpiritUseResp(protocol_data)
	self.__spirit_data.cur_use_spirit = protocol_data.spirit_id

	self:FireNextFrame(Macros.Event.SpiritModule.SPIRIT_ON_FIGHT_RESP, protocol_data.spirit_id)

	--统计红点
    CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.SPIRIT, nil, Macros.Game.SystemID.MAX)

	--指引下一步
	CallModuleFunc(ModuleType.LEAD,"NextSectionByLogic",Macros.Game.NecessaryLeadKeyPointFilterType.SPIRIT_ON_FIGHTING)
end

--精灵培养请求
function Modules.Spirit:SpiritTrainReq(spirit_id,train_times)
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SPRITE_TRAIN_REQ)
	protocol.sprite_id = spirit_id
	protocol.train_times = train_times
	GlobalNetAdapter:OnSend(protocol)
end

--精灵培养响应
function Modules.Spirit:SpiritTrainResp(protocol_data)
	self:FireNextFrame(Macros.Event.SpiritModule.SPIRIT_TRAIN_RESP, protocol_data.sprite_id)

	--统计红点
    CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.SPIRIT, nil, Macros.Game.SystemID.SPIRIT_SUBLIMATE)
    CallModuleFunc(ModuleType.RED_MGR, "StatisticsRed", Macros.Game.SystemID.SPIRIT, nil, Macros.Game.SystemID.SPIRIT_SUBLIMATE, true)
end

--精灵升星请求
function Modules.Spirit:SpiritStarUpReq(spirit_id)
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SPRITE_STAR_UP_REQ)
	protocol.sprite_id = spirit_id
	GlobalNetAdapter:OnSend(protocol)
end

--精灵升星响应
function Modules.Spirit:SpiritStarUpResp(protocol_data)
	--自己加星
	local spirit_info = self:GetSpiritInfoByID(protocol_data.sprite_id)
	if spirit_info then
		spirit_info.star = spirit_info.star + 1
	end
	self:FireNextFrame(Macros.Event.SpiritModule.SPIRIT_TRAIN_RESP, protocol_data.sprite_id, true)
end

--[[
bless_type
1= 祝福
2= 替换
3= 取消祝福
]]--
--精灵祝福请求
function Modules.Spirit:SpiritBlessReq(req_data,req_battle_spiritid)
	self.__req_battle_spiritid = req_battle_spiritid
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SPRITE_BLESS_REQ)
	protocol.sprite_id = req_data.sprite_id
	protocol.bless_type = req_data.bless_type
	protocol.formation_pos = req_data.formation_pos
	GlobalNetAdapter:OnSend(protocol)
end

--精灵祝福响应
function Modules.Spirit:SpiritBlessResp(protocol_data)
	self:FireNextFrame(Macros.Event.SpiritModule.SPIRIT_BLESS_RESP)
	if self.__req_battle_spiritid then
		self:SpiritUseReq(self.__req_battle_spiritid)
	end
end

--消息说明: 精灵合成请求 消息Id: 22017
function Modules.Spirit:SpriteComposeReq( info )
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_SPRITE_COMPOSE_REQ)
	protocol.fragment_id = info.fragment_id
	GlobalNetAdapter:OnSend(protocol)
end

--消息说明: 精灵合成响应 消息Id: 22018
-- "fragment_id__I",       --碎片ID
function Modules.Spirit:SpriteComposeResp( info )
end

--计算红点
function Modules.Spirit:CountRedDot()
	self.__red_dot_info_map = {}

	if self.__spirit_data.cur_use_spirit == 0 then
		return
	end

	self:CountEnhanceRedDot()
	self:CountTrainRedDot()
	self:CountSublimateRedDot()
end

--计算可升级的红点
function Modules.Spirit:CountEnhanceRedDot()
	self.__red_dot_info_map[Modules.RedMgr.SystemID.SPIRIT_ENHANCE] = {}
	if not self.__spirit_data or (self.__spirit_data and self.__spirit_data.cur_use_spirit == 0) then
		return
	end

	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	local is_open = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.SPIRIT_ENHANCE)
	if is_open then 
		--计算可升级红点
		for spirit_id,spirit_info in pairs(self.__spirit_data.spirit_info_map) do
			self.__red_dot_info_map[Modules.RedMgr.SystemID.SPIRIT_ENHANCE][spirit_id] = 0
			--精灵强化配置
			local spirit_info = CPPGameLib.GetConfig("SpiritEnhace",spirit_id)
			local on_fight_enhance_level = self.__spirit_data.spirit_info_map[spirit_id].enhance_level
			--是否满级
			if on_fight_enhance_level < table.maxn(spirit_info.enhance_list) then
				--满足角色等级条件
				if role_level >= spirit_info.enhance_list[on_fight_enhance_level].need_level then
					--可升级
					local item_num = ObjIDTool.GetOwnNum(spirit_info.enhance_list[on_fight_enhance_level].cost_item_id_list[1])
					self.__red_dot_info_map[Modules.RedMgr.SystemID.SPIRIT_ENHANCE][spirit_id] = 
					item_num >= spirit_info.enhance_list[on_fight_enhance_level].cost_item_num_list[1] and 1 or 0		
				end
			end
		end		
	end
end

--计算可培养的红点
function Modules.Spirit:CountTrainRedDot()
	self.__red_dot_info_map[Modules.RedMgr.SystemID.SPIRIT_TRAIN] = {}
	if not self.__spirit_data or (self.__spirit_data and self.__spirit_data.cur_use_spirit == 0) then
		return
	end

	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	local is_open = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.SPIRIT_SUBLIMATE)
	if is_open then
		--计算培养红点
		for spirit_id,spirit_info in pairs(self.__spirit_data.spirit_info_map) do
			self.__red_dot_info_map[Modules.RedMgr.SystemID.SPIRIT_TRAIN][spirit_id] = 0
			--精灵培养配置
			local spirit_train_config = CPPGameLib.GetConfig("SpriteTrain",spirit_id)
			local cur_stage = self.__spirit_data.spirit_info_map[spirit_id].train_stage
			--是否满级
			if cur_stage < #spirit_train_config.stage_list then
				--是否是升华
				local is_on_sublimate = self.__spirit_data.spirit_info_map[spirit_id].train_times >= spirit_train_config.stage_list[cur_stage].need_train_times
				--不可升华		
				if not is_on_sublimate then
					local is_enough = true
					for i,pro_type in ipairs(spirit_train_config.stage_list[cur_stage].pro_type_list) do
						--材料数量
						if ObjIDTool.GetOwnNum(spirit_train_config.stage_list[cur_stage].cost_item_id_list[i]) < spirit_train_config.stage_list[cur_stage].cost_item_num_list[i] then
							is_enough = false
							break
						end
					end
					self.__red_dot_info_map[Modules.RedMgr.SystemID.SPIRIT_TRAIN][spirit_id] = is_enough and 1 or 0
				end	
			end			
		end		
	end
end

--计算可升华的红点
function Modules.Spirit:CountSublimateRedDot()
	self.__red_dot_info_map[Modules.RedMgr.SystemID.SPIRIT_SUBLIMATE] = {}
	if not self.__spirit_data or (self.__spirit_data and self.__spirit_data.cur_use_spirit == 0) then
		return
	end	
	
	--计算升华红点
	for spirit_id,spirit_info in pairs(self.__spirit_data.spirit_info_map) do
		self.__red_dot_info_map[Modules.RedMgr.SystemID.SPIRIT_SUBLIMATE][spirit_id] = 0
		--精灵培养配置
		local spirit_train_config = CPPGameLib.GetConfig("SpriteTrain",spirit_id)
		--是否满级
		if spirit_info.train_stage < #spirit_train_config.stage_list then
			--是否是升华
			local is_on_sublimate = spirit_info.train_times >= spirit_train_config.stage_list[spirit_info.train_stage].need_train_times
			--是否满足强化等级
			local is_setisfly_level = spirit_info.enhance_level >= spirit_train_config.stage_list[spirit_info.train_stage].need_enhance_level
			if is_on_sublimate and is_setisfly_level then
				local item_num = ObjIDTool.GetOwnNum(spirit_train_config.stage_list[spirit_info.train_stage].stage_up_cost_item[1])
				if item_num >= spirit_train_config.stage_list[spirit_info.train_stage].stage_up_cost_item[2] then
					self.__red_dot_info_map[Modules.RedMgr.SystemID.SPIRIT_SUBLIMATE][spirit_id] = 1
				end
			end
		end
	end		
end

--获取精灵系统红点数据
function Modules.Spirit:GetRedDotInfoMap()
	return self.__red_dot_info_map or {}
end

--获取精灵数量
function Modules.Spirit:GetNumByInfoID(spirit_id)
	return self.__spirit_data.spirit_info_map[spirit_id] and 1 or 0
end

--战斗中是否显示精灵祝福效果
function Modules.Spirit:IsShowBattleEffect()
	if GlobalModulesMgr:IsOpen(Macros.Game.SystemID.SPIRIT) then
		return true
	end
end

--精灵表
function Modules.Spirit:GetSpiritCnf()
	local lst = CPPGameLib.GetConfig("Spirit", nil, nil , true )
	local t1 = {}
	for i , v in pairs(lst) do
		local data = {}
		data.id = i
		table.insert( t1 , data )
	end

	table.sort( t1 , function ( a , b )
		return a.id < b.id
	end )

	return t1
end

--现在选中的通用技能id
function Modules.Spirit:SetCommonKillId( id )
	self.commonKillId = id
	Cookies.Set(Cookies.LevelType.ROLE, Macros.Setting.BattleSpriteCommonSkillID, id)
end

--现在选中的通用技能id
function Modules.Spirit:GetCommonKillId( )
	if not self.commonKillId then
		local lst = self:GetSpiritCnf()
		local cur_config = CPPGameLib.GetConfig("Spirit",lst[1].id)
		return cur_config.common_skill_id
	else
		return self.commonKillId
	end
end

--获取当前非出战且已获得精灵的列表，并找到对应祝福的英雄
function Modules.Spirit:GetSpiritBlessList()
	local spirit_bless_list = {}
	for spirit_id,spirit_info in pairs(CPPGameLib.GetConfig("Spirit",nil,nil,true)) do
		--非出战精灵
		if spirit_id ~= self.__spirit_data.cur_use_spirit then
			local cur_spirit_info = self.__spirit_data.spirit_info_map[spirit_id]
			local cp_data = cur_spirit_info and CPPGameLib.CopyTblShallow(cur_spirit_info) or {}
			cp_data.id = spirit_id
			cp_data.sort_id = spirit_info.sort_id
			cp_data.is_exist = cur_spirit_info and true or false
			--遍历是否有祝福阵位英雄
			local lineup_list = CallModuleFunc(ModuleType.HERO,"GetLineupList")
			for _,lineup_info in ipairs(lineup_list) do
				if lineup_info.bless_sprite_id == spirit_id then
					cp_data.bless_herouid = lineup_info.uid
					break
				end
			end
			table.insert(spirit_bless_list,cp_data)
		end
	end
	--排序
	table.sort(spirit_bless_list,function (spirit_data1,spirit_data2)
		if spirit_data1.is_exist and not spirit_data2.is_exist then
			return true
		elseif not spirit_data1.is_exist and spirit_data2.is_exist then
			return false
		else
			return spirit_data1.sort_id < spirit_data2.sort_id
		end	

	end)
	return spirit_bless_list
end

--获取当前精灵是否已经祝福英雄
function Modules.Spirit:GetIsSpiritBlessBySpiritId(spirit_id)
	if not spirit_id then return end
	local is_bless,formation_pos
	--遍历是否有祝福阵位英雄
	local lineup_list = CallModuleFunc(ModuleType.HERO,"GetLineupList")
	for lineup_index,lineup_info in ipairs(lineup_list) do
		if lineup_info.bless_sprite_id == spirit_id then
			is_bless = true
			formation_pos = lineup_index - 1
			break
		end
	end
	return is_bless,formation_pos
end
