--
-- @brief 英雄碎片列表
-- @author: yjg
-- @date: 2016年8月29日14:39:26
--

Modules = Modules or {}

Modules.HeroDebris = Modules.HeroDebris or BaseClass(UILayer)

function Modules.HeroDebris:__init()
    local params =  
    {
        item_class = Modules.HeroDebrisItem,
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
    self.lst_list:SetDataList(CallModuleFunc(ModuleType.Pack, "GetAllHeroDebris"))
end

function Modules.HeroDebris:__delete()
    if self.lst_list then
        self.lst_list:DeleteMe()
        self.lst_list = nil
    end
end

function Modules.HeroDebris:SetCell( info )
    self.lst_list:SetDataList(CallModuleFunc(ModuleType.Pack, "GetAllHeroDebris"))
end


Modules = Modules or {}
Modules.HeroDebrisItem = Modules.HeroDebrisItem or BaseClass(GUI.ScrollItem)

function Modules.HeroDebrisItem:__init()
    self:InitTemplate(GUI.TemplateRegistry.EQUIP_DEDRIS_ITEM)   
end

function Modules.HeroDebrisItem:__delete()

end

function Modules.HeroDebrisItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self:__Getwidget(data)
    self:__ShowUI(data)
end

function Modules.HeroDebrisItem:__Getwidget( data )
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
    self.Button_debrisGet:SetVisible(false)
    --合成
    self.Button_debrisCompound = Util:WidgetButton( self:GetWidget("Button_debrisCompound") ,CPPGameLib.GetString("sell_compose"), function ( ... )
        CallModuleFunc(ModuleType.SUNDRY, "PackVacancy", Macros.Game.ObjType.HERO , 0 , true , function ( sum )
            if sum > 0 then
                CallModuleFunc(ModuleType.HERO, "HeroComposeReq",data.target_id)
            end
        end )
    end,nil,nil,Macros.Color.button_yellow)
    self.Button_debrisCompound:SetVisible(false)
    --图标
    self.Panel_debrisIcon = self:GetWidget("Panel_debrisIcon") 
end

function Modules.HeroDebrisItem:__ShowUI(data)

    --头像
    local icon = LogicGUI.IconLayer.New(false,true)
    self.Panel_debrisIcon:AddChild(icon:GetNode())
    icon:SetData({id = data.item_id})
    PosTool.Center(icon)
    icon:SetTouchCallback(function ()
        CallModuleFunc(ModuleType.HERO,"OpenView",BaseViewType.HERO_HANDBOOK_DETAIL,data.item_id)
    end)

    --名字
    local name = Util:Name(data.item_name , nil , data.quality)
    name:SetAnchorPoint(0,0.5)
    name:SetPosition(0,self.Text_debrisName:GetContentHeight()/2)
    self.Text_debrisName:AddChild(name)

    --持有数量
    local numText = Util:Label( CPPGameLib.GetString("pack_num") , Macros.TypefaceSize.normal , Macros.Color.keypoint)
    numText:SetAnchorPoint(0,0.5)
    numText:SetPosition(0,self.Text_debrisNum:GetContentHeight()/2 + 5 )
    self.Text_debrisNum:AddChild(numText)

    self.Button_debrisGet:SetVisible(true)
    if data.item_num >= data.target_num then
        self.Button_debrisCompound:SetVisible(true)
    -- else
    --     self.Button_debrisGetCenter:SetVisible(true)
    end

    local effect_id,effect = self:__CreateEffect({res_name = "ui_anniuzoubian"},self.Button_debrisCompound)
    PosTool.Center(effect)
    effect:SetScaleFactor(0.8)


    self.num = Util:Label(data.item_num.."/"..data.target_num , Macros.TypefaceSize.normal , Macros.Color.content )
    PosTool.RightTo(numText,self.num )
    self.Text_debrisNum:AddChild(self.num )
end