
--[[
%% @module: UI/场景加载模块
%% @author: swordman sue
%% @created: 2017-03-23
--]]

Modules = Modules or {}

Modules.LoadingModule = Modules.LoadingModule or BaseClass(BaseModule)

local loading_view_type_map = 
{
	[BaseViewType.UI_LOADING] = true,
	[BaseViewType.UI_LOADING_II] = true,
	[BaseViewType.SCENE_LOADING] = true,
	[BaseViewType.NET_LOADING] = true,
	[BaseViewType.NET_CONNECT_LOADING] = true,
}

function Modules.LoadingModule:__init()
end

function Modules.LoadingModule:__delete()
	self:DumpViews()
end

--销毁所有loading界面
function Modules.LoadingModule:DumpViews()
	local view
	for view_type, _ in pairs(loading_view_type_map) do
		view = GlobalViewMgr:GetViewByType(view_type)
		if view then
			view:ResetRefObj()
		end
	end
	GlobalViewMgr:CloseViews(loading_view_type_map, BaseView.CloseMode.DESTROY, true, true)
end

--销毁协议loading界面
function Modules.LoadingModule:DumpNetLoadingView()
	local view = GlobalViewMgr:GetViewByType(BaseViewType.NET_LOADING)
	if view then
		view:ResetRefObj()
	end
	GlobalViewMgr:CloseViews({[BaseViewType.NET_LOADING] = true}, BaseView.CloseMode.DESTROY, true, true)	

	local view = GlobalViewMgr:GetViewByType(BaseViewType.NET_CONNECT_LOADING)
	if view then
		view:ResetRefObj()
	end
	GlobalViewMgr:CloseViews({[BaseViewType.NET_CONNECT_LOADING] = true}, BaseView.CloseMode.DESTROY, true, true)		
end

--任意loading界面打开着
function Modules.LoadingModule:IsAnyLoadingViewOpen()
	local view
	for view_type, _ in pairs(loading_view_type_map) do
		view = GlobalViewMgr:GetViewByType(view_type)
		if view and view:IsOpen() then
			return true
		end
	end	
end

--是否loading界面类型
function Modules.LoadingModule:IsLoadingViewType(view_type)
	return loading_view_type_map[view_type]
end