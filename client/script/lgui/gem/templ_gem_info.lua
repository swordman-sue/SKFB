--
-- @brief 宝石信息
-- @author: yjg
-- @date: 2017年9月21日10:15:43
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplGemInfo = LogicGUI.TemplGemInfo or BaseClass(GUI.Template)

function LogicGUI.TemplGemInfo:__init( id , btnType )
    self.__is_auto_load = true
    -- self.__bg_params = {type = BaseView.BGType.SOLID}   
    self.__open_callback = function()
        self.id = id
        self:__Getwidget( btnType )
        self:__ShowUI( id )
    end
    
    self:BindGlobalEvent(Macros.Event.GemModule.COMPOUND , function (_, data)
        local isCompound = CallModuleFunc(ModuleType.GEM, "ToIdGetGemIsCompound", self.id )
        GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , isCompound )
    end)

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.GEM)
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_GEM_INFO)  
end

function LogicGUI.TemplGemInfo:__Getwidget( btnType )

    -- 内容背景
    self.Image_bg = self:GetWidget("Image_bg") 
    -- 内容
    self.Image_content = self:GetWidget("Image_content")
    --图标
    self.Image_icon = self:GetWidget("Image_icon")
    --名字
    self.Panel_name = self:GetWidget("Panel_name")
    --战力
    self.Panel_military = self:GetWidget("Panel_military")
    --技能
    self.Panel_skill = self:GetWidget("Panel_skill")

    --合成
    self.Button_1 = Util:WidgetButton(  
                                            self:GetWidget("Button_1") , 
                                            CPPGameLib.GetString("gem_compound") , 
                                            CPPGameLib.Handler(self,self.OnButtonCompound) ,
                                            nil,nil,Macros.Color.button_yellow
                                            ) 
    local BtnEffect_con = nil
    self.btnEffect_id , BtnEffect_con = self:__CreateEffect({res_name = "ui_anniuzoubian"} , self.Button_1 , 999)
    BtnEffect_con:SetAnchorPoint(0.5,0.5)
    PosTool.Center( BtnEffect_con )
    GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , false )

    --镶嵌
    self.Button_2 = Util:WidgetButton(  
                                            self:GetWidget("Button_2") , 
                                            CPPGameLib.GetString("gem_inlay") , 
                                            CPPGameLib.Handler(self,self.OnButtonLnlay) ,
                                            nil,nil,Macros.Color.button_yellow
                                            ) 
    --分解
    self.Button_3 = Util:WidgetButton(  
                                            self:GetWidget("Button_3") , 
                                            CPPGameLib.GetString("gem_resolve") , 
                                            CPPGameLib.Handler(self,self.OnButtonResolve) ,
                                            nil,nil,Macros.Color.button_yellow
                                            ) 

    -- 
    if btnType == "normal" then
        self.Button_1:SetVisible(false)
        self.Button_2:SetVisible(false)
        self.Button_3:SetVisible(false)

        self.Image_bg:SetAnchorPoint( 0.5 , 1 )
        self.Image_bg:SetContentSize( self.Image_bg:GetContentWidth() , self.Image_bg:GetContentHeight() - 60 ) 
        
        self.Image_content:SetPosition( self.Image_content:GetPositionX() , self.Image_content:GetPositionY() - 65 )

        self.Image_icon:SetPosition( self.Image_icon:GetPositionX() , self.Image_icon:GetPositionY() - 65 )

    elseif btnType == "discharge" then
        self.Button_1:SetVisible(false)
        --分解
        self.Button_2 = Util:WidgetButton(  
                                                self:GetWidget("Button_2") , 
                                                CPPGameLib.GetString("gem_discharge") , 
                                                CPPGameLib.Handler(self,self.OnButtonDischarge) ,
                                                nil,nil,Macros.Color.button_yellow
                                                ) 
        self.Button_3:SetVisible(false)
    end
end

function LogicGUI.TemplGemInfo:__ShowUI( id )
    WidgetUnity.SetWidgetListener(self.__root_widget, nil, nil, function ( ... )
        self:DeleteMe()
    end)

    local icon_layer = LogicGUI.IconLayer.New( false )
    icon_layer:SetData({id = id})
    self.Image_icon:AddChild(icon_layer:GetNode())
    PosTool.Center(icon_layer)   

    icon_layer:ShowIconQuality(false)
    icon_layer:ShowIconShade(false)
    icon_layer:ShowIconBG(false)

    local gemCnf = CallModuleFunc(ModuleType.GEM, "GetGemCnf", id)

    --无法合成
    if gemCnf.new_gem_from_compose == 0 then
        Util:GreyButton( self.Button_1 )
    end

    local isCompound = CallModuleFunc(ModuleType.GEM, "ToIdGetGemIsCompound", id )
    GlobalEffectMgr:SetEffectVisible(self.btnEffect_id , isCompound )

    --名字
    self.name = Util:Label( gemCnf.name , Macros.TypefaceSize.popup , Macros.Color.Special )
    self.name:SetAnchorPoint( 0 , 0.5 )
    self.name:SetPosition( 0 , self.Panel_name:GetContentHeight()/2 )
    self.Panel_name:AddChild( self.name )

    --名字等级
    local namelevel = Util:Label( CPPGameLib.GetString("common_lev9" , gemCnf.level) , Macros.TypefaceSize.normal , Macros.Color.green )
    self.Panel_name:AddChild( namelevel )
    PosTool.RightTo( self.name , namelevel , 15 )

    --战斗力
    local maincity_battle_power = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "maincity_battle_power" , true) )
    maincity_battle_power:SetAnchorPoint( 0 , 0.5 )
    maincity_battle_power:SetPosition( 0 , self.Panel_military:GetContentHeight()/2 )
    self.Panel_military:AddChild( maincity_battle_power )

    --战斗力
    local power = Util:Label( "+" .. gemCnf.power , nil , Macros.Color.golden )
    self.Panel_military:AddChild( power )    
    PosTool.RightTo( maincity_battle_power , power , 5 )

    --技能
    local skill = Util:Sprite( Resource.PathTool.GetUIResPath("gem/sp_gem_skill_name_" .. gemCnf.skill_id ,true) )
    skill:SetAnchorPoint( 0 , 0.5 )
    skill:SetPosition( 0 , self.Panel_skill:GetContentHeight()/2 )
    self.Panel_skill:AddChild( skill )

    --名字等级
    local skilllevel = Util:Label(  CPPGameLib.GetString("common_lev9" , gemCnf.skill_level ) , Macros.TypefaceSize.normal , Macros.Color.green )
    self.Panel_skill:AddChild( skilllevel )
    PosTool.RightTo( skill , skilllevel , 15 )

    --属性
    self.listView = UIListViewEx.Create()
    self.listView:SetContentSize(self.Image_content:GetContentWidth() , self.Image_content:GetContentHeight() )
    self.Image_content:AddChild(self.listView)
    PosTool.Center( self.onNum )

    local list = CallModuleFunc(ModuleType.GEM, "GetGemProperty" , id )

    local skill = Util:Sprite( Resource.PathTool.GetUIResPath("gem/sp_gem_skill_name_" .. gemCnf.skill_id ,true) )
    skill:SetAnchorPoint( 0 , 0.5 )
    skill:SetPosition( 20 , self.Image_content:GetContentHeight()/2 - 50 )
    self.Image_content:AddChild( skill )
    
    --名字等级
    local skilllevel = Util:Label(  CPPGameLib.GetString("common_lev9" , gemCnf.skill_level ) , Macros.TypefaceSize.normal , Macros.Color.green )
    self.Image_content:AddChild( skilllevel )
    PosTool.RightTo( skill , skilllevel , 15 )
    
    for i,v in ipairs(list) do
        local layout = Util:Layout( self.Image_content:GetContentWidth() , self.Image_content:GetContentHeight()/2 )
        layout:SetAnchorPoint(0.5, 0.5)

        local sp_gem_name = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.GEM, "sp_gem_name" ) )
        sp_gem_name:SetAnchorPoint( 0 , 0.5 )
        sp_gem_name:SetPosition( 0 , layout:GetContentHeight() - 20 )
        layout:AddChild( sp_gem_name )

        local name = Util:Label( v.name , Macros.TypefaceSize.normal , Macros.Color.btn )
        name:SetAnchorPoint( 0 , 0.5 )
        name:SetPosition( 20 , sp_gem_name:GetContentHeight()/2 )
        sp_gem_name:AddChild( name )

        local y = 35
        if i == 2 then
            y = 55        
        end

        local posLst = {
                [1] = { x = 20 , y = sp_gem_name:GetPositionY() - y },
                [2] = { x = layout:GetContentWidth()/2 + 20 , y = sp_gem_name:GetPositionY() - y },
                [3] = { x = 20 , y = sp_gem_name:GetPositionY() - (y + 30) },
                [4] = { x = layout:GetContentWidth()/2 + 20 , y = sp_gem_name:GetPositionY() - (y + 30) },
        }

        for j , vv in pairs( v.lst ) do
            if vv.type ~= 0 then
                local key , worth = Util:Nature( vv.type , vv.num , true )

                local lst = Util:Label( key , Macros.TypefaceSize.normal , Macros.Color.Special )
                lst:SetAnchorPoint( 0 , 0.5 )
                lst:SetPosition( posLst[j].x , posLst[j].y )
                layout:AddChild( lst )

                local nature = Util:Label( worth , Macros.TypefaceSize.normal , Macros.Color.btn )
                layout:AddChild( nature )
                PosTool.RightTo( lst , nature , 15 )
            end
        end
        self.listView:PushBackCustomItem(layout)
    end
end

function LogicGUI.TemplGemInfo:OnButtonCompound( id )
    print("xy" , "合成")
    LogicGUI.TemplGemCompound.New( self.id )
end

function LogicGUI.TemplGemInfo:OnButtonLnlay( id )
    print("xy" , "镶嵌")
    local onVacancy = CallModuleFunc(ModuleType.GEM, "GetOnVacancy" )
    CPPGameLib.PrintTable("xy" , onVacancy ,"选择的孔位")   
    if not onVacancy then
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("gem_selectInlayTips") )
        return 
    end

    local gemLst = CallModuleFunc(ModuleType.GEM, "GetNoGemLocationLst")
    CPPGameLib.PrintTable("xy", gemLst ,"持有的孔位") 
    if #gemLst ~= 0 then
        onVacancy = gemLst[1].noGemPos
    end

    --阵位上装备的宝石
    local inlayLst , index = CallModuleFunc(ModuleType.GEM, "GetOnGemLocationLst")
    CPPGameLib.PrintTable("xy", inlayLst ,"阵位上装备的宝石") 
    --便利阵位上的宝石
    local t1 = {}
    for i , v in ipairs(inlayLst) do
        if v ~= 0 then
            local gemCnf = CallModuleFunc(ModuleType.GEM, "GetGemCnf" , v )
            local inlayCnf = CallModuleFunc(ModuleType.GEM, "GetGemInlayCnf" , gemCnf.type )
            if not t1[ v ] then
                t1[ v ] = { num = 0 , max = inlayCnf.inlay_num_limit , type = gemCnf.type }
            end
            t1[ v ].num = t1[ v ].num + 1
        end
    end

    CPPGameLib.PrintTable("xy", t1 ,"便利阵位上的宝石") 

    --被我替换的宝石
    local target = CallModuleFunc(ModuleType.GEM, "GetGemCnf" , inlayLst[ onVacancy ] )
    CPPGameLib.PrintTable("xy", target ,"被我替换的宝石") 
    
    --我要穿的宝石
    local selected = CallModuleFunc(ModuleType.GEM, "GetGemCnf" ,  self.id  )
    CPPGameLib.PrintTable("xy", selected ,"我要穿的宝石") 

    --替换
    if inlayLst[ onVacancy ] and inlayLst[ onVacancy ] ~= 0 then
        if target.type ~= selected.type then
            --上限清算
            local t2 = {}
            for i , v in pairs( t1 ) do
                if not t2[ v.type ] then
                    t2[ v.type  ] = {}
                end
                local inlayCnf = CallModuleFunc(ModuleType.GEM, "GetGemInlayCnf" , v.type )
                if #t2[ v.type  ] < inlayCnf.inlay_num_limit then
                    table.insert( t2[ v.type  ] , v )
                end
            end

            if t2[ selected.type ] then
                if ( #t2[ selected.type ] >= t2[ selected.type ][1].max ) or ( t2[ selected.type ][1].num >= t2[ selected.type ][1].max ) then
                    GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("gem_this_max"))
                    return 
                end
            end
        end
    else
        --上限清算
        local t2 = {}
        for i , v in pairs( t1 ) do
            if not t2[ v.type ] then
                t2[ v.type  ] = {}
            end
            local inlayCnf = CallModuleFunc(ModuleType.GEM, "GetGemInlayCnf" , v.type )
            if #t2[ v.type  ] < inlayCnf.inlay_num_limit then
                table.insert( t2[ v.type  ] , v )
            end
        end
        CPPGameLib.PrintTable("xy", t2 ,"上限清算") 

        if t2[ selected.type ] then
            if ( #t2[ selected.type ] >= t2[ selected.type ][1].max ) or ( t2[ selected.type ][1].num >= t2[ selected.type ][1].max ) then
                GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("gem_this_max"))
                return 
            end
        end
    end

    local t2 = {}
    table.insert( t2 , { id = self.id , hole = onVacancy } )

    local _ , index = CallModuleFunc(ModuleType.GEM, "GetOnGemLocationLst" )
    local info = {}
    info.formation_slot = index
    info.gem_inlay_list = t2
    CallModuleFunc(ModuleType.GEM, "GemInlayReq" , info )
    
    self:DeleteMe()
end

function LogicGUI.TemplGemInfo:OnButtonResolve( id )
    print("xy" , "分解")
    LogicGUI.TemplGemResolve.New( self.id ,function ( ... )
        local data = CallModuleFunc(ModuleType.GEM, "GetPack" , self.id )
        if not data or data.num <= 0 then
            self:DeleteMe()
        end
    end)
end

function LogicGUI.TemplGemInfo:OnButtonDischarge( id )
    print("xy" , "卸下")

    local onVacancy = CallModuleFunc(ModuleType.GEM, "GetOnVacancy" )
    if not onVacancy then
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("gem_selectInlayTips") )
        return 
    end

    local _ , index = CallModuleFunc(ModuleType.GEM, "GetOnGemLocationLst" )
    local info = {}
    info.formation_slot = index
    info.gem_hole = onVacancy
    CallModuleFunc(ModuleType.GEM, "GemUnloadReq" , info )

    self:DeleteMe()
end


--
-- @brief 宝石合成
-- @author: yjg
-- @date: 2017年9月21日10:15:43
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplGemCompound = LogicGUI.TemplGemCompound or BaseClass(GUI.Template)

function LogicGUI.TemplGemCompound:__init( id )
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID}   
    self.__open_callback = function()

        local gemData = CallModuleFunc(ModuleType.GEM, "GetPack", id )
        local GemCnf = CallModuleFunc(ModuleType.GEM, "GetGemCnf", id )

        self.id = id
        --默认选择数量
        self.onIndex = 1
        --可选择上限 
        self.onMax = math.floor(gemData.num / GemCnf.compose_cost_num)

        self:__Getwidget( )
        self:__ShowUI( id )
        self:__SetShowUI()
        self:__Server() 
    end

    self:InitTemplate(GUI.TemplateRegistry.TEMPL_GEM_COMPOUND)  
end

function LogicGUI.TemplGemCompound:__Getwidget( btnType )
    -- 内容背景
    self.Image_bg = self:GetWidget("Image_bg") 
    -- 标题
    self.Image_headline = self:GetWidget("Image_headline")
    --内容
    self.Image_content = self:GetWidget("Image_content")
    --碎片背景
    -- self.Image_debrisbg = self:GetWidget("Image_debrisbg")
    --碎片
    self.Image_debris = self:GetWidget("Image_debris")
    --目标背景
    -- self.Image_targetbg = self:GetWidget("Image_targetbg")
    --目标
    self.Image_target = self:GetWidget("Image_target")
    --绿色
    -- self.Image_arrows = self:GetWidget("Image_arrows")
    --数量背景
    self.Image_import = self:GetWidget("Image_import")
    
    --关闭
    self.Button_delete = Util:WidgetButton(  
                                            self:GetWidget("Button_delete") , 
                                            nil , 
                                            CPPGameLib.Handler(self,self.OnButtonDelete)
                                            ) 

    --减一
    self.Button_subtract1 = Util:WidgetButton(  
                                            self:GetWidget("Button_subtract1") , 
                                            nil , 
                                            function( ... )
                                                -- body
                                                self:OnButtonSubtract( 1 )
                                            end,nil,nil,nil,nil,0
                                            ) 
    --减一
    self.Button_subtract10 = Util:WidgetButton(  
                                            self:GetWidget("Button_subtract10") , 
                                            nil , 
                                            function( ... )
                                                self:OnButtonSubtract( 10 )
                                            end,nil,nil,nil,nil,0
                                            ) 

    --加一
    self.Button_add1 = Util:WidgetButton(  
                                            self:GetWidget("Button_add1") , 
                                            nil , 
                                            function( ... )
                                                self:OnButtonAdd( 1 )
                                            end,nil,nil,nil,nil,0
                                            ) 

    --加十
    self.Button_add1 = Util:WidgetButton(  
                                            self:GetWidget("Button_add10") , 
                                            nil , 
                                            function( ... )
                                                self:OnButtonAdd( 10 )
                                            end,nil,nil,nil,nil,0
                                            ) 

    --合成
    self.Button_compound = Util:WidgetButton(  
                                            self:GetWidget("Button_compound") , 
                                            CPPGameLib.GetString("gem_compound") ,  
                                            CPPGameLib.Handler(self,self.OnButtonCompound),
                                            nil,nil,Macros.Color.button_yellow
                                            ) 
end

function LogicGUI.TemplGemCompound:__ShowUI( id )
    --标题
    local gem_gemcompound = Util:Label( CPPGameLib.GetString("gem_gemcompound") , Macros.TypefaceSize.popup , Macros.Color.lilyWhite )
    self.Image_headline:AddChild( gem_gemcompound )
    PosTool.Center(gem_gemcompound)

    local index = self.onIndex
    if self.onMax <= 0 then
        index = 1
    end

    -- 2630 【优化】宝石合成界面，点击宝石的图标，弹出下面的没有按钮的tips
    --数量
    self.onNum = Util:Label( index , Macros.TypefaceSize.normal , Macros.Color.content )
    self.onNum:SetAnchorPoint( 0.5 , 0.5 )
    self.Image_import:AddChild( self.onNum )
    PosTool.Center( self.onNum )

    --合成
    local icon_layer = LogicGUI.IconLayer.New( true )
    icon_layer:SetData({id = id})
    self.Image_debris:AddChild(icon_layer:GetNode())
    PosTool.Center(icon_layer)   

    icon_layer:ShowIconQuality(false)
    icon_layer:ShowIconShade(false)
    icon_layer:ShowIconBG(false)

    local gemCnf = CallModuleFunc(ModuleType.GEM, "GetGemCnf", id)
    local name = Util:Label( gemCnf.name , Macros.TypefaceSize.largish , Macros.Color.headline )
    name:SetAnchorPoint( 0.5 , 0.5 )
    self.Image_debris:AddChild( name )
    PosTool.Center( name , 0 , -70 )

    local quality = Util:Label( CPPGameLib.GetString("common_lev5" , gemCnf.level) , Macros.TypefaceSize.normal , Macros.Color.green )
    quality:SetAnchorPoint( 0 , 0.5 )
    quality:SetPosition( name:GetPositionX() + (name:GetContentWidth()/2) + 5 , name:GetPositionY() )
    self.Image_debris:AddChild( quality )

    self.debris = Util:Name( index * gemCnf.compose_cost_num , Macros.TypefaceSize.normal )
    self.Image_debris:AddChild( self.debris , 999)
    PosTool.Center( self.debris , 0 , -28 )  

    --目标
    local target = CallModuleFunc(ModuleType.GEM, "GetGemCnf", gemCnf.new_gem_from_compose)
    local icon_layer = LogicGUI.IconLayer.New( true )
    icon_layer:SetData({id = gemCnf.new_gem_from_compose})
    self.Image_target:AddChild(icon_layer:GetNode())
    PosTool.Center(icon_layer)   

    icon_layer:ShowIconQuality(false)
    icon_layer:ShowIconShade(false)
    icon_layer:ShowIconBG(false)

    local name = Util:Label( target.name , Macros.TypefaceSize.largish , Macros.Color.headline )
    name:SetAnchorPoint( 0.5 , 0.5 )
    self.Image_target:AddChild( name )
    PosTool.Center( name , 0 , -70 )

    local quality = Util:Label( CPPGameLib.GetString("common_lev5" , target.level) , Macros.TypefaceSize.normal , Macros.Color.green )
    quality:SetAnchorPoint( 0 , 0.5 )
    quality:SetPosition( name:GetPositionX() + (name:GetContentWidth()/2) + 5 , name:GetPositionY() )
    self.Image_target:AddChild( quality )

    self.target = Util:Name( index , Macros.TypefaceSize.normal )
    self.Image_target:AddChild( self.target , 999)
    PosTool.Center( self.target , 0 , -28 )  

end

function LogicGUI.TemplGemCompound:__Server()
    --合成
    self:BindGlobalEvent(Macros.Event.GemModule.COMPOUND , function (_, data)
        self:OnButtonDelete()
    end)
end

function LogicGUI.TemplGemCompound:__SetShowUI( )

    local index = self.onIndex
    if self.onMax <= 0 then
        index = 1
    end

    self.onNum:SetString( index )
    local gemCnf = CallModuleFunc(ModuleType.GEM, "GetGemCnf" , self.id )
    self.debris:SetString( index * gemCnf.compose_cost_num )
    self.target:SetString( index )
end

--减少
function LogicGUI.TemplGemCompound:OnButtonSubtract( index )
    self.onIndex = self.onIndex - index
    if self.onIndex <= 0 then
        self.onIndex = 1
    end

    self:__SetShowUI()
end

--增加
function LogicGUI.TemplGemCompound:OnButtonAdd( index )
    self.onIndex = self.onIndex + index
    if self.onIndex >= self.onMax then
        self.onIndex = self.onMax
    end

    self:__SetShowUI()
end

--关闭
function LogicGUI.TemplGemCompound:OnButtonDelete( id )
    print("xy" , "关闭")
    self:DeleteMe()
end

--合成
function LogicGUI.TemplGemCompound:OnButtonCompound( id )
    print("xy" , "合成")
    if self.onMax ~= 0 and self.onIndex ~= 0 then
        local info = {}
        info.gem_num = self.onIndex
        info.gem_id = self.id
        CallModuleFunc(ModuleType.GEM, "GemComposeReq", info )
    else
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("gem_debris_insufficient"))
    end 
end


--
-- @brief 宝石分解
-- @author: yjg
-- @date: 2017年9月21日10:15:43
--
LogicGUI = LogicGUI or {}

LogicGUI.TemplGemResolve = LogicGUI.TemplGemResolve or BaseClass(GUI.Template)

function LogicGUI.TemplGemResolve:__init( id , func )
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID}   
    self.__open_callback = function()

        self.id = id

        self.func = func
        --默认选择数量
        self.onIndex = 1
        --可选择上限
        self.onMax = CallModuleFunc(ModuleType.GEM, "GetPack", id).num

        --信息
        self.gemCnf = CallModuleFunc(ModuleType.GEM, "GetGemCnf", id)

        self:__Getwidget( )
        self:__ShowUI( id )
        self:__SetShowUI()
    end


    --英雄选择
    self:BindGlobalEvent(Macros.Event.GemModule.DECOMPOSE , function( ... )
        if self.func then
            self.func()
        end
        self:OnButtonDelete()
    end)
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_GEM_RESOLVE)  
end

function LogicGUI.TemplGemResolve:__Getwidget( btnType )
    -- 内容背景
    self.Image_bg = self:GetWidget("Image_bg") 
    -- 标题
    self.Image_headline = self:GetWidget("Image_headline")
    --内容
    self.Image_content = self:GetWidget("Image_content")
    --碎片
    self.Image_debris = self:GetWidget("Image_debris")
    --名字
    self.Text_name = self:GetWidget("Text_name")
    --数量
    self.Text_num = self:GetWidget("Text_num")

    --数量背景
    self.Image_import = self:GetWidget("Image_import")
    
    --关闭
    self.Button_delete = Util:WidgetButton(  
                                            self:GetWidget("Button_delete") , 
                                            nil , 
                                            CPPGameLib.Handler(self,self.OnButtonDelete)
                                            ) 

    --减一
    self.Button_subtract1 = Util:WidgetButton(  
                                            self:GetWidget("Button_subtract1") , 
                                            nil , 
                                            function( ... )
                                                -- body
                                                self:OnButtonSubtract( 1 )
                                            end,nil,nil,nil,nil,0
                                            ) 
    --减一
    self.Button_subtract10 = Util:WidgetButton(  
                                            self:GetWidget("Button_subtract10") , 
                                            nil , 
                                            function( ... )
                                                self:OnButtonSubtract( 10 )
                                            end,nil,nil,nil,nil,0
                                            ) 

    --加一
    self.Button_add1 = Util:WidgetButton(  
                                            self:GetWidget("Button_add1") , 
                                            nil , 
                                            function( ... )
                                                self:OnButtonAdd( 1 )
                                            end,nil,nil,nil,nil,0
                                            ) 

    --加十
    self.Button_add1 = Util:WidgetButton(  
                                            self:GetWidget("Button_add10") , 
                                            nil , 
                                            function( ... )
                                                self:OnButtonAdd( 10 )
                                            end,nil,nil,nil,nil,0
                                            ) 

    --取消
    self.Button_cancel = Util:WidgetButton(  
                                            self:GetWidget("Button_cancel") , 
                                            CPPGameLib.GetString("common_cancel") ,  
                                            CPPGameLib.Handler(self,self.OnButtonCancel),
                                            nil,nil, Macros.Color.button_yellow
                                            ) 

    --合成
    self.Button_affirm = Util:WidgetButton(  
                                            self:GetWidget("Button_affirm") , 
                                            CPPGameLib.GetString("common_confirm") ,  
                                            CPPGameLib.Handler(self,self.OnButtonAffirm),
                                            nil,nil, Macros.Color.button_red
                                            ) 
end

function LogicGUI.TemplGemResolve:__ShowUI( id )
    --标题
    local gem_ongkeyresolve = Util:Label( CPPGameLib.GetString("gem_ongkeyresolve") , Macros.TypefaceSize.popup , Macros.Color.lilyWhite )
    self.Image_headline:AddChild( gem_ongkeyresolve )
    PosTool.Center(gem_ongkeyresolve)

    --数量
    self.onNum = Util:Label( self.onIndex , Macros.TypefaceSize.normal , Macros.Color.content )
    self.onNum:SetAnchorPoint( 0.5 , 0.5 )
    self.Image_import:AddChild( self.onNum )
    PosTool.Center( self.onNum )

    local icon_layer = LogicGUI.IconLayer.New( false )
    icon_layer:SetData({id = id })
    self.Image_debris:AddChild(icon_layer:GetNode())
    PosTool.Center(icon_layer)   

    icon_layer:ShowIconQuality(false)
    icon_layer:ShowIconShade(false)
    icon_layer:ShowIconBG(false)

    Util:WidgetLabel( self.Text_name , self.gemCnf.name , Macros.TypefaceSize.popup , Macros.Color.headline )

    Util:WidgetLabel( self.Text_num , CPPGameLib.GetString("gem_own") , Macros.TypefaceSize.largish , Macros.Color.keypoint )

    local num = Util:Label( self.onMax , Macros.TypefaceSize.largish , Macros.Color.content )
    self.Image_debris:AddChild( num )
    PosTool.RightTo( self.Text_num , num )

    local tab = {
                    { zi = CPPGameLib.GetString("gem_resolve_gain") , ys = Macros.Color.keypoint , dx = Macros.TypefaceSize.largish },
                    { zi = "100" , ys = Macros.Color.content , dx = Macros.TypefaceSize.largish , jg = 5},
                }
    --用作好几个组件拼起来居中显示的强迫症控件 2.0版
    self.gem_resolve_gain , self.gainLst = Util:LabCenter(tab)
    self.gem_resolve_gain:SetAnchorPoint( 0.5 , 0.5 )
    self.Image_import:AddChild( self.gem_resolve_gain )
    PosTool.Center( self.gem_resolve_gain , 0 , -40 ) 

end

function LogicGUI.TemplGemResolve:__SetShowUI( )

    local index = self.onIndex 
    if self.onMax == 0 then
        index = 1
    end

    self.onNum:SetString( index )
    self.gainLst[2]:SetString( index * self.gemCnf.gem_coin )
    self.gem_resolve_gain:SetPosition( self.Image_import:GetContentWidth()/2 , self.gem_resolve_gain:GetPositionY() )
end

--减少
function LogicGUI.TemplGemResolve:OnButtonSubtract( index )
    self.onIndex = self.onIndex - index
    if self.onIndex <= 0 then
        self.onIndex = 1
    end
    self:__SetShowUI()
end

--增加
function LogicGUI.TemplGemResolve:OnButtonAdd( index )
    self.onIndex = self.onIndex + index
    if self.onIndex >= self.onMax then
        self.onIndex = self.onMax
    end
    self:__SetShowUI()
end

--关闭
function LogicGUI.TemplGemResolve:OnButtonDelete( id )
    print("xy" , "关闭")
    self:DeleteMe()
end

--取消
function LogicGUI.TemplGemResolve:OnButtonCancel( id )
    print("xy" , "取消")
    self:OnButtonDelete()
end

--确定
function LogicGUI.TemplGemResolve:OnButtonAffirm( id )
    print("xy" , "确定")
    if self.gemCnf.type ~= 1 then

        local tips = LogicGUI.TemplAffirmTips.New( { 
                                                        type = LogicGUI.TemplAffirmTipsType.YES_NO,
                                                        content = " ",
                                                        confirm_func = function ( ... )
                                                            local info = {}
                                                            info.gem_id = self.id 
                                                            info.gem_num = self.onIndex
                                                            CallModuleFunc(ModuleType.GEM, "GemDecomposeReq", info )
                                                        end
                                                    } )
        tips:SetLoadedCallback( function ( ... )
            local gem_dismantle_tips_1 = Util:Label( CPPGameLib.GetString("gem_dismantle_tips_1") , Macros.TypefaceSize.popup , Macros.Color.content )
            gem_dismantle_tips_1:SetAnchorPoint(0.5,0.5)
            gem_dismantle_tips_1:SetPosition(tips:GetNode():GetContentWidth()/2 , tips:GetNode():GetContentHeight()/2 + 20 )
            tips:GetNode():AddChild(gem_dismantle_tips_1)

            local gem_dismantle_tips_2 = Util:Label( CPPGameLib.GetString("gem_dismantle_tips_2") , Macros.TypefaceSize.popup , Macros.Color.content )
            gem_dismantle_tips_2:SetAnchorPoint(0.5,0.5)
            gem_dismantle_tips_2:SetPosition(tips:GetNode():GetContentWidth()/2 , tips:GetNode():GetContentHeight()/2 - 25 )
            tips:GetNode():AddChild(gem_dismantle_tips_2)
        end )

    else
        local info = {}
        info.gem_id = self.id 
        info.gem_num = self.onIndex
        CallModuleFunc(ModuleType.GEM, "GemDecomposeReq", info )
    end
end

