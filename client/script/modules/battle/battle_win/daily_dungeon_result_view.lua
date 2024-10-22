require "modules/battle/battle_win/sp_battle_result_win_base"

--[[
%% @module: 日常副本结算
%% @author: yjg
%% @created: 2018年12月12日15:26:12
--]]

Modules = Modules or {}

Modules.DailyDungeonResultWinView = Modules.DailyDungeonResultWinView or BaseClass( Modules.SpBattleResultWinBase )

function Modules.DailyDungeonResultWinView:__init()
end

function Modules.DailyDungeonResultWinView:__OpenCallback()
	Modules.SpBattleResultWinBase.__OpenCallback(self)
	
	self:__ShowTopInfo()
end

function Modules.DailyDungeonResultWinView:__Dispose()
end

--上方信息
function Modules.DailyDungeonResultWinView:__ShowTopInfo()
	Modules.SpBattleResultWinBase.__ShowTopInfo( self )
	if self.topInfo ~= true then
		return 
	end
	self.topInfo = false

	--战斗奖励
	self:__ShowCommonRewards()
end

--战斗奖励
function Modules.DailyDungeonResultWinView:__ShowCommonRewards()
	Modules.SpBattleResultWinBase.__ShowCommonRewards( self )
	if self.commonRewards ~= true then
		return true
	end
	self.commonRewards = false

	self.Text_CommonRewardTitle:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE, "img_evaluate", true) )

	local __open_data = self.__open_data[1]
	local content , tips

	--我方剩余血量不低于x%
	if __open_data.type == Macros.Game.BattleWinCondition.MY_HP_MORE_THAN then
		content = CPPGameLib.GetString( "battle_tower_daily_HP_name" , __open_data.data )
		tips = "(" .. CPPGameLib.GetString( "battle_tower_daily_HP_tips" ) .. ")"

	--x秒内获胜
	elseif __open_data.type == Macros.Game.BattleWinCondition.TIME_LESS_THAN then		
		content = CPPGameLib.GetString( "battle_tower_daily_time_name" , __open_data.data )
		tips = "(" .. CPPGameLib.GetString( "battle_tower_daily_time_tips" ) .. ")"

	--时间结束之后根据伤害算奖励
	elseif __open_data.type == Macros.Game.BattleWinCondition.KILL_ALL_ENEMY then
		content =  CPPGameLib.GetString( "battle_tower_daily_harm_name" , string.format( "%.2f", tonumber(__open_data.data) ) )
		tips = "(" .. CPPGameLib.GetString( "battle_tower_daily_harm_tips" ) .. ")"
	--常规
	elseif __open_data.type == Macros.Game.BattleWinCondition.NORMAL then
		content = " "
		tips = " "

		self.panel_common_reward:SetVisible(false)
		self.Text_CommonRewardTitle:SetVisible(false)
		self.Image_1:SetVisible(false)

		local Image_2 = self:GetWidget("Image_2")
		Image_2:SetPosition(Image_2:GetPositionX() , Image_2:GetPositionY() + 80 )

		local Text_SpecialRewardTitle = self:GetWidget("Text_SpecialRewardTitle")
		Text_SpecialRewardTitle:SetPosition(Text_SpecialRewardTitle:GetPositionX() , Text_SpecialRewardTitle:GetPositionY() + 80 )
		
		local panel_extra_reward = self:GetWidget("Panel_SpecialReward")
		panel_extra_reward:SetPosition( panel_extra_reward:GetPositionX() , panel_extra_reward:GetPositionY() + 80 )
	end

	local contentT = Util:Label( content , Macros.TypefaceSize.largish , Macros.Color.white )
	contentT:SetAnchorPoint(0.5 , 1)
	self.panel_common_reward:AddChild( contentT )
	PosTool.CenterTop(contentT , 0 , -15)

	local tipsT = Util:Label( tips , Macros.TypefaceSize.normal , Macros.Color.Special )
	tipsT:SetAnchorPoint(0.5 , 0)
	self.panel_common_reward:AddChild( tipsT )
	PosTool.CenterBottom(tipsT , 0 , 15)

	self:__ShowExtraRewards()

	Modules.SpBattleResultWinBase.CommonRewardFinishedCallback( self )
end

--额外奖励
function Modules.DailyDungeonResultWinView:__ShowExtraRewards()
	Modules.SpBattleResultWinBase.__ShowExtraRewards( self )
	
	if self.extraRewards ~= true then
		return 
	end
	self.extraRewards = true
	
	Modules.SpBattleResultWinBase.__ShowDropRewards( self , self.panel_extra_reward)
	Modules.SpBattleResultWinBase.__CheckFinished( self )
end