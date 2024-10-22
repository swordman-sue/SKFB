Modules = Modules or {}

Modules.WorldHeart = Modules.WorldHeart or BaseClass(BaseModule)

function Modules.WorldHeart:__init()
	self:__RegistProtocolForOpenView(BaseViewType.WORLD_HEART,Net.ProtocolNo.C2S_HEART_OF_WORLD_DATA_REQ, false)
end

function Modules.WorldHeart:__delete()
	-- body
end

--请求精灵系统信息
function Modules.WorldHeart:WorldHeartDataReq()
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_HEART_OF_WORLD_DATA_REQ)
	GlobalNetAdapter:OnSend(protocol)
end

--请求精灵系统信息响应
function Modules.WorldHeart:WorldHeartDataResp(data)	
	--接收的是当前已经开启完的
	self.__worldheart_data = {}
	self.__worldheart_data.__stage = data.stage
	self.__worldheart_data.__level = data.level
	self.__worldheart_data.org_stage = data.stage
	self.__worldheart_data.org_level = data.level
	local elves_info = CPPGameLib.GetConfig("HeartOfWorld",self.__worldheart_data.__stage)
	--移动到下一阶
	if data.level >= #elves_info.elves_list and data.stage < #CPPGameLib.GetConfig("HeartOfWorld",nil,nil,true) then
		self.__worldheart_data.__level = 1
		self.__worldheart_data.__stage = data.stage + 1
	elseif data.level >= #elves_info.elves_list and data.stage >= #CPPGameLib.GetConfig("HeartOfWorld",nil,nil,true) then
		self.__worldheart_data.__is_full_level = true
		self.__worldheart_data.__level = #elves_info.elves_list
	else
		self.__worldheart_data.__level = math.min(data.level + 1, Macros.Game.WORLD_HEART_STAGE_NUM)
	end	
end

--获取精灵系统信息数据
function Modules.WorldHeart:GetWorldHeartData()
	return self.__worldheart_data
end

--请求精灵系统升级
function Modules.WorldHeart:WorldHeartLevelUpDataReq()
	local protocol = GlobalNetAdapter:GetProtocol(Net.ProtocolNo.C2S_HEART_OF_WORLD_LEVEL_UP_REQ)
	GlobalNetAdapter:OnSend(protocol)
end

--请求精灵系统升级响应
function Modules.WorldHeart:WorldHeartLevelUpDataResp(data)
	CPPGameLib.PrintTable("ldx",data,"datadata: ")
	self.__worldheart_data = {}
	self.__worldheart_data.__stage = data.stage
	self.__worldheart_data.__level = data.level
	self.__worldheart_data.org_stage = data.stage
	self.__worldheart_data.org_level = data.level
	local elves_info = CPPGameLib.GetConfig("HeartOfWorld",self.__worldheart_data.__stage)
	--移动到下一阶
	if data.level >= #elves_info.elves_list and data.stage < #CPPGameLib.GetConfig("HeartOfWorld",nil,nil,true) then
		self.__worldheart_data.__level = 1
		self.__worldheart_data.__stage = data.stage + 1
	elseif data.level >= #elves_info.elves_list and data.stage >= #CPPGameLib.GetConfig("HeartOfWorld",nil,nil,true) then
		self.__worldheart_data.__is_full_level = true
		self.__worldheart_data.__level = #elves_info.elves_list
	else
		self.__worldheart_data.__level = math.min(data.level + 1, Macros.Game.WORLD_HEART_STAGE_NUM)
	end	
	self:FireNextFrame(Macros.Event.WorldHeartModule.WORLD_HEART_LEVELUP,self.__worldheart_data)
	--指引下一步
	CallModuleFunc(ModuleType.LEAD,"NextSectionByLogic",Macros.Game.NecessaryLeadKeyPointFilterType.WORLD_HEART_LIGHT_UP)
end