require "modules/battle/templ_common_reward" 
require "modules/battle/battle_win/sp_battle_result_win_base"

--[[
%% @module: 怪物攻城
%% @author: yjg
%% @created: 2018年12月13日16:38:36
--]]

Modules = Modules or {}

Modules.AttackCityResultWinView = Modules.AttackCityResultWinView or BaseClass( Modules.SpBattleResultWinBase )

function Modules.AttackCityResultWinView:__init()
end

function Modules.AttackCityResultWinView:__OpenCallback()
	Modules.SpBattleResultWinBase.__OpenCallback(self)
	
	self:__ShowTopInfo()
end

function Modules.AttackCityResultWinView:__Dispose()
end

--上方信息
function Modules.AttackCityResultWinView:__ShowTopInfo()
	Modules.SpBattleResultWinBase.__ShowTopInfo( self )
	if self.topInfo ~= true then
		return 
	end
	self.topInfo = false

	local battle_info = self:CallModuleFunc("GetCurInfo")

	local image_top_title = self:GetWidget("Image_TopTitle")
	if battle_info.is_win == 1 then 
	else
		image_top_title:SetImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE_RESULT_WIN, "gameover", true))
	end
	image_top_title:IgnoreContentAdaptWithSize(true)
	self:__ShowCommonRewards()
end

--战斗奖励
function Modules.AttackCityResultWinView:__ShowCommonRewards( end_callback )
	Modules.SpBattleResultWinBase.__ShowCommonRewards( self )
	if self.commonRewards ~= true then
		return true
	end
	self.commonRewards = false

	local battle_info = self:CallModuleFunc("GetCurInfo")

	self.Text_CommonRewardTitle:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE_RESULT_WIN, "award", true) )

	local cnf = CPPGameLib.GetConfig("AttackCityMonster", tonumber(battle_info.end_params_list[3]) , false )

	local y = -25

	local num = CallModuleFunc(ModuleType.MONSTER_STORMCASTLE,"GetMonsterStormCityEncourageTimes")

	local encourage_config = CPPGameLib.GetConfig("AttackCityEncourage", num , false )
	local score_add_rate = 0
	if encourage_config then
		score_add_rate = MathTool.TransProbCoeff2Decimal( encourage_config.score_add_rate )
	end

	if battle_info.is_win == 1 then

		self.__templ_common_reward_1 = Modules.TemplBattleCommonReward.New(
		{
			parent_node = self.panel_common_reward,
			notShowExp = true ,
			add_type = { { 
							text = CPPGameLib.GetString("monster_stormcastle_kil_intergral") ,  
							value = cnf.kill_score + ( cnf.kill_score * score_add_rate )
						} },
			time = 0.5,
			callback = function()
				Modules.SpBattleResultWinBase.CommonRewardFinishedCallback(self)
			end
		})	
		self.__templ_common_reward_1:SetPosition( -7 , self.panel_common_reward:GetContentHeight()/2 - 50 )
		y = -52
	end

	self.__templ_common_reward = Modules.TemplBattleCommonReward.New(
	{
		parent_node = self.panel_common_reward,
		notShowExp = true ,
		-- add_gold = 100 ,
		add_type = {
					{
						id = cnf.battle_reward_currency_type_list[1],
						value = cnf.battle_reward_currency_value_list[1],
					} ,
					{ 
						text = CPPGameLib.GetString("monster_stormcastle_att_intergral") ,  
						value = cnf.battle_score + ( cnf.battle_score * score_add_rate )
					} },
		time = 0.5,
		callback = function()
			Modules.SpBattleResultWinBase.CommonRewardFinishedCallback(self)
		end
	})	
	self.__templ_common_reward:SetPosition(-7, y )

	self.extraRewards = true
	self.__extra_reward_finished = true

	local drop_list = cnf.kill_reward_item_id_list

	if drop_list and #drop_list ~= 0 and battle_info.is_win == 1 then
		self:__ShowExtraRewards()
	else
		self.panel_common_reward:SetPosition( self.panel_common_reward:GetPositionX() , self.panel_common_reward:GetPositionY() - 30 )
		self.Text_CommonRewardTitle:SetPosition( self.Text_CommonRewardTitle:GetPositionX() , self.Text_CommonRewardTitle:GetPositionY() - 30 )
		self.Image_1:SetPosition( self.Image_1:GetPositionX() , self.Image_1:GetPositionY() - 30 )

		local panel_extra_reward = self:GetWidget("Panel_SpecialReward")
		panel_extra_reward:SetVisible(false)
		local Text_SpecialRewardTitle = self:GetWidget("Text_SpecialRewardTitle")
		Text_SpecialRewardTitle:SetVisible(false)
		local Image_2 = self:GetWidget("Image_2")	
		Image_2:SetVisible(false)
	end
end

--额外奖励
function Modules.AttackCityResultWinView:__ShowExtraRewards()
	Modules.SpBattleResultWinBase.__ShowExtraRewards( self )
	
	if self.extraRewards ~= true then
		return 
	end
	self.extraRewards = true

	local cnf = CPPGameLib.GetConfig("AttackCityMonster", battle_info.end_params_list[3] , false )
	local drop_list = cnf.kill_reward_item_id_list

	if drop_list and #drop_list ~= 0 then
		local t1 = {}
		for i,v in ipairs( drop_list ) do
			table.insert( t1 , { id = v , num = cnf.kill_reward_item_num_list[i] } )
		end			
		Modules.SpBattleResultWinBase.__ShowDropRewards(self , self.panel_extra_reward , t1 )
	end
end