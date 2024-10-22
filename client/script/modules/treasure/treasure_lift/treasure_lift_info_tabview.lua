
--[[
%% @module: 圣器信息界面
%% @author: yjg
%% @created: 2016年11月5日16:58:56
--]]

Modules = Modules or {}

Modules.TreasureLiftInfoTabView = Modules.TreasureLiftInfoTabView or BaseClass(BaseTabView)

function Modules.TreasureLiftInfoTabView:__init()
end

--选项卡界面创建完成回调
function Modules.TreasureLiftInfoTabView:__LoadedCallback()
	self:__Getwidget()
	self:__ShowUI()
end

--选项卡界面打开回调
function Modules.TreasureLiftInfoTabView:__OpenCallback()
	self:__SetShowUI()
end

--选项卡界面关闭回调
function Modules.TreasureLiftInfoTabView:__CloseCallback()
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.TreasureLiftInfoTabView:__Dispose()
end

--获得所有组件
function Modules.TreasureLiftInfoTabView:__Getwidget()
	--信息容器
	self.Panel_treasureInfo = self:GetWidget("Panel_treasureInfo") 
	--右侧列表容器
	self.Panel_property_0 = self:GetWidget("Panel_property_0") 
	--左侧信息容器
	self.Image_infoBg = self:GetWidget("Image_infoBg")
	--名字
	self.Text_name = self:GetWidget("Text_name") 
	--名字背景
	self.Image_nameBg = self:GetWidget("Image_nameBg")
	--图标
	self.Panel_icon = self:GetWidget("Panel_icon") 
	--属性框
	self.Image_property_bg = self:GetWidget("Image_property_bg") 
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

function Modules.TreasureLiftInfoTabView:__ShowUI()
	--图标
	self.icon = LogicGUI.IconLayer.New(false)
    self.icon:SetPosition(self.Panel_icon:GetContentWidth()/2 ,self.Panel_icon:GetContentHeight()/2)
    self.icon:SetAnchorPoint(0.5, 0.5 )
    self.icon:SetScaleFactor(1.2)
    self.Panel_icon:AddChild(self.icon:GetNode())

    --名字 
	self.name = Util:Name( "  " , Macros.TypefaceSize.largish )
	self.name:SetPosition( self.Text_name:GetContentWidth()/2 , self.Text_name:GetContentHeight()/2 )
	self.name:SetAnchorPoint(0.5,0.5)
	self.Text_name:AddChild(self.name)

    -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    --强化属性
    -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    local equip_intensify = Util:Label( CPPGameLib.GetString("equip_intensifyProperty") , Macros.TypefaceSize.largish )
    equip_intensify:SetAnchorPoint( 0 , 0.5 )
   	equip_intensify:SetPosition( self.Image_type1:GetPositionX() + 10 , self.Image_type1:GetPositionY() + (self.Image_type1:GetContentHeight()/2) )
    self.Image_property_bg:AddChild(equip_intensify)
    
    --强化等级
    local equip_intensifyLv = Util:Label( CPPGameLib.GetString("equip_intensifyLv") , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
    equip_intensifyLv:SetAnchorPoint(1,1)
    equip_intensifyLv:SetPosition(equip_intensify:GetContentWidth() + equip_intensify:GetPositionX() + ( equip_intensify:GetContentWidth()/2 ) , equip_intensify:GetPositionY() - equip_intensify:GetContentHeight() )
    self.Image_property_bg:AddChild(equip_intensifyLv)

    self.intensifyLv = Util:Label("000" , Macros.TypefaceSize.slightly , Macros.Color.content )
	self.intensifyLv:SetAnchorPoint(0,1)
	self.intensifyLv:SetPosition( equip_intensifyLv:GetPositionX() + 5, equip_intensifyLv:GetPositionY() )
    self.Image_property_bg:AddChild(self.intensifyLv)

    self.intensifyProperty = {}
    self.intensifyProperty.type = {}
    self.intensifyProperty.value = {}

    local y = equip_intensify:GetPositionY() - equip_intensify:GetContentHeight() - 35
    for i=1,2 do
    	if self.intensifyProperty.type[i-1] then
    		y = self.intensifyProperty.type[i-1]:GetPositionY() - 25
    	end
    	self.intensifyProperty.type[i] = Util:Label("  ", Macros.TypefaceSize.slightly , Macros.Color.keypoint)
    	self.intensifyProperty.type[i]:SetAnchorPoint(1,0.5)
    	self.intensifyProperty.type[i]:SetPosition(equip_intensifyLv:GetPositionX() , y )
    	self.Image_property_bg:AddChild(self.intensifyProperty.type[i])

    	self.intensifyProperty.value[i] = Util:Label("  ", Macros.TypefaceSize.slightly , Macros.Color.content )
    	self.intensifyProperty.value[i]:SetAnchorPoint(0,0.5)
    	self.intensifyProperty.value[i]:SetPosition(self.intensifyProperty.type[i]:GetPositionX() + 5 , y )
    	self.Image_property_bg:AddChild(self.intensifyProperty.value[i])

    end

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
    equip_refineLv:SetPosition(equip_refineProperty:GetContentWidth() + equip_refineProperty:GetPositionX() + (equip_refineProperty:GetContentWidth()/2) , equip_refineProperty:GetPositionY() - equip_intensify:GetContentHeight() )
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

--刷新页面的数据 讲道理这个是不会变的才对
function Modules.TreasureLiftInfoTabView:__SetShowUI()
    --获得我选中的圣器信息
    self.onTreasureInfo = CallModuleFunc(ModuleType.TREASURE, "GetOnTreasureInfo")
    --选中的圣器的两种上限
	local liftMaxLst = CallModuleFunc(ModuleType.TREASURE, "GetLiftMaxInfo" , self.onTreasureInfo.quality )
	--将两种上限加进列表
	self.onTreasureInfo.strengthenMax = liftMaxLst.strengthenMax
	self.onTreasureInfo.refineMax = liftMaxLst.refineMax

	--按照是否穿戴来决定穿戴按钮的显示
	if self.onTreasureInfo.hero_infoid ~= 0 then
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
	self.icon:SetData({ id = self.onTreasureInfo.info_id })

	--名字
	local refine_level = ""
	if self.onTreasureInfo.refine_level ~= 0 then
		refine_level = " +"..self.onTreasureInfo.refine_level
	end
	self.name:SetString( self.onTreasureInfo.name .. refine_level )
	self.name:SetColor(unpack(Macros.Quality[self.onTreasureInfo.quality]))
	self.name:SetStrokeColor(unpack(Macros.Quality_Stroke[self.onTreasureInfo.quality]))	

	--强化上限
	self.intensifyLv:SetString(self.onTreasureInfo.strengthen_level .. "/" .. self.onTreasureInfo.strengthenMax)
	
	--获得强化属性
	local getIntensify = CallModuleFunc(ModuleType.TREASURE, "GetStrengthenPropPlusMap" , self.onTreasureInfo.uid )
	if getIntensify ~= 0 then
		for i,v in ipairs(getIntensify.lst) do
			local intensifyType , intensifyValue = Util:Nature(v.type , v.value ,true)
			self.intensifyProperty.type[i]:SetString(intensifyType .." ")
			self.intensifyProperty.value[i]:SetString(intensifyValue)
		end
	end

	--获得精炼属性
	local getRefine = CallModuleFunc(ModuleType.TREASURE, "GetRefinePropPlusMap" , self.onTreasureInfo.uid)
	self.refineLv:SetString(self.onTreasureInfo.refine_level .. "/" .. self.onTreasureInfo.refineMax)
    
	if getRefine ~= 0 then
		for i,v in ipairs(getRefine.lst) do
			local refineType , refineValue = Util:Nature(v.type , v.value , true)
			self.refineProperty.type[i]:SetString(refineType.." ")
			self.refineProperty.value[i]:SetString(refineValue)
		end
	end

	--装备描述
	local type_ , info_ = ObjIDTool.GetInfo(self.onTreasureInfo.info_id)
	self.equip_introduce:SetString(info_.description)


	--右边的信息框列表
	self.list:RemoveAllItems()
	local layout = Modules.TreasureLiftInfoTabViewItem.New(self.onTreasureInfo)
	self.list:PushBackCustomItem(layout:GetNode())

	local lst = CallModuleFunc(ModuleType.TREASURE, "GetReinforcement" , self.onTreasureInfo.info_id)
	self.Image_up:SetVisible(false)
	self.Image_below:SetVisible(false)

	if #lst > 1 then
		self.Image_up:SetVisible(true)
		self.Image_below:SetVisible(true)
	end 
end

--更换按钮
function Modules.TreasureLiftInfoTabView:OnButtonChange()
    local item_type = self.onTreasureInfo.type
    CallModuleFunc(ModuleType.TREASURE, "OpenView", BaseViewType.TREASURE_DRESS,{ 
    				hero_id = self.onTreasureInfo.hero_infoid , 
    				index = CallModuleFunc(ModuleType.HERO, "GetLineupIndexOfEquipOrTreasure", self.onTreasureInfo.uid), 
    				item_type = item_type 
    			})
    -- self.__parent_view:CloseManual()
end

--卸下按钮
function Modules.TreasureLiftInfoTabView:OnButtonDischarge()
	local info = {}
	info.formation_pos = CallModuleFunc(ModuleType.HERO, "GetLineupIndexOfEquipOrTreasure", self.onTreasureInfo.uid)
	info.treasure_pos = self.onTreasureInfo.type
	info.treasure_uid = 0
	CallModuleFunc(ModuleType.TREASURE, "TreasurePutOnReq",info)
	CallModuleFunc(ModuleType.HERO, "OpenView", BaseViewType.HERO_LINEUP, BaseViewSubType.HeroLineup.MAIN , info.formation_pos)
end

--按钮特效
function Modules.TreasureLiftInfoTabView:OnButtonEffects()
	
	local cnf = CallModuleFunc(ModuleType.TREASURE, "GetBetterTreasureList", self.onTreasureInfo )
	--背景特效
	if self.btnEffect_id then
		self:__DelEffect(self.btnEffect_id)
		self.btnEffect_id = nil
		self.btnEffect_con = nil
	end	

	if cnf and #cnf ~= 0 then
		self.btnEffect_id , self.btnEffect_con = self:__CreateEffect({res_name = "ui_anniuzoubian"},self.Button_change,999)
		self.btnEffect_con:SetPosition(self.Button_change:GetContentWidth()/2 , self.Button_change:GetContentHeight()/2 )
	end
end

--[[
%% @module: 装备信息界面2
%% @author: yjg
%% @created: 2016年11月5日16:58:56
--]]

Modules = Modules or {}
Modules.TreasureLiftInfoTabViewItem = Modules.TreasureLiftInfoTabViewItem or BaseClass(GUI.Template)
function Modules.TreasureLiftInfoTabViewItem:__init(data)
	self.__layout_mode = GUI.Template.LayoutMode.CODE
	self:InitTemplate(nil)
	self:__ShowUI(data)
end

function Modules.TreasureLiftInfoTabViewItem:__CreateWidgetFromLuaCode()
	self.layout = Util:Layout()
	self.layout:SetAnchorPoint(0, 0)
	self.layout:SetContentSize(415.00, 33.00)
	return self.layout
end

function Modules.TreasureLiftInfoTabViewItem:__AddToParent()

end

function Modules.TreasureLiftInfoTabViewItem:__delete()
end

function Modules.TreasureLiftInfoTabViewItem:__Getwidget()
end

function Modules.TreasureLiftInfoTabViewItem:__ShowUI(data)

    local path = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp_headline")
	local vessel = Util:Sprite(path)
	vessel:SetPosition( 0 , self.layout:GetContentHeight() - 5)
	vessel:SetAnchorPoint( 0 , 1 )
	self.layout:AddChild(vessel)

	local lab = Util:Label( CPPGameLib.GetString("treasure_grid_fate") , nil )
	lab:SetAnchorPoint( 0 , 0.5 )
	lab:SetPosition(vessel:GetPositionX() + 10 , vessel:GetContentHeight()/2 )
	vessel:AddChild(lab)

	local lst = CallModuleFunc(ModuleType.TREASURE, "GetReinforcement" , data.info_id )

	self.layout:SetContentSize( self.layout:GetContentWidth() , self.layout:GetContentHeight() + (#lst * 140) )

    vessel:SetPosition(vessel:GetPositionX() , self.layout:GetContentHeight() - (vessel:GetContentHeight()/2) )

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_item_checked")
    local y = 0
    local heroLst = {}
    for i,v in ipairs(lst) do
    	if heroLst[i - 1] then
    		y = heroLst[i - 1]:GetPositionY() - ( heroLst[i - 1]:GetContentHeight() + 5 )
    	else
    		y = vessel:GetPositionY() - 110
    	end

    	--背景
    	heroLst[i] = Util:Sprite9( sp )
    	heroLst[i]:SetPosition( self.layout:GetContentWidth()/2 , y )
    	self.layout:AddChild(heroLst[i])

    	--图标
        local icon = LogicGUI.IconLayer.New(true)
        icon:SetAnchorPoint(0.5,0.5)
        icon:SetData({id = v.heroInfoId})
        icon:SetPosition( icon:GetNode():GetContentWidth()/2 + 15 , heroLst[i]:GetContentHeight()/2 + 10 ) 
        heroLst[i]:AddChild(icon:GetNode())

        local type_ , info = ObjIDTool.GetInfo( v.heroInfoId )
        --名字
		local name = Util:Name(info.name , nil , info.init_quality )
		name:SetPosition( icon:GetNode():GetPositionX() ,  icon:GetNode():GetPositionY() - icon:GetNode():GetContentHeight()/2 - 15 )
		name:SetAnchorPoint(0.5,0.5)
		heroLst[i]:AddChild(name)

		--宿命名
		local fateName = Util:Label( v.fateName ,Macros.TypefaceSize.normal , Macros.Color.keypoint ) 
		fateName:SetPosition( icon:GetNode():GetPositionX() + ( icon:GetNode():GetContentWidth()/2 ) + 10 , icon:GetNode():GetPositionY() + (icon:GetNode():GetContentHeight()/2) - (fateName:GetContentHeight()/2) - 5 )
		fateName:SetAnchorPoint(0 , 0.5)
		heroLst[i]:AddChild(fateName)	

		--宿命
		local fateDescribe = Util:Label( v.fateDescribe , Macros.TypefaceSize.slightly , Macros.Color.content , 255) 
		fateDescribe:SetPosition( icon:GetNode():GetPositionX() + ( icon:GetNode():GetContentWidth()/2 ) + 10 , icon:GetNode():GetPositionY() - (icon:GetNode():GetContentHeight()/2) + (fateDescribe:GetContentHeight()/2) + 20 )
		fateDescribe:SetAnchorPoint(0 , 1)
		heroLst[i]:AddChild(fateDescribe)		
    end



end