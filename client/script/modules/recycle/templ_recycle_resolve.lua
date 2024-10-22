
-- @brief 分解界面
-- @author: yjg
-- @date: 2016年9月13日20:33:58

LogicGUI = LogicGUI or {}

LogicGUI.TemplRecycleResolve = LogicGUI.TemplRecycleResolve or BaseClass(BaseTabView)

function LogicGUI.TemplRecycleResolve:__init()
end

--选项卡界面创建完成回调
function LogicGUI.TemplRecycleResolve:__LoadedCallback()
    self:__Getwidget()
    self:__ShowUI()
end

--选项卡界面打开回调
function LogicGUI.TemplRecycleResolve:__OpenCallback()
    self:__SetShowUI()
    self:__UIShowUI()
    
    self:BindGlobalEvent(Macros.Event.LeadModule.CHECK_NEXT_SECTION, function(_, pre_section_info)
        --新手指引特殊操作检测(英雄回收-分解)
        CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.HERO_RECOVERY_DECOMPOSE, self.confirm)                        
    end)        
end

--选项卡界面关闭回调
function LogicGUI.TemplRecycleResolve:__CloseCallback()
end

--选项卡界面销毁回调(释放非托管资源)
function LogicGUI.TemplRecycleResolve:__Dispose()
end

--获得所有组件
function LogicGUI.TemplRecycleResolve:__Getwidget()
    print("xy","__Getwidget")
    for i=1,5 do
        self["Button_AddBtn_"..i] = Util:WidgetButton( self:GetWidget("Button_AddBtn_"..i) , nil , CPPGameLib.Handler(self,self.OnAddBtnLst) )
        self["Panel_itemPos"..i] = self:GetWidget("Panel_itemPos"..i)
        self["Panel_itemPos"..i]:SetZOrder(20)
    end

    self.Panel_panel = self:GetWidget("Panel_panel")
    self.Panel_panel:SetBackGroundColor(0, 0, 0)
    self.Panel_panel:SetBackGroundColorType(1)
    self.Panel_panel:SetBackGroundColorOpacity(120)
    
    self.Image_shopBg = self:GetWidget("Image_shopBg")

    self.Button_shop = Util:WidgetButton( self:GetWidget("Button_shop") , nil , CPPGameLib.Handler(self,self.OnButtonShop) )
end

--显示UI
function LogicGUI.TemplRecycleResolve:__ShowUI()

    self.shopText = Util:Name( " " )
    self.shopText:SetPosition(self.Button_shop:GetContentWidth()/2 , 5)
    self.Button_shop:AddChild(self.shopText)

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_blue")
    self.confirm  =  Util:Button(   
                                    sp,CPPGameLib.Handler(self, self.OnConfirm),
                                    CPPGameLib.GetString("Awaken_Equip_Resolve"),
                                    Macros.TypefaceSize.largish,nil, Macros.Color.button_red )
    self.confirm:SetAnchorPoint(1 , 0.5 )
    self.confirm:SetPosition( self.Panel_panel:GetContentWidth() - 30  , self.__parent_view.Panel_panel:GetContentHeight()/2 + 10 )
    self.Panel_panel:AddChild(self.confirm)

    -- --新手指引特殊操作检测(英雄回收-自动选择)
    -- CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.HERO_RECOVERY_AUTO_CHOICE, self.auto)                  
end


function LogicGUI.TemplRecycleResolve:__SetShowUI()
end

function LogicGUI.TemplRecycleResolve:__UIShowUI()
end

function LogicGUI.TemplRecycleResolve:OnButtonShop()
end

function LogicGUI.TemplRecycleResolve:ShowOnData()
    print("xy" , "LogicGUI.TemplRecycleResolve:ShowOnData()---------------" )
end

--确定
function LogicGUI.TemplRecycleResolve:OnConfirm()
    CallModuleFunc(ModuleType.RECYCLE, "RecycleDecomposeReturnReq")
end

-- function LogicGUI.TemplRecycleResolve:OnAddBtnLst()
-- end