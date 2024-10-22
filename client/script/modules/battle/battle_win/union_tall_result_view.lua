-- require "modules/battle/templ_common_reward" 
require "modules/battle/battle_win/sp_battle_result_win_base"

--[[
%% @module: 公会高级结算
%% @author: yjg
%% @created: 2018年12月17日10:37:02
--]]

Modules = Modules or {}

Modules.UnionTallResultWinView = Modules.UnionTallResultWinView or BaseClass( Modules.SpBattleResultWinBase )

function Modules.UnionTallResultWinView:__init()
end

function Modules.UnionTallResultWinView:__OpenCallback()
	Modules.SpBattleResultWinBase.__OpenCallback(self)
	
	self:__ShowTopInfo()
end

function Modules.UnionTallResultWinView:__Dispose()
end

--上方信息
function Modules.UnionTallResultWinView:__ShowTopInfo()
	Modules.SpBattleResultWinBase.__ShowTopInfo( self )
	if self.topInfo ~= true then
		return 
	end
	self.topInfo = false

	local image_top_title = self:GetWidget("Image_TopTitle")
	if battle_info.is_win == 1 then
		image_top_title:SetImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE_RESULT_WIN, "img_win", true))
	else
		image_top_title:SetImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE_RESULT_WIN, "gameover", true))
	end

	self:__ShowCommonRewards()
	--额外奖励
	self:__ShowExtraRewards()
end

--战斗奖励
function Modules.UnionTallResultWinView:__ShowCommonRewards( end_callback )
	Modules.SpBattleResultWinBase.__ShowCommonRewards( self )
	if self.commonRewards ~= true then
		return true
	end
	self.commonRewards = false

	local battle_info = self:CallModuleFunc("GetCurInfo")

	self.Text_CommonRewardTitle:SetVisible(false)

	local union_combat_residueHP = Util:RichText( CPPGameLib.GetString("union_combat_residueHP" , battle_info.context1.."%" ))
	union_combat_residueHP:SetAnchorPoint( 0.5 , 0.5 )
	self.Image_1:AddChild( union_combat_residueHP )
	PosTool.Center( union_combat_residueHP , 0 , 25 )

	local union_combat_residueLast = Util:RichText( CPPGameLib.GetString("union_combat_residueLast" , battle_info.end_params_list[5] ))
	union_combat_residueLast:SetAnchorPoint( 0.5 , 0.5 )
	self.Image_1:AddChild( union_combat_residueLast )
	PosTool.Center( union_combat_residueLast )

	if battle_info.is_win == 1 then 
		local name = ""
		local server = ""
		local attackObject = CallModuleFunc(ModuleType.UNION , "GetAttackObject")
		name = attackObject.name
		if attackObject.server then
			server = attackObject.server
		end

		local union_combat_defenseTime = Util:RichText( "S"..server.."." .. CPPGameLib.GetString("union_combat_defenseTime", name ) )
		union_combat_defenseTime:SetAnchorPoint( 0.5 , 0.5 )
		self.Image_1:AddChild( union_combat_defenseTime )
		PosTool.Center( union_combat_defenseTime , 0 , -25 )
	else
		PosTool.Center( union_combat_residueHP , 0 , 20 )
		PosTool.Center( union_combat_residueLast , 0 , -20 )
	end
end

--额外奖励
function Modules.UnionTallResultWinView:__ShowExtraRewards()
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