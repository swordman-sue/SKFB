
local SelectMode = {
    SAME_QUALITY = 0,
    SAME_PROFESSION = 1,
}

--选择材料列表
function Modules.HeroBreakTabView:__InitMaterialList()
    self.__panel_material = self.__parent_view:GetWidget("Panel_UpgradeMaterial")

    --标题
    Util:WidgetButton(WidgetUnity.GetWidgetByName(self.__panel_material, "Button_1_0"), CPPGameLib.GetString("common_select_list"),nil,Macros.TypefaceSize.largish,Macros.Color.btn,Macros.Color.button_yellow)

    local btn = WidgetUnity.GetWidgetByName(self.__panel_material, "Button_screen_0")
    btn:SetVisible(false)

    --确定
    Util:WidgetButton(WidgetUnity.GetWidgetByName(self.__panel_material, "Button_sell_0") , CPPGameLib.GetString("common_confirm"), function()
        --关闭选择材料列表
        self:__CloseMaterialsList()
    end,Macros.TypefaceSize.largish, Macros.Color.btn, Macros.Color.button_yellow)
end

--打开选择材料列表
function Modules.HeroBreakTabView:__OpenMaterialsList(hero_info, select_mode, need_select_num, material_info_index)
    self.__select_mode = select_mode
    self.__need_select_num = need_select_num
    self.__material_info_index = material_info_index
    self.__is_search_same_quality_hero = self.__select_mode == SelectMode.SAME_QUALITY

    local _, _, _, _, cost_same_quality_hero_num, cost_same_profession_hero_num = ConfigAdapter.Hero.GetBreakLevelupInfo(hero_info)
    cost_same_quality_hero_num = self.__is_search_same_quality_hero and cost_same_quality_hero_num or 0
    cost_same_profession_hero_num = not self.__is_search_same_quality_hero and cost_same_profession_hero_num or 0
    
    local _, cost_same_quality_hero_list, cost_same_profession_hero_list = self:CallModuleFunc("GetBreakCostHeroList", hero_info.uid, 0, cost_same_quality_hero_num, cost_same_profession_hero_num, true)
    local data_list = self.__is_search_same_quality_hero and cost_same_quality_hero_list or cost_same_profession_hero_list
    if not data_list or #data_list <= 0 then
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString(self.__is_search_same_quality_hero and "hero_break_tips3" or "hero_break_tips4"))
        return
    end

    self.__parent_view_close_func = self.__parent_view:GetTitleBarCloseFunc()
    self.__parent_view:SetTitleBarCloseFunc(function()
        self:__CloseMaterialsList()
    end)
    self.__panel_material:SetVisible(true)
    self.__panel_main:SetVisible(false)

    if not self.__material_scv then
        local function calc_sel_num()
            local selected_cost_hero_map = 
            self.__is_search_same_quality_hero and 
            self.__selected_cost_same_quality_hero_map or self.__selected_cost_same_profession_hero_map
            
            local num = 0
            for _, _ in pairs(selected_cost_hero_map) do
                num = num + 1
            end
            return num
        end

        --检测是否选中
        local function check_selected_func(hero_uid)
            local hero_info = CallModuleFunc(ModuleType.HERO, "GetHeroInfo", hero_uid)
            if not hero_info then
                return false
            end

            local selected_cost_hero_map = 
            self.__is_search_same_quality_hero and 
            self.__selected_cost_same_quality_hero_map or self.__selected_cost_same_profession_hero_map

            return selected_cost_hero_map[hero_info.uid] ~= nil
        end

        --单选框选择回调
        local function cbox_callback(cbox, hero_uid, selected)
            local hero_info = CallModuleFunc(ModuleType.HERO, "GetHeroInfo", hero_uid)
            if not hero_info then
                return
            end

            if selected then
                local sel_num = calc_sel_num()
                if sel_num >= self.__need_select_num then
                    cbox:SetSelectedState(false)
                    GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_break_tips5")) 
                    return
                end
            end

            local selected_cost_hero_map = 
            self.__is_search_same_quality_hero and 
            self.__selected_cost_same_quality_hero_map or self.__selected_cost_same_profession_hero_map

            selected_cost_hero_map[hero_uid] = selected and true or nil
        end

        --选择列表
        local panel_material_list = WidgetUnity.GetWidgetByName(self.__panel_material, "Image_scrollviewDeepBg")
        local params =  
        {
            item_class = LogicGUI.HeroBreakMaterialItem,
            item_width = 455,
            item_height = 120,
            row = 4,
            col = 2,
            item_space_r = 5,
            item_space_c = 5,
            view_height = panel_material_list:GetContentHeight() - 10,
            filter_func = function(data)
                --过滤已被互斥方选择的英雄
                local opposite_selected_cost_hero_map = self.__is_search_same_quality_hero and 
                self.__selected_cost_same_profession_hero_map or self.__selected_cost_same_quality_hero_map
                return not opposite_selected_cost_hero_map[data.uid]
            end,
            structre_params = {cbox_callback, check_selected_func},
        }
        self.__material_scv = GUI.ScrollView.New(params)
        panel_material_list:AddChild(self.__material_scv:GetNode())
        PosTool.Center(self.__material_scv)
    end
    self.__material_scv:SetDataList(data_list)
end

--关闭选择材料列表
function Modules.HeroBreakTabView:__CloseMaterialsList()
    self.__parent_view:SetTitleBarCloseFunc(self.__parent_view_close_func)
    self.__panel_material:SetVisible(false)
    self.__panel_main:SetVisible(true)

    --刷新材料数量
    local material_info = self.__material_info_list[self.__material_info_index]
    if material_info then
        --材料数量
        if self.__is_search_same_quality_hero then
            cost_obj_own_num = CPPGameLib.GetTableLength(self.__selected_cost_same_quality_hero_map)

        elseif self.__select_mode == SelectMode.SAME_PROFESSION then
            cost_obj_own_num = CPPGameLib.GetTableLength(self.__selected_cost_same_profession_hero_map)
        end

        if cost_obj_own_num >= self.__need_select_num then
            --材料充足
            local num_str = string.format("%d/%d", cost_obj_own_num, self.__need_select_num)
            num_str = GUI.RichText.GetColorWord(num_str, Macros.Color.front_color_hex)
            material_info.text_num:SetText(num_str)
        else
            --材料不足
            local own_num_str = GUI.RichText.GetColorWord(tostring(cost_obj_own_num), Macros.Color.red_hex)
            local num_str = GUI.RichText.GetColorWord(string.format("%s/%d", own_num_str, self.__need_select_num), Macros.Color.keypoint_hex)
            material_info.text_num:SetText(num_str)
        end         
    end
end

--------------------------------------------------------------------------------------------------------------------------------
--英雄进阶材料条目
--------------------------------------------------------------------------------------------------------------------------------
LogicGUI.HeroBreakMaterialItem = LogicGUI.HeroBreakMaterialItem or BaseClass(GUI.ScrollItem)

function LogicGUI.HeroBreakMaterialItem:__init(cbox_callback, check_selected_func)
    self:InitTemplate(GUI.TemplateRegistry.SALE_ITEM)   

    self:__InitUI()
    self.__cbox_callback = cbox_callback
    self.__check_selected_func = check_selected_func
end

function LogicGUI.HeroBreakMaterialItem:__delete()
end

function LogicGUI.HeroBreakMaterialItem:__InitUI()
    --名字
    self.__text_name = Util:WidgetLabel(self:GetWidget("Text_sellName"))

    --经验
    self.__text_exp = self:GetWidget("Text_price")
    self.__text_exp:SetVisible(false)

    --等级
    self.__text_lev = self:GetWidget("Text_intensify")

    --图标
    local panel_icon = self:GetWidget("Panel_icon") 
    self.__icon = LogicGUI.IconLayer.New(false, false)
    panel_icon:AddChild(self.__icon:GetNode())
    PosTool.Center(self.__icon)

    --选择框
    self.Panel_cbox = self:GetWidget("Panel_cbox") 
    local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_select_no")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_select_yes")
    self.__cbox = UICheckBoxEx.CreateWithImage(sp_1,sp_2)
    self.Panel_cbox:AddChild(self.__cbox)
    PosTool.Center(self.__cbox)
    WidgetUnity.SetCheckboxListener(self.__cbox, 
        function()
            --选中
            self.__cbox_callback(self.__cbox, self.__data.uid, true)
        end,
        function()
            --反选
            self.__cbox_callback(self.__cbox, self.__data.uid, false)
        end)

    self.__check_selected_func = check_selected_func    
end

function LogicGUI.HeroBreakMaterialItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)

    local hero_info = CallModuleFunc(ModuleType.HERO, "GetHeroInfo", self.__data.uid)

    local config_hero = CPPGameLib.GetConfig("Hero", hero_info.info_id)

    --头像
    self.__icon:SetData({id = hero_info.info_id})

    --名字
    Util:WidgetLabel(self.__text_name, config_hero.name, nil, Macros.Quality[hero_info.quality_level], nil, nil, Macros.Quality_Stroke[hero_info.quality_level])

    --等级
    local level = Util:LabCenter({
                                {zi = CPPGameLib.GetString("common_lev1") , dx = Macros.TypefaceSize.normal , ys = Macros.Color.keypoint},
                                {zi = hero_info.level , dx = Macros.TypefaceSize.normal,ys = Macros.Color.content},
                            })
    level:SetAnchorPoint(0, 0.5)
    self.__text_lev:AddChild(level)

    --已选处理
    local var = self.__check_selected_func(hero_info.uid)
    self.__cbox:SetSelectedState(var)    
end
