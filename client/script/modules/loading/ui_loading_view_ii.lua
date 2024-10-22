
--[[
%% @module: UI加载界面
%% @author: swordman sue
%% @created: 2017-03-23
--]]

Modules = Modules or {}

Modules.UILoadingViewII = Modules.UILoadingViewII or BaseClass(Modules.UILoadingView)

function Modules.UILoadingViewII:__init()
	self.__bg_params = {type = BaseView.BGType.SOLID, color = {0, 0, 0, 0}}
end

function Modules.UILoadingViewII:__LoadedCallback()
	Modules.UILoadingView.__LoadedCallback(self)

	if StartConfig.IsTest then		
		local lab_tips = Util:Label(CPPGameLib.GetString("resloading_tips"), Macros.TypefaceSize.popup, Macros.Color.orange, nil, nil, Macros.Color.orange_stroke)
		lab_tips:SetAnchorPoint(0.5, 0.5)
		self:AddChild(lab_tips)
		PosTool.Center(lab_tips, 0, -100)
	end
end

function Modules.UILoadingViewII:__OpenCallback()
	self:__ClearLoadingTimer()	
    self.__delay_timer = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(
        function()
        	self:__CreateLoadingEffect()
        end), 0.4)

    GlobalEffectMgr:SetEffectVisible(self.__loading_effect_id, false)
end

function Modules.UILoadingViewII:__CloseCallback()
	self:__ClearLoadingTimer()
end

function Modules.UILoadingViewII:__Dispose()
	self:__ClearLoadingTimer()		
end

function Modules.UILoadingViewII:__CreateWidgetFromLuaCode()
	local layout = Util:Layout()
	layout:SetSize(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height)
	return layout
end

function Modules.UILoadingViewII:__ClearLoadingTimer()
	if self.__delay_timer then
		GlobalTimerQuest:CancelQuest(self.__delay_timer)
		self.__delay_timer = nil
	end
end

function Modules.UILoadingViewII:__CreateLoadingEffect()
	if not GlobalEffectMgr:GetEffect(self.__loading_effect_id) then
		local effect_root
		self.__loading_effect_id, effect_root = self:__CreateEffect({res_name = "UI_loading"}, self.__root_widget)
		PosTool.Center(effect_root)	
	else
	    GlobalEffectMgr:SetEffectVisible(self.__loading_effect_id, true)	
	end
end