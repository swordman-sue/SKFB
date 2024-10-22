
--[[
%% @module: 充值回馈红点
%% @author: yjg
%% @created: 2018年12月24日19:45:34
--]]
Modules.Red.Feedback = Modules.Red.Feedback or BaseClass( Modules.Red.BaseRed )

function Modules.Red.Feedback:__init( ... )
end

--红点统计逻辑
function Modules.Red.Feedback:__Statistics( sub_system_id )
    self.red = CallModuleFunc(ModuleType.FEEDBACK, "CheckReward", true)
end