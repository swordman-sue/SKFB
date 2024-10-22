--
-- @brief 圣器穿戴列表
-- @author: yjg
-- @date: 2016年8月17日14:07:31
--

Modules = Modules or {}

Modules.TreasureCrossListView = Modules.TreasureCrossListView or BaseClass(BaseView)

function Modules.TreasureCrossListView:__init()
    self.__layout_name = "equip"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("equip_Treasure_replace"),
        show_diamond = true,
        show_gold = true,
        show_stamina = true,
        show_energy = true,
        close_func = function()
          self:CloseManual()
        end,
    }
    --穿戴开关
    self.key = false
    self.__open_callback = function()
        self:__OpenCallback()
    end
    self:__AddPreloadTemplate(GUI.TemplateRegistry.EQUIP_CROSS)
end

function Modules.TreasureCrossListView:__Dispose()
    if self.lst_list then
        self.lst_list:DeleteMe()
        self.lst_list = nil
    end
end

function Modules.TreasureCrossListView:__OpenCallback()    
    self:__Getwidget()
    self:__ShowUI()
end

function Modules.TreasureCrossListView:__Getwidget( ... )
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

function Modules.TreasureCrossListView:__ShowUI( ... )
    --选中框
    local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_select_no")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_select_yes")
    local cbx = UICheckBoxEx.CreateWithImage(sp_1,sp_2)
    cbx:SetSelectedState(true)
    cbx:SetPosition(self.Panel_cBox:GetContentWidth()/2 , self.Panel_cBox:GetContentHeight()/2 )
    self.Panel_cBox:AddChild(cbx)
    cbx:SetEventListener(
        GlobalCallbackMgr:AddCallBackFunc(function()
            self.key = false
            self.lst_list:RefreshLayout()
        end),
        GlobalCallbackMgr:AddCallBackFunc(function()
            self.key = true
            self.lst_list:RefreshLayout()
        end))

    --tips
    local lab = Util:Name(CPPGameLib.GetString("equip_hideCrossTreasure"),Macros.TypefaceSize.largish)
    PosTool.RightTo(cbx,lab)
    self.Panel_cBox:AddChild(lab)

    local params =  
    {
        item_class = Modules.TemplTreasureCrossItem,
        item_width = 460.5,
        item_height = 120,
        row = 4,
        col = 2,
        item_space_r = 5,
        item_space_c = 5,
        view_height = 482.50 ,
        touchable = true,
        filter_func = function(item_data)
            if self.key == false then
                if item_data.hero_infoid == 0 then
                    return true
                end
            else
                return true
            end
        end,
        touch_callback = function(item_index, item_data)
            local info = {}
            info.formation_pos = item_data.index
            info.treasure_pos = item_data.item_type
            info.treasure_uid = item_data.uid
            CallModuleFunc(ModuleType.TREASURE, "TreasurePutOnReq",info)
            -- self:CloseManual()
            CallModuleFunc(ModuleType.HERO, "OpenView", BaseViewType.HERO_LINEUP, BaseViewSubType.HeroLineup.MAIN , item_data.index )
        end,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.Image_content:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode())

    self:Setlist()

    --监听指引下一个步骤事件
    self:BindGlobalEvent(Macros.Event.LeadModule.CHECK_NEXT_SECTION, function(_, pre_section_info)
        self.lst_list:ItemIter(function(item_index, item)
            item:CheckSectionOperateSP()
        end)
    end)
end

function Modules.TreasureCrossListView:Setlist( ... )
    local treasureLst = CallModuleFunc(ModuleType.TREASURE, "GetTreasureSort")

    local lst = {}
    for i,v in ipairs( CPPGameLib.CopyTbl(treasureLst) ) do
        local info = CPPGameLib.GetConfig("Treasure", v.info_id)
        if info.type == unpack(self.__open_data).item_type then
            v.index = unpack(self.__open_data).index
            v.item_type = info.type
            table.insert(lst,v)
        end
    end

    local cnf = CPPGameLib.GetConfig("Hero", unpack(self.__open_data).hero_id)
    
    local uid = nil
    for index = 1,5 do
        local lineup_info = CallModuleFunc(ModuleType.HERO,"GetLineupInfo",index)
        if lineup_info.uid ~= 0 then
            local heroinfo = CallModuleFunc(ModuleType.HERO,"GetHeroInfo", lineup_info.uid ) 
            if heroinfo.info_id == unpack(self.__open_data).hero_id then
                uid = lineup_info.uid
            end
        end
    end

    local t1 = {}
    if uid and CallModuleFunc(ModuleType.HERO, "GetIsHeroEvolution" , uid) then
        if cnf.relation_treasure_ex_list then
            for _,v in ipairs(cnf.relation_treasure_ex_list) do
                for __,vv in ipairs(v) do
                    t1[vv] = cnf.relation_treasure_reinforcementname_list[_]
                end
            end
        end
    else
        if cnf.relation_treasure_list then
            for _,v in ipairs(cnf.relation_treasure_list) do
                for __,vv in ipairs(v) do
                    t1[vv] = cnf.relation_treasure_reinforcementname_list[_]
                end
            end
        end
    end

    local t2 = lst
    local t3 = {}
    local t4 = {}
    for i,v in ipairs(t2) do
        if t1[v.info_id] then
            v.treasure_reinfo = t1[v.info_id]
            table.insert(t3 , v)
        else
            table.insert(t4 , v)
        end
    end

    for i,v in ipairs(t4) do
        table.insert(t3 , v)
    end

    self.lst_list:SetDataList(t3)

end

Modules = Modules or {}
Modules.TemplTreasureCrossItem = Modules.TemplTreasureCrossItem or BaseClass(GUI.ScrollItem)

function Modules.TemplTreasureCrossItem:__init(data,fun)
    self:InitTemplate(GUI.TemplateRegistry.EQUIP_CROSS)  
    self:__Getwidget()
end

function Modules.TemplTreasureCrossItem:__delete()
end

function Modules.TemplTreasureCrossItem:__AddToParent()
end

function Modules.TemplTreasureCrossItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self.data = data
    self:__ShowUI(data)
end

function Modules.TemplTreasureCrossItem:SetViewIndex(var)
    GUI.ScrollItem.SetViewIndex(self, var)

    --新手指引特殊操作检测(圣器穿戴（格子索引，从1开始）)
    CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.TREASURE_WEAR, self.Button_change, self:GetViewIndex())
end

function Modules.TemplTreasureCrossItem:__Getwidget( ... )
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
    --属性4
    self.Text_property4 = self:GetWidget("Text_property4")
    --更换按钮
    self.Button_change =  Util:WidgetButton( 
                                                self:GetWidget("Button_change") , 
                                                CPPGameLib.GetString("equip_btn_cross") , 
                                                CPPGameLib.Handler(self,self.OnButtonChange) ,
                                                nil,nil,Macros.Color.button_yellow
                                            )
    self.Button_change:SetPosition( self.Button_change:GetPositionX() , 61.00 )
    --英雄名字
    self.Text_heroName = self:GetWidget("Text_heroName")
end

function Modules.TemplTreasureCrossItem:__ShowUI( data )
    local info = CPPGameLib.GetConfig("Treasure", data.info_id)

    -- 1746 圣器/装备精炼等级显示位置修改
    -- 重现步骤

    -- 圣器/装备在背包和替换列表中强化等级和精炼等级位置调整

    -- 1、强化等级：统一显示在图标左下角

    -- 2、精炼等级：统一显示在名字后面

    local level = ""
    if data.refine_level ~= 0 then
        level = CPPGameLib.GetString("common_add4" , data.refine_level ) 
    end
    --名字
    local name = Util:Name("[".. CPPGameLib.GetString("treasure_type_"..info.type) .."]" .. info.name .." " .. level ,Macros.TypefaceSize.normal,info.quality)
    name:SetAnchorPoint(0,0.5)
    name:SetPosition(0,self.Text_name:GetContentHeight()/2 + 3)
    self.Text_name:AddChild(name)

    --图标
    local icon_layer = LogicGUI.IconLayer.New(true)
    icon_layer:SetData({id = data.info_id , obj_info = data })
    icon_layer:ShowTreasureStrengthenLev(true)
    self.Panel_icon:AddChild(icon_layer:GetNode())
    PosTool.Center(icon_layer)   

    --英雄名称
    if data.hero_infoid ~= 0 then
        local info = CPPGameLib.GetConfig("Hero", data.hero_infoid)
        local heroName = Util:Label(info.name..CPPGameLib.GetString("equip_cross") , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
        heroName:SetAnchorPoint(1,0.5)
        heroName:SetPosition(self.Text_heroName:GetContentWidth() , self.Text_heroName:GetContentHeight() / 2)
        self.Text_heroName:AddChild(heroName)

        self.Button_change:SetEnabled(false)
    end


    -- 2017年4月14日14:45:22
    -- 1362 圣器背包的圣器要显示全部属性

    local t2 = CallModuleFunc(ModuleType.TREASURE, "GetLiftPropPlusInfos",data)

    for i,v in ipairs(t2) do
        local type_ , value_ = Util:Nature(v.type,v.value,true,true)
        local tab = {
                        {zi = type_ .. CPPGameLib.GetString("common_colon") , ys = Macros.Color.keypoint , dx = Macros.TypefaceSize.slightly},
                        {zi = value_ , ys = Macros.Color.content , dx = Macros.TypefaceSize.slightly},
                    }
                    
        local property = Util:LabCenter( tab )
        property:SetAnchorPoint(0,0.5)
        property:SetPosition( self["Text_property"..i]:GetContentWidth() ,  self["Text_property"..i]:GetContentHeight() / 2 )
        self["Text_property"..i]:AddChild(property)

        if i == 2 and data.treasure_reinfo then
            local treasure_reinfo = Util:Label( CPPGameLib.GetString("equip_lot") , Macros.TypefaceSize.slightly , Macros.Color.green)
            treasure_reinfo:SetAnchorPoint( 1 , 0.5)
            treasure_reinfo:SetPosition( 230 , self["Text_property"..i]:GetContentHeight()/2)
            treasure_reinfo:SetShadow(true)
            treasure_reinfo:SetShadowColor(0, 0, 0, 255)
            self["Text_property"..i]:AddChild(treasure_reinfo)
        end
    end

    -- --精炼等级
    -- if data.refine_level ~= 0 then
    --     local shuxing4 = Util:LabCenter({
    --                                     {zi = CPPGameLib.GetString("equip_refineLv") , ys = Macros.Color.keypoint , dx = Macros.TypefaceSize.slightly},
    --                                     {zi = data.refine_level , ys = Macros.Color.content , dx = Macros.TypefaceSize.slightly},
    --                                 })
    --     shuxing4:SetAnchorPoint(0,0.5)
    --     if #t2 >= 3 then
    --         shuxing4:SetPosition(0,self.Text_property4:GetContentHeight()/2)
    --         self.Text_property4:AddChild(shuxing4)   
    --     else
    --         shuxing4:SetPosition(0,self.Text_property3:GetContentHeight()/2)
    --         self.Text_property3:AddChild(shuxing4)   
    --     end
    -- end


    local bool = CallModuleFunc( ModuleType.HERO , "GetLineupEquipDress" , data.index , data.item_type , Macros.Game.ObjType.TREASURE )
    if bool then
        self.Button_change:SetText( CPPGameLib.GetString("equip_btn_change") )
    end

end

function Modules.TemplTreasureCrossItem:GetTouchNode()
    return self.Button_change
end

function Modules.TemplTreasureCrossItem:CheckSectionOperateSP()
    --新手指引特殊操作检测(圣器穿戴（格子索引，从1开始）)
    CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.TREASURE_WEAR, self.Button_change, self:GetViewIndex())
end
