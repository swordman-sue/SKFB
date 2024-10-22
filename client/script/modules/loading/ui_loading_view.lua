
--[[
%% @module: UI加载界面(无可视节点，仅用于界面异步加载中的屏蔽层)
%% @author: swordman sue
%% @created: 2017-03-23
--]]

Modules = Modules or {}

Modules.UILoadingView = Modules.UILoadingView or BaseClass(BaseView)

function Modules.UILoadingView:__init()
	self.__layout_mode = BaseView.LayoutMode.CODE	
	self.__close_mode = BaseView.CloseMode.VISIBLE
	self.__render_queue = Scene.RenderQueue.GRQ_UI_TOP
	self.__bg_params = {type = BaseView.BGType.SOLID, color = {0, 0, 0, 0}}
	self.__open_loading = false
	self.__is_ignore_iphone_safe_area = true
	self.is_auto_close = false

	self.__loaded_callback = function()
		self:__LoadedCallback()
	end
    
    self.__open_callback = function()
    	self:__OpenCallback()
    end

    self.__close_callback = function()
    	self:__CloseCallback()
    end

    self.__ref_obj = RefObj.New()
end

function Modules.UILoadingView:__LoadedCallback()
	self:SetUpdateFunc(CPPGameLib.Handler(self, self.Update))	
end

function Modules.UILoadingView:__OpenCallback()
end

function Modules.UILoadingView:__CloseCallback()
end

function Modules.UILoadingView:__Dispose()
end

function Modules.UILoadingView:__CreateWidgetFromLuaCode()
	local layout = Util:Layout()
	layout:SetSize(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height)
	return layout
end

function Modules.UILoadingView:Update(elapse_time)
	if self.__open_data_list then
		self.__checking_open_data = true
		local callback
		for _, open_data in ipairs(self.__open_data_list) do
			callback = unpack(open_data)
			if type(callback) == "function" then
				callback()
			end
		end
		self.__checking_open_data = false
		self.__open_data_list = nil
	end

	if self.__cache_open_data_list then
		self.__open_data_list = self.__cache_open_data_list
		self.__cache_open_data_list = nil
	end
end

function Modules.UILoadingView:Open(...)
	if not self.__checking_open_data then
		self.__open_data_list = self.__open_data_list or {}
		table.insert(self.__open_data_list, {...})
	else
		self.__cache_open_data_list = self.__cache_open_data_list or {}
		table.insert(self.__cache_open_data_list, {...})
	end

	self.__ref_obj:Retain()

	BaseView.Open(self, ...)
end

function Modules.UILoadingView:CloseManual(ignore_view_stack, ignore_view_sound)
	self.__ref_obj:Release()

	if not self.__ref_obj:IsValid() then
		BaseView.CloseManual(self, ignore_view_stack, ignore_view_sound)
	end
end

function Modules.UILoadingView:ResetRefObj()
	self.__ref_obj:Reset()
end

