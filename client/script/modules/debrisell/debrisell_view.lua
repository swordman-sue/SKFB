
--
-- @brief 出售界面
-- @author: yjg
-- @date: 2016年11月2日11:54:44
--

Modules = Modules or {}

Modules.DebriSellView = Modules.DebriSellView or BaseClass(BaseView)

function Modules.DebriSellView:__init()
    self.__layout_name = "sale"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("sell_chushou"),
        show_diamond = true,
        show_gold = true,
        show_stamina = true,
        show_energy = true,
        close_func = function()
            CallModuleFunc(ModuleType.DEBRISELL, "RemoveAllOnData")
            self:CloseManual()
        end,
    }

    self.__open_callback = function()
        self:__OpenCallback()
    end

    local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no")
    self.__tab_params = {
        {sp_2,sp_1},
        {
            CPPGameLib.GetString("pack"),
        },
        10,
        CPPGameLib.Handler(self, self.OnYeMei),
        false,
        nil,nil,self.__lock_level_list
    }

    --列表条目
    self:__AddPreloadTemplate(GUI.TemplateRegistry.SALE_ITEM)
end

function Modules.DebriSellView:__OpenCallback()    
    self:__Getwidget()
    self:__ShowUI()
end

function Modules.DebriSellView:__Dispose()   
    if self.__zones_scrollview then
        self.__zones_scrollview:DeleteMe()
        self.__zones_scrollview = nil
    end
end

function Modules.DebriSellView:__RegistAllEvents()
    --删除装备
    self:BindGlobalEvent(Macros.Event.EquipModule.EQUIP_REMOVE_EQUIP,function (_, data)
        print("xy" , "删除装备")
        self:OnUpEquip(data)
    end)

    --刷新装备碎片
    self:BindGlobalEvent(Macros.Event.EquipModule.EQUIP_UP_DEBRIS,function (_, data)
        print("xy" , "刷新碎片")
        self:OnUPDebris(data)
    end)

    --刷新英雄碎片
    self:BindGlobalEvent(Macros.Event.HeroModule.HERO_UP_DEBRIS,function (_, data)
        print("xy" , "刷新碎片")
        self:OnUPDebris(data)
    end)

    --删除英雄
    self:BindGlobalEvent(Macros.Event.HeroModule.HERO_REMOVE, function ( _, data )
        print("xy" , "删除英雄")       
        self:OnUpEquip(data)
    end)

    --删除圣器
    self:BindGlobalEvent(Macros.Event.TreasureModule.TREASURE_REMOVE,function (_, data)
        print("xy" , "删除圣器")  
        self:OnUpEquip(data)
    end)

    --界面刷新
    self:BindGlobalEvent(Macros.Event.DebriSellModule.DEDRISELL_UP,function (_, data)
        self:ShowOnData()
    end)
end

function Modules.DebriSellView:__Dispose()
end

function Modules.DebriSellView:__Getwidget( ... )

    --页签
    self.Panel_Backpackcbx = self:GetWidget("Panel_Backpackcbx")
    --内容背景
    self.Image_content = self:GetWidget("Image_content") 
    --列表背景
    self.Image_5 = self:GetWidget("Image_5") 
    --选中数量
    self.Text_onNum_0 = self:GetWidget("Text_onNum_0") 
    --出售可获得
    self.Text_gain_0 = self:GetWidget("Text_gain_0") 
    --筛选
    self.Button_screen_0 =  Util:WidgetButton( 
                                                self:GetWidget("Button_screen_0") , 
                                                CPPGameLib.GetString("sell_pinzhishaixuan") , 
                                                CPPGameLib.Handler(self,self.OnButtonScreen) ,
                                                nil,nil, Macros.Color.button_red
                                                )
    --出售
    self.Button_sell_0 = Util:WidgetButton( 
                                                self:GetWidget("Button_sell_0") ,
                                                CPPGameLib.GetString("sell_chushou") , 
                                                CPPGameLib.Handler(self,self.OnButtonSell) ,
                                                nil,nil, Macros.Color.button_yellow
                                                )
end

function Modules.DebriSellView:__ShowUI( ... )

    --页签0
    self.__tab:On(1)
    self.__tab:SetPosition(0 , 10)
    self.__tab:SetCboxColor( Macros.Color.button_yellow , Macros.Color.button_red )
    self.Panel_Backpackcbx:AddChild(self.__tab:GetNode())

    --列表
    local params =  
    {
        item_class = Modules.SaleItem,
        item_width = 455.5,
        item_height = 120,
        row = 4,
        col = 2,
        item_space_r = 2.5,
        item_space_c = 2.5,
        -- view_width = 925.00,
        view_height = self.Image_5:GetContentHeight() - 10 ,
    }
    -- 创建参数
    self.__zones_scrollview = GUI.ScrollView.New(params)
    self.Image_5:AddChild(self.__zones_scrollview:GetNode())
    PosTool.Center(self.__zones_scrollview:GetNode())


    local btn = self.__tab:GetBtn(1)
    btn:SetTouchEnabled(false)

    if unpack(self.__open_data) == Macros.Game.SELECT_TYPE.EQUIP then
        self:SetEquip( btn )
    elseif unpack(self.__open_data) == Macros.Game.SELECT_TYPE.EQUIP_DEBRIS then
        self:SetEquipDebris( btn )
    elseif unpack(self.__open_data) == Macros.Game.SELECT_TYPE.HERO then
        self:SetHero( btn )
    elseif unpack(self.__open_data) == Macros.Game.SELECT_TYPE.HERO_DEBRIS then
        self:SetHeroDebris( btn )
    elseif unpack(self.__open_data) == Macros.Game.SELECT_TYPE.TREASURE then
        self:SetTreasure( btn )
    end

    --选择数量
    local onNum = Util:Label(CPPGameLib.GetString("sell_yixuanshuliang") , nil , Macros.Color.keypoint )
    onNum:SetAnchorPoint(0,0.5)
    onNum:SetPosition(10,self.Text_onNum_0:GetContentHeight() )
    self.Text_onNum_0:AddChild(onNum)

    self.onNum = Util:Label( "0" , nil , Macros.Color.content )
    PosTool.RightTo(onNum,self.onNum)
    self.Text_onNum_0:AddChild(self.onNum)

    --可获得
    self.gain = Util:Label(CPPGameLib.GetString("sell_chushouhuode") , nil , Macros.Color.keypoint )
    self.gain:SetAnchorPoint(0,0.5)
    self.gain:SetPosition(10,self.Text_gain_0:GetContentHeight() )
    self.Text_gain_0:AddChild(self.gain)

    self:ShowOnData()
end

--选择信息显示
function Modules.DebriSellView:ShowOnData( )
    if unpack(self.__open_data) == Macros.Game.SELECT_TYPE.EQUIP then
        self:SetOnEquipData()
    elseif unpack(self.__open_data) == Macros.Game.SELECT_TYPE.EQUIP_DEBRIS then
        self:SetOnEquipDebrisData()
    elseif unpack(self.__open_data) == Macros.Game.SELECT_TYPE.HERO then
        self:SetOnHeroData()
    elseif unpack(self.__open_data) == Macros.Game.SELECT_TYPE.HERO_DEBRIS then
        self:SetOnHeroDebrisData()
    elseif unpack(self.__open_data) == Macros.Game.SELECT_TYPE.TREASURE then
        self:SetOnTreasureData()
    end

    self.__zones_scrollview:ItemDataIter(function(i, item_data)
        local item = self.__zones_scrollview:GetItem(i)
        if item then
            item:SetSelected( item_data )
        end
    end)
end

--切换页签
function Modules.DebriSellView:OnYeMei(index)
    local com_sp_cbox_wire = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp_cbox_wire")
    if self.com_sp_cbox_wire then
        self.com_sp_cbox_wire:RemoveSelf(true)
        self.com_sp_cbox_wire = nil
    end
    self.com_sp_cbox_wire = Util:Sprite(com_sp_cbox_wire)
    self.__tab.btn_list[index]:AddChild( self.com_sp_cbox_wire )
    PosTool.Center( self.com_sp_cbox_wire , 0 , -35 )
end

------------------------------------------------------------------------------------------------------------------------
--装备
------------------------------------------------------------------------------------------------------------------------
function Modules.DebriSellView:SetEquip( btn )
    btn:SetText( CPPGameLib.GetString("sell_equipHeadline") )

    local lst = CallModuleFunc(ModuleType.DEBRISELL, "GetEquipLst" )
    self.__zones_scrollview:SetDataList( lst )

    WidgetUnity.SetWidgetListener( self.Button_screen_0 , nil, nil, function ( ... )

        GUITemplateMgr:CreateTemplateInstance( 
                                                GUI.TemplateRegistry.SALE_SCREEN , 
                                                false , { 1 , 2 } , 
                                                function( lst )
                                                    local heroLst = CallModuleFunc(ModuleType.DEBRISELL, "GetEquipLst" )
                                                    for i , v in ipairs( heroLst ) do
                                                        if lst[v.quality] then
                                                            CallModuleFunc(ModuleType.DEBRISELL, "SetOnData", v , true )
                                                        end
                                                    end
                                                end )
    end, nil, 0.5*1000)
end

--装备显示信息
function Modules.DebriSellView:SetOnEquipData(index)
    local lst = CallModuleFunc(ModuleType.DEBRISELL, "GetOnData" )

    local num = CPPGameLib.GetTableLength( lst )
    self.onNum:SetString( num )

    local price = 0
    for i,v in pairs( lst ) do
        price = price + v.price
    end

    if self.gain_ then
        self.gain_:RemoveSelf(true)
        self.gain_ = nil
    end

    self.gain_ = Util:MoneyControl( Macros.Game.RolePropType.GOLD , 0.8 , price or 0 ,nil, Macros.Color.content , 10 )
    PosTool.RightTo(self.gain,self.gain_)
    self.Text_gain_0:AddChild(self.gain_)
end

------------------------------------------------------------------------------------------------------------------------
--装备碎片
------------------------------------------------------------------------------------------------------------------------
function Modules.DebriSellView:SetEquipDebris( btn )
    btn:SetText( CPPGameLib.GetString("sell_equipHeadline_1") )

    local lst = CallModuleFunc(ModuleType.DEBRISELL, "GetEquipDebrisLst" )
    self.__zones_scrollview:SetDataList( lst )
    WidgetUnity.SetWidgetListener( self.Button_screen_0 , nil, nil, function ( ... )
        GUITemplateMgr:CreateTemplateInstance( 
                                                GUI.TemplateRegistry.SALE_SCREEN , 
                                                false , { 2 , 3 } , 
                                                function( lst )
                                                    local heroLst = CallModuleFunc(ModuleType.DEBRISELL, "GetEquipDebrisLst" )
                                                    for i , v in ipairs( heroLst ) do
                                                        if lst[v.quality] then
                                                            CallModuleFunc(ModuleType.DEBRISELL, "SetOnData", v , true )
                                                        end
                                                    end
                                                end)
    end, nil, 0.5*1000)
end

--装备碎片显示信息
function Modules.DebriSellView:SetOnEquipDebrisData(index)
    local lst = CallModuleFunc(ModuleType.DEBRISELL, "GetOnData" )

    local index = 0
    local sum = 0
    for i,v in pairs( lst ) do
        index = index + v.item_num
        sum = sum + v.price
    end 

    self.onNum:SetString( index )

    if self.gain_ then
        self.gain_:RemoveSelf(true)
        self.gain_ = nil
    end

    self.gain_ = Util:MoneyControl( Macros.Game.RolePropType.TOWER_PRESTIGE , 0.8 , sum or 0 ,nil, Macros.Color.content , 10 )
    PosTool.RightTo(self.gain,self.gain_)
    self.Text_gain_0:AddChild(self.gain_)
end

------------------------------------------------------------------------------------------------------------------------
--英雄
------------------------------------------------------------------------------------------------------------------------
function Modules.DebriSellView:SetHero( btn )
    btn:SetText( CPPGameLib.GetString("sell_yinxiong") )

    local lst = CallModuleFunc(ModuleType.DEBRISELL, "GetHeroLst" )
    self.__zones_scrollview:SetDataList( lst )

    WidgetUnity.SetWidgetListener( self.Button_screen_0 , nil, nil, function ( ... )
        GUITemplateMgr:CreateTemplateInstance( 
                                                GUI.TemplateRegistry.SALE_SCREEN , 
                                                false , { 0 , 1 , 2 } , 
                                                function( lst )
                                                    local heroLst = CallModuleFunc(ModuleType.DEBRISELL, "GetHeroLst" )
                                                    for i , v in ipairs( heroLst ) do
                                                        CPPGameLib.PrintTable("xy" , v , "heroLst")
                                                        if lst[v.quality] then
                                                            CallModuleFunc(ModuleType.DEBRISELL, "SetOnData", v , true )
                                                        end
                                                    end
                                                end)
    end, nil, 0.5*1000)
end

--英雄显示信息
function Modules.DebriSellView:SetOnHeroData(index)
    local lst = CallModuleFunc(ModuleType.DEBRISELL, "GetOnData" )

    local num = CPPGameLib.GetTableLength( lst )
    self.onNum:SetString( num )

    local price = 0
    for i,v in pairs( lst ) do
        price = price + v.price
    end

    if self.gain_ then
        self.gain_:RemoveSelf(true)
        self.gain_ = nil
    end

    self.gain_ = Util:MoneyControl( Macros.Game.RolePropType.GOLD , 0.8 , price or 0 ,nil, Macros.Color.content , 10 )
    PosTool.RightTo(self.gain,self.gain_)
    self.Text_gain_0:AddChild(self.gain_)
end

------------------------------------------------------------------------------------------------------------------------
--英雄碎片
------------------------------------------------------------------------------------------------------------------------
function Modules.DebriSellView:SetHeroDebris( btn )
    btn:SetText( CPPGameLib.GetString("sell_yinxiong_1") )

    local lst = CallModuleFunc(ModuleType.DEBRISELL, "GetHeroDebrisLst" )
    self.__zones_scrollview:SetDataList( lst )

    WidgetUnity.SetWidgetListener( self.Button_screen_0 , nil, nil, function ( ... )
        GUITemplateMgr:CreateTemplateInstance( 
                                                GUI.TemplateRegistry.SALE_SCREEN , 
                                                false , { 2 , 3 } , 
                                                function( lst )
                                                    local heroLst = CallModuleFunc(ModuleType.DEBRISELL, "GetHeroDebrisLst" )
                                                    for i , v in ipairs( heroLst ) do
                                                        if lst[v.quality] then
                                                            CallModuleFunc(ModuleType.DEBRISELL, "SetOnData", v , true )
                                                        end
                                                    end
                                                end)
    end, nil, 0.5*1000)
end

--英雄碎片显示信息
function Modules.DebriSellView:SetOnHeroDebrisData(index)
    local lst = CallModuleFunc(ModuleType.DEBRISELL, "GetOnData" )

    local index = 0
    local sum = 0
    for i,v in pairs( lst ) do
        index = index + v.item_num
        sum = sum + v.price
    end 

    self.onNum:SetString( index )

    if self.gain_ then
        self.gain_:RemoveSelf(true)
        self.gain_ = nil
    end

    self.gain_ = Util:MoneyControl( Macros.Game.RolePropType.HERO_SOUL , 0.8 , sum or 0 ,nil, Macros.Color.content , 10 )
    PosTool.RightTo(self.gain,self.gain_)
    self.Text_gain_0:AddChild(self.gain_)
end

------------------------------------------------------------------------------------------------------------------------
--圣器
------------------------------------------------------------------------------------------------------------------------
function Modules.DebriSellView:SetTreasure( btn )
    btn:SetText( CPPGameLib.GetString("sell_treasure") )

    local lst = CallModuleFunc(ModuleType.DEBRISELL, "GetTreasureLst" )
    self.__zones_scrollview:SetDataList( lst )

    WidgetUnity.SetWidgetListener( self.Button_screen_0 , nil, nil, function ( ... )
        GUITemplateMgr:CreateTemplateInstance( 
                                                GUI.TemplateRegistry.SALE_SCREEN , 
                                                false , { 2 } , 
                                                function( lst )
                                                    local heroLst = CallModuleFunc(ModuleType.DEBRISELL, "GetTreasureLst" )
                                                    for i , v in ipairs( heroLst ) do
                                                        if lst[v.quality] then
                                                            CallModuleFunc(ModuleType.DEBRISELL, "SetOnData", v , true )
                                                        end
                                                    end
                                                end)
    end, nil, 0.5*1000)
end

--圣器显示信息
function Modules.DebriSellView:SetOnTreasureData(index)
    local lst = CallModuleFunc(ModuleType.DEBRISELL, "GetOnData" )

    local num = CPPGameLib.GetTableLength( lst )
    self.onNum:SetString( num )

    local price = 0
    for i,v in pairs( lst ) do
        price = price + v.price
    end

    if self.gain_ then
        self.gain_:RemoveSelf(true)
        self.gain_ = nil
    end

    self.gain_ = Util:MoneyControl( Macros.Game.RolePropType.GOLD , 0.8 , price or 0 ,nil, Macros.Color.content , 10 )
    PosTool.RightTo(self.gain,self.gain_)
    self.Text_gain_0:AddChild(self.gain_)
end

--筛选
function Modules.DebriSellView:OnButtonScreen( ... )
    CPPGameLib.PrintTable("xy" , self.__open_data , "self.__open_data")
end

--出售
function Modules.DebriSellView:OnButtonSell( ... )
    
    local sell = CallModuleFunc(ModuleType.DEBRISELL, "GetOnData")
    local num = CPPGameLib.GetTableLength( sell )
    if not sell or num <= 0 then
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("sell_meixuan"))
        return
    end
    local t1 = {}
    local index = 1
    local sum = 0
    for i,v in pairs(sell) do
        t1[index] = {}
        if unpack(self.__open_data) == Macros.Game.SELECT_TYPE.EQUIP_DEBRIS or 
            unpack(self.__open_data) == Macros.Game.SELECT_TYPE.HERO_DEBRIS then
            t1[index].goods_id = v.info_id
        else
            t1[index].goods_id = v.uid
        end
        index = index + 1
    end

    local index = 0
    for i,v in pairs( sell ) do
        if unpack(self.__open_data) == Macros.Game.SELECT_TYPE.EQUIP_DEBRIS or 
            unpack(self.__open_data) == Macros.Game.SELECT_TYPE.HERO_DEBRIS then
            index = index + v.item_num
        else
            index = index + 1
        end
        sum = sum + v.price
    end 

    local text = CPPGameLib.GetString("sell_treasure_1" , index)
    local type_ = Macros.Game.RolePropType.GOLD
    local sellType = Macros.Game.ObjType.TREASURE

    if unpack(self.__open_data) == Macros.Game.SELECT_TYPE.EQUIP then
        text = CPPGameLib.GetString("sell_equip" , index)
        type_ = Macros.Game.RolePropType.GOLD
        sellType = Macros.Game.ObjType.EQUIP
    elseif unpack(self.__open_data) == Macros.Game.SELECT_TYPE.EQUIP_DEBRIS then
        text = CPPGameLib.GetString("sell_equipItem" , index)
        type_ = Macros.Game.RolePropType.TOWER_PRESTIGE
        sellType = Macros.Game.ObjType.ITEM
    elseif unpack(self.__open_data) == Macros.Game.SELECT_TYPE.HERO then
        text = CPPGameLib.GetString("sell_hero" , index)
        type_ = Macros.Game.RolePropType.GOLD
        sellType = Macros.Game.ObjType.HERO
    elseif unpack(self.__open_data) == Macros.Game.SELECT_TYPE.HERO_DEBRIS then
        text = CPPGameLib.GetString("sell_heroItem" , index)
        type_ = Macros.Game.RolePropType.HERO_SOUL
        sellType = Macros.Game.ObjType.ITEM
    end

    local tips = LogicGUI.TemplAffirmTips.New( { 
                                                    type = LogicGUI.TemplAffirmTipsType.YES_NO,
                                                    content = " ",
                                                    confirm_text = CPPGameLib.GetString("common_confirm") ,
                                                    confirm_func = function ( ... )

                                                        local info = {
                                                            type = sellType ,
                                                            goods_list = t1
                                                        }

                                                        CallModuleFunc(ModuleType.SHOP, "ShopGoodsSellRep",info)

                                                        CallModuleFunc(ModuleType.DEBRISELL, "RemoveAllOnData")
                                                        self:ShowOnData()
                                                    end,
                                                    cancel_text = CPPGameLib.GetString("common_cancel") ,
                                                    cancel_func = function ( ... )
                                                        print("----2")
                                                    end,
                                                } )

    tips:SetLoadedCallback( function ( ... )
        local dungeon_resetMax = Util:Label( text , nil , Macros.Color.content )
        dungeon_resetMax:SetPosition( tips.txt_content:GetContentWidth()/2 , (tips.txt_content:GetContentHeight()/2) + (dungeon_resetMax:GetContentHeight()/2 ) + 2 )
        tips.txt_content:AddChild(dungeon_resetMax)

        local dungeon_resetVipTips = Util:Label( CPPGameLib.GetString("sell_goldTips" , sum ) .. CPPGameLib.GetString("currency_get_"..type_) , nil , Macros.Color.content )
        dungeon_resetVipTips:SetPosition( tips.txt_content:GetContentWidth()/2 , (tips.txt_content:GetContentHeight()/2) - (dungeon_resetVipTips:GetContentHeight()/2) - 2 )
        tips.txt_content:AddChild(dungeon_resetVipTips)
    end )

end

--刷新装备 or 英雄 按uid删除
function Modules.DebriSellView:OnUpEquip( info )
    --列表相关调整
    local del_index
    self.__zones_scrollview:ItemDataIter(function(i, item_data)
        if item_data.uid == info.uid then
            del_index = i
        end
    end)
    if del_index then
        self.__zones_scrollview:DetachItem(del_index)
        self.__zones_scrollview:RefreshLayout()
    end
end

--刷新碎片
function Modules.DebriSellView:OnUPDebris( info )
    --列表相关调整
    local del_index
    self.__zones_scrollview:ItemDataIter(function(i, item_data)
        if item_data.info_id == info.item_id then
            del_index = i
        end
    end)
    if del_index then
        self.__zones_scrollview:DetachItem(del_index)
        self.__zones_scrollview:RefreshLayout()
    end
end

--刷新选中
function Modules.DebriSellView:UPOnData( data )
    CPPGameLib.PrintTable("xy", data ,"datadatadata")

    --列表相关调整    
    local lst = {}
    self.__zones_scrollview:ItemDataIter(function(i, item_data)
        for _, v in ipairs(data) do
            if item_data.quality == v.value then
                local item = self.__zones_scrollview:GetItem(i)
                if item then
                    item.cbx:SetSelectedState(true)
                end
                CallModuleFunc(ModuleType.DEBRISELL, "AddOnData", item_data)

                table.insert( lst , v )
            end
        end
    end)

    if #lst <= 0 then
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("sell_meiyou"))
    end
end





Modules = Modules or {}
Modules.SaleItem = Modules.SaleItem or BaseClass(GUI.ScrollItem)

function Modules.SaleItem:__init()
    self:InitTemplate(GUI.TemplateRegistry.SALE_ITEM)   
end

function Modules.SaleItem:__delete()

end

function Modules.SaleItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)

    self:__Getwidget()
    self:__ShowUI(data)
end

function Modules.SaleItem:__Getwidget()
    --条目
    self.Image_sellItem = self:GetWidget("Image_sellItem") 
    --条目容器
    self.Panel_sellItem = self:GetWidget("Panel_sellItem") 
    --名字
    self.Text_sellName = self:GetWidget("Text_sellName") 
    --强化&属性
    self.Text_intensify = self:GetWidget("Text_intensify") 
    --价钱
    self.Text_price = self:GetWidget("Text_price") 
    --图标
    self.Panel_icon = self:GetWidget("Panel_icon") 
    --选择框
    self.Panel_cbox = self:GetWidget("Panel_cbox") 
end

function Modules.SaleItem:__ShowUI(data)

    -- 头像
    local icon = LogicGUI.IconLayer.New(false)
    icon:SetPosition(self.Panel_icon:GetContentWidth()/2 ,self.Panel_icon:GetContentHeight()/2)
    self.Panel_icon:AddChild(icon:GetNode())

    local type_ , info_ = ObjIDTool.GetInfo(data.info_id)
    local iconData = {}
    if type_ ~= Macros.Game.ObjType.HERO and type_ ~= Macros.Game.ObjType.EQUIP and type_ ~= Macros.Game.ObjType.TREASURE then
       iconData = { id = data.info_id , num = data.item_num }
    else
        iconData = { id = data.info_id }
    end
    icon:SetData(iconData)

    --名字
    self.name = Util:Name(data.name, Macros.TypefaceSize.largish , data.quality)
    self.name:SetAnchorPoint(0, 0.5)
    self.name:SetPosition(10,self.Text_sellName:GetContentHeight()/2)
    self.Text_sellName:AddChild(self.name)

    --价钱
    local price = Util:MoneyControl(data.iconType , 0.8 , data.price , nil , Macros.Color.content , 10 )
    price:SetAnchorPoint(0.5,0.5)
    price:SetPosition(self.Text_price:GetContentWidth()/2 , self.Text_price:GetContentHeight()/2 )
    self.Text_price:AddChild(price)

    local lab = CPPGameLib.GetString("role_info_gold") ..  CPPGameLib.GetString("common_colon")
    if data.iconType == Macros.Game.RolePropType.TOWER_PRESTIGE then
        lab = CPPGameLib.GetString("role_info_tower_prestige") ..  CPPGameLib.GetString("common_colon")
    elseif data.iconType == Macros.Game.RolePropType.HERO_SOUL then
        lab = CPPGameLib.GetString("role_info_hero_soul") ..  CPPGameLib.GetString("common_colon")
    end

    local typeName = Util:Label(lab, nil , Macros.Color.keypoint )
    typeName:SetAnchorPoint(1,0.5)
    typeName:SetPosition( self.Text_price:GetContentWidth()/2 - price:GetContentWidth()/2 , self.Text_price:GetContentHeight()/2  )
    self.Text_price:AddChild(typeName)

    --选择框
    local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_select_no")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_select_yes")
    self.cbx = UICheckBoxEx.CreateWithImage(sp_1,sp_2)
    self.cbx:SetPosition(self.Panel_cbox:GetContentWidth()/2,self.Panel_cbox:GetContentHeight()/2)
    self.Panel_cbox:AddChild(self.cbx)
    self.cbx:SetEventListener(
        GlobalCallbackMgr:AddCallBackFunc(function()

            CallModuleFunc(ModuleType.DEBRISELL, "SetOnData", data , true )

        end),
        GlobalCallbackMgr:AddCallBackFunc(function()

            CallModuleFunc(ModuleType.DEBRISELL, "SetOnData", data , nil )

        end))    
    local info = CallModuleFunc(ModuleType.DEBRISELL, "GetOnData", data )
    self.cbx:SetSelectedState( info ~= nil )

    local type_ , info = ObjIDTool.GetInfo(data.info_id)
    if data.sellType == Macros.Game.SELECT_TYPE.EQUIP then
        --装备特色属性显示
        self:__EquipShowUI(data,info)
    elseif data.sellType == Macros.Game.SELECT_TYPE.HERO then
        --装备特色属性显示
        self:__HeroShowUI(data,info)
    elseif data.sellType == Macros.Game.SELECT_TYPE.HERO_DEBRIS then
        self:__HeroDebrisShowUI(data,info)
    elseif data.sellType == Macros.Game.SELECT_TYPE.EQUIP_DEBRIS then
        self:__EquipDebrisShowUI(data,info)
    end
end

--设置选中
function Modules.SaleItem:SetSelected( data )
    local info = CallModuleFunc(ModuleType.DEBRISELL, "GetOnData", data )
    self.cbx:SetSelectedState( info ~= nil )
end

------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--装备条目
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
function Modules.SaleItem:__EquipShowUI(data,info)
    local intensifyType = Util:Label(CPPGameLib.GetString("equip_intensifyLv") , nil , Macros.Color.keypoint )
    intensifyType:SetAnchorPoint(0,0.5)
    intensifyType:SetPosition(10 , self.Text_intensify:GetContentHeight() / 2 )
    self.Text_intensify:AddChild(intensifyType)

    local intensifyLv = Util:Label(data.strengthen_level , nil , Macros.Color.content)
    PosTool.RightTo(intensifyType , intensifyLv,5)
    self.Text_intensify:AddChild(intensifyLv)
end

------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--英雄条目
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
function Modules.SaleItem:__HeroShowUI(data,info)
    local intensifyType = Util:Label(CPPGameLib.GetString("common_lev1") , nil , Macros.Color.keypoint )
    intensifyType:SetAnchorPoint(0,0.5)
    intensifyType:SetPosition(10 , self.Text_intensify:GetContentHeight() / 2 )
    self.Text_intensify:AddChild(intensifyType)

    local intensifyLv = Util:Label(data.lv , nil , Macros.Color.content)
    PosTool.RightTo(intensifyType , intensifyLv,5)
    self.Text_intensify:AddChild(intensifyLv)
end

------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--英雄碎片条目
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
function Modules.SaleItem:__HeroDebrisShowUI(data,info)
    local info_ = CPPGameLib.GetConfig("HeroCompound", info.target_id)

    local numLab = Util:Label( CPPGameLib.GetString("pack_num") , nil , Macros.Color.keypoint )
    numLab:SetAnchorPoint(0,0.5)
    numLab:SetPosition(self.name:GetPositionX() , self.name:GetPositionY() - 50 )
    self.Text_sellName:AddChild(numLab)

    local numLabLv = Util:Label( data.item_num .." / " .. info_.item_num , nil , Macros.Color.content )
    PosTool.RightTo(numLab,numLabLv)
    self.Text_sellName:AddChild(numLabLv)
end
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--装备碎片条目
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
function Modules.SaleItem:__EquipDebrisShowUI(data,info)
    local info_ = CPPGameLib.GetConfig("EquipmentCompose", info.target_id)

    local numLab = Util:Label( CPPGameLib.GetString("pack_num") , nil , Macros.Color.keypoint )
    numLab:SetAnchorPoint(0,0.5)
    numLab:SetPosition(self.name:GetPositionX() , self.name:GetPositionY() - 50 )
    self.Text_sellName:AddChild(numLab)
    
    local numLabLv = Util:Label( data.item_num .." / " .. info_.item_num , nil , Macros.Color.content )
    PosTool.RightTo(numLab,numLabLv)
    self.Text_sellName:AddChild(numLabLv)
end