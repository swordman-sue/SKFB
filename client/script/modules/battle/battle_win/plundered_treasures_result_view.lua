-- require "modules/battle/templ_common_reward"
require "modules/battle/battle_win/sp_battle_result_win_base"

--[[
%% @module: 夺宝副本结算
%% @author: yjg
%% @created: 2018年12月13日11:26:38
--]]

Modules = Modules or {}

Modules.PlunderedTreasuresResultWinView = Modules.PlunderedTreasuresResultWinView or BaseClass( Modules.SpBattleResultWinBase )

function Modules.PlunderedTreasuresResultWinView:__init()
end

function Modules.PlunderedTreasuresResultWinView:__OpenCallback()
	Modules.SpBattleResultWinBase.__OpenCallback(self)
	
	self:__ShowTopInfo()
end

function Modules.PlunderedTreasuresResultWinView:__Dispose()
end

--上方信息
function Modules.PlunderedTreasuresResultWinView:__ShowTopInfo()
	Modules.SpBattleResultWinBase.__ShowTopInfo( self )
	if self.topInfo ~= true then
		return 
	end
	self.topInfo = false
	
	self.panel_top_info:SetVisible(false)

    --重设位置
	PosTool.MoveBy(self.Text_CommonRewardTitle, 0, -110)
	PosTool.MoveBy(self.panel_common_reward, 0, -110)
	PosTool.MoveBy(self.Image_1, 0, -110)
	self.Text_CommonRewardTitle:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE_RESULT_WIN, "award", true) )

	self:__ShowCommonRewards()
end

--战斗奖励
function Modules.PlunderedTreasuresResultWinView:__ShowCommonRewards( end_callback )
	Modules.SpBattleResultWinBase.__ShowCommonRewards( self )
	if self.commonRewards ~= true then
		return true
	end
	self.commonRewards = false

	local battle_info = self:CallModuleFunc("GetCurInfo")

	self.__templ_common_reward = Modules.TemplBattleCommonReward.New(
	{
		parent_node = self.panel_common_reward,
		add_gold = CPPGameLib.GetConfig("DuobaoMap", battle_info.pre_role_info.level , false).win_gold ,
		add_exp = CPPGameLib.GetConfig("DuobaoMap", battle_info.pre_role_info.level , false).exp ,
		time = 0.5,
		callback = function()
			Modules.SpBattleResultWinBase.CommonRewardFinishedCallback( self )
		end
	})	
	self.__templ_common_reward:SetPosition(-7, 0)
	self:__ShowExtraRewards()
end

--额外奖励
function Modules.PlunderedTreasuresResultWinView:__ShowExtraRewards()
	Modules.SpBattleResultWinBase.__ShowExtraRewards( self )
	
	if self.extraRewards ~= true then
		return 
	end
	self.extraRewards = true

	local battle_info = self:CallModuleFunc("GetCurInfo")

	self.Image_2:SetVisible(false)
	self.Text_SpecialRewardTitle:SetVisible(false)
	
	PosTool.MoveBy(self.panel_extra_reward, 0, 180 )

	local has_fragment = battle_info.end_params_list[1]
	if has_fragment == Macros.Global.TRUE then

		local fragment_id = battle_info.param4
		local config_item = CPPGameLib.GetConfig("Item", fragment_id)

		--有碎片
        local lab = Util:LabCenter({
        							{zi = CPPGameLib.GetString("battle_result_extra_reward_tips1"), ys = Macros.Color.blinkcontent , dx = Macros.TypefaceSize.popup },
        							{zi = config_item.name, ys = Macros.Quality[config_item.quality], jg = 4 , dx = Macros.TypefaceSize.popup },
        						})
		lab:SetAnchorPoint(0.5, 1)
		self.panel_extra_reward:AddChild(lab)
		PosTool.CenterTop(lab, 0 , 10)		


		CPPActionManager.FadeOut(lab , 0 ,GlobalCallbackMgr:AddCallBackFunc(function ( ... )
			CPPActionManager.FadeIn(lab , 1 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
			end) )
		end) )

		local icon_layer = LogicGUI.IconLayer.New(false, false)
		icon_layer:SetAnchorPoint(0.5, 0.5	)
		icon_layer:SetData({id = fragment_id, num = 1})
		icon_layer:PlayAppearEffect()
		self.panel_extra_reward:AddChild(icon_layer:GetNode())
		PosTool.Center(icon_layer:GetNode() , 0 , -10)	

	else
		--无碎片
		local lab = Util:Label(CPPGameLib.GetString("battle_result_extra_reward_tips2"), Macros.TypefaceSize.tab , Macros.Color.blinkcontent)
		lab:SetAnchorPoint(0.5, 1)
		self.panel_extra_reward:AddChild(lab)
		PosTool.Center(lab)		

		CPPActionManager.FadeOut(lab , 0 ,GlobalCallbackMgr:AddCallBackFunc(function ( ... )
			CPPActionManager.FadeIn(lab , 1 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
			end) )
		end) )

	end

	self.__extra_reward_finished = true
	Modules.SpBattleResultWinBase.__CheckFinished(self)	
end