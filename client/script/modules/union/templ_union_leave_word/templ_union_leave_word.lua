--
-- @brief 公会留言
-- @author: yjg
-- @date: 2016年10月20日11:51:50
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplUnionLeaveWord = LogicGUI.TemplUnionLeaveWord or BaseClass(GUI.Template)

function LogicGUI.TemplUnionLeaveWord:__init(data)

    -- self.ShowList = self:BindGlobalEvent(Macros.Event.UnionModule.UNION_GUESTBOOK,function ( lst ,type_)
    --     self:SetShowUI(lst,type_)
    -- end)
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__open_callback = function()
        self:__Getwidget()
        self:__ShowUI()
        self:__Server()
    end
    -- CallModuleFunc(ModuleType.UNION , "GuildLeavemsgListReq")
    self:__AddPreloadTemplate(GUI.TemplateRegistry.UNION_LEAVE_WORD_ITEM) 
    self:InitTemplate(GUI.TemplateRegistry.UNION_LEAVE_WORD) 
end

function LogicGUI.TemplUnionLeaveWord:__delete()
end

function LogicGUI.TemplUnionLeaveWord:__Server( ... )
    CallModuleFunc(ModuleType.UNION , "GuildLeavemsgListReq")

    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_GUESTBOOK, function(_, index)
        self:SetShowUI()
        -- if index then
        --     LogicGUI.TemplUnionConstructionSucceed.New( index )
        -- end
    end)
end

function LogicGUI.TemplUnionLeaveWord:__Getwidget( ... )
    --背景
    self.Image_Bg = self:GetWidget("Image_Bg")
    --关闭
    self.Button_close = Util:WidgetButton( self:GetWidget("Button_close") , "" , CPPGameLib.Handler(self,self.OnButtonCancel) )
    --标题
    self.Image_light = self:GetWidget("Image_light")
    --内容背景
    self.Image_content = self:GetWidget("Image_content")
    --留言
    self.Button_establish = Util:WidgetButton( 
                                                self:GetWidget("Button_establish") , 
                                                CPPGameLib.GetString("union_leaveWord") , 
                                                CPPGameLib.Handler(self,self.OnButtonEstablish) ,
                                                nil,nil,Macros.Color.button_yellow
                                            )
    --关闭
    self.Button_cancel = Util:WidgetButton( 
                                            self:GetWidget("Button_cancel") , 
                                            CPPGameLib.GetString("common_close") , 
                                            CPPGameLib.Handler(self,self.OnButtonCancel) ,
                                            nil,nil,Macros.Color.button_yellow
                                        )
end


function LogicGUI.TemplUnionLeaveWord:__ShowUI( ... )
    --标题
    local name = Util:Label(CPPGameLib.GetString("union_messageBoard") , Macros.TypefaceSize.popup , Macros.Color.white)
    name:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2)
    self.Image_light:AddChild(name)

    --列表
    local params =  
    {
      item_class = Modules.TemplUnionLeaveWordItem,
      item_width = self.Image_content:GetContentWidth(),
      item_height = 135.00 ,
      row = 4,
      col = 1,
      view_height = self.Image_content:GetContentHeight() - 10 ,
    }
    -- 创建参数
    self.__zones_scrollview = GUI.ScrollView.New(params)
    self.__zones_scrollview:SetPosition(self.Image_content:GetContentWidth()/2 , self.Image_content:GetContentHeight()/2 )
    self.Image_content:AddChild(self.__zones_scrollview:GetNode())
end

function LogicGUI.TemplUnionLeaveWord:SetShowUI( lst )
    print("xy" , "=-======-=-=SetShowUI")
    local lst = CallModuleFunc(ModuleType.UNION , "GetLeaveWordList")
    self.__zones_scrollview:SetDataList( lst )
    self.__zones_scrollview:LocateToTop()
end
    
--取消
function LogicGUI.TemplUnionLeaveWord:OnButtonCancel()
    self:DeleteMe()
end

--确定
function LogicGUI.TemplUnionLeaveWord:OnButtonEstablish( ... )
    LogicGUI.TemplUnionImport.New( function ( text )
        local info = {}
        info.leave_msg = text
        CallModuleFunc(ModuleType.UNION , "GuildLeaveEssageReq" , info)
    end )
end


Modules = Modules or {}
Modules.TemplUnionLeaveWordItem = Modules.TemplUnionLeaveWordItem or BaseClass(GUI.ScrollItem)

function Modules.TemplUnionLeaveWordItem:__init()
    self:InitTemplate(GUI.TemplateRegistry.UNION_LEAVE_WORD_ITEM) 
    self:__Getwidget()
end

function Modules.TemplUnionLeaveWordItem:__delete()
end

function Modules.TemplUnionLeaveWordItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self:__ShowUI( data )
end

function Modules.TemplUnionLeaveWordItem:__AddToParent()
end

function Modules.TemplUnionLeaveWordItem:__Getwidget()
    --条目
    self.Image_item = self:GetWidget("Image_item")
    --内容框
    self.Image_content = self:GetWidget("Image_content")
    --名字
    self.Text_name = self:GetWidget("Text_name")
    --时间
    self.Text_time = self:GetWidget("Text_time")
end

function Modules.TemplUnionLeaveWordItem:__ShowUI( data )
    --名字
    self.Text_name = Util:WidgetLabel( self.Text_name , data.name .. CPPGameLib.GetString("union_pos"..data.pos) , Macros.TypefaceSize.normal , Macros.Color.keypoint )
    --时间
    self.Text_time = Util:WidgetLabel( self.Text_time , TimeTool.OfflineTime(data.time) , Macros.TypefaceSize.normal, Macros.Color.content )
    --留言
    local lab = Util:Label( data.text ,Macros.TypefaceSize.slightly , Macros.Color.content  ,self.Image_content:GetContentWidth() - 10 )
    lab:SetAnchorPoint( 0 , 1 )
    lab:SetPosition( 5  , self.Image_content:GetContentHeight() - 10 )
    self.Image_content:AddChild(lab) 
end
