--
-- @brief 大师达成 
-- @author: yjg
-- @date: 2016年8月16日10:06:15
--
LogicGUI = LogicGUI or {}

LogicGUI.TempMonster = LogicGUI.TempMonster or BaseClass()

function LogicGUI.TempMonster:__init( )
end

function LogicGUI.TempMonster:__delete( )
end

function LogicGUI.TempMonster.DisposeData( data , type )
	local t1 = {}

	if type == "equipStrengthen" then

		local index , allLst , eqLst , trLst = CallModuleFunc( ModuleType.HERO , "ToUidGetPlace" , data.equipment_uid )
		if CPPGameLib.GetTableLength(eqLst) < 4 then
			CallModuleFunc(ModuleType.ROLE,"RecordBattlePowerChanged" , Macros.Game.COMBAT_CHANGE.EQUIP_INTENSIFY , true )
			return 
		end

		for k,v in pairs(eqLst) do
			local old_level = v.strengthen_level
			if v.uid == data.equipment_uid then
				old_level = data.strengthen_level - data.type - data.crit_times
			end
			table.insert( t1 , { new_level = v.strengthen_level , old_level = old_level } )		
		end

	elseif type == "equipRefine" then

		local index , allLst , eqLst , trLst = CallModuleFunc( ModuleType.HERO , "ToUidGetPlace" , data.equipment_uid )
		
		if CPPGameLib.GetTableLength(eqLst) < 4 then
			CallModuleFunc(ModuleType.ROLE,"RecordBattlePowerChanged" , Macros.Game.COMBAT_CHANGE.EQUIP_REFINE ,true )
			return 
		end

		for k,v in pairs(eqLst) do
			local old_level = v.refine_level
			if v.uid == data.equipment_uid then
				old_level = data.old_refine_level
			end
			table.insert( t1 , { new_level = v.refine_level , old_level = old_level } )			
		end

	elseif type == "treasureStrengthen" then
		local index , allLst , eqLst , trLst = CallModuleFunc( ModuleType.HERO , "ToUidGetPlace" , data.uid )
		
		if CPPGameLib.GetTableLength(trLst) < 2 then
			CallModuleFunc(ModuleType.ROLE,"RecordBattlePowerChanged" , Macros.Game.COMBAT_CHANGE.TREASURE_INTENSIFY , true )
			return 
		end

		for k,v in pairs(trLst) do
			local old_level = v.strengthen_level
			local strengthen_level = v.strengthen_level
			if v.uid == data.uid then
				old_level = data.old_level
				strengthen_level = data.new_level
			end
			table.insert( t1 , { new_level = strengthen_level , old_level = old_level } )			
		end

	elseif type == "treasureRefine" then

		local index , allLst , eqLst , trLst = CallModuleFunc( ModuleType.HERO , "ToUidGetPlace" , data.uid )

		if CPPGameLib.GetTableLength(trLst) < 2 then
			CallModuleFunc(ModuleType.ROLE,"RecordBattlePowerChanged" , Macros.Game.COMBAT_CHANGE.TREASURE_REFINE , true )
			return 
		end
		for k,v in pairs(trLst) do
			local old_level = v.refine_level
			if v.uid == data.uid then
				old_level = data.refine_level - 1
			end
			table.insert( t1 , { new_level = v.refine_level , old_level = old_level } )			
		end

	elseif type == "oneKeyStrengthen" then
		t1 = data.enhance_level_list

		if #t1 < 4 then
			return 
		end	
	end

	 LogicGUI.TempMonster.Skip( t1 , type ) 
end

function LogicGUI.TempMonster.Skip( info , type ) 
	local text = {}
	text.equipStrengthen = { head = "equip_masterEquipICondition" , text = "forge_equip_intensify_master" , path = ConfigAdapter.Forge.GetEquipStrengthenPlusPropList }
	text.equipRefine = { head = "equip_masterEquipRCondition" , text = "forge_equip_refine_master" , path = ConfigAdapter.Forge.GetEquipRefinePlusPropList }
	text.treasureStrengthen = { head = "equip_masterTreasureICondition" , text = "forge_treasure_intensify_master" , path = ConfigAdapter.Forge.GetTreasureStrengthenPlusPropList }
	text.treasureRefine = { head = "equip_masterTreasureRCondition" , text = "forge_treasure_refine_master" , path = ConfigAdapter.Forge.GetTreasureRefinePlusPropList }
	text.oneKeyStrengthen = { head = "equip_masterCondition" , text = "forge_equip_intensify_master" , path = ConfigAdapter.Forge.GetEquipStrengthenPlusPropList }

	local t1 = {}
	for k,v in ipairs( info ) do
		table.insert( t1 , { old = v.old_level , new = v.new_level } )
	end

	local old = nil
	table.sort( t1 , function ( a, b )
		return a.old < b.old 
	end )
	old = t1[1]

	local new = nil
	table.sort( t1 , function ( a, b )
		return a.new < b.new 
	end )
	new = t1[1]

	local newtypeLst , newvalueLst , newshowLv , newLst = text[type].path( new.new )

	local oldtypeLst , oldvalueLst , oldshowLv = text[type].path( old.old )

	if newshowLv ~= oldshowLv then
		if LogicGUI.TempMonster.master then
			LogicGUI.TempMonster.master:DeleteMe()
			LogicGUI.TempMonster.master = nil
		end
		LogicGUI.TempMonster.master = LogicGUI.TemplEquipOnekeyMaster.New( info , type)
	else
		print("xy" , "继续战力滚动")
		CallModuleFunc(ModuleType.ROLE,"RecordBattlePowerChanged" , Macros.Game.COMBAT_CHANGE.ONE_KEY_EQUIP_INTENSIFY , true )
		-- CallModuleFunc(ModuleType.ROLE,"RecordBattlePowerChanged" , Macros.Game.COMBAT_CHANGE.EQUIP_INTENSIFY , true )
		-- CallModuleFunc(ModuleType.ROLE,"RecordBattlePowerChanged" , Macros.Game.COMBAT_CHANGE.EQUIP_REFINE ,true )
	end

end


LogicGUI = LogicGUI or {}
LogicGUI.TemplEquipOnekeyMaster = LogicGUI.TemplEquipOnekeyMaster or BaseClass(GUI.Template)

function LogicGUI.TemplEquipOnekeyMaster:__init( info , type )
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__render_queue = Scene.RenderQueue.GRQ_UI_UP

	self.__is_auto_load = true
	self.__open_callback = function()

		self:__Getwidget()
		--彩带
		local Image_headline = self:GetWidget("Image_headline")
		Image_headline:SetScaleFactor(1.8)
		CPPActionManager.ScaleTo(Image_headline , 0.16 , 0.9 ,GlobalCallbackMgr:AddCallBackFunc(function ( ... )
			CPPActionManager.ScaleTo(Image_headline , 0.15 , 1)

			local Panel_BGEffect = self:GetWidget("Panel_BGEffect")
			local effectid,effect = self:__CreateEffect({res_name = "UI_zhanduishengji_xuanzhuanguang"} , Panel_BGEffect )
			effect:SetAnchorPoint( 0.5 , 0.5 )
			effect:SetPosition( Panel_BGEffect:GetContentWidth()/2  , Panel_BGEffect:GetContentHeight()/2 - 50 )

			self:Master( info , type)
		end))

	end

	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.EQUIPPACK,true)
	self:InitTemplate(GUI.TemplateRegistry.EQUIP_ONEKEY_MASTRE)	
end

function LogicGUI.TemplEquipOnekeyMaster:__Getwidget()

	--内容层
	self.Image_content = self:GetWidget("Image_content") 
	--标题
	self.Text_Title = self:GetWidget("Text_Title") 

	--等级标题层
	self.Panel_Title = self:GetWidget("Panel_Title") 
	--等级标题1
	self.Text_Title1 = self:GetWidget("Text_Title1") 
	--等级标题2
	self.Text_Title2 = self:GetWidget("Text_Title2") 

	--属性层
	self.Panel_propertyBg = self:GetWidget("Panel_property") 

	self.Panel_property = {}
	for i = 1 , 5 do
		--内容层
		self.Panel_property[i] = self:GetWidget( "Panel_property_" .. i ) 
		self.Panel_property[i]:SetVisible(false)
	end

    local image_content = self:GetWidget("Image_content")
    image_content:SetImage(Resource.PathTool.GetUIResPath("citycommonslices/bg_sp9_master_bg"), TextureResType.LOCAL)
end

function LogicGUI.TemplEquipOnekeyMaster:Master( info , type )
	local text = {}
	text.equipStrengthen = { head = "equip_masterEquipICondition" , text = "forge_equip_intensify_master" , path = ConfigAdapter.Forge.GetEquipStrengthenPlusPropList }
	text.equipRefine = { head = "equip_masterEquipRCondition" , text = "forge_equip_refine_master" , path = ConfigAdapter.Forge.GetEquipRefinePlusPropList }
	text.treasureStrengthen = { head = "equip_masterTreasureICondition" , text = "forge_treasure_intensify_master" , path = ConfigAdapter.Forge.GetTreasureStrengthenPlusPropList }
	text.treasureRefine = { head = "equip_masterTreasureRCondition" , text = "forge_treasure_refine_master" , path = ConfigAdapter.Forge.GetTreasureRefinePlusPropList }
	text.oneKeyStrengthen = { head = "equip_masterCondition" , text = "forge_equip_intensify_master" , path = ConfigAdapter.Forge.GetEquipStrengthenPlusPropList }

	local t1 = {}
	for k,v in ipairs( info ) do
		table.insert( t1 , { old = v.old_level , new = v.new_level } )
	end

	local old = nil
	table.sort( t1 , function ( a, b )
		return a.old < b.old 
	end )
	old = t1[1]

	local new = nil
	table.sort( t1 , function ( a, b )
		return a.new < b.new 
	end )
	new = t1[1]

	local newtypeLst , newvalueLst , newshowLv , newLst = text[type].path( new.new )
	   CPPGameLib.PrintTable("xy" , newtypeLst , "CPPGameLib")


	local oldtypeLst , oldvalueLst , oldshowLv = text[type].path( old.old )
	   CPPGameLib.PrintTable("xy" , old.old , "old.old")

	--标题
	Util:WidgetLabel(
						self.Text_Title, 
						CPPGameLib.GetString( text[type].head , newLst.strengthen_level or newLst.refine_level ), 
						Macros.TypefaceSize.popup, Macros.Color.tangerine_color 
					)

	--等级标题1
	Util:WidgetLabel(
						self.Text_Title1, 
						CPPGameLib.GetString( text[type].text ,oldshowLv) , 
						Macros.TypefaceSize.normal, Macros.Color.lilyWhite 
					)

	--等级标题2
	Util:WidgetLabel(
						self.Text_Title2, 
						CPPGameLib.GetString( text[type].text ,newshowLv) , 
						Macros.TypefaceSize.normal, Macros.Color.lilyWhite 
					)

    --播放CSB动画
    self:PlayActionTimeline("animation0", false, function()
		self.__text_continue = Util:Label(CPPGameLib.GetString("common_continue"), Macros.TypefaceSize.popup, Macros.Color.blinkcontent)
		self.__text_continue:SetAnchorPoint(0.5 , 1 )
		self.__text_continue:SetPosition( self.Image_content:GetContentWidth()/2 , -20 )
		self.Image_content:AddChild( self.__text_continue )

		local layou = Util:Layout( Macros.Global.DesignSize.width, Macros.Global.DesignSize.height ,function ( ... )
			self:DeleteMe()
			print("xy" , "发出继续战力滚动展示")
			CallModuleFunc(ModuleType.ROLE,"RecordBattlePowerChanged" , Macros.Game.COMBAT_CHANGE.ONE_KEY_EQUIP_INTENSIFY , true )
		end)
		layou:SetAnchorPoint( 0.5 , 0.5 )
		self:AddChild( layou )
		PosTool.Center(layou)

    end)

	for i = 1 , #newtypeLst do
		self.Panel_property[i]:SetVisible(true)

		self.Text_Title1_ = self:GetWidget("Text_Title" .. i .. "_1") 
		local oldType_ , oldValue_ = Util:Nature( newtypeLst[i] , oldvalueLst[i] or 0 ,true , true )
		--类型
	    local oldTypeText = Util:Label( oldType_ ..  CPPGameLib.GetString("common_colon") , Macros.TypefaceSize.normal , Macros.Color.golden )
	    oldTypeText:SetAnchorPoint( 1 , 0.5)
	    oldTypeText:SetPosition( self.Text_Title1_:GetContentWidth()/2 , self.Text_Title1_:GetContentHeight()/2 )
	    self.Text_Title1_:AddChild( oldTypeText )
	    --值
        local oldTypeValue = Util:Label( oldValue_ , Macros.TypefaceSize.normal , Macros.Color.lilyWhite )
        oldTypeValue:SetAnchorPoint( 0 , 0.5)
        oldTypeValue:SetPosition( self.Text_Title1_:GetContentWidth()/2 , self.Text_Title1_:GetContentHeight()/2 )
        self.Text_Title1_:AddChild( oldTypeValue )

        self.Text_Title2_ = self:GetWidget("Text_Title" .. i .. "_2") 
		local newType_ , newValue_ = Util:Nature( newtypeLst[i] , newvalueLst[i] or 0 ,true , true )
		--值X2
		Util:WidgetLabel(
							self.Text_Title2_, 
							newValue_ , 
							Macros.TypefaceSize.normal, Macros.Color.lilyWhite 
						)

	    local num = 0
        local symbol = string.sub( newValue_ , -1 )
        local symbol_ = ""
        if symbol == "%" then
        	symbol_ = "%"

        	local num_1 = oldValue_
        	local symbol_ = string.sub( oldValue_ , -1 )
        	if symbol_ == "%" then
        		num_1 = string.sub( oldValue_ , 1 , -2 )
        	end
        	local num_2 = string.sub( newValue_ , 1 , -2 )
        	num = (tonumber(num_2) or 0)  - (tonumber(num_1) or 0)
        else
        	num = tonumber(newValue_) - tonumber(oldValue_)
        end

		self.Text_Title3_ = self:GetWidget("Text_Title" .. i .. "_3") 
		--值X3
		Util:WidgetLabel(
							self.Text_Title3_, 
							num .. symbol_ , 
							Macros.TypefaceSize.normal, Macros.Color.green 
						)

		if num == 0 then
			self.Text_Title3_ :SetVisible(false) 
			self:GetWidget("Image_arrows" .. i):SetVisible(false) 
		end
	end
end