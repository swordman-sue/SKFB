--[[
%% @module: 转盘
%% @author: yjg
%% @created: 2018年3月16日15:18:17
--]]

Modules = Modules or {} 
Modules.TurntableView = Modules.TurntableView or BaseClass(BaseView)

Modules.TurntableView.REPEATEDIY = 5

function Modules.TurntableView:__init()
    self.__layout_name = "turntable"	
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
    self.__title_bar_params = 
    {
      title = CPPGameLib.GetString("turntable"), 
      show_diamond = true,
      show_gold = true,
      show_stamina = true,
      show_energy = true,
      close_func = function()
        if self.touch == true then
            self:CloseManual()
        end
      end,
    }

    self.__open_callback = function()
        self:__OpenCallback()
    end

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.TURNTABLE)  
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.TURNTABLE , true )  
end

function Modules.TurntableView:__OpenCallback( )
    local activityTime = CallModuleFunc(ModuleType.TURNTABLE,"GetActivityTime")
    
    self.touch = true
    self:__Getwidget()
    self:__ShowUI()
    self:__Server()
end	

function Modules.TurntableView:__Server()
    CallModuleFunc(ModuleType.TURNTABLE,"TurntableDataReq")
    self:BindGlobalEvent(Macros.Event.TurntableModule.TURNTABLE_DATA , function (  )
        self:__SetShowUI( )
    end)

    --转圈
    self:BindGlobalEvent(Macros.Event.TurntableModule.WHIRL_DATA , function (  )
        self:ArrowsDispose( )
        self:__SetJackpot()
    end)
end

function Modules.TurntableView:__Dispose()
    --图片
    if self.customSp then
        for i = 1 , #self.customSp do
            self.customSp[i]:RemoveSelf( true )
            self.customSp[i] = nil
        end
    end

    --特效
    if self.customEffects then
        for i = 1 , #self.customEffects do
            GlobalEffectMgr:DelEffect(self.customEffects[i])
            self.customEffects[i] = nil
        end
    end

    if self.time then
        GlobalTimerQuest:CancelQuest(self.time)
        self.time = nil
    end

    self:DeleteEffects()
end

function Modules.TurntableView:__Getwidget( )
    --装饰
    self.Image_decorate = self:GetWidget("Image_decorate") 

    --居中
    self.Panel_center = self:GetWidget("Panel_center")

    --背景
    self.Image_bg = self:GetWidget("Image_bg")

    --标题
    self.Image_headline = self:GetWidget("Image_headline")
    self.Image_headline:SetVisible(false)

    --列表标题
    self.Panel_headline = self:GetWidget("Panel_headline")
    
    --列表
    self.Panel_lst = self:GetWidget("Panel_lst")

    --点一下
    self.Button_Click_1 = Util:WidgetButton( 
                                        self:GetWidget("Button_Click_1") , 
                                        CPPGameLib.GetString("turntable_click" , 1 ) , 
                                        CPPGameLib.Handler(self,self.OnButtonClick1),
                                        nil,Macros.Color.lilyWhite,Macros.Color.button_red
                                    )
    self.Button_Click_1:SetEnabled( false )
    --点一下
    self.Button_Click_10 = Util:WidgetButton( 
                                        self:GetWidget("Button_Click_10") , 
                                        CPPGameLib.GetString("turntable_click" , Modules.TurntableView.REPEATEDIY ) , 
                                        CPPGameLib.Handler(self,self.OnButtonClick10),
                                        nil,Macros.Color.lilyWhite,Macros.Color.button_red
                                    )
    self.Button_Click_10:SetEnabled( false )

    --转盘
    self.Image_dial = self:GetWidget("Image_dial")

    --led
    self.Panel_led = {}
    self.Image_led = {}
    for i = 1 , 8 do
        self.Panel_led[i] = self:GetWidget("Panel_led_"..i)
        self.Image_led[i] = self:GetWidget("Image_led_"..i)
    end

    --数字
    self.Image_num = self:GetWidget("Image_num")

    --奖励按钮
    self.Button_directstore = Util:WidgetButton( 
                                        self:GetWidget("Button_directstore") , 
                                        " " , 
                                        CPPGameLib.Handler(self,self.OnButtonDirectstore)
                                    )
    self.Button_directstore:SetNormalImage( Resource.PathTool.GetSystemEntryIconPath("btn_directstore"),TextureResType.LOCAL )

    local Button_everyday = Util:Name( CPPGameLib.GetString("turntable_award") , Macros.TypefaceSize.largish )
    Button_everyday:SetAnchorPoint(0.5,0.5)
    Button_everyday:SetPosition(self.Button_directstore:GetContentWidth()/2 , 15 )
    self.Button_directstore:AddChild(Button_everyday)
end 

function Modules.TurntableView:__ShowUI( )
    --图标
    self.showIcon = {}
    local step = 360/8
    local count = 0
    for i = 1 , 8 do
        self.showIcon[i] = LogicGUI.IconLayer.New(true)
        -- self.showIcon[i]:SetData( { id = 13999 , num = i } )
        self.Panel_led[i]:AddChild( self.showIcon[i]:GetNode() )
        PosTool.Center( self.showIcon[i] , 0 , 140 )
        self.showIcon[i]:SetScaleFactor( 0.7 )
        
        CPPActionManager.RotateTo( self.showIcon[i]:GetNode() , 0 , self.count )
        count = count + step
        self.count = 360 - count
    end

    --箭头
    self.arrows = Util:Layout( self.Image_dial:GetContentWidth() , self.Image_dial:GetContentHeight() )
    self.arrows:SetAnchorPoint( 0.5 , 0.5 )
    self.Image_dial:AddChild( self.arrows )
    PosTool.Center( self.arrows )
    -- self.arrows:SetVisible( false )

    local arrows = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TURNTABLE, "sp_turntable_arrows"))
    self.arrows:AddChild( arrows )
    PosTool.Center( arrows , 0 , 100 )

    --标题
    local title = Util:Label( CPPGameLib.GetString("turntable_lotteryRecord") , Macros.TypefaceSize.largish , Macros.Color.Special )
    self.Panel_headline:AddChild( title )
    PosTool.Center( title )

    --左线
    local sp_turntable_wire = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TURNTABLE, "sp_turntable_wire" ) )
    sp_turntable_wire:SetAnchorPoint( 0.5 , 0.5 )
    self.Panel_headline:AddChild( sp_turntable_wire )
    PosTool.Center( sp_turntable_wire , 0 - ( (title:GetContentWidth()/2) + (sp_turntable_wire:GetContentWidth()/2) + 10 ) )

    --右线
    local sp_turntable_wire = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TURNTABLE, "sp_turntable_wire" ) )
    sp_turntable_wire:SetAnchorPoint( 0.5 , 0.5 )
    sp_turntable_wire:SetFlipX(true)
    self.Panel_headline:AddChild( sp_turntable_wire )
    PosTool.Center( sp_turntable_wire , ( (title:GetContentWidth()/2) + (sp_turntable_wire:GetContentWidth()/2) + 10 ) )

    --列表
    self.lst_list = UIListViewEx.Create()
    self.lst_list:SetAnchorPoint( 0.5 , 0.5 )
    self.lst_list:SetContentSize( self.Panel_lst:GetContentWidth() , self.Panel_lst:GetContentHeight() )
    self.Panel_lst:AddChild(self.lst_list)
    PosTool.Center( self.lst_list , 0 , 2 )
    self.lst_list:SetTouchEnabled( false )

    --当前积分
    local cur_getPoint = Util:Label( CPPGameLib.GetString( "cur_getPoint" ) , Macros.TypefaceSize.normal , Macros.Color.white )
    cur_getPoint:SetAnchorPoint( 0 , 0.5 )
    cur_getPoint:SetPosition( 10 , self.Image_num:GetContentHeight()/2 )
    self.Image_num:AddChild( cur_getPoint )

    self.cur_getPoint = Util:Label( "00000000" , Macros.TypefaceSize.normal , Macros.Color.white )
    self.Image_num:AddChild( self.cur_getPoint )
    PosTool.RightTo( cur_getPoint , self.cur_getPoint )

    --活动时间
    self.direct_buy_time = Util:Label( CPPGameLib.GetString( "direct_buy_time" , 0 ) , Macros.TypefaceSize.slightly , Macros.Color.white )
    self.direct_buy_time:SetAnchorPoint( 0 , 0.5 )
    self.direct_buy_time:SetPosition( 125 , 14 )
    self.Image_headline:AddChild( self.direct_buy_time )

    --奖池金额
    local turntable_jackpotMoney = Util:Name( CPPGameLib.GetString( "turntable_jackpotMoney" ) , Macros.TypefaceSize.slightly , 102 )
    self.Image_dial:AddChild( turntable_jackpotMoney )
    PosTool.Center( turntable_jackpotMoney , 0 , 20 )

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "black1_bg" )
    self.bag_num = Util:Sprite9( sp )
    self.bag_num:SetContentSize( 197 , self.bag_num:GetContentHeight() )
    self.bag_num:SetAnchorPoint( 1 , 0.5 )
    self.bag_num:SetPosition( self.Image_bg:GetContentWidth() , -37 )
    self.Image_bg:AddChild( self.bag_num , 1 )

end

function Modules.TurntableView:__SetShowUI( )   

    self.Button_Click_1:SetEnabled( true )
    self.Button_Click_10:SetEnabled( true )
    --当前积分
    local nowIntegral = CallModuleFunc(ModuleType.TURNTABLE,"GetNowIntegral")
    self.cur_getPoint:SetString( nowIntegral )

    --持有数量
    local cnf = CallModuleFunc( ModuleType.TURNTABLE,"GetTurntableCnf" ) 
    local num = CallModuleFunc(ModuleType.Pack,"GetNumByItemID", cnf.cost_item_id ) 
    local type_ , info_ = ObjIDTool.GetInfo( cnf.cost_item_id )
    local tab = {
                 { zi = CPPGameLib.GetString("login_possess") },
                 { tp = Resource.PathTool.GetItemPath(info_.res_id) , dx = 0.5},
                 { zi = "x" .. num , jg = 3 },
                }

    if self.login_possess then
        self.login_possess:RemoveSelf(true)
        self.login_possess = nil
    end
    self.login_possess = Util:LabCenter( tab )
    self.login_possess:SetAnchorPoint( 0 , 0.5 )
    self.login_possess:SetPosition( 10 , self.bag_num:GetContentHeight()/2 )
    self.bag_num:AddChild( self.login_possess )


    --活动时间
    local activityTime = CallModuleFunc(ModuleType.TURNTABLE,"GetActivityTime")
    if activityTime then
        local year, month, day, hour, minute, second, week = TimeTool.GetDateFromTimeStamp( activityTime.star)
        local star = year .."." .. month .."." .. day  .." " .. hour ..":" ..  minute

        local year, month, day, hour, minute, second, week = TimeTool.GetDateFromTimeStamp( activityTime.over)
        local over = year .."." .. month .."." .. day  .." " .. hour ..":" ..  minute

        self.direct_buy_time:SetString( CPPGameLib.GetString( "direct_buy_time" , star .. " ~ " .. over ) )
    end

    --列表调整
    local recordList = CallModuleFunc(ModuleType.TURNTABLE,"GetRecordList")

    self.lst_list:RemoveAllItems()

    for i , v in ipairs( recordList ) do
        local layou = Util:Layout( self.Panel_lst:GetContentWidth() , self.Panel_lst:GetContentHeight()/4 )

        local name = string.format( Macros.Color.Special_rt , v.role_name ) 
        local congratulate = string.format( Macros.Color.white_rt , CPPGameLib.GetString("turntable_Lucky") ) 
        local gift = string.format( Macros.Quality_RT[ v.quality or 2 ] , v.text ) 

        --名字
        local text = Util:RichText( name..congratulate..gift )
        text:SetAnchorPoint( 0.5 , 0.5 )
        layou:AddChild( text )
        PosTool.Center( text )

        self.lst_list:InsertCustomItem( layou , 0 )
    end

    --奖池列表
    local lst = CallModuleFunc(ModuleType.TURNTABLE,"GetTurntablePond")

    for i , v in ipairs( lst ) do
        self.showIcon[i]:SetData( {id = v.id or v.item_id , num = v.num} )

        if v.isCurrency then
            self.showIcon[i]:SetNum( v.num.."%" )
        end

        if not self.showIconEff then
            self.showIconEff = {}
        else
            if self.showIconEff[i] then
                self:__DelEffect( self.showIconEff[i] )
                self.showIconEff[i] = nil
            end
        end

        if v.icon_effects then
            local effect = nil
            self.showIconEff[i], effect = self:__CreateEffect({res_name = v.icon_effects}, self.showIcon[i]:GetNode() )
            PosTool.Center(effect)
        end

    end

    local cnf = CallModuleFunc(ModuleType.TURNTABLE,"GetTurntableCnf")
    if not cnf then
        return 
    end
    Modules.TurntableView.REPEATEDIY = cnf.one_key_lottery_times

    self:SetJackpotMoney()

    self:__SetCustomUI()
end

--重置箭头
function Modules.TurntableView:ResetArrows( )
    CPPActionManager.RotateTo( self.arrows , 0 , 0 )
end

--点击1
function Modules.TurntableView:OnButtonClick1( )
    local cnf = CallModuleFunc( ModuleType.TURNTABLE,"GetTurntableCnf" ) 
    local diamondNum = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.DIAMOND)

    local num = 0
    if cnf then
        local id = cnf.cost_item_id
        num = CallModuleFunc(ModuleType.Pack,"GetNumByItemID", cnf.cost_item_id ) 
        if num < cnf.cost_item_num then
            if cnf.normal_lottery_cost_diamond then
                if diamondNum < cnf.normal_lottery_cost_diamond then
                    self.touch = true
                    GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("shop_can") )
                    return
                end
            else
                local _ , cnf_ = ObjIDTool.GetInfo( cnf.cost_item_id )
                self.touch = true
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("hero_potential_tips2" , cnf_.name ) )
                return
            end
        end
    else
        return
    end

    if self.touch == true then
        local info = {}
        info.lottery_type = 0
        -- 1是道具
        info.is_cost_item = 1
        if num < cnf.cost_item_num then
            info.is_cost_item = 0
        end

        local type_ = CallModuleFunc(ModuleType.TURNTABLE , "GetNowNotShow")
        if type_ ~= true and info.is_cost_item == 0 then     
            self:DiamondTips( cnf.normal_lottery_cost_diamond , 1 , function( ... )
                CallModuleFunc(ModuleType.TURNTABLE,"TurntableLotteryReq" , info )
                self.touch = false
            end )
        else
            CallModuleFunc(ModuleType.TURNTABLE,"TurntableLotteryReq" , info )
            self.touch = false
        end
    end
end

--点击10
function Modules.TurntableView:OnButtonClick10( )
    local cnf = CallModuleFunc( ModuleType.TURNTABLE,"GetTurntableCnf" ) 
    local diamondNum = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.DIAMOND)
    local num = 0
    if cnf then
        local id = cnf.cost_item_id
        num = CallModuleFunc(ModuleType.Pack,"GetNumByItemID", cnf.cost_item_id ) 
        if num < ( cnf.cost_item_num * Modules.TurntableView.REPEATEDIY ) then
            if cnf.one_key_lottery_cost_diamond then
                if diamondNum < cnf.one_key_lottery_cost_diamond then
                    self.touch = true
                    GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("shop_can") )
                    return
                end
            else
                local _ , cnf_ = ObjIDTool.GetInfo( cnf.cost_item_id )
                self.touch = true
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("hero_potential_tips2" , cnf_.name ) )
                return
            end
        end
    else
        return
    end

    if self.touch == true then
        local info = {}
        info.lottery_type = 1
        -- 1是道具
        info.is_cost_item = 1
        if num < ( cnf.cost_item_num * Modules.TurntableView.REPEATEDIY ) then
            info.is_cost_item = 0
        end

        local type_ = CallModuleFunc(ModuleType.TURNTABLE , "GetNowNotShow")
        if type_ ~= true and info.is_cost_item == 0 then         
            self:DiamondTips( cnf.one_key_lottery_cost_diamond , Modules.TurntableView.REPEATEDIY , function( ... )
                CallModuleFunc(ModuleType.TURNTABLE,"TurntableLotteryReq" , info )
                self.touch = false
            end )
        else
            CallModuleFunc(ModuleType.TURNTABLE,"TurntableLotteryReq" , info )
            self.touch = false
        end
    end
end

--点击10
function Modules.TurntableView:DiamondTips( num , item , Send )
    local tips = LogicGUI.TemplAffirmTips.New( { 
                                                type = LogicGUI.TemplAffirmTipsType.YES_NO,
                                                content = "" ,
                                                cancel_text = CPPGameLib.GetString("common_cancel") ,
                                                confirm_func = function ( ... )
                                                   Send() 
                                                end
                                            } )
    tips:SetLoadedCallback( function ( ... )
        --选择框

        local turntable_diamond = Util:LabCenter( {
                                            {zi = CPPGameLib.GetString("turntable_diamond1" , num) ,  dx = Macros.TypefaceSize.largish , ys = Macros.Color.content },
                                            {tp = Util:GetMoneyIconImgData( Macros.Game.RolePropType.DIAMOND ) , dx = 0.6 , jg = 5 },
                                            {zi = CPPGameLib.GetString("turntable_diamond2" , item) ,  dx = Macros.TypefaceSize.largish , ys = Macros.Color.content },
                                        } )
        turntable_diamond:SetAnchorPoint( 0.5 , 0 )
        tips.txt_content:AddChild( turntable_diamond )
        PosTool.Center( turntable_diamond , 0 , 10 )

        local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_select_no")
        local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_select_yes")
        local cbx = UICheckBoxEx.CreateWithImage(sp_1,sp_2)

        local common_no_show = Util:Label( CPPGameLib.GetString("common_no_show") , Macros.TypefaceSize.normal , Macros.Color.red )

        local layout = Util:Layout( cbx:GetContentWidth() + common_no_show:GetContentWidth() + 5, 50 )
        layout:SetAnchorPoint( 0.5 , 0.5 )
        tips:AddChild( layout )
        PosTool.Center( layout , 0 , -35 )

        cbx:SetAnchorPoint( 0 , 0.5 )
        cbx:SetPosition( 0 , layout:GetContentHeight()/2)
        layout:AddChild(cbx)
        cbx:SetEventListener(
            GlobalCallbackMgr:AddCallBackFunc(function()
                CallModuleFunc(ModuleType.TURNTABLE , "SetNowNotShow" , true )
            end),
            GlobalCallbackMgr:AddCallBackFunc(function()
                CallModuleFunc(ModuleType.TURNTABLE , "SetNowNotShow" , false )
            end)
        )   
        layout:AddChild(common_no_show)
        PosTool.RightTo( cbx , common_no_show , 5 )
    end )
end

--奖励
function Modules.TurntableView:OnButtonDirectstore( )
    LogicGUI.TempTurntableAward.New()
end

--箭头处理
function Modules.TurntableView:ArrowsDispose( )
    --重置按钮
    self:ResetArrows()
    --重置特效
    self:DeleteEffects()
    --闪灯
    self:FlickerLed()
    --旋转
    self:ArrowsRotate( )
end

--飘字
function Modules.TurntableView:__SetJackpot( )
    local lst = CallModuleFunc(ModuleType.TURNTABLE,"GetTurntableCnf")
    if not lst then
        return 
    end

    local selected = CallModuleFunc(ModuleType.TURNTABLE,"GetSelected")

    --飘字
    local text = lst.normal_lottery_diamond
    if selected then
        if #selected ~= 1 then
            text = lst.one_key_lottery_diamond
        end

        local lab = TypefaceEffects:LittleFloat( "+".. text , nil , 1.5 , 0 , 20 , nil , 1 , nil , Macros.TypefaceSize.normal )
        lab:SetAnchorPoint( 0.5 , 0.5 )
        self.Image_dial:AddChild( lab , 999 )
        PosTool.Center( lab , 0 , -10 )
        
        local num = CallModuleFunc(ModuleType.TURNTABLE,"GeturntableQuantity")
        CallModuleFunc(ModuleType.TURNTABLE,"SeturntableQuantity" , num + text )
        self:SetJackpotMoney()
    end
end

--箭头
function Modules.TurntableView:ArrowsRotate(endid , index )

    local lst = CallModuleFunc(ModuleType.TURNTABLE,"GetSelected")
      
    -- 当前时间    t   主要变量  
    local t = 0  
    -- 奖励格子    n   随机变量    
    local n = lst[#lst].id
    -- 匀速速度    v   定量  1800
    local v = 360 * 4
    -- 加速时长    t1  定量  720/v是转一圈，最后的*2是转2圈完成加速
    local t1 = ( 720 / v ) * 2
    -- 减速时长    t2  定量  720/v是转一圈，最后的*3是转3圈完成减速
    local t2 = ( 720 / v ) * ( 2 + math.min(#lst, 6) )
    -- 随机偏移量
    local random = math.random(-30 , 30) / 800
    -- 匀速至少圈数  x   定量  5
    local x = 3 + random
    -- 匀速时长 t3 = ( 360 / 8 * n + 360 * x ) / v         
    local t3 = ( ( (360 / 8) * (n - 1) ) + (360 * x) ) / v 

    local t4 = {}
    for index , value in ipairs( lst ) do
        local t_1 = t1 + t3 + (t2 - math.pow( t2 * t2 - t2 / v * ( 720 * ( index + 2 ) + 90 * ( value.id - n)) , 0.5))
        t_1 = math.floor(t_1*30+0.5)/30
        t4[t_1] = true
        if index == 5 then
            break
        end
    end

    -- local index = 0
    self.xuanzhong = {}
    -- self.xuanzhong_1 = {}

    self.time = GlobalTimerQuest:AddPeriodQuest(
    GlobalCallbackMgr:AddCallBackFunc(function()
        t = math.floor( t * 30 + 1 + 0.5 ) / 30
    -- 当前角度 w = 1/2 * v / t1 * math.power(math.min(t1 , t) , 2) + v * (math.min(t3 + t1 , math.max(t , t1)) - t1) + v * math.min(math.max(t - t1 - t3 , 0) , t1 + t3 + t2) - 1/2 * v * math.power(math.min(math.max(t - t1 - t3 , 0) , t1 + t3 + t2) , 2) / t2
        local w = 1/2 * v / t1 * math.pow(math.min(t1 , t) , 2) + v * (math.min(t3 + t1 , math.max(t , t1)) - t1) + v * math.min(math.max(t - t1 - t3 , 0) , t1 + t3 + t2) - 1/2 * v * math.pow(math.min(math.max(t - t1 - t3 , 0) , t1 + t3 + t2) , 2) / t2
        
        if t4[t] then
            local pos = math.mod( math.floor( ( w - ( random * 360 )) / 45 + 0.5 ) , 8 ) + 1
            local effect = nil

            -- index = index + 1
            if not self.xuanzhong[pos] then
                self.xuanzhong[pos] , effect = self:__CreateEffect({res_name = "UI_zp_xuanzhong_1" , end_callback = function ( ... )
                    -- if not self.xuanzhong_1[pos] then
                    --     local effect = nil
                    --     self.xuanzhong_1[pos] , effect = self:__CreateEffect({res_name = "UI_zp_xuanzhong_1"}, self.Panel_led[ pos ] , -1 )
                    --     PosTool.Center(effect , -2.5 , 134 )
                    -- end
                end }, self.Panel_led[pos] , -1 )
                PosTool.Center(effect , -2.5 , 134 )
            end
        end

        CPPActionManager.RotateTo( self.arrows , 0 , w )

        if t >= t1 + t3 + t2 then
            GlobalTimerQuest:CancelQuest(self.time)
            self.time = nil

            --结束
            local cnf = CallModuleFunc(ModuleType.TURNTABLE,"GetTurntablePond")
            local t1 = {}
            for i , v in ipairs( lst ) do
                if cnf[v.id] then
                    table.insert(t1,{id = cnf[v.id].id or cnf[v.id].item_id , num = v.num})
                end
            end
            -- 3106 5连弹出恭喜获得后，底部选中特效要全部取消
            GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS , false , t1,function ( ... )
                self:DeleteEffects()
            end,nil,nil,true )
            
            --彩灯结束
            for i = 1 , #self.Image_led do
                -- self.Image_led[i]:SetOpacity( 255 )
                -- CPPActionManager.StopAllActions( self.Image_led[i] )
                if self.effect_id and self.effect_id[i] then
                    self:__DelEffect( self.effect_id[i] )
                    self.effect_id[i] = nil
                end
                self.touch = true
                CallModuleFunc(ModuleType.TURNTABLE,"TurntableDataReq")
            end

        end
    end) )
end

--设置界面显示
function Modules.TurntableView:__SetCustomUI( )
    local cnf = CallModuleFunc(ModuleType.TURNTABLE,"GetTurntableCnf")
    self.Button_Click_10:SetText( CPPGameLib.GetString("turntable_click" , cnf.cost_item_num * Modules.TurntableView.REPEATEDIY ) )

    local num = CallModuleFunc(ModuleType.Pack,"GetNumByItemID", cnf.cost_item_id ) 
    local itemCnf = CPPGameLib.GetConfig("Item", cnf.cost_item_id )

    --1
    local t1 = {}
    if cnf.normal_lottery_cost_diamond and num < cnf.cost_item_num then
        t1 = {
                {tp = Util:GetMoneyIconImgData( Macros.Game.RolePropType.DIAMOND ) , dx = 0.6},
                {zi = "X " .. cnf.normal_lottery_cost_diamond , mb = true , ys = 102 , dx = Macros.TypefaceSize.slightly , jg = 5 },
            }
    else
        t1 = {
                {tp = Resource.PathTool.GetItemPath( itemCnf.res_id ), dx = 0.4},
                {zi = "X " .. cnf.cost_item_num , mb = true , ys = 102 , dx = Macros.TypefaceSize.slightly , jg = 5 },
            }
    end
    if self.click_1 then
        self.click_1:RemoveSelf(true)
        self.click_1 = nil
    end
    self.click_1 = Util:LabCenter( t1 )
    self.click_1:SetAnchorPoint( 0.5 , 0 )
    self.Button_Click_1:AddChild( self.click_1 )
    PosTool.Center( self.click_1 , 0 , 30 )


    --10
    local t1 = {}
    if cnf.one_key_lottery_cost_diamond and num < ( cnf.cost_item_num * Modules.TurntableView.REPEATEDIY ) then
        t1 = {
                {tp = Util:GetMoneyIconImgData( Macros.Game.RolePropType.DIAMOND ) , dx = 0.6},
                {zi = "X " .. cnf.one_key_lottery_cost_diamond , mb = true , ys = 102 , dx = Macros.TypefaceSize.slightly , jg = 5 },
            }
    else
        t1 = {
                {tp = Resource.PathTool.GetItemPath( itemCnf.res_id ), dx = 0.4},
                {zi = "X " .. cnf.cost_item_num * Modules.TurntableView.REPEATEDIY , mb = true , ys = 102 , dx = Macros.TypefaceSize.slightly , jg = 5 },
            }
    end
    if self.click_10 then
        self.click_10:RemoveSelf(true)
        self.click_10 = nil
    end
    self.click_10 = Util:LabCenter(t1)
    self.click_10:SetAnchorPoint( 0.5 , 0 )
    self.Button_Click_10:AddChild( self.click_10 )
    PosTool.Center( self.click_10 , 0 , 30 )

    if self.customSp or self.customEffects or self.customText then
        return
    end

    local id = CallModuleFunc(ModuleType.TURNTABLE,"GetActivityId")
    local cnf = CPPGameLib.GetConfig("TurntablePuzzle", id )

    self.customSp = {}
    --图片
    if cnf.sp_lst then
        for i , v in ipairs( cnf.sp_lst ) do
            if i == 1 then
                -- 标题图
                self.Image_headline:SetImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TURNTABLE, v , true) )
                self.Image_headline:SetVisible(true)
                --坐标
                if cnf.sp_pos_lst then
                    self.Image_headline:SetPosition( self.Image_headline:GetPositionX() + cnf.sp_pos_lst[1][1]  , self.Image_headline:GetPositionY() + cnf.sp_pos_lst[1][2] )
                end
            else
                self.customSp[ i - 1 ] = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TURNTABLE, v , true) )
                self.customSp[ i - 1 ]:SetAnchorPoint( 0.5 , 0.5 )
                self.Image_headline:AddChild( self.customSp[ i - 1 ] )
                local x , y 
                if cnf.sp_pos_lst and cnf.sp_pos_lst[i] then
                    x = tonumber(cnf.sp_pos_lst[i][1])
                    y = tonumber(cnf.sp_pos_lst[i][2])
                end
                PosTool.Center( self.customSp[ i - 1 ] , x , y )
            end
        end
    end

    --特效
    self.customEffects = {}
    if cnf.effects_lst then
        for i , v in ipairs(cnf.effects_lst) do
            local id = tonumber(v)
            local tab = {}
            if id then
                tab = { id = id }
            else
                tab = { res_name = v }
            end
            local zob = 0
            if cnf.effects_zob_lst and cnf.effects_zob_lst[i] then
                zob = cnf.effects_zob_lst[i]
            end
            local effect = nil
            self.customEffects[i] , effect = self:__CreateEffect( tab , self.Image_headline , zob )
            effect:SetAnchorPoint(0.5,0.5)

            local x , y
            if cnf.effects_pos_lst then
                x = tonumber(cnf.effects_pos_lst[i][1])
                y = tonumber(cnf.effects_pos_lst[i][2])
            end
            PosTool.Center( effect , x , y )
        end
    end

    --文字
    self.customText = {}
    if cnf.text_lst then
        --文字
        for i , v in ipairs(cnf.text_lst) do
            self.customText[i] = Util:RichText( v )
            local x , y
            if cnf.text_pos_lst then
                x = tonumber(cnf.text_pos_lst[i][1])
                y = tonumber(cnf.text_pos_lst[i][2])
            end
            self.customText[i]:SetAnchorPoint( 0.5 , 0.5 )
            self.Image_headline:AddChild( self.customText[i] )
            PosTool.Center( self.customText[i] , x , y )
        end
    end

    --消耗
    local cnf = CallModuleFunc(ModuleType.TURNTABLE,"GetTurntableCnf")
    if not cnf then
        return 
    end

    local turntable_integral = Util:Label( CPPGameLib.GetString("turntable_integral" , cnf.normal_lottery_score ) , Macros.TypefaceSize.slightly )
    turntable_integral:SetAnchorPoint( 0.5 , 1 )
    self.Button_Click_1:AddChild( turntable_integral )
    PosTool.Center( turntable_integral , 0 , -32 )

    local turntable_integral = Util:Label( CPPGameLib.GetString("turntable_integral" , cnf.one_key_lottery_score ) , Macros.TypefaceSize.slightly )
    turntable_integral:SetAnchorPoint( 0.5 , 1 )
    self.Button_Click_10:AddChild( turntable_integral )
    PosTool.Center( turntable_integral , 0 , -32 )
end

--删除特效
function Modules.TurntableView:DeleteEffects( )
    print("xy" , "===========删除特效=")
    for i = 1 , 10 do
        if self.xuanzhong and self.xuanzhong[i] then
            self:__DelEffect( self.xuanzhong[i] )
            self.xuanzhong[i] = nil
        end
        if self.xuanzhong_1 and self.xuanzhong_1[i] then
            self:__DelEffect( self.xuanzhong_1[i] )
            self.xuanzhong_1[i] = nil
        end
    end
end

--闪烁彩灯
function Modules.TurntableView:FlickerLed( )
    for i = 1 , #self.Image_led do
        -- CPPActionManager.DelayTo( self.Image_led[i] , 0.1 , GlobalCallbackMgr:AddCallBackFunc(function ()
            -- ActionManager.GetInstance():CycleBlinkEffect( self.Image_led[i] , MathTool.GetRandom() + MathTool.GetRandom() )
            if not self.effect_id then
                self.effect_id = {}
            end
            local effect = nil
            self.effect_id[i] , effect = self:__CreateEffect({res_name = "UI_zp_shandeng"}, self.Image_led[i] , 0 )
            PosTool.Center(effect , 0 , 2)
        -- end))
    end
end


--设置奖池
function Modules.TurntableView:SetJackpotMoney( )
    --奖池金额设置
    if self.jackpotMoney then
        self.jackpotMoney:RemoveSelf(true)
        self.jackpotMoney = nil
    end
    
    local num = CallModuleFunc(ModuleType.TURNTABLE,"GeturntableQuantity")
    self.jackpotMoney = Util:LabCenter( {
                                            {tp = Util:GetMoneyIconImgData( Macros.Game.RolePropType.DIAMOND ) , dx = 0.6 },
                                            {zi = num , mb = true , ys = 102 , dx = Macros.TypefaceSize.slightly , jg = 5 },
                                        }
                                    )
    self.jackpotMoney:SetAnchorPoint( 0.5 , 0.5 )
    self.Image_dial:AddChild( self.jackpotMoney )
    PosTool.Center( self.jackpotMoney , 0 , -10 )
end
