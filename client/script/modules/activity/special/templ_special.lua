--
-- @brief 节日活动
-- @author: yjg
-- @date: 2017年9月13日17:02:30
--

Modules = Modules or {}
Modules.TemplSpecialItem = Modules.TemplSpecialItem or BaseClass(GUI.ScrollItem)

Modules.TemplSpecialItem.COMMON = {
            --单充
            [ Macros.Game.ACTIVITY_FESTIVAL.ONCE_PAY ] = true , 
            --累充
            [ Macros.Game.ACTIVITY_FESTIVAL.ADDUP_PAY ] = true , 
            --登录
            [ Macros.Game.ACTIVITY_FESTIVAL.ENTER ] = true , 
            --主线
            [ Macros.Game.ACTIVITY_FESTIVAL.MEAN_LINE ] = true , 
            --圣器
            [ Macros.Game.ACTIVITY_FESTIVAL.PYX ] = true , 
            --竞技场
            [ Macros.Game.ACTIVITY_FESTIVAL.ARENA ] = true ,
            --深渊 
            [ Macros.Game.ACTIVITY_FESTIVAL.CHASM ] = true , 
            --召唤
            [ Macros.Game.ACTIVITY_FESTIVAL.CALL ] = true , 
            --开服登录
            [ Macros.Game.ACTIVITY_FESTIVAL.OPEN_SERVICE ] = true , 
            --组队副本通关
            [ Macros.Game.ACTIVITY_FESTIVAL.TEAM_PLAYER ] = true , 
            --每日通关精英副本
            [ Macros.Game.ACTIVITY_FESTIVAL.EXERYDAY_ELITE ] = true , 
            --每日战胜勇者试炼精英关卡
            [ Macros.Game.ACTIVITY_FESTIVAL.EXERYDAY_TOWER_ELITE ] = true , 
            --累计勇者试炼重置
            [ Macros.Game.ACTIVITY_FESTIVAL.OVERALL_TRIAL ] = true , 
            --累计攻击叛军
            [ Macros.Game.ACTIVITY_FESTIVAL.OVERALL_REBEL ] = true , 
            --累计挑战无尽深渊
            [ Macros.Game.ACTIVITY_FESTIVAL.OVERALL_ACCRO ] = true , 
            --累计挑战王者赛
            [ Macros.Game.ACTIVITY_FESTIVAL.OVERALL_KING ] = true , 
            --累计占卜翻牌
            [ Macros.Game.ACTIVITY_FESTIVAL.OVERALL_LOW_AUGURY ] = true , 
            --累计高级占卜翻牌
            [ Macros.Game.ACTIVITY_FESTIVAL.OVERALL_HIGH_AUGURY ] = true , 

            --历收藏值最高达到
            [ Macros.Game.ACTIVITY_FESTIVAL.CARD_COLLECT ] = true , 
            --活动期间消耗
            [ Macros.Game.ACTIVITY_FESTIVAL.ACTIVITY_CONSUME ] = true , 
            --英雄培养
            [ Macros.Game.ACTIVITY_FESTIVAL.HERO_TRAIN ] = true , 
            --精灵培养
            [ Macros.Game.ACTIVITY_FESTIVAL.STRING_TRAIN ] = true , 
            --累计扭蛋次数
            [ Macros.Game.ACTIVITY_FESTIVAL.TOMY_ITEMS ] = true , 

            --累计装备精炼次数
            [ Macros.Game.ACTIVITY_FESTIVAL.EQUIP_REFINE ] = true , 

            --累计特殊扭蛋次数
            [ Macros.Game.ACTIVITY_FESTIVAL.ND_LOTTERY ] = true , 
    }

Modules.TemplSpecialItem.DISCOUNT = {
            --打折
            [ Macros.Game.ACTIVITY_FESTIVAL.SALE ] = true , 
            --兑换
            [ Macros.Game.ACTIVITY_FESTIVAL.CONVER ] = true , 
    }

function Modules.TemplSpecialItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplSpecialItem:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(639,137)
    self.layout:SetClippingEnable(false)
    self.layout:SetTouchEnabled(false)
    self.layout:SetBackGroundColor(0, 0, 0)

    return self.layout
end

function Modules.TemplSpecialItem:__delete()
end

function Modules.TemplSpecialItem:__AddToParent()
end

function Modules.TemplSpecialItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self:__ShowUI(data.cnf)
end

function Modules.TemplSpecialItem:__ShowUI( data )
    --货架
    self.sp_activity_rack = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY_NEW , "sp_activity_rack" ) )
    self.sp_activity_rack:SetAnchorPoint( 0.5,0.5 )
    self.layout:AddChild( self.sp_activity_rack )
    PosTool.Center( self.sp_activity_rack )

    --按钮设置
    if not self.button then
        self.button = Util:Button( 
                                    Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW , "btn_btn_biggreen2" ) , 
                                    function ( ... )

                                    end , nil, Macros.TypefaceSize.popup 
                                )
        self.button:SetAnchorPoint( 0.5 , 0.5 )
        self.button:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 - 10 )
        self.sp_activity_rack:AddChild( self.button )
        self.button:SetVisible(false)


        local btnEffect_con = nil
        self.btnEffect_id , btnEffect_con = self:__CreateEffect({res_name = "ui_anniuzoubian2"} , self.button )
        btnEffect_con:SetPosition(self.button:GetContentWidth()/2 , self.button:GetContentHeight()/2 )
        GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , false)
    end

    --普通奖励物品
    if Modules.TemplSpecialItem.COMMON[ data.logic_type ] then
        self:__ShowCommonAwardIcon( data )
    --打折奖励物品
    elseif Modules.TemplSpecialItem.DISCOUNT[ data.logic_type ] then
        self:__ShowDiscountAwardIcon( data ) 
    --特殊处理
    else
        if data.logic_type >= 21 and data.logic_type <= 30 then
            self:__ShowCommonAwardIcon( data )
        end
    end

    self:SetShowUI( data )
end


--普通奖励物品
function Modules.TemplSpecialItem:__ShowCommonAwardIcon( data )

    if not data.reward_item_id_list then
        return 
    end

    for i , v in ipairs( data.reward_item_id_list ) do
        local sp_activity_packBG = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY_NEW , "sp_activity_packBG" ) )
        sp_activity_packBG:SetPosition( 60 + 100 * (i - 1) , self.sp_activity_rack:GetContentHeight()/2 - 45 )
        sp_activity_packBG:SetAnchorPoint( 0.5 , 0.5 )
        self.sp_activity_rack:AddChild(sp_activity_packBG)

        local icon = LogicGUI.IconLayer.New()
        icon:SetData( {id = v , num = data.reward_item_num_list[i] } )
        icon:SetPosition( sp_activity_packBG:GetContentWidth()/2 , sp_activity_packBG:GetContentHeight()/2 + 1 )
        icon:SetAnchorPoint( 0.5 , 0 )
        icon:SetTouchSwallow(false)
        sp_activity_packBG:AddChild(icon:GetNode())
    end
end

--打折奖励物品
function Modules.TemplSpecialItem:__ShowDiscountAwardIcon( data )

    local id , num = nil , nil
    if data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.SALE then
        local config = CPPGameLib.GetConfig("HuoBi", Macros.Game.RolePropType.DIAMOND )
        id = config.big_icon
        num = data.x
    elseif data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.CONVER then
        id = data.x
        num = data.y
    end

    local sp_activity_packBG = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY_NEW , "sp_activity_packBG" ) )
    sp_activity_packBG:SetPosition( 60 , self.sp_activity_rack:GetContentHeight()/2 - 45 )
    sp_activity_packBG:SetAnchorPoint( 0.5 , 0.5 )
    self.sp_activity_rack:AddChild(sp_activity_packBG , 5 )

    local icon = LogicGUI.IconLayer.New()
    icon:SetData( {id = id , num = num } )
    icon:SetPosition( sp_activity_packBG:GetContentWidth()/2 , sp_activity_packBG:GetContentHeight()/2 + 1 )
    icon:SetAnchorPoint( 0.5 , 0 )
    icon:SetTouchSwallow(false)
    sp_activity_packBG:AddChild(icon:GetNode())

    if data.discount then
        local discount = LogicGUI.LconDiscount.New( data.discount or 5 )
        discount:SetAnchorPoint( 0.5 , 0.5 )
        discount:SetPosition( 12 , icon:GetNode():GetContentHeight() + 5 )
        icon:AddChild( discount:GetNode() , 999 )
    end

    local sp_activity_equal = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW , "sp_activity_equal" ) )
    sp_activity_equal:SetPosition( 60 + 75 , self.sp_activity_rack:GetContentHeight()/2 )
    sp_activity_equal:SetAnchorPoint( 0.5 , 0.5 )
    self.sp_activity_rack:AddChild(sp_activity_equal)

    if not data.reward_item_id_list then
        return 
    end

    for i , v in ipairs( data.reward_item_id_list ) do
        local sp_activity_packBG = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY_NEW , "sp_activity_packBG" ) )
        sp_activity_packBG:SetPosition( 210 + 100 * (i - 1) , self.sp_activity_rack:GetContentHeight()/2 - 45 )
        sp_activity_packBG:SetAnchorPoint( 0.5 , 0.5 )
        self.sp_activity_rack:AddChild(sp_activity_packBG)

        local icon = LogicGUI.IconLayer.New()
        icon:SetData( {id = v , num = data.reward_item_num_list[i] } )
        icon:SetPosition( sp_activity_packBG:GetContentWidth()/2 , sp_activity_packBG:GetContentHeight()/2 + 1 )
        icon:SetAnchorPoint( 0.5 , 0 )
        icon:SetTouchSwallow(false)
        sp_activity_packBG:AddChild(icon:GetNode())
    end
end

--兑换奖励物品
function Modules.TemplSpecialItem:__ShowExchangeAwardIcon( data )
    if not data.reward_item_id_list then
        return 
    end

    for i , v in ipairs( data.reward_item_id_list ) do
        local sp_activity_packBG = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ACTIVITY_NEW , "sp_activity_packBG" ) )
        sp_activity_packBG:SetPosition( 60 + 100 * (i - 1) , self.sp_activity_rack:GetContentHeight()/2 - 45 )
        sp_activity_packBG:SetAnchorPoint( 0.5 , 0.5 )
        self.sp_activity_rack:AddChild(sp_activity_packBG)

        local icon = LogicGUI.IconLayer.New()
        icon:SetData( {id = v , num = data.reward_item_num_list[i] } )
        icon:SetPosition( sp_activity_packBG:GetContentWidth()/2 , sp_activity_packBG:GetContentHeight()/2 + 1 )
        icon:SetAnchorPoint( 0.5 , 0 )
        icon:SetTouchSwallow(false)
        sp_activity_packBG:AddChild(icon:GetNode())
    end
end


function Modules.TemplSpecialItem:SetShowUI( data )
    --描述
    if not self.desc then
        self.desc = Util:RichText( "" , Macros.TypefaceSize.largish , Macros.Color.white_rt )
        self.desc:SetAnchorPoint( 0 , 1 )
        self.desc:SetPosition( 20 , self.sp_activity_rack:GetContentHeight() - 5 )
        self.sp_activity_rack:AddChild( self.desc )
    end

    --领取次数
    if not self.showittem then
        self.showittem = Util:Label( "" , Macros.TypefaceSize.largish , Macros.Color.white )
        self.showittem:SetAnchorPoint( 0.5 , 0.5 )
        self.showittem:SetPosition( 530 , self.sp_activity_rack:GetContentHeight()/2 + 40 )
        self.sp_activity_rack:AddChild( self.showittem )
    end

    if data.showittem == 1 then

        CPPGameLib.PrintTable("xy" , self.__data , "请求物品列表")

        local _ , info = CallModuleFunc(ModuleType.ACTIVITY,"GetFestivalActivityItem" , self.__data.id , self.__data.type_ )
        local num = 0
        if info then
            num = info.take_reward_times
        end
        self.showittem:SetString( CPPGameLib.GetString("activity_residue" , data.reward_times - num ) )
    end

    -- 单充
    if data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.ONCE_PAY then
        self:OnOncePay( self.__data )
    --累充
    elseif data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.ADDUP_PAY then
        self:OnAddPpPay( self.__data )
    --打折
    elseif data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.SALE then       
        self:OnSalePay( self.__data )
    --兑换
    elseif data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.CONVER then     
        self:OnConverPay( self.__data )
    --节日登录
    elseif data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.ENTER then     
        self:OnEnter( self.__data )
    --主线
    elseif data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.MEAN_LINE then  
        self:OnMeanLine( self.__data )
    --圣器
    elseif data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.PYX then      
        self:OnPYX( self.__data )
    --竞技场
    elseif data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.ARENA then     
        self:OnArena( self.__data )
    --深渊
    elseif data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.CHASM then       
        self:OnChasm( self.__data )
    --召唤
    elseif data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.CALL then     
        self:OnCall( self.__data )
    --开服登录
    elseif data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.OPEN_SERVICE then    
        self:OpenService( self.__data )
    --组队副本通关
    elseif data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.TEAM_PLAYER then    
        self:OnTeamPlayer( self.__data )

    --每日通关精英副本
    elseif data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.EXERYDAY_ELITE then    
        self:OnExerydayElite( self.__data )

    --每日战胜勇者试炼精英关卡
    elseif data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.EXERYDAY_TOWER_ELITE then    
        self:OnOverallTrial( self.__data )

    --累计勇者试炼重置
    elseif data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.OVERALL_TRIAL then    
        self:OnOverallTrial( self.__data )

    --累计攻击叛军
    elseif data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.OVERALL_REBEL then    
        self:OnOverallRebel( self.__data )

    --累计挑战无尽深渊
    elseif data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.OVERALL_ACCRO then    
        self:OnOverallAccro( self.__data )

    --累计挑战王者赛
    elseif data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.OVERALL_KING then    
        self:OnOverallKing( self.__data )

    --累计占卜翻牌
    elseif data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.OVERALL_LOW_AUGURY then    
        self:OnOverallLowAugury( self.__data )

    --累计高级占卜翻牌
    elseif data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.OVERALL_HIGH_AUGURY then    
        self:OnOverallHighAugury( self.__data )

    --历收藏值最高达到
    elseif data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.CARD_COLLECT then    
        self:OnCardCollect( self.__data )
    --活动期间消耗
    elseif data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.ACTIVITY_CONSUME then    
        self:OnActivityConsume( self.__data )

    --英雄培养
    elseif data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.HERO_TRAIN then    
        self:OnHeroTrain( self.__data )

    --精灵培养
    elseif data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.STRING_TRAIN then    
        self:OnStringTrain( self.__data )

    --累计扭蛋次数
    elseif data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.TOMY_ITEMS then    
        self:OnTomyItems( self.__data )

    --累计装备精炼次数
    elseif data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.EQUIP_REFINE then    
        self:OnEquipRefine( self.__data )

    --累计特殊扭蛋次数
    elseif data.logic_type == Macros.Game.ACTIVITY_FESTIVAL.ND_LOTTERY then    
        self:OnNDLottery( self.__data )

    --特殊处理
    else
        --"同时拥有1张10级的卡牌"
        if data.logic_type >= 21 and data.logic_type <= 30 then
            self:OnCardActivity( self.__data )
        end
    end
end

--发送
function Modules.TemplSpecialItem:Sell( data )
    local cnf = CPPGameLib.GetConfig("FestivalActivityParam", data.id )
    local t1 = {}
    for i,v in ipairs( cnf.reward_item_id_list ) do
        table.insert( t1 , { id = v , num = cnf.reward_item_num_list[i] } )
    end

    CallModuleFunc(ModuleType.SUNDRY, "LstPackVacancy", t1 , true , function ( text )
        if #text == 0 then
            local info = {}
            info.activity_type = data.type_
            info.target_id = data.id
            CallModuleFunc(ModuleType.ACTIVITY,"TakeFestivalActivityRewardReq" , info )
        end
    end)
end

--发送
function Modules.TemplSpecialItem:Exchange( data , index)

    local cnf = CPPGameLib.GetConfig("FestivalActivityParam", data.id )
    local t1 = {}
    for i,v in ipairs( cnf.reward_item_id_list ) do
        table.insert( t1 , { id = v , num = cnf.reward_item_num_list[i] * index  } )

    end

    CallModuleFunc(ModuleType.SUNDRY, "LstPackVacancy", t1 , true , function ( text )
        if #text == 0 then
            local info = {}
            info.activity_type = data.type_
            info.target_id = data.id
            info.exchange_num = index
            CallModuleFunc(ModuleType.ACTIVITY,"FestivalActivityGoodsExchangeReq" , info )
        end
    end)
end

