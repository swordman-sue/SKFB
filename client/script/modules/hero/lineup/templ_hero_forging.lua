
--
-- @brief 锻造大师
-- @author: yjg
-- @date: 2016年8月24日21:29:02
--

Modules = Modules or {}

Modules.TemplHeroForging = Modules.TemplHeroForging or BaseClass(GUI.Template)

function Modules.TemplHeroForging:__init( pos )
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}

	local _ , equipInfo , treasureInfo = CallModuleFunc(ModuleType.HERO, "GetLineupEquipInfo", pos )
	local info = {}
	local equipNum = CPPGameLib.GetTableLength( equipInfo )
	if equipNum == 4 then
		info[1] = CPPGameLib.GetString("forge_equip_intensify")
		if GlobalModulesMgr:IsOpen(Macros.Game.SystemID.EQUIP_REFINE) then
			info[2] = CPPGameLib.GetString("forge_equip_refine")
		end
	end

	local treasureNum = CPPGameLib.GetTableLength( treasureInfo )
	if treasureNum == 2 then

		if GlobalModulesMgr:IsOpen(Macros.Game.SystemID.TREASURE_INTENSIFY) then
			info[#info + 1] = CPPGameLib.GetString("forge_treasure_intensify")
		end

		if GlobalModulesMgr:IsOpen(Macros.Game.SystemID.TREASURE_REFINE) then
			info[#info + 1] = CPPGameLib.GetString("forge_treasure_refine")
		end

	end
 	local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen")
  	local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bigblue")
  	self.__tab_params = {
		{sp_2,sp_1},
		{
			info[1] or nil ,
			info[2] or nil ,
			info[3] or nil ,
			info[4] or nil ,
		},
		5,
		CPPGameLib.Handler(self, self.OnYeMei),
		true
	}
		
	self.__open_callback = function ()
		self:__Getwidget()
		self:__ShowUI(pos)
	end	

    self:InitTemplate(GUI.TemplateRegistry.EQUIP_FORGE)
end

function Modules.TemplHeroForging:__Dispose()
end

function Modules.TemplHeroForging:__Getwidget( ... )
	--背景
    self.Image_bg = self:GetWidget("Image_bg") 
    --关闭按钮
    self.Button_close = Util:WidgetButton( self:GetWidget("Button_close") , nil , CPPGameLib.Handler(self,self.OnButtonClose) )
    --内容框
    self.Image_lstBg = self:GetWidget("Image_lstBg") 
    --光
    self.img_headline = self:GetWidget("img_headline") 
    --页签容器
    self.Panel_TAB = self:GetWidget("Panel_TAB") 

    --规则按钮
	local Button_rule = Util:WidgetButton( self:GetWidget("Button_rule") , nil , function ( ... )
		print("xy" , "(4)锻造大师，小图标（关于锻造大师/精炼大师的介绍，在几个标签页下通用）：" )
        local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
        help:SetLoadedCallback( function ( ... )
           help:SetCell("Rule_Equip_Master")
        end )
	end )
	Button_rule:SetZOrder(100)
end

function Modules.TemplHeroForging:__ShowUI( pos )

	--提示
	local tips = Util:Label( CPPGameLib.GetString("forge") , Macros.TypefaceSize.popup , Macros.Color.lilyWhite )
	tips:SetPosition(self.img_headline:GetContentWidth()/2 , self.img_headline:GetContentHeight()/2 )
	self.img_headline:AddChild(tips)

	self.pos = pos
	self.lst = {}

  	self.__tab:On(1)
  	self.__tab:SetPosition(self.Panel_TAB:GetContentWidth() , self.Panel_TAB:GetContentHeight() - 5 )
  	self.Panel_TAB:AddChild(self.__tab:GetNode())
end

function Modules.TemplHeroForging:OnYeMei(index)
	for i=1,4 do
		if self.lst[i] then
			self.lst[i]:SetVisible(false)
		end
	end

	if not self.lst[index] then
		self.lst[index] = Modules.TemplForgeDate.New(self.Image_lstBg ,index , self.pos ,function ( ... )
			self:OnButtonClose()
		end)
	else
		self.lst[index]:SetVisible(true)
	end
end

function Modules.TemplHeroForging:OnButtonClose()
	self:DeleteMe()
end



Modules = Modules or {}

Modules.TemplForgeDate = Modules.TemplForgeDate or BaseClass(GUI.Template)

function Modules.TemplForgeDate:__init(parent_node , index , pos , fun)
	self.__is_auto_load = true
	self.__open_callback = function ()

	   	self.parent_node = parent_node
	   	self.index = index
	   	self.data = data
	   	self.fun = fun

		self:__Getwidget()
		self:__ShowUI(pos)

	end	
    self:InitTemplate(GUI.TemplateRegistry.EQUIP_FORGE_ITEM,parent_node)
end

function Modules.TemplForgeDate:__Getwidget()
	--大背景
    self.Panel_9 = self:GetWidget("Panel_9") 
    --标题
    self.Text_head = self:GetWidget("Text_head") 
    --上方右边的线
    self.Image_right_wire_Up = self:GetWidget("Image_right_wire_Up") 
    --上方左边的线
    self.Image_left_wire_Up = self:GetWidget("Image_left_wire_Up") 
    --条目
    self.Image_item = {}
    --图标
    self.Panel_icon = {}
    --名字
    self.Text_name = {}
    --进度条背景
    self.Image_barBg = {}
    --进度条
    self.LoadingBar = {}
    for i=1,4 do
    	self.Image_item[i] = self:GetWidget("Image_item"..i)
    	self.Image_item[i]:SetVisible(false)
    	self.Panel_icon[i] = self:GetWidget("Panel_icon"..i)
    	self.Text_name[i] = self:GetWidget("Text_name"..i)
    	self.Image_barBg[i] = self:GetWidget("Image_barBg"..i)
    	self.LoadingBar[i] = self:GetWidget("LoadingBar_"..i)
    end
    --标题2
    self.Text_head2 = self:GetWidget("Text_head2") 
    --右边的线
    self.Image_right_wire = self:GetWidget("Image_right_wire") 
    --左边的线
    self.Image_left_wire = self:GetWidget("Image_left_wire") 
    --箭头
    self.Image_10_0 = self:GetWidget("Image_10_0") 
    --字板1
    self.Panel_text = self:GetWidget("Panel_text") 
    -- self.Panel_text:SetBackGroundColor(0, 0, 0)
    -- self.Panel_text:SetBackGroundColorType(1)
    -- self.Panel_text:SetBackGroundColorOpacity(120)

	--字板2
    self.Panel_text2 = self:GetWidget("Panel_text2") 
    -- self.Panel_text2:SetBackGroundColor(0, 0, 0)
    -- self.Panel_text2:SetBackGroundColorType(1)
    -- self.Panel_text2:SetBackGroundColorOpacity(120)
    --提示
    self.Text_tips = self:GetWidget("Text_tips") 
end

function Modules.TemplForgeDate:__ShowUI(pos)

	local _ , equipInfo , treasureInfo = CallModuleFunc(ModuleType.HERO, "GetLineupEquipInfo", pos )
	
	local equipNum = CPPGameLib.GetTableLength( equipInfo )
	local text = {}
	local allLst = {}
	local forge_all = {}
	local head_all = {}

	if equipNum == 4 then
		text[1] = { 
					head = CPPGameLib.GetString("forge_equip_intensify_plan") , 
					tipe = CPPGameLib.GetString("forge_equip_intensify_click") 
				}

		text[2] = { 
					head = CPPGameLib.GetString("forge_equip_refine_plan") , 
					tipe = CPPGameLib.GetString("forge_equip_refine_click") 
				}

		forge_all[1] = "forge_allequip_intensify"
		forge_all[2] = "forge_allequip_refine"

		head_all[1] = "forge_equip_intensify_master"
		head_all[2] = "forge_equip_refine_master"		

		local t1 = {}
		for k,v in pairs(equipInfo) do
			table.insert( t1 , v )
		end

		table.sort( t1 , function ( a , b )
			return a.type < b.type
		end )

		table.insert( allLst , t1 )
		table.insert( allLst , t1 )
	end

	local treasureNum = CPPGameLib.GetTableLength( treasureInfo )
	if treasureNum == 2 then
		if not text[1] then
			text[1] = { 
						head = CPPGameLib.GetString("forge_treasure_intensify_plan") , 
						tipe = CPPGameLib.GetString("forge_treasure_intensify_click") 
					} 
			text[2] = { 
						head = CPPGameLib.GetString("forge_treasure_refine_plan") ,	
						tipe = CPPGameLib.GetString("forge_treasure_refine_click") 
					}

			forge_all[1] = "forge_alltreasure_intensify"
			forge_all[2] = "forge_alltreasure_refine"

			head_all[1] = "forge_treasure_intensify_master"
			head_all[2] = "forge_treasure_refine_master"		

		else
			text[3] = { 
						head = CPPGameLib.GetString("forge_treasure_intensify_plan") , 
						tipe = CPPGameLib.GetString("forge_treasure_intensify_click") 
					}
			text[4] = { 
						head = CPPGameLib.GetString("forge_treasure_refine_plan") ,	
						tipe = CPPGameLib.GetString("forge_treasure_refine_click") 
					}
			forge_all[3] = "forge_alltreasure_intensify"
			forge_all[4] = "forge_alltreasure_refine"

			head_all[3] = "forge_treasure_intensify_master"
			head_all[4] = "forge_treasure_refine_master"	
		end

		local t1 = {}
		for k,v in pairs(treasureInfo) do
			table.insert( t1 , v )
		end

		table.sort( t1 , function ( a , b )
			return a.type < b.type
		end )

		table.insert( allLst , t1 )
		table.insert( allLst , t1 )
	end

	--标题
	local head = Util:Label( text[self.index].head , Macros.TypefaceSize.normal , Macros.Color.headline)
	head:SetPosition(self.Text_head:GetContentWidth()/2 , self.Text_head:GetContentHeight()/2 )
	self.Text_head:AddChild(head)

	--右线
	self.Image_right_wire_Up:SetPosition( self.Text_head:GetPositionX() + head:GetContentWidth()/2 + 5 , self.Image_right_wire_Up:GetPositionY() )
	--左线
	self.Image_left_wire_Up:SetPosition( self.Text_head:GetPositionX() - head:GetContentWidth()/2 - 5 , self.Image_right_wire_Up:GetPositionY() )
	
	--提示
	local tips = Util:Label( text[self.index].tipe , Macros.TypefaceSize.minimum , Macros.Color.content )
	tips:SetPosition(self.Text_tips:GetContentWidth()/2 , self.Text_tips:GetContentHeight()/2 )
	self.Text_tips:AddChild(tips)
		
	local textType = self.index%2
	local showType = ""
	local fourInOne = ""
	if textType == 1 then
		showType = "qianghua"
		fourInOne = "strengthen_level"
	else
		showType = "jinglian"
		fourInOne = "refine_level"
	end

	for i,v in ipairs( allLst[self.index] ) do
		self.Image_item[i]:SetVisible(true)
	end

	local nowLst , newLst , nowLevel , newLevel , levelName , max = CallModuleFunc(ModuleType.FORGINGMASTER , "GetProperty" , pos , self.index )

	for i,v in ipairs( allLst[self.index ] ) do
		--头像
		local icon_layer = LogicGUI.IconLayer.New(false)
		icon_layer:SetData({id = v.info_id})
		icon_layer:SetAnchorPoint(0.5,0.5)
		icon_layer:SetScaleFactor(0.8,0.8)
		icon_layer:SetTouchCallback(function()
			if showType == "qianghua" then
				local type_ , __ = ObjIDTool.GetInfo(v.info_id)
				if type_ == Macros.Game.ObjType.EQUIP then
					local info = {}
			        info.equip_uid = v.uid
			        info.btn = false 
			        CallModuleFunc(ModuleType.EQUIP, "OpenView", BaseViewType.EQUIPINTENSIFY , 2 , info)
				elseif type_ == Macros.Game.ObjType.TREASURE then
					local info = {}
			        info.treasure_uid = v.uid
			        info.btn = false 
			        CallModuleFunc(ModuleType.EQUIP, "OpenView", BaseViewType.TREASURE_LIFT , 2 , info)
				end
			elseif showType == "jinglian" then
				local type_ , __ = ObjIDTool.GetInfo(v.info_id)
				if type_ == Macros.Game.ObjType.EQUIP then
					local info = {}
			        info.equip_uid = v.uid
			        info.btn = false 
			        CallModuleFunc(ModuleType.EQUIP, "OpenView", BaseViewType.EQUIPINTENSIFY , 3 , info)
				elseif type_ == Macros.Game.ObjType.TREASURE then
					local info = {}
			        info.treasure_uid = v.uid
			        info.btn = false 
			        CallModuleFunc(ModuleType.EQUIP, "OpenView", BaseViewType.TREASURE_LIFT , 3 , info)
				end
			end

			if self.fun then
				self.fun()
			end

		end)
		self.Panel_icon[i]:AddChild(icon_layer:GetNode())
		PosTool.Center(icon_layer)

		--名字
		local name = Util:Name(v.name,nil,v.quality)
		name:SetAnchorPoint(0,0.5)
		name:SetPosition( 10 ,self.Text_name[i]:GetContentHeight()/2)
		self.Text_name[i]:AddChild(name)

		--进度条
		self.LoadingBar[i]:SetPercent( v[fourInOne] / newLevel * 100 )

		--文字
		local num = Util:Label( v[ fourInOne ] .. "/" .. newLevel  , Macros.TypefaceSize.minimum )
		num:SetPosition(self.LoadingBar[i]:GetPositionX() , self.LoadingBar[i]:GetPositionY() )
		self.Image_item[i]:AddChild(num)
	end

	--标题
	local head2 = Util:Label( levelName , Macros.TypefaceSize.normal , Macros.Color.headline)
	head2:SetPosition(self.Text_head2:GetContentWidth()/2 , self.Text_head2:GetContentHeight()/2 )
	self.Text_head2:AddChild(head2)
	
	--右线
	self.Image_right_wire:SetPosition( head2:GetContentWidth()/2 + self.Text_head2:GetPositionX() + 5 , self.Image_right_wire:GetPositionY() )
	--左线
	self.Image_left_wire:SetPosition( self.Text_head2:GetPositionX() - head2:GetContentWidth()/2 - 5 , self.Image_left_wire:GetPositionY() )
	
	--标题1
	local head_1 = Util:Label( CPPGameLib.GetString( head_all[self.index] , nowLst.level ) , Macros.TypefaceSize.slightly , Macros.Color.keypoint , self.Panel_text:GetContentWidth() - 10 )
	head_1:SetTextHorizontalAlign( LabelEx.ALIGIN_CENTER)
	head_1:SetAnchorPoint(0.5,1)
	self.Panel_text:AddChild(head_1)
	if nowLst.level == 0 then
		head_1:SetString( CPPGameLib.GetString( forge_all[self.index] , newLevel ) )
		head_1:SetPosition( self.Panel_text:GetContentWidth()/2 , self.Panel_text:GetContentHeight() / 2 )
	else
		head_1:SetPosition( self.Panel_text:GetContentWidth()/2 , self.Panel_text:GetContentHeight() - 5 )
	end

	--标题2
	local head_2 = Util:Label( CPPGameLib.GetString( head_all[self.index] , newLst.level ) , Macros.TypefaceSize.slightly , Macros.Color.keypoint , self.Panel_text:GetContentWidth() - 10)
	head_2:SetTextHorizontalAlign( LabelEx.ALIGIN_CENTER)
	head_2:SetAnchorPoint(0.5,1)
	self.Panel_text2:AddChild(head_2)

	if newLst == max.showLv then
		head_2:SetPosition( self.Panel_text2:GetContentWidth()/2 , self.Panel_text2:GetContentHeight() / 2 )
	else
		head_2:SetPosition( self.Panel_text2:GetContentWidth()/2 , self.Panel_text2:GetContentHeight() - 5 )
	end

	if #nowLst.lst ~= 0 then
		local y = 134

		for i,v in ipairs(nowLst.lst) do
			y = y - 23 
			local type_ , value = Util:Nature(v.type , v.value ,true)

			local tab = Util:Label( type_ , Macros.TypefaceSize.slightly , Macros.Color.keypoint)
			tab:SetAnchorPoint(1 , 0.5 )
			tab:SetPosition( self.Panel_text:GetContentWidth()/2 - 20 , y )
			self.Panel_text:AddChild(tab)

			local value_ = Util:Label( value , Macros.TypefaceSize.slightly , Macros.Color.front_color)
			value_:SetAnchorPoint(0 , 0.5 )
			value_:SetPosition( self.Panel_text:GetContentWidth()/2 + 15 , y )
			self.Panel_text:AddChild(value_)	
		end
	end


	local tips = Util:Label( "(".. CPPGameLib.GetString( forge_all[self.index] , newLevel ) ..")" , Macros.TypefaceSize.slightly , Macros.Color.red_color , self.Panel_text:GetContentWidth() - 10 )
	tips:SetTextHorizontalAlign( LabelEx.ALIGIN_CENTER)
	tips:SetAnchorPoint(0.5,1)
	tips:SetPosition( self.Panel_text2:GetContentWidth()/2 , self.Panel_text2:GetContentHeight() - 25 )
	self.Panel_text2:AddChild(tips)

	local y = tips:GetPositionY() - tips:GetContentHeight() + 10
	for i,v in ipairs(newLst.lst) do
		y = y - 23 
		local type_ , value = Util:Nature(v.type , v.value ,true)

		local tab = Util:Label( type_ , Macros.TypefaceSize.slightly , Macros.Color.keypoint)
		tab:SetAnchorPoint(1 , 0.5 )
		tab:SetPosition( self.Panel_text2:GetContentWidth()/2 - 20 , y )
		self.Panel_text2:AddChild(tab)

		local value_ = Util:Label( value , Macros.TypefaceSize.slightly , Macros.Color.front_color)
		value_:SetAnchorPoint(0 , 0.5 )
		value_:SetPosition( self.Panel_text2:GetContentWidth()/2 + 15 , y )
		self.Panel_text2:AddChild(value_)	
	end
end
