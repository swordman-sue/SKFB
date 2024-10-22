
--[[
%% @module: 装备强化界面
%% @author: yjg
%% @created: 2016年11月5日16:58:56
--]]

Modules = Modules or {}

Modules.EquipLiftIntensifyTabView = Modules.EquipLiftIntensifyTabView or BaseClass(BaseTabView)

function Modules.EquipLiftIntensifyTabView:__init()
end

--选项卡界面创建完成回调
function Modules.EquipLiftIntensifyTabView:__LoadedCallback()
    self:__Getwidget()
    self:__ShowUI()
end

--选项卡界面打开回调
function Modules.EquipLiftIntensifyTabView:__OpenCallback()
    -- --调用普通
    self:__SetCommonUI()

    --强化装备
    self:BindGlobalEvent(Macros.Event.EquipModule.EQUIP_INTENSIFY,function (_, data)
        self:__UpgradeProperty( data )
    end)
end

--选项卡界面关闭回调
function Modules.EquipLiftIntensifyTabView:__CloseCallback()
    if self.effect then
        self:__DelEffect(self.effect_id )
    end
    if self.windwords_effect then
        self.windwords_effect:DeleteMe()
        self.windwords_effect = nil 
    end
    if self.faceEffects_1 then
        self.faceEffects_1:DeleteMe()
        self.faceEffects_1 = nil
    end
    if self.faceEffects_2 then
        self.faceEffects_2:DeleteMe()
        self.faceEffects_2 = nil
    end
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.EquipLiftIntensifyTabView:__Dispose()
    if self.faceEffects_1 then
        self.faceEffects_1:DeleteMe()
        self.faceEffects_1 = nil
    end
    if self.faceEffects_2 then
        self.faceEffects_2:DeleteMe()
        self.faceEffects_2 = nil
    end
end

--获得所有组件
function Modules.EquipLiftIntensifyTabView:__Getwidget()
    --信息面板
    self.Image_infoBg = self:GetWidget("Image_infoBg") 
    --强化五次
    self.Button_intensify5 = Util:WidgetButton( 
                                                self:GetWidget("Button_intensify5") , 
                                                CPPGameLib.GetString("equip_intensify5") , 
                                                CPPGameLib.Handler(self,self.OnButtonIntensify5) ,
                                                nil,nil, Macros.Color.button_red
                                                )
    --强化
    self.Button_intensify = Util:WidgetButton( 
                                                self:GetWidget("Button_intensify") , 
                                                CPPGameLib.GetString("equip_intensify") , 
                                                CPPGameLib.Handler(self,self.OnButtonIntensify),
                                                nil,nil, Macros.Color.button_yellow
                                                )
    --箭头
    self.Image_arrows = self:GetWidget("Image_arrows") 
    --当前
    self.Text_nowLv = self:GetWidget("Text_nowLv") 
    --当前等级
    self.Text_current_0 = self:GetWidget("Text_current_0") 

    --下级
    self.Text_nextLv = self:GetWidget("Text_nextLv") 
    --下级等级
    self.Text_junior_0 = self:GetWidget("Text_junior_0") 

    --当前属性
    self.Text_nowType = self:GetWidget("Text_nowType") 
    --当前属性值
    -- self.Text_nowTypeValue = self:GetWidget("Text_nowTypeValue") 
    --下级属性
    self.Text_nextType = self:GetWidget("Text_nextType") 
    --下级属性值
    -- self.Text_nextTypeValue = self:GetWidget("Text_nextTypeValue") 

    --天赋
    self.Image_talent = self:GetWidget("Image_9_1_0") 
    self.Image_talent:SetVisible(false)
    self.Text_talent = self:GetWidget("Text_talent") 

    --价钱                                                                                           
    self.Image_price = self:GetWidget("Image_price") 
    --图样背景
    self.Image_packBg = self:GetWidget("Image_packBg") 
    --名字
    self.Image_nameBg = self:GetWidget("Image_nameBg")
    self.Text_name = self:GetWidget("Text_name") 
    --图标
    self.Panel_icon = self:GetWidget("Panel_icon")
    --飘字容器
    self.Panel_upgradeEffect = self:GetWidget("Panel_upgradeEffect")
end

function Modules.EquipLiftIntensifyTabView:__ShowUI()

    local equip_nowProperty = Util:Label( CPPGameLib.GetString("equip_nowProperty") , nil , Macros.Color.headline )
    self.Text_current_0:AddChild(equip_nowProperty)
    PosTool.Center(equip_nowProperty)

    local equip_nexProperty = Util:Label( CPPGameLib.GetString("equip_nexProperty") , nil , Macros.Color.headline )
    self.Text_junior_0:AddChild(equip_nexProperty)
    PosTool.Center(equip_nexProperty)

    local equip_talent = Util:Label( CPPGameLib.GetString("equip_talent") , nil , Macros.Color.headline )
    self.Text_talent:AddChild(equip_talent)
    PosTool.Center(equip_talent)

    -- 图标
    local path = Resource.PathTool.GetBigIconPath(20101)
    self.icon = Util:Sprite(path)
    self.icon:SetPosition(self.Panel_icon:GetContentWidth()/2 ,self.Panel_icon:GetContentHeight()/2 - 60  )
    self.icon:SetAnchorPoint(0.5, 0.5 )
    self.Panel_icon:AddChild(self.icon)
    self.icon:SetScaleFactor( 0.8 )
    ActionManager.GetInstance():UpDownMoveAnimation(self.icon , 1 , 15)


    self.name = Util:Name( "  " , Macros.TypefaceSize.largish)
    self.name:SetPosition( self.Text_name:GetContentWidth()/2 , self.Text_name:GetContentHeight()/2 )
    self.name:SetAnchorPoint(0.5,0.5)
    self.Text_name:AddChild(self.name)

    -- 品级
    self.taste = Util:RichText( CPPGameLib.GetString("equip_grade_1" , 0) , Macros.TypefaceSize.largish )
    self.taste:SetAnchorPoint( 0.5 , 1 )
    self.taste:SetPosition( self.Text_name:GetContentWidth()/2 , -20 )
    self.Text_name:AddChild(self.taste)

    --强化等级
    local equip_intensifyLv = Util:Label( CPPGameLib.GetString("equip_Lv").." " , nil , Macros.Color.keypoint )
    equip_intensifyLv:SetAnchorPoint(1,0.5)
    equip_intensifyLv:SetPosition(  0 , self.Text_nowLv:GetContentHeight()/2 )
    self.Text_nowLv:AddChild(equip_intensifyLv)

    --强化等级
    self.intensifyLv = Util:Label(" " , nil , Macros.Color.content )
    -- self.intensifyLv:SetPosition( self.Text_nowLv:GetPositionX() + self.Text_nowLv:GetContentWidth() + 10   , self.Text_nowLv:GetContentHeight()/2 )
    PosTool.RightTo( equip_intensifyLv , self.intensifyLv , -35 )
    self.Text_nowLv:AddChild(self.intensifyLv)

    --强化属性
    self.intensifyProperty = Util:Label(" " , nil , Macros.Color.keypoint )
    self.intensifyProperty:SetAnchorPoint(1,0.5)
    self.intensifyProperty:SetPosition( 0 , self.Text_nowType:GetContentHeight()/2 )
    self.Text_nowType:AddChild(self.intensifyProperty)

    --强化属性值
    self.intensifyPropertyValue = Util:Label(" " , nil , Macros.Color.content )
    PosTool.RightTo( self.intensifyProperty , self.intensifyPropertyValue , -35 )
    self.Text_nowType:AddChild(self.intensifyPropertyValue)

    --下一等级
    local equip_junior = Util:Label( CPPGameLib.GetString("equip_Lv").." " , nil , Macros.Color.keypoint  )
    equip_junior:SetAnchorPoint(1,0.5)
    equip_junior:SetPosition( 0 , self.Text_nextLv:GetContentHeight()/2 )
    self.Text_nextLv:AddChild(equip_junior)

    -- 下一等级
    self.junior = Util:Label(" " , nil , Macros.Color.front_color )
    PosTool.RightTo( equip_junior , self.junior , -35 )
    self.Text_nextLv:AddChild(self.junior)

    --下级属性
    self.juniorProperty = Util:Label(" " , nil , Macros.Color.keypoint )
    self.juniorProperty:SetAnchorPoint(1,0.5)
    self.juniorProperty:SetPosition( 0 , self.Text_nextType:GetContentHeight()/2 )
    self.Text_nextType:AddChild(self.juniorProperty)

    --下级属性
    self.juniorPropertyValue = Util:Label(" ", nil , Macros.Color.front_color)
    PosTool.RightTo( self.juniorProperty , self.juniorPropertyValue , -35 )
    self.Text_nextType:AddChild(self.juniorPropertyValue)

    self.talent = Util:Label( " " , nil , Macros.Color.keypoint )
    self.talent:SetAnchorPoint( 0.5 , 0.5)
    self.talent:SetPosition( self.Text_talent:GetContentWidth()/2 , self.Text_talent:GetContentHeight()/2 - 50 )
    self.Text_talent:AddChild(self.talent)

    --升星
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_up_star",true)
    self.btn_up_star = Util:Button( sp , function( ... )
        CallModuleFunc(ModuleType.EQUIP, "OpenView", BaseViewType.EQUIP_UP_STAR ) 
    end )
    self.btn_up_star:SetPosition( self.Image_nameBg:GetPositionX() + self.Image_nameBg:GetContentWidth()/2 , self.Image_nameBg:GetPositionY() )
    self.btn_up_star:SetAnchorPoint( 0 , 0.5 )
    self.btn_up_star:SetVisible( false )
    self.Image_packBg:AddChild(self.btn_up_star) 
    self.btn_up_star:SetScaleFactor( 0.8 )
end

function Modules.EquipLiftIntensifyTabView:__SetShowUI()
    if self.windwords_effect then
        self.windwords_effect:DeleteMe()
        self.windwords_effect = nil 
    end

    if self.effect and self.effect.id then
        for i = 1 , #self.effect.id do
            if self.effect.id[i] then
                if self.effect.id[i] then
                    self:__DelEffect(self.effect.id[i])
                    self.effect.id[i] = nil
                    self.effect.con[i] = nil
                end 
            end
        end
    end

    self:__SetCommonUI()
end

--不需要动的UI
function Modules.EquipLiftIntensifyTabView:__SetCommonUI(  )
    
    local equipInfo = CallModuleFunc(ModuleType.EQUIP, "GetOnEquipInfo")

    local info = CallModuleFunc(ModuleType.EQUIP, "ToUidGetEquipUpStarInfo", equipInfo.uid )

    local starUpIsOpen = CallModuleFunc(ModuleType.EQUIP, "StarUpIsOpen")
    if starUpIsOpen then
        if info.nexInfo and info.nexInfo.name then
            self.btn_up_star:SetVisible( true )
        else
            self.btn_up_star:SetVisible( false )
        end
    end

    --获得选中的道具的信息
    self.onEquipInfo = CallModuleFunc(ModuleType.EQUIP, "GetOnEquipInfo")
    --选中的装备的两种上限
    local liftMaxLst = CallModuleFunc(ModuleType.EQUIP, "GetLiftMaxInfo" , self.onEquipInfo.quality )
    --将两种上限加进列表
    self.onEquipInfo.strengthenMax = liftMaxLst.strengthenMax
    self.onEquipInfo.refineMax = liftMaxLst.refineMax

    --名字
    local refine_level = ""
    if self.onEquipInfo.refine_level ~= 0 then
        refine_level = " +"..self.onEquipInfo.refine_level
    end
    self.name:SetString( self.onEquipInfo.name .. refine_level )
    self.name:SetColor(unpack(Macros.Quality[self.onEquipInfo.quality]))
    self.name:SetStrokeColor(unpack(Macros.Quality_Stroke[self.onEquipInfo.quality]))

    self.taste:SetText( CPPGameLib.GetString("equip_grade_1" , self.onEquipInfo.taste) )
    
    -- 设置图标
    local equipinfo = CPPGameLib.GetConfig("Equipment", self.onEquipInfo.info_id )
    self.icon:SetImage(Resource.PathTool.GetBigIconPath( equipinfo.res_id ))

    --背光
    if self.iconId then
        self:__DelEffect(self.iconId )
    end
    local iconLed = nil
    self.iconId , iconLed = TypefaceEffects.New():Spotlight( self , self.Panel_icon , self.onEquipInfo.quality )
    iconLed:SetPosition( self.Panel_icon:GetContentWidth()/2 , -215 )
    iconLed:SetScaleFactor(0.8)

    --强化属性
    local getIntensify , onceIntensify , nextIntensify = CallModuleFunc(ModuleType.EQUIP, "GetStrengthenPropPlusMap" , self.onEquipInfo.uid )
    --当前等级
    local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
    local maxLv = userDate.level * 2
    if getIntensify.level == nextIntensify.level then
        maxLv = getIntensify.level
    end

    local nowType , nowValue = Util:Nature( getIntensify.lst[1].type , getIntensify.lst[1].value , true )
    --当前等级
    self.intensifyLv:SetString( getIntensify.level .. " / " .. maxLv )
    --当前属性
    self.intensifyProperty:SetString(nowType.." ")
    --当前属性值
    self.intensifyPropertyValue:SetString(" "..nowValue)

    self.intensifyProperty:SetPosition( 0 , self.Text_nowType:GetContentHeight()/2 )
    PosTool.RightTo( self.intensifyProperty , self.intensifyPropertyValue , -35 )

    local nextType , nextValue = Util:Nature( nextIntensify.lst[1].type , nextIntensify.lst[1].value , true )
    --下一等级
    self.junior:SetString( nextIntensify.level  .. " / " .. maxLv )
    --下一属性
    self.juniorProperty:SetString(nextType.." ")
    --下一属性值
    self.juniorPropertyValue:SetString(" "..nextValue)
    
    self.juniorProperty:SetPosition( 0 , self.Text_nextType:GetContentHeight()/2 )
    PosTool.RightTo( self.juniorProperty , self.juniorPropertyValue , -35 )

-- 显示当前装备强化等级下一个可以解锁的天赋的信息
-- 如果没有装备强化天赋则不显示
    local _ , __ , ___ , talent = CallModuleFunc(ModuleType.EQUIP, "GetStrengthenPropPlusMap" , self.onEquipInfo.uid )

   if talent.talent and talent.nexLst[1] then
        self.talent:SetString( CPPGameLib.GetString("equip_talent_tips" , talent.nexLst[1].name , talent.nexLst[1].nature_describe , talent.nexLst[1].intensify_level ) )
        self.Image_talent:SetVisible(true)
    else
        self.Image_talent:SetVisible(false)
    end
        
    self:SetPrice()
end

function Modules.EquipLiftIntensifyTabView:SetPrice()
    --价钱
    if self.costBg then
        self.costBg.icon:RemoveSelf(true)
        self.costBg.icon = nil

        self.costBg.gold:RemoveSelf(true)
        self.costBg.gold = nil
    end

    local goldtype ,gold ,type_ , num = self:JudgeHave()
    self.costBg = {}
    self.costBg.icon = Util:MoneyControl( goldtype , 0.8 )
    self.costBg.icon:SetPosition(5 ,self.Image_price:GetContentHeight()/2 - 2)
    self.costBg.icon:SetAnchorPoint(0,0.5)
    self.Image_price:AddChild(self.costBg.icon)

    self.costBg.gold = Util:Label( gold )
    self.costBg.gold:SetAnchorPoint( 1 , 0.5)
    self.costBg.gold:SetPosition( self.Image_price:GetContentWidth() - 5 , self.Image_price:GetContentHeight()/2 )
    self.Image_price:AddChild( self.costBg.gold )
end

--升级属性
function Modules.EquipLiftIntensifyTabView:__UpgradeProperty( data )
    self:Effect( data )

    LogicGUI.TempMonster.DisposeData( data , "equipStrengthen" )

    self.Button_intensify:SetTouchEnabled( true )
    self.Button_intensify5:SetTouchEnabled( true )

    --获得选中的道具的信息
    self.onEquipInfo = CallModuleFunc(ModuleType.EQUIP, "GetOnEquipInfo")
    --选中的装备的两种上限
    local liftMaxLst = CallModuleFunc(ModuleType.EQUIP, "GetLiftMaxInfo" , self.onEquipInfo.quality )
    --将两种上限加进列表
    self.onEquipInfo.strengthenMax = liftMaxLst.strengthenMax
    self.onEquipInfo.refineMax = liftMaxLst.refineMax

    --强化属性
    local getIntensify , onceIntensify , nextIntensify , talent = CallModuleFunc(ModuleType.EQUIP, "GetStrengthenPropPlusMap" , self.onEquipInfo.uid )
    local nowType , nowValue = Util:Nature( getIntensify.lst[1].type , getIntensify.lst[1].value , true )
    local nextType , nextValue = Util:Nature( nextIntensify.lst[1].type , nextIntensify.lst[1].value , true)

    local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
    --特效
    local pos = {
        [1] = { x = self.Text_nowLv:GetPositionX() , y = self.Text_nowLv:GetPositionY() },
        [2] = { x = self.Text_nowType:GetPositionX() , y = self.Text_nowType:GetPositionY() }
    }

    local t1 = LogicGUI.EquipmentIntensifyFontEffects:GetEquipmentIntensifyTab( data , pos )

    CPPGameLib.PrintTable("xy" , t1 , "LogicGUI.EquipmentIntensifyFontEffects:GetEquipmentIntensifyTab--------------")  

    self.windwords_effect = WindWordsEffects.New()
    self.windwords_effect:Subsection( self.__root_widget , self.Image_infoBg , t1 , 10 ,function ( ... )
        if not self.faceEffects_1 then
            self.faceEffects_1 = TypefaceEffects.New()
        end
        self.faceEffects_1:Roll( self.intensifyPropertyValue, nowValue , nil , function ( ... )
        
        end)

        local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
        local maxLv = userDate.level * 2
        if getIntensify.level == nextIntensify.level then
            maxLv = getIntensify.level
        end
        if not self.faceEffects_2 then
            self.faceEffects_2 = TypefaceEffects.New()
        end
        self.intensifyLv:SetString(onceIntensify.level)
        self.faceEffects_2:Roll( self.intensifyLv , getIntensify.level ,  "/" .. maxLv , function ( ... )
       
        end)

        --下一等级
        self.junior:SetString( nextIntensify.level  .. "/" .. maxLv )
        --下一属性
        self.juniorProperty:SetString(nextType.." ")
        --下一属性值
        self.juniorPropertyValue:SetString(nextValue)

        if talent.talent then
            self.talent:SetString( CPPGameLib.GetString("equip_talent_tips" , talent.nexLst[1].name , talent.nexLst[1].nature_describe , talent.nexLst[1].intensify_level ) )
            self.Image_talent:SetVisible(true)
        else
            self.Image_talent:SetVisible(false)
        end

        --如果强化满了
        -- local data = CPPGameLib.GetConfig("Equipment", self.onEquipInfo.info_id)
        if self.onEquipInfo.strengthen_level >= self.onEquipInfo.strengthenMax then
            self.Button_intensify5:SetVisible(false)
            self.Button_intensify:SetVisible(false)
        end

        self:SetPrice()
    end )
end

--强化
function Modules.EquipLiftIntensifyTabView:OnButtonIntensify()
    local goldtype ,gold ,type_ , num = self:JudgeHave()
    local lv = self:JudgeLevel()
    if lv then
        GlobalTipMsg.GetInstance():Show(lv)
    elseif type_ == false then
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("common_gold_not_enough"))
        CallModuleFunc(ModuleType.SUNDRY, "ToIdGetPath", Macros.Game.RolePropType.GOLD )
    else
        self.Button_intensify:SetTouchEnabled( false )

        local data = {}
        data.type = 1               --次数
        data.id = self.onEquipInfo.uid     --uid
        CallModuleFunc(ModuleType.EQUIP, "EquipMentstrengReq",data)
    end
end

--强化5
function Modules.EquipLiftIntensifyTabView:OnButtonIntensify5()
    local goldtype ,gold ,type_ , num = self:JudgeHave()
    local lv = self:JudgeLevel()
    if type_ == false then
        GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("common_gold_not_enough"))
        CallModuleFunc(ModuleType.SUNDRY, "ToIdGetPath", Macros.Game.RolePropType.GOLD )
    elseif lv then
        GlobalTipMsg.GetInstance():Show(lv)
    else

        self.Button_intensify5:SetTouchEnabled( false )

        local data = {}
        data.type = num               --次数
        data.id = self.onEquipInfo.uid     --uid
        CallModuleFunc(ModuleType.EQUIP, "EquipMentstrengReq",data)
    end
end

function Modules.EquipLiftIntensifyTabView:JudgeHave()
    local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
    local equip = CPPGameLib.GetConfig("Equipment", self.onEquipInfo.info_id)
    local strenConfig = CPPGameLib.GetConfig("EquipmentStrenthen", equip.quality)
    local info = strenConfig.level_list[self.onEquipInfo.strengthen_level]
    if info and info.cost_value_list[1] > userDate.gold then
        return info.cost_type_list[1] ,info.cost_value_list[1] ,false , 0  
    else
        local num = math.floor(userDate.gold / info.cost_value_list[1])
        if num > 5  then
            num = 5
        end
        return info.cost_type_list[1] ,info.cost_value_list[1] ,true , num
    end
end

function Modules.EquipLiftIntensifyTabView:JudgeLevel( ... )
    local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
    if self.onEquipInfo.strengthen_level >= userDate.level * 2 then
        return CPPGameLib.GetString("equip_uplvmax")
    end
end

function Modules.EquipLiftIntensifyTabView:Effect( data )
	-- self.effect = {}
	-- self.effect.id = {}
	-- self.effect.con ={}
	-- local func = nil
	-- local index = 1
 --    local function moveAnimate( ... )
 --    	if index <= data.type then
	--     	index = index + 1
	-- 		self.effect.id[index] , self.effect.con[index] = self:__CreateEffect({id = 100025 , loop = false }, self.Panel_icon)
	-- 		self.effect.con[index]:SetPosition( self.icon:GetPositionX(), self.icon:GetPositionY() )
	-- 		CPPActionManager.DelayTo(self.icon , 0.2 , func)
	-- 	end
 --    end	    
 --    func = GlobalCallbackMgr:AddCallBackFunc(moveAnimate)
 --    moveAnimate()
    -- local effect_id,effect = self:__CreateEffect({id = 100025,loop = false},self.Panel_upgradeEffect, 2)
    -- effect:SetAnchorPoint(0.5,0)
    -- PosTool.Center(effect,0,20)
end