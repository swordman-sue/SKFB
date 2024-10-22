
--
-- @brief 装备背包(主界面)
-- @author: yjg
-- @date: 2016年8月16日10:06:10
--

Modules = Modules or {}

Modules.EquipView = Modules.EquipView or BaseClass(BaseView)

function Modules.EquipView:__init()
    self.__layout_name = "equip"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("equip_knapsack"),
        show_diamond = true,
        show_gold = true,
        show_stamina = true,
        show_energy = true,
        close_func = function()
            local init_tab, open_approach_info_id = unpack(self.__open_data) 
            if open_approach_info_id and not self.__init_approach_success then
                return
            end
          	self:CloseManual()
            CallModuleFunc(ModuleType.HERO,"ToUidGetPlace",28)
        end,
    }

    local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_no")
    self.__tab_params = {
        {sp_2,sp_1},
        {
            CPPGameLib.GetString("equip"),
            CPPGameLib.GetString("equip_debris")
        },
        10,
        CPPGameLib.Handler(self, self.OnYeMei),
        false
    }

    self.__open_callback = function()
    	self:__OpenCallback()
    end

    --列表条目
    self:__AddPreloadTemplate(GUI.TemplateRegistry.EQUIP_PROP_ITEM)
    self:__AddPreloadTemplate(GUI.TemplateRegistry.EQUIP_DEDRIS_ITEM)
    self:__AddPreloadTemplate(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGCELL)
end

function Modules.EquipView:__OpenCallback()   
  	CallModuleFunc(ModuleType.EQUIP, "OneKeyIntensify",1)

  	--背包
  	self.pack = nil
  	self.bebris = nil
  	 
  	self:__Getwidget()
  	self:__ShowUI()
end

function Modules.EquipView:__RegistAllEvents()
    --获得装备
    self:BindGlobalEvent(Macros.Event.EquipModule.EQUIP_ADD_EQUIP, function (_, data)
      if self.index == 1 then
        self:OnUpEquip(data, "get")
      else
        self.__is_equip_list_dirty = true
      end
    end)

    --删除装备
    self:BindGlobalEvent(Macros.Event.EquipModule.EQUIP_REMOVE_EQUIP, function (_, data)
      if self.index == 1 then
        self:OnUpEquip(data, "remove")
      else
        self.__to_del_equip_list = self.__to_del_equip_list or {}
        table.insert(self.__to_del_equip_list, data)
      end
    end)

    --刷新碎片
    self:BindGlobalEvent(Macros.Event.EquipModule.EQUIP_UP_DEBRIS, function (_, data)
        if self.index == 2 then
          self:OnUpDebris(data)
        else
          self.__is_equip_debris_list_dirty = true
        end
    end)  

    --红点更新事件
    self:BindGlobalEvent(Macros.Event.RedModule.RED_UPDATE, function ( _ , system_id)
        if system_id == Macros.Game.SystemID.EQUIP then
            self:__UpdateRedDot()
        end
    end)	

    --监听指引下一个步骤事件
    self:BindGlobalEvent(Macros.Event.LeadModule.CHECK_NEXT_SECTION, function(_, pre_section_info)
        self.bebris:CheckSectionOperateSP()
    end)  

    --打开获取途径
    self:BindGlobalEvent(Macros.Event.ApproachOfAchievingModule.OPEN_APPROACHVIEW, function(_, info_id)     
        self.__open_approach_info_id = info_id
    end)  
end

function Modules.EquipView:__Dispose()		
    if self.pack then
        self.pack:DeleteMe()
        self.pack = nil
    end
    if self.bebris then
        self.bebris:DeleteMe()
        self.bebris = nil
    end
end

function Modules.EquipView:__Getwidget( ... )
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
        LogicGUI.TemplBuyBagMax.New( Macros.Game.VIPType.EQUIP_PACK_MAX )
    end )
    self.Button_showMax:SetVisible(true)

    local lv , max = CallModuleFunc(ModuleType.VIP, "ExtendPack", Macros.Game.VIPType.EQUIP_PACK_MAX  )
    print("xy" , lv , max)
    if lv and max then
      self.Button_showMax:SetVisible(false)
    end

    --页签
    self.Panel_Backpackcbx = self:GetWidget("Panel_Backpackcbx")
end

function Modules.EquipView:__ShowUI( ... )
    local init_tab, open_approach_info_id = unpack(self.__open_data)         

  	--上限显示
  	local showMax = Util:Label(" ",nil,Macros.Color.lilyWhite )
  	self.Image_showMax:AddChild(showMax)
  	PosTool.Center(showMax)
    
    --装备列表
  	self.pack = Modules.EquipPackGoods.New(showMax)
  	self.Image_content:AddChild(self.pack:GetNode())
  	PosTool.Center(self.pack:GetNode())

  	--碎片列表
  	self.bebris = Modules.EquipPackDebris.New()
  	self.Image_content:AddChild(self.bebris:GetNode())
  	PosTool.Center(self.bebris:GetNode())
  
	  --页签
  	self.__tab:SetPosition(0 , 0)
  	self.Panel_Backpackcbx:AddChild(self.__tab:GetNode())
  	self.__tab:On(init_tab or 1)
    self.__tab:SetCboxColor( Macros.Color.button_yellow , Macros.Color.button_red )

    --红点
    local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")           
    self.icon_red_point = Util:Sprite(icon_red_point)
    self.icon_red_point:SetVisible(false)
    self.icon_red_point:SetPosition(self.__tab.btn_list[2]:GetContentWidth() - 10 , self.__tab.btn_list[2]:GetContentHeight() - 10 )
    self.__tab.btn_list[2]:AddChild(self.icon_red_point , 20)
    self:__UpdateRedDot()

    if open_approach_info_id then
        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, open_approach_info_id) 
        self.__init_approach_success = true     
    end
end

--页眉回调
function Modules.EquipView:OnYeMei(index)
  	self.index = index
  	
    local com_sp_cbox_wire = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp_cbox_wire")

    if self.com_sp_cbox_wire then
        self.com_sp_cbox_wire:RemoveSelf(true)
        self.com_sp_cbox_wire = nil
    end
    self.com_sp_cbox_wire = Util:Sprite(com_sp_cbox_wire)
    self.__tab.btn_list[index]:AddChild( self.com_sp_cbox_wire )
    PosTool.Center( self.com_sp_cbox_wire , 0 , -35 )

  	self.pack:SetVisible(false)
  	self.bebris:SetVisible(false)
  	self.Button_showMax:SetVisible(false)

  	if index == 1 then
    		self.pack:SetVisible(true)
    		self.Button_showMax:SetVisible(true)

        --刷新装备列表
        if self.__is_equip_list_dirty then
          self.__is_equip_list_dirty = false
          self.__to_del_equip_list = nil
          self:OnUpEquip(nil, "get")

        elseif self.__to_del_equip_list then
          for _, data in ipairs(self.__to_del_equip_list) do
            self:OnUpEquip(data, "remove")
          end
          self.__to_del_equip_list = nil
        end

  	else
    		self.bebris:SetVisible(true)

        --刷新碎片列表
        if self.__is_equip_debris_list_dirty then
          self.__is_equip_debris_list_dirty = false
          self:OnUpDebris()
        end
  	end
end

--出售
function Modules.EquipView:OnSell()
  	local type_ = nil
  	if self.index == 1 then
        type_ = Macros.Game.SELECT_TYPE.EQUIP
  	elseif self.index == 2 then
        type_ = Macros.Game.SELECT_TYPE.EQUIP_DEBRIS
  	end

  	CallModuleFunc(ModuleType.DEBRISELL, "OpenView", BaseViewType.DEBRISELL,type_)
end

--更新装备
function Modules.EquipView:OnUpEquip( data ,type)
    self.pack:UPcell(data,type)
end

--刷新碎片
function Modules.EquipView:OnUpDebris( info )
    self.bebris:SetCell( )
end

--显示选择框
function Modules.EquipView:ShowSelect()	
    if self.sellLayer then
        self.sellLayer:ShowSelect()
    end
end

--获取打开参数
function Modules.EquipView:GetOpenParams()
    if self.index then
        return {self.index, self.__open_approach_info_id}
    end
end

--更新红点
function Modules.EquipView:__UpdateRedDot()
  --如果有可以合成的
  local red_num = CallModuleFunc(ModuleType.RED_MGR , "GetRed", Macros.Game.SystemID.EQUIP )
  self.icon_red_point:SetVisible(red_num > 0)
end
