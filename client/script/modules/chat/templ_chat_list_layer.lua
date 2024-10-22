--
-- @brief 聊天界面(主界面)
-- @author: yjg
-- @date: 2017年5月15日16:02:39
--

Modules = Modules or {}

Modules.TemplChatListLayer = Modules.TemplChatListLayer or BaseClass(UILayer)

function Modules.TemplChatListLayer:__init(node)
    local params =  
    {
        item_class = Modules.TemplChatListItem,
        item_width = 638.00 ,
        item_height = 105 ,
        row = 4,
        col = 1,
        view_height = node:GetContentHeight() - 10 ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    node:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode())
end

function Modules.TemplChatListLayer:__delete()
    if self.lst_list then
        self.lst_list:DeleteMe()
        self.lst_list = nil 
    end
end

function Modules.TemplChatListLayer:__ShowType( index )
    local info = {}

    if index == Macros.Game.CHAT.PRIVATE then                  --私聊
        info = CallModuleFunc(ModuleType.CHAT , "GetWhisperLst")
    elseif index == Macros.Game.CHAT.WORLD then                --世界
        info = CallModuleFunc(ModuleType.CHAT , "GetWorldLst")
    elseif index == Macros.Game.CHAT.UNION then                --公会
        info = CallModuleFunc(ModuleType.CHAT , "GetGroupLst")
    elseif index == Macros.Game.CHAT.TEAM then                 --队伍
        info = CallModuleFunc(ModuleType.CHAT , "GetTeamLst")
    end
    self:SetCell(info)
end

--设置细胞
function Modules.TemplChatListLayer:SetCell( info )
    self.lst_list:SetDataList( info ) 
    self.lst_list:LocateToLast()
end

--新信息插入
function Modules.TemplChatListLayer:NewCell( info )
    self.lst_list:AttachItem(info)
    self.lst_list:LocateToLast()
end


LogicGUI = LogicGUI or {}

Modules.TemplChatListItem = Modules.TemplChatListItem or BaseClass(GUI.ScrollItem)

function Modules.TemplChatListItem:__init(data)
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplChatListItem:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(637.00 , 115 )
    return self.layout
end

function Modules.TemplChatListItem:SetData( data )
    GUI.ScrollItem.SetData(self, data)

    self:__Getwidget()
    self:__ShowUI()
end

function Modules.TemplChatListItem:__Getwidget()
    --头像
    self.img_head = LogicGUI.RoleIcon.New(true)
    self.img_head:SetTouchCallback(function()
        if self.__data.who == "he" then
            GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.CHECKROLEINFO , false , self.__data )
        else
            GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("head_click") )
        end
    end)
    self.img_head:SetTouchSwallow(false)
    self.img_head:SetData(self.__data.head_icon)
    self.img_head:SetAnchorPoint(0.5,0.5)
    self.img_head:SetPosition(self.layout:GetContentWidth()/2 , self.layout:GetContentHeight()/2 )
    self.layout:AddChild(self.img_head:GetNode())

    local color = nil
    if self.__data.chat_type == Macros.Game.CHAT.PRIVATE then
        color = Macros.Color.pm
    elseif self.__data.chat_type == Macros.Game.CHAT.WORLD then
        color = Macros.Color.golden
    elseif self.__data.chat_type == Macros.Game.CHAT.UNION then
        color = Macros.Color.union
    end

    --类型
    local pingdao_show_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHAT , "pingdao_show_"..self.__data.chat_type , true)
    self.type_ = Util:Sprite(pingdao_show_sp)
    self.type_:SetAnchorPoint(0.5,0.5)
    self.type_:SetPosition(self.layout:GetContentWidth()/2 , self.layout:GetContentHeight()/2)
    self.layout:AddChild(self.type_) 

    --v字图标
    local img_name = CPPSdkTools.GetCPChannelType() == Macros.SDK.CPChannelType.NNS_BT and "gm_title_icon" or "vip_title_icon"
    self.v_icon = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, img_name, true )  )
    self.v_icon:SetScaleFactor( 0.8 )
   
    --vip等级
    local vip_or_gm_lev = GlobalCustomizedLogicAdapterMgr:Call("GetChatVIPOrGMLevOfPlayer", self.__data)
    self.power_font = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "vipnum", true))
    self.power_font:SetText(tostring(vip_or_gm_lev))

    local layout = Util:Layout( (self.v_icon:GetContentWidth() * 0.8) + self.power_font:GetNode():GetContentWidth() + 2 , self.v_icon:GetContentHeight() )
    layout:SetAnchorPoint( 0.5 , 0.5 )
    layout:SetPosition( self.img_head:GetNode():GetContentWidth()/2 , 0  )
    self.img_head:AddChild(layout) 

    self.v_icon:SetAnchorPoint( 0 , 0.5)
    self.v_icon:SetPosition( 0 , layout:GetContentHeight()/2 )
    layout:AddChild(self.v_icon) 

    self.power_font:SetAnchorPoint(0,0.5)
    self.power_font:SetPosition( self.v_icon:GetPositionX() + ( self.v_icon:GetContentWidth() * 0.8 ) + 2 , layout:GetContentHeight()/2 )
    layout:AddChild(self.power_font:GetNode())  

    --称号
    if self.__data.cur_use_title_id ~= 0 then
        local cnf = CPPGameLib.GetConfig("Title", self.__data.cur_use_title_id )
        if not cnf then
            return 
        end
        if cnf.type == Macros.Game.TITLE_TYPE.PREFIX then
            self.title = Util:Sprite( Resource.PathTool.GetTitleIconPath( "sp_prefix_"..self.__data.cur_use_title_id , true ) )
            self.title:SetAnchorPoint( 0.5 , 0.5 )
            self.layout:AddChild(self.title) 
        end 
    end

    local  name = self.__data.role_name
    if self.__data.chat_type == Macros.Game.CHAT.TEAM then
        name = "S" .. self.__data.server_id .. "." .. self.__data.role_name
    end

    --名字
    self.name = Util:Label( name , Macros.TypefaceSize.slightly , Macros.Color.headline)
    self.name:SetAnchorPoint(0.5,0.5)
    self.name:SetPosition(self.layout:GetContentWidth()/2 , self.layout:GetContentHeight()/2)
    self.layout:AddChild(self.name) 

    local chat_text = CallModuleFunc(ModuleType.CHAT, "GetChatTextWithSmile", self.__data)
    local tab = Util:RichText( string.format(Macros.Color.content_rt , chat_text or " " ) , Macros.TypefaceSize.slightly , 300 , -1 )
    --对话框
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHAT, "lt_import")

    local lab = Util:RichText( string.format(Macros.Color.content_rt , chat_text or " " ) , Macros.TypefaceSize.slightly)

    local width = lab:GetContentWidth()
    self.y = 15
    if lab:GetContentWidth() >= tab:GetContentWidth() then
        width = tab:GetContentWidth()
        self.y = 0
    end

    self.item = Util:Sprite9(sp)
    self.item:SetContentSize(width + 20 , tab:GetContentHeight() + 20)
    self.item:SetAnchorPoint(0.5,1)
    self.item:SetPosition(self.layout:GetContentWidth()/2 , self.layout:GetContentHeight()/2 )
    -- self.item:SetZOrder(999)
    self.layout:AddChild(self.item) 

    --对白位置设置
    tab:SetAnchorPoint(0 , 0.5)
    tab:SetPosition( 10 , self.item:GetContentHeight()/2)
    self.item:AddChild(tab)    

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHAT, "corner")
    self.jt = Util:Sprite(sp)
    self.jt:SetAnchorPoint(0.5,0.5)
    self.jt:SetPosition(self.layout:GetContentWidth()/2 , self.layout:GetContentHeight()/2)

    self.layout:AddChild(self.jt)  
    self:__ShowUI()
end
  
function Modules.TemplChatListItem:__ShowUI()
    if self.__data.who == "he" then
        --头像
        self.img_head:SetPosition( 
                                    0 + (self.img_head:GetNode():GetContentWidth()) - 15 , 
                                    self.layout:GetContentHeight()/2 
                                )
        --频道
        self.type_:SetPosition( 
                                    self.img_head:GetNode():GetPositionX() + 
                                    (self.img_head:GetNode():GetContentWidth()/2) + 
                                    (self.type_:GetContentWidth()/2) + 15 ,

                                    self.img_head:GetNode():GetPositionY() + 
                                    (self.img_head:GetNode():GetContentHeight()/2) -
                                    (self.type_:GetContentHeight()/2) + 3
                                )

        --名字        
        self.name:SetPosition( 
                                    self.type_:GetPositionX() + 
                                    (self.type_:GetContentWidth()/2) + 
                                    (self.name:GetContentWidth()/2) + 10 ,   

                                    self.type_:GetPositionY()
                                )
        --称号
        if self.title then
            self.title:SetPosition( 
                                        self.name:GetPositionX() + 
                                        (self.name:GetContentWidth()/2) + 
                                        (self.title:GetContentWidth()/2) + 5 ,   

                                        self.name:GetPositionY()
                                    )
        end

        self.item:SetPosition( 
                                    self.img_head:GetNode():GetPositionX() + 
                                    (self.img_head:GetNode():GetContentWidth()/2) +  
                                    (self.item:GetContentWidth()/2) + 20 , 

                                    self.item:GetContentHeight() + 10 + self.y
                                    )
        self.jt:SetPosition( 
                                self.img_head:GetNode():GetPositionX() + 
                                (self.img_head:GetNode():GetContentWidth()/2) + 
                                (self.jt:GetContentWidth()/2) + 9.5 , 

                                self.item:GetContentHeight() - 5 + self.y
                            )
        if self.__data.vip_level <= 0 then
            self.v_icon:SetVisible(false)
            self.power_font:SetVisible(false)
        end

    elseif self.__data.who == "me" then

        --头像
        self.img_head:SetPosition( 
                                    self.layout:GetContentWidth() - 
                                    ( self.img_head:GetNode():GetContentWidth() - 15 ) , 
                                    self.layout:GetContentHeight()/2 
                                )
        --频道
        self.type_:SetPosition( 
                                    self.img_head:GetNode():GetPositionX() - 
                                    (self.img_head:GetNode():GetContentWidth()/2) - 
                                    (self.type_:GetContentWidth()/2) - 15 ,

                                    self.img_head:GetNode():GetPositionY() + 
                                    (self.img_head:GetNode():GetContentHeight()/2) -
                                    (self.type_:GetContentHeight()/2) + 3
                                )
        --名字        
        self.name:SetPosition( 
                                    self.type_:GetPositionX() - 
                                    (self.type_:GetContentWidth()/2) - 
                                    (self.name:GetContentWidth()/2) - 10 ,   

                                    self.type_:GetPositionY()
                                )
        --称号
        if self.title then
            self.title:SetPosition( 
                                        self.name:GetPositionX() - 
                                        (self.name:GetContentWidth()/2) - 
                                        (self.title:GetContentWidth()/2) - 5 ,   

                                        self.name:GetPositionY()
                                    )
        end

        --对话框
        self.item:SetPosition( 
                                    self.img_head:GetNode():GetPositionX() - 
                                    (self.img_head:GetNode():GetContentWidth()/2) -  
                                    (self.item:GetContentWidth()/2) - 20 , 

                                    self.item:GetContentHeight() + 10 +  self.y
                                    )

        self.jt:SetPosition( 
                                self.img_head:GetNode():GetPositionX() - 
                                (self.img_head:GetNode():GetContentWidth()/2) - 
                                (self.jt:GetContentWidth()/2) - 9.5 , 

                                self.item:GetContentHeight() - 5 + self.y
                            )
        self.jt:SetFlipX(true)

        if self.__data.vip_level <= 0 then
            self.v_icon:SetVisible(false)
            self.power_font:SetVisible(false)
        end
    end
end 
