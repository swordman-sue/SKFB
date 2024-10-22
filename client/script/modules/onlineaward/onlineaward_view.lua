--[[
%% @module: 在线奖励
%% @author: yjg
%% @created: 2018年1月20日10:59:51
--]]

Modules = Modules or {} 
Modules.OnlineawardView = Modules.OnlineawardView or BaseClass(BaseView)

function Modules.OnlineawardView:__init()
    self.__layout_name = "temp_online_award"    
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__tab_params_ex = {0}

    self.__open_callback = function()
      self:__OpenCallback()
    end

    local openClothing = CallModuleFunc(ModuleType.ROLE,"GetRoleCreateTime")

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.ONLINE_AWARD)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.ONLINE_AWARD,true)
end

function Modules.OnlineawardView:__OpenCallback( )
    self:__Getwidget()
    self:__ShowUI()
    self:__Server()
end 

function Modules.OnlineawardView:__Dispose()

end

function Modules.OnlineawardView:__Getwidget( )

    WidgetUnity.SetWidgetListener( self.__root_widget , nil, nil, function ( ... )
        self:CloseManual()
    end)

    --背景
    self.Image_bg = self:GetWidget("Image_bg")
    --时间奖励
    self.Panel_item = self:GetWidget("Panel_item")
    --最佳奖励
    self.Image_good = self:GetWidget("Image_good")
    --奖励容器
    self.Panel_icon = self:GetWidget("Panel_icon")
    --选择框
    self.Panel_select = self:GetWidget("Panel_select")
    --领取奖励
    self.Button_award = Util:WidgetButton(
                                            self:GetWidget("Button_award"),
                                            CPPGameLib.GetString("tower_drawAward") , 
                                            CPPGameLib.Handler(self,self.OnButtonAward),
                                            Macros.TypefaceSize.popup ,nil,Macros.Color.button_yellow
                                           )
    self.Button_award:SetVisible(false)

    local btnEffect_con = nil
    self.btnEffect_id , btnEffect_con = self:__CreateEffect({res_name = "ui_anniuzoubian"} , self.Button_award )
    btnEffect_con:SetPosition(self.Button_award:GetContentWidth()/2 , self.Button_award:GetContentHeight()/2 )
    GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , false )

    --图标背景
    self.Image_iconBg = {}
    --图标框
    self.Image_frame = {}
    --状态
    self.Image_character = {}

    self.Image_icon = {}

    for i = 1 , 4 do

        self.Image_iconBg[i] = self:GetWidget("Image_iconBg"..i)

        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ONLINE_AWARD, "sp_online_award_shade2")
        local iconBg = Util:Sprite( sp )
        iconBg:SetScaleFactor(0.9)

        self.Image_icon[i] = ClippingNodeEx.Node()
        self.Image_icon[i]:SetAlphaTestValue(0)
        self.Image_icon[i]:SetStencilMask( iconBg )
        self.Image_icon[i]:SetAnchorPoint( 0.5 , 0.5 )
        self.Image_icon[i]:SetPosition( self.Image_iconBg[i]:GetContentWidth()/2 , self.Image_iconBg[i]:GetContentHeight()/2 )
        self.Image_iconBg[i]:AddChild( self.Image_icon[i] )


        self.Image_frame[i] = self:GetWidget("Image_frame"..i)
        self.Image_frame[i]:SetVisible( false )

        -- self.Image_character[i] = self:GetWidget("Image_character"..i)
        -- self.Image_character[i]:SetZOrder( 2 )
    end

    --关闭
    self.Button_close =  Util:WidgetButton( self:GetWidget("Button_close") , nil , CPPGameLib.Handler(self,self.OnButtonClose))
end 

function Modules.OnlineawardView:__ShowUI( )

    if not self.preview then
        --图标
        self.preview = {}
        --文字
        self.textType = {}
        --遮罩
        self.shade = {}
        --√
        self.signin = {}
    end

    for i = 1 , #self.Image_iconBg do
        --图标
        self.preview[i] = LogicGUI.IconLayer.New( false )
        self.preview[i]:SetData( {id = 13999} )
        self.preview[i]:SetAnchorPoint( 0.5 , 0.5 )
        self.Image_icon[i]:AddChild( self.preview[i]:GetNode() )
        PosTool.Center( self.preview[i] )
        self.preview[i]:SetScaleFactor( 0.9 )

        self.preview[i]:ShowIconBG( false )
        self.preview[i]:ShowIconQuality( false )
        self.preview[i]:ShowIconShade( false )
        self.preview[i]:ShowAptitude( false )

        --文字状态
        self.textType[i] = Util:Name( "啊啊啊啊" , Macros.TypefaceSize.slightly )
        self.textType[i]:SetAnchorPoint( 0.5 , 0.5 )
        self.Panel_item:AddChild( self.textType[i] , 99 )
        self.textType[i]:SetPosition( self.Image_iconBg[i]:GetPositionX() , self.Image_iconBg[i]:GetPositionY() - 45 )

        self.shade[i] = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ONLINE_AWARD, "sp_online_award_shade") )
        self.shade[i]:SetAnchorPoint(0.5,0.5)
        self.Image_iconBg[i]:AddChild( self.shade[i] , 5 )
        PosTool.Center( self.shade[i] )
        self.shade[i]:SetVisible(false)

        self.signin[i] = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_activity_signin") )
        self.signin[i]:SetAnchorPoint(0.5,0.5)
        self.Image_iconBg[i]:AddChild( self.signin[i] , 6 )
        PosTool.Center( self.signin[i] )
        self.signin[i]:SetVisible(false)
    end

    --选中框
    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "circle_selected_frame")
    self.selected = Util:Sprite( on )
    self.selected:SetAnchorPoint( 0.5 , 0.5 )
    self.Panel_item:AddChild( self.selected , 1 )
    self.selected:SetPosition( self.Image_iconBg[1]:GetPositionX() , self.Image_iconBg[1]:GetPositionY() )

    --奖励
    if not self.award then
        self.award = {}
    end

    local interval = self.Panel_icon:GetContentWidth() / 3
    for i = 1 , 3 do
        self.award[i] = LogicGUI.IconLayer.New()
        self.award[i]:SetData( {id = 13999} )
        self.award[i]:SetAnchorPoint( 0.5 , 0.5 )
        self.award[i]:SetPosition( (interval/2) + interval * (i - 1)  , self.Panel_icon:GetContentHeight()/2 )
        self.Panel_icon:AddChild( self.award[i]:GetNode() )
    end

    --剩余
    self.onlineawardTime = Util:Label( CPPGameLib.GetString("onlineawardTime" , "00:00:00" , "00" , "00") , nil , Macros.Color.headline )
    self.onlineawardTime:SetAnchorPoint( 0.5 , 0.5 )
    self.onlineawardTime:SetPosition( self.Panel_icon:GetContentWidth()/2  , -20 )
    self.Panel_icon:AddChild( self.onlineawardTime )
    self.onlineawardTime:SetVisible( false )

    -- self.
    self.title = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ONLINE_AWARD, "sp_online_award_time16" , true ) )
    self.title:SetAnchorPoint( 0 , 0.5 )
    self.title:SetPosition( 110 , 400 )
    self.Image_bg:AddChild( self.title )

    local cnf = CallModuleFunc(ModuleType.ONLINE_AWARD , "GetOnlineawardCnf")
    self.__tab:SetContentSize( self.Panel_select:GetContentWidth() , self.Panel_select:GetContentHeight() )
    self.__tab:SetData( cnf )
    self.__tab:SetCeil( )  

    self.__tab:SetPosition(self.Panel_select:GetContentWidth()/2 + 14 , self.Panel_select:GetContentHeight()/2 )
    self.Panel_select:AddChild(self.__tab:GetNode())
    self.__tab:SetBounceEnabled(false)
end

function Modules.OnlineawardView:__Server()
    CallModuleFunc( ModuleType.ONLINE_AWARD , "OnlineRewardDataReq" )
    self:BindGlobalEvent(Macros.Event.OnlineawardModule.ONLINE_REWARD_DATA , function (_, data)
        local cnf = CallModuleFunc(ModuleType.ONLINE_AWARD , "GetOnlineawardCnf")
        local openClothing = CallModuleFunc(ModuleType.ROLE,"GetRoleCreateTime")
        self.__tab:On( openClothing , cnf[openClothing] )
    end)

    self:BindGlobalEvent(Macros.Event.OnlineawardModule.ONLINE_COUNTDOWN , function (_, time)
        self:SetCountdown( time )
    end)

    local cnf = CallModuleFunc(ModuleType.ONLINE_AWARD , "GetOnlineawardCnf")
    local openClothing = CallModuleFunc(ModuleType.ROLE,"GetRoleCreateTime")
    self.__tab:On( openClothing , cnf[openClothing] )
end

function Modules.OnlineawardView:Ceil( index , data )

    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btm_pitchOn")
    local sizeSp = Util:Sprite( on )

    local layer = Util:Layout( sizeSp:GetContentWidth() , sizeSp:GetContentHeight() )

    local no = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btm_pitchNo")
    if not self.item then
        self.item = {}
    end
    self.item[index] = Util:Sprite( no )
    self.item[index]:SetAnchorPoint( 0.5 , 0.5 )  
    layer:AddChild( self.item[index] )
    PosTool.Center( self.item[index] )

    if not self.onlineawardDay then
        self.onlineawardDay = {}
    end
    self.onlineawardDay[index] = Util:Label( CPPGameLib.GetString("onlineawardDay" , " " .. data.day .. " " ) , Macros.TypefaceSize.popup , Macros.Color.keypoint )
    self.onlineawardDay[index]:SetAnchorPoint( 0.5 , 0.5 )
    layer:AddChild( self.onlineawardDay[index] )
    PosTool.Center( self.onlineawardDay[index] )

    local openClothing = CallModuleFunc(ModuleType.ROLE,"GetRoleCreateTime")
    if openClothing > index then
        self.item[index]:SetGray( true )
        self.onlineawardDay[index]:SetColor( unpack( Macros.Color.Gray ) )
        self.onlineawardDay[index]:SetStrokeColor( unpack(Macros.Color.forbidden_stroke) )
    end

    return layer
end

function Modules.OnlineawardView:OnCallBack( index , data )

    print("xy" , "=======================" , index )
    local openClothing = CallModuleFunc(ModuleType.ROLE,"GetRoleCreateTime")
    if openClothing > index then
        return 
    end
    if not self.onlineawardDay[index] then
        return
    end

    local no = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btm_pitchNo")
    for i = 1 , #self.item do
        self.item[i]:SetImage( no )

        local openClothing = CallModuleFunc(ModuleType.ROLE,"GetRoleCreateTime")
        if openClothing > i then
            self.onlineawardDay[i]:SetColor( unpack( Macros.Color.white ) )
        else
            self.onlineawardDay[i]:SetColor( unpack( Macros.Color.keypoint ) )
        end
    end

    self.onlineawardDay[index]:SetColor( unpack( Macros.Color.golden ) )

    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btm_pitchOn")
    if self.item[index] then 
        self.item[index]:SetImage( on )

        --目前选中的条目
        self.onCallBackIndex = index

        local time = CallModuleFunc(ModuleType.ROLE,"GetRoleCreateTime")
        if index == time then
            
            local rewardRecordList = CallModuleFunc( ModuleType.ONLINE_AWARD , "GetRewardRecordList" , data.day )
            if rewardRecordList then
                for i , v in ipairs( rewardRecordList ) do
                    if v.GetType == "yet" then
                        self.onSelectedNodeIndex = i + 1
                    end
                end
            end

        else
            --重置选中
            self.onSelectedNodeIndex = 1
        end

        self:SetSelectedNode( data )
    end
end

--设置选中节点
function Modules.OnlineawardView:SetSelectedNode( data )


    local rewardRecordList = CallModuleFunc( ModuleType.ONLINE_AWARD , "GetRewardRecordList" , data.day )
    

    for i , v in ipairs( data.info ) do
        self.shade[i]:SetVisible(false)
        self.signin[i]:SetVisible(false)

        --图标
        self.preview[i]:SetData( {id = v.reward[1].item } )
        self.preview[i]:SetTouchCallback( function( ... )
            self.onSelectedNodeIndex = i
            self:SetSelectedPage( v , self.onSelectedNodeIndex )
            self.selected:SetPosition( self.Image_iconBg[i]:GetPositionX() , self.Image_iconBg[i]:GetPositionY() )
        end )

        local text = ""
        local color = Macros.Color.white
        if rewardRecordList then
            --已领
            if rewardRecordList[i].GetType == "yet" then
                text = CPPGameLib.GetString("onlineawardYet")
                color = Macros.Color.green
                self.shade[i]:SetVisible(true)
                self.signin[i]:SetVisible(true)
            --可以
            elseif rewardRecordList[i].GetType == "may" then
                -- text = CPPGameLib.GetString("onlineawardMay")
                text = CPPGameLib.GetString("onlineawardNodeTime" , v.time )
            --不行
            elseif rewardRecordList[i].GetType == "nein" then
                -- text = CPPGameLib.GetString("onlineawardNein")
                text = CPPGameLib.GetString("onlineawardNodeTime" , v.time )
            --过期
            elseif rewardRecordList[i].GetType == "overdue" then
                self.shade[i]:SetVisible(true)
                text = CPPGameLib.GetString("onlineawardoverdue")
                color = Macros.Color.red
            end
        end

        --时间
        self.textType[i]:SetString( text )
        self.textType[i]:SetColor(unpack(color))
    end

    --默认显示第一个


    -- CPPGameLib.PrintTable("xy" , data.info , "请求物品列表")  
    -- print( "xy" , " self.onSelectedNodeIndex" , self.onSelectedNodeIndex )

    local index = data.info[ self.onSelectedNodeIndex ]
    if not index then
        index = 1
    else
        index = self.onSelectedNodeIndex
    end

    self:SetSelectedPage( data.info[ index ] or index , index )
    self.selected:SetPosition( self.Image_iconBg[ index ]:GetPositionX() , self.Image_iconBg[ index ]:GetPositionY() )

end

--设置子页面
function Modules.OnlineawardView:SetSelectedPage( data , index )
    -- CPPGameLib.PrintTable("xy" , data , "请求物品列表")  

    if not data then
        return 
    end

    --奖励
    for i , v in ipairs( data.reward ) do
        self.award[i]:SetData( {id = v.item , num = v.num } )
    end

    --print("xy" , "==============" , index )
    --按钮
    self:SetBtn( index )

    --倒计时
    self:SetCountdownShow( )

    if self.LastTimeOnIndex ~= self.onCallBackIndex then
        self.LastTimeOnIndex = self.onCallBackIndex
    else
        return 
    end

    --模型
    local _ , data_ = self.__tab:GetBtn( self.LastTimeOnIndex )
    self:SetModel( data_ )
end

--设置模型
function Modules.OnlineawardView:SetModel( data )

    --图片
    if self.spModel then
        self.spModel:RemoveSelf( true )
        self.spModel = nil
    end

    --模型
    if self.heroModel then
        self.heroModel:DeleteMe()
        self.heroModel = nil
    end

    --名字
    if self.name then
        self.name:RemoveSelf( true )
        self.name = nil
    end

    --名字
    if self.tips then
        self.tips:RemoveSelf( true )
        self.tips = nil
    end
    
    if data.title then
        local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ONLINE_AWARD, data.title ,true )
        self.title:SetImage( on )
        self.title:SetVisible( true )
    else
        self.title:SetVisible( false )
    end

    local onlineawardCnf = CallModuleFunc( ModuleType.ONLINE_AWARD , "GetOnlineawardCnf" , self.onCallBackIndex )
    local award = onlineawardCnf.optimum

    local type_ , info_ = ObjIDTool.GetInfo( award )

    --名字
    self.name = Util:Name( info_.name , Macros.TypefaceSize.normal , info_.init_quality or info_.quality )
    self.name:SetAnchorPoint( 0.5 , 0.5 )
    self.name:SetPosition( 250 , 120 )
    self.Image_bg:AddChild( self.name , 10 )

    CPPGameLib.PrintTable("xy" , onlineawardCnf , "onlineawardCnf")  
    --描述
    self.tips = Util:Name( onlineawardCnf.additional , Macros.TypefaceSize.slightly )
    self.tips:SetAnchorPoint( 0.5 , 0.5 )
    self.tips:SetPosition( 250 , 95 )
    self.Image_bg:AddChild( self.tips , 10 )

    --道具
    if type_ == Macros.Game.ObjType.ITEM then
        self.spModel = Util:Sprite( Resource.PathTool.GetBigIconPath( info_.res_id ))
        self.spModel:SetAnchorPoint( 0.5 , 0.5 )
        self.spModel:SetPosition( 250 , 230 )
        self.Image_bg:AddChild( self.spModel )
        self.spModel:SetScaleFactor( onlineawardCnf.zoom )

        ActionManager.GetInstance():UpDownMoveAnimation(self.spModel , 1 , 15)

    elseif type_ == Macros.Game.ObjType.EQUIP then
        self.spModel = Util:Sprite( Resource.PathTool.GetBigIconPath( info_.res_id ))
        self.spModel:SetAnchorPoint( 0.5 , 0.5 )
        self.spModel:SetPosition( 250 , 230 )
        self.Image_bg:AddChild( self.spModel )
        self.spModel:SetScaleFactor( onlineawardCnf.zoom )

        ActionManager.GetInstance():UpDownMoveAnimation(self.spModel , 1 , 15)

    elseif type_ == Macros.Game.ObjType.HERO then

        self.heroModel = LogicGUI.Model.New({info_id = info_.model_id, parent = self.Image_bg })
        self.heroModel:SetPosition( 250 , 150  )
        self.heroModel:SetEndCallback(function ()
            self.heroModel:PlayAnimation(RenderObj.ActionIndex.IDLE)
        end)
        self.heroModel:PlayAnimation( data.action )
        self.heroModel:SetScaleFactor( onlineawardCnf.zoom )

    elseif type_ == Macros.Game.ObjType.TREASURE then
        self.spModel = Util:Sprite( Resource.PathTool.GetBigIconPath( info_.res_id ))
        self.spModel:SetAnchorPoint( 0.5 , 0.5 )
        self.spModel:SetPosition( 250 , 230 )
        self.Image_bg:AddChild( self.spModel )
        self.spModel:SetScaleFactor( onlineawardCnf.zoom )

        ActionManager.GetInstance():UpDownMoveAnimation(self.spModel , 1 , 15)
    end
end

--设置按钮
function Modules.OnlineawardView:SetBtn( index )
    GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , false )
    self.Button_award:SetVisible( true )

    local rewardRecordList = CallModuleFunc( ModuleType.ONLINE_AWARD , "GetRewardRecordList" , self.onCallBackIndex )
    local text = ""
    local color = Macros.Color.white

    if rewardRecordList and rewardRecordList[index] then
        --已领
        if rewardRecordList[index].GetType == "yet" then
            Util:GreyButton( self.Button_award )
            self.Button_award:SetText( CPPGameLib.GetString("onlineawardYet") )

        --可以
        elseif rewardRecordList[index].GetType == "may" then

            self.Button_award:SetEnabled( true )
            GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , true )
            Util:WidgetButton(
                                self.Button_award,
                                CPPGameLib.GetString("tower_drawAward") , 
                                CPPGameLib.Handler(self,self.OnButtonAward),
                                Macros.TypefaceSize.popup,nil,Macros.Color.button_yellow
                               )
        --不行
        elseif rewardRecordList[index].GetType == "nein" then
            Util:GreyButton( self.Button_award )
            self.Button_award:SetText( CPPGameLib.GetString("tower_drawAward") )

        --过期
        elseif rewardRecordList[index].GetType == "overdue" then
            Util:GreyButton( self.Button_award )
            self.Button_award:SetText( CPPGameLib.GetString("onlineawardoverdue") )
        end
    end

    local openClothing = CallModuleFunc(ModuleType.ROLE,"GetRoleCreateTime")
    if self.onCallBackIndex ~= openClothing then
        Util:GreyButton( self.Button_award )
        self.Button_award:SetText( CPPGameLib.GetString("tower_drawAward") )
        GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , false )
    end

end

--设置倒计时显示
function Modules.OnlineawardView:SetCountdownShow( )
    self.onlineawardTime:SetVisible(false)

    local rewardRecordList = CallModuleFunc( ModuleType.ONLINE_AWARD , "GetRewardRecordList" , self.onCallBackIndex )
    
    if not rewardRecordList then
        return
    end

    --如果没有可领取的便找到最后一个领取的时间
    local yetIndex = 0
    for i , v in ipairs( rewardRecordList ) do
        if v.GetType == "yet" then
            if yetIndex < i then
                yetIndex = i
            end
        end
    end
    
    local nextIndex = yetIndex + 1

    if not rewardRecordList[ nextIndex ] then
        return 
    end

    --如果他的下一个是未领取的话
    if rewardRecordList[ nextIndex ].GetType == "nein" and nextIndex == self.onSelectedNodeIndex then
        self.onlineawardTime:SetVisible(true)
    end

    local openClothing = CallModuleFunc(ModuleType.ROLE,"GetRoleCreateTime")
    if self.onCallBackIndex ~= openClothing then
        self.onlineawardTime:SetVisible(false)
    end
end

--设置倒计时
function Modules.OnlineawardView:SetCountdown( time )
    local onlineawardCnf = CallModuleFunc( ModuleType.ONLINE_AWARD , "GetOnlineawardCnf" , self.onCallBackIndex )
    self.onlineawardTime:SetString( CPPGameLib.GetString( "onlineawardTime" , TimeTool.TransTimeStamp("HH:MM:SS", time ) ) )

    if time == 0 then
        self:SetBtn( self.onSelectedNodeIndex )
        self:SetCountdownShow()
    end
end

--领取奖励
function Modules.OnlineawardView:OnButtonAward( )
    --print("xy" , "=================================OnButtonAward")
    CallModuleFunc( ModuleType.ONLINE_AWARD , "TakeOnlineRewardReq" )
end

function Modules.OnlineawardView:OnButtonClose( )
    self:CloseManual()
end
