GUI = GUI or {}

GUI.TemplHeroHandBookHeroFetterView = GUI.TemplHeroHandBookHeroFetterView or BaseClass(GUI.Template)

function GUI.TemplHeroHandBookHeroFetterView:__init(id,hero_info)
	self:InitTemplate(GUI.TemplateRegistry.HERO_HANDBOOK_HEROFETTERVIEW)	
	self:createView(id,hero_info)
end

function GUI.TemplHeroHandBookHeroFetterView:__AddToParent()
	
end

function GUI.TemplHeroHandBookHeroFetterView:createView(id,hero_info)
	--获取英雄信息
	local info = CPPGameLib.GetConfig("Hero",id)
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("hero_fetter"), Macros.TypefaceSize.popup,Macros.Color.btn)
	--头像
	local Panel_heroIcon = self:GetWidget("Panel_heroIcon")
	local icon = LogicGUI.IconLayer.New(false,false)
	Panel_heroIcon:AddChild(icon:GetNode())
	icon:SetData({id = id})
	PosTool.Center(icon)	
	--名字
	local Panel_name = self:GetWidget("Panel_name")
	local namelabel = Util:Name(info.name,Macros.TypefaceSize.popup,hero_info and hero_info.quality_level or info.init_quality)
	namelabel:SetAnchorPoint(0,0.5)
	Panel_name:AddChild(namelabel)
	PosTool.LeftCenter(namelabel)
	--碎片类型
	local Image_type = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON,"icon_profession_"..info.profession,true)
	self:GetWidget("Image_type"):SetImage(Image_type)
	--隐藏所有宿命
	for i=1,12 do
		self:GetWidget("Image_point"..i):SetVisible(false)
	end
	self.dataTable = {}
	--英雄宿命
	local heroTable = {}
	local totalNum = 0
	if info.relation_hero_list then
		for i,relation_hero_list in ipairs(info.relation_hero_list) do
			totalNum = totalNum + 1
			self:GetWidget("Image_point"..totalNum):SetVisible(true)
			local Text_fetter = WidgetUnity.GetWidgetByName(self:GetWidget("Image_point"..totalNum),"Text_fetter")
			Util:WidgetLabel(Text_fetter,info.relation_hero_reinforcementname_list[i],Macros.TypefaceSize.slightly, Macros.Color.headline )
			
			for key,heroId in ipairs(relation_hero_list) do
				if heroId ~= 0 then
					local data = {}
					local heroInfo = CPPGameLib.GetConfig("Hero",heroId)
					data.quality = heroInfo.init_quality
					data.name = heroInfo.name
					data.id = heroId
					data.des = "["..info.relation_hero_reinforcementname_list[i].."]".." "..info.relation_hero_reinforcementdes_list[i]
					table.insert(heroTable,data)
				end
			end
		end
	end
	--给英雄宿命排序
	table.sort(heroTable,function (a,b)
		if a.quality ~= b.quality then
			return a.quality > b.quality
		else
			return a.id < b.id
		end
	end)
	for i,v in ipairs(heroTable) do
		table.insert(self.dataTable,v)
	end
	--装备宿命
	local equipTable = {}
	local equip_list = hero_info and info.relation_equip_ex_list or info.relation_equip_list
	local equip_reinforcementname = hero_info and info.relation_equip_reinforcementname_ex_list or info.relation_equip_reinforcementname_list
	local equip_reinforcementdes = hero_info and info.relation_equip_reinforcementdes_ex_list or info.relation_equip_reinforcementdes_list
	if equip_list then
		for i,relation_equip_list in ipairs(equip_list) do
			totalNum = totalNum + 1
			self:GetWidget("Image_point"..totalNum):SetVisible(true)
			local Text_fetter = WidgetUnity.GetWidgetByName(self:GetWidget("Image_point"..totalNum),"Text_fetter")
			Util:WidgetLabel(Text_fetter,equip_reinforcementname[i],Macros.TypefaceSize.slightly, Macros.Color.headline )

			for key,equipId in ipairs(relation_equip_list) do
				local data = {}
				local equipInfo = CPPGameLib.GetConfig("Equipment",equipId)
				data.quality = equipInfo.quality
				data.name = equipInfo.name
				data.id = equipId
				data.des = "["..equip_reinforcementname[i].."]".." "..equip_reinforcementdes[i]				
				table.insert(equipTable,data)
			end				
		end
	end
	--给装备宿命排序
	table.sort(equipTable,function (a,b)
		if a.quality ~= b.quality then
			return a.quality > b.quality
		else
			return a.id < b.id
		end
	end)
	for i,v in ipairs(equipTable) do
		table.insert(self.dataTable,v)
	end
	--圣器宿命
	local treasureTable = {}
	local treasure_list = hero_info and info.relation_treasure_ex_list or info.relation_treasure_list
	local treasure_reinforcementname = hero_info and info.relation_treasure_reinforcementname_ex_list or info.relation_treasure_reinforcementname_list
	local treasure_reinforcementdes = hero_info and info.relation_treasure_reinforcementdes_ex_list or info.relation_treasure_reinforcementdes_list
	if treasure_list then
		for i,relation_treasure_list in ipairs(treasure_list) do
			totalNum = totalNum + 1
			self:GetWidget("Image_point"..totalNum):SetVisible(true)
			local Text_fetter = WidgetUnity.GetWidgetByName(self:GetWidget("Image_point"..totalNum),"Text_fetter")
			Util:WidgetLabel(Text_fetter,treasure_reinforcementname[i],Macros.TypefaceSize.slightly,Macros.Color.headline )

			for key,treasureId in ipairs(relation_treasure_list) do
				local data = {}
				local treasureInfo = CPPGameLib.GetConfig("Treasure",treasureId)
				data.quality = treasureInfo.quality
				data.name = treasureInfo.name
				data.id = treasureId
				data.des = "["..treasure_reinforcementname[i].."]".." "..treasure_reinforcementdes[i]
				table.insert(treasureTable,data)
			end				
		end
	end
	--给圣器宿命排序
	table.sort(treasureTable,function (a,b)
		if a.quality ~= b.quality then
			return a.quality > b.quality
		else
			return a.id < b.id
		end
	end)
	for i,v in ipairs(treasureTable) do
		table.insert(self.dataTable,v)
	end
	--卡牌宿命
	local divinationTable = {}
	if info.relation_card_list then
		for i,relation_card_list in ipairs(info.relation_card_list) do
			totalNum = totalNum + 1
			self:GetWidget("Image_point"..totalNum):SetVisible(true)
			local Text_fetter = WidgetUnity.GetWidgetByName(self:GetWidget("Image_point"..totalNum),"Text_fetter")
			Util:WidgetLabel(Text_fetter,info.relation_card_reinforcementname_list[i],Macros.TypefaceSize.slightly,Macros.Color.headline )

			for key,card_type in ipairs(relation_card_list) do
				local card_id = info.recommend_card_list[i][key]				
				if card_id then
					local data = {}
					data.quality = info.init_quality
					data.name = CPPGameLib.GetString("divination_type"..card_type) or ""
					data.id = card_id
					data.card_type = card_type
					data.des = "["..info.relation_card_reinforcementname_list[i].."]".." "..info.relation_card_reinforcementdes_list[i]
					table.insert(divinationTable,data)
				end
			end				
		end
	end
	--给卡牌宿命排序
	table.sort(divinationTable,function (a,b)
		if a.card_type ~= b.card_type then
			return a.card_type > b.card_type
		else
			return a.id < b.id
		end
	end)
	for i,v in ipairs(divinationTable) do
		table.insert(self.dataTable,v)
	end

	local ih = 87
	--宿命条目
	local scrollviewPanel = self:GetWidget("Panel_heroFetter")
	local params = {
		item_class = GUI.TemplHeroHandBookHeroFetterViewCell,
    	item_width = 532,
    	item_height	= ih ,
    	row	= 2,
    	col	= 1,
    	item_space_r = 5,
    	view_width 	= 532,
    	view_height = 230,
	}
	self.scrollview = GUI.ScrollView.New(params)
	scrollviewPanel:AddChild(self.scrollview:GetNode())
	self.scrollview:SetDataList(self.dataTable)
	PosTool.Center(self.scrollview)
end

function GUI.TemplHeroHandBookHeroFetterView:__delete()
	if self.scrollview then
		self.scrollview:DeleteMe()
		self.scrollview = nil
	end
end

--英文定制
function GUI.TemplHeroHandBookHeroFetterView:AdapterEnglish()



end