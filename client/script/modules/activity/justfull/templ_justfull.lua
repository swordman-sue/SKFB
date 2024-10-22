
--[[
%% @module: vip福利
%% @author: yjg
%% @created: 2017年5月11日10:21:46
--]]
-- Image_direct
-- sp_activity_direct

Modules = Modules or {}

Modules.TemplJustFull = Modules.TemplJustFull or BaseClass(BaseTabView)

function Modules.TemplJustFull:__init()

end

--选项卡界面创建完成回调
function Modules.TemplJustFull:__LoadedCallback()
	self:__Getwidget()
end

--选项卡界面打开回调
function Modules.TemplJustFull:__OpenCallback()

end

--选项卡界面关闭回调
function Modules.TemplJustFull:__CloseCallback()
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.TemplJustFull:__Dispose()

end

--获得组件
function Modules.TemplJustFull:__Getwidget()
    --领取
    self.Button_Go = Util:WidgetButton( 
    										self:GetWidget("Button_Go") , 
    										CPPGameLib.GetString("direct_buy_GoExchange") , 
    										CPPGameLib.Handler(self,self.OnButtonGo),
    										nil,nil,Macros.Color.button_red
    										)
   
    local sp = Resource.PathTool.GetUIResPath("activitybig/sp_activity_direct" , true )
    local level_gift = Util:Sprite( sp )
    level_gift:SetAnchorPoint(0.5,0.5)
    self.Image_direct:AddChild( level_gift )
    PosTool.Center( level_gift )
end

--创建组件
function Modules.TemplJustFull:OnButtonGo()
	CallModuleFunc(ModuleType.DIRECT_BUY_SHOP, "OpenView", BaseViewType.DIRECT_BUY_SHOP )
end
