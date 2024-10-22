
-- @brief 卡牌分解界面
-- @author: yjg
-- @date: 2017年11月23日20:44:24

LogicGUI = LogicGUI or {}

LogicGUI.TemplCardResolve = LogicGUI.TemplCardResolve or BaseClass( LogicGUI.TemplRecycleResolve )

function LogicGUI.TemplCardResolve:__init()
    self.key = true
    self.modes = {}
    self.lab_name = {}
    self.Button_remove = {}
end

function LogicGUI.TemplCardResolve:__CloseCallback()
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

    print("xy" , "卡牌分解界面--------------")
    CallModuleFunc(ModuleType.RECYCLE, "RemoveAllOnData")
end

function LogicGUI.TemplCardResolve:__SetShowUI()
    --商店字样
    self.shopText = Util:Name( CPPGameLib.GetString("divination_starShop"))
    self.shopText:SetPosition(self.Button_shop:GetContentWidth()/2 , 5)
    self.Button_shop:AddChild(self.shopText)

    --商店货币
    local star_soul = CallModuleFunc(ModuleType.ROLE,"GetPropValue" , Macros.Game.RolePropType.CARD_CURRENCY )

    self.labCenter =  Util:LabCenter( { 
                                        { zi = CPPGameLib.GetString("role_info_starsoul").. CPPGameLib.GetString("common_colon") , mb = true , dx = Macros.TypefaceSize.minimum } ,
                                        { tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.CARD_CURRENCY) , dx = 0.7 } ,
                                        { zi = star_soul , mb = true , dx = Macros.TypefaceSize.minimum , jg = 5 } , 
                                    } )
    self.labCenter:SetPosition( self.Image_shopBg:GetContentWidth()/2 , self.Image_shopBg:GetContentHeight()/2)
    self.Image_shopBg:AddChild(self.labCenter)

    --提示
    self.lab = Util:RichText( CPPGameLib.GetString("recycle_cardTips") ,Macros.TypefaceSize.slightly)
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

    --  --新手指引特殊操作检测(英雄回收-自动选择)
    -- CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.HERO_RECOVERY_AUTO_CHOICE, self.auto) 

    self.Button_shop = Util:WidgetButton( self:GetWidget("Button_shop") , nil , CPPGameLib.Handler(self,self.OnButtonShop) )
     
end


function LogicGUI.TemplCardResolve:__UIShowUI()
end

--按钮回调
function LogicGUI.TemplCardResolve:OnAddBtnLst()
    CallModuleFunc(ModuleType.RECYCLE, "OpenView", BaseViewType.RESOLVE_LST , Macros.Game.ObjType.FAIRY )
end

--商店按钮
function LogicGUI.TemplCardResolve:OnButtonShop( ... )
    CallModuleFunc(ModuleType.DIVINATION , "OpenView" , BaseViewType.DIVINATION_SHOP )
end

--设置选中的按钮
function LogicGUI.TemplCardResolve:ShowOnData( info )
    self.key = true

    if info then
        self:ResolveEffects( info )
        return 
    end
    self:RemoveUi( )

    --商店货币
    local star_soul = CallModuleFunc(ModuleType.ROLE,"GetPropValue" , Macros.Game.RolePropType.CARD_CURRENCY )
    self.labCenter =  Util:LabCenter( { 
                                        { zi = CPPGameLib.GetString("role_info_starsoul").. CPPGameLib.GetString("common_colon") , mb = true , dx = Macros.TypefaceSize.minimum } ,
                                        { tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.CARD_CURRENCY) , dx = 0.7 } ,
                                        { zi = star_soul , mb = true , dx = Macros.TypefaceSize.minimum , jg = 5 } , 
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

function LogicGUI.TemplCardResolve:RemoveUi( )
    --重置选择控件 
    for i=1,10 do
        if self.modes[i] then
            self.modes[i]:DeleteMe()
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
function LogicGUI.TemplCardResolve:OnAuto()
    local list = CallModuleFunc(ModuleType.RECYCLE, "DateLst", Macros.Game.ObjType.FAIRY )
    local index = 0
    if list and #list ~= 0 then
        for i,v in ipairs(list) do
            local data = CallModuleFunc(ModuleType.RECYCLE, "GetOnData")
            if i <= 5 and #data<5 and v.quality < 3 then
                if v.strengthen_level == 1 and v.refine_level == 0 then
                    index = index + 1 
                    CallModuleFunc(ModuleType.RECYCLE, "AddOnData",v)
                end
            else
                break
            end
        end
    end

    if index == 0 then 
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("recycle_insufficient_3") )
    end
end

--分解特效
function LogicGUI.TemplCardResolve:ResolveEffects( info_ )
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