
--[[
%% @module: 卡牌信息界面
%% @author: yjg
%% @created: 2017年11月14日16:34:39
--]]

Modules = Modules or {}

Modules.TemplDivinationMinuteInfo = Modules.TemplDivinationMinuteInfo or BaseClass(BaseTabView)

function Modules.TemplDivinationMinuteInfo:__init()
end

--选项卡界面创建完成回调
function Modules.TemplDivinationMinuteInfo:__LoadedCallback()	
	self:__Getwidget()
	self:__ShowUI()
end

--选项卡界面打开回调
function Modules.TemplDivinationMinuteInfo:__OpenCallback()
	self:__SetShowUI()
end

--选项卡界面关闭回调
function Modules.TemplDivinationMinuteInfo:__CloseCallback()
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.TemplDivinationMinuteInfo:__Dispose()
end

--获得所有组件
function Modules.TemplDivinationMinuteInfo:__Getwidget()
	--信息背景
	self.Image_infoBg = self:GetWidget("Image_infoBg")

	--名字背景
	self.Image_nameBg = self:GetWidget("Image_nameBg")
	self.Image_nameBg:SetVisible(false)
	--名字
	-- self.Text_name = self:GetWidget("Text_name")

	--图标
	self.Panel_icon = self:GetWidget("Panel_icon")
	--属性背景
	self.Image_property_bg = self:GetWidget("Image_property_bg")

	--类型1
	self.Image_type1 = self:GetWidget("Image_type1")
	--类型2
	self.Image_type2 = self:GetWidget("Image_type2")

	--属性
	self.Panel_property_0 = self:GetWidget("Panel_property_0")
	--上
	self.Image_up = self:GetWidget("Image_up")
	--下
	self.Image_below = self:GetWidget("Image_below")

	--替换
	self.Button_change = self:GetWidget("Button_change")
    self.Button_change = Util:WidgetButton( 
                                            self:GetWidget("Button_change") , 
                                            CPPGameLib.GetString("equip_btn_change") , 
                                            CPPGameLib.Handler(self,self.OnButtonChange) ,
                                            nil,nil, Macros.Color.button_red
                                            )
    self.Button_change:SetVisible(false)

	--卸下
	self.Button_discharge = self:GetWidget("Button_discharge")
    self.Button_discharge = Util:WidgetButton( 
                                            self:GetWidget("Button_discharge") , 
                                            CPPGameLib.GetString("equip_btn_discharge") , 
                                            CPPGameLib.Handler(self,self.OnButtonDischarge) ,
                                            nil,nil, Macros.Color.button_yellow
                                            )
    self.Button_discharge:SetVisible(false)
end

function Modules.TemplDivinationMinuteInfo:__ShowUI()

    local divinationUid = CallModuleFunc(ModuleType.DIVINATION, "GetOnDivinationUid")
	--图标
	self.icon = LogicGUI.IconLayer.New(false)
    self.icon:SetPosition(self.Panel_icon:GetContentWidth()/2 ,self.Panel_icon:GetContentHeight()/2)
    self.icon:SetAnchorPoint(0.5, 0.5 )
    self.icon:SetScaleFactor(1)
    self.Panel_icon:AddChild(self.icon:GetNode())

    --名字 
	self.name = Util:Name( "  " , Macros.TypefaceSize.largish )
	self.name:SetPosition( self.Image_nameBg:GetPositionX() , self.Image_nameBg:GetPositionY() )
	self.name:SetAnchorPoint( 0 , 0 )
	self.Image_infoBg:AddChild(self.name)

    -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    --强化属性
    -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    local equip_intensify = Util:Label( CPPGameLib.GetString("equip_intensifyProperty") , Macros.TypefaceSize.normal )
    equip_intensify:SetAnchorPoint( 0 , 0.5 )
   	equip_intensify:SetPosition( self.Image_type1:GetPositionX() + 10 , self.Image_type1:GetPositionY() + (self.Image_type1:GetContentHeight()/2) )
    self.Image_property_bg:AddChild(equip_intensify)
    
    --强化等级
    local equip_intensifyLv = Util:Label( CPPGameLib.GetString("equip_intensifyLv") , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
    equip_intensifyLv:SetAnchorPoint(1,1)

    equip_intensifyLv:SetPosition(80 + equip_intensify:GetPositionX() + ( 80/2 ) + 13 , equip_intensify:GetPositionY() - equip_intensify:GetContentHeight() )
    self.Image_property_bg:AddChild(equip_intensifyLv)

    self.intensifyLv = Util:Label(" " , Macros.TypefaceSize.slightly , Macros.Color.content )
	self.intensifyLv:SetAnchorPoint(0,1)
	self.intensifyLv:SetPosition( equip_intensifyLv:GetPositionX() , equip_intensifyLv:GetPositionY() )
    self.Image_property_bg:AddChild(self.intensifyLv)

    --强化属性
    self.intensifyProperty = {}
    self.intensifyProperty.type = {}
    self.intensifyProperty.value = {}

    local y = equip_intensifyLv:GetPositionY() - 25
    for i = 1 , 2 do
    	if self.intensifyProperty.type[i - 1] then
    		y = self.intensifyProperty.type[i - 1]:GetPositionY() - 25
    	end
	    self.intensifyProperty.type[i] = Util:Label(" ", Macros.TypefaceSize.slightly , Macros.Color.keypoint)
	    self.intensifyProperty.type[i]:SetAnchorPoint(1,1)
	    self.intensifyProperty.type[i]:SetPosition(equip_intensifyLv:GetPositionX() ,y)
	    self.Image_property_bg:AddChild(self.intensifyProperty.type[i])

		self.intensifyProperty.value[i] = Util:Label(" ", Macros.TypefaceSize.slightly , Macros.Color.content)
	    self.intensifyProperty.value[i]:SetAnchorPoint(0,1)
	    self.intensifyProperty.value[i]:SetPosition(self.intensifyProperty.type[i]:GetPositionX() + 5 ,y)
	    self.Image_property_bg:AddChild(self.intensifyProperty.value[i])    
    end

    -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    --精炼属性
    -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    local equip_refineProperty = Util:Label( CPPGameLib.GetString("equip_advancedProperty") ,nil )
    equip_refineProperty:SetAnchorPoint( 0 , 0.5 )
   	equip_refineProperty:SetPosition(  self.Image_type2:GetPositionX() + 10 , self.Image_type2:GetPositionY() + (self.Image_type2:GetContentHeight()/2) )
    self.Image_property_bg:AddChild(equip_refineProperty)

    --精炼等级
    local equip_refineLv = Util:Label( CPPGameLib.GetString("equip_advancedLv"), Macros.TypefaceSize.slightly , Macros.Color.keypoint )
    equip_refineLv:SetAnchorPoint(1,1)
    equip_refineLv:SetPosition(80 + equip_refineProperty:GetPositionX() + (80/2) + 13 , equip_refineProperty:GetPositionY() - equip_intensify:GetContentHeight() )
    self.Image_property_bg:AddChild(equip_refineLv)

    self.refineLv = Util:Label(" ", Macros.TypefaceSize.slightly , Macros.Color.content )
	self.refineLv:SetAnchorPoint(0,1)
	self.refineLv:SetPosition( equip_refineLv:GetPositionX() + 5, equip_refineLv:GetPositionY() )
    self.Image_property_bg:AddChild(self.refineLv)

    --精炼属性
    self.refineProperty = {}
    self.refineProperty.type = {}
    self.refineProperty.value = {}
    local y = equip_refineLv:GetPositionY() - 25
    for i = 1 , 2 do
    	if self.refineProperty.type[i - 1] then
    		y = self.refineProperty.type[i - 1]:GetPositionY() - 25
    	end
	    self.refineProperty.type[i] = Util:Label(" ", Macros.TypefaceSize.slightly , Macros.Color.keypoint)
	    self.refineProperty.type[i]:SetAnchorPoint(1,1)
	    self.refineProperty.type[i]:SetPosition(equip_refineLv:GetPositionX() ,y)
	    self.Image_property_bg:AddChild(self.refineProperty.type[i])

		self.refineProperty.value[i] = Util:Label(" ", Macros.TypefaceSize.slightly , Macros.Color.content )
	    self.refineProperty.value[i]:SetAnchorPoint(0,1)
	    self.refineProperty.value[i]:SetPosition(self.refineProperty.type[i]:GetPositionX() + 5 ,y)
	    self.Image_property_bg:AddChild(self.refineProperty.value[i])    
    end

    --介绍
    self.equip_introduce = Util:Label( " " , Macros.TypefaceSize.slightly , Macros.Color.content , self.Image_nameBg:GetContentWidth() )
    self.equip_introduce:SetAnchorPoint( 0 , 1)
    self.equip_introduce:SetPosition( self.Image_nameBg:GetPositionX()  , self.Image_nameBg:GetPositionY() - 7 )
    self.Image_infoBg:AddChild(self.equip_introduce)

	--滚动列表
	self.list = UIListViewEx.Create()
	self.list:SetContentSize(self.Panel_property_0:GetContentWidth() - 5 ,self.Panel_property_0:GetContentHeight() - 5 )
	self.list:SetPosition(2.5,2.5)
	self.list:SetAnchorPoint(0,0)
	self.list:SetBounceEnabled(true)
	self.Panel_property_0:AddChild(self.list) 
end

function Modules.TemplDivinationMinuteInfo:__SetShowUI()
	--获得我选中的装备信息
	local divinationUid = CallModuleFunc(ModuleType.DIVINATION, "GetOnDivinationUid")
	self.onDivination = CallModuleFunc(ModuleType.DIVINATION, "GetDivination" , divinationUid )

	if self.onDivination.hero_id ~= 0 then
		self.Button_change:SetVisible( true )
		self.Button_discharge:SetVisible( true )
		local t1 = CallModuleFunc(ModuleType.DIVINATION , "ToUidGetStrongCard" , divinationUid )
		if #t1 ~= 0 then
		    local effect
		    self.effect_id , effect = self:__CreateEffect({res_name = "ui_anniuzoubian"}, self.Button_change )
			effect:SetPosition( self.Button_change:GetContentWidth()/2 , self.Button_change:GetContentHeight()/2 + 2 )
		end
	end

	-- 图标
	local _ , divinationinfo = ObjIDTool.GetInfo( self.onDivination.info_id )

	self.icon:SetData({ id = self.onDivination.info_id })


	local text = ""
	if self.onDivination.break_level ~= 0 then
		text = " + " .. self.onDivination.break_level
	end
	--名字
	self.name:SetString( self.onDivination.name .. text )
	self.name:SetColor(unpack(Macros.Quality[self.onDivination.quality]))
	self.name:SetStrokeColor(unpack(Macros.Quality_Stroke[self.onDivination.quality]))	


	CPPGameLib.PrintTable("xy" , self.onDivination , "请求物品列表") 
	local pro_info = CallModuleFunc(ModuleType.DIVINATION, "ToInfoIdGetDivinationInfo" , self.onDivination.info_id , self.onDivination.level , self.onDivination.break_level )
	CPPGameLib.PrintTable("xy" , pro_info , "pro_info-----------") 
	--装备描述
	self.equip_introduce:SetString(divinationinfo.description)

	--强化上限
	local info = CallModuleFunc(ModuleType.DIVINATION, "GetDivinationUpMAX" , divinationUid )
	self.intensifyLv:SetString( self.onDivination.level .. "/" .. info.level )

	for i = 1 , #self.intensifyProperty.type do
		self.intensifyProperty.type[i]:SetString("  ")
		self.intensifyProperty.value[i]:SetString(" ")
	end

	--强化属性
	for i,v in ipairs( pro_info.up_proEx ) do
		local intensifyType , intensifyValue = Util:Nature(v.type , v.value , true)
		self.intensifyProperty.type[i]:SetString(intensifyType.."  ")
		self.intensifyProperty.value[i]:SetString(intensifyValue)
	end

	--精炼属性
	local _ , info = CallModuleFunc(ModuleType.DIVINATION, "GetDivinationBreakMAX" , divinationUid )
	CPPGameLib.PrintTable("xy" , info , "请求物品列表")  
	local text = self.onDivination.break_level
	if info.break_level then
		text = info.break_level + 1
	end
	self.refineLv:SetString( self.onDivination.break_level .. "/" .. text )

	for i = 1 , #self.refineProperty.type do
		self.refineProperty.type[i]:SetString("  ")
		self.refineProperty.value[i]:SetString(" ")
	end
	
	for i,v in ipairs( pro_info.break_pro ) do
		local refineType , refineValue = Util:Nature(v.type , v.value , true)
		self.refineProperty.type[i]:SetString(refineType.."  ")
		self.refineProperty.value[i]:SetString(refineValue)
	end

	--右边的信息框列表
	self.list:RemoveAllItems()

    --标题
    local function headline( text )
        local layou = Util:Layout( self.list:GetContentWidth() , 45  )

        --基本属性sp
        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp_headline")
        local basic = Util:Sprite(sp)
        basic:SetAnchorPoint( 0 , 1 )
        basic:SetPosition( 0 , layou:GetContentHeight() - 5 )
        layou:AddChild(basic)

       --基本属性
        local basicValue = Util:Label( text )
        basicValue:SetAnchorPoint(0,0.5)
        basicValue:SetPosition( 10 ,basic:GetContentHeight()/2)
        basic:AddChild(basicValue)

        return layou    
    end

	self.list:PushBackCustomItem( headline(CPPGameLib.GetString("divination_cardFate")) )	

    local function property( info_ )
    	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_item_checked")
    	local basic = Util:Sprite(sp)
	    basic:SetAnchorPoint( 0.5 , 0.5 )

        local layou = Util:Layout( self.list:GetContentWidth() , basic:GetContentHeight() )
	    basic:SetPosition( layou:GetContentWidth()/2 , layou:GetContentHeight()/2 )
	    layou:AddChild(basic)

		--图标
        local icon = LogicGUI.IconLayer.New(true)
        icon:SetAnchorPoint(0.5,0.5)
        icon:SetData({id = info_.id })
        icon:SetPosition( icon:GetNode():GetContentWidth()/2 + 15 , basic:GetContentHeight()/2 + 10 ) 
        basic:AddChild(icon:GetNode())

        local type_ , info = ObjIDTool.GetInfo( info_.id )
        --名字
		local name = Util:Name(info.name , nil , info.init_quality )
		name:SetPosition( icon:GetNode():GetPositionX() ,  icon:GetNode():GetPositionY() - icon:GetNode():GetContentHeight()/2 - 15 )
		name:SetAnchorPoint(0.5,0.5)
		basic:AddChild(name)

		--宿命名
		local fateName = Util:Label( info_.describeName ,Macros.TypefaceSize.normal , Macros.Color.keypoint )
		fateName:SetPosition( icon:GetNode():GetPositionX() + ( icon:GetNode():GetContentWidth()/2 ) + 10 , icon:GetNode():GetPositionY() + (icon:GetNode():GetContentHeight()/2) - (fateName:GetContentHeight()/2) - 5 )
		fateName:SetAnchorPoint(0 , 0.5)
		basic:AddChild(fateName)	

		--宿命
		local fateDescribe = Util:Label( info_.describeContent , Macros.TypefaceSize.slightly , Macros.Color.content , 255) 
		fateDescribe:SetPosition( icon:GetNode():GetPositionX() + ( icon:GetNode():GetContentWidth()/2 ) + 10 , icon:GetNode():GetPositionY() - (icon:GetNode():GetContentHeight()/2) + (fateDescribe:GetContentHeight()/2) + 20 )
		fateDescribe:SetAnchorPoint(0 , 1)
		basic:AddChild(fateDescribe)		

        return layou    
    end 

	local info = CallModuleFunc(ModuleType.DIVINATION, "ToUidGetFate" , divinationUid )
    --属性
    for i , v in ipairs( info ) do
	    self.list:PushBackCustomItem( property( v ) )
    end
	
end

--替换
function Modules.TemplDivinationMinuteInfo:OnButtonChange()
	print("xy" , "替换----")

	local divinationUid = CallModuleFunc(ModuleType.DIVINATION, "GetOnDivinationUid")
	local lineup_index , index = CallModuleFunc(ModuleType.HERO,"GetDivinationFormationPosAndCardSlotByCardUid" , divinationUid )
	local hero_info = CallModuleFunc(ModuleType.HERO, "GetLineupHeroInfo", lineup_index)
	-- CPPGameLib.PrintTable("xy" , hero_info , "请求物品hero_info列表") 
	GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.DIVINATION_DRESS, {{	
																					hero_id = hero_info.info_id ,  
																					index = lineup_index , 
																					item_type = index
																				}})
end

--卸下
function Modules.TemplDivinationMinuteInfo:OnButtonDischarge()

	--通过卡牌UID获取卡牌所在的阵位和孔位
	local divinationUid = CallModuleFunc(ModuleType.DIVINATION, "GetOnDivinationUid")
	local pos , index = CallModuleFunc(ModuleType.HERO,"GetDivinationFormationPosAndCardSlotByCardUid" , divinationUid )

	local info = {}
	info.formation_pos = pos
	info.card_slot = index
	CallModuleFunc(ModuleType.DIVINATION, "CollectionUnloadCardReq" , info )

	CallModuleFunc(ModuleType.HERO, "OpenView", BaseViewType.HERO_LINEUP, BaseViewSubType.HeroLineup.MAIN )
end