--[[
%% @module: 我要变强入口
%% @author: ldx
%% @created: 2018-12-21
--]]

Modules = Modules or {}

Modules.StrongerEntry = Modules.StrongerEntry or BaseClass(Modules.BaseMainCityEntry)

function Modules.StrongerEntry:__init()
	--我要变强
	local type_ = CallModuleFunc(ModuleType.STRONGER , "GetisFirstOpen")
	self.__entry_red_dot:SetVisible( type_ )
end