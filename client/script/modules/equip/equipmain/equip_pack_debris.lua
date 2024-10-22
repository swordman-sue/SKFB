--
-- @brief 碎片列表
-- @author: yjg
-- @date: 2016年8月16日16:22:11
--

Modules = Modules or {}

Modules.EquipPackDebris = Modules.EquipPackDebris or BaseClass(UILayer)

function Modules.EquipPackDebris:__init( ... )
    local params =  
    {
        item_class = Modules.EquipDebrisItem,
        item_width = 460.5,
        item_height = 120,
        row = 4,
        col = 2,
        item_space_r = 5,
        item_space_c = 5,
        -- view_width = 925.00,
        view_height = 482.50 ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode())
    self.lst_list:SetDataList(CallModuleFunc(ModuleType.Pack, "GetAllEquipDebris"))
end

function Modules.EquipPackDebris:__delete()
    if self.lst_list then
        self.lst_list:DeleteMe()
        self.lst_list = nil
    end
end

--设置内容
function Modules.EquipPackDebris:SetCell( )
    self.lst_list:SetDataList(CallModuleFunc(ModuleType.Pack, "GetAllEquipDebris"))
end

--新手指引特殊操作检测(装备界面-碎片-合成（格子索引，从1开始）)
function Modules.EquipPackDebris:CheckSectionOperateSP()    
    self.lst_list:ItemIter(function(item_index, item)
        item:CheckSectionOperateSP()
    end)    
end


Modules = Modules or {}
Modules.EquipDebrisItem = Modules.EquipDebrisItem or BaseClass(GUI.ScrollItem)

function Modules.EquipDebrisItem:__init()
    self:InitTemplate(GUI.TemplateRegistry.EQUIP_DEDRIS_ITEM)   
end

function Modules.EquipDebrisItem:__delete()

end

function Modules.EquipDebrisItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self:__Getwidget(data)
    self:__ShowUI(data)
end

function Modules.EquipDebrisItem:__Getwidget( data )
    --条目
    self.Image_debrisItem = self:GetWidget("Image_debrisItem") 
    --条目容器
    self.Panel_debrisItem = self:GetWidget("Panel_debrisItem") 
    --名字
    self.Text_debrisName = self:GetWidget("Text_debrisName") 
    --持有数量
    self.Text_debrisNum = self:GetWidget("Text_debrisNum") 
    --获取按钮下
    self.Button_debrisGet = Util:WidgetButton( self:GetWidget("Button_debrisGet") , nil , function ( ... )
        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, data.item_id)
    end)
    --合成
    self.Button_debrisCompound = Util:WidgetButton( 
                                                    self:GetWidget("Button_debrisCompound") , 
                                                    CPPGameLib.GetString("btn_compound") , 
                                                    function ( ... )
                                                        CallModuleFunc(ModuleType.SUNDRY, "PackVacancy", Macros.Game.ObjType.EQUIP , 0 , true , function ( sum )
                                                            if sum > 0 then
                                                                CallModuleFunc(ModuleType.EQUIP, "EquipmentComposeReq",data.target_id)
                                                            end
                                                        end )
                                                    end, nil ,nil,Macros.Color.button_yellow )
    self.Button_debrisCompound:SetVisible(false)
    --图标
    self.Panel_debrisIcon = self:GetWidget("Panel_debrisIcon") 
end

function Modules.EquipDebrisItem:__ShowUI(data)
    --头像
    local icon = LogicGUI.IconLayer.New(true)
    self.Panel_debrisIcon:AddChild(icon:GetNode())
    icon:SetData({id = data.item_id})
    PosTool.Center(icon)

    --名字
    local name = Util:Name(data.item_name , Macros.TypefaceSize.normal , data.quality)
    name:SetAnchorPoint(0,0.5)
    name:SetPosition(0,self.Text_debrisName:GetContentHeight()/2)
    self.Text_debrisName:AddChild(name)

    --持有数量
    local numText = Util:Label( CPPGameLib.GetString("pack_num") , nil , Macros.Color.keypoint )
    numText:SetAnchorPoint(0,0.5)
    numText:SetPosition(0,self.Text_debrisNum:GetContentHeight()/2)
    self.Text_debrisNum:AddChild(numText)

    if data.item_num >= data.target_num then
        self.Button_debrisCompound:SetVisible(true)
    end
    
    local effect_id,effect = self:__CreateEffect({res_name = "ui_anniuzoubian"},self.Button_debrisCompound)
    PosTool.Center(effect)
    effect:SetScaleFactor(0.85)

    local color = Macros.Color.red_color
    if data.item_num >= data.target_num then
        color = Macros.Color.front_color
    end

    self.num = Util:Label(data.item_num.."/"..data.target_num , nil , color )
    PosTool.RightTo(numText,self.num )
    self.Text_debrisNum:AddChild(self.num )
end

--新手指引特殊操作检测(装备界面-碎片-合成（格子索引，从1开始）)
function Modules.EquipDebrisItem:CheckSectionOperateSP()    
    CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.EQUIP_FRAGMENT_COMPOSE, self.Button_debrisCompound, self:GetViewIndex())        
end
