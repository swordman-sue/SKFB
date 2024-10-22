--
-- @brief 装备详情界面
-- @author: yjg
-- @date: 2016年8月17日20:18:01
--

TemplEquipInfo = TemplEquipInfo or BaseClass()

function TemplEquipInfo.GetInstance(data)
    if TemplEquipInfo.instance then
        TemplEquipInfo.instance:DeleteMe()
    end
    TemplEquipInfo.instance = LogicGUI.TemplEquipInfo.New(data,function ( ... )
        TemplEquipInfo.DelInstance()
    end)
    return TemplEquipInfo.instance
end

function TemplEquipInfo.DelInstance()
    if TemplEquipInfo.instance then
        TemplEquipInfo.instance:DeleteMe()
        TemplEquipInfo.instance = nil
    end
end

LogicGUI = LogicGUI or {}

LogicGUI.TemplEquipInfo = LogicGUI.TemplEquipInfo or BaseClass(GUI.Template)

function LogicGUI.TemplEquipInfo:__init(data,fun)
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID , color = {0,0,0,125}} 
    --关闭界面事件
    self.__DeleteApproachViewEvent = self:BindGlobalEvent(Macros.Event.ApproachOfAchievingModule.DELETE_APPROACHVIEW, function()
        self:DeleteMe()
    end)
    self.__open_callback = function ()
        self.fun = fun
        self:__Getwidget()
        self:__SettleData(data)
    end 
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_EQUIP_INFO)
end

function LogicGUI.TemplEquipInfo:__delete()
    if self.__shield_layer then
        self.__shield_layer:DeleteMe()
        self.__shield_layer = nil
    end
end

function LogicGUI.TemplEquipInfo:__Getwidget( ... )
    --背景容器
    self.Panel_heroDetailMessage = self:GetWidget("Panel_heroDetailMessage") 
    --背景
    self.Image_bg = self:GetWidget("Image_bg") 
    --关闭按钮
    self.Button_close =  Util:WidgetButton( self:GetWidget("Button_close") , nil , CPPGameLib.Handler(self,self.Onbtn_no) )
    self.Button_close:SetZOrder( 99 )
    
    -- --左箭头
    self.Image_left = self:GetWidget("Image_left")
    WidgetUnity.SetWidgetListener(self.Image_left, nil, nil,function ()
        self:Onbox(1)
    end)

    -- --右箭头
    self.Image_right = self:GetWidget("Image_right")
    WidgetUnity.SetWidgetListener(self.Image_right, nil, nil,function ()
        self:Onbox(2)
    end)

    local effect = nil
    self.effectid_1 ,effect = self:__CreateEffect({res_name = "UI_shangdianjiantou" } , self.Image_left , 999 )
    effect:SetAnchorPoint(0.5,0.5)
    effect:SetPosition( self.Image_left:GetContentWidth()/2 , self.Image_left:GetContentHeight()/2 )
    effect:SetScaleXFactor( -1 )

    local effect = nil
    self.effectid_2 ,effect = self:__CreateEffect({res_name = "UI_shangdianjiantou" } , self.Image_right , 999 )
    effect:SetAnchorPoint(0.5,0.5)
    effect:SetPosition( self.Image_right:GetContentWidth()/2 , self.Image_right:GetContentHeight()/2 )

    --翻页容器
    local PageView = self:GetWidget("PageView_heroDetailMessage")
    PageView:SetVisible(false)

    local Pvbg = Util:Layout( PageView:GetContentWidth() , PageView:GetContentHeight() )
    Pvbg:SetPosition( PageView:GetPositionX() , PageView:GetPositionY() )
    Pvbg:SetAnchorPoint( 0.5 , 0 )
    self.Panel_heroDetailMessage:AddChild( Pvbg )

    self.PageView_heroDetailMessage = GUI.PageView.New(
                                        {
                                            view_width = Pvbg:GetContentWidth(),
                                            view_height = Pvbg:GetContentHeight(),
                                            turn_page_func = function( ... )
                                                local index = self.PageView_heroDetailMessage:GetCurPageIndex()
                                                self:Onbox( index + 1 )
                                            end
                                        }
                                    )
    Pvbg:AddChild(self.PageView_heroDetailMessage:GetNode())
    PosTool.Center(self.PageView_heroDetailMessage)

    --页面
    self.Panel_layer = {}
    --头像
    self.Panel_heroChipIcon = {}
    --名字
    self.Text_name = {}
    --描述
    self.Text_des = {}
    --背景
    self.Image_bg = {}
    --标题
    self.Text_title = {}
    --数字文字
    self.Text_num = {}
    --数字
    self.Text_numCount = {}
    --光
    self.Image_12 = self:GetWidget("Image_12") 
end

function LogicGUI.TemplEquipInfo:__SettleData( data )

    local uid = nil
    if data.data and data.data.uid then
        uid = data.data.uid
    elseif data.uid then
        uid = data.uid
    end

    -- 获得要看的类型和信息
    local type_ , typeInfo =  nil , nil

    local itemid = 0
    if data.obj_info then
        type_ , typeInfo = ObjIDTool.GetInfo(data.obj_info.info_id)
        itemid = data.obj_info.info_id
    else
        type_ , typeInfo = ObjIDTool.GetInfo(data.id)
        itemid = data.id
    end

    local t1 = {}
    t1[1] = typeInfo
    t1[1].type_ = type_

    if type_ == Macros.Game.ObjType.ITEM then
        t1[1].info_id = itemid
        local type_ , debrisInfo_ = ObjIDTool.GetInfo(t1[1].target_id)
        t1[2] = debrisInfo_
        t1[2].type_ = type_
        t1[2].info_id = t1[1].target_id
        t1[2].uid = uid
    elseif type_ == Macros.Game.ObjType.EQUIP then
        local debrisInfo = CPPGameLib.GetConfig("EquipmentCompose", itemid)
        t1[1].info_id = itemid
        t1[1].uid = uid
        if debrisInfo then
            local type_ , debrisInfo_ = ObjIDTool.GetInfo(debrisInfo.item_id)
            if not debrisInfo_ then
                return 
            end
            t1[2] = debrisInfo_
            t1[2].type_ = type_
            t1[2].info_id = debrisInfo.item_id
        end
    end
    self:__ShowUI(t1)
end

function LogicGUI.TemplEquipInfo:__ShowUI( data )
    local pageView = {
                [1] = { page = GUI.TemplEquipDebrisPage , headline = CPPGameLib.GetString("equip_ToGet") , type_ = CPPGameLib.GetString("equip_ToGet")},
                [2] = { page = GUI.TemplEquipInfoPage , headline = CPPGameLib.GetString("equip_info") ,type_ = CPPGameLib.GetString("equip_SomeThing") },
    }

    if #data <= 1 then
        self.Image_left:SetVisible(false)
        self.Image_right:SetVisible(false)
    else
        self.Image_left:SetVisible(true)
        self.Image_right:SetVisible(true)
    end

    for i,v in ipairs(data) do
        v.page_class = pageView[v.type_].page
    end

    self.PageView_heroDetailMessage:SetDataList( data )


    local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_select_no")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_select_yes")
    local layou = {}
    self.cbx = {}
    self.cbxText = {}
    for i=1,#data do
        self.cbx[i] = UICheckBoxEx.CreateWithImage(sp_1,sp_2)
        self.cbx[i]:SetEventListener(
            GlobalCallbackMgr:AddCallBackFunc(function()
                self:Onbox(i)
            end)
        )  
        self.cbx[i]:SetSelectedState(false)

        self.cbx[1]:SetSelectedState(true)

        self.cbxText[i] = Util:Label( pageView[data[i].type_].type_ ,Macros.TypefaceSize.normal , Macros.Color.content )
        layou[i] = Util:Layout(self.cbx[i]:GetContentWidth() + self.cbxText[i]:GetContentWidth() + 5, self.cbx[i]:GetContentHeight() )
        self.cbxText[1]:SetColor( unpack( Macros.Color.keypoint ) )

        self.cbx[i]:SetAnchorPoint(0,0.5)
        self.cbx[i]:SetPosition( 0 , layou[i]:GetContentHeight()/2 )
        layou[i]:AddChild(self.cbx[i])

        self.cbxText[i]:SetAnchorPoint(0,0.5)
        self.cbxText[i]:SetPosition(self.cbx[i]:GetContentWidth() + 5 , layou[i]:GetContentHeight()/2)
        layou[i]:AddChild(self.cbxText[i]) 
    end
    local long = 0
    for i=1,#layou do
        long = long + layou[i]:GetContentWidth()
        if i == #layou then
            long = long + 20 
        end
    end

    local layer = Util:Layout( long , layou[1]:GetContentHeight() )
    layer:SetAnchorPoint(0.5,0.5)
    layer:SetPosition(self.Panel_heroDetailMessage:GetContentWidth()/2 , 20 )
    self.Panel_heroDetailMessage:AddChild(layer)
    for i=1,#layou do
        layou[i]:SetAnchorPoint(0,0.5)
        local length = 0
        if layou[i - 1] then
            length = layou[i - 1]:GetContentWidth() + 20 
        end
        layou[i]:SetPosition(0 + length, layer:GetContentHeight()/2 ) 
        layer:AddChild(layou[i])
    end

    -- --翻页监听
    -- local function pageViewEvent()
    --     local index = self.PageView_heroDetailMessage:GetCurPageIndex()
    --     self:Onbox( index + 1 )
    -- end
    -- WidgetUnity.SetPageViewListener(self.PageView_heroDetailMessage,pageViewEvent)
end

--选中
function LogicGUI.TemplEquipInfo:Onbox(index)
    for i=1,#self.cbx do
        self.cbx[i]:SetSelectedState(false)
        self.cbxText[i]:SetColor( unpack( Macros.Color.content ) )
    end
    self.cbx[index]:SetSelectedState(true)
    self.cbxText[index]:SetColor( unpack( Macros.Color.keypoint ) )

    self.PageView_heroDetailMessage:ScrollToPage(index - 1)
end

--取消
function LogicGUI.TemplEquipInfo:Onbtn_no()
    self:DeleteMe()
    if self.fun then
        self.fun()
    end
end

--碎片页面
GUI = GUI or {}

GUI.TemplEquipDebrisPage = GUI.TemplEquipDebrisPage or BaseClass(GUI.Template)

function GUI.TemplEquipDebrisPage:__init(data)
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_EQUIP_INFO_PAGE)
    -- self:__Getwidget(data)
    -- self:__ShowUI(data)  
end

function GUI.TemplEquipDebrisPage:__AddToParent()
end

function GUI.TemplEquipDebrisPage:SetData( data )
    self:__Getwidget( )
    self:__ShowUI( data )
end

function GUI.TemplEquipDebrisPage:__delete()
    if self.ScrollView then
        self.ScrollView:DeleteMe()
        self.ScrollView = nil
    end    
end

function GUI.TemplEquipDebrisPage:__Getwidget(data)
    --容器
    self.Panel_layer1 = self:GetWidget("Panel_layer1") 
    --图标
    self.Panel_heroChipIcon1 = self:GetWidget("Panel_heroChipIcon1") 
    --名字背景
    self.Image_1_0 = self:GetWidget("Image_1_0") 
    --名字
    self.Text_name1 = self:GetWidget("Text_name1") 
    --描述
    self.Text_des1 = self:GetWidget("Text_des1") 
    --背景
    self.Image_bg1 = self:GetWidget("Image_bg1") 
    --标题字
    self.Text_title1 = self:GetWidget("Text_title1") 
    --数量:字样
    self.Text_num1 = self:GetWidget("Text_num1")
    --数量 
    self.Text_numCount1 = self:GetWidget("Text_numCount1") 
end

function GUI.TemplEquipDebrisPage:__ShowUI(data)
    --标题
    local title1 = Util:Label( CPPGameLib.GetString("equip_debris"), Macros.TypefaceSize.popup ,Macros.Color.lilyWhite)
    title1:SetAnchorPoint(0.5,0.5)
    title1:SetPosition(self.Text_title1:GetContentWidth()/2 ,self.Text_title1:GetContentHeight()/2 )
    self.Text_title1:AddChild(title1)

    --图标
    local icon = LogicGUI.IconLayer.New(false)
    icon:SetPosition(self.Panel_heroChipIcon1:GetContentWidth()/2 , self.Panel_heroChipIcon1:GetContentHeight()/2) 
    icon:SetData({id = data.info_id})
    self.Panel_heroChipIcon1:AddChild(icon:GetNode())
    PosTool.Center(icon:GetNode())

    --名字
    local name = Util:Name(data.name , nil , data.quality )
    name:SetAnchorPoint(0,0.5)
    name:SetPosition( 5 , self.Text_name1:GetContentHeight() / 2 )
    self.Text_name1:AddChild(name)

    --描述
    local describe = Util:Label(data.description , Macros.TypefaceSize.normal ,Macros.Color.content, self.Image_1_0:GetContentWidth() - 15 )
    describe:SetAnchorPoint(0 , 1)
    describe:SetPosition(5,self.Text_des1:GetContentHeight())
    self.Text_des1:AddChild(describe)

    local equipment = CPPGameLib.GetConfig("EquipmentCompose", data.target_id)
    if not equipment then
        return
    end

    local numText = Util:Label( CPPGameLib.GetString("pack_num") ,Macros.TypefaceSize.normal ,  Macros.Color.keypoint )
    numText:SetAnchorPoint(1,0.5)
    numText:SetPosition(self.Text_num1:GetContentWidth() , self.Text_num1:GetContentHeight()/2 )
    self.Text_num1:AddChild(numText)

    local num = Util:Label( CallModuleFunc(ModuleType.Pack,"GetNumByItemID",data.info_id) .."/"..equipment.item_num , Macros.TypefaceSize.normal , Macros.Color.content )
    num:SetAnchorPoint(0,0.5)
    num:SetPosition(self.Text_numCount1:GetContentWidth() , self.Text_numCount1:GetContentHeight()/2 )
    self.Text_numCount1:AddChild(num)

    local params = {
        item_class  = Modules.TemplApproachOfAchievingCell,
        item_width  = 530,
        item_height = 80,
        row = 3,
        col = 1,
        item_space_r = 5,
        view_width = self.Image_bg1:GetContentWidth() ,
        view_height = self.Image_bg1:GetContentHeight() - 10 ,
    }
    self.ScrollView = GUI.ScrollView.New(params)
    self.Image_bg1:AddChild(self.ScrollView:GetNode())
    PosTool.Center(self.ScrollView) 
    self.ScrollView:SetDataList( CallModuleFunc(ModuleType.APPROACH_OF_ACHIEVING,"HandleApproachOfAchievingData",data.info_id) )
end


--信息页面
GUI = GUI or {}

GUI.TemplEquipInfoPage = GUI.TemplEquipInfoPage or BaseClass(GUI.Template)

function GUI.TemplEquipInfoPage:__init(  )
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_EQUIP_INFO_PAGE)
    -- self:__Getwidget(data)
    -- self:__ShowUI(data)
end

function GUI.TemplEquipInfoPage:__AddToParent()
    
end

function GUI.TemplEquipInfoPage:SetData( data )
    self:__Getwidget( )
    self:__ShowUI( data )
end

function GUI.TemplEquipInfoPage:__Getwidget(data)
    --容器
    self.Panel_layer1 = self:GetWidget("Panel_layer1") 
    --图标
    self.Panel_heroChipIcon1 = self:GetWidget("Panel_heroChipIcon1") 
    --名字背景
    self.Image_1_0 = self:GetWidget("Image_1_0") 
    --名字
    self.Text_name1 = self:GetWidget("Text_name1") 
    --描述
    self.Text_des1 = self:GetWidget("Text_des1") 
    self.Text_des1:SetPosition( self.Text_des1:GetPositionX() , self.Text_des1:GetPositionY() + 5 )
    --背景
    self.Image_bg1 = self:GetWidget("Image_bg1") 
    --标题字
    self.Text_title1 = self:GetWidget("Text_title1") 
    --数量:字样
    self.Text_num1 = self:GetWidget("Text_num1")
    --数量 
    self.Text_numCount1 = self:GetWidget("Text_numCount1") 
end

function GUI.TemplEquipInfoPage:__ShowUI(data)

    --标题
    local title1 = Util:Label( CPPGameLib.GetString("equip_info"), Macros.TypefaceSize.popup ,Macros.Color.lilyWhite)
    title1:SetAnchorPoint(0.5,0.5)
    title1:SetPosition(self.Text_title1:GetContentWidth()/2 ,self.Text_title1:GetContentHeight()/2 )
    self.Text_title1:AddChild(title1)

    --图标
    local icon = LogicGUI.IconLayer.New(false)
    icon:SetPosition(self.Panel_heroChipIcon1:GetContentWidth()/2 , self.Panel_heroChipIcon1:GetContentHeight()/2) 
    icon:SetData({id = data.info_id})
    self.Panel_heroChipIcon1:AddChild(icon:GetNode())
    PosTool.Center(icon:GetNode())

    --名字
    local name = Util:Name(data.name , nil , data.quality )
    name:SetAnchorPoint(0,0.5)
    name:SetPosition( 5 , self.Text_name1:GetContentHeight() / 2 )
    self.Text_name1:AddChild(name)

    --名字
    local taste = Util:Label( CPPGameLib.GetString("equip_grade_" , data.taste ) , Macros.TypefaceSize.largish, Macros.Color.keypoint )
    self.Text_name1:AddChild(taste)
    PosTool.RightTo( name , taste ,10 )

    --描述
    local describe = Util:Label(data.description , Macros.TypefaceSize.normal , Macros.Color.content , self.Image_1_0:GetContentWidth() - 10 )
    describe:SetAnchorPoint(0 , 1)
    describe:SetPosition( 5 , self.Text_des1:GetContentHeight() )
    self.Text_des1:AddChild(describe)

    --获取按钮
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_achieving",true)
    local btn = Util:Button( sp , CPPGameLib.Handler(self,self.Onbtn_no) )
    btn:SetAnchorPoint(1,0.5)
    btn:SetVisible(false)
    btn:SetPosition(self.Image_bg1:GetContentWidth() , self.Text_name1:GetPositionY())
    self.Panel_layer1:AddChild(btn)

    --列表容器
    self.listView = UIListViewEx.Create()
    self.listView:SetAnchorPoint(0.5,0.5)
    self.listView:SetContentSize(self.Image_bg1:GetContentWidth() ,self.Image_bg1:GetContentHeight() - 10 )
    self.Image_bg1:AddChild(self.listView)
    self.listView:SetTouchSwallow(false)
    PosTool.Center(self.listView)

    --标题
    local function headline( text )
        local layou = Util:Layout( self.Image_bg1:GetContentWidth() , 45  )

        --基本属性sp
        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp_headline")
        local basic = Util:Sprite(sp)
        basic:SetAnchorPoint( 0 , 1 )
        basic:SetPosition( 0 , layou:GetContentHeight() - 5 )
        layou:AddChild(basic)

       --基本属性
        local basicValue = Util:Label( text )
        basicValue:SetAnchorPoint(0,0.5)
        basicValue:SetPosition( 10 ,basic:GetContentHeight()/2)
        basic:AddChild(basicValue)

        return layou    
    end

    --属性
    local function property( lst )
        local layou = Util:Layout( self.Image_bg1:GetContentWidth() , 40 * #lst )
        local interval = layou:GetContentHeight() / #lst

        for i,v in ipairs( lst ) do
            --属性
            local type_ , value = Util:Nature(v.type , v.value , true )

            local basicType_ = Util:Label( type_ ..  CPPGameLib.GetString("common_colon"), Macros.TypefaceSize.slightly , Macros.Color.keypoint )
            basicType_:SetAnchorPoint(0,0.5)
            basicType_:SetPosition(20 , ( 40/2 ) + interval * (i - 1)  )
            layou:AddChild(basicType_)

            local basicValue = Util:Label( value , Macros.TypefaceSize.slightly , Macros.Color.content)
            PosTool.RightTo(basicType_,basicValue , 5)
            layou:AddChild(basicValue)
        end

        return layou    
    end 

    local t1 = CallModuleFunc(ModuleType.EQUIP, "ToInfoIdGetEquipProperty" , data.info_id )

    --强化
    self.listView:PushBackCustomItem( headline(CPPGameLib.GetString("equip_intensifyProperty")) )

    --强化属性
    self.listView:PushBackCustomItem( property(t1[1]) )

    --每级强化增加
    self.listView:PushBackCustomItem( headline(CPPGameLib.GetString("equip_intensifyProperty_1")) )

    local t2 = {}
    table.insert( t2 , {type = t1[1][1].type , value = data.strengthen_add_pro } )
    --精炼属性
    self.listView:PushBackCustomItem( property(t2) )

    local equipLst , natureLst , suitList = CallModuleFunc(ModuleType.EQUIP, "GetSuit", nil , data)
    if equipLst then
        --套装
        self.listView:PushBackCustomItem( headline(CPPGameLib.GetString("equip_suit")) )
        --套装属性
        local layer = Modules.TemplAllSuitItem.New(data)
        self.listView:PushBackCustomItem( layer:GetNode() )
    end

    local level = 1
    if data.uid then
        local info = CallModuleFunc(ModuleType.EQUIP, "GetEquip", data.uid )
        level = info.strengthen_level
    end

    local bg_sp9_resource = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_resource")
    --属性
    local function property_( lst )
        CPPGameLib.PrintTable("xy" , lst ,"lst")

        local layou = Util:Layout( self.Image_bg1:GetContentWidth() , 35  )
        local interval = layou:GetContentHeight() / 1

        local name_sp = Util:Sprite9( bg_sp9_resource )
        name_sp:SetAnchorPoint( 0 , 0.5 )
        name_sp:SetContentSize( 95 , 30 )
        name_sp:SetPosition( 50 , layou:GetContentHeight()/2 )
        layou:AddChild(name_sp)

        local nature_sp = Util:Sprite9( bg_sp9_resource )
        nature_sp:SetAnchorPoint( 0 , 0.5 )
        nature_sp:SetContentSize( 325 , 30 )
        PosTool.RightTo(name_sp,nature_sp , 10 )
        layou:AddChild(nature_sp)

        local basicType_ = Util:Label( lst.name , Macros.TypefaceSize.slightly , Macros.Color.content )
        basicType_:SetAnchorPoint(0.5 , 0.5)
        basicType_:SetPosition(name_sp:GetContentWidth()/2 , name_sp:GetContentHeight()/2  )
        name_sp:AddChild(basicType_)

        local text = ""
        if level < lst.intensify_level then
           text = CPPGameLib.GetString("equip_talent_tips_2" , lst.intensify_level )
        end

        local basicTips = Util:Label( text , Macros.TypefaceSize.slightly , Macros.Color.headline)
        basicTips:SetAnchorPoint( 0 , 0.5)
        basicTips:SetPosition( 10 , nature_sp:GetContentHeight()/2  )
        nature_sp:AddChild(basicTips)

        local basicValue = Util:Label( lst.nature_describe , Macros.TypefaceSize.slightly , Macros.Color.content)
        nature_sp:AddChild(basicValue)
        PosTool.RightTo( basicTips , basicValue )

        return layou    
    end 

    --强化天赋属性
    local equipInfo = CPPGameLib.GetConfig("Equipment", data.info_id)
    if equipInfo.talent then
        --强化天赋
        self.listView:PushBackCustomItem( headline(CPPGameLib.GetString("equip_talent_")) )
        local lst = CPPGameLib.GetConfig("EquipmentTalent", equipInfo.talent )
        table.sort( lst.level_list , function ( a , b )
            return a.talent_level < b.talent_level
        end )

        for i , v in ipairs( lst.level_list ) do
            self.listView:PushBackCustomItem( property_(v) )
        end
    end

end


function GUI.TemplEquipInfoPage:Onbtn_no( ... )

end



Modules = Modules or {}
Modules.TemplAllSuitItem = Modules.TemplAllSuitItem or BaseClass(GUI.Template)

function Modules.TemplAllSuitItem:__init(data)
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_EQUIP_DETAIY_ITEM_PROPERTY)
    self:__Getwidget(data)
    self:__ShowUI(data)
end

function Modules.TemplAllSuitItem:__AddToParent()
end

function Modules.TemplAllSuitItem:__delete()
end

function Modules.TemplAllSuitItem:__Getwidget()
    --背景容器
    self.Panel_vessel = self:GetWidget("Panel_vessel") 
    --属性类型1
    self.Image_result1 = self:GetWidget("Image_result1") 
    --属性类型2
    self.Image_result2 = self:GetWidget("Image_result2") 
    --属性类型3
    self.Image_result3 = self:GetWidget("Image_result3") 
    --属性值1
    self.Image_value1 = self:GetWidget("Image_value1") 
    --属性值2
    self.Image_value2 = self:GetWidget("Image_value2") 
    --属性值3
    self.Image_value3 = self:GetWidget("Image_value3") 
    --套装名
    self.Text_name = self:GetWidget("Text_name") 
    self.Text_name:SetPosition(self.Text_name:GetPositionX() , self.Text_name:GetPositionY() + 10 )
    --图标框
    self.Panel_iconlst = self:GetWidget("Panel_iconlst") 
end

function Modules.TemplAllSuitItem:__ShowUI(data)

    local equipLst , natureLst , suitList = CallModuleFunc(ModuleType.EQUIP, "GetSuit", nil , data)
    if not equipLst then
        print("xy" , "=================----=-=")
        return
    end
    local info = CPPGameLib.GetConfig("EquipmentSuit", equipLst[1].suit_id)
    if not info then
        return
    end
    --名字
    local name = Util:Name(info.name , nil , data.quality )
    name:SetPosition(self.Text_name:GetContentWidth()/2 ,self.Text_name:GetContentHeight()/2 )
    name:SetAnchorPoint(0.5,0.5)
    self.Text_name:AddChild(name)

    --线
    local shortlineSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "shortline")

    --左边
    local shortline_1 = Util:Sprite9( shortlineSp , {50,10} )
    shortline_1:SetContentSize(shortline_1:GetContentWidth()/2 , shortline_1:GetContentHeight())
    shortline_1:SetAnchorPoint( 1 , 0.5 )
    shortline_1:SetPosition( name:GetPositionX() - (name:GetContentWidth()/2) - 10 , self.Text_name:GetContentHeight()/2 )
    self.Text_name:AddChild(shortline_1)

    --左边
    local shortline_2 = Util:Sprite9( shortlineSp , {50,10})
    shortline_2:SetContentSize(shortline_2:GetContentWidth()/2 , shortline_2:GetContentHeight())
    shortline_2:SetAnchorPoint( 1 , 0.5 )
    shortline_2:SetFlipX(true)
    shortline_2:SetPosition( name:GetPositionX() + (name:GetContentWidth()/2) + 10 , self.Text_name:GetContentHeight()/2 )
    self.Text_name:AddChild(shortline_2)

    --居中用
    local layer = Util:Layout(90 * #equipLst , 76 )
    layer:SetAnchorPoint(0.5,0.5)
    layer:SetPosition(self.Panel_iconlst:GetContentWidth()/2 , self.Panel_iconlst:GetContentHeight()/2 + 15)
    self.Panel_iconlst:AddChild(layer)

    local interval = layer:GetContentWidth() / #equipLst
    --图标
    for i,v in ipairs(equipLst) do
        local icon_ = LogicGUI.IconLayer.New(true)
        icon_:SetPosition( 45 + interval * (i-1) ,layer:GetContentHeight()/2 ) 
        icon_:SetAnchorPoint(0.5,0.5)
        icon_:SetData({id = v.id})
        -- icon_:SetScaleFactor(0.85)
        layer:AddChild(icon_:GetNode())

        local name_ = Util:Name(v.name , nil , data.quality )
        name_:SetAnchorPoint(0.5, 1 )
        name_:SetPosition(icon_:GetNode():GetContentWidth()/2 , -10 )
        icon_:GetNode():AddChild(name_)
    end

    local hold = 0
    for i,v in ipairs(suitList) do
        local num = CallModuleFunc(ModuleType.EQUIP, "GetNumByInfoID", v)
        if num ~= 0 then
            hold = hold + 1
        end
    end

    --效果
    for i,lst in ipairs(natureLst) do
        local lab = ""
        for __,v in ipairs(lst) do
            lab = lab .. "  " .. Util:Nature(v.type , v.value)
        end

        local color = Macros.Color.keypoint
        if hold > i then
            color = Macros.Color.red_color
        end

        --套装效果名
        local type_ = Util:Label( CPPGameLib.GetString("equip_JS"..i) , nil , color )
        type_:SetAnchorPoint(0.5 , 0.5)
        type_:SetPosition(self["Image_result"..i]:GetContentWidth()/2 , self["Image_result"..i]:GetContentHeight()/2  )
        self["Image_result"..i]:AddChild(type_)

        local image_value = Util:Label(lab , nil , color )
        image_value:SetAnchorPoint(0,0.5)
        image_value:SetPosition(0,self["Image_value"..i]:GetContentHeight()/2)
        self["Image_value"..i]:AddChild(image_value)
    end
end