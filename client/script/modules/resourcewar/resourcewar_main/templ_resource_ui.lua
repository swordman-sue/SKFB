--
-- @brief 资源战UI层 
-- @author: yjg
-- @date: 2016年8月16日10:06:15
--

Modules = Modules or {}
Modules.TemplResourceUI = Modules.TemplResourceUI or BaseClass(GUI.Template)

function Modules.TemplResourceUI:__init( chapter )
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil) 

    self.chapter = chapter
    --左上角
    self:ShowUpLeftUI()
    --右上角
    self:ShowUpRightUI()
    --左下角
    self:ShowLowerLeftUI()
    --右下角
    self:ShowLowerRightUI()

    self:__Server()
end

function Modules.TemplResourceUI:__Server()

    CallModuleFunc(ModuleType.RESOURCE_WAR,"PlayerResourceWarDataReq" )

    --刷新碎片
    self:BindGlobalEvent(Macros.Event.ResourceWarModule.RESOURCE_WAR_DATA,function (_, data)
        --左上角
        self:SetUpLeftUI()

        local chapter = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetChapter" )
        local info = {}
        info.chapter = self.chapter or chapter
        CallModuleFunc(ModuleType.RESOURCE_WAR,"ResourceWarChapterDataReq" , info )

    end)    

    --跳转选中
    self:BindGlobalEvent(Macros.Event.ResourceWarModule.RESOURCE_CHAPTER,function (_, data)
        local atPresentChapter = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetAtPresentChapter" )
        local info = {}
        info.chapter = atPresentChapter
        self:ShowOnLowerLeftBtn( info )

        --右上角
        self:SetUpRightUI()
    end)    
end

function Modules.TemplResourceUI:__CreateWidgetFromLuaCode()
    self.widget = Util:Layout( Macros.Global.DesignSize.width, Macros.Global.DesignSize.height )
    return self.widget
end

function Modules.TemplResourceUI:__delete()
end

--左上角UI
function Modules.TemplResourceUI:ShowUpLeftUI()
    --星级商店
    local sp = Resource.PathTool.GetSystemEntryIconPath("btn_store")
    local shop = Util:Button( sp , CPPGameLib.Handler(self, self.OnShop) )
    shop:SetAnchorPoint( 0 , 1 )
    shop:SetPosition( 30 , self.widget:GetContentHeight() - 80 )
    self.widget:AddChild( shop )
    shop:SetScaleFactor( 0.8 )

    local resources_war_shop = Util:Label(
                                            CPPGameLib.GetString("resources_war_shop"),
                                            Macros.TypefaceSize.normal, 
                                            Macros.Color.btn, 0, 0,
                                            Macros.Color.white_stroke
                                            )
    shop:AddChild(resources_war_shop)
    PosTool.CenterBottom(resources_war_shop,0,10)


    local id = 1
    --占领
    local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_ICON, "icon_sp_item_bg_"..id - 1)
    self.occupy = {}
    self.occupy.bg = Util:Sprite(imgdata)
    self.occupy.bg:SetAnchorPoint( 0 , 1 )
    self.occupy.bg:SetPosition( shop:GetPositionX() + shop:GetContentWidth() + 10 , self.widget:GetContentHeight() - 90 )
    self.widget:AddChild( self.occupy.bg )
    self.occupy.bg:SetScaleFactor( 0.8 )
    self.occupy.bg:SetVisible( false )

    local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.RESOURCE_WAR, "resource_seek_"..id)
    self.occupy.content = Util:Sprite(imgdata)
    self.occupy.bg:AddChild( self.occupy.content )
    PosTool.Center( self.occupy.content )
    self.occupy.content:SetScaleFactor( 0.77 )

    local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_ICON, "icon_sp_item_"..id - 1)
    self.occupy.frame = Util:Sprite(imgdata , function( ... )
        local id = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetOccupyId" )
        Modules.TemplResourceMine.New( id )
    end )
    self.occupy.bg:AddChild( self.occupy.frame )
    PosTool.Center( self.occupy.frame )

    self.occupy.time = Util:Label(
                                    "00:00:00",
                                    Macros.TypefaceSize.normal, 
                                    Macros.Color.btn, 0, 0,
                                    Macros.Color.white_stroke
                                    )
    self.widget:AddChild(self.occupy.time , 10 )
    self.occupy.time:SetPosition( self.occupy.bg:GetPositionX() + (self.occupy.bg:GetContentWidth()/2) , self.occupy.bg:GetPositionY() - self.occupy.bg:GetContentHeight() + 15 )
    self.occupy.time:SetScaleFactor( 0.8 )

    local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.RESOURCE_WAR, "not_occupy" )
    self.occupy.none = Util:Sprite(imgdata)
    self.occupy.none:SetAnchorPoint( 0 , 1 )
    self.occupy.none:SetPosition( shop:GetPositionX() + shop:GetContentWidth() + 10 , self.widget:GetContentHeight() - 75 )
    self.widget:AddChild( self.occupy.none )
    self.occupy.none:SetScaleFactor( 0.6 )
    self.occupy.none:SetVisible( false )
end

--右上角UI
function Modules.TemplResourceUI:ShowUpRightUI()
    --资料卡
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.RESOURCE_WAR, "resource_mine_info" )
    self.resource_mine_info = Util:Sprite9( sp )
    self.resource_mine_info:SetContentSize( self.resource_mine_info:GetContentWidth() , self.resource_mine_info:GetContentHeight() - 40 )
    self.resource_mine_info:SetAnchorPoint( 1 , 1 )
    self.resource_mine_info:SetPosition( self.widget:GetContentWidth() - 10 , self.widget:GetContentHeight() - 10 )
    self.widget:AddChild( self.resource_mine_info )

    --占领军团
    self.resources_war_occupyUnion = Util:Label( CPPGameLib.GetString("resources_war_occupyUnion", CPPGameLib.GetString("rank_haveNoUnion") ) , Macros.TypefaceSize.slightly )
    self.resources_war_occupyUnion:SetAnchorPoint( 0.5 , 0.5 )
    self.resources_war_occupyUnion:SetPosition( self.resource_mine_info:GetContentWidth()/2 , self.resource_mine_info:GetContentHeight()/2 + 15  )
    self.resource_mine_info:AddChild( self.resources_war_occupyUnion )

    --占领军团
    local tab = {
                    { zi = CPPGameLib.GetString("resources_war_population") , dx = Macros.TypefaceSize.slightly  },
                    { zi = CPPGameLib.GetString("resources_war_person","0"), ys = 5 , mb = true , dx = Macros.TypefaceSize.slightly  },
                    { zi = CPPGameLib.GetString("resources_war_yield"), dx = Macros.TypefaceSize.slightly  },
                    { zi = "0%" , ys = 5 , mb = true , dx = Macros.TypefaceSize.slightly  },
                }

    self.resources_war_addition = Util:LabCenter( tab )
    self.resources_war_addition:SetAnchorPoint( 0.5 , 0.5 )
    self.resources_war_addition:SetPosition( self.resource_mine_info:GetContentWidth()/2 , self.resource_mine_info:GetContentHeight()/2 - 15  )
    self.resource_mine_info:AddChild( self.resources_war_addition )
end

--左下角UI
function Modules.TemplResourceUI:ShowLowerLeftUI()
    --翻页按钮
    local btn_select_tab = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_select_tab")

    local x = { 
                [1] = { x = 50  },
                [2] = { x = self.widget:GetContentWidth() - 50  }
             }
    for i = 1 , 2 do
        local select_tab = Util:Button( btn_select_tab , function( ... )
            self:SetSelectTab( nil , i )
        end )
        select_tab:SetAnchorPoint( 0.5 , 0.5 )
        select_tab:SetPosition( x[i].x , self.widget:GetContentHeight()/2 )
        self.widget:AddChild(select_tab) 
        if i == 1 then
            select_tab:SetFlipX( true )
        end
    end

    --跳转
    local black1_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "black1_bg")
    --按钮跳转
    local btn_skip = Util:Sprite9( black1_bg , nil )
    btn_skip:SetContentSize( 360 , 52 )
    btn_skip:SetAnchorPoint( 0 , 0 )
    btn_skip:SetPosition( 30 , 5 )
    self.widget:AddChild( btn_skip , -1)

    local ont = Util:Layout( btn_skip:GetContentWidth() , btn_skip:GetContentHeight() , function ( ... )
        -- body
    end)
    ont:SetAnchorPoint( 0.5 , 0.5 )
    btn_skip:AddChild( ont )
    PosTool.Center( ont )

    --设置按钮
    self:ShowLowerLeftBtn( btn_skip )

    --输入跳转
    local import_skip = Util:Sprite9( black1_bg )
    import_skip:SetContentSize( 295 , 40 )
    import_skip:SetAnchorPoint( 0 , 0 )
    import_skip:SetPosition( btn_skip:GetPositionX() , btn_skip:GetPositionY() + btn_skip:GetContentHeight() )
    self.widget:AddChild( import_skip , -1)

    --跳转按钮
    local resource_skip = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.RESOURCE_WAR, "resource_skip")
    local one =  Util:Button( resource_skip , function( ... )
        self:SetSelectTab( 1 )
    end , CPPGameLib.GetString("resources_war_one") , 14 ,  Macros.Color.btn , Macros.Color.white_stroke )
    one:SetAnchorPoint( 0 , 0.5 )
    one:SetPosition( 20 , import_skip:GetContentHeight()/2 - 2 )
    import_skip:AddChild( one )

    --输入框
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_num_frame" )
    local resource_mine_info = Util:Sprite9( sp )
    resource_mine_info:SetContentSize( 100 , resource_mine_info:GetContentHeight() - 5 )
    resource_mine_info:SetAnchorPoint( 0 , 0.5 )
    resource_mine_info:SetPosition( one:GetPositionX() + one:GetContentWidth() + 10 , one:GetPositionY() )
    import_skip:AddChild( resource_mine_info )

    --显示数据
    self.showText = Util:Label( "" , nil , Macros.Color.content) 
    self.showText:SetAnchorPoint( 0.5 , 0.5 )
    resource_mine_info:AddChild( self.showText )
    PosTool.Center( self.showText )

    --输入
    local TextField = UITextFieldEx.Create()
    TextField:SetMaxLength(3)
    TextField:SetMaxLengthEnabled(true)
    TextField:SetAnchorPoint(0.5, 0.5)
    TextField:SetTouchSoundInfo(Macros.Game.SystemIDForSound.COMMON, 2)
    TextField:SetContentSize( resource_mine_info:GetContentWidth() , resource_mine_info:GetContentHeight() )
    TextField:SetTouchAreaEnabled(true)
    TextField:SetTouchSize( resource_mine_info:GetContentWidth() , resource_mine_info:GetContentHeight() )
    TextField:SetTextColor( Macros.Color.content )
    resource_mine_info:AddChild( TextField )
    PosTool.Center( TextField )
    CPPActionManager.FadeOut( TextField , 0 )

    TextField:SetEventListener(
             GlobalCallbackMgr:AddCallBackFunc(function()
                print("xy" , "==================1111")
             end),
             GlobalCallbackMgr:AddCallBackFunc(function()
                print("xy" , "==================2222")
             end),
             GlobalCallbackMgr:AddCallBackFunc(function()
                self.showText:SetString( TextField:GetText() )
                print("xy" , "==================3333" , TextField:GetText() )
             end),
             GlobalCallbackMgr:AddCallBackFunc(function()
                self.showText:SetString( TextField:GetText() )
                print("xy" , "==================4444" , TextField:GetText() )
             end)
    )  

    --跳转
    local skip =  Util:Button( resource_skip , function( ... )
        self:SetSelectTab( TextField:GetText() )
    end , CPPGameLib.GetString("resources_war_skip") , 14 ,  Macros.Color.btn , Macros.Color.white_stroke )
    skip:SetAnchorPoint( 0 , 0.5 )
    skip:SetPosition( resource_mine_info:GetPositionX() + resource_mine_info:GetContentWidth() + 10 , resource_mine_info:GetPositionY() )
    import_skip:AddChild( skip )

    --末页
    local ended =  Util:Button( resource_skip , function( ... )
        self:SetSelectTab( "end" )
    end , CPPGameLib.GetString("resources_war_end") , 14 ,  Macros.Color.btn , Macros.Color.white_stroke )
    ended:SetAnchorPoint( 0 , 0.5 )
    ended:SetPosition( skip:GetPositionX() + skip:GetContentWidth() + 10 , skip:GetPositionY() )
    import_skip:AddChild( ended )
end

--左下角按钮
function Modules.TemplResourceUI:ShowLowerLeftBtn( bg )
    local params =  
    {
        item_class = Modules.TemplResourceItem,
        item_width = 72 ,
        item_height = 52 ,
        row = 1,
        col = 5,
        horizon = true ,
        view_width = 350 ,
        selectable = true,
        select_callback = function ( index , item )
            Modules.TemplResourceShade.New( item.chapter , function()
                --跳转
                local info = {}
                info.chapter = item.chapter
                CallModuleFunc(ModuleType.RESOURCE_WAR,"ResourceWarChapterDataReq" , info )
            end )
            self:ShowOnLowerLeftBtn( item )
        end
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New( params )
    bg:AddChild( self.lst_list:GetNode() )
    PosTool.Center( self.lst_list:GetNode() )

    local cnf = CPPGameLib.GetConfig("ResourceWarChapter", nil , nil , true )
    self.lst_list:SetDataList( cnf )
end

function Modules.TemplResourceUI:ShowOnLowerLeftBtn( data )
    self.lst_list:ItemIter( function( i, item , itemDate  )
        if item then
            item:SelectedIcon( false )
            if itemDate.chapter == data.chapter then
                item:SelectedIcon( true )
            end
        end
    end )
    local index = data.chapter - 1
    if data.chapter - 1 <= 0 then
        index = 1
    end
    self.lst_list:LocateItem( index )
end

--右下角UI
function Modules.TemplResourceUI:ShowLowerRightUI()
    --敌军
    local sp = Resource.PathTool.GetSystemEntryIconPath("btn_store")

    local lst = { 
                    [1] = { sc = 0.65 , sp = Resource.PathTool.GetSystemEntryIconPath("btn_activity") , name = CPPGameLib.GetString("resources_war_function_1") , id = "dijun" },
                    [2] = { sc = 0.8 , sp = Resource.PathTool.GetSystemEntryIconPath("btn_lineup") , name = CPPGameLib.GetString("resources_war_function_2") , id = "zhengrong" },
                    [3] = { sc = 0.8 , sp = Resource.PathTool.GetSystemEntryIconPath("btn_daily") , name = CPPGameLib.GetString("resources_war_function_3") , id = "zhanbao" },
                    [4] = { sc = 0.8 , sp = Resource.PathTool.GetSystemEntryIconPath("btn_handbook") , name = CPPGameLib.GetString("resources_war_function_4") , id = "keji" },
                    [5] = { sc = 0.8 , sp = Resource.PathTool.GetSystemEntryIconPath("btn_guild") , name = CPPGameLib.GetString("resources_war_function_5") , id = "juntuanziyuan" },
                    [6] = { sc = 0.65 , sp = Resource.PathTool.GetSystemEntryIconPath("btn_dungeon") , name = CPPGameLib.GetString("resources_war_function_6") , id = "sousuo" },
                }

    for i,v in ipairs( lst ) do
        local transfer = nil
        transfer = Util:Button( v.sp , function( ... )
            self:OnTransfer( v.id , transfer )
        end )
        transfer:SetAnchorPoint( 1 , 0 )
        transfer:SetPosition( self.widget:GetContentWidth() - 30 - ( (i-1) * 90 ) , 20 )
        self.widget:AddChild( transfer )
        transfer:SetScaleFactor( v.sc )

        local resources_war_shop = Util:Label(
                                                v.name ,
                                                Macros.TypefaceSize.normal, 
                                                Macros.Color.btn, 0, 0,
                                                Macros.Color.white_stroke
                                                )
        self.widget:AddChild(resources_war_shop)
        resources_war_shop:SetPosition( transfer:GetPositionX() - ( (transfer:GetContentWidth()*v.sc)  /2) , transfer:GetPositionY() + 7 )
        resources_war_shop:SetScaleFactor( 0.8 )
    end
end

--设置左上角
function Modules.TemplResourceUI:SetUpLeftUI()
    --重置已占领
    self.occupy.bg:SetVisible( false )

    CPPActionManager.StopAllActions( self.occupy.time )

    local id = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetOccupyId" )

    
    if not id or id == 0 then
        self.occupy.none:SetVisible( true )
        self.occupy.time:SetString( CPPGameLib.GetString("resources_war_Notoccupy") )
        return
    end
    self.occupy.none:SetVisible( false )
    self.occupy.bg:SetVisible( true )

    local resourceCnf = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetResourceWarResourceCnf", id )
        CPPGameLib.PrintTable("xy" , resourceCnf , "resourceCnf")  
    local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_ICON, "icon_sp_item_bg_"..resourceCnf.resource_type - 1)
    self.occupy.bg:SetImage( imgdata )
    local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.RESOURCE_WAR, "resource_seek_"..resourceCnf.resource_type)
    self.occupy.content:SetImage( imgdata )
    --倒计时
    self:Countdown()
end

--设置右上角
function Modules.TemplResourceUI:SetUpRightUI()

    local occupyUnion , index = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetOccupyUnion" )

    local unionName = CPPGameLib.GetString("rank_haveNoUnion")
    if occupyUnion and occupyUnion ~= "" then
        unionName = occupyUnion
    end

    --占领军团
    self.resources_war_occupyUnion:SetString( CPPGameLib.GetString("resources_war_occupyUnion", unionName ) )

    if self.resources_war_addition then
        self.resources_war_addition:RemoveSelf(true)
        self.resources_war_addition = nil
    end

    if occupyUnion == "" then
        index = 0
    end

    local guildProfitCnf = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetResourceWarGuildProfitCnf" , index )

    local percentage = 0
    if index ~= 0 and guildProfitCnf then
        percentage = guildProfitCnf.base_reward_add_rate
    end

    local tab = {
                    { zi = CPPGameLib.GetString("resources_war_population") , dx = Macros.TypefaceSize.slightly  },
                    { zi = CPPGameLib.GetString("resources_war_person", index ), ys = 5 , mb = true , dx = Macros.TypefaceSize.slightly  },
                    { zi = CPPGameLib.GetString("resources_war_yield"), dx = Macros.TypefaceSize.slightly  },
                    { zi = MathTool.TransProbCoeffPercentage(  percentage  ) .. "%" , ys = 5 , mb = true , dx = Macros.TypefaceSize.slightly  },
                }

    self.resources_war_addition = Util:LabCenter( tab )
    self.resources_war_addition:SetAnchorPoint( 0.5 , 0.5 )
    self.resources_war_addition:SetPosition( self.resource_mine_info:GetContentWidth()/2 , self.resource_mine_info:GetContentHeight()/2 - 15  )
    self.resource_mine_info:AddChild( self.resources_war_addition )
end

--星级商店
function Modules.TemplResourceUI:OnShop()
    print("xy" , "=======星级商店=======")
    CallModuleFunc(ModuleType.RESOURCE_WAR, "OpenView", BaseViewType.RESOURCE_SHOP )
end

--设置目标
function Modules.TemplResourceUI:SetSelectTab( target , count )
    --当前章节
    local atPresentChapter = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetAtPresentChapter" )

    local _ , ended = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetResourceWarChapterCnf" , nil , true )

    local chapter = 0
    --是不是翻页
    if count then
        --左翻
        if count == 1 then
            if atPresentChapter - 1 > 0 then
                chapter = atPresentChapter - 1
                print("xy" , "=======计算翻页=======左" , atPresentChapter - 1 )
            else
                GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("resources_war_arriveBegin"))
                return 
            end
        --右翻
        elseif count == 2 then
            if atPresentChapter + 1 <= ended then
                chapter = atPresentChapter + 1
                print("xy" , "=======计算翻页=======右" , atPresentChapter + 1 )
            else
                GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("resources_war_arriveEnd"))
                return
            end     
        end
    end


    if not count then
        --有没有目标
        if target ~= "end" and not tonumber(target) then
            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("resources_war_Notskip"))
            return
        elseif target == "end" then
            chapter = ended
        else
            chapter = tonumber(target)
        end
    end

    if chapter >= ended then
        chapter = ended
    end

    Modules.TemplResourceShade.New( chapter , function()
        if chapter ~= 0 then
            --跳转
            local info = {}
            info.chapter = chapter
            CallModuleFunc(ModuleType.RESOURCE_WAR,"ResourceWarChapterDataReq" , info )
        end
    end )
end

--功能中转
function Modules.TemplResourceUI:OnTransfer( id , btn )
    if id == "dijun" then
        self:OnEnemy()
    elseif id == "zhengrong" then
        print("xy" , "=========阵容")
        self:OnCast()
    elseif id == "zhanbao" then
        print("xy" , "=========战报")
        self:OnShark()
    elseif id == "keji" then
        print("xy" , "=========科技")
        self:OnCnTech()
    elseif id == "juntuanziyuan" then
        print("xy" , "=========军团资源")
        self:OnUnion()
    elseif id == "sousuo" then
        print("xy" , "=========搜索")
        self:OnSeek( btn )
    end
end

--选中敌人
function Modules.TemplResourceUI:OnEnemy( )
    Modules.TemplResourceEnemy.New()
end

--选中阵容
function Modules.TemplResourceUI:OnCast( )
    GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.HERO_LINEUP, {BaseViewSubType.HeroLineup.MAIN, nil , nil , nil , true})
end

--选中战报
function Modules.TemplResourceUI:OnShark( )
    Modules.TemplResourceShark.New()
end

--选中科技
function Modules.TemplResourceUI:OnCnTech( )
    Modules.TemplResourceTech.New()
end

--选中军团资源
function Modules.TemplResourceUI:OnUnion( )
    local role_info = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo")
    if role_info.guild_id ~= 0 then
        Modules.TemplResourceUnion.New()
    else
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("rank_haveNoUnion"))
    end
end

--选中搜索
function Modules.TemplResourceUI:OnSeek( btn )
    print("xy" , "=========btn1" , btn )
    Modules.TemplResourceSeek.New( btn )
end

--倒计时
function Modules.TemplResourceUI:Countdown( )
    if not self.occupy or not self.occupy.time then
        return 
    end
    CPPActionManager.StopAllActions( self.occupy.time )
        
    local overTime = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetOccupyOverTime" )
    if not overTime or overTime == 0 then
        return
    end

    local func = nil
    local function moveAnimate( ... )
        local severTime = CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime")
        if overTime - severTime <= 0 then
            CallModuleFunc(ModuleType.RESOURCE_WAR,"PlayerResourceWarDataReq" )
        else
            self.occupy.time:SetString( TimeTool.TransTimeStamp( "HH:MM:SS" , overTime - severTime ) )
            CPPActionManager.DelayTo( self.occupy.time , 1 , func)
        end
    end
    func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
    moveAnimate()
end


Modules = Modules or {}

Modules.TemplResourceItem = Modules.TemplResourceItem or BaseClass(GUI.ScrollItem)

function Modules.TemplResourceItem:__init(data)
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplResourceItem:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout( 72 , 52 )
    return self.layout
end

function Modules.TemplResourceItem:SetData( data )
    GUI.ScrollItem.SetData(self, data)

    self:__Getwidget( data )
    -- self:__ShowUI()
end

function Modules.TemplResourceItem:__Getwidget( data )
    local resource_tab_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.RESOURCE_WAR, "resource_tab")
    local resource_tab =  Util:Sprite( resource_tab_sp , function( ... )
        -- self:OnResourceTab( data , true )
    end , false )
    resource_tab:SetAnchorPoint( 0.5 , 0.5 )
    self.layout:AddChild( resource_tab )
    PosTool.Center( resource_tab )
    -- resource_tab:SetTouchSwallow(false)

    local text = Util:Label( data.chapter )
    text:SetAnchorPoint( 0.5 , 0.5 )
    self.layout:AddChild( text )
    PosTool.Center( text )

    local resource_tab_ray_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.RESOURCE_WAR, "resource_tab_ray")
    self.resource_tab_ray_sp = Util:Sprite( resource_tab_ray_sp )
    self.resource_tab_ray_sp:SetAnchorPoint( 0.5 , 0.5 )
    self.layout:AddChild( self.resource_tab_ray_sp )
    PosTool.Center( self.resource_tab_ray_sp )

    local atPresentChapter = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetAtPresentChapter" )
    if atPresentChapter == data.chapter  then
        self:SelectedIcon( true )
    else
        self:SelectedIcon( false )
    end
end

function Modules.TemplResourceItem:SelectedIcon( type_ )
    self.resource_tab_ray_sp:SetVisible( type_ )
end









Modules = Modules or {}
Modules.TemplResourceShade = Modules.TemplResourceShade or BaseClass(GUI.Template)

function Modules.TemplResourceShade:__init( chapter , func )
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  

    self:__SetShowUI( chapter , func )
end

function Modules.TemplResourceShade:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height , function( ... )
    end)
    return self.layout
end

function Modules.TemplResourceShade:__SetShowUI( chapter , func )

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.RESOURCE_WAR, "resource_pageTurn")
    local resource_pageTurn = Util:Sprite( sp )

    --背景
    local layer = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height)
    layer:SetAnchorPoint( 0.5 , 0.5 )
    layer:SetPosition( self.layout:GetContentWidth()/2 , self.layout:GetContentHeight()/2 )
    layer:SetVisible(false)
    self.layout:AddChild( layer )

    --方向
    resource_pageTurn:SetAnchorPoint( 0.5 , 0.5 )
    resource_pageTurn:SetPosition( layer:GetContentWidth()/2 , layer:GetContentHeight()/2 )
    layer:AddChild( resource_pageTurn )
    resource_pageTurn:SetScaleFactor( 2 )

    --黑底 接续方向
    local black = Util:Layout( resource_pageTurn:GetContentWidth() , resource_pageTurn:GetContentHeight() )
    black:SetAnchorPoint( 0 , 0.5 )
    black:SetPosition( resource_pageTurn:GetContentWidth() , resource_pageTurn:GetContentHeight()/2 )
    resource_pageTurn:AddChild( black )
    black:SetBackGroundColor(0, 0, 0)
    black:SetBackGroundColorType(1)
    black:SetBackGroundColorOpacity(255)    


    local atPresentChapter = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetAtPresentChapter" )
    local function SetStop( ... )
        if func then
            func()
        end
        CPPActionManager.DelayTo( layer , 0.2 , GlobalCallbackMgr:AddCallBackFunc( function( ... )
            CPPActionManager.FadeOut( layer , 0.2 , GlobalCallbackMgr:AddCallBackFunc( function( ... )
                self:DeleteMe()
            end ) )
        end ) )
    end

    --如果当前小于目标
    if atPresentChapter < chapter then
        layer:SetPosition( 0 - layer:GetContentWidth() , layer:GetPositionY() )
        layer:SetFlipX(true)
        layer:SetVisible(true)
        CPPActionManager.MoveTo( layer , 0.6 , self.layout:GetContentWidth()/2 + black:GetContentWidth() , self.layout:GetContentHeight()/2 , GlobalCallbackMgr:AddCallBackFunc( function( ... )
            SetStop()
        end ) )
    else
        layer:SetPosition( self.layout:GetContentWidth() + layer:GetContentWidth() , layer:GetPositionY() )
        layer:SetVisible(true)
        CPPActionManager.MoveTo( layer , 0.6 , self.layout:GetContentWidth()/2 - black:GetContentWidth() , self.layout:GetContentHeight()/2 , GlobalCallbackMgr:AddCallBackFunc( function( ... )
            SetStop()
        end ) )
    end
end