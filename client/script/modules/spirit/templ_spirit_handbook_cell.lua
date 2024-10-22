Modules = Modules or {}

Modules.TemplSpiritHandBookCell = Modules.TemplSpiritHandBookCell or BaseClass(GUI.ScrollItem)


function Modules.TemplSpiritHandBookCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_SPIRIT_HANDBOOK_CELL)
end

function Modules.TemplSpiritHandBookCell:__delete()

end

function Modules.TemplSpiritHandBookCell:SetData(data)	
	GUI.ScrollItem.SetData(self, data)
	local handbook_info = CPPGameLib.GetConfig("SpiritPokedex",self.__data.handbook_index)
	--图鉴名字
	Util:WidgetLabel(self:GetWidget("Text_hangbookName"),handbook_info.pokedex_list[1].name, Macros.TypefaceSize.popup, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
	--图鉴提示
	local txt_tips = self:GetWidget("Text_tips")
	Util:WidgetLabel(txt_tips,CPPGameLib.GetString("spirit_handbook_tips"), Macros.TypefaceSize.largish, Macros.Color.btn, 350, 0, Macros.Color.common_stroke)
	--属性预览
	Util:WidgetButton(self:GetWidget("Button_handbook"),"",function ()
		LogicGUI.TemplSpiritIntensifyMasterView.New(self.__data.handbook_index)
	end)
	Util:WidgetLabel(self:GetWidget("Text_handbook"),CPPGameLib.GetString("spirit_handbook_property_prelook"), Macros.TypefaceSize.button, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
	--图鉴精灵初始化
	for i,spirit_id in ipairs(self.__data.pokedex_list) do
		--精灵数据
		local spirit_data = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritInfoByID",spirit_id)
		--精灵强化数据
		local spirit_enhance_data = CPPGameLib.GetConfig("SpiritEnhace",spirit_id)
		--头像
		local image_spirit = self:GetWidget("Image_spirit"..i)
		local path = Resource.PathTool.GetSpiritHandBookIconPath(spirit_id)
		image_spirit:IgnoreContentAdaptWithSize(true)
		image_spirit:SetImage(path,TextureResType.LOCAL)
		image_spirit:SetGray(not spirit_data)
		--名字
		local spirit_info = CPPGameLib.GetConfig("Spirit",spirit_id)		
		local text_spiritname = self:GetWidget("Text_spiritName"..i)
		Util:WidgetLabel(text_spiritname,spirit_info.name,Macros.TypefaceSize.largish,Macros.Quality[spirit_info.quality],
		 0, 0, Macros.Quality_Stroke[spirit_info.quality])
		if not spirit_data then
			text_spiritname:SetColor(unpack(Macros.Color.Gray))
		end
		--进度条
		self:GetWidget("LoadingBar_"..i):SetPercent(((spirit_data and spirit_data.enhance_level or 0)/table.maxn(spirit_enhance_data.enhance_list))*100)		
		--进度条文字	
		Util:WidgetLabel(self:GetWidget("Text_progressBar"..i),CPPGameLib.GetString("spirit_numcost",spirit_data and spirit_data.enhance_level or 0,table.maxn(spirit_enhance_data.enhance_list)),Macros.TypefaceSize.button, Macros.Color.btn)	
	end
	--加成属性
	local spirit_handbook_data = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritHandBookInfoByID",self.__data.handbook_index)
	local text_property = self:GetWidget("Text_property1")
	Util:WidgetLabel(text_property,"",Macros.TypefaceSize.button, Macros.Color.green)	
	text_property:SetVisible(spirit_handbook_data and true or false)
	if spirit_handbook_data then
		local str = Util:Nature(handbook_info.pokedex_list[spirit_handbook_data.level].pro_type,handbook_info.pokedex_list[spirit_handbook_data.level].pro_value, nil, true)
		text_property:SetText(str)
	end	
end