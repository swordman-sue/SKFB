
--[[
%% @module: 兑换码
%% @author: yjg
%% @created: 2017年5月11日10:21:46
--]]
-- Panel_2
-- cdk

Modules = Modules or {}

Modules.TemplCdkey = Modules.TemplCdkey or BaseClass(BaseTabView)

function Modules.TemplCdkey:__init()
end

--选项卡界面创建完成回调
function Modules.TemplCdkey:__LoadedCallback()
	self:__Getwidget()
end

--选项卡界面打开回调
function Modules.TemplCdkey:__OpenCallback()
	self:__SetShowUI()
	--错误码回调
	self:BindGlobalEvent(Macros.Event.ErrorNotifyModule.ERROR_NOTIFY, function(_,error_code)									
		if error_code == 20001 then
			self:DeleteMe()
		end
	end)
end

--选项卡界面关闭回调
function Modules.TemplCdkey:__CloseCallback()

end

--选项卡界面销毁回调(释放非托管资源)
function Modules.TemplCdkey:__Dispose()
end

--获得所有组件
function Modules.TemplCdkey:__Getwidget()
	self.Panel_2 = self:GetWidget("Panel_2")

	self.TextField_CDKEYImport = Util:WidgetTextField(self:GetWidget("TextField_CDKEYImport"))
	self.TextField_CDKEYImport:SetPlaceHolderText(CPPGameLib.GetString("men_patch"))
	self.TextField_CDKEYImport:SetMaxLengthEnabled(true)
	self.TextField_CDKEYImport:SetMaxLength(20)
	self.TextField_CDKEYImport:SetTextColor( unpack( Macros.Color.content ) )
	self.TextField_CDKEYImport:SetTextHorizontalAlign(LabelEx.ALIGIN_MIDDLE)

	--发送按钮
	self.Button_ok = Util:WidgetButton( 
											self:GetWidget("Button_ok") , 
											CPPGameLib.GetString("notice_sell") , 
											CPPGameLib.Handler(self,self.OnButtonOk) ,
											nil, nil , Macros.Color.button_red

										)

	--粘贴按钮
	self.Button_paste = Util:WidgetButton( 
											self:GetWidget("Button_paste") , 
											CPPGameLib.GetString("notice_paste") , 
											CPPGameLib.Handler(self,self.OnButtonPaste) ,
											nil, nil , Macros.Color.button_red 
										)

	local sp = Resource.PathTool.GetUIResPath("activitybig/cdk" , true )
	local level_gift = Util:Sprite( sp )
	level_gift:SetAnchorPoint(0.5,0.5)
	self.Panel_2:AddChild( level_gift )
	PosTool.Center( level_gift )
end

function Modules.TemplCdkey:__SetShowUI()
end

function Modules.TemplCdkey:OnButtonOk()
	local text = self.TextField_CDKEYImport:GetText()

	CallModuleFunc(ModuleType.ACTIVITY, "TakeActivityGiftReq" , text )
end

-- function Modules.TemplCdkey:OnButtonPaste()
-- 	local text = self.TextField_CDKEYImport:GetText()
-- 	CallModuleFunc(ModuleType.ACTIVITY, "CdkeyReq" , text )
-- end
