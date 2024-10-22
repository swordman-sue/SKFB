--[[
%% @module: 占卜书
%% @author: yjg
%% @created: 2017年12月6日16:48:30
--]]
-- templ_divination_collect

Modules = Modules or {} 
Modules.DivinationBookView = Modules.DivinationBookView or BaseClass(BaseView)

function Modules.DivinationBookView:__init()
    self.__layout_name = "divination_book"
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bp_sp_challenge")}
    self.__title_bar_params = 
    {
      title = CPPGameLib.GetString("divination_collect"), 
        show_diamond = true,
        show_gold = true,
        show_destiny_coin = true,
        show_star_soul = true,
        close_func = function()
            self:CloseManual()
        end,
    }

    self.__open_callback = function()
        -- self.__tab_index = nil
        self:__OpenCallback()
    end
    -- self:__AddPreloadTextureAtlas( Resource.UITextureAtlasType.DIVINATION_COLLECT )  
    -- self:__AddPreloadTextureAtlas( Resource.UITextureAtlasType.DIVINATION_COLLECT ,true )  
end

function Modules.DivinationBookView:__OpenCallback( )
    self.index = 1
    self.ShowItem = {}
    self:__Getwidget()
    self:__ShowUI()
    self:__Server()
end 

function Modules.DivinationBookView:__Dispose()   
    if self.__zones_scrollview then
        self.__zones_scrollview:DeleteMe()
        self.__zones_scrollview = nil
    end
end

function Modules.DivinationBookView:__Getwidget( )
    self.Image_1 = self:GetWidget("Image_1")
    self.Image_1:SetImage( Resource.PathTool.GetUIResPath("divinationbig/sp_divination_collect_write") , TextureResType.LOCAL )
    self.Panel_1 = self:GetWidget("Panel_1")
end 

function Modules.DivinationBookView:__ShowUI( lst )

    local intervalW = (self.Panel_1:GetContentWidth()) / 2
    local intervalH = (self.Panel_1:GetContentHeight()) / 3

    self.itemLst = {}
    for i = 1 , 6 do
        local col_index = (i-1)%2
        local row_index = 3 - math.floor((i-1)/2) - 1

        self.itemLst[i] = Util:Layout( intervalW - 5 , intervalH - 5 )
        self.itemLst[i]:SetAnchorPoint( 0.5 , 0.5 )
        self.itemLst[i]:SetPosition( (intervalW/2) + intervalW * (col_index) , 20 + (intervalH/3) + intervalH * (row_index) )
        self.Panel_1:AddChild( self.itemLst[i] )
        self.itemLst[i]:SetVisible(false)
    end

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp_bag_num" )
    local bag_num = Util:Sprite( sp )
    bag_num:SetAnchorPoint( 0.5 , 0.5 )
    self.Panel_1:AddChild( bag_num , 1 )
    PosTool.Center(bag_num , 0 , -280 )

    self.bag_num = Util:Label( "00/00" )
    self.bag_num:SetAnchorPoint( 0.5 , 0.5 )
    bag_num:AddChild( self.bag_num , 1 )
    PosTool.Center( self.bag_num )

    self.Button_left = Util:Layout(100 , 100 , function ( ... )
        self:OpenTheBook( "Left" )
        self:OnLeft()
    end)
    self.Button_left:SetAnchorPoint(0.5,0.5)
    self.Button_left:SetPosition( -100 , self.Panel_1:GetContentHeight()/2 - 40 )
    self.Panel_1:AddChild(self.Button_left , 20 )

    local effect = nil
    self.effectid_1 ,effect = self:__CreateEffect({res_name = "UI_shangdianjiantou" } , self.Button_left , 999 )
    effect:SetAnchorPoint(0.5,0.5)
    effect:SetPosition( self.Button_left:GetContentWidth()/2 , self.Button_left:GetContentHeight()/2 )
    effect:SetScaleXFactor( -1 )


    self.Button_right = Util:Layout(100 , 100 , function ( ... )
        self:OpenTheBook( "Right" )
        self:OnRight()
    end)
    self.Button_right:SetAnchorPoint(0.5,0.5)
    self.Button_right:SetPosition( self.Panel_1:GetContentWidth() + 100 , self.Panel_1:GetContentHeight()/2 - 40 )
    self.Panel_1:AddChild(self.Button_right , 20 )

    local effect = nil
    self.effectid_2 ,effect = self:__CreateEffect({res_name = "UI_shangdianjiantou" } , self.Button_right , 999 )
    effect:SetAnchorPoint(0.5,0.5)
    effect:SetPosition( self.Button_right:GetContentWidth()/2 , self.Button_right:GetContentHeight()/2 )

    self.touch = 0
    self.ended = 0

    --触摸
    self.Panel_1:SetTouchEnabled(true)
    WidgetUnity.SetWidgetListener( self.Panel_1 , 
                                          function ( x , y )
                                              self:SetSelect( x , y , "touch" )
                                          end,
                                          function ( x , y )
                                              self:SetSelect( x , y , "moved" )
                                          end,
                                          function ( x , y )
                                              self:SetSelect( x , y , "remove" )
                                          end,
                                          function ( x , y )
                                              self:SetSelect( x , y , "ended" )
                                          end
                                  )

    self:__SetShowUI()
end

function Modules.DivinationBookView:__Server(  )
    --刷新
    self:BindGlobalEvent(Macros.Event.DivinationModule.COLLECT_UP , function (_, data)
        self:SetItem()
    end)
end

function Modules.DivinationBookView:__SetShowUI( )

    local cnf = CallModuleFunc(ModuleType.DIVINATION,"GetCollectionBookCnf" , unpack(self.__open_data) )

    local info = Util:SegmentationTab( cnf.lst , 6 )

    self.bag_num:SetString( self.index .. "/" .. #info )

    if not self.info then
        self.info = info
    end
    self.pageMax = #self.info

    -- self.Button_left:SetEnabled( true )
    -- self.Button_right:SetEnabled( true )

    -- if self.index - 1 <= 0 then
    --     self.Button_left:SetEnabled( false )
    -- elseif self.index + 1 > #self.info then
    --     self.Button_right:SetEnabled( false )
    -- end

    --隐藏所有条目
    for i , v in pairs( self.ShowItem ) do
        self.ShowItem[i].bg:SetVisible(false)
    end

    for i = 1 , #self.itemLst do
        if self.info[self.index][i] then
            self.itemLst[i]:SetVisible(true)
            self:NewItem( self.itemLst[i] , self.info[self.index][i].id , i )
        else
            self.itemLst[i]:SetVisible(false)
        end
    end

    self:SetItem()
end 


function Modules.DivinationBookView:SetSelect( x , y , type_ )
    if type_ == "touch" then
        self.touch = x
    elseif type_ == "remove" then
        self.ended = x
    elseif type_ == "ended" then
        self.ended = x
    end

    if self.openBook == true then
        return 
    end

    if self.touch ~= 0 and self.ended ~= 0 then
        --右
        if self.touch > self.ended then
            if math.abs( self.touch - self.ended ) >= 100 then
                print("xy" , "右")
                self:OpenTheBook( "Right" )
                self:OnRight()
            end
        --左
        else
            if math.abs( self.touch - self.ended ) >= 100 then
                print("xy" , "左")
                self:OpenTheBook( "Left" )
                self:OnLeft()
            end
        end
    end
end

function Modules.DivinationBookView:OpenTheBook( type_ )
    self.openBook = true
    -- local card , con = GlobalEffectMgr:CreateUIEffect( {     
    --                                                     res_name = "UI_zhanbu_xinshu" , 
    --                                                     type = Effect.EffectType.SKELETON , 
    --                                                     act_index = RenderObj.ActionIndex.IDLE ,
    --                                                     loop = false
    --                                                 } , self.Panel_1 , 10 )
    -- con:SetAnchorPoint(0.5 , 0.5 )
    -- con:SetPosition( self.Panel_1:GetContentWidth()/2 - 10 + 5 - 1 ,  -86 + 19 -5 )

    CPPActionManager.DelayTo( self.Panel_1 , 0.5 , GlobalCallbackMgr:AddCallBackFunc(function()
        print("xy" , "特效做完")
        self.openBook = false
        self.touch = 0
        self.ended = 0
    end))

    local ide = nil
    if type_ == "Right" then
        ide = RenderObj.ActionIndex.IDLE
    elseif type_ == "Left" then
        ide = RenderObj.ActionIndex.IDLE2
    end
end

--创建条目
function Modules.DivinationBookView:NewItem( node , cardId , index )

    local info = CallModuleFunc(ModuleType.DIVINATION,"GetCollectionGroupCnf" , cardId )

    if not self.ShowItem[ cardId ] then
        self.ShowItem[ cardId ] = {}

        --背景
        self.ShowItem[ cardId ].bg = Util:Layout( node:GetContentWidth() , node:GetContentHeight() )
        self.ShowItem[ cardId ].bg:SetAnchorPoint( 0.5 , 0.5 )
        node:AddChild( self.ShowItem[ cardId ].bg )
        PosTool.Center( self.ShowItem[ cardId ].bg )

        --图标
        self.ShowItem[ cardId ].lst = {} 
        self.ShowItem[ cardId ].lsteff = {}

        for i , v in ipairs( info.cost_card_list ) do
            --图标
            self.ShowItem[ cardId ].lst[i] = LogicGUI.IconLayer.New(true)
            self.ShowItem[ cardId ].lst[i]:SetAnchorPoint( 0.5 , 0.5 )
            self.ShowItem[ cardId ].lst[i]:SetPosition( 50 + ( (i-1) * 80 ) , self.ShowItem[ cardId ].bg:GetContentHeight()/2 + 10 )
            self.ShowItem[ cardId ].bg:AddChild(self.ShowItem[ cardId ].lst[i]:GetNode())
            self.ShowItem[ cardId ].lst[i]:SetScaleFactor( 0.8 )
            self.ShowItem[ cardId ].lst[i]:SetData({ id = v })

            self.ShowItem[ cardId ].lst[i]:SetGray(true)

            self.ShowItem[ cardId ].lsteff[i] , effect = self:__CreateEffect({res_name = "UI_panjuntouxiang"}, self.ShowItem[ cardId ].lst[i] , 3 )
            PosTool.Center(effect)
            GlobalEffectMgr:SetEffectVisible( self.ShowItem[ cardId ].lsteff[i] , false )
        end

        -- 名字
        self.ShowItem[ cardId ].name = Util:Label( info.name , Macros.TypefaceSize.largish , Macros.Color.content )
        self.ShowItem[ cardId ].name:SetAnchorPoint( 0 , 1 )
        self.ShowItem[ cardId ].name:SetPosition( 10 , self.ShowItem[ cardId ].bg:GetContentHeight() - 5 )
        self.ShowItem[ cardId ].bg:AddChild( self.ShowItem[ cardId ].name )

        --暗星
        self.ShowItem[ cardId ].star = {} 
        for i = 1 , info.max_star do
            --星星
            local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_sky_star" )
            local write = Util:Sprite( sp )
            write:SetAnchorPoint( 0.5 , 1 )
            self.ShowItem[ cardId ].bg:AddChild( write , 1 )
            write:SetPosition( self.ShowItem[ cardId ].bg:GetContentWidth()/2 + ( (i-1) * 30 ) - 10 , self.ShowItem[ cardId ].bg:GetContentHeight() - 5 )
            
            local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_bright_star" )
            self.ShowItem[ cardId ].star[i] = Util:Sprite( sp )
            self.ShowItem[ cardId ].star[i]:SetAnchorPoint( 0.5 , 0.5 )
            write:AddChild( self.ShowItem[ cardId ].star[i] , 1 )
            PosTool.Center( self.ShowItem[ cardId ].star[i] )
            self.ShowItem[ cardId ].star[i]:SetVisible(false)
        end

        --属性
        self.ShowItem[ cardId ].pro = {}
        local intervalW = 290 / 2
        local intervalH = 50 / 2
        local pro = CallModuleFunc(ModuleType.DIVINATION,"ToGroupIdGetProLst" , unpack(self.__open_data) , cardId )
        for i , v in ipairs( pro ) do
            local col_index = (i-1)%2
            local row_index = 2 - math.floor((i-1)/2) - 1

            local type_ , value_ = Util:Nature( v.type , v.value , true )

            --类型
            local protype_ = Util:Label( type_ .. CPPGameLib.GetString("common_colon") , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
            protype_:SetAnchorPoint( 0 , 0.5 )
            protype_:SetPosition( 10 + intervalW * (col_index) , 10 + (intervalH/3) + intervalH * (row_index) )
            self.ShowItem[ cardId ].bg:AddChild( protype_ , 1 )

            self.ShowItem[ cardId ].pro[v.type] = Util:Label( value_ , Macros.TypefaceSize.slightly , Macros.Color.content )
            self.ShowItem[ cardId ].bg:AddChild( self.ShowItem[ cardId ].pro[v.type] , 1 )
            PosTool.RightTo( protype_ , self.ShowItem[ cardId ].pro[v.type] )
        end

        --升星按钮
        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_littleyellow")
        self.ShowItem[ cardId ].btn = Util:Button( sp , function( ... )
            self:UpStar( cardId )
        end)
        self.ShowItem[ cardId ].btn:SetAnchorPoint( 1 , 0 )
        self.ShowItem[ cardId ].btn:SetPosition( self.ShowItem[ cardId ].bg:GetContentWidth() - 15 , 10 )
        self.ShowItem[ cardId ].bg:AddChild( self.ShowItem[ cardId ].btn )

        self.ShowItem[ cardId ].btnff , effect = self:__CreateEffect({res_name = "ui_anniuzoubian"}, self.ShowItem[ cardId ].btn , 3 )
        PosTool.Center(effect)
        effect:SetScaleFactor(0.8)
        GlobalEffectMgr:SetEffectVisible( self.ShowItem[ cardId ].btnff , false )

        --按钮状态
        local btnType = CallModuleFunc(ModuleType.DIVINATION,"ToGroupIdGetUpStarType" , cardId )
        if btnType == true then
            self.ShowItem[ cardId ].btn:SetEnabled( true )
            GlobalEffectMgr:SetEffectVisible( self.ShowItem[ cardId ].btnff , true )
        else
            self.ShowItem[ cardId ].btn:SetEnabled( false )
            GlobalEffectMgr:SetEffectVisible( self.ShowItem[ cardId ].btnff , false )
        end

        --满星
        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DIVINATION_COLLECT, "sp_divinationcollect_maxStar" , true )
        self.ShowItem[ cardId ].maxStar = Util:Sprite( sp )
        self.ShowItem[ cardId ].maxStar:SetAnchorPoint( 0.5 , 0.5 )
        self.ShowItem[ cardId ].bg:AddChild( self.ShowItem[ cardId ].maxStar , 1 )
        self.ShowItem[ cardId ].maxStar:SetPosition( self.ShowItem[ cardId ].btn:GetPositionX() - 55 , self.ShowItem[ cardId ].btn:GetPositionY() + 30 )
        self.ShowItem[ cardId ].maxStar:SetVisible(false)

        --详情按钮
        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "examine")
        local examine  =  Util:Button(   
                                        sp,
                                        function ( ... )
                                            local cnf = CallModuleFunc(ModuleType.DIVINATION,"GetGatherUpgradeInfo" , cardId )
                                            local tips = LogicGUI.TemplDivinationBookInfo.New( )
                                            tips:SetLoadedCallback( function ( ... )
                                                tips:SetName( cnf[1].name )
                                                tips:SetCell( cnf )
                                            end )
                                        end
                                    )
        examine:SetPosition( self.ShowItem[ cardId ].bg:GetContentWidth()/2 + 90 , self.ShowItem[ cardId ].bg:GetContentHeight()/2 + 10 )
        self.ShowItem[ cardId ].bg:AddChild(examine) 

        --取出按钮
        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_divina_rebirth")
        self.ShowItem[ cardId ].red_arrow  =  Util:Button(   
                                        sp,
                                        function ( ... )
                                            LogicGUI.TemplAffirmTips.New({
                                                type = LogicGUI.TemplAffirmTipsType.YES_NO,
                                                content = CPPGameLib.GetString("divination_extractTips"),
                                                confirm_text = CPPGameLib.GetString("common_confirm"),
                                                confirm_func = function()
                                                    --判断背包容量
                                                    self:OnRedArrow( cardId )
                                                end,
                                            })      
                                        end
                                    )
        self.ShowItem[ cardId ].red_arrow:SetPosition( examine:GetPositionX() + 50 , self.ShowItem[ cardId ].bg:GetContentHeight()/2 + 10 )
        self.ShowItem[ cardId ].red_arrow:SetVisible(false)
        self.ShowItem[ cardId ].bg:AddChild(self.ShowItem[ cardId ].red_arrow) 

        if index < 5 then
            local cut_off_line = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "thinline")
            local line = Util:Sprite9( cut_off_line )
            line:SetAnchorPoint(0.5,0.5)
            line:SetContentSize( self.ShowItem[ cardId ].bg:GetContentWidth() - 20 , line:GetContentHeight() )
            line:SetPosition( self.ShowItem[ cardId ].bg:GetContentWidth()/2 , 0 )
            self.ShowItem[ cardId ].bg:AddChild(line)
        end
    else
        self.ShowItem[ cardId ].bg:SetVisible( true )
    end
end

--设置条目
function Modules.DivinationBookView:SetItem( )
    --这本书的条目
    local lst = CallModuleFunc(ModuleType.DIVINATION,"GetBookLst" , unpack(self.__open_data) )
    --数据整合
    local t1 = {}
    if lst then
        for i , v in ipairs( lst ) do
            t1[v.id] = v.star
        end
    end

    local cnf = CallModuleFunc(ModuleType.DIVINATION,"GetCollectionBookCnf" , unpack(self.__open_data) )
    local info = Util:SegmentationTab( cnf.lst , 6 )
    if not self.info then
        self.info = info
    end

    if not self.info[self.index] then
        return 
    end

    for i = 1 , #self.itemLst do
        if not self.info[self.index][i] then
            return
        end
        local cardId = self.info[self.index][i].id
        if self.ShowItem[ cardId ] then
            --取下按钮
            if t1[cardId] and t1[cardId] ~= 0 then
                self.ShowItem[ cardId ].red_arrow:SetVisible(true)
                for j = 1 , t1[cardId] do
                    if self.ShowItem[ cardId ].star[j] then
                        self.ShowItem[ cardId ].star[j]:SetVisible(true)
                    end
                end
            else
                self.ShowItem[ cardId ].red_arrow:SetVisible(false)
                if self.ShowItem[ cardId ].star then
                    for j = 1 , #self.ShowItem[ cardId ].star do
                        self.ShowItem[ cardId ].star[j]:SetVisible(false)
                    end
                end
            end
            
            --属性
            local pro = CallModuleFunc(ModuleType.DIVINATION,"ToGroupIdGetProLst" , unpack(self.__open_data) , cardId )
            for i , v in ipairs( pro ) do
                local type_ , value_ = Util:Nature( v.type , v.value , true )
                self.ShowItem[ cardId ].pro[v.type]:SetString(value_)
            end

            --按钮状态
            local btnType = CallModuleFunc(ModuleType.DIVINATION,"ToGroupIdGetUpStarType" , cardId )
            self.ShowItem[ cardId ].maxStar:SetVisible(false)
            self.ShowItem[ cardId ].btn:SetVisible( false )

            if btnType == true then

                local info = CallModuleFunc(ModuleType.DIVINATION,"GetCollectionGroupCnf" , cardId )
                if t1[cardId] and t1[cardId] ~= 0 and t1[cardId] < info.max_star then
                    self.ShowItem[ cardId ].btn:SetVisible( true )
                    self.ShowItem[ cardId ].btn:SetText( CPPGameLib.GetString("divination_upSatrText") )
                    self.ShowItem[ cardId ].btn:SetEnabled( true )
                    self.ShowItem[ cardId ].btn:SetTextColor(unpack(Macros.Color.btn))
                    local r, g, b, a = unpack(Macros.Color.button_yellow)
                    self.ShowItem[ cardId ].btn:SetOutline(r, g, b, a, 2)
                    GlobalEffectMgr:SetEffectVisible( self.ShowItem[ cardId ].btnff , true )

                elseif t1[cardId] and t1[cardId] >= info.max_star then
                    self.ShowItem[ cardId ].maxStar:SetVisible(true)
                    self.ShowItem[ cardId ].btn:SetVisible( false )
                    GlobalEffectMgr:SetEffectVisible( self.ShowItem[ cardId ].btnff , false )

                else
                    self.ShowItem[ cardId ].btn:SetVisible( true )
                    self.ShowItem[ cardId ].btn:SetText( CPPGameLib.GetString("divination_collectText") )
                    self.ShowItem[ cardId ].btn:SetEnabled( true )
                    self.ShowItem[ cardId ].btn:SetTextColor(unpack(Macros.Color.btn))
                    local r, g, b, a = unpack(Macros.Color.button_yellow)
                    self.ShowItem[ cardId ].btn:SetOutline(r, g, b, a, 2)
                    GlobalEffectMgr:SetEffectVisible( self.ShowItem[ cardId ].btnff , true )
                end

            else
                self.ShowItem[ cardId ].btn:SetVisible( true )

                if t1[cardId] and t1[cardId] ~= 0 then
                    self.ShowItem[ cardId ].btn:SetText( CPPGameLib.GetString("divination_upSatrText") )
                else
                    self.ShowItem[ cardId ].btn:SetText( CPPGameLib.GetString("divination_collectText") )
                end
                Util:GreyButton( self.ShowItem[ cardId ].btn )

                GlobalEffectMgr:SetEffectVisible( self.ShowItem[ cardId ].btnff , false )
            end

            --图标
            local info_lst = CallModuleFunc(ModuleType.DIVINATION,"GetCollectionGroupCnf" , cardId )
            for j , v in ipairs( info_lst.cost_card_list ) do

                local index = CallModuleFunc(ModuleType.DIVINATION,"ToInfoIdGetDivinationNum" , v , true )
                print("xy" , "index----" , index )
                if index <= 0 then
                    self.ShowItem[ cardId ].lst[j]:SetGray(true)
                    GlobalEffectMgr:SetEffectVisible( self.ShowItem[ cardId ].lsteff[j] , false )
                else
                    self.ShowItem[ cardId ].lst[j]:SetGray(false)
                    if btnType == true then
                        GlobalEffectMgr:SetEffectVisible( self.ShowItem[ cardId ].lsteff[j] , true )
                    end
                end
            end
        end
    end
end

--升星按钮
function Modules.DivinationBookView:UpStar( cardId )
    local info = {}
    info.book_id = unpack(self.__open_data)
    info.group_id = cardId

    local star = CallModuleFunc(ModuleType.DIVINATION,"ToBookAndIdGetStar" , unpack(self.__open_data) , cardId )
    if not star or star <= 0 then
        CallModuleFunc(ModuleType.DIVINATION,"CellectionGroupActivateReq" , info )
    else
        CallModuleFunc(ModuleType.DIVINATION,"CellectionGroupStarUpReq" , info )
    end
end

--取出按钮
function Modules.DivinationBookView:OnRedArrow( cardId )

    --这本书的条目
    local lst = CallModuleFunc(ModuleType.DIVINATION,"GetBookLst" , unpack(self.__open_data) )
    --数据整合
    local t1 = {}
    if lst then
        for i , v in ipairs( lst ) do
            t1[v.id] = v.star
        end
    end

    local index = 0
    CallModuleFunc(ModuleType.SUNDRY, "PackVacancy", Macros.Game.ObjType.FAIRY , t1[cardId] , true , function ( sum )
        index = sum
    end )
    
    if index - t1[cardId] <= 0 then
        return 
    end

    local info = {}
    info.book_id = unpack(self.__open_data)
    info.group_id = cardId
    CallModuleFunc(ModuleType.DIVINATION,"CellectionGroupRestReq" , info )
end


--←
function Modules.DivinationBookView:OnLeft(  )
    if self.index - 1 <= 0 then
        self.index = 1
    else
        self.index = self.index - 1
    end 
    self:__SetShowUI()
end

--→
function Modules.DivinationBookView:OnRight(  )
    if self.index + 1 >= self.pageMax then
        self.index = self.pageMax
    else
        self.index = self.index + 1
    end 
    self:__SetShowUI()
end

