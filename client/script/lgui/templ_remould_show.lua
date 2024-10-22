
--
-- @brief 进化弹窗
-- @author: yjg
-- @date: 2017年8月3日11:34:06
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplRemouldShow = LogicGUI.TemplRemouldShow or BaseClass(GUI.Template)

function LogicGUI.TemplRemouldShow:__init(id , fun)
	self.__is_auto_load = true
	self.__fun = fun
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__open_callback = function()
		self:__Getwidget()
		self:__SettleData(id)
		self:Effects()
	end


	self:InitTemplate(GUI.TemplateRegistry.TEMPL_ACQUIRE_TIPS )	
end

function LogicGUI.TemplRemouldShow:__delete()
end

function LogicGUI.TemplRemouldShow:__Getwidget()
	--背景
	self.Image_bg = self:GetWidget("Image_bg")
	--特效
	self.Panel_effects = self:GetWidget("Panel_effects")
	self.Panel_effects:SetPosition(self.Panel_effects:GetPositionX() , self.Panel_effects:GetPositionY() + 10 )
	--特效文字
	self.Panel_effectText = self:GetWidget("Panel_effectText")

	--确定按钮
	self.Button_ok = Util:WidgetButton( 
										self:GetWidget("Button_ok") , 
										CPPGameLib.GetString("common_confirm") , 
										CPPGameLib.Handler(self,self.OnBtnClose) 
										,nil ,Macros.Color.white ,Macros.Color.button_yellow
										)
end

function LogicGUI.TemplRemouldShow:__SettleData( id )
	local type_ , info = ObjIDTool.GetInfo( id )

	self.iconId , iconLed = TypefaceEffects.New():Spotlight( self , self.Image_bg , info.quality or info.init_quality )
	iconLed:SetPosition( self.Image_bg:GetContentWidth()/2 - 180 , 30 )
	iconLed:SetScaleFactor( 0.85 )

	local icon = nil
	if type_ == Macros.Game.ObjType.EQUIP or type_ == Macros.Game.ObjType.TREASURE then
		icon = Util:Sprite( Resource.PathTool.GetBigIconPath(id))
		icon:SetAnchorPoint( 0.5 , 0.5 )
		icon:SetPosition( self.Image_bg:GetContentWidth()/2 - 180 , self.Image_bg:GetContentHeight()/2 )
		self.Image_bg:AddChild( icon )
		
		if type_ == Macros.Game.ObjType.EQUIP then 
			icon:SetScaleFactor( 0.65 )
		elseif type_ == Macros.Game.ObjType.TREASURE then
			icon:SetScaleFactor( 1 )
		end

		ActionManager.GetInstance():UpDownMoveAnimation(icon , 1 , 15)

	end

	local name = Util:Name( info.name , Macros.TypefaceSize.popup , info.quality or info.init_quality )
	name:SetAnchorPoint( 0 , 0.5 )
	name:SetPosition( self.Image_bg:GetContentWidth()/2 , self.Image_bg:GetContentHeight()/2 + 60 )
	self.Image_bg:AddChild(name)


	local t1 = {}
	if type_ == Macros.Game.ObjType.TREASURE then
	    --属性
		local onTreasureInfo = CallModuleFunc(ModuleType.TREASURE, "GetOnTreasureInfo")
	    local evolveInfo = CallModuleFunc(ModuleType.TREASURE, "GetLiftPropPlusInfos", onTreasureInfo )  
	    table.insert( t1 , { type = CPPGameLib.GetString("equip_intensifyLv") .. onTreasureInfo.strengthen_level } )
	    for i , v in ipairs(evolveInfo) do
	        local type_ = Util:Nature( v.type , v.value , nil , true )
	        table.insert( t1 , { type = type_ , value = value } )
	    end

    elseif type_ == Macros.Game.ObjType.EQUIP then

	    local onequipInfo = CallModuleFunc(ModuleType.EQUIP, "GetOnEquipInfo")
	    local equipInfo = CallModuleFunc(ModuleType.EQUIP, "GetEquip",onequipInfo.uid)
	    --属性
	    local evolveInfo = CallModuleFunc(ModuleType.EQUIP, "GetLiftPropPlusInfos", equipInfo )  
	    table.insert( t1 , { type = CPPGameLib.GetString("equip_intensifyLv") .. equipInfo.strengthen_level } )
	    for i , v in ipairs(evolveInfo) do
	        local type_ , value = Util:Nature( v.type , v.value , nil , true )
	        table.insert( t1 , { type = type_ , value = value } )
	    end
	end

    local lst = {}
    for i , v in ipairs( t1 ) do
    	lst[i] = Util:Label( v.type )
		lst[i]:SetAnchorPoint( 0 , 0.5 )
		lst[i]:SetPosition( self.Image_bg:GetContentWidth()/2 , self.Image_bg:GetContentHeight()/2 + 25 - ( (i-1) * 28 ) )
		self.Image_bg:AddChild( lst[i] )
    end

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "examine")
	self.examine  =  Util:Button(	
									sp,
									function ( ... )
										self:__ShowTips( type_ , id , info )
									end
								)
	self.examine:SetPosition( self.Image_bg:GetContentWidth()/2 - 100 , 30 )
	self.Image_bg:AddChild(self.examine) 

end

--展示信息
function LogicGUI.TemplRemouldShow:__ShowTips( obj_type , id , info)

	if obj_type == Macros.Game.ObjType.TREASURE then
		GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.PLUNDERED_TREASURES_DETAILMESSAGE, false, id)
    elseif obj_type == Macros.Game.ObjType.EQUIP then	
    	local onequipInfo = CallModuleFunc(ModuleType.EQUIP, "GetOnEquipInfo")
    	local data = {}
    	data.id = onequipInfo.info_id
		TemplEquipInfo.GetInstance( data )
    end
end

function LogicGUI.TemplRemouldShow:OnBtnClose()
	self:DeleteMe()
	if self.__fun then
		self.__fun()
	end
end

--特效
function LogicGUI.TemplRemouldShow:Effects()
	--特效
	local effect_id, effect = self:__CreateEffect({id = 100033}, self.Panel_effects)
	PosTool.CenterBottom(effect)	

	local effect_id, effect = self:__CreateEffect({id = 100011}, self.Panel_effectText)
	PosTool.CenterBottom(effect)
end
