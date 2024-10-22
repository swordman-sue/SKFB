--
-- @brief 卡牌详情界面
-- @author: yjg
-- @date: 2017年11月16日14:47:18
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplCardInfo = LogicGUI.TemplCardInfo or BaseClass(GUI.Template)

function LogicGUI.TemplCardInfo:__init(data,fun)
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID , color = {0,0,0,125}} 
    --关闭界面事件
    self.__DeleteApproachViewEvent = self:BindGlobalEvent(Macros.Event.ApproachOfAchievingModule.DELETE_APPROACHVIEW, function()
        self:DeleteMe()
    end)
    self.__open_callback = function ()
        self.fun = fun
        self:__Getwidget()
        self:__ShowUI(data)
    end 
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_CARD_INFO)
end

function LogicGUI.TemplCardInfo:__delete()
    if self.__shield_layer then
        self.__shield_layer:DeleteMe()
        self.__shield_layer = nil
    end
end

function LogicGUI.TemplCardInfo:__Getwidget( ... )
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

function LogicGUI.TemplCardInfo:__ShowUI( data )
    local pageView = {
                [1] = { headline = CPPGameLib.GetString("divination_CardInfo") , headline_ = CPPGameLib.GetString("divination_CardInfo") },
                [2] = { headline = CPPGameLib.GetString("equip_ToGet") , headline_ = CPPGameLib.GetString("approach_titleText") }, 
    }

    for i , v in ipairs( pageView ) do
        v.data = data
        v.page_class = GUI.TemplCardInfoPage
    end

    self.PageView_heroDetailMessage:SetDataList( pageView )


    local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_select_no")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_select_yes")
    local layou = {}
    self.cbx = {}
    self.cbxText = {}
    for i=1 , #pageView do
        self.cbx[i] = UICheckBoxEx.CreateWithImage(sp_1,sp_2)
        self.cbx[i]:SetEventListener(
            GlobalCallbackMgr:AddCallBackFunc(function()
                self:Onbox(i)
            end)
        )  
        self.cbx[i]:SetSelectedState(false)

        self.cbx[1]:SetSelectedState(true)

        self.cbxText[i] = Util:Label( pageView[i].headline ,Macros.TypefaceSize.normal , Macros.Color.content )
        layou[i] = Util:Layout(self.cbx[i]:GetContentWidth() + self.cbxText[i]:GetContentWidth() + 5, self.cbx[i]:GetContentHeight() )
        self.cbxText[1]:SetColor( unpack(Macros.Color.keypoint) )

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
function LogicGUI.TemplCardInfo:Onbox(index)
    for i=1,#self.cbx do
        self.cbx[i]:SetSelectedState(false)
        self.cbxText[i]:SetColor( unpack( Macros.Color.content ) )
    end
    if self.cbx[index] then
        self.cbx[index]:SetSelectedState(true)
        self.cbxText[index]:SetColor( unpack( Macros.Color.keypoint ) )
        self.PageView_heroDetailMessage:ScrollToPage(index - 1)
    end
end

--取消
function LogicGUI.TemplCardInfo:Onbtn_no()
    self:DeleteMe()
    if self.fun then
        self.fun()
    end
end


--信息页面
GUI = GUI or {}

GUI.TemplCardInfoPage = GUI.TemplCardInfoPage or BaseClass(GUI.Template)

function GUI.TemplCardInfoPage:__init( )
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_EQUIP_INFO_PAGE)
end

function GUI.TemplCardInfoPage:__AddToParent()
    
end

function GUI.TemplCardInfoPage:SetData( allLst )
    self:__Getwidget( )
    self:__ShowUI( allLst )
end

function GUI.TemplCardInfoPage:__Getwidget()
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

function GUI.TemplCardInfoPage:__ShowUI( allLst)
    local data = allLst.data
    local uidata = allLst


    --标题
    local title1 = Util:Label( uidata.headline_ , Macros.TypefaceSize.popup ,Macros.Color.lilyWhite)
    title1:SetAnchorPoint(0.5,0.5)
    title1:SetPosition(self.Text_title1:GetContentWidth()/2 ,self.Text_title1:GetContentHeight()/2 )
    self.Text_title1:AddChild(title1)

    --图标
    local icon = LogicGUI.IconLayer.New(false)
    icon:SetPosition(self.Panel_heroChipIcon1:GetContentWidth()/2 , self.Panel_heroChipIcon1:GetContentHeight()/2) 
    icon:SetData({id = data.id})
    self.Panel_heroChipIcon1:AddChild(icon:GetNode())
    PosTool.Center(icon:GetNode())

    local type_ , info_ = ObjIDTool.GetInfo(data.id)
    --名字
    local name = Util:Name( info_.name , nil , info_.quality )
    name:SetAnchorPoint(0,0.5)
    name:SetPosition( 5 , self.Text_name1:GetContentHeight() / 2 )
    self.Text_name1:AddChild(name)

    -- --品级
    -- local taste = Util:Label( CPPGameLib.GetString("equip_grade_" , data.taste ) , Macros.TypefaceSize.largish, Macros.Color.keypoint )
    -- self.Text_name1:AddChild(taste)
    -- PosTool.RightTo( name , taste ,10 )

    --描述
    local describe = Util:Label(info_.description , Macros.TypefaceSize.normal ,Macros.Color.content, self.Image_1_0:GetContentWidth() - 10 )
    describe:SetAnchorPoint(0 , 1)
    describe:SetPosition( 5 , self.Text_des1:GetContentHeight() )
    self.Text_des1:AddChild(describe)

    CPPGameLib.PrintTable("xy" , uidata.headline_ , "uidata.headline_-------------")  


    if uidata.headline_ == CPPGameLib.GetString("divination_CardInfo") then
        self:Property( data )
    else
        self:Acquire( data )
    end
end

--属性
function GUI.TemplCardInfoPage:Property( data )
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
    local t1 = CallModuleFunc(ModuleType.DIVINATION, "ToInfoIdGetDivinationInfo" , data.id , 2 , 1 )

    --属性
    local function property( lst )
        local num = type(lst) == 'number' and 1 or #lst
        local layou = Util:Layout( self.Image_bg1:GetContentWidth() , 40 * num )
        local interval = layou:GetContentHeight() / num
        if type(lst) == 'number' then
            local front = ""
            local back = ""
            if lst == 1 then
                front = CPPGameLib.GetString("divination_to_exp_txt")
                back = t1.to_exp or 0
            elseif lst == 2 then
                front = CPPGameLib.GetString("divination_exp_could_no_levelup")
            elseif lst == 3 then
                front = CPPGameLib.GetString("divination_exp_could_no_breakup")
            end
            local basicType_ = Util:Label(front , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
            basicType_:SetAnchorPoint(0,0.5)
            basicType_:SetPosition(20 ,40/2)
            layou:AddChild(basicType_)
            local basicValue = Util:Label( back , Macros.TypefaceSize.slightly , Macros.Color.content)
            PosTool.RightTo(basicType_,basicValue , 5)
            layou:AddChild(basicValue)
        elseif type(lst) == 'table' then
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
        end
        return layou    
    end 

    --基础属性
    self.listView:PushBackCustomItem( headline(CPPGameLib.GetString("divination_basicsPro")) )

    --基础属性
    self.listView:PushBackCustomItem( property(t1.type2 == Macros.Game.CARD_QUALITY_TYPE.EXP and 1 or t1.base_pro) )

    --升级每级增加
    self.listView:PushBackCustomItem( headline(CPPGameLib.GetString("divination_upLevelPro")) )

    --升级每级增加
    self.listView:PushBackCustomItem( property(t1.type2 == Macros.Game.CARD_QUALITY_TYPE.EXP and 2 or t1.up_pro ) )

    --进阶每级增加
    self.listView:PushBackCustomItem( headline(CPPGameLib.GetString("divination_advancedPro")) )

    --进阶每级增加
    self.listView:PushBackCustomItem( property(t1.type2 == Macros.Game.CARD_QUALITY_TYPE.EXP and 3 or t1.break_pro ) )
end

--获取
function GUI.TemplCardInfoPage:Acquire( data )
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
    PosTool.Center(self.ScrollView , 7 ) 
    self.ScrollView:SetDataList( CallModuleFunc(ModuleType.APPROACH_OF_ACHIEVING,"HandleApproachOfAchievingData", data.id ) )
end