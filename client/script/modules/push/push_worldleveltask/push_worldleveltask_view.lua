--[[
%% @module: 世界等级任务
%% @author: yjg
%% @created: 2018年5月24日10:10:12
--]]

Modules = Modules or {}

Modules.PushWorldLevelView = Modules.PushWorldLevelView or BaseClass(BaseView)

function Modules.PushWorldLevelView:__init( )
    self.__layout_name = "worldleveltask"  
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__open_callback = function()
        self:__Getwidget()
        self:__ShowUI( )
        self:__Server() 
    end
end

--数据
function Modules.PushWorldLevelView:__Server()
    local cnf = CallModuleFunc(ModuleType.PUSH_WORLDLEVEL , "WorldLevelTaskDataReq" )
    --七日登录
    self:BindGlobalEvent(Macros.Event.PushWorldLevelModule.WORLDLEVEL_DATA , function (_, data)
        self:__SetShowUI()
    end)
end

function Modules.PushWorldLevelView:__Getwidget( ... )
    -- 背景
    self.Image_bg = self:GetWidget("Image_bg") 
    --信息
    self.Panel_info = self:GetWidget("Panel_info") 
    --列表
    self.Panel_lst = self:GetWidget("Panel_lst") 
    --关闭
    self.Button_close =  Util:WidgetButton( self:GetWidget("Button_close") , nil , CPPGameLib.Handler(self,self.OnButtonClose))
end

function Modules.PushWorldLevelView:__ShowUI(  )

    --背景
    self.Image_bg:SetImage(Resource.PathTool.GetUIResPath("push/worldleveltask/worldleveltask",true), TextureResType.LOCAL)

    --今日世界等级
    self.WorldLevel_today = Util:Label( CPPGameLib.GetString("WorldLevel_today" , 0) , Macros.TypefaceSize.normal , Macros.Color.white )
    self.WorldLevel_today:SetAnchorPoint( 0 , 0.5 )
    self.WorldLevel_today:SetPosition( 5  , self.Panel_info:GetContentHeight() - 20 )
    self.Panel_info:AddChild( self.WorldLevel_today )

    --我的等级
    self.WorldLevel_myLevel = Util:Label( CPPGameLib.GetString("WorldLevel_myLevel" , 0) , Macros.TypefaceSize.normal , Macros.Color.golden )
    self.WorldLevel_myLevel:SetAnchorPoint( 0 , 0.5 )
    self.WorldLevel_myLevel:SetPosition( 5  , 20 )
    self.Panel_info:AddChild( self.WorldLevel_myLevel )

    --进度条
    local progressBar_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_little_preview_bg")
    local progressBar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_little_preview")
    self.daliyMissionProgressBar = GUI.ProgressBar9Layer.New(progressBar,progressBar_bg,187,15,191,19) 
    self.daliyMissionProgressBar:SetPosition( 250 , self.WorldLevel_myLevel:GetPositionY() )
    self.daliyMissionProgressBar:SetAnchorPoint( 0.5 , 0.5 )
    self.daliyMissionProgressBar:SetZOrder(0)
    self.Panel_info:AddChild(self.daliyMissionProgressBar:GetNode())  

    --进度条字样
    self.daliyMissionProgressText = Util:Label( CPPGameLib.GetString("tower_amount" , 0 , 0 ) , Macros.TypefaceSize.minimum )
    self.daliyMissionProgressText:SetAnchorPoint( 0.5 , 0.5 )
    self.daliyMissionProgressText:SetPosition( self.daliyMissionProgressBar:GetPositionX() , self.daliyMissionProgressBar:GetPositionY() )
    self.Panel_info:AddChild(self.daliyMissionProgressText) 

    --奖励按钮
    local orange_box_close = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "orange_box_close")
    self.award = Util:Button( orange_box_close , function( ... )
        -- body
    end )
    self.award:SetPosition( 422 , self.Panel_info:GetContentHeight()/2 + 13 )
    self.award:SetAnchorPoint( 0.5 , 0.5 )
    self.Panel_info:AddChild( self.award , 5 )

    --奖励特效
    self.UI_xingjibaoxiang, effect = self:__CreateEffect({res_name = "UI_xingjibaoxiang"}, self.Panel_info )
    effect:SetPosition(self.award:GetPositionX(),self.award:GetPositionY())
    GlobalEffectMgr:SetEffectVisible(self.UI_xingjibaoxiang , false)

    --已领取
    local seven_get = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_seven_get" , true )
    self.seven_get = Util:Sprite( seven_get )
    self.seven_get:SetAnchorPoint( 0.5 , 0.5 )
    self.seven_get:SetVisible( false )
    self.award:AddChild( self.seven_get )
    PosTool.Center( self.seven_get )

    --任务进度
    self.taskSchedule = Util:Name( "" , Macros.TypefaceSize.slightly , Macros.Game.QualityType.GREEN )
    self.taskSchedule:SetAnchorPoint( 0.5 , 0.5 )
    self.taskSchedule:SetPosition( self.award:GetPositionX() + 10 , self.award:GetPositionY() - 18 )
    self.Panel_info:AddChild( self.taskSchedule , 9 )

    --完成全部任务领取
    self.WorldLevel_awardTips = Util:Name( CPPGameLib.GetString("WorldLevel_awardTips") , Macros.TypefaceSize.slightly , 100 )
    self.WorldLevel_awardTips:SetAnchorPoint( 0.5 , 0.5 )
    self.WorldLevel_awardTips:SetPosition( self.award:GetPositionX() + 10 , self.award:GetPositionY() - 40 )
    self.Panel_info:AddChild( self.WorldLevel_awardTips , 5 )

    local params =  
    {
        item_class = LogicGUI.TemplPushWorldLevelItem,
        item_width = 519,
        item_height = 123,
        row = 3,
        col = 1,
        item_space_r = 5,
        view_height = self.Panel_lst:GetContentHeight() - 5 ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.Panel_lst:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode())
end

function Modules.PushWorldLevelView:__SetShowUI()
    --列表设置
    local cnf = CallModuleFunc(ModuleType.PUSH_WORLDLEVEL , "GetWorldLevelTaskEventCnf" )
    self.lst_list:SetDataList( cnf )

    --今日世界等级
    local level = CallModuleFunc(ModuleType.PUSH_WORLDLEVEL , "GetWorldLevel" )
    self.WorldLevel_today:SetString( CPPGameLib.GetString("WorldLevel_today" , level ) )

    --我的等级
    local level = CallModuleFunc(ModuleType.PUSH_WORLDLEVEL , "GetWorldLevelMyLevel" )
    self.WorldLevel_myLevel:SetString( CPPGameLib.GetString("WorldLevel_myLevel" , level ) )

    --任务进度
    CPPActionManager.StopAllActions( self.award )
    GlobalEffectMgr:SetEffectVisible(self.UI_xingjibaoxiang , false)
    local num , max = CallModuleFunc(ModuleType.PUSH_WORLDLEVEL , "GetTaskSchedule" )
    if num < max then
        self.taskSchedule:SetString( CPPGameLib.GetString("tower_amount" , num , max ) )
    else
        self.taskSchedule:SetString( "" )
        --大奖领取状态
        local todayAwardType = CallModuleFunc(ModuleType.PUSH_WORLDLEVEL , "GetTodayAwardType" )
        --已经领取
        if todayAwardType then
            self.award:SetTouchEnabled(false)
            self.seven_get:SetVisible( true )
        else
            --呼吸
            ActionManager:ZoomAnimation(self.award , 1 , 1.05 , 0.95 )
            GlobalEffectMgr:SetEffectVisible(self.UI_xingjibaoxiang , true)
        end
    end

    --经验
    local role_info = CallModuleFunc(ModuleType.ROLE, "GetRoleInfo")
    self.daliyMissionProgressBar:SetProgress(role_info.exp / role_info.max_exp * 100)
    self.daliyMissionProgressText:SetString( CPPGameLib.GetString("tower_amount" , role_info.exp , role_info.max_exp ) )

    --大奖按钮设置
    Util:WidgetButton( self.award , nil  , CPPGameLib.Handler(self,self.OnAward) )
end

--大奖
function Modules.PushWorldLevelView:OnAward()
    local num , max = CallModuleFunc(ModuleType.PUSH_WORLDLEVEL , "GetTaskSchedule" )
    if num < max then
        LogicGUI.TemplPushWorldLevelTips.New()
    else
        print("xy" , "可领取")
        CallModuleFunc(ModuleType.PUSH_WORLDLEVEL , "TakeWorldLevelTaskFinalRewardReq" )
    end
end

--关闭
function Modules.PushWorldLevelView:OnButtonClose()
    if unpack(self.__open_data) == "push" then 
        self:CloseManual(true)
        CallModuleFunc(ModuleType.PUSH,"Transfer",Macros.Game.SystemID.PUSH_WORLDLEVEL)
    else
        self:CloseManual()
    end
end

--
-- @brief 任务表
-- @author: yjg
-- @date: 2018年5月24日14:48:01
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplPushWorldLevelItem = LogicGUI.TemplPushWorldLevelItem or BaseClass(GUI.ScrollItem)

function LogicGUI.TemplPushWorldLevelItem:__init(data)
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function LogicGUI.TemplPushWorldLevelItem:__CreateWidgetFromLuaCode()
    self.__layout = Util:Layout(519,123)
    return self.__layout
end

function LogicGUI.TemplPushWorldLevelItem:SetData(info)
    GUI.ScrollItem.SetData(self, info)
    self:__ShowUI( info )
end

function LogicGUI.TemplPushWorldLevelItem:__Getwidget()
end

function LogicGUI.TemplPushWorldLevelItem:__ShowUI( info )
    --条目背景
    local bg_item_w = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_item_w")
    self.bg_item = Util:Sprite9( bg_item_w )
    self.bg_item:SetContentSize( self.__layout:GetContentWidth() - 10 , self.__layout:GetContentHeight() )
    self.__layout:AddChild( self.bg_item )
    PosTool.Center( self.bg_item )

    --目标
    local WorldLevel_task = Util:Label( CPPGameLib.GetString("WorldLevel_task") , Macros.TypefaceSize.normal , Macros.Color.content )
    WorldLevel_task:SetAnchorPoint( 0 , 0.5 )
    WorldLevel_task:SetPosition( 20 , self.bg_item:GetContentHeight() - 25 )
    self.bg_item:AddChild( WorldLevel_task )

    --描述
    local dec = string.format( Macros.Color.green_rt , info.content )
    if info.content < info.parameterX then
        dec = string.format( Macros.Color.red_rt , info.content )
    else
        dec = string.format( Macros.Color.green_rt , info.parameterX )
    end

    local describe = Util:RichText( string.format( info.describe , dec ) , Macros.TypefaceSize.normal )
    self.bg_item:AddChild( describe )
    PosTool.RightTo( WorldLevel_task , describe , 0 , 3 )

    --奖励
    local WorldLevel_award = Util:Label( CPPGameLib.GetString("WorldLevel_award") , Macros.TypefaceSize.normal , Macros.Color.content )
    WorldLevel_award:SetAnchorPoint( 0 , 0.5 )
    WorldLevel_award:SetPosition( 20 , self.bg_item:GetContentHeight()/2 - 10 )
    self.bg_item:AddChild( WorldLevel_award )

    --奖励
    for i , v in ipairs( info.lst ) do
        local icon = LogicGUI.IconLayer.New()
        self.bg_item:AddChild(icon:GetNode())
        local iconData = { id = v.id , num = v.num }
        icon:SetData(iconData)
        icon:SetPosition( 
                            WorldLevel_award:GetPositionX() + WorldLevel_award:GetContentWidth() + (icon:GetNode():GetContentWidth()/2) - 8 + ( (i-1) * 70 ) , 
                            WorldLevel_award:GetPositionY() 
                        )
        icon:SetScaleFactor(0.8)
    end

    --按钮
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_green")
    self.Button_Receive = Util:Button( sp , 
                                 CPPGameLib.Handler(self, self.OnBtn) , 
                                 " " , 
                                 Macros.TypefaceSize.button , 
                                 nil , Macros.Color.button_yellow 
                                 )
    self.Button_Receive:SetPosition( self.bg_item:GetContentWidth()/2 + 160 , self.bg_item:GetContentHeight()/2 )
    self.bg_item:AddChild(self.Button_Receive) 

    --红点
    self.btnEffect_id , BtnEffect_con = self:__CreateEffect({res_name = "ui_anniuzoubian"} , self.Button_Receive , 999)
    BtnEffect_con:SetAnchorPoint(0.5,0.5)
    PosTool.Center( BtnEffect_con )
    GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , false )

    self:Transfer( info )
end

--中转
function LogicGUI.TemplPushWorldLevelItem:Transfer( info )
    GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , false )
    --还没完成
    if info.getType == "weiwancheng" then
        self:NotAccomplish( info )
    --已经领取
    elseif info.getType == "yilingqu" then
        self:AlreadyGet( info )
    --可以领取
    elseif info.getType == "kelingqu" then
        GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , true )
        self:MayGet( info )
    end
end

--未完成
function LogicGUI.TemplPushWorldLevelItem:NotAccomplish( info )
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_achieving",true)
    self.Button_Receive:SetNormalImage( sp )
    Util:WidgetButton( self.Button_Receive , "" ,  function( ... )
        local system_info = CPPGameLib.GetConfig("SystemEntry", info.approach )
        local openType = GlobalModulesMgr:IsOpen( info.approach , true )
        if openType then
            CallModuleFunc( system_info.module_type , "OpenView", system_info.view_type or system_info.module_type )
        end
    end )
end

--已领取
function LogicGUI.TemplPushWorldLevelItem:AlreadyGet( info )
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_green")
    self.Button_Receive:SetNormalImage( sp )
    self.Button_Receive:SetText( CPPGameLib.GetString("dungeon_have_receive") )
    Util:GreyButton( self.Button_Receive )
end

--可领取
function LogicGUI.TemplPushWorldLevelItem:MayGet( info )
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_green")
    self.Button_Receive:SetNormalImage( sp )
    Util:WidgetButton( self.Button_Receive , CPPGameLib.GetString("dungeon_receive") , function( ... )
        print("xy"  ,"===========可领取")
        local data = {}
        data.task_id = info.id
        CallModuleFunc(ModuleType.PUSH_WORLDLEVEL , "TakeWorldLevelTaksRewardReq" , data )
    end ,  Macros.TypefaceSize.button , nil , Macros.Color.button_yellow  )
end

--
-- @brief 今日奖励
-- @author: yjg
-- @date: 2018年5月24日19:55:33
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplPushWorldLevelTips = LogicGUI.TemplPushWorldLevelTips or BaseClass(GUI.Template)

function LogicGUI.TemplPushWorldLevelTips:__init( )
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    -- self.__bg_params = {type = BaseView.BGType.SOLID}   
    self:InitTemplate(nil)
    self:__Getwidget()
    self:__ShowUI( )
end

function LogicGUI.TemplPushWorldLevelTips:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout( Macros.Global.DesignSize.width , Macros.Global.DesignSize.height , function ( ... )
        self:DeleteMe()
    end)
    self.layout:SetClippingEnable(false)
    self.layout:SetAnchorPoint(0.5,0.5)
    return self.layout
end

function LogicGUI.TemplPushWorldLevelTips:__delete()
    --背景特效
    if self.effect_id then
        self:__DelEffect(self.effect_id)
        self.effect_id = nil
    end  
end

function LogicGUI.TemplPushWorldLevelTips:__Getwidget()
end

function LogicGUI.TemplPushWorldLevelTips:__ShowUI(  )
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_pack_tips_2")
    local bg_sp9 = Util:Sprite9( sp )
    bg_sp9:SetContentSize( 380 , 250 )
    self.layout:AddChild( bg_sp9 )
    PosTool.Center( bg_sp9 )

    self.pack_tips = Util:Layout( bg_sp9:GetContentWidth() , bg_sp9:GetContentHeight() , function ( ... )
    end)
    self.pack_tips:SetAnchorPoint(0.5,0.5)
    bg_sp9:AddChild( self.pack_tips )
    PosTool.Center( self.pack_tips )
    
    --名字
    local name = Util:Name( CPPGameLib.GetString("WorldLevel_todayaward") , nil , Macros.Game.QualityType.GREEN)
    name:SetPosition(self.pack_tips:GetContentWidth()/2 ,self.pack_tips:GetContentHeight() - 30 )
    name:SetAnchorPoint(0.5,0.5)
    self.pack_tips:AddChild(name)

    --线
    local shortlineSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "shortline")

    --左边
    local shortline_1 = Util:Sprite9( shortlineSp , {50,10} )
    shortline_1:SetContentSize(shortline_1:GetContentWidth() - 35 , shortline_1:GetContentHeight())
    shortline_1:SetAnchorPoint( 1 , 0.5 )
    shortline_1:SetPosition( name:GetPositionX() - (name:GetContentWidth()/2) - 10 , name:GetPositionY() )
    self.pack_tips:AddChild(shortline_1)

    --左边
    local shortline_2 = Util:Sprite9( shortlineSp , {50,10})
    shortline_2:SetContentSize(shortline_2:GetContentWidth() - 35 , shortline_2:GetContentHeight())
    shortline_2:SetAnchorPoint( 1 , 0.5 )
    shortline_2:SetFlipX(true)
    shortline_2:SetPosition( name:GetPositionX() + (name:GetContentWidth()/2) + 10 , name:GetPositionY() )
    self.pack_tips:AddChild(shortline_2)

    --内容框
    local black1_bg_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "black1_bg")
    self.select = Util:Sprite9(black1_bg_sp)
    self.select:SetAnchorPoint( 0.5 , 0 )
    self.select:SetPosition( self.pack_tips:GetContentWidth()/2  , 15 )
    self.select:SetContentSize( self.pack_tips:GetContentWidth() - 30 , self.pack_tips:GetContentHeight() - 90 )
    self.pack_tips:AddChild( self.select )

    --奖励提示
    local WorldLevel_todayawardTips = Util:Label( CPPGameLib.GetString("WorldLevel_todayawardTips") )
    WorldLevel_todayawardTips:SetAnchorPoint( 0 , 0 )
    WorldLevel_todayawardTips:SetPosition( 0 , self.select:GetContentHeight() + 5 )
    self.select:AddChild( WorldLevel_todayawardTips )

    --今日大奖列表
    local layer = Util:Layout( 0 , 0 )
    layer:SetAnchorPoint(0.5,0.5)
    self.select:AddChild( layer )
    PosTool.Center( layer )

    local lst = CallModuleFunc(ModuleType.PUSH_WORLDLEVEL , "GetTodayAwardLst" )
    layer:SetContentSize( 100 * #lst , 1 )
    local interval = layer:GetContentWidth() / #lst
    for i , v in ipairs( lst ) do
        local icon = LogicGUI.IconLayer.New()
        layer:AddChild(icon:GetNode())
        local iconData = { id = v.id , num = v.num }
        icon:SetData(iconData)
        icon:SetPosition( 50 + interval * (i - 1) , layer:GetContentHeight()/2 + 20 )

        icon:ShowItemName(true)
        icon:ShowAwakenToolName(true)
        icon:ShowEquipName(true)
        icon:ShowTreasureName(true)
        icon:ShowHeroName(true)
        icon:ShowGemName(true)
        icon:ShowDivinationName(true)
    end
end