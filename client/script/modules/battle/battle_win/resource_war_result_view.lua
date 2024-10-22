-- require "modules/battle/templ_common_reward" 
require "modules/battle/battle_win/sp_battle_result_win_base"

--[[
%% @module: 资源赛
%% @author: yjg
%% @created: 2018年12月13日17:59:26
--]]

Modules = Modules or {}

Modules.UnionResultWinView = Modules.UnionResultWinView or BaseClass( Modules.SpBattleResultWinBase )

function Modules.UnionResultWinView:__init()
end

function Modules.UnionResultWinView:__OpenCallback()
	Modules.SpBattleResultWinBase.__OpenCallback(self)
	
	self:__ShowTopInfo()
end

function Modules.UnionResultWinView:__Dispose()
end

--上方信息
function Modules.UnionResultWinView:__ShowTopInfo()
	Modules.SpBattleResultWinBase.__ShowTopInfo( self )
	if self.topInfo ~= true then
		return 
	end
	self.topInfo = false

	local text = CPPGameLib.GetString("resources_war_challenge_")
	local pitchOnData = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetPitchOnData" )
	--打的是人
	if pitchOnData.role_name  then
		text = CPPGameLib.GetString("resources_war_challenge" , pitchOnData.role_name .. ".s" .. pitchOnData.server_id )
	end

	local battle_defeat_invadeTipe = Util:RichText( 
												text , 
												Macros.TypefaceSize.normal ,
												nil , nil , Macros.Color.white_hex
												)
	battle_defeat_invadeTipe:SetAnchorPoint( 0.5 , 0.5 )
	self.panel_top_info:AddChild( battle_defeat_invadeTipe )
	PosTool.Center( battle_defeat_invadeTipe , 0 , -15 )

	self.__common_reward_finished = true
	self.__extra_reward_finished = true

	self:__ShowCommonRewards()
	--额外奖励
	self:__ShowExtraRewards()	

end

--战斗奖励
function Modules.UnionResultWinView:__ShowCommonRewards( end_callback )
	Modules.SpBattleResultWinBase.__ShowCommonRewards( self )
	if self.commonRewards ~= true then
		return true
	end
	self.commonRewards = false

	local battle_info = self:CallModuleFunc("GetCurInfo")

	self.Text_CommonRewardTitle:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE_RESULT_WIN, "award", true) )
	self.Image_1:SetPosition( self.Image_1:GetPositionX() , self.Image_1:GetPositionY() - 80 )
	self.Text_CommonRewardTitle:SetPosition( self.Text_CommonRewardTitle:GetPositionX() , self.Text_CommonRewardTitle:GetPositionY() - 80 )
	panel_common_reward:SetPosition( panel_common_reward:GetPositionX() , panel_common_reward:GetPositionY() - 80 )

	local resourceCnf = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetResourceWarResourceCnf", battle_info.param2 )

	local text = CPPGameLib.GetString( "resources_war_getOccupyRight" , string.format( Macros.Quality_Stroke_RT[resourceCnf.resource_type - 1] , resourceCnf.name ) )
	local resources_war_getOccupyRight = Util:RichText( text )
	resources_war_getOccupyRight:SetAnchorPoint( 0.5 , 0.5 )
	self.Image_1:AddChild( resources_war_getOccupyRight )
	PosTool.Center( resources_war_getOccupyRight )
end

--额外奖励
function Modules.UnionResultWinView:__ShowExtraRewards()
	Modules.SpBattleResultWinBase.__ShowExtraRewards( self )
	
	if self.extraRewards ~= true then
		return 
	end
	self.extraRewards = true

	self.panel_extra_reward:SetVisible(false)
	self.Text_SpecialRewardTitle:SetVisible(false)
	self.Image_2:SetVisible(false)	

	Modules.SpBattleResultWinBase.__CheckFinished( self )
end