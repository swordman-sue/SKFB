--[[
%% @module: 爵位
%% @author: yjg
%% @created: 2017年8月14日15:50:17
--]]


Modules = Modules or {}

Modules.KnighthoodModule = Modules.KnighthoodModule or BaseClass(BaseModule)

function Modules.KnighthoodModule:__init()
	self.nobilityCnf = {}
	self.nobility_level = nil
	self:SettleCnf()
end

function Modules.KnighthoodModule:__delete()
end

-- 消息说明:爵位激活请求 消息Id: 24901
function Modules.KnighthoodModule:NobilityActivateReq( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:爵位激活请求 消息Id: 24901") 
	local data = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_NOBILITY_ACTIVATE_REQ)
	GlobalNetAdapter:OnSend(data)

	--终止战力飘动
	CallModuleFunc(ModuleType.ROLE,"RecordBattlePowerChanged" , Macros.Game.COMBAT_CHANGE.KNIGHTHOOD )
end

-- 消息说明:爵位激活响应 消息Id: 24902
function Modules.KnighthoodModule:NobilityActivateResp( info )
	CPPGameLib.PrintTable("xy" , info , "消息说明:爵位激活响应 消息Id: 24902") 
	self.nobility_level = info.nobility_level
	CallModuleFunc(ModuleType.ROLE,"SetPropValue" , Macros.Game.RolePropType.NOBILITY , self.nobility_level )
	GlobalEventSystem:FireNextFrame(Macros.Event.KnighthoodModule.KNIGHTHOOD)
end

--获得图标列表
function Modules.KnighthoodModule:GetIconLstCnf()
	local t1 = {}

	for i , v in pairs( self.nobilityCnf ) do
		local t2 = {}
		for ii , vv in ipairs( v.pro_type_list ) do
			table.insert( t2 , { type = vv , value = v.pro_value_list[ii] } )
		end
		table.insert( t1 , { id = i , properText = v.pro_type_show , properLst = t2 } )
	end

	local t3 = {}
	for i = 1 , self:GetActivateRecord() do
		if t1[i] then
			t3[i] = t1[i].properLst
		end
	end

	local t4 = {}
	for _ , vv in ipairs( t3 ) do
		for i , v in ipairs(vv) do
			-- if t4[ v.type ] then
			-- 	t4[ v.type ] = t4[ v.type ] + v.value
			-- else
				t4[ v.type ] = v.value
			-- end
		end
	end

	local t5 = {}
	for i , v in pairs( t4 ) do
		table.insert( t5 , { type = i , value = v } )
	end

	table.sort( t5, function ( a , b )
		return a.type < b.type 
	end )

	for i,data in ipairs(t1) do
		data.index = i
	end
	return t1 , t5
end

--获得需求战斗力
function Modules.KnighthoodModule:GetNeedFighting( id )
	local need_power = nil
	if self.nobilityCnf[id] and self.nobilityCnf[id].need_power then
		need_power = self.nobilityCnf[id].need_power
	end

	local my_combat_value = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.BATTLE_VALUE)
	return my_combat_value , need_power
end

--获得激活记录
function Modules.KnighthoodModule:GetActivateRecord( id )
	-- local t1 = {}
	-- for i = 1 , 10 do
	-- 	if i < 3 then
	-- 		t1[i] = true
	-- 	else
	-- 		t1[i] = false
	-- 	end
	-- end
	return CallModuleFunc(ModuleType.ROLE,"GetNobility")
end

--整理
function Modules.KnighthoodModule:SettleCnf()
	for i , v in pairs( CPPGameLib.GetConfig("Nobility", nil , nil , true ) ) do
		self.nobilityCnf[i] = v
	end
end

--获取爵位属性加成
function Modules.KnighthoodModule:GetPlusPropMap()
	local plus_prop_map
	local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")
	local config_nobility = CPPGameLib.GetConfig("Nobility", role_info.nobility_level, false)
	if config_nobility then
		for i, pro_type in ipairs(config_nobility.pro_type_list) do
			plus_prop_map = plus_prop_map or {}
			plus_prop_map[pro_type] = plus_prop_map[pro_type] or 0
			plus_prop_map[pro_type] = plus_prop_map[pro_type] + config_nobility.pro_value_list[i]
		end		
	end
	return plus_prop_map
end

