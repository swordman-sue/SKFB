
--[[
%% @module: 网络加载界面
%% @author: swordman sue
%% @created: 2017-04-11
--]]

Modules = Modules or {}

Modules.NetLoadingView = Modules.NetLoadingView or BaseClass(Modules.UILoadingView)

function Modules.NetLoadingView:__init()
	self.__bg_params = {type = BaseView.BGType.SOLID, color = {0, 0, 0, 0}}
end

function Modules.NetLoadingView:__OpenCallback()
    local param = unpack(self.__open_data)
    if param == "is_connect_loading" then
        self:__CreateLoadingEffect()        
    else
    	self:__ClearLoadingTimer()	
        self.__delay_timer = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(
            function() 
            	self:__CreateLoadingEffect()
            end), Net.NetAdapter.NetLoadingTimeout / 4)

        GlobalEffectMgr:SetEffectVisible(self.__loading_effect_id, false)
    end
end

function Modules.NetLoadingView:__CloseCallback()
	self:__ClearLoadingTimer()		
end

function Modules.NetLoadingView:__Dispose()
	self:__ClearLoadingTimer()		
end

function Modules.NetLoadingView:__CreateWidgetFromLuaCode()
	local layout = Util:Layout()
	layout:SetSize(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height)
	return layout
end

function Modules.NetLoadingView:__ClearLoadingTimer()
	if self.__delay_timer then
		GlobalTimerQuest:CancelQuest(self.__delay_timer)
		self.__delay_timer = nil
	end
end

function Modules.NetLoadingView:__CreateLoadingEffect()
	if not GlobalEffectMgr:GetEffect(self.__loading_effect_id) then
		local effect_root
		self.__loading_effect_id, effect_root = self:__CreateEffect({res_name = "UI_loading"}, self.__root_widget)
		PosTool.Center(effect_root)	
	else
	    GlobalEffectMgr:SetEffectVisible(self.__loading_effect_id, true)	
	end
end