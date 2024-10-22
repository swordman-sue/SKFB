--
-- @brief 圣器强化选择界面
-- @author: yjg
-- @date: 2016年9月5日09:52:47
--

Modules = Modules or {}
Modules.TreasureDevour = Modules.TreasureDevour or BaseClass(BaseView)

function Modules.TreasureDevour:__init(data)
    self.__layout_name = "sale"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
    self.__title_bar_params = 
    {
      show_diamond = true,
      show_gold = true,
      show_stamina = true,
      show_energy = true,
      close_func = function()
        self:OnButtonSell()
      end,
    }

    self.__open_callback = function()
        self:__OpenCallback()
    end  

    local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
    self.__tab_params = {
        {sp_2,sp_1},
        {  
            CPPGameLib.GetString("treasure_onbread"), 
        },10,CPPGameLib.Handler(self, self.OnYeMei),false
    }

    self:__AddPreloadTemplate(GUI.TemplateRegistry.SALE_ITEM)
end

function Modules.TreasureDevour:__OpenCallback()   
    self:__Getwidget()
    self:__ShowUI()
    self:__SetShowUi()
end

function Modules.TreasureDevour:__delete()
    if self.__zones_scrollview then
        self.__zones_scrollview:DeleteMe()
        self.__zones_scrollview = nil
    end
end

function Modules.TreasureDevour:__RegistAllEvents()
    --获得装备
    self:BindGlobalEvent(Macros.Event.TreasureModule.TREASURE_SELECT,function (_, data)
        self:__SetShowUi()
    end)
end

function Modules.TreasureDevour:__Getwidget( ... )
    --上背景
    self.Panel_Backpackcbx = self:GetWidget("Panel_Backpackcbx") 
    --内容背景
    self.Image_content = self:GetWidget("Image_content") 
    --列表背景
    self.Image_5 = self:GetWidget("Image_5") 
    --选中数量
    self.Text_onNum_0 = self:GetWidget("Text_onNum_0") 
    --出售可获得
    self.Text_gain_0 = self:GetWidget("Text_gain_0") 

    --筛选
    self.Button_screen_0 =  Util:WidgetButton( self:GetWidget("Button_screen_0") , CPPGameLib.GetString("sell_pinzhishaixuan") , CPPGameLib.Handler(self,self.OnButtonScreen) )
    self.Button_screen_0:SetVisible(false)
    --出售
    self.Button_sell_0 = Util:WidgetButton( 
                                            self:GetWidget("Button_sell_0") , 
                                            CPPGameLib.GetString("common_confirm") , 
                                            CPPGameLib.Handler(self,self.OnButtonSell) ,
                                            nil,nil,Macros.Color.button_yellow
                                            )
end

function Modules.TreasureDevour:__ShowUI( ... )

    self.__title_bar:SetTitle( CPPGameLib.GetString("treasure_onbread") )
    
    self.__tab:SetPosition(0 , 10)
    self.Panel_Backpackcbx:AddChild(self.__tab:GetNode())
    self.__tab:On( 1 )

    local params =  
    {
        item_class = Modules.TreasureIntensifyMaterialItem,
        item_width = 455.5,
        item_height = 120,
        row = 4,
        col = 2,
        item_space_r = 2.5,
        item_space_c = 2.5,
        view_height = self.Image_5:GetContentHeight() - 10 ,
    }
    -- 创建参数
    self.__zones_scrollview = GUI.ScrollView.New(params)
    self.Image_5:AddChild(self.__zones_scrollview:GetNode())
    PosTool.Center(self.__zones_scrollview:GetNode())
    self.__zones_scrollview:SetDataList( CallModuleFunc(ModuleType.TREASURE, "GetStrengthenMaterialList") )

    --已经选择数量
    local yixuanshuliang = Util:Label( CPPGameLib.GetString("hero_upgrade_need_exp") , nil ,Macros.Color.keypoint)
    yixuanshuliang:SetAnchorPoint(0,0.5)
    yixuanshuliang:SetPosition(0,self.Text_onNum_0:GetContentHeight()/2)
    self.Text_onNum_0:AddChild(yixuanshuliang)

    self.onNum = Util:Label( "000" , nil ,Macros.Color.content)
    PosTool.RightTo(yixuanshuliang,self.onNum)
    self.Text_onNum_0:AddChild(self.onNum)

    --可获得经验
    local huodejingyan = Util:Label( CPPGameLib.GetString("hero_upgrade_add_exp") , nil ,Macros.Color.keypoint)
    huodejingyan:SetAnchorPoint(0,0.5)
    yixuanshuliang:SetPosition(0,self.Text_onNum_0:GetContentHeight()/2)
    self.Text_gain_0:AddChild(huodejingyan)

    self.gain = Util:Label( "000" , nil ,Macros.Color.content)
    PosTool.RightTo(huodejingyan,self.gain)
    self.Text_gain_0:AddChild(self.gain)
end

function Modules.TreasureDevour:__SetShowUi()
    local _, gold = CallModuleFunc(ModuleType.TREASURE, "GetSelectedStrengthenMaterialInfo")
    local _ , _, nowExp = CallModuleFunc(ModuleType.TREASURE, "CalculateUpgrade")

    self.onNum:SetString(nowExp)
    self.gain:SetString(gold)
end

--确定
function Modules.TreasureDevour:OnButtonSell( ... )
    local onData = CallModuleFunc(ModuleType.TREASURE, "GetOnTreasureInfo")
    GlobalEventSystem:FireNextFrame(Macros.Event.TreasureModule.TREASURE_SELECT_TAB)
    self:CloseManual()
end

function Modules.TreasureDevour:OnYeMei( index )
    local com_sp_cbox_wire = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp_cbox_wire")
    if self.com_sp_cbox_wire then
        self.com_sp_cbox_wire:RemoveSelf(true)
        self.com_sp_cbox_wire = nil
    end
    self.com_sp_cbox_wire = Util:Sprite(com_sp_cbox_wire)
    self.__tab.btn_list[index]:AddChild( self.com_sp_cbox_wire )
    PosTool.Center( self.com_sp_cbox_wire , 0 , -35 )
end


Modules = Modules or {}
Modules.TreasureIntensifyMaterialItem = Modules.TreasureIntensifyMaterialItem or BaseClass(GUI.ScrollItem)

function Modules.TreasureIntensifyMaterialItem:__init()
    self:InitTemplate(GUI.TemplateRegistry.SALE_ITEM)   
end

function Modules.TreasureIntensifyMaterialItem:__delete()

end

function Modules.TreasureIntensifyMaterialItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self:__Getwidget()
    self:__ShowUI(data)
end

function Modules.TreasureIntensifyMaterialItem:__Getwidget()
    --条目
    self.Image_sellItem = self:GetWidget("Image_sellItem") 
    --条目容器
    self.Panel_sellItem = self:GetWidget("Panel_sellItem") 
    --名字
    self.Text_sellName = self:GetWidget("Text_sellName") 
    --强化&属性
    self.Text_intensify = self:GetWidget("Text_intensify") 
    --价钱
    self.Text_price = self:GetWidget("Text_price") 
    --图标
    self.Panel_icon = self:GetWidget("Panel_icon") 
    --选择框
    self.Panel_cbox = self:GetWidget("Panel_cbox") 
end

function Modules.TreasureIntensifyMaterialItem:__ShowUI(data)
    local treasure_info = CallModuleFunc(ModuleType.TREASURE, "GetTreasure", data.uid)

    local type_ , info = ObjIDTool.GetInfo(treasure_info.info_id)
    -- 头像
    local icon = LogicGUI.IconLayer.New(false)
    icon:SetPosition(self.Panel_icon:GetContentWidth()/2 ,self.Panel_icon:GetContentHeight()/2)
    self.Panel_icon:AddChild(icon:GetNode())
    local iconData = { id = treasure_info.info_id }
    icon:SetData(iconData)

    --名字
    local name = Util:Name(info.name , Macros.TypefaceSize.largish , treasure_info.quality)
    name:SetAnchorPoint(0, 0.5)
    name:SetPosition(10,self.Text_sellName:GetContentHeight()/2)
    self.Text_sellName:AddChild(name)

    local price = Util:LabCenter( {
                                    {zi = CPPGameLib.GetString("equip_undergo") , dx = Macros.TypefaceSize.slightly , ys = Macros.Color.keypoint },
                                    {zi = data.exp , dx = Macros.TypefaceSize.slightly , ys = Macros.Color.content },
                                    } )
    price:SetAnchorPoint(0.5,0.5)
    price:SetPosition(self.Text_price:GetContentWidth()/2 , self.Text_price:GetContentHeight()/2 )
    self.Text_price:AddChild(price)

    local intensifyType = Util:Label(CPPGameLib.GetString("equip_intensifyLv") , nil , Macros.Color.keypoint )
    intensifyType:SetAnchorPoint(0,0.5)
    intensifyType:SetPosition(10 , self.Text_intensify:GetContentHeight() / 2 )
    self.Text_intensify:AddChild(intensifyType)

    local intensifyLv = Util:Label(treasure_info.strengthen_level , nil , Macros.Color.content)
    PosTool.RightTo(intensifyType , intensifyLv,5)
    self.Text_intensify:AddChild(intensifyLv)

    --选择框
    local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_select_no")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_select_yes")
    self.cbx = UICheckBoxEx.CreateWithImage(sp_1,sp_2)
    self.cbx:SetPosition(self.Panel_cbox:GetContentWidth()/2,self.Panel_cbox:GetContentHeight()/2)
    self.Panel_cbox:AddChild(self.cbx)
    self.cbx:SetEventListener(
        GlobalCallbackMgr:AddCallBackFunc(function()
            local selected_material_list = CallModuleFunc(ModuleType.TREASURE, "GetSelectedStrengthenMaterialMap")
            --已选材料可强化到的等级
            local _, _, _, upLevel = CallModuleFunc(ModuleType.TREASURE, "CalculateUpgrade")
            --选中的圣器
            local onTreasureInfo = CallModuleFunc(ModuleType.TREASURE, "GetOnTreasureInfo")
            --选中的圣器的上限等级
            local toliftMax = CallModuleFunc(ModuleType.TREASURE, "GetLiftMaxInfo", onTreasureInfo.quality).strengthenMax

            if CPPGameLib.GetTableLength(selected_material_list) >= 5 then
                self.cbx:SetSelectedState(false)
                GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("treasure_nobreadMax"))
            
            elseif upLevel >= toliftMax then
                self.cbx:SetSelectedState(false)
                GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("equip_intensifyMax"))
            else
                CallModuleFunc(ModuleType.TREASURE, "SetSelectedStrengthenMaterial", data , "add" )
            end
        end),
        GlobalCallbackMgr:AddCallBackFunc(function()
            CallModuleFunc(ModuleType.TREASURE, "SetSelectedStrengthenMaterial", data , "remove"  )
        end))    

    self.cbx:SetSelectedState(data.is_selected)
end