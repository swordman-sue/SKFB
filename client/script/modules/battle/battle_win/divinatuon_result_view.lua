-- require "modules/battle/templ_common_reward" 
require "modules/battle/battle_win/sp_battle_result_win_base"

--[[
%% @module: 占卜战斗
%% @author: yjg
%% @created: 2018年12月13日17:20:57
--]]

Modules = Modules or {}

Modules.DivinatuonResultWinView = Modules.DivinatuonResultWinView or BaseClass( Modules.SpBattleResultWinBase )

function Modules.DivinatuonResultWinView:__init()
end

function Modules.DivinatuonResultWinView:__OpenCallback()
	Modules.SpBattleResultWinBase.__OpenCallback(self)
	
	self:__ShowTopInfo()
end

function Modules.DivinatuonResultWinView:__Dispose()
end

--上方信息
function Modules.DivinatuonResultWinView:__ShowTopInfo()
	Modules.SpBattleResultWinBase.__ShowTopInfo( self )
	if self.topInfo ~= true then
		return 
	end
	self.topInfo = false

	--战斗奖励
	self:__ShowCommonRewards()
	--额外奖励
	self:__ShowExtraRewards()	
end

--战斗奖励
function Modules.DivinatuonResultWinView:__ShowCommonRewards( end_callback )
	Modules.SpBattleResultWinBase.__ShowCommonRewards( self )
	if self.commonRewards ~= true then
		return true
	end
	self.commonRewards = false

	local battle_info = self:CallModuleFunc("GetCurInfo")

	self.Image_1:SetVisible(false)
	self.Text_CommonRewardTitle:SetVisible(false)
	self.panel_common_reward:SetVisible(false)	
	Modules.SpBattleResultWinBase.CommonRewardFinishedCallback( self )  
end

--额外奖励
function Modules.DivinatuonResultWinView:__ShowExtraRewards()
	Modules.SpBattleResultWinBase.__ShowExtraRewards( self )
	
	if self.extraRewards ~= true then
		return 
	end
	self.extraRewards = true

	self.Image_2:SetPosition(self.Image_2:GetPositionX() , self.Image_2:GetPositionY() + 80 )
	self.Text_SpecialRewardTitle:SetPosition(self.Text_SpecialRewardTitle:GetPositionX() , self.Text_SpecialRewardTitle:GetPositionY() + 80 )
	self.panel_extra_reward:SetPosition( self.panel_extra_reward:GetPositionX() , self.panel_extra_reward:GetPositionY() + 80 )
	
	Modules.SpBattleResultWinBase.__ShowDropRewards(self , self.panel_extra_reward)	
end