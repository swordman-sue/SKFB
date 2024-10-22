
--
-- @brief 怪物难度选择
-- @author: yjg
-- @date: 2016年9月27日20:18:25
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplTowerDifficultySelect = LogicGUI.TemplTowerDifficultySelect or BaseClass(GUI.Template)


function LogicGUI.TemplTowerDifficultySelect:__init(data)
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self.__open_callback = function()
        self.data = data
        self:__Getwidget()
        self:__ShowUI()
    end

    self:InitTemplate(GUI.TemplateRegistry.TOWER_DIFFICULTY_SELECT) 
end

function LogicGUI.TemplTowerDifficultySelect:__delete()
    if self.__shield_layer then
        self.__shield_layer:DeleteMe()
        self.__shield_layer = nil
    end
end

function LogicGUI.TemplTowerDifficultySelect:SetData(data)
end

function LogicGUI.TemplTowerDifficultySelect:__Getwidget( ... )
    --居中显示
    self.Image_center = self:GetWidget("Image_center") 
    --光
    -- self.Image_light = self:GetWidget("Image_light") 
    --关闭
    self.Button_close = Util:WidgetButton(self:GetWidget("Button_close"), nil , CPPGameLib.Handler( self,self.OnBtnExit ) )  
    --关卡名字
    self.Text_level = self:GetWidget("Text_level") 
    --通关条件
    self.Text_condition = self:GetWidget("Text_condition") 
    --背景层
    self.Image_content = self:GetWidget("Image_content") 

    --难度背景
    self.Image_difficulty = {}
    --挑战按钮
    self.Button_select = {}
    --可获得
    self.Text_get = {}
    --金币
    self.Text_gold = {}
    --威名
    self.Text_prestige = {}
    --战斗力
    self.Image_war = {}

    for i=1,3 do
        self.Image_difficulty[i] = self:GetWidget( "Image_difficulty" .. i )
        self.Button_select[i] = Util:WidgetButton(self:GetWidget("Button_select"..i), CPPGameLib.GetString("tower_challenge") , function ( ... )        
            local checkBattle = CallModuleFunc(ModuleType.TOWER , "CheckBattleCondition" , Macros.Game.FBType.TOWER_COMMON )
            if checkBattle then
                CallModuleFunc(ModuleType.BATTLE, "BattleStartReq", Macros.Game.FBType.TOWER_COMMON , i - 1)
            end
        end,nil,nil, Macros.Color.button_yellow )
        self.Text_get[i] = self:GetWidget( "Text_get" .. i )
        self.Text_gold[i] = self:GetWidget( "Text_gold" .. i )
        self.Text_prestige[i] = self:GetWidget( "Text_prestige" .. i )
        self.Image_war[i] = self:GetWidget( "Image_war" .. i )
    end
end

function LogicGUI.TemplTowerDifficultySelect:__ShowUI( ... )    


    --名字
    local name = Util:LabCenter( {
                                    {zi = CPPGameLib.GetString("tower_screening",self.data.game_level) ,dx = Macros.TypefaceSize.popup , ys = Macros.Color.headline },
                                    {zi = self.data.name , jg = 2 , dx = Macros.TypefaceSize.popup , ys = Macros.Color.headline },
                            } )
    name:SetPosition(self.Text_level:GetContentWidth()/2 , self.Text_level:GetContentHeight()/2)
    name:SetAnchorPoint(0.5,0.5)
    self.Text_level :AddChild(name)

     --线
    local shortlineSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "shortline")

    --左边
    local shortline_1 = Util:Sprite9( shortlineSp , {50,10} )
    shortline_1:SetContentSize(shortline_1:GetContentWidth() , shortline_1:GetContentHeight())
    shortline_1:SetAnchorPoint( 1 , 0.5 )
    shortline_1:SetPosition( name:GetPositionX() - (name:GetContentWidth()/2) - 10 , self.Text_level:GetContentHeight()/2 )
    self.Text_level:AddChild(shortline_1)

    --左边
    local shortline_2 = Util:Sprite9( shortlineSp , {50,10})
    shortline_2:SetContentSize(shortline_2:GetContentWidth() , shortline_2:GetContentHeight())
    shortline_2:SetAnchorPoint( 1 , 0.5 )
    shortline_2:SetFlipX(true)
    shortline_2:SetPosition( name:GetPositionX() + (name:GetContentWidth()/2) + 10 , self.Text_level:GetContentHeight()/2 )
    self.Text_level:AddChild(shortline_2)

    --通关条件
    local str_condition = CPPGameLib.GetString("tower_cond_tips_4")
    local tower_game_level = CallModuleFunc(ModuleType.TOWER, "GetCurMapLevel")
    local config_tower_game_level = CPPGameLib.GetConfig("TowerGameLevel", tower_game_level + 1 )
    if config_tower_game_level and config_tower_game_level.win_condition then
        local c_type = config_tower_game_level.win_condition[1]
        local c_param = config_tower_game_level.win_condition[2]
        str_condition = CPPGameLib.GetString("tower_clearance_"..c_type, c_param)           
    end 

    --通关条件
    local clearance = Util:LabCenter( {
                                        {zi = CPPGameLib.GetString("tower_condition1") ,dx = Macros.TypefaceSize.normal , ys = Macros.Color.keypoint },
                                        {zi = str_condition , jg = 2 , dx = Macros.TypefaceSize.normal , ys = Macros.Color.front_color },
                                    } )
    clearance:SetPosition(self.Text_condition:GetContentWidth()/2 , self.Text_condition:GetContentHeight()/2)
    clearance:SetAnchorPoint(0.5,0.5)
    self.Text_condition :AddChild(clearance)

    --星
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "star")

    for i=1,3 do
        --可得
        local tower_kede = Util:Label( CPPGameLib.GetString("tower_kede") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
        tower_kede:SetAnchorPoint(0,0.5)
        tower_kede:SetPosition( 0 , self.Text_get[i]:GetContentHeight()/2 )
        self.Text_get[i]:AddChild(tower_kede)

        local little_star = Util:LabCenter( {
                                            {tp = sp , dx = 1 , jg = 10},
                                            {zi = self.data.award_star_list[i] , jg = 2 , dx = Macros.TypefaceSize.normal , ys = Macros.Color.content },
                                        } )
        little_star:SetAnchorPoint(0,0.5)
        little_star:SetPosition( tower_kede:GetContentWidth() - 20 , self.Text_get[i]:GetContentHeight()/2)
        self.Text_get[i] :AddChild(little_star)
 
        --金币
        local lib = CPPGameLib.GetConfig("GameLevel",self.data.map_list[i])

        local tower_gold = Util:Label( CPPGameLib.GetString("tower_gold") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
        tower_gold:SetAnchorPoint(0,0.5)
        tower_gold:SetPosition( 0 , self.Text_gold[i]:GetContentHeight()/2 )
        self.Text_gold[i]:AddChild(tower_gold)

        local gold = Util:LabCenter({
                                {tp =  Util:GetMoneyIconImgData(lib.reward_currency_type_list[1] or Macros.Game.RolePropType.GOLD) , dx = 0.65 , jg = 10},
                                {zi = lib.reward_currency_value_list[1] or lib.gold , jg = 2 , dx = Macros.TypefaceSize.normal , ys = Macros.Color.content },
                            })
        gold:SetPosition( tower_gold:GetContentWidth() - 20 , self.Text_gold[i]:GetContentHeight()/2)
        gold:SetAnchorPoint( 0 , 0.5)
        self.Text_gold[i]:AddChild(gold)     

        --威名
        local tower_prestige = Util:Label( CPPGameLib.GetString("tower_prestige") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
        tower_prestige:SetAnchorPoint(0,0.5)
        tower_prestige:SetPosition( 0 , self.Text_prestige[i]:GetContentHeight()/2 )
        self.Text_prestige[i]:AddChild(tower_prestige)

        local diamond = Util:LabCenter({
                                {tp = Util:GetMoneyIconImgData(lib.reward_currency_type_list[2]  or Macros.Game.RolePropType.TOWER_PRESTIGE) , dx = 0.65 , jg = 10},
                                {zi = lib.reward_currency_value_list[2] or lib.context , jg = 2 , dx = Macros.TypefaceSize.normal , ys = Macros.Color.content },
                            })
        diamond:SetPosition( tower_prestige:GetContentWidth() - 20 , self.Text_prestige[i]:GetContentHeight()/2)
        diamond:SetAnchorPoint( 0 , 0.5)
        self.Text_prestige[i]:AddChild(diamond) 

        -- --战斗力Util:Label( lib.recommended_power , Macros.TypefaceSize.normal , Macros.Color.keypoint , nil , nil , Macros.Color.common_stroke )
        local recommended_power = GUI.ImgFont.New( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "power_num", true) )
        recommended_power:SetAnchorPoint(0,0.5)
        recommended_power:SetPosition( self.Image_war[i]:GetContentWidth() + self.Image_war[i]:GetPositionX() + 10 , self.Image_war[i]:GetPositionY() )
        self.Image_difficulty[i]:AddChild(recommended_power:GetNode())

        self.myriad = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "myriad", true) )
        self.Image_difficulty[i]:AddChild(self.myriad)
        self.myriad:SetVisible(false)

        local battle_value = lib.recommended_power
        if lib.recommended_power >= 1000000 then
            battle_value = math.floor( lib.recommended_power/10000 )
            self.myriad:SetVisible(true)
        end

        recommended_power:SetText(tostring(battle_value))

        PosTool.RightTo( recommended_power , self.myriad )
    end
end

--关闭
function LogicGUI.TemplTowerDifficultySelect:OnBtnExit( ... )
    self:DeleteMe()
end