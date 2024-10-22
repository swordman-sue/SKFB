--[[
%% @module: 自定义超值购
%% @author: yjg
%% @created: 2018年2月26日17:41:59
--]]

Modules = Modules or {} 
Modules.CustompurchaseView = Modules.CustompurchaseView or BaseClass(BaseView)

function Modules.CustompurchaseView:__init()
    self.__layout_name = ""  
    self.__layout_mode = BaseView.LayoutMode.CODE 
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__open_callback = function()
        self:__OpenCallback()
    end
end

function Modules.CustompurchaseView:__OpenCallback( )
    CallModuleFunc(ModuleType.CUSTOM_PURCHASE, "SetOpenType" , unpack(self.__open_data) )

    self:__Getwidget()
    self:__ShowUI()
    self:__Server()
    -- self:__SetShowUI()
end	

function Modules.CustompurchaseView:__Dispose()
end

function Modules.CustompurchaseView:__CreateWidgetFromLuaCode()
    self.widget = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height)
    return self.widget
end

function Modules.CustompurchaseView:__Server()

    CallModuleFunc(ModuleType.CUSTOM_PURCHASE, "SuperDiscountDataReq")

    -- 超值折扣数据响应
    self:BindGlobalEvent( Macros.Event.CustomPurchaseModule.CUSTOM_PURCHASE_DATA , function ( ... )
        self:__SetShowUI()
    end )
    
    -- 领取超值折扣购买奖励响应
    self:BindGlobalEvent( Macros.Event.CustomPurchaseModule.DISCOUNT_BUY_REWARD , function ( ... )
        self:__SetShowUI()
    end)

end

function Modules.CustompurchaseView:__Getwidget()
    local cnf = CallModuleFunc(ModuleType.CUSTOM_PURCHASE, "GetCustomPurchaseCnf" , unpack(self.__open_data) )

    --背景
    if cnf.bg then
        self.bg = Util:Sprite( Resource.PathTool.GetUIResPath( cnf.bg ) )   
        self.bg:SetAnchorPoint( 0.5 , 0.5 ) 
        self.widget:AddChild( self.bg )
        local x , y
        if cnf.bg_pos then
            x = cnf.bg_pos[1]
            y = cnf.bg_pos[2]
        end
        PosTool.Center( self.bg , x , y )
    end


    --图片 普通
    if cnf.sprite_lst then
        for i , v in ipairs( cnf.sprite_lst  ) do

            local sprite = Util:Sprite( Resource.PathTool.GetUIResPath( v ) )   
            sprite:SetAnchorPoint( 0.5 , 0.5 ) 
            self.bg:AddChild( sprite )

            local x , y
            if cnf.sprite_pos_lst then
                x = cnf.sprite_pos_lst[i][1]
                y = cnf.sprite_pos_lst[i][2]
            end
            PosTool.Center( sprite , x , y )

        end
    end


    --图片 普通
    if cnf.sprite_language_lst then
        for i , v in ipairs( cnf.sprite_language_lst  ) do

            local sprite = Util:Sprite( Resource.PathTool.GetUIResPath( v , true ) )   
            sprite:SetAnchorPoint( 0.5 , 0.5 ) 
            self.bg:AddChild( sprite )

            local x , y
            if cnf.sprite_language_pos_lst then
                x = cnf.sprite_language_pos_lst[i][1]
                y = cnf.sprite_language_pos_lst[i][2]
            end
            PosTool.Center( sprite , x , y )

        end
    end


    --图标容器
    self.iconLayou = {}
    self.isLayou = false
    if cnf.layou_lst then
        for i , v in ipairs( cnf.layou_lst  ) do
            self.iconLayou[i] = Util:Sprite( Resource.PathTool.GetUIResPath( v ) )   
            self.iconLayou[i]:SetAnchorPoint( 0.5 , 0.5 ) 
            self.bg:AddChild( self.iconLayou[i] )

            local x , y
            if cnf.layou_pos_lst then
                x = cnf.layou_pos_lst[i][1]
                y = cnf.layou_pos_lst[i][2]
            end
            PosTool.Center( self.iconLayou[i] , x , y )
        end
    else
        self.isLayou = true
        self.iconLayou[1] = Util:Layout( 90 * 4 , 110 )
        self.iconLayou[1]:SetAnchorPoint( 0.5 , 0.5 ) 
        self.bg:AddChild( self.iconLayou[1] )
        local x , y
        if cnf.layou_pos_lst and cnf.layou_pos_lst[1] then
            x = cnf.layou_pos_lst[1][1]
            y = cnf.layou_pos_lst[1][2]
        end
        PosTool.Center( self.iconLayou[1] , x , y )
    end


    --文字描述
    if cnf.text_lst then
        for i , v in ipairs( cnf.text_lst  ) do
            local text = Util:RichText( v )   
            text:SetAnchorPoint( 0.5 , 0.5 ) 
            self.bg:AddChild( text )

            local x , y
            if cnf.text_pos_lst then
                x = cnf.text_pos_lst[i][1]
                y = cnf.text_pos_lst[i][2]
            end
            PosTool.Center( text , x , y )
        end
    end

    --活动时间
    self.time = {}

    local anchor_x = 0.5
    self.timeColor = "%s"
    local zoom = nil

    if cnf.time_info_lst then
        if cnf.time_info_lst[1] then
            anchor_x = cnf.time_info_lst[1]
        end

        if cnf.time_info_lst[2] then
            self.timeColor = cnf.time_info_lst[2]
        end

        if cnf.time_info_lst[3] then
            zoom = cnf.time_info_lst[3]
        end
    end

    for i = 1 , 2 do
        self.time[i] = Util:RichText( "v" , zoom )   
        self.time[i]:SetAnchorPoint( anchor_x , 0.5 ) 
        self.bg:AddChild( self.time[i] )

        local x , y
        if cnf.time_lst and cnf.time_lst[i] then
            x = cnf.time_lst[i][1]
            y = cnf.time_lst[i][2]
        end
        PosTool.Center( self.time[i] , x , y )
    end


    --模型列表
    if cnf.model_lst then
        --特效
        for i , v in ipairs(cnf.model_lst) do

            zorder = nil
            if cnf.model_zob_lst and cnf.model_zob_lst[i] then
                zorder = cnf.model_zob_lst[i]
            end
            local icon = LogicGUI.Model.New({ path = Resource.PathTool.GetModelPath( "hero" , v ) , parent = self.bg , zorder = zorder })
            
            local x , y
            if cnf.model_pos_lst then
                x = tonumber(cnf.model_pos_lst[i][1])
                y = tonumber(cnf.model_pos_lst[i][2])
            end

            PosTool.Center( icon , x , y )
        end
    end

    --特效
    if cnf.effects_lst then
        --特效
        for i , v in ipairs(cnf.effects_lst) do
            local id = tonumber(v)
            local tab = {}
            if id then
                tab = { id = id }
            else
                tab = { res_name = v }
            end

            local effectid,effect = self:__CreateEffect( tab , self.bg , cnf.effects_zob_lst[i] )
            effect:SetAnchorPoint(0.5,0.5)
            local x , y
            if cnf.effects_pos_lst then
                x = tonumber(cnf.effects_pos_lst[i][1])
                y = tonumber(cnf.effects_pos_lst[i][2])
            end

            PosTool.Center( effect , x , y )
        end
    end


    --按钮
    local t1 = {  --关闭
                  [1] = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW,"btn_red_close"),
                  --领取
                  [2] = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN,"btn_btn_green"),
                  --价格
                  [3] = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN,"btn_btn_green"),
               }
    self.btn = {}
    for i = 1 , 3 do
        self.btn[i] = Util:Button( t1[i] ,function ( ... )
        end , nil , Macros.TypefaceSize.popup )
        self.btn[i]:SetAnchorPoint(0.5,0.5)
        self.bg:AddChild(self.btn[i])
        self.btn[i]:SetVisible(false)

        local x , y
        if cnf.btn_pos_lst and cnf.btn_pos_lst[i] then
            x = tonumber(cnf.btn_pos_lst[i][1])
            y = tonumber(cnf.btn_pos_lst[i][2])
        end
        PosTool.Center( self.btn[i] , x , y )
    end

end


function Modules.CustompurchaseView:__ShowUI()

    --奖励的物品信息
    local cnf = CallModuleFunc(ModuleType.CUSTOM_PURCHASE, "GetNewSuperDiscountBuyRewardCnf" , unpack(self.__open_data) )

    --图标列表
    local params =  
    {
        item_class = Modules.CustompurchaseViewItem,
        item_width = 90 ,
        item_height = 110 ,
        row = 1,
        col = 4,
        item_space_r = 0 ,
        view_width = self.iconLayou[1]:GetContentWidth() + 10,
        view_height = self.iconLayou[1]:GetContentHeight() + 10 ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.iconLayou[1]:AddChild(self.lst_list:GetNode())
    self.lst_list:SetPosition(self.iconLayou[1]:GetContentWidth()/2 , self.iconLayou[1]:GetContentHeight()/2 )

    --今天是开活动的第几天
    local days = CallModuleFunc(ModuleType.CUSTOM_PURCHASE, "GetActivityHowManyDays" )
    self.lst_list:SetDataList( cnf[days].lst )

    --天数
    self.day = Util:Name( "0/0" )
    self.day:SetAnchorPoint( 0.5 , 1 )
    self.day:SetPosition( self.btn[2]:GetPositionX() , self.btn[2]:GetPositionY() - 12 )
    self.bg:AddChild( self.day )

    --已购买
    self.buy_2 = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "buy_2" , true ) )
    self.buy_2:SetPosition( self.btn[3]:GetPositionX() , self.btn[3]:GetPositionY() )
    self.bg:AddChild( self.buy_2 ) 
    self.buy_2:SetVisible( false )

    --已购买
    self.buy_1 = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "buy_1" , true ) )
    self.bg:AddChild( self.buy_1 ) 
    PosTool.Center( self.buy_1 , 110 , 40 )
    self.buy_1:SetVisible( false )

    local BtnEffect_con = nil
    self.btnEffect_id , BtnEffect_con = self:__CreateEffect({res_name = "ui_anniuzoubian"} , self.bg , 999)
    BtnEffect_con:SetAnchorPoint(0.5,0.5)
    BtnEffect_con:SetPosition( self.btn[3]:GetPositionX() , self.btn[3]:GetPositionY() )
    GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , false )

    local BtnEffect_con = nil
    self.btnEffect , BtnEffect_con = self:__CreateEffect({res_name = "ui_anniuzoubian"} , self.bg , 999)
    BtnEffect_con:SetAnchorPoint(0.5,0.5)
    BtnEffect_con:SetPosition( self.btn[2]:GetPositionX() , self.btn[2]:GetPositionY() )
    GlobalEffectMgr:SetEffectVisible(self.btnEffect , false )
end

function Modules.CustompurchaseView:__SetShowUI()
    --时间
    self:SetTime()

    --超值购2 
    if unpack(self.__open_data) == Macros.Game.BE_DIFFERENT.OVERFLOW_GEM then
        self:OverflowGem()
    elseif unpack(self.__open_data) == Macros.Game.BE_DIFFERENT.OVERFLOW_HERO then 
        self:OverflowGem()
    end
end

--设置时间
function Modules.CustompurchaseView:SetTime()
    local info  = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.CUSTOM_PURCHASE )
    --开始时间
    self.time[1]:SetText( string.format( self.timeColor , TimeTool.GetTimeFormat( info.star , TimeTool.defaul_format[20] )  ) )
    --结束时间
    self.time[2]:SetText( string.format( self.timeColor , TimeTool.GetTimeFormat( info.over , TimeTool.defaul_format[20] )  ) )
end

--设置图标
function Modules.CustompurchaseView:SetIcon()

end

--超值宝石
function Modules.CustompurchaseView:OverflowGem()
    --按钮处理
    self:OverflowGemBtn()

    --进度条处理
    self:OverflowGemBar()
end

--按钮处理 1:关闭 2:领取 3:充值
function Modules.CustompurchaseView:OverflowGemBtn()
    --关闭
    WidgetUnity.SetWidgetListener( self.btn[1] , nil, nil, function( ... )
        --关闭
        self:CloseManual()
    end, nil, 0.5*1000)

    local cnf = CallModuleFunc(ModuleType.CUSTOM_PURCHASE, "GetNewSuperDiscountBuyRewardCnf" , unpack(self.__open_data) )
    local days = CallModuleFunc(ModuleType.CUSTOM_PURCHASE, "GetActivityHowManyDays" )
    --显示关闭按钮
    self.btn[1]:SetVisible(true)

    --领取
    WidgetUnity.SetWidgetListener( self.btn[2] , nil, nil, function( ... )
        CallModuleFunc(ModuleType.CUSTOM_PURCHASE, "SuperDiscountAccumulateRewardReq" )
    end, nil, 0.5*1000)
    local r, g, b, a = unpack( Macros.Color.button_red )
    self.btn[2]:SetOutline(r, g, b, a, 2)
    self.btn[2]:SetText( CPPGameLib.GetString("manor_get") )
    -- 当前购买次数
    local curBuyTime = CallModuleFunc(ModuleType.CUSTOM_PURCHASE,"GetCurBuyTime")
    local rewardTimes = CallModuleFunc(ModuleType.CUSTOM_PURCHASE,"GetTakeAccumulateRewardTimes")
    local info , lst = CallModuleFunc(ModuleType.CUSTOM_PURCHASE, "GetNewSuperDiscountAccumulateRewardCnf" , unpack(self.__open_data) , rewardTimes )
    local max = 0
    if info then
        max = info.need_buy_times
    else
        max = lst[1].need_buy_times
    end
    --进度条显示
    if curBuyTime >= max then
        self.btn[2]:SetVisible(true)
        GlobalEffectMgr:SetEffectVisible(self.btnEffect , true )
    else
        self.btn[2]:SetVisible(false)
        GlobalEffectMgr:SetEffectVisible(self.btnEffect , false )
    end

    --充值
    -- 购买状态
    local status = CallModuleFunc(ModuleType.CUSTOM_PURCHASE,"GetStatus")
    self.buy_2:SetVisible( false )
    if status == "weiGouMai" then
        self.btn[3]:SetText( CallModuleFunc(ModuleType.SUNDRY, "GetCurrencyText" , cnf[days].pay_id ) )
        self.btn[3]:SetVisible( true )
        GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , false )
    elseif status == "keLingQu" then
        self.btn[3]:SetText( CPPGameLib.GetString("manor_get") )
        self.btn[3]:SetVisible( true )
        GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , true )
    elseif status == "yiLingQu" then
        self.btn[3]:SetVisible( false )
        self.buy_2:SetVisible( true )
        GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , false )
    end

    WidgetUnity.SetWidgetListener( self.btn[3] , nil, nil, function( ... )
        if status == "weiGouMai" then
            --服务端GM命令
            CallModuleFunc(ModuleType.CHARGE , "RechargeOrderIDReq" , Macros.Game.RechargeType.NEW_SUPER_DISCOUNTVIEW , {recharge_id = cnf[days].pay_id } )
        elseif status == "keLingQu" then
            CallModuleFunc(ModuleType.CUSTOM_PURCHASE,"SuperDiscountBuyRewardReq")
        end
    end, nil, 0.5*1000)
    local r, g, b, a = unpack( Macros.Color.button_red )
    self.btn[3]:SetOutline(r, g, b, a, 2)

    if rewardTimes ~= 0 then
        self.buy_1:SetVisible(true)
    end
end


--进度条处理
function Modules.CustompurchaseView:OverflowGemBar()
    --进度条
    if not self.__bar_exp then
        local imgdata_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_bar_bar_custompurchase")
        local imgdata_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_bar_bg_custompurchase")
        --进度条
        self.__bar_exp = GUI.ProgressBar9Layer.New(imgdata_bar, imgdata_bg , 282 , 18 , 284 , 20 , 100, 100)
        self.__bar_exp:SetAnchorPoint(0.5, 0.5)
        self.bg:AddChild(self.__bar_exp:GetNode())
        self.__bar_exp:SetPosition( self.btn[2]:GetPositionX() , self.btn[2]:GetPositionY() )
    end

    self.__bar_exp:SetVisible( false )
    self.btn[2]:SetVisible(false)
    GlobalEffectMgr:SetEffectVisible(self.btnEffect , false )

    local curBuyTime = CallModuleFunc(ModuleType.CUSTOM_PURCHASE,"GetCurBuyTime")
    local rewardTimes = CallModuleFunc(ModuleType.CUSTOM_PURCHASE,"GetTakeAccumulateRewardTimes")
    local info , lst = CallModuleFunc(ModuleType.CUSTOM_PURCHASE, "GetNewSuperDiscountAccumulateRewardCnf" , unpack(self.__open_data) , rewardTimes )
    local max = 0
    if info then
        max = info.need_buy_times
        self.__bar_exp:SetProgressMax( max )
        self.__bar_exp:SetProgress( curBuyTime )
    else
        max = lst[1].need_buy_times
        self.__bar_exp:SetProgressMax( max )
        self.__bar_exp:SetProgress( curBuyTime )   
    end

    --天数
    self.day:SetString( CPPGameLib.GetString( "custom_day" , curBuyTime .." / ".. max ) )

    --进度条显示
    if curBuyTime >= max then
        self.__bar_exp:SetVisible( false )
        self.btn[2]:SetVisible(true)
        self.day:SetString( " " )
        GlobalEffectMgr:SetEffectVisible(self.btnEffect , true )
    else
        self.btn[2]:SetVisible(false)
        self.__bar_exp:SetVisible( true )
        GlobalEffectMgr:SetEffectVisible(self.btnEffect , false )
    end

    --累计奖励
    if rewardTimes ~= 0 then
        self.btn[2]:SetVisible(false)
        self.__bar_exp:SetVisible( false )
        self.day:SetString( " " )
        GlobalEffectMgr:SetEffectVisible(self.btnEffect , false )
    end
end



Modules = Modules or {}
Modules.CustompurchaseViewItem = Modules.CustompurchaseViewItem or BaseClass(GUI.ScrollItem)

function Modules.CustompurchaseViewItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.CustompurchaseViewItem:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(85,85)
    self.layout:SetBackGroundColor(0, 0, 0)
    return self.layout
end

function Modules.CustompurchaseViewItem:__delete()
    
end

function Modules.CustompurchaseViewItem:SetData( data )
    GUI.ScrollItem.SetData(self, data)
    self:__ShowUI( )
end

function Modules.CustompurchaseViewItem:__ShowUI()
    --头像
    self.icon = LogicGUI.IconLayer.New(true)
    self.icon:SetPosition(self.layout:GetContentWidth()/2 ,self.layout:GetContentHeight() / 2 )
    self.icon:SetAnchorPoint(0.5, 0.5 )
    self.layout:AddChild(self.icon:GetNode())
    self.icon:SetData( self.__data )
end











