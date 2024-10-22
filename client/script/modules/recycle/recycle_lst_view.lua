
--
-- @brief 重生列表
-- @author: yjg
-- @date: 2016年11月24日16:40:26
--

Modules = Modules or {}

Modules.RecycleLstView = Modules.RecycleLstView or BaseClass(BaseView)

function Modules.RecycleLstView:__init()
    self.__layout_name = "equip"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
    self.__title_bar_params = 
    {
        title = " ",
        show_diamond = true,
        show_gold = true,
        show_stamina = true,
        show_energy = true,
        close_func = function()
            self:CloseManual()
            -- self:OnButtonSell()
        end,
    }
    --穿戴开关
    self.__open_callback = function()
        self:__OpenCallback()
    end
    self:__AddPreloadTemplate(GUI.TemplateRegistry.EQUIP_CROSS)
    -- self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.EQUIPPACK)
end

function Modules.RecycleLstView:__delete()
    if self.lst_list then
        self.lst_list:DeleteMe()
        self.lst_list = nil
    end
end

function Modules.RecycleLstView:__OpenCallback()    
    self:__Getwidget()
    self:__ShowUI()
end

function Modules.RecycleLstView:__Dispose()
end

function Modules.RecycleLstView:__Getwidget( ... )
    -- --下框
    -- self.Image_1 = self:GetWidget("Image_1") 
    -- --上框
    -- self.Image_2 = self:GetWidget("Image_2") 
    --内容框
    self.Image_content = self:GetWidget("Image_content") 

    --穿戴容器
    self.Panel_changePack = self:GetWidget("Panel_changePack") 
    self.Panel_changePack:SetVisible(true)
    --穿戴组件
    self.Text_changeTips = self:GetWidget("Text_changeTips") 
    --穿戴组件
    self.Panel_cBox = self:GetWidget("Panel_cBox") 
    self.Panel_cBox:SetVisible(true)

    --列表容器
    self.Panel_Backpack = self:GetWidget("Panel_Backpack")
    self.Panel_Backpack:SetVisible(false)
    --出售按钮
    self.Button_sell = Util:WidgetButton( self:GetWidget("Button_sell") , CPPGameLib.GetString("recycle_sell") , CPPGameLib.Handler(self,self.OnSell) )
    --数量上限
    self.Button_showMax = self:GetWidget("Button_showMax") 

    --页签
    self.Panel_Backpackcbx = self:GetWidget("Panel_Backpackcbx")

    self.Panel_9 = self:GetWidget("Panel_9")
end

function Modules.RecycleLstView:__ShowUI( ... )
    local tab = {
            [Macros.Game.ObjType.EQUIP] = CPPGameLib.GetString("recycle_onEquipLst"),
            [Macros.Game.ObjType.HERO] = CPPGameLib.GetString("recycle_onHeroLst"),
            [Macros.Game.ObjType.TREASURE] = CPPGameLib.GetString("recycle_onTreasure"),
            [Macros.Game.ObjType.FAIRY] = CPPGameLib.GetString("recycle_onCard"),
        }

    self.__title_bar:SetTitle( tab[unpack(self.__open_data)] )

    local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no")
    self.__tab = GUI.FeignCheckBoxBtn.New({sp_2,sp_1},{ tab[unpack(self.__open_data)] },10,CPPGameLib.Handler(self, self.OnYeMei),false)
    self.__tab:On(1)
    self.__tab:SetPosition(0 , 0)
    self.Panel_Backpackcbx:AddChild(self.__tab:GetNode())

    --选中框
    local params =  
    {
        item_class = Modules.TemplRecycleLstItem,
        item_width = 460.5,
        item_height = 120,
        row = 4,
        col = 2,
        item_space_r = 5,
        item_space_c = 5,
        -- view_width = 925.00,
        view_height = 482.50 ,
        touchable = true,
        touch_callback = function(item_index, item_data)

            CallModuleFunc(ModuleType.RECYCLE, "AddOnData", item_data )
            local obj_type = 0
            if unpack(self.__open_data) == Macros.Game.ObjType.HERO then
                obj_type = Macros.Game.ObjType.HERO
            elseif unpack(self.__open_data) == Macros.Game.ObjType.EQUIP then
                obj_type = Macros.Game.ObjType.EQUIP
            elseif unpack(self.__open_data) == Macros.Game.ObjType.TREASURE then
                obj_type = Macros.Game.ObjType.TREASURE

            elseif unpack(self.__open_data) == Macros.Game.ObjType.FAIRY then
                obj_type = Macros.Game.ObjType.FAIRY
            end
            local tab_index = CallModuleFunc(ModuleType.RECYCLE, "GetTabIndex", obj_type, true)
            CallModuleFunc(ModuleType.RECYCLE, "OpenView", BaseViewType.RECYCLE, tab_index , tab_index , obj_type)
        end,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.Image_content:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode())

    self:Setlist()
end

function Modules.RecycleLstView:Setlist( ... )
    local lst = CallModuleFunc(ModuleType.RECYCLE, "DateLst",unpack(self.__open_data) , true )
    local t1 = {}
    for i,v in ipairs(lst) do
        if v.type ~= Macros.Game.ObjType.HERO then
            if v.strengthen_level > 1 or v.refine_level ~= 0 then
                table.insert( t1 , v )
            end
        elseif v.type == Macros.Game.ObjType.HERO then
            if v.lv > 1 then
                table.insert( t1 , v )
            end
        else
            table.insert( t1 , v )
        end
    end
    self.lst_list:SetDataList( t1 )
end

function Modules.RecycleLstView:ShowOnData( ... )
end

function Modules.RecycleLstView:OnYeMei( index )
    local com_sp_cbox_wire = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp_cbox_wire")
    if self.com_sp_cbox_wire then
        self.com_sp_cbox_wire:RemoveSelf(true)
        self.com_sp_cbox_wire = nil
    end
    self.com_sp_cbox_wire = Util:Sprite(com_sp_cbox_wire)
    self.__tab.btn_list[index]:AddChild( self.com_sp_cbox_wire )
    PosTool.Center( self.com_sp_cbox_wire , 0 , -35 )
end

--确定
function Modules.RecycleLstView:OnButtonSell()

    -- print("xy" , "unpack(self.__open_data)" , unpack(self.__open_data) )
    -- local type_ = 0
    -- if unpack(self.__open_data) == Macros.Game.ObjType.HERO then
    --     type_ = 4
    -- elseif unpack(self.__open_data) == Macros.Game.ObjType.EQUIP then
    --     type_ = 5
    -- elseif unpack(self.__open_data) == Macros.Game.ObjType.TREASURE then
    --     type_ = 6
    -- elseif unpack(self.__open_data) == Macros.Game.ObjType.FAIRY then
    --     type_ = 7
    -- end

    -- CallModuleFunc(ModuleType.RECYCLE, "OpenView", BaseViewType.RECYCLE,type_)
end





Modules = Modules or {}
Modules.TemplRecycleLstItem = Modules.TemplRecycleLstItem or BaseClass(GUI.ScrollItem)

function Modules.TemplRecycleLstItem:__init(data,fun)
    self:InitTemplate(GUI.TemplateRegistry.EQUIP_CROSS)  
    self:__Getwidget()
end

function Modules.TemplRecycleLstItem:__delete()
end

function Modules.TemplRecycleLstItem:__AddToParent()
end

function Modules.TemplRecycleLstItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self.data = data
    self:__ShowUI(data)
end

function Modules.TemplRecycleLstItem:__Getwidget( ... )
    --背景
    self.Image_sellItem = self:GetWidget("Image_sellItem")
    --图标
    self.Panel_icon = self:GetWidget("Panel_icon")
    --名字
    self.Text_name = self:GetWidget("Text_name")
    --属性1
    self.Text_property1 = self:GetWidget("Text_property1")
    --属性2
    self.Text_property2 = self:GetWidget("Text_property2")
    --属性3
    self.Text_property3 = self:GetWidget("Text_property3")
    --更换按钮
    self.Button_change =  Util:WidgetButton( 
                                                self:GetWidget("Button_change") , 
                                                CPPGameLib.GetString("recycle_select") , 
                                                CPPGameLib.Handler(self,self.OnButtonChange) ,
                                                nil,nil,Macros.Color.button_yellow
                                                )
    self.Button_change:SetPosition( self.Button_change:GetPositionX() , 61.00 )
    --英雄名字
    self.Text_heroName = self:GetWidget("Text_heroName")
end

function Modules.TemplRecycleLstItem:__ShowUI( data )
    local type_ , info_ = ObjIDTool.GetInfo(data.info_id)

    --如果是英雄
    if type_ == Macros.Game.ObjType.HERO then
        self:__HeroShowUI(data , info_ )

    elseif type_ == Macros.Game.ObjType.EQUIP then
        self:__EquipShowUI(data , info_ )

    elseif type_ == Macros.Game.ObjType.TREASURE then  
        self:__TreasureShowUI( data , info_  )

    elseif type_ == Macros.Game.ObjType.FAIRY then  
        self:__CardShowUI( data , info_  )

    end

    --图标
    local icon_layer = LogicGUI.IconLayer.New(true)
    icon_layer:SetData({id = data.info_id})
    icon_layer:ShowEquipStrengthenLev(true)
    self.Panel_icon:AddChild(icon_layer:GetNode())
    PosTool.Center(icon_layer)   

    if type_ == Macros.Game.ObjType.EQUIP then
        local info = CallModuleFunc(ModuleType.EQUIP, "GetEquip", data.uid )
        icon_layer:SetEquipStar( info.star  )
    end
end

function Modules.TemplRecycleLstItem:GetTouchNode()
    return self.Button_change
end

------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--英雄条目
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
function Modules.TemplRecycleLstItem:__HeroShowUI(data,info)

    --类型标志
    self.__img_profession = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON, "icon_profession_"..info.profession, true))
    self.__img_profession:SetAnchorPoint(0,0.5)
    self.__img_profession:SetPosition(0,self.Text_name:GetContentHeight()/2)
    self.Text_name:AddChild(self.__img_profession)

    --名字
    local name = Util:Name(info.name,Macros.TypefaceSize.normal,data.quality)
    name:SetAnchorPoint(0,0.5)
    PosTool.RightTo( self.__img_profession , name , 5 )
    self.Text_name:AddChild(name)

    --等级
    local lvText = Util:Label( CPPGameLib.GetString("common_lev1") , Macros.TypefaceSize.normal , Macros.Color.keypoint)
    lvText:SetAnchorPoint(0,0.5)
    lvText:SetPosition( self.Text_name:GetPositionX() , self.Text_name:GetPositionY() - 40 )
    self.Image_sellItem:AddChild(lvText)

    local lv = Util:Label(data.lv,Macros.TypefaceSize.normal , Macros.Color.content )
    PosTool.RightTo(lvText,lv)
    self.Image_sellItem:AddChild(lv)

    --潜能
    local hero_potency = Util:Label( CPPGameLib.GetString("hero_potency")..  CPPGameLib.GetString("common_colon") , Macros.TypefaceSize.normal , Macros.Color.keypoint)
    hero_potency:SetAnchorPoint(0,0.5)
    hero_potency:SetPosition( self.Text_name:GetPositionX() ,self.Text_name:GetPositionY() - 70)
    self.Image_sellItem:AddChild(hero_potency)

    local propCriValue = Util:Label(data.potential_level,Macros.TypefaceSize.normal , Macros.Color.content )
    PosTool.RightTo(hero_potency,propCriValue)
    self.Image_sellItem:AddChild(propCriValue)

    --属性3
    local refine = Util:Label( CPPGameLib.GetString("hero_awaken") ..  CPPGameLib.GetString("common_colon") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
    refine:SetAnchorPoint(0,0.5)
    refine:SetPosition( self.Text_name:GetPositionX() + 100 ,self.Text_name:GetPositionY() - 40 )
    self.Image_sellItem:AddChild(refine)

    local refineValue = Util:Label( CPPGameLib.GetString( "hero_awaken_propertyTitle" , data.awaken_star , data.awakening_lv ) ,Macros.TypefaceSize.normal , Macros.Color.content )
    PosTool.RightTo(refine,refineValue)
    self.Image_sellItem:AddChild(refineValue)

end

------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--装备条目
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
function Modules.TemplRecycleLstItem:__EquipShowUI(data,info)
   --名字
    local equipType = CPPGameLib.GetString( "common_brackets" , CPPGameLib.GetString("equip_type"..info.item_type) )
    local name = Util:Name(equipType..info.name , nil ,info.quality)
    name:SetAnchorPoint(0, 0.5)
    name:SetPosition(0,self.Text_name:GetContentHeight()/2)
    self.Text_name:AddChild(name)

    --等级
    local lvText = Util:Label( CPPGameLib.GetString("recycle_intensify") , Macros.TypefaceSize.normal , Macros.Color.keypoint)
    lvText:SetAnchorPoint(0,0.5)
    lvText:SetPosition( self.Text_name:GetPositionX() - 5 , self.Text_name:GetPositionY() - 40 )
    self.Image_sellItem:AddChild(lvText)

    local lv = Util:Label(data.strengthen_level,Macros.TypefaceSize.normal, Macros.Color.content)
    PosTool.RightTo(lvText,lv)
    self.Image_sellItem:AddChild(lv)

    --属性3
    local refine = Util:Label( CPPGameLib.GetString("recycle_refine") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
    refine:SetAnchorPoint(0,0.5)
    refine:SetPosition( self.Text_name:GetPositionX() + 125 ,self.Text_name:GetPositionY() - 40 )
    self.Image_sellItem:AddChild(refine)

    local refineValue = Util:Label(data.refine_level,Macros.TypefaceSize.normal, Macros.Color.content)
    PosTool.RightTo(refine,refineValue)
    self.Image_sellItem:AddChild(refineValue)
end

------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--圣器条目
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
function Modules.TemplRecycleLstItem:__TreasureShowUI(data,info)
   --名字
    local equipType = CPPGameLib.GetString( "common_brackets" , CPPGameLib.GetString("treasure_type_"..info.type) )
    local name = Util:Name(equipType..info.name , nil ,info.quality)
    name:SetAnchorPoint(0, 0.5)
    name:SetPosition(0,self.Text_name:GetContentHeight()/2 )
    self.Text_name:AddChild(name)

    --等级
    local lvText = Util:Label( CPPGameLib.GetString("equip_intensifyLv") , Macros.TypefaceSize.normal , Macros.Color.keypoint)
    lvText:SetAnchorPoint(0,0.5)
    lvText:SetPosition( self.Text_name:GetPositionX() , self.Text_property1:GetPositionY())
    self.Image_sellItem:AddChild(lvText)

    local lv = Util:Label(data.strengthen_level,Macros.TypefaceSize.normal, Macros.Color.content)
    PosTool.RightTo(lvText,lv)
    self.Image_sellItem:AddChild(lv)

    --属性3
    local refine = Util:Label( CPPGameLib.GetString("equip_refineLv") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
    refine:SetAnchorPoint(0,0.5)
    refine:SetPosition( self.Text_name:GetPositionX() ,self.Text_property2:GetPositionY())
    self.Image_sellItem:AddChild(refine)

    local refineValue = Util:Label(data.refine_level,Macros.TypefaceSize.normal, Macros.Color.content)
    PosTool.RightTo(refine,refineValue)
    self.Image_sellItem:AddChild(refineValue)
end

------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--卡牌条目
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
function Modules.TemplRecycleLstItem:__CardShowUI(data,info)
   --名字
    local equipType = " "
    -- "【"..CPPGameLib.GetString("treasure_type_"..info.type).."】"
    local name = Util:Name(equipType..info.name , nil ,info.quality)
    name:SetAnchorPoint(0, 0.5)
    name:SetPosition(0,self.Text_name:GetContentHeight()/2 )
    self.Text_name:AddChild(name)

    --等级
    local lvText = Util:Label( CPPGameLib.GetString("equip_intensifyLv") , Macros.TypefaceSize.normal , Macros.Color.keypoint)
    lvText:SetAnchorPoint(0,0.5)
    lvText:SetPosition( self.Text_name:GetPositionX() , self.Text_property1:GetPositionY())
    self.Image_sellItem:AddChild(lvText)

    local lv = Util:Label(data.strengthen_level,Macros.TypefaceSize.normal, Macros.Color.content)
    PosTool.RightTo(lvText,lv)
    self.Image_sellItem:AddChild(lv)

    --属性3
    local refine = Util:Label( CPPGameLib.GetString("hero_break_lev_title") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
    refine:SetAnchorPoint(0,0.5)
    refine:SetPosition( self.Text_name:GetPositionX() ,self.Text_property2:GetPositionY())
    self.Image_sellItem:AddChild(refine)

    local refineValue = Util:Label(data.refine_level,Macros.TypefaceSize.normal, Macros.Color.content)
    PosTool.RightTo(refine,refineValue)
    self.Image_sellItem:AddChild(refineValue)
end
