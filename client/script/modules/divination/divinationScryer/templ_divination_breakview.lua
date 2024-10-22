--[[
%% @module: 占卜等级突破
%% @author: yjg
%% @created: 2017年12月5日10:55:07
--]]

LogicGUI = LogicGUI or {}

LogicGUI.TemplDivinationBreakview = LogicGUI.TemplDivinationBreakview or BaseClass(GUI.Template)

function LogicGUI.TemplDivinationBreakview:__init( )
	self.__is_auto_load = true	
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:InitUI()
	end
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_DIVINATION_BREAKVIEW)
end

function LogicGUI.TemplDivinationBreakview:InitUI( )

    local effectid, effect = self:__CreateEffect({res_name = "UI_zhanbukashengji_language_chs" }, self:GetWidget("Panel_effectText") ,2)
    PosTool.Center(effect)

	--排名突破至
	Util:WidgetLabel(self:GetWidget("Text_breakUpTo"),CPPGameLib.GetString("divination_cardUp"), Macros.TypefaceSize.headline,Macros.Color.Special)

	local index = CallModuleFunc(ModuleType.DIVINATION, "GetLowCardGroupMapIdx" )
	local cnf = CallModuleFunc(ModuleType.DIVINATION, "GetCardGroupMap" , index - 1)
	--排名
	Util:WidgetLabel(self:GetWidget("Text_breakRank"), cnf.name , Macros.TypefaceSize.headline,Macros.Color.green)
	
	--排名上升
	local Text_rankUp = Util:WidgetLabel(self:GetWidget("Text_rankUp"), "  " , Macros.TypefaceSize.headline,Macros.Color.green)
	
	local Image_bg = self:GetWidget("Image_bg")

	local describe = Util:RichText( cnf.describe , Macros.TypefaceSize.headline )
	describe:SetOpacity(0)	
	Image_bg:AddChild( describe )
	PosTool.Center( describe , 0 , - 20)

    CPPActionManager.DelayTo( Image_bg , 2 ,GlobalCallbackMgr:AddCallBackFunc( function ( ... )
    	CPPActionManager.FadeIn( describe , 1 )
    end ) )

	--点击屏幕提示
	Util:WidgetLabel(self:GetWidget("Text_continue"),CPPGameLib.GetString("battle_result_continue"), Macros.TypefaceSize.headlineII,Macros.Color.btn)

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
		end)
	end), 0.8)

	self.__delay_timer2 = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
		self.__delay_timer2 = nil
		GlobalEffectMgr:PasueEffect(effect_id)				
	end),2)	
end

function LogicGUI.TemplDivinationBreakview:__delete()
	for i=1,2 do
		if self["__delay_timer"..i] then
			GlobalTimerQuest:CancelQuest(self["__delay_timer"..i])
	    	self["__delay_timer"..i] = nil
		end
	end
end