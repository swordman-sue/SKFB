
--
-- @brief 充值排行
-- @author: yjg
-- @date: 2018年12月18日11:55:17
--

Modules = Modules or {}

Modules.ChargeRankView = Modules.ChargeRankView or BaseClass(BaseView)

function Modules.ChargeRankView:__init( )
    self.__layout_name = "" 
    self.__layout_mode = BaseView.LayoutMode.CODE 
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__type = BaseViewType.RECHARGE_RANK
    self.__tab_params_ex = { 30 }

    self.__open_callback = function()
        self:__OpenCallback()
    end

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.CHARGE_RANK)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.CHARGE_RANK , true )
end

function Modules.ChargeRankView:__delete()
end

function Modules.ChargeRankView:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height)
    return self.layout
end

function Modules.ChargeRankView:__OpenCallback()
    self:__Getwidget()
    self:__ShowUI( )
end

function Modules.ChargeRankView:__RegistAllEvents()
    --排行榜数据
    self:BindGlobalEvent(Macros.Event.RankModule.RANK_RECHARGE_REQ,function ( _ , type_ )
        if type_ == "chongzhi" then
            self:__SetBlue()
        else
            self:__SetRed()
        end
    end)
end

function Modules.ChargeRankView:__Getwidget( ... )
    --背景
    self.bg = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHARGE_RANK, "blue_bg"))
    self.bg:SetAnchorPoint(0.5, 0.5)
    self.layout:AddChild( self.bg )
    PosTool.Center( self.bg , 0 , 5 )

    --底板
    self.bottom = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHARGE_RANK, "blue_bg_bottom"))
    self.bottom:SetAnchorPoint(0.5, 0.5)
    self.bg:AddChild( self.bottom , -1)
    PosTool.Center( self.bottom , 43.5 , -10)

    --关闭按钮
    local btn_register = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_register")
    local close = Util:Button( btn_register , function( ... )
        self:OnButtonClose()
    end )
    close:SetAnchorPoint( 0.5 , 0.5 )
    self.bg:AddChild( close )
    close:SetPosition( self.bg:GetContentWidth() - 20 , self.bg:GetContentHeight() - 100 )

    --标题
    local lab = Util:Button( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHARGE_RANK, "lab" , true) )
    lab:SetAnchorPoint( 0.5 , 0.5 )
    lab:SetPosition( 225 , self.bg:GetContentHeight() - 82 )
    self.bg:AddChild( lab )

    --我的排名
    local myRankBg = Util:Sprite9( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "sp_myrank_bg" ) )
    myRankBg:SetContentSize( 220 , 50 )
    myRankBg:SetAnchorPoint( 0.5 , 0.5 )
    myRankBg:SetPosition( 230 , 50 )
    self.bg:AddChild( myRankBg )

    --我的排名
    self.rank_myRank = Util:Label( CPPGameLib.GetString("rank_myRank") , nil , Macros.Color.content )
    self.rank_myRank:SetAnchorPoint( 0 , 0.5 )
    myRankBg:AddChild( self.rank_myRank )
    self.rank_myRank:SetPosition( 15 , myRankBg:GetContentHeight()/2 )
end

function Modules.ChargeRankView:__ShowUI(  )
    --玩家列表
    local params =  
    {
        item_class = Modules.ChargeRankViewItem,
        item_width = 420,
        item_height = 80,
        row = 6,
        col = 1,
        item_space_r = 5,
        view_height = 410 ,
    }
    -- 创建参数
    self.player_list = GUI.ScrollView.New(params)
    self.bg:AddChild(self.player_list:GetNode())
    PosTool.Center(self.player_list:GetNode() , -185 , -32 )

    --奖励列表
    self.award_list = UIListViewEx.Create()
    self.award_list:SetAnchorPoint( 0.5 , 0.5 )
    self.award_list:SetContentSize( 358 , 410 )
    -- self.award_list:SetBounceEnabled( true )
    self.bg:AddChild(self.award_list)
    PosTool.Center( self.award_list , 236 , -31 )

    --页签
    self.__tab:SetContentSize( 150 , 300 )
    self.__tab:SetData( { CPPGameLib.GetString("funpax_recharge") , CPPGameLib.GetString("funpax_consume") } )
    self.__tab:SetItemAlign( 0 )

    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHARGE_RANK, "on_tab")
    local no = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHARGE_RANK, "no_tab")
    self.__tab:SetOnCeil( on , no )  

    self.__tab:SetPosition( 0  , self.bg:GetContentHeight()/2 )
    self.bg:AddChild( self.__tab:GetNode() , -1)
end

function Modules.ChargeRankView:__SetShowUI( index )
    self.award_list:RemoveAllItems()

    if self.describe then
        self.describe:RemoveSelf(true)
        self.describe = nil
    end

    if self.nowDescribe then
        self.nowDescribe:RemoveSelf(true)
        self.nowDescribe = nil
    end

    if index == 1 then
        self:__SetBlue()
    else
        self:__SetRed()
    end
end

function Modules.ChargeRankView:Ceil( index , data )
    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHARGE_RANK, "on_tab")
    local sizeSp = Util:Sprite( on )
    local layer = Util:Layout( sizeSp:GetContentWidth() , sizeSp:GetContentHeight() )

    if not self.lab then
        self.lab = {}
    end
    self.lab[index] = Util:Label( data , Macros.TypefaceSize.popup , Macros.Color.keypoint )
    self.lab[index]:SetAnchorPoint( 0.5 , 0.5 )
    self.lab[index]:SetPosition( layer:GetContentWidth()/2 , layer:GetContentHeight()/2 + 5 )
    layer:AddChild( self.lab[index] )

    return layer
end

function Modules.ChargeRankView:OnCallBack( index , data )
    for i , v in ipairs( self.lab ) do
        if index == i then
            v:SetColor( unpack( Macros.Color.golden ) )
        else
            v:SetColor( unpack( Macros.Color.keypoint ) )
        end
    end  

    self:__SetShowUI( index )
end


--蓝色——充值榜
function Modules.ChargeRankView:__SetBlue( )
    self.bg:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHARGE_RANK, "blue_bg") )
    self.bottom:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHARGE_RANK, "blue_bg_bottom") )
    if not self.rechargeRank then
        self.rechargeRank = true
        CallModuleFunc(ModuleType.RECHARGE_RANK, "RechargeRankReq")
    else
        self:__SetShowBlue( )
    end
end

--蓝色——充值榜
function Modules.ChargeRankView:__SetShowBlue( )
    local myRank = CallModuleFunc(ModuleType.RECHARGE_RANK, "Recharge_GetMyRank")
    if myRank ~= 0 then
        self.rank_myRank:SetString( CPPGameLib.GetString("rank_myRank") .. CPPGameLib.GetString("rebel_mingci_1" , myRank ) )
    else
        self.rank_myRank:SetString( CPPGameLib.GetString("rank_myRank") .. CPPGameLib.GetString("rebel_meimingci" ) )
    end


    local rankLst = CallModuleFunc(ModuleType.RECHARGE_RANK, "Recharge_GetRankLst")
    self.player_list:SetDataList( rankLst )

    --累计充值
    local rankCnf = CallModuleFunc(ModuleType.RECHARGE_RANK, "Recharge_GetRechargeRankCnf")
    local parent = self.rank_myRank:GetParent()
    self.describe = Util:LabCenter( {   
                                        { zi = CPPGameLib.GetString("funpax_describe_1") , ys = Macros.Color.content , dx = Macros.TypefaceSize.largish },
                                        { tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.DIAMOND) , dx = 0.75 , jg = 5 },
                                        { zi = rankCnf.rank_need_diamond , ys = Macros.Color.content , dx = Macros.TypefaceSize.largish , jg = 5 },
                                        { zi = CPPGameLib.GetString("funpax_finalist") , ys = Macros.Color.content , dx = Macros.TypefaceSize.largish , jg = 5 },
                                    } )
    self.describe:SetAnchorPoint( 0 , 0.5 )
    self.describe:SetPosition( parent:GetContentWidth() , self.rank_myRank:GetPositionY() )
    parent:AddChild(self.describe)

    --当前充值
    local rankValue = CallModuleFunc(ModuleType.RECHARGE_RANK, "Recharge_GetMyRankValue")
    self.nowDescribe = Util:LabCenter( {   
                                        { zi = CPPGameLib.GetString("recharge_nowPay") , ys = Macros.Color.content , dx = Macros.TypefaceSize.slightly },
                                        { zi = rankValue , ys = Macros.Color.content , dx = Macros.TypefaceSize.slightly , jg = 5 },
                                        { tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.DIAMOND) , dx = 0.5 , jg = 5 },
                                    } )
    self.nowDescribe:SetAnchorPoint( 1 , 0.5 )
    self.nowDescribe:SetPosition( self.bg:GetContentWidth() - 65 , parent:GetPositionY() )
    self.bg:AddChild(self.nowDescribe)

    local timeNode = self:__NewTimeNode()
    self.award_list:PushBackCustomItem(timeNode)

    local timeNode = self:__NewDescribeNode( 1 )
    self.award_list:PushBackCustomItem(timeNode)
    
    local lst = CallModuleFunc(ModuleType.RECHARGE_RANK, "Recharge_GetRechargeAwardCnf")
    for i , v in ipairs( lst ) do
        local awardNode = self:__NewAwardNode( v )
        self.award_list:PushBackCustomItem( awardNode )
    end
end

--红色——消费榜
function Modules.ChargeRankView:__SetRed( )
    self.bg:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHARGE_RANK, "red_bg") )
    self.bottom:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHARGE_RANK, "red_bg_bottom") )
    if not self.consumeRank then
        self.consumeRank = true
        CallModuleFunc(ModuleType.RECHARGE_RANK, "ConsumeRankReq")
    else
        self:__SetShowRed( )
    end
end

--红色——消费榜
function Modules.ChargeRankView:__SetShowRed( )
    local myRank = CallModuleFunc(ModuleType.RECHARGE_RANK, "Consume_GetMyRank")
    if myRank ~= 0 then
        self.rank_myRank:SetString( CPPGameLib.GetString("rank_myRank") .. CPPGameLib.GetString("rebel_mingci_1" , myRank ) )
    else
        self.rank_myRank:SetString( CPPGameLib.GetString("rank_myRank") .. CPPGameLib.GetString("rebel_meimingci" ) )
    end

    local rankLst = CallModuleFunc(ModuleType.RECHARGE_RANK, "Consume_GetRankLst")
    self.player_list:SetDataList( rankLst )

    --累计充值
    local rankCnf = CallModuleFunc(ModuleType.RECHARGE_RANK, "Consume_GetRechargeRankCnf")
    local parent = self.rank_myRank:GetParent()
    self.describe = Util:LabCenter( {   
                                        { zi = CPPGameLib.GetString("funpax_describe_2") , ys = Macros.Color.content , dx = Macros.TypefaceSize.largish },
                                        { tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.DIAMOND) , dx = 0.75 , jg = 5 },
                                        { zi = rankCnf.rank_need_diamond , ys = Macros.Color.content , dx = Macros.TypefaceSize.largish , jg = 5 },
                                        { zi = CPPGameLib.GetString("funpax_finalist") , ys = Macros.Color.content , dx = Macros.TypefaceSize.largish , jg = 5 },
                                    } )
    self.describe:SetAnchorPoint( 0 , 0.5 )
    self.describe:SetPosition( parent:GetContentWidth() , self.rank_myRank:GetPositionY() )
    parent:AddChild(self.describe)

    --当前充值
    local rankValue = CallModuleFunc(ModuleType.RECHARGE_RANK, "Consume_GetMyRankValue")
    self.nowDescribe = Util:LabCenter( {   
                                        { zi = CPPGameLib.GetString("recharge_nowExpense") , ys = Macros.Color.content , dx = Macros.TypefaceSize.slightly },
                                        { zi = rankValue , ys = Macros.Color.content , dx = Macros.TypefaceSize.slightly , jg = 5 },
                                        { tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.DIAMOND) , dx = 0.5 , jg = 5 },
                                    } )
    self.nowDescribe:SetAnchorPoint( 1 , 0.5 )
    self.nowDescribe:SetPosition( self.bg:GetContentWidth() - 65 , parent:GetPositionY() )
    self.bg:AddChild(self.nowDescribe)
    
    local timeNode = self:__NewTimeNode()
    self.award_list:PushBackCustomItem(timeNode)

    local timeNode = self:__NewDescribeNode( 2 )
    self.award_list:PushBackCustomItem(timeNode)
    
    local lst = CallModuleFunc(ModuleType.RECHARGE_RANK, "Consume_GetRechargeAwardCnf")
    for i , v in ipairs( lst ) do
        local awardNode = self:__NewAwardNode( v )
        self.award_list:PushBackCustomItem( awardNode )
    end
end

--关闭
function Modules.ChargeRankView:OnButtonClose()
    self:CloseManual()
end

--时间
function Modules.ChargeRankView:__NewTimeNode()
    local node = Util:Layout(self.award_list:GetContentWidth() , 30)
    node:SetAnchorPoint(0.5, 0.5)

    local time = Util:Label( " " , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
    time:SetAnchorPoint( 0 , 0.5 )
    time:SetPosition( 10 , node:GetContentHeight()/2 )
    node:AddChild( time )

    local delay = nil
    local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.RECHARGE_RANK )
    
    local star = TimeTool.GetTimeFormat( info.star  , TimeTool.defaul_format[2] )
    local over = TimeTool.GetTimeFormat( info.over  , TimeTool.defaul_format[2] )

    local Countdown = function () 
        local n_Time = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
        if info.reward - n_Time > 0 then
            CPPActionManager.DelayTo( time , 1 , Countdown)
            time:SetString( CPPGameLib.GetString("queen_time") .. TimeTool.TransTimeStamp( CPPGameLib.GetString("time_fate") , info.reward - n_Time ) )
        else
            time:SetString(CPPGameLib.GetString("direct_buy_timeEnd"))
        end
    end
    delay = GlobalCallbackMgr:AddCallBackFunc(Countdown)
    Countdown()

    return node
end

--描述
function Modules.ChargeRankView:__NewDescribeNode( type_ )
    local node = Util:Layout( self.award_list:GetContentWidth() , 120 )
    node:SetAnchorPoint(0.5, 0.5)

    local rankCnf = CallModuleFunc(ModuleType.RECHARGE_RANK, "Recharge_GetRechargeRankCnf")
    if type_ == 2 then
        rankCnf = CallModuleFunc(ModuleType.RECHARGE_RANK, "Consume_GetRechargeRankCnf")
    end
    
    local bg_sp9_label_frame = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_get_approach") 
    local bg = Util:Sprite9( bg_sp9_label_frame )
    bg:SetAnchorPoint( 0.5 , 0.5 )
    bg:SetContentSize( node:GetContentWidth() - 10 , node:GetContentHeight() - 10 )
    node:AddChild( bg )
    PosTool.Center( bg )

    local text_lst = Util:RichText( rankCnf.text_lst , Macros.TypefaceSize.slightly , bg:GetContentWidth() - 20 , nil , Macros.Color.keypoint_hex )
    text_lst:SetAnchorPoint( 0 , 1 )
    text_lst:SetPosition( 10 , bg:GetContentHeight() - 10 )
    bg:AddChild( text_lst )

    return node
end

--奖励
function Modules.ChargeRankView:__NewAwardNode( data )
    local node = Util:Layout(self.award_list:GetContentWidth() , 80 )
    node:SetAnchorPoint(0.5, 0.5)

    local bg_sp9_label_frame = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_get_approach") 
    local item = Util:Sprite9( bg_sp9_label_frame )
    item:SetAnchorPoint( 0.5 , 0.5 )
    item:SetContentSize( node:GetContentWidth() - 10 , node:GetContentHeight() - 10 )
    node:AddChild( item )
    PosTool.Center( item )

    local sort = nil 
    if type(data.sort) == "number" then
        local sort_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp_rank_num_" .. data.sort )
        sort = Util:Sprite( sort_sp )
    else
        text = data.sort
        sort = Util:Label( text , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
    end
    sort:SetAnchorPoint( 0.5 , 0.5 )
    sort:SetPosition( 45 , item:GetContentHeight()/2 )
    item:AddChild( sort )

    local award = Util:Layout( item:GetContentWidth() - 100 , item:GetContentHeight() )
    award:SetAnchorPoint(0.5, 0.5)
    node:AddChild( award )
    PosTool.Center( award , 35 )

    local interval = award:GetContentWidth() / #data.item_lst
    for i , v in ipairs( data.item_lst ) do
        local icon = LogicGUI.IconLayer.New(true)
        icon:SetData( { id = v.item_id , num = v.item_num } )
        icon:SetAnchorPoint( 0.5 , 0.5 )
        award:AddChild( icon:GetNode() )
        icon:SetPosition( ( (icon:GetNode():GetContentWidth() * 0.65) /2) + ( (i-1) * interval ) , award:GetContentHeight()/2 )
        icon:SetScaleFactor( 0.65 )
    end

    return node
end



Modules = Modules or {}
Modules.ChargeRankViewItem = Modules.ChargeRankViewItem or BaseClass(GUI.ScrollItem)

function Modules.ChargeRankViewItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.ChargeRankViewItem:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(420,80)
    return self.layout
end

function Modules.ChargeRankViewItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self:__Getwidget( )
    self:__ShowUI( data )
end

function Modules.ChargeRankViewItem:__Getwidget()
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_bottom_frame")
    self.item = Util:Sprite9(sp)
    self.item:SetContentSize( self.layout:GetContentWidth() , self.layout:GetContentHeight() )
    self.item:SetAnchorPoint(0.5,0.5)
    self.layout:AddChild(self.item)  
    PosTool.Center( self.item )
end

function Modules.ChargeRankViewItem:__ShowUI( data )
    local sort = nil 
    if data.sort <= 3 then
        local sort_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp_rank_num_" .. data.sort )
        sort = Util:Sprite( sort_sp )
    else
        sort = Util:Label( data.sort , Macros.TypefaceSize.popup , Macros.Color.content )
    end
    sort:SetAnchorPoint( 0.5 , 0.5 )
    sort:SetPosition( 45 , self.item:GetContentHeight()/2 )
    self.item:AddChild( sort )

    local bg_sp9_resource = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp_equip_attribute")
    local item = Util:Sprite9( bg_sp9_resource )
    item:SetAnchorPoint( 1 , 0.5 )
    item:SetContentSize( 320 , self.item:GetContentHeight() - 15 )
    item:SetPosition( self.item:GetContentWidth() - 10 , self.item:GetContentHeight()/2 )
    self.item:AddChild( item )

    --图标
    local icon_layer = LogicGUI.RoleIcon.New(true)
    icon_layer:SetData( data.head_icon )
    icon_layer:SetPosition( 10 , item:GetContentHeight()/2)
    icon_layer:SetAnchorPoint( 0 ,0.5)
    icon_layer:SetScaleFactor( 0.75 )
    item:AddChild(icon_layer:GetNode())  

    if data.vip_level ~= 0 then
        --VIP图标
        local vip_icon_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_vip_bg", true)
        local vip_icon = Util:Sprite(vip_icon_sp)       
        icon_layer:AddChild(vip_icon)
        vip_icon:SetScaleFactor(0.7)
        vip_icon:SetPosition( 0 , 10 )

        --VIP数字
        local vip = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "poodlenum", true))
        vip:SetText(tostring(data.vip_level))
        vip:SetAnchorPoint(0.5, 0.5)
        vip_icon:AddChild(vip:GetNode())    
        PosTool.Center(vip, 0, 0)
    end

    --服务器
    local server = Util:Label( "S." .. data.server_id  , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
    server:SetAnchorPoint( 0 , 0.5 )
    server:SetPosition( icon_layer:GetPositionX() + icon_layer:GetNode():GetContentWidth() , item:GetContentHeight()/2 + 15 )
    item:AddChild( server )

    --名字
    local name = Util:Label( data.role_name , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
    name:SetAnchorPoint( 0 , 0.5 )
    name:SetPosition( server:GetPositionX() + 55 , server:GetPositionY() )
    item:AddChild( name )

    --累计充值
    local item_recharge_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHARGE_RANK, "item_recharge_bg")
    local recharge = Util:Sprite( item_recharge_bg )
    recharge:SetAnchorPoint( 0 , 0.5 )
    recharge:SetPosition( server:GetPositionX() , item:GetContentHeight()/2 - 15 )
    item:AddChild( recharge )
    recharge:SetScaleFactor(1.2)

    local queen_title = Util:Label( CPPGameLib.GetString("funpax_describe_" .. data.type_ ) , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
    queen_title:SetAnchorPoint( 0 , 0.5 )
    queen_title:SetPosition( server:GetPositionX() + 5  , recharge:GetPositionY() )
    item:AddChild( queen_title )

    --充值数量
    local recharge = Util:LabCenter( { 
                                    { tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.DIAMOND) , dx = 0.75},
                                    { zi = data.context , ys = Macros.Color.keypoint , dx = Macros.TypefaceSize.slightly , jg = 5 },
                                } )
    recharge:SetAnchorPoint( 0 , 0.5 )
    recharge:SetPosition( queen_title:GetPositionX() + queen_title:GetContentWidth() + 10 , queen_title:GetPositionY() )
    item:AddChild(recharge)
end