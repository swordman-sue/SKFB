Modules = Modules or {}

Modules.TemplAwakenEquipCell = Modules.TemplAwakenEquipCell or BaseClass(GUI.ScrollItem)

function Modules.TemplAwakenEquipCell:__init()
    self:InitTemplate(GUI.TemplateRegistry.PACK_ITEM)     
end

function Modules.TemplAwakenEquipCell:__delete()
end

function Modules.TemplAwakenEquipCell:SetData(data)
    GUI.ScrollItem.SetData(self, data)

    local awaken_config = CPPGameLib.GetConfig("AwakeningItem",data.item_id)
    --上按钮
    self:GetWidget("Button_up1"):SetVisible(false)
    --下按钮
    self:GetWidget("Button_disposable1"):SetVisible(false)
    --头像
    local Panel_icon = self:GetWidget("Panel_icon")
    self.icon_layer = LogicGUI.IconLayer.New(false)
    Panel_icon:AddChild(self.icon_layer:GetNode())
    PosTool.Center(self.icon_layer)
    self.icon_layer:SetData({id = data.item_id , num = data.item_num})
    self.icon_layer.__touch_callback = function ( ... )
        LogicGUI.TemplHeroAwakenPropView.New(data.item_id,5)
    end
    --名字
    local textName = self:GetWidget("Text_name1")
    local name = Util:Name(awaken_config.name,nil,data.quality)
    name:SetPosition(textName:GetContentWidth()/2,textName:GetContentHeight()/2)
    name:SetAnchorPoint(0,0.5)
    textName:AddChild(name)

    --文本   
    local Image_content = self:GetWidget("Image_content1")
    local dice_label = Util:RichText("",Macros.TypefaceSize.slightly,Image_content:GetContentWidth() - 20  , 0 )
    dice_label:SetPosition(10 , Image_content:GetContentHeight() - 10)
    dice_label:SetAnchorPoint(0,1)
    Image_content:AddChild(dice_label)

    --分解
    Util:WidgetButton(self:GetWidget("Button_center1"),CPPGameLib.GetString("Awaken_Equip_Resolve"),function ( )     
        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.PACK_ONEUSE , false , self.__data.item_id , nil, self.__data.item_id )    
    end,Macros.TypefaceSize.button,Macros.Color.btn,Macros.Color.button_yellow)
end