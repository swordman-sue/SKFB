Modules = Modules or {}

Modules.TemplSystemForeShowScrollItem =  Modules.TemplSystemForeShowScrollItem or BaseClass(GUI.ScrollItem)

function Modules.TemplSystemForeShowScrollItem:__init()
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_SYSTEM_FORESHOW_CELL)
end

function Modules.TemplSystemForeShowScrollItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	--开启等级
	local txt_openlevel = self:GetWidget("Text_openLevel")
	Util:WidgetLabel(txt_openlevel,CPPGameLib.GetString("system_foreshow_openlevel_text",self.__data.open_level), Macros.TypefaceSize.largish, Macros.Color.content)
	--系统名字
	Util:WidgetLabel(self:GetWidget("Text_systemName"),self.__data.system_name, Macros.TypefaceSize.largish, Macros.Color.headline )
	--系统描述
	Util:WidgetLabel(self:GetWidget("Text_systemDes"),self.__data.system_des, Macros.TypefaceSize.largish, Macros.Color.keypoint,326,50)
	--系统图标
	local Image_data = Resource.PathTool.GetSystemEntryIconPath(self.__data.icon_id)
	local Image_icon = self:GetWidget("Image_icon")
	Image_icon:IgnoreContentAdaptWithSize(true)
	Image_icon:SetImage(Image_data,TextureResType.LOCAL)
	Image_icon:SetScaleFactor(self.__data.icon_zoom)
	--升级按钮
	local button_levelup = self:GetWidget("Button_levelup")
	button_levelup:SetVisible(self.__data.is_levelup_visible)
	if self.__data.is_levelup_visible then
		Util:WidgetButton(button_levelup,CPPGameLib.GetString("system_foreshow_goto_levelup_text"),function ()
			local chapter_index,gamelevel_index = CallModuleFunc(ModuleType.DUNGEON,"GetCurDungeonLatestChapterAndGameLevelByType",Macros.Game.FBType.MAIN_DUNGEON) 
			GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.DUNGEON_GAMELEVEL,{chapter_index,gamelevel_index})
		end, Macros.TypefaceSize.largish, Macros.Color.btn, Macros.Color.button_yellow)
	end
	--已开启
	self:GetWidget("Image_havedOpen"):SetVisible(self.__data.role_level >= self.__data.open_level)
	
end