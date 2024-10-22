--[[
%% @module: 女王
%% @author: yjg
%% @created: 2018年2月5日16:54:37
--]]

Modules = Modules or {} 
Modules.QueenView = Modules.QueenView or BaseClass(BaseView)

function Modules.QueenView:__init()
    self.__layout_name = "recharge_queen"	
    self.__bg_params = {type = BaseView.BGType.SOLID} 

    self.__open_callback = function()
      self:__OpenCallback()
    end
end

function Modules.QueenView:__OpenCallback( )
    self:__Getwidget()
    self:__ShowUI()
    self:__Server()
end	

function Modules.QueenView:__Server( )
    self:SetShowUi()

    self:BindGlobalEvent(Macros.Event.QueenModule.QUEEN_DATA , function (_, data)
        self:SetShowUi()
    end)

    self:BindGlobalEvent(Macros.Event.QueenModule.QUEEN_COUNTDOWN , function (_, time)
        self:Countdown(time)
    end)
    CallModuleFunc(ModuleType.QUEEN , "Countdown")
end

function Modules.QueenView:__Dispose()
end

function Modules.QueenView:__Getwidget( )
    --背景
    self.Image_bg = self:GetWidget("Image_bg") 
    --标题
    self.Image_headline = self:GetWidget("Image_headline")
    --充值背景
    self.Image_recharge = self:GetWidget("Image_recharge") 
    self.Image_recharge:SetImage( Resource.PathTool.GetUIResPath("queen/sp_queen_money"), TextureResType.LOCAL)
    --充值字
    self.Image_recharge_text2 = self:GetWidget("Image_recharge_text2")  
    self.Image_recharge_text2:IgnoreContentAdaptWithSize(true)
    self.Image_recharge_text2:SetImage( Resource.PathTool.GetUIResPath("queen/sp_queen_recharge",true), TextureResType.LOCAL)

    self.Image_iconBg = {}
    self.Image_currency = {}
    self.Image_green = {}

    for i = 1 , 4 do
        if self:GetWidget("Image_iconBg" .. i ) then
            --图标背景
            self.Image_iconBg[i] = self:GetWidget("Image_iconBg" .. i ) 
            self.Image_iconBg[i]:SetImage( Resource.PathTool.GetUIResPath("queen/sp_queen_circle"), TextureResType.LOCAL)
            self.Image_iconBg[i]:IgnoreContentAdaptWithSize(true)
            self.Image_iconBg[i]:SetVisible(false)
        end
        if self:GetWidget("Image_currency" .. i ) then
            --货币背景
            self.Image_currency[i] = self:GetWidget("Image_currency" .. i )  
            self.Image_currency[i]:SetImage(Resource.PathTool.GetUIResPath("queen/sp_queen_currency"), TextureResType.LOCAL)
            self.Image_currency[i]:IgnoreContentAdaptWithSize(true)
            self.Image_currency[i]:SetVisible(false)
        end
        if self:GetWidget("Image_green" .. i , false ) then
            --绿色箭头
            self.Image_green[i] = self:GetWidget("Image_green" .. i ) 
            self.Image_green[i]:SetVisible(false)
        end
    end

    local cnf = CallModuleFunc(ModuleType.QUEEN , "GetTotalRechargeRewardCnf") or {} 
    for i = 1 , #cnf - 1 do
        --图标背景
        self.Image_iconBg[i]:SetVisible(true)
        --货币背景
        self.Image_currency[i]:SetVisible(true)
        if i < (#cnf - 1) then
            --绿色箭头
            self.Image_green[i]:SetVisible(true)
        end
    end

    --领取奖励
    self.Button_draw = Util:WidgetButton( 
                                        self:GetWidget("Button_draw") , 
                                        " " , 
                                        CPPGameLib.Handler(self,self.OnButtonDraw),
                                        nil,nil,Macros.Color.button_yellow
                                    )

    self.Image_draw_text = self:GetWidget("Image_draw_text")

    --关闭
    self.Button_close = Util:WidgetButton( 
                                        self:GetWidget("Button_close") , 
                                        " " , 
                                        CPPGameLib.Handler(self,self.OnButtonClose),
                                        nil,nil,Macros.Color.button_yellow
                                    )

    --快速充值
    self.Button_recharge = Util:WidgetButton( 
                                        self:GetWidget("Button_recharge") , 
                                        " " , 
                                        CPPGameLib.Handler(self,self.OnButtonRecharge),
                                        nil,nil,Macros.Color.button_yellow
                                    )
    --充值字
    self.Image_recharge_text1 = self:GetWidget("Image_recharge_text1")  
    self.Image_recharge_text1:IgnoreContentAdaptWithSize(true)
    self.Image_recharge_text1:SetImage( Resource.PathTool.GetUIResPath("queen/sp_queen_rapid",true), TextureResType.LOCAL)
end 

function Modules.QueenView:__ShowUI( )

    local info = CallModuleFunc(ModuleType.ACTIVITY, "ToIdGetActivityTime" , Macros.Game.SystemID.QUEEN )
    -- info.subtype = 22
    local cnfLst = CPPGameLib.GetConfig("TotalRechargeUI", nil , false , true )
    local sta = nil
    for k , v in pairs( cnfLst ) do
        if not sta then
            sta = v.data_id
        end
        if v.data_id < sta then
            sta = v.data_id
        end   
    end

    info.subtype = info.subtype or sta

    local cnf = CPPGameLib.GetConfig("TotalRechargeUI", info.subtype )

    --背景
    local bg = Util:Sprite( Resource.PathTool.GetUIResPath("queen/".. cnf.bg ) )
    bg:SetAnchorPoint( 0.5 , 0.5 )
    self.Image_bg:AddChild( bg , -10 )
    PosTool.Center( bg )
    if cnf.bg_pos then
        PosTool.Center( bg , tonumber(cnf.bg_pos[1]) , tonumber(cnf.bg_pos[2]) )
    end

    --精灵
    if cnf.sprite then
        for i , v in ipairs( cnf.sprite ) do
            local sprite = Util:Sprite( Resource.PathTool.GetHalfHeadPath( v ) )
            sprite:SetAnchorPoint( 0.5 , 0.5 )
            bg:AddChild( sprite )
            PosTool.Center( sprite )
            if cnf.sprite_pos then
                PosTool.Center( sprite , cnf.sprite_pos[i][1] , cnf.sprite_pos[i][2] )
            end
            if cnf.sprite_sc_lst then
                sprite:SetScaleFactor( cnf.sprite_sc_lst[i] )
            end
            if cnf.sprite_zo_lst then
                sprite:SetZOrder( cnf.sprite_zo_lst[i] )
            end
        end
    end

    --图片
    if cnf.sp then
        for i , v in ipairs( cnf.sp ) do
            local sp = Util:Sprite( Resource.PathTool.GetUIResPath("queen/" .. v ) )
            sp:SetAnchorPoint( 0.5 , 0.5 )
            bg:AddChild( sp )
            PosTool.Center( sp )
            if cnf.sp_pos_lst then
                PosTool.Center( sp , cnf.sp_pos_lst[i][1] , cnf.sp_pos_lst[i][2] )
            end
            if cnf.sp_zo_lst then
                sp:SetZOrder( cnf.sp_zo_lst[i] )
            end

        end
    end

    --中文图片
    if cnf.spl then
        for i , v in ipairs( cnf.spl ) do
            local spl = Util:Sprite( Resource.PathTool.GetUIResPath("queen/" .. v , true) )
            spl:SetAnchorPoint( 0.5 , 0.5 )
            bg:AddChild( spl )
            PosTool.Center( spl )
            if cnf.spl_pos_lst then
                PosTool.Center( spl , cnf.spl_pos_lst[i][1] , cnf.spl_pos_lst[i][2] )
            end
            if cnf.spl_zo_lst then
                spl:SetZOrder( cnf.spl_zo_lst[i] )
            end
        end
    end

    --特效
    if cnf.effects_lst then
        for i , v in ipairs( cnf.effects_lst ) do
            local zorder = nil
            if cnf.effects_zob_lst[i] then
                zorder = cnf.effects_zob_lst[i]
            end
            local btnEffect_id , btnEffect_con = self:__CreateEffect({ res_name = v } , bg )
            btnEffect_con:SetAnchorPoint( 0.5,0.5 )
            PosTool.Center( btnEffect_con )
            if cnf.effects_pos_lst then
                PosTool.Center( btnEffect_con , cnf.effects_pos_lst[i][1] , cnf.effects_pos_lst[i][2] )
            end
        end
    end

    --活动剩余时间
    local queen_time = Util:Name( CPPGameLib.GetString("monster_stormcastle_residuetimetxt") )
    queen_time:SetAnchorPoint( 0 , 0.5 )
    -- queen_time:SetPosition( 500 , 475 )
    bg:AddChild( queen_time )
    PosTool.Center( queen_time )

    if cnf.time_pos_lst then
        queen_time:SetPosition( cnf.time_pos_lst[1] , cnf.time_pos_lst[2] )
    end

    self.queen_time = Util:Name( " " , nil , 1 )
    bg:AddChild( self.queen_time )
    PosTool.RightTo( queen_time , self.queen_time )

    --按钮特效
    local btnEffect_con = nil
    self.btnEffect_id , btnEffect_con = self:__CreateEffect({res_name = "ui_anniuzoubian2"} , self.Button_draw )
    btnEffect_con:SetPosition(self.Button_draw:GetContentWidth()/2 , self.Button_draw:GetContentHeight()/2 )
    GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , false)

    local brnEffect_id , btnEffect_con = self:__CreateEffect({res_name = "UI_chongzhitongyong"} , self.Button_recharge )
    btnEffect_con:SetPosition(self.Button_recharge:GetContentWidth()/2 , self.Button_recharge:GetContentHeight()/2 )

    --容器
    local layou = Util:Layout( 380 , 100 )
    layou:SetAnchorPoint( 0.5 , 0.5 )
    layou:SetPosition( self.Button_draw:GetPositionX() , self.Button_draw:GetPositionY() + 85 )
    self.Image_bg:AddChild( layou )

    --道具列表
    local lst = CallModuleFunc(ModuleType.QUEEN , "GetAwardPackage")

    local interval = layou:GetContentWidth() / #lst
    for i , v in ipairs( lst ) do
        local icon = LogicGUI.IconLayer.New(true)
        icon:SetPosition( (interval/2) + interval * (i - 1) , layou:GetContentHeight()/2 )
        icon:SetData({ id = v.id , num = v.num })
        icon:SetAnchorPoint(0.5,0.5)
        layou:AddChild(icon:GetNode())
    end

    --累计充值
    self.recharge = Util:Label( "000/000" , Macros.TypefaceSize.largish , Macros.Color.golden )
    self.recharge:SetAnchorPoint( 0.5 , 0.5 )
    self.Image_recharge:AddChild( self.recharge )
    PosTool.Center( self.recharge )

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_seven_get" , true)
    self.seven = Util:Sprite( sp )
    self.seven:SetAnchorPoint( 0.5 , 0.5 )
    self.seven:SetPosition( self.Button_draw:GetPositionX() , self.Button_draw:GetPositionY() )
    self.Image_bg:AddChild(self.seven)
    self.seven:SetVisible(false)
    
    self.iconEff = {}
    self.icon = {}
    --奖励列表
    local lst = CallModuleFunc(ModuleType.QUEEN , "GetAwardLst")
    for i , v in ipairs( lst ) do
        if self.Image_iconBg[i] then
            local icon = LogicGUI.IconLayer.New(false)
            icon:SetData({ id = v.id })
            icon:SetAnchorPoint(0.5,0.5)
            self.Image_iconBg[i]:AddChild(icon:GetNode())
            PosTool.Center( icon )

            icon:ShowIconBG(false)
            icon:ShowIconQuality(false)
            icon:ShowIconShade(false)
            icon:ShowAptitude(false)

            icon:SetTouchCallback( function ( ... )
                self:OnShowIcon( i )
            end )

            local btnEffect_con = nil
            self.iconEff[i] , btnEffect_con = self:__CreateEffect({res_name = "UI_libao"} , self.Image_iconBg[i] )
            btnEffect_con:SetPosition(self.Image_iconBg[i]:GetContentWidth()/2 , self.Image_iconBg[i]:GetContentHeight()/2 )
            GlobalEffectMgr:SetEffectVisible( self.iconEff[i] , false )

            local tab = {}
            if CPPSdkTools.IsInlandChannel() then
                tab = {
                            { zi = v.recharge , dx = Macros.TypefaceSize.slightly },
                            { zi = CPPGameLib.GetString( "Currency_CNY" , " " ) },
                        }
            else
                tab = {
                        { zi = v.recharge , dx = Macros.TypefaceSize.slightly },
                        { tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.DIAMOND) , jg = 5 , dx = 0.6 },
                    }
            end

            local currency = Util:LabCenter(tab)
            currency:SetAnchorPoint( 0.5 , 0.5 )
            self.Image_currency[i]:AddChild( currency )
            PosTool.Center( currency , 0 ,- 1)

            local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_seven_get" , true)
            self.icon[i] = Util:Sprite( sp )
            self.icon[i]:SetAnchorPoint( 0.5 , 0.5 )
            self.Image_iconBg[i]:AddChild(self.icon[i])
            PosTool.Center( self.icon[i] )
            self.icon[i]:SetVisible(false)

        end
    end
end

function Modules.QueenView:SetShowUi( )
    --累计充值
    local num , max = CallModuleFunc(ModuleType.QUEEN , "GetGrandTotalPay" )
    self.recharge:SetString( num .. "/" .. max )

    local tab = {}
    if CPPSdkTools.IsInlandChannel() then
        self.recharge:SetString( CPPGameLib.GetString( "Currency_CNY" , num .. "/" .. max )  )
        tab = {
            { zi = "" },
        }
    else
        tab = {
                { tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.DIAMOND), dx = 0.7 , jg = 5},
            }
    end

    if not self.item then
        --用作好几个组件拼起来居中显示的强迫症控件 2.0版
        self.item = Util:LabCenter(tab)
        self.item:SetAnchorPoint( 0 , 0.5 )
        self.item:SetPosition( self.recharge:GetPositionX() + (self.recharge:GetContentWidth()/2) , self.Image_recharge:GetContentHeight()/2  )
        self.Image_recharge:AddChild( self.item , 2)
    end

    --可领取图标显示
    for i = 1 , #self.iconEff do
        local lst = CallModuleFunc(ModuleType.QUEEN , "GetAwardLst" , i )
        if lst.recharge then
            local type_ = CallModuleFunc(ModuleType.QUEEN , "GetRewardRecordList" , i )
            if lst.recharge <= num and not type_ then
                GlobalEffectMgr:SetEffectVisible( self.iconEff[i] , true )
            else
                GlobalEffectMgr:SetEffectVisible( self.iconEff[i] , false )
                if type_ then
                    self.icon[i]:SetVisible(true)
                end
            end
        end
    end

    --高级礼物按钮设置
    local type_ = CallModuleFunc(ModuleType.QUEEN , "GetAwardPackageType" )
    --已经
    if "Already" == type_ then
        self.Button_draw:SetVisible( false )
        GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , false)
        self.seven:SetVisible(true)
    --可以
    elseif "May" == type_ then
        self.Button_draw:SetEnabled( true )
        self.Image_draw_text:SetGray( false )
        GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , true)
    --不行
    elseif "Nein" == type_ then
        self.Button_draw:SetEnabled( false )
        self.Image_draw_text:SetGray( true )
        GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , false)
    end
end

function Modules.QueenView:Countdown( time )
    print("xy" , "======time---------------" , time )
    if time <= 0 then
        self.queen_time:SetString( CPPGameLib.GetString("queen_timeEnd") )
    else
        self.queen_time:SetString( TimeTool.TransTimeStamp( CPPGameLib.GetString("queen_Countdown") , time ) )
    end
end

function Modules.QueenView:OnButtonDraw( )
    print("xy" , "=======OnButtonDraw")
    local cnf = CallModuleFunc(ModuleType.QUEEN , "GetTotalRechargeRewardCnf" ) or {}

    local info = {}
    info.reward_id = #cnf
    CallModuleFunc(ModuleType.QUEEN , "TakeTotalRechargeRewardReq" , info )
end

function Modules.QueenView:OnButtonClose( )
    self:CloseManual()
end

function Modules.QueenView:OnButtonRecharge( )
    print("xy" , "=======OnButtonRecharge")
    CallModuleFunc(ModuleType.CHARGE, "OpenView", BaseViewType.CHARGE)
end

function Modules.QueenView:OnShowIcon( index )
    print("xy" , "================index" , index )
    --累计充值
    local num , max = CallModuleFunc(ModuleType.QUEEN , "GetGrandTotalPay" )

    --可领取图标显示
    local lst = CallModuleFunc(ModuleType.QUEEN , "GetAwardLst" , index )
    if lst.recharge then
        local type_ = CallModuleFunc(ModuleType.QUEEN , "GetRewardRecordList" , index )
        if lst.recharge <= num and not type_ then
            local info = {}
            info.reward_id = index
            CallModuleFunc(ModuleType.QUEEN , "TakeTotalRechargeRewardReq" , info )
            return 
        end
    end
    LogicGUI.TemplQueenView.New( index )
end






LogicGUI = LogicGUI or {}

LogicGUI.TemplQueenView = LogicGUI.TemplQueenView or BaseClass(GUI.Template)

function LogicGUI.TemplQueenView:__init(i)
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__open_callback = function ()
        self:InitUI(i)
    end
    self:InitTemplate(GUI.TemplateRegistry.DUNGEON_BOXINFO)
end

function LogicGUI.TemplQueenView:InitUI(i)
    --奖励列表
    local widget = self:GetWidget("ScrollView_Rewards")
    local item_w = 84
    local item_space = 15
    local offset_x = item_w / 2
      
    local cnf = CallModuleFunc(ModuleType.QUEEN , "GetAwardLst" , i )

    local itemInfo = CPPGameLib.GetConfig( "Item" , cnf.id )
    CPPGameLib.PrintTable("xy" , cnf , "cnf--------")  

-- yangshuo(杨硕) 02-05 11:07:08
-- 就读这个字段的物品表里面名字，我们配成礼包
    if itemInfo.item_id_list then
        for i, v in ipairs( itemInfo.item_id_list ) do
            local icon_layer = LogicGUI.IconLayer.New()
            icon_layer:SetData({id = v , num = itemInfo.item_max_num_list[i] })
            icon_layer:SetAnchorPoint(0.5, 1)
            icon_layer:ShowItemName(true)
            icon_layer:ShowEquipName(true)
            icon_layer:ShowHeroName(true)
            icon_layer:ShowTreasureName(true)
            icon_layer:ShowGemName(true)
            icon_layer:ShowDivinationName(true)

            widget:AddChild(icon_layer:GetNode())
            PosTool.LeftTop(icon_layer, offset_x)
            offset_x = offset_x + item_w + item_space
        end 
        offset_x = offset_x - item_w / 2 - item_space
        widget:SetInnerContainerSize(offset_x + 4, 0)
    end

    --标题
    Util:WidgetLabel(self:GetWidget("Text_Title"), CPPGameLib.GetString("award_box_Title") , Macros.TypefaceSize.popup, Macros.Color.btn)
    --关闭按钮
    Util:WidgetButton(self:GetWidget("Button_Close"), "", function()
        self:DeleteMe()
    end)                

    --领取按钮
    local btn_receive = Util:WidgetButton(self:GetWidget("Button_Receive") , CPPGameLib.GetString("common_close") , function()
        self:DeleteMe()  
    end,Macros.TypefaceSize.largish , Macros.Color.btn , Macros.Color.button_red)


    --领取条件   
    self:GetWidget("Panel_StarCondition"):SetVisible(false)

    Util:WidgetLabel(self:GetWidget("Text_GameLevelCondition"), " " , Macros.TypefaceSize.normal, Macros.Color.content )
    local tab = {}
    if CPPSdkTools.IsInlandChannel() then
        tab = {
                    { zi = CPPGameLib.GetString("queen_title") , dx = Macros.TypefaceSize.normal , ys = Macros.Color.content },
                    { zi = cnf.recharge , dx = Macros.TypefaceSize.normal , ys = Macros.Color.content },
                    { zi = CPPGameLib.GetString( "Currency_CNY" , " " ) , dx = Macros.TypefaceSize.normal , ys = Macros.Color.content , jg = 5 },
                    { zi = CPPGameLib.GetString("onlineawardMay") , dx = Macros.TypefaceSize.normal , ys = Macros.Color.content },
                }
    else
        tab = {
                    { zi = CPPGameLib.GetString("queen_title") , dx = Macros.TypefaceSize.normal , ys = Macros.Color.content },
                    { zi = cnf.recharge , dx = Macros.TypefaceSize.normal , ys = Macros.Color.content },
                    { tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.DIAMOND) , jg = 5 , dx = 0.8 },
                    { zi = CPPGameLib.GetString("onlineawardMay") , dx = Macros.TypefaceSize.normal , ys = Macros.Color.content },
                }
    end

    local currency = Util:LabCenter(tab)
    currency:SetAnchorPoint( 0.5 , 0.5 )
    self:GetWidget("Text_GameLevelCondition"):AddChild( currency )
    PosTool.Center( currency )
end

function LogicGUI.TemplQueenView:__delete()
    if self.__button_zoubian_effect then
        self:__DelEffect(self.__button_zoubian_effect)
        self.__button_zoubian_effect = nil
    end
end