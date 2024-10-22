
--[[
%% @module: 限时召唤
%% @author: yjg
%% @created: 2017年6月27日10:10:43
--]]
-- Panel_welfare
-- summon

Modules = Modules or {}

Modules.TemplSummon = Modules.TemplSummon or BaseClass(BaseTabView)

function Modules.TemplSummon:__init()
	-- self.__tab_params_ex = {15 , true}
	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_ACTIVIITY_ONE_RECHARGE_ITEM)
end

--选项卡界面创建完成回调
function Modules.TemplSummon:__LoadedCallback()
	self:__Getwidget()
	self:__ShowUI()
end

--选项卡界面打开回调
function Modules.TemplSummon:__OpenCallback()
    CallModuleFunc(ModuleType.ACTIVITY, "LimitRecruitDataReq")
    self:__SetShowUI()
    self:BindGlobalEvent(Macros.Event.ActivityModule.ACTIVITY_RECRUIT,function (_, data)
        self:__SetShowUI()
    end)
end

--选项卡界面关闭回调
function Modules.TemplSummon:__CloseCallback()
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.TemplSummon:__Dispose()
end

--获得组件
function Modules.TemplSummon:__Getwidget()
    self.Image_contentLst = self:GetWidget("Image_contentLst") 

    self.Panel_welfare = self:GetWidget("Panel_welfare") 
end

--创建组件
function Modules.TemplSummon:__ShowUI()
    local params =  
    {
        item_class = Modules.TemplSummonItem,
        item_width = 660.00,
        item_height = 140.00 ,
        row = 4,
        col = 1,
        view_height = self.Image_contentLst:GetContentHeight() - 5  ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.Image_contentLst:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode())
    
    local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.RECRUIT )

    local sp = Resource.PathTool.GetUIResPath("activitybig/summon" , true )
    local level_gift = Util:Sprite( sp )
    level_gift:SetAnchorPoint(0.5,0.5)
    self.Panel_welfare:AddChild( level_gift )
    PosTool.Center( level_gift )

    local activity_starTime = Util:Label( CPPGameLib.GetString("activity_starTime" , TimeTool.GetTimeFormat( info.star  , TimeTool.defaul_format[6] ) ) )
    activity_starTime:SetAnchorPoint( 0 , 0.5 )
    activity_starTime:SetPosition( 5 , 43 )
    self.Panel_welfare:AddChild( activity_starTime )

    local activity_overTime = Util:Label( CPPGameLib.GetString("activity_overTime" , TimeTool.GetTimeFormat( info.over  , TimeTool.defaul_format[6] ) ) )
    activity_overTime:SetAnchorPoint( 0 , 0.5 )
    activity_overTime:SetPosition( 5 , 18 )
    self.Panel_welfare:AddChild( activity_overTime )
    
end

function Modules.TemplSummon:__SetShowUI()
    local t1 = CallModuleFunc(ModuleType.ACTIVITY, "GetLimitRecruitCnf")
    self.lst_list:SetDataList( t1 )
end


Modules = Modules or {}
Modules.TemplSummonItem = Modules.TemplSummonItem or BaseClass(GUI.ScrollItem)

function Modules.TemplSummonItem:__init()
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_ACTIVIITY_ONE_RECHARGE_ITEM)   
    self:__Getwidget()
end

function Modules.TemplSummonItem:__delete()
    if self.chongzhiEffect_id then
        self:__DelEffect(self.chongzhiEffect_id)
        self.chongzhiEffect_id = nil
    end
    if self.btnEffect_id then
        self:__DelEffect(self.btnEffect_id)
        self.btnEffect_id = nil
    end  
end

function Modules.TemplSummonItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self:__ShowUI( data )
end

function Modules.TemplSummonItem:__Getwidget()
    --条目
    self.Image_item = self:GetWidget("Image_item")
    --图标容器
    self.Image_content = self:GetWidget("Image_content")
    --按钮占位
    self.Panel_btn = self:GetWidget("Panel_btn")
    --标题
    self.Text_item = self:GetWidget("Text_item")
end

function Modules.TemplSummonItem:__ShowUI(data)

    --提示
    self.tips = Util:RichText( string.format(Macros.Color.headline_rt , CPPGameLib.GetString("recharge_summonTips" , data.context ) ) , Macros.TypefaceSize.largish )
    self.tips:SetAnchorPoint( 0 , 0.5 )
    self.tips:SetPosition(0 , self.Text_item:GetContentHeight()/2 )
    self.Text_item:AddChild( self.tips )

    --图标
    for i , v in ipairs( data.item ) do
        --头像
        local icon = LogicGUI.IconLayer.New()
        self.Image_content:AddChild(icon:GetNode())
        local iconData = {id = v.id , num = v.num }
        icon:SetData(iconData)
        icon:SetPosition( (icon:GetNode():GetContentWidth()/2 ) + 10 + ( (i-1) * (icon:GetNode():GetContentWidth() + 10 ) ) , self.Image_content:GetContentHeight()/2 )
    end

    self:Transfer( data )
end

--设置可以购买
function Modules.TemplSummonItem:Transfer(data)
	local max = CallModuleFunc(ModuleType.ACTIVITY, "GetLimitRecruitMax")
    local record = CallModuleFunc(ModuleType.ACTIVITY, "GetLimitRecruitRecord" , data.id )

    --充值够了
    if max and data.context <= max then
    	--有记录
    	if record then
    		self:SetNoMay( data )
    	else
    		self:SetMay( data )
    	end
    else
    	self:SetMay( data )
    end
end

--设置可以购买
function Modules.TemplSummonItem:SetMay( data )

    --按钮
    if self.btn then
        self.btn:RemoveSelf(true)
        self.btn = nil
    end

    --按钮
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen2")
    self.btn = Util:Button( sp , 
                                 CPPGameLib.Handler(self, self.OnBtn) , 
                                 CPPGameLib.GetString("activity_vipRecharge") , 
                                 Macros.TypefaceSize.button , 
                                 nil , Macros.Color.button_yellow
                                 )
    self.btn:SetPosition( self.Panel_btn:GetContentWidth()/2 , self.Panel_btn:GetContentHeight()/2 )
    self.Panel_btn:AddChild(self.btn) 

    local btnEffect_con = nil
    self.btnEffect_id , btnEffect_con = self:__CreateEffect({res_name = "ui_anniuzoubian2"} , self.btn )
    btnEffect_con:SetPosition(self.btn:GetContentWidth()/2 , self.btn:GetContentHeight()/2 )
    GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , false)

    local btnEffect_con = nil
    self.chongzhiEffect_id , btnEffect_con = self:__CreateEffect({res_name = "UI_chongzhitongyong"} , self.btn )
    btnEffect_con:SetPosition(self.btn:GetContentWidth()/2 , self.btn:GetContentHeight()/2 )
    GlobalEffectMgr:SetEffectVisible(self.chongzhiEffect_id , false)
    

    local max = CallModuleFunc(ModuleType.ACTIVITY, "GetLimitRecruitMax")
    --如果充值不够的话
    if not max or data.context > max then
        self.btn:SetText( CPPGameLib.GetString("activity_call") )
        -- GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , true)
        WidgetUnity.SetWidgetListener(self.btn, nil, nil, function ( ... )
            CallModuleFunc(ModuleType.HERO , "OpenView", BaseViewType.HERO_RECRUIT)
        end, nil, 0.5*1000)
    else
        self.btn:SetText( CPPGameLib.GetString("activity_vipRecharge") )
        GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , true)

        WidgetUnity.SetWidgetListener(self.btn, nil, nil, function ( ... )
            self:OnBtn()
        end, nil, 0.5*1000)
	end

    -- 累充数据
    local tab = {
                    { zi = CPPGameLib.GetString( "progress_Text" ) , ys = Macros.Color.pack_color , dx = Macros.TypefaceSize.slightly },
                    { zi = (max or 0) .. "/" .. data.context , ys = Macros.Color.pack_color , dx = Macros.TypefaceSize.slightly },
                }
    --用作好几个组件拼起来居中显示的强迫症控件 2.0版
    local item = Util:LabCenter(tab)
    item:SetAnchorPoint( 0.5 , 0 )
    item:SetPosition( self.btn:GetContentWidth()/2 , self.btn:GetContentHeight()  )
    self.btn:AddChild( item )
end

--设置买过了
function Modules.TemplSummonItem:SetNoMay(data)

    --按钮
    if self.btn then
    	self.btn:RemoveSelf(true)
    	self.btn = nil
    end

    --按钮
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "receive_icon" , true)
	self.btn = Util:Button( sp , CPPGameLib.Handler(self, self.OnNoBtn) )
	self.btn:SetPosition( self.Panel_btn:GetContentWidth()/2 , self.Panel_btn:GetContentHeight()/2 )
	self.Panel_btn:AddChild(self.btn) 
	self.btn:SetTouchEnabled(false)
end

--购买回调
function Modules.TemplSummonItem:OnBtn(data)
    local info = {}
    info.data_id = self.__data.id
    CallModuleFunc(ModuleType.ACTIVITY, "TakeLimitActivityRewardReq" , info )
end

--不可购买回调
function Modules.TemplSummonItem:OnNoBtn(data)

end