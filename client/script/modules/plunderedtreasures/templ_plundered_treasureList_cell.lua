Modules = Modules or {}

Modules.TemplPlunderedTreasureListCell = Modules.TemplPlunderedTreasureListCell or BaseClass(GUI.ScrollItem)

function Modules.TemplPlunderedTreasureListCell:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplPlunderedTreasureListCell:__CreateWidgetFromLuaCode()
  self.__layout = Util:Layout(100,80)
  self.__layout:SetClippingEnable(false)
  self.__layout:SetBackGroundColor(0, 0, 0)
  return self.__layout
end

function Modules.TemplPlunderedTreasureListCell:SetData(data)
	GUI.ScrollItem.SetData(self,data)

	--头像
	local icon = LogicGUI.IconLayer.New(false,false)
	self.__layout:AddChild(icon:GetNode())			
	icon:SetData({id = self.__data.treasureId})	
	PosTool.Center(icon)
	
	--检查是否有红点
	local info = CPPGameLib.GetConfig("TreasureCompose",self.__data.treasureId)
	local is_could_compose = true
	for i,treasureId in ipairs(info.cost_item_list) do								
		local pieceNum = CallModuleFunc(ModuleType.Pack,"GetNumByItemID",treasureId)
		if pieceNum < 1 then
			is_could_compose = false	
			break		
		end
	end
	if is_could_compose then
		self:__AddRedDot()
	else
		self:__HideRedDot()
	end
end

--新手指引特殊操作检测(圣器争夺-选择圣器（圣器ID）)
function Modules.TemplPlunderedTreasureListCell:CheckSectionOperateSP()
	CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.PLUNDERED_TREASURES_CHOOSE_TREASURE, self.__layout, self.__data.treasureId)                		
end

--添加红点
function Modules.TemplPlunderedTreasureListCell:__AddRedDot()
	self.__data.has_red_dot = true
	--红点
	if not self.entry_red_dot then
		local entry_red_dot_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
		self.entry_red_dot = SpriteEx.CreateWithImageFile(entry_red_dot_image_data)
		self.entry_red_dot:SetAnchorPoint(1, 1)
		self.__layout:AddChild(self.entry_red_dot,2)
		PosTool.RightTop(self.entry_red_dot,-10,0)
	end
	self.entry_red_dot:SetVisible(true)
end

--隐藏红点
function Modules.TemplPlunderedTreasureListCell:__HideRedDot()
	self.__data.has_red_dot = false

	if self.entry_red_dot then
		self.entry_red_dot:SetVisible(false)
	end	
end

function Modules.TemplPlunderedTreasureListCell:SetSelected(var)
	if var then
		if not self.__selected_image then
		    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_selected")
		    self.__selected_image = Util:Sprite(sp)
		    self.__layout:AddChild(self.__selected_image, 1)
		    PosTool.Center(self.__selected_image)
		    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "glow_arrows")
		    self.__selected_arrows = Util:Sprite(sp)
		    self.__layout:AddChild(self.__selected_arrows, 1)
		    PosTool.RightCenter(self.__selected_arrows,-5,0)
		end
	else
		if self.__selected_image then
			self.__selected_image:RemoveSelf(true)
			self.__selected_image = nil
			self.__selected_arrows:RemoveSelf(true)
			self.__selected_arrows = nil
		end
	end	
end