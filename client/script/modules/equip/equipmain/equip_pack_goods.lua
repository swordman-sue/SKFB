--
-- @brief 装备列表
-- @author: yjg
-- @date: 2016年8月16日16:22:11
--

Modules = Modules or {}

Modules.EquipPackGoods = Modules.EquipPackGoods or BaseClass(UILayer)

function Modules.EquipPackGoods:__init(maxNode)
    self.maxNode = maxNode
    local params =  
    {
        item_class = Modules.EquipPropItem,
        item_width = 460.5,
        item_height = 120,
        row = 4,
        col = 2,
        item_space_r = 5,
        item_space_c = 5,
        view_height = 482.50 ,
        selectable = true,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode())

    local equip_list = CallModuleFunc(ModuleType.EQUIP, "GetEquipSort")
    self.lst_list:SetDataList(equip_list)

    local operate_id, data_id = CallModuleFunc(ModuleType.LEAD, "GetOperateInfo")
    if operate_id == Macros.Game.LeadOperateSpecialID.EQUIP_CHOOSE and #equip_list <= 0 then
        --前往装备界面后指引选择装备时，如果没有装备，则打断指引
        CallModuleFunc(ModuleType.LEAD, "Break")
    end

    local packMAX = CallModuleFunc(ModuleType.VIP, "GetPrivilege", Macros.Game.VIPType.EQUIP_PACK_MAX )
    self.maxNode:SetString( #CallModuleFunc(ModuleType.EQUIP, "GetEquipSort") .. "/" .. packMAX)
end

function Modules.EquipPackGoods:__delete()
    if self.lst_list then
        self.lst_list:DeleteMe()
        self.lst_list = nil
    end
end

function Modules.EquipPackGoods:UPcell(info,type_)
    if type_ == "remove" then
        --列表相关调整
        local del_index
        self.lst_list:ItemDataIter(function(i, item_data)
            if item_data.uid == info.uid then
                del_index = i
                return true
            end            
        end)
        if del_index then
            self.lst_list:DetachItem(del_index)
            self.lst_list:RefreshLayout()
        end
    else
        self.lst_list:SetDataList( CallModuleFunc(ModuleType.EQUIP, "GetEquipSort") )
    end
end

Modules = Modules or {}
Modules.EquipPropItem = Modules.EquipPropItem or BaseClass(GUI.ScrollItem)

function Modules.EquipPropItem:__init()
    self:InitTemplate(GUI.TemplateRegistry.EQUIP_PROP_ITEM)   
    self:__Getwidget()
end

function Modules.EquipPropItem:__delete()

end

function Modules.EquipPropItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)

    self:__ShowUI(data)

    --从新增记录表中删除
    CallModuleFunc(ModuleType.EQUIP, "DelFrmNewRecordMap", self.__data.uid)
end

function Modules.EquipPropItem:SetViewIndex(var)
    GUI.ScrollItem.SetViewIndex(self, var)

    --新手指引特殊操作检测(装备界面-装备（格子索引，从1开始）)
    CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.EQUIP_CHOOSE, self.__icon:GetNode(), self:GetViewIndex())    
end

function Modules.EquipPropItem:__Getwidget()
    --条目
    self.Image_propItem = self:GetWidget("Image_propItem") 
    --条目容器
    self.Panel_propItem = self:GetWidget("Panel_propItem") 
    --名字
    self.Text_propName = self:GetWidget("Text_propName") 
    --持有人
    self.Text_propHeroName = self:GetWidget("Text_propHeroName") 
    --攻击
    self.Text_propAtk = self:GetWidget("Text_propAtk") 
    --暴击
    self.Text_propCri = self:GetWidget("Text_propCri")
    --精炼 
    self.Text_propRefine = self:GetWidget("Text_propRefine") 
    --图标
    self.Panel_propIcon = self:GetWidget("Panel_propIcon") 
end

function Modules.EquipPropItem:__ShowUI(data)
    local type_ , info = ObjIDTool.GetInfo(data.info_id)

    local is_in_lead = CallModuleFunc(ModuleType.LEAD, "IsInLead")

    --头像
    self.__icon = LogicGUI.IconLayer.New()
    self.Panel_propIcon:AddChild(self.__icon:GetNode())
    PosTool.Center(self.__icon:GetNode())
    local iconData = {id = data.info_id , obj_info = data }
    self.__icon:SetData(iconData)
    self.__icon:ShowEquipStrengthenLev(true)
    self.__icon:SetEquipStar( )

    --名字
    local equipType = CPPGameLib.GetString( "common_brackets" , CPPGameLib.GetString("equip_type"..info.item_type) )
    local name = Util:Name(equipType..info.name , Macros.TypefaceSize.normal ,info.quality)
    name:SetAnchorPoint(0, 0.5)
    name:SetPosition(-10,self.Text_propName:GetContentHeight()/2)
    self.Text_propName:AddChild(name)

    --装备中
    if data.equip_heroid ~= 0 then
        local config_hero = CPPGameLib.GetConfig("Hero", data.equip_heroid)
        if config_hero then
            local heroName = Util:Label(config_hero.name..CPPGameLib.GetString("equip_cross") , Macros.TypefaceSize.slightly , Macros.Color.content )
            heroName:SetAnchorPoint(1,0.5)
            heroName:SetPosition(self.Text_propHeroName:GetContentWidth() , self.Text_propHeroName:GetContentHeight() / 2 )
            self.Text_propHeroName:AddChild(heroName)
        end
    end

    local t2 = CallModuleFunc(ModuleType.EQUIP, "GetLiftPropPlusInfos",data)

    --属性1
    local type1 ,value1 = Util:Nature(t2[1].type,t2[1].value,true,true)
    local propAtk = Util:Label( type1 ..": " , Macros.TypefaceSize.normal , Macros.Color.keypoint)
    propAtk:SetAnchorPoint(0,0.5)
    propAtk:SetPosition( 0 ,self.Text_propAtk:GetContentHeight()/2)
    self.Text_propAtk:AddChild(propAtk)

    local propAtk1 = Util:Label( value1 , nil , Macros.Color.content)
    PosTool.RightTo(propAtk , propAtk1)
    -- propAtk1:SetShadow(true)
    -- propAtk1:SetShadowColor(0, 0, 0, 255)
    self.Text_propAtk:AddChild(propAtk1)  

    --属性2
    local type2 ,value2 = Util:Nature(t2[2].type,t2[2].value,true,true)
    local propCri = Util:Label( type2 ..": " , Macros.TypefaceSize.normal , Macros.Color.keypoint)
    propCri:SetAnchorPoint(0,0.5)
    propCri:SetPosition( 0 ,self.Text_propCri:GetContentHeight()/2)
    self.Text_propCri:AddChild(propCri)

    local propAtk2 = Util:Label( value2 , Macros.TypefaceSize.normal , Macros.Color.content )
    PosTool.RightTo(propCri , propAtk2)
    self.Text_propCri:AddChild(propAtk2) 

    --属性3
    if data.refine_level ~= 0 then
        local refine = Util:Label( CPPGameLib.GetString("equip_refineLv") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
        refine:SetAnchorPoint(0,0.5)
        refine:SetPosition( -10 ,self.Text_propRefine:GetContentHeight()/2)
        self.Text_propRefine:AddChild(refine)

        local refine2 = Util:Label( data.refine_level , Macros.TypefaceSize.normal , Macros.Color.content )
        PosTool.RightTo(refine , refine2)
        self.Text_propRefine:AddChild(refine2) 
    end

    --new
    self.new = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "new" , true ) )
    self.new:SetPosition( self.__icon:GetNode():GetContentWidth()/2 + 10 , self.__icon:GetNode():GetContentHeight()/2 + 40 )
    self.new:SetAnchorPoint( 0.5 , 0.5 )
    self.__icon:AddChild(self.new , 999 ) 
    self.new:SetVisible( false )

    local data = CallModuleFunc(ModuleType.EQUIP , "GetNewRecord" , data.uid )   
    if data and data.type == true then
        self.new:SetVisible( true )
    end

end