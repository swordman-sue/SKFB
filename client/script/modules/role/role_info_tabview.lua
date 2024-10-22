
--[[
%% @module: 角色信息界面
%% @author: swordman sue
%% @created: 2016-11-2
--]]

Modules = Modules or {}

Modules.RoleInfoTabView = Modules.RoleInfoTabView or BaseClass(BaseTabView)

function Modules.RoleInfoTabView:__init()
    self.__prop_recovery_node_map = {}
end

--选项卡界面创建完成回调
function Modules.RoleInfoTabView:__LoadedCallback()
    local role_info = self:CallModuleFunc("GetRoleInfo")

    --名字
    Util:WidgetLabel(   
                        self:GetWidget("Text_RoleName"), 
                        role_info.role_name, 
                        Macros.TypefaceSize.normal, 
                        Macros.Color.content
                    ) 

    --头像
    widget = self:GetWidget("Panel_RoleIcon")
    self.role_icon = LogicGUI.RoleIcon.New()
    self.role_icon:SetData(role_info.head_icon)
    widget:AddChild(self.role_icon:GetNode(), -1)
    PosTool.Center(self.role_icon)

    --切换头像
    local btn_change_head_icon = self:GetWidget("Button_ChangeHeadIcon")
    Util:WidgetButton(
                        btn_change_head_icon, 
                        CPPGameLib.GetString("role_info_change_headicon"), 
                        function()
                            LogicGUI.TemplRoleFacelift.New()
                        end, 
                        Macros.TypefaceSize.slightly ,
                        nil , Macros.Color.button_yellow
                    )
    --审核状态下屏蔽改头像按钮
    btn_change_head_icon:SetVisible(not CPPSdkTools.IsInExamine())

    --等级
    local common_lev2 = Util:WidgetLabel(   
                                            self:GetWidget("Text_RoleLevel"), 
                                            CPPGameLib.GetString("common_lev1"), 
                                            Macros.TypefaceSize.normal, 
                                            Macros.Color.keypoint
                                        ) 
    local level = Util:Label( role_info.level , Macros.TypefaceSize.normal , Macros.Color.content )
    self:GetWidget("Image_2"):AddChild(level)
    PosTool.RightTo(common_lev2 , level )

    --经验条
    widget = Util:WidgetLabel(self:GetWidget("Text_Exp_Title")," ") 
    local teamexpSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON, "teamexp", true)
    local teamexp = Util:Sprite(teamexpSp)
    widget:AddChild(teamexp)
    PosTool.Center(teamexp)

    local prog_bg = self:GetWidget("Image_ExpProg")
    local prog_bg_size = prog_bg:GetContentSize()
    local prog_bar_imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_PROGRESS, "prog_preview")
    local prog_bar = GUI.ProgressBar9Layer.New(prog_bar_imgdata, nil, prog_bg_size.width - 2 , prog_bg_size.height - 4 )
    prog_bar:SetAnchorPoint(0.5, 0.5)
    prog_bg:AddChild(prog_bar:GetNode())
    PosTool.Center(prog_bar)    
    prog_bar:SetProgress(role_info.exp / role_info.max_exp * 100)

    --经验值
    widget = Util:WidgetLabel(
                                self:GetWidget("Text_ExpProg"), 
                                string.format("%d/%d", role_info.exp, role_info.max_exp),
                                Macros.TypefaceSize.slightly, 
                                Macros.Color.lilyWhite
                            ) 
    widget:SetZOrder(1)

    --战力
    widget = self:GetWidget("Panel_Power")
    local battle_powerSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "battle_power", true)
    local battle_power = Util:Sprite(battle_powerSp)
    battle_power:SetAnchorPoint(0,0.5)
    battle_power:SetPosition(0,widget:GetContentHeight()/2)
    widget:AddChild(battle_power)

    local power_font = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "power_num", true))
    power_font:SetText(tostring(role_info.battle_value))
    power_font:SetAnchorPoint(0, 0.5)
    widget:AddChild(power_font:GetNode())
    PosTool.RightTo( battle_power , power_font , 5 )

    --VIP
    widget = self:GetWidget("Panel_VIP")
    local vip_font = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "vipnum", true))
    vip_font:SetText(tostring(role_info.vip_level))
    vip_font:SetAnchorPoint(0 , 0.5)
    vip_font:SetPosition( widget:GetContentWidth() + 10 , widget:GetContentHeight()/2 )
    widget:AddChild(vip_font:GetNode())

    --改名
    self.Button_rename = Util:WidgetButton( 
                                            self:GetWidget("Button_rename"),
                                            "",
                                            function()
                                                LogicGUI.TemplRoleRename.New()
                                            end
                                            )
    --审核状态下，屏蔽改名按钮
    self.Button_rename:SetVisible(not CPPSdkTools.IsInExamine())    

    --体力恢复时间
    local Text_muscleTime = self:GetWidget("Text_muscleTime")
    local role_info_muscleTime = Util:Label( 
                                                CPPGameLib.GetString("role_info_muscleTime") , 
                                                Macros.TypefaceSize.normal , 
                                                Macros.Color.keypoint 
                                            )
    role_info_muscleTime:SetAnchorPoint(0,0.5)
    role_info_muscleTime:SetPosition( 0 , Text_muscleTime:GetContentHeight()/2 )
    Text_muscleTime:AddChild(role_info_muscleTime)

    local role_info_muscleTime2 = Util:Label( CPPGameLib.GetString("role_info_max") , Macros.TypefaceSize.normal , Macros.Color.front_color )
    PosTool.RightTo(role_info_muscleTime , role_info_muscleTime2 , 5)
    Text_muscleTime:AddChild(role_info_muscleTime2) 
    self.__prop_recovery_node_map[Macros.Game.RolePropType.STAMINA] = role_info_muscleTime2

    --精力恢复时间
    local Text_vigorTime = self:GetWidget("Text_vigorTime")
    local role_info_vigorTime = Util:Label( 
                                                CPPGameLib.GetString("role_info_vigorTime") , 
                                                Macros.TypefaceSize.normal , 
                                                Macros.Color.keypoint 
                                            )
    role_info_vigorTime:SetAnchorPoint(0,0.5)
    role_info_vigorTime:SetPosition( 0 , Text_vigorTime:GetContentHeight()/2 )
    Text_vigorTime:AddChild(role_info_vigorTime)

    local role_info_vigorTime2 = Util:Label( CPPGameLib.GetString("role_info_max") , Macros.TypefaceSize.normal , Macros.Color.front_color )
    PosTool.RightTo(role_info_vigorTime , role_info_vigorTime2 , 5)
    Text_vigorTime:AddChild(role_info_vigorTime2) 
    self.__prop_recovery_node_map[Macros.Game.RolePropType.ENERGY] = role_info_vigorTime2

    --讨伐令恢复时间
    local Text_killTime = self:GetWidget("Text_killTime")
    local role_info_killTime = Util:Label( 
                                                CPPGameLib.GetString("role_info_killTime") , 
                                                Macros.TypefaceSize.normal , 
                                                Macros.Color.keypoint 
                                        )
    role_info_killTime:SetAnchorPoint(0,0.5)
    role_info_killTime:SetPosition( 0 , Text_killTime:GetContentHeight()/2 )
    Text_killTime:AddChild(role_info_killTime)

    local role_info_killTime2 = Util:Label( CPPGameLib.GetString("role_info_max") , Macros.TypefaceSize.normal , Macros.Color.front_color )
    PosTool.RightTo(role_info_killTime , role_info_killTime2 , 5)
    Text_killTime:AddChild(role_info_killTime2)
    self.__prop_recovery_node_map[Macros.Game.RolePropType.WANTED] = role_info_killTime2
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.RoleInfoTabView:__Dispose()
end

--选项卡界面打开回调
function Modules.RoleInfoTabView:__OpenCallback()    
    --角色属性相关
    local widget_prop_map = 
    {
        [Macros.Game.RolePropType.GOLD] = "Text_Gold",
        [Macros.Game.RolePropType.DIAMOND] = "Text_Diamond",
        [Macros.Game.RolePropType.HERO_SOUL] = "Text_HeroSoul",
        [Macros.Game.RolePropType.GODSOUL] = "Text_Soul",
        [Macros.Game.RolePropType.ARENA_PRESTIGE] = "Text_ArenaPrestige",
        [Macros.Game.RolePropType.TOWER_PRESTIGE] = "Text_TowerPrestige",
        [Macros.Game.RolePropType.REBEL_VALUE] = "Text_RebelVal",
        [Macros.Game.RolePropType.STAMINA] = "Text_Stamina",
        [Macros.Game.RolePropType.ENERGY] = "Text_Energy",
        [Macros.Game.RolePropType.WANTED] = "Text_Wanted",
        [Macros.Game.RolePropType.TOTAL_FRIENDSHIP_POINT] = "Text_Friendship",
    }    

    --设置恢复信息
    local function SetRecoveryInfo(prop_type, prop_value, prop_value_max)
        local prop_recovery_node = self.__prop_recovery_node_map[prop_type]
        if not prop_recovery_node then
            return
        end

        if not prop_value or not prop_value_max then
            prop_value = self:CallModuleFunc("GetPropValue", prop_type)
            prop_value_max = self:CallModuleFunc("GetMaxPropValue", prop_type)
        end

        --属性已满
        if prop_value >= prop_value_max then
            prop_recovery_node:SetString( CPPGameLib.GetString("role_info_max") )
            prop_recovery_node:SetColor( unpack( Macros.Color.front_color ) )

        --恢复中
        else
            local prop_recovery_info = self:CallModuleFunc("GetRecoveryInfo", prop_type)
            local next_recover_time = prop_recovery_info and prop_recovery_info.next_recover_time or 0
            local recover_countdown_time = math.max(0, next_recover_time - CallModuleFunc(ModuleType.TIME_SYNC, "GetServerTime"))
            prop_recovery_node:SetString( TimeTool.TransTimeStamp("HH:MM:SS", recover_countdown_time) )
            prop_recovery_node:SetColor( unpack( Macros.Color.red_color ) )
        end
    end

    --设置属性数据
    local prop_title_widget, prop_val_widget
    local SetProperties = function(prop_type, ignore_adjust_pos)
        local prop_value = self:CallModuleFunc("GetPropValue", prop_type)
        local prop_value_max = self:CallModuleFunc("GetMaxPropValue", prop_type)
        local widget_name = widget_prop_map[prop_type]
        if not widget_name then
            return
        end

        --属性标题
        prop_title_widget = Util:WidgetLabel(
                            self:GetWidget(widget_name.."_Title"), 
                            CPPGameLib.GetString("role_info_"..Macros.Game.RolePropKey[prop_type]).."：", 
                            Macros.TypefaceSize.normal , 
                            Macros.Color.keypoint
                        )

        --属性值
        prop_val_widget = Util:WidgetLabel(
                                            self:GetWidget(widget_name), 
                                            "", Macros.TypefaceSize.normal, 
                                            Macros.Color.content
                                        )      

        if not ignore_adjust_pos then
            prop_val_widget:SetPosition( prop_title_widget:GetPositionX() + prop_title_widget:GetContentWidth() , prop_val_widget:GetPositionY() )
        end

        if not prop_value_max then
            prop_val_widget:SetText(tostring(prop_value))
        else
            prop_val_widget:SetText(string.format("%d/%d", prop_value, prop_value_max))
        end

        --设置恢复信息
        SetRecoveryInfo(prop_type, prop_value, prop_value_max)
    end

    --设置属性显示
    for prop_type, _ in pairs(widget_prop_map) do
        SetProperties(prop_type)
    end    

    --设置货币数据
    local img_widget
    for _, currency_type in ipairs(Macros.Game.CurrencyTypeList) do
        img_widget = self:GetWidget("Image_Money"..currency_type, false)
        if img_widget then
            img_widget:SetImage(Util:GetMoneyIconImgData(currency_type))
        end
    end

    --角色属性变化
    self:BindGlobalEvent(Macros.Event.RoleModule.PROP_CHANGE, function(_, prop_type, old_value, new_value)
        --更新属性显示
        SetProperties(prop_type, true)
    end)

    --角色属性恢复倒计时
    self:BindGlobalEvent(Macros.Event.RoleModule.PROP_RECOVERY_COUNTDOWN, function(_, prop_type, recover_countdown_time)
        local prop_recovery_node = self.__prop_recovery_node_map[prop_type]
        if not prop_recovery_node then
            return
        end

        --显示倒计时
        prop_recovery_node:SetString( TimeTool.TransTimeStamp("HH:MM:SS", recover_countdown_time) )
        prop_recovery_node:SetColor( unpack( Macros.Color.red_color ) ) 
    end)

    --头像更新
    self:BindGlobalEvent(Macros.Event.RoleModule.HEAD_CHANGE, function(_, data)
        local role_info = self:CallModuleFunc("GetRoleInfo")
        self.role_icon:SetData(role_info.head_icon)
    end)

    --角色名更新
    self:BindGlobalEvent(Macros.Event.RoleModule.ROLE_NAME_CHANGED, function(_, data)
        local role_info = self:CallModuleFunc("GetRoleInfo")
        self:GetWidget("Text_RoleName"):SetText( role_info.role_name )
    end)
end

--选项卡界面关闭回调
function Modules.RoleInfoTabView:__CloseCallback()
end

