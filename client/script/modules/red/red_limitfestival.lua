
--[[
%% @module: 限时活动节日红点
%% @author: ldx
%% @created: 2019年1月05日
--]]
Modules.Red.LimitFestival = Modules.Red.LimitFestival or BaseClass( Modules.Red.BaseRed )

function Modules.Red.LimitFestival:__init( ... )
end

--红点统计逻辑
function Modules.Red.LimitFestival:__Statistics()    
	--阵营招募
	if self.__system_id == Macros.Game.SystemID.CAMP_RECRUIT then
		local camp_recruit_data = CallModuleFunc(ModuleType.LIMIT_ACTIVITY, "GetCampRecruitData")
		self.red = camp_recruit_data and camp_recruit_data.today_already_recruit_times < 1 and 1 or 0

	--圣诞活动、端午活动、春节活动
	else
		local reddom_num
		for snowball_id,_ in pairs(CPPGameLib.GetConfig("ChristmasSnowBall",nil,nil,true)) do
			reddom_num = ObjIDTool.GetOwnNum(snowball_id)
			if reddom_num > 0 then
				self.red = reddom_num
				break
			end
		end
	end
end