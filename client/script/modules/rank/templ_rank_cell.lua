Modules = Modules or {}

Modules.TemplRankCell = Modules.TemplRankCell or BaseClass(GUI.ScrollItem)


function Modules.TemplRankCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.RANK_VIEW_CELL)
end

function Modules.TemplRankCell:__delete()

end

function Modules.TemplRankCell:SetData(data)	
	GUI.ScrollItem.SetData(self, data)
	--是否显示星数的
	local is_show_star = false
	if self.__data.rankType == Macros.Game.RankType.MAIN_DUNGEON_STAR or self.__data.rankType == Macros.Game.RankType.KRUUNU then
		is_show_star = true
	end
	--排名图标
	local Image_rank = self:GetWidget("Image_rank")
	--星星图标
	local Image_star = self:GetWidget("Image_star")
	Image_star:SetVisible(is_show_star)
	--角色图标
	local Panel_roleicon = self:GetWidget("Panel_roleicon")
	local icon = LogicGUI.RoleIcon.New(true)
	Panel_roleicon:AddChild(icon:GetNode())
	icon:SetTouchCallback(function()
		-- CallModuleFunc(ModuleType.CHECKROLEINFO, "OpenView", BaseViewType.CHECKROLEINFO,data)
		if not self.__data.is_robot then
			GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.CHECKROLEINFO , false , data )
		else			
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("arena_rank_find_tips"))
		end
    end)
	icon:SetData(data.head_icon)
	icon:adaptParentContentSize()
	PosTool.Center(icon)

	--v字图标
	if self.__data.vip_level > 0 then 
	    self.v_icon = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "vip_title_icon" , true ))
	    self.v_icon:SetScaleFactor(0.8)
	   	self.v_icon:SetAnchorPoint(0,0)
	   	icon:GetNode():AddChild(self.v_icon,100)
	   	self.v_icon:SetPosition(-2,-6)
	    --vip等级
	    self.power_font = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "vipnum", true))
	    self.power_font:SetSpan(-3)
	    self.power_font:SetText(tostring(self.__data.vip_level))
	    self.power_font:SetAnchorPoint(0,0)
	   	icon:GetNode():AddChild(self.power_font:GetNode(),100)
	   	self.power_font:SetPosition(50,-6)
   end

--排名数字
	local Text_rank = self:GetWidget("Text_rank")
	Util:WidgetLabel(Text_rank,"", 26, Macros.Color.headline)
	if data.rank <=3 then 
		local imageData = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"com_sp_rank_num_"..data.rank)
		Image_rank:SetImage(imageData)
		Text_rank:SetVisible(false)
	else
		Image_rank:SetVisible(false)
		Text_rank:SetText(data.rank)
	end
	--公会
	local str = not is_show_star and CPPGameLib.GetString("rank_union") or CPPGameLib.GetString("rank_typeText1")
	local text_union = self:GetWidget("Text_union")
	Util:WidgetLabel(text_union,str, Macros.TypefaceSize.normal , Macros.Color.keypoint)
	--公会名字
	str = not is_show_star and (data.guild_name ~= "" and data.guild_name or CPPGameLib.GetString("rank_haveNoUnion")) or data.battle_value
	local text_unionname = self:GetWidget("Text_unionname")
	Util:WidgetLabel(text_unionname,str, Macros.TypefaceSize.normal , Macros.Color.content)
	--玩家名字
	Util:WidgetLabel(self:GetWidget("Text_playerName"), data.role_name, Macros.TypefaceSize.normal , Macros.Color.headline )	
	--其他类型
	local txt_anotherprotype = self:GetWidget("Text_anotherprotype")
	--其他类型值
	local txt_protypevalue = self:GetWidget("Text_protypevalue")
	--类型
	Util:WidgetLabel(self:GetWidget("Text_protype"), CPPGameLib.GetString("rank_itemtype_text_"..data.rankType) or "", 
Macros.TypefaceSize.normal , Macros.Color.headline )	
	--当前类型数值
	local pro_value = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "power_num", true))
    pro_value:SetAnchorPoint(not is_show_star and 0.5 or 0, 0.5)  
    self:GetNode():AddChild(pro_value:GetNode())	    	    	
    pro_value:SetPosition(not is_show_star and 541 or 528,48.5)	
	if data.rankType == Macros.Game.RankType.LEVEL then
		--另一种属性
		Util:WidgetLabel(txt_anotherprotype, CPPGameLib.GetString("rank_typeText1"), Macros.TypefaceSize.normal , Macros.Color.keypoint)
		--另一种类型数值
		Util:WidgetLabel(txt_protypevalue, data.battle_value, Macros.TypefaceSize.normal , Macros.Color.content)
		--当前类型数值	
	    pro_value:SetText(tostring(data.level))
	elseif is_show_star then
		--另一种属性
		Util:WidgetLabel(txt_anotherprotype, CPPGameLib.GetString("rank_typeText2"), Macros.TypefaceSize.normal , Macros.Color.keypoint)
		--另一种类型数值
		Util:WidgetLabel(txt_protypevalue, data.level, Macros.TypefaceSize.normal , Macros.Color.content)
		--当前类型数值	
	    pro_value:SetText(tostring(data.context))
	elseif data.rankType == Macros.Game.RankType.BATTLE or data.rankType == Macros.Game.RankType.ARENA then
		--另一种类型属性
		Util:WidgetLabel(txt_anotherprotype, CPPGameLib.GetString("rank_typeText2"), Macros.TypefaceSize.normal , Macros.Color.keypoint)
		--另一种类型数值
		Util:WidgetLabel(txt_protypevalue, data.level, Macros.TypefaceSize.normal , Macros.Color.content)	
		--战力	    
		local battle_value = data.battle_value
	    --当前类型数值	
	    pro_value:SetText(tostring(battle_value))
	    if self.__my_riad then
		   PosTool.RightTo(pro_value, self.__my_riad )
		end
	else
		--另一种属性
		Util:WidgetLabel(txt_anotherprotype, CPPGameLib.GetString("rank_typeText1"), Macros.TypefaceSize.normal , Macros.Color.keypoint)
		--另一种类型数值
		Util:WidgetLabel(txt_protypevalue, data.battle_value, Macros.TypefaceSize.normal , Macros.Color.content)
		--当前类型数值	
	    pro_value:SetText(tostring(data.context))
	end		
end

