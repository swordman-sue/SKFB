
LogicGUI = LogicGUI or {}

LogicGUI.TemplPlayerDetailMessage = LogicGUI.TemplPlayerDetailMessage or BaseClass(GUI.Template)

function LogicGUI.TemplPlayerDetailMessage:__init(info)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:InitUI(info)
	end
	self:InitTemplate(GUI.TemplateRegistry.PLAYER_DETAILMESSAGE)	
end

function LogicGUI.TemplPlayerDetailMessage:__delete()
	if self.__ui_hero then
		self.__ui_hero:DeleteMe()
		self.__ui_hero = nil
	end
end


function LogicGUI.TemplPlayerDetailMessage:InitUI(info)
	--背景
	self:GetWidget("Image_bg"):IgnoreContentAdaptWithSize(true)
	self:GetWidget("Image_bg"):SetImage(Resource.PathTool.GetUIResPath("bigbg/bg_herolineup"),TextureResType.LOCAL)
	--控件
	self.__txt_hero_name = self:GetWidget("Text_HeroName")
	-- self.__txt_hero_pro = Util:WidgetLabel(self:GetWidget("Text_HeroProName"), nil, nil, Macros.Color.blinkcontent)
	self.__img_hero_pro = self:GetWidget("Image_Profession")
	--玩家详细信息
	self.playerMessageList = info
	--玩家名字
	Util:WidgetLabel(self:GetWidget("Text_title"), self.playerMessageList.role_name, Macros.TypefaceSize.popup, Macros.Color.btn)
	--关闭按钮
	local closeButton = self:GetWidget("Button_close")
	WidgetUnity.SetWidgetListener(closeButton, nil, nil, function ()
		self:DeleteMe()
	end, nil)
	--上面头像框
	for i,detailMessage in ipairs(self.playerMessageList.formation_list) do
		if detailMessage.equipment_list.GetData then
			detailMessage.equipment_list = detailMessage.equipment_list:GetData()
		end
		if detailMessage.treasure_list.GetData then
			detailMessage.treasure_list = detailMessage.treasure_list:GetData()
		end				
	end
	--英雄队列
	local Panel_herolist = self:GetWidget("Panel_herolist")
	local params = {
		item_class = Modules.PlayerHeroMessageViewItem,
	    item_width = 80,
	    item_height	= 80,
	    row	= 5,
	    col	= 1,
	    item_space_r = 10,
	    view_width = 80,
	    view_height  = 398,
	    selectable = true,
	    select_callback = function (index, data)
	    	self:SetData(data)
	    end,
	}	
	self.__heroicon_scrollView = GUI.ScrollView.New(params)
	Panel_herolist:AddChild(self.__heroicon_scrollView:GetNode())
	PosTool.Center(self.__heroicon_scrollView)
	self.__heroicon_scrollView:SetDataList(self.playerMessageList.formation_list)

	self.__grid_type_map = 
	{
		["weapon_equipment_uid"] = Macros.Game.EquipType.WEAPON,
		["hat_equipment_uid"] = Macros.Game.EquipType.HELMET,
		["belt_equipment_uid"] = Macros.Game.EquipType.BELT,
		["clothes_equipment_uid"] = Macros.Game.EquipType.CLOTH,
		["attack_treasure_uid"] = Macros.Game.TreasureType.EMBLEM,
		["defence_treasure_uid"] = Macros.Game.TreasureType.GAD,
	}		

	local grid_widget_map =
	{
		["weapon_equipment_uid"] = "Weapon",
		["hat_equipment_uid"] = "Helmet",
		["belt_equipment_uid"] = "Belt",
		["clothes_equipment_uid"] = "Cloth",
		["attack_treasure_uid"] = "AttackingTreasure",
		["defence_treasure_uid"] = "DefenseTreasure",
	}
	self.__grid_widget_info_map = {}
	for key, grid_name in pairs(grid_widget_map) do
		local widget_info = {}
		widget_info.panel = self:GetWidget("Panel_"..grid_name)
		widget_info.txt_name = WidgetUnity.GetWidgetByName(widget_info.panel, "Text_"..grid_name)		

		if self:__IsTreasureGrid(key) then
			Util:WidgetLabel(widget_info.txt_name, CPPGameLib.GetString("treasure_grid_name"))
		else
			Util:WidgetLabel(widget_info.txt_name, CPPGameLib.GetString("equip_type"..self.__grid_type_map[key]))
		end
	
		self.__grid_widget_info_map[key] = widget_info
	end	
	--默认打开第一个
	self.__heroicon_scrollView:SelectItem(1)
	-- self:hideSelected(1)
	-- self:SetData(self.playerMessageList.formation_list[1])
end

--隐藏选中框
function LogicGUI.TemplPlayerDetailMessage:hideSelected(key)
	for k=1,5 do
		local selected = WidgetUnity.GetWidgetByName(self:GetWidget("Panel_pos"..k),"Image_selected")	
		if k ~= key then			
			selected:SetVisible(false)
		else
			selected:SetVisible(true)
		end
	end
end

--是否圣器格子
function LogicGUI.TemplPlayerDetailMessage:__IsTreasureGrid(key)
	if key == "attack_treasure_uid" or
		key == "defence_treasure_uid" then
		return true
	end
end

--设置装备信息
function LogicGUI.TemplPlayerDetailMessage:SetData(data)
	--当前的英雄信息
	self.curHeroMessageList = data	
	--英雄信息
	--英雄职业名
	local config_hero = CPPGameLib.GetConfig("Hero", self.curHeroMessageList.hero_info_id)
	if self.curHeroMessageList.break_level > 0 then
		self.__txt_hero_name:SetText(CPPGameLib.GetString("common_add6", config_hero.name, self.curHeroMessageList.break_level))
	else
		self.__txt_hero_name:SetText(config_hero.name)
	end
	self.__txt_hero_name:SetTextColor(Macros.QualityColor(self.curHeroMessageList.quality))
	self.__txt_hero_name:SetStrokeColor(Macros.QualityColorStroke(self.curHeroMessageList.quality))
	self.__img_hero_pro:SetImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON, "icon_profession_"..config_hero.profession, true))

	--英雄模型
	if not self.__ui_hero then
		self.__ui_hero = LogicGUI.Model.New({info_id = self.curHeroMessageList.hero_info_id, parent = self:GetWidget("Panel_HeroModel")})	
		self.__ui_hero:SetScaleFactor(1.2)
		self.__ui_hero:SetPosition(75,0)
	else
		self.__ui_hero:SetInfoID(self.curHeroMessageList.hero_info_id)
	end

	--装备&&圣器格子
	local obj_info
	for key,value in pairs(self.__grid_widget_info_map) do
		if self:__IsTreasureGrid(key) then
			if #self.curHeroMessageList.treasure_list == 0 then
				obj_info = nil
			else
				for i,treasureMessage in ipairs(self.curHeroMessageList.treasure_list) do
					local treasureConfig = CPPGameLib.GetConfig("Treasure",treasureMessage.info_id)
					if treasureConfig.type == self.__grid_type_map[key] then
						obj_info = treasureMessage
						break
					elseif i == #self.curHeroMessageList.treasure_list then
						obj_info = nil
					end
				end
			end
		else
			if #self.curHeroMessageList.equipment_list == 0 then
				obj_info = nil
			else
				for i,equipmentMessage in ipairs(self.curHeroMessageList.equipment_list) do
					local equipmentConfig = CPPGameLib.GetConfig("Equipment",equipmentMessage.info_id)
					if equipmentConfig.item_type == self.__grid_type_map[key] then
						obj_info = equipmentMessage
						break
					elseif i == #self.curHeroMessageList.equipment_list then
						obj_info = nil
					end
				end
			end
		end
		self:__UpdateGrid(obj_info,key)
	end	
end

--设置装备信息
function LogicGUI.TemplPlayerDetailMessage:__UpdateGrid(obj_info, key)
	local widget_info = self.__grid_widget_info_map[key]
	widget_info.txt_name:SetVisible(false)
	if widget_info.icon_equip then
		widget_info.icon_equip:SetVisible(false)
	end
    if not obj_info then
    	--显示空格子
		widget_info.panel:SetBackGroundImageOpacity(255)		
		widget_info.txt_name:SetVisible(true)						
    else
		widget_info.panel:SetBackGroundImageOpacity(0)
		--显示装备、圣器
		if not widget_info.icon_equip then
			widget_info.icon_equip = LogicGUI.IconLayer.New(false,false)			
			widget_info.panel:AddChild(widget_info.icon_equip:GetNode())
			PosTool.Center(widget_info.icon_equip)								
		end
		widget_info.icon_equip:SetVisible(true)
		widget_info.icon_equip:SetData({id = obj_info.info_id, obj_info = obj_info})	
		widget_info.icon_equip:ShowEquipName(true,0,0)
		widget_info.icon_equip:ShowEquipStrengthenLev(true)
		widget_info.icon_equip:ShowTreasureName(true, 0, nil, 0, -20)
		widget_info.icon_equip:ShowTreasureStrengthenLev(true)	
		widget_info.icon_equip:SetEquipStar()	
    end
end



--------------------------------------------------------------------------------------------
--英雄列表条目
--------------------------------------------------------------------------------------------
Modules = Modules or {}
Modules.PlayerHeroMessageViewItem = Modules.PlayerHeroMessageViewItem or BaseClass(GUI.ScrollItem)

function Modules.PlayerHeroMessageViewItem:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.PlayerHeroMessageViewItem:__CreateWidgetFromLuaCode()
  self.layout = Util:Layout(80,80)
  self.layout:SetClippingEnable(false)
  self.layout:SetBackGroundColor(0, 0, 0)
  --self.layout:SetTouchEnabled(true)
  return self.layout
end

function Modules.PlayerHeroMessageViewItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	self.icon = LogicGUI.IconLayer.New(false,false)
	self.layout:AddChild(self.icon:GetNode())
	PosTool.Center(self.icon)
	self.icon:SetData({id = self.__data.hero_info_id})
end

function Modules.PlayerHeroMessageViewItem:SetSelected(var)
	if var then
		if not self.selected_image then
		    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_selected")
		    self.selected_image = Util:Sprite(sp)
		    self.icon:AddChild(self.selected_image, Macros.Global.z_order_top)
		    PosTool.Center(self.selected_image)
		end
	else
		if self.selected_image then
			self.selected_image:RemoveSelf(true)
			self.selected_image = nil
		end
	end	
end