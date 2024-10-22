
-- @brief 装备分解界面
-- @author: yjg
-- @date: 2016年9月13日20:33:58

LogicGUI = LogicGUI or {}

LogicGUI.TemplEquipResolve = LogicGUI.TemplEquipResolve or BaseClass( LogicGUI.TemplRecycleResolve )

function LogicGUI.TemplEquipResolve:__init()
    self.key = true
    self.modes = {}
    self.lab_name = {}
    self.Button_remove = {}
end

function LogicGUI.TemplEquipResolve:__CloseCallback()
    --商店字样
    if self.shopText then
        self.shopText:RemoveSelf(true)
        self.shopText = nil
    end

    if self.lab then
        self.lab:RemoveSelf(true)
        self.lab = nil
    end
    
    if self.auto then
        self.auto:RemoveSelf(true)
        self.auto = nil
    end

    self:RemoveUi()

    print("xy" , "装备分解界面--------------")
    CallModuleFunc(ModuleType.RECYCLE, "RemoveAllOnData")
end

function LogicGUI.TemplEquipResolve:__SetShowUI()
    --商店字样
    self.shopText = Util:Name( CPPGameLib.GetString("tower_shopBtn"))
    self.shopText:SetPosition(self.Button_shop:GetContentWidth()/2 , 5)
    self.Button_shop:AddChild(self.shopText)

    --商店货币
    local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
    self.labCenter =  Util:LabCenter( { 
                                        { zi = CPPGameLib.GetString("role_info_tower_prestige").. CPPGameLib.GetString("common_colon") , mb = true , dx = Macros.TypefaceSize.minimum } ,
                                        { tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.TOWER_PRESTIGE) , dx = 0.7 } ,
                                        { zi = userDate.tower_prestige , mb = true , dx = Macros.TypefaceSize.minimum , jg = 5 } , 
                                    } )
    self.labCenter:SetPosition( self.Image_shopBg:GetContentWidth()/2 , self.Image_shopBg:GetContentHeight()/2)
    self.Image_shopBg:AddChild(self.labCenter)

     --提示
    local tips = GlobalCustomizedLogicAdapterMgr:Call("GetEquipRecycleTips")     
    self.lab = Util:RichText( tips or CPPGameLib.GetString("recycle_equipTips") ,Macros.TypefaceSize.slightly)
    self.lab:SetPosition(self.Panel_panel:GetContentWidth()/2 , 10 )
    self.lab:SetAnchorPoint(0.5,0)
    self.Panel_panel:AddChild(self.lab)

    --分解控制面板
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_green")
    self.auto  =  Util:Button(  
                                sp,CPPGameLib.Handler(self, self.OnAuto),
                                CPPGameLib.GetString("recycle_zidong"),
                                Macros.TypefaceSize.largish,nil, Macros.Color.button_yellow )
    self.auto:SetAnchorPoint(0 , 0.5 )
    self.auto:SetPosition( 30  , self.Panel_panel:GetContentHeight()/2 + 10 )
    self.Panel_panel:AddChild(self.auto)

    for i=1,5 do
        self["Button_AddBtn_"..i] = Util:WidgetButton( self:GetWidget("Button_AddBtn_"..i) , nil , CPPGameLib.Handler(self,self.OnAddBtnLst) )
        self["Panel_itemPos"..i] = self:GetWidget("Panel_itemPos"..i)
        self["Panel_itemPos"..i]:SetZOrder(20)
    end

    self.Button_shop = Util:WidgetButton( self:GetWidget("Button_shop") , nil , CPPGameLib.Handler(self,self.OnButtonShop) )
end

function LogicGUI.TemplEquipResolve:__UIShowUI()
    WidgetUnity.SetWidgetListener(self.auto, nil, nil, function ( ... )
        self:OnAuto()
    end, nil, 0.5*1000)
end

--按钮回调
function LogicGUI.TemplEquipResolve:OnAddBtnLst()
    CallModuleFunc(ModuleType.RECYCLE, "RemoveAllOnData")
    CallModuleFunc(ModuleType.RECYCLE, "OpenView", BaseViewType.RESOLVE_LST , Macros.Game.ObjType.EQUIP)
end

--商店按钮
function LogicGUI.TemplEquipResolve:OnButtonShop( ... )
    CallModuleFunc(ModuleType.TOWER, "OpenView", BaseViewType.TOWERSHOP)
end

--设置选中的按钮
function LogicGUI.TemplEquipResolve:ShowOnData( info )

    self.key = true

    if info then
        self:ResolveEffects( info )
        return 
    end

    self:RemoveUi( )

    --商店货币
    local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
    self.labCenter =  Util:LabCenter( { 
                                        { zi = CPPGameLib.GetString("role_info_tower_prestige").. CPPGameLib.GetString("common_colon") , mb = true , dx = Macros.TypefaceSize.minimum } ,
                                        { tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.TOWER_PRESTIGE) , dx = 0.7 } ,
                                        { zi = userDate.tower_prestige , mb = true , dx = Macros.TypefaceSize.minimum , jg = 5 } , 
                                    } )
    self.labCenter:SetPosition( self.Image_shopBg:GetContentWidth()/2 , self.Image_shopBg:GetContentHeight()/2)
    self.Image_shopBg:AddChild(self.labCenter)


    --选中数据
    local lst = CallModuleFunc(ModuleType.RECYCLE, "GetOnData")
    --红点
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_delete")

    for i , v in ipairs( lst ) do
        local type_ , info_ = ObjIDTool.GetInfo(v.info_id)

        --模型
        self.modes[i] = LogicGUI.IconLayer.New(false)
        self.modes[i]:SetData( {id = v.info_id , obj_info = v } )
        self.modes[i]:ShowEquipStrengthenLev(true)
        self["Panel_itemPos"..i]:AddChild(self.modes[i]:GetNode())
        PosTool.Center( self.modes[i] )

        --名字
        self.lab_name[i] = Util:Name( info_.name , Macros.TypefaceSize.normal , info_.quality or info_.init_quality )
        self.lab_name[i]:SetAnchorPoint(0.5,1)
        self.lab_name[i]:SetPosition( self["Panel_itemPos"..i]:GetContentWidth()/2 , -45 )
        self["Panel_itemPos"..i]:AddChild(self.lab_name[i])

        --设置红点
        self.Button_remove[i] = Util:Button(sp)
        self.Button_remove[i]:SetPosition( self["Panel_itemPos"..i]:GetContentWidth() + 35 , self["Panel_itemPos"..i]:GetContentHeight() + 35 )
        self["Panel_itemPos"..i]:AddChild(self.Button_remove[i])
        WidgetUnity.SetWidgetListener(self.Button_remove[i], nil, nil, function ()
            CallModuleFunc(ModuleType.RECYCLE, "RemoveOnData", v )
        end)

        --添加按钮屏蔽
        self["Button_AddBtn_"..i]:SetVisible(false)
    end
end

function LogicGUI.TemplEquipResolve:RemoveUi( )
    --重置选择控件 
    for i=1,10 do
        if self.modes[i] then
            self.modes[i]:RemoveSelf(true)
            self.modes[i] = nil
        end
        if self.Button_remove[i] then
            self.Button_remove[i]:RemoveSelf(true)
            self.Button_remove[i] = nil
        end

        if self["Button_AddBtn_"..i] then
            self["Button_AddBtn_"..i]:SetVisible(true)
        end

        if self.lab_name[i] then
            self.lab_name[i]:RemoveSelf(true)
            self.lab_name[i] = nil
        end
    end

    if self.labCenter then
        self.labCenter:RemoveSelf( true )
        self.labCenter = nil
    end
end

-- 自动添加
function LogicGUI.TemplEquipResolve:OnAuto()
    CallModuleFunc(ModuleType.RECYCLE, "RemoveAllOnData")
    local list = CallModuleFunc(ModuleType.RECYCLE, "DateLst", Macros.Game.ObjType.EQUIP )
    local index = 0
    if list and #list ~= 0 then
        for i,v in ipairs(list) do
            local data = CallModuleFunc(ModuleType.RECYCLE, "GetOnData")
            if i <= 5 and #data<5 and v.quality < 3  then
                index = index + 1 
                CallModuleFunc(ModuleType.RECYCLE, "AddOnData",v)
            else
                break
            end
        end
    end

    if index == 0 then 
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("recycle_insufficient_2") )
    end
end

--分解特效
function LogicGUI.TemplEquipResolve:ResolveEffects( info_ )
    --print("xy","ResolveEffects")
    if not self.layou then
        self.layou = Util:Layout( self.__parent_view.Panel_panel:GetContentWidth() , self.__parent_view.Panel_panel:GetContentHeight() )
        self.layou:SetAnchorPoint(0.5,0.5)
        self.layou:SetTouchEnabled(true)
        self.__parent_view.Panel_panel:AddChild( self.layou )
        PosTool.Center( self.layou )
    end

    for i=1,5 do
        if self.modes[i] then

            local function onComplete( ... )
                self:RemoveUi( )

                self["Panel_itemPos"..i]:SetPosition( self["Button_AddBtn_"..i]:GetPositionX() , self["Button_AddBtn_"..i]:GetPositionY() )
                CPPActionManager.ScaleTo(self["Panel_itemPos"..i] ,  0 , 1)

            end

            local function baozha( ... )
                local function func ( ... )

                    if self.key == true then
                        self.key = false
                        CallModuleFunc(ModuleType.RECYCLE, "RecycleDecomposerReq",info_)
                        if self.layou then
                            self.layou:RemoveSelf(true)
                            self.layou = nil
                        end
                    end

                end
                local effect_id , effect_con = self:__CreateEffect({id = 100005 , loop = false }, self.__root_widget)
                effect_con:SetPosition( self.__root_widget:GetContentWidth()/2 - 10 , self.__root_widget:GetContentHeight()/2 - 30)
                CPPActionManager.DelayTo(self["Panel_itemPos"..i] , 1 , GlobalCallbackMgr:AddCallBackFunc(func))
            end
            CPPActionManager.DelayTo(self["Panel_itemPos"..i] , 0.2 , GlobalCallbackMgr:AddCallBackFunc(baozha))

            local onComplete_callback = GlobalCallbackMgr:AddCallBackFunc(onComplete)
            CPPActionManager.JumpTo(self["Panel_itemPos"..i] , 0.6 , self.__root_widget:GetContentWidth()/2 - 10 , self.__root_widget:GetContentHeight()/2 - 30 , 60 , 1);
            CPPActionManager.ScaleTo(self["Panel_itemPos"..i] ,  0.6 , 0.5, onComplete_callback)
        end
    end

end