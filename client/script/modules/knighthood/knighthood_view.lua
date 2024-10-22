--[[
%% @module: 爵位
%% @author: yjg
%% @created: 2017年8月14日15:37:46
--]]

Modules = Modules or {} 
Modules.KnighthoodView = Modules.KnighthoodView or BaseClass(BaseView)

function Modules.KnighthoodView:__init()
    self.__layout_name = "knighthood"   
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_knighthood")}
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("knighthood"), 
        close_func = function()
            self:CloseManual()
        end,
    }

    self.__open_callback = function()
      self:__OpenCallback()
    end

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.KNIGHTHOOD)  
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.KNIGHTHOOD_ICON )
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.KNIGHTHOOD_ICON , true )  
end

function Modules.KnighthoodView:__OpenCallback( )
    self.lst = {}

    self:__Getwidget()

    self:__ShowUI() 

    self:__Server() 
end 

function Modules.KnighthoodView:__Dispose()
    for i=1,#self.lst do
        self.lst[i]:DeleteMe()
        self.lst[i] = nil
    end

    if self.__chapters_scrollview then
        self.__chapters_scrollview:DeleteMe()
        self.__chapters_scrollview = nil
    end 

end

function Modules.KnighthoodView:__Getwidget( )
    --属性框
    self.Image_paper = self:GetWidget("Image_paper") 
    --标题
    self.Image_headline = self:GetWidget("Image_headline") 
   
    --属性
    self.Text_property = {}
    for i = 1 , 6 do
        self.Text_property[i] = self:GetWidget("Text_property_"..i) 
        self.Text_property[i]:SetPosition( self.Text_property[i]:GetPositionX() - 50 , self.Text_property[i]:GetPositionY() )
    end

    --进度容器
    self.Panel_schedule = self:GetWidget("Panel_schedule") 
    --进度条
    self.Image_schedule = self:GetWidget("Image_schedule") 
    self.LoadingBar_schedule = self:GetWidget("LoadingBar_schedule") 

    --战斗力
    self.Image_battlepower = self:GetWidget("Image_battlepower") 
    self.Image_battlepower_text = self:GetWidget("Image_battlepower_text") 

    --战力加成
    self.Image_addition = self:GetWidget("Image_addition") 

    --激活爵位
    self.Button_activate = Util:WidgetButton( 
                                        self:GetWidget("Button_activate") , 
                                        CPPGameLib.GetString( "knighthood_activate" ) , 
                                        CPPGameLib.Handler(self,self.OnActivate),
                                        nil,nil,Macros.Color.button_yellow
                                    )
    --提升战斗力
    self.Button_promote = Util:WidgetButton( 
                                        self:GetWidget("Button_promote") , 
                                        nil , 
                                        CPPGameLib.Handler(self,self.OnPromote),
                                        nil,nil,Macros.Color.button_yellow
                                    )
    self.Button_promote:SetNormalImage( Resource.PathTool.GetSystemEntryIconPath("btn_strong"), TextureResType.LOCAL )

    self.knighthood_UpStrength = Util:Name( CPPGameLib.GetString("knighthood_UpStrength") )
    self.knighthood_UpStrength:SetPosition( self.Button_promote:GetContentWidth()/2 , 0 )
    self.knighthood_UpStrength:SetAnchorPoint( 0.5 , 0.5 )
    self.Button_promote:AddChild( self.knighthood_UpStrength )


    --图标容器
    self.Panel_icon = self:GetWidget("Panel_icon") 
end 

function Modules.KnighthoodView:__ShowUI( )
    --属性加成
    self.addition = Util:Label( "" , Macros.TypefaceSize.largish , Macros.Color.Special )
    self.addition:SetAnchorPoint( 0.5 , 0.5 )
    self.addition:SetPosition( self.Image_addition:GetContentWidth()/2 , self.Image_addition:GetContentHeight()/2 )
    self.Image_addition:AddChild( self.addition )

    -- 战队总加成
    local knighthood_addition = Util:Label( CPPGameLib.GetString("knighthood_addition") , Macros.TypefaceSize.largish ,  Macros.Color.Special )
    knighthood_addition:SetAnchorPoint( 0.5 , 0.5 )
    self.Image_headline:AddChild( knighthood_addition )
    PosTool.Center(knighthood_addition)   

    --分割线
    local num_division_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "num_division", true)
    local num_division = Util:Sprite( num_division_sp )
    num_division:SetAnchorPoint( 0.5 , 0.5 )
    num_division:SetPosition( self.__root_widget:GetContentWidth()/2 , self.Panel_schedule:GetPositionY() )
    self.__root_widget:AddChild( num_division )

    --战力
    local poodlenum = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "knighthood_num", true)
    self.myPower = GUI.ImgFont.New(poodlenum)
    self.myPower:SetText( tostring(0) )
    self.myPower:SetAnchorPoint( 1 , 0.5 )
    self.myPower:SetPosition( self.__root_widget:GetContentWidth()/2 - 15 , self.Panel_schedule:GetPositionY() )
    self.__root_widget:AddChild( self.myPower:GetNode() )

    --所需战力
    self.neetPower = GUI.ImgFont.New(poodlenum)
    self.neetPower:SetText( tostring(0) )
    self.neetPower:SetAnchorPoint( 0 , 0.5 )
    self.neetPower:SetPosition( self.__root_widget:GetContentWidth()/2 + 15 , self.Panel_schedule:GetPositionY() )
    self.__root_widget:AddChild( self.neetPower:GetNode() )

    --已激活
    local sp_conclude = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_conclude", true)
    self.sp_conclude = Util:Sprite( sp_conclude )
    self.sp_conclude:SetAnchorPoint( 0.5 , 0.5 )
    self.sp_conclude:SetPosition( self.Button_activate:GetPositionX() , self.Button_activate:GetPositionY() )
    self.Panel_schedule:AddChild( self.sp_conclude )
    self.sp_conclude:SetVisible(false)

    self.activate = Util:Label( CPPGameLib.GetString("knighthood_NoActivate_") ,  Macros.TypefaceSize.largish ,  Macros.Color.Special  )
    self.activate:SetAnchorPoint( 0.5 , 0.5 )
    self.activate:SetPosition( self.Button_activate:GetPositionX() , self.Button_activate:GetPositionY() )
    self.Panel_schedule:AddChild( self.activate )
    self.activate:SetVisible(false)

    local effect
    self.effect_id , effect = self:__CreateEffect({res_name = "ui_anniuzoubian2"}, self.Button_activate )
    effect:SetPosition( self.Button_activate:GetContentWidth()/2 , self.Button_activate:GetContentHeight()/2 )


    local sp_bg_decorate_ = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.KNIGHTHOOD, "sp_bg_decorate")
    local sp_bg_decorate = Util:Sprite( sp_bg_decorate_ )
    sp_bg_decorate:SetAnchorPoint( 0.5 , 0.5 )
    sp_bg_decorate:SetPosition( (self.Panel_icon:GetContentWidth()/2) - (self.Panel_icon:GetContentWidth()/3) , self.Panel_icon:GetContentHeight()/2 - 16 )
    self.Panel_icon:AddChild( sp_bg_decorate )

    local sp_bg_decorate = Util:Sprite( sp_bg_decorate_ )
    sp_bg_decorate:SetAnchorPoint( 0.5 , 0.5 )
    sp_bg_decorate:SetPosition( (self.Panel_icon:GetContentWidth()/2) + (self.Panel_icon:GetContentWidth()/3) , self.Panel_icon:GetContentHeight()/2 - 16 )
    self.Panel_icon:AddChild( sp_bg_decorate )

    local effect = nil
    self.effectid_1 ,effect = self:__CreateEffect({res_name = "UI_shangdianjiantou" } , self.Panel_icon , 999 )
    effect:SetAnchorPoint(0.5,0.5)
    effect:SetPosition( self.Panel_icon:GetContentWidth() , self.Panel_icon:GetContentHeight()/2 )

    local effect = nil
    self.effectid_2 ,effect = self:__CreateEffect({res_name = "UI_shangdianjiantou" } , self.Panel_icon , 999 )
    effect:SetAnchorPoint(0.5,0.5)
    effect:SetPosition( 0 , self.Panel_icon:GetContentHeight()/2 )
    effect:SetScaleXFactor( -1 )
end

function Modules.KnighthoodView:__Server()
    self:BindGlobalEvent(Macros.Event.KnighthoodModule.KNIGHTHOOD, function ( ... )

        local lab = Util:Label( 
                                    CPPGameLib.GetConfig("Nobility", CallModuleFunc(ModuleType.ROLE,"GetNobility") ).pro_type_show , 
                                    Macros.TypefaceSize.headline, Macros.Color.green, 0, 0, 
                                    Macros.Color.green_stroke
                                )
        self.__root_widget:AddChild(lab,5)
        lab:SetPosition( self.__root_widget:GetContentWidth()/2 , 300 )    

        local windwords_list = {}
        local info = {}
        info.node = lab
        info.anim_type = Macros.Game.WIND_WORDS_TYPE.UPSLOPE        
        info.tgt_pos = {x = self.__root_widget:GetContentWidth()/2 , y = 350} 
        table.insert(windwords_list,info)

        local windwords_effect = WindWordsEffects.New()
        windwords_effect:Wind(windwords_list,function ()
            if windwords_effect then
                windwords_effect:DeleteMe()
                windwords_effect = nil
            end
        end)

        -- local cnf = CallModuleFunc(ModuleType.KNIGHTHOOD, "GetIconLstCnf")
        local index = CallModuleFunc(ModuleType.KNIGHTHOOD, "GetActivateRecord")
        local num = 1
        if index - 1 ~= 0 then
            num = index
        end
        -- self:SetShowUI( cnf[ num ],true)
        self:SetEffshow( )

        -- 2668 当激活当前勋章后，播放完激活的特效后，再自动切换到下一个未激活的勋章
        CPPActionManager.DelayTo( self.Image_paper, 4 , GlobalCallbackMgr:AddCallBackFunc(function()

            local nobility_config = CPPGameLib.GetConfig("Nobility",nil,nil,true)
            if num + 1 <= #nobility_config then

                self.index = num + 1

                self.__chapters_scrollview:CheckItemIsOnScreenCenterByIndex( self.index ) 
                self.__chapters_scrollview:SelectItem( self.index )

                local cnf = CallModuleFunc(ModuleType.KNIGHTHOOD, "GetIconLstCnf")

                self:SetShowUI( cnf[self.index] )

            else

                local cnf = CallModuleFunc(ModuleType.KNIGHTHOOD, "GetIconLstCnf")
                self:SetShowUI( cnf[#cnf] )

            end

            self:SetEffshow( )
            self:ShowStrength()
        end))
    end)
    --根据回调来确认当前显示档数
    self:SetPageViewCell()
end

function Modules.KnighthoodView:SetPageViewCell( ... )
    --创建章节列表
    local item_w = self.Panel_icon:GetContentWidth()/3
    local params = 
    {
        item_class = LogicGUI.KnighthoodItem,
        item_width = item_w,
        item_height = 400,
        row = 1,
        col = 3 ,
        view_width = self.Panel_icon:GetContentWidth() , 
        horizon = true,
        selectable = true,
        select_callback = function(item_index, item_data)
            if item_data then
                self.index = item_index
                self:SetShowUI( item_data )
                self:SetEffshow( )
            end
        end,
    }
    self.__chapters_scrollview = GUI.SpecialScrollView.New(params)
    self.Panel_icon:AddChild(self.__chapters_scrollview:GetNode())
    PosTool.Center(self.__chapters_scrollview)      

    
    self.__chapters_scrollview:SetChangeData( 1 , 0 , 0 )
    self.__chapters_scrollview:SetIsNotUnconditionalSelected(true)


    --设置解锁副本的章数
    local index = CallModuleFunc(ModuleType.KNIGHTHOOD, "GetActivateRecord")
    local nobility_config = CPPGameLib.GetConfig("Nobility",nil,nil,true)
    -- local cur_lock_index = math.min(index+1,#nobility_config) 
    self.__chapters_scrollview:SetNecessaryData(#nobility_config,3,true)

    local cnf = CallModuleFunc(ModuleType.KNIGHTHOOD, "GetIconLstCnf")
    self.__chapters_scrollview:SetDataList(cnf)

    --将索引为Index的条目移动到屏幕中心
    --self.__chapters_scrollview:MoveItemToScreenCenterByIndex( index ) 
    --设置条目选择过渡效果的完成回调
    self.__chapters_scrollview:SetSelectItemEffectEndCallback(function()
    end) 
    
    --获得需求战斗力
    local my_combat_value , need_power = CallModuleFunc(ModuleType.KNIGHTHOOD, "GetNeedFighting" , index + 1)
    if need_power and my_combat_value >= need_power then
        index = index + 1
    end
    if index <= 0 then
        index = 1
    end
    self.__chapters_scrollview:MoveItemToScreenCenterByIndex( index )
    self.__chapters_scrollview:SelectItem( index )
end

--设置ui
function Modules.KnighthoodView:SetShowUI( data,is_ignore)
    self.addition:SetString( data.properText )

    --设置属性
    local cnf , propertyLst = CallModuleFunc(ModuleType.KNIGHTHOOD, "GetIconLstCnf")
    for i , v in ipairs( propertyLst ) do
        local type_ , value = Util:Nature( v.type , v.value )
        self.Text_property[i] =  Util:WidgetLabel( 
                                                    self.Text_property[i] , type_ , 
                                                    Macros.TypefaceSize.largish, 
                                                    Macros.Color.keypoint
                                                )
        self.Text_property[i]:SetAnchorPoint( 0 , 0.5 )
    end

    --设置战斗力
    local myPower , neetPower = CallModuleFunc(ModuleType.KNIGHTHOOD, "GetNeedFighting" , data.id )
    self.myPower:SetText( tostring(myPower) )
    self.neetPower:SetText( tostring(neetPower) )

    --设置进度条
    self.LoadingBar_schedule:SetPercent( myPower/neetPower * 100 )

    --设置按钮
    local activateRecord = CallModuleFunc(ModuleType.KNIGHTHOOD, "GetActivateRecord" , data.id )
    
    self.activate:SetVisible(false)
    self.Button_activate:SetVisible( false )
    self.sp_conclude:SetVisible(false)
    GlobalEffectMgr:SetEffectVisible(self.effect_id , false)

    --正序支持
    -- --记录不是当前的前置
    -- if myPower < neetPower then
    --     --不可激活        
    --     self.Button_activate:SetVisible(true)
    --     WidgetUnity.SetWidgetListener( self.Button_activate , nil, nil, function ( ... )
    --         self:NoOnActivate()
    --     end, nil, 0.5*1000)
    local operate_id, data_id = CallModuleFunc(ModuleType.LEAD, "GetOperateInfo")
    local is_could_light_up = false

    print("xy" , "activateRecord >= data.id then " , activateRecord )

    --记录id大于等于这个id
    if activateRecord >= data.id then
        --激活过
        self.sp_conclude:SetVisible(true)
    elseif (activateRecord + 1) == data.id and myPower >= neetPower then
        WidgetUnity.SetWidgetListener( self.Button_activate , nil, nil, function ( ... )
            self:OnActivate()
        end, nil, 0.5*1000)
        self.Button_activate:SetVisible(true)
        GlobalEffectMgr:SetEffectVisible(self.effect_id , true)
        is_could_light_up = true
        --新手指引特殊操作检测(爵位系统-激活徽章)
        CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.KNIGHTHOOD_LIGHT_UP, self.Button_activate)
    elseif (activateRecord + 1) == data.id and myPower < neetPower then
        --不可激活        
        self.Button_activate:SetVisible(true)
        WidgetUnity.SetWidgetListener( self.Button_activate , nil, nil, function ( ... )
            self:NoOnActivate()
        end, nil, 0.5*1000)
    else
        --激活上个
        self.activate:SetVisible(true) 
    end

    if operate_id == Macros.Game.LeadOperateSpecialID.KNIGHTHOOD_LIGHT_UP and not is_could_light_up and not is_ignore then
        --前往爵位系统激活徽章时，如果不能激活，则打断指引
        CallModuleFunc(ModuleType.LEAD, "Break")
    end

    -- --设置解锁副本的章数
    -- local index = CallModuleFunc(ModuleType.KNIGHTHOOD, "GetActivateRecord") + 1
    -- local nobility_config = CPPGameLib.GetConfig("Nobility",nil,nil,true)
    -- index = math.min(index,#nobility_config) 
    -- self.__chapters_scrollview:SetLockChapterNum(index)

    -- local cnf = CallModuleFunc(ModuleType.KNIGHTHOOD, "GetIconLstCnf")
    -- self.__chapters_scrollview:SetDataList(cnf)
    
    --无序支持    
    -- --激活过
    -- if activateRecord == true then
    --     self.sp_conclude:SetVisible(true)
    -- --没激活过
    -- elseif myPower >= neetPower then
    --     WidgetUnity.SetWidgetListener( self.Button_activate , nil, nil, function ( ... )
    --         self:OnActivate()
    --     end, nil, 0.5*1000)
    --     self.Button_activate:SetVisible(true)
    --     GlobalEffectMgr:SetEffectVisible(self.effect_id , true)
    -- --不能激活
    -- else
    --     self.Button_activate:SetVisible(true)
    --     WidgetUnity.SetWidgetListener( self.Button_activate , nil, nil, function ( ... )
    --         self:NoOnActivate()
    --     end, nil, 0.5*1000)
    -- end
end

--显示特效
function Modules.KnighthoodView:SetEffshow( )
    --列表相关调整
    self.__chapters_scrollview:ItemIter(function(index, item)
        item:SetSelected( false )
        if index == self.index then
            item:SetSelected( true )
        end
    end)
end

--激活
function Modules.KnighthoodView:OnActivate( data )
    self.shield = Util:Layout( Macros.Global.DesignSize.width, Macros.Global.DesignSize.height,function ( ... )
        print("xy" , "Macros.Global.DesignSize.width,")
    end )
    self.shield:SetAnchorPoint( 0.5 , 0.5 )
    self:AddChild( self.shield , 999 )
    PosTool.Center( self.shield )

    CallModuleFunc(ModuleType.KNIGHTHOOD, "NobilityActivateReq" )
end

--不能激活
function Modules.KnighthoodView:NoOnActivate( data )
    GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("knighthood_NoActivate") )
end

--提升战斗力
function Modules.KnighthoodView:OnPromote( data )
    CallModuleFunc(ModuleType.STRONGER, "OpenView", BaseViewType.STRONGER )
end


--显示战斗力
function Modules.KnighthoodView:ShowStrength( data )
    -- yangshuo(杨硕) 08-21 15:46:09
    -- 像世界之心那样，特效都显示完了再让点返回
    CallModuleFunc(ModuleType.ROLE,"RecordBattlePowerChanged" , Macros.Game.COMBAT_CHANGE.KNIGHTHOOD , true )
    if self.shield then
        self.shield:RemoveSelf(true)
        self.shield = nil
    end
end











LogicGUI = LogicGUI or {}

LogicGUI.KnighthoodItem = LogicGUI.KnighthoodItem or BaseClass(GUI.ScrollItem)

function LogicGUI.KnighthoodItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  

    self:BindGlobalEvent(Macros.Event.KnighthoodModule.KNIGHTHOOD, function ( ... )
        self:Activate( )
    end)
end


function LogicGUI.KnighthoodItem:__delete()
end

function LogicGUI.KnighthoodItem:__CreateWidgetFromLuaCode()
    self.__layout = Util:Layout( 960/3 , 400 )
    self.__layout:SetClippingEnable(false)
    self.__layout:SetTouchEnabled(false)
    return self.__layout
end

function LogicGUI.KnighthoodItem:SetData(data)
    GUI.Template.SetData(self , data)
    self:__ShowUI( data )
end

function LogicGUI.KnighthoodItem:__ShowUI( data )
    --图标
    local icon_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.KNIGHTHOOD_ICON, "sp_level".. data.id , true )
    self.icon = Util:Sprite( icon_sp )
    self.icon:SetAnchorPoint( 0.5 , 0.5 )
    self.icon:SetPosition( self.__layout:GetContentWidth()/2 , self.__layout:GetContentHeight()/2 )
    self.__layout:AddChild( self.icon )

    --字样
    local icon_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.KNIGHTHOOD_ICON, "sp_level".. data.id )
    self.cover = Util:Sprite( icon_sp )
    self.cover:SetAnchorPoint( 0.5 , 0.5 )
    self.cover:SetPosition( self.icon:GetContentWidth()/2 , self.icon:GetContentHeight()/2 )
    self.icon:AddChild( self.cover )

    local t1 = {
        [1] = { eff = "UI_juewei_xinxingzhidui" , x = -14 , y = 43 } , 
        [2] = { eff = "UI_juewei_kaituozhidui" , x = 2 , y = 45 } , 
        [3] = { eff = "UI_juewei_guangmingzhidui" , x = -5 , y = 45 } , 
        [4] = { eff = "UI_juewei_yongqizhidui" , x = 0 , y = 44 } , 
        [5] = { eff = "UI_juewei_rongyaozhidui" , x = 0 , y = 46 } , 
        [6] = { eff = "UI_juewei_wangzhezhidui" , x = -8 , y = 42 } , 
        [7] = { eff = "UI_juewei_zhushenzhidui" , x = -19 , y = 44 } ,
    }

    local effect = nil
    self.effectid,effect = self:__CreateEffect({res_name = t1[data.id].eff } , self.icon , 999 )
    effect:SetAnchorPoint(0.5,0.5)
    PosTool.Center( effect , t1[data.id].x , t1[data.id].y )
    GlobalEffectMgr:SetEffectVisible( self.effectid , false)

end

function LogicGUI.KnighthoodItem:__AddToParent()
end

function LogicGUI.KnighthoodItem:IsTouchEnable()
    if not GUI.ScrollItem.IsTouchEnable(self) then
        return false
    end

    -- local cur_lock_index = CallModuleFunc(ModuleType.KNIGHTHOOD, "GetActivateRecord") + 1
    local nobility_config = CPPGameLib.GetConfig("Nobility",nil,nil,true)
    -- cur_lock_index = math.min(cur_lock_index,#nobility_config) 
    return self.__data.index <= #nobility_config
end

function LogicGUI.KnighthoodItem:GetChangeWidget()
    return self.icon
end

function LogicGUI.KnighthoodItem:SetSelected( var )

    local cur_lock_index = CallModuleFunc(ModuleType.KNIGHTHOOD, "GetActivateRecord")
    if cur_lock_index >= self.__data.index then
        GlobalEffectMgr:SetEffectVisible( self.effectid , var)
    end

    self.cover:SetVisible(var == false )
end

function LogicGUI.KnighthoodItem:Activate()
    --设置解锁副本的章数
    local index = CallModuleFunc(ModuleType.KNIGHTHOOD, "GetActivateRecord")
    print("xy" , "=====11111===========" , index )
    if self.__data.id == index then
        local effectid,effect = self:__CreateEffect({id = 100060 + (index - 1) } , self.icon , 999 )
        effect:SetAnchorPoint(0.5,0.5)
        PosTool.Center( effect )
    end
end


