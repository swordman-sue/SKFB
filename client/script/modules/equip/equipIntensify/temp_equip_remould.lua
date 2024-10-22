--[[
%% @module: 装备进化
%% @author: yjg
%% @created: 2018年7月31日16:36:48
--]]

Modules = Modules or {} 
Modules.EquipRemouldView = Modules.EquipRemouldView or BaseClass(BaseView)

function Modules.EquipRemouldView:__init()
    self.__layout_name = ""  
    self.__layout_mode = BaseView.LayoutMode.CODE 
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bp_sp_remould"), image_size = COCOSize(1136,640) }
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("equip_evolution"),
        show_power = true,
        show_diamond = true,
        show_gold = true,
        show_level = true,
        close_func = function()
            self:CloseManual()
        end,
    }
    self.__open_callback = function()
        self:__OpenCallback()
    end
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.TREASURE)
end

function Modules.EquipRemouldView:__OpenCallback( )
    self:__Getwidget()
    self:__ShowUI()
    self:__Server()
end	

function Modules.EquipRemouldView:__Dispose()
end

function Modules.EquipRemouldView:__CreateWidgetFromLuaCode()
    self.widget = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height)
    return self.widget
end

function Modules.EquipRemouldView:__Server()
    self:BindGlobalEvent(Macros.Event.EquipModule.EQUIP_ADVANCE,function (_, uid)
        --刷新
        self:__SetShowUI( uid )
        local equipInfo = CallModuleFunc(ModuleType.EQUIP, "GetEquip", uid )
        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_REMOULD_SHOW , false , equipInfo.info_id , function( ... )
            local evolveInfo = CallModuleFunc(ModuleType.EQUIP, "ToUidGetEvolveInfo", uid )
            if not evolveInfo.nexInfo then
                self:CloseManual()
            end
        end  )
    end)
end

function Modules.EquipRemouldView:__Getwidget()
    --圆圈
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TREASURE, "sp_evolve_bg")
    local sp_evolve_bg = Util:Sprite( sp )
    self.widget:AddChild(sp_evolve_bg) 
    sp_evolve_bg:SetPosition( self.widget:GetContentWidth()/2 , self.widget:GetContentHeight()/2 + 45 )
    -- sp_evolve_bg:SetScaleFactor( 0.8 )

    --标题
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "sp_float_bg")
    local sp_float_bg = Util:Sprite( sp )
    sp_float_bg:SetAnchorPoint( 0.5 , 0.5 )
    sp_float_bg:SetPosition( sp_evolve_bg:GetContentWidth()/2 , sp_evolve_bg:GetContentHeight()/2 + 140 )
    sp_evolve_bg:AddChild(sp_float_bg) 

    --标题
    local treasure_materials = Util:Name( CPPGameLib.GetString( "treasure_materials" ) , Macros.TypefaceSize.popup )
    treasure_materials:SetAnchorPoint( 0.5 , 0.5 )
    sp_float_bg:AddChild(treasure_materials) 
    PosTool.Center( treasure_materials )

    --物品框
    local sp = Resource.PathTool.GetUIResPath("citycommonslices/bg_discoveryarmy")
    local bg_discoveryarmy = Util:Sprite( sp )
    bg_discoveryarmy:SetAnchorPoint( 0.5 , 0.5 )
    bg_discoveryarmy:SetPosition( sp_evolve_bg:GetContentWidth()/2 , sp_evolve_bg:GetContentHeight()/2 )
    sp_evolve_bg:AddChild(bg_discoveryarmy) 

    --材料框
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TREASURE, "sp_evolve_frame")
    self.sp_evolve_frame = Util:Sprite( sp )
    self.sp_evolve_frame:SetAnchorPoint( 0.5 , 0.5 )
    self.sp_evolve_frame:SetPosition( bg_discoveryarmy:GetContentWidth()/2 , bg_discoveryarmy:GetContentHeight()/2 )
    bg_discoveryarmy:AddChild(self.sp_evolve_frame) 

    --箭头
    local x = { 
                [1] = bg_discoveryarmy:GetContentWidth()/2 - 140,
                [2] = bg_discoveryarmy:GetContentWidth()/2 + 140,
             }
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_arrow")
    for i = 1 , 2 do
        local btn_arrow = Util:Sprite( sp )
        btn_arrow:SetPosition( x[i] , bg_discoveryarmy:GetContentHeight()/2 )
        bg_discoveryarmy:AddChild( btn_arrow )
    end

    --价格
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TREASURE, "sp_type")
    local sp_type = Util:Sprite( sp )
    sp_type:SetAnchorPoint( 0.5 , 1 )
    sp_type:SetPosition( bg_discoveryarmy:GetContentWidth()/2 , 0 )
    bg_discoveryarmy:AddChild(sp_type) 

    --价格
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp_bag_num")
    self.bg_sp_bag_num = Util:Sprite9( sp )
    self.bg_sp_bag_num:SetAnchorPoint( 0.5 , 0.5 )
    self.bg_sp_bag_num:SetPosition( sp_type:GetContentWidth()/2 , sp_type:GetContentHeight()/2 + 22 )
    self.bg_sp_bag_num:SetContentSize( sp_type:GetContentWidth() - 30 , 28 )
    sp_type:AddChild(self.bg_sp_bag_num) 

    --确定
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW , "btn_btn_bigred" )
    self.btn_bigred = Util:Button( sp ,  function ( ... )
        self:OnBtnBigred()
    end , CPPGameLib.GetString("treasure_remould") , Macros.TypefaceSize.tab , nil, Macros.Color.button_red )
    self.btn_bigred:SetPosition( sp_evolve_bg:GetContentWidth()/2 , -80 )
    sp_evolve_bg:AddChild(self.btn_bigred) 

    --查看信息
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW , "examine" )
    self.examine = Util:Button( sp ,  function ( ... )
        -- CallModuleFunc(ModuleType.EQUIP, "OpenView", BaseViewType.TREASURE_LIFT, 1, info)
    end )
    self.examine:SetPosition( sp_evolve_bg:GetContentWidth() + 220 , 100 )
    self.examine:SetVisible(false)
    sp_evolve_bg:AddChild(self.examine) 

    local service_examine = Util:Name( CPPGameLib.GetString("service_examine") )
    service_examine:SetPosition( self.examine:GetContentWidth()/2 , 0 )
    service_examine:SetAnchorPoint( 0.5 , 0.5 )
    self.examine:AddChild( service_examine )

    --柱子
    self.terrace = {}
    --属性
    self.plankLst = {}

    local x = { 
                [1] = { x = bg_discoveryarmy:GetContentWidth()/2 - 340 , sp = "sp_evolve_low" },
                [2] = { x = bg_discoveryarmy:GetContentWidth()/2 + 340 , sp = "sp_evolve_tall" }
             }

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "sp_float_bg")
    --箭头
    local arrow = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_arrow")
   
    for i = 1 , 2 do
        self.terrace[i] = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TREASURE, x[i].sp ) )
        self.terrace[i]:SetPosition( x[i].x , -250 )
        bg_discoveryarmy:AddChild( self.terrace[i] )

        local plank = Util:Sprite9( sp )
        plank:SetContentSize( plank:GetContentWidth() , 130 )
        self.terrace[i]:AddChild( plank )
        PosTool.Center( plank , 0 , 60 )

        self.plankLst[i] = {}
        for j = 1 , 4 do
            self.plankLst[i][j] = {}

            self.plankLst[i][j].type = Util:Label( "啊啊啊啊：" , nil , Macros.Color.orange )
            self.plankLst[i][j].type:SetAnchorPoint( 0 , 0.5 )
            self.plankLst[i][j].type:SetPosition( 15 , plank:GetContentHeight() - 25 - ( (j-1) *27 ) )
            plank:AddChild( self.plankLst[i][j].type )

            self.plankLst[i][j].value = Util:Label( "123456" , nil , Macros.Color.white )
            self.plankLst[i][j].value:SetAnchorPoint( 0 , 0.5 )
            self.plankLst[i][j].value:SetPosition( plank:GetContentWidth()/2 + 20 , self.plankLst[i][j].type:GetPositionY() )
            plank:AddChild( self.plankLst[i][j].value )

            if i == 2 then
                self.plankLst[i][j].up = Util:Sprite( arrow )
                self.plankLst[i][j].up:SetRotation( -90 )
                self.plankLst[i][j].up:SetScaleFactor( 0.5 ) 
                self.plankLst[i][j].up:SetPosition( plank:GetContentWidth() + 10 , self.plankLst[i][j].value:GetPositionY() )     
                self.plankLst[i][j].up:SetVisible(false)
                plank:AddChild( self.plankLst[i][j].up )

                ActionManager:FadeAnimation( self.plankLst[i][j].up , 1 )
            end
        end
    end
end

function Modules.EquipRemouldView:__ShowUI()
    local props = {}
    for i = 1 , 2 do
        props[i] = {}
        --图标
        local path = Resource.PathTool.GetBigIconPath( 40201 )
        props[i].icon = Util:Sprite(path)
        props[i].icon:SetPosition(self.terrace[i]:GetContentWidth()/2 , self.terrace[i]:GetContentHeight() + 110 )
        props[i].icon:SetAnchorPoint(0.5, 0.5 )
        props[i].icon:SetVisible( false )
        self.terrace[i]:AddChild( props[i].icon )
        props[i].icon:SetScaleFactor( 0.9 )
        ActionManager.GetInstance():UpDownMoveAnimation( props[i].icon , 1 , 15)
        --名字
        props[i].name = Util:Name( " " , Macros.TypefaceSize.largish , 0 )
        props[i].name:SetPosition( self.terrace[i]:GetContentWidth()/2 , self.terrace[i]:GetContentHeight() + 270 )
        props[i].name:SetAnchorPoint(0.5,0.5)
        props[i].name:SetVisible( false )
        self.terrace[i]:AddChild( props[i].name )
        --品质
        props[i].quality = Util:Name( " " , Macros.TypefaceSize.slightly , 0 )
        props[i].quality:SetPosition( self.terrace[i]:GetContentWidth()/2 , self.terrace[i]:GetContentHeight() + 240 )
        props[i].quality:SetAnchorPoint(0.5,0.5)
        props[i].quality:SetVisible( false )
        self.terrace[i]:AddChild( props[i].quality )
    end
    self.nowInfo = props[1]
    self.nexInfo = props[2]

    --名字
    self.consumeName = Util:Name( "" )
    self.consumeName:SetAnchorPoint( 0.5 , 0.5 )
    self.sp_evolve_frame:AddChild( self.consumeName )
    PosTool.Center( self.consumeName , 0 , 62 )

    --数量
    self.consumeNum = Util:Label( "" )
    self.consumeNum:SetAnchorPoint( 0.5 , 0.5 )
    self.sp_evolve_frame:AddChild( self.consumeNum )
    PosTool.Center( self.consumeNum , 0 , -62 )

    local onequipInfo = CallModuleFunc(ModuleType.EQUIP, "GetOnEquipInfo")
    local equipInfo = CallModuleFunc(ModuleType.EQUIP, "GetEquip",onequipInfo.uid)
    self:__SetShowUI( onequipInfo.uid )
end

function Modules.EquipRemouldView:__SetShowUI( uid )
    --进化信息
    local evolveInfo = CallModuleFunc(ModuleType.EQUIP, "ToUidGetEvolveInfo", uid )

    --清空属性
    for i = 1 , 2 do
        for j = 1 , 4 do
            self.plankLst[i][j].type:SetString( " " )
            self.plankLst[i][j].value:SetString( " " )
            if self.plankLst[i][j].up then
                self.plankLst[i][j].up:SetVisible( false )
            end
        end
    end
    --当前的圣器
    self:SetNowTreasure( evolveInfo.nowInfo )
    --进化后圣器
    self:SetNexTreasure( evolveInfo.nexInfo )
    --消耗
    self:SetConsume( evolveInfo.consume )
end

--现在的圣器
function Modules.EquipRemouldView:SetNowTreasure( info )
    if not info then
        --隐藏图标
        self.nowInfo.icon:SetVisible( false )
        --重置名字
        self.nowInfo.name:SetString( "" )
        --重置品级
        self.nowInfo.quality:SetString( " " )
        return
    end

    --图标
    self.nowInfo.icon:SetImage( Resource.PathTool.GetBigIconPath( info.res_id ) )
    self.nowInfo.icon:SetVisible( true )

    local text = ""
    if info.refine_level ~= 0 then
        text = "+" .. info.refine_level
    end
    --名字
    self.nowInfo.name:SetString( info.name .. text )
    self.nowInfo.name:SetVisible( true )
    self.nowInfo.name:SetColor(unpack(Macros.Quality[info.quality]))
    self.nowInfo.name:SetStrokeColor( unpack(Macros.Quality_Stroke[info.quality]) )
    --品级
    self.nowInfo.quality:SetString( CPPGameLib.GetString("equip_grade_",info.quality ) )
    self.nowInfo.quality:SetVisible( true )
    self.nowInfo.quality:SetColor(unpack(Macros.Quality[info.quality]))
    self.nowInfo.quality:SetStrokeColor( unpack(Macros.Quality_Stroke[info.quality]) )

    --属性
    local evolveInfo = CallModuleFunc(ModuleType.EQUIP, "GetLiftPropPlusInfos", info )  
    local t1 = {}
    table.insert( t1 , { type = CPPGameLib.GetString("equip_intensifyLv")  , value = info.strengthen_level } )
    for i , v in ipairs(evolveInfo) do
        local type_ , value = Util:Nature( v.type , v.value , true )
        table.insert( t1 , { type = type_ , value = value } )
    end

    for i , v in ipairs( t1 ) do
        self.plankLst[1][i].type:SetString( v.type )
        self.plankLst[1][i].value:SetString( v.value )
    end
end

--进化后圣器
function Modules.EquipRemouldView:SetNexTreasure( info )
    if not info then
        --隐藏图标
        self.nexInfo.icon:SetVisible( false )
        --重置名字
        self.nexInfo.name:SetString( "" )
        --重置品级
        self.nexInfo.quality:SetString( " " )
        return
    end

    --图标
    self.nexInfo.icon:SetImage( Resource.PathTool.GetBigIconPath( info.res_id ) )
    self.nexInfo.icon:SetVisible( true )
    local text = ""
    if info.refine_level ~= 0 then
        text = "+" .. info.refine_level
    end
    --名字
    self.nexInfo.name:SetString( info.name .. text )
    self.nexInfo.name:SetVisible( true )
    self.nexInfo.name:SetColor(unpack(Macros.Quality[info.quality]))
    self.nexInfo.name:SetStrokeColor( unpack(Macros.Quality_Stroke[info.quality]) )
    --品级
    self.nexInfo.quality:SetString( CPPGameLib.GetString("equip_grade_",info.quality ) )
    self.nexInfo.quality:SetVisible( true )
    self.nexInfo.quality:SetColor(unpack(Macros.Quality[info.quality]))
    self.nexInfo.quality:SetStrokeColor( unpack(Macros.Quality_Stroke[info.quality]) )

    --属性
    local evolveInfo = CallModuleFunc(ModuleType.EQUIP, "GetLiftPropPlusInfos", info , true )  
    local t1 = {}
    table.insert( t1 , { type = CPPGameLib.GetString("equip_intensifyLv")  , value = info.strengthen_level } )
    for i , v in ipairs(evolveInfo) do
        local type_ , value = Util:Nature( v.type , v.value , true )
        table.insert( t1 , { type = type_ , value = value } )
    end

    for i , v in ipairs( t1 ) do
        local text = ""
        if i ~= 1 then
            text =  CPPGameLib.GetString("common_colon")
        end

        self.plankLst[2][i].type:SetString( v.type .. text )
        self.plankLst[2][i].value:SetString( v.value )
        if i ~= 1 then
            self.plankLst[2][i].up:SetVisible( true )
        end
    end
end

--消耗
function Modules.EquipRemouldView:SetConsume( info )
    --消耗物品
    if self.consumeItem then
        self.consumeItem:RemoveSelf(false)
        self.consumeItem = nil
    end
    --货币
    if self.currency then
        self.currency:RemoveSelf( false )
        self.currency = nil
    end
    --名字
    self.consumeName:SetString( "" )
    --数量
    self.consumeNum:SetString( "" )
    --消耗
    self.consume = info

    if not info then
        return
    end

    --图标
    self.consumeItem = LogicGUI.IconLayer.New(true)
    self.consumeItem:SetAnchorPoint(0.5,0.5)
    self.consumeItem:SetData( {id = info.goods.item_id } )
    self.sp_evolve_frame:AddChild( self.consumeItem:GetNode() )
    PosTool.Center( self.consumeItem )

    --名字
    local type_ , info_ = ObjIDTool.GetInfo( info.goods.item_id )
    self.consumeName:SetString( info_.name )
    self.consumeName:SetColor(unpack(Macros.Quality[info_.quality]))

    --数量
    local num = CallModuleFunc(ModuleType.Pack,"GetNumByItemID", info.goods.item_id  )
    self.consumeNum:SetString( num .." / ".. info.goods.item_num )

    if num >= info.goods.item_num then
        self.consumeNum:SetColor( unpack( Macros.Color.green ) )
    else
        self.consumeNum:SetColor( unpack( Macros.Color.red ) )
    end

    --货币
    local tab = {
                    {tp = Util:GetMoneyIconImgData( info.currency.type ), jg = 5 ,dx = 0.8 },
                    {zi = info.currency.num , dx = Macros.TypefaceSize.normal, jg = 4 },
                }
    self.currency = Util:LabCenter(tab)
    self.currency:SetAnchorPoint( 0 , 0.5 )
    self.currency:SetPosition( 0 , self.bg_sp_bag_num:GetContentHeight()/2 )
    self.bg_sp_bag_num:AddChild( self.currency )
end

--消耗
function Modules.EquipRemouldView:OnBtnBigred( )

    if not self.consume then
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("treasure_remouldMax") ) 
        return
    end

    --物品数量
    local num = CallModuleFunc(ModuleType.Pack,"GetNumByItemID", self.consume.goods.item_id  )
    if num < self.consume.goods.item_num then
        local type_ , info_ = ObjIDTool.GetInfo( self.consume.goods.item_id )
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("hero_potential_tips2" , info_.name ) )  
        return    
    end
    --货币
    local item_num = CallModuleFunc(ModuleType.ROLE,"GetPropValue" , self.consume.currency.type )
    if item_num < self.consume.currency.num then
        local text = "currency_insufficient_"..self.consume.currency.type
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString(text) )  
        return   
    end

    local onequipInfo = CallModuleFunc(ModuleType.EQUIP, "GetOnEquipInfo")
    local info = {}
    info.uid = onequipInfo.uid
    CallModuleFunc(ModuleType.EQUIP, "EquipmentQualityUpReq", info ) 
end
