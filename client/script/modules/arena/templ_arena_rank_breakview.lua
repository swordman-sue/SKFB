LogicGUI = LogicGUI or {}

LogicGUI.TemplArenaRankBreakView = LogicGUI.TemplArenaRankBreakView or BaseClass(GUI.Template)

function LogicGUI.TemplArenaRankBreakView:__init(nowRank,enemy_rank,history_rank)
	self.__is_auto_load = true	
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:InitUI(nowRank,enemy_rank,history_rank)
	end
	self:InitTemplate(GUI.TemplateRegistry.ARENA_RANK_BREAK_VIEW)
end

function LogicGUI.TemplArenaRankBreakView:InitUI(nowRank,enemy_rank,history_rank)
	--排名突破至
	Util:WidgetLabel(self:GetWidget("Text_breakUpTo"),CPPGameLib.GetString("arena_break_rank_To"), Macros.TypefaceSize.headline,Macros.Color.Special)
	--排名
	Util:WidgetLabel(self:GetWidget("Text_breakRank"),enemy_rank,Macros.TypefaceSize.headline,Macros.Color.green)
	--排名上升
	Util:WidgetLabel(self:GetWidget("Text_rankUp"),CPPGameLib.GetString("arena_rankUp"), Macros.TypefaceSize.headline, Macros.Color.Special)
	--排名上升数量
	Util:WidgetLabel(self:GetWidget("Text_rankUpNum"),nowRank - enemy_rank,Macros.TypefaceSize.headline, Macros.Color.btn)
	--突破奖励
	Util:WidgetLabel(self:GetWidget("Text_breakEncourage"),CPPGameLib.GetString("arena_breakEncourage"), Macros.TypefaceSize.headline, Macros.Color.Special)
	--突破奖励数量
	local encourgeNum = 0
	history_rank = history_rank - 1
	for i = history_rank,enemy_rank,-1 do
		local first_WinData = CPPGameLib.GetConfig("ArenaFirstRankAward",i)
		encourgeNum = encourgeNum + first_WinData.diamond
	end
	Util:WidgetLabel(self:GetWidget("Text_breakEncourageNum"),encourgeNum, Macros.TypefaceSize.headline, Macros.Color.btn)
	--点击屏幕提示
	Util:WidgetLabel(self:GetWidget("Text_continue"),CPPGameLib.GetString("battle_result_continue"), Macros.TypefaceSize.headlineII,Macros.Color.btn)

	--排名特效
	local panel_effecttext = self:GetWidget("Panel_effectText")
	local effect_id,effect = self:__CreateEffect({id = 100046,loop = false},panel_effecttext)
	PosTool.Center(effect)

	self.__delay_timer1 = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
		self.__delay_timer1 = nil
		GlobalEffectMgr:PasueEffect(effect_id)				
		self:PlayActionTimeline("animation0",false,function ()			
			self:PlayActionTimeline("animation1",true,nil)			
			local function close_func()
				self:DeleteMe()
			end
			local panel_lead = self:GetWidget("Panel_Lead")

			--关闭监听
			WidgetUnity.SetWidgetListener(self:GetNode(), nil, nil, close_func)
			WidgetUnity.SetWidgetListener(panel_lead, nil, nil, close_func)

		    --新手指引特殊操作检测(战队升级继续)
		    CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.ARENA_RANK_BREAK_CONTINUE, panel_lead)         						
		end)
	end), 0.8)

	self.__delay_timer2 = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
		self.__delay_timer2 = nil
		GlobalEffectMgr:PasueEffect(effect_id)				
	end),2)	
end

function LogicGUI.TemplArenaRankBreakView:__delete()
	for i=1,2 do
		if self["__delay_timer"..i] then
			GlobalTimerQuest:CancelQuest(self["__delay_timer"..i])
	    	self["__delay_timer"..i] = nil
		end
	end
end