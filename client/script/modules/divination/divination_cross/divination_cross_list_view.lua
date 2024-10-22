--
-- @brief 卡牌穿戴穿戴列表
-- @author: yjg
-- @date: 2016年8月17日14:07:31
--

Modules = Modules or {}

Modules.DivinationCrossView = Modules.DivinationCrossView or BaseClass(BaseView)

function Modules.DivinationCrossView:__init()
    self.__layout_name = "equip"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("divination_cross"),
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
    -- self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.EQUIPPACK)
end

function Modules.DivinationCrossView:__Dispose()
    if self.lst_list then
        self.lst_list:DeleteMe()
        self.lst_list = nil
    end
end

function Modules.DivinationCrossView:__OpenCallback()    
    self:__Getwidget()
    self:__ShowUI()
end

function Modules.DivinationCrossView:__Getwidget( ... )
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

function Modules.DivinationCrossView:__ShowUI( ... )
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
    local lab = Util:Name(CPPGameLib.GetString("divination_hideCrossCard"),Macros.TypefaceSize.largish)
    PosTool.RightTo(cbx,lab)
    self.Panel_cBox:AddChild(lab)

    local params =  
    {
        item_class = Modules.TemplDivinationCrossItem,
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
                if item_data.data.hero_id == 0 then
                    return true
                end
            else
                return true
            end
        end,
        touch_callback = function(item_index, item_data)
            local info = {}
            info.formation_pos = unpack(self.__open_data).index
            info.card_slot = unpack(self.__open_data).item_type
            info.card_uid = item_data.data.uid

            CallModuleFunc(ModuleType.DIVINATION, "CollectionEquipCardReq",info)
            self:CloseManual()

            CallModuleFunc(ModuleType.HERO, "OpenView", BaseViewType.HERO_LINEUP, BaseViewSubType.HeroLineup.MAIN , item_data.data.index )
        end,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.Image_content:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode())

    self:Setlist()

    -- --监听指引下一个步骤事件
    -- self:BindGlobalEvent(Macros.Event.LeadModule.CHECK_NEXT_SECTION, function(_, pre_section_info)
    --     self.lst_list:ItemIter(function(item_index, item)
    --         item:CheckSectionOperateSP()
    --     end)
    -- end)
end

function Modules.DivinationCrossView:Setlist( ... )

    local typeLst = CallModuleFunc(ModuleType.HERO, "GetLineupDivinationInfo" , unpack(self.__open_data).index )
    local divinationLst = CallModuleFunc(ModuleType.DIVINATION, "GetDivinationSort")
    local cur_obj_type = CallModuleFunc(ModuleType.HERO,"GetHeroDivinationTypeByLineUpIndexAndPos" , unpack(self.__open_data).index,unpack(self.__open_data).item_type)
    local lst = {}
    for i,v in ipairs( divinationLst ) do
        local info = CPPGameLib.GetConfig("Card", v.info_id)
        if not typeLst[ info.type ] or cur_obj_type == info.type then
            table.insert(lst,v)
            lst[#lst].index = unpack(self.__open_data).index
            lst[#lst].item_type = info.type
        end
    end


    --宿命
    local cnf = CPPGameLib.GetConfig("Hero", unpack(self.__open_data).hero_id )
    local t1 = {}
    if cnf.relation_card_list then
        for _,v in ipairs(cnf.relation_card_list) do
            for __,vv in ipairs(v) do
                t1[vv] = cnf.relation_card_reinforcementdes_list[_]
            end
        end
    end

    local t2 = lst
    local t3 = {}
    local t4 = {}
    for i,v in ipairs(t2) do
        if v.type ~= 0 then
            if t1[v.type] then
                v.relation_card_reinforcementname_list = t1[v.type]
                table.insert(t3 , {data = v , fate = true} )
            else
                table.insert(t4 , {data = v , fate = false})
            end
        end
    end

    for i,v in ipairs(t4) do
        table.insert(t3 , v)
    end
    self.lst_list:SetDataList(t3)

end


Modules = Modules or {}
Modules.TemplDivinationCrossItem = Modules.TemplDivinationCrossItem or BaseClass(GUI.ScrollItem)

function Modules.TemplDivinationCrossItem:__init(data,fun)
    self:InitTemplate(GUI.TemplateRegistry.EQUIP_CROSS)  
    self:__Getwidget()
end

function Modules.TemplDivinationCrossItem:__delete()
end

function Modules.TemplDivinationCrossItem:__AddToParent()
end

function Modules.TemplDivinationCrossItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self.data = data
    self:__ShowUI(data)
end

-- function Modules.TemplDivinationCrossItem:SetViewIndex(var)
--     GUI.ScrollItem.SetViewIndex(self, var)

--     -- --新手指引特殊操作检测(圣器穿戴（格子索引，从1开始）)
--     -- CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.TREASURE_WEAR, self.Button_change, self:GetViewIndex())
-- end

function Modules.TemplDivinationCrossItem:__Getwidget( ... )
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

function Modules.TemplDivinationCrossItem:__ShowUI( data )
    local info = CPPGameLib.GetConfig("Card", data.data.info_id)

    -- 1746 圣器/装备精炼等级显示位置修改
    -- 重现步骤

    -- 圣器/装备在背包和替换列表中强化等级和精炼等级位置调整

    -- 1、强化等级：统一显示在图标左下角

    -- 2、精炼等级：统一显示在名字后面

    local level = ""
    if data.data.break_level ~= 0 then
        level = CPPGameLib.GetString("common_add4" , data.data.break_level ) 
    end
    --名字
    local name = Util:Name( info.name .." " .. level ,Macros.TypefaceSize.normal,info.quality)
    name:SetAnchorPoint(0,0.5)
    name:SetPosition(0,self.Text_name:GetContentHeight()/2)
    self.Text_name:AddChild(name)

    --图标
    local icon_layer = LogicGUI.IconLayer.New(true)
    icon_layer:SetData({id = data.data.info_id , obj_info = data.data })
    icon_layer:ShowDivinationStrengthenLev(true)
    self.Panel_icon:AddChild(icon_layer:GetNode())
    PosTool.Center(icon_layer)   

    --英雄名称
    if data.data.hero_id ~= 0 then
        local info = CPPGameLib.GetConfig("Hero", data.data.hero_id)
        local heroName = Util:Label(info.name..CPPGameLib.GetString("equip_cross") , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
        heroName:SetAnchorPoint(1,0.5)
        heroName:SetPosition(self.Text_heroName:GetContentWidth() , self.Text_heroName:GetContentHeight() / 2)
        self.Text_heroName:AddChild(heroName)

        Util:GreyButton( self.Button_change )
    end

    --宿命
    if data.fate == true then
        local Fate = Util:Name( CPPGameLib.GetString("divination_cardFate_1") , Macros.TypefaceSize.slightly , Macros.Game.QualityType.GREEN )
        Fate:SetAnchorPoint(1,0.5)
        Fate:SetPosition(self.Text_heroName:GetContentWidth() - 120 , self.Text_heroName:GetContentHeight() / 2)
        self.Text_heroName:AddChild(Fate)
    end

    -- local hero_info = CallModuleFunc(ModuleType.HERO, "GetLineupHeroInfo", info. )

    -- 2017年4月14日14:45:22
    -- 1362 圣器背包的圣器要显示全部属性
    local t2 = CallModuleFunc(ModuleType.DIVINATION, "ToInfoIdGetDivinationInfo", data.data.info_id , data.data.level , data.data.break_level )

    for i,v in ipairs(t2.property) do

        local type_ , value_ = Util:Nature( v.type , v.value ,true,true)
        local property = Util:LabCenter({
                                        {zi = type_ .. CPPGameLib.GetString("common_colon") , ys = Macros.Color.keypoint , dx = Macros.TypefaceSize.slightly},
                                        {zi = value_ , ys = Macros.Color.content , dx = Macros.TypefaceSize.slightly},
                                    })
        property:SetAnchorPoint(0,0.5)
        property:SetPosition( self["Text_property"..i]:GetContentWidth() ,  self["Text_property"..i]:GetContentHeight() / 2 )
        self["Text_property"..i]:AddChild(property)

        if i == 2 and data.data.treasure_reinfo then
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


    -- local bool = CallModuleFunc( ModuleType.HERO , "GetLineupEquipDress" , data.index , data.item_type , Macros.Game.ObjType.FAIRY )
    -- if bool then
    --     self.Button_change:SetText( CPPGameLib.GetString("equip_btn_change") )
    -- end

end

function Modules.TemplDivinationCrossItem:GetTouchNode()
    return self.Button_change
end

function Modules.TemplDivinationCrossItem:CheckSectionOperateSP()
    -- --新手指引特殊操作检测(圣器穿戴（格子索引，从1开始）)
    -- CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.TREASURE_WEAR, self.Button_change, self:GetViewIndex())
end
