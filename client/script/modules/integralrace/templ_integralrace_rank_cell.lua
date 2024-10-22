Modules = Modules or {}
Modules.TemplIntegralRaceRankCell = Modules.TemplIntegralRaceRankCell or BaseClass(GUI.ScrollItem)

function Modules.TemplIntegralRaceRankCell:__init()
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_INTEGRALRACE_RANK_CELL) 
end

function Modules.TemplIntegralRaceRankCell:__delete()
end

function Modules.TemplIntegralRaceRankCell:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	--排名图标
	local Image_rank = self:GetWidget("Image_rank")
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
	Util:WidgetLabel(Text_rank,"", Macros.TypefaceSize.tab , Macros.Color.headline)
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
	Util:WidgetLabel(text_union,str, Macros.TypefaceSize.largish , Macros.Color.keypoint)
	--公会名字
	str = not is_show_star and (data.guild_name ~= "" and data.guild_name or CPPGameLib.GetString("rank_haveNoUnion")) or data.battle_value
	local text_unionname = self:GetWidget("Text_unionname")
	Util:WidgetLabel(text_unionname,str, Macros.TypefaceSize.largish , Macros.Color.content)
	--玩家名字
	Util:WidgetLabel(self:GetWidget("Text_playerName"), data.role_name, Macros.TypefaceSize.largish , Macros.Color.headline )	
	--类型
	Util:WidgetLabel(self:GetWidget("Text_protype"), CPPGameLib.GetString("rank_itemtype_text_"..data.rank_type) or "", Macros.TypefaceSize.largish , Macros.Color.headline )	
	--另一种属性
	Util:WidgetLabel(self:GetWidget("Text_anotherprotype"), CPPGameLib.GetString("rank_typeText1"), Macros.TypefaceSize.largish , Macros.Color.keypoint)
	--另一种类型数值
	Util:WidgetLabel(self:GetWidget("Text_protypevalue"), data.battle_value, Macros.TypefaceSize.largish , Macros.Color.content)
	--累计积分
	local txt_integral = self:GetWidget("Text_Integral")
	Util:WidgetLabel(txt_integral,self.__data.context or 0, Macros.TypefaceSize.largish , Macros.Color.content)
end