
--[[
%% @module: 装备精炼界面
%% @author: yjg
%% @created: 2016年11月5日16:58:56
--]]

Modules = Modules or {}

Modules.TreasureLiftRefineView = Modules.TreasureLiftRefineView or BaseClass(BaseTabView)

function Modules.TreasureLiftRefineView:__init()
end

--选项卡界面创建完成回调
function Modules.TreasureLiftRefineView:__LoadedCallback()
	self:__Getwidget()
	self:__ShowUI()
end

--选项卡界面打开回调
function Modules.TreasureLiftRefineView:__OpenCallback()
	self.faceEffects = {}
   	--调用普通
	self:__SetCommonUI()
	--调用属性
	self:__SetProperty()
	--一开始就要显示的材料
	self:UpdateMaterials()

    --精炼圣器
    self:BindGlobalEvent(Macros.Event.TreasureModule.TREASURE_REFINE,function (_, data)
		--调用属性
		self:__UpgradeProperty( data )
		--一开始就要显示的材料
		self:UpdateMaterials()

		self:__SetCommonUI()
    end)
end

--选项卡界面关闭回调
function Modules.TreasureLiftRefineView:__CloseCallback()
    if self.effect then
        self:__DelEffect(self.effect_id )
    end
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.TreasureLiftRefineView:__Dispose()
	for i=1,#self.faceEffects do
		if self.faceEffects[i] then
			self.faceEffects[i]:DeleteMe()
			self.faceEffects[i] = nil
		end
	end

end

--获得所有组件
function Modules.TreasureLiftRefineView:__Getwidget()
	--背景容器
	self.Panel_treasureRefine = self:GetWidget("Panel_treasureRefine") 
	--图标框
	self.Image_packBg_0 = self:GetWidget("Image_packBg_0") 
	--名字
	self.Text_name = self:GetWidget("Text_name") 
	--图标
	self.Panel_icon = self:GetWidget("Panel_icon") 
	--信息框
	self.Image_infoBg_0 = self:GetWidget("Image_infoBg_0") 
	--当前属性1
	self.Text_currentValue1 = self:GetWidget("Text_currentValue1") 
	--当前属性2
	self.Text_currentValue2 = self:GetWidget("Text_currentValue2") 
	--下级属性1
	self.Text_juniorValue1 = self:GetWidget("Text_juniorValue1") 
	--下级属性2
	self.Text_juniorValue2 = self:GetWidget("Text_juniorValue2") 
	--箭头
	self.Image_arrows = self:GetWidget("Image_arrows") 
	--当前
	self.Text_current = self:GetWidget("Text_current") 
	--下阶
	self.Text_junior = self:GetWidget("Text_junior") 
	--线2
	self.Image_1_0_0 = self:GetWidget("Image_1_0_0")
	--强化按钮 
	self.Button_intensify = Util:WidgetButton( 
												self:GetWidget("Button_intensify") , 
												CPPGameLib.GetString("equip_refine") , 
												CPPGameLib.Handler(self,self.OnButtonFntensify) ,
												nil,nil,Macros.Color.button_yellow
											)
	--价钱
	self.Image_price = self:GetWidget("Image_price") 
	--图标列表
	self.Panel_iconLst = self:GetWidget("Panel_iconlst")
    --飘字容器
    self.Panel_upgradeEffect = self:GetWidget("Panel_upgradeEffect")
end

function Modules.TreasureLiftRefineView:__ShowUI()
	-- 图标
  	local path = Resource.PathTool.GetBigIconPath(20101)
	self.icon = Util:Sprite(path)
    self.icon:SetPosition(self.Panel_icon:GetContentWidth()/2 ,self.Panel_icon:GetContentHeight()/2 - 10 )
    self.icon:SetAnchorPoint(0.5, 0.5 )
    self.Panel_icon:AddChild(self.icon)
    self.icon:SetScaleFactor( 0.8 )
    ActionManager.GetInstance():UpDownMoveAnimation(self.icon , 1 , 15)
    
    --名字
	self.name = Util:Name( "   " , Macros.TypefaceSize.largish )
	self.name:SetPosition( self.Text_name:GetContentWidth()/2 , self.Text_name:GetContentHeight()/2 )
	self.name:SetAnchorPoint(0.5,0.5)
	self.Text_name:AddChild(self.name)

	--当前属性
	self.nowCurrent = Util:Label( " " ,nil , Macros.Color.headline )
	self.nowCurrent:SetAnchorPoint(0.5,0.5)
	self.nowCurrent:SetPosition(self.Text_current:GetContentWidth() / 2 , self.Text_current:GetContentHeight()/2 )
	self.Text_current:AddChild(self.nowCurrent)

	--下阶属性
	self.nexCurrent = Util:Label( " " ,nil , Macros.Color.headline )
	self.nexCurrent:SetAnchorPoint(0.5,0.5)
	self.nexCurrent:SetPosition(self.Text_junior:GetContentWidth() / 2 , self.Text_junior:GetContentHeight()/2 )
	self.Text_junior:AddChild(self.nexCurrent)

	self.current = {}
	self.current.type = {}
	self.current.value = {}
	self.junior = {}
	self.junior.type = {}
	self.junior.value = {}
	for i=1,2 do
		self.current.type[i] = Util:Label(" ",Macros.TypefaceSize.slightly , Macros.Color.keypoint)
		self.current.type[i]:SetAnchorPoint(1,0.5)
		self.current.type[i]:SetPosition(self["Text_currentValue"..i]:GetContentWidth() - 10 , self["Text_currentValue"..i]:GetContentHeight()/2 )
		self["Text_currentValue"..i]:AddChild(self.current.type[i])

		self.current.value[i] = Util:Label(" ",Macros.TypefaceSize.slightly, Macros.Color.content)
		self.current.value[i]:SetAnchorPoint(0,0.5)
		self.current.value[i]:SetPosition(10 , self["Text_currentValue"..i]:GetContentHeight()/2 )
		self["Text_currentValue"..i]:AddChild(self.current.value[i])

		self.junior.type[i] = Util:Label(" ",Macros.TypefaceSize.slightly , Macros.Color.keypoint)
		self.junior.type[i]:SetAnchorPoint(1,0.5)
		self.junior.type[i]:SetPosition(self["Text_juniorValue"..i]:GetContentWidth() - 10 , self["Text_juniorValue"..i]:GetContentHeight()/2 )
		self["Text_juniorValue"..i]:AddChild(self.junior.type[i])

		self.junior.value[i] = Util:Label(" ",Macros.TypefaceSize.slightly , Macros.Color.front_color)
		self.junior.value[i]:SetAnchorPoint(0,0.5)
		self.junior.value[i]:SetPosition(10 , self["Text_juniorValue"..i]:GetContentHeight()/2 )
		self["Text_juniorValue"..i]:AddChild(self.junior.value[i])		

	end
     --货币图标
   	self.icon_cost_type = Util:MoneyControl(Macros.Game.RolePropType.GOLD , 0.8 )
	self.icon_cost_type:SetAnchorPoint(0,0.5)
	self.icon_cost_type:SetPosition(0,self.Image_price:GetContentHeight()/2)
	self.Image_price:AddChild(self.icon_cost_type)

	--价格字体
	self.priceValue = Util:Label( "  " )
	self.priceValue:SetAnchorPoint(0.5 , 0.5)
	self.priceValue:SetPosition(self.Image_price:GetContentWidth()/2 ,self.Image_price:GetContentHeight()/2)
	self.Image_price:AddChild(self.priceValue)

    --提示
    self.hero_train_level_no_ = Util:Label( CPPGameLib.GetString("hero_train_level_no_") , Macros.TypefaceSize.slightly , Macros.Color.red_color )
    self.hero_train_level_no_:SetAnchorPoint(0.5,0.5)
    self.hero_train_level_no_:SetPosition( self.Image_price:GetPositionX() , self.Image_price:GetPositionY() )
    self.Image_infoBg_0:AddChild(self.hero_train_level_no_)
    self.hero_train_level_no_:SetVisible(false)

end

--更新界面
function Modules.TreasureLiftRefineView:__SetShowUI( type_ )
	self:UpdateMaterials()
	if not type_ then
		self:__SetCommonUI()
		self:__SetProperty()
	end
end

--不需要动的UI
function Modules.TreasureLiftRefineView:__SetCommonUI(  )
	--获得选中的道具的信息
	self.onTreasureInfo = CallModuleFunc(ModuleType.TREASURE, "GetOnTreasureInfo")

	local cnf = CPPGameLib.GetConfig( "TreasureRefine" , self.onTreasureInfo.quality )
	local level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)

	if cnf.level_list[self.onTreasureInfo.refine_level].need_level > level then
		self.hero_train_level_no_:SetString( CPPGameLib.GetString("hero_train_level_no_" , cnf.level_list[self.onTreasureInfo.refine_level].need_level ) )
		self.hero_train_level_no_:SetVisible(true)
		self.Image_price:SetVisible(false)
	else
		self.Image_price:SetVisible(true)
		self.hero_train_level_no_:SetVisible(false)
	end

	--名字
	local refine_level = ""
	if self.onTreasureInfo.refine_level ~= 0 then
		refine_level = " +"..self.onTreasureInfo.refine_level
	end
	self.name:SetString( self.onTreasureInfo.name .. refine_level )
	self.name:SetColor(unpack(Macros.Quality[self.onTreasureInfo.quality]))
	self.name:SetStrokeColor(unpack(Macros.Quality_Stroke[self.onTreasureInfo.quality]))

	--设置图标
	self.icon:SetImage(Resource.PathTool.GetBigIconPath(self.onTreasureInfo.info_id))

	--背光
	if self.iconLed then
		self:__DelEffect(self.iconId )
	end
	self.iconId , self.iconLed = TypefaceEffects.New():Spotlight( self , self.Panel_icon , self.onTreasureInfo.quality )
	self.iconLed:SetPosition( self.Panel_icon:GetContentWidth()/2 , -215 )
	self.iconLed:SetScaleFactor(0.8)
	
	--当前阶级
	self.nowCurrent:SetString( CPPGameLib.GetString("equip_nowRefineLv" , self.onTreasureInfo.refine_level ) )

	--下阶阶级
	self.nexCurrent:SetString( CPPGameLib.GetString("equip_nexRefineLv" , self.onTreasureInfo.refine_level + 1 ) )
end

--属性
function Modules.TreasureLiftRefineView:__SetProperty( )
	-- 	  当前的属性   --上次的属性 	--下次的属性
	local nowProperty , onceProperty , nexProperty = CallModuleFunc(ModuleType.TREASURE, "GetRefinePropPlusMap" , self.onTreasureInfo.uid )
	--显示当前的属性
	for i,v in ipairs(nowProperty.lst) do
		local nowType , nowValue = Util:Nature( v.type , v.value , true )
		--类型
		self.current.type[i]:SetString( nowType )
		--当前值
		self.current.value[i]:SetString( nowValue )
	end

	--如果这两个等级是一样的话
	if nowProperty.level == nexProperty.level then
		for i=1 , #self.junior.type do
			self.junior.type[i]:SetString( "  " )
			self.junior.value[i]:SetString( CPPGameLib.GetString("equip_refineMax") )
			self.junior.value[i]:SetAnchorPoint(0.5,0.5)

			self.junior.value[i]:SetPosition( self["Text_juniorValue"..i]:GetContentWidth()/2  , self["Text_juniorValue"..i]:GetContentHeight()/2 )
		end
	else
		--显示当前的属性
		for i,v in ipairs(nexProperty.lst) do
			local nowType , nowValue = Util:Nature( v.type , v.value , true )
			--类型
			self.junior.type[i]:SetString( nowType )
			--当前值
			self.junior.value[i]:SetString( nowValue )

			self.junior.type[i]:SetAnchorPoint(1,0.5)
			self.junior.type[i]:SetPosition(self["Text_juniorValue"..i]:GetContentWidth() - 10 , self["Text_juniorValue"..i]:GetContentHeight()/2 )
			

			self.junior.value[i]:SetAnchorPoint(0,0.5)
			self.junior.value[i]:SetPosition(10 , self["Text_juniorValue"..i]:GetContentHeight()/2 )

		end
	end
end

--设置属性
function Modules.TreasureLiftRefineView:__UpgradeProperty( data )
	self:Effect( data )
	LogicGUI.TempMonster.DisposeData( data , "treasureRefine" )
    
    --特效
    local pos = {
    	[1] = { x = self.Text_current:GetPositionX() , y = self.Text_current:GetPositionY() },
        [2] = { x = self.Text_currentValue1:GetPositionX() , y = self.Text_currentValue1:GetPositionY() },
        [3] = { x = self.Text_currentValue2:GetPositionX() , y = self.Text_currentValue2:GetPositionY() }
    }
	-- 当前的属性   --上次的属性 	--下次的属性
	local nowProperty , onceProperty , nexProperty = CallModuleFunc(ModuleType.TREASURE, "GetRefinePropPlusMap" , self.onTreasureInfo.uid )
	CPPActionManager.ScaleTo(self.nowCurrent, 0 , 1)
	local t1 = LogicGUI.TreasureRefineFontEffects:GetTreasureRefineTab( data , pos )
   	local windwords_effect = WindWordsEffects.New()
    windwords_effect:Subsection( self.__root_widget , self.Image_infoBg_0 , t1 , 10 ,function ( ... )
		for i,v in ipairs(nowProperty.lst) do
			local nowType , nowValue = Util:Nature( v.type , v.value , true )
			if not self.faceEffects[i] then
				self.faceEffects[i] = TypefaceEffects.New()
			end
			self.faceEffects[i]:Roll( self.current.value[i] , nowValue , nil , function ( ... )
				CPPActionManager.ScaleTo(self.nowCurrent, 0.2 , 1 )
				self.nowCurrent:SetString( CPPGameLib.GetString("equip_nowRefineLv",nowProperty.level ))
			end)
			CPPActionManager.ScaleTo(self.nowCurrent, 0.2 , 1.6)

			self.nexCurrent:SetString( CPPGameLib.GetString("equip_nexRefineLv",nowProperty.level + 1))
		end

		--如果这两个等级是一样的话
		if nowProperty.level == nexProperty.level then
			for i=1 , #self.junior.type do
				self.junior.type[i]:SetString( "  " )
				self.junior.value[i]:SetString( CPPGameLib.GetString("equip_refineMax") )

				self.junior.value[i]:SetPosition( self["Text_juniorValue"..i]:GetContentWidth()/2  , self["Text_juniorValue"..i]:GetContentHeight()/2 )
			end
		else
			--显示当前的属性
			for i,v in ipairs(nexProperty.lst) do
				local nowType , nowValue = Util:Nature( v.type , v.value , true )
				--类型
				self.junior.type[i]:SetString( nowType )
				--当前值
				self.junior.value[i]:SetString( nowValue )

				self.junior.type[i]:SetAnchorPoint(1,0.5)
				self.junior.type[i]:SetPosition(self["Text_juniorValue"..i]:GetContentWidth() - 10 , self["Text_juniorValue"..i]:GetContentHeight()/2 )
				

				self.junior.value[i]:SetAnchorPoint(0,0.5)
				self.junior.value[i]:SetPosition(10 , self["Text_juniorValue"..i]:GetContentHeight()/2 )

			end
		end
    end)
end

--设置预览 材料
function Modules.TreasureLiftRefineView:UpdateMaterials( )
	--每次都删掉整个材料面板
	if self.layer then
		self.layer:RemoveSelf(true)
		self.layer = nil
	end

	--居中层
    self.layer = Util:Layout((self.Image_1_0_0:GetContentWidth()/2) * 2 , 130 )
    self.layer:SetAnchorPoint(0.5,0.5)
    self.layer:SetPosition(self.Panel_iconLst:GetContentWidth()/2 , self.Panel_iconLst:GetContentHeight()/2)
    self.Panel_iconLst:AddChild(self.layer)

    --道具
    local icon = {}
    icon[1] = LogicGUI.IconLayer.New(false,true)
    icon[1]:SetPosition( 5 ,self.layer:GetContentHeight()/2 ) 
    icon[1]:SetAnchorPoint(0,0.5)
    self.layer:AddChild(icon[1]:GetNode())
    
	--自己
	icon[2] = LogicGUI.IconLayer.New(false,true)
	icon[2]:SetPosition( self.layer:GetContentWidth()/2 + 5 ,self.layer:GetContentHeight()/2 ) 
	icon[2]:SetAnchorPoint(0,0.5)
	self.layer:AddChild(icon[2]:GetNode())

    --材料列表
    self.packLst = CallModuleFunc(ModuleType.TREASURE, "GetTreasureRefineMaterialList")

	local key = false
	--数量
	if self.packLst and #self.packLst ~= 0 then
		for i,v in ipairs(self.packLst) do
			if v.value ~= 0 then
				key = true
			end
		end
	end
	self.Button_intensify:SetEnabled(key)

    --设置材料列表
    for i=1,2 do
    	local key = i
    	--道具数据
    	local type_ , cnfInfo = ObjIDTool.GetInfo( self.packLst[i].id )
	    --如果是道具
	    local color = Macros.Color.green
	    if type_ == Macros.Game.ObjType.ITEM then

			if self.packLst[i].own < self.packLst[i].value then
				color = Macros.Color.red
			end

	    elseif type_ == Macros.Game.ObjType.TREASURE then

			if self.packLst[i].own < self.packLst[i].value then
				color = Macros.Color.red
			end
			
	    end

	    -- 2017年6月8日20:52:08
	    -- 1879 圣器精炼石没有获取引导

    	if self.packLst[ i ].value ~= 0 then
	    	if self.packLst[ i - 1 ] and self.packLst[ i - 1].value == 0 then
	    		key = i - 1
	    	end
	    	icon[key]:SetData({ id = self.packLst[i].id })
	    	-- if key == 1 then
			    icon[key]:SetTouchCallback(function ()
			    	GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, self.packLst[i].id)
			    end)
		    -- end
		    --道具名
		    local name = Util:Name(cnfInfo.name , Macros.TypefaceSize.slightly , cnfInfo.quality )
		    name:SetAnchorPoint(0,1)
		    name:SetPosition(icon[key]:GetNode():GetContentWidth() + icon[key]:GetNode():GetPositionX() + 5 , icon[key]:GetNode():GetContentHeight()/2 + icon[key]:GetNode():GetPositionY() - 5 )
		    self.layer:AddChild(name)
		    local tab = Util:LabCenter( { 
		    								{ zi = self.packLst[i].own , ys = color , dx = Macros.TypefaceSize.slightly },
		    								{ zi = "/" .. self.packLst[i].value , ys = Macros.Color.content , dx = Macros.TypefaceSize.slightly },
									     } )
		    tab:SetAnchorPoint(0 , 0.5)
		    tab:SetPosition(name:GetPositionX() , icon[key]:GetNode():GetPositionY() - icon[key]:GetNode():GetContentHeight()/2 + 10)
		    self.layer:AddChild(tab)
    	end
    end

	--找到价格的种类
	local sp = Util:GetMoneyIconImgData( self.packLst[3].id )
	--重新设置价格图标
	self.icon_cost_type:SetImage(sp)
	--重新设置价格
	self.priceValue:SetString( self.packLst[3].value )
end

--发送协议
function Modules.TreasureLiftRefineView:OnButtonFntensify()
	local text = {
			[1] = "common_material_not_enough",
			[2] = "common_material_not_enough",
			[3] = "currency_insufficient_"..self.packLst[3].id
	}
	local t1 = {}
	for i,v in ipairs(self.packLst) do
		if v.value ~= 0 and v.value > v.own then
			table.insert( t1 , text[i] )
		end
	end

	local cnf = CPPGameLib.GetConfig( "TreasureRefine" , self.onTreasureInfo.quality )
	local level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)

	if #t1 ~= 0 then
		if t1[1] == "currency_insufficient_"..self.packLst[3].id then
			CallModuleFunc(ModuleType.SUNDRY, "ToIdGetPath", Macros.Game.RolePropType.GOLD )
		end
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString( t1[1] ) )

	elseif cnf.level_list[self.onTreasureInfo.refine_level].need_level > level then
		
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("hero_train_level_no") )
		return
	else
		local info = {}
		info.treasure_uid = self.onTreasureInfo.uid
		CallModuleFunc(ModuleType.TREASURE , "TreasureRefineReq",info)
	end
end

function Modules.TreasureLiftRefineView:Effect( data )
	-- local effect_id , effect_con = self:__CreateEffect({id = 100009 , loop = false }, self.Panel_icon)
	-- effect_con:SetPosition( self.icon:GetPositionX(), self.icon:GetPositionY() )
	-- CPPActionManager.DelayTo(self.icon , 0.2 , func)

-- local effect_id,effect = self:__CreateEffect({id = 100009 ,loop = false},self.Panel_upgradeEffect, 2)
--     effect:SetAnchorPoint(0.5,0)
--     PosTool.Center(effect,0,20)
end