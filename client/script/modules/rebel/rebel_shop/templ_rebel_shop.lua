--
-- @brief 叛军战功商店
-- @author: yjg
-- @date: 2016年9月19日18:26:29
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplRebelShop = LogicGUI.TemplRebelShop or BaseClass(GUI.Template)

function LogicGUI.TemplRebelShop:__init()

	self.infoUp = self:BindGlobalEvent(Macros.Event.RoleModule.PROP_CHANGE,function (_,data)
		self:UpPossess()
	end)

	self.itemPackAdd = self:BindGlobalEvent(Macros.Event.PackModule.ADD_GET_PACK,function (_,data)
		self:UpItem(data)
	end)

	self.itemPackUp = self:BindGlobalEvent(Macros.Event.PackModule.UPDATE_PACK,function (_,data)
		self:UpItem(data)
	end)

	self.__is_auto_load = true

	self.__open_callback = function()
		--遮罩层
		local BG = self:GetWidget("node_th")
		self.__shield_layer = LogicGUI.ShieldLayer.New(function()
			self:DeleteMe()
		end)
		BG:AddChild(self.__shield_layer:GetNode())

		self:__Getwidget()
		self:__ShowUI()
	end
	self:__AddPreloadTemplate(GUI.TemplateRegistry.REBEL_SHOP_ITEM)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.PACK_ONEUSE)
	self:InitTemplate(GUI.TemplateRegistry.REBEL_SHOP)	
end

function LogicGUI.TemplRebelShop:__delete()
	if self.__shield_layer then
		self.__shield_layer:DeleteMe()
		self.__shield_layer = nil
	end

	if self.lst_list then
		self.lst_list:DeleteMe()
		self.lst_list = nil
	end

end

function LogicGUI.TemplRebelShop:__Getwidget()
	self.img_tou = self:GetWidget("img_tou")

	self.img_bg = self:GetWidget("img_bg")

	self.img_lstBg = self:GetWidget("img_lstBg")
end	

function LogicGUI.TemplRebelShop:__ShowUI()
	local txt = Util:Label(CPPGameLib.GetString("rebel_shop"),Macros.TypefaceSize.headline,Macros.Color.headline)
	txt:SetPosition(self.img_tou:GetContentWidth()/2 , self.img_tou:GetContentHeight()/2)
	self.img_tou:AddChild(txt)

	local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
	CPPGameLib.PrintTable(userDate,"服务器推送")

	local possessTxt = Util:Label( CPPGameLib.GetString("rebel_own_work"))
	possessTxt:SetPosition( 20 , self.img_bg:GetContentHeight() - 35 )
	possessTxt:SetAnchorPoint(0,0.5)
	self.img_bg:AddChild(possessTxt)

  --价格图标
	local img_diamond = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_MONEYICON, "type_5")
	local diamond = Scale9SpriteEx.CreateWithImageFile(img_diamond)
	diamond:SetScaleFactor(0.8)
	PosTool.RightTo(possessTxt,diamond)
	self.img_bg:AddChild(diamond)

	--价格
	self.possess = Util:Label(userDate.rebel_value)
	PosTool.RightTo(diamond,self.possess)
	self.img_bg:AddChild(self.possess)

	local rebel_value = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo").rebel_value
	local t1 = {}
	for i,v in pairs(CPPGameLib.GetConfig("RebelShop", nil, nil , true )) do
		if v.need_role_level <= userDate.level then
			local info = {}
			info.item_id = v.item_id
			info.goods_id = i
			info.item_num = v.item_num
			info.cost_type_list = v.cost_type_list
			info.cost_value_list = v.cost_value_list		
			info.type = Macros.Game.SHOP.REBEL
			info.buy_num_max = math.floor(rebel_value / v.cost_value_list[1] )
			info.buy_type = 0
			table.insert(t1,info)
		end
	end

	local params =  
	{
		item_class = LogicGUI.TemplShopPropItem,
		item_width = 649.00,
		item_height = 110.00,
		row = 5,
		col = 1,
		item_space_r = -2,
		view_width = 649,
		view_height = 441,
	}
	-- 创建参数
	self.lst_list = GUI.ScrollView.New(params)
	self.lst_list:SetDataList(t1)
	self.img_lstBg:AddChild(self.lst_list:GetNode())
	PosTool.Center(self.lst_list:GetNode())

	--屏蔽掉属性说明
	self.lst_list:ItemDataIter( function ( index, data, item )
		data.notNature = true
		if item then
			if item.nature then
				item.nature:SetVisible(false)
			end
		end
	end )
end

function LogicGUI.TemplRebelShop:UpPossess( ... )
	local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
	if self.possess then
		self.possess:SetString(userDate.rebel_value)
	end
end

function LogicGUI.TemplRebelShop:UpItem( info )
	--列表相关调整
	local item_data
	self.lst_list:ItemIter(function(i, item)
		item_data = item:GetData()
		if info.item_id == item_data.item_id then
			local numText = " "
			local type_ = ObjIDTool.GetInfo(item_data.item_id)
			if type_ == Macros.Game.ObjType.ITEM then
				local info = CPPGameLib.GetConfig("Item", item_data.item_id)
				if info.sub_type == Macros.Game.NormalItemSubType.EQUIPDEBRIS then
					local pdebris = CPPGameLib.GetConfig("EquipmentCompose", info.target_id )
					local packnum = CallModuleFunc(ModuleType.Pack , "GetNumByItemID" , pdebris.item_id )
					numText = CPPGameLib.GetString("tower_amount",packnum,pdebris.item_num)

				elseif info.sub_type == Macros.Game.NormalItemSubType.HERODEBRIS then
					local pdebris = CPPGameLib.GetConfig("HeroCompound", info.target_id )
					local packnum = CallModuleFunc(ModuleType.Pack , "GetNumByItemID" , pdebris.item_id )
					numText = CPPGameLib.GetString("tower_amount",packnum,pdebris.item_num)

				elseif info.sub_type == Macros.Game.NormalItemSubType.TREASURE_DEBRIS then

				end
			elseif type_ == Macros.Game.ObjType.EQUIP then
				local info = CPPGameLib.GetConfig("Equipment", item_data.item_id)
			elseif type_ == Macros.Game.ObjType.HERO then
				local info = CPPGameLib.GetConfig("Hero", item_data.item_id )
			elseif type_ == Macros.Game.ObjType.TREASURE then
				local info = CPPGameLib.GetConfig("Treasure", item_data.item_id )
			end
			if item.tips then
				item.tips:SetString(numText)
			end
		end
	end)
end
