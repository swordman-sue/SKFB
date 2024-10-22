require "modules/battle/templ_common_reward"
require "modules/battle/battle_lose/sp_battle_result_lose_base"

--[[
%% @module: 竞技场结算
%% @author: yjg
%% @created: 2018年12月25日11:51:08
--]]

Modules = Modules or {}

Modules.ArenaResultLoseView = Modules.ArenaResultLoseView or BaseClass( Modules.SpBattleResultLoseBase )

function Modules.ArenaResultLoseView:__init()
end

function Modules.ArenaResultLoseView:__OpenCallback()
	Modules.SpBattleResultLoseBase.__OpenCallback(self)
	
	--战斗奖励
	self:__ShowCommonRewards()
end

function Modules.ArenaResultLoseView:__Dispose()
	Modules.SpBattleResultLoseBase.__Dispose(self)
	if self.__templ_common_reward then
		self.__templ_common_reward:DeleteMe()
		self.__templ_common_reward = nil
	end

	self.__num_addition_anim_list = nil
end

--上方信息
function Modules.ArenaResultLoseView:__ShowTopInfo()
end

--战斗奖励
function Modules.ArenaResultLoseView:__ShowCommonRewards( )
	Modules.SpBattleResultLoseBase.__ShowCommonRewards(self)
	local battle_info = self:CallModuleFunc("GetCurInfo")

	local config_arena_award = CPPGameLib.GetConfig("ArenaAward", battle_info.pre_role_info.level)
	self.__templ_common_reward = Modules.TemplBattleCommonReward.New(
	{
		parent_node = self.panel_common_reward,
		add_gold = config_arena_award.fail_gold,
		add_exp = config_arena_award.exp,
		add_type = { 
						{ id = Macros.Game.RolePropType.ARENA_PRESTIGE , value = config_arena_award.fail_arena_prestige } 
					},
		time = 0.5,
		callback = function()
			self.__common_reward_finished = true
			Modules.SpBattleResultLoseBase.__CheckFinished(self)
		end
	})	
	self.__templ_common_reward:GetNode():SetPosition( (self.panel_common_reward:GetContentWidth() - self.__templ_common_reward:GetNode():GetContentWidth() ) /2  , 0 )
end

--额外奖励
function Modules.ArenaResultLoseView:__ShowExtraRewards()
end