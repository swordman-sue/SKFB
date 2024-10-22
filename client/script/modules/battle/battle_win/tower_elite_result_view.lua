-- require "modules/battle/templ_common_reward"
require "modules/battle/battle_win/sp_battle_result_win_base"

--[[
%% @module: 爬塔精英结算
%% @author: yjg
%% @created: 2018年12月12日18:32:09
--]]

Modules = Modules or {}

Modules.TowerEliteResultWinView = Modules.TowerEliteResultWinView or BaseClass( Modules.SpBattleResultWinBase )

function Modules.TowerEliteResultWinView:__init()
end

function Modules.TowerEliteResultWinView:__OpenCallback()
	Modules.SpBattleResultWinBase.__OpenCallback(self)
	
	self:__ShowTopInfo()
	self:__ShowCommonRewards()
	self:__ShowExtraRewards()
end

function Modules.TowerEliteResultWinView:__Dispose()
end

--上方信息
function Modules.TowerEliteResultWinView:__ShowTopInfo()
	Modules.SpBattleResultWinBase.__ShowTopInfo( self )
	if self.topInfo ~= true then
		return 
	end
	self.topInfo = false
end

--战斗奖励
function Modules.TowerEliteResultWinView:__ShowCommonRewards( end_callback )
	Modules.SpBattleResultWinBase.__ShowCommonRewards( self )
	if self.commonRewards ~= true then
		return true
	end
	self.commonRewards = false

	self.panel_common_reward:SetVisible(false)
	self.Text_CommonRewardTitle:SetVisible(false)
	self.Image_1:SetVisible(false)

	Modules.SpBattleResultWinBase.CommonRewardFinishedCallback( self )
end

--额外奖励
function Modules.TowerEliteResultWinView:__ShowExtraRewards()
	Modules.SpBattleResultWinBase.__ShowExtraRewards( self )
	
	if self.extraRewards ~= true then
		return 
	end
	self.extraRewards = true

	self.Image_2:SetPosition( self.Image_2:GetPositionX() , self.Image_2:GetPositionY() + 80 )
	self.Text_SpecialRewardTitle:SetPosition( self.Text_SpecialRewardTitle:GetPositionX() , self.Text_SpecialRewardTitle:GetPositionY() + 80 )
	self.panel_extra_reward:SetPosition( self.panel_extra_reward:GetPositionX() , self.panel_extra_reward:GetPositionY() + 80 )

	Modules.SpBattleResultWinBase.__ShowDropRewards( self , self.panel_extra_reward )
	self.__extra_reward_finished = true
	Modules.SpBattleResultWinBase.__CheckFinished( self )	

end