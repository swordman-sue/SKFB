
-- @brief 道具图标
--

Modules = Modules or {}
Modules.IconItem = Modules.IconItem or BaseClass(GUI.ScrollItem)

function Modules.IconItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.IconItem:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(80,80)
    self.layout:SetClippingEnable(false)
    -- self.layout:SetBackGroundColor(0, 0, 0)
    self.layout:SetTouchEnabled(false)
    return self.layout
end

function Modules.IconItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)  
    --头像
    if not self.icon then
        self.icon = LogicGUI.IconLayer.New()
        self.icon:SetAnchorPoint(0.5,0.5)  
        self.layout:AddChild(self.icon:GetNode())
        PosTool.Center(self.icon)
    end
    
    self.icon:SetData({ id = self.__data.item_id or 
                                self.__data.icon or 
                                self.__data.info_id or
                                self.__data.id , 
                        num = self.__data.item_num or 
                            self.__data.num
                         })
    self.icon:SetTouchSwallow(false)
    self.icon:SetScaleFactor( self.__data.sc or 0.95 )

end