--[[
%% @module: 卡牌翻牌界面
%% @author: yjg
%% @created: 2017年12月11日14:19:25
--]]

LogicGUI = LogicGUI or {}

LogicGUI.TemplDivinationCard = LogicGUI.TemplDivinationCard or BaseClass(GUI.Template)

function LogicGUI.TemplDivinationCard:__init( )
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil) 

    self:__Getwidget()
    self:__ShowUI()   
end

function LogicGUI.TemplDivinationCard:__CreateWidgetFromLuaCode()

    self.layout = Util:Layout( 813.00 , 450.00 )
    self.layout:SetAnchorPoint( 0 , 0 )
    self.layout:SetTouchEnabled(false)
    return self.layout
end

function LogicGUI.TemplDivinationCard:__delete()
    for i = 1 , #self.pageLayer do
        if self.pageLayer[i] then
            self.pageLayer[i]:DeleteMe()
            self.pageLayer[i] = nil
        end
    end
end

function LogicGUI.TemplDivinationCard:__AddToParent()
end

function LogicGUI.TemplDivinationCard:__Getwidget()

end

function LogicGUI.TemplDivinationCard:__ShowUI()
    self.page = UIPageViewEx.Create()
    self.page:SetPageSize( self.layout:GetContentWidth() , self.layout:GetContentHeight() )
    self.page:SetAnchorPoint( 0.5 , 0.5 )
    self.layout:AddChild( self.page )
    PosTool.Center( self.page )
    self.page:SetTouchEnabled(false)

    self.pageLayer = {}
    for i = 1 , 2 do
        self.pageLayer[i] = LogicGUI.TemplDivinationScryer.New( i )
        self.page:AddPage(self.pageLayer[i]:GetNode())
    end  
end

--页面切换
function LogicGUI.TemplDivinationCard:SetCardCut( cardType )
    if cardType == Macros.Game.CARD_TYPE.LOW then
        self.page:ScrollToPage( 0 )
    elseif cardType == Macros.Game.CARD_TYPE.TALL then
        self.page:ScrollToPage( 1 )
    end
end





LogicGUI = LogicGUI or {}

LogicGUI.TemplDivinationScryer = LogicGUI.TemplDivinationScryer or BaseClass(GUI.Template)

function LogicGUI.TemplDivinationScryer:__init( cardType , func )
    print("xy" , "===========TemplDivinationScryer" )
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_DIVINATION_SCRYER) 

    self.cardType = cardType
    self.notAuto = false
    self.notAuto = false

    self:__Getwidget()
    self:__ShowUI()
    self:__Server()
end

function LogicGUI.TemplDivinationScryer:__AddToParent()
end

function LogicGUI.TemplDivinationScryer:__delete()
    if self.onTouch then
        self.onTouch:RemoveSelf(true)
        self.onTouch = nil
    end

    for i = 1 , 10 do
        if self.card[i] then
            self.card[i]:DeleteMe()
            self.card[i] = nil
        end
    end
end

function LogicGUI.TemplDivinationScryer:__Server()
    --翻牌
    self:BindGlobalEvent(Macros.Event.DivinationModule.DIVINATION_DRAW , function ( _ , info )
        if info.type == self.cardType then
            self:DrawCard(info)
        end
    end )

    --洗牌
    self:BindGlobalEvent(Macros.Event.DivinationModule.DIVINATION_SHUFFLE , function ( _ , info )
        if info.refresh_type == self.cardType then
            self:ShuffleCard( info )
        end
    end )
end

function LogicGUI.TemplDivinationScryer:__Getwidget()
    --容器
    self.Panel_PageView = self:GetWidget("Panel_PageView")
    --背景
    self.Image_card = self:GetWidget("Image_card")
    --按钮层
    self.Panel_icon = self:GetWidget("Panel_icon")

    -- 背包
    self.Button_pack = Util:WidgetButton( 
                                              self:GetWidget("Button_pack") , 
                                              "" , 
                                              CPPGameLib.Handler(self,self.OnButtonPack)
                                          )
    self.Button_pack:IgnoreContentAdaptWithSize(true)
    self.Button_pack:SetNormalImage(Resource.PathTool.GetSystemEntryIconPath("btn_bag"), TextureResType.LOCAL )
    local Button_pack = Util:Name( CPPGameLib.GetString("divination_pack") )
    Button_pack:SetPosition( self.Button_pack:GetContentWidth()/2 , 0 )
    Button_pack:SetAnchorPoint( 0.5 , 0.5 )
    self.Button_pack:AddChild( Button_pack )

    --预览
    self.Button_preview = Util:WidgetButton( 
                                              self:GetWidget("Button_preview") , 
                                              "" , 
                                              CPPGameLib.Handler(self,self.OnButtonPreview)
                                          )
    self.Button_preview:IgnoreContentAdaptWithSize(true)
    self.Button_preview:SetNormalImage(Resource.PathTool.GetSystemEntryIconPath("btn_divine"), TextureResType.LOCAL )
    local Button_preview = Util:Name( CPPGameLib.GetString("divination_preview") )
    Button_preview:SetPosition( self.Button_preview:GetContentWidth()/2 , 0 )
    Button_preview:SetAnchorPoint( 0.5 , 0.5 )
    self.Button_preview:AddChild( Button_preview )

    --帮助
    self.Button_help = Util:WidgetButton( 
                                              self:GetWidget("Button_help") , 
                                              "" , 
                                              CPPGameLib.Handler(self,self.OnButtonHelp)
                                          )
    self.Button_help:IgnoreContentAdaptWithSize(true)
    self.Button_help:SetNormalImage(Resource.PathTool.GetSystemEntryIconPath("btn_daily"), TextureResType.LOCAL )
    local Button_help = Util:Name( CPPGameLib.GetString("divination_help") )
    Button_help:SetPosition( self.Button_help:GetContentWidth()/2 , 0 )
    Button_help:SetAnchorPoint( 0.5 , 0.5 )
    self.Button_help:AddChild( Button_help )

    --兑换
    self.Button_astrology = Util:WidgetButton( 
                                              self:GetWidget("Button_astrology") , 
                                              "" , 
                                              CPPGameLib.Handler(self,self.OnButtonAstrology)
                                          )
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DIVINATION, "sp_btn_astrology")
    self.Button_astrology:IgnoreContentAdaptWithSize(true)
    self.Button_astrology:SetNormalImage(sp)
    self.Button_astrology:SetVisible(false)
    local Button_astrology = Util:Name( CPPGameLib.GetString("divination_astrology") )
    Button_astrology:SetPosition( self.Button_astrology:GetContentWidth()/2 , 0 )
    Button_astrology:SetAnchorPoint( 0.5 , 0.5 )
    self.Button_astrology:AddChild( Button_astrology )
end

function LogicGUI.TemplDivinationScryer:__ShowUI()
    --按钮设置
    if self.cardType == Macros.Game.CARD_TYPE.TALL then
        self.Button_astrology:SetVisible(true)
    end

    --创建卡面
    self:NewCard( true )

    --设置卡面
    self:SetCardInfo()

    --创建触摸层
    self:NweTouch()
end

--创建卡牌
function LogicGUI.TemplDivinationScryer:NewCard( visible )
    --所有卡牌
    self.card = {}
    --所有卡牌坐标
    self.posData = {}
    for i = 1 , 10 do
        --背景容器
        self.card[i] = LogicGUI.TempDivinationItem.New( i )
        self.Panel_PageView:AddChild( self.card[i]:GetNode() )
        self.card[i]:ShowCardBack( self.cardType )
        self.card[i]:SetVisible( visible )

        local x = 17 + ( self.card[i]:GetNode():GetContentWidth()/2 ) + ((i- 1)%5)  * (self.card[i]:GetNode():GetContentWidth() + 10)
        local y = self.Panel_PageView:GetContentHeight()/2 + 92
        if i > 5 then
            y = self.Panel_PageView:GetContentHeight()/2 - 92
        end
        self.card[i]:SetPosition( x , y )
        table.insert( self.posData , { x = x , y = y ,w = self.card[i]:GetNode():GetContentWidth() , h = self.card[i]:GetNode():GetContentHeight() } )
    end
end

--设置卡面
function LogicGUI.TemplDivinationScryer:SetCardInfo( )
    --获得列表
    local card = {}
    if self.cardType == Macros.Game.CARD_TYPE.LOW then
        card = CallModuleFunc(ModuleType.DIVINATION, "GetLowBCardGroupInfo" )
        card = card.low_card_group_list
    elseif self.cardType == Macros.Game.CARD_TYPE.TALL then
        card = CallModuleFunc(ModuleType.DIVINATION, "GetTyrantCardGroupInfo" )
        card = card.high_card_group_list
    end

    --遍历所有卡槽
    for i = 1 , #self.card do
        --已经打开的卡面
        if card[i] and card[i].is_already_open == 1 then
            self.card[i]:ShowCardFront( card[i].card_id )
        end
    end
end

--翻牌
function LogicGUI.TemplDivinationScryer:DrawCard( info )
    local index = 0
    local func = nil 
    local lst = info.card_idx_list
    local function DelayMove( ... )
        if index ~= #lst then
            self:NewShade()
            index = index + 1
            --删除特效
            if self.onLst[index] then
                self:__DelEffect(self.onLst[index])
                self.onLst[index] = nil
            end  

            self.card[lst[index].card_idx]:ShowCardDraw( lst[index].card_id )
            local type_ , info_ = ObjIDTool.GetInfo( lst[index].card_id )
            if info_.quality >= Macros.Game.QualityType.PURPLE then
                GUITemplateMgr:CreateTemplateInstance( GUI.TemplateRegistry.TEMPL_DIVINATION_GAIN , false , lst[index].card_id , function( ... )
                    CPPActionManager.DelayTo( self.card[lst[index].card_idx]:GetNode() , 0.2 , func ) 
                end )
            else
                CPPActionManager.DelayTo( self.card[lst[index].card_idx]:GetNode() , 0.2 , func ) 
            end
            self:GainManifestation(lst[index])  

        else

            --刷新显示的价钱
            for i = 1 , #self.card do
                if not self.onIndex[i] then
                    self.card[i]:SetShowCardPrice( )
                end
            end

            CPPActionManager.DelayTo( self.Panel_PageView , 2 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
                --清除选中的信息
                self:ResetOnData( )
                -- self.selectKey = true
            end) ) 
        end
    end
    func = GlobalCallbackMgr:AddCallBackFunc(DelayMove)
    DelayMove()
end

--洗牌
function LogicGUI.TemplDivinationScryer:ShuffleCard( info )
    self:NewShade()
    for i = 1 , 10 do
        CPPActionManager.FadeOut( self.card[i]:GetNode() , 0.2 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
            self.card[i]:DeleteMe()
            self.card[i] = nil

            if i == 10 then
                self:DealDeal()
            end
        end) )
    end
end

--发牌
function LogicGUI.TemplDivinationScryer:DealDeal()
    self.posData = {}

    for i = 1 , 10 do
        if self.card[i] then
            self.card[i]:DeleteMe()
            self.card[i] = nil
        end
        --背景容器
        self.card[i] = LogicGUI.TempDivinationItem.New( i )
        self.Image_card:AddChild( self.card[i]:GetNode() )
        self.card[i]:ShowCardBack( self.cardType  )
        self.card[i]:SetVisible(false)

        local x = 17 + ( self.card[i]:GetNode():GetContentWidth()/2 ) + ((i- 1)%5)  * (self.card[i]:GetNode():GetContentWidth() + 10)
        local y = self.Image_card:GetContentHeight()/2 + 92
        if i > 5 then
            y = self.Image_card:GetContentHeight()/2 - 92
        end
        self.card[i]:SetPosition( x , y )
        table.insert( self.posData , { x = x , y = y ,w = self.card[i]:GetNode():GetContentWidth() , h = self.card[i]:GetNode():GetContentHeight() } )
    end

    local viewInfo = GlobalViewMgr:GetCurViewInfo()
    local view = GlobalViewMgr:GetView( viewInfo.view_id )

    local TIME = 0.2

    local index = 0
    local func = nil 
    local lst = #self.card
    local function DelayMove( ... )
        index = index + 1
        if index <= lst then
            local effectid, effect = self:__CreateEffect({ id = 100078 ,loop = false }, view ,2)
            effect:SetPosition( 280 - 29 , 250 - 23 )

            local target_pos = NodeUnity.ConvertPosBetweenTwoContainer(
                                                                            self.Panel_PageView, 
                                                                            view:GetNode() , 
                                                                            COCOPoint(
                                                                                        self.posData[index].x,
                                                                                        self.posData[index].y
                                                                                        )
                                                                        )

            CPPActionManager.MoveTo( effect , TIME , target_pos.x , target_pos.y + 30 , GlobalCallbackMgr:AddCallBackFunc( function ( ... )
                local effectid, effect = self:__CreateEffect({ id = 100077 ,loop = false }, self.Image_card ,2)
                effect:SetPosition( self.posData[index].x , self.posData[index].y )
                self.card[index]:SetVisible(true)
            end ) )
            CPPActionManager.DelayTo( self.card[index]:GetNode() , TIME , func ) 
        else
            CPPActionManager.DelayTo( self.Image_card , TIME + 1.1 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
                self:SetCardInfo()
                self:ResetOnData()
            end) ) 
        end
    end
    func = GlobalCallbackMgr:AddCallBackFunc(DelayMove)
    DelayMove()
end

--删除所有选中信息
function LogicGUI.TemplDivinationScryer:ResetOnData( info )
    --防止重复选择
    self.onIndex = {}
    --选择列表
    self.onIndexLst = {}
    --特效列表
    for i = 1 , 10 do
        if self.onLst[i] then
            self:__DelEffect(self.onLst[i])
            self.onLst[i] = nil
        end  
    end
    --删除遮罩
    if self.onTouch then
        self.onTouch:RemoveSelf(true)
        self.onTouch = nil
    end
end

--创建遮罩层
function LogicGUI.TemplDivinationScryer:NewShade( )
    if not self.onTouch then
        local viewInfo = GlobalViewMgr:GetCurViewInfo()
        local view = GlobalViewMgr:GetView( viewInfo.view_id )
        self.onTouch = Util:Layout( Macros.Global.DesignSize.width, Macros.Global.DesignSize.height , function ( ... )
        end )
        self.onTouch:SetAnchorPoint( 0.5 , 0.5 )
        view:AddChild( self.onTouch , Macros.Global.z_order_top + 1)
        PosTool.Center( self.onTouch )
    end
end

--背包
function LogicGUI.TemplDivinationScryer:OnButtonPack()
    CallModuleFunc(ModuleType.DIVINATION , "OpenView" , BaseViewType.DIVINATION_PACK )
end

--预览
function LogicGUI.TemplDivinationScryer:OnButtonPreview()
    LogicGUI.TemplDivinationRewardPrelook.New( self.cardType )
end

--规则
function LogicGUI.TemplDivinationScryer:OnButtonHelp()
    local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
    help:SetLoadedCallback( function ( ... )
       help:SetCell("Rule_Divination_Draw")
    end )
end

--兑换
function LogicGUI.TemplDivinationScryer:OnButtonAstrology()
    if self.divinationConvert then
        return
    end
    
    self.divinationConvert = LogicGUI.TemplDivinationConvert.New( function( ... )
        self.divinationConvert = nil
    end )
end

--创建触摸层
function LogicGUI.TemplDivinationScryer:NweTouch()
    self.selectTouch = Util:Layout( self.Image_card:GetContentWidth() , self.Image_card:GetContentHeight() )
    self.selectTouch:SetAnchorPoint( 0.5 , 0.5 )
    self.Image_card:AddChild( self.selectTouch )
    PosTool.Center( self.selectTouch )

    --防止重复选择
    self.onIndex = {}
    --选择列表
    self.onIndexLst = {}
    --特效列表
    self.onLst = {}
    self.selectTouch:SetTouchEnabled(true)
    WidgetUnity.SetWidgetListener( self.selectTouch , 
                                          function ( x , y )
                                              -- self:SetSelect( x , y , self.selectTouch )
                                          end,
                                          function ( x , y )
                                              self:SetSelect( x , y , self.selectTouch )
                                          end,
                                          function ( x , y )
                                              self:SetSelect( x , y , self.selectTouch )
                                              self:TouchEnd( )
                                          end,
                                          function ( x , y )
                                              self:SetSelect( x , y , self.selectTouch )
                                              self:TouchEnd( )
                                          end
                                  )
end

-- 选中
function LogicGUI.TemplDivinationScryer:SetSelect( x , y , layer )
    --抽取状态
    local extractType , currencyType , consumeType = CallModuleFunc(ModuleType.DIVINATION, "DrawCalculate" , self.cardType , #self.onIndexLst + 1 )
    if extractType == false then
        if currencyType == false then
            if self.cardType == Macros.Game.CARD_TYPE.LOW then
                local txt = "currency_insufficient_"..consumeType

                if consumeType == Macros.Game.RolePropType.GOLD and not self.gold then
                    self.gold = CallModuleFunc(ModuleType.SUNDRY, "ToIdGetPath", Macros.Game.RolePropType.GOLD , function ( ... )
                        self.gold = nil
                    end)
                end
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString(txt) )

            elseif self.cardType == Macros.Game.CARD_TYPE.TALL then
                local cnf = CPPGameLib.GetConfig("Item", consumeType )
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("hero_potential_tips2" , cnf.name ) )
            
                self:OnButtonAstrology()
            end
        end
        return
    end

    --获得列表
    local card = {}
    if self.cardType == Macros.Game.CARD_TYPE.LOW then
        card = CallModuleFunc(ModuleType.DIVINATION, "GetLowBCardGroupInfo" )
        card = card.low_card_group_list

    elseif self.cardType == Macros.Game.CARD_TYPE.TALL then
        card = CallModuleFunc(ModuleType.DIVINATION, "GetTyrantCardGroupInfo" )
        card = card.high_card_group_list
    end

    local t1 = {}
    --获得列表
    for i , v in ipairs( card ) do
        if v.card_id ~= 0 then
            t1[i] = v.card_id
        end
    end


    local pos = NodeUnity.ConvertToNodePositon( layer , COCOPoint( x , y ) )
    for i , v in ipairs( self.posData ) do
        if pos.x >= (v.x - (v.w/2)) and pos.x <= (v.x + (v.w/2)) and pos.y >= (v.y - (v.h/2)) and pos.y <= (v.y + (v.h/2)) then
            if not self.onIndex[i] and not t1[i] then
                self.onIndex[i] = i
                table.insert(self.onIndexLst , i )
                self.onLst[i], effect = self:__CreateEffect( {id = 100074 }, self.card[i] )
                PosTool.Center(effect)
            end
        end
    end
    GlobalEventSystem:FireNextFrame(Macros.Event.DivinationModule.DIVINATION_SELECT , #self.onIndexLst )

    --刷新显示的价钱
    for i = 1 , #self.card do
        if not self.onIndex[i] then
            self.card[i]:SetShowCardPrice( #self.onIndexLst )
        end
    end
end

--发送
function LogicGUI.TemplDivinationScryer:TouchEnd( x , y , layer )
    local t1 = {}
    for i , v in ipairs(self.onIndexLst) do
        table.insert( t1 , { card_idx = v } )
    end

    if #t1 <= 0 then
        return 
    end 
    local info = {}
    info.type = self.cardType
    info.card_idx_list = t1
    CallModuleFunc(ModuleType.DIVINATION, "OpenCardReq" , info )
end

--获得图标显示
function LogicGUI.TemplDivinationScryer:GainManifestation(info)
    --创建图标
    local icon = LogicGUI.IconLayer.New(true)
    icon:SetData({id = info.card_id})
    icon:SetAnchorPoint(0.5 , 0.5 )

    --图标容器
    self["__cur_sp_layout"..info.card_idx] = Util:Layout(icon:GetNode():GetContentWidth(), icon:GetNode():GetContentHeight())
    self["__cur_sp_layout"..info.card_idx]:SetAnchorPoint(0.5,0.5)
    self["__cur_sp_layout"..info.card_idx]:SetScaleFactor( 0.5 )    --

    --安放图标
    self["__cur_sp_layout"..info.card_idx]:AddChild( icon:GetNode() ) --+ self.card[info.card_idx]:GetNode():GetContentWidth()/2
    PosTool.Center(icon:GetNode())

    --获得每个图标应在位置   
    local cur_posx = self.card[info.card_idx]:GetNode():GetPositionX() 
    local cur_posy = self.card[info.card_idx]:GetNode():GetPositionY() + 10 + self.card[info.card_idx]:GetNode():GetContentHeight()/2
   
   --添加
    self.Panel_PageView:AddChild( self["__cur_sp_layout"..info.card_idx] , 100 )
    self["__cur_sp_layout"..info.card_idx]:SetPosition(cur_posx , cur_posy )
    self["__cur_sp_layout"..info.card_idx]:SetOpacity(0)

    --上下浮动
    self["__fundid"..info.card_idx] = GlobalCallbackMgr:AddCallBackFunc(function ()
        ActionManager.GetInstance():RunJumpByAction(self["__cur_sp_layout"..info.card_idx],0.5,0,-10,0,1,GlobalCallbackMgr:AddCallBackFunc(function ()
            self:JumpByFuction(info.card_idx)
        end))
    end)

    --延迟表现
    CPPActionManager.DelayTo(self["__cur_sp_layout"..info.card_idx],1.5, GlobalCallbackMgr:AddCallBackFunc(function ()
        --浮现
        CPPActionManager.FadeIn(self["__cur_sp_layout"..info.card_idx],0.5,GlobalCallbackMgr:AddCallBackFunc(function ()
            --上下浮动
            self:JumpByFuction(info.card_idx) 
            --延时飞行
            self:RunBeforeAwakenEffect(info.card_idx)        
        end)) 
        --命运硬币
        if info.destiny_coin and info.destiny_coin > 0 then 
            local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DIVINATION, "bg_destiny_coin")
            local coin_txt_bg = Util:Sprite(sp) 
            self.card[info.card_idx]:GetNode():AddChild(coin_txt_bg,10)
            PosTool.Center(coin_txt_bg,0,40)   
            local coin_txt = Util:Label(CPPGameLib.GetString("divination_destiny_coin_num",info.destiny_coin), Macros.TypefaceSize.slightly, Macros.Color.Special)
            coin_txt:SetAnchorPoint( 0.5 , 0.5 ) 
            coin_txt_bg:AddChild(coin_txt,10)
            PosTool.Center(coin_txt)
            --淡出
            CPPActionManager.FadeOut(coin_txt_bg,3,GlobalCallbackMgr:AddCallBackFunc(function ()
                coin_txt_bg:RemoveSelf(true)
            end))
        end    
    end))    
end

--获得图标显示
function LogicGUI.TemplDivinationScryer:JumpByFuction(card_idx)
    ActionManager.GetInstance():RunJumpByAction(self["__cur_sp_layout"..card_idx],0.5,0,10,0,1,self["__fundid"..card_idx])
end

--飞过去
function LogicGUI.TemplDivinationScryer:RunBeforeAwakenEffect(card_idx)
    CPPActionManager.DelayTo(self["__cur_sp_layout"..card_idx],1,GlobalCallbackMgr:AddCallBackFunc(function ()
        --停止任何动作
        CPPActionManager.StopAllActions(self["__cur_sp_layout"..card_idx])
        --飞过去
        local cur_time = math.sqrt(math.pow(self.Button_pack:GetPositionX() - self["__cur_sp_layout"..card_idx]:GetPositionX(),2)+math.pow(self.Button_pack:GetPositionY() - self["__cur_sp_layout"..card_idx]:GetPositionY(),2))/1500
        local target_pos = NodeUnity.ConvertPosBetweenTwoContainer(
                                                                        self.Panel_icon, 
                                                                        self.Panel_PageView, 
                                                                        COCOPoint(
                                                                                    self.Button_pack:GetPositionX(),
                                                                                    self.Button_pack:GetPositionY()
                                                                                    )
                                                                    )
        CPPActionManager.MoveTo(self["__cur_sp_layout"..card_idx] , cur_time , target_pos.x , target_pos.y ,GlobalCallbackMgr:AddCallBackFunc(function ()
            self["__cur_sp_layout"..card_idx]:RemoveSelf(true)
            self["__cur_sp_layout"..card_idx] = nil
            --飞入特效
            local effect_id,effect = self:__CreateEffect({res_name = "UI_zhanbushanguang",loop = false,scale = 0.6},self.Panel_icon,15)
            effect:SetPosition(self.Button_pack:GetPositionX(),self.Button_pack:GetPositionY())
        end))          
    end))
end

