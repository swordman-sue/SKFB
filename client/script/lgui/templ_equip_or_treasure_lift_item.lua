
Modules = Modules or {}
Modules.EquipOrTreasureLiftItem = Modules.EquipOrTreasureLiftItem or BaseClass(GUI.ScrollItem)

function Modules.EquipOrTreasureLiftItem:__init(obj_type)
    self.__obj_type = obj_type
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.EquipOrTreasureLiftItem:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(90,110)
    self.layout:SetBackGroundColor(0, 0, 0)
    return self.layout
end

function Modules.EquipOrTreasureLiftItem:__delete()
    
end

function Modules.EquipOrTreasureLiftItem:SetData( data )
    GUI.ScrollItem.SetData(self, data)
    self:__ShowUI()
end

function Modules.EquipOrTreasureLiftItem:__ShowUI()
    local data = self.__data
    local type_ , info_ = ObjIDTool.GetInfo(data.info_id)
    --头像
    self.icon = LogicGUI.IconLayer.New(false, false)
    self.icon:SetPosition(self.layout:GetContentWidth()/2 ,self.layout:GetContentHeight() / 2 + 10)
    self.icon:SetAnchorPoint(0.5, 0.5 )
    self.layout:AddChild(self.icon:GetNode())
    self.icon:SetData({id = data.info_id , obj_info = data })
    self.icon:ShowEquipStrengthenLev(true)
    self.icon:SetTouchSwallow(false)
    self.icon:SetEquipStar()

    local refine = ""
    if data.refine_level ~= 0 then
      refine = " +"..data.refine_level
    end

    --名字
    self.name = Util:Name(info_.name..refine,Macros.TypefaceSize.slightly,info_.quality)
    self.name:SetAnchorPoint(0.5,1)
    self.name:SetPosition(self.icon:GetNode():GetContentWidth()/2 , -5 )
    self.icon:GetNode():AddChild(self.name)

    --更新红点
    self:UpdateIconRedDot()
end

--更新红点
function Modules.EquipOrTreasureLiftItem:UpdateIconRedDot()
    self.__data.can_intensify = self.__obj_type == Macros.Game.ObjType.EQUIP and 
        CallModuleFunc(ModuleType.EQUIP, "ToUidGetEquipIsStrengthen", self.__data.uid) or 
        CallModuleFunc(ModuleType.TREASURE, "ToUidGetTreasureIsStrengthen", self.__data.uid)
    
    self.__data.can_refine = self.__obj_type == Macros.Game.ObjType.EQUIP and 
        CallModuleFunc(ModuleType.EQUIP, "ToUidGetEquipIsRefine", self.__data.uid) or
        CallModuleFunc(ModuleType.TREASURE, "ToUidGetTreasureIsRefine", self.__data.uid) 
    
    if self.__data.can_intensify == "true" or self.__data.can_refine == "true" then
        self:__SetIconRed()
    else
        self:__RemoveIconRed()
    end
end

function Modules.EquipOrTreasureLiftItem:__SetIconRed()
    if not self.icon_red_point then
        self.icon_red_point = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point"))
        self.icon_red_point:SetPosition(self.icon:GetNode():GetContentWidth() - 12 , self.icon:GetNode():GetContentHeight() - 5 )
        self.icon:AddChild(self.icon_red_point , 20)
    end
    self.icon_red_point:SetVisible(true)
end

function Modules.EquipOrTreasureLiftItem:__RemoveIconRed()
    if self.icon_red_point then
        self.icon_red_point:SetVisible(false)
    end
end

