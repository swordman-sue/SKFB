
--[[
%% @module: 全名福利
%% @author: yjg
%% @created: 2017年5月11日10:21:46
--]]
-- Panel_fund
-- welfare

Modules = Modules or {}

Modules.TemplRechargeWelfare = Modules.TemplRechargeWelfare or BaseClass(BaseTabView)

function Modules.TemplRechargeWelfare:__init()
    -- self.__tab_params_ex = {15 , true}
    self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_ACTIVIITY_FUND_ITEM)
end

--选项卡界面创建完成回调
function Modules.TemplRechargeWelfare:__LoadedCallback()
    self:__Getwidget()
    self:__ShowUI()
end

--选项卡界面打开回调
function Modules.TemplRechargeWelfare:__OpenCallback()
    CallModuleFunc(ModuleType.ACTIVITY, "OpenServerFundDataReq")
    -- self:__SetShowUI()
    self:BindGlobalEvent(Macros.Event.ActivityModule.ACTIVITY_FUND,function (_, data)
        self:__SetShowUI()
    end)

    self:BindGlobalEvent(Macros.Event.ActivityModule.ACTIVITY_FUND_RECORD,function (_, data)
        self:__SetShowLst()
    end)
end

--选项卡界面关闭回调
function Modules.TemplRechargeWelfare:__CloseCallback()
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.TemplRechargeWelfare:__Dispose()
end

--获得组件
function Modules.TemplRechargeWelfare:__Getwidget()
    self.Image_contentLst = self:GetWidget("Image_contentLst") 

    self.Panel_fund = self:GetWidget("Panel_fund") 

    self.Button_recharge = Util:WidgetButton( 
                                            self:GetWidget("Button_recharge") , 
                                            nil , 
                                            CPPGameLib.Handler(self,self.OnButtonRecharge)  
                                        )
    self.Image_recharge = self:GetWidget("Image_recharge") 

    self.Image_num = {}
    for i = 1 , 4 do
     self.Image_num[i] = self:GetWidget( "Image_num_" .. i ) 
    end
end

--创建组件
function Modules.TemplRechargeWelfare:__ShowUI()

    local sp = Resource.PathTool.GetUIResPath("activitybig/welfare" , true )
    local level_gift = Util:Sprite( sp )
    level_gift:SetAnchorPoint(0.5,0.5)
    self.Panel_fund:AddChild( level_gift , - 1)
    PosTool.Center( level_gift )
    
    local params =  
    {
        item_class = Modules.TemplRechargeWelfareItem,
        item_width = 660.00,
        item_height = 137.00 ,
        row = 4,
        col = 1,
        view_height = self.Image_contentLst:GetContentHeight() - 5  ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.Image_contentLst:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode())

    self.numText = {}
    for i = 1 , #self.Image_num do
     self.numText[i] = Util:Label( "0" , Macros.TypefaceSize.largish , Macros.Color.white ,nil,nil,Macros.Color.button_yellow )
     self.Image_num[i]:AddChild( self.numText[i] )
     PosTool.Center( self.numText[i] )
    end

end

function Modules.TemplRechargeWelfare:__SetShowUI()
    local t1 = CallModuleFunc(ModuleType.ACTIVITY, "GetAllThePeopleWelfareCnf")
    self.lst_list:SetDataList( t1 )

    local openServerData = CallModuleFunc(ModuleType.ACTIVITY, "GetOpenServerData")
    --够没有购买过基金
    if openServerData.is_already_buy_fund ~= 0 then
        self.Button_recharge:SetNormalImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY, "buy_2", true) )
        self.Button_recharge:SetTouchEnabled( false )
        self.Button_recharge:SetScale9Enabled( false )
        self.Image_recharge:SetVisible( false )
    end

    --设置当前购买人数(不信不改
    local nowTab = CallModuleFunc(ModuleType.ACTIVITY, "GetOpenServerNum")
    for i,v in ipairs( nowTab ) do
        self.numText[i]:SetString( v )
    end
end

--购买刷新
function Modules.TemplRechargeWelfare:__SetShowLst()
    local t1 = CallModuleFunc(ModuleType.ACTIVITY, "GetAllThePeopleWelfareCnf")
    self.lst_list:SetDataList( t1 )
end

function Modules.TemplRechargeWelfare:OnButtonRecharge()
    self.__parent_view.__tab:On( self.__parent_view.nowTab[ Macros.Game.SystemID.REBATE ] )
end



Modules = Modules or {}
Modules.TemplRechargeWelfareItem = Modules.TemplRechargeWelfareItem or BaseClass(GUI.ScrollItem)

function Modules.TemplRechargeWelfareItem:__init()
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_ACTIVIITY_FUND_ITEM)   
    self:__Getwidget()
end

function Modules.TemplRechargeWelfareItem:__delete()

end

function Modules.TemplRechargeWelfareItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self:__ShowUI( data )
end

function Modules.TemplRechargeWelfareItem:__Getwidget()
    --条目
    self.Image_item = self:GetWidget("Image_item")
    --图标
    self.Panel_icon = self:GetWidget("Panel_icon")
    --提示
    self.Text_tips = self:GetWidget("Text_tips")
    --按钮占位
    self.Panel_btn = self:GetWidget("Panel_btn")
    --名字
    self.Image_name = self:GetWidget("Image_name")
end

function Modules.TemplRechargeWelfareItem:__ShowUI(data)

    --头像
    self.__icon = LogicGUI.IconLayer.New()
    self.Panel_icon:AddChild(self.__icon:GetNode())
    PosTool.Center(self.__icon:GetNode())
    local iconData = {id = data.item_id_list[1] }
    self.__icon:SetData(iconData)
    self.__icon:SetTouchSwallow( false )
    
    local type_ , info_ = ObjIDTool.GetInfo(data.item_id_list[1])
    local name = Util:Name(info_.name .. " X " .. data.item_num_list[1] , Macros.TypefaceSize.largish , info_.quality or info_.init_quality )
    name:SetAnchorPoint( 0 , 0.5 )
    name:SetPosition( 10 , self.Image_name:GetContentHeight()/2 )
    self.Image_name:AddChild(name)

    --提示
    self.tips = Util:RichText( " " , Macros.TypefaceSize.largish )
    self.tips:SetAnchorPoint( 0 , 0.5 )
    self.tips:SetPosition(0 , self.Text_tips:GetContentHeight()/2 )
    self.Text_tips:AddChild( self.tips )

    self:Transfer( data )
end

--设置可以购买
function Modules.TemplRechargeWelfareItem:Transfer(data)
    print("xy" , "================1")
    local _ , now = CallModuleFunc(ModuleType.ACTIVITY, "GetOpenServerNum")
    local record = CallModuleFunc(ModuleType.ACTIVITY, "GetAllPeopleWelfareTakeRecord" , data.id )
    local openServerData = CallModuleFunc(ModuleType.ACTIVITY, "GetOpenServerData")

    --等级够了
    if data.need_buy_num <= now then
        --有记录
        if record then
            self:SetNoMay( data )
        else
            self:SetMay( data )
        end
    else
        self:SetMay( data )
    end
end

--设置可以购买
function Modules.TemplRechargeWelfareItem:SetMay( data )
    local _ , now = CallModuleFunc(ModuleType.ACTIVITY, "GetOpenServerNum")
    --提示
    local color = Macros.Color.red_color_rt
    --如果等级不够的话
    if data.need_buy_num <= now then
        color = Macros.Color.front_color_rt
    end

    local text = string.format( color , data.need_buy_num )
    self.tips:SetText( string.format( Macros.Color.keypoint_rt , CPPGameLib.GetString("fund_buyTips_" , text ) ) )

    --按钮
    if self.btn then
        self.btn:RemoveSelf(true)
        self.btn = nil
    end

    --按钮
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_green")
    self.btn = Util:Button( sp , 
                                 CPPGameLib.Handler(self, self.OnBtn) , 
                                 CPPGameLib.GetString("activity_vipRecharge") , 
                                 Macros.TypefaceSize.button , 
                                 nil , Macros.Color.button_yellow 
                                 )
    self.btn:SetPosition( self.Panel_btn:GetContentWidth()/2 , self.Panel_btn:GetContentHeight()/2 )
    self.Panel_btn:AddChild(self.btn) 

    --如果等级不够的话
    local openServerData = CallModuleFunc(ModuleType.ACTIVITY, "GetOpenServerData")
    if data.need_buy_num > now then
        Util:GreyButton( self.btn )
    else
        self.btn:SetEnabled(true)
        local r, g, b, a = unpack( Macros.Color.button_yellow  )
        self.btn:SetOutline(r, g, b, a, 2)
    end
end

--设置买过了
function Modules.TemplRechargeWelfareItem:SetNoMay(data)
    --提示
    local color = Macros.Color.front_color_rt
    local text = string.format( color , data.need_buy_num )
    self.tips:SetText( string.format( Macros.Color.keypoint_rt , CPPGameLib.GetString("fund_buyTips_" , text ) ) )

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
function Modules.TemplRechargeWelfareItem:OnBtn(data)

    
    local info = {}
    info.welfare_id = self.__data.id
    CallModuleFunc(ModuleType.ACTIVITY, "AllPeopleWelfareTakeReq" , info )
end

--不可购买回调
function Modules.TemplRechargeWelfareItem:OnNoBtn(data)

end