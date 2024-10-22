
--[[
%% @module: 英雄对象
%% @author: swordman sue
%% @created: 2016-07-11
--]]

Scene = Scene or {}

Scene.Hero = Scene.Hero or BaseClass(Scene.BattleObj)

function Scene.Hero:__init()	
	self.__obj_type = Scene.ObjType.HERO

	--加载VO
	self:__LoadVO()

    GlobalVOMgr:AddHeroVO(self.__vo.obj_id, self.__vo)
end

function Scene.Hero:__delete()
	GlobalVOMgr:DeleteHeroVO(self.__vo.obj_id)
end

--能否累计怒气
function Scene.Hero:IsAngerAdditive()
	return true
end

--怒气是否充足
function Scene.Hero:IsAngerEnough(is_plus)
    if self.__vo.cost_of_anger_or_joint_skill <= 0 then
        return false
    end

    if not is_plus then
        return GlobalVOMgr:GetAnger() >= self.__vo.cost_of_anger_or_joint_skill

    elseif self.__vo.skill_id_of_anger_plus_skill or self.__vo.skill_id_of_joint_plus_skill then
        return GlobalVOMgr:GetAnger() >= self.__vo.cost_of_anger_or_joint_skill * 2
    end
end

--怒气处理
function Scene.Hero:DoAngerChange(change_anger)    
    if self.is_real_dead then
        return
    end

    if not self:IsAngerAdditive() then
        return
    end
    change_anger = change_anger
    CallModuleFunc(ModuleType.SP_LOGIC_MGR, "DoAngerChange", change_anger)
end

--战斗伤害翻倍提示效果
--is_lineuplist_center 是否队伍中间
--is_fullpower_hit 是否全力一击
function Scene.Hero:ShowDoubleEffect(is_lineuplist_center, is_fullpower_hit, is_fullpower_hit_tag)
	is_lineuplist_center = is_lineuplist_center or false
	is_fullpower_hit = is_fullpower_hit or false
	is_fullpower_hit_tag = is_fullpower_hit_tag or false
    local break_level = self.__vo.break_level

	--伤害倍数
	local double_effect = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE_COMMON, "battle_damage_num", true), GUI.ImgFont.ImgType.Num2)
    double_effect:SetAnchorPoint(0.5, 0)  
    double_effect:SetText(is_fullpower_hit_tag and break_level > 0 and "X"..(break_level*2.5) or break_level > 0 and "X"..break_level or "")
    self.__model:AddChild(double_effect,RenderObj.InnerLayer.ALL_KIND_OF_TIPS)
    double_effect:SetScaleFactor(1.3)
    PosTool.CenterBottom(double_effect)

    double_effect:SetOpacity(0)
    ActionManager.GetInstance():RunFadeInAndMoveToEffect(double_effect:GetNode(),0.15,double_effect:GetNode():GetPositionX(),double_effect:GetNode():GetPositionY()+70,GlobalCallbackMgr:AddCallBackFunc(function ()
		 CPPActionManager.DelayTo(double_effect:GetNode(),1,GlobalCallbackMgr:AddCallBackFunc(function ()
			ActionManager.GetInstance():RunFadeOutAndMoveToEffect(double_effect:GetNode(),0.15,double_effect:GetNode():GetPositionX(),double_effect:GetNode():GetPositionY()+70,GlobalCallbackMgr:AddCallBackFunc(function ()
				if is_fullpower_hit then
					self:ShowDoubleEffect(is_lineuplist_center,false,true)
				end
			end))
		end))
	end))

    --中间飘字
    local tips_name_data = is_fullpower_hit_tag and "battle_fullpowerhit_buff" or "battle_break_buff"
    local tips_name = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE_COMMON, tips_name_data, true))
    tips_name:SetAnchorPoint(0.5, 0)    
    tips_name:SetVisible(is_lineuplist_center)
    tips_name:SetScaleFactor(1.8)
    ActionManager:GetInstance():BattleTeamWordsUpslopeEffect(tips_name)
end
