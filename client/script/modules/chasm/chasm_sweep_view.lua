--[[
%% @module: 无尽深渊扫荡
%% @author: yjg
%% @created: 2017年7月17日16:22:35
--]]

Modules = Modules or {} 
Modules.ChasmSweepView = Modules.ChasmSweepView or BaseClass(BaseView)

function Modules.ChasmSweepView:__init()
    self.__layout_name = "chasm"  
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__title_bar_params = 
    {
        -- title = CPPGameLib.GetString("pack"),
        -- show_diamond = true,
        -- show_gold = true,
        -- show_stamina = true,
        -- show_energy = true,
        -- close_func = function()
        --     self:CloseManual()
        -- end,
    }

    self.__open_callback = function()
      self:__OpenCallback()
    end
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.CHASM)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.CHASM,true)
end

function Modules.ChasmSweepView:__OpenCallback( )

    self.time = 0
    self.numOpen = true
    self:__Getwidget()
    self:__SetPuppetStage()
    self:__ShowUI()
end	

function Modules.ChasmSweepView:__Dispose()
    if self.wujinsaodang_1 then
        self:__DelEffect(self.wujinsaodang_1)
        self.wujinsaodang_1 = nil
    end
    if self.wujinsaodang_2 then
        self:__DelEffect(self.wujinsaodang_2)
        self.wujinsaodang_2 = nil
    end
    if self.wujinchuansong_1 then
        self:__DelEffect(self.wujinchuansong_1)
        self.wujinchuansong_1 = nil
    end
end

function Modules.ChasmSweepView:__Getwidget( )
    --后台
    self.Panel_bottom = self:GetWidget("Panel_bottom") 

    --前台
    self.Panel_ui = self:GetWidget("Panel_ui") 

    self.Button_rule = self:GetWidget("Button_rule")
    self.Button_rule:SetVisible( false )
    
    self.Image_acquisition = self:GetWidget("Image_acquisition")
    self.Image_acquisition:SetVisible( false )

    --本次获得
    self.Image_acquireBg = self:GetWidget("Image_acquireBg")
    -- self.Image_acquireBg:SetVisible( false )
end 

function Modules.ChasmSweepView:__ShowUI( )

    local chasm_sweep_tips_1 = Util:Label( CPPGameLib.GetString("chasm_sweep_tips_1") , Macros.TypefaceSize.largish , Macros.Color.white )
    chasm_sweep_tips_1:SetAnchorPoint( 0.5 , 0.5 )
    chasm_sweep_tips_1:SetPosition( self.Panel_ui:GetContentWidth()/2 , self.Panel_ui:GetContentHeight()/2 + 240 )
    self.Panel_ui:AddChild( chasm_sweep_tips_1 )

    local wave = CallModuleFunc(ModuleType.CHASM, "GetHistoryWaveRecord")
    local init_wave = CPPGameLib.GetConfig("EndlessLandWave", wave )
    local chasm_sweep_tips_2 = Util:Label( CPPGameLib.GetString("chasm_sweep_tips_2" , init_wave.init_wave ) , Macros.TypefaceSize.largish , Macros.Color.white )
    chasm_sweep_tips_2:SetAnchorPoint( 0.5 , 0.5 )
    chasm_sweep_tips_2:SetPosition( self.Panel_ui:GetContentWidth()/2 , chasm_sweep_tips_1:GetPositionY() - chasm_sweep_tips_1:GetContentHeight() )
    self.Panel_ui:AddChild( chasm_sweep_tips_2 )   

    --击杀背景
    local sp_shasm_kill_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHASM, "sp_chasm_kill")
    self.sp_shasm_kill = Util:Sprite( sp_shasm_kill_sp )
    self.sp_shasm_kill:SetAnchorPoint( 0.5 , 0.5 )
    self.Panel_ui:AddChild(self.sp_shasm_kill)   
    PosTool.Center( self.sp_shasm_kill , 280 , 190 )
    self.sp_shasm_kill:SetVisible( false )

    --击杀1
    local sp_shasm_kill_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHASM, "sp_chasm_kill2" , true )
    local sp_chasm_kill2 = Util:Sprite( sp_shasm_kill_sp )
    sp_chasm_kill2:SetAnchorPoint( 0.5 , 0.5 )
    sp_chasm_kill2:SetPosition(  self.sp_shasm_kill:GetContentWidth()/2 + 90 , self.sp_shasm_kill:GetContentHeight()/2 - 34 )
    self.sp_shasm_kill:AddChild(sp_chasm_kill2) 
    CPPActionManager.RotateBy(sp_chasm_kill2, 0 , -10 )

    --击杀2
    local sp_shasm_kill_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHASM, "sp_chasm_kill1" , true )
    local sp_chasm_kill1 = Util:Sprite( sp_shasm_kill_sp )
    sp_chasm_kill1:SetAnchorPoint( 0.5 , 0.5 )
    sp_chasm_kill1:SetPosition(  self.sp_shasm_kill:GetContentWidth()/2 + 36 , self.sp_shasm_kill:GetContentHeight()/2 + 61 )
    self.sp_shasm_kill:AddChild(sp_chasm_kill1) 
    CPPActionManager.RotateBy(sp_chasm_kill1, 0 , -10 )

    --击杀数
    local battle_num = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "chasm_kill", true)
    self.kill_num = GUI.ImgFont.New(battle_num)
    self.kill_num:SetText(tostring(0))
    self.kill_num:SetAnchorPoint( 0.5 , 0.5 )
    self.sp_shasm_kill:AddChild(self.kill_num:GetNode()) 
    PosTool.Center( self.kill_num )
    CPPActionManager.RotateBy(self.kill_num:GetNode(), 0 , -10 )

    local sp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.ENDLESS_STORE)
    local icon = Util:Sprite( sp )
    icon:SetAnchorPoint( 1 , 0.5 )
    icon:SetPosition( self.Image_acquireBg:GetContentWidth()- 5 , self.Image_acquireBg:GetContentHeight()/2 )
    self.Image_acquireBg:AddChild( icon )

    self.acquire = Util:Label( "" , Macros.TypefaceSize.normal , Macros.Color.white )
    self.acquire:SetAnchorPoint( 1 , 0.5 )
    self.acquire:SetPosition( self.Image_acquireBg:GetContentWidth() - 42 , self.Image_acquireBg:GetContentHeight()/2 )
    self.Image_acquireBg:AddChild( self.acquire )

    self.text = Util:Label( CPPGameLib.GetString("chasm_acquire") , Macros.TypefaceSize.normal , Macros.Color.Special )
    self.text:SetAnchorPoint( 1 , 0.5 )
    self.text:SetPosition( self.acquire:GetPositionX() - self.acquire:GetContentWidth() - 5 , self.Image_acquireBg:GetContentHeight()/2 )
    self.Image_acquireBg:AddChild( self.text )

end

--卷轴的世界
function Modules.ChasmSweepView:__SetPuppetStage( )

    self.backgroundLst = CallModuleFunc(ModuleType.CHASM, "GetAccelerateBackground" ) 

    --加载台词
    self.tipsLst = CPPGameLib.GetConfig("EndlessLandTips", nil , nil , true )
    self.tipsLst = self.tipsLst[1]
    
    local scene = {}
    --卷轴背景
    for i , v in ipairs( self.backgroundLst ) do
        --层容器
        scene[i] = Util:Layout( v.content[1] , v.content[2] ) 
        scene[i]:SetAnchorPoint( v.anchor[1] , v.anchor[2] )
        scene[i]:SetPosition( self.Panel_bottom:GetContentWidth()/2 , v.positionY )
        self.Panel_bottom:AddChild( scene[i] )

        local bgLst = {}
        local index = 1
        local sp = Resource.PathTool.GetUIResPath("chasm_bg/".. v.bglst[1] )
        bgLst[1] = Util:Sprite9( sp , { 768 , 433 } )
        bgLst[1]:SetContentSize( scene[i]:GetContentWidth() , scene[i]:GetContentHeight() )
        bgLst[1]:SetPosition( scene[i]:GetContentWidth()/2 + v.positionX , scene[i]:GetContentHeight()/2 )
        scene[i]:AddChild( bgLst[1] )

        if v.speed ~= 0 then
            CPPActionManager.MoveBy( bgLst[1] , bgLst[1]:GetContentWidth() / v.speed , 0 - bgLst[1]:GetContentWidth() , 0 ,  GlobalCallbackMgr:AddCallBackFunc(function ( ... )
                bgLst[1]:RemoveSelf(true)
                bgLst[1] = nil
                table.remove( bgLst , 1 )
            end) )

            local func
            local moveAnimate = function () 

                if bgLst[#bgLst]:GetPositionX() - (bgLst[#bgLst]:GetContentWidth()/2) <= 0 then
                    if v.bglst[index + 1] then
                        index = index + 1
                    else
                        index = 1
                    end

                    local sp = Resource.PathTool.GetUIResPath("chasm_bg/".. v.bglst[index] )
                    bgLst[#bgLst + 1] = Util:Sprite9( sp , { 768 , 433 } )
                    bgLst[#bgLst]:SetContentSize( scene[i]:GetContentWidth() , scene[i]:GetContentHeight() )
                    bgLst[#bgLst]:SetPosition( 
                                                bgLst[#bgLst - 1 ]:GetContentWidth()/2 + 
                                                bgLst[#bgLst - 1 ]:GetPositionX() + 
                                                (bgLst[#bgLst]:GetContentWidth()/2) , 
                                                scene[i]:GetContentHeight()/2 
                                            )
                    scene[i]:AddChild( bgLst[#bgLst] )

                    CPPActionManager.MoveBy( bgLst[#bgLst] , bgLst[#bgLst]:GetContentWidth() / (v.speed / 2) , 0 - (bgLst[#bgLst]:GetContentWidth() * 2) , 0 ,  GlobalCallbackMgr:AddCallBackFunc(function ( ... )
                        bgLst[1]:RemoveSelf(true)
                        bgLst[1] = nil
                        table.remove( bgLst , 1 )
                    end) )
                end

                if bgLst[#bgLst - 1] then
                    bgLst[#bgLst]:SetPosition( 
                                                    bgLst[#bgLst - 1 ]:GetContentWidth()/2 + 
                                                    bgLst[#bgLst - 1 ]:GetPositionX() + 
                                                    (bgLst[#bgLst]:GetContentWidth()/2) , 
                                                    scene[i]:GetContentHeight()/2 
                                                )
                end

                CPPActionManager.DelayTo( scene[i] , 0.1 , func)
            end
            func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
            moveAnimate()
        end
    end

    --勇者
    self:TheBrave()

    --魔王
    self:Devil()

    --背景特效
    local effect = nil
    self.wujinsaodang_1, effect = self:__CreateEffect( {res_name = "UI_wujinsaodang_1"} , self.Panel_bottom , 2)
    PosTool.Center( effect , 0 , 0 )

    --英雄特效
    local effect = nil
    self.pioneer_id = self.heroidLst[ #self.heroidLst ]
    self.pioneer = self.heroArmy[ self.pioneer_id ]
    self.wujinsaodang_2, effect = self:__CreateEffect( {res_name = "UI_wujinsaodang_2"} , self.Panel_ui , 2)
    effect:SetPosition( self.pioneer:GetNode():GetPositionX() , 140 )

    --英雄特效
    local effect = nil
    self.wujinsaodang_3, effect = self:__CreateEffect( {res_name = "UI_wujinsaodang_3"} , self.pioneer:GetModel() , 2)
    PosTool.Center( effect , 50 , -10 )

    --怪物地光
    self.bg_shadow = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_hero_shadow"))
    self.bg_shadow:SetPosition( 0 , 0 )
    self.pioneer:GetModel():AddChild(self.bg_shadow)
end

--进击的勇者
function Modules.ChasmSweepView:TheBrave( )
    --招募勇者
    self.heroidLst = {}
    local own_icon = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.HEAD_ICON)
    table.insert( self.heroidLst , own_icon )
    --勇者大军
    self.heroArmy = {}
    --窃窃私语
    self.whispering = false

    for i , v in ipairs( self.heroidLst ) do
        --排好站位
        self.heroArmy[v] = LogicGUI.Model.New({info_id = v , parent = self.Panel_ui })
        --扫荡界面战队XY坐标
        self.heroArmy[v]:SetPosition(  280+ 50 + ( 80 * (i-1) ) , 200)
        self.heroArmy[v]:SetScaleFactor( 0.6 )
        --跑起来
        self.heroArmy[v]:PlayAnimation(RenderObj.ActionIndex.RUN)

        local func = nil
        --嘲讽
        local whispering = function( node )
            if self.whispering ~= false then
                --私聊触发
                local time = MathTool.GetRandom( 2 , 5 )
                CPPActionManager.DelayTo( node , time , func )
                return
            end
            self.whispering = true

            CPPActionManager.DelayTo( self.Panel_ui , 2 , GlobalCallbackMgr:AddCallBackFunc( function ( ... )
                self.whispering = false
                --私聊触发
                local time = MathTool.GetRandom( 2 , 5 )
                CPPActionManager.DelayTo( node , time , func )
            end ) )

        end

        --私聊触发
        local time = MathTool.GetRandom( 2 , 5 )
        CPPActionManager.DelayTo( self.heroArmy[v]:GetNode() , time , func )
        func = GlobalCallbackMgr:AddCallBackFunc( function ( ... )
            whispering( self.heroArmy[v]:GetNode() )
        end )
        whispering( self.heroArmy[v]:GetNode() )
    end
end

--挨揍的魔王
function Modules.ChasmSweepView:Devil( )

    local sweepcnf = CPPGameLib.GetConfig("EndlessLandSweep" ,nil , nil ,true)

    self.pioneer_id = self.heroidLst[ #self.heroidLst ]
    self.pioneer = self.heroArmy[ self.pioneer_id ]

    --多久刷新一只
    local addMonster = nil
    local newMonster = function () 

        local index = MathTool.GetRandom( 1 , #self.tipsLst.monster_show )
        --怪物
        local monsterArmy = LogicGUI.Model.New({info_id = self.tipsLst.monster_show[index] , parent = self.Panel_ui , model_type = LogicGUI.ModelType.MONSTER })
        monsterArmy:SetPosition( self.Panel_ui:GetContentWidth() + 100 , self.pioneer:GetNode():GetPositionY())
        monsterArmy:SetScaleFactor( 0.6 )
        monsterArmy:SetDir(MathTool.HORZ_LEFT)
        monsterArmy:PlayAnimation(RenderObj.ActionIndex.RUN)

        --怪物地光
        local monsterArmyShadow = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_hero_shadow"))
        monsterArmyShadow:SetPosition( 0 , 0 )
        monsterArmy:GetModel():AddChild( monsterArmyShadow )

        --靠近勇者
        CPPActionManager.MoveTo( 
                                    monsterArmy:GetNode() , 
                                    1 , 
                                    self.pioneer:GetNode():GetPositionX() + 140 , 
                                    self.pioneer:GetNode():GetPositionY() ,  
                                    GlobalCallbackMgr:AddCallBackFunc( function ( ... )
                                        monsterArmy:PlayAnimation(RenderObj.ActionIndex.REPEL)
                                        --在这留个给服务器说砍人
                                        CallModuleFunc(ModuleType.CHASM, "EndlessLandCollisionReq")
                                        self:__SetShowUi()

                                            CPPActionManager.BezierBy( monsterArmy:GetNode() , (sweepcnf[1].monster_rebirth / 1000 )/2  , COCOPoint( 400 , 0 ) , COCOPoint(0, 0) , COCOPoint(300, 200) , GlobalCallbackMgr:AddCallBackFunc( function ( ... )
                                                monsterArmy:DeleteMe()
                                                monsterArmy = nil
                                            end ) )

                                    end ) )
        CPPActionManager.DelayTo( self.Panel_ui , (sweepcnf[1].monster_rebirth / 1000 ) , addMonster)
    end
    --多久刷新一只
    addMonster = GlobalCallbackMgr:AddCallBackFunc(newMonster)
    newMonster()
end

--获得物品
function Modules.ChasmSweepView:OnGetPack( text )
    local text = CallModuleFunc(ModuleType.CHASM, "GetCrashReward" )
    if text ~= "" then
        GlobalTipMsg.GetInstance():ShowGetLayer( text , nil , 1 , "sp_shasm_get" , 200 )
    end
end

--结束扫荡
function Modules.ChasmSweepView:ShowEndTime( num )
    local layout = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height)
    layout:SetAnchorPoint(0.5, 0.5)
    layout:SetTouchEnabled(true)
    self.Panel_ui:AddChild( layout , 999 )
    PosTool.Center(layout)

    Util:SetLayoutBGColor(layout, Macros.Color.lilyBlack )
    layout:SetOpacity(0)

    -- "你在无情杀戮了 %s 只怪物后终于平息了下来"
    local chasm_sweep_tips_3 = Util:Label( CPPGameLib.GetString("chasm_sweep_tips_3" , num ) , Macros.TypefaceSize.popup , Macros.Color.white )
    chasm_sweep_tips_3:SetAnchorPoint( 0.5 , 0.5 )
    layout:AddChild( chasm_sweep_tips_3 )
    PosTool.Center(chasm_sweep_tips_3 , 0 , 50 )
    chasm_sweep_tips_3:SetOpacity(0)

    -- "现在你将冷静一下，在接下来的战斗中给怪物留下一丝尊严"
    local chasm_sweep_tips_4 = Util:Label( CPPGameLib.GetString("chasm_sweep_tips_4") , Macros.TypefaceSize.popup , Macros.Color.white )
    chasm_sweep_tips_4:SetAnchorPoint( 0.5 , 0.5 )
    layout:AddChild( chasm_sweep_tips_4 )
    PosTool.Center(chasm_sweep_tips_4 , 0 , 20 )
    chasm_sweep_tips_4:SetOpacity(0)

    -- "点击进入常规战斗"
    local chasm_sweep_tips_5 = Util:Label( CPPGameLib.GetString("chasm_sweep_tips_5") , Macros.TypefaceSize.popup , Macros.Color.white )
    chasm_sweep_tips_5:SetAnchorPoint( 0.5 , 0.5 )
    layout:AddChild( chasm_sweep_tips_5 )
    PosTool.Center(chasm_sweep_tips_5 , 0 , -70)
    chasm_sweep_tips_5:SetOpacity(0)  

    --变黑时间  
    local num_1 = 0.4
    --第一行字
    local num_2 = 0.4
    --第二行字
    local num_3 = 0.4
    --第三行字
    local num_4 = 0.1

    -- 不要的选中按下ctrl+/
    --逐条显示
    CPPActionManager.FadeIn(layout, num_1 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
        CPPActionManager.FadeIn(chasm_sweep_tips_3, num_2 , GlobalCallbackMgr:AddCallBackFunc( function ( ... )
            CPPActionManager.FadeIn(chasm_sweep_tips_4, num_3 , GlobalCallbackMgr:AddCallBackFunc( function ( ... )
                CPPActionManager.FadeIn(chasm_sweep_tips_5, num_4 , GlobalCallbackMgr:AddCallBackFunc( function ( ... )
                    WidgetUnity.SetWidgetListener(layout, nil, nil, function ( ... )
                        local checkBattle = CallModuleFunc(ModuleType.CHASM, "CheckBattleCondition")
                        if checkBattle then
                            CallModuleFunc(ModuleType.BATTLE, "BattleStartReq", Macros.Game.FBType.CHASM)
                        end
                    end)
                end ) )
            end ) )
        end ) )
    end) )
end

function Modules.ChasmSweepView:__SetShowUi( )
    if self.numOpen ~= true then
        return 
    end
    self.numOpen = false

    local wave = CallModuleFunc(ModuleType.CHASM, "GetHistoryWaveRecord")
    local init_wave = CPPGameLib.GetConfig("EndlessLandWave", wave )

    local monsterNum = 0
    local endlessNum = 0
    if init_wave then
        for i = 1 , init_wave.init_wave - 1 do
            local cnf = CPPGameLib.GetConfig("EndlessLandWave", i )
            monsterNum = monsterNum + cnf.monster_num
            endlessNum = endlessNum + cnf.endless_stone
        end
    end

    -- local is_first_battle = CallModuleFunc(ModuleType.CHASM, "GetTodayFirstBattle")
    -- local is_first_battle_ = CallModuleFunc(ModuleType.CHASM, "GetTodaySweepGetType")
    -- if is_first_battle == true or is_first_battle_ == true then
        CallModuleFunc(ModuleType.CHASM, "SetTotalObtain" , endlessNum , true)
    -- end

    local sweepcnf = CPPGameLib.GetConfig("EndlessLandSweep" ,nil , nil ,true)
    local MAX = (sweepcnf[1].sweep_time - ( sweepcnf[1].monster_rebirth / 2) ) / 1000

    --出怪
    -- 2751 无尽之地扫荡要5秒之内扫完 现在杀多少怪，就有多长，具体问大硕硕
    local func
    local index = 1
    local swop = function () 
        if monsterNum >= index then
            self.sp_shasm_kill:SetVisible( true )
            ActionManager.GetInstance():SpreadAnimation(self.kill_num:GetNode() , 1.5 , 0.1 )
            self.kill_num:SetText( tostring(index) )

            CPPActionManager.DelayTo( self.sp_shasm_kill , ( MAX / monsterNum ) , func)
            index = index + 1

        end
    end
    func = GlobalCallbackMgr:AddCallBackFunc(swop)
    swop()

    --获得
    local func
    local indexGet = 0
    local swopGet = function () 
        if endlessNum >= indexGet then
            indexGet = indexGet + 1
            self:SetAcquire( indexGet )
            CPPActionManager.DelayTo( self.Image_acquireBg , ( MAX / endlessNum ) , func)

        end
    end
    func = GlobalCallbackMgr:AddCallBackFunc(swopGet)
    swopGet()


    local func
    local indexTiming = 0
    local swopTiming = function () 
        if MAX >= indexTiming then
            CPPActionManager.DelayTo( self.Panel_bottom , 0.1 , func)
            indexTiming = indexTiming + 0.1
        else
            self:ShowEndTime( monsterNum )
        end
    end
    func = GlobalCallbackMgr:AddCallBackFunc(swopTiming)
    swopTiming()

end

--扫荡获得
function Modules.ChasmSweepView:SetAcquire( index )
    -- local is_first_battle = CallModuleFunc(ModuleType.CHASM, "GetTodaySweepGetType")
    -- print("xy" , "=-==============================is_first_battle" , is_first_battle )
    -- if is_first_battle ~= true then
    --     return
    -- end
    self.Image_acquireBg:SetVisible(true)

    self.acquire:SetString( index )
    self.text:SetPosition( self.acquire:GetPositionX() - self.acquire:GetContentWidth() - 8 , self.Image_acquireBg:GetContentHeight()/2 )
end

