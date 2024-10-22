--[[
%% @module: 征战入口
%% @author: ldx
%% @created: 2018-12-24
--]]

Modules = Modules or {}

Modules.DungeonEntry = Modules.DungeonEntry or BaseClass(Modules.BaseMainCityEntry)

function Modules.DungeonEntry:__init()
	self:SetAnchorPoint(1, 0)
	self:SetZOrder(100)
	PosTool.RightBottom(self:GetNode(),0,0)

	--按钮
	PosTool.CenterBottom(self.__entry_btn, self.__region_layout_info.entry_btn_offset_x, self.__region_layout_info.entry_btn_offset_y-5)
	
	--文字
	PosTool.CenterBottom(self.__entry_txt, 0, 3)	

	self:CreateEffect()

	self:CheckLeadToBattle()
end

function Modules.DungeonEntry:RegistAllEvents()
	Modules.BaseMainCityEntry.RegistAllEvents(self)

	--指引开始隐藏指示
	self:BindGlobalEvent(Macros.Event.LeadModule.START, function()
		if self.__lead_player_attack_effectid then
			local effect = GlobalEffectMgr:GetEffect(self.__lead_player_attack_effectid)
			if effect then
				effect:SetVisible(false)
			end
		end
	end)
	
	--指引结束显示指示
	self:BindGlobalEvent(Macros.Event.LeadModule.STOP, function()
		if self.__lead_player_attack_effectid then
			local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
			if role_level <= ConfigAdapter.Common.GetMainCityViewAddLeadLimitLevel() then
				local effect = GlobalEffectMgr:GetEffect(self.__lead_player_attack_effectid)
				if effect then
					effect:SetVisible(true)
				end
			else
				self:DeleteEffectById(self.__lead_player_attack_effectid)
				self.__lead_player_attack_effectid = nil
			end
		end
	end)
end

function Modules.DungeonEntry:CreateEffect()
	self:DeleteEffectById("UI_zhengzhan")
	local effect
	self.__entry_effect_map["UI_zhengzhan"],effect = self:__CreateEffect({res_name = "UI_zhengzhan"}, self.__entry_btn)
	PosTool.Center(effect,0,-6)
end

--检查弱指引
function Modules.DungeonEntry:CheckLeadToBattle()
	if CPPSdkTools.IsInExamine() then
		--审核中，屏蔽弱指引
		return
	end

	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	if role_level <= ConfigAdapter.Common.GetMainCityViewAddLeadLimitLevel() and (not self.__lead_player_attack_effectid) then
		local effect
		self.__lead_player_attack_effectid,effect = self:__CreateEffect({res_name = "UI_zhiyinshouzhi"}, self)
		PosTool.Center(effect, -45, 45)
		effect:SetVisible(not CallModuleFunc(ModuleType.LEAD, "IsInLead"))
	else
		if self.__lead_player_attack_effectid then
			self:DeleteEffectById(self.__lead_player_attack_effectid)
			self.__lead_player_attack_effectid = nil
		end
	end
end

--等级变化相关处理
function Modules.DungeonEntry:__HandleLevelChange()
	--更新弱指引检查
	self:CheckLeadToBattle()
end

