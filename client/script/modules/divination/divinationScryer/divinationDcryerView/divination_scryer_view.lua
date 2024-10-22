--[[
%% @module: 占卜主界面
%% @author: yjg
%% @created: 2017年11月14日11:32:03
--]]

Modules = Modules or {} 
Modules.DivinationScryerView = Modules.DivinationScryerView or BaseClass(BaseView)

-- Modules.DivinationScryerView.TIME = 0.1
function Modules.DivinationScryerView:__init()
    self.__layout_name = "divination_scryer"    
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_divination_bg")}
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("divination_divination"), 
        show_diamond = true,
        show_gold = true,
        show_destiny_coin = true,
        show_star_soul = true,
        close_func = function()
            --开启入场动画
            CallModuleFunc(ModuleType.DIVINATION, "SetShowMoveOpen" )
            -- if self.onTouchType == true then
                self:CloseManual()
            -- end
        end,
    }
    self.__open_callback = function()
      self:__OpenCallback()
    end
    
    -- --翻牌
    -- self:BindGlobalEvent(Macros.Event.DivinationModule.DIVINATION_DRAW , function ( _ , info )
    --     self.onTouchType = false
    --     CPPActionManager.DelayTo( self.__root_widget , (#info.card_idx_list * 0.2) + 1 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
    --         self.onTouchType = true
    --     end) ) 
    -- end )

    self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_DIVINATION_NPC) 
    self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_DIVINATION_SCRYER)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.DIVINATION)
end

function Modules.DivinationScryerView:__OpenCallback( )
    self.cutIndex = Macros.Game.CARD_TYPE.LOW
    -- self.page = {}
    self:__Getwidget()
    self:__ShowUI()
    self:__Server()
    -- self.onTouchType = true
end 

function Modules.DivinationScryerView:__Dispose()
    if self.challenge then
        self.challenge:DeleteMe()
        self.challenge = nil
    end
    if self.card then
        self.card:DeleteMe()
        self.card = nil
    end
end

function Modules.DivinationScryerView:__Getwidget()
    print("xy" , "======__Getwidget=")
    --滑动层
    self.Panel_PageView = self:GetWidget("Panel_PageView")
    self.Panel_PageView:SetPosition( self.__root_widget:GetContentWidth() , self.Panel_PageView:GetPositionY() )

    --UI层
    self.Panel_UI = self:GetWidget("Panel_UI")
    --提示
    self.Image_tips = self:GetWidget("Image_tips")

    --自动翻牌
    self.Button_auto = Util:WidgetButton( 
                                              self:GetWidget("Button_auto") , 
                                              CPPGameLib.GetString("divination_autoOpen") ,
                                              CPPGameLib.Handler(self,self.OnButtonAuto) ,
                                              nil,nil, Macros.Color.button_yellow 
                                          )
    --洗牌
    self.Button_shuffle = Util:WidgetButton( 
                                              self:GetWidget("Button_shuffle") , 
                                              CPPGameLib.GetString("divination_shuffle") , 
                                              CPPGameLib.Handler(self,self.OnButtonShuffle) ,
                                              nil,nil, Macros.Color.button_yellow 
                                          )
    --阵容
    self.Button_dress = Util:WidgetButton( 
                                              self:GetWidget("Button_dress") , 
                                              "" , 
                                              CPPGameLib.Handler(self,self.OnButtonDress)
                                          )
    self.Button_dress:IgnoreContentAdaptWithSize(true)
    self.Button_dress:SetNormalImage(Resource.PathTool.GetSystemEntryIconPath("btn_lineup"),TextureResType.LOCAL)
    local Button_dress = Util:Name( CPPGameLib.GetString("divination_equipCard") , nil )
    Button_dress:SetPosition( self.Button_dress:GetContentWidth()/2 , 0 )
    Button_dress:SetAnchorPoint( 0.5 , 0.5 )
    self.Button_dress:AddChild( Button_dress ) 

    --切换
    self.Button_cut = Util:WidgetButton( 
                                              self:GetWidget("Button_cut") , 
                                              "" , 
                                              CPPGameLib.Handler(self,self.OnButtonCut)
                                          )
    self.Button_cut:IgnoreContentAdaptWithSize(true)

    self.text_Button_cut = Util:Name("" , nil )
    self.text_Button_cut:SetPosition( self.Button_cut:GetContentWidth()/2 , 0 )
    self.text_Button_cut:SetAnchorPoint( 0.5 , 0.5 )
    self.Button_cut:AddChild( self.text_Button_cut )
end

function Modules.DivinationScryerView:__ShowUI()
    --挑战界面
    self.challenge = LogicGUI.TemplDivinationNpc.New(  )
    self.__root_widget:AddChild( self.challenge:GetNode() , -1 )
    PosTool.Center( self.challenge )

    --ui控件
    --最高可获得
    local divination_maxGet = Util:Label( CPPGameLib.GetString( "divination_maxGet" ) )
    divination_maxGet:SetAnchorPoint( 0 , 0.5 )
    divination_maxGet:SetPosition( 10 , self.Image_tips:GetContentHeight()/2 )
    self.Image_tips:AddChild( divination_maxGet )

    self.text_tips = Util:RichText( "" )
    self.Image_tips:AddChild( self.text_tips )
    PosTool.RightTo( divination_maxGet , self.text_tips )

    --翻牌
    self.card = LogicGUI.TemplDivinationCard.New(  )
    self.Panel_PageView:AddChild( self.card:GetNode() )
    self.card:SetPosition( 0 , 0 )

    --倒计时
    self.lowCountdown = Util:Label( " " , nil , Macros.Color.Special )
    self.lowCountdown:SetAnchorPoint( 0.5 , 0.5 )
    self.lowCountdown:SetPosition( self.Button_shuffle:GetContentWidth()/2 , self.Button_shuffle:GetContentHeight()/2 + 50 )
    self.Button_shuffle:AddChild( self.lowCountdown )
    -- self.lowCountdown:SetVisible(false)

    --倒计时
    self.tallCountdown = Util:Label( " " , nil , Macros.Color.Special )
    self.tallCountdown:SetAnchorPoint( 0.5 , 0.5 )
    self.tallCountdown:SetPosition( self.Button_shuffle:GetContentWidth()/2 , self.Button_shuffle:GetContentHeight()/2 + 50 )
    self.Button_shuffle:AddChild( self.tallCountdown )
    self.tallCountdown:SetVisible(false)

    --占星睡觉
    local cnf = CPPGameLib.GetConfig( "Item", ConfigAdapter.Common.GetTyrantDivinationDraw() )
    local num = CallModuleFunc(ModuleType.Pack,"GetNumByItemID",ConfigAdapter.Common.GetTyrantDivinationDraw())

    local tab = {
                    { zi = cnf.name ..  CPPGameLib.GetString("common_colon") , dx = Macros.TypefaceSize.normal , mb = true },
                    { tp = Resource.PathTool.GetItemPath(cnf.res_id) , dx = 0.6 , jg = - 5 },
                    { zi = num , dx = Macros.TypefaceSize.normal , jg = 5 , mb = true },
                }
    self.crystal , self.crystalLst = Util:LabCenter(tab)
    self.crystal:SetAnchorPoint( 0 , 1 )
    self.Panel_UI:AddChild( self.crystal )
    self.crystal:SetPosition( -125 , self.Panel_UI:GetContentHeight() - 25 )
    self.crystal:SetVisible( false )

    self:__SetShowUI()
end

function Modules.DivinationScryerView:__SetShowUI()
    --当前关卡信息
    local info = CallModuleFunc(ModuleType.DIVINATION, "GetNowCollectionInfo" )

    --信息
    local img_key, lab_key
    if self.cutIndex == Macros.Game.CARD_TYPE.LOW then
        self.text_tips:SetText( info.low_describe )
        
        img_key = "sp_divination_toHigh"
        lab_key = "divination_switch_to_high"

    elseif self.cutIndex == Macros.Game.CARD_TYPE.TALL then
        self.text_tips:SetText( info.tall_describe )

        img_key = "sp_divination_toLow"
        lab_key = "divination_switch_to_low"
    end

    --提示
    local textW_1 = Util:Label( CPPGameLib.GetString( "divination_maxGet" ) ):GetContentWidth()
    local textW = self.text_tips:GetContentWidth()
    self.Image_tips:SetContentSize( textW_1 + textW + 20 , self.Image_tips:GetContentHeight() )

    --切换按钮设置
    self.Button_cut:SetNormalImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DIVINATION, img_key))
    self.text_Button_cut:SetString(CPPGameLib.GetString(lab_key))

    local num = CallModuleFunc(ModuleType.Pack,"GetNumByItemID",ConfigAdapter.Common.GetTyrantDivinationDraw())
    self.crystalLst[3]:SetString( num )

    --npc设置
    self.challenge:SetNpc( self.cutIndex )

    --卡牌切换
    self.card:SetCardCut( self.cutIndex )

    --自动翻牌按钮
    self:SetButtonAuto()

    --设置消耗
    self:SetConsume()
end

function Modules.DivinationScryerView:__Server()
    CallModuleFunc(ModuleType.DIVINATION, "PlayerZhanbuDataReq" )
   
    --选择
    self:BindGlobalEvent(Macros.Event.DivinationModule.DIVINATION_SELECT , function ( _ , index )
        print("xy" , "==========DIVINATION_SELECT=" , index )
        self:SetButtonAuto( index )
    end )

    --洗牌
    self:BindGlobalEvent(Macros.Event.DivinationModule.DIVINATION_DEAL , function ( _ , info )
        self:__SetShowUI()
        local isCombat = CallModuleFunc(ModuleType.DIVINATION, "GetIscombat" )
        if isCombat == true then
            CallModuleFunc(ModuleType.DIVINATION, "SetIscombat" , false )
            self:__Promote()
        end
    end )

    --低级自动刷新时间 
    self:BindGlobalEvent(Macros.Event.DivinationModule.DIVINATION_LOW_RENEW , function ( _ , time)
        self:SetLowShuffle()
        self:SetConsume()
    end)

    --高级自动刷新时间 
    self:BindGlobalEvent(Macros.Event.DivinationModule.DIVINATION_HIGH_RENEW , function ( _ , time)
        self:SetTallShuffle()
    end)

    --兑换商店
    self:BindGlobalEvent(Macros.Event.DivinationModule.DIVINATION_CONVERT , function ( _ , time)
        self:__SetShowUI()
    end)

    --翻牌
    self:BindGlobalEvent(Macros.Event.DivinationModule.DIVINATION_DRAW , function ( _ , time)
        self:__SetShowUI()
    end)  
end

function Modules.DivinationScryerView:__Promote()
    LogicGUI.TemplDivinationBreakview.New()
end

--自动翻牌
function Modules.DivinationScryerView:OnButtonAuto()
    local lst = {}
    if self.cutIndex == Macros.Game.CARD_TYPE.LOW then
        local card = CallModuleFunc(ModuleType.DIVINATION, "GetLowBCardGroupInfo" )
        lst = card.low_card_group_list
    elseif self.cutIndex == Macros.Game.CARD_TYPE.TALL then
        local card = CallModuleFunc(ModuleType.DIVINATION, "GetTyrantCardGroupInfo" )
        lst = card.high_card_group_list
    end

    --获得列表
    local t1 = {}
    for i , v in ipairs( lst ) do
        if v.card_id == 0 then
            local extractType , currencyType , consumeType = CallModuleFunc(ModuleType.DIVINATION, "DrawCalculate" , self.cutIndex , #t1+1 )
            if extractType == true then
                table.insert( t1 , { card_idx = i } )
            elseif currencyType == false then
                if self.cutIndex == Macros.Game.CARD_TYPE.LOW then
                    local txt = "currency_insufficient_"..consumeType
                    if consumeType == Macros.Game.RolePropType.GOLD and not self.gold then
                        self.gold = CallModuleFunc(ModuleType.SUNDRY, "ToIdGetPath", Macros.Game.RolePropType.GOLD , function ( ... )
                            self.gold = nil
                        end)
                    end
                    GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString(txt) )
                elseif self.cutIndex == Macros.Game.CARD_TYPE.TALL then
                    local cnf = CPPGameLib.GetConfig("Item", consumeType )
                    GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("hero_potential_tips2" , cnf.name ) )
                end
            end
        end
    end

    if #t1 <= 0 then
        return 
    end 

    local info = {}
    info.type = self.cutIndex
    info.card_idx_list = t1
    CallModuleFunc(ModuleType.DIVINATION, "OpenCardReq" , info )
end

--洗牌
function Modules.DivinationScryerView:OnButtonShuffle()
    local free , consumeType , consumeValue = CallModuleFunc(ModuleType.DIVINATION, "ShuffleCalculate" , self.cutIndex )

    if free ~= 0 then
        local info = {}
        info.refresh_type = self.cutIndex
        CallModuleFunc(ModuleType.DIVINATION, "CardRefreshReq" , info )
    else
        local tips = LogicGUI.TemplAffirmTips.New( { 
                                                        type = LogicGUI.TemplAffirmTipsType.YES_NO,
                                                        content = " ",
                                                        confirm_text = CPPGameLib.GetString("common_confirm") ,
                                                        confirm_func = function ( ... )
                                                            local item_num = CallModuleFunc(ModuleType.ROLE,"GetPropValue" , consumeType )
                                                            if tonumber(item_num) >= tonumber(consumeValue) then
                                                                local info = {}
                                                                info.refresh_type = self.cutIndex
                                                                CallModuleFunc(ModuleType.DIVINATION, "CardRefreshReq" , info ) 
                                                            else
                                                                local txt = "currency_insufficient_"..consumeType
                                                                if consumeType == Macros.Game.RolePropType.GOLD and not self.gold then
                                                                    self.gold = CallModuleFunc(ModuleType.SUNDRY, "ToIdGetPath", Macros.Game.RolePropType.GOLD , function ( ... )
                                                                        self.gold = nil
                                                                    end)
                                                                end
                                                                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString(txt) )
                                                                -- self:RemoveOnData()
                                                            end
                                                        end,
                                                        cancel_func = function ( ... )
                                                            -- self:RemoveOnData()
                                                        end,
                                                    } )
        tips:SetLoadedCallback(function()
            local Affirm = Util:LabCenter( { 
                                            { zi = CPPGameLib.GetString("tower_expenditureAffirm") , ys = Macros.Color.content },
                                            { tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.DIAMOND) , jg = 5 ,dx = 0.8},
                                            { zi = consumeValue , ys = Macros.Color.content },
                                            { zi = CPPGameLib.GetString("divination_buyUpTimes") , ys = Macros.Color.content },
                                        } )
            Affirm:SetPosition(tips.txt_content:GetContentWidth()/2 , tips.txt_content:GetContentHeight()/2 )
            tips.txt_content:AddChild(Affirm)
        end)
    end
end

-- 阵容跳转
function Modules.DivinationScryerView:OnButtonDress()
    print("xy" , "阵容")
    GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.HERO_LINEUP, {BaseViewSubType.HeroLineup.MAIN, nil , nil , nil , true})
end

-- 切换
function Modules.DivinationScryerView:OnButtonCut()
    self.crystal:SetVisible(false)
    if self.cutIndex == Macros.Game.CARD_TYPE.LOW then  
        self.crystal:SetVisible(true)
        self.cutIndex = Macros.Game.CARD_TYPE.TALL
    else
        self.cutIndex = Macros.Game.CARD_TYPE.LOW
    end
    self:__SetShowUI()
    self:SetButtonShuffle()
end

--设置自动
function Modules.DivinationScryerView:SetButtonAuto( num )
    if self.auto then
        self.auto:RemoveSelf(true)
        self.auto = nil
    end

    local icon , index = nil
    if self.cutIndex == Macros.Game.CARD_TYPE.LOW then
        local config = CPPGameLib.GetConfig("HuoBi", Macros.Game.RolePropType.GOLD )
        icon = config.big_icon
        index = CallModuleFunc(ModuleType.DIVINATION, "GetDrawAllLowConsume" , num )
    elseif self.cutIndex == Macros.Game.CARD_TYPE.TALL then
        icon = ConfigAdapter.Common.GetTyrantDivinationDraw()
        index = CallModuleFunc(ModuleType.DIVINATION, "GetDrawAllTallConsume" , num)
    end

    if index > 0 then
        local tab = {
                    { tp = Resource.PathTool.GetItemPath( icon ) , dx = 0.4 },
                    { zi = index , jg = 5 },
                }
        local itemLst = nil
        self.auto , itemLst = Util:LabCenter( tab )
        self.auto:SetAnchorPoint( 0.5 , 0.5 )
        self.auto:SetPosition( self.Button_auto:GetContentWidth()/2 , self.Button_auto:GetContentHeight()/2 - 50 )
        self.Button_auto:AddChild( self.auto )
    end
end

--设置洗牌
function Modules.DivinationScryerView:SetButtonShuffle()
    self.lowCountdown:SetVisible(false)
    self.tallCountdown:SetVisible(false)
    if self.cutIndex == Macros.Game.CARD_TYPE.LOW then
        self.Button_shuffle:SetText( CPPGameLib.GetString("divination_divination") )

        self.__title_bar:SetTitle( CPPGameLib.GetString("divination_divination") )

        self.lowCountdown:SetVisible(true)
        self:SetLowShuffle(  )
    elseif self.cutIndex == Macros.Game.CARD_TYPE.TALL then
        self.Button_shuffle:SetText( CPPGameLib.GetString("divination_divination_") )

        self.__title_bar:SetTitle( CPPGameLib.GetString("divination_divination_") )
        
        self.tallCountdown:SetVisible(true)
        self:SetTallShuffle(  )
    end
end

--设置屌丝洗牌按钮
function Modules.DivinationScryerView:SetLowShuffle()

    --倒计时
    local time = CallModuleFunc(ModuleType.DIVINATION, "GetLowCountdownTime" )
    local text = ""
    if time then
        text = CPPGameLib.GetString("divination_lowFree" , TimeTool.TransTimeStamp( "HH:MM:SS" , time ) )
    end
    self.lowCountdown:SetString( text )
end

--设置屌丝洗牌按钮消耗
function Modules.DivinationScryerView:SetConsume()
    --刷新次数
    if self.free then
        self.free:RemoveSelf(true)
        self.free = nil
    end

    --消耗
    local free , consumeType , consumeValue = CallModuleFunc(ModuleType.DIVINATION, "ShuffleCalculate" , self.cutIndex )
    
    local tab = {}
    if free ~= 0 then
        if self.cutIndex == Macros.Game.CARD_TYPE.LOW then
            local data = CallModuleFunc(ModuleType.DIVINATION, "GetLowBCardGroupInfo" )
            tab = {
                    { zi = CPPGameLib.GetString("hero_recruitFreeText") .. CPPGameLib.GetString("common_colon") },
                    { zi = data.cur_low_card_group_refresh_times .. "/" .. ConfigAdapter.Common.GetLowBDivinationFreeMax()  },
                  }
        elseif self.cutIndex == Macros.Game.CARD_TYPE.TALL then
            data = CallModuleFunc(ModuleType.DIVINATION, "GetTyrantCardGroupInfo" )
            tab = {
                    { zi = CPPGameLib.GetString("divination_current_Gratis")},
                  }
        end
    else
        if self.cutIndex == Macros.Game.CARD_TYPE.LOW then
            local data = CallModuleFunc(ModuleType.DIVINATION, "GetLowBCardGroupInfo" )
            tab = {
                        { tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.DIAMOND) , dx = 0.7 , jg = - 5 },
                        { zi = consumeValue , dx = Macros.TypefaceSize.normal , jg = 5 , mb = true },
                    }
        elseif self.cutIndex == Macros.Game.CARD_TYPE.TALL then
            data = CallModuleFunc(ModuleType.DIVINATION, "GetTyrantCardGroupInfo" )
            tab = {
                        { tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.DIAMOND) , dx = 0.7 , jg = - 5 },
                        { zi = consumeValue , dx = Macros.TypefaceSize.normal , jg = 5 , mb = true },
                    }
        end
    end

    local itemLst = nil
    self.free , itemLst = Util:LabCenter( tab )
    self.free:SetAnchorPoint( 0.5 , 0.5 )
    self.free:SetPosition( self.Button_shuffle:GetContentWidth()/2 , self.Button_shuffle:GetContentHeight()/2 - 50 )
    self.Button_shuffle:AddChild( self.free )
end

--设置土豪洗牌按钮
function Modules.DivinationScryerView:SetTallShuffle()

    local num , time = CallModuleFunc(ModuleType.DIVINATION, "GetHighCountdownTime" )

    local text = ""
    if num ~= 0 then
    else
        text = CPPGameLib.GetString("divination_lowFree" , TimeTool.TransTimeStamp( "HH:MM:SS" , time ) )
    end
    self.tallCountdown:SetString( text )
end

