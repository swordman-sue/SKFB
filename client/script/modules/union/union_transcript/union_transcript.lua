--
-- @brief 公会副本
-- @author: yjg
-- @date: 2017年5月18日10:43:55
--

Modules = Modules or {}

Modules.UnionTranscriptView = Modules.UnionTranscriptView or BaseClass(BaseView)
function Modules.UnionTranscriptView:__init()
    self.__layout_name = "union_transcript"  
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bp_sp_union_dungeon")}
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

function Modules.UnionTranscriptView:__OpenCallback()    
    self:__Getwidget()

    self:__ShowUI()
    self:__Server()
end

--数据
function Modules.UnionTranscriptView:__Server()
    CallModuleFunc(ModuleType.UNION , "GuildFbDataReq")
    CallModuleFunc(ModuleType.UNION , "GuildMonsterDataReq")
    CallModuleFunc(ModuleType.UNION , "GuildMyMapRewardRecordReq")
    --一层
    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_FB_AWARD , function (_, data)
        self:__Transfer( false )
    end)

    --数据刷新
    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_FB , function (_, data)
        self:__SetShowUI()
    end)


    --奖励刷新
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
end

function Modules.UnionTranscriptView:__Getwidget( ... )
    --次数背景
    self.Image_timeBg = self:GetWidget("Image_timeBg")
    --增加按钮
    self.Button_5 = Util:WidgetButton( 
                                            self:GetWidget("Button_5"),
                                            "",
                                            function( ... )
                                                -- 剩余次数
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
function Modules.UnionTranscriptView:__Transfer( type_ )
    -- self:ResetBG( {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bp_sp_union_dungeon")} )
    self.Panel_one:SetVisible(true)
    self:__SetShowUI()
    self:__SetSelectShowUI()
end

-- GetContentHeight
function Modules.UnionTranscriptView:__ShowUI( ... )
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

    --第一层
    --关卡章节
    local item_w = 214
    local item_s_c = 10
    local params = 
    {
        item_class = Modules.TemplUnionTranscriptItem,
        item_width = item_w,
        item_height = 600,
        row = 1,
        col = 5,
        item_space_c = item_s_c,
        view_width = self.Panel_Chapters:GetContentWidth() , 
        horizon = true,
        selectable = true,
        select_callback = function(item_index, item_data)
            local item = self.__chapters_scrollview:GetItem(item_index)
            if not item then
                return
            end

            --章节
            local section = CallModuleFunc(ModuleType.UNION , "GetNowAttackSection")
            if item_index > section then
                print("xy" , "还没开")
                return 
            end
            --时间类型
            local timeType = CallModuleFunc(ModuleType.UNION , "GetStartTime")
            if timeType == false then
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_not_activity_time") )
                return 
            end

            --今天开始的章节
            local startSection = CallModuleFunc(ModuleType.UNION , "GetTodayStartSection")
            if item_index < startSection  then
                print("xy" , "打过了")
                return     
            end

            --设置当前打开章节          
            CPPActionManager.ScaleTo(item:GetNode(), 0.25 , 1.2 , GlobalCallbackMgr:AddCallBackFunc(function()
                local info = {}
                info.section = item_index
                CallModuleFunc(ModuleType.UNION, "OpenView", BaseViewType.UNION_TRANSCRIPT_TWO , info )
            end))                       
        end,
    }
    self.__chapters_scrollview = GUI.SpecialScrollView.New(params)
    self.Panel_Chapters:AddChild(self.__chapters_scrollview:GetNode())
    PosTool.Center(self.__chapters_scrollview , 0 , 0 )      
    self.__chapters_scrollview:SetNecessaryData(1)

end

--设置ui
function Modules.UnionTranscriptView:__SetShowUI( ... )
    local chapter_show_list = CallModuleFunc(ModuleType.UNION , "GetUnionTranscriptCnf")
    self.__chapters_scrollview:SetDataList(chapter_show_list)
    
    local section = CallModuleFunc(ModuleType.UNION , "GetNowAttackSection")
    self.__chapters_scrollview:SetNecessaryData(section)

    --将索引为Index的条目移动到屏幕中心
    self.__chapters_scrollview:MoveItemToScreenCenterByIndex(section)  

    local red = CallModuleFunc(ModuleType.UNION , "GetMayGetAwardRed")
    if red ~= 0 then
        self.icon_red_point:SetVisible(true)
        self.union_player:SetString( CPPGameLib.GetString("tower_drawAward") )
    else
        self.icon_red_point:SetVisible(false)
        self.union_player:SetString( CPPGameLib.GetString("union_player") )
    end

    --今日已经挑战次数
    self.union_threats:SetString( CPPGameLib.GetString("union_threats") .. CallModuleFunc(ModuleType.UNION , "GetNowChallengeTime") ) 
end

--设置选择ui层
function Modules.UnionTranscriptView:__SetSelectShowUI( ... )
    --设置现在要打的章节
    local section = CallModuleFunc(ModuleType.UNION , "GetNowAttackSection")
    --将索引为Index的条目移动到屏幕中心
    self.__chapters_scrollview:MoveItemToScreenCenterByIndex( section ) 
    self.__chapters_scrollview:SelectItem( section )  
end

--箱子
function Modules.UnionTranscriptView:OnBox()
    print("xy","箱子")
end

--通关奖励
function Modules.UnionTranscriptView:OnButtonPlayer()
    print("xy" , "通关奖励")
    LogicGUI.TemplUnionPlayerAward.New()
end

--成员战绩
function Modules.UnionTranscriptView:OnButtonRecord()
    print("xy" , "成员战绩")
    LogicGUI.TemplUnionTranscriptRecord.New()
end

--重置设定
function Modules.UnionTranscriptView:OnButtonReset()
    print("xy" , "重置设定")
    LogicGUI.TemplUnionTranscriptReset.New()
end

--规则说明
function Modules.UnionTranscriptView:OnButtonRule()
    print("xy" , "(4)军团副本，大图标（介绍军团副本的规则）：")
    local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
    help:SetLoadedCallback( function ( ... )
       help:SetCell("Rule_Union_Ectype")
    end )
end


