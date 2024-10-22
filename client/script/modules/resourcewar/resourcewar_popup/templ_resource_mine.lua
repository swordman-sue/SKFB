--
-- @brief 资源战矿场
-- @author: yjg
-- @date: 2018年8月9日16:26:02
--

Modules = Modules or {}
Modules.TemplResourceMine = Modules.TemplResourceMine or BaseClass(GUI.Template)

function Modules.TemplResourceMine:__init( id , chapter )
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self:InitTemplate(nil) 

    self.resourceId = id
    self.chapter = chapter
	self:__Getwidget()
    self:__ShowUI()
    self:__Server()
end

function Modules.TemplResourceMine:__CreateWidgetFromLuaCode()
    self.widget = Util:Layout( Macros.Global.DesignSize.width, Macros.Global.DesignSize.height )
    return self.widget
end

function Modules.TemplResourceMine:__delete()
end

function Modules.TemplResourceMine:__Server()
    --刷新章节
    self:BindGlobalEvent(Macros.Event.ResourceWarModule.RESOURCE_WAR_DATA,function (_, data)
        self:__SetShowUI()
    end)  
end

function Modules.TemplResourceMine:__Getwidget()
    local ResourceWarResourceCnf = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetResourceWarResourceCnf", self.resourceId )

    --背景
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "resource_frame" )
    self.bg = Util:Sprite( sp )
    self.bg:SetAnchorPoint( 0.5 , 0.5 )
    self.widget:AddChild( self.bg )
    PosTool.Center( self.bg )

    --标题
    local worldBoss_score = Util:Name( ResourceWarResourceCnf.name , Macros.TypefaceSize.largish , ResourceWarResourceCnf.type )
    worldBoss_score:SetAnchorPoint( 0.5 , 0.5 )
    worldBoss_score:SetPosition( self.bg:GetContentWidth()/2 , self.bg:GetContentHeight() - 20 )
    self.bg:AddChild( worldBoss_score )

    --关闭按钮
    local btn_btn_delete2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_delete2")
    local close = Util:Button( btn_btn_delete2 , CPPGameLib.Handler(self,self.OnBtnClose) )
    close:SetAnchorPoint( 1 , 1 )
    close:SetPosition( self.bg:GetContentWidth() - 20 , self.bg:GetContentHeight() - 12 )
    self.bg:AddChild( close )

    --标题
    local resources_war_extra = Util:Label( CPPGameLib.GetString("resources_war_extra") , Macros.TypefaceSize.largish ,nil ,nil ,nil,Macros.Color.button_yellow )
    resources_war_extra:SetAnchorPoint( 0.5 , 0.5 )
    resources_war_extra:SetPosition( self.bg:GetContentWidth()/2 , self.bg:GetContentHeight()/2 - 25  )
    self.bg:AddChild( resources_war_extra )
    
    -- 占领者
    local resources_war_occupant = Util:Name( CPPGameLib.GetString("resources_war_occupant") )
    resources_war_occupant:SetAnchorPoint( 0.5 , 0.5 )
    resources_war_occupant:SetPosition( 125 , self.bg:GetContentHeight() - 73 )
    self.bg:AddChild( resources_war_occupant )

    -- 产出
    local resources_war_output = Util:Name( CPPGameLib.GetString("resources_war_output") )
    resources_war_output:SetAnchorPoint( 0.5 , 0.5 )
    resources_war_output:SetPosition( resources_war_occupant:GetPositionX() , resources_war_occupant:GetPositionY() - 53 )
    self.bg:AddChild( resources_war_output )

    -- 预计收益
    local resources_war_earnings = Util:Name( CPPGameLib.GetString("resources_war_earnings") )
    resources_war_earnings:SetAnchorPoint( 0.5 , 0.5 )
    resources_war_earnings:SetPosition( resources_war_output:GetPositionX() , resources_war_output:GetPositionY() - 57 )
    self.bg:AddChild( resources_war_earnings )
end

function Modules.TemplResourceMine:__ShowUI()

    local chapterData = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetChapterData", self.resourceId )
    local lst = CallModuleFunc(ModuleType.RESOURCE_WAR,"ToIDAndTimeGetOutput", self.resourceId , chapterData.occupyDuration/60 )

    --当前产出
    for i , v in ipairs( lst.now_lst ) do
        local tab = {
                     { tp = Util:GetMoneyIconImgData( v.item_id ) , jg = 5 ,dx = 0.8},
                     { zi = v.item_num , jg = 5 },
                    }
        self.output = Util:LabCenter(tab)
        self.output:SetAnchorPoint( 0 , 0.5 )
        self.output:SetPosition( (self.bg:GetContentWidth()/2) - 90 + ( (i-1) * 160 )  , self.bg:GetContentHeight() - 130 )
        self.bg:AddChild( self.output )
    end

    --预计收益
    --小时
    if lst.max_time then
        self.resources_war_hour = Util:Label( CPPGameLib.GetString("resources_war_hour" ,  math.floor( lst.max_time.time / 60 )  ) )
        self.resources_war_hour:SetAnchorPoint( 0.5 , 0.5 )
        self.resources_war_hour:SetPosition( (self.bg:GetContentWidth()/2) - 48 , self.bg:GetContentHeight() - 185 )
        self.bg:AddChild( self.resources_war_hour )
    end

    --当前产出
    if lst.max_lst then
        for i , v in ipairs( lst.max_lst ) do
            local tab = {
                         { tp = Util:GetMoneyIconImgData( v.item_id ) , jg = 5 ,dx = 0.6},
                         { zi = v.item_num , jg = 5 , dx = Macros.TypefaceSize.slightly },
                        }
            self.predictOutput = Util:LabCenter(tab)
            self.predictOutput:SetAnchorPoint( 0 , 0.5 )
            self.predictOutput:SetPosition( (self.bg:GetContentWidth()/2) + 40 + ( (i-1) * 160 )  , self.bg:GetContentHeight() - 185 )
            self.bg:AddChild( self.predictOutput )
        end
    end

    --加成
    local additionLst = CallModuleFunc(ModuleType.RESOURCE_WAR,"ToIdGetAddition", self.resourceId )
    for i , v in ipairs( additionLst ) do
        if v.type ~= " " then
            local time = Util:Label( v.type .. v.value .."%" , 14 )
            time:SetAnchorPoint( 0 , 0.5 )
            time:SetPosition( (self.bg:GetContentWidth()/2) + 160 , self.bg:GetContentHeight() - 175 - (( i-1 ) * 20) )
            self.bg:AddChild( time )
        end
    end
        
    --经验条
    local imgdata_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_PROGRESS, "prog_preview")
    local imgdata_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_PROGRESS, "prog_main_bg")
    --进度条
    self.__bar_exp = GUI.ProgressBar9Layer.New(imgdata_bar, imgdata_bg , 376 , 18 , 380 , 21 , 100, 100)
    self.__bar_exp:SetAnchorPoint( 0.5 , 0.5)
    self.bg:AddChild(self.__bar_exp:GetNode())
    PosTool.Center( self.__bar_exp , 0 , -90 )

    --进度
    self.__bar_exp:SetProgressMax( lst.bar_max )

    local chapterData = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetChapterData",self.resourceId)

    local occupyId = CallModuleFunc( ModuleType.RESOURCE_WAR,"GetOccupyId" )
    if occupyId == self.resourceId then
        self.__bar_exp:SetProgress( math.floor(chapterData.occupyDuration / 60) )
    else
        self.__bar_exp:SetProgress( 0 )
    end

    --箱子
    for i , v in ipairs( lst.extra_lst ) do
        local ratio = math.floor( (v.time.time / lst.bar_max) * 100 )
        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "resource_box_"..i)
        local btn = Util:Button( sp , function( ... )
            LogicGUI.TemplResourceMineBox.New( v )
        end )
        btn:SetAnchorPoint( 0.5 , 0 ) 
        btn:SetPosition( (ratio*0.01) * 376 , self.__bar_exp:GetNode():GetContentHeight() - 8 )
        self.__bar_exp:AddChild( btn )

        local time = Util:Name( CPPGameLib.GetString("resources_war_hour" , v.time.time/60 ) , 14 )
        time:SetAnchorPoint( 0.5 , 1 )
        time:SetPosition( btn:GetPositionX() , btn:GetPositionY() + 8 )
        self.__bar_exp:AddChild( time )
    end

    --个人信息
    local chapterData = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetChapterData" , self.resourceId )
    if not chapterData then
        return  
    end

    if chapterData.role_name then
        --头像
        self.RoleIcon = LogicGUI.RoleIcon.New()
        self.RoleIcon:SetData( chapterData.head_img )
        self.RoleIcon:SetAnchorPoint( 0.5 , 0.5 )
        self.RoleIcon:SetPosition( self.bg:GetContentWidth()/2 - 60 , self.bg:GetContentHeight() - 73 )
        self.bg:AddChild(self.RoleIcon:GetNode())
        --缩放调整
        self.RoleIcon:SetScaleFactor( 0.55 )

        --名字
        local name = Util:Label( chapterData.role_name )
        name:SetAnchorPoint( 0  , 0.5 )
        name:SetPosition( self.RoleIcon:GetNode():GetContentWidth()/2 + self.RoleIcon:GetNode():GetPositionX() + 10 , self.RoleIcon:GetNode():GetPositionY() )
        self.bg:AddChild( name )

        --战力
        local zhandouli = Util:Label( CPPGameLib.GetString("zhandouli") , Macros.TypefaceSize.minimum  )
        zhandouli:SetAnchorPoint( 0  , 0.5 )
        zhandouli:SetPosition( self.bg:GetContentWidth()/2 + 130 , name:GetPositionY() )
        self.bg:AddChild( zhandouli )

        local value = Util:Label( chapterData.power , Macros.TypefaceSize.minimum , Macros.Color.orange )
        self.bg:AddChild( value )
        PosTool.RightTo( zhandouli , value )
    else
        --名字
        local name = Util:Label( CPPGameLib.GetString("resources_war_nothave") )
        name:SetAnchorPoint( 0  , 0.5 )
        name:SetPosition( self.bg:GetContentWidth()/2 - 60 , self.bg:GetContentHeight() - 73 )
        self.bg:AddChild( name )
    end


    local occupyId = CallModuleFunc( ModuleType.RESOURCE_WAR,"GetOccupyId" )

    if occupyId ~= self.resourceId then
        self:SetNotHold()
    else
        self:SetHold()
    end
end

--设置持有
function Modules.TemplResourceMine:SetHold()
    local ResourceWarResourceCnf = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetResourceWarResourceCnf", self.resourceId )

    local chapterData = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetChapterData", self.resourceId )
    local target = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetKeepOccupyTimes" )

    local lst = CallModuleFunc(ModuleType.RESOURCE_WAR,"ToIDAndTimeGetOutput", self.resourceId , 0 , target )

    local nexLst = lst.extra_lst[ target + 1 ]
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bigred" )
    --延长    
    self.lengthen = Util:Button( sp , function( ... )
        local value = CallModuleFunc(ModuleType.ROLE,"GetPropValue" , Macros.Game.RolePropType.DIAMOND )
        if nexLst.time.item <= value then
            CallModuleFunc(ModuleType.RESOURCE_WAR,"ResourceWarKeepOccupyReq" , nexLst.time.need_time )
        else
            GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("currency_insufficient_" .. Macros.Game.RolePropType.DIAMOND ) )
        end
    end )
    self.lengthen:SetAnchorPoint( 0.5 , 0.5 )
    self.lengthen:SetPosition( self.bg:GetContentWidth()/2 - 100 , 50 )
    self.bg:AddChild( self.lengthen ) 
    self.lengthen:SetScaleFactor( 0.9 )
    self.lengthen:SetVisible(false)

    --放弃
    self.abandon = Util:Button( sp , function( ... )
        CallModuleFunc(ModuleType.RESOURCE_WAR,"ResourceWarGiveUpOccupyReq" )
        self:OnBtnClose()
    end )
    self.abandon:SetAnchorPoint( 0.5 , 0.5 )
    self.abandon:SetPosition( self.bg:GetContentWidth()/2 + 100 , 50 )
    self.bg:AddChild( self.abandon ) 
    self.abandon:SetScaleFactor( 0.9 )

    --放弃占领
    local resources_war_extra = Util:Label( CPPGameLib.GetString("resources_war_abandon") , Macros.TypefaceSize.largish ,nil ,nil ,nil,Macros.Color.button_red )
    resources_war_extra:SetAnchorPoint( 0.5 , 0.5 )
    resources_war_extra:SetPosition( self.abandon:GetContentWidth()/2 , self.abandon:GetContentHeight()/2 )
    self.abandon:AddChild( resources_war_extra )

    local occupyTimes = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetKeepOccupyTimes" )

    local color_1 = nil
    local color_2 = nil

    local time = 0
    local spread = 0

    color_1 = Macros.Color.common_stroke
    color_2 = 0

    if lst.extra_lst[ target + 1 ] then
        time = lst.extra_lst[ target + 1 ].time.need_time

        spread = lst.extra_lst[ target + 1 ].time.item
    end

    if target + 1 > #lst.extra_lst then
        Util:GreyButton( self.lengthen )
        self.lengthen:SetVisible(false)
        self.abandon:SetPosition( self.bg:GetContentWidth()/2 , 50 )
        return
    end

    --进度条
    self.lengthen:SetVisible( true )
    self.resources_war_lengthen = Util:Label( CPPGameLib.GetString("resources_war_lengthen" , time / 60 ) , 16 , nil ,nil ,nil, color_1 )
    self.resources_war_lengthen:SetPosition( self.lengthen:GetContentWidth()/2 , self.lengthen:GetContentHeight()/2 + 10 )
    self.lengthen:AddChild( self.resources_war_lengthen )

    local tab = {
                 { tp = Util:GetMoneyIconImgData( Macros.Game.RolePropType.DIAMOND ) , dx = 0.5 },
                 { zi = spread , jg = 5, mb = true, dx = 14 , ys = color_2 },
                }
    --用作好几个组件拼起来居中显示的强迫症控件 2.0版
    self.consume = Util:LabCenter(tab)
    self.consume:SetPosition( self.lengthen:GetContentWidth()/2 , self.lengthen:GetContentHeight()/2 - 10)
    self.lengthen:AddChild( self.consume )
end

--设置未持有
function Modules.TemplResourceMine:SetNotHold()
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bigred" )
    --占领
    local occupy = Util:Button( sp , function( ... )
        local checkBattle = CallModuleFunc(ModuleType.RESOURCE_WAR,"CheckBattleCondition" , self.resourceId )
        if checkBattle then
            local chapter = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetAtPresentChapter" )
            CallModuleFunc(ModuleType.BATTLE, "BattleStartReq", Macros.Game.FBType.RESOURCE_WAR , chapter , self.resourceId )
        end
    end )
    occupy:SetAnchorPoint( 0.5 , 0.5 )
    occupy:SetPosition( self.bg:GetContentWidth()/2 , 50 )
    self.bg:AddChild( occupy ) 
    occupy:SetScaleFactor( 0.9 )

    --占领
    local resources_war_abandon_1 = Util:Label( CPPGameLib.GetString("resources_war_abandon_1") , Macros.TypefaceSize.popup ,nil ,nil ,nil,Macros.Color.button_red )
    resources_war_abandon_1:SetAnchorPoint( 0 , 0.5 )
    resources_war_abandon_1:SetPosition( 20 , occupy:GetContentHeight()/2 )
    occupy:AddChild( resources_war_abandon_1 )

    --显示消耗
    local resourceCnf = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetResourceWarResourceCnf" , self.resourceId )
    local tab = {
                 { tp = Util:GetMoneyIconImgData( Macros.Game.RolePropType.DIAMOND ) , jg = 5 ,dx = 0.6},
                 { zi = resourceCnf.cost_diamond , jg = 5 , dx = Macros.TypefaceSize.slightly , mb = true },
                }
    local show = Util:LabCenter(tab)
    show:SetAnchorPoint( 0 , 0.5 )
    show:SetPosition( resources_war_abandon_1:GetPositionX() + resources_war_abandon_1:GetContentWidth() + 5 , occupy:GetContentHeight()/2 )
    occupy:AddChild( show )
end

function Modules.TemplResourceMine:__SetShowUI()
    --拥有刷新
    local chapterData = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetChapterData", self.resourceId )
    local lst = CallModuleFunc(ModuleType.RESOURCE_WAR,"ToIDAndTimeGetOutput", self.resourceId , chapterData.occupyDuration/60 )
    --预计收益
    --小时
    if lst.max_time then
        self.resources_war_hour:SetString( CPPGameLib.GetString("resources_war_hour" ,  math.floor( lst.max_time.time / 60 ) ) )  
    end

    --当前产出
    if self.predictOutput then
        self.predictOutput:RemoveSelf(false)
        self.predictOutput = nil
    end

    --预计产出
    if lst.max_lst then
        for i , v in ipairs( lst.max_lst ) do
            local tab = {
                         { tp = Util:GetMoneyIconImgData( v.item_id ) , jg = 5 ,dx = 0.6},
                         { zi = v.item_num , jg = 5 , dx = Macros.TypefaceSize.slightly },
                        }
            self.predictOutput = Util:LabCenter(tab)
            self.predictOutput:SetAnchorPoint( 0 , 0.5 )
            self.predictOutput:SetPosition( (self.bg:GetContentWidth()/2) + 40 + ( (i-1) * 160 )  , self.bg:GetContentHeight() - 185 )
            self.bg:AddChild( self.predictOutput )
        end
    end

    --进度
    self.__bar_exp:SetProgressMax( lst.bar_max )

    local chapterData = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetChapterData",self.resourceId)
    self.__bar_exp:SetProgress( math.floor(chapterData.occupyDuration / 60) )

    --按钮消耗
    local target = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetKeepOccupyTimes" )
    local lst = CallModuleFunc(ModuleType.RESOURCE_WAR,"ToIDAndTimeGetOutput", self.resourceId , 0 , target )
    local occupyTimes = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetKeepOccupyTimes" )

    local color_1 = nil
    local color_2 = nil

    local time = 0
    local spread = 0
    if lst.extra_lst[ target + 1 ] then
        time = lst.extra_lst[ target + 1 ].time.need_time
        spread = lst.extra_lst[ target + 1 ].time.item
    end

    local occupyId = CallModuleFunc( ModuleType.RESOURCE_WAR,"GetOccupyId" )
    if occupyId ~= self.resourceId then
        return 
    end
    if target + 1 > #lst.extra_lst then
        Util:GreyButton( self.lengthen )
        self.lengthen:SetVisible(false)
        self.abandon:SetPosition( self.bg:GetContentWidth()/2 , 50 )
        return
    end
    local tab = {
                 { tp = Util:GetMoneyIconImgData( Macros.Game.RolePropType.DIAMOND ) , dx = 0.5 },
                 { zi = spread , jg = 5, mb = true, dx = 14 , ys = color_2 },
                }
    --用作好几个组件拼起来居中显示的强迫症控件 2.0版
    self.consume = Util:LabCenter(tab)
    self.consume:SetPosition( self.lengthen:GetContentWidth()/2 , self.lengthen:GetContentHeight()/2 - 10)
    self.lengthen:AddChild( self.consume )

    self.resources_war_lengthen:SetString( CPPGameLib.GetString("resources_war_lengthen" , time / 60 ) )
end

function Modules.TemplResourceMine:OnBtnClose()
	self:DeleteMe()
end

-- @brief 预览宝箱 
-- @author: yjg
-- @date: 2018年8月24日15:43:42
--
LogicGUI = LogicGUI or {}

LogicGUI.TemplResourceMineBox = LogicGUI.TemplResourceMineBox or BaseClass(GUI.Template)

function LogicGUI.TemplResourceMineBox:__init(data)
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__open_callback = function()
        self.data = data
        self:__Getwidget()
        self:__ShowUI(data)

    end
    self:InitTemplate(GUI.TemplateRegistry.DUNGEON_BOXINFO)
end

function LogicGUI.TemplResourceMineBox:__delete()

end

function LogicGUI.TemplResourceMineBox:__Getwidget()
    --居中层
    self.Panel_boxInfo = self:GetWidget("Panel_boxInfo") 
    --背景
    self.Image_6 = self:GetWidget("Image_6") 
    --标题背景
    self.Image_4 = self:GetWidget("Image_4") 
    --内容
    self.Image_9 = self:GetWidget("Image_9") 
    --标题
    self.Text_Title = Util:WidgetLabel( self:GetWidget("Text_Title") , CPPGameLib.GetString("award_box_Title") , Macros.TypefaceSize.popup )
    --提示
    self.Text_GameLevelCondition = self:GetWidget("Text_GameLevelCondition") 

    self.Panel_StarCondition = self:GetWidget("Panel_StarCondition") 
    self.Panel_StarCondition:SetVisible(false)

    self.ScrollView_Rewards = self:GetWidget("ScrollView_Rewards") 
    self.ScrollView_Rewards:SetVisible(false)

    --关闭
    self.Button_Close =  Util:WidgetButton( self:GetWidget("Button_Close") , " " , CPPGameLib.Handler(self,self.OnBtnClose) )
    --确定
    self.Button_Receive = Util:WidgetButton( 
                                                self:GetWidget("Button_Receive") , 
                                                CPPGameLib.GetString("common_confirm") , 
                                                CPPGameLib.Handler(self,self.Button_Receive) ,
                                                nil , Macros.Color.btn , Macros.Color.button_red
                                            )
end

function LogicGUI.TemplResourceMineBox:__ShowUI(data)
    Util:WidgetLabel( self.Text_GameLevelCondition , " " , Macros.TypefaceSize.normal , Macros.Color.content )

    local interval = self.Image_9:GetContentWidth() / 4
    for i,v in ipairs(data.award) do
        local layer = LogicGUI.IconLayer.New(true)
        layer:SetData({ id = v.item_id , num = v.item_num } )
        layer:SetPosition( ( interval / 2 ) + interval * (i - 1) , self.Image_9:GetContentHeight() - ( ( self.Image_9:GetContentHeight()/2 ) / 2 ) - 20 )
        layer:SetAnchorPoint(0.5,0.5)
        layer:ShowItemName(true)
        layer:SetTouchSwallow(false)
        self.Image_9:AddChild(layer:GetNode())
    end

end

function LogicGUI.TemplResourceMineBox:OnBtnClose()
    self:DeleteMe()
end

function LogicGUI.TemplResourceMineBox:Button_Receive()
    self:DeleteMe()
end