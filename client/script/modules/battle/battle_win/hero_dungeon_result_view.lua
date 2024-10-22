require "modules/battle/templ_common_reward"
require "modules/battle/battle_win/sp_battle_result_win_base"

--[[
%% @module: 英雄副本结算
%% @author: yjg
%% @created: 2018年12月12日17:07:59
--]]

Modules = Modules or {}

Modules.HeroDungeonResultWinView = Modules.HeroDungeonResultWinView or BaseClass( Modules.SpBattleResultWinBase )

function Modules.HeroDungeonResultWinView:__init()
end

function Modules.HeroDungeonResultWinView:__OpenCallback()
	Modules.SpBattleResultWinBase.__OpenCallback(self)
	
	self:__ShowTopInfo()
end

function Modules.HeroDungeonResultWinView:__Dispose()
end

--上方信息
function Modules.HeroDungeonResultWinView:__ShowTopInfo()
	Modules.SpBattleResultWinBase.__ShowTopInfo( self )
	if self.topInfo ~= true then
		return 
	end
	self.topInfo = false

	self.panel_top_info:SetVisible(false)

	local battle_info = self:CallModuleFunc("GetCurInfo")

	if CPPGameLib.GetConfig("HeroFBChapter", battle_info.param1).extra_map_id ~= 0 then
		if battle_info.gamelevel_id ~= CPPGameLib.GetConfig("HeroFBChapter", battle_info.param1).extra_map_id then
			self:__ShowCommonRewards()
		else
			self.panel_common_reward:SetVisible(false)
			self.Text_CommonRewardTitle:SetVisible(false)
			self.Image_1:SetVisible(false)

			local panel_extra_reward = self:GetWidget("Panel_SpecialReward")
			local Text_SpecialRewardTitle = self:GetWidget("Text_SpecialRewardTitle")
			local Image_2 = self:GetWidget("Image_2")

			Image_2:SetPosition(Image_2:GetPositionX() , Image_2:GetPositionY() + 80 )
			Text_SpecialRewardTitle:SetPosition(Text_SpecialRewardTitle:GetPositionX() , Text_SpecialRewardTitle:GetPositionY() + 80 )
			panel_extra_reward:SetPosition( panel_extra_reward:GetPositionX() , panel_extra_reward:GetPositionY() + 80 )
			
			self:__ShowCommonRewards()
		end
	else
		self:__ShowCommonRewards()
	end
end

--战斗奖励
function Modules.HeroDungeonResultWinView:__ShowCommonRewards()
	Modules.SpBattleResultWinBase.__ShowCommonRewards( self )
	if self.commonRewards ~= true then
		return true
	end
	self.commonRewards = false

	local battle_info = self:CallModuleFunc("GetCurInfo")

	self.__templ_common_reward = Modules.TemplBattleCommonReward.New(
	{
		parent_node = self.panel_common_reward,
		add_gold = battle_info.reward_currency_map[ Macros.Game.RolePropType.GOLD ],
		add_exp =  battle_info.reward_currency_map[ Macros.Game.RolePropType.EXP ],
		time = 0.5,
		callback = function()
			Modules.SpBattleResultWinBase.CommonRewardFinishedCallback( self )
		end
	})	
	self.__templ_common_reward:SetPosition(-7, 0)
	self:__ShowExtraRewards()
end

--额外奖励
function Modules.HeroDungeonResultWinView:__ShowExtraRewards()
	Modules.SpBattleResultWinBase.__ShowExtraRewards( self )
	
	if self.extraRewards ~= true then
		return 
	end
	self.extraRewards = true
	
	Modules.SpBattleResultWinBase.__ShowDropRewards( self , self.panel_extra_reward , self:CallModuleFunc("GetDropList") )
	self.__extra_reward_finished = true
	Modules.SpBattleResultWinBase.__CheckFinished( self )
end