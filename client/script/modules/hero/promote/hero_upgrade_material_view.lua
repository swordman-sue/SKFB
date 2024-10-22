
--选择材料列表
function Modules.HeroUpgradeTabView:__InitMaterialList()
    self.__panel_material = self.__parent_view:GetWidget("Panel_UpgradeMaterial")

    --标题
    Util:WidgetButton(WidgetUnity.GetWidgetByName(self.__panel_material, "Button_1_0"), CPPGameLib.GetString("common_select_list"),nil,Macros.TypefaceSize.largish,Macros.Color.btn,Macros.Color.button_yellow)

    --升级所需
    local text_temp = Util:WidgetLabel(WidgetUnity.GetWidgetByName(self.__panel_material, "Text_onNum_0"), CPPGameLib.GetString("hero_upgrade_need_exp"), Macros.TypefaceSize.normal, Macros.Color.keypoint)
    self.__txt_material_need_exp = Util:Label("0",Macros.TypefaceSize.normal,Macros.Color.content)
    self.__panel_material:AddChild(self.__txt_material_need_exp)
    PosTool.RightTo(text_temp, self.__txt_material_need_exp, 2, 0, 0, 0.5)
   
    --获得经验
    text_temp = Util:WidgetLabel(WidgetUnity.GetWidgetByName(self.__panel_material, "Text_gain_0"), CPPGameLib.GetString("hero_upgrade_add_exp"), Macros.TypefaceSize.normal, Macros.Color.keypoint)
    self.__txt_material_add_exp = Util:Label("0",Macros.TypefaceSize.normal, Macros.Color.content)
    self.__panel_material:AddChild(self.__txt_material_add_exp)
    PosTool.RightTo(text_temp, self.__txt_material_add_exp, 2, 0, 0, 0.5)

    local btn = WidgetUnity.GetWidgetByName(self.__panel_material, "Button_screen_0")
    btn:SetVisible(false)

    --确定
    Util:WidgetButton(WidgetUnity.GetWidgetByName(self.__panel_material, "Button_sell_0") , CPPGameLib.GetString("common_confirm"), function()
        --关闭选择材料列表
        self:__CloseMaterialsList()
    end,Macros.TypefaceSize.largish, Macros.Color.btn, Macros.Color.button_yellow)
end

--打开选择材料列表
function Modules.HeroUpgradeTabView:__OpenMaterialsList()
    --检测等级上限
    if self:__CheckUpperLimit() then
        return
    end

    local data_list = self:CallModuleFunc("GetUpgradeMaterialList")
    if not data_list or #data_list <= 0 then
        --获取途径
        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, ConfigAdapter.Common.GetPurpleExpHero())
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_upgrade_tips5"))
        return
    end

    self.__parent_view_close_func = self.__parent_view:GetTitleBarCloseFunc()
    self.__parent_view:SetTitleBarCloseFunc(function()
        self:__CloseMaterialsList()
    end)
    self.__panel_material:SetVisible(true)
    self.__panel_main:SetVisible(false)

    --展示已选的材料
    self.__material_sel_map = {}
    for _, material_info in ipairs(self.__material_info_list) do
        if material_info.hero_uid then
            self.__material_sel_map[material_info.hero_uid] = true
        end
    end

    if not self.__material_scv then
        --计算已选材料获得的经验
        self.__material_calc_add_exp = function()
            local exp = 0
            local hero_info, hero_config
            for hero_uid, _ in pairs(self.__material_sel_map) do
                hero_info = self:CallModuleFunc("GetHeroInfo", hero_uid)
                exp = exp + Modules.HeroModule.GetHeroExp(hero_info)
            end
            return exp
        end

        --计算已选材料数量
        local function calc_sel_num()
            local num = 0
            for _, _ in pairs(self.__material_sel_map) do
                num = num + 1
            end
            return num
        end

        --检测材料是否已选
        local function check_selected_func(hero_info)
            if not hero_info then
                return false
            end
            return self.__material_sel_map[hero_info.uid] ~= nil
        end

        --材料选择/反选回调
        local function cbox_callback(cbox, hero_info, selected)
            if selected then
                local sel_num = calc_sel_num()
                if sel_num >= Modules.HeroModule.MaxUpgradeMaterialNum then
                    --最多只能选择5个英雄宝宝
                    cbox:SetSelectedState(false)
                    GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_upgrade_tips4")) 
                    return
                end
            end

            local pre_add_exp
            local limit_exp = self:__GetUpperLimitExp()

            if selected then
                pre_add_exp = self.__material_calc_add_exp()
            end
            self.__material_sel_map[hero_info.uid] = selected and true or nil
            local add_exp = self.__material_calc_add_exp()

            --获得经验
            self.__txt_material_add_exp:SetString(tostring(add_exp))
        end

        --选择列表
        local panel_material_list = WidgetUnity.GetWidgetByName(self.__panel_material, "Image_scrollviewDeepBg")
        local params =  
        {
            item_class = LogicGUI.HeroUpgradeMaterialItem,
            item_width = 455,
            item_height = 120,
            row = 4,
            col = 2,
            item_space_r = 5,
            item_space_c = 5,
            view_height = panel_material_list:GetContentHeight() - 10 ,
            structre_params = {cbox_callback, check_selected_func},
        }
        self.__material_scv = GUI.ScrollView.New(params)
        panel_material_list:AddChild(self.__material_scv:GetNode())
        PosTool.Center(self.__material_scv)
    end
    self.__material_scv:SetDataList(data_list)

    --新手指引特殊操作检测(英雄升级选择材料)
    local material_item = self.__material_scv:GetItem(1)
    if material_item then
        material_item:CheckSectionOperateSP()
    end

    --获得经验
    local add_exp = self.__material_calc_add_exp()
    self.__txt_material_add_exp:SetString(tostring(add_exp))
end

--关闭选择材料列表
function Modules.HeroUpgradeTabView:__CloseMaterialsList()
    self.__parent_view:SetTitleBarCloseFunc(self.__parent_view_close_func)
    self.__panel_material:SetVisible(false)
    self.__panel_main:SetVisible(true)

    --添加选中的材料
    local cost_hero_list = {}
    for hero_uid, _ in pairs(self.__material_sel_map) do
        local cost_hero_info = self:CallModuleFunc("GetHeroInfo", hero_uid)
        table.insert(cost_hero_list, cost_hero_info)
    end
    
    self:__AddMaterials(cost_hero_list, false)
end

--------------------------------------------------------------------------------------------------------------------------------
--英雄材料条目
--------------------------------------------------------------------------------------------------------------------------------
LogicGUI.HeroUpgradeMaterialItem = LogicGUI.HeroUpgradeMaterialItem or BaseClass(GUI.ScrollItem)

function LogicGUI.HeroUpgradeMaterialItem:__init(cbox_callback, check_selected_func)
    self:InitTemplate(GUI.TemplateRegistry.SALE_ITEM)   

    self:__InitUI()
    self.__cbox_callback = cbox_callback
    self.__check_selected_func = check_selected_func
end

function LogicGUI.HeroUpgradeMaterialItem:__delete()
end

function LogicGUI.HeroUpgradeMaterialItem:__InitUI()
    --名字
    self.__text_name = Util:WidgetLabel(self:GetWidget("Text_sellName"))

    --经验
    self.__text_exp = self:GetWidget("Text_price")

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
            self.__cbox_callback(self.__cbox, self.__data, true)
        end,
        function()
            --反选
            self.__cbox_callback(self.__cbox, self.__data, false)
        end)

    self.__check_selected_func = check_selected_func    
end

function LogicGUI.HeroUpgradeMaterialItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)

    local config_hero = CPPGameLib.GetConfig("Hero", self.__data.info_id)

    --头像
    self.__icon:SetData({id = self.__data.info_id})

    --名字
    Util:WidgetLabel(self.__text_name, config_hero.name, nil, Macros.Quality[self.__data.quality_level], nil, nil, Macros.Quality_Stroke[self.__data.quality_level])

    --等级
    local level = Util:LabCenter({
                                {zi = CPPGameLib.GetString("common_lev1") , dx = Macros.TypefaceSize.normal , ys = Macros.Color.keypoint},
                                {zi = self.__data.level , dx = Macros.TypefaceSize.normal,ys = Macros.Color.content},
                            })
    level:SetAnchorPoint(0, 0.5)
    self.__text_lev:AddChild(level)

    --经验
    local to_exp = Modules.HeroModule.GetHeroExp(self.__data)
    local exp = Util:LabCenter({
                                {zi = CPPGameLib.GetString("common_exp1") , dx = Macros.TypefaceSize.normal , ys = Macros.Color.keypoint },
                                {zi = to_exp , dx = Macros.TypefaceSize.normal,ys = Macros.Color.content },
                            })
    exp:SetAnchorPoint(0.5,0.5)
    self.__text_exp:AddChild(exp)

    --已选处理
    local var = self.__check_selected_func(self.__data)
    self.__cbox:SetSelectedState(var)    
end

--新手指引特殊操作检测(英雄升级选择材料)
function LogicGUI.HeroUpgradeMaterialItem:CheckSectionOperateSP()
    CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.HERO_UPGRADE_SELECT_MATERIAL, self.__cbox)     
end

