-- require "modules/battle/templ_common_reward"
require "modules/battle/battle_win/sp_battle_result_win_base"

--[[
%% @module: 爬塔普通结算
%% @author: yjg
%% @created: 2018年12月12日18:03:52
--]]

Modules = Modules or {}

Modules.TowerCommonResultWinView = Modules.TowerCommonResultWinView or BaseClass( Modules.SpBattleResultWinBase )

function Modules.TowerCommonResultWinView:__init()
end

function Modules.TowerCommonResultWinView:__OpenCallback()
	Modules.SpBattleResultWinBase.__OpenCallback(self)
	
	self:__ShowExtraRewards()
	self:__ShowTopInfo()
end

function Modules.TowerCommonResultWinView:__Dispose()
end

--上方信息
function Modules.TowerCommonResultWinView:__ShowTopInfo()
	Modules.SpBattleResultWinBase.__ShowTopInfo( self )
	if self.topInfo ~= true then
		return 
	end
	self.topInfo = false
	
	local battle_info = self:CallModuleFunc("GetCurInfo")

	local str_condition = CPPGameLib.GetString("tower_clearance_4")
	local tower_game_level = CallModuleFunc(ModuleType.TOWER, "GetCurMapLevel")
	local config_tower_game_level = CPPGameLib.GetConfig("TowerGameLevel", tower_game_level)

	if config_tower_game_level and config_tower_game_level.win_condition then
		local c_type = config_tower_game_level.win_condition[1]
		local c_param = config_tower_game_level.win_condition[2]
		str_condition = CPPGameLib.GetString("tower_clearance_"..c_type, c_param)
	end
    local lab = Util:LabCenter({
    							{zi = CPPGameLib.GetString("battle_tower_result_info"), dx = Macros.TypefaceSize.popup , ys = Macros.Color.golden},
    							{zi = str_condition, ys = Macros.Color.green , dx = Macros.TypefaceSize.popup , jg = 8},
				        	})
	lab:SetAnchorPoint(0.5, 0.5)
    self.panel_top_info:AddChild(lab)
    PosTool.Center(lab)
    self.panel_top_info:SetPosition( self.panel_top_info:GetPositionX() , self.panel_top_info:GetPositionY() - 60 )

    --重设位置
	--隐藏战斗奖励		
	self.Text_CommonRewardTitle:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE_RESULT_WIN, "award", true) )
	self.Text_CommonRewardTitle:SetPosition( self.Text_CommonRewardTitle:GetPositionX() , self.Text_CommonRewardTitle:GetPositionY() - 80 )
	CPPActionManager.FadeOut(self.Text_CommonRewardTitle , 0 )
	
	self.Image_1:SetPosition( self.Image_1:GetPositionX() , self.Image_1:GetPositionY() - 80 )
	CPPActionManager.FadeOut(self.Image_1 , 0 )

	self.panel_common_reward:SetPosition(self.panel_common_reward:GetPositionX() , self.panel_common_reward:GetPositionY() - 80 )
	CPPActionManager.FadeOut(self.panel_common_reward , 0 )


	CPPActionManager.FadeOut(lab , 0 ,GlobalCallbackMgr:AddCallBackFunc(function ( ... )
		CPPActionManager.FadeIn(lab , 0.8 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )

			--显示战斗奖励
			CPPActionManager.FadeIn(self.Text_CommonRewardTitle , 0 )
			CPPActionManager.FadeIn(self.Image_1 , 0 )

			CPPActionManager.FadeIn(lab , 0.8  , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
				--战斗奖励
				self:__ShowCommonRewards(function ( ... )
				end)
			end) )

		end) )
	end) )
end

--战斗奖励
function Modules.TowerCommonResultWinView:__ShowCommonRewards( end_callback )
	Modules.SpBattleResultWinBase.__ShowCommonRewards( self )
	if self.commonRewards ~= true then
		return true
	end
	self.commonRewards = false
	
	local battle_info = self:CallModuleFunc("GetCurInfo")

	local prestige_finished, gold_finished
	local function check_finished()
		if prestige_finished and gold_finished then
			Modules.SpBattleResultWinBase.CommonRewardFinishedCallback( self , end_callback )
		end
	end

	CPPActionManager.FadeOut(self.panel_common_reward , 0)

	local num = CallModuleFunc(ModuleType.ACTIVITY,"GetTimedOutput" , Macros.Game.FBType.TOWER_COMMON )
	local battle_double = ""
	if num ~= 1 then
		battle_double = CPPGameLib.GetString("battle_double")
	end

	local prestige = battle_info.reward_currency_map[Macros.Game.RolePropType.TOWER_PRESTIGE] or 0
	--获得威名
	local lab_num = Util:Label( prestige * num , Macros.TypefaceSize.popup )
	lab_num:SetAnchorPoint( 1 , 0.5)
	lab_num:SetPosition( self.panel_common_reward:GetContentWidth()/2 - 40 , self.panel_common_reward:GetContentHeight()/2 )
	self.panel_common_reward:AddChild(lab_num)

	Modules.SpBattleResultWinBase.__AddNumAdditionAnim(self , lab_num, 0, prestige * num , 0.5, function()
		prestige_finished = true
		check_finished()
	end)	

	--威名标题
   	local lab_title = Util:LabCenter({
    							{zi = CPPGameLib.GetString("common_get_arena_prestige"), ys = Macros.Color.Special , dx = Macros.TypefaceSize.popup },
    							{tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.TOWER_PRESTIGE), dx = 0.8 , jg = -5 },
    							{zi = battle_double, ys = Macros.Color.green , dx = Macros.TypefaceSize.popup },
				        	})		
	lab_title:SetAnchorPoint( 1 , 0.5 )
	lab_title:SetPosition( lab_num:GetPositionX() - lab_num:GetContentWidth() - 5 , lab_num:GetPositionY() )
	self.panel_common_reward:AddChild(lab_title)

	--金币标题
   	local lab_title = Util:LabCenter({
    							{zi = CPPGameLib.GetString("common_get_gold"), ys = Macros.Color.Special , dx = Macros.TypefaceSize.popup },
    							{tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.GOLD), dx = 0.8 , jg = -5},
				        	})		
	lab_title:SetAnchorPoint( 0 , 0.5 )
	lab_title:SetPosition( self.panel_common_reward:GetContentWidth()/2 + 40 , self.panel_common_reward:GetContentHeight()/2 )
	self.panel_common_reward:AddChild(lab_title)

	--获得金币
	lab_num = Util:Label("" , Macros.TypefaceSize.popup )
	lab_num:SetAnchorPoint(0, 0.5)
	lab_num:SetPosition( lab_title:GetContentWidth() + lab_title:GetPositionX() + 5 , lab_title:GetPositionY() )
	self.panel_common_reward:AddChild(lab_num)
	Modules.SpBattleResultWinBase.__AddNumAdditionAnim(self , lab_num, 0, battle_info.reward_currency_map[Macros.Game.RolePropType.GOLD] or 0, 0.5, function()
		gold_finished = true
		check_finished()
	end)	

	CPPActionManager.FadeIn(self.panel_common_reward , 0.01)
	CPPActionManager.MoveTo(self.panel_common_reward, 0.2 , self.Image_1:GetPositionX() + 7 , self.Image_1:GetPositionY() , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
		CPPActionManager.MoveTo(self.panel_common_reward, 0.3 , self.Image_1:GetPositionX() , self.Image_1:GetPositionY(),  GlobalCallbackMgr:AddCallBackFunc(function ( ... )
		end) )
	end) )

end

--额外奖励
function Modules.TowerCommonResultWinView:__ShowExtraRewards()
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