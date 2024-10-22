
--[[
%% @module: 大厅
%% @author: yjg
%% @created: 2016年11月5日16:58:56
--]]

Modules = Modules or {}

Modules.TemplUnionHallHall = Modules.TemplUnionHallHall or BaseClass(BaseTabView)

function Modules.TemplUnionHallHall:__init()
end

--选项卡界面创建完成回调
function Modules.TemplUnionHallHall:__LoadedCallback()
    self:__Getwidget()
    self:__ShowUI()
end

--选项卡界面打开回调
function Modules.TemplUnionHallHall:__OpenCallback()
    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_HALL_DATE_UP,function (_, data)
        self:__SetShowUI()
    end)
    self:__SetShowUI()
end

--选项卡界面关闭回调
function Modules.TemplUnionHallHall:__CloseCallback()

end

--选项卡界面销毁回调(释放非托管资源)
function Modules.TemplUnionHallHall:__Dispose()

end

--读取组件
function Modules.TemplUnionHallHall:__Getwidget()
    --旗子
    self.Image_flag = self:GetWidget("Image_flag")
    --等级背景
    self.Image_levelBg = self:GetWidget("Image_levelBg")
    --图标背景
    self.Panel_iconBg = self:GetWidget("Panel_iconBg")
    --换头像按钮
    self.Button_change = Util:WidgetButton( 
                                            self:GetWidget("Button_change") , 
                                            CPPGameLib.GetString("union_changeIcon") , 
                                            CPPGameLib.Handler(self,self.OnButtonChange) ,
                                            nil , nil , Macros.Color.button_yellow 
                                        )

    --公告字样
    self.Image_notice = self:GetWidget("Image_notice")
    self.Image_notice:IgnoreContentAdaptWithSize(true)

    --公告显示背景
    self.Image_noticeShow = self:GetWidget("Image_noticeShow")
    local layou = Util:Layout(self.Image_noticeShow:GetContentWidth(), self.Image_noticeShow:GetContentHeight(), function ( ... )
        self:OnButtonNotice()
    end)
    layou:SetAnchorPoint(0.5,0.5)
    self.Image_noticeShow:AddChild( layou , 999 )
    PosTool.Center(layou)

    --公告编写按钮
    self.Button_notice = Util:WidgetButton( 
                                            self:GetWidget("Button_notice") , 
                                            nil , CPPGameLib.Handler(self,self.OnButtonNotice)
                                        )

    --宣言字样
    self.Image_manifesto = self:GetWidget("Image_manifesto")
    self.Image_manifesto:IgnoreContentAdaptWithSize(true)
    
    --宣言显示背景
    self.Image_manifestoShow = self:GetWidget("Image_manifestoShow")
    local layou = Util:Layout(self.Image_manifestoShow:GetContentWidth(), self.Image_manifestoShow:GetContentHeight(), function ( ... )
        self:OnButtonManifesto()
    end)
    layou:SetAnchorPoint(0.5,0.5)
    self.Image_manifestoShow:AddChild( layou , 999)
    PosTool.Center(layou)

    --宣言编写按钮
    self.Button_manifesto = Util:WidgetButton( 
                                            self:GetWidget("Button_manifesto") , 
                                            nil , CPPGameLib.Handler(self,self.OnButtonManifesto)
                                        )

    --解散工会按钮
    self.Button_dissolve = Util:WidgetButton( 
                                            self:GetWidget("Button_dissolve") , 
                                            CPPGameLib.GetString("union_dissolve") , 
                                            CPPGameLib.Handler(self,self.OnButtonDissolve) ,
                                            nil , nil , Macros.Color.button_red
                                        )
    --入会审核按钮
    self.Button_audit = Util:WidgetButton( 
                                            self:GetWidget("Button_audit") , 
                                            CPPGameLib.GetString("union_audit") , 
                                            CPPGameLib.Handler(self,self.OnButtonAudit) ,
                                            nil , nil , Macros.Color.button_yellow
                                        )

    local Button_rule = Util:WidgetButton( self:GetWidget("Button_rule") , nil , function ( ... )
        print("xy" , "(1)议事厅，小图标（用于介绍军团各职务以及军团大体规则）：" )
        local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
        help:SetLoadedCallback( function ( ... )
           help:SetCell("Rule_Union_Hall")
        end )
    end )
    Button_rule:SetZOrder(100)
end

--创建组件
function Modules.TemplUnionHallHall:__ShowUI()
    --名字
    local guild_name = Util:Label( CallModuleFunc(ModuleType.UNION , "GetUnionName") , Macros.TypefaceSize.largish , Macros.Color.Special )
    guild_name:SetAnchorPoint( 0.5 , 0.5 )
    self.Image_flag:AddChild( guild_name )
    PosTool.Center( guild_name , 0 , 205 )

    --图标
    local iconSp = "union_icon" .. CallModuleFunc(ModuleType.UNION , "GetUnionIcon")
    self.icon = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION , iconSp ) )
    self.icon:SetAnchorPoint(0.5,0.5)
    self.Panel_iconBg:AddChild( self.icon )
    PosTool.Center( self.icon )

    --等级
    local tab = {
                    { zi = CPPGameLib.GetString("common_lev10"), dx = Macros.TypefaceSize.normal , ys = Macros.Color.Special },
                    { zi = CallModuleFunc(ModuleType.UNION , "GetUnionLevel") , ys = Macros.Color.white , dx = Macros.TypefaceSize.normal , jg = 2 },
                }
    local level = Util:LabCenter(tab)
    level:SetAnchorPoint(0.5,0.5)
    self.Image_levelBg:AddChild( level )
    PosTool.Center( level )

    --经验
    local exp , expMax = CallModuleFunc(ModuleType.UNION , "GetUnionExp")
    --经验条
    local imgdata_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_preview")
    local prog_bar_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_bar_bg")
    local __hero_hp_bar_bg = GUI.ProgressBar9Layer.New(  
                                                        imgdata_bar , 
                                                        prog_bar_bg , 
                                                        180 , 17 , 184 , 21 , 50 , 100
                                                    )
    __hero_hp_bar_bg:SetAnchorPoint(0.5, 0.5)
    self.Image_flag:AddChild(__hero_hp_bar_bg:GetNode())
    PosTool.Center(__hero_hp_bar_bg , 0 , -20 )
    __hero_hp_bar_bg:SetProgress( (exp/expMax) * 100 )

    --经验字样
    local expText = Util:Label( exp .. "/" .. expMax , Macros.TypefaceSize.slightly , Macros.Color.white )
    expText:SetAnchorPoint(0.5, 0.5)
    expText:SetPosition( __hero_hp_bar_bg:GetPositionX() , __hero_hp_bar_bg:GetPositionY() - 2 )
    self.Image_flag:AddChild(expText)

    --团长
    local union_leader = {
                    { zi = CPPGameLib.GetString("union_leader"), dx = Macros.TypefaceSize.normal , ys = Macros.Color.Special },
                    { zi = CallModuleFunc(ModuleType.UNION , "GetUnionMaster") , ys = Macros.Color.white , dx = Macros.TypefaceSize.normal , jg = 2 },
                }
    local master
    master , self.masterLst = Util:LabCenter(union_leader)
    master:SetAnchorPoint( 0 , 0.5 )
    master:SetPosition( __hero_hp_bar_bg:GetPositionX() - ( __hero_hp_bar_bg:GetNode():GetContentWidth()/2 ) , __hero_hp_bar_bg:GetPositionY() - master:GetContentHeight() - 10 )
    self.Image_flag:AddChild( master )

    --成员
    local memberNum , memberNumMax = CallModuleFunc(ModuleType.UNION , "GetUnionMemberNum")
    local union_leader  = {
                                { zi = CPPGameLib.GetString("union_num"), dx = Macros.TypefaceSize.normal , ys = Macros.Color.Special },
                                { zi = memberNum .. "/" .. memberNumMax , ys = Macros.Color.white , dx = Macros.TypefaceSize.normal , jg = 2 },
                            }
    local member = nil
    member , self.union_leader = Util:LabCenter(union_leader)
    member:SetAnchorPoint( 0 , 0.5 )
    member:SetPosition( master:GetPositionX() , master:GetPositionY() - member:GetContentHeight() - 10 )
    self.Image_flag:AddChild( member )

    --公告
    local text = " "
    self.notice = Util:RichText( string.format( Macros.Color.content_rt , text ) , Macros.TypefaceSize.largish , self.Image_noticeShow:GetContentWidth() - 15 )
    self.notice:SetAnchorPoint( 0 , 1 )
    self.notice:SetPosition( 10 , self.Image_noticeShow:GetContentHeight() - (self.notice:GetContentHeight()/3) )
    self.Image_noticeShow:AddChild( self.notice )

    --宣言
    local text = " "
    self.manifesto = Util:RichText( string.format( Macros.Color.content_rt , text ) , Macros.TypefaceSize.largish , self.Image_manifestoShow:GetContentWidth() - 15 )
    self.manifesto:SetAnchorPoint( 0 , 1 )
    self.manifesto:SetPosition( 10 , self.Image_manifestoShow:GetContentHeight() - (self.manifesto:GetContentHeight()/3) )
    self.Image_manifestoShow:AddChild( self.manifesto )

    --红点
    self.red = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point") )
    self.red:SetAnchorPoint( 0.5 , 0.5 )
    self.red:SetPosition( self.Button_audit:GetContentWidth() , self.Button_audit:GetContentHeight() - 10 )
    self.red:SetVisible( false )
    self.Button_audit:AddChild( self.red )

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp_bag_num" )
    local bag_num = Util:Sprite( sp )
    bag_num:SetAnchorPoint( 1 , 0.5 )
    bag_num:SetPosition( self.Image_noticeShow:GetContentWidth() + 12 , self.Image_noticeShow:GetContentHeight() + 90 )
    self.Image_noticeShow:AddChild( bag_num , 1 )

    local active = CallModuleFunc(ModuleType.UNION , "GetMyActive")
    local union_combat_liveness = Util:Label( CPPGameLib.GetString("union_combat_liveness" , active ) , Macros.TypefaceSize.slightly )
    union_combat_liveness:SetAnchorPoint( 0 , 0.5 )
    union_combat_liveness:SetPosition( 10 , bag_num:GetContentHeight()/2 )
    bag_num:AddChild( union_combat_liveness )
end

--刷新界面
function Modules.TemplUnionHallHall:__SetShowUI()
    --图标
    local iconSp = "union_icon" .. CallModuleFunc(ModuleType.UNION , "GetUnionIcon")
    self.icon:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION , iconSp ) )

    local notice = CallModuleFunc(ModuleType.UNION , "GetUnionNotice")
    if CallModuleFunc(ModuleType.UNION , "GetUnionNotice") == "" then
        notice = CPPGameLib.GetString("union_nonotice")
    end

    local color = Macros.Color.content_rt
    self.notice :SetText( string.format( color , notice ) )

    local manifesto = CallModuleFunc(ModuleType.UNION , "GetUnionDeclaration")
    if CallModuleFunc(ModuleType.UNION , "GetUnionDeclaration") == "" then
        manifesto = CPPGameLib.GetString("union_nomanifesto")
    end
    self.manifesto :SetText( string.format( color , manifesto ) )

    --解散
    local type_ = CallModuleFunc(ModuleType.UNION , "GetMyPermission" , Macros.Game.UNION.DISSOLVE )
    if type_ ~= 1 then
        self.Button_dissolve:SetVisible(false) 
    else
        self.Button_dissolve:SetVisible(true) 
    end

    --审核
    local type_ = CallModuleFunc(ModuleType.UNION , "GetMyPermission" , Macros.Game.UNION.APPLY )
    if type_ ~= 1 then
        self.Button_audit:SetVisible(false) 
    else
        self.Button_audit:SetVisible(true) 
    end

    --会长刷新
    self.masterLst[2]:SetString( CallModuleFunc(ModuleType.UNION , "GetUnionMaster") )

        print("xy" , "============================111")
        --成员数量
        local memberNum , memberNumMax = CallModuleFunc(ModuleType.UNION , "GetUnionMemberNum")
        self.union_leader[2]:SetString( memberNum .. "/" .. memberNumMax )

    self:SetRed()
end

--更换头像
function Modules.TemplUnionHallHall:OnButtonChange()
    print("xy" , "-----------更换头像")
    if CallModuleFunc(ModuleType.UNION , "GetMyPermission" , Macros.Game.UNION.ICON ) ~= 0 then
        LogicGUI.TemplUnionWall.New()
    else
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_noRights") )
    end
end

--公告更改
function Modules.TemplUnionHallHall:OnButtonNotice()
    print("xy" , "-----------公告更改")
    if CallModuleFunc(ModuleType.UNION , "GetMyPermission" , Macros.Game.UNION.NOTICE ) ~= 0 then
        local tips = LogicGUI.TemplUnionImport.New( function ( text )
            local info = {}
            info.type = Macros.Game.UNION.NOTICE
            info.text = text
            CallModuleFunc(ModuleType.UNION , "GuildInfoModifyReq",info)
        end )

        tips:SetLoadedCallback( function ( ... )
            tips:SetName( CPPGameLib.GetString("union_notice") )
        end )

    else
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_noRights")  )
    end
end

--宣言更改
function Modules.TemplUnionHallHall:OnButtonManifesto()

    if CallModuleFunc(ModuleType.UNION , "GetMyPermission" , Macros.Game.UNION.MANIFESTO ) ~= 0 then
        local tips = LogicGUI.TemplUnionImport.New( function ( text )
            local info = {}
            info.type = Macros.Game.UNION.MANIFESTO
            info.text = text
            CallModuleFunc(ModuleType.UNION , "GuildInfoModifyReq",info)
        end )

        tips:SetLoadedCallback( function ( ... )
            tips:SetName( CPPGameLib.GetString("union_manifesto_") )
        end )

    else
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_noRights") )
    end

end

--解散按钮
function Modules.TemplUnionHallHall:OnButtonDissolve()
    print("xy" , "-----------解散按钮")
    if CallModuleFunc(ModuleType.UNION , "GetMyPermission" , Macros.Game.UNION.DISSOLVE ) == 0 then
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_noRights") )
        return
    end

    local type_ = CallModuleFunc( ModuleType.UNION , "GetGuildWarVSActivityTimeCnf" )
    local duiZhanKey = false
    for i , v in ipairs( type_ ) do
        if v == "duiZhan" then
            duiZhanKey = true
        end
    end

    local unionLevel = CallModuleFunc(ModuleType.UNION , "GetUnionLevel")
    local cnf = CPPGameLib.GetConfig( "GuildBuilding", 6 )
    if duiZhanKey then
        if unionLevel >= cnf.need_guild_level then
            GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_combat_notdissolve") )
            return
        end
    end

    local function dissolveTips2( ... )
        LogicGUI.TemplAffirmTips.New( { 
                                        type = LogicGUI.TemplAffirmTipsType.YES_NO,
                                        content = CPPGameLib.GetString("union_dissolveTips__") ,
                                        cancel_text = CPPGameLib.GetString("common_cancel") ,
                                        confirm_func = function ( ... )
                                            CallModuleFunc(ModuleType.UNION , "GuildDismissReq")
                                            --指引中，关闭主界面外的所有界面
                                            GlobalViewMgr:CloseViewsExcludeCity()
                                        end,
                                        cancel_func = function ( ... )
                                        end,
                                    } )
    end

    local function dissolveTips1( ... )
        LogicGUI.TemplAffirmTips.New( { 
                                        type = LogicGUI.TemplAffirmTipsType.YES_NO,
                                        content = CPPGameLib.GetString("union_dissolveTips_") ,
                                        cancel_text = CPPGameLib.GetString("common_cancel") ,
                                        confirm_func = function ( ... )
                                            dissolveTips2()
                                        end,
                                        cancel_func = function ( ... )
                                        end,
                                    } )
    end

    LogicGUI.TemplAffirmTips.New( { 
                                    type = LogicGUI.TemplAffirmTipsType.YES_NO,
                                    content = CPPGameLib.GetString("union_dissolveTips") ,
                                    cancel_text = CPPGameLib.GetString("common_cancel") ,
                                    confirm_func = function ( ... )
                                        dissolveTips1()
                                    end,
                                    cancel_func = function ( ... )
                                    end,
                                } )
end

--申请按钮
function Modules.TemplUnionHallHall:OnButtonAudit()
    print("xy" , "-----------申请按钮")
    LogicGUI.TemplUnionAudit.New()
end

--设置红点
function Modules.TemplUnionHallHall:SetRed()
    local lst = CallModuleFunc(ModuleType.UNION, "GetUnionRedLst" )
    if lst[ Macros.Game.UNION_RED.AUDIT ] then
        self.red:SetVisible( true )
    else
        self.red:SetVisible( false )
    end
end