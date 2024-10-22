
--[[
%% @module: 红点对象基类
%% @author: swordman sue
%% @created: 2018-12-6
%% @description: 负责检测某系统是否进行红点提示
--]]

Modules = Modules or {}

Modules.Red = Modules.Red or {}

Modules.Red.BaseRed = Modules.Red.BaseRed or BaseClass(EventAndTimerWrapper)

--是否显示红点数量
Modules.Red.BaseRed.IS_SHOW_NUM = nil

function Modules.Red.BaseRed:__init()
	self.__interval = 1
	self.red = 0
end

function Modules.Red.BaseRed:__delete()
end

--更新
function Modules.Red.BaseRed:Update(now_time, elapse_time)
	--检测红点统计
	self:__CheckStatistics(elapse_time)
end

--申请统计某系统的红点
function Modules.Red.BaseRed:Statistics(is_immediately, sub_system_id, ...)
	--初始化子系统信息
	self.__sub_system_map = self.__sub_system_map or {}
	local sub_system_info = self.__sub_system_map[sub_system_id]
	if not sub_system_info then
		sub_system_info = {}
		self.__sub_system_map[sub_system_id] = sub_system_info
	end

	--设置检测所需参数
	if not sub_system_info.dirty then
		sub_system_info.dirty = true
		sub_system_info.elapse_time = 0
	end
	sub_system_info.params_list = {...}

	--立即检测红点统计
	if is_immediately then
		self:__CheckStatistics(nil, sub_system_id, true)
	end
end

--获取红点信息
function Modules.Red.BaseRed:GetRed(sub_system_id)
	--立即检测红点统计，保证获得最新数据
	self:__CheckStatistics(nil, sub_system_id, true)

	--总红点数
	if self.red then
		return self.red
	
	elseif self.red_map then
		--子系统红点数
		if sub_system_id and sub_system_id ~= Macros.Game.SystemID.MAX then
			return self.red_map[sub_system_id] or 0

		--总红点数
		else
			local red_num = 0
			local is_only_maincity_view_open = GlobalViewMgr:IsOnlyCityViewOpen()
			for sub_system_id, num in pairs(self.red_map) do
				if is_only_maincity_view_open then
					--在主界面时，优先使用服务端统计的红点
					red_num = red_num + (CallModuleFunc(ModuleType.RED_MGR, "GetServerRed", sub_system_id) or num)
				else
					red_num = red_num + num
				end
				if red_num > 0 and not Modules.Red.BaseRed.IS_SHOW_NUM then
					break
				end
			end
			return red_num
		end	
	end

	return 0
end

--设置系统ID
function Modules.Red.BaseRed:SetSystemID(var)
	self.__system_id = var
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--红点统计逻辑
function Modules.Red.BaseRed:__Statistics(sub_system_id, params_list)
end

--检测红点统计
function Modules.Red.BaseRed:__CheckStatistics(elapse_time, appointed_sub_system_id, is_immediately)
	--处理函数
	local function HandleFunc(sub_system_id, sub_system_info)
		sub_system_info = sub_system_info or self.__sub_system_map[sub_system_id]
		if not sub_system_info or not sub_system_info.dirty then
			return
		end

		--立即检测红点统计
		if is_immediately then
			sub_system_info.elapse_time = self.__interval
		else
			sub_system_info.elapse_time = sub_system_info.elapse_time + elapse_time
		end
		--检测红点统计
		if sub_system_info.elapse_time >= self.__interval then
			sub_system_info.elapse_time = 0
			sub_system_info.dirty = nil

			--执行红点统计逻辑
			self:__Statistics(sub_system_id, sub_system_info.params_list)

			--发送红点更新事件
			self:FireNextFrame(Macros.Event.RedModule.RED_UPDATE, self.__system_id, sub_system_id)
		end
	end

	--检测指定子系统
	if appointed_sub_system_id then
		HandleFunc(appointed_sub_system_id)

	--检测所有子系统
	else
		for sub_system_id, sub_system_info in pairs(self.__sub_system_map) do
			HandleFunc(sub_system_id, sub_system_info)
		end
	end
end

