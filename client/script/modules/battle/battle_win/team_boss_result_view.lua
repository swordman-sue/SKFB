-- require "modules/battle/templ_common_reward" 
require "modules/battle/battle_win/sp_battle_result_win_base"

--[[
%% @module: 组队普通战斗
%% @author: yjg
%% @created: 2018年12月13日16:56:22
--]]

Modules = Modules or {}

Modules.TeamBossResultWinView = Modules.TeamBossResultWinView or BaseClass( Modules.SpBattleResultWinBase )

function Modules.TeamBossResultWinView:__init()
end

function Modules.TeamBossResultWinView:__OpenCallback()
	Modules.SpBattleResultWinBase.__OpenCallback(self)
	
	self:__ShowTopInfo()
end

function Modules.TeamBossResultWinView:__Dispose()
end

--上方信息
function Modules.TeamBossResultWinView:__ShowTopInfo()
	Modules.SpBattleResultWinBase.__ShowTopInfo( self )
	if self.topInfo ~= true then
		return 
	end
	self.topInfo = false

	local battle_info = self:CallModuleFunc("GetCurInfo")

	local image_top_title = self:GetWidget("Image_TopTitle")
	if battle_info.is_win == 1 then  -- 赢了
	else   -- 输了
		image_top_title:SetImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE_RESULT_WIN, "gameover", true))
	end
	image_top_title:IgnoreContentAdaptWithSize(true)
	
	self:__ShowCommonRewards()
	self:__ShowExtraRewards()

	self.__common_reward_finished = true
	self.__extra_reward_finished = true

	Modules.SpBattleResultWinBase.__CheckFinished( self )
end

--战斗奖励
function Modules.TeamBossResultWinView:__ShowCommonRewards( end_callback )
	Modules.SpBattleResultWinBase.__ShowCommonRewards( self )
	if self.commonRewards ~= true then
		return true
	end
	self.commonRewards = false

	local battle_info = self:CallModuleFunc("GetCurInfo")

	self.Text_CommonRewardTitle:SetVisible(false)
	if battle_info.is_win == 1 then

		local battle_succeed_kill = Util:Label( CPPGameLib.GetString("battle_succeed_kill") , Macros.TypefaceSize.normal , Macros.Color.Special )
		battle_succeed_kill:SetAnchorPoint( 0.5 , 0.5 )
		self.panel_common_reward:AddChild( battle_succeed_kill )
		PosTool.Center( battle_succeed_kill , 0 , 19 )

		local tab = {
						{ zi = CPPGameLib.GetString("common_get_gold") , ys = Macros.Color.Special , dx = Macros.TypefaceSize.normal },
						{ tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.GOLD), dx = 0.8, jg = 3 },
						{ zi = battle_info.reward_currency_map[Macros.Game.RolePropType.GOLD] , jg = 5 , ys = Macros.Color.white , dx = Macros.TypefaceSize.normal },
					}
		local lab = Util:LabCenter( tab )
		lab:SetAnchorPoint( 0.5 , 0.5 )
		self.panel_common_reward:AddChild( lab )
		PosTool.Center( lab , 0 , -19 )
	else

		local total_damage = CallModuleFunc(ModuleType.BATTLE, "GetRealTotalDamage")

		local tab = {
						{ zi = CPPGameLib.GetString("common_do_damage") , ys = Macros.Color.Special , dx = Macros.TypefaceSize.normal },
						{ zi = total_damage , ys = Macros.Color.white , dx = Macros.TypefaceSize.normal },
					}
		local lab = Util:LabCenter( tab )
		lab:SetAnchorPoint( 0.5 , 0.5 )
		self.panel_common_reward:AddChild( lab )
		PosTool.Center( lab )
	end

end

--额外奖励
function Modules.TeamBossResultWinView:__ShowExtraRewards()
	Modules.SpBattleResultWinBase.__ShowExtraRewards( self )
	
	if self.extraRewards ~= true then
		return 
	end
	self.extraRewards = true

	self.panel_extra_reward:SetVisible(false)
	self.Text_SpecialRewardTitle:SetVisible(false)
	self.Image_2:SetVisible(false)
end