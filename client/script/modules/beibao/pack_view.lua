--
-- @brief 物品背包 
-- @author: yjg
-- @date: 2016年8月9日17:23:47
--

Modules = Modules or {}

Modules.PackView = Modules.PackView or BaseClass(BaseView)

function Modules.PackView:__init()
  	self.__layout_name = "pack"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("pack"),
        show_diamond = true,
        show_gold = true,
        show_stamina = true,
        show_energy = true,
        close_func = function()
            self:CloseManual()
        end,
    }

    self.__tab_view_params = 
    {
        {Modules.TemplPackGoods, "Image_content" , 0 , 1 },
        {Modules.TemplPackAwaken, "Image_content", 0 , 2 },
    }

    --功能开启等级  
    self:__CountLockLevel()
    local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no")
    self.__tab_params = {
        {sp_2,sp_1},
        {
            CPPGameLib.GetString("pack"),
            CPPGameLib.GetString("Awaken_Equip_Title")
        },
        10,
        CPPGameLib.Handler(self, self.OnYeMei),
        false,
        nil,nil,self.__lock_level_list
    }

    self.__open_callback = function()
    	 self:__OpenCallback()
    end

    self:__AddPreloadTemplate(GUI.TemplateRegistry.PACK_ITEM)
end

function Modules.PackView:__OpenCallback()
    self:__Getwidget()
    self:__ShowUI() 
end

--开放等级
function Modules.PackView:__CountLockLevel()
    self.__lock_level_list = {}
    local module_type_list = {Macros.Game.SystemID.Pack,Macros.Game.SystemID.HERO_AWAKEN}
    for i,module_id in ipairs(module_type_list) do
        local module_info = CPPGameLib.GetConfig("SystemEntry",module_id)
        table.insert(self.__lock_level_list,module_info.level)
    end
end


function Modules.PackView:__Dispose()   
    if self.newPack then
      --去除不新鲜的物品
      for i,v in pairs( self.newPack ) do
          CallModuleFunc(ModuleType.Pack, "DelFrmNewRecordMap" , i )
      end
    end
end

function Modules.PackView:__Getwidget()
    --下
    -- self.Image_1 = self:GetWidget("Image_1")
    --内容
    self.Image_content = self:GetWidget("Image_content")
    --页签
    self.Panel_Backpackcbx = self:GetWidget("Panel_Backpackcbx")
end

function Modules.PackView:__ShowUI()
    --图鉴按钮
    self.__handbookbutton = self:GetWidget("Button_handbook")
    self.__handbookbutton:IgnoreContentAdaptWithSize(true)
    local entry_btn_res_path = Resource.PathTool.GetSystemEntryIconPath("btn_handbook")
    self.__handbookbutton:SetNormalImage(entry_btn_res_path,TextureResType.LOCAL)

    local label = Util:Label(CPPGameLib.GetString("hero_awaken_PropHandBookTitle"),Macros.TypefaceSize.normal, Macros.Color.btn, 0, 0,Macros.Color.button_yellow)
    self.__handbookbutton:AddChild(label)
    PosTool.CenterBottom(label,0,20)
    Util:WidgetButton(self.__handbookbutton,"",function ()
        LogicGUI.TemplHeroAwakenPropHandBookView.New()
    end)
    self.__handbookbutton:SetVisible(false)

    --页签
    local open_index = unpack(self.__open_data)
    open_index = open_index or 1
    self.__tab:On(open_index)
    self.__tab:SetPosition(0 , 10)
    self.__tab:SetCboxColor( Macros.Color.button_yellow , Macros.Color.button_red )
    self.Panel_Backpackcbx:AddChild(self.__tab:GetNode())
end

function Modules.PackView:__RegistAllEvents()

end

--切换页签
function Modules.PackView:OnYeMei(index)
    self:__TabChange( index )

    if index == 2 then
        self.__handbookbutton:SetVisible(true)
    end
    
    local com_sp_cbox_wire = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp_cbox_wire")
    if self.com_sp_cbox_wire then
        self.com_sp_cbox_wire:RemoveSelf(true)
        self.com_sp_cbox_wire = nil
    end
    self.com_sp_cbox_wire = Util:Sprite(com_sp_cbox_wire)
    self.__tab.btn_list[index]:AddChild( self.com_sp_cbox_wire )
    PosTool.Center( self.com_sp_cbox_wire , 0 , -35 )

    local info = nil
    if index == 1 then
        info = { show_diamond = true , show_gold = true , show_stamina = true , show_energy = true }
    else
        info = { show_godsoul = true , show_energy = true , show_gold = true , show_diamond = true }
    end   

    self.__title_bar:SetCurrency( info )
end
