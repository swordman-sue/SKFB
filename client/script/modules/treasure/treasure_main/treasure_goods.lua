--
-- @brief ：圣器列表
-- @author: yjg
-- @date: 2016年9月2日11:43:54
--

Modules = Modules or {}

Modules.TreasureGoods = Modules.TreasureGoods or BaseClass(UILayer)

function Modules.TreasureGoods:__init(maxNode)

    self.maxNode = maxNode
    local params =  
    {
        item_class = Modules.TreasureGoodsItem,
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
    self.lst_list:SetDataList( CallModuleFunc(ModuleType.TREASURE, "GetTreasureSort") )

    local packMAX = CallModuleFunc(ModuleType.VIP, "GetPrivilege", Macros.Game.VIPType.TREASURE_PACK_MAX )
    self.maxNode:SetString( # CallModuleFunc(ModuleType.TREASURE, "GetTreasureSort") .. "/" .. packMAX)
end

function Modules.TreasureGoods:__delete()
    if self.lst_list then
        self.lst_list:DeleteMe()
        self.lst_list = nil
    end
end

function Modules.TreasureGoods:UPcell(info)
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



Modules = Modules or {}
Modules.TreasureGoodsItem = Modules.TreasureGoodsItem or BaseClass(GUI.ScrollItem)

function Modules.TreasureGoodsItem:__init()
    self:InitTemplate(GUI.TemplateRegistry.EQUIP_PROP_ITEM)   
    self:__Getwidget()
end

function Modules.TreasureGoodsItem:__delete()

end

function Modules.TreasureGoodsItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)

    self:__ShowUI(data)

    CallModuleFunc(ModuleType.TREASURE, "DelFrmNewRecordMap" , self.__data.uid)
end

function Modules.TreasureGoodsItem:__Getwidget()
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
    --第三条属性
    self.Text_property = self:GetWidget("Text_property") 

    --图标
    self.Panel_propIcon = self:GetWidget("Panel_propIcon") 

    self.Image_1 = self:GetWidget("Image_1") 
    
end

function Modules.TreasureGoodsItem:__ShowUI(info)
    --头像
    local icon = LogicGUI.IconLayer.New(true)
    self.Panel_propIcon:AddChild(icon:GetNode())
    PosTool.Center(icon:GetNode())
    local iconData = {id = info.info_id , obj_info = info }
    icon:SetData(iconData)
    icon:ShowTreasureStrengthenLev(true)

    if info.type == Macros.Game.TreasureType.EXP then
        icon:SetTouchCallback(function ( ... )
        end)
    end

    --名字
    local name = Util:Name( CPPGameLib.GetString( "common_brackets" , CPPGameLib.GetString("treasure_type_"..info.type) ) ..info.name , nil ,info.quality)
    name:SetAnchorPoint(0, 0.5)
    name:SetPosition(-10,self.Text_propName:GetContentHeight()/2)
    self.Text_propName:AddChild(name)

    --装备中
    if info.hero_infoid ~= 0 then
        local data = CPPGameLib.GetConfig("Hero", info.hero_infoid)
        local heroName = Util:LabCenter({
                                        {zi = data.name , ys = Macros.Color.content , dx = Macros.TypefaceSize.slightly },
                                        {zi = CPPGameLib.GetString("equip_cross") , ys = Macros.Color.content , dx = Macros.TypefaceSize.slightly },
                                    })
        heroName:SetAnchorPoint(1,0.5)
        heroName:SetPosition(self.Text_propHeroName:GetContentWidth() , self.Text_propHeroName:GetContentHeight() / 2 )
        self.Text_propHeroName:AddChild(heroName)
    end

    local t2 = CallModuleFunc(ModuleType.TREASURE, "GetLiftPropPlusInfos" , info ) 

    if t2 == 0 then
        local type_ , info_ = ObjIDTool.GetInfo(info.info_id)

        local propAtk = Util:Label( info_.description, Macros.TypefaceSize.slightly , Macros.Color.keypoint , self.Image_1:GetContentWidth() - 5 )
        propAtk:SetAnchorPoint(0,1)
        propAtk:SetPosition( self.Text_propAtk:GetPositionX() , self.Text_propAtk:GetPositionY() + 20 )
        self.Panel_propItem:AddChild(propAtk)

        return
    end

    -- 2017年4月14日14:48:02
    -- 1362 圣器背包的圣器要显示全部属性

    if #t2 ~= 0 then
        --属性1
        local type_ , value_ = Util:Nature(t2[1].type,t2[1].value,true,true)
        local propAtk = Util:Label( type_ .. CPPGameLib.GetString("common_colon"), Macros.TypefaceSize.slightly , Macros.Color.keypoint)
        propAtk:SetAnchorPoint(0,0.5)
        propAtk:SetPosition( 0 ,self.Text_propAtk:GetContentHeight()/2)
        self.Text_propAtk:AddChild(propAtk)

        local propAtkValue = Util:Label( value_ , Macros.TypefaceSize.slightly , Macros.Color.content)
        PosTool.RightTo(propAtk , propAtkValue)
        self.Text_propAtk:AddChild(propAtkValue)

        --属性2
        local type_ , value_ = Util:Nature(t2[2].type,t2[2].value,true,true)
        local propCri = Util:Label( type_.. CPPGameLib.GetString("common_colon") , Macros.TypefaceSize.slightly , Macros.Color.keypoint)
        propCri:SetAnchorPoint(0,0.5)
        propCri:SetPosition( 0 ,self.Text_propCri:GetContentHeight()/2)
        self.Text_propCri:AddChild(propCri)

        local propCriValue = Util:Label( value_ , Macros.TypefaceSize.slightly , Macros.Color.content)
        PosTool.RightTo(propCri , propCriValue)
        self.Text_propCri:AddChild(propCriValue)

        --属性3
        local type_ , value_ = Util:Nature(t2[3].type , t2[3].value , true , true)
        local propCri = Util:Label( type_.. CPPGameLib.GetString("common_colon") , Macros.TypefaceSize.slightly , Macros.Color.keypoint)
        propCri:SetAnchorPoint(0,0.5)
        propCri:SetPosition( 0 ,self.Text_property:GetContentHeight()/2)
        self.Text_property:AddChild(propCri)

        local propCriValue = Util:Label( value_ , Macros.TypefaceSize.slightly , Macros.Color.content)
        PosTool.RightTo(propCri , propCriValue)
        self.Text_property:AddChild(propCriValue)

        --属性4
        if info.refine_level ~= 0 then
            local refine = Util:Label( CPPGameLib.GetString("equip_refineLv") , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
            refine:SetAnchorPoint(0,0.5)
            refine:SetPosition( 0 ,self.Text_propRefine:GetContentHeight()/2)
            self.Text_propRefine:AddChild(refine)

            local refineValue = Util:Label( info.refine_level , Macros.TypefaceSize.slightly , Macros.Color.content)
            PosTool.RightTo(refine , refineValue)
            self.Text_propRefine:AddChild(refineValue)
        end
    end

    --new
    self.new = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "new" , true ) )
    self.new:SetPosition( icon:GetNode():GetContentWidth()/2 + 10 , icon:GetNode():GetContentHeight()/2 + 40 )
    self.new:SetAnchorPoint( 0.5 , 0.5 )
    icon:AddChild(self.new , 999 ) 
    self.new:SetVisible( false )

    local data = CallModuleFunc(ModuleType.TREASURE , "GetNewRecord" , info.uid )   
    if data and data.type == true then
        self.new:SetVisible( true )
    end

end
