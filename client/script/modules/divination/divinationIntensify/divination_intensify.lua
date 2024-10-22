
--
-- @brief 卡牌提升界面
-- @author: yjg
-- @date: 2017年11月14日15:13:51
--

-- 传进来的参数 第一个是页签 第二个是UID
Modules = Modules or {}

Modules.DivinationIntensifyView = Modules.DivinationIntensifyView or BaseClass(BaseView)

function Modules.DivinationIntensifyView:__init()
    self.__layout_name = "divination_intensify"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("divination"),
        show_power = true,
        show_diamond = true,
        show_gold = true,
        show_level = true,
        close_func = function()
            CallModuleFunc(ModuleType.DIVINATION, "SetSelectedStrengthenMaterial")
            self:CloseManual()
        end,
    }

    local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no")
    self.__tab_params = 
    {
        {sp_2,sp_1},
        {
            CPPGameLib.GetString("divination_info"),
            CPPGameLib.GetString("divination_intensify"),
            CPPGameLib.GetString("divination_refine"),
        },
        10 , CPPGameLib.Handler(self, self.OnBox) , false , nil,
        self.__prepare_level_list,
        self.__lock_level_list
    }

    self.__tab_view_params = 
    {
        {Modules.TemplDivinationMinuteInfo, "Panel_divinationInfo"},
        {Modules.TemplDivinationIntensify, "Panel_divinationIntensify"},
        {Modules.TemplDivinationRefine, "Panel_divinationRefine"},
    }

    self.__open_callback = function()
        self:__OpenCallback()
    end  
end

function Modules.DivinationIntensifyView:__OpenCallback()
    self:__Getwidget()
    self:__ShowUI()
    self:__Server()
end

function Modules.DivinationIntensifyView:__Server()
    self:BindGlobalEvent(Macros.Event.DivinationModule.DIVINATION_UP , function ( ... )
        self:SelectedIcon( nil , true )
    end)
    self:BindGlobalEvent(Macros.Event.DivinationModule.DIVINATION_BREAK , function ( ... )
        self:SelectedIcon( nil , true )
    end)
end

function Modules.DivinationIntensifyView:__RegistAllEvents()
end

function Modules.DivinationIntensifyView:__Dispose()
end

function Modules.DivinationIntensifyView:__Getwidget()
    --背景
    self.Panel_interlayer = self:GetWidget("Panel_interlayer")
    --图标列表
    self.Image_lstBg = self:GetWidget("Image_lstBg")
    self.Image_lstBg:SetVisible(false)
    --内容图标
    self.Image_contentBg = self:GetWidget("Image_contentBg")
    --页签容器
    self.Panel_Backpackcbx = self:GetWidget("Panel_Backpackcbx")
end

function Modules.DivinationIntensifyView:__ShowUI( ... )

    if self.__open_data[2] then
        local info = CallModuleFunc(ModuleType.DIVINATION, "GetDivination" , self.__open_data[2] )
        CallModuleFunc(ModuleType.DIVINATION, "SetOnDivinationUid", self.__open_data[2] )
        if info.hero_id and info.hero_id ~= 0 then
            self.Image_lstBg:SetVisible(true)
        else
            self.Image_lstBg:SetVisible(false)
            self.Image_contentBg:SetPosition( self.Panel_interlayer:GetContentWidth()/2 , self.Image_contentBg:GetPositionY() )
        end
    end

    --页签
    self.__tab:SetPosition(0 , 5)
    self.Panel_Backpackcbx:AddChild(self.__tab:GetNode())
    self.__tab:On(self.__open_data[1] or 1)
    self.__tab:SetCboxColor( Macros.Color.button_yellow , Macros.Color.button_red )

    local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")

    self.icon_intensify_point = Util:Sprite(icon_red_point)
    self.icon_intensify_point:SetPosition(self.__tab.btn_list[2]:GetContentWidth() - 10 , self.__tab.btn_list[2]:GetContentHeight() - 10 )
    self.__tab.btn_list[2]:AddChild(self.icon_intensify_point , 20)
    self.icon_intensify_point:SetVisible(false)

    self.icon_refine_point = Util:Sprite(icon_red_point)
    self.icon_refine_point:SetPosition(self.__tab.btn_list[3]:GetContentWidth() - 10 , self.__tab.btn_list[3]:GetContentHeight() - 10 )
    self.__tab.btn_list[3]:AddChild(self.icon_refine_point , 20)
    self.icon_refine_point:SetVisible(false)

    --图标列表
    local params =  
    {
        item_class = Modules.DivinationIntensifyItem,
        item_width = self.Image_lstBg:GetContentWidth(),
        item_height = 110,
        row = 4,
        col = 1,
        item_space_r = 10 ,
        view_width = self.Image_lstBg:GetContentWidth(),
        view_height = self.Image_lstBg:GetContentHeight() - 30 ,
        selectable = true,
        select_callback = function ( index , item )
            if item.id ~= 0 then
                CallModuleFunc(ModuleType.DIVINATION, "SetOnDivinationUid",item.id)
                self:SelectedIcon()
            end
        end
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.Image_lstBg:AddChild(self.lst_list:GetNode())
    self.lst_list:SetPosition(self.Image_lstBg:GetContentWidth()/2 , self.Image_lstBg:GetContentHeight()/2  )

    local lineup_index = CallModuleFunc(ModuleType.HERO,"GetDivinationFormationPosAndCardSlotByCardUid" , self.__open_data[2])

    local lineup_list = CallModuleFunc(ModuleType.HERO,"GetDivinationListByLineupIndex",lineup_index)

    local t1 = {}
    local t2 = {}
    for i,v in ipairs(lineup_list) do
        if v ~= 0 then
            table.insert( t1 , {id = v} )
        else
            table.insert( t2 , {id = v} )
        end
    end

    --列表设定
    self.lst_list:SetDataList( t1 )

    --界面接受参数
    for i , v in ipairs( t1 ) do
        if v.id == self.__open_data[2] then
            CallModuleFunc(ModuleType.DIVINATION, "SetOnDivinationUid",v.id)
            self:SelectedIcon( i )
            break
        end
    end 
    
    --默认传参
    if not self.__open_data[2] and t1[1] then
        CallModuleFunc(ModuleType.DIVINATION, "SetOnDivinationUid", t1[1].id)
        self:SelectedIcon( 1 )
    end
end

function Modules.DivinationIntensifyView:OnBox( index )
    self.index = index
    self:__TabChange(index)    
end

function Modules.DivinationIntensifyView:SelectedIcon( index , up_type_ )
    local selected = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_selected")
    local divinationUid = CallModuleFunc(ModuleType.DIVINATION, "GetOnDivinationUid")

    local redType = CallModuleFunc(ModuleType.DIVINATION, "ToUidGetCardIsStrengthen", divinationUid )   
    if redType == "true" then
        self.icon_intensify_point:SetVisible(true)
    else
        self.icon_intensify_point:SetVisible(false)
    end

    local redType = CallModuleFunc(ModuleType.DIVINATION, "ToUidGetCardIsBreak", divinationUid ) 
    if redType == "true" then
        self.icon_refine_point:SetVisible(true)
    else
        self.icon_refine_point:SetVisible(false)
    end

    --列表相关调整
    self.lst_list:ItemIter(function(i, item)
        local item = self.lst_list:GetItem(i)
        item:__SetIconRed()
        item:__SetShowLevel()
    end)

    if index then
        self.lst_list:SelectAndLocateItem(index)
    end

    --刷新信息
    local page = self:GetActivatedTabView()
    if page then
        if up_type_ then
            page:__SetShowUI(type_ )
        else
            page:__SetShowUI(type_ , true )
        end
    end
end


Modules = Modules or {}
Modules.DivinationIntensifyItem = Modules.DivinationIntensifyItem or BaseClass(GUI.ScrollItem)

function Modules.DivinationIntensifyItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.DivinationIntensifyItem:__delete()
    if self.icon then
        self.icon:DeleteMe()
        self.icon = nil
    end
end

function Modules.DivinationIntensifyItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self:__ShowUI(data)
end

function Modules.DivinationIntensifyItem:__CreateWidgetFromLuaCode()
    self.__layout = Util:Layout(90,110)

    return self.__layout
end

function Modules.DivinationIntensifyItem:__ShowUI(data)
    --头像
    if data.id ~= 0 then
        local info = CallModuleFunc(ModuleType.DIVINATION,"GetDivination", data.id )
        if info then
            self.icon = LogicGUI.IconLayer.New( false , false )
            self.__layout:AddChild(self.icon:GetNode())
            PosTool.Center(self.icon)
            self.icon:SetData({ id = info.info_id , obj_info = info })

            local selected = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_selected")
            self.selected = Util:Sprite9( selected )
            self.icon:AddChild( self.selected , 5)
            PosTool.Center( self.selected )
            self.selected:SetVisible(false)

            --名字
            self.name = Util:Name(info.name , Macros.TypefaceSize.slightly , info.quality)
            self.name:SetAnchorPoint(0.5,1)
            self.name:SetPosition(self.icon:GetNode():GetContentWidth()/2 , -5 )
            self.icon:GetNode():AddChild(self.name)

            self:__SetShowLevel()
        end
    end
    self:__SetIconRed()
end

function Modules.DivinationIntensifyItem:SetSelected(var)
    self.selected:SetVisible(var)
end

function Modules.DivinationIntensifyItem:__SetIconRed()
    self:__RemoveIconRed()
    if self.__data.id == 0 then
        return 
    end

    local redType = CallModuleFunc(ModuleType.DIVINATION, "ToUidGetCardIsStrengthen", self.__data.id ) 
    if redType == "false" then
        return 
    end

    local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
    self.icon_red_point = Util:Sprite(icon_red_point)
    self.icon_red_point:SetPosition(self.icon:GetNode():GetContentWidth() - 12 , self.icon:GetNode():GetContentHeight() - 5 )
    self.icon:AddChild(self.icon_red_point , 20)
end

function Modules.DivinationIntensifyItem:__SetShowLevel()
    if self.__data.id == 0 then
        return 
    end
    local info = CallModuleFunc(ModuleType.DIVINATION,"GetDivination", self.__data.id )

    self.icon:ShowDivinationStrengthenLev(true)
    self.icon:SetDivinationStrengthenLev( info.level )
end

function Modules.DivinationIntensifyItem:__RemoveIconRed()
    if self.icon_red_point then
        self.icon_red_point:RemoveSelf(true)
        self.icon_red_point = nil
    end
end



