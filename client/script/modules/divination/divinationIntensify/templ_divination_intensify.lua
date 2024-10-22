
--[[
%% @module: 卡牌强化界面
%% @author: yjg
%% @created: 2017年11月14日16:36:04
--]]

Modules = Modules or {}

Modules.TemplDivinationIntensify = Modules.TemplDivinationIntensify or BaseClass(BaseTabView)

function Modules.TemplDivinationIntensify:__init()
end

--选项卡界面创建完成回调
function Modules.TemplDivinationIntensify:__LoadedCallback()
    self:__Getwidget()
    self:__ShowUI()
end

--选项卡界面打开回调
function Modules.TemplDivinationIntensify:__OpenCallback()
    self:__Server()
    self:__SetShowUI( nil , true )
end

--选项卡界面关闭回调
function Modules.TemplDivinationIntensify:__CloseCallback()
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.TemplDivinationIntensify:__Dispose()
    self:DelRollEffects()
end

function Modules.TemplDivinationIntensify:__Server()
    self:BindGlobalEvent(Macros.Event.DivinationModule.DIVINATION_UP , function ( ... )
        self:UpEffects()
        self:__SetShowUI()
    end)
end

--获得所有组件
function Modules.TemplDivinationIntensify:__Getwidget()
    --物品背景
    self.Image_packBg = self:GetWidget("Image_packBg")
    self.Image_packBg:SetImage(Resource.PathTool.GetUIResPath("citycommonslices/bg_sp_equip_strengthen"), TextureResType.LOCAL)

    --名字背景
    self.Image_nameBg = self:GetWidget("Image_nameBg")
    self.Image_nameBg:SetVisible(false)
    --物品图标
    self.Panel_icon = self:GetWidget("Panel_icon")
    --信息背景
    self.Image_infoBg = self:GetWidget("Image_infoBg")
    --信息容器    
    self.Panel_property = self:GetWidget("Panel_property")

    --等级
    self.Image_level = self:GetWidget("Image_level")
    self.Image_level_value = self:GetWidget("Image_level_value")

    --战斗力
    self.Image_attack = self:GetWidget("Image_attack")
    self.Image_attack_value = self:GetWidget("Image_attack_value")

    --暴击
    self.Image_crit = self:GetWidget("Image_crit")
    self.Image_crit_value = self:GetWidget("Image_crit_value")

    --分割线
    self.Image_division = self:GetWidget("Image_division")

    --进度条
    self.Panel_barBg = self:GetWidget("Panel_Panel_barBg")

    --图标背景
    self.Image_iconLst = self:GetWidget("Image_iconLst")

    --自动选择
    self.Button_autoSelect = Util:WidgetButton( 
                                            self:GetWidget("Button_autoSelect") , 
                                            CPPGameLib.GetString("divination_autoSelect") , 
                                            CPPGameLib.Handler(self,self.OnButtonAutoSelect) ,
                                            nil, nil, Macros.Color.button_yellow
                                            )

    --取消选择
    self.Button_cancelSelect = Util:WidgetButton( 
                                            self:GetWidget("Button_cancelSelect") , 
                                            CPPGameLib.GetString("divination_cancelSelect") , 
                                            CPPGameLib.Handler(self,self.OnButtonCancelSelect) ,
                                            nil, nil, Macros.Color.button_yellow
                                            )

    --强化
    self.Button_intensify = Util:WidgetButton( 
                                            self:GetWidget("Button_intensify") , 
                                            CPPGameLib.GetString("divination_intensify") , 
                                            CPPGameLib.Handler(self,self.OnButtonIntensify) ,
                                            nil, nil, Macros.Color.button_yellow
                                            )
end

function Modules.TemplDivinationIntensify:__ShowUI()
    local imgdata_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_preview")
    local imgdata_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_bar_interior_bg1")
    --进度条
    self.__bar_exp = GUI.ProgressBar9Layer.New(imgdata_bar, imgdata_bg, self.Image_iconLst:GetContentWidth() - 5,18, self.Image_iconLst:GetContentWidth(),23 , 100, 100)
    self.__bar_exp:SetAnchorPoint(0.5, 0.5)
    self.Panel_barBg:AddChild(self.__bar_exp:GetNode())
    PosTool.Center(self.__bar_exp)

    --进度条值
    self.barValue = Util:Label("00/00",nil,Macros.Color.lilyWhite)
    self.barValue:SetAnchorPoint(0.5,0.5)
    self.barValue:SetPosition(self.__bar_exp:GetNode():GetPositionX() , self.__bar_exp:GetNode():GetPositionY() )
    self.Panel_barBg:AddChild(self.barValue)

    -- 图标
    local path = Resource.PathTool.GetBigIconPath(20101)
    self.icon = Util:Sprite(path)
    self.icon:SetPosition(self.Panel_icon:GetContentWidth()/2 ,self.Panel_icon:GetContentHeight()/2 - 60  )
    self.icon:SetAnchorPoint(0.5, 0.5 )
    self.Panel_icon:AddChild(self.icon)

    -- 图标框
    self.icon_frame = Util:Sprite(path)
    self.icon_frame:SetPosition(self.icon:GetContentWidth()/2 ,self.icon:GetContentHeight()/2 )
    self.icon_frame:SetAnchorPoint(0.5, 0.5 )
    self.icon:AddChild(self.icon_frame)

    --名字
    self.name = Util:Name( " " , Macros.TypefaceSize.tab )
    self.name:SetAnchorPoint( 0.5 , 0.5 )
    self.name:SetPosition( 60 + 70 + 13 , 35 )
    self.icon_frame:AddChild( self.name )

    --强化等级
    local equip_intensifyLv = Util:Label( CPPGameLib.GetString("equip_Lv") , nil , Macros.Color.keypoint )
    equip_intensifyLv:SetAnchorPoint(0.5 , 0.5)
    equip_intensifyLv:SetPosition( self.Image_level:GetContentWidth()/2 , self.Image_level:GetContentHeight()/2 )
    self.Image_level:AddChild(equip_intensifyLv)

    --强化等级
    self.intensifyLv = Util:Label( " " , nil , Macros.Color.content )
    self.intensifyLv:SetAnchorPoint( 0 , 0.5 )
    self.intensifyLv:SetPosition( 10 , self.Image_level_value:GetContentHeight()/2 )
    self.Image_level_value:AddChild(self.intensifyLv)
    
    -- 属性1
    self.pro_1_type = Util:Label(" " , nil , Macros.Color.keypoint )
    self.pro_1_type:SetAnchorPoint(0.5 , 0.5)
    self.pro_1_type:SetPosition( self.Image_attack:GetContentWidth()/2 , self.Image_attack:GetContentHeight()/2 )
    self.Image_attack:AddChild(self.pro_1_type)

    -- 属性1
    self.pro_1_value = Util:Label(" " , nil , Macros.Color.content )
    self.pro_1_value:SetAnchorPoint(0 , 0.5)
    self.pro_1_value:SetPosition( 10 , self.Image_attack_value:GetContentHeight()/2 )
    self.Image_attack_value:AddChild(self.pro_1_value)

    --属性2
    self.pro_2_type = Util:Label(" " , nil , Macros.Color.keypoint )
    self.pro_2_type:SetAnchorPoint(0.5 , 0.5)
    self.pro_2_type:SetPosition( self.Image_crit:GetContentWidth()/2 , self.Image_crit:GetContentHeight()/2 )
    self.Image_crit:AddChild(self.pro_2_type)

    --属性2
    self.pro_2_value = Util:Label(" ", nil , Macros.Color.content)
    self.pro_2_value:SetAnchorPoint(0 , 0.5)
    self.pro_2_value:SetPosition( 10 , self.Image_crit_value:GetContentHeight()/2 )
    self.Image_crit_value:AddChild(self.pro_2_value)

    --材料列表
    local params =  
    {
        item_class = Modules.TemplDivinationIntensifyItem,
        item_width = self.Image_iconLst:GetContentWidth() / 4 ,
        item_height = 130 ,
        row = 3 ,
        col = 4 ,
        item_space_r = 0 ,
        view_width = self.Image_iconLst:GetContentWidth(),
        view_height = self.Image_iconLst:GetContentHeight() - 10 ,

        touchable = true,
        touch_callback = function ( index , data )
            if self.__in_up_effect then
                return
            end

            local info = CallModuleFunc(ModuleType.DIVINATION, "GetDivination" , data.uid )
            if not info then
                return
            end

            local type_ = CallModuleFunc(ModuleType.DIVINATION, "GetOnDogFood" , data.uid )
            if type_ then
                CallModuleFunc(ModuleType.DIVINATION, "SetSelectedStrengthenMaterial" , data.uid , nil )
            else
                CallModuleFunc(ModuleType.DIVINATION, "SetSelectedStrengthenMaterial" , data.uid , true )
            end

            self:SelectedIcon(index, data.uid)
        end
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.Image_iconLst:AddChild(self.lst_list:GetNode())
    self.lst_list:SetPosition(self.Image_iconLst:GetContentWidth()/2 , self.Image_iconLst:GetContentHeight()/2 )

    --设置材料列表
    local divinationUid = CallModuleFunc(ModuleType.DIVINATION, "GetOnDivinationUid")

    self.lst_list:SetDataList( CallModuleFunc(ModuleType.DIVINATION, "GetDogFoodLst" , divinationUid ) )
end

function Modules.TemplDivinationIntensify:__SetShowUI( _ , type_ )
    local divinationUid = CallModuleFunc(ModuleType.DIVINATION, "GetOnDivinationUid")
    local info = CallModuleFunc(ModuleType.DIVINATION, "GetDivination" , divinationUid )
    
    self.icon:SetImage(Resource.PathTool.GetBigIconPath( info.bigicon ))

    local sp = Resource.PathTool.GetUIResPath("divinationbig/icon_sp_card_".. info.quality )
    -- Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_ICON, "icon_sp_card_" .. info.quality )
    self.icon_frame:SetImage( sp )
    self.icon_frame:SetPosition(self.icon:GetContentWidth()/2 ,self.icon:GetContentHeight()/2 )

    self.name:SetString( info.name )

    --等级
    self.intensifyLv:SetString( info.level )

    local pro_info = CallModuleFunc(ModuleType.DIVINATION, "ToInfoIdGetDivinationInfo" , info.info_id , info.level , info.break_level )

    --是否保存当前强化等级
    if type_ then
        self.nowLevel = info.level
        self.nowExp = info.exp

        for i = 1 , 2 do
            self[ "pro_" .. i .."_type" ]:SetString( " " ) 
            self[ "pro_" .. i .."_value" ]:SetString( " " ) 
        end

        --属性
        for i , v in ipairs( pro_info.up_proEx ) do
            local type_ , value_ = Util:Nature( v.type , v.value , true )
            if self[ "pro_" .. i .."_type" ] then
                self[ "pro_" .. i .."_type" ]:SetString( type_ ) 
                self[ "pro_" .. i .."_value" ]:SetString( value_ ) 
            end
        end
    end

    --经验条
    local _ , __ , now_info = CallModuleFunc(ModuleType.DIVINATION, "GetDivinationUpMAX" , divinationUid )

    self.__bar_exp:SetProgressMax( now_info.exp )
    self.__bar_exp:SetProgress( info.exp )
    self.barValue:SetString( info.exp .. "/" .. now_info.exp )
    if info.exp > now_info.exp then
         self.__bar_exp:SetProgressMax( 100 )
        self.__bar_exp:SetProgress( 100 )
        self.barValue:SetString( CPPGameLib.GetString("equip_toMax") )
    end

    self:OnButtonCancelSelect()    
end

--设置选中材料
function Modules.TemplDivinationIntensify:SelectedIcon(sel_dog_food_index, sel_dog_food_uid, ignore_upper_limit)
    --选中
    local sel_dog_food_item = self.lst_list:GetItem(sel_dog_food_index)    
    if sel_dog_food_item then
        sel_dog_food_item:SetSelect()
    end

    --当前卡牌信息
    local divinationUid = CallModuleFunc(ModuleType.DIVINATION, "GetOnDivinationUid")
    local info = CallModuleFunc(ModuleType.DIVINATION, "GetDivination", divinationUid)

    --统计材料所得经验
    local foodLst = CallModuleFunc(ModuleType.DIVINATION, "GetOnDogFood")
    local food_num = 0
    local item_exp
    local add_exp = 0
    local sel_item_exp = 0
    for id, v in pairs(foodLst) do
        food_num = food_num + 1
        item_exp = CallModuleFunc(ModuleType.DIVINATION, "GetOneDogFoodExp", id)
        add_exp = add_exp + item_exp

        --选中材料的经验
        if id == sel_dog_food_uid then
            sel_item_exp = item_exp
        end        
    end

    --当前卡牌经验+材料所得经验
    local total_exp = info.exp + add_exp

    --根据所选材料，计算可升多少级
    local changeLevel = CallModuleFunc(ModuleType.DIVINATION, "ToExpGetUpLevel" , divinationUid , add_exp)
    --获取卡牌最高等级、当前等级信息
    local max_info, _, now_info = CallModuleFunc(ModuleType.DIVINATION, "GetDivinationUpMAX" , divinationUid)

    local is_abort

    --经验溢出
    if not ignore_upper_limit and 
        (now_info.total_exp + total_exp - sel_item_exp >= max_info.total_exp + max_info.exp) then
        --已达上限，中断
        if info.level >= max_info.level and info.exp >= max_info.exp then
            is_abort = true
            self.barValue:SetString( CPPGameLib.GetString("equip_toMax") )

        --材料数量大于1(对于经验溢出的卡牌，需要一张卡牌触发强化)，中断
        elseif food_num > 1 then
            is_abort = true
        end
    end

    if is_abort then
        --取消选中
        if sel_dog_food_item then
            CallModuleFunc(ModuleType.DIVINATION, "SetSelectedStrengthenMaterial", sel_dog_food_uid, nil)
            sel_dog_food_item:SetSelect()
            GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("divination_levelup_max_tips"))
        end
        return
    end

    local level_str = "+ " .. changeLevel 
    self.barValue:SetString( total_exp .. "/" .. now_info.exp )

    --预览的进度条
    local preview_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_bar_bar_preview")
    self.__bar_exp:StartPreviewEffect( total_exp, preview_bar )        

    -- 属性显示
    if self.previewLst then
        for _, preview_node in pairs(self.previewLst) do
            preview_node:RemoveSelf(true)
        end
    end
    self.previewLst = {}

    if changeLevel > 0 then
        self.previewLst[1] = Util:Label( level_str  , nil , Macros.Color.front_color )
        self.previewLst[1]:SetAnchorPoint(0 , 0.5)
        self.previewLst[1]:SetPosition( self.Image_level_value:GetContentWidth()/2 , self.Image_level_value:GetContentHeight()/2 )
        self.Image_level_value:AddChild(self.previewLst[1])

        --属性
        local nowPropLst = CallModuleFunc(ModuleType.DIVINATION, "ToInfoIdGetDivinationInfo" , info.info_id , info.level )
        
        local nextPropLst = CallModuleFunc(ModuleType.DIVINATION, "ToInfoIdGetDivinationInfo" , info.info_id , (changeLevel + info.level))
        
        local type_ , value_ = Util:Nature( nextPropLst.up_proEx[1].type , nextPropLst.up_proEx[1].value - nowPropLst.up_proEx[1].value , true )
        self.previewLst[2] = Util:Label( value_  , nil , Macros.Color.front_color )
        self.previewLst[2]:SetAnchorPoint(0 , 0.5)
        self.previewLst[2]:SetPosition( self.Image_attack_value:GetContentWidth()/2 , self.Image_attack_value:GetContentHeight()/2 )
        self.Image_attack_value:AddChild(self.previewLst[2])

        if nextPropLst.up_proEx[2] then
            local type_ , value_ = Util:Nature( nextPropLst.up_proEx[2].type , nextPropLst.up_proEx[2].value - nowPropLst.up_proEx[2].value , true )
            if tostring(value_) ~= "+0" and tostring(value_) ~= "+0%" then
                self.previewLst[3] = Util:Label( value_  , nil , Macros.Color.front_color )
                self.previewLst[3]:SetAnchorPoint(0 , 0.5)
                self.previewLst[3]:SetPosition( self.Image_crit_value:GetContentWidth()/2 , self.Image_crit_value:GetContentHeight()/2 )
                self.Image_crit_value:AddChild(self.previewLst[3])   
            end
        end             
    end
end

--自动选择
function Modules.TemplDivinationIntensify:OnButtonAutoSelect()
    local divinationUid = CallModuleFunc(ModuleType.DIVINATION, "GetOnDivinationUid")    
    --当前卡牌信息
    local info = CallModuleFunc(ModuleType.DIVINATION, "GetDivination" , divinationUid )
    --获取卡牌最高等级、当前等级信息    
    local max_info, _, now_info = CallModuleFunc(ModuleType.DIVINATION, "GetDivinationUpMAX" , divinationUid )
    if info.level >= max_info.level and info.exp >= now_info.exp then
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("divination_levelup_max_tips"))
        return
    end
   
    Modules.TemplDebrisellScreen.New( 
        { 
            {str = CPPGameLib.GetString("divination_expCard"), quality = 0},
            {str = CPPGameLib.GetString("divination_whiteCard"), quality = 0},
            {str = CPPGameLib.GetString("divination_greenCard"), quality = 1},
            {str = CPPGameLib.GetString("divination_blueCard"), quality = 2},
        } , 
        function( lst )
            local has_exp_card, card_quality_map
            for k, v in pairs(lst) do
                if v == 1 then
                    has_exp_card = true
                else
                    card_quality_map = card_quality_map or {}
                    card_quality_map[v - 2] = true
                end
            end

            if has_exp_card or card_quality_map then
                local food_num = 0
                local total_exp = info.exp
                self.lst_list:ItemDataIter(function(index, data, item)

                    --经验卡牌、品质卡牌
                    if (has_exp_card and data.type2 == Macros.Game.CARD_QUALITY_TYPE.EXP) or (card_quality_map and card_quality_map[data.quality]) then

                        --经验溢出
                        if now_info.total_exp + total_exp >= max_info.total_exp + max_info.exp then
                            --已达上限，中断
                            if info.level >= max_info.level and info.exp >= max_info.exp then
                                return true
    
                            --材料数量大于1(对于经验溢出的卡牌，需要一张卡牌触发强化)，中断
                            elseif food_num >= 1 then
                                return true
                            end
                        end

                        --添加材料
                        local cur_exp = CallModuleFunc(ModuleType.DIVINATION, "GetOneDogFoodExp", data.uid)
                        total_exp = total_exp + cur_exp
                        food_num = food_num + 1
                        CallModuleFunc(ModuleType.DIVINATION, "SetSelectedStrengthenMaterial", data.uid , true)
                    end
                end)                
            end

            --刷新材料列表
            self.lst_list:ItemIter(function(index, item)
                item:SetSelect()
            end)
            self:SelectedIcon(nil, nil, true)
        end)
end

--取消选择
function Modules.TemplDivinationIntensify:OnButtonCancelSelect()
    CallModuleFunc(ModuleType.DIVINATION, "SetSelectedStrengthenMaterial")

    self.lst_list:ItemIter(function(index, item)
        item:SetSelect()
    end)
    self:SelectedIcon()
    self:JudgeMatericalTips()
end

function Modules.TemplDivinationIntensify:JudgeMatericalTips()
    if self.lst_list:GetItemCount() <= 0 then
        --提示文字
        if not self.__move_to_achievement_materical then
            --提示文字
            self.__move_to_achievement_materical = Util:Label(CPPGameLib.GetString("divination_achievement_materical_tips"), Macros.TypefaceSize.normal, Macros.Color.keypoint)
            self.Image_iconLst:AddChild(self.__move_to_achievement_materical)
            PosTool.Center(self.__move_to_achievement_materical,0,30)
            --获取按钮
            local img = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_achieving",true)
            self.__move_to_achievement_btn = Util:Button(img,function ()
                --前往占卜
                GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.DIVINATION)
            end)
            self.Image_iconLst:AddChild(self.__move_to_achievement_btn)
            PosTool.RightBottom(self.__move_to_achievement_btn,-70,110)
        end
        self.__move_to_achievement_materical:SetVisible(true)
        self.__move_to_achievement_btn:SetVisible(true)
    else
        if self.__move_to_achievement_materical then
            self.__move_to_achievement_materical:SetVisible(false)
            self.__move_to_achievement_btn:SetVisible(false)
        end
    end
end

--强化
function Modules.TemplDivinationIntensify:OnButtonIntensify()
    local lst = CallModuleFunc(ModuleType.DIVINATION, "GetOnDogFood")
    local divinationUid = CallModuleFunc(ModuleType.DIVINATION, "GetOnDivinationUid")
    local num = CPPGameLib.GetTableLength( lst )
    if num <= 0 then
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("divination_noSelectFood"))
        return
    end
    local info = {}
    info.uid = divinationUid
    info.cost_card_list = {}
    local t1 = {}
    for k , v in pairs( lst ) do
        table.insert(info.cost_card_list , {id = k}  )
        t1[ k ] = true
    end

    --图标播放特效
    self.lst_list:ItemIter(function(index, item)
        if item then
            local data = item:GetData()
            item:SetSelectVisible(false)
            if t1[ data.uid  ] then
                if item.icon then
                    local effect_id,effect = self:__CreateEffect({id = 100015 , loop = false,time_scale = 2 } , item.icon , 2 )
                    PosTool.Center( effect )
                end
            end
        end
    end)

    CallModuleFunc(ModuleType.DIVINATION, "CardLevelUpReq" , info )
end

--升级特效
function Modules.TemplDivinationIntensify:UpEffects()
    self:DelRollEffects()
    self.faceEffects = {}

    local divinationUid = CallModuleFunc(ModuleType.DIVINATION, "GetOnDivinationUid")
    local info = CallModuleFunc(ModuleType.DIVINATION, "GetDivination" , divinationUid )
    local pro_info = CallModuleFunc(ModuleType.DIVINATION, "ToInfoIdGetDivinationInfo" , info.info_id , info.level , info.break_level )

    --升级了
    if self.nowLevel ~= info.level then
        self.__in_up_effect = true

        --特效
        local pos = {
            [1] = { x = self.Image_level:GetPositionX() , y = self.Image_level_value:GetPositionY() + 400 },
            [2] = { x = self.Image_attack:GetPositionX() , y = self.Image_attack_value:GetPositionY() + 400 },
        }

        local t1 = LogicGUI.DivinationIntensifyFontEffects:GetDivinationIntensifyTab( info , pos )

        local windwords_effect = WindWordsEffects.New()
        windwords_effect:Subsection( self.__root_widget , self.Image_infoBg , t1 , 10 ,function ( ... )
            --属性
            for i , v in ipairs( pro_info.up_proEx ) do
                local type_ , value_ = Util:Nature( v.type , v.value , true )
                if self[ "pro_" .. i .."_type" ] then
                    self.intensifyLv:SetString( info.level )
                    self.faceEffects[i] = TypefaceEffects.New()
                    self.faceEffects[i]:Roll( self[ "pro_" .. i .."_value" ] , value_ , "" , function ( ... )
                        -- 属性显示
                        if self.previewLst then
                            for i = 1 , #self.previewLst do
                                if self.previewLst[i] then
                                    self.previewLst[i]:RemoveSelf(true)
                                    self.previewLst[i] = nil
                                end
                            end
                        end
                    end)
                end
            end

            self.__in_up_effect = false

            local divinationUid = CallModuleFunc(ModuleType.DIVINATION, "GetOnDivinationUid")
            self.lst_list:SetDataList( CallModuleFunc(ModuleType.DIVINATION, "GetDogFoodLst" , divinationUid ) )

            local preview_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_bar_bar_preview")
            self.__bar_exp:StartPreviewEffect( 0 , preview_bar )

            self:JudgeMatericalTips()
        end)

    else
        local layout = Util:Layout( 100 , 100 )
        layout:SetAnchorPoint( 0.5 , 0.5 )

        local tab = {
                        { zi = CPPGameLib.GetString("divination_expAdd") , ys = 0 , mb = true, dx = Macros.TypefaceSize.popup },
                        { zi = info.exp - self.nowExp , ys = 1 , jg = 5, mb = true, dx = Macros.TypefaceSize.popup },
                    }
        local item_des = Util:LabCenter(tab)
        item_des:SetAnchorPoint(0.5 , 0.5)
        layout:AddChild(item_des)
        PosTool.Center( item_des )

        GlobalTipMsg.GetInstance():ShowLayer( layout , false , true , true  )

        local divinationUid = CallModuleFunc(ModuleType.DIVINATION, "GetOnDivinationUid")
        self.lst_list:SetDataList( CallModuleFunc(ModuleType.DIVINATION, "GetDogFoodLst" , divinationUid ) )
    end

    self.nowLevel = info.level
    self.nowExp = info.exp
end

--销毁滚字特效
function Modules.TemplDivinationIntensify:DelRollEffects()
    if self.faceEffects then
        for _, effect in pairs(self.faceEffects) do
            effect:DeleteMe()
        end
        self.faceEffects = nil
    end
end

--------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------
--[[
    强化材料条目
--]]

Modules = Modules or {}
Modules.TemplDivinationIntensifyItem = Modules.TemplDivinationIntensifyItem or BaseClass(GUI.ScrollItem)

function Modules.TemplDivinationIntensifyItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplDivinationIntensifyItem:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(90,130)
    self.layout:SetBackGroundColor(0, 0, 0)
    return self.layout
end

function Modules.TemplDivinationIntensifyItem:__delete()
    
end

function Modules.TemplDivinationIntensifyItem:SetData( data )
    GUI.ScrollItem.SetData(self, data)
    self:__ShowUI()
end

function Modules.TemplDivinationIntensifyItem:__ShowUI()
    local data = self.__data

    --头像
    self.icon = LogicGUI.IconLayer.New(false, false)
    self.icon:SetPosition(self.layout:GetContentWidth()/2 ,self.layout:GetContentHeight() / 2 + 25 )
    self.icon:SetAnchorPoint(0.5, 0.5 )
    self.layout:AddChild(self.icon:GetNode())
    self.icon:SetData({id = data.info_id , obj_info = data })
    self.icon:ShowEquipStrengthenLev(true)
    self.icon:SetTouchSwallow(false)
    self.icon:SetScaleFactor( 0.9 )

    --名字
    self.name = Util:Name( data.name , Macros.TypefaceSize.slightly , data.quality , nil , self.layout:GetContentWidth() )
    self.name:SetAnchorPoint(0.5,1)
    self.name:SetPosition( self.layout:GetContentWidth()/2  , self.icon:GetNode():GetPositionY() - (self.icon:GetNode():GetContentWidth()/2) + 5 )
    self.layout:AddChild(self.name)

    local black1_bg_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "black1_bg")
    self.select = Util:Sprite9(black1_bg_sp)
    self.select:SetPosition( self.layout:GetContentWidth()/2  , self.icon:GetNode():GetPositionY() )
    self.select:SetContentSize( self.icon:GetNode():GetContentWidth() - 4 , self.icon:GetNode():GetContentHeight() - 4 )
    self.layout:AddChild( self.select , 20 )

    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_select_yes")
    local select_ = Util:Sprite(sp_2)
    self.select:AddChild( select_ , 20 ) 
    PosTool.Center( select_ )

    self:SetSelect()
end

function Modules.TemplDivinationIntensifyItem:SetSelect()
    local type_ = CallModuleFunc(ModuleType.DIVINATION, "GetOnDogFood" , self.__data.uid )
    self.select:SetVisible( type_ )
end

function Modules.TemplDivinationIntensifyItem:SetSelectVisible( type_ )
    self.select:SetVisible( type_ )
end
