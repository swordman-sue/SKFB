
--[[
%% @module: 精灵红点
%% @author: swordman sue
--]]
Modules.Red.Spirit = Modules.Red.Spirit or BaseClass( Modules.Red.BaseRed )

function Modules.Red.Spirit:__init()
	self.red = nil
	self.red_map = {}
end

--红点统计逻辑
function Modules.Red.Spirit:__Statistics(sub_system_id, params_list)
	local red_dot_info_key

	--精灵
	if sub_system_id == Macros.Game.SystemID.MAX then
		CallModuleFunc(ModuleType.SPIRIT, "CountRedDot")

	--精灵强化
	elseif sub_system_id == Macros.Game.SystemID.SPIRIT_ENHANCE then
		red_dot_info_key = Modules.RedMgr.SystemID.SPIRIT_ENHANCE
		CallModuleFunc(ModuleType.SPIRIT, "CountEnhanceRedDot")

	--精灵培养、升华
	elseif sub_system_id == Macros.Game.SystemID.SPIRIT_SUBLIMATE then 
		local is_train = params_list[1]
		if is_train then
			red_dot_info_key = Modules.RedMgr.SystemID.SPIRIT_TRAIN
			CallModuleFunc(ModuleType.SPIRIT, "CountTrainRedDot")
		else
			red_dot_info_key = Modules.RedMgr.SystemID.SPIRIT_SUBLIMATE
			CallModuleFunc(ModuleType.SPIRIT, "CountSublimateRedDot")
		end
	else
		return
	end

	local red_dot_map = CallModuleFunc(ModuleType.SPIRIT, "GetRedDotInfoMap")		
	if red_dot_map then
		--记录子系统红点
		if red_dot_info_key then
			local red_dot_info = red_dot_map[red_dot_info_key]
			if red_dot_info then
				local red_num = 0
				for _, var in pairs(red_dot_info) do
					red_num = red_num + var
					if red_num > 0 and not Modules.Red.BaseRed.IS_SHOW_NUM then
						break
					end
				end
				self.red_map[red_dot_info_key] = red_num
			end

		--记录所有红点
		else
			for red_dot_info_key, red_dot_info in pairs(red_dot_map) do
				local red_num = 0
				for _, var in pairs(red_dot_info) do
					red_num = red_num + var
					if red_num > 0 and not Modules.Red.BaseRed.IS_SHOW_NUM then
						break
					end
				end
				self.red_map[red_dot_info_key] = red_num
			end
		end
	end
end
