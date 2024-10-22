
-- @brief 卡牌重生界面
-- @author: yjg
-- @date: 2017年11月23日20:44:15

LogicGUI = LogicGUI or {}

LogicGUI.TemplCardRebirth = LogicGUI.TemplCardRebirth or BaseClass( LogicGUI.TemplRecycleRebirth )

function LogicGUI.TemplCardRebirth:__init()
    self.key = true
    -- self.modes = {}
    -- self.lab_name = {}
    -- self.Button_remove = {}
end

function LogicGUI.TemplCardRebirth:__CloseCallback()
    LogicGUI.TemplRecycleRebirth.__CloseCallback(self )

    --商店字样
    if self.shopText then
        self.shopText:RemoveSelf(true)
        self.shopText = nil
    end

    if self.lab then
        self.lab:RemoveSelf(true)
        self.lab = nil
    end

    if self.consume then
        self.consume:RemoveSelf(true)
        self.consume = nil
    end

    print("xy" , "卡牌重生界面--------------")
    CallModuleFunc(ModuleType.RECYCLE, "RemoveAllOnData")
end

function LogicGUI.TemplCardRebirth:__SetShowUI()
    --提示
    self.lab = Util:RichText( CPPGameLib.GetString("recycle_rebirthCardTips") ,Macros.TypefaceSize.slightly)
    self.lab:SetPosition(self.Panel_panel:GetContentWidth()/2 , 10 )
    self.lab:SetAnchorPoint(0.5,0)
    self.Panel_panel:AddChild(self.lab)

    self.Button_rebirthAddBtn = Util:WidgetButton( self:GetWidget("Button_rebirthAddBtn") , nil , CPPGameLib.Handler(self,self.OnAddBtnLst) )
end


function LogicGUI.TemplCardRebirth:__UIShowUI()
end

--按钮回调
function LogicGUI.TemplCardRebirth:OnAddBtnLst()
    CallModuleFunc(ModuleType.RECYCLE, "OpenView", BaseViewType.RECYCLE_LST , Macros.Game.ObjType.FAIRY )
end

--设置选中的按钮
function LogicGUI.TemplCardRebirth:ShowOnData( info )

    self.key = true

    if info then
        self:ResolveEffects( info )
        return 
    end
    self:RemoveUi( )

    LogicGUI.TemplRecycleRebirth.ShowOnData(self )
    
    self.reset = 0
    --选中数据
    local lst = CallModuleFunc(ModuleType.RECYCLE, "GetOnData")
    --红点
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_delete")

    for i , v in ipairs( lst ) do
        local type_ , info_ = ObjIDTool.GetInfo(v.info_id)

        self.reset = info_.reset_cost_diamond
        --模型
        self.modes = LogicGUI.IconLayer.New(false)
        self.Panel_rebirthItemPos:AddChild(self.modes:GetNode())
        PosTool.Center(self.modes:GetNode())
        self.modes:SetData( {id = v.info_id , obj_info = v } )
        self.modes:ShowTreasureStrengthenLev(true)

        --名字
        self.lab_name:SetPosition( self.Panel_rebirthItemPos:GetContentWidth()/2 , -80 )

        --设置红点
        self.Button_remove:SetPosition( self.Panel_rebirthItemPos:GetContentWidth() + 20 , self.Panel_rebirthItemPos:GetContentHeight() + 60)
    end

    if self.reset ~= 0 then
        self.consume = Util:MoneyControl( Macros.Game.RolePropType.DIAMOND , 0.8 , self.reset ,nil,nil,10)
        self.consume:SetPosition(self.confirm:GetContentWidth()/2 , self.confirm:GetContentHeight() + 15)
        self.confirm:AddChild(self.consume)
    end
end

function LogicGUI.TemplCardRebirth:RemoveUi( )
    LogicGUI.TemplRecycleRebirth.RemoveUi(self )
    
    --重置选择控件 
    if self.modes then
        self.modes:DeleteMe()
        self.modes = nil
    end

    if self.Button_rebirthAddBtn then
        self.Button_rebirthAddBtn:SetVisible(true)
    end

    if self.consume then
        self.consume:RemoveSelf(true)
        self.consume = nil
    end
end

--分解特效
function LogicGUI.TemplCardRebirth:ResolveEffects( info_ )
    local moneyNum = CallModuleFunc(ModuleType.ROLE,"GetPropValue", Macros.Game.RolePropType.DIAMOND )
    if moneyNum >= self.reset then 
        if self.modes then
            local function onComplete( ... )
                CallModuleFunc(ModuleType.RECYCLE, "RecycleResetReq",info_)
                self.modes:DeleteMe()
                self.modes = nil
                self.Button_rebirthAddBtn:SetVisible(true)
            end

            local effect_id , effect_con = self:__CreateEffect({id = 100004 , loop = false }, self.Panel_rebirthItemPos)
            effect_con:SetPosition( self.Panel_rebirthItemPos:GetContentWidth()/2 - 10 , self.Panel_rebirthItemPos:GetContentHeight()/2 - 30)
            CPPActionManager.DelayTo(self.Panel_rebirthItemPos , 0.5 , GlobalCallbackMgr:AddCallBackFunc(onComplete))
        end
    else
        local txt = "currency_insufficient_"..Macros.Game.RolePropType.DIAMOND
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString(txt) )
    end
end