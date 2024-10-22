-- require "modules/battle/templ_common_reward"
require "modules/battle/battle_win/sp_battle_result_win_base"

--[[
%% @module: 叛军副本结算
%% @author: yjg
%% @created: 2018年12月13日10:34:40
--]]

Modules = Modules or {}

Modules.RebelResultWinView = Modules.RebelResultWinView or BaseClass( Modules.SpBattleResultWinBase )

function Modules.RebelResultWinView:__init()
end

function Modules.RebelResultWinView:__OpenCallback()
	Modules.SpBattleResultWinBase.__OpenCallback(self)
	
	self:__ShowTopInfo()
	self:__ShowExtraRewards()
end

function Modules.RebelResultWinView:__Dispose()
end

--上方信息
function Modules.RebelResultWinView:__ShowTopInfo()
	Modules.SpBattleResultWinBase.__ShowTopInfo( self )
	if self.topInfo ~= true then
		return 
	end
	self.topInfo = false

	local image_top_title = self:GetWidget("Image_TopTitle")
	image_top_title:SetImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE_RESULT_WIN, "gameover", true))
	image_top_title:IgnoreContentAdaptWithSize(true)

	local battle_info = self:CallModuleFunc("GetCurInfo")

	local harm_rank = battle_info.end_params_list[1]
	local exploit_rank = battle_info.end_params_list[2]
	local harm_rank_new = battle_info.end_params_list[3]
	local exploit_rank_new = battle_info.end_params_list[4]

	local show_exploit = exploit_rank ~= exploit_rank_new
	local show_harm = harm_rank ~= harm_rank_new
	local harm_jg = show_exploit and 24 or 0
	local exploit_table = show_exploit and
						{
							{zi = CPPGameLib.GetString("battle_result_top_info_rebel1"), ys = Macros.Color.golden, dx = Macros.TypefaceSize.slightly },
							{zi = exploit_rank > 0 and tostring(exploit_rank) or CPPGameLib.GetString("rank_reflection"), jg = 6, dx = Macros.TypefaceSize.slightly },
							{tp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON, "team_updatearrow"), dx = 1, jg = 6},
							{zi = tostring(exploit_rank_new), ys = Macros.Color.golden, jg = 6, dx = Macros.TypefaceSize.slightly },
						} or nil

	local harm_table = show_harm and
						{
							{zi = CPPGameLib.GetString("battle_result_top_info_rebel2"), ys = Macros.Color.golden, dx = Macros.TypefaceSize.slightly },
							{zi = harm_rank > 0 and tostring(harm_rank) or CPPGameLib.GetString("rank_reflection"), jg = 6, dx = Macros.TypefaceSize.slightly },
							{tp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON, "team_updatearrow"), dx = 1, jg = 6},
							{zi = tostring(harm_rank_new), ys = Macros.Color.golden, jg = 6, dx = Macros.TypefaceSize.slightly },
				    	} or nil

	CPPActionManager.FadeOut(self.Image_1 , 0 )
	CPPActionManager.FadeOut(self.panel_common_reward , 0 )
	CPPActionManager.FadeOut(self.Text_CommonRewardTitle , 0 )

	if exploit_table or harm_table then

		local lab = Util:LabCenter(CPPGameLib.MergeTable2List(exploit_table))
		lab:SetAnchorPoint(0.5, 0.5 )
        lab:SetPosition(self.panel_top_info:GetContentWidth()/2 , self.panel_top_info:GetContentHeight()/2 - 25 )
        self.panel_top_info:AddChild(lab)

        local lab_ = Util:LabCenter(CPPGameLib.MergeTable2List(harm_table))
		lab_:SetAnchorPoint(0.5, 0.5 )
		lab_:SetPosition(self.panel_top_info:GetContentWidth()/2 , lab:GetPositionY() - 50 )
        self.panel_top_info:AddChild(lab_)

		CPPActionManager.FadeOut(lab , 0 )
		CPPActionManager.FadeOut(lab_ , 0 )

		CPPActionManager.FadeIn(lab , 0.6 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )

			CPPActionManager.FadeIn( lab_ , 0.6 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )

				CPPActionManager.FadeIn(self.Image_1 , 0 )
				CPPActionManager.FadeIn(self.panel_common_reward , 0 )
				CPPActionManager.FadeIn(self.Text_CommonRewardTitle , 0 )
			   	self:__ShowCommonRewards()

			end) )

		end) )

    else
		CPPActionManager.FadeIn(self.Image_1 , 0 )
		CPPActionManager.FadeIn(self.panel_common_reward , 0 )
		CPPActionManager.FadeIn(self.Text_CommonRewardTitle , 0 )
	   	self:__ShowCommonRewards()
    end
end

--战斗奖励
function Modules.RebelResultWinView:__ShowCommonRewards( end_callback )
	Modules.SpBattleResultWinBase.__ShowCommonRewards( self )
	if self.commonRewards ~= true then
		return true
	end
	self.commonRewards = false

	local battle_info = self:CallModuleFunc("GetCurInfo")

	self.Text_CommonRewardTitle:SetPosition( self.Text_CommonRewardTitle:GetPositionX() , self.Text_CommonRewardTitle:GetPositionY() - 80 )
	self.panel_common_reward:SetPosition( self.panel_common_reward:GetPositionX() , self.panel_common_reward:GetPositionY() - 80 )
	self.Image_1:SetPosition( self.Image_1:GetPositionX() , self.Image_1:GetPositionY() - 80 )

	local total_damage = CallModuleFunc(ModuleType.BATTLE, "GetTotalDamage")

	local harm_finished, exploit_finished, battle_achievement_finished	
	local function check_finished()
		if harm_finished and exploit_finished and battle_achievement_finished then
			Modules.SpBattleResultWinBase.CommonRewardFinishedCallback( self )
		end
	end

	--伤害标题
	local lab_title = Util:Label(CPPGameLib.GetString("common_do_damage"), nil, Macros.Color.golden)
	lab_title:SetAnchorPoint(0, 0.5)
	lab_title:SetPosition( 40 , self.panel_common_reward:GetContentHeight()/2 + 15 )
	self.panel_common_reward:AddChild(lab_title)

	--造成伤害
	local lab_num = Util:Label(0)
	self.panel_common_reward:AddChild(lab_num)
	PosTool.RightTo(lab_title, lab_num, 4 )		
	Modules.SpBattleResultWinBase.__AddNumAdditionAnim(self , lab_num, 0, total_damage, 0.5, function()
		harm_finished = true
		check_finished()
	end)

	--功勋标题
	local lab_title = Util:Label(CPPGameLib.GetString("common_get_exploit"), nil, Macros.Color.golden)
	lab_title:SetAnchorPoint(0, 0.5)
	lab_title:SetPosition(self.panel_common_reward:GetContentWidth()/2 + 40 , self.panel_common_reward:GetContentHeight()/2 + 15 )
	self.panel_common_reward:AddChild(lab_title)

	--获得功勋
	local rebel_exploit = math.ceil(total_damage/1000)
	if CallModuleFunc(ModuleType.REBEL, "IsExploitDoubleTime") then
		rebel_exploit = rebel_exploit * 2
	end
	local lab_num = Util:Label(0)
	self.panel_common_reward:AddChild(lab_num)
	PosTool.RightTo(lab_title, lab_num, 4)		
	Modules.SpBattleResultWinBase.__AddNumAdditionAnim(self , lab_num, 0, rebel_exploit, 0.5, function()
		exploit_finished = true
		check_finished()
	end)

	--战功标题
   	local lab_title = Util:LabCenter({
    							{zi = CPPGameLib.GetString("common_get_battle_achievement"), ys = Macros.Color.golden},
    							{tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.REBEL_VALUE), dx = 0.8, jg = 6},
				        	})		
	lab_title:SetAnchorPoint(0, 0.5)
	lab_title:SetPosition( 40 , self.panel_common_reward:GetContentHeight()/2 - 20)
	self.panel_common_reward:AddChild(lab_title)

	--获得战功
	lab_num = Util:Label(0)
	lab_num:SetAnchorPoint(0, 0.5)
	self.panel_common_reward:AddChild(lab_num)
	PosTool.RightTo(lab_title, lab_num, 4)	
	Modules.SpBattleResultWinBase.__AddNumAdditionAnim(self , lab_num, 0, battle_info.end_params_list[5] , 0.5, function()
		battle_achievement_finished = true
		check_finished()
	end)	

	CPPActionManager.FadeIn(self.panel_common_reward , 0.2)
end

--额外奖励
function Modules.RebelResultWinView:__ShowExtraRewards()
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