require "modules/battle/templ_common_reward" 
require "modules/battle/battle_win/sp_battle_result_win_base"

--[[
%% @module: 竞技场结算
%% @author: yjg
%% @created: 2018年12月13日14:52:24
--]]

Modules = Modules or {}

Modules.ArenaResultWinView = Modules.ArenaResultWinView or BaseClass( Modules.SpBattleResultWinBase )

function Modules.ArenaResultWinView:__init()
end

function Modules.ArenaResultWinView:__OpenCallback()
	Modules.SpBattleResultWinBase.__OpenCallback(self)
	
	self:__ShowTopInfo()
end

function Modules.ArenaResultWinView:__Dispose()
end

--上方信息
function Modules.ArenaResultWinView:__ShowTopInfo()
	Modules.SpBattleResultWinBase.__ShowTopInfo( self )
	if self.topInfo ~= true then
		return 
	end
	self.topInfo = false

	local rank, enemy_rank, enemy_name , enemy_id , isNotrank = CallModuleFunc(ModuleType.ARENA, "GetFightResultViewMessage")

	local text = CPPGameLib.GetString("battle_result_top_info_arena")
	local tab = {  
			{zi = enemy_rank , dx = Macros.TypefaceSize.popup, ys = Macros.Color.Special },
			{zi = enemy_name , dx = Macros.TypefaceSize.popup, ys = Macros.Color.Special },
		}
    local lab = Util:Echo( text , tab , Macros.Color.btn , Macros.TypefaceSize.popup) 
	lab:SetAnchorPoint(0.5, 0.5)
	lab:SetPosition(self.panel_top_info:GetContentWidth()/2 , self.panel_top_info:GetContentHeight()/2 - 50 )
	self.panel_top_info:AddChild(lab)

	 local lab_ = nil
	--挑战高排名玩家，显示排名变化
	if rank > enemy_rank then

		if isNotrank == true then
			rank = CPPGameLib.GetString("rank_reflection")
		end
		
        lab_ = Util:LabCenter({
    							{zi = CPPGameLib.GetString("common_cur_rank"), ys = Macros.Color.golden, dx = Macros.TypefaceSize.slightly },
    							{zi = tostring(rank), jg = 8, dx = Macros.TypefaceSize.slightly },
    							{tp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON, "team_updatearrow"), dx = 1, jg = 12},
    							{zi = tostring(enemy_rank), ys = Macros.Color.front_color, jg = 12, dx = Macros.TypefaceSize.slightly },
				        	})
		lab_:SetAnchorPoint(0.5, 0)		
		lab_:SetPosition(self.panel_top_info:GetContentWidth()/2 , lab:GetPositionY() - 50 )
        self.panel_top_info:AddChild(lab_)
    end

    if lab_ then
    	CPPActionManager.FadeOut(lab_ , 0 )
    end

    --重新设置战斗奖励的位置
	self.Text_CommonRewardTitle:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE_RESULT_WIN, "award", true) )
	self.Image_1:SetPosition( self.Image_1:GetPositionX() , self.Image_1:GetPositionY() - 80 )
	self.Text_CommonRewardTitle:SetPosition( self.Text_CommonRewardTitle:GetPositionX() , self.Text_CommonRewardTitle:GetPositionY() - 80 )
	self.panel_common_reward:SetPosition( self.panel_common_reward:GetPositionX() , self.panel_common_reward:GetPositionY() - 80 )

	--隐藏战斗奖励		
	CPPActionManager.FadeOut(self.panel_common_reward , 0 )
	CPPActionManager.FadeOut(self.Text_CommonRewardTitle , 0 )
	CPPActionManager.FadeOut(self.Image_1 , 0 )

	CPPActionManager.FadeOut(lab , 0 ,GlobalCallbackMgr:AddCallBackFunc(function ( ... )
		CPPActionManager.FadeIn(lab , 0.8 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
			--显示战斗奖励
			CPPActionManager.FadeIn(self.Text_CommonRewardTitle , 0 )
			CPPActionManager.FadeIn(self.panel_common_reward , 0 )
			CPPActionManager.FadeIn(self.Image_1 , 0 )

			if lab_ then
				CPPActionManager.FadeIn(lab_ , 0.8  , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
					--战斗奖励
					self:__ShowCommonRewards()
				end) )
			else
				--战斗奖励
				self:__ShowCommonRewards()
			end
		end) )
	end) )
	--额外奖励
	self:__ShowExtraRewards()
end

--战斗奖励
function Modules.ArenaResultWinView:__ShowCommonRewards( end_callback )
	Modules.SpBattleResultWinBase.__ShowCommonRewards( self )
	if self.commonRewards ~= true then
		return true
	end
	self.commonRewards = false

	local battle_info = self:CallModuleFunc("GetCurInfo")

	local config_arena_award = CPPGameLib.GetConfig("ArenaAward", battle_info.pre_role_info.level)

	local num = CallModuleFunc(ModuleType.ACTIVITY,"GetTimedOutput" , Macros.Game.FBType.ARENA )
	local win_gold = config_arena_award.win_gold * num
	local win_arena_prestige = config_arena_award.win_arena_prestige * num

	self.__templ_common_reward = Modules.TemplBattleCommonReward.New(
	{
		parent_node = self.panel_common_reward,
		add_gold = win_gold ,
		add_exp = config_arena_award.exp,
		add_type = { { 
						id = Macros.Game.RolePropType.ARENA_PRESTIGE ,  
						value = win_arena_prestige
					} },
		time = 0.5,
		callback = function()
			Modules.SpBattleResultWinBase.CommonRewardFinishedCallback( self )
		end
	})	
	self.__templ_common_reward:SetPosition(-7, 0)
end

--额外奖励
function Modules.ArenaResultWinView:__ShowExtraRewards()
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