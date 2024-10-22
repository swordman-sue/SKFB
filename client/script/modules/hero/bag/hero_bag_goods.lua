--
-- @brief 英雄列表
-- @author: yjg
-- @date: 2016年8月29日14:39:22
--

Modules = Modules or {}

Modules.HeroGoods = Modules.HeroGoods or BaseClass(UILayer)

function Modules.HeroGoods:__init(maxNode)
    self.maxNode = maxNode
    local params =  
    {
        item_class = Modules.HeroGoodsItem,
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
    self.lst_list:SetDataList( CallModuleFunc(ModuleType.HERO, "GetHeroInfoList") )

    local packMAX = CallModuleFunc(ModuleType.VIP, "GetPrivilege", Macros.Game.VIPType.HERO_PACK_MAX )
    self.maxNode:SetString( #CallModuleFunc(ModuleType.HERO, "GetHeroInfoList") .. "/" .. packMAX)
end

function Modules.HeroGoods:__delete()
    if self.lst_list then
        self.lst_list:DeleteMe()
        self.lst_list = nil
    end
end

function Modules.HeroGoods:SetCell(info , type_ )
    if type_ == "add" then
        self.lst_list:SetDataList( CallModuleFunc(ModuleType.HERO, "GetHeroInfoList") )
        local packMAX = CallModuleFunc(ModuleType.VIP, "GetPrivilege", Macros.Game.VIPType.HERO_PACK_MAX )
        self.maxNode:SetString( #CallModuleFunc(ModuleType.HERO, "GetHeroInfoList") .. "/" .. packMAX)
    else
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
    end
end


Modules = Modules or {}
Modules.HeroGoodsItem = Modules.HeroGoodsItem or BaseClass(GUI.ScrollItem)

function Modules.HeroGoodsItem:__init()
    self:InitTemplate(GUI.TemplateRegistry.EQUIP_PROP_ITEM)   
    self:__Getwidget()
end

function Modules.HeroGoodsItem:__delete()

end

function Modules.HeroGoodsItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)

    self:__ShowUI(data)

    --从新增记录表中删除
    CallModuleFunc(ModuleType.HERO, "DelFrmNewRecordMap", self.__data.uid)
end

function Modules.HeroGoodsItem:__Getwidget()
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

function Modules.HeroGoodsItem:__ShowUI(data)
    local type_ , info = ObjIDTool.GetInfo(data.info_id)
    --头像
    local icon = LogicGUI.IconLayer.New(false,true)
    self.Panel_propIcon:AddChild(icon:GetNode())
    PosTool.Center(icon:GetNode())
    local iconData = {id = data.info_id , obj_info = data }
    icon:SetData(iconData)
    icon:ShowEquipStrengthenLev(true)
    icon:ShowAwakenStar(data.awaken_star)
    icon:SetTouchCallback(function ()
        if info.type ~= Macros.Game.HeroType.EXP then
            CallModuleFunc(ModuleType.HERO, "OpenView", BaseViewType.HERO_PROMOTE, nil, data.uid,true)
        else
            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_exp_hero_not_promote"))
        end
    end)

    --图标
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON, "icon_profession_"..info.profession, true)
    local tb = Util:Sprite(sp)
    tb:SetPosition(0,self.Text_propName:GetContentHeight()/2)
    tb:SetAnchorPoint(0, 0.5)
    self.Text_propName:AddChild(tb)

    local text = ""
    if data.break_level ~= 0 then
        text = "+"..data.break_level 
    end

    --名字
    local name = Util:Name(info.name .. text , Macros.TypefaceSize.normal , data.quality_level)
    PosTool.RightTo(tb,name , 5 )
    self.Text_propName:AddChild(name)

    --属性1
    local propAtk = Util:Label( CPPGameLib.GetString("common_lev1") , Macros.TypefaceSize.normal , Macros.Color.keypoint)
    propAtk:SetAnchorPoint(0,0.5)
    propAtk:SetPosition( 0 ,self.Text_propAtk:GetContentHeight()/2)
    self.Text_propAtk:AddChild(propAtk)

    local propAtkValue = Util:Label(data.level , Macros.TypefaceSize.normal , Macros.Color.content )
    PosTool.RightTo(propAtk,propAtkValue)
    self.Text_propAtk:AddChild(propAtkValue)

    --潜能开启
    if GlobalModulesMgr:IsOpen(Macros.Game.SystemID.HERO_POTENTIAL,false) == true then
        --属性2
        local propCri = Util:Label( CPPGameLib.GetString("hero_potency")..  CPPGameLib.GetString("common_colon") , Macros.TypefaceSize.normal , Macros.Color.keypoint)
        propCri:SetAnchorPoint(0,0.5)
        propCri:SetPosition( 0 ,self.Text_propCri:GetContentHeight()/2)
        self.Text_propCri:AddChild(propCri)

        local propCriValue = Util:Label(data.potential_level , Macros.TypefaceSize.normal , Macros.Color.content)
        PosTool.RightTo(propCri,propCriValue)
        self.Text_propCri:AddChild(propCriValue)

    end

    if data.awaken_level ~= 0 or data.awaken_star ~= 0 then
        --属性3
        local refine = Util:Label( CPPGameLib.GetString("hero_awaken") ..  CPPGameLib.GetString("common_colon") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
        refine:SetAnchorPoint(0,0.5)
        refine:SetPosition( -10 ,self.Text_propRefine:GetContentHeight()/2)
        self.Text_propRefine:AddChild(refine)

        local refineValue = Util:Label( CPPGameLib.GetString("hero_awakenlevel" , data.awaken_star , data.awaken_level )  , Macros.TypefaceSize.normal , Macros.Color.keypoint )
        PosTool.RightTo(refine,refineValue)
        self.Text_propRefine:AddChild(refineValue)
    end

    if data.is_to_battle ~= 0 then
        --上阵中
        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.HERO_BAG , "hero_yet" , true)
        local equipType = Util:Sprite(sp)
        equipType:SetPosition( 15 , icon:GetNode():GetContentHeight() - 5)
        icon:AddChild(equipType,999)
    end

    --new
    self.new = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "new" , true ) )
    self.new:SetPosition( icon:GetNode():GetContentWidth()/2 + 10 , icon:GetNode():GetContentHeight()/2 + 40 )
    self.new:SetAnchorPoint( 0.5 , 0.5 )
    icon:AddChild(self.new , 999 ) 
    self.new:SetVisible( false )

    local data = CallModuleFunc(ModuleType.HERO , "GetNewRecord" , data.uid )   
    if data and data.type == true then
        self.new:SetVisible( true )
    end
end