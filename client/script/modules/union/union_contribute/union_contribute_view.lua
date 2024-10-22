--
-- @brief 公会贡献界面
-- @author: yjg
-- @date: 2016年10月20日17:27:10
--


LogicGUI = LogicGUI or {}

LogicGUI.TemplUnionContribute = LogicGUI.TemplUnionContribute or BaseClass(GUI.Template)

function LogicGUI.TemplUnionContribute:__init(data)
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__open_callback = function()
        self:__Getwidget()
        self:__ShowUI()
        self:__Server()
    end
    self:InitTemplate(GUI.TemplateRegistry.UNION_CONSTRUCTION) 
end

function LogicGUI.TemplUnionContribute:__delete()
end

function LogicGUI.TemplUnionContribute:__Getwidget( ... )
    --背景
    self.Image_bg = self:GetWidget("Image_bg")
    --背景1
    self.Image_bg_1 = self:GetWidget("Image_bg_1")
    --光
    self.Image_light = self:GetWidget("Image_light")
    --关闭按钮
    self.Button_close = Util:WidgetButton( self:GetWidget("Button_close") ,nil , CPPGameLib.Handler(self,self.OnButtonClose) )
    --装饰
    -- self.Image_decorate = self:GetWidget("Image_decorate")
    --进度条
    self.Panel_bar = self:GetWidget("Panel_bar")
    --内容框
    self.Image_content = self:GetWidget("Image_content")

    --条目
    self.Image_lv = {}
    --标题
    self.Image_found = {}
    --图标
    self.Panel_icon = {}
    --按钮
    self.Button_select = {}
    for i = 1 , 3 do
        self.Image_lv[i] = self:GetWidget("Image_lv"..i)
        self.Image_found[i] = self:GetWidget("Image_found"..i)
        self.Panel_icon[i] = self:GetWidget("Panel_icon"..i)
        self.Button_select[i] = Util:WidgetButton( self:GetWidget("Button_select"..i ) , CPPGameLib.GetString("union_btnConstruction") , function ( ... )
            self:OnButtonSelect( i )
        end , nil , nil , Macros.Color.button_yellow )
    end
    --话板
    self.Image_chat = self:GetWidget("Image_chat")

    local Button_rule = Util:WidgetButton( self:GetWidget("Button_rule") , nil , function ( ... )
        print("xy" , "(2)军团建设，小图标（用于介绍军团建设的用途）：" )
        local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
        help:SetLoadedCallback( function ( ... )
           help:SetCell("Rule_Union_Build")
        end )
    end )
    Button_rule:SetZOrder(100)
end

function LogicGUI.TemplUnionContribute:__ShowUI( ... )
    --标题
    local name = Util:Label(CPPGameLib.GetString("union_construction") , Macros.TypefaceSize.popup , Macros.Color.white)
    name:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2)
    self.Image_light:AddChild(name)

    --图标
    for i = 1 , #self.Panel_icon do
        local icon = Util:Sprite( Resource.PathTool.GetSystemEntryIconPath( "btn_shop" ))
        icon:SetAnchorPoint( 0.5 , 0.5 )
        self.Panel_icon[i]:AddChild( icon )
        PosTool.Center( icon )
    end

    local txt = {
            [1] = { name = "union_unionExp" , value = "exp" } ,
            [2] = { name = "union_unionContribution" , value = "contribution" } ,
            [3] = { name = "union_unionConstruction" , value = "qs" } ,
    }

    self.hero_recruitCostText = {}
    for i = 1 , #self.Image_lv do
        local cnf = CPPGameLib.GetConfig("GuildQY", i)
        for j = 1 , 3 do
            local name = Util:Label( CPPGameLib.GetString( txt[i].name ) , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
            name:SetAnchorPoint( 1 , 0.5 )
            name:SetPosition( self.Image_lv[j]:GetContentWidth()/2 + 20 , (self.Image_lv[j]:GetContentHeight()/2 - 10 ) - (name:GetContentHeight() + 10) * (i-1) )
            self.Image_lv[j]:AddChild(name)

            local value = Util:Label( "+" .. cnf[ txt[j].value ] , Macros.TypefaceSize.slightly , Macros.Color.content )
            value:SetAnchorPoint( 0 , 0.5 )
            value:SetPosition( self.Image_lv[i]:GetContentWidth()/2 + 20 , (self.Image_lv[i]:GetContentHeight()/2 - 10 ) - (name:GetContentHeight() + 10) * (j-1) )
            self.Image_lv[i]:AddChild(value)
        end


        local vipLevel = CallModuleFunc(ModuleType.VIP , "GetVipLV")


        local tab = nil 
        if cnf.need_vip_level > vipLevel then
            tab = {
                    { zi = CPPGameLib.GetString("vip_levelOpen" , cnf.need_vip_level ) , ys = Macros.Color.red , dx = Macros.TypefaceSize.slightly } ,
                }
        else
            tab = {
                    { zi = CPPGameLib.GetString("hero_recruitCostText") , ys = Macros.Color.keypoint , dx = Macros.TypefaceSize.slightly } ,
                    { tp = Util:GetMoneyIconImgData( cnf.cost_type_list[1] ) , dx = 0.7 },
                    { zi = cnf.cost_value_list[1] , ys = Macros.Color.content , dx = Macros.TypefaceSize.slightly , jg = 3 } ,
                }
        end
        self.hero_recruitCostText[i] = Util:LabCenter( tab )
        self.hero_recruitCostText[i]:SetAnchorPoint( 0.5 , 0 )
        self.Image_lv[i]:AddChild(self.hero_recruitCostText[i])
        PosTool.CenterBottom( self.hero_recruitCostText[i] , 0 , 10 )
    end


    local imgdata_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_preview")
    local imgdata_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_bar_interior_bg1")
    --进度条
    self.LoadingBar_loadingBar = GUI.ProgressBar9Layer.New(imgdata_bar, imgdata_bg, self.Panel_bar:GetContentWidth() - 4 , 20 , self.Panel_bar:GetContentWidth() , 24 , 0, 100)
    self.LoadingBar_loadingBar:SetAnchorPoint(0.5, 0.5)
    self.Panel_bar:AddChild(self.LoadingBar_loadingBar:GetNode())
    PosTool.Center(self.LoadingBar_loadingBar)
    self.Panel_bar:GetContentSize( self.LoadingBar_loadingBar:GetNode():GetContentWidth() , self.LoadingBar_loadingBar:GetNode():GetContentHeight() )


    self.icon_path = {
    [1] = { 
                ed = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "green_box_close"),
                op = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "green_box_open"),
                effects = "UI_wood_box"
            },
    [2] = { 
                ed = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "blue_box_close"),
                op = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "blue_box_open"),
                effects = "UI_silverybox"
            },                
    [3] = { 
                ed = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "purple_box_close"),
                op = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "purple_box_open"),
                effects = "UI_blue_box"
            },
    [4] = { 
                ed = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "orange_box_close"),
                op = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "orange_box_open"),
                effects = "UI_red_box"
            },
    }

    local t1 = CallModuleFunc(ModuleType.UNION , "GetUnionScheduleCnf")
    
    
    self.daily = {}
    self.daily_bg = {}
    self.effectsLst = {}
    self.effectsLst_ = {}
    for i,v in ipairs( t1 ) do
        self.daily[v.qs] = Util:Sprite( self.icon_path[i].ed or self.icon_path[1].ed )
        self.daily[v.qs]:SetPosition( self.Panel_bar:GetContentWidth()/(140 / v.qs ) , self.Panel_bar:GetContentHeight() + 30 )
        self.Panel_bar:AddChild(self.daily[v.qs])

        self.daily_bg[v.qs] = Util:Layout( self.daily[v.qs]:GetContentWidth() , self.daily[v.qs]:GetContentHeight() , function ( ... )
            self:OnBox( v )
        end )
        self.daily_bg[v.qs]:SetAnchorPoint(0.5,0.5)
        self.daily_bg[v.qs]:SetPosition( self.Panel_bar:GetContentWidth()/(140 / v.qs ) , self.Panel_bar:GetContentHeight() + 30 )
        self.Panel_bar:AddChild(self.daily_bg[v.qs])

        local target_num = Util:Name(v.qs , Macros.TypefaceSize.slightly )
        target_num:SetPosition( self.daily_bg[v.qs]:GetContentWidth()/2 , -10 )
        self.daily_bg[v.qs]:AddChild(target_num)
    end

    local union_schedule = Util:Label(CPPGameLib.GetString("union_schedule") , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
    union_schedule:SetAnchorPoint( 0 , 0.5 )
    union_schedule:SetPosition( self.Panel_bar:GetPositionX() - (self.Panel_bar:GetContentWidth()/2) , self.Panel_bar:GetPositionY() - union_schedule:GetContentHeight() - 5 )
    self.Image_bg:AddChild( union_schedule )

    self.union_schedule = Util:Label( "0" , Macros.TypefaceSize.slightly , Macros.Color.content )
    self.Image_bg:AddChild( self.union_schedule )
    PosTool.RightTo( union_schedule , self.union_schedule )

    local union_scheduleNNT = Util:Label(CPPGameLib.GetString("union_scheduleNNT") , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
    union_scheduleNNT:SetAnchorPoint( 0 , 0.5 )
    union_scheduleNNT:SetPosition( self.Panel_bar:GetPositionX() + 100 , self.Panel_bar:GetPositionY() - union_schedule:GetContentHeight() - 5 )
    self.Image_bg:AddChild( union_scheduleNNT )

    self.union_scheduleNNT = Util:Label( "00/00" , Macros.TypefaceSize.slightly , Macros.Color.content )
    self.Image_bg:AddChild( self.union_scheduleNNT )
    PosTool.RightTo( union_scheduleNNT , self.union_scheduleNNT )

    self.union_constructionTips = Util:RichText( CPPGameLib.GetString("union_constructionTips" , 0 , 0 , 0 , 0) , Macros.TypefaceSize.popup , self.Image_chat:GetContentWidth() - 35 )
    self.union_constructionTips:SetAnchorPoint( 0 , 1 )
    self.union_constructionTips:SetPosition( 17.5 , self.Image_chat:GetContentHeight() - 10 )
    self.Image_chat:AddChild( self.union_constructionTips )
end

function LogicGUI.TemplUnionContribute:__Server( ... )
    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_MYUNION, function(_, index)
        self:__SetShowUI()
        if index then
            LogicGUI.TemplUnionConstructionSucceed.New( index )
        end
    end)
    self:__SetShowUI()
end

function LogicGUI.TemplUnionContribute:__SetShowUI( ... )

    --选择的类型
    local type_ = CallModuleFunc(ModuleType.UNION , "GetUnionType")
    if type_ ~= 0 then
        self.hero_recruitCostText[type_]:SetVisible(false)
        for i = 1 , 3 do
            Util:GreyButton( self.Button_select[i] )
        end

        local union_yet_found = Util:Sprite( 
                                                Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION, "union_yet_found" , true )
                                            )
        union_yet_found:SetAnchorPoint( 0.5 , 0 )
        union_yet_found:SetPosition( self.hero_recruitCostText[type_]:GetPositionX() , self.hero_recruitCostText[type_]:GetPositionY() )
        self.Image_lv[type_]:AddChild( union_yet_found )
    end
    --当前进度
    local schedule = CallModuleFunc(ModuleType.UNION , "GetUnionSchedule")
    self.union_schedule:SetString( schedule )
    --建设人数
    local scheduleNum = CallModuleFunc(ModuleType.UNION , "GetUnionScheduleNum")
    local memberNum , max = CallModuleFunc(ModuleType.UNION , "GetUnionMemberNum")

    self.union_scheduleNNT:SetString( scheduleNum .. "/" .. max )

    local type_ = CallModuleFunc(ModuleType.UNION , "GetUnionLastType")
    if type_ ~= 0 then
        local cnf = CPPGameLib.GetConfig("GuildQY", type_)
        self.Image_chat:SetVisible(true)
        self.union_constructionTips:SetText( 
                                                CPPGameLib.GetString("union_constructionTips" , 
                                                CallModuleFunc(ModuleType.UNION , "GetUnionLastName") , 
                                                CPPGameLib.GetString("union_constructionType" .. type_ ) , 
                                                cnf.qs , 
                                                CallModuleFunc(ModuleType.UNION , "GetUnionLastValue")
                                            ) )
    else
        self.Image_chat:SetVisible(false)
    end

    self:__SetGiftBag()
end

--礼包
function LogicGUI.TemplUnionContribute:__SetGiftBag()

    local t1 = CallModuleFunc(ModuleType.UNION , "GetUnionScheduleCnf")
    local schedule = CallModuleFunc(ModuleType.UNION , "GetUnionSchedule")
    self.LoadingBar_loadingBar:SetProgress( schedule / 140 * 100 )

    for i,v in ipairs( t1 ) do
        local type_ = CallModuleFunc(ModuleType.UNION, "GetBoxType" , v.qs )
        if type_ then

            self.daily[v.qs]:SetImage( self.icon_path[i].op )
            self.daily[v.qs]:SetPosition( self.daily[v.qs]:GetPositionX() , 52.5 )

            if self.effectsLst[v.qs] then
                self:__DelEffect(self.effectsLst[v.qs].id)
                self.effectsLst[v.qs].id = nil
                self.effectsLst[v.qs].con = nil
            end

            if self.effectsLst_[v.qs] then
                self:__DelEffect(self.effectsLst_[v.qs].id)
                self.effectsLst_[v.qs].id = nil
                self.effectsLst_[v.qs].con = nil
            end

            self.daily[v.qs]:SetVisible(true)

        elseif v.qs <= schedule then

            if self.effectsLst[v.qs] then
                self:__DelEffect(self.effectsLst[v.qs].id)
                self.effectsLst[v.qs].id = nil
                self.effectsLst[v.qs].con = nil
            end

            if self.effectsLst_[v.qs] then
                self:__DelEffect(self.effectsLst_[v.qs].id)
                self.effectsLst_[v.qs].id = nil
                self.effectsLst_[v.qs].con = nil
            end

            self.effectsLst_[v.qs] = {}
            self.effectsLst_[v.qs].id , self.effectsLst_[v.qs].con = self:__CreateEffect({res_name = "UI_xingjibaoxiang" }, self.daily_bg[v.qs] )
            self.effectsLst_[v.qs].con:SetAnchorPoint(0.5 , 0.5 )
            self.effectsLst_[v.qs].con:SetPosition(  self.daily_bg[v.qs]:GetContentWidth()/2 + 10 ,  self.daily_bg[v.qs]:GetContentHeight()/2 )

            self.effectsLst[v.qs] = {}
            self.effectsLst[v.qs].id , self.effectsLst[v.qs].con = self:__CreateEffect( {res_name = self.icon_path[i].effects , type = Effect.EffectType.SKELETON , act_index = RenderObj.ActionIndex.IDLE } , self.daily_bg[v.qs] )
            self.effectsLst[v.qs].con:SetAnchorPoint(0.5 , 0.5 )
            self.effectsLst[v.qs].con:SetPosition(  self.daily_bg[v.qs]:GetContentWidth()/2 + 10 ,  0 )
        
            self.daily[v.qs]:SetVisible(false)
        end
    end
end

function LogicGUI.TemplUnionContribute:OnButtonClose( ... )
    self:DeleteMe()
end

function LogicGUI.TemplUnionContribute:OnButtonSelect( i )
    local type_ = CallModuleFunc(ModuleType.UNION , "GetUnionType")
    if type_ ~= 0 then
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_todayYetConstruction") )
    else
        local cnf = CPPGameLib.GetConfig("GuildQY", i)
        local num = CallModuleFunc( ModuleType.ROLE , "GetPropValue" , cnf.cost_type_list[1] )
        local lv = CallModuleFunc(ModuleType.VIP , "GetVipLV")

        local scheduleNum = CallModuleFunc(ModuleType.UNION , "GetUnionScheduleNum")
        local unionLevel = CallModuleFunc(ModuleType.UNION , "GetUnionLevel")
        local unionCnf = CPPGameLib.GetConfig("Guild", unionLevel )

        if scheduleNum >= unionCnf.member_max then
            GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString( "union_constructionMax" ) )
        elseif num >= cnf.cost_value_list[1] and cnf.need_vip_level <= lv then
            local info = {}
            info.type = i
            CallModuleFunc(ModuleType.UNION , "GuildQyReq" , info )
        elseif cnf.need_vip_level > lv then
            GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString( "union_desiredNoVip" ) )
        else
            GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString( "currency_insufficient_" .. cnf.cost_type_list[1] ) )
        end
    end
end

function LogicGUI.TemplUnionContribute:OnBox( data )
    local type_ = CallModuleFunc(ModuleType.UNION, "GetBoxType" , data.qs )
    local schedule = CallModuleFunc(ModuleType.UNION , "GetUnionSchedule")

    if not type_ and schedule >= data.qs then
         LogicGUI.TemplUnionConstructionPreview.New( data , function ( ... )
            CallModuleFunc(ModuleType.UNION , "GuildQsAwardReq" , data )
         end , CPPGameLib.GetString("get_Award_Text") )
    elseif type_ then
        LogicGUI.TemplUnionConstructionPreview.New( data , nil , CPPGameLib.GetString("rebel_getWin") )
    else
        LogicGUI.TemplUnionConstructionPreview.New( data , nil , CPPGameLib.GetString("daliyMission_CloseGetAwardView") )
    end
end




-- @brief 建设成功 
-- @author: yjg
-- @date: 2016年8月8日15:53:48
--
LogicGUI = LogicGUI or {}
LogicGUI.TemplUnionConstructionSucceed  = LogicGUI.TemplUnionConstructionSucceed or BaseClass(GUI.Template)

function LogicGUI.TemplUnionConstructionSucceed:__init(data)
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__open_callback = function()
        self:__Getwidget()
        self:__ShowUI(data)
    end
    self:InitTemplate(GUI.TemplateRegistry.UNION_CONSTRUCTION_SUCCEED)
end

function LogicGUI.TemplUnionConstructionSucceed:__delete()
end

function LogicGUI.TemplUnionConstructionSucceed:__Getwidget()
    --彩带
    self.Image_colour = self:GetWidget("Image_colour") 
    --星
    self.Image_fryStar = self:GetWidget("Image_fryStar") 
    --内容
    self.Image_content = self:GetWidget("Image_content") 
    self.Image_content:SetImage(Resource.PathTool.GetUIResPath("citycommonslices/bg_first_win"), TextureResType.LOCAL)

    --建设成功
    self.Image_foundSucceed = self:GetWidget("Image_foundSucceed") 
    self.Image_foundSucceed:IgnoreContentAdaptWithSize(true)

    --点击继续
    self.Text_continue = self:GetWidget("Text_continue") 

    --军团建设    
    self.Text_breakUpTo = self:GetWidget("Text_breakUpTo") 

    --军团建设Value    
    self.Text_breakRank = self:GetWidget("Text_breakRank") 

    --个人贡献    
    self.Text_rankUp = self:GetWidget("Text_rankUp")    

    --个人贡献Value  
    self.Text_rankUpNum = self:GetWidget("Text_rankUpNum")  

    --建设进度  
    self.Text_breakEncourage = self:GetWidget("Text_breakEncourage")  
    
    --建设进度Value
    self.Text_breakEncourageNum = self:GetWidget("Text_breakEncourageNum")  

end

function LogicGUI.TemplUnionConstructionSucceed:__ShowUI( index )
    -- local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION, "union_found_succeed" , true )
    -- local union_found_succeed = Util:Sprite( sp )
    -- union_found_succeed:SetAnchorPoint( 0.5 , 0.5 )
    -- self.Image_colour:AddChild( union_found_succeed )
    -- PosTool.Center( union_found_succeed , 0 , 5)

    local txt = {
            [1] = { name = "union_unionExp" , value = "exp" } ,
            [2] = { name = "union_unionContribution" , value = "contribution" } ,
            [3] = { name = "union_unionConstruction" , value = "qs" } ,
    }
    local cnf = CPPGameLib.GetConfig("GuildQY", CallModuleFunc(ModuleType.UNION , "GetUnionType" )  )

    --军团建设  
    self.Text_breakUpTo = Util:WidgetLabel( 
                                            self.Text_breakUpTo , 
                                            CPPGameLib.GetString("union_unionExp"), 
                                            Macros.TypefaceSize.popup , 
                                            Macros.Color.Special
                                        )

    --军团建设Value   
    self.Text_breakRank = Util:WidgetLabel( 
                                            self.Text_breakRank , 
                                            "+"..CallModuleFunc(ModuleType.UNION , "GetUnionQyExp" ) , 
                                            Macros.TypefaceSize.popup , 
                                            Macros.Color.green
                                        )

    --个人贡献   
    self.Text_rankUp = Util:WidgetLabel( 
                                            self.Text_rankUp , 
                                            CPPGameLib.GetString("union_unionContribution") , 
                                            Macros.TypefaceSize.popup , 
                                            Macros.Color.Special
                                        )
    --个人贡献Value  
    self.Text_rankUpNum = Util:WidgetLabel( 
                                            self.Text_rankUpNum , 
                                            "+"..CallModuleFunc(ModuleType.UNION , "GetUnionQyContribution" ) , 
                                            Macros.TypefaceSize.popup , 
                                            Macros.Color.green
                                        )

    --个人贡献   
    self.Text_breakEncourage = Util:WidgetLabel( 
                                            self.Text_breakEncourage , 
                                            CPPGameLib.GetString("union_unionConstruction") , 
                                            Macros.TypefaceSize.popup , 
                                            Macros.Color.Special
                                        )
    --个人贡献Value  
    self.Text_breakEncourageNum = Util:WidgetLabel( 
                                            self.Text_breakEncourageNum , 
                                            "+"..cnf.qs , 
                                            Macros.TypefaceSize.popup , 
                                            Macros.Color.green
                                        )

    --点击  
    self.Text_continue = Util:WidgetLabel( 
                                            self.Text_continue , 
                                            CPPGameLib.GetString("common_continue") , 
                                            Macros.TypefaceSize.popup , 
                                            Macros.Color.white
                                        )
    -- self.__bg_params.close_func = function ( ... )
    --     self:DeleteMe()
    -- end
    -- 播放动画
    self:PlayActionTimeline("animation", false,function ()
        WidgetUnity.SetWidgetListener(self:GetNode(), nil, nil,function ()
            self:DeleteMe()
        end)
    end)

end



-- @brief 还未开启的预览 
-- @author: yjg
-- @date: 2016年8月8日15:53:48
--
LogicGUI = LogicGUI or {}

LogicGUI.TemplUnionConstructionPreview = LogicGUI.TemplUnionConstructionPreview or BaseClass(GUI.Template)

function LogicGUI.TemplUnionConstructionPreview:__init(data , func , text )
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__open_callback = function()
        self.data = data
        self.func = func
        self:__Getwidget( text )
        self:__ShowUI(data)
    end
    self:InitTemplate(GUI.TemplateRegistry.DUNGEON_BOXINFO)
end

function LogicGUI.TemplUnionConstructionPreview:__delete()
    if self.lst_list then 
        self.lst_list:DeleteMe()
        self.lst_list = nil
    end
end

function LogicGUI.TemplUnionConstructionPreview:__Getwidget( text )
    --标题
    self.Text_Title = Util:WidgetLabel(     
                                        self:GetWidget("Text_Title"), 
                                        CPPGameLib.GetString("union_constructionPreview"), 
                                        Macros.TypefaceSize.popup, 
                                        Macros.Color.white
                                    )
    --不要的
    self.Text_GameLevelCondition = self:GetWidget("Text_GameLevelCondition") 
    self.Text_GameLevelCondition:SetVisible(false)
    self.Panel_StarCondition = self:GetWidget("Panel_StarCondition") 
    self.Panel_StarCondition:SetVisible(false)
    self.ScrollView_Rewards = self:GetWidget("ScrollView_Rewards") 
    self.ScrollView_Rewards:SetVisible(false)

    --内容
    self.Image_9 = self:GetWidget("Image_9") 
    
    self.Button_Close =  Util:WidgetButton( self:GetWidget("Button_Close") , " " , CPPGameLib.Handler(self,self.OnBtnClose) )
    self.Button_Receive =  Util:WidgetButton( 
                                                self:GetWidget("Button_Receive") , 
                                                text , 
                                                CPPGameLib.Handler(self,self.OnButtonReceive) ,
                                                nil,nil,Macros.Color.button_red
                                                )
end


-- 2017年5月8日16:55:09
-- 大硕硕说最多四个
function LogicGUI.TemplUnionConstructionPreview:__ShowUI(data)

    local layou = Util:Layout( self.Image_9:GetContentWidth() - 20 , self.Image_9:GetContentHeight() )
    layou:SetAnchorPoint(0.5,0.5)
    layou:SetPosition(self.Image_9:GetContentWidth()/2 ,self.Image_9:GetContentHeight()/2 )
    self.Image_9:AddChild(layou)

    local interval = layou:GetContentWidth() / 4
    --图标
    for i,v in ipairs(data.item_id_list) do
        local icon = LogicGUI.IconLayer.New(true)
        icon:SetData({ id = v , num = data.item_num_list[i] })
        icon:SetPosition( (interval/2) + interval * (i - 1)  , layou:GetContentHeight()/2 + 10)
        icon:SetAnchorPoint(0.5,0.5)
        layou:AddChild(icon:GetNode())

        local type_ , info_ = ObjIDTool.GetInfo(v)
        local name = Util:Name( info_.name , Macros.TypefaceSize.normal ,info_.quality , nil , interval )
        name:SetAnchorPoint( 0.5 , 1 )
        name:SetPosition( icon:GetNode():GetPositionX() , 50 )
        layou:AddChild(name)
    end

end

function LogicGUI.TemplUnionConstructionPreview:OnBtnClose()
    self:DeleteMe()
end

function LogicGUI.TemplUnionConstructionPreview:OnButtonReceive()
    if self.func then
        self.func()
    end
    self:OnBtnClose()
end
