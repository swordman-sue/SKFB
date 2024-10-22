
-- @brief 重生界面
-- @author: yjg
-- @date: 2016年9月18日10:40:36


LogicGUI = LogicGUI or {}

LogicGUI.TemplRecycleRebirth = LogicGUI.TemplRecycleRebirth or BaseClass(BaseTabView)

function LogicGUI.TemplRecycleRebirth:__init()
end

--选项卡界面创建完成回调
function LogicGUI.TemplRecycleRebirth:__LoadedCallback()
    self:__Getwidget()
    self:__ShowUI()
end

--选项卡界面打开回调
function LogicGUI.TemplRecycleRebirth:__OpenCallback()
    self:__SetShowUI()
end

--选项卡界面关闭回调
function LogicGUI.TemplRecycleRebirth:__CloseCallback()
    -- self:RemoveBtnUi()
    self:RemoveUi()
end

--选项卡界面销毁回调(释放非托管资源)
function LogicGUI.TemplRecycleRebirth:__Dispose()

end


--获得所有组件
function LogicGUI.TemplRecycleRebirth:__Getwidget()
    self.Panel_rebirthItemPos = self:GetWidget("Panel_rebirthItemPos1_0")

    self.Button_rebirthAddBtn = Util:WidgetButton( self:GetWidget("Button_rebirthAddBtn") , nil , CPPGameLib.Handler(self,self.OnAddBtnLst) )

    self.Panel_panel = self:GetWidget("Panel_panel")
    self.Panel_panel:SetBackGroundColor(0, 0, 0)
    self.Panel_panel:SetBackGroundColorType(1)
    self.Panel_panel:SetBackGroundColorOpacity(120)

    self.Image_shopBg = self:GetWidget("Image_shopBg")

    self.Button_shop = self:GetWidget("Button_shop")
end

--显示UI
function LogicGUI.TemplRecycleRebirth:__ShowUI()
    -- print("重生控制面板")
    self.Button_shop:SetVisible(false)
    self.Image_shopBg:SetVisible(false)

    -- 重生控制面板
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_blue")
    self.confirm = Util:Button( 
                                sp,CPPGameLib.Handler(self, self.OnConfirm),
                                CPPGameLib.GetString("recycle_chongsheng"),
                                nil,nil, Macros.Color.button_red
                                )
    self.confirm:SetAnchorPoint(0.5 , 0.5 )
    self.confirm:SetPosition( self.Panel_panel:GetContentWidth()/2  , self.Panel_panel:GetContentHeight()/2)
    self.Panel_panel:AddChild(self.confirm)

end


--设置选中的按钮
function LogicGUI.TemplRecycleRebirth:ShowOnData( info_ )

    --选中数据
    local lst = CallModuleFunc(ModuleType.RECYCLE, "GetOnData")
    --红点
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_delete")

    for i , v in ipairs( lst ) do
        local type_ , info_ = ObjIDTool.GetInfo(v.info_id)
        --名字
        self.lab_name = Util:Name( info_.name , Macros.TypefaceSize.normal , info_.quality or info_.init_quality )
        self.lab_name:SetAnchorPoint(0.5,1)
        self.lab_name:SetPosition( self.Panel_rebirthItemPos:GetContentWidth()/2 , 0 )
        self.Panel_rebirthItemPos:AddChild(self.lab_name)

        --设置红点
        self.Button_remove = Util:Button(sp)
        self.Button_remove:SetPosition( self.Panel_rebirthItemPos:GetContentWidth() + 20 , self.Panel_rebirthItemPos:GetContentHeight() + 60)
        self.Panel_rebirthItemPos:AddChild(self.Button_remove)
        WidgetUnity.SetWidgetListener(self.Button_remove, nil, nil, function ()
            CallModuleFunc(ModuleType.RECYCLE, "RemoveOnData", v )
        end)

        --添加按钮屏蔽
        self.Button_rebirthAddBtn:SetVisible(false)
    end
end

--设置显示的UI
function LogicGUI.TemplRecycleRebirth:__SetShowUI()

end

function LogicGUI.TemplRecycleRebirth:RemoveUi(info_)
    if self.Button_remove then
        self.Button_remove:RemoveSelf(true)
        self.Button_remove = nil
    end
    
    if self.lab_name then
        self.lab_name:RemoveSelf(true)
        self.lab_name = nil
    end
end

function LogicGUI.TemplRecycleRebirth:RemoveBtnUi()

end

function LogicGUI.TemplRecycleRebirth:OnAddBtnLst()
end

function LogicGUI.TemplRecycleRebirth:OnConfirm()
    local data = CallModuleFunc(ModuleType.RECYCLE, "RecycleResetReturnReq")
end
