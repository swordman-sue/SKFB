require "modules/battle/templ_common_reward"
require "modules/battle/battle_lose/sp_battle_result_lose_base"

--[[
%% @module: 夺宝副本结算
%% @author: yjg
%% @created: 2018年12月25日12:01:02
--]]

Modules = Modules or {}

Modules.PlunderedTreasuresResultLoseView = Modules.PlunderedTreasuresResultLoseView or BaseClass( Modules.SpBattleResultLoseBase )

function Modules.PlunderedTreasuresResultLoseView:__init()
end

function Modules.PlunderedTreasuresResultLoseView:__OpenCallback()
	Modules.SpBattleResultLoseBase.__OpenCallback(self)
	self:__ShowCommonRewards()
end

function Modules.PlunderedTreasuresResultLoseView:__Dispose()
end

--上方信息
function Modules.PlunderedTreasuresResultLoseView:__ShowTopInfo()
end

--战斗奖励
function Modules.PlunderedTreasuresResultLoseView:__ShowCommonRewards( )
	Modules.SpBattleResultLoseBase.__ShowCommonRewards(self)
	local battle_info = self:CallModuleFunc("GetCurInfo")

	PosTool.MoveBy(self.panel_common_reward, 0, 62)

	self.__templ_common_reward = Modules.TemplBattleCommonReward.New(
	{
		parent_node = self.panel_common_reward,
		add_gold = CPPGameLib.GetConfig("DuobaoMap", battle_info.pre_role_info.level , false).fail_gold ,
		add_exp = CPPGameLib.GetConfig("DuobaoMap", battle_info.pre_role_info.level , false).exp ,
		time = 0.5,
		callback = function()
			self.__common_reward_finished = true
			Modules.SpBattleResultLoseBase.__CheckFinished(self)
		end
	})	

	self.__templ_common_reward:SetAnchorPoint(0.5, 0.5)
	PosTool.Center( self.__templ_common_reward:GetNode() ,0 , -70 )
end

--额外奖励
function Modules.PlunderedTreasuresResultLoseView:__ShowExtraRewards()
end