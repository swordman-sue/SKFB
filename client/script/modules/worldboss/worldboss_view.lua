--[[
%% @module: 世界boss
%% @author: yjg
%% @created: 2016年11月17日15:33:19
--]]

Modules = Modules or {} 
Modules.WorldBossView = Modules.WorldBossView or BaseClass(BaseView)

function Modules.WorldBossView:__init()
    self.__layout_name = "worldboss"	
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_worldboss")}
    self.__title_bar_params = 
    {
      title = CPPGameLib.GetString("worldBoss"), 
      close_func = function()
        self:CloseManual()
      end,
    }

    self.__open_callback = function()
      self:__OpenCallback()
    end

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.WORLDBOSS)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.WORLDBOSS,true)
end

function Modules.WorldBossView:__OpenCallback( )
    self:__Getwidget()
    self:__ShowUI()
    self:__Server()
end	

function Modules.WorldBossView:__Server( )

    CallModuleFunc(ModuleType.WORLDBOSS, "WorldBossDataReq")

    local info = {}
    info.reward_type = Modules.WorldBossModule.REWARD_TYPE.HONOR
    CallModuleFunc(ModuleType.WORLDBOSS, "WorldBossRewardRecordReq",info)

    local info = {}
    info.reward_type = Modules.WorldBossModule.REWARD_TYPE.KILL
    CallModuleFunc(ModuleType.WORLDBOSS, "WorldBossRewardRecordReq",info)


    --世界boss出世
    self:BindGlobalEvent(Macros.Event.WorldBossModule.WORLDBOSS_INFO,function (_, data)
        --选择阵营
        self:SelectCamp()
        -- --次数回复
        -- self:TimeReply()
        --boss复活
        self:TimeRelive()
        --设置数据
        self:SetShowUI()
    end)

    --世界boss阵容选择信息
    self:BindGlobalEvent(Macros.Event.WorldBossModule.WORLDBOSS_CAST,function (_, data)
    end)

    --世界boss伤害公告
    self:BindGlobalEvent(Macros.Event.WorldBossModule.WORLDBOSS_NOTICE,function (_, data)

        if data.damage == 0 then
            return 
        end

        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.WORLDBOSS, "img_name_bg")
        local img_name_bg = Util:Sprite(sp)
        img_name_bg:SetAnchorPoint( 0.5 , 0.5 )
        img_name_bg:SetPosition( MathTool.GetRandom( 0 , self.ballBg:GetContentWidth() ) , MathTool.GetRandom( 0 , self.ballBg:GetContentHeight() ) )
        self.ballBg:AddChild( img_name_bg )

        local name = Util:Name( data.attack_name , Macros.TypefaceSize.largish , Macros.Game.QualityType.ORANGE )
        img_name_bg:AddChild( name )
        PosTool.Center( name )

        local battle_up_num = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.WORLDBOSS, "battle_crit_num" , true) , GUI.ImgFont.ImgType.Num3  )
        battle_up_num:SetAnchorPoint(0.5, 0)
        battle_up_num:SetText("-"..tostring(data.damage))
        battle_up_num:SetPosition( img_name_bg:GetContentWidth()/2 , img_name_bg:GetContentHeight() + 10 )
        img_name_bg:AddChild(battle_up_num:GetNode())

        ActionManager.GetInstance():RunFadeOut( img_name_bg , 3 , function ( ... )
            img_name_bg:RemoveSelf(true)
            img_name_bg = nil
        end )
        
        --设置数据
        self:SetShowUI()
    end)

    self:BindGlobalEvent(Macros.Event.WorldBossModule.WORLDBOSS_AWARD,function (_, data)
        self:SetRed()
    end)

    self:BindGlobalEvent(Macros.Event.LimitActivityModule.REFRASH_ACTIVITY_END,function (_,system_id)
        if system_id == Macros.Game.SystemID.WORLDBOSS then
            CallModuleFunc(ModuleType.WORLDBOSS, "WorldBossDataReq")
        end
    end)
end

function Modules.WorldBossView:__Getwidget( )
    --UI层
    self.Panel_ui = self:GetWidget("Panel_ui") 
    --奖励按钮
    self.Button_award = Util:WidgetButton( self:GetWidget("Button_award") , " " , CPPGameLib.Handler(self,self.OnDaward) )
    local btn_shop = Resource.PathTool.GetSystemEntryIconPath("btn_shop")
    self.Button_award:SetNormalImage( btn_shop , TextureResType.LOCAL )
    self.Button_award:SetScale9Enabled( false )
    local worldBoss_honorAward = Util:Name( CPPGameLib.GetString("worldBoss_honorAward") , Macros.TypefaceSize.largish )
    worldBoss_honorAward:SetAnchorPoint(0.5,0.5)
    worldBoss_honorAward:SetPosition(self.Button_award:GetContentWidth()/2 , 0)
    self.Button_award:AddChild(worldBoss_honorAward)

    --商店按钮
    self.Button_shop = Util:WidgetButton( self:GetWidget("Button_shop") , " " , CPPGameLib.Handler(self,self.OnButtonShop) )
    local btn_store = Resource.PathTool.GetSystemEntryIconPath("btn_store")
    self.Button_shop:SetNormalImage( btn_store , TextureResType.LOCAL )
    self.Button_shop:SetScale9Enabled( false )
    local worldBoss_exploitsShop = Util:Name( CPPGameLib.GetString("worldBoss_exploitsShop") , Macros.TypefaceSize.largish )
    worldBoss_exploitsShop:SetAnchorPoint(0.5,0.5)
    worldBoss_exploitsShop:SetPosition(self.Button_shop:GetContentWidth()/2 , 0)
    self.Button_shop:AddChild(worldBoss_exploitsShop)

    --货币背景
    self.Image_currencyBg = self:GetWidget("Image_currencyBg") 

    --荣誉榜
    self.Button_honor = Util:WidgetButton( self:GetWidget("Button_honor") , " " , CPPGameLib.Handler(self,self.OnHonor) )
    local btn_rank = Resource.PathTool.GetSystemEntryIconPath("btn_rank")
    self.Button_honor:SetNormalImage( btn_rank , TextureResType.LOCAL)
    self.Button_honor:SetScale9Enabled( false )
    local worldBoss_honorRank = Util:Name( CPPGameLib.GetString("worldBoss_honorRank") , Macros.TypefaceSize.largish )
    worldBoss_honorRank:SetAnchorPoint(0.5,0.5)
    worldBoss_honorRank:SetPosition(self.Button_honor:GetContentWidth()/2 , 0)
    self.Button_honor:AddChild(worldBoss_honorRank)

    --伤害榜
    self.Button_harm = Util:WidgetButton( self:GetWidget("Button_harm") , " " , CPPGameLib.Handler(self,self.OnHarm) )
    self.Button_harm:SetNormalImage( btn_rank , TextureResType.LOCAL)
    self.Button_harm:SetScale9Enabled( false )
    local worldBoss_harmRank = Util:Name( CPPGameLib.GetString("worldBoss_harmRank") , Macros.TypefaceSize.largish )
    worldBoss_harmRank:SetAnchorPoint(0.5,0.5)
    worldBoss_harmRank:SetPosition(self.Button_harm:GetContentWidth()/2 , 0)
    self.Button_harm:AddChild(worldBoss_harmRank)

    --战报
    self.Button_record = Util:WidgetButton( self:GetWidget("Button_record") , " " , CPPGameLib.Handler(self,self.OnScore) )
    self.Button_record:SetNormalImage( btn_rank , TextureResType.LOCAL)
    self.Button_record:SetScale9Enabled( false )
    local worldBoss_score = Util:Name( CPPGameLib.GetString("worldBoss_score") , Macros.TypefaceSize.largish )
    worldBoss_score:SetAnchorPoint(0.5,0.5)
    worldBoss_score:SetPosition(self.Button_record:GetContentWidth()/2 , 0)
    self.Button_record:AddChild(worldBoss_score)

    --boss名
    self.Panel_name = self:GetWidget("Panel_name") 
    --boss模型
    self.Panel_modes = self:GetWidget("Panel_modes") 

    --战斗进行时
    self.Panel_proceed = self:GetWidget("Panel_proceed")
    --血条 
    self.Panel_barHp = self:GetWidget("Panel_barHp") 
    --攻击按钮
    self.Button_challenge = Util:WidgetButton( 
                                                self:GetWidget("Button_challenge") , 
                                                CPPGameLib.GetString("challenge") , 
                                                CPPGameLib.Handler(self,self.OnButtonHit) , 
                                                Macros.TypefaceSize.popup , nil , Macros.Color.button_yellow
                                             )
    --榜单背景
    self.Image_proceedInfoBg1 = self:GetWidget("Image_proceedInfoBg1") 

    --已击杀
    self.Panel_over = self:GetWidget("Panel_over") 
    --时间提示
    self.Panel_timeTips = self:GetWidget("Panel_timeTips") 
    --榜单背景2
    self.Image_proceedInfoBg2 = self:GetWidget("Image_proceedInfoBg2") 
    --排行榜
    self.Image_honor = self:GetWidget("Image_honor") 
    --名字
    self.Image_headline = self:GetWidget("Image_headline") 
    self.Image_headline:IgnoreContentAdaptWithSize(true)

    --阵营图标
    self.Image_form_icon = {}
    --阵营信息背景
    self.Image_form_bg = {}
    for i = 1 , 3 do
        self.Image_form_icon[i] = self:GetWidget("Image_form_icon_"..i)
        self.Image_form_bg[i] = self:GetWidget("Image_form_bg_"..i)
    end
    --击杀标志
    self.Image_12 = self:GetWidget("Image_12") 

    --规则按钮
    local Button_rule = Util:WidgetButton( self:GetWidget("Button_rule") , nil , function ( ... )
        print("xy" , "(7)世界BOSS，大图标：" )
        local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
        help:SetLoadedCallback( function ( ... )
           help:SetCell("Rule_WorldBoss")
        end )
    end )
    Button_rule:SetZOrder(100)
    local common_rule = Util:Name( CPPGameLib.GetString("common_rule") , Macros.TypefaceSize.largish )
    common_rule:SetAnchorPoint(0.5,0.5)
    common_rule:SetPosition(Button_rule:GetContentWidth()/2 , 0)
    Button_rule:AddChild(common_rule)
    
    self.Panel_proceed:SetVisible(false)
    self.Panel_over:SetVisible(false)
end 

function Modules.WorldBossView:__ShowUI( )
    --战功
    local worldBoss_exploits = Util:Name( CPPGameLib.GetString("worldBoss_exploits") )
    worldBoss_exploits:SetAnchorPoint( 0 , 0.5 )
    worldBoss_exploits:SetPosition(10 , self.Image_currencyBg:GetPositionY() - 1 )
    self.Panel_ui:AddChild(worldBoss_exploits)

    local img = Util:GetMoneyIconImgData(Macros.Game.RolePropType.REBEL_VALUE)
    local icon = Util:Sprite( img )
    PosTool.RightTo( worldBoss_exploits , icon )
    self.Panel_ui:AddChild( icon )

    self.worldBoss_exploits = Util:Name(" ")
    PosTool.RightTo( icon , self.worldBoss_exploits , 5 )
    self.Panel_ui:AddChild(self.worldBoss_exploits)

    --挑战总次数
    local worldBoss_dareTime = Util:Label( CPPGameLib.GetString("worldBoss_dareTime") , nil , Macros.Color.Special  )
    worldBoss_dareTime:SetAnchorPoint(0,0.5)
    worldBoss_dareTime:SetPosition( 15 , self.Image_proceedInfoBg1:GetContentHeight() - worldBoss_dareTime:GetContentHeight() - 10 )
    self.Image_proceedInfoBg1:AddChild( worldBoss_dareTime )

    self.worldBoss_dareTime = Util:Label( "" , nil , Macros.Color.white  )
    PosTool.RightTo( worldBoss_dareTime , self.worldBoss_dareTime , 5 )
    self.Image_proceedInfoBg1:AddChild( self.worldBoss_dareTime ) 

    --次数添加
    local btn_btn_worldboss_addnum = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_worldboss_addnum")
    local addBtn = Util:Button( btn_btn_worldboss_addnum ,  CPPGameLib.Handler(self,self.OnButtonAddTime) )
    addBtn:SetAnchorPoint(1,0.5)
    addBtn:SetPosition( self.Image_proceedInfoBg1:GetContentWidth() - ( addBtn:GetContentWidth()/3 ) , worldBoss_dareTime:GetPositionY() )
    self.Image_proceedInfoBg1:AddChild( addBtn ) 
    
    --我的荣誉
    local worldBoss_Myhonor = Util:Label( CPPGameLib.GetString("worldBoss_Myhonor") , nil , Macros.Color.Special  )
    worldBoss_Myhonor:SetAnchorPoint(0,0.5)
    worldBoss_Myhonor:SetPosition( worldBoss_dareTime:GetPositionX() , worldBoss_dareTime:GetPositionY() - ( worldBoss_Myhonor:GetContentHeight() * 2 ) )
    self.Image_proceedInfoBg1:AddChild( worldBoss_Myhonor )

    self.worldBoss_Myhonor = Util:Label( " " , nil , Macros.Color.white  )
    self.worldBoss_Myhonor:SetAnchorPoint(0,0.5)
    self.worldBoss_Myhonor:SetPosition( worldBoss_dareTime:GetPositionX() , worldBoss_Myhonor:GetPositionY() - ( self.worldBoss_Myhonor:GetContentHeight() ) )
    self.Image_proceedInfoBg1:AddChild( self.worldBoss_Myhonor )

    self.MyhonorRank = Util:Label( CPPGameLib.GetString("worldBoss_NoRank") , nil , Macros.Color.green  )
    PosTool.RightTo( self.worldBoss_Myhonor , self.MyhonorRank , 5 )
    self.Image_proceedInfoBg1:AddChild( self.MyhonorRank )
   
    --最高伤害
    local worldBoss_maxHarm = Util:Label(CPPGameLib.GetString("worldBoss_maxHarm") , nil , Macros.Color.Special )
    worldBoss_maxHarm:SetAnchorPoint(0,0.5)
    worldBoss_maxHarm:SetPosition( self.worldBoss_Myhonor:GetPositionX() , self.worldBoss_Myhonor:GetPositionY() - ( worldBoss_maxHarm:GetContentHeight() * 2 ) )
    self.Image_proceedInfoBg1:AddChild(worldBoss_maxHarm)

    self.worldBoss_maxHarm = Util:Label( " " , nil , Macros.Color.white  )
    self.worldBoss_maxHarm:SetAnchorPoint(0,0.5)
    self.worldBoss_maxHarm:SetPosition( worldBoss_dareTime:GetPositionX() , worldBoss_maxHarm:GetPositionY() - ( self.worldBoss_maxHarm:GetContentHeight() ) )
    self.Image_proceedInfoBg1:AddChild( self.worldBoss_maxHarm )

    self.MyHarmRank = Util:Label( CPPGameLib.GetString("worldBoss_NoRank") , nil , Macros.Color.green  )
    PosTool.RightTo( self.worldBoss_maxHarm , self.MyHarmRank , 5 )
    self.Image_proceedInfoBg1:AddChild( self.MyHarmRank )

    --进度条
    local imgdata_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.WORLDBOSS, "bar_sp9_HPstrip")
    local imgdata_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.WORLDBOSS, "bar_sp9_HPbg")

    self.__bar_exp = GUI.ProgressBar9Layer.New(
                                                imgdata_bar, imgdata_bg, 
                                                self.Panel_barHp:GetContentWidth() - 5 , 
                                                self.Panel_barHp:GetContentHeight() - 5 , 
                                                self.Panel_barHp:GetContentWidth() ,
                                                self.Panel_barHp:GetContentHeight() - 2 , 
                                                100, 100
                                            )
    self.__bar_exp:SetAnchorPoint(0.5, 0.5)
    self.Panel_barHp:AddChild(self.__bar_exp:GetNode())
    PosTool.Center(self.__bar_exp)

    --进度条文字
    self.barText = Util:Label( "100/100" , nil , Macros.Color.white )
    self.barText:SetPosition(self.__bar_exp:GetNode():GetPositionX(), self.__bar_exp:GetNode():GetPositionY() )
    self.barText:SetAnchorPoint(0.5,0.5)
    self.Panel_barHp:AddChild(self.barText)

    --模型
    self.modes = LogicGUI.Model.New({parent = self.Panel_modes, model_type = LogicGUI.ModelType.MONSTER , dir = MathTool.HORZ_LEFT })
    self.modes:SetInfoID( 999999 )
    self.modes:GetNode():SetVisible(false)
    self.modes:SetPosition( self.Panel_modes:GetContentWidth()/2 , 0 )

    --回复次数
    self.timeTips = Util:Label(" " , nil , Macros.Color.white )
    self.timeTips:SetAnchorPoint( 0.5 , 1 )
    self.timeTips:SetPosition(self.Button_challenge:GetPositionX() , self.Button_challenge:GetPositionY() - 50 )
    self.Panel_proceed:AddChild(self.timeTips)

    --死亡
    --复活时间
    self.resurgenceTips = Util:Label(" " , nil , Macros.Color.green )
    self.resurgenceTips:SetAnchorPoint( 0.5 , 1 )
    self.resurgenceTips:SetPosition(self.Panel_timeTips:GetContentWidth()/2 , self.Panel_timeTips:GetContentHeight()/2 )
    self.Panel_timeTips:AddChild(self.resurgenceTips)

    --我的荣誉
    local worldBoss_Myhonor_end = Util:Label( CPPGameLib.GetString("worldBoss_Myhonor") , nil , Macros.Color.Special  )
    worldBoss_Myhonor_end:SetAnchorPoint(0,0.5)
    worldBoss_Myhonor_end:SetPosition( worldBoss_dareTime:GetPositionX() , worldBoss_dareTime:GetPositionY() - ( worldBoss_Myhonor_end:GetContentHeight() * 2 ) )
    self.Image_proceedInfoBg2:AddChild( worldBoss_Myhonor_end )

    self.worldBoss_Myhonor_end = Util:Label( " " , nil , Macros.Color.white  )
    self.worldBoss_Myhonor_end:SetAnchorPoint(0,0.5)
    self.worldBoss_Myhonor_end:SetPosition( worldBoss_dareTime:GetPositionX() , worldBoss_Myhonor_end:GetPositionY() - ( self.worldBoss_Myhonor_end:GetContentHeight() ) )
    self.Image_proceedInfoBg2:AddChild( self.worldBoss_Myhonor_end )

    self.MyhonorRank_end = Util:Label( CPPGameLib.GetString("worldBoss_NoRank") , nil , Macros.Color.green  )
    PosTool.RightTo( self.worldBoss_Myhonor_end , self.MyhonorRank_end , 5 )
    self.Image_proceedInfoBg2:AddChild( self.MyhonorRank_end )
   
    --最高伤害
    local worldBoss_maxHarm_end = Util:Label(CPPGameLib.GetString("worldBoss_maxHarm") , nil , Macros.Color.Special )
    worldBoss_maxHarm_end:SetAnchorPoint(0,0.5)
    worldBoss_maxHarm_end:SetPosition( self.worldBoss_Myhonor_end:GetPositionX() , self.worldBoss_Myhonor_end:GetPositionY() - ( worldBoss_maxHarm_end:GetContentHeight() * 2 ) )
    self.Image_proceedInfoBg2:AddChild(worldBoss_maxHarm_end)

    self.worldBoss_maxHarm_end = Util:Label( " " , nil , Macros.Color.white  )
    self.worldBoss_maxHarm_end:SetAnchorPoint(0,0.5)
    self.worldBoss_maxHarm_end:SetPosition( worldBoss_dareTime:GetPositionX() , worldBoss_maxHarm_end:GetPositionY() - ( self.worldBoss_maxHarm_end:GetContentHeight() ) )
    self.Image_proceedInfoBg2:AddChild( self.worldBoss_maxHarm_end )

    self.MyHarmRank_end = Util:Label( CPPGameLib.GetString("worldBoss_NoRank") , nil , Macros.Color.green  )
    PosTool.RightTo( self.worldBoss_maxHarm_end , self.MyHarmRank_end , 5 )
    self.Image_proceedInfoBg2:AddChild( self.MyHarmRank_end )


    local typeLst = {
            [1] = 1,
            [2] = 0,           
            [3] = 2,   
    }
    self.name = {}
    self.tips = {}
    for i = 1 , 3 do
        self.name[ typeLst[i] ] = Util:Label(" 111 ")
        self.name[ typeLst[i] ]:SetAnchorPoint( 0 , 0.5 )
        self.name[ typeLst[i] ]:SetPosition( 20 , self.Image_form_bg[i]:GetContentHeight() -  self.name[ typeLst[i] ]:GetContentHeight() )
        self.Image_form_bg[i]:AddChild( self.name[ typeLst[i] ] )

        local worldBoss_honor = Util:Label( CPPGameLib.GetString("worldBoss_honor") , nil , Macros.Color.white  )
        worldBoss_honor:SetAnchorPoint(0,0.5)
        worldBoss_honor:SetPosition( self.name[ typeLst[i] ]:GetPositionX() , 0 + worldBoss_honor:GetContentHeight() )
        self.Image_form_bg[i]:AddChild( worldBoss_honor )

        self.tips[ typeLst[i] ] = Util:Label(" 111 ")
        PosTool.RightTo( worldBoss_honor , self.tips[ typeLst[i] ]  )
        self.Image_form_bg[i]:AddChild( self.tips[ typeLst[i] ] )
    end

    --未开启
    self.noOpen = Util:Name( CPPGameLib.GetString("worldBoss_openTime" , CallModuleFunc(ModuleType.WORLDBOSS, "OpenTime") ) )
    self.noOpen:SetAnchorPoint( 0.5 , 0.5 )
    self.noOpen:SetPosition( self.Button_challenge:GetPositionX() , self.Button_challenge:GetPositionY() )
    self.Panel_over:AddChild( self.noOpen )

    --弹字框
    self.ballBg = Util:Layout( 600 , 400 )
    self.ballBg:SetAnchorPoint( 0.5 , 0.5 )
    self.ballBg:SetPosition( self.Panel_ui:GetContentWidth()/2 , self.Panel_ui:GetContentHeight()/2 )
    self.Panel_ui:AddChild( self.ballBg )

    local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
    self.shopRed = Util:Sprite( icon_red_point )
    self.shopRed:SetAnchorPoint(0.5,0.5)
    self.shopRed:SetPosition( self.Button_shop:GetContentWidth() , self.Button_shop:GetContentHeight() )
    self.Button_shop:AddChild( self.shopRed )

    self.awardRed = Util:Sprite( icon_red_point )
    self.awardRed:SetAnchorPoint(0.5,0.5)
    self.awardRed:SetPosition( self.Button_award:GetContentWidth() , self.Button_award:GetContentHeight() )
    self.Button_award:AddChild( self.awardRed )
end

function Modules.WorldBossView:SetShowUI() 
    self.Panel_proceed:SetVisible(false)
    self.Panel_over:SetVisible(false)

    --获得boss生存状态
    local bossHP = CallModuleFunc(ModuleType.WORLDBOSS, "GetBossHP")
    if bossHP ~= 0 then
        self.Panel_proceed:SetVisible(true)
    else
        self.modes:SetGray( true )
        self.modes:SetToPlayTime( 0 )
        
        self.Panel_proceed:SetVisible(false)
        self.Panel_over:SetVisible(true)
        self.Image_honor:SetVisible(false)
    end

    --开启没有
    local isOpen = CallModuleFunc(ModuleType.WORLDBOSS, "isOpen")
    if isOpen == false then
        --模型置灰
        self.modes:SetGray( true )
        self.modes:SetToPlayTime( 0 )
        --战斗进行屏蔽
        self.Panel_proceed:SetVisible(false)
        self.Panel_over:SetVisible(true)
        self.Image_honor:SetVisible(true)

        self.noOpen:SetVisible(true)
        --击杀标志
        self.Image_12:SetVisible(false)
    else
        self.noOpen:SetVisible(false)
    end

    --战功
    self.worldBoss_exploits:SetString( CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.REBEL_VALUE) )

    --总次数
    local challengeTime = CallModuleFunc(ModuleType.WORLDBOSS, "GetNowChallengeTime")
    self.worldBoss_dareTime:SetString( challengeTime )

    --我的荣誉
    local honor , honorRank = CallModuleFunc(ModuleType.WORLDBOSS, "GetBossHonor")
    self.worldBoss_Myhonor:SetString( honor )
    if honorRank ~= 0 then
        self.MyhonorRank:SetString( CPPGameLib.GetString( "worldBoss_Rank" , honorRank ) )
    end
    PosTool.RightTo( self.worldBoss_Myhonor , self.MyhonorRank , 5 )

    --最高伤害
    local honor , honorRank = CallModuleFunc(ModuleType.WORLDBOSS, "GetBossDamage")
    self.worldBoss_maxHarm:SetString( honor )
    if honorRank ~= 0 then
        self.MyHarmRank:SetString( CPPGameLib.GetString( "worldBoss_Rank" , honorRank ) )
    end
    PosTool.RightTo( self.worldBoss_maxHarm , self.MyHarmRank , 5 )

    
    local hp , hpMax = CallModuleFunc(ModuleType.WORLDBOSS, "GetBossHP")
    --进度条字样
    self.barText:SetString( hp .. "/" .. hpMax )
    --进度条
    self.__bar_exp:SetProgress( hp / hpMax * 100 )

    --设置模型
    local bossLeve = CallModuleFunc(ModuleType.WORLDBOSS, "GetBossLevel")

    print("xy" , "bossLeve" , bossLeve )
    local bossCnf = CPPGameLib.GetConfig("WorldBoss", bossLeve )
    print("xy" , "GameLevel" , bossCnf.map_id )
    local cnf = CPPGameLib.GetConfig("GameLevel", tonumber(bossCnf.map_id))
     print("xy" , "SetInfoID" , cnf.icon_id )
    self.modes:SetInfoID( cnf.icon_id )

    CPPActionManager.DelayTo(self.Panel_modes , 0.2 , GlobalCallbackMgr:AddCallBackFunc( function( ... )
        self.modes:GetNode():SetVisible(true)
    end ) )

    --设置名字
    if self.bossName then
        self.bossName:RemoveSelf()
        self.bossName = nil
    end
    --名字
    self.bossName = Util:LabCenter({
                                     { zi = cnf.name, ys = 4 , mb = true, dx = Macros.TypefaceSize.popup },
                                     { zi =  CPPGameLib.GetString("common_lev6" , bossLeve ) , ys = 0 , mb = true, dx = Macros.TypefaceSize.popup , jg = 10 },
                                    })
    self.bossName:SetPosition(self.Panel_name:GetContentWidth() / 2 , self.Panel_name:GetContentHeight() / 2 )
    self.bossName:SetAnchorPoint(0.5,0.5)
    self.Panel_name:AddChild(self.bossName)

    --我的荣誉2
    local honor , honorRank = CallModuleFunc(ModuleType.WORLDBOSS, "GetBossHonor")
    self.worldBoss_Myhonor_end:SetString( honor )
    self.MyhonorRank_end:SetString( CPPGameLib.GetString( "worldBoss_Rank" , honorRank ) )
    PosTool.RightTo( self.worldBoss_Myhonor_end , self.MyhonorRank_end , 5 )

    --最高伤害2
    local honor , honorRank = CallModuleFunc(ModuleType.WORLDBOSS, "GetBossDamage")
    self.worldBoss_maxHarm_end:SetString( honor )
    self.MyHarmRank_end:SetString( CPPGameLib.GetString( "worldBoss_Rank" , honorRank ) )
    PosTool.RightTo( self.worldBoss_maxHarm_end , self.MyHarmRank_end , 5 )

    --榜单
    local lst = CallModuleFunc(ModuleType.WORLDBOSS, "GetInterfaceRank")
    for i,v in ipairs(lst) do
        local name = v.role_name
        local honor = v.honor
        if v.role_name == "" then
            name = CPPGameLib.GetString("worldBoss_NoRank")
            honor = " "
        end
        self.name[ i - 1 ]:SetString( name )
        self.tips[ i - 1 ]:SetString( honor )
    end

    
    local index = CallModuleFunc(ModuleType.REBEL,"GetNowBuyNum")
    if index ~= 0 then
        self.shopRed:SetVisible(true)
    else
        self.shopRed:SetVisible(false)
    end
end

--荣誉榜
function Modules.WorldBossView:OnHonor(index)
    print("xy","荣誉榜")
    LogicGUI.TemplWorldbossHonor.New( Macros.Game.RankType.WORLDBOSS_HONOR )
end

--伤害榜
function Modules.WorldBossView:OnHarm(index)
    print("xy","伤害榜")
    -- LogicGUI.TemplWorldbossHarm.New()
    LogicGUI.TemplWorldbossHonor.New( Macros.Game.RankType.WORLDBOSS_HARM )
end

--奖励
function Modules.WorldBossView:OnDaward(index)
    print("xy","奖励")
    LogicGUI.TemplWorldbossAward.New()
end

--战功商店
function Modules.WorldBossView:OnButtonShop(index)
    print("xy","战功商店")
    CallModuleFunc(ModuleType.REBEL, "OpenView", BaseViewType.REBELSHOP)
end

--战报
function Modules.WorldBossView:OnScore(index)
    print("xy","战报")
    LogicGUI.TemplWorldbossScore.New()
end

--添加次数
function Modules.WorldBossView:OnButtonAddTime(index)
    print("xy","添加次数")
    LogicGUI.TemplWorldbossBuytime.New()
end

--攻击
function Modules.WorldBossView:OnButtonHit(index)
    local camp = CallModuleFunc(ModuleType.WORLDBOSS, "GetCamp")
    if camp == Macros.Game.FORCE.CAMP_INVALID then
        LogicGUI.TemplWorldbossGroup.New()
    else
        local checkBattle = CallModuleFunc(ModuleType.WORLDBOSS, "CheckBattleCondition")
        if checkBattle then
            CallModuleFunc(ModuleType.BATTLE, "BattleStartReq", Macros.Game.FBType.WORLD_BOSS)
        else
            GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("worldBoss_notime") )
        end
    end
end

--复活时间
function Modules.WorldBossView:TimeRelive(index)
    local now = CallModuleFunc(ModuleType.WORLDBOSS, "GetReliveTime")
    if now <= 0 then
        return
    end


    self.modes:SetGray( true )
    self.modes:SetToPlayTime( 0 )

    local func = nil
    local function moveAnimate()
        local runTime = CallModuleFunc(ModuleType.WORLDBOSS, "GetReliveTime")
        if runTime <=0 then
            self.resurgenceTips:SetString(" ")
            self.modes:SetGray( false )
            self.modes:SetToPlayTime( nil )
            CallModuleFunc(ModuleType.WORLDBOSS, "WorldBossDataReq")
        else

            self.modes:SetGray( true )
            self.modes:SetToPlayTime( 0 )

            local lab = "MM"..CPPGameLib.GetString("common_minute").."SS"..CPPGameLib.GetString("common_second")..CPPGameLib.GetString("worldBoss_resurgence")
            self.resurgenceTips:SetString(TimeTool.TransTimeStamp(lab, runTime))

            CPPActionManager.DelayTo(self.resurgenceTips,1,func)
        end
    end
    func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
    moveAnimate()
end

-- 世界BOSS这个倒计时可以去掉了，服务端已经注销了，倒计时恢复次数
-- --回复倒计时
-- function Modules.WorldBossView:TimeReply(index)
--     local now = CallModuleFunc(ModuleType.WORLDBOSS, "GetReplyTime")
--     if now <= 0 then
--         return
--     end

--     local func = nil
--     local function moveAnimate()
--         local runTime = CallModuleFunc(ModuleType.WORLDBOSS, "GetReplyTime")
--         if runTime <=0 then
--             self.timeTips:SetString(" ")
--             CallModuleFunc(ModuleType.WORLDBOSS, "WorldBossDataReq")
--         else
--             local lab = "MM"..CPPGameLib.GetString("common_minute").."SS"..CPPGameLib.GetString("common_second")..CPPGameLib.GetString("worldBoss_reply")
--             self.timeTips:SetString(TimeTool.TransTimeStamp(lab, runTime))

--             CPPActionManager.DelayTo(self.timeTips,1,func)
--         end
--     end
--     func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
--     moveAnimate()
-- end

--选择阵营
function Modules.WorldBossView:SelectCamp(index)
    local camp = CallModuleFunc(ModuleType.WORLDBOSS, "GetCamp")
    local isOpen = CallModuleFunc(ModuleType.WORLDBOSS, "isOpen")
    if camp == Macros.Game.FORCE.CAMP_INVALID and isOpen ~= false then
        LogicGUI.TemplWorldbossGroup.New()
    end
end

--红点
function Modules.WorldBossView:SetRed(index)
    local t1 = {
            [1] = Modules.WorldBossModule.REWARD_TYPE.HONOR , 
            [2] = Modules.WorldBossModule.REWARD_TYPE.KILL,
    }
    local index = 0
    for i = 1 , #t1 do
        local lst = CallModuleFunc(ModuleType.WORLDBOSS, "AutoGet" , t1[i] )
        if lst and #lst.reward_list ~= 0 then
            index = index + 1
        end
    end

    if index ~= 0 then
        self.awardRed:SetVisible(true)
    else
        self.awardRed:SetVisible(false)
    end
end