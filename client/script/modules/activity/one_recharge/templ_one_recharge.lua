
--[[
%% @module: 单笔充值
%% @author: yjg
%% @created: 2017年6月27日10:10:43
--]]
-- Panel_welfare
-- one_recharge

Modules = Modules or {}

Modules.TemplOneRecharge = Modules.TemplOneRecharge or BaseClass(BaseTabView)

function Modules.TemplOneRecharge:__init()
	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_ACTIVIITY_ONE_RECHARGE_ITEM)
end

--选项卡界面创建完成回调
function Modules.TemplOneRecharge:__LoadedCallback()
	self:__Getwidget()
	self:__ShowUI()
end

--选项卡界面打开回调
function Modules.TemplOneRecharge:__OpenCallback()
    CallModuleFunc(ModuleType.ACTIVITY, "SingleRechargeDataReq")
	self:__SetShowUI()
    self:BindGlobalEvent(Macros.Event.ActivityModule.ACTIVITY_ONE_RECHARGE,function (_, data)
    	self:__SetShowUI()
    end)
end

--选项卡界面关闭回调
function Modules.TemplOneRecharge:__CloseCallback()
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.TemplOneRecharge:__Dispose()
end

--获得组件
function Modules.TemplOneRecharge:__Getwidget()
	self.Image_contentLst = self:GetWidget("Image_contentLst") 

	self.Panel_welfare = self:GetWidget("Panel_welfare") 
end

--创建组件
function Modules.TemplOneRecharge:__ShowUI()
	local params =  
	{
	    item_class = Modules.TemplOneRechargeItem,
	    item_width = 660.00,
	    item_height = 140.00 ,
	    row = 4,
	    col = 1,
	    view_height = self.Image_contentLst:GetContentHeight() - 5  ,
	}
	-- 创建参数
	self.lst_list = GUI.ScrollView.New(params)
	self.Image_contentLst:AddChild(self.lst_list:GetNode())
	PosTool.Center(self.lst_list:GetNode())

    local sp = Resource.PathTool.GetUIResPath("activitybig/one_recharge" , true )
    local level_gift = Util:Sprite( sp )
    level_gift:SetAnchorPoint(0.5,0.5)
    self.Panel_welfare:AddChild( level_gift )
    PosTool.Center( level_gift ) 

    local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.ONE_RECHARGE )

    local activity_starTime = Util:Label( CPPGameLib.GetString("activity_starTime" , TimeTool.GetTimeFormat( info.star  , TimeTool.defaul_format[6] ) ) )
    activity_starTime:SetAnchorPoint( 0 , 0.5 )
    activity_starTime:SetPosition( 5 , 43 )
    self.Panel_welfare:AddChild( activity_starTime )

    local activity_overTime = Util:Label( CPPGameLib.GetString("activity_overTime" , TimeTool.GetTimeFormat( info.over  , TimeTool.defaul_format[6] ) ) )
    activity_overTime:SetAnchorPoint( 0 , 0.5 )
    activity_overTime:SetPosition( 5 , 18 )
    self.Panel_welfare:AddChild( activity_overTime )

end

function Modules.TemplOneRecharge:__SetShowUI()
	local t1 = CallModuleFunc(ModuleType.ACTIVITY, "GetOneRechargeCnf")
	self.lst_list:SetDataList( t1 )
end




Modules = Modules or {}
Modules.TemplOneRechargeItem = Modules.TemplOneRechargeItem or BaseClass(GUI.ScrollItem)

function Modules.TemplOneRechargeItem:__init()
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_ACTIVIITY_ONE_RECHARGE_ITEM)   
    self:__Getwidget()
end

function Modules.TemplOneRechargeItem:__delete()

end

function Modules.TemplOneRechargeItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self:__ShowUI( data )
end

function Modules.TemplOneRechargeItem:__Getwidget()
	--条目
	self.Image_item = self:GetWidget("Image_item")
	--图标容器
	self.Image_content = self:GetWidget("Image_content")
	--按钮占位
	self.Panel_btn = self:GetWidget("Panel_btn")
	--标题
	self.Text_item = self:GetWidget("Text_item")
end

function Modules.TemplOneRechargeItem:__ShowUI(data)

    --提示
    self.tips = Util:RichText( string.format(Macros.Color.headline_rt , CPPGameLib.GetString("recharge_oneActivityTips" , data.money ) ) , Macros.TypefaceSize.largish )
    self.tips:SetAnchorPoint( 0 , 0.5 )
    self.tips:SetPosition(0 , self.Text_item:GetContentHeight()/2 )
    self.Text_item:AddChild( self.tips )

    --图标
    for i , v in ipairs( data.item ) do
        --头像
        local icon = LogicGUI.IconLayer.New()
        self.Image_content:AddChild(icon:GetNode())
        local iconData = {id = v.id , num = v.num }
        icon:SetData(iconData)
        icon:SetPosition( (icon:GetNode():GetContentWidth()/2 ) + 10 + ( (i-1) * (icon:GetNode():GetContentWidth() + 10 ) ) , self.Image_content:GetContentHeight()/2 )
        icon:SetTouchSwallow( false )
    end

    self:Transfer( data )
end

--设置可以购买
function Modules.TemplOneRechargeItem:Transfer(data)
	local info = CallModuleFunc(ModuleType.ACTIVITY, "GetOneRechargeMax" , data.money )
    
    local record = CallModuleFunc(ModuleType.ACTIVITY, "GetOneRechargeRecord" , data.id )

    if info and data.reward_times_max >= info.already then
        if record then
            self:SetNoMay( data )
        else
            self:SetMay( data )
        end
    else --如果没有买过或者次数完了
        self:SetMay( data )
    end
end

--设置可以购买
function Modules.TemplOneRechargeItem:SetMay( data )
    
    --按钮
    if self.btn then
        self.btn:RemoveSelf(true)
        self.btn = nil
    end

    --按钮
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen2")
    self.btn = Util:Button( sp , 
                                 CPPGameLib.Handler(self, self.OnBtn) , 
                                 CPPGameLib.GetString("activity_vipRecharge") , 
                                 Macros.TypefaceSize.button , 
                                 nil , Macros.Color.button_yellow
                                 )
    self.btn:SetPosition( self.Panel_btn:GetContentWidth()/2 , self.Panel_btn:GetContentHeight()/2 )
    self.Panel_btn:AddChild(self.btn) 

    local btnEffect_con = nil
    self.btnEffect_id , btnEffect_con = self:__CreateEffect({res_name = "ui_anniuzoubian2"} , self.btn )
    btnEffect_con:SetPosition(self.btn:GetContentWidth()/2 , self.btn:GetContentHeight()/2 )

    local info = CallModuleFunc(ModuleType.ACTIVITY, "GetOneRechargeMax" , data.money )
    if not info or info.recharge <= info.already then
        self.btn:SetText( CPPGameLib.GetString("activity_vipDraw") )
        GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , false)
    else
        self.btn:SetText( CPPGameLib.GetString("activity_vipRecharge") )
        GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , true)
	end

    local max = 0
    if info and info.already ~= 0 then
        max = info.already
    end

    if max > data.reward_times_max then
        max = data.reward_times_max
    end

    -- 累充数据
    local tab = {
                    { zi = CPPGameLib.GetString( "heroShop_residue" ) , ys = Macros.Color.pack_color , dx = Macros.TypefaceSize.slightly },
                    { zi = data.reward_times_max - max  .. "/" .. data.reward_times_max , ys = Macros.Color.pack_color , dx = Macros.TypefaceSize.slightly },
                }
    --用作好几个组件拼起来居中显示的强迫症控件 2.0版
    local item = Util:LabCenter(tab)
    item:SetAnchorPoint( 0.5 , 0 )
    item:SetPosition( self.btn:GetContentWidth()/2 , self.btn:GetContentHeight()  )
    self.btn:AddChild( item )
end

--设置买过了
function Modules.TemplOneRechargeItem:SetNoMay(data)
    --按钮
    if self.btn then
    	self.btn:RemoveSelf(true)
    	self.btn = nil
    end

    --按钮
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "receive_icon" , true)
	self.btn = Util:Button( sp , CPPGameLib.Handler(self, self.OnNoBtn) )
	self.btn:SetPosition( self.Panel_btn:GetContentWidth()/2 , self.Panel_btn:GetContentHeight()/2 )
	self.Panel_btn:AddChild(self.btn) 
	self.btn:SetTouchEnabled(false)
end

--购买回调
function Modules.TemplOneRechargeItem:OnBtn(data)
    local info = CallModuleFunc(ModuleType.ACTIVITY, "GetOneRechargeMax" , self.__data.money )

    if not info or info.recharge <= info.already then
        CallModuleFunc(ModuleType.CHARGE, "OpenView", BaseViewType.CHARGE)
    else
        self.btn:SetText( CPPGameLib.GetString("activity_vipRecharge") )
    	local info = {}
    	info.data_id = self.__data.id
    	CallModuleFunc(ModuleType.ACTIVITY, "TakeLimitActivityRewardReq" , info )
    end
end

--不可购买回调
function Modules.TemplOneRechargeItem:OnNoBtn(data)

end