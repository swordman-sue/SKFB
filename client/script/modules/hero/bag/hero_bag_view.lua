
--[[
%% @module: 英雄背包界面
%% @author: swordman sue
%% @created: 2016-08-23
--]]

Modules = Modules or {}

Modules.HeroBagView = Modules.HeroBagView or BaseClass(BaseView)

function Modules.HeroBagView:__init()
    self.__layout_name = "equip"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("hero_pack"),
        show_diamond = true,
        show_gold = true,
        show_stamina = true,
        show_energy = true,
        close_func = function()
            self:CloseManual()
        end,
    }

    local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no")
    self.__tab_params = {
        {sp_2,sp_1},
        {
            CPPGameLib.GetString("hero"),
            CPPGameLib.GetString("hero_chip")
        },
        10,
        CPPGameLib.Handler(self, self.OnYeMei),
        false
    }

    --背包
    self.pack = nil
    self.bebris = nil

    self.__open_callback = function()
        self:__OpenCallback()
    end

    --列表条目
    self:__AddPreloadTemplate(GUI.TemplateRegistry.EQUIP_PROP_ITEM)
    self:__AddPreloadTemplate(GUI.TemplateRegistry.EQUIP_DEDRIS_ITEM)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_PROMOTE)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_BAG , true )
end

function Modules.HeroBagView:__OpenCallback()
    self.heroGoods = nil
    self.heroDebri = nil

    self:__Getwidget()
    self:__ShowUI()
end

function Modules.HeroBagView:__RegistAllEvents()
    --增加新英雄
    self:BindGlobalEvent(Macros.Event.HeroModule.HERO_ADD, function ( _, info )
        if self.index == 1 then
            self:UpdateHeros(info,"add")
        else
            self.__is_hero_list_dirty = true
        end
    end)

    --删除英雄
    self:BindGlobalEvent(Macros.Event.HeroModule.HERO_REMOVE, function ( _, info )
        if self.index == 1 then
            self:UpdateHeros(info,"remove")
        else
            self.__to_del_hero_list = self.__to_del_hero_list or {}
            table.insert(self.__to_del_hero_list, data)
        end
    end)

    --刷新英雄碎片
    self:BindGlobalEvent(Macros.Event.HeroModule.HERO_UP_DEBRIS, function ( _, info )
        if self.index == 2 then
            self.heroDebri:SetCell()
        else
            self.__is_hero_debris_list_dirty = true
        end
    end)

    --打开获取途径
    self:BindGlobalEvent(Macros.Event.ApproachOfAchievingModule.OPEN_APPROACHVIEW, function(_, info_id)
        self.__open_approach_info_id = info_id
    end)

    --红点更新
    self:BindGlobalEvent(Macros.Event.RedModule.RED_UPDATE , function ( _ , system_id)
        if system_id == Macros.Game.SystemID.HERO_BAG then
            self:UpdateRedDot( )
        end
    end)    
end

function Modules.HeroBagView:__Dispose()
    if self.heroGoods then
        self.heroGoods:DeleteMe()
        self.heroGoods = nil
    end

    if self.heroDebri then
        self.heroDebri:DeleteMe()
        self.heroDebri = nil
    end
end

function Modules.HeroBagView:__Getwidget( ... )
    --内容框
    self.Image_content = self:GetWidget("Image_content") 

    --穿戴容器
    self.Panel_changePack = self:GetWidget("Panel_changePack") 
    self.Panel_changePack:SetVisible(false)
    --穿戴组件
    self.Text_changeTips = self:GetWidget("Text_changeTips") 
    --穿戴组件
    self.Panel_cBox = self:GetWidget("Panel_cBox") 

    --列表容器
    self.Panel_Backpack = self:GetWidget("Panel_Backpack")
    self.Panel_Backpack:SetVisible(true)
    
    self.Image_showMax = self:GetWidget("Image_showMax")
    --出售按钮
    self.Button_sell = Util:WidgetButton( self:GetWidget("Button_sell") , nil , CPPGameLib.Handler(self,self.OnSell) )
    --数量上限
    self.Button_showMax = Util:WidgetButton( self:GetWidget("Button_showMax") , nil , function ( ... )
        LogicGUI.TemplBuyBagMax.New( Macros.Game.VIPType.HERO_PACK_MAX )
    end )
    self.Button_showMax:SetVisible(true)

    local lv , max = CallModuleFunc(ModuleType.VIP, "ExtendPack", Macros.Game.VIPType.HERO_PACK_MAX  )
    if lv and max then
      self.Button_showMax:SetVisible(false)
    end

    --页签
    self.Panel_Backpackcbx = self:GetWidget("Panel_Backpackcbx")

    self.Panel_9 = self:GetWidget("Panel_9")
end

function Modules.HeroBagView:__ShowUI( ... )
    local init_tab, open_approach_info_id = unpack(self.__open_data)       

    --上限显示
    local showMax = Util:Label(" ",nil,Macros.Color.lilyWhite )
    self.Image_showMax:AddChild(showMax)
    PosTool.Center(showMax)

    --装备列表
    self.heroGoods = Modules.HeroGoods.New(showMax)
    self.Image_content:AddChild(self.heroGoods:GetNode())
    PosTool.Center(self.heroGoods:GetNode())

    --碎片列表
    self.heroDebri = Modules.HeroDebris.New(self)
    self.Image_content:AddChild(self.heroDebri:GetNode())
    PosTool.Center(self.heroDebri:GetNode())

    --页签
    self.__tab:On(init_tab or 1)
    self.__tab:SetPosition(0 , 0)
    self.__tab:SetCboxColor( Macros.Color.button_yellow , Macros.Color.button_red )
    self.Panel_Backpackcbx:AddChild(self.__tab:GetNode())

    --红点
    self.icon_red_point = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point"))
    self.icon_red_point:SetPosition(self.__tab.btn_list[2]:GetContentWidth() - 10 , self.__tab.btn_list[2]:GetContentHeight() - 10 )
    self.__tab.btn_list[2]:AddChild(self.icon_red_point , 20)
    self:UpdateRedDot()

    if open_approach_info_id then   
        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, open_approach_info_id)   
    end
end

function Modules.HeroBagView:OnYeMei( index )
    local com_sp_cbox_wire = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp_cbox_wire")
    if self.com_sp_cbox_wire then
        self.com_sp_cbox_wire:RemoveSelf(true)
        self.com_sp_cbox_wire = nil
    end
    self.com_sp_cbox_wire = Util:Sprite(com_sp_cbox_wire)
    self.__tab.btn_list[index]:AddChild( self.com_sp_cbox_wire )
    PosTool.Center( self.com_sp_cbox_wire , 0 , -30 )

    self.index = index
    self.heroGoods:SetVisible(false)
    self.heroDebri:SetVisible(false)
    self.Button_showMax:SetVisible(false)

    if self.index == 1 then
        self.heroGoods:SetVisible(true)
        self.Button_showMax:SetVisible(true)

        --刷新英雄列表
        if self.__is_hero_list_dirty then
            self.__is_hero_list_dirty = false
            self.__to_del_hero_list = nil
            self:UpdateHeros(nil,"add")

        elseif self.__to_del_hero_list then
            for _, data in ipairs(self.__to_del_hero_list) do
                self:UpdateHeros(data,"remove")
            end
            self.__to_del_hero_list = nil
        end

    else
        self.heroDebri:SetVisible(true)

        --刷新碎片列表
        if self.__is_hero_debris_list_dirty then
            self.__is_hero_debris_list_dirty = false
            self.heroDebri:SetCell()
        end
    end
end

--出售
function Modules.HeroBagView:OnSell()
    local type_ = nil
    if self.index == 1 then
      type_ = Macros.Game.SELECT_TYPE.HERO
    elseif self.index == 2 then
      type_ = Macros.Game.SELECT_TYPE.HERO_DEBRIS
    end
    CallModuleFunc(ModuleType.DEBRISELL, "OpenView", BaseViewType.DEBRISELL,type_)
end

--刷新英雄
function Modules.HeroBagView:UpdateHeros( info , type )
    self.heroGoods:SetCell(info , type)
end

--更新红点
function Modules.HeroBagView:UpdateRedDot()
    if self.icon_red_point then
        self.icon_red_point:SetVisible(CallModuleFunc(ModuleType.RED_MGR , "GetRed", Macros.Game.SystemID.HERO_BAG ) > 0)
    end
end

--获取打开参数
function Modules.HeroBagView:GetOpenParams()
    if self.index then
        return {self.index, self.__open_approach_info_id}
    end
end
