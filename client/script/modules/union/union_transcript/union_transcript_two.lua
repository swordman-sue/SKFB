--
-- @brief 公会副本
-- @author: yjg
-- @date: 2017年5月18日10:43:55
--

Modules = Modules or {}

Modules.UnionTranscriptTwoView = Modules.UnionTranscriptTwoView or BaseClass(BaseView)
function Modules.UnionTranscriptTwoView:__init()
    self.__layout_name = "union_transcript"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bp_sp_union_transcript")}
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("union_transcript"),
        show_diamond = true,
        show_gold = true,
        show_contribution = true,
        -- show_energy = true,
        show_power = true,
        close_func = function()
            self:CloseManual()
        end,
    }

    self.__open_callback = function()
        self:__OpenCallback()
    end

    self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_UNION_TRANSCRIPT_ITEM) 
end

function Modules.UnionTranscriptTwoView:__Dispose()   
    if self.transcriptPlayer then
        self.transcriptPlayer:DeleteMe()
        self.transcriptPlayer = nil
    end
end

function Modules.UnionTranscriptTwoView:__OpenCallback()    
    self:__Getwidget()

    self:__ShowUI()
    self:__Server()
end

--数据
function Modules.UnionTranscriptTwoView:__Server()
    CallModuleFunc(ModuleType.UNION , "GuildFbDataReq")
    CallModuleFunc(ModuleType.UNION , "GuildMonsterDataReq")
    CallModuleFunc(ModuleType.UNION , "GuildMyMapRewardRecordReq")
    
    --二层

    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_FB_DATA , function (_, data)
        self:__Transfer( true )
    end)

    --血量刷新
    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_MURBER , function (_, data)
        self:__SetOnSelectShowUI()
    end)

    --军团副本章节奖励
    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_AWARD_GET , function (_, data)
        local red = CallModuleFunc(ModuleType.UNION , "GetMayGetAwardRed")
        if red ~= 0 then
            self.icon_red_point:SetVisible(true)
            self.union_player:SetString( CPPGameLib.GetString("tower_drawAward") )
        else
            self.icon_red_point:SetVisible(false)
            self.union_player:SetString( CPPGameLib.GetString("union_player") )
        end
    end)

    --军团副本关卡奖励
    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_GET_RECORD , function (_, data)
        self:__SetShowUI()
    end)

    --数据刷新
    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_FB , function (_, data)
        self:__SetShowUI()
    end)
end

function Modules.UnionTranscriptTwoView:__Getwidget( ... )
    --次数背景
    self.Image_timeBg = self:GetWidget("Image_timeBg")
    --增加按钮
    self.Button_5 =  Util:WidgetButton( 
                                            self:GetWidget("Button_5"),
                                            "",
                                            function( ... )
                                                --剩余次数
                                                local time_ = CallModuleFunc(ModuleType.UNION, "GetUnionTranscriptBuyTime")
                                                if time_ ~= 0 then
                                                    LogicGUI.TemplUnionBuytime.New()
                                                else
                                                    local tips = LogicGUI.TemplAffirmTips.New( { 
                                                                                                    type = LogicGUI.TemplAffirmTipsType.YES_NO,
                                                                                                    content = CPPGameLib.GetString("union_surplus") ,
                                                                                                    cancel_text = CPPGameLib.GetString("vip_Isee") ,
                                                                                                    confirm_text = CPPGameLib.GetString("vip_Toimprove") ,
                                                                                                    confirm_func = function ( ... )
                                                                                                        CallModuleFunc(ModuleType.CHARGE, "OpenView", BaseViewType.CHARGE )
                                                                                                    end,
                                                                                                    cancel_func = function ( ... )
                                                                                                    end
                                                                                                } )
                                                end
                                            end
                                            )

    --提示
    self.Text_time = self:GetWidget("Text_time")
    --通关奖励
    self.Button_player = Util:WidgetButton( 
                                            self:GetWidget("Button_player"),
                                            "",
                                            CPPGameLib.Handler(self,self.OnButtonPlayer)
                                            )
    --成员战绩
    self.Button_record = Util:WidgetButton( 
                                            self:GetWidget("Button_record"),
                                            "",
                                            CPPGameLib.Handler(self,self.OnButtonRecord)
                                            )  
    --重置设定
    self.Button_reset = Util:WidgetButton( 
                                            self:GetWidget("Button_reset"),
                                            "",
                                            CPPGameLib.Handler(self,self.OnButtonReset)
                                            )  
    --规则说明
    self.Button_rule = Util:WidgetButton( 
                                            self:GetWidget("Button_rule"),
                                            "",
                                            CPPGameLib.Handler(self,self.OnButtonRule)
                                            )  

    --第一层
    self.Panel_one = self:GetWidget("Panel_one")
    self.Panel_one:SetVisible(false)
        --滚动层
        self.Panel_Chapters = self:GetWidget("Panel_Chapters")

    --第二层
    self.Panel_two = self:GetWidget("Panel_two")
    self.Panel_two:SetVisible(false)
        --名字容器
        self.Image_name = self:GetWidget("Image_name")
        --进度条
        self.Panel_bar = self:GetWidget("Panel_bar")

        --箱子
        self.Panel_box = self:GetWidget("Panel_box")
        --怪物模型
        self.Panel_monster = {}
        for i = 1 , 3 do
            self.Panel_monster[i] = self:GetWidget( "Panel_monster" .. i )
        end
end

--中转
function Modules.UnionTranscriptTwoView:__Transfer( type_ )
    self.Panel_two:SetVisible(true)
    self:__SetShowUI()
    self:__SetOnSelectShowUI()
end

-- GetContentHeight
function Modules.UnionTranscriptTwoView:__ShowUI( ... )
    --通关奖励
    self.Button_player:SetNormalImage(Resource.PathTool.GetSystemEntryIconPath("btn_reward"), TextureResType.LOCAL)

    --红点
    local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
    self.icon_red_point = Util:Sprite(icon_red_point)
    self.icon_red_point:SetPosition( self.Button_player:GetContentWidth() - 10 , self.Button_player:GetContentHeight() - 10 )
    self.Button_player:AddChild( self.icon_red_point , 20 )
    self.icon_red_point:SetVisible(false)

    self.union_player = Util:Name(CPPGameLib.GetString("union_player"))
    self.union_player:SetAnchorPoint( 0.5,0.5 )
    self.union_player:SetPosition( self.Button_player:GetContentWidth()/2 , 0 )
    self.Button_player:AddChild( self.union_player )

    --成员战绩
    self.Button_record:SetNormalImage(Resource.PathTool.GetSystemEntryIconPath("btn_daily"), TextureResType.LOCAL)

    local union_record = Util:Name(CPPGameLib.GetString("union_record"))
    union_record:SetAnchorPoint( 0.5,0.5 )
    union_record:SetPosition( self.Button_record:GetContentWidth()/2 , 0 )
    self.Button_record:AddChild( union_record )

    --重置设定
    self.Button_reset:SetNormalImage(Resource.PathTool.GetSystemEntryIconPath("btn_guild"), TextureResType.LOCAL)
    
    local union_reset = Util:Name(CPPGameLib.GetString("union_reset"))
    union_reset:SetAnchorPoint( 0.5,0.5 )
    union_reset:SetPosition( self.Button_reset:GetContentWidth()/2 , 0 )
    self.Button_reset:AddChild( union_reset )  

    --规则说明
    -- self.Button_rule:SetNormalImage(Resource.PathTool.GetSystemEntryIconPath("btn_guild"), TextureResType.LOCAL)
    
    local union_rule = Util:Name(CPPGameLib.GetString("union_rule"))
    union_rule:SetAnchorPoint( 0.5,0.5 )
    union_rule:SetPosition( self.Button_rule:GetContentWidth()/2 , 0 )
    self.Button_rule:AddChild( union_rule )  

    --今日已挑战次数
    self.union_threats = Util:Label(CPPGameLib.GetString("union_threats"))
    self.union_threats:SetAnchorPoint( 0 , 0.5 )
    self.union_threats:SetPosition( 5 , self.Image_timeBg:GetContentHeight()/2 )
    self.Image_timeBg:AddChild( self.union_threats )

    local _ , startTime , endtime =  CallModuleFunc(ModuleType.UNION , "GetStartTime")
    local time = ConfigAdapter.Common.GetUnionDefaultChallengeTime()
    local cd = ConfigAdapter.Common.GetUnionDefaultChallengeCD()
    --提示
    local union_transcript_tips = Util:Name(CPPGameLib.GetString("union_transcript_tips" , startTime , endtime , time , cd ))
    union_transcript_tips:SetAnchorPoint(0.5,0.5)
    self.Text_time:AddChild( union_transcript_tips )
    PosTool.Center( union_transcript_tips )
 

    --第二层
    --名字
    self.name = Util:RichText(string.format(Macros.Color.city_headline_rt, "  " ), Macros.TypefaceSize.normal)
    self.name:SetAnchorPoint( 0 , 0.5 )
    self.name:SetPosition( 10 , self.Image_name:GetContentHeight()/2 )
    self.Image_name:AddChild( self.name )

    local bg_hero_shadow = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_hero_shadow"))
    self.Panel_box:AddChild( bg_hero_shadow )
    bg_hero_shadow:SetScaleFactor( 1.3 )
    PosTool.Center( bg_hero_shadow , 0 , 0 - (bg_hero_shadow:GetContentHeight()/2) - 10 )
    
    --宝箱
    local ed = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "orange_box_close")
    self.box = Util:Button( ed , CPPGameLib.Handler(self,self.OnBox) )
    bg_hero_shadow:AddChild( self.box )
    self.box:SetScaleFactor( 1.2 )
    PosTool.Center( self.box , -10 , (bg_hero_shadow:GetContentHeight()/2) + 10 )

    --怪物
    self.monster = {}
    self.monster.mode = {}
    self.monster.bar = {}
    self.monster.killName = {}
    self.monster.monsterName = {}
    self.monster.killed = {}
    self.monster.shadow = {}

    --血条
    local progressBar_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_PROGRESS, "prog_bar_bg")
    local progressBar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_PROGRESS, "prog_bar_bar_green2")
    --击杀图样
    local txt_killed = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "txt_killed" , true )

    for i = 1 , 3 do
        --怪物地光
        self.monster.shadow[i] = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_hero_shadow"))
        self.monster.shadow[i]:SetPosition( self.Panel_monster[i]:GetContentWidth()/2  ,0 )
        self.Panel_monster[i]:AddChild(self.monster.shadow[i])

        --模型
        self.monster.mode[i] = LogicGUI.Model.New({info_id = nil , model_type = LogicGUI.ModelType.MONSTER , parent = self.monster.shadow[i] , dir = MathTool.HORZ_LEFT })
        self.monster.mode[i] :SetScaleFactor(0.9)
        self.monster.mode[i] :SetPosition(self.monster.shadow[i]:GetContentWidth()/2  , self.monster.shadow[i]:GetContentHeight()/2 )
        self.Panel_monster[i]:SetVisible(false)
        
        --血条
        self.monster.bar[i] = GUI.ProgressBar9Layer.New(progressBar,progressBar_bg,143,12,143,12) 
        self.monster.bar[i]:SetAnchorPoint(0.5,0.5)
        self.Panel_monster[i]:AddChild(self.monster.bar[i]:GetNode() )
        PosTool.Center( self.monster.bar[i] , 0 , 120 )
        self.monster.bar[i]:SetVisible(false)

        --击杀者名字
        self.monster.killName[i] = Util:Name( "" , Macros.TypefaceSize.normal , 100 )
        self.monster.killName[i]:SetAnchorPoint( 0.5 , 0.5 )
        self.Panel_monster[i]:AddChild(self.monster.killName[i] )
        PosTool.Center( self.monster.killName[i] , 0 , 120 )
        self.monster.killName[i]:SetVisible(false)

        --怪物名
        self.monster.monsterName[i] = {}
        local tab = {
            { tp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "camp_icon_".. i - 1 , true ) , dx = 0.6 },
            { zi = "" , dx = Macros.TypefaceSize.normal , jg = 10 },
        }
        self.monster.monsterName[i].layer , self.monster.monsterName[i].lst = Util:LabCenter( tab )
        self.monster.monsterName[i].layer:SetAnchorPoint( 0.5 , 0.5 )
        self.Panel_monster[i]:AddChild( self.monster.monsterName[i].layer )
        PosTool.Center( self.monster.monsterName[i].layer , 0 , 145 )
        self.monster.monsterName[i].layer:SetVisible(false)

        self.monster.monsterName[i].lst[1]:SetPosition( self.monster.monsterName[i].lst[1]:GetPositionX() , self.monster.monsterName[i].lst[1]:GetPositionY() + 5 )

        local icon_upgrade_tips = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_upgrade_tips" )
        local icon_upgrade_tips_ = Util:Sprite( icon_upgrade_tips )
        icon_upgrade_tips_:SetAnchorPoint( 0.5 , 0 )
        icon_upgrade_tips_:SetPosition( self.monster.monsterName[i].lst[1]:GetContentWidth() - 7 , 1 )
        self.monster.monsterName[i].lst[1]:AddChild( icon_upgrade_tips_ )
        icon_upgrade_tips_:SetScaleFactor( 1 )

        --击杀图样
        self.monster.killed[i] = Util:Sprite( txt_killed )
        self.monster.killed[i]:SetAnchorPoint( 0.5 , 0.5 )
        self.Panel_monster[i]:AddChild( self.monster.killed[i] )
        PosTool.Center( self.monster.killed[i])
        self.monster.killed[i]:SetVisible(false)
    end

    --血条
    local progressBar_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_little_preview_bg")
    local progressBar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_little_preview")
    self.__progressbar = GUI.ProgressBar9Layer.New(progressBar,progressBar_bg,196,20,200,23) 
    self.__progressbar:SetAnchorPoint(0.5,0.5)
    self.Panel_bar:AddChild(self.__progressbar:GetNode())
    PosTool.Center( self.__progressbar )

    --字
    self.barText = Util:Name( "0%" , Macros.TypefaceSize.slightly )
    self.barText:SetAnchorPoint( 0.5 ,0.5 )
    self.Panel_bar:AddChild(self.barText)
    PosTool.Center( self.barText , 0 , -1 )

    local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
    self.icon_red_point_ = Util:Sprite(icon_red_point)
    self.icon_red_point_:SetPosition( self.Panel_box:GetContentWidth() - 5 , self.Panel_box:GetContentHeight() + 5 )
    self.Panel_box:AddChild(self.icon_red_point_ , 20)
    self.icon_red_point_:SetVisible( false )
end

--设置ui
function Modules.UnionTranscriptTwoView:__SetShowUI( ... )

    --今日已经挑战次数
    self.union_threats:SetString( CPPGameLib.GetString("union_threats") .. CallModuleFunc(ModuleType.UNION , "GetNowChallengeTime") ) 

    local lst = CallModuleFunc(ModuleType.UNION, "GetMyMapReward" , unpack(self.__open_data).section )

    if not lst then return end

    local section = CallModuleFunc(ModuleType.UNION, "GetNowAttackSection") 
    local getType_ = true
    for i , v in ipairs( lst ) do
        if v.is_take_reward == 0 then
            if section == unpack(self.__open_data).section then
                local campHP = CallModuleFunc(ModuleType.UNION , "GetMonsterData" , i )

                CPPGameLib.PrintTable("xy" , campHP , "campHP")  

                if campHP.killer_name ~= "" then
                    getType_ = false
                end
            else
                getType_ = false
            end
        end
    end

    if getType_ == false then
        self.icon_red_point_:SetVisible(true)
    else
        self.icon_red_point_:SetVisible(false)
    end
end

--设置怪物ui
function Modules.UnionTranscriptTwoView:__SetOnSelectShowUI( ... )
    --设置现在要打的章节
    local section = CallModuleFunc(ModuleType.UNION , "GetNowAttackSection")

    local zj = true
    if self.__open_data[1].section ~= section then
        zj = false
        section = self.__open_data[1].section
    end

    local levelCnf = CallModuleFunc(ModuleType.UNION , "GetMonsterData" , self.__open_data[1].level )
    local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")

    if self.__open_data[1].is_me_kill and self.__open_data[1].is_me_kill == 1 then
        if not self.TranscriptPlayer then
            self.__open_data[1].is_me_kill = 0
            self.transcriptPlayer = LogicGUI.TemplUnionTranscriptPlayer.New( self.__open_data[1] , function ( ... )
                self.transcriptPlayer = nil
            end )
        end
    end

    --副本
    local FBcnf = CPPGameLib.GetConfig("GuildFB", section)

    --设置模型
    for i,v in ipairs(FBcnf.gamelevel_list) do
        local level = CPPGameLib.GetConfig("GameLevel" , v )
        --模型
        local monter = CPPGameLib.GetConfig( "Monster" , level.icon_id )

        self.monster.mode[i]:SetScaleFactor( monter.show_scale or 1 )
        self.monster.mode[i]:SetInfoID( level.icon_id )
        self.monster.mode[i]:SetTouchRect(-100, 0, 200, 220)
        self.Panel_monster[i]:SetVisible( true )

        self.monster.killName[i]:SetVisible(false)
        self.monster.bar[i]:SetVisible(false)
        self.monster.monsterName[i].layer:SetVisible(false)
        self.monster.killed[i]:SetVisible(false)

        --血量
        local campHP = CallModuleFunc(ModuleType.UNION , "GetMonsterData" , i )
        local _ , __ , hp = ConfigAdapter.GameLevel.GetMonsterList( v )
        self.monster.bar[i]:SetProgressMax( hp )
        self.monster.bar[i]:SetProgress( campHP.max - campHP.damage )

        --被杀掉了
        if campHP.max - campHP.damage <= 0 or zj == false then
            --设置击杀者名字
            local name = CallModuleFunc(ModuleType.UNION , "GetMonsterData" , i ).killer_name
            if zj == true then
                self.monster.killName[i]:SetString( CPPGameLib.GetString("union_whoSmote") .. name )
                self.monster.killName[i]:SetVisible(true)
            end
            --模型触摸
            self.monster.mode[i]:SetTouchFun(nil, nil, function()
                print("xy"  ,"已经被击杀" )
            end)
            self.monster.killed[i]:SetVisible(true)
            self.monster.mode[i]:SetGray(true)
            self.monster.mode[i]:SetToPlayTime(0)
        else
            --血条显示
            self.monster.bar[i]:SetVisible(true)
            --名字
            self.monster.monsterName[i].layer:SetVisible(true)
            --怪物表
            local monsterCnf = CPPGameLib.GetConfig("Monster" , level.icon_id )
            --设置
            self.monster.monsterName[i].lst[2]:SetString( monsterCnf.name )
            self.monster.monsterName[i].lst[2]:SetColor(unpack( Macros.Color.purple ))
            self.monster.monsterName[i].lst[2]:SetStrokeColor(unpack( Macros.Color.purple_stroke ))
            self.monster.monsterName[i].lst[2]:SetStrokeWidth(2)

            self.monster.monsterName[i].layer:SetContentSize( 
                                                                self.monster.monsterName[i].layer:GetContentWidth() + self.monster.monsterName[i].lst[2]:GetContentWidth() , 
                                                                self.monster.monsterName[i].layer:GetContentHeight()
                                                                )
            PosTool.Center( self.monster.monsterName[i].layer , 0 , 145 )

            --模型触摸
            self.monster.mode[i]:SetTouchFun(nil, nil, function()
                local info = {}
                info.type = i - 1
                info.contrapose = FBcnf.contrapose[i]
                info.level = v
                info.hp = campHP.max - campHP.damage
                info.max = hp
                LogicGUI.TemplUnionCombat.New( info )
            end)


        end
    end

    local section = CallModuleFunc(ModuleType.UNION , "GetNowAttackSection")
    if self.__open_data[1].section ~= section then
        local alwaysMAX = CallModuleFunc(ModuleType.UNION , "GetToSectionHp" , section )
        --获得总血量
        self.__progressbar:SetProgressMax( alwaysMAX )
        self.__progressbar:SetProgress( alwaysMAX )
        self.barText:SetString( 100 .. "%" )
    else
        --获得总血量
        local alwaysHP , alwaysMAX = CallModuleFunc(ModuleType.UNION , "GetOutsideHP")
        self.__progressbar:SetProgressMax( alwaysMAX )
        self.__progressbar:SetProgress( alwaysHP )
        
        self.barText:SetString( alwaysHP .. "%")
    end

    --名字
    self.name:SetText( string.format(Macros.Color.city_headline_rt, FBcnf.name or "0" )  )

    local red = CallModuleFunc(ModuleType.UNION , "GetMayGetAwardRed")
    if red ~= 0 then
        self.icon_red_point:SetVisible(true)
        self.union_player:SetString( CPPGameLib.GetString("tower_drawAward") )
    else
        self.icon_red_point:SetVisible(false)
        self.union_player:SetString( CPPGameLib.GetString("union_player") )
    end
end

--箱子
function Modules.UnionTranscriptTwoView:OnBox()
    print("xy","箱子")
    LogicGUI.TemplUnionTranscriptBox.New( self.__open_data[1].section )
end

--通关奖励
function Modules.UnionTranscriptTwoView:OnButtonPlayer()
    print("xy" , "通关奖励")
    LogicGUI.TemplUnionPlayerAward.New()
end

--成员战绩
function Modules.UnionTranscriptTwoView:OnButtonRecord()
    print("xy" , "成员战绩")
    LogicGUI.TemplUnionTranscriptRecord.New()
end

--重置设定
function Modules.UnionTranscriptTwoView:OnButtonReset()
    print("xy" , "重置设定")
    LogicGUI.TemplUnionTranscriptReset.New()
end

--规则说明
function Modules.UnionTranscriptTwoView:OnButtonRule()
    print("xy" , "(4)军团副本，大图标（介绍军团副本的规则）：")
    local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
    help:SetLoadedCallback( function ( ... )
       help:SetCell("Rule_Union_Ectype")
    end )
end

LogicGUI = LogicGUI or {}

LogicGUI.TemplUnionTranscriptPlayer = LogicGUI.TemplUnionTranscriptPlayer or BaseClass(GUI.Template)

function LogicGUI.TemplUnionTranscriptPlayer:__init( data ,func )
    self.__is_auto_load = true  
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__open_callback = function ()
        self.func = func
        self:__Getwidget()
        self:__ShowUI( data )
    end
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_UNION_TRANSCRIPT_PLAYER)
end

function LogicGUI.TemplUnionTranscriptPlayer:__delete()
    if self.func then
        self.func()
    end
end

function LogicGUI.TemplUnionTranscriptPlayer:__Getwidget()

    --标题
    self.Panel_effectText = self:GetWidget("Panel_effectText")

    --军团贡献Type
    self.Text_contributionType = Util:WidgetLabel(
                                                    self:GetWidget("Text_contributionType"),
                                                    CPPGameLib.GetString("union_contributionType"), 
                                                    Macros.TypefaceSize.headline,
                                                    Macros.Color.Special
                                                )

    --军团贡献Value
    self.Text_contributionValue = self:GetWidget("Text_contributionValue")

    --图标
    self.Image_icon = self:GetWidget("Image_icon")

    --军团经验Type
    self.Text_EXPType = Util:WidgetLabel(
                                            self:GetWidget("Text_EXPType"),
                                            CPPGameLib.GetString("union_ExpType"), 
                                            Macros.TypefaceSize.headline,
                                            Macros.Color.Special
                                        )

    --军团经验Value
    self.Text_EXPValue = self:GetWidget("Text_EXPValue")

    --点击屏幕继续
    self.Text_continue = Util:WidgetLabel(
                                            self:GetWidget("Text_continue"),
                                            CPPGameLib.GetString("battle_result_continue"), 
                                            Macros.TypefaceSize.headlineII,
                                            Macros.Color.btn
                                        )

    -- --排名突破至
    -- Util:WidgetLabel(self:GetWidget("Text_breakUpTo"),CPPGameLib.GetString("arena_break_rank_To"), Macros.TypefaceSize.headline,Macros.Color.Special)
    -- --排名
    -- Util:WidgetLabel(self:GetWidget("Text_breakRank"),enemy_rank,Macros.TypefaceSize.headline,Macros.Color.green)
    -- --排名上升
    -- Util:WidgetLabel(self:GetWidget("Text_rankUp"),CPPGameLib.GetString("arena_rankUp"), Macros.TypefaceSize.headline, Macros.Color.Special)
    -- --排名上升数量
    -- Util:WidgetLabel(self:GetWidget("Text_rankUpNum"),nowRank - enemy_rank,Macros.TypefaceSize.headline, Macros.Color.btn)
    -- --突破奖励
    -- Util:WidgetLabel(self:GetWidget("Text_breakEncourage"),CPPGameLib.GetString("arena_breakEncourage"), Macros.TypefaceSize.headline, Macros.Color.Special)
    -- --突破奖励数量
    -- local encourgeNum = 0
    -- history_rank = history_rank - 1
    -- for i = history_rank,enemy_rank,-1 do
    --     local first_WinData = CPPGameLib.GetConfig("ArenaFirstRankAward",i)
    --     encourgeNum = encourgeNum + first_WinData.diamond
    -- end
    -- Util:WidgetLabel(self:GetWidget("Text_breakEncourageNum"),encourgeNum, Macros.TypefaceSize.headline, Macros.Color.btn)
    -- --点击屏幕提示
    -- Util:WidgetLabel(self:GetWidget("Text_continue"),CPPGameLib.GetString("battle_result_continue"), Macros.TypefaceSize.headlineII,Macros.Color.btn)



    -- self.__delay_timer1 = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
    --     self.__delay_timer1 = nil
    --     GlobalEffectMgr:PasueEffect(effect_id)              
    --     self:PlayActionTimeline("animation0",false,function ()          
    --         self:PlayActionTimeline("animation1",true,nil)          
    --         --关闭监听
    --         WidgetUnity.SetWidgetListener(self:GetNode(), nil, nil,function ()
    --             GlobalEventSystem:FireNextFrame(Macros.Event.ArenaModule.ARENA_UPDTAE_ANIMATION)
    --             self:DeleteMe()
    --         end)
    --         --新手指引特殊操作检测(战队升级继续)
    --         CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.ARENA_RANK_BREAK_CONTINUE, self:GetWidget("Panel_Lead"))                              
    --     end)
    -- end), 0.8)

    -- self.__delay_timer2 = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
    --     self.__delay_timer2 = nil
    --     GlobalEffectMgr:PasueEffect(effect_id)              
    -- end),2) 
end

function LogicGUI.TemplUnionTranscriptPlayer:__ShowUI( data )

    --排名特效
    local effect_id,effect = self:__CreateEffect( { id = 100053 } , self.Panel_effectText )
    PosTool.Center(effect)
    
    -- 可攻打的章节
    local guildCnf = CPPGameLib.GetConfig("GuildFB", data.section)
    
    --军团贡献Value
    Util:WidgetLabel(
                        self.Text_contributionValue,
                        guildCnf.kill_contribution, 
                        Macros.TypefaceSize.headline,
                        Macros.Color.white
                    )

    --军团贡献图标
    self.Image_icon:SetImage( Util:GetMoneyIconImgData(Macros.Game.RolePropType.GUILD_CONTRIBUTION) )

    --军团经验Value
    Util:WidgetLabel(
                        self.Text_EXPValue,
                        "+" .. guildCnf.kill_exp, 
                        Macros.TypefaceSize.headline,
                        Macros.Color.green
                    )

    self:PlayActionTimeline("animation0",false,function ()          
        self:PlayActionTimeline("animation1",true,nil)          
        --关闭
        WidgetUnity.SetWidgetListener(self:GetNode(), nil, nil,function ()
            self:DeleteMe()
        end)
    end)
end