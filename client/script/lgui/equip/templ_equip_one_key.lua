
--
-- @brief 确认弹窗
-- @author: yjg
-- @date: 2016年10月28日17:51:53
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplEquipOneKey = LogicGUI.TemplEquipOneKey or BaseClass(GUI.Template)

function LogicGUI.TemplEquipOneKey:__init(lineup_index , func )
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__render_queue = Scene.RenderQueue.GRQ_UI_UP	

	self.__is_auto_load = true
	self.__open_callback = function()
		self.func = func
		self:InitWidget()
		self:SetContent(lineup_index)
	end
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_AFFIRM_TIPS)	
end

function LogicGUI.TemplEquipOneKey:InitWidget()
	--标题
	self.txt_headline = Util:WidgetLabel(self:GetWidget("Text_headline"), CPPGameLib.GetString("common_tips") , Macros.TypefaceSize.largish, Macros.Color.lilyWhite)
	--内容
	local img_content = self:GetWidget("Image_lstBg")
	self.txt_content = Util:WidgetLabel(self:GetWidget("Text_content"), "" , nil , nil , img_content:GetContentWidth() - 12)
	--关闭按钮
	self.btn_close = Util:WidgetButton( self:GetWidget("Button_close") , " " , CPPGameLib.Handler(self,self.OnBtnClose) )
	--确定按钮
	self.btn_yes = Util:WidgetButton( 
										self:GetWidget("Button_yes") , 
										CPPGameLib.GetString("common_confirm") , 
										CPPGameLib.Handler(self,self.OnButtonYes) , 
										Macros.TypefaceSize.largish , nil , Macros.Color.button_red)
	--取消按钮
	self.btn_no = Util:WidgetButton( 
										self:GetWidget("Button_no") ,
										CPPGameLib.GetString("common_cancel") , 
										CPPGameLib.Handler(self,self.OnButtonNo) , 
										Macros.TypefaceSize.largish , nil , Macros.Color.button_yellow)
end

function LogicGUI.TemplEquipOneKey:SetContent(lineup_index)

	local A , B , C = ConfigAdapter.Forge.GetEquipStrengthenPlusPropList(10)

	self.lineup_index = lineup_index
	self.num ,_ , self.notype = CallModuleFunc(ModuleType.EQUIP, "OneKeyIntensify" , lineup_index )

	if self.notype == "" then

		local equip_oneKeyIntensifyTips = Util:Echo(
														CPPGameLib.GetString("equip_oneKeyIntensifyTips") ,
														{
															{ zi = self.num ,
															  ys = Macros.Color.content
															}
														},Macros.Color.content
													)
		equip_oneKeyIntensifyTips:SetAnchorPoint(0.5,0.5)
		self.txt_content:AddChild(equip_oneKeyIntensifyTips)
		PosTool.Center( equip_oneKeyIntensifyTips )

	elseif self.notype == "nolevel" then

		local equip_oneKeyIntensifyTips = Util:Echo(
														CPPGameLib.GetString("equip_noIntensifyTips") ,
														{},Macros.Color.content
													)
		equip_oneKeyIntensifyTips:SetAnchorPoint(0.5,0.5)
		self.txt_content:AddChild(equip_oneKeyIntensifyTips)
		PosTool.Center( equip_oneKeyIntensifyTips )	

	elseif self.notype == "nogold" then

		local equip_oneKeyIntensifyTips = Util:Echo(
														CPPGameLib.GetString("equip_noMoney") ,
														{},Macros.Color.content
													)
		equip_oneKeyIntensifyTips:SetAnchorPoint(0.5,0.5)
		self.txt_content:AddChild(equip_oneKeyIntensifyTips)
		PosTool.Center( equip_oneKeyIntensifyTips )	

	elseif self.notype == "noCross" then
		
		local equip_oneKeyIntensifyTips = Util:Echo(
														CPPGameLib.GetString("equip_noCross") ,
														{},Macros.Color.content
													)
		equip_oneKeyIntensifyTips:SetAnchorPoint(0.5,0.5)
		self.txt_content:AddChild(equip_oneKeyIntensifyTips)
		PosTool.Center( equip_oneKeyIntensifyTips )			
			
	end
end


function LogicGUI.TemplEquipOneKey:OnBtnClose()
	self:DeleteMe()
end

function LogicGUI.TemplEquipOneKey:OnButtonYes()
	if self.notype == "" then
		local info = {}
		info.slot = self.lineup_index - 1
		CallModuleFunc(ModuleType.EQUIP, "EquipmentOneKeyEnhanceReq" , info )
		if self.func then
			self.func()
		end
	end

	self:DeleteMe()
end

function LogicGUI.TemplEquipOneKey:OnButtonNo()
	self:DeleteMe()
end


--
-- @brief 一键强化成功属性弹窗 
-- @author: yjg
-- @date: 2016年8月16日10:06:15
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplEquipOnekeyNature = LogicGUI.TemplEquipOnekeyNature or BaseClass(GUI.Template)

function LogicGUI.TemplEquipOnekeyNature:__init( info )
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__render_queue = Scene.RenderQueue.GRQ_UI_UP	

	self.__is_auto_load = true
	self.__open_callback = function()
		local image_content = self:GetWidget("Image_content")
	    image_content:SetImage(Resource.PathTool.GetUIResPath("citycommonslices/bg_sp9_intensify_bg"), TextureResType.LOCAL)

		self:Nature( info )
		self:Master( info )
	end

	self.__bg_params.close_func = function ( ... )

		if self.masterType == true then
			LogicGUI.TempMonster.DisposeData(info,"oneKeyStrengthen")
		else
			-- CallModuleFunc(ModuleType.ROLE,"RecordBattlePowerChanged" , Macros.Game.COMBAT_CHANGE.ONE_KEY_EQUIP_INTENSIFY , true )
		end
		self:DeleteMe()
	end

	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.EQUIPPACK,true)
	self:InitTemplate(GUI.TemplateRegistry.EQUIP_ONEKEY_NATURE)	
end

function LogicGUI.TemplEquipOnekeyNature:Nature( info )
	local _ , herofo , __ = CallModuleFunc(ModuleType.HERO, "GetLineupEquipInfo", info.slot + 1 )

	local t1 = {}
	for k,v in pairs(herofo) do
		local num = v.type + 1
		if info.enhance_level_list[num].old_level ~= info.enhance_level_list[num].new_level then
			table.insert( t1 , v )
		end
	end
	table.sort( t1 , function (a , b)
		return a.type < b.type
	end)

    local col_num = 2
    local last_login_servers_max = 4

    local Image_content = self:GetWidget("Image_content")

    local row_num = math.ceil(last_login_servers_max/col_num)

    local attribute = Util:Layout( Image_content:GetContentWidth() - 110 , Image_content:GetContentHeight()/2 - 30 )
    attribute:SetAnchorPoint(0.5,1)
    attribute:SetPosition( Image_content:GetContentWidth()/2 , Image_content:GetContentHeight() )
    Image_content:AddChild(attribute)

    local intervalW = attribute:GetContentWidth() / col_num
    local intervalH = attribute:GetContentHeight() / col_num

    local key = nil
	for i,v in ipairs(t1) do
        local col_index = (i-1)%col_num
        local row_index = row_num - math.floor((i-1)/col_num) - 1

        local now = CallModuleFunc(ModuleType.EQUIP, "GetStrengthenPropPlusMap", v.uid , v.strengthen_level)
        
	    local type_ , value_ = Util:Nature( now.lst[1].type , now.lst[1].value ,true )

        local text = Util:Name( type_ ..  CPPGameLib.GetString("common_colon").. value_  , Macros.TypefaceSize.normal , Macros.Game.QualityType.GREEN )
        text:SetAnchorPoint(0.5,0)
        text:SetPosition( (intervalW/2) + intervalW * (col_index) , (intervalH/2) + intervalH * (row_index) - 40 )
        attribute:AddChild( text )


		CPPActionManager.DelayTo(text,0.2,GlobalCallbackMgr:AddCallBackFunc(function()
			--特效
			local effect_id,effect = self:__CreateEffect({id = 100045}, text)
			effect:SetAnchorPoint(0.5,0.5)
			effect:SetPosition( text:GetContentWidth()/2  ,  text:GetContentHeight()/2 - 18 )
		end))

	end
end

function LogicGUI.TemplEquipOnekeyNature:Master( info )
	self.masterType = false

	local t1 = {}

	for k,v in ipairs( info.enhance_level_list ) do
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

	local typeLst , valueLst , showLv = ConfigAdapter.Forge.GetEquipStrengthenPlusPropList( new.new )

	local typeLst1 , valueLst1 , showLv1 = ConfigAdapter.Forge.GetEquipStrengthenPlusPropList( old.old )

	if showLv ~= showLv1 then
		self.masterType = true
	end

    local col_num = 2
    local last_login_servers_max = 4

    local Image_content = self:GetWidget("Image_content")
    local row_num = math.ceil(6/col_num)

    local attribute = Util:Layout( Image_content:GetContentWidth() - 110 , Image_content:GetContentHeight()/2 - 30 )
    attribute:SetAnchorPoint( 0.5 , 0 )
    attribute:SetPosition( Image_content:GetContentWidth()/2 , 5 )
    Image_content:AddChild(attribute)

    local intervalW = attribute:GetContentWidth() / col_num
    local intervalH = attribute:GetContentHeight() / col_num

	if showLv ~= 0 then
		local forge_equip_intensify_master1 = Util:LabCenter( {
																{ zi = CPPGameLib.GetString("forge_equip_intensify_master1") , ys = Macros.Color.tangerine_color ,dx = Macros.TypefaceSize.largish },
																{ zi = showLv , ys = Macros.Color.lilyWhite , dx = Macros.TypefaceSize.largish }
															} )
		forge_equip_intensify_master1:SetAnchorPoint(0.5,0.5)
		forge_equip_intensify_master1:SetPosition( Image_content:GetContentWidth()/2 , Image_content:GetContentHeight()/2 - 10 ) 
		Image_content:AddChild( forge_equip_intensify_master1 )

		for i,v in ipairs(typeLst) do
 
	        local col_index = (i-1)%col_num
	        local row_index = row_num - math.floor((i-1)/col_num) - 1

		    local type_ , value_ = Util:Nature( v , valueLst[i] ,true )

	        local text = Util:Name( type_ ..  CPPGameLib.GetString("common_colon").. value_  , Macros.TypefaceSize.normal , Macros.Game.QualityType.GREEN )
	        text:SetAnchorPoint(0.5,0)
	        text:SetPosition( (intervalW/2) + intervalW * (col_index) , ( ( intervalH + (intervalH/2))/2) * (row_index) )
	        attribute:AddChild( text )

			CPPActionManager.DelayTo(text,0.2,GlobalCallbackMgr:AddCallBackFunc(function()
				--特效
				local effect_id,effect = self:__CreateEffect({id = 100045}, text)
				effect:SetAnchorPoint(0.5,0.5)
				effect:SetPosition( text:GetContentWidth()/2  ,  text:GetContentHeight()/2 - 18 )
			end))

		end

	end

	self.__text_continue = Util:Label(CPPGameLib.GetString("common_continue"), Macros.TypefaceSize.popup, Macros.Color.blinkcontent)
	self.__text_continue:SetAnchorPoint(0.5 , 1 )
	self.__text_continue:SetPosition( Image_content:GetContentWidth()/2 , -20 )
	Image_content:AddChild( self.__text_continue )

end

