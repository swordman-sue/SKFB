-- require "modules/battle/templ_common_reward" 
require "modules/battle/battle_win/sp_battle_result_win_base"

--[[
%% @module: 公会结算
%% @author: yjg
%% @created: 2018年12月13日15:52:36
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

	local image_top_title = self:GetWidget("Image_TopTitle")
	image_top_title:SetImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE_RESULT_WIN, "gameover", true))
	image_top_title:IgnoreContentAdaptWithSize(true)

	self:__ShowCommonRewards()
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
	self.Text_CommonRewardTitle:SetPosition( self.Text_CommonRewardTitle:GetPositionX() , self.Text_CommonRewardTitle:GetPositionY() - 80 )
	self.panel_common_reward:SetPosition( self.panel_common_reward:GetPositionX() , self.panel_common_reward:GetPositionY() - 80 )
	self.Image_1:SetPosition( self.Image_1:GetPositionX() , self.Image_1:GetPositionY() - 80 )

	local total_damage = CallModuleFunc(ModuleType.BATTLE, "GetTotalDamage")

	--伤害标题
	local lab_title = Util:Label(CPPGameLib.GetString("common_do_damage"), nil, Macros.Color.golden)
	lab_title:SetAnchorPoint(0, 0.5)
	lab_title:SetPosition( 40 , self.panel_common_reward:GetContentHeight()/2 )
	self.panel_common_reward:AddChild(lab_title)

	--造成伤害
	local lab_num = Util:Label(0)
	self.panel_common_reward:AddChild(lab_num)
	PosTool.RightTo(lab_title, lab_num, 4 )		
	Modules.SpBattleResultWinBase.__AddNumAdditionAnim( self , lab_num, 0, total_damage, 0.5, function()
		harm_finished = true
	end)

	--战功标题
   	local lab_title = Util:LabCenter({
    							{zi = CPPGameLib.GetString("currency_get_20").. CPPGameLib.GetString("common_colon"), ys = Macros.Color.golden},
    							{tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.GUILD_CONTRIBUTION), dx = 0.8, jg = 6},
				        	})		
	lab_title:SetAnchorPoint(0, 0.5)
	lab_title:SetPosition( self.panel_common_reward:GetContentWidth()/2 + 40 , self.panel_common_reward:GetContentHeight()/2 )
	self.panel_common_reward:AddChild(lab_title)
	
	local section = CallModuleFunc(ModuleType.UNION, "GetNowAttackSection")	
	local fbcnf = CPPGameLib.GetConfig("GuildFB", section )

	--获得战功
	lab_num = Util:Label(0)
	lab_num:SetAnchorPoint(0, 0.5)
	self.panel_common_reward:AddChild(lab_num)
	PosTool.RightTo(lab_title, lab_num, 4)	
	Modules.SpBattleResultWinBase.__AddNumAdditionAnim( self , lab_num, 0, fbcnf.battle_contribution , 0.5, function()
		battle_achievement_finished = true
		Modules.SpBattleResultWinBase.CommonRewardFinishedCallback( self )
	end)	

	CPPActionManager.FadeIn(self.panel_common_reward , 0.2)
	CPPActionManager.MoveTo(self.panel_common_reward, 0.1 , self.Image_1:GetPositionX() + 7 , self.Image_1:GetPositionY() , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
		CPPActionManager.MoveTo(self.panel_common_reward, 0.2 , self.Image_1:GetPositionX() , self.Image_1:GetPositionY(),  GlobalCallbackMgr:AddCallBackFunc(function ( ... )
		end) )
	end) )

	self.extraRewards = true

	self:__ShowExtraRewards()
end

--额外奖励
function Modules.UnionResultWinView:__ShowExtraRewards()
	Modules.SpBattleResultWinBase.__ShowExtraRewards( self )
	
	if self.extraRewards ~= true then
		return 
	end
	self.extraRewards = true

	self.Text_SpecialRewardTitle:SetVisible(false)
	self.panel_extra_reward:SetVisible(false)
	self.Image_2:SetVisible(false)
	self.__extra_reward_finished = true
	Modules.SpBattleResultWinBase.__CheckFinished( self )
end