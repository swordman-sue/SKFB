--[[
%% @module: 无尽深渊
%% @author: yjg
%% @created: 2017年7月17日17:31:09
--]]

Modules = Modules or {} 
Modules.ChasmView = Modules.ChasmView or BaseClass(BaseView)

function Modules.ChasmView:__init()
    self.__layout_name = "chasm"  
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("chasm"),
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

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.CHASM)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.CHASM,true)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.BATTLE_COMMON)
end

function Modules.ChasmView:__OpenCallback( )    
    
    CallModuleFunc(ModuleType.CHASM , "SetTotalObtain" , nil , true )

    local viewInfo = GlobalViewMgr:GetCurViewInfo()

    self:__Getwidget()
    self:__SetPuppetStage()
    self:__ShowUI()
    self:__Server()
end 

function Modules.ChasmView:__Dispose()
    --征战漩涡特效
    if self.__zhengzhan_effect_id then
        self:__DelEffect(self.__zhengzhan_effect_id)
        self.__zhengzhan_effect_id = nil
    end
end

function Modules.ChasmView:__Getwidget( )
    --后台
    self.Panel_bottom = self:GetWidget("Panel_bottom") 

    --前台
    self.Panel_ui = self:GetWidget("Panel_ui") 
    
    --排行榜
    self.Button_rank = Util:WidgetButton( 
                                            self:GetWidget("Button_rank") , 
                                            nil , 
                                            CPPGameLib.Handler(self,self.OnButtonRank)
                                        )
    self.Button_rank:SetNormalImage( Resource.PathTool.GetSystemEntryIconPath("btn_rank"), TextureResType.LOCAL )
    local rank_title = Util:Name( CPPGameLib.GetString("rank_title") , Macros.TypefaceSize.largish )
    rank_title:SetAnchorPoint(0.5,0.5)
    rank_title:SetPosition(self.Button_rank:GetContentWidth()/2 , 0)
    self.Button_rank:AddChild(rank_title)

    --商店
    self.Button_shop = Util:WidgetButton( 
                                            self:GetWidget("Button_shop") , 
                                            nil , 
                                            CPPGameLib.Handler(self,self.OnButtonShop)
                                        )
    self.Button_shop:SetNormalImage( Resource.PathTool.GetSystemEntryIconPath("btn_store"), TextureResType.LOCAL )
    local chasm_shop = Util:Name( CPPGameLib.GetString("chasm_shop") , Macros.TypefaceSize.largish )
    chasm_shop:SetAnchorPoint(0.5,0.5)
    chasm_shop:SetPosition(self.Button_shop:GetContentWidth()/2 , 0)
    self.Button_shop:AddChild(chasm_shop)

    --击杀背景
    self.Image_kill_bg = self:GetWidget("Image_kill_bg") 

    --进入
    self.Button_enter = Util:WidgetButton( 
                                            self:GetWidget("Button_enter") , 
                                            nil , 
                                            CPPGameLib.Handler(self,self.OnButtonEnter),
                                            nil,nil,Macros.Color.button_yellow
                                        )
    self.Button_enter:SetNormalImage( Resource.PathTool.GetSystemEntryIconPath("btn_dungeon"), TextureResType.LOCAL )

    local label = Util:RichText(string.format(Macros.Color.city_headline_rt, CPPGameLib.GetString("chasm_enter") ), Macros.TypefaceSize.popup)
    label:SetAnchorPoint(0.5,0.5)
    label:SetScaleFactor(1)
    label:SetPosition( self.Button_enter:GetContentWidth()/2 , 6 )
    self.Button_enter:AddChild(label)
    -- local label = Util:Name(CPPGameLib.GetString("chasm_enter"),Macros.TypefaceSize.normal)
    -- self.Button_enter:AddChild(label)
    -- PosTool.CenterBottom(label,0, 20)

    local effect
    self.__zhengzhan_effect_id,effect = self:__CreateEffect({res_name = "UI_zhengzhan"},self.Button_enter)
    PosTool.Center(effect,0,-6)
    
    self.Image_acquisition = self:GetWidget("Image_acquisition")

    self.Image_prestigeBg = self:GetWidget("Image_prestigeBg")

    --规则按钮
    local Button_rule = Util:WidgetButton( self:GetWidget("Button_rule") , nil , function ( ... )
        print("xy" , "(2)深渊" )
        local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
        help:SetLoadedCallback( function ( ... )
           help:SetCell("Rule_Chasm")
        end )
    end )
    Button_rule:SetZOrder(100)
    
    local union_rule = Util:Name(CPPGameLib.GetString("union_rule") , Macros.TypefaceSize.largish )
    union_rule:SetAnchorPoint( 0.5,0.5 )
    union_rule:SetPosition( Button_rule:GetContentWidth()/2 , 0 )
    Button_rule:AddChild( union_rule )  

    --本次获得
    self.Image_acquireBg = self:GetWidget("Image_acquireBg")
    self.Image_acquireBg:SetVisible( false )

end 

function Modules.ChasmView:__ShowUI( )

    --威望lab
    local tower_prestige = Util:Name( CPPGameLib.GetString("chasm_stone") ..  CPPGameLib.GetString("common_colon") )
    tower_prestige:SetPosition( 10 , self.Image_prestigeBg:GetPositionY() )
    tower_prestige:SetAnchorPoint( 0 , 0.5 )
    self.Panel_ui:AddChild( tower_prestige )

    --图标
    local prestigeSp = Util:Sprite( Util:GetMoneyIconImgData(Macros.Game.RolePropType.ENDLESS_STORE) )
    PosTool.RightTo(tower_prestige, prestigeSp , -5 )
    prestigeSp:SetScaleFactor(0.85)
    self.Panel_ui:AddChild( prestigeSp )

    --无尽石
    self.prestige = Util:Name( "0" )
    PosTool.RightTo(prestigeSp, self.prestige)
    self.Panel_ui:AddChild( self.prestige )

    --免费次数
    local tab = {
                    { zi = CPPGameLib.GetString("divination_remainTime") , ys = Macros.Color.white , dx = Macros.TypefaceSize.normal },
                    -- { tp = Resource.PathTool.GetItemPath( ConfigAdapter.Common.GetCollisionConsume() ) , dx = 0.5 , hz = TextureResType.LOCAL },
                    { zi = "0" , ys = Macros.Color.white , dx = Macros.TypefaceSize.normal , jg = -10 },
                }

    self.free , self.freeLst = Util:LabCenter(tab)
    self.free:SetAnchorPoint( 0 , 0.5 )
    self.free:SetPosition( 10 , self.Image_acquisition:GetContentHeight()/2 )
    self.Image_acquisition:AddChild( self.free )
    self.free:SetVisible(false)

    local layout = Util:Layout( self.Image_acquisition:GetContentWidth()/2 , self.Image_acquisition:GetContentHeight() , function ( ... )
    end)
    layout:SetAnchorPoint( 0 , 0.5 )
    layout:SetPosition( 20 , self.Image_acquisition:GetContentHeight()/2 )
    self.Image_acquisition:AddChild(layout)

    --按钮
    local btn_btn_bag_add = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bag_add")
    self.addBtn = Util:Button( btn_btn_bag_add , function ( ... )
        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, ConfigAdapter.Common.GetCollisionConsume())
    end )
    self.addBtn:SetAnchorPoint( 1 , 0.5 )
    self.addBtn:SetPosition( self.Image_acquisition:GetContentWidth() - 10 , self.Image_acquisition:GetContentHeight()/2 )
    self.Image_acquisition:AddChild( self.addBtn )
    self.addBtn:SetVisible(false) 

    local chasm_max_kill = Util:Label( CPPGameLib.GetString("chasm_max_kill") , Macros.TypefaceSize.normal , Macros.Color.Special  )
    chasm_max_kill:SetAnchorPoint( 1 , 0.5 )
    chasm_max_kill:SetPosition( self.Image_kill_bg:GetContentWidth()/2 + 20 , self.Image_kill_bg:GetContentHeight()/2 )
    self.Image_kill_bg:AddChild( chasm_max_kill )

    self.chasm_max_kill = Util:Label( "0000" , Macros.TypefaceSize.normal , Macros.Color.white  )
    self.chasm_max_kill:SetAnchorPoint( 0 , 0.5 )
    self.chasm_max_kill:SetPosition( chasm_max_kill:GetPositionX() + 2 , self.Image_kill_bg:GetContentHeight()/2 )
    self.Image_kill_bg:AddChild( self.chasm_max_kill )


end

function Modules.ChasmView:__Server( )
    --深渊
    CallModuleFunc(ModuleType.CHASM , "EndlessLandDataReq") 
    --物品获得
    self:BindGlobalEvent(Macros.Event.ChasmModule.CHASM_DATA , function (_, text)
        self:__SetShowUI( )
    end)
end

function Modules.ChasmView:__SetShowUI( )
    local first = CallModuleFunc(ModuleType.CHASM, "GetTodayFirstBattle" )

    self.free:SetVisible(true)

    self.prestige:SetString( CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.ENDLESS_STORE) )

    --剩余次数
    local num , consume = CallModuleFunc(ModuleType.CHASM, "GetEntranceTime" )
    self.freeLst[2]:SetString( consume - num .. "/" .. consume ) 

    if consume - num <= 0 then
        self.freeLst[2]:SetColor( unpack(Macros.Color.red_color) )
    else
        self.freeLst[2]:SetColor( unpack(Macros.Color.front_color) )
    end

    --设置击杀
    local num = CallModuleFunc(ModuleType.CHASM, "GetKeepKillNum" )
    self.chasm_max_kill:SetString( num )

end

--卷轴的世界
function Modules.ChasmView:__SetPuppetStage( )

    self.backgroundLst = CallModuleFunc(ModuleType.CHASM, "GetBackground" ) 

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

        bgLst[1] = Util:Layout( v.content[1] , v.content[2] ) 
        bgLst[1]:SetAnchorPoint( 0.5 , 0.5 )
        bgLst[1]:SetPosition( (scene[i]:GetContentWidth()/2) + v.positionX , scene[i]:GetContentHeight()/2 )
        scene[i]:AddChild( bgLst[1] )

        local spLst = {}
        for j , vv in ipairs( v.bglst ) do
            print("xy" , "========")
            local sp = Resource.PathTool.GetUIResPath("chasm_bg/".. vv )
            local x = 0
            if spLst[j-1] then
                x = spLst[j-1]:GetPositionX() + spLst[j-1]:GetContentWidth()
            end
            spLst[j] = Util:Sprite( sp )
            spLst[j]:SetAnchorPoint( 0 , 0.5 )
            spLst[j]:SetPosition( x , bgLst[1]:GetContentHeight()/2 )
            bgLst[1]:AddChild( spLst[j] )
        end



        if v.speed ~= 0 then
            CPPActionManager.MoveBy( bgLst[1] , bgLst[1]:GetContentWidth() / v.speed , 0 - bgLst[1]:GetContentWidth() , 0 ,  GlobalCallbackMgr:AddCallBackFunc(function ( ... )
                bgLst[1]:RemoveSelf(true)
                bgLst[1] = nil
                table.remove( bgLst , 1 )
            end) )

            local func
            local moveAnimate = function () 

                if bgLst[#bgLst]:GetPositionX() - (bgLst[#bgLst]:GetContentWidth()/2) <= 0 then

                    bgLst[#bgLst + 1] = Util:Layout( v.content[1] , v.content[2] ) 
                    bgLst[#bgLst]:SetAnchorPoint( 0.5 , 0.5 )
                    bgLst[#bgLst]:SetPosition( 
                                                bgLst[#bgLst - 1 ]:GetContentWidth()/2 + 
                                                bgLst[#bgLst - 1 ]:GetPositionX() + 
                                                (bgLst[#bgLst]:GetContentWidth()/2) , 
                                                scene[i]:GetContentHeight()/2 
                                            )
                    scene[i]:AddChild( bgLst[#bgLst] )

                    local spLst = {}
                    for j , vv in ipairs( v.bglst ) do
                        print("xy" , "========")
                        local sp = Resource.PathTool.GetUIResPath("chasm_bg/".. vv )
                        local x = 0
                        if spLst[j-1] then
                            x = spLst[j-1]:GetPositionX() + spLst[j-1]:GetContentWidth()
                        end
                        spLst[j] = Util:Sprite( sp )
                        spLst[j]:SetAnchorPoint( 0 , 0.5 )
                        spLst[j]:SetPosition( x , bgLst[#bgLst]:GetContentHeight()/2 )
                        bgLst[#bgLst]:AddChild( spLst[j] )
                    end


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
    CPPActionManager.DelayTo( self.Panel_bottom , 2 , GlobalCallbackMgr:AddCallBackFunc( function ( ... )
        self:Devil()
    end))

    --传送门
    local effect = nil
    self.wujinchuansong_1, effect = self:__CreateEffect( {res_name = "UI_wujinchuansong"} , self.Panel_ui , 2)
    -- PosTool.Center( effect , self.Panel_ui:GetContentWidth() , 50 )
    effect:SetPosition( self.Panel_ui:GetContentWidth() - 70  , 250 )
end

--进击的勇者
function Modules.ChasmView:TheBrave( )
    -- 主界面战队X坐标
    self.X = 150

    --招募勇者
    self.heroidLst = {}
    for lineup_index = 1 , 8 do
        -- --不见天日的援军
        -- local hero_info = CallModuleFunc(ModuleType.HERO, "GetReinforcementLineupInfo", lineup_index)
        -- if hero_info.uid ~= 0 then
        --     local hero_info_ = CallModuleFunc(ModuleType.HERO, "GetHeroInfo", hero_info.uid )
        --     if #self.heroidLst ~= 0 then
        --         local sort = MathTool.GetRandom( 1 , #self.heroidLst )
        --         table.insert( self.heroidLst , sort , hero_info_.info_id )
        --     else
        --         table.insert( self.heroidLst , hero_info_.info_id )
        --     end
        -- end
        --过劳死的一队
        local hero_info = CallModuleFunc(ModuleType.HERO, "GetLineupHeroInfo", lineup_index)
        if hero_info then
            if #self.heroidLst ~= 0 then
                local sort = MathTool.GetRandom( 1 , #self.heroidLst )
                table.insert( self.heroidLst , sort , hero_info.info_id )
            else
                table.insert( self.heroidLst , hero_info.info_id )
            end
        end
    end

    --勇者大军
    self.heroArmy = {}
    --
    self.heroArmyTips = {}
    --
    self.heroArmyShadow = {}
    --窃窃私语
    self.whispering = false

    for i , v in ipairs( self.heroidLst ) do
        --排好站位
        self.heroArmy[v] = LogicGUI.Model.New({info_id = v , parent = self.Panel_ui })
        --主界面战队XY坐标
        self.heroArmy[v]:SetPosition( self.X + 10 + ( 120 * (i-1) ) , 180 )
        self.heroArmy[v]:SetScaleFactor( 0.6 )
        --跑起来
        self.heroArmy[v]:PlayAnimation(RenderObj.ActionIndex.RUN)
        self.heroArmyTips[v] = Util:Layout( 100 , 100 )

        self.heroArmyTips[v]:SetPosition( self.X + 10 + ( 120 * (i-1) ) , 180 )
        self.heroArmyTips[v]:SetAnchorPoint(0.5 , 0.5)
        self.Panel_ui:AddChild( self.heroArmyTips[v] , 9 )

        --怪物地光
        self.heroArmyShadow[v] = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_hero_shadow"))
        self.heroArmyShadow[v]:SetPosition( 0 , 0 )
        self.heroArmy[v]:GetModel():AddChild(self.heroArmyShadow[v])

        local func = nil
        --嘲讽
        local whispering = function( node )
            if self.whispering ~= false then
                local time = MathTool.GetRandom( 2 , 5 )
                CPPActionManager.DelayTo( node:GetNode() , time , func )
                return
            end
            self.whispering = true

            --喊话
            local kailBg = Util:Sprite9( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "lead_tips_bg") )
            kailBg:SetContentSize( 
                                    ( kailBg:GetContentWidth() * 0.48 + 10 ) ,  --按原图缩放百分之40 + 10像素
                                    ( kailBg:GetContentHeight() * 0.48 + 10 )   --按原图缩放百分之40 + 10像素
                                )
            kailBg:SetPosition( (self.heroArmyTips[v]:GetContentWidth()/2) + (kailBg:GetContentWidth()/2) + 40 , 180 )
            self.heroArmyTips[v]:AddChild(kailBg , 5 )

            local index = MathTool.GetRandom( 1 , #self.tipsLst.hero_say )
            local hero_say = Util:Label( self.tipsLst.hero_say[index] , Macros.TypefaceSize.slightly , Macros.Color.content , 100 )
            hero_say:SetPosition( kailBg:GetContentWidth()/2 , kailBg:GetContentHeight()/2 )
            kailBg:AddChild(hero_say)

            CPPActionManager.DelayTo( self.Image_kill_bg , 2 , GlobalCallbackMgr:AddCallBackFunc( function ( ... )

                hero_say:RemoveSelf(true)
                hero_say = nil

                kailBg:RemoveSelf(true)
                kailBg = nil

                self.whispering = false
                --私聊触发
                local time = MathTool.GetRandom( 2 , 5 )
                CPPActionManager.DelayTo( node:GetNode() , time , func )
            end ) )

        end

        --私聊触发
        local time = MathTool.GetRandom( 2 , 5 )
        CPPActionManager.DelayTo( self.heroArmy[v]:GetNode() , time , func )
        func = GlobalCallbackMgr:AddCallBackFunc( function ( ... )
            whispering( self.heroArmy[v] )
        end )
        whispering( self.heroArmy[v] )
    end
end

--车轮战
function Modules.ChasmView:CarriageWheel( )
    local t1 = {}
    for i = 1 , #self.heroidLst do
        if self.heroidLst[i-1] then
            t1[i] = self.heroidLst[i-1]
        else
            t1[i] = self.heroidLst[#self.heroidLst]
        end
    end
    self.heroidLst = t1

    local time = math.abs( #self.heroidLst - 1 ) / 2 

    for i , v in ipairs( self.heroidLst ) do
        self.heroArmy[v]:PlayAnimation(RenderObj.ActionIndex.RUN)
        local x = self.X + 50 + ( 120 * (i-1) )
        if x ~= self.heroArmy[v]:GetNode():GetPositionX() then
            ---如果不是现在的第一位
            if i ~= 1 then
                CPPActionManager.MoveTo( self.heroArmy[v]:GetNode() , 1 + time / 2 , x , self.heroArmy[v]:GetNode():GetPositionY() )
                CPPActionManager.MoveTo( self.heroArmyTips[v] , 1 + time / 2 , x , self.heroArmyTips[v]:GetPositionY() )
            else
                CPPActionManager.MoveTo( self.heroArmy[v]:GetNode() , 1 + time / 2, x , self.heroArmy[v]:GetNode():GetPositionY() , GlobalCallbackMgr:AddCallBackFunc( function ( ... )
                    -- self.pioneer_trade = false
                end ) )
                CPPActionManager.MoveTo( self.heroArmyTips[v] , 1 + time / 2 , x , self.heroArmyTips[v]:GetPositionY() )
            end
        end
        self.heroArmy[v]:GetNode():SetZOrder( i )
    end
end


--挨揍的魔王
function Modules.ChasmView:Devil( )
     -- 挨揍怪物
    self.monsterArmy = nil
    self.monsterArmyTips = nil
    local func
    local earthbags = function () 
        self.pioneer_id = self.heroidLst[ #self.heroidLst ]
        self.pioneer = self.heroArmy[ self.pioneer_id ]

        local index = MathTool.GetRandom( 1 , #self.tipsLst.monster_show )

        self.monsterArmy = LogicGUI.Model.New({info_id = self.tipsLst.monster_show[index] , parent = self.Panel_ui , model_type = LogicGUI.ModelType.MONSTER })
        --主界面怪物XY坐标
        self.monsterArmy:SetPosition( self.Panel_ui:GetContentWidth() - 100 , 180 )
        self.monsterArmy:SetScaleFactor( 0.6 )
        self.monsterArmy:SetDir(MathTool.HORZ_LEFT)
        self.monsterArmy:PlayAnimation(RenderObj.ActionIndex.RUN)

        --主界面怪物对话XY坐标
        self.monsterArmyTips = Util:Layout( self.monsterArmy:GetWidth() , self.monsterArmy:GetHeight() )
        self.monsterArmyTips:SetPosition( self.Panel_ui:GetContentWidth() - 100 , 180 )
        self.monsterArmyTips:SetAnchorPoint(0.5 , 0.5)
        self.Panel_ui:AddChild( self.monsterArmyTips , 9 )

        --怪物地光
        self.monsterArmyShadow = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_hero_shadow"))
        self.monsterArmyShadow:SetPosition( 0 , 0 )
        self.monsterArmy:GetModel():AddChild(self.monsterArmyShadow)

        --对话
        self:DevilSay()

        --移动
        self:DevilMove()
    end

    earthbags()
end


--挨揍的魔王台词
function Modules.ChasmView:DevilSay( )
    local time = MathTool.GetRandom( 0 , Macros.Global.DesignSize.width /  (self.backgroundLst[#self.backgroundLst].speed * 2 ) )
   
    CPPActionManager.DelayTo( self.monsterArmy:GetNode() , time , GlobalCallbackMgr:AddCallBackFunc( function ( ... )
        local index = MathTool.GetRandom( 1 , #self.tipsLst.monster_say )
        if self.tipsLst.monster_say[index] then
            --喊话
            local kailBg = Util:Sprite9( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "lead_tips_bg") )
            kailBg:SetContentSize( 
                                    ( kailBg:GetContentWidth() * 0.48 + 10) ,  --按原图缩放百分之40 + 10像素
                                    ( kailBg:GetContentHeight() * 0.48 + 10 )   --按原图缩放百分之40 + 10像素
                                )
            kailBg:SetPosition( (self.monsterArmyTips:GetContentWidth()/2) - 40 , 180 )
            kailBg:SetFlipX(true)
            self.monsterArmyTips:AddChild(kailBg , 5 )
            
            self.monsterArmyTips:SetContentSize( kailBg:GetContentWidth() , kailBg:GetContentHeight() )

            local hero_say = Util:Label( self.tipsLst.monster_say[index] or "" , Macros.TypefaceSize.slightly , Macros.Color.content , 100 )
            hero_say:SetPosition( kailBg:GetPositionX() , kailBg:GetPositionY() )
            self.monsterArmyTips:AddChild(hero_say ,9 )

            CPPActionManager.DelayTo( self.monsterArmyTips , 2 , GlobalCallbackMgr:AddCallBackFunc( function ( ... )
                self.monsterArmyTips:RemoveSelf(true)
                self.monsterArmyTips = nil
            end ) )
        end
    end ) )
end

--挨揍的魔王漫步
function Modules.ChasmView:DevilMove( )
    self.pioneer_id = self.heroidLst[ #self.heroidLst ]
    self.pioneer = self.heroArmy[ self.pioneer_id ]
    self.pioneerTips = self.heroArmyTips[ self.pioneer_id ]

    -- 主界面怪物对话移动XY坐标
    CPPActionManager.MoveTo( 
                            self.monsterArmyTips , 
                            --移动所需时间
                            0.7, 
                            --对话框移动X轴
                            self.pioneer:GetNode():GetPositionX() + 100 , 
                            --对话框移动Y轴
                            self.pioneer:GetNode():GetPositionY()
                        )
    --撞击点
    self.collide =  self.pioneer:GetNode():GetPositionX() + 110

    --主界面怪物攻击移动XY坐标
    CPPActionManager.MoveTo( 
                            self.monsterArmy:GetNode() , 
                                                                                                                --移动所需时间
                            0.7 , 
                            --怪物移动X轴                        --到什么坐标做攻击动作
                            self.pioneer:GetNode():GetPositionX() + 110 , 
                            --怪物移动Y轴
                            self.pioneer:GetNode():GetPositionY(),
                            GlobalCallbackMgr:AddCallBackFunc( function ( ... )

                                --靠近勇者做攻击
                                local cnf = CPPGameLib.GetConfig("Hero", self.pioneer_id )
                                if Macros.Game.IsProfessionNear( cnf.type  ) then
                                    self.pioneer:PlayAnimation(RenderObj.ActionIndex.ATTACK , false)
                                else
                                    self.pioneer:PlayAnimation(RenderObj.ActionIndex.ATTACK_NEAR , false)
                                end
                                self.monsterArmy:PlayAnimation(RenderObj.ActionIndex.ATTACK , false)

                                --主界面怪物击退移动XY坐标
                                CPPActionManager.MoveTo( 
                                                        self.monsterArmy:GetNode() , 
                                                        --到指定位置坐攻击动作所需时间
                                                        0.03 , 
                                                        --怪物攻击X轴
                                                        self.pioneer:GetNode():GetPositionX() + 110 , 
                                                        --怪物攻击Y轴
                                                        self.pioneer:GetNode():GetPositionY(),
                                                        GlobalCallbackMgr:AddCallBackFunc( function ( ... )

                                                            self.pioneer:PlayAnimation(RenderObj.ActionIndex.REPEL , false)
                                                            self.monsterArmy:PlayAnimation(RenderObj.ActionIndex.REPEL , false)
                                                                --击退时间。X、Y轴
                                                            CPPActionManager.MoveBy( self.monsterArmy:GetNode() , 0.5 , 280 , 300 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
                                                               --下落时间，X,Y
                                                                CPPActionManager.MoveBy( self.monsterArmy:GetNode() , 0.2 , 30 , -40 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
                                                                    self:MonsterRemove()
                                                                end) )
                                                            end) )

                                                            self.pass_time = 0
                                                            self.hero_distance = 0
                                                            -- self.manager_distance = 0
                                                            -- --移动
                                                            self:SetUpdateFunc(CPPGameLib.Handler(self, self.__UpRepel))
                                                        end )
                                                    )
                            end )
                        )
end


function Modules.ChasmView:__UpRepel(elapse_time )
    self.pass_time = self.pass_time + elapse_time 

    --击退位移 = v * t * ( 1 - v * t / 4 / l )
    local v = ConfigAdapter.Common.GetBeatbackInitSpeed()
    local t = self.pass_time

    --英雄被击退距离
    local heroL = 600
    local dis = v * t * (1 - v * t / 4 / heroL)
    if dis < self.hero_distance then
        local mov_dis = heroL * -1
        local now_pos = self.collide  + mov_dis 
        self.pioneer:SetPosition( now_pos , self.pioneer:GetNode():GetPositionY() )
        if self.pioneerTips then
            self.pioneerTips:SetPosition( now_pos , self.pioneerTips:GetPositionY() )
        end
        self:SetUpdateFunc()
        self:CarriageWheel()
    else
        self.hero_distance = dis
        local mov_dis = self.hero_distance * -1
        local now_pos = self.collide + mov_dis    
        self.pioneer:SetPosition( now_pos , self.pioneer:GetNode():GetPositionY() )
        if self.pioneerTips then
            self.pioneerTips:SetPosition( now_pos , self.pioneerTips:GetPositionY() )
        end
    end

    -- --怪物被击退距离
    -- local monsterL = 600
    -- local dis_ = v * t * (1 - v * t / 4 / monsterL)
    -- if dis_ < self.manager_distance then
    --     local mov_dis = monsterL * 1
    --     local now_pos = self.collide  + mov_dis 
    --     self.monsterArmy:SetPosition( now_pos , self.monsterArmy:GetNode():GetPositionY() )
    --     if self.monsterArmyTips then
    --         self.monsterArmyTips:SetPosition( now_pos , self.monsterArmy:GetNode():GetPositionY() )
    --     end
    --     self:SetUpdateFunc()
    --     self:MonsterRemove()
    -- else
    --     self.manager_distance = dis_
    --     local mov_dis = self.manager_distance * 1
    --     local now_pos = self.collide + mov_dis    
    --     self.monsterArmy:SetPosition( now_pos , self.monsterArmy:GetNode():GetPositionY() )
    --     if self.monsterArmyTips then
    --         self.monsterArmyTips:SetPosition( now_pos , self.monsterArmy:GetNode():GetPositionY() )
    --     end
    -- end
end

--怪物删除
function Modules.ChasmView:MonsterRemove( )
    if self.monsterArmyTips then
        self.monsterArmyTips:RemoveSelf(true)
        self.monsterArmyTips = nil
    end
    self.monsterArmy:DeleteMe()
    self.monsterArmy = nil
    
    --怪物刷新间隔时间
    CPPActionManager.DelayTo( self.Panel_ui , 3.5 , GlobalCallbackMgr:AddCallBackFunc(function ( ... )
        self:Devil()
    end)  ) 
end


--排行榜
function Modules.ChasmView:OnButtonRank( )
    CallModuleFunc(ModuleType.CHASM, "OpenView", BaseViewType.CHASM_RANK )
end

--商店
function Modules.ChasmView:OnButtonShop( )
    CallModuleFunc(ModuleType.CHASM, "OpenView", BaseViewType.CHASM_SHOP )
end

--进入
function Modules.ChasmView:OnButtonEnter( )
    local function start( ... )
        CallModuleFunc(ModuleType.CHASM, "EndlessLandEnterReq" )
    end
    --是不是免费
    local first = CallModuleFunc(ModuleType.CHASM, "GetTodayFirstBattle" )
    --已经进入次数 , 今日可进入次数
    local entrance_time , gratis_entrance , freeTimes  = CallModuleFunc(ModuleType.CHASM, "GetEntranceTime" )
    --下次等级 , 下次数字
    local nexLevel , nexTimes = CallModuleFunc(ModuleType.CHASM, "GetVipEntranceTips" )
   
    --是不是免费
    if first then
        start()
    --有进入次数
    elseif entrance_time < gratis_entrance then
        LogicGUI.TemplChasmAffirmTips.New()
    --有下级
    elseif nexLevel then
        --弹出VIP升级提示
        local tips = LogicGUI.TemplAffirmTips.New( { 
                                                        type = LogicGUI.TemplAffirmTipsType.YES_NO,
                                                        content = " ",
                                                        confirm_func = function ( ... )
                                                            CallModuleFunc(ModuleType.CHARGE, "OpenView", BaseViewType.CHARGE )
                                                        end
                                                    } )
        tips:SetLoadedCallback( function ( ... )
            local gem_dismantle_tips_1 = Util:Label( CPPGameLib.GetString("chasm_UpVip" , nexLevel , nexTimes - freeTimes ) , nil , Macros.Color.content )
            gem_dismantle_tips_1:SetAnchorPoint(0.5,0.5)
            gem_dismantle_tips_1:SetPosition(tips:GetNode():GetContentWidth()/2 , tips:GetNode():GetContentHeight()/2 + 20 )
            tips:GetNode():AddChild(gem_dismantle_tips_1)

            local gem_dismantle_tips_2 = Util:Label( CPPGameLib.GetString("chasm_GoUpVip") , nil , Macros.Color.content )
            gem_dismantle_tips_2:SetAnchorPoint(0.5,0.5)
            gem_dismantle_tips_2:SetPosition(tips:GetNode():GetContentWidth()/2 , tips:GetNode():GetContentHeight()/2 - 25 )
            tips:GetNode():AddChild(gem_dismantle_tips_2)
        end )
    else
        --没次数
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("chasm_NoTips") )
    end
end

--获得物品
function Modules.ChasmView:OnGetPack( text )
    local text = CallModuleFunc(ModuleType.CHASM, "GetCrashReward" )
    if text ~= "" then
        GlobalTipMsg.GetInstance():ShowGetLayer( text , nil , 1 , "sp_shasm_get" , 200 )
    end
end
