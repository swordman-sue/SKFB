Modules = Modules or {}

Modules.TemplDungeonMessageCell = Modules.TemplDungeonMessageCell or BaseClass(GUI.ScrollItem)

function Modules.TemplDungeonMessageCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.DUNGEON_MESSAGE_CELL)
end

function Modules.TemplDungeonMessageCell:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	--热评标记
	self.__image_hottag = self:GetWidget("Image_hotTag")
	self.__image_hottag:SetVisible(self.__data.__index <= 3)
	--玩家名字
	Util:WidgetLabel(self:GetWidget("Text_playerName"),self.__data.role_name, Macros.TypefaceSize.largish, Macros.Color.headline  )
	--点赞
	self.__button_parise = self:GetWidget("Button_parise")
	Util:WidgetButton(self.__button_parise, "",function ()
		local role_info = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo")
		CPPGameLib.PrintTable("ldx",self.__data.clean_up_list,"self.__data.clean_up_list: ")
		if role_info.role_name ~= self.__data.role_name then
			if self.__data.clean_up_list[role_info.role_id] then
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("dungeon_haved_parise_comment"))
			else
				self.__button_parise:SetEnabled(false)
				CallModuleFunc(ModuleType.DUNGEON,"MapCommentLikeReq",{map_id = self.__data.map_id,comment_id = self.__data.comment_id})
			end
		else
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("dungeon_could_not_parise_self"))
		end
	end)
	--点赞次数
	self.__text_parisenum = self:GetWidget("Text_pariseNum")
	Util:WidgetLabel(self.__text_parisenum,self.__data.like_times, Macros.TypefaceSize.largish, Macros.Color.keypoint)
	--vip 数字
	local panel_vipnum = self:GetWidget("Panel_vipNum")
	self.__vip_font = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "vipnum", true))
    self.__vip_font:SetText(tostring(self.__data.vip_level))
    self.__vip_font:SetAnchorPoint(0, 0.5)
    panel_vipnum:AddChild(self.__vip_font:GetNode())	
    PosTool.LeftCenter(self.__vip_font, 0, 1)
    --评论
    Util:WidgetLabel(self:GetWidget("Text_message"),self.__data.text, Macros.TypefaceSize.normal, Macros.Color.content ,585,50)
    
end

function Modules.TemplDungeonMessageCell:ResetData()
	self.__button_parise:SetEnabled(true)
	self.__data.like_times = self.__data.like_times + 1
	self.__text_parisenum:SetText(self.__data.like_times)
	local role_info = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo")
	self.__data.clean_up_list[role_info.role_id] = role_info.role_id
end

function Modules.TemplDungeonMessageCell:SetHotTag()
	self.__image_hottag:SetVisible(self.__data.__index <= 3)
end

