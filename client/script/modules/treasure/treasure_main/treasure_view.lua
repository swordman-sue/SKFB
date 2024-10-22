--
-- @brief 圣器背包
-- @author: yjg
-- @date: 2016年9月2日10:33:46
--

Modules = Modules or {}

Modules.TreasureView = Modules.TreasureView or BaseClass(BaseView)

function Modules.TreasureView:__init()
    self.__layout_name = "equip"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
    self.__title_bar_params = 
    {
      title = CPPGameLib.GetString("treasure_title"),
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
        CPPGameLib.GetString("treasure_btn")
      },10,CPPGameLib.Handler(self, self.OnYeMei),false
    }

    self.__open_callback = function()
    	self:__OpenCallback()
    end

    --列表条目
    self:__AddPreloadTemplate(GUI.TemplateRegistry.EQUIP_PROP_ITEM)
end

function Modules.TreasureView:__OpenCallback()   
    --请求圣器列表     
	self:__Getwidget()
	self:__ShowUI()
end

function Modules.TreasureView:__RegistAllEvents()
  --删除圣器
  self:BindGlobalEvent(Macros.Event.TreasureModule.TREASURE_REMOVE, function(_, data)
      self:ShowGoods(data)
  end)
end

function Modules.TreasureView:__Dispose()
  if self.goods then
    self.goods:DeleteMe()
    self.goods = nil
  end
end

function Modules.TreasureView:__Getwidget( ... )
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
        LogicGUI.TemplBuyBagMax.New( Macros.Game.VIPType.TREASURE_PACK_MAX )
    end )
    self.Button_showMax:SetVisible(true)

    local lv , max = CallModuleFunc(ModuleType.VIP, "ExtendPack", Macros.Game.VIPType.TREASURE_PACK_MAX  )
    if lv and max then
      self.Button_showMax:SetVisible(false)
    end

    --页签
    self.Panel_Backpackcbx = self:GetWidget("Panel_Backpackcbx")

    self.Panel_9 = self:GetWidget("Panel_9")
end

function Modules.TreasureView:__ShowUI( ... )
  	--上限显示
  	local showMax = Util:Label(" ",nil,Macros.Color.lilyWhite )
  	self.Image_showMax:AddChild(showMax)
  	PosTool.Center(showMax)

    --装备列表
    self.goods = Modules.TreasureGoods.New(showMax)
    self.Image_content:AddChild(self.goods:GetNode())
    PosTool.Center(self.goods:GetNode())

    --页签
    self.__tab:On(1)
    self.__tab:SetPosition(0 , 0)
    self.Panel_Backpackcbx:AddChild(self.__tab:GetNode())

    --夺宝
    local btn_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
    local robBtn = Util:Button(btn_sp, function()
                                            CallModuleFunc(ModuleType.PLUNDEREDTREASURES, "OpenView", BaseViewType.PLUNDEREDTREASURES)
                                        end, 
                                        CPPGameLib.GetString("plunderedtreasures_get"), 
                                        Macros.TypefaceSize.button, 
                                        Macros.Color.btn,
                                        Macros.Color.button_yellow
                                        )
    robBtn:SetScale9Enabled(true)
    robBtn:SetContentSize(119.00,49.00)
    robBtn:SetPosition( self.Button_sell:GetPositionX() - 120 , self.Button_sell:GetPositionY())
    robBtn:SetAnchorPoint(0.5,0.5)
    self.Panel_Backpack:AddChild(robBtn)

    local red_num = CallModuleFunc(ModuleType.RED_MGR, "GetRed", Macros.Game.SystemID.PLUNDEREDTREASURES)
    if red_num ~= 0 then
      local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
      self.icon_red_point = Util:Sprite(icon_red_point)
      self.icon_red_point:SetPosition(robBtn:GetContentWidth() - 12 , robBtn:GetContentHeight() - 5 )
      robBtn:AddChild(self.icon_red_point , 20)
    end
end


function Modules.TreasureView:OnYeMei( index )
    local com_sp_cbox_wire = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp_cbox_wire")
    if self.com_sp_cbox_wire then
        self.com_sp_cbox_wire:RemoveSelf(true)
        self.com_sp_cbox_wire = nil
    end
    self.com_sp_cbox_wire = Util:Sprite(com_sp_cbox_wire)
    self.__tab.btn_list[index]:AddChild( self.com_sp_cbox_wire )
    PosTool.Center( self.com_sp_cbox_wire , 0 , -35 )
    
    self.index = index
    self.goods:SetVisible(false)
    self.Button_showMax:SetVisible(false)
    if index == 1 then
    	self.goods:SetVisible(true)
      self.Button_showMax:SetVisible(true)
    end
end

--出售
function Modules.TreasureView:OnSell()
  	local type_ = nil
  	if self.index == 1 then
  		type_ = Macros.Game.SELECT_TYPE.TREASURE
  	end
  	CallModuleFunc(ModuleType.DEBRISELL, "OpenView", BaseViewType.DEBRISELL,type_)
end

function Modules.TreasureView:ShowGoods( info )
  	self.goods:UPcell(info)
end
