--[[
%% @module: 充值回馈
%% @author: yjg
%% @created: 2018年2月2日11:33:36
--]]

Modules = Modules or {} 
Modules.FeedbackView = Modules.FeedbackView or BaseClass(BaseView)

function Modules.FeedbackView:__init()
    self.__layout_name = "recharge_feedback"	
    self.__bg_params = {type = BaseView.BGType.SOLID} 
    self.__title_bar_params = 
    {
      -- title = CPPGameLib.GetString("vip"), 
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

    self.__tab_params_ex = {0}

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.VIP,true)  
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.FEEDBACK) 
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.FEEDBACK,true) 
end

function Modules.FeedbackView:__OpenCallback( )

    --开服天数
    local time = CallModuleFunc(ModuleType.ROLE,"GetOpenClothing")
    self:__Getwidget()
    self:__ShowUI()
    self:__Server()
end	

function Modules.FeedbackView:__Dispose()
end

function Modules.FeedbackView:__Getwidget( )
    --背景
    self.Image_bg = self:GetWidget("Image_bg")
    --标题
    self.Image_headline = self:GetWidget("Image_headline")
    --充值按钮
    self.Button_recharge = Util:WidgetButton( self:GetWidget("Button_recharge") , nil , function ( ... )
    end )
    --按钮字样
    self.Image_btn_text = self:GetWidget("Image_btn_text")

    local btnEffect_con = nil
    self.btnEffect_id , btnEffect_con = self:__CreateEffect({res_name = "ui_anniuzoubian2"} , self.Button_recharge )
    btnEffect_con:SetPosition(self.Button_recharge:GetContentWidth()/2 , self.Button_recharge:GetContentHeight()/2 )
    GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , false )

    local btnEffect_con = nil
    self.btnRapid_id , btnEffect_con = self:__CreateEffect({res_name = "UI_chongzhitongyong"} , self.Button_recharge )
    btnEffect_con:SetPosition(self.Button_recharge:GetContentWidth()/2 , self.Button_recharge:GetContentHeight()/2 )
    GlobalEffectMgr:SetEffectVisible(self.btnRapid_id , false)


    --领取类型
    self.Image_type = self:GetWidget("Image_type")
    self.Image_type:SetVisible( false )
    self.Image_type:SetPosition( self.Image_type:GetPositionX() , self.Image_type:GetPositionY() + 15 )
    --列表容器
    self.Panel_btn = self:GetWidget("Panel_btn")
    --关闭
    self.Button_close = Util:WidgetButton( self:GetWidget("Button_close") , nil , function ( ... )
        self:CloseManual()
    end )

    --缩放容器
    self.Panel_zoom = self:GetWidget("Panel_1")
    self.Panel_zoom:SetVisible(false)
    --领取
    self.Button_get = Util:WidgetButton( self:GetWidget("Button_get") , nil , CPPGameLib.Handler(self, self.OnButtonGet) )
    local btnEffect_id , btnEffect_con = self:__CreateEffect({res_name = "ui_anniuzoubian2"} , self.Button_get )
    btnEffect_con:SetPosition(self.Button_get:GetContentWidth()/2 , self.Button_get:GetContentHeight()/2 )

    self.Button_show = Util:WidgetButton( self:GetWidget("Button_show") , nil , CPPGameLib.Handler(self, self.OnButtonShow) )
    local btnRapid_id , btnEffect_con = self:__CreateEffect({res_name = "UI_chongzhihuikui"} , self.Button_show )
    btnEffect_con:SetPosition(self.Button_show:GetContentWidth()/2 , self.Button_show:GetContentHeight()/2 )
    
    local btnRapid_id , btnEffect_con = self:__CreateEffect({res_name = "UI_chongzhihuikui_di"} , self.Button_show , -1 )
    btnEffect_con:SetPosition(self.Button_show:GetContentWidth()/2 , self.Button_show:GetContentHeight()/2 - 50 )

end 

function Modules.FeedbackView:__Server()
    CallModuleFunc(ModuleType.FEEDBACK , "RechargeRewardDataReq")
    self:BindGlobalEvent( Macros.Event.FeedbackModule.FEEDBACK_DATA , function ( ... )
        self:SetShowUi()
        self:SetRed()
    end)
end

function Modules.FeedbackView:__ShowUI( )
    --提示
    self.feedbackTips = Util:Label( CPPGameLib.GetString( "feedbackTips" , 0 ) )
    self.feedbackTips:SetAnchorPoint( 0 , 0.5 )
    self.feedbackTips:SetPosition( 255 , 150 )
    self.Image_bg:AddChild( self.feedbackTips )

    --奖励列表
    self.awardLst = {}
    for i = 1 , 4 do
        self.awardLst[i] = LogicGUI.IconLayer.New()
        self.awardLst[i]:SetData( { id = 13999 } )
        self.awardLst[i]:SetAnchorPoint( 0 , 0.5 )
        self.awardLst[i]:SetPosition( 255 + ( (i - 1) * 90 ) , 95 )
        self.Image_bg:AddChild( self.awardLst[i]:GetNode() )
        self.awardLst[i]:SetScaleFactor( 0.9 )
    end

    local cnf = CallModuleFunc(ModuleType.FEEDBACK , "GetRechargeExtraReward")

    local path = Resource.PathTool.GetBigIconPath( cnf.award[1].id )
    local award = Util:Sprite( path )
    award:SetAnchorPoint( 0.5 , 0.5 )
    award:SetPosition( 710 , 325 )
    self.Image_bg:AddChild( award )

    --提示
    self.feedbackTips2 = Util:Label( CPPGameLib.GetString( "feedbackTips2" , 0 ) )
    self.feedbackTips2:SetAnchorPoint( 0.5 , 0.5 )
    self.feedbackTips2:SetPosition( 710 , 225 )
    self.Image_bg:AddChild( self.feedbackTips2 )
    self.feedbackTips2:SetVisible( false )

    --名字
    local _ , info = ObjIDTool.GetInfo( cnf.award[1].id )
    local name = Util:Name( info.name , nil , info.init_quality or info.init_quality )
    name:SetAnchorPoint( 0.5 , 0.5 )
    name:SetPosition( 710 , 430 )
    self.Image_bg:AddChild( name )

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_seven_get" , true)
    self.seven_get = Util:Sprite( path )
    self.seven_get:SetVisible(false)
    self.seven_get:SetAnchorPoint( 0.5 , 0.5 )
    self.Button_show:AddChild( self.seven_get )
    PosTool.Center( self.seven_get )

    local feedbackBegin = Util:Label( CPPGameLib.GetString("feedbackBegin") , Macros.TypefaceSize.slightly )
    feedbackBegin:SetAnchorPoint( 0 , 0.5 )
    feedbackBegin:SetPosition( 240 , 460 )
    self.Image_bg:AddChild( feedbackBegin )

    self.feedbackBegin = Util:Label( "00/00/00" , Macros.TypefaceSize.slightly )
    self.Image_bg:AddChild( self.feedbackBegin )
    PosTool.RightTo( feedbackBegin , self.feedbackBegin )

    local feedbackAbort = Util:Label( CPPGameLib.GetString("feedbackAbort") , Macros.TypefaceSize.slightly )
    feedbackAbort:SetAnchorPoint( 0 , 0.5 )
    feedbackAbort:SetPosition( 240 , 430 )
    self.Image_bg:AddChild( feedbackAbort )

    self.feedbackAbort = Util:Label( "00/00/00" , Macros.TypefaceSize.slightly )
    self.Image_bg:AddChild( self.feedbackAbort )
    PosTool.RightTo( feedbackAbort , self.feedbackAbort )

    self.__tab:SetContentSize( self.Panel_btn:GetContentWidth() , self.Panel_btn:GetContentHeight() + 10 )
    local cnf = CallModuleFunc(ModuleType.FEEDBACK , "GetRechargeReward")
    self.__tab:SetNoOn( true )
    self.__tab:SetData( cnf )
    self.__tab:SetCeil( )  
    self.__tab:SetPosition(self.Panel_btn:GetContentWidth()/2 , self.Panel_btn:GetContentHeight()/2 )
    self.Panel_btn:AddChild(self.__tab:GetNode())
    self.__tab:SetBounceEnabled( false )

    local index = CallModuleFunc(ModuleType.FEEDBACK , "GetSelectedDefault")
    self.__tab:On( index or 1 )
end

function Modules.FeedbackView:Ceil( index , data )
    local no = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.FEEDBACK, "btn_feedback_no")
    local sp = Util:Sprite( no )

    local layer = Util:Layout( sp:GetContentWidth() , self.Panel_btn:GetContentHeight() / 7 )
    layer:SetAnchorPoint( 0.5 , 0.5 ) 

    --条目
    if not self.item then
        self.item = {}
    end
    self.item[index] = Util:Sprite( no )
    self.item[index]:SetAnchorPoint( 0.5 , 0.5 )  
    layer:AddChild( self.item[index] )
    PosTool.Center( self.item[index] )

    --线
    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.FEEDBACK, "btn_feedback_on")
    local feedback_on = Util:Sprite( on )

    local sp_feedback_wire = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.FEEDBACK, "sp_feedback_wire")
    local feedback_wire = Util:Sprite( sp_feedback_wire )
    feedback_wire:SetAnchorPoint( 0.5 , 0.5 )
    layer:AddChild( feedback_wire , -1)
    feedback_wire:SetPosition( layer:GetContentWidth()/2 , layer:GetContentHeight() - feedback_on:GetContentHeight() )

    if index == 1 then
        local feedback_wire = Util:Sprite( sp_feedback_wire )
        feedback_wire:SetAnchorPoint( 0.5 , 0.5 )
        layer:AddChild( feedback_wire , -1 )
        feedback_wire:SetPosition( layer:GetContentWidth()/2 , feedback_on:GetContentHeight() )
        feedback_wire:SetRotation( 180 )
    end

    --第几天
    if not self.itemText then
        self.itemText = {}
    end
    self.itemText[index] = Util:Label( CPPGameLib.GetString("activity_dateValue" , data.day ) , Macros.TypefaceSize.popup , Macros.Color.keypoint )
    self.itemText[index]:SetAnchorPoint( 0.5 , 0.5 )  
    layer:AddChild( self.itemText[index] )
    PosTool.Center( self.itemText[index] )

    local time = CallModuleFunc(ModuleType.ROLE,"GetOpenClothing")
    local time_ = CallModuleFunc(ModuleType.FEEDBACK,"GetExtraIncreaseTime")

    if time < (data.day + time_ ) then
        local lock_icon = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "lock_icon_1")
        
        local lock = Util:Sprite( lock_icon )
        lock:SetAnchorPoint( 0.5 , 0.5 )
        lock:SetPosition( layer:GetContentWidth()/2 + 60 , layer:GetContentHeight()/2 )
        layer:AddChild( lock )
        lock:SetScaleFactor( 0.65 )
    end

    return layer
end

function Modules.FeedbackView:OnCallBack( index , data )
    local no = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.FEEDBACK, "btn_feedback_no")
    for i = 1 , #self.item do
        self.item[i]:SetImage( no )
        self.itemText[i]:SetColor(unpack( Macros.Color.keypoint ))
    end

    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.FEEDBACK, "btn_feedback_on")
    if self.item[index] then 
        self.item[index]:SetImage( on )
        self.itemText[index]:SetColor(unpack( Macros.Color.white ))
    end

    self.onIndex = index

    self:SetShowUi()
end

function Modules.FeedbackView:SetShowUi( )
    local cnf = CallModuleFunc(ModuleType.FEEDBACK , "GetRechargeReward" , self.onIndex )
    --奖励
    for i = 1 , #self.awardLst do
        if cnf and cnf.award[i] and cnf.award[i].id then
            self.awardLst[i]:SetData( {id = cnf.award[i].id , num = cnf.award[i].num } )
            self.awardLst[i]:SetVisible( true )
        else
            self.awardLst[i]:SetVisible( false )
        end
    end

    local info = CallModuleFunc(ModuleType.FEEDBACK , "GetFeedbackTime" )

    self.feedbackBegin:SetString( TimeTool.GetTimeFormat( info.star  , TimeTool.defaul_format[12] ) )
    self.feedbackAbort:SetString( TimeTool.GetTimeFormat( info.over  , TimeTool.defaul_format[12] ) )

    --今日充值
    local text = ""
    if cnf then
        text = CallModuleFunc(ModuleType.SUNDRY, "GetCurrencyText" , cnf.recharge_id )
    end
    self.feedbackTips:SetString( CPPGameLib.GetString("feedbackTips" , text) )

    self:SetBtn()

    self:SetAwards()
end

function Modules.FeedbackView:SetBtn( )
    
    self.Button_recharge:SetVisible(false)
    self.Button_recharge:SetEnabled( true )
 
    self.Image_type:SetVisible( false )
    self.Image_btn_text:SetGray( false )

    GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , false )
    GlobalEffectMgr:SetEffectVisible(self.btnRapid_id , false)

    local info = CallModuleFunc(ModuleType.FEEDBACK , "GetDailyDataList" , self.onIndex )
    --过期
    if info == "Expired" then

        self.Image_type:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_seven_end" , true) )
        self.Image_type:SetVisible( true )

    --还没充值
    elseif info == "NoMoney" then
        self.Image_btn_text:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.VIP, "vip_rapid" , true) )
        self.Image_btn_text:IgnoreContentAdaptWithSize( true )
        GlobalEffectMgr:SetEffectVisible(self.btnRapid_id , true)

        self.Button_recharge = Util:WidgetButton( self.Button_recharge , nil , function ( ... )
            CallModuleFunc(ModuleType.CHARGE, "OpenView", BaseViewType.CHARGE)
        end )
        self.Button_recharge:SetVisible(true)

    --充值了
    elseif info == "Rich" then
        self.Image_btn_text:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "draw_1" , true) )
        self.Image_btn_text:IgnoreContentAdaptWithSize( true )
        GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , true )

        self.Button_recharge = Util:WidgetButton( self.Button_recharge , nil , function ( ... )
            local info = {}
            info.day = self.onIndex
            CallModuleFunc(ModuleType.FEEDBACK , "TakeRechargeRewardReq" , info )
        end )
        self.Button_recharge:SetVisible(true)

    --已领取
    elseif info == "Draw" then

        self.Image_type:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_seven_get" , true) )
        self.Image_btn_text:IgnoreContentAdaptWithSize( true )
        self.Image_type:SetVisible( true )

    --未开启
    elseif info == "NoOpen" then
        
        self.Image_btn_text:SetGray( true )
        self.Image_btn_text:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.VIP, "vip_rapid" , true) )
        self.Image_btn_text:IgnoreContentAdaptWithSize( true )

        self.Button_recharge:SetEnabled( false )
        self.Button_recharge = Util:WidgetButton( self.Button_recharge , nil , function ( ... )
        end )
        self.Button_recharge:SetVisible(true)
    end
end

function Modules.FeedbackView:SetAwards( )
    --已累计充值
    self.feedbackTips2:SetVisible( false )
    local index = CallModuleFunc(ModuleType.FEEDBACK, "GetAccumulativePayDay")
    self.feedbackTips2:SetString( CPPGameLib.GetString("feedbackTips2" , index ) )  

    --领取按钮
    self.Panel_zoom:SetVisible(false)

    --图标呼吸
    CPPActionManager.StopAllActions(self.Button_show)
    self.seven_get:SetVisible(false)

    --大奖信息

    local type_ = CallModuleFunc(ModuleType.FEEDBACK , "GetExtraRewardDrawType")
    
    --可以领取
    if type_ == "May" then
        self.Panel_zoom:SetVisible( true )
        ActionManager:ZoomAnimation(self.Button_show , 0.5 , 1.02 , 0.98 )
    --已经领取
    elseif type_ == "Draw" then
        self.seven_get:SetVisible( true )
    --不能领取
    elseif type_ == "Unable" then
        self.feedbackTips2:SetVisible( true )
    end
end

--领取
function Modules.FeedbackView:OnButtonGet( )
    local index = CallModuleFunc(ModuleType.FEEDBACK, "GetAccumulativePayDay")
    local cnf = CallModuleFunc(ModuleType.FEEDBACK , "GetRechargeExtraReward")
    if cnf.day <= index then
        CallModuleFunc(ModuleType.FEEDBACK , "TakeRechargeExtraRewardReq")
    else
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("feedbackUnableGet"))
    end
end

--显示
function Modules.FeedbackView:OnButtonShow( )
    LogicGUI.TemplFeedbackAwardView.New()
end

--显示红点
function Modules.FeedbackView:SetRed( )
    if not self.redLst then
        self.redLst = {}
    end

    local cnf = CallModuleFunc(ModuleType.FEEDBACK , "GetRechargeReward")
    for i , v in ipairs( cnf ) do
        local text = CallModuleFunc(ModuleType.FEEDBACK , "GetDailyDataList" , v.day )
        if self.redLst[i] then
            self.redLst[i]:RemoveSelf( true )
            self.redLst[i] = nil
        end

        if text == "Rich" then
            local item = self.__tab:GetBtn( i )
            if item then
                self.redLst[i] = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point") )
                self.redLst[i]:SetPosition( item:GetContentWidth() - 15 , item:GetContentHeight() - 10 )
                item:AddChild( self.redLst[i] )
            end
        end
    end    
end




LogicGUI = LogicGUI or {}

LogicGUI.TemplFeedbackAwardView = LogicGUI.TemplFeedbackAwardView or BaseClass(GUI.Template)

function LogicGUI.TemplFeedbackAwardView:__init(i,isCouldGet,isHaveGet)
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__open_callback = function ()
        self:InitUI(i,isCouldGet,isHaveGet)
    end
    self:InitTemplate(GUI.TemplateRegistry.DUNGEON_BOXINFO)
end

function LogicGUI.TemplFeedbackAwardView:InitUI(i,isCouldGet,isHaveGet)
    --奖励列表
    local widget = self:GetWidget("ScrollView_Rewards")
    local item_w = 84
    local item_space = 15
    local offset_x = item_w / 2
      
    local cnf = CallModuleFunc(ModuleType.FEEDBACK , "GetRechargeExtraReward")

    local itemInfo = CPPGameLib.GetConfig( "Item" , cnf.award[1].id )
    CPPGameLib.PrintTable("xy" , itemInfo , "请求物品列表")  

-- yangshuo(杨硕) 02-05 11:07:08
-- 就读这个字段的物品表里面名字，我们配成礼包
    if itemInfo.item_id_list then
        for i, v in ipairs( itemInfo.item_id_list ) do
            local icon_layer = LogicGUI.IconLayer.New()
            icon_layer:SetData({id = v ,num = itemInfo.item_max_num_list[i] })
            icon_layer:SetAnchorPoint(0.5, 1)
            icon_layer:ShowItemName(true)
            icon_layer:ShowEquipName(true)
            icon_layer:ShowHeroName(true)
            widget:AddChild(icon_layer:GetNode())
            PosTool.LeftTop(icon_layer, offset_x)
            offset_x = offset_x + item_w + item_space
        end 
        offset_x = offset_x - item_w / 2 - item_space
        widget:SetInnerContainerSize(offset_x + 4, 0)
    end

    --标题
    Util:WidgetLabel(self:GetWidget("Text_Title"), CPPGameLib.GetString("feedback"), Macros.TypefaceSize.popup, Macros.Color.btn)
    --关闭按钮
    Util:WidgetButton(self:GetWidget("Button_Close"), "", function()
        self:DeleteMe()
    end)                

    --领取按钮
    local btn_receive = Util:WidgetButton(self:GetWidget("Button_Receive") , CPPGameLib.GetString("common_close") , function()
        self:DeleteMe()  
    end,Macros.TypefaceSize.largish , Macros.Color.btn , Macros.Color.button_red)


    --领取条件   
    self:GetWidget("Panel_StarCondition"):SetVisible(false)
    local cnf = CallModuleFunc(ModuleType.FEEDBACK , "GetRechargeExtraReward")
    Util:WidgetLabel(self:GetWidget("Text_GameLevelCondition"), CPPGameLib.GetString("feedbackAwardTips" , cnf.day ) , Macros.TypefaceSize.normal, Macros.Color.content )
end

function LogicGUI.TemplFeedbackAwardView:__delete()
    if self.__button_zoubian_effect then
        self:__DelEffect(self.__button_zoubian_effect)
        self.__button_zoubian_effect = nil
    end
end