
--[[
%% @module: 圣器强化界面
%% @author: yjg
%% @created: 2016年11月5日16:58:56
--]]

Modules = Modules or {}

Modules.TreasureLiftIntensifyTabView = Modules.TreasureLiftIntensifyTabView or BaseClass(BaseTabView)

function Modules.TreasureLiftIntensifyTabView:__init()
end

--选项卡界面创建完成回调
function Modules.TreasureLiftIntensifyTabView:__LoadedCallback()
	self.__material_icon = {}
	self.__material_btn = {}
	self:__Getwidget()
	self:__ShowUI()
end

--选项卡界面打开回调
function Modules.TreasureLiftIntensifyTabView:__OpenCallback()
	self.faceEffects = {}
   	--调用普通
	self:__SetCommonUI()
	--调用属性
	self:__SetProperty()
	--调用进度条
	self:__SetWhen()

    --请求强化回调
    self:BindGlobalEvent(Macros.Event.TreasureModule.TREASURE_INTENSIFY,function (_,data)
		self:__SetCommonUI()
		self:__UpgradeProperty(data)
		self:UpdateMaterials(true)
		self:PreviewConformity()
    end)

    --选择材料回调
    self:BindGlobalEvent(Macros.Event.TreasureModule.TREASURE_SELECT_TAB,function (_,data)
		--预览处理
		self:PreviewConformity()
	    self:UpdateMaterials()
    end)

end

--选项卡界面关闭回调
function Modules.TreasureLiftIntensifyTabView:__CloseCallback()
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.TreasureLiftIntensifyTabView:__Dispose()
	for i=1,#self.faceEffects do
		if self.faceEffects[i] then
			self.faceEffects[i]:DeleteMe()
			self.faceEffects[i] = nil
		end
	end

	if self.faceEffects_1 then
		self.faceEffects_1:DeleteMe()
		self.faceEffects_1 = nil
	end
end

--获得所有组件
function Modules.TreasureLiftIntensifyTabView:__Getwidget()
	--背景容器
	self.Panel_treasureIntensify = self:GetWidget("Panel_treasureIntensify") 
	--信息面板
	self.Image_infoBg = self:GetWidget("Image_infoBg") 
	--添加按钮
	self.Button_IntensifyAdd =  Util:WidgetButton( 
													self:GetWidget("Button_IntensifyAdd") , 
													CPPGameLib.GetString("info_intensify") , 
													CPPGameLib.Handler(self,self.OnButtonIntensifyAdd),
													nil,nil,Macros.Color.button_yellow 
													)
	--价钱背景
	self.Image_2_0 = self:GetWidget("Image_2_0") 
	--线
	self.Image_wire = self:GetWidget("Image_wire") 
	-- --强化等级
	-- self.Text_IntensifyLv = self:GetWidget("Text_IntensifyLv") 
	--进度条背景
	self.Panel_barBg = self:GetWidget("Panel_barBg") 
	--类型0
	self.Image_type0 = self:GetWidget("Image_type0")
	--类型1
	self.Image_type1 = self:GetWidget("Image_type1")
	--类型2 
	self.Image_type2 = self:GetWidget("Image_type2") 
	--值0
	self.Image_value0 = self:GetWidget("Image_value0") 
	--值1
	self.Image_value1 = self:GetWidget("Image_value1") 
	--值2
	self.Image_value2 = self:GetWidget("Image_value2") 
	--图标列表
	self.Panel_iconLst = self:GetWidget("Panel_iconLst")
	--图标背景
	self.Image_packBg = self:GetWidget("Image_packBg") 
	--名称
	self.Text_name = self:GetWidget("Text_name") 
	--图标
	self.Panel_icon = self:GetWidget("Panel_icon") 
    --飘字容器
    self.Panel_upgradeEffect = self:GetWidget("Panel_upgradeEffect")
end

function Modules.TreasureLiftIntensifyTabView:__ShowUI()
	-- 图标
  	local path = Resource.PathTool.GetBigIconPath(20101)
	self.icon = Util:Sprite(path)
    self.icon:SetPosition(self.Panel_icon:GetContentWidth()/2 ,self.Panel_icon:GetContentHeight()/2 - 15 )
    self.icon:SetAnchorPoint(0.5, 0.5 )
    self.Panel_icon:AddChild(self.icon)
    self.icon:SetScaleFactor( 0.8 )
    ActionManager.GetInstance():UpDownMoveAnimation(self.icon , 1 , 15)

    --名字
	self.name = Util:Name( "   ", Macros.TypefaceSize.largish)
	self.name:SetPosition( self.Text_name:GetContentWidth()/2 , self.Text_name:GetContentHeight()/2 )
	self.name:SetAnchorPoint(0.5,0.5)
	self.Text_name:AddChild(self.name)

    self.intensify = {}
    self.intensify.type = {}
    self.intensify.value = {}
    self.intensify.addValue = {}

    for i=1,2 do
    	--类型
    	self.intensify.type[i] = Util:Label(" ",Macros.TypefaceSize.normal , Macros.Color.keypoint )
    	self.intensify.type[i]:SetAnchorPoint(0.5,0.5)
    	self.intensify.type[i]:SetPosition(self["Image_type"..i]:GetContentWidth()/2 , self["Image_type"..i]:GetContentHeight()/2 )
    	self["Image_type"..i]:AddChild(self.intensify.type[i])
    	--值
      	self.intensify.value[i] = Util:Label(" ",Macros.TypefaceSize.normal , Macros.Color.content )
    	self.intensify.value[i]:SetAnchorPoint(0,0.5)
    	self.intensify.value[i]:SetPosition( 10 , self["Image_value"..i]:GetContentHeight()/2 )
    	self["Image_value"..i]:AddChild(self.intensify.value[i])
    	--增加值
      	self.intensify.addValue[i] = Util:Label(" ",Macros.TypefaceSize.normal,Macros.Color.front_color)
    	self.intensify.addValue[i]:SetAnchorPoint(0,0.5)
    	self.intensify.addValue[i]:SetPosition( self["Image_value"..i]:GetContentWidth()/2 , self["Image_value"..i]:GetContentHeight()/2 )
    	-- self.intensify.addValue[i]:SetVisible(false)
    	self["Image_value"..i]:AddChild(self.intensify.addValue[i])  
		ActionManager.GetInstance():FadeAnimation( self.intensify.addValue[i] , 0.5 )
    end
	--强化等级
	self.intensifyLv = {}

	self.intensifyLv.type = Util:Label( CPPGameLib.GetString("equip_Lv") ,Macros.TypefaceSize.normal , Macros.Color.keypoint)
	self.intensifyLv.type:SetAnchorPoint( 0.5 , 0.5 )
	self.intensifyLv.type:SetPosition( self.Image_type0:GetContentWidth()/2 , self.Image_type0:GetContentHeight()/2 )
    self.Image_type0:AddChild(self.intensifyLv.type)	

	self.intensifyLv.now = Util:Label(" ",Macros.TypefaceSize.normal , Macros.Color.content)
	self.intensifyLv.now:SetAnchorPoint(0,0.5)
	self.intensifyLv.now:SetPosition( 10 , self.Image_value0:GetContentHeight()/2 )
    self.Image_value0:AddChild(self.intensifyLv.now)

	self.intensifyLv.junior = Util:Label(" " , Macros.TypefaceSize.normal , Macros.Color.front_color)
	self.intensifyLv.junior:SetAnchorPoint(0,0.5)
	self.intensifyLv.junior:SetPosition( self.Image_value0:GetContentWidth()/2 , self.Image_value0:GetContentHeight()/2 )
    self.Image_value0:AddChild(self.intensifyLv.junior)
    ActionManager.GetInstance():FadeAnimation( self.intensifyLv.junior , 0.5 )

    --进度条
	local imgdata_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_preview")
	local imgdata_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_bar_interior_bg1")

    self.__bar_exp = GUI.ProgressBar9Layer.New(imgdata_bar, imgdata_bg, self.Image_wire:GetContentWidth() - 5,18, self.Image_wire:GetContentWidth(),23 , 0, 100)
    self.__bar_exp:SetAnchorPoint(0.5, 0.5)
    self.Panel_barBg:AddChild(self.__bar_exp:GetNode())
    PosTool.Center(self.__bar_exp)

    --进度条值
    self.barValue = Util:Label("00/00" , nil , Macros.Color.lilyWhite)
    self.barValue:SetAnchorPoint(0.5,0.5)
    self.barValue:SetPosition(self.__bar_exp:GetNode():GetPositionX() , self.__bar_exp:GetNode():GetPositionY() )
    self.Panel_barBg:AddChild(self.barValue)

    self.layou = Util:Layout(82 * 5 ,130)
    self.layou:SetPosition(self.Panel_iconLst:GetContentWidth()/2 , self.Panel_iconLst:GetContentHeight()/2 )
    self.layou:SetAnchorPoint(0.5,0.5)
    self.Panel_iconLst:AddChild(self.layou)

    local interval = self.layou:GetContentWidth() / 5
    local imgdata_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_item_treasure")
    local addicon = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_num_add_big")
    self.addIcon = {}
    self.addIconBg = {}
    for i=1,5 do
    	self.addIconBg[i] = Util:Sprite(imgdata_bar)
    	self.addIconBg[i]:SetAnchorPoint( 0.5 , 0.5)
    	self.addIconBg[i]:SetPosition( 41 + interval * (i-1) , self.layou:GetContentHeight()/2 + 10)
    	self.layou:AddChild(self.addIconBg[i] , 5 - i )  

    	self.addIcon[i] = Util:Button(addicon)
    	self.addIcon[i]:SetPosition(self.addIconBg[i]:GetContentWidth()/2 , self.addIconBg[i]:GetContentHeight()/2)
    	self.addIcon[i]:SetAnchorPoint(0.5,0.5)
    	self.addIconBg[i]:AddChild(self.addIcon[i])
    	WidgetUnity.SetWidgetListener(self.addIcon[i],nil,nil,function ( ... )
    		self:OnAddIcon(i)
    	end)
    end

    --货币图标
   	self.icon_cost_type = Util:MoneyControl(Macros.Game.RolePropType.GOLD , 0.8 )
	self.icon_cost_type:SetAnchorPoint(0,0.5)
	self.icon_cost_type:SetPosition(0,self.Image_2_0:GetContentHeight()/2)
	self.Image_2_0:AddChild(self.icon_cost_type)

	--价格字体
	self.priceValue = Util:Label( "  " )
	self.priceValue:SetAnchorPoint(0.5 , 0.5)
	self.priceValue:SetPosition(self.Image_2_0:GetContentWidth()/2 , self.Image_2_0:GetContentHeight()/2)
	self.Image_2_0:AddChild(self.priceValue)

    --提示
    self.hero_train_level_no_ = Util:Label( CPPGameLib.GetString("hero_train_level_no_") , Macros.TypefaceSize.slightly , Macros.Color.red_color )
    self.hero_train_level_no_:SetAnchorPoint(0.5,0.5)
    self.hero_train_level_no_:SetPosition( self.Image_2_0:GetPositionX() , self.Image_2_0:GetPositionY() )
    self.Image_infoBg:AddChild(self.hero_train_level_no_)
    self.hero_train_level_no_:SetVisible(false)

    --自动添加
    local btn_btn_green = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_green")
    self.auto_button = Util:Button( btn_btn_green , function ( )
    	self:AutoButtonAdd()
    end, CPPGameLib.GetString("hero_upgrade_btn") , Macros.TypefaceSize.popup , nil , Macros.Color.button_yellow )
    self.auto_button:SetAnchorPoint(0.5,0.5)
    self.Image_infoBg:AddChild( self.auto_button )
    self.auto_button:SetPosition( self.Image_infoBg:GetContentWidth()/2 - 100 , self.Button_IntensifyAdd:GetPositionY() )
    self.auto_button:SetVisible( false )

    local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
    if role_level >= ConfigAdapter.Common.GetTreasureOneKeyIntensify() then
    	self.Button_IntensifyAdd:SetText( CPPGameLib.GetString("hero_upgrade_onekey_levelup_btn") )
    	WidgetUnity.SetWidgetListener(self.Button_IntensifyAdd, nil, nil, function()
			self:OneKeyUpgrade()
		end)

    else
    	self.Button_IntensifyAdd:SetPosition( self.Image_infoBg:GetContentWidth()/2 + 100 , self.Button_IntensifyAdd:GetPositionY() )
    	self.Button_IntensifyAdd:SetText( CPPGameLib.GetString("info_intensify") )
		WidgetUnity.SetWidgetListener(self.Button_IntensifyAdd, nil, nil, function()
			self:OnButtonIntensifyAdd()
		end)
		self.auto_button:SetVisible( true )

    end
end

--刷新的UI
function Modules.TreasureLiftIntensifyTabView:__SetShowUI( type_ )
    self:__SetCommonUI()
	self:PreviewConformity()
	
	if not type_ then
		self:__SetProperty()

		for _, material_icon in pairs(self.__material_icon) do
			material_icon:DeleteMe()
		end
		self.__material_icon = {}
	end
end

--不需要动的UI
function Modules.TreasureLiftIntensifyTabView:__SetCommonUI(  )
	--获得选中的道具的信息
	self.onTreasureInfo = CallModuleFunc(ModuleType.TREASURE, "GetOnTreasureInfo")

	local cnf = CPPGameLib.GetConfig( "TreasureStrengthen" , self.onTreasureInfo.quality )
	local level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)

	if cnf.level_list[self.onTreasureInfo.strengthen_level].need_level > level then
		self.hero_train_level_no_:SetString( CPPGameLib.GetString("hero_train_level_no_" , cnf.level_list[self.onTreasureInfo.strengthen_level].need_level ) )
		self.hero_train_level_no_:SetVisible(true)
		self.Image_2_0:SetVisible(false)
	else
		self.Image_2_0:SetVisible(true)
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

    local lst = CallModuleFunc(ModuleType.TREASURE, "GetMaxStrengthenInfo" , self.onTreasureInfo.quality )
	--当前等级
	self.intensifyLv.now:SetString( self.onTreasureInfo.strengthen_level .. " / " .. lst.strengthen_level )

	PosTool.RightTo( self.intensifyLv.now , self.intensifyLv.text , 5)
end

--属性
function Modules.TreasureLiftIntensifyTabView:__SetProperty( )
	-- 	  当前的属性   --上次的属性 	--下次的属性
	local nowProperty , onceProperty , nexProperty = CallModuleFunc(ModuleType.TREASURE, "GetStrengthenPropPlusMap" , self.onTreasureInfo.uid )
	--显示当前的属性
	for i,v in ipairs(nowProperty.lst) do
		local nowType , nowValue = Util:Nature( v.type , v.value , true , true)
		--类型
		self.intensify.type[i]:SetString( nowType )
		--当前值
		self.intensify.value[i]:SetString( nowValue )

		if nowProperty.level == nexProperty.level then
			self.intensify.addValue[i]:SetString( CPPGameLib.GetString("equip_intensifyMax") )
		end
	end
end

--升级属性
function Modules.TreasureLiftIntensifyTabView:__UpgradeProperty( data )
	if data.old_level == data.new_level then
		self.dataExp = data.exp
		return 
	end
	LogicGUI.TempMonster.DisposeData( data , "treasureStrengthen" )

	self.dataExp = nil

    local lst = CallModuleFunc(ModuleType.TREASURE, "GetMaxStrengthenInfo" , self.onTreasureInfo.quality )
	--当前等级
	self.intensifyLv.now:SetString( data.old_level .. " / " .. lst.strengthen_level )

	PosTool.RightTo( self.intensifyLv.now , self.intensifyLv.text , 5 )

    --特效
    local pos = {
        [1] = { x = self.Image_value0:GetPositionX() , y = self.Image_value0:GetPositionY() },
        [2] = { x = self.Image_value1:GetPositionX() , y = self.Image_value1:GetPositionY() },
        [3] = { x = self.Image_value2:GetPositionX() , y = self.Image_value2:GetPositionY() }
    }
	local t1 = LogicGUI.TreasureIntensifyFontEffects:GetTreasureIntensifyTab( data , pos )
	-- 	  当前的属性   --上次的属性 	--下次的属性
	local nowProperty , onceProperty , nexProperty = CallModuleFunc(ModuleType.TREASURE, "GetStrengthenPropPlusMap" , self.onTreasureInfo.uid )
   	local windwords_effect = WindWordsEffects.New()
    windwords_effect:Subsection( self.__root_widget , self.Image_infoBg , t1 , 10 ,function ( ... )

		for i,v in ipairs(nowProperty.lst) do
			local nowType , nowValue = Util:Nature( v.type , v.value , true , true )
			if not self.faceEffects[i] then
				self.faceEffects[i] = TypefaceEffects.New()
			end
			self.faceEffects[i]:Roll( self.intensify.value[i] , nowValue , nil , function ( ... )
			end)
		end

		if not self.faceEffects_1 then
			self.faceEffects_1 = TypefaceEffects.New()
		end
		self.faceEffects_1:Roll( self.intensifyLv.now , data.new_level , " / " .. lst.strengthen_level , function ( ... )
			PosTool.RightTo( self.intensifyLv.now , self.intensifyLv.text , 5)
		end )

    end)
end

--预览整合
function Modules.TreasureLiftIntensifyTabView:PreviewConformity( ... )
    self:__SetWhen()
	self:SetPreviewProperty()
end

--设置进度条 包括预览 还有价钱
function Modules.TreasureLiftIntensifyTabView:__SetWhen( ... )
	-- 材料数据整理
    -- 当前经验 上限经验 , 差额 , 吃完的等级 , 材料给的经验
	local onInfoExp , nowInfoMaxExp , margin , upLevel , dogFoodExp = CallModuleFunc(ModuleType.TREASURE, "CalculateUpgrade")
	
	--等级到了最高
	if onInfoExp == false then
		self.__bar_exp:SetProgress( 100 )
		self.barValue:SetString( CPPGameLib.GetString("equip_toMax") )
	else
		--设置进度条
		self.__bar_exp:SetProgress( self.onTreasureInfo.strengthen_exp / nowInfoMaxExp * 100 )
		--设置进度条上的字
		self.barValue:SetString( self.onTreasureInfo.strengthen_exp + dogFoodExp .. "/" .. nowInfoMaxExp )
	end

	--预览的进度条
	local preview_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_bar_bar_preview")
	self.__bar_exp:StartPreviewEffect( (self.onTreasureInfo.strengthen_exp + dogFoodExp )  / nowInfoMaxExp * 100 , preview_bar )
	
	--材料列表
	local selected_materials_map = CallModuleFunc(ModuleType.TREASURE, "GetSelectedStrengthenMaterialMap")
	local _, gold = CallModuleFunc(ModuleType.TREASURE, "GetSelectedStrengthenMaterialInfo")

	local _, selected_material = next(selected_materials_map)
	if selected_material ~= nil then
		--找到价格的种类
		local sp = Util:GetMoneyIconImgData( selected_material.cost_type )
		--重新设置价格图标
		self.icon_cost_type:SetImage(sp)

    	self.Button_IntensifyAdd:SetText( CPPGameLib.GetString("info_intensify") )
		WidgetUnity.SetWidgetListener(self.Button_IntensifyAdd, nil, nil, function()
			self:OnButtonIntensifyAdd()
		end)
	else
	    local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	    if role_level >= ConfigAdapter.Common.GetTreasureOneKeyIntensify() then
	    	self.Button_IntensifyAdd:SetText( CPPGameLib.GetString("hero_upgrade_onekey_levelup_btn") )
	    	WidgetUnity.SetWidgetListener(self.Button_IntensifyAdd, nil, nil, function()
				self:OneKeyUpgrade()
			end)
	    else
	    	self.Button_IntensifyAdd:SetText( CPPGameLib.GetString("info_intensify") )
			WidgetUnity.SetWidgetListener(self.Button_IntensifyAdd, nil, nil, function()
				self:OnButtonIntensifyAdd()
			end)
	    end
	end

	self.priceValue:SetString( gold )
end

--更新材料列表
function Modules.TreasureLiftIntensifyTabView:UpdateMaterials( is_intensify_success )
	--清空已选材料
	for uid, material_icon in pairs(self.__material_icon) do
		--强化成功，吞噬材料动画
		if is_intensify_success then
			local function moveAnimate()
				material_icon:DeleteMe()

				local r, g, b, a = unpack(Macros.Color.button_yellow)

				self.Button_IntensifyAdd:SetOutline(r, g, b, a, 2 )
				self.Button_IntensifyAdd:SetEnabled( true )

				if self.dataExp then
					--飘增加的经验
					local lab = TypefaceEffects:LittleFloat( string.format( Macros.Color.green_stroke_rt , "+" .. self.dataExp )  , nil , 1 , 0 , 40)
					lab:SetPosition(self.Panel_barBg:GetContentWidth()/2 , self.Panel_barBg:GetContentHeight()/2 )
					self.Panel_barBg:AddChild(lab)

					--飘字
					local lab = Util:LabCenter( {
													{ zi =  CPPGameLib.GetString("treasure_intensifyExp") , ys = 0 , dx = Macros.TypefaceSize.popup , mb = true },
													{ zi =  self.dataExp , ys = 100 , dx = Macros.TypefaceSize.popup , mb = true },
												} )
					self:AddChild(lab)
					PosTool.Center(lab)
					ActionManager.GetInstance():WindWordsUpslopeEffect(lab,  COCOPoint( lab:GetPositionX() , lab:GetPositionY() + 10 ) )
				end
			end

			self.Button_IntensifyAdd:SetText(" ")
			Util:GreyButton( self.Button_IntensifyAdd )

			local effect_id,effect = self:__CreateEffect({
				id = 100015, loop = false, time_scale = 2, 
				end_callback = function()
					local new_pos = NodeUnity.ConvertPosBetweenTwoContainer(self.Image_infoBg, material_icon:GetNode(), COCOPoint(self.Image_infoBg:GetContentWidth()/2 , self.Panel_barBg:GetPositionY() - 10 ))				
					ActionManager:RunScaleToAndMoveToEffect(material_icon:GetNode() , 
															0.3 , new_pos.x , new_pos.y , 
															nil , GlobalCallbackMgr:AddCallBackFunc(moveAnimate)) 
				end}, 
				material_icon:GetNode():GetParent(), 2)

			PosTool.Center(effect)
		else
			material_icon:DeleteMe()
		end
	end
	self.__material_icon = {}

	--更新已选材料
	local material_index = 1
	local selected_materials_map = CallModuleFunc(ModuleType.TREASURE, "GetSelectedStrengthenMaterialMap")	
	local addicon = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_delete")
	for uid, material_data in pairs(selected_materials_map) do
		local treasure_info = self:CallModuleFunc("GetTreasure", uid)

		--创建材料
		self.__material_icon[uid] = LogicGUI.IconLayer.New(true)
	    self.__material_icon[uid]:SetData( {id = treasure_info.info_id} )
	    self.__material_icon[uid]:SetAnchorPoint(0.5,0.5)
	    self.__material_icon[uid]:ShowEquipStrengthenLev(true)
	    self.__material_icon[uid]:SetPosition( self.addIcon[material_index]:GetPositionX() , self.addIcon[material_index]:GetPositionY() )
	    self.addIconBg[material_index]:AddChild(self.__material_icon[uid]:GetNode())
		self.__material_icon[uid]:SetTouchSwallow(true)

	    --删除材料
    	self.__material_btn[uid] = Util:Button(addicon)
    	self.__material_btn[uid]:SetPosition( self.__material_icon[uid]:GetNode():GetContentWidth() - 8 , self.__material_icon[uid]:GetNode():GetContentHeight() - 2)
    	self.__material_btn[uid]:SetAnchorPoint(0.5,0.5)
    	self.__material_icon[uid]:AddChild(self.__material_btn[uid] , 998)
    	WidgetUnity.SetWidgetListener(self.__material_btn[uid],nil,nil,function ( ... )
    		--真·删除
    		self.__material_icon[uid]:DeleteMe()
    		self.__material_icon[uid] = nil
    		--把预存的数据也删掉
			CallModuleFunc(ModuleType.TREASURE, "SetSelectedStrengthenMaterial", material_data , "remove")
			--预览的也不能留下
			self:PreviewConformity()
    	end)

    	local name = Util:Name(treasure_info.name , nil , treasure_info.quality , nil , 100 )
    	name:SetAnchorPoint(0.5,1)
    	name:SetPosition(self.__material_icon[uid]:GetNode():GetContentWidth()/2 , -10 )
    	self.__material_icon[uid]:AddChild(name)

    	material_index = material_index + 1
	end
end

--设置预览 属性
function Modules.TreasureLiftIntensifyTabView:SetPreviewProperty( ... )
	-- 材料数据计算
    -- 当前经验 上限经验 , 差额 , 吃完的等级 , 材料给的经验
	local nowExp , maxExp , margin , laterLevel , dogFoodExp = CallModuleFunc(ModuleType.TREASURE, "CalculateUpgrade")
	if laterLevel ~= self.onTreasureInfo.strengthen_level then
		-- 吃完的等级
		local text = " +" .. laterLevel - self.onTreasureInfo.strengthen_level
		if laterLevel == 0 then
			text = " "
		end
		self.intensifyLv.junior:SetString(text)
		-- 目标的属性
		local targetProperty  = CallModuleFunc(ModuleType.TREASURE, "GetStrengthenPropPlusMap" , self.onTreasureInfo.uid , laterLevel )
		-- 当前的属性
		local nowProperty  = CallModuleFunc(ModuleType.TREASURE, "GetStrengthenPropPlusMap" , self.onTreasureInfo.uid )
		-- 显示当前的属性
		for i,v in ipairs(targetProperty.lst) do
			--下级值
			local nexType , nexValue = Util:Nature( v.type , v.value - nowProperty.lst[i].value , true )
			local text = nexValue
			if laterLevel == 0 then
				text = CPPGameLib.GetString("equip_intensifyMax")
			end
			self.intensify.addValue[i]:SetString(text)
			self.intensify.addValue[i]:SetVisible(true)
		end
	--如果没有材料
	else
		self.intensifyLv.junior:SetString(" ")
		for i=1 , #self.intensify.addValue do
			self.intensify.addValue[i]:SetString("  ")
			self.intensify.addValue[i]:SetVisible(false)
		end
	end
end

--自动添加按钮 
function Modules.TreasureLiftIntensifyTabView:AutoButtonAdd()
	local is_added, show_tips = CallModuleFunc(ModuleType.TREASURE, "AutoAddStrengthenMaterials")

	--添加成功，显示预览
	if is_added then
		self:PreviewConformity()
		self:UpdateMaterials()

	--添加失败
	elseif show_tips then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("treasure_noUsableBread"))

		local config_treasure = ConfigAdapter.Treasure.GetMinTreasureInfoByEXPType()
		if config_treasure then
			GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, config_treasure.id)
		end
	end
end

--强化
function Modules.TreasureLiftIntensifyTabView:OnButtonIntensifyAdd()
	--获得材料列表
	local selected_materials_map = CallModuleFunc(ModuleType.TREASURE, "GetSelectedStrengthenMaterialMap")
	local exp, gold = CallModuleFunc(ModuleType.TREASURE, "GetSelectedStrengthenMaterialInfo")
	local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
	local levelMax = CallModuleFunc(ModuleType.TREASURE, "GetLiftMaxInfo",self.onTreasureInfo.quality)

	local cnf = CPPGameLib.GetConfig( "TreasureStrengthen" , self.onTreasureInfo.quality )
    local lst = CallModuleFunc(ModuleType.TREASURE, "GetMaxStrengthenInfo" , self.onTreasureInfo.quality )

	if gold > userDate.gold then
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("common_gold_not_enough") )
		CallModuleFunc(ModuleType.SUNDRY, "ToIdGetPath", Macros.Game.RolePropType.GOLD )
		
	elseif next(selected_materials_map) == nil then
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("treasure_nobread") )

	elseif self.onTreasureInfo.strengthen_level >= levelMax.strengthenMax then
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("equip_intensifyMax") )
		return

	elseif self.onTreasureInfo.strengthen_level + 1 > lst.strengthen_level  then		
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("hero_train_level_no") )
		return

	else
		Util:GreyButton( self.Button_IntensifyAdd )

		local info = {}
		info.treasure_uid = self.onTreasureInfo.uid
		info.cost_treasure_list = CPPGameLib.ConvertMap2List(selected_materials_map)
		CallModuleFunc(ModuleType.TREASURE, "TreasureStrengthenReq", info)
	end
end

--点击添加按钮
function Modules.TreasureLiftIntensifyTabView:OnAddIcon(index)
	local toliftMax = CallModuleFunc(ModuleType.TREASURE, "GetLiftMaxInfo" , self.onTreasureInfo.quality ).strengthenMax
	if self.onTreasureInfo.strengthen_level >= toliftMax then
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("equip_intensifyMax") )
	else
		--清除所有材料
		for _, material_icon in pairs(self.__material_icon) do
			material_icon:DeleteMe()
		end
		self.__material_icon = {}

		local treasureLst = CallModuleFunc(ModuleType.TREASURE, "GetAllTreasureLst" , true)
		if #treasureLst <= 0  then
			local config_treasure = ConfigAdapter.Treasure.GetMinTreasureInfoByEXPType()
			if config_treasure then
				GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, config_treasure.id)
			end
		else
			CallModuleFunc(ModuleType.TREASURE, "OpenView", BaseViewType.TREASURDEVOUR)
		end
	end
end

function Modules.TreasureLiftIntensifyTabView:OneKeyUpgrade()
	--计算可用材料数量
	local material_num = 0
	CallModuleFunc(ModuleType.TREASURE, "GetStrengthenMaterialList", true, false, function()
		material_num = material_num + 1
	end)

	if material_num > 0 then
		LogicGUI.TemplTreasureOnekeyIntensify.New(self.onTreasureInfo.uid, function(levelup, cost_material_list, cost_exp, cost_gold)
			--检测强化材料
			if not cost_material_list or #cost_material_list < 1 then
				--异常，从逻辑上不会出现
				return
			end

			--检测金币是否充足
			local role_gold = CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.GOLD)
			if cost_gold > role_gold then
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("common_gold_not_enough"))
				return
			end

			--已有强化材料不足以提升一级
			if levelup <= 0 then
			    LogicGUI.TemplAffirmTips.New({
			    	type = LogicGUI.TemplAffirmTipsType.YES_NO,
			        content = CPPGameLib.GetString("equip_insufficientUP") ,
			        confirm_text = CPPGameLib.GetString("common_confirm") , 
					confirm_func = function ( ... )
						local info = {}
						info.treasure_uid = self.onTreasureInfo.uid
						info.cost_treasure_list = cost_material_list
						CallModuleFunc(ModuleType.TREASURE, "TreasureStrengthenReq", info)
					end,
			    })

			--已有强化材料足以提升一级或以上
			else
				local info = {}
				info.treasure_uid = self.onTreasureInfo.uid
				info.cost_treasure_list = cost_material_list
				CallModuleFunc(ModuleType.TREASURE, "TreasureStrengthenReq", info)
			end
		end)
	else
		local config_treasure = ConfigAdapter.Treasure.GetMinTreasureInfoByEXPType()
		if config_treasure then
			GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, config_treasure.id)
		end
	end
end
