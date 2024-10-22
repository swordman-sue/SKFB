
--[[
%% @module: 开服基金
%% @author: yjg
%% @created: 2017年5月11日10:21:46
--]]
-- Panel_welfare
-- fund


Modules = Modules or {}

Modules.TemplFund = Modules.TemplFund or BaseClass(BaseTabView)

function Modules.TemplFund:__init()
    -- self.__tab_params_ex = {15 , true}
    self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_ACTIVIITY_FUND_ITEM)
end

--选项卡界面创建完成回调
function Modules.TemplFund:__LoadedCallback()
    self:__Getwidget()
    self:__ShowUI()
end

--选项卡界面打开回调
function Modules.TemplFund:__OpenCallback()
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
function Modules.TemplFund:__CloseCallback()
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.TemplFund:__Dispose()
end

--获得组件
function Modules.TemplFund:__Getwidget()
    self.Image_contentLst = self:GetWidget("Image_contentLst") 

    self.Panel_welfare = self:GetWidget("Panel_welfare") 

    self.Button_recharge = Util:WidgetButton( 
                                            self:GetWidget("Button_recharge") , 
                                            nil , 
                                            CPPGameLib.Handler(self,self.OnButtonRecharge)  

                                        )
    self.Image_recharge = self:GetWidget("Image_recharge") 

    self.Text_BuyNum = self:GetWidget("Text_BuyNum") 

    self.Text_tips_1 = self:GetWidget("Text_tips_1") 

    self.Text_tips_2 = self:GetWidget("Text_tips_2")
end

--创建组件
function Modules.TemplFund:__ShowUI()

    local sp = Resource.PathTool.GetUIResPath("activitybig/fund" , true )
    local level_gift = Util:Sprite( sp )
    level_gift:SetAnchorPoint(0.5,0.5)
    self.Panel_welfare:AddChild( level_gift , -1 )
    PosTool.Center( level_gift )
    
    local params =  
    {
        item_class = Modules.TemplActivityFundItem,
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

    --提示
    self.tips = Util:RichText( " " , Macros.TypefaceSize.largish )
    self.tips:SetAnchorPoint( 0.5 , 0.5 )
    self.tips:SetPosition( self.Text_BuyNum:GetContentWidth()/2 , self.Text_BuyNum:GetContentHeight()/2 )
    self.Text_BuyNum:AddChild( self.tips )

    --当前vip等级
    local fund_nowVip = Util:Label( CPPGameLib.GetString("fund_nowVip") , Macros.TypefaceSize.normal , Macros.Color.white )
    fund_nowVip:SetAnchorPoint( 0 , 0.5 )
    fund_nowVip:SetPosition( 0 , self.Text_tips_1:GetContentHeight()/2 ) 
    self.Text_tips_1:AddChild( fund_nowVip )

    --vip图样
    local vip_title_icon_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "vip_title_icon", true )
    local vip_title_icon = Util:Sprite(vip_title_icon_sp)
    PosTool.RightTo( fund_nowVip , vip_title_icon , 2 )
    self.Text_tips_1:AddChild( vip_title_icon ) 
    
    --数值
    local vipnum_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "vipnum", true)
    local vipnum = GUI.ImgFont.New(vipnum_sp)
    vipnum:SetText(tostring( CallModuleFunc(ModuleType.VIP , "GetVipLV") ))
    PosTool.RightTo( vip_title_icon , vipnum , 2 )
    self.Text_tips_1:AddChild( vipnum:GetNode() ) 

    --居中容器
    local layout = Util:Layout( 10 , 10 )
    layout:SetAnchorPoint( 0.5 , 0.5 )
    self.Text_tips_2:AddChild( layout )

    --vip字样
    local vip_title_icon = Util:Sprite(vip_title_icon_sp)
    layout:AddChild( vip_title_icon )
    --vip等级
    local vipnum = GUI.ImgFont.New(vipnum_sp)
    vipnum:SetText(tostring( ConfigAdapter.Common.GetFundBuyVipLevel() ))
    layout:AddChild( vipnum:GetNode() )
    --可购买
    local activity_Purchasability = Util:Label( CPPGameLib.GetString("activity_Purchasability") , Macros.TypefaceSize.normal , Macros.Color.white )
    layout:AddChild( activity_Purchasability )

    layout:SetContentSize( vip_title_icon:GetContentWidth() + vipnum:GetNode():GetContentWidth() + 4 + activity_Purchasability:GetContentWidth() , 10 )
    
    vip_title_icon:SetAnchorPoint( 0 , 0.5 )
    vip_title_icon:SetPosition( 0 , layout:GetContentHeight()/2 )

    PosTool.RightTo( vip_title_icon , vipnum , 2 )
    PosTool.RightTo( vipnum , activity_Purchasability , 2 ) 

end

function Modules.TemplFund:__SetShowUI()

    local t1 = CallModuleFunc(ModuleType.ACTIVITY, "GetOpenServerFundCnf")
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
    local _ , now = CallModuleFunc(ModuleType.ACTIVITY, "GetOpenServerNum")
    self.tips:SetText( string.format( Macros.Color.white_rt , CPPGameLib.GetString( "fund_buyTips" , string.format( Macros.Color.green_rt , now ) ) ) )

end

--购买刷新
function Modules.TemplFund:__SetShowLst()

    local t1 = CallModuleFunc(ModuleType.ACTIVITY, "GetOpenServerFundCnf")
    self.lst_list:SetDataList( t1 )
end


function Modules.TemplFund:OnButtonRecharge()
    local level = ConfigAdapter.Common.GetFundBuyVipLevel()
    local vipLevel = CallModuleFunc(ModuleType.VIP , "GetVipLV")

    local diamondNum = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.DIAMOND)

    if vipLevel >= level and diamondNum >= ConfigAdapter.Common.GetFundBuyConsume() then

        CallModuleFunc(ModuleType.ACTIVITY, "OpenServerFundBuyReq")

    elseif diamondNum < ConfigAdapter.Common.GetFundBuyConsume() then

        LogicGUI.TemplAffirmTips.New( { 
                                    type = LogicGUI.TemplAffirmTipsType.YES_NO,
                                    content = CPPGameLib.GetString("fund_noDiamond") ,
                                    cancel_text = CPPGameLib.GetString("common_cancel") ,
                                    confirm_func = function ( ... )
                                        CallModuleFunc(ModuleType.CHARGE, "OpenView", BaseViewType.CHARGE )
                                    end
                                } )        
    else
        --经验
        local nowExp , upExp = CallModuleFunc(ModuleType.VIP , "GetVipExp" , 2 )

        local tips = LogicGUI.TemplAffirmTips.New( { 
                                                        type = LogicGUI.TemplAffirmTipsType.YES_NO,
                                                        content = " ",
                                                        cancel_text = CPPGameLib.GetString("common_cancel") ,
                                                        confirm_func = function ( ... )
                                                            CallModuleFunc(ModuleType.CHARGE, "OpenView", BaseViewType.CHARGE)
                                                        end
                                                    } )

        tips:SetLoadedCallback( function ( ... )
            local text = 
                {
                    { zi = CPPGameLib.GetString("vip_continue"),dx = Macros.TypefaceSize.normal , ys = Macros.Color.content },
                    { tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.DIAMOND), dx = 0.7 , jg = 5},
                    { zi = upExp - nowExp ,ys = Macros.Color.content , dx = Macros.TypefaceSize.normal, jg = 5},
                    { zi = CPPGameLib.GetString("vip_can_1" , level ) ,ys = Macros.Color.content , dx = Macros.TypefaceSize.normal, jg = 5},
                }
            local Affirm = Util:LabCenter(text)
            Affirm:SetPosition(tips:GetNode():GetContentWidth()/2 , tips:GetNode():GetContentHeight()/2 + 25 )
            tips:GetNode():AddChild(Affirm)

            local arena_today_buyTimes = Util:Label( CPPGameLib.GetString("vip_recharge_1") , Macros.TypefaceSize.normal , Macros.Color.content )
            arena_today_buyTimes:SetAnchorPoint(0.5,0.5)
            arena_today_buyTimes:SetPosition(tips:GetNode():GetContentWidth()/2 , tips:GetNode():GetContentHeight()/2 - 25 )
            tips:GetNode():AddChild(arena_today_buyTimes)
        end )
    end
end



Modules = Modules or {}
Modules.TemplActivityFundItem = Modules.TemplActivityFundItem or BaseClass(GUI.ScrollItem)

function Modules.TemplActivityFundItem:__init()
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_ACTIVIITY_FUND_ITEM)   
    self:__Getwidget()
end

function Modules.TemplActivityFundItem:__delete()

end

function Modules.TemplActivityFundItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self:__ShowUI( data )
end

function Modules.TemplActivityFundItem:__Getwidget()
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

function Modules.TemplActivityFundItem:__ShowUI(data)
    -- CPPGameLib.PrintTable("xy" , data,"物品合成响应")  

    
    local huobi  = CPPGameLib.GetConfig("HuoBi",Macros.Game.RolePropType.DIAMOND)
    --头像
    self.__icon = LogicGUI.IconLayer.New()
    self.Panel_icon:AddChild(self.__icon:GetNode())
    PosTool.Center(self.__icon:GetNode())
    local iconData = {id = huobi.big_icon }
    self.__icon:SetData(iconData)
    self.__icon:SetTouchSwallow( false )

    local icon = Util:Sprite( Util:GetMoneyIconImgData(Macros.Game.RolePropType.DIAMOND) )  
    icon:SetAnchorPoint( 0 , 0.5 )
    icon:SetPosition( 2 , self.Image_name:GetContentHeight()/2 )
    self.Image_name:AddChild( icon )
    
    --数值
    local vipnum = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "vipnum", true)
    local diamond = GUI.ImgFont.New(vipnum)
    diamond:SetText(tostring( data.diamond ))
    self.Image_name:AddChild(diamond:GetNode())
    PosTool.RightTo( icon , diamond , 5 )
 
    --提示
    self.tips = Util:RichText( " " , Macros.TypefaceSize.largish )
    self.tips:SetAnchorPoint( 0 , 0.5 )
    self.tips:SetPosition(0 , self.Text_tips:GetContentHeight()/2 )
    self.Text_tips:AddChild( self.tips )

    self:Transfer( data )
end

--设置可以购买
function Modules.TemplActivityFundItem:Transfer(data)
    local roleLevel = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
    local record = CallModuleFunc(ModuleType.ACTIVITY, "GetOpenServerAwardRecord" , data.id )
    local openServerData = CallModuleFunc(ModuleType.ACTIVITY, "GetOpenServerData")
    --等级够了
    if data.need_role_level <= roleLevel and openServerData.is_already_buy_fund ~= 0 then
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
function Modules.TemplActivityFundItem:SetMay( data )

    --提示
    local color = Macros.Color.red_color_rt
    local roleLevel = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
    --如果等级不够的话
    if data.need_role_level <= roleLevel then
        color = Macros.Color.front_color_rt
    end

    local text = string.format( color , CPPGameLib.GetString("common_lev8" , data.need_role_level ) )
    self.tips:SetText( string.format( Macros.Color.keypoint_rt , CPPGameLib.GetString("fund_getTips" , text ) ) )

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
    if openServerData.is_already_buy_fund == 0 or data.need_role_level > roleLevel then
        Util:GreyButton( self.btn )
    else
        self.btn:SetEnabled(true)
        local r, g, b, a = unpack( Macros.Color.button_yellow )
        self.btn:SetOutline(r, g, b, a, 2)
    end
end

--设置买过了
function Modules.TemplActivityFundItem:SetNoMay(data)
    --提示
    local color = Macros.Color.front_color_rt
    local text = string.format( color , CPPGameLib.GetString("common_lev8" , data.need_role_level ) )
    self.tips:SetText( string.format( Macros.Color.keypoint_rt , CPPGameLib.GetString("fund_getTips" , text ) ) )

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
function Modules.TemplActivityFundItem:OnBtn(data)
    local info = {}
    info.reward_id = self.__data.id
    CallModuleFunc(ModuleType.ACTIVITY, "OpenServerFundRewardTakeReq" , info )
end

--不可购买回调
function Modules.TemplActivityFundItem:OnNoBtn(data)

end