-- require "modules/battle/templ_common_reward" 
require "modules/battle/battle_win/sp_battle_result_win_base"

--[[
%% @module: 积分赛结算
%% @author: yjg
%% @created: 2018年12月13日15:46:43
--]]

Modules = Modules or {}

Modules.IntegralRaceResultWinView = Modules.IntegralRaceResultWinView or BaseClass( Modules.SpBattleResultWinBase )

function Modules.IntegralRaceResultWinView:__init()
end

function Modules.IntegralRaceResultWinView:__OpenCallback()
	Modules.SpBattleResultWinBase.__OpenCallback(self)
	
	self:__ShowTopInfo()
end

function Modules.IntegralRaceResultWinView:__Dispose()
end

--上方信息
function Modules.IntegralRaceResultWinView:__ShowTopInfo()
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
function Modules.IntegralRaceResultWinView:__ShowCommonRewards( end_callback )
	Modules.SpBattleResultWinBase.__ShowCommonRewards( self )
	if self.commonRewards ~= true then
		return true
	end
	self.commonRewards = false

	local battle_info = self:CallModuleFunc("GetCurInfo")

	local integral = ""
	local is_win = CallModuleFunc(ModuleType.INTEGRAL_RACE, "GetISWin" )
	if is_win == true then 
		integral = CPPGameLib.GetString("integral_win")
	else
		integral = CPPGameLib.GetString("integral_lose")
		local image_top_title = self:GetWidget("Image_TopTitle")
		image_top_title:SetImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE_RESULT_WIN, "gameover", true))
		image_top_title:IgnoreContentAdaptWithSize(true)
	end

	CPPActionManager.FadeOut(self.panel_common_reward , 0)
	self.Text_CommonRewardTitle:SetVisible(false)

	local integral_win = Util:Label( integral , Macros.TypefaceSize.normal , Macros.Color.Special )
	integral_win:SetAnchorPoint( 0.5 , 0.5 )
	self.panel_common_reward:AddChild( integral_win )
	PosTool.Center( integral_win , 0 , 23 )

	local num = CallModuleFunc(ModuleType.ACTIVITY,"GetTimedOutput" , Macros.Game.FBType.TOWER_COMMON )
	local battle_double = ""
	if num ~= 1 then
		battle_double = CPPGameLib.GetString("battle_double")
	end

	--获得荣誉
	lab_num = Util:Label("" , Macros.TypefaceSize.popup )
	lab_num:SetAnchorPoint(0, 0.5)
	lab_num:SetPosition( self.panel_common_reward:GetContentWidth()/2 - 40 , self.panel_common_reward:GetContentHeight()/2 - 20 )
	self.panel_common_reward:AddChild(lab_num)
	Modules.SpBattleResultWinBase.__AddNumAdditionAnim(self , lab_num, 0, battle_info.end_params_list[2] or 0, 0.5, function()
		Modules.SpBattleResultWinBase.CommonRewardFinishedCallback( self )
	end)	

	--获得荣誉
   	local lab_title = Util:LabCenter({
   		        				{zi = CPPGameLib.GetString("worldBoss_honor"), ys = Macros.Color.Special , dx = Macros.TypefaceSize.popup },
    							{tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.SCORE_MATCH_HONOR), dx = 0.8 , jg = -5 },
    							{zi = battle_double, ys = Macros.Color.green , dx = Macros.TypefaceSize.popup },
   		        				
				        	})		
	lab_title:SetAnchorPoint( 1 , 0.5 )
	lab_title:SetPosition( lab_num:GetPositionX() + lab_num:GetContentWidth() , lab_num:GetPositionY() )
	self.panel_common_reward:AddChild(lab_title)

	--积分标题
   	local lab_title = Util:LabCenter({
								{zi = CPPGameLib.GetString("pvp_integral_"), ys = Macros.Color.Special , dx = Macros.TypefaceSize.popup },
				        	})		
	lab_title:SetAnchorPoint( 0 , 0.5 )
	lab_title:SetPosition( self.panel_common_reward:GetContentWidth()/2 + 40 , self.panel_common_reward:GetContentHeight()/2 - 20 )
	self.panel_common_reward:AddChild(lab_title)

	local prestige = battle_info.end_params_list[1] or 0
	--获得积分
	local lab_num = Util:Label( prestige * num , Macros.TypefaceSize.popup )
	lab_num:SetAnchorPoint( 0 , 0.5)
	lab_num:SetPosition( lab_title:GetContentWidth() + lab_title:GetPositionX() , lab_title:GetPositionY() )
	self.panel_common_reward:AddChild(lab_num)
	Modules.SpBattleResultWinBase.__AddNumAdditionAnim(self , lab_num, 0, prestige * num , 0.5, function()
		--重置
		CallModuleFunc(ModuleType.INTEGRAL_RACE, "GetISWin" , false )
		Modules.SpBattleResultWinBase.CommonRewardFinishedCallback( self )
	end)	

	CPPActionManager.FadeIn(self.panel_common_reward , 0.01)
	CPPActionManager.MoveTo(self.panel_common_reward, 0.2 , Image_1:GetPositionX() + 7 , Image_1:GetPositionY() , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
		CPPActionManager.MoveTo(self.panel_common_reward, 0.3 , Image_1:GetPositionX() , Image_1:GetPositionY(),  GlobalCallbackMgr:AddCallBackFunc(function ( ... )
		end) )
	end) )
end

--额外奖励
function Modules.IntegralRaceResultWinView:__ShowExtraRewards()
	Modules.SpBattleResultWinBase.__ShowExtraRewards( self )
	
	if self.extraRewards ~= true then
		return 
	end
	self.extraRewards = true

	self.panel_extra_reward:SetVisible(false)
	self.Text_SpecialRewardTitle:SetVisible(false)
	self.Image_2:SetVisible(false)
	self.__extra_reward_finished = true
end