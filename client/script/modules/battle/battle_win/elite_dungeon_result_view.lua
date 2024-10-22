require "modules/battle/templ_common_reward"
require "modules/battle/battle_win/sp_battle_result_win_base"

--[[
%% @module: 精英副本结算
%% @author: yjg
%% @created: 2018年12月12日17:54:59
--]]

Modules = Modules or {}

Modules.EliteDungeonResultWinView = Modules.EliteDungeonResultWinView or BaseClass( Modules.SpBattleResultWinBase )

function Modules.EliteDungeonResultWinView:__init()
end

function Modules.EliteDungeonResultWinView:__OpenCallback()
	Modules.SpBattleResultWinBase.__OpenCallback(self)
	
	self:__ShowTopInfo()
end

function Modules.EliteDungeonResultWinView:__Dispose()
end

--上方信息
function Modules.EliteDungeonResultWinView:__ShowTopInfo()
	Modules.SpBattleResultWinBase.__ShowTopInfo( self )
	if self.topInfo ~= true then
		return 
	end
	self.topInfo = false
	
	local battle_info = self:CallModuleFunc("GetCurInfo")

	local eliteTranscriptTidy = CallModuleFunc(ModuleType.DUNGEON, "GetEliteTranscriptTidy" , battle_info.gamelevel_id )

	--星
	Modules.SpBattleResultWinBase.__ShowStarDesc(self)

	if not eliteTranscriptTidy then

		--星级评价
		local tab = {
						{ zi = CPPGameLib.GetString("battle_result_star_desc_title") , ys = Macros.Color.golden , dx = Macros.TypefaceSize.popup },
						{ zi = battle_info.star .. CPPGameLib.GetString("battle_star") , ys = Macros.Color.white , dx = Macros.TypefaceSize.popup },
						{ zi = CPPGameLib.GetString("battle_clearance_time",battle_info.cost_time ) , ys = Macros.Color.white , dx = Macros.TypefaceSize.popup }
					}
		local Text_WinDescTitle = Util:LabCenter( tab )
		Text_WinDescTitle:SetAnchorPoint( 0.5 , 0.5 )
		self.panel_top_info:AddChild( Text_WinDescTitle )
		PosTool.Center( Text_WinDescTitle )
		
	else

		local battle_defeat_invade = Util:Label( 
													CPPGameLib.GetString("battle_defeat_invade") , 
													Macros.TypefaceSize.normal ,
													Macros.Color.white
												)
		battle_defeat_invade:SetAnchorPoint( 0.5 , 0.5 )
		self.panel_top_info:AddChild( battle_defeat_invade )
		PosTool.Center( battle_defeat_invade , 0 , 10 )

		local cnf = CPPGameLib.GetConfig("GameLevel", battle_info.gamelevel_id )

		local config_monster = CPPGameLib.GetConfig("Battle", cnf.battle_list[1] )
		
		local monster = CPPGameLib.GetConfig("Monster", config_monster.representativeId )

		local text = string.format( Macros.Quality_RT[monster.quality]  , monster.name )
		
		local battle_defeat_invadeTipe = Util:RichText( 
													CPPGameLib.GetString("battle_defeat_invadeTipe" , text ) , 
													Macros.TypefaceSize.normal ,
													nil , nil , Macros.Color.white_hex
													)
		battle_defeat_invadeTipe:SetAnchorPoint( 0.5 , 0.5 )
		self.panel_top_info:AddChild( battle_defeat_invadeTipe )
		PosTool.Center( battle_defeat_invadeTipe , 0 , -15 )
	end

	self:__ShowCommonRewards()
end

--战斗奖励
function Modules.EliteDungeonResultWinView:__ShowCommonRewards()
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
	self.extraRewards = true
	self:__ShowExtraRewards()
end

--额外奖励
function Modules.EliteDungeonResultWinView:__ShowExtraRewards()
	Modules.SpBattleResultWinBase.__ShowExtraRewards( self )
	
	if self.extraRewards ~= true then
		return 
	end
	self.extraRewards = true

	self.__extra_reward_finished = true
	Modules.SpBattleResultWinBase.__ShowDropRewards( self , self.panel_extra_reward )
	Modules.SpBattleResultWinBase.__CheckFinished( self )
end