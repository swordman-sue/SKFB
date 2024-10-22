-- require "modules/battle/templ_common_reward" 
require "modules/battle/battle_win/sp_battle_result_win_base"

--[[
%% @module: 公会低级
%% @author: yjg
%% @created: 2018年12月17日10:25:48
--]]

Modules = Modules or {}

Modules.UnionLowResultWinView = Modules.UnionLowResultWinView or BaseClass( Modules.SpBattleResultWinBase )

function Modules.UnionLowResultWinView:__init()
end

function Modules.UnionLowResultWinView:__OpenCallback()
	Modules.SpBattleResultWinBase.__OpenCallback(self)
	
	self:__ShowTopInfo()
end

function Modules.UnionLowResultWinView:__Dispose()
end

--上方信息
function Modules.UnionLowResultWinView:__ShowTopInfo()
	Modules.SpBattleResultWinBase.__ShowTopInfo( self )
	if self.topInfo ~= true then
		return 
	end
	self.topInfo = false

	local image_top_title = self:GetWidget("Image_TopTitle")

	local battle_info = self:CallModuleFunc("GetCurInfo")

	if battle_info.is_win == 1 then
		image_top_title:SetImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE_RESULT_WIN, "img_win", true))
	else
		image_top_title:SetImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE_RESULT_WIN, "gameover", true))
	end

	local battle_defeat_invadeTipe = Util:RichText( 
											CPPGameLib.GetString("union_combat_residueHP" , battle_info.context1 .. "%" )  , 
											Macros.TypefaceSize.normal ,
											nil , nil , Macros.Color.white_hex
											)
	battle_defeat_invadeTipe:SetAnchorPoint( 0.5 , 0.5 )
	panel_top_info:AddChild( battle_defeat_invadeTipe )
	PosTool.Center( battle_defeat_invadeTipe , 0 , -15 )

	self:__ShowCommonRewards()
	--额外奖励
	self:__ShowExtraRewards()		
end

--战斗奖励
function Modules.UnionLowResultWinView:__ShowCommonRewards( end_callback )
	Modules.SpBattleResultWinBase.__ShowCommonRewards( self )

	if self.commonRewards ~= true then
		return true
	end
	self.commonRewards = false

	local battle_info = self:CallModuleFunc("GetCurInfo")
	self.Text_CommonRewardTitle:SetVisible(false)

	local starDetail = CallModuleFunc(ModuleType.UNION , "GetUnionCombatStarDetail" )
	local starCnf = CallModuleFunc(ModuleType.UNION , "GetGuildWarStarCnf" , starDetail.star_id )
	
	local id = starCnf.winner_reward_id_list[1]
	local num = starCnf.winner_reward_num_list[1]
	if battle_info.is_win ~= 1 then
		id = starCnf.loser_reward_id_list[1]
		num = starCnf.loser_reward_num_list[1]
	end

	local item = CPPGameLib.GetConfig("Item", id )
	
	local t1 = {
					{zi = CPPGameLib.GetString("union_combat_get" , item.name ) , dx = Macros.TypefaceSize.normal , ys = Macros.Color.Special } ,
			        {tp = Resource.PathTool.GetItemPath(item.res_id) , dx = 0.5 , jg = 5 },
			        {zi = "+ " .. starCnf.winner_reward_num_list[1] , dx = Macros.TypefaceSize.normal , jg = 5 , ys = Macros.Color.white }
			    }
	local union_combat_get = Util:LabCenter( t1 )
	union_combat_get:SetAnchorPoint( 0.5 , 0.5 )
	self.Image_1:AddChild( union_combat_get )
	PosTool.Center( union_combat_get , 0 , 20 )

	local ys = string.format( Macros.Quality_RT[starCnf.star_type] , starCnf.name )
	local text = CPPGameLib.GetString("union_combat_getIntegral", ys )
	local rt_text = string.format( Macros.Color.Special_rt , text )
	local num = string.format( Macros.Quality_RT[0] , battle_info.end_params_list[5] )
	local union_combat_getIntegral = Util:RichText( rt_text .. num , Macros.TypefaceSize.normal )
	union_combat_getIntegral:SetAnchorPoint( 0.5 , 0.5 )
	self.Image_1:AddChild( union_combat_getIntegral )
	PosTool.Center( union_combat_getIntegral , 0 , -20 )
end

--额外奖励
function Modules.UnionLowResultWinView:__ShowExtraRewards()
	Modules.SpBattleResultWinBase.__ShowExtraRewards( self )
	
	if self.extraRewards ~= true then
		return 
	end
	self.extraRewards = true

	self.Text_SpecialRewardTitle:SetVisible(false)
	self.panel_extra_reward:SetVisible(false)
	self.Image_2:SetVisible(false)

	self.__common_reward_finished = true
	self.__extra_reward_finished = true
	
	Modules.SpBattleResultWinBase.__CheckFinished( self )
end