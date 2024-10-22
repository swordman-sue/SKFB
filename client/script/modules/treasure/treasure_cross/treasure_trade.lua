--
-- @brief 装备更换入口
-- @author: yjg
-- @date: 2016年8月26日11:59:29
--

Modules = Modules or {}
-- templ_equip_exchange
Modules.TreasureTradeView = Modules.TreasureTradeView or BaseClass(BaseView)

function Modules.TreasureTradeView:__init()
    self.__layout_name = "equipdetails"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
    self.__title_bar_params = 
    {
        show_diamond = true,
        show_gold = true,
        show_stamina = true,
        show_energy = true,
        close_func = function()
          self:CloseManual()
        end,
    }

    self.__open_callback = function()
        self:__OpenCallback()
    end

    self:__AddPreloadTemplate(GUI.TemplateRegistry.EQUIP_EXCHANGE)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.EQUIPPACK)
end

function Modules.TreasureTradeView:__Dispose()

    if self.trde then
        self.trde:DeleteMe()
        self.trde = nil
    end
end

function Modules.TreasureTradeView:__OpenCallback()
    --标题
    self:__Getwidget()
    self:__ShowUI()
end

function Modules.TreasureTradeView:__Getwidget( ... )
   
end

function Modules.TreasureTradeView:__ShowUI( ... )
    self.trde = Modules.TemplTrade.New(unpack(self.__open_data),function ()
        self:OnClose()
    end)
end

function Modules.TreasureTradeView:SetlistCell( ... )
  
end

function Modules.TreasureTradeView:OnClose( ... )
    self:DeleteMe()
end


Modules = Modules or {}

Modules.TemplTrade = Modules.TemplTrade or BaseClass(GUI.Template)

function Modules.TemplTrade:__init(data,fun)
    self:InitTemplate(GUI.TemplateRegistry.EQUIP_EXCHANGE)  

    local info = {}

    if data.weapon_equipment_info and data.weapon_equipment_info.uid then
        info.weapon = data.weapon_equipment_info.uid
    end
    if data.hat_equipment_info and data.hat_equipment_info.uid then
        info.hat = data.hat_equipment_info.uid
    end
    if data.belt_equipment_info and data.belt_equipment_info.uid then
        info.belt = data.belt_equipment_info.uid
    end
    if data.clothes_equipment_info and data.clothes_equipment_info.uid then
        info.clothes = data.clothes_equipment_info.uid
    end

    self.info = CallModuleFunc(ModuleType.FORGE, "EquipIdSort",data.item_type , info)

    for i,v in ipairs(self.info) do
        v.index = data.lineup_index
    end
    self.icon = {}
    self.ontype = data.item_type
    self.data = self.info[1]
    self.fun = fun 

    self:__Getwidget()
    self:__ShowUI()
end

function Modules.TemplTrade:__delete()

end

function Modules.TemplTrade:__Getwidget( ... )
    --图标背景
    self.img_iconbg = self:GetWidget("img_iconbg")
    --六芒星
    self.img_liumangxing = self:GetWidget("img_liumangxing")
    --标题
    self.img_biaoti = self:GetWidget("img_biaoti")
    --内容背景
    self.img_listBg = self:GetWidget("img_listBg")
    --图标内容
    self.node_node = self:GetWidget("node_node")
    --装备名字
    self.lab_name = self:GetWidget("lab_name")
    --装备类型
    self.lab_type = self:GetWidget("lab_type")
    --装备品质
    self.lab_quality = self:GetWidget("lab_quality")
    --装备更换
    self.btn_change = self:GetWidget("btn_change")
    self.btn_change:SetText(CPPGameLib.GetString("equip_change"))
    self.btn_change:SetFontName(Util.FontType)
    self.btn_change:SetFontSize(Macros.TypefaceSize.button)
    self.btn_change:SetTextColor(unpack(Macros.Color.keypoint))
    WidgetUnity.SetWidgetListener(self.btn_change,nil,nil,function ()
        -- local equipType = CallModuleFunc(ModuleType.EQUIP, "GetTypeEquip",self.data.item_type)
        -- if #equipType > 0 then 
        --     CallModuleFunc(ModuleType.EQUIP, "OpenView",BaseViewType.EQUIPLIST,self.data)
        -- else
        --     GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("equip_noequip"))
        -- end
    end)

    --卸下装备
    self.btn_excrete = self:GetWidget("btn_excrete")
    self.btn_excrete:SetText(CPPGameLib.GetString("equip_discharge"))
    self.btn_excrete:SetFontName(Util.FontType)
    self.btn_excrete:SetFontSize(Macros.TypefaceSize.button)
    self.btn_excrete:SetTextColor(unpack(Macros.Color.keypoint))
    WidgetUnity.SetWidgetListener(self.btn_excrete,nil,nil,function ()
        local info = {}
        info.formation_pos = self.data.index
        info.equipment_pos = self.data.item_type
        info.equipment_uid = 0
        CallModuleFunc(ModuleType.EQUIP, "EquipMentPutOnReq",info)
        if self.fun then
            self.fun()
        end
    end)
end

function Modules.TemplTrade:__ShowUI( ... )
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "greenarrows")
    self.pitchOn = Util:Sprite(sp)
    self.pitchOn:SetZOrder(998)
    self.img_iconbg:AddChild(self.pitchOn)

    --装备选择框
    local y = self.img_iconbg:GetContentHeight() - 75

    for i,v in ipairs(self.info) do
        self.icon[i] = LogicGUI.IconLayer.New(false)
        self.icon[i]:SetData({type = Macros.Game.ObjType.EQUIP, id = v.info_id })
        -- self.icon[i]:ShowEquipStrengthenLev(true)
        self.icon[i]:SetTouchCallback(function()
            self:SetShowUI(v)
            self.pitchOn:SetPosition(self.icon[i]:GetNode():GetPositionX() + 30 ,self.icon[i]:GetNode():GetPositionY())
        end)
        self.icon[i]:SetPosition(self.img_iconbg:GetContentWidth()/2 , y)
        self.img_iconbg:AddChild(self.icon[i]:GetNode())
        local name = Util:Label(v.name)
        name:SetPosition(self.icon[i]:GetNode():GetContentWidth()/2 , - 15 )
        self.icon[i]:AddChild(name)
        y = y - 130
    end

    --装备说明
    local tips = Util:Label(CPPGameLib.GetString("equip_explain"),Macros.TypefaceSize.largish,Macros.Color.keypoint)
    tips:SetPosition(self.img_biaoti:GetContentWidth()/2,self.img_biaoti:GetContentHeight()/2)
    self.img_biaoti:AddChild(tips)

    --装备说明
    self.richText = Util:RichText(nil,nil,276,-1)
    self.richText:SetAnchorPoint(0,1)
    self.richText:SetPosition(0,  -10 )
    self.img_biaoti:AddChild(self.richText)

    --信息栏
    self.lst_list = UIListViewEx.Create()
    self.lst_list:SetPosition( 2.5 , 7.5 )
    self.lst_list:SetAnchorPoint(0 , 0)
    self.lst_list:SetItemsMargin(5)
    self.lst_list:SetContentSize(self.img_listBg:GetContentWidth() - 5,self.img_listBg:GetContentHeight() - 15)
    self.img_listBg:AddChild(self.lst_list)

    for i,v in ipairs(self.info) do
        if self.ontype == v.item_type then
            self:SetShowUI(v)
            self.pitchOn:SetPosition(self.icon[i]:GetNode():GetPositionX() + 30 ,self.icon[i]:GetNode():GetPositionY())
        end
    end
end

function Modules.TemplTrade:SetShowUI( data )
    self.data = data
    --装备名字
    self.lab_name:SetText(data.name)
    self.lab_name:SetFontName(Util.FontType)
    self.lab_name:SetTextColor(unpack(Macros.Quality[data.quality]))
    self.lab_name:SetStroke(true)
    self.lab_name:SetStrokeColor(unpack(Macros.Quality_Stroke[data.quality]))

    --装备类型
    self.lab_type:SetText( CPPGameLib.GetString("equip_type"..data.item_type) )
    self.lab_type:SetFontName(Util.FontType)
    self.lab_type:SetTextColor(unpack(Macros.Quality[data.quality]))
    self.lab_type:SetStroke(true)
    self.lab_type:SetStrokeColor(unpack(Macros.Quality_Stroke[data.quality]))

    --装备品质
    self.lab_quality:SetText(data.quality)
    self.lab_quality:SetFontName(Util.FontType)
    self.lab_quality:SetTextColor(unpack(Macros.Quality[data.quality]))
    self.lab_quality:SetStroke(true)
    self.lab_quality:SetStrokeColor(unpack(Macros.Quality_Stroke[data.quality]))

    --装备说明
    self.richText:SetText(string.format(Macros.Color.content_rt,data.description))
  
    --清空数据
    self.lst_list:RemoveAllItems()

    self.userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
    self:Intensify(data)
end

function Modules.TemplTrade:Intensify(data)
    local headBg , lab = self:HeadBG(layou)
    lab:SetString(CPPGameLib.GetString("equip_intensify"))

    local natureLv = Util:Label(CPPGameLib.GetString("equip_intensifyLv") .. data.strengthen_level .. "/" .. (self.userDate.level * 2) ,Macros.TypefaceSize.slightly,Macros.Color.keypoint)
    natureLv:SetAnchorPoint(0,0.5)
    natureLv:SetPosition( 20 , headBg:GetContentHeight()/2 + 15 )
    headBg:AddChild(natureLv)

    local intensify = CallModuleFunc(ModuleType.EQUIP, "GetIntensify",data)
    local naturetype = Util:Label(Util:Nature(intensify.type,intensify.value) ,Macros.TypefaceSize.slightly,Macros.Color.keypoint)
    naturetype:SetAnchorPoint(0,0.5)
    naturetype:SetPosition( 20 , headBg:GetContentHeight()/2 - 5)
    headBg:AddChild(naturetype)

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN,"btn_btn_green")
    local btn_1 = Util:Button(sp,function ( ... )

        local info = {}
        info.refine_level = self.data.refine_level
        info.equip_heroid = self.data.equip_heroid
        info.ontype = 1
        info.refine_exp = self.data.refine_exp
        info.uid = self.data.uid
        info.info_id = self.data.info_id
        info.strengthen_level = self.data.strengthen_level
        CallModuleFunc(ModuleType.EQUIP, "OpenView", BaseViewType.EQUIPINTENSIFY,info) 

    end,CPPGameLib.GetString("equip_intensify"),Macros.TypefaceSize.normal,Macros.Color.keypoint)
    btn_1:SetPosition(headBg:GetContentWidth()/2 + 50 ,headBg:GetContentHeight()/2 - 20 )
    btn_1:SetAnchorPoint(0,0.5)
    headBg:AddChild(btn_1)

    self.lst_list:PushBackCustomItem(headBg)
    self:Refine(data)
end

function Modules.TemplTrade:Refine( data )
    local headBg , lab = self:HeadBG(layou)
    lab:SetString(CPPGameLib.GetString("equip_refineProperty"))

    local config = CPPGameLib.GetConfig("Equipment", data.info_id)
    local natureLv = Util:Label(CPPGameLib.GetString("equip_refineLv") .. data.refine_level .. "/" .. config.refine_level_max ,Macros.TypefaceSize.slightly,Macros.Color.keypoint)
    natureLv:SetAnchorPoint(0,0.5)
    natureLv:SetPosition( 20 , headBg:GetContentHeight()/2 + 15 )
    headBg:AddChild(natureLv)

    local refine = CallModuleFunc(ModuleType.EQUIP, "GetRefine",data)
    local y = headBg:GetContentHeight()/2 - 5
    for i,v in ipairs(refine) do
        local nature = Util:Label(Util:Nature(v.type,v.value) ,Macros.TypefaceSize.slightly,Macros.Color.keypoint)
        nature:SetAnchorPoint(0,0.5)
        nature:SetPosition( 20 , y )
        y = y -20
        headBg:AddChild(nature)
    end

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN,"btn_btn_green")
    local btn_1 = Util:Button(sp,function ( ... )

        local info = {}
        info.refine_level = self.data.refine_level
        info.equip_heroid = self.data.equip_heroid
        info.ontype = 2
        info.refine_exp = self.data.refine_exp
        info.uid = self.data.uid
        info.info_id = self.data.info_id
        info.strengthen_level = self.data.strengthen_level
        CallModuleFunc(ModuleType.EQUIP, "OpenView", BaseViewType.EQUIPINTENSIFY,info) 

    end,CPPGameLib.GetString("equip_refine"),Macros.TypefaceSize.normal,Macros.Color.keypoint)
    btn_1:SetPosition(headBg:GetContentWidth()/2 + 50 ,headBg:GetContentHeight()/2 - 20)
    btn_1:SetAnchorPoint(0,0.5)
    headBg:AddChild(btn_1)

    self.lst_list:PushBackCustomItem(headBg)
    self:Suit(data)
end

function Modules.TemplTrade:Suit( data )
    local headBg ,lab = self:HeadBG(true) 
    lab:SetString(CPPGameLib.GetString("equip_suit"))

    local equipLst , natureLst = CallModuleFunc(ModuleType.EQUIP, "GetSuit",data.uid)

    if equipLst ~= false then
        local x = 50
        for i,v in ipairs(equipLst) do
            local icon = LogicGUI.IconLayer.New(true)
            icon:SetData({type = Macros.Game.ObjType.EQUIP, id = v.id })
            -- icon:ShowEquipStrengthenLev(true)
            icon:SetPosition(x , headBg:GetContentHeight()/2 + 30 )
            icon:SetScaleFactor(0.7)
            headBg:AddChild(icon:GetNode())

            local name = Util:Label( v.name ,Macros.TypefaceSize.minimum,Macros.Color.keypoint)
            name:SetAnchorPoint(0.5,0.5)
            name:SetPosition(x , headBg:GetContentHeight()/2 - 15 )
            headBg:AddChild(name)  
            x = x + 90
        end

        local y = headBg:GetContentHeight()/2 - 39
        for i,v in ipairs(natureLst) do
            local txt = ""
            for s,n in ipairs(v) do
                txt = txt .. Util:Nature(n.type,n.value) .. "  "
            end

            local nature = Util:Label(
                                        CPPGameLib.GetString("equip_JS"..i) .. txt,
                                        Macros.TypefaceSize.minimum,Macros.Color.keypoint)
            nature:SetAnchorPoint(0,0.5)
            nature:SetPosition( 15 , y )
            y = y - 22
            headBg:AddChild(nature)
        end
        self.lst_list:PushBackCustomItem(headBg)
    end
end

--cell组件
function Modules.TemplTrade:HeadBG(ev)

    local height = 125
    if ev then
        height = 195
    end

    local layou = Util:Layout()
    layou:SetContentSize(self.lst_list:GetContentWidth(),height)
    layou:SetAnchorPoint(0.5,0.5)
    layou:SetTouchEnabled(true)

    local bg = Scale9SpriteEx.CreateWithImageFile(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "bg_sp9_lst_layer1"))
    bg:SetPosition(layou:GetContentWidth()/2 ,layou:GetContentHeight()/2)
    bg:SetContentSize(370,height)
    bg:SetAnchorPoint(0.5,0.5)
    layou:AddChild(bg)

    local headBg = Scale9SpriteEx.CreateWithImageFile(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bb_biaoti"))
    headBg:SetPosition(0,bg:GetContentHeight() - 3)
    headBg:SetAnchorPoint(0,1)
    bg:AddChild(headBg)

    local head = Util:Label("",nil,Macros.Color.content)
    head:SetAnchorPoint(0,0.5)
    head:SetPosition( 10 ,headBg:GetContentHeight()/2)
    headBg:AddChild(head)   

    return layou , head
end
