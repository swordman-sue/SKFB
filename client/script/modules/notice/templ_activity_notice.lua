--
-- @brief 活动公告
-- @author: yjg
-- @date: 2016年8月17日21:38:39
--

Modules = Modules or {}

Modules.TemplActivityNotice = Modules.TemplActivityNotice or BaseClass(GUI.Template)

function Modules.TemplActivityNotice:__init( id )
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID}   
    self.__open_callback = function()
        self:__Getwidget()
        self:__ShowUI( id )
    end

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.NOTICE)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.NOTICE,true)
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_ACTIVITY_NOTICE)  
end

function Modules.TemplActivityNotice:__Getwidget( ... )
    --标题
    self.PageView_headline = self:GetWidget("PageView_headline")

    --滑动层
    self.Panel_lst = self:GetWidget("Panel_lst")

    --内容
    self.Panel_content = self:GetWidget("Panel_content")

    --关闭按钮
    self.Button_close = Util:WidgetButton(  
                                            self:GetWidget("Button_close") , 
                                            CPPGameLib.GetString("notice_know") , 
                                            CPPGameLib.Handler(self,self.OnButtonClose) ,
                                            nil,Macros.Color.btn,Macros.Color.button_yellow
                                            ) 
end

function Modules.TemplActivityNotice:__ShowUI( id )
    --内容列表
    self.listView = UIListViewEx.Create()
    self.listView:SetDirection(UIListViewEx.ScrollDirVertical)
    self.listView:SetContentSize(self.Panel_content:GetContentWidth() - 5 ,self.Panel_content:GetContentHeight() )
    self.listView:SetAnchorPoint( 0.5 , 0.5 )
    self.listView:SetBounceEnabled(true)
    self.Panel_content:AddChild(self.listView)
    PosTool.Center( self.listView )

    --选择列表
    local params =  
    {
        item_class = Modules.TemplNoticeItem,
        item_width = self.Panel_lst:GetContentWidth() ,
        item_height = 72 ,
        row = 4,
        col = 1,
        item_space_r = 5,
        view_height = self.Panel_lst:GetContentHeight() - 5 ,
        selectable = true , 
        select_callback = function( index , itemData )
            self.listView:RemoveAllItems()
            --充值测试，应用宝公告特殊处理
            local content = itemData.context
            local lst = CPPGameLib.SplitEx(content, "<GH>")
            self:Cell(lst)
        end
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.Panel_lst:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode())

    local tabLst , iconLst = CallModuleFunc( ModuleType.NOTICE, "GetActivityNoticeData" )
    --标题
    for i , v in ipairs( iconLst ) do
        local layout = Util:Layout( self.PageView_headline:GetContentWidth() , self.PageView_headline:GetContentHeight() )
        layout:SetBackGroundImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.NOTICE, v , true) )
        --设置标题滑动
        self.PageView_headline:AddPage( layout )
    end
    WidgetUnity.SetPageViewListener(self.PageView_headline,CPPGameLib.Handler(self, function ( ... )
        local index = self.PageView_headline:GetCurPageIndex()
        if #iconLst ~= 1 then
            self:SetOnShowLst( index + 1 )
        end
    end ))

    self.lst_list:SetDataList( tabLst ) 

    self:SetOnShowLst()
end

--设置选中
function Modules.TemplActivityNotice:SetOnShowLst( index )
    self.lst_list:SelectItem( index or 1 )
end

function Modules.TemplActivityNotice:Cell( data )
    local list_view_width = self.listView:GetContentWidth()
    for i,v in ipairs(data) do
        local layout = Util:Layout(0,0)
        layout:SetAnchorPoint(0.5, 0.5)
        
        local lab = Util:RichText( v , Macros.TypefaceSize.normal ,  list_view_width ,0)
        layout:SetContentSize(list_view_width, lab:GetContentHeight() + 5 )
        lab:SetAnchorPoint(0,0.5)
        lab:SetPosition(0 , layout:GetContentHeight()/2)
        layout:AddChild(lab)
        local OnLinkClicked = function (x,y,link_type,user_data,str_url)
            CPPSdkTools.GotoBrowser( str_url )
        end  
        local link_func = GlobalCallbackMgr:AddCallBackFunc(OnLinkClicked)
        lab:SetLuaTouchLink(link_func)

        self.listView:PushBackCustomItem(layout)
    end

    local layout = Util:Layout(0,0)
    layout:SetAnchorPoint(0.5, 0.5)
    layout:SetContentSize(list_view_width, 30 )
    self.listView:PushBackCustomItem(layout)
end

function Modules.TemplActivityNotice:OnButtonClose( data )
    self:DeleteMe()
end

Modules = Modules or {}

Modules.TemplNoticeItem = Modules.TemplNoticeItem or BaseClass(GUI.ScrollItem)

function Modules.TemplNoticeItem:__init(data)
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplNoticeItem:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout( 243.00 , 72 )
    return self.layout
end

function Modules.TemplNoticeItem:SetData( data )
    GUI.ScrollItem.SetData(self, data)
    self:__Getwidget()
    self:__ShowUI( data )
end

function Modules.TemplNoticeItem:__Getwidget()
    local no_select = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.NOTICE, "no_select")
    self.btn = Util:Sprite( no_select )
    self.layout:AddChild( self.btn )
    PosTool.Center( self.btn )
end

function Modules.TemplNoticeItem:__ShowUI( data )
    --标题
    local headline = Util:Label( data.title , Macros.TypefaceSize.normal , Macros.Color.white ,  180 , 0 , Macros.Color.common_stroke )
    headline:SetAnchorPoint( 0  , 0.5 )
    self.btn:AddChild( headline )
    PosTool.Center( headline , -80 )
    headline:SetTextHorizontalAlign(LabelEx.ALIGIN_CENTER)

    if data.type then
        --状态
        local type_ = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.NOTICE, "type_" .. data.type , true)
        local type_sp = Util:Sprite( type_ )
        type_sp:SetAnchorPoint( 0 , 1 )
        type_sp:SetPosition( 5 , self.btn:GetContentHeight() )
        self.btn:AddChild( type_sp )
    end
end

function Modules.TemplNoticeItem:SetSelected( var )
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.NOTICE, "no_select")
    if var == true then
        sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.NOTICE, "on_select")
    end
    self.btn:SetImage( sp )
end