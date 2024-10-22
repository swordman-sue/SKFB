--
-- @brief 公会搜索
-- @author: yjg
-- @date: 2016年11月21日16:52:23
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplUnionJoin = LogicGUI.TemplUnionJoin or BaseClass(GUI.Template)

function LogicGUI.TemplUnionJoin:__init( func )
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
    self.__open_callback = function()
        self.func = func
        self:__Getwidget()
        self:__ShowUI()
        self:__Server() 
    end
    
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.UNION)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.UNION,true)

    self:__AddPreloadTemplate(GUI.TemplateRegistry.UNION_JOIN_ITEM) 
    self:InitTemplate(GUI.TemplateRegistry.UNION_JOIN)  
end

function LogicGUI.TemplUnionJoin:__delete()
end

function LogicGUI.TemplUnionJoin:__Server()
    CallModuleFunc(ModuleType.UNION , "GuildListReq")
    --公会列表
    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_JOIN , function ( _, data , type_ )
        if not type_ then
            self:SetShowUI( data )
        else
            self:UpShowUI( data , 1)
        end
    end)

    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_ESTABLISH , function ( _, data , type_ )
        self:DeleteMe()
    end)

    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_APPLY , function ( _, data , type_ )
        self:UpShowUI( data , 0)
    end)
end

function LogicGUI.TemplUnionJoin:__Getwidget()
    --背景
    self.Image_Bg = self:GetWidget("Image_Bg")
    --内容框
    self.Image_1 = self:GetWidget("Image_1")
    --光
    self.Image_light = self:GetWidget("Image_light")
    --关闭
    self.Button_close = Util:WidgetButton( 
                                            self:GetWidget("Button_close") , "" , 
                                            CPPGameLib.Handler(self,self.OnBtnClose) 
                                        )
    --输入框背景
    self.Image_import = self:GetWidget("Image_import")
    --输入框
    self.TextField_1 = self:GetWidget("TextField_1")
    -- self.TextField_1:SetAnchorPoint( 0,0.5 )

    --搜索
    self.Button_seek = Util:WidgetButton( 
                                            self:GetWidget("Button_seek") , 
                                            CPPGameLib.GetString("union_seek") , 
                                            CPPGameLib.Handler(self,self.OnButtonSeek) ,
                                            nil,nil,Macros.Color.button_yellow
                                        )
    --创建
    self.Button_establish = Util:WidgetButton( 
                                                self:GetWidget("Button_establish") , 
                                                CPPGameLib.GetString("union_establish") , 
                                                CPPGameLib.Handler(self,self.OnButtonEstablish) ,
                                                nil,nil,Macros.Color.button_yellow
                                            )
end

function LogicGUI.TemplUnionJoin:__ShowUI()
    local name = Util:Label(CPPGameLib.GetString("union_lst") , Macros.TypefaceSize.popup , Macros.Color.white)
    name:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2)
    self.Image_light:AddChild(name)

    --输入框
    self.TextField_1:SetMaxLength(10)
    self.TextField_1:SetTextColor(Macros.Color.keypoint)
    self.TextField_1:SetPlaceHolderText(CPPGameLib.GetString("union_perch"))

     local params =  
     {
       item_class = Modules.TemplUnionJoinItem,
       item_width = 776.00,
       item_height = 145.00,
       row = 4,
       col = 1,
       view_height = self.Image_1:GetContentHeight() - 5 ,
     }
     -- 创建参数
     self.__zones_scrollview = GUI.ScrollView.New(params)
     self.__zones_scrollview:SetPosition(self.Image_1:GetContentWidth()/2 , self.Image_1:GetContentHeight()/2 )
     self.Image_1:AddChild(self.__zones_scrollview:GetNode())

end

--设置列表
function LogicGUI.TemplUnionJoin:SetShowUI( lst )
    self.__zones_scrollview:SetDataList( lst )
end

--刷新列表
function LogicGUI.TemplUnionJoin:UpShowUI( id , index )
    self.__zones_scrollview:ItemDataIter(function(i, item_data)
                                            if item_data.guild_id == id then
                                                item_data.is_apply = index
                                            end
                                            local item = self.__zones_scrollview:GetItem(i)
                                            if item then
                                                item:SetShowUI(item_data)
                                            end
                                        end)
end

--关闭
function LogicGUI.TemplUnionJoin:OnBtnClose()
    if self.func then
        self.func()
    end
    self:DeleteMe()
end

--搜索
function LogicGUI.TemplUnionJoin:OnButtonSeek()
    local info = {}
    info.guild_name = self.TextField_1:GetText()
    local name , isop = LanguageFilter.NameFilter(info.guild_name)
    if isop then
        CallModuleFunc(ModuleType.UNION , "GuildSearchReq" , info )
    else
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_name_invalid") )
    end
end

--创建
function LogicGUI.TemplUnionJoin:OnButtonEstablish()
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.UNION_ESTABLISH ,false)
end




Modules = Modules or {}
Modules.TemplUnionJoinItem = Modules.TemplUnionJoinItem or BaseClass(GUI.ScrollItem)

function Modules.TemplUnionJoinItem:__init()
    self:InitTemplate(GUI.TemplateRegistry.UNION_JOIN_ITEM) 
    self:__Getwidget()
end

function Modules.TemplUnionJoinItem:__delete()
end

function Modules.TemplUnionJoinItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self:__ShowUI( data )
end

function Modules.TemplUnionJoinItem:__AddToParent()
end

function Modules.TemplUnionJoinItem:__Getwidget()
    --条目
    self.Image_item = self:GetWidget("Image_item")
    --图标
    self.Panel_icon = self:GetWidget("Panel_icon")
    --线
    self.Image_thinline = self:GetWidget("Image_thinline")
    --名字
    self.Text_name = self:GetWidget("Text_name")
    --团长
    self.Text_colonel = self:GetWidget("Text_colonel")
    --宣言
    self.Text_manifesto = self:GetWidget("Text_manifesto")
    --等级
    self.Text_lv = self:GetWidget("Text_lv")
    --成员
    self.Text_member = self:GetWidget("Text_member")

    --申请
    self.Button_1 = Util:WidgetButton( 
                                            self:GetWidget("Button_1") , "" , 
                                            CPPGameLib.Handler(self,self.ApplyFor) 
                                        )
end

function Modules.TemplUnionJoinItem:__ShowUI(data)
    --图标
    local iconSp = "union_icon" .. data.icon_id
    self.Panel_icon:SetBackGroundImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION , iconSp) )
    self.Panel_icon:SetBackGroundImageScale9Enabled(true)
    self.Panel_icon:SetBackGroundImageCapInsets( 70 , 70 , 0 , 0 )

    --等级
    self.common_lev9 = Util:Label( CPPGameLib.GetString("common_lev9" , data.guild_lv ) , Macros.TypefaceSize.normal , Macros.Color.content )
    self.common_lev9:SetAnchorPoint( 0.5 , 0.5 )
    self.Text_lv:AddChild( self.common_lev9 )
    PosTool.Center( self.common_lev9 )

    --名字
    self.name = Util:Label( data.guild_name , Macros.TypefaceSize.largish , Macros.Color.headline )
    self.name:SetAnchorPoint( 0 , 0.5 )
    self.name:SetPosition( 0 , self.Text_name:GetContentHeight()/2 )
    self.Text_name:AddChild( self.name )

    --团长
    local tab = 
     {
         { zi = CPPGameLib.GetString("union_leader") , ys = Macros.Color.keypoint , dx = Macros.TypefaceSize.normal },
         { zi = data.leader_name , ys = Macros.Color.content , dx = Macros.TypefaceSize.normal },
     }
    self.union_leader = Util:LabCenter_3( tab , nil , 500 , nil )
    self.union_leader:SetAnchorPoint( 0 , 1 )
    self.union_leader:SetPosition( -10 , self.Text_colonel:GetContentHeight() )
    self.Text_colonel:AddChild( self.union_leader )

    --军团宣言
    local tab = 
     {
         { zi = CPPGameLib.GetString("union_manifesto") , ys = Macros.Color.keypoint , dx = Macros.TypefaceSize.normal },
         { zi = data.declaration , ys = Macros.Color.content , dx = Macros.TypefaceSize.normal },
     }
    self.union_manifesto = Util:LabCenter_3( tab , nil , 450 , nil )
    self.union_manifesto:SetAnchorPoint( 0 , 1 )
    self.union_manifesto:SetPosition( -10 , self.Text_manifesto:GetContentHeight() )
    self.Text_manifesto:AddChild( self.union_manifesto )

    local guildCnf = CPPGameLib.GetConfig("Guild", data.guild_lv )
    if not guildCnf then
        return
    end
    local max = guildCnf.member_max
    --成员数量
    local tab = 
     {
         { zi = CPPGameLib.GetString("union_num") , ys = Macros.Color.keypoint , dx = Macros.TypefaceSize.normal },
         { zi = data.member_num .."/"..max , ys = Macros.Color.front_color , dx = Macros.TypefaceSize.normal },
     }
    self.union_num = Util:LabCenter_3( tab , nil , 450 , nil )
    self.union_num:SetAnchorPoint( 0 , 0.5 )
    self.union_num:SetPosition( 0 , self.Text_member:GetContentHeight()/2 )
    self.Text_member:AddChild( self.union_num )  

    self:SetShowUI( data )
end 

-- 1993 军团申请添加主动取消
function Modules.TemplUnionJoinItem:SetShowUI( data )
    --如果是满员
    if data.is_full == 1 then
        -- self.Button_1:SetNormalImage(  )
        self.Button_1:SetText( CPPGameLib.GetString("union_yetFull") )
        Util:GreyButton( self.Button_1 )
    elseif data.is_apply == 1 then
        local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bigred")
        self.Button_1:SetNormalImage( imgdata )
        self.Button_1:SetText( CPPGameLib.GetString("union_yetApply") )
        local r, g, b, a = unpack(Macros.Color.button_red)
        self.Button_1:SetOutline(r, g, b, a, 2)
        self.Button_1:SetTouchEnabled(true)
        
        WidgetUnity.SetWidgetListener( self.Button_1 , nil, nil, function ( ... )
            self:NotApplyFor()
        end, nil, 0.5*1000)
    else
        local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen2")
        self.Button_1:SetNormalImage( imgdata )
        self.Button_1:SetText( CPPGameLib.GetString("union_apply") )
        local r, g, b, a = unpack(Macros.Color.button_yellow)
        self.Button_1:SetOutline(r, g, b, a, 2)
        self.Button_1:SetTouchEnabled(true)

        WidgetUnity.SetWidgetListener( self.Button_1 , nil, nil, function ( ... )
            self:ApplyFor()
        end, nil, 0.5*1000)

    end

end

function Modules.TemplUnionJoinItem:ApplyFor( data )
    local info = {}
    info.guild_id = self.__data.guild_id
    CallModuleFunc(ModuleType.UNION , "GuildApplyReq" , info )
end


function Modules.TemplUnionJoinItem:NotApplyFor( data )
    local info = {}
    info.guild_id = self.__data.guild_id
    CallModuleFunc(ModuleType.UNION , "GuildApplyCancelReq" , info )
end