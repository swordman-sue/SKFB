
--[[
%% @module: 装备信息界面
%% @author: yjg
%% @created: 2016年11月5日16:58:56
--]]

Modules = Modules or {}

Modules.EquipLiftInfoTabView = Modules.EquipLiftInfoTabView or BaseClass(BaseTabView)

function Modules.EquipLiftInfoTabView:__init()
	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_EQUIP_DETAIY_ITEM)
end

--选项卡界面创建完成回调
function Modules.EquipLiftInfoTabView:__LoadedCallback()	
	self:__Getwidget()
	self:__ShowUI()
end

--选项卡界面打开回调
function Modules.EquipLiftInfoTabView:__OpenCallback()
	self:__SetShowUI()
end

--选项卡界面关闭回调
function Modules.EquipLiftInfoTabView:__CloseCallback()
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.EquipLiftInfoTabView:__Dispose()
end

--获得所有组件
function Modules.EquipLiftInfoTabView:__Getwidget()
	--信息容器
	self.Panel_equipInfo = self:GetWidget("Panel_equipInfo") 
	--右侧列表容器
	self.Panel_property_0 = self:GetWidget("Panel_property_0") 
	--左侧信息容器
	self.Image_infoBg = self:GetWidget("Image_infoBg")
	--名字背景
	self.Image_nameBg = self:GetWidget("Image_nameBg")
	--名字
	self.Text_name = self:GetWidget("Text_name") 
	--图标
	self.Panel_icon = self:GetWidget("Panel_icon") 
	--属性框
	self.Image_property_bg = self:GetWidget("Image_property_bg") 
	--线
	-- self.Image_division2 = self:GetWidget("Image_division2") 
	--强化属性
	self.Image_type1 = self:GetWidget("Image_type1") 
	--精炼属性
	self.Image_type2 = self:GetWidget("Image_type2") 
	--上箭头
	self.Image_up = self:GetWidget("Image_up") 
	self.Image_up:SetZOrder( 998 )
	--下箭头
	self.Image_below = self:GetWidget("Image_below") 
	self.Image_below:SetZOrder( 998 )
	--更换按钮
	self.Button_change = Util:WidgetButton( 
											self:GetWidget("Button_change") , 
											CPPGameLib.GetString("equip_btn_change") , 
											CPPGameLib.Handler(self,self.OnButtonChange),
											nil,nil,Macros.Color.button_red
											)
	self.Button_change:SetVisible(false)
	--卸下按钮 
	self.Button_discharge = Util:WidgetButton( 
												self:GetWidget("Button_discharge") , 
												CPPGameLib.GetString("equip_btn_discharge") , 
												CPPGameLib.Handler(self,self.OnButtonDischarge),
												nil,nil,Macros.Color.button_yellow
												)
	self.Button_discharge:SetVisible(false)
end

function Modules.EquipLiftInfoTabView:__ShowUI()
	--图标
	self.icon = LogicGUI.IconLayer.New(false)
    self.icon:SetPosition(self.Panel_icon:GetContentWidth()/2 ,self.Panel_icon:GetContentHeight()/2)
    self.icon:SetAnchorPoint(0.5, 0.5 )
    self.icon:SetScaleFactor(1)
    self.Panel_icon:AddChild(self.icon:GetNode())

    --名字 
	self.name = Util:Name( "  " , Macros.TypefaceSize.largish )
	self.name:SetPosition( self.Text_name:GetContentWidth()/2 , self.Text_name:GetContentHeight()/2 )
	self.name:SetAnchorPoint(0.5,0.5)
	self.Text_name:AddChild(self.name)

	self.taste = Util:RichText( CPPGameLib.GetString("equip_grade" , 0) )
	self.taste:SetAnchorPoint( 0.5 , 0.5 )
	self.taste:SetPosition( self.Panel_icon:GetContentWidth()/2 , -8 )
	self.Panel_icon:AddChild(self.taste)

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

    self.intensifyLv = Util:Label("000" , Macros.TypefaceSize.slightly , Macros.Color.content )
	self.intensifyLv:SetAnchorPoint(0,1)
	self.intensifyLv:SetPosition( equip_intensifyLv:GetPositionX() , equip_intensifyLv:GetPositionY() )
    self.Image_property_bg:AddChild(self.intensifyLv)

    --强化属性
    self.intensifyProperty = Util:Label(" ", Macros.TypefaceSize.slightly , Macros.Color.keypoint )
    self.intensifyProperty:SetAnchorPoint(1,0.5)
    self.intensifyProperty:SetPosition(equip_intensifyLv:GetPositionX() , equip_intensifyLv:GetPositionY() - (equip_intensifyLv:GetContentHeight()/2) - 25)
    self.Image_property_bg:AddChild(self.intensifyProperty)

    --强化属性值
    self.intensifyPropertyValue = Util:Label(" ", Macros.TypefaceSize.slightly , Macros.Color.content )
    self.intensifyPropertyValue:SetAnchorPoint(0,0.5)
    self.intensifyPropertyValue:SetPosition(self.intensifyProperty:GetPositionX() + 5, self.intensifyProperty:GetPositionY())
    self.Image_property_bg:AddChild(self.intensifyPropertyValue)

    -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    --精炼属性
    -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    local equip_refineProperty = Util:Label( CPPGameLib.GetString("equip_refineProperty") ,nil )
    equip_refineProperty:SetAnchorPoint( 0 , 0.5 )
   	equip_refineProperty:SetPosition(  self.Image_type2:GetPositionX() + 10 , self.Image_type2:GetPositionY() + (self.Image_type2:GetContentHeight()/2) )
    self.Image_property_bg:AddChild(equip_refineProperty)

    --精炼等级
    local equip_refineLv = Util:Label( CPPGameLib.GetString("equip_refineLv"), Macros.TypefaceSize.slightly , Macros.Color.keypoint )
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
    for i=1,2 do
    	if self.refineProperty.type[i - 1] then
    		y = self.refineProperty.type[i - 1]:GetPositionY() - 25
    	end
	    self.refineProperty.type[i] = Util:Label(" ", Macros.TypefaceSize.slightly , Macros.Color.keypoint)
	    self.refineProperty.type[i]:SetAnchorPoint(1,1)
	    self.refineProperty.type[i]:SetPosition(equip_refineLv:GetPositionX() ,y)
	    self.Image_property_bg:AddChild(self.refineProperty.type[i])

		self.refineProperty.value[i] = Util:Label(" ", Macros.TypefaceSize.slightly , Macros.Color.content)
	    self.refineProperty.value[i]:SetAnchorPoint(0,1)
	    self.refineProperty.value[i]:SetPosition(self.refineProperty.type[i]:GetPositionX() + 5 ,y)
	    self.Image_property_bg:AddChild(self.refineProperty.value[i])    

    end

    -- --介绍
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

--刷新页面的数据 讲道理这个是不会变的才对
function Modules.EquipLiftInfoTabView:__SetShowUI()
	--获得我选中的装备信息
	self.onEquipInfo = CallModuleFunc(ModuleType.EQUIP, "GetOnEquipInfo")

	self.taste:SetText( CPPGameLib.GetString("equip_grade" , self.onEquipInfo.taste) )

    --选中的装备的两种上限
	local liftMaxLst = CallModuleFunc(ModuleType.EQUIP, "GetLiftMaxInfo" , self.onEquipInfo.quality )
	
	--将两种上限加进列表
	self.onEquipInfo.strengthenMax = liftMaxLst.strengthenMax
	self.onEquipInfo.refineMax = liftMaxLst.refineMax

	if self.btnEffect_id then
		self:__DelEffect(self.btnEffect_id)
		self.btnEffect_id = nil
		self.BtnEffect_con = nil
	end

	--按照是否穿戴来决定穿戴按钮的显示
	if self.onEquipInfo.equip_heroid ~= 0 then
		self.Button_change:SetVisible( true )
		self.Button_discharge:SetVisible( true )
		self:OnButtonEffects()
	else
		self.Button_change:SetVisible( false )
		self.Button_discharge:SetVisible( false )	
	end

	if self.__open_data[1].btn == false then
		self.Button_change:SetVisible( false )
		self.Button_discharge:SetVisible( false )	
	end

	-- 图标
	local equipinfo = CPPGameLib.GetConfig("Equipment", self.onEquipInfo.info_id )
	self.icon:SetData({ id = equipinfo.res_id })

	--名字
	local refine_level = ""
	if self.onEquipInfo.refine_level ~= 0 then
		refine_level = " +"..self.onEquipInfo.refine_level
	end
	self.name:SetString( self.onEquipInfo.name .. refine_level )
	self.name:SetColor(unpack(Macros.Quality[self.onEquipInfo.quality]))
	self.name:SetStrokeColor(unpack(Macros.Quality_Stroke[self.onEquipInfo.quality]))	
	
	--强化属性
	local getIntensify , onceIntensify , nextIntensify = CallModuleFunc(ModuleType.EQUIP, "GetStrengthenPropPlusMap" , self.onEquipInfo.uid )
	local intensifyType , intensifyValue = Util:Nature(getIntensify.lst[1].type , getIntensify.lst[1].value , true)
	local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")

	local maxLv = userDate.level * 2
	if getIntensify.level == nextIntensify.level then
		maxLv = getIntensify.level
	end

	self.intensifyLv:SetString(self.onEquipInfo.strengthen_level .. "/" .. maxLv )
	self.intensifyProperty:SetString(intensifyType.."  ")
	self.intensifyPropertyValue:SetString(intensifyValue)

	--精炼属性
	local  getRefine , onceRefine , nextRefine = CallModuleFunc(ModuleType.EQUIP, "GetRefinePropPlusMap" , self.onEquipInfo.uid )	
	self.refineLv:SetString( getRefine.level .. "/" .. self.onEquipInfo.refineMax)

	for i,v in ipairs(getRefine.lst) do
		local refineType , refineValue = Util:Nature(v.type , v.value , true)
		self.refineProperty.type[i]:SetString(refineType.."  ")
		self.refineProperty.value[i]:SetString(refineValue)
	end

	--装备描述
	local type_ , info_ = ObjIDTool.GetInfo(self.onEquipInfo.info_id)
	self.equip_introduce:SetString(info_.description)

	--右边的信息框列表
	self.list:RemoveAllItems()
	local layout = Modules.EquipLiftInfoTabViewItem.New(self.onEquipInfo)
	self.list:PushBackCustomItem(layout:GetNode())	

	local lst = CallModuleFunc(ModuleType.EQUIP, "GetReinforcement" , self.onEquipInfo.info_id)
	self.Image_up:SetVisible(false)
	self.Image_below:SetVisible(false)

	if #lst > 1 then
		self.Image_up:SetVisible(true)
		self.Image_below:SetVisible(true)
	end 
end

--更换按钮
function Modules.EquipLiftInfoTabView:OnButtonChange()
	print("更换按钮")
    local item_type = CPPGameLib.GetConfig("Equipment", self.onEquipInfo.info_id).item_type
    CallModuleFunc(ModuleType.EQUIP, "OpenView", BaseViewType.EQUIP_DRESS, { 
    		hero_id = self.onEquipInfo.equip_heroid , 
    		index = CallModuleFunc(ModuleType.HERO, "GetLineupIndexOfEquipOrTreasure", self.onEquipInfo.uid) , 
    		item_type = item_type 
    	})
	-- self.__parent_view:CloseManual()
end

--卸下按钮
function Modules.EquipLiftInfoTabView:OnButtonDischarge()
	print("卸下按钮")
	local item_type = CPPGameLib.GetConfig("Equipment", self.onEquipInfo.info_id).item_type
	local info = {}
	info.formation_pos = CallModuleFunc(ModuleType.HERO, "GetLineupIndexOfEquipOrTreasure", self.onEquipInfo.uid)
	info.equipment_pos = item_type
	info.equipment_uid = 0
	CallModuleFunc(ModuleType.EQUIP, "EquipMentPutOnReq",info)
	CallModuleFunc(ModuleType.HERO, "OpenView", BaseViewType.HERO_LINEUP, BaseViewSubType.HeroLineup.MAIN , info.formation_pos)
end

--按钮特效
function Modules.EquipLiftInfoTabView:OnButtonEffects()
	local _, info = ObjIDTool.GetInfo(self.onEquipInfo.info_id)
	local lst = CallModuleFunc(ModuleType.EQUIP, "GetTypeEquip", info.item_type)
	local lineup_index = CallModuleFunc(ModuleType.HERO, "GetLineupIndexOfEquipOrTreasure", self.onEquipInfo.uid)
	local is_have_reddot = CallModuleFunc(ModuleType.HERO, 
		"ParseEquipListReinforcement", lst, self.onEquipInfo, self.onEquipInfo.equip_heroid, lineup_index)

	--背景特效
	if self.btnEffect_id then
		self:__DelEffect(self.btnEffect_id)
		self.btnEffect_id = nil
		self.btnEffect_con = nil
	end	

	if is_have_reddot == true then
		self.btnEffect_id , self.btnEffect_con = self:__CreateEffect({res_name = "ui_anniuzoubian"},self.Button_change,999)
		self.btnEffect_con:SetPosition(self.Button_change:GetContentWidth()/2 , self.Button_change:GetContentHeight()/2 )
	end
end

Modules = Modules or {}
Modules.EquipLiftInfoTabViewItem = Modules.EquipLiftInfoTabViewItem or BaseClass(GUI.Template)

function Modules.EquipLiftInfoTabViewItem:__init(data)
	-- self:InitTemplate(GUI.TemplateRegistry.TEMPL_EQUIP_DETAIY_ITEM)
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil) 

	self:__ShowUI(data)
end

function Modules.EquipLiftInfoTabViewItem:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout( 420 , 0 )
    self.layout:SetTouchEnabled(false)
    return self.layout
end


function Modules.EquipLiftInfoTabViewItem:__AddToParent()
end

function Modules.EquipLiftInfoTabViewItem:__delete()
end

function Modules.EquipLiftInfoTabViewItem:__ShowUI(data)

	self.data = data
	self.PosY = 10

	local cnf = CPPGameLib.GetConfig("Equipment",data.info_id)
	local starUpIsOpen = CallModuleFunc(ModuleType.EQUIP, "StarUpIsOpen")
	if cnf.star_max and starUpIsOpen then
		local equipInfo = CallModuleFunc(ModuleType.EQUIP, "GetOnEquipInfo")
		local info = CallModuleFunc(ModuleType.EQUIP, "ToUidGetEquipUpStarInfo", equipInfo.uid )
		self.__root_widget:SetContentSize( self.__root_widget:GetContentWidth() , self.PosY + ( (#info.nowInfo.info + 1) * 32.5 ) + 85 + self.__root_widget:GetContentHeight() )
	end

	local equipLst , natureLst , suitList = CallModuleFunc(ModuleType.EQUIP, "GetSuit", nil , data)
	local equipType = false
	if equipLst then
	    self.__root_widget:SetContentSize( self.__root_widget:GetContentWidth() , self.PosY + 315 + self.__root_widget:GetContentHeight() )
		equipType = true
	end

    local lst = CPPGameLib.GetConfig("EquipmentTalent", data.talent, false)
    local talentLst = nil
    if lst and lst.level_list then
    	talentLst = lst.level_list
    end

    local talentType = false
    if talentLst and #talentLst ~= 0 then
    	self.__root_widget:SetContentSize( self.__root_widget:GetContentWidth() , self.PosY + ( (#talentLst + 1) * 32.5 ) + 65 + self.__root_widget:GetContentHeight() )
		talentType = true
	end

	local luckLst = CallModuleFunc(ModuleType.EQUIP, "GetReinforcement" , data.info_id)
	luckType = false
	if luckLst and #luckLst  ~= 0 then
		self.__root_widget:SetContentSize( self.__root_widget:GetContentWidth() , self.PosY + (#luckLst * 140) + 12 + self.__root_widget:GetContentHeight() )
		luckType = true
	end

	if cnf.star_max and starUpIsOpen then
		self:EquipUpStar(data) 	-- 升星
	end

	if equipLst then
		self:EquipSuit(data)   -- 套装
	end

	if talentLst and #talentLst ~= 0 then
		self:EquipTalent(data)	--天赋
	end
	
	if luckLst and #luckLst  ~= 0 then
		self:EquipFate(data)	--宿命
	end

	if equipType == false and talentType == false and luckType == false then
		self:EquipGet()
	end
end


--升星
function Modules.EquipLiftInfoTabViewItem:EquipUpStar(data)
    local path = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp_headline")
	self.Image_headline1 = Util:Sprite(path)
	self.Image_headline1:SetPosition( 0 , self.layout:GetContentHeight() - self.PosY )
	self.Image_headline1:SetAnchorPoint( 0 , 1 )
	self.layout:AddChild(self.Image_headline1)

    --升星
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_up_star",true)
    self.btn_up_star = Util:Button( sp , function( ... )
    	CallModuleFunc(ModuleType.EQUIP, "OpenView", BaseViewType.EQUIP_UP_STAR ) 
    end )
    self.btn_up_star:SetPosition( 340 , self.Image_headline1:GetContentHeight()/2 - 11 )
    self.btn_up_star:SetAnchorPoint( 0 , 0.5 )
    self.Image_headline1:AddChild(self.btn_up_star) 
    self.btn_up_star:SetScaleFactor( 0.8 )

	--套装属性
	local equip_suit = Util:Label(CPPGameLib.GetString("equip_starSuit")) 
	equip_suit:SetPosition( 10  , self.Image_headline1:GetContentHeight()/2)
	equip_suit:SetAnchorPoint( 0 ,0.5)
	self.Image_headline1:AddChild(equip_suit)

	local equipInfo = CallModuleFunc(ModuleType.EQUIP, "GetOnEquipInfo")
	local info = CallModuleFunc(ModuleType.EQUIP, "ToUidGetEquipUpStarInfo", equipInfo.uid )

	local talentLst = info.nowInfo.info

    local t1 = { 
                [1] = { type = CPPGameLib.GetString("equip_star") , value = " " }
             }
    for i , v in ipairs( talentLst ) do
        local type_ , value_ = Util:Nature( v.type , v.value , true  , true )
        table.insert( t1 , { type = type_ , value = "+"..value_ } )
    end

	CPPGameLib.PrintTable("xy" , info.nowInfo , "data---------------") 

    local y = 0
    local heroLst = {}
    for i,v in ipairs(t1) do
    	if heroLst[i - 1] then
    		y = heroLst[i - 1]:GetPositionY() - ( heroLst[i - 1]:GetContentHeight() + 5 )
    	else
    		y = self.Image_headline1:GetPositionY() - 85
    	end

    	local bg_sp9_resource = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_resource")

    	--背景
    	heroLst[i] = Util:Layout( self.layout:GetContentWidth() , 30 )
    	heroLst[i]:SetPosition( self.layout:GetContentWidth()/2 , y )
    	heroLst[i]:SetAnchorPoint( 0.5 , 0.5 )
    	self.layout:AddChild(heroLst[i])

    	local name_sp = Util:Sprite9( bg_sp9_resource )
    	name_sp:SetContentSize( 116.00 , 30 )
    	name_sp:SetPosition( 77.00 , heroLst[i]:GetContentHeight()/2 )
    	heroLst[i]:AddChild(name_sp)

    	--效果名
    	local type_ = Util:Label( v.type ..  CPPGameLib.GetString("common_colon") , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
    	type_:SetAnchorPoint(0.5 , 0.5)
    	type_:SetPosition(name_sp:GetContentWidth()/2 , name_sp:GetContentHeight()/2  )
    	name_sp:AddChild(type_)

    	--内容
    	local content_sp = Util:Sprite9( bg_sp9_resource )
    	content_sp:SetContentSize( 253.09 , 30 )
    	content_sp:SetPosition( 267.92 , heroLst[i]:GetContentHeight()/2 )
    	heroLst[i]:AddChild(content_sp)

    	local scale = 0.5
	    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "star2")
    	if i == 1 then
		    for j = 1 , equipInfo.star do
		        local star = Util:Sprite( sp )
		        star:SetAnchorPoint( 0 , 0.5 )
		        star:SetPosition( ( star:GetContentWidth() * scale )/2 - 5  + ( (j-1) * ( ( star:GetContentWidth() * scale ) + 5 ) ) , content_sp:GetContentHeight()/2 )
		        content_sp:AddChild( star )
		        star:SetScaleFactor(0.5)
		    end

		    local level = Util:Name( " (" .. info.nowInfo.name ..") " , 14 , 1 )
		    level:SetAnchorPoint( 1 , 0 )
		    level:SetPosition( content_sp:GetContentWidth() - 2 , 2 )
		    content_sp:AddChild( level )
    	end

    	local image_value = Util:Label( v.value , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
    	image_value:SetAnchorPoint(0.5,0.5)
    	image_value:SetPosition( content_sp:GetContentWidth()/2 ,content_sp:GetContentHeight()/2)
    	content_sp:AddChild(image_value)
    end
    self.PosY = self.PosY + ( (#info.nowInfo.info + 1) * 32.5 ) + 85
end

--套装
function Modules.EquipLiftInfoTabViewItem:EquipSuit(data)

    local path = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp_headline")
	self.Image_headline2 = Util:Sprite(path)
	self.Image_headline2:SetPosition( 0 , self.layout:GetContentHeight() - self.PosY )
	self.Image_headline2:SetAnchorPoint( 0 , 1 )
	self.layout:AddChild(self.Image_headline2)

	--套装属性
	local equip_suit = Util:Label(CPPGameLib.GetString("equip_suit")) 
	equip_suit:SetPosition( 10  , self.Image_headline2:GetContentHeight()/2)
	equip_suit:SetAnchorPoint( 0 ,0.5)
	self.Image_headline2:AddChild(equip_suit)

	local equipLst , natureLst , suitList = CallModuleFunc(ModuleType.EQUIP, "GetSuit", nil , data)

	local info = CPPGameLib.GetConfig("EquipmentSuit", equipLst[1].suit_id)

	--名字
	local name = Util:Name(info.name , nil , data.quality )
	name:SetPosition(self.layout:GetContentWidth()/2 , self.Image_headline2:GetPositionY() - 50 )
	name:SetAnchorPoint(0.5,0.5)
	self.layout:AddChild(name)

	--线
	local shortlineSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "shortline")

	--右边
	local shortline_1 = Util:Sprite9( shortlineSp , {50,10} )
	shortline_1:SetContentSize(shortline_1:GetContentWidth()/2 , shortline_1:GetContentHeight())
	shortline_1:SetAnchorPoint( 1 , 0.5 )
	shortline_1:SetPosition( name:GetContentWidth() + shortline_1:GetContentWidth() + 10 , name:GetContentHeight()/2 )
	name:AddChild(shortline_1)

	--左边
	local shortline_2 = Util:Sprite9( shortlineSp , {50,10} )
	shortline_2:SetContentSize(shortline_2:GetContentWidth()/2 , shortline_2:GetContentHeight())
	shortline_2:SetAnchorPoint( 1 , 0.5 )
	shortline_2:SetFlipX(true)
	shortline_2:SetPosition( 0 - shortline_2:GetContentWidth() - 10 , name:GetContentHeight()/2 )
	name:AddChild(shortline_2)

	--居中用
    local layer = Util:Layout(100 * #equipLst , 76 )
    layer:SetAnchorPoint(0.5,0.5)
    layer:SetPosition(self.layout:GetContentWidth()/2 , self.Image_headline2:GetPositionY() - 110 )
    self.layout:AddChild(layer)

    local interval = layer:GetContentWidth() / #equipLst

	local crossLst = CallModuleFunc(ModuleType.EQUIP, "GetCross", data.equip_heroid )

	local iconEffectLst = {}
	for i,v in ipairs(crossLst) do
		iconEffectLst[v.info_id] = v
	end

	--图标
    for i,v in ipairs(equipLst) do

        local icon_ = LogicGUI.IconLayer.New(true)
        icon_:SetPosition( 50 + interval * (i-1) ,layer:GetContentHeight()/2 - 2 ) 
        icon_:SetAnchorPoint(0.5,0.5)

        icon_:SetData({id = v.id , data = iconEffectLst[v.id] })
        layer:AddChild(icon_:GetNode())

        if not iconEffectLst[v.id] then
        	icon_:ShowAptitude(false)
        	icon_:SetGray(true)

		 --    local effect_id , effect = self:__CreateEffect({res_name = "UI_taozhuang" }, icon_:GetNode() )
			-- effect:SetPosition( icon_:GetNode():GetContentWidth()/2 - 2 , icon_:GetNode():GetContentHeight()/2 + 2)	
   --      	effect:SetScaleFactor(1.05)
   --      	effect:SetZOrder(999)
        end

        local name_ = Util:Name(v.name , nil , data.quality )
        name_:SetAnchorPoint(0.5, 1 )
        name_:SetPosition(icon_:GetNode():GetContentWidth()/2 , -5 )
        icon_:GetNode():AddChild(name_)
    end

	local suit = {}
	local suitItem = 0
	for i,v in ipairs(crossLst) do
		if v.suit_id == equipLst[1].suit_id then
			suitItem = suitItem + 1
		end
	end


    local y = 0
    local heroLst = {}

    local bg_sp9_resource = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_resource")

    --效果
    for i,lst in ipairs(natureLst) do
    	local lab = ""
    	for __,v in ipairs(lst) do
    		local type_ , value = Util:Nature(v.type , v.value , true)

    		lab = lab .. "   " .. type_ .." ".. value
    	end
    	
    	local color = Macros.Color.keypoint
    	if i < suitItem then
    		color = Macros.Color.red_color
    	end

    	if heroLst[i - 1] then
    		y = heroLst[i - 1]:GetPositionY() - ( heroLst[i - 1]:GetContentHeight() + 5 )
    	else
    		y = self.Image_headline2:GetPositionY() - 210
    	end

    	--背景
    	heroLst[i] = Util:Layout( self.layout:GetContentWidth() , 30 )
    	heroLst[i]:SetPosition( self.layout:GetContentWidth()/2 , y )
    	heroLst[i]:SetAnchorPoint( 0.5 , 0.5 )
    	self.layout:AddChild(heroLst[i])

    	local name_sp = Util:Sprite9( bg_sp9_resource )
    	name_sp:SetContentSize( 116.00 , 30 )
    	name_sp:SetPosition( 77.00 , heroLst[i]:GetContentHeight()/2 )
    	heroLst[i]:AddChild(name_sp)

    	--内容
    	local content_sp = Util:Sprite9( bg_sp9_resource )
    	content_sp:SetContentSize( 253.09 , 30 )
    	content_sp:SetPosition( 267.92 , heroLst[i]:GetContentHeight()/2 )
    	heroLst[i]:AddChild(content_sp)

    	--套装效果名
    	local type_ = Util:Label( CPPGameLib.GetString("equip_JS"..i) , Macros.TypefaceSize.slightly , color )
    	type_:SetAnchorPoint(0.5 , 0.5)
    	type_:SetPosition(name_sp:GetContentWidth()/2 , name_sp:GetContentHeight()/2  )
    	name_sp:AddChild(type_)

    	local image_value = Util:Label(lab , Macros.TypefaceSize.slightly , color )
    	image_value:SetAnchorPoint(0,0.5)
    	image_value:SetPosition(0,content_sp:GetContentHeight()/2)
    	content_sp:AddChild(image_value)
    end

    self.PosY = self.PosY + 310
end

--天赋
function Modules.EquipLiftInfoTabViewItem:EquipTalent(data)

    local path = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp_headline")
	self.Image_headline3 = Util:Sprite(path)
	self.Image_headline3:SetPosition( 0 , self.layout:GetContentHeight() - self.PosY )
	self.Image_headline3:SetAnchorPoint( 0 , 1 )
	self.layout:AddChild(self.Image_headline3)

  --装备天赋
	local equip_talent = Util:Label(CPPGameLib.GetString("equip_talent") ) 
	equip_talent:SetPosition( 10 , self.Image_headline3:GetContentHeight()/2)
	equip_talent:SetAnchorPoint( 0 ,0.5)
	self.Image_headline3:AddChild(equip_talent)


    local talentLst = {}
    if data.talent then
	    local lst = CPPGameLib.GetConfig("EquipmentTalent",data.talent)
	    talentLst = lst.level_list
	    table.sort( talentLst , function ( a , b )
	    	return a.talent_level < b.talent_level
	    end )
    end

    local y = 0
    local heroLst = {}
    for i,v in ipairs(talentLst) do
    	if heroLst[i - 1] then
    		y = heroLst[i - 1]:GetPositionY() - ( heroLst[i - 1]:GetContentHeight() + 5 )
    	else
    		y = self.Image_headline3:GetPositionY() - 65
    	end

    	local bg_sp9_resource = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_resource")

    	--背景
    	heroLst[i] = Util:Layout( self.layout:GetContentWidth() , 30 )
    	heroLst[i]:SetPosition( self.layout:GetContentWidth()/2 , y )
    	heroLst[i]:SetAnchorPoint( 0.5 , 0.5 )
    	self.layout:AddChild(heroLst[i])

    	local color = Macros.Color.content
    	local text = CPPGameLib.GetString("equip_clear" , v.intensify_level )
    	if v.intensify_level <= data.strengthen_level then
    		color = Macros.Color.front_color
    		text = ""
    	end

    	local name_sp = Util:Sprite9( bg_sp9_resource )
    	name_sp:SetContentSize( 116.00 , 30 )
    	name_sp:SetPosition( 77.00 , heroLst[i]:GetContentHeight()/2 )
    	heroLst[i]:AddChild(name_sp)

    	local name = Util:Label( v.name , Macros.TypefaceSize.normal , color )
    	name:SetAnchorPoint( 0.5 , 0.5 )
    	name_sp:AddChild( name )
    	PosTool.Center( name )

    	--内容
    	local content_sp = Util:Sprite9( bg_sp9_resource )
    	content_sp:SetContentSize( 253.09 , 30 )
    	content_sp:SetPosition( 267.92 , heroLst[i]:GetContentHeight()/2 )
    	heroLst[i]:AddChild(content_sp)

    	local content_tips = Util:Label( text , Macros.TypefaceSize.normal , Macros.Color.headline )
    	content_tips:SetAnchorPoint( 0 , 0.5 )
    	content_tips:SetPosition( 10 , content_sp:GetContentHeight()/2 )
    	content_sp:AddChild( content_tips )

    	local nature_describe = Util:Label( v.nature_describe , Macros.TypefaceSize.normal , color )
    	content_sp:AddChild( nature_describe )
    	PosTool.RightTo( content_tips , nature_describe , 5 )
    end

    self.PosY = self.PosY + ( (#talentLst + 1) * 32.5 ) + 65 + 10
end

--宿命
function Modules.EquipLiftInfoTabViewItem:EquipFate(data)

    local path = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp_headline")
	self.Image_headline4 = Util:Sprite(path)
	self.Image_headline4:SetPosition( 0 , self.layout:GetContentHeight() - self.PosY )
	self.Image_headline4:SetAnchorPoint( 0 , 1 )
	self.layout:AddChild(self.Image_headline4)

    --装备宿命
	local equip_lot = Util:Label( CPPGameLib.GetString("equip_lot") ) 
	equip_lot:SetPosition( 10 , self.Image_headline4:GetContentHeight()/2)
	equip_lot:SetAnchorPoint( 0 ,0.5)
	self.Image_headline4:AddChild(equip_lot)

	local luckLst = CallModuleFunc(ModuleType.EQUIP, "GetReinforcement" , data.info_id)
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_item_checked")
    local y = 0
    local heroLst = {}
    for i,v in ipairs(luckLst) do
    	if heroLst[i - 1] then
    		y = heroLst[i - 1]:GetPositionY() - ( heroLst[i - 1]:GetContentHeight() + 5 )
    	else
    		y = self.Image_headline4:GetPositionY() - 110
    	end

    	--背景
    	heroLst[i] = Util:Sprite( sp )
    	heroLst[i]:SetPosition( self.layout:GetContentWidth()/2 , y )
    	self.layout:AddChild(heroLst[i])

		-- 2615 装备界面的宿命，头像框和名字往上提2个像素
    	--图标
        local icon = LogicGUI.IconLayer.New(true)
        icon:SetAnchorPoint(0.5,0.5)
        icon:SetData({id = v.heroInfoId})
        icon:SetPosition( icon:GetNode():GetContentWidth()/2 + 15 , heroLst[i]:GetContentHeight()/2 + 12 ) 
        heroLst[i]:AddChild(icon:GetNode())

        local type_ , info = ObjIDTool.GetInfo( v.heroInfoId )
        --名字
		local name = Util:Name(info.name , nil , info.init_quality )
		name:SetPosition( icon:GetNode():GetPositionX() ,  icon:GetNode():GetPositionY() - icon:GetNode():GetContentHeight()/2 - 15 )
		name:SetAnchorPoint(0.5,0.5)
		heroLst[i]:AddChild(name)

		--宿命名
		local fateName = Util:Label( v.fateName ,nil , Macros.Color.pack_color ) 
		fateName:SetPosition( icon:GetNode():GetPositionX() + ( icon:GetNode():GetContentWidth()/2 ) + 10 , icon:GetNode():GetPositionY() + (icon:GetNode():GetContentHeight()/2) - (fateName:GetContentHeight()/2) - 5 )
		fateName:SetAnchorPoint(0 , 0.5)
		heroLst[i]:AddChild(fateName)	

		--宿命
		local fateDescribe = Util:Label( v.fateDescribe , Macros.TypefaceSize.normal , Macros.Color.keypoint , 270) 
		fateDescribe:SetPosition( icon:GetNode():GetPositionX() + ( icon:GetNode():GetContentWidth()/2 ) + 10 , icon:GetNode():GetPositionY() - (icon:GetNode():GetContentHeight()/2) + (fateDescribe:GetContentHeight()/2) + 5 )
		fateDescribe:SetAnchorPoint(0 , 1)
		heroLst[i]:AddChild(fateDescribe)		
    end

    self.PosY = self.PosY + (#luckLst * 140) + 12
end

--获取
function Modules.EquipLiftInfoTabViewItem:EquipGet(data)

	self.__root_widget:SetContentSize( self.__root_widget:GetContentWidth() , 474.00 )

	local suit = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "suit"))
	suit:SetAnchorPoint(0.5, 0.5)
	suit:SetPosition( self.layout:GetContentWidth()/2 , self.layout:GetContentHeight()/2 + 50 )
	self.layout:AddChild(suit , 0)

	local equip_NoNature = Util:Label( CPPGameLib.GetString("equip_NoNature") , Macros.TypefaceSize.normal , Macros.Color.content , 200 )
	equip_NoNature:SetAnchorPoint( 0 , 1 )
	equip_NoNature:SetPosition( 220 , 195 )
	suit:AddChild(equip_NoNature , 0)

	--更多属性
	local tower_moreNature = Util:Text( 
										CPPGameLib.GetString("equip_getNewequip") , 
										Macros.TypefaceSize.normal , 
										Macros.Color.headline , function ( ... )
											GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, self.data.info_id,true)
										end) 
	tower_moreNature:SetAnchorPoint(0.5 , 0.5)
	tower_moreNature:SetPosition( suit:GetContentWidth()/2 , 0 - 20 )
	suit:AddChild( tower_moreNature )
end
