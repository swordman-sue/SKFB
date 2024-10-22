Modules = Modules or {}

Modules.TemplMonsterStormCastleIntergralRankCell = Modules.TemplMonsterStormCastleIntergralRankCell or BaseClass(GUI.ScrollItem)

function Modules.TemplMonsterStormCastleIntergralRankCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_MONSTER_STROMCASTLE_RANK_CELL)	
end

function Modules.TemplMonsterStormCastleIntergralRankCell:SetData(data)
	GUI.ScrollItem.SetData(self,data)
	--排名
	local num_Image = self.__data.rank ~= 0 and self.__data.rank <= 3 and Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp_rank_num_"..self.__data.rank) or nil
	--排名
	Util:WidgetLabel(self:GetWidget("Text_rank"),"",Macros.TypefaceSize.popup,Macros.Color.headline)
	if num_Image then
		self:GetWidget("Image_rank"):SetImage(num_Image)
		self:GetWidget("Text_rank"):SetVisible(false)
	else
		self:GetWidget("Image_rank"):SetVisible(false)
		self:GetWidget("Text_rank"):SetText(self.__data.rank)
	end		
	--头像框
	local Panel_roleIcon = self:GetWidget("Panel_roleIcon")
	local roleIcon = LogicGUI.RoleIcon.New(true)
	roleIcon:SetTouchCallback(function()
			-- CallModuleFunc(ModuleType.CHECKROLEINFO, "OpenView", BaseViewType.CHECKROLEINFO,data)
		GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.CHECKROLEINFO , false , data )
    end)
	Panel_roleIcon:AddChild(roleIcon:GetNode())
	PosTool.Center(roleIcon)
	roleIcon:SetData(self.__data.head_icon)
	--名字	
	Util:WidgetLabel(self:GetWidget("Text_playerName"),self.__data.role_name,Macros.TypefaceSize.largish,Macros.Color.headline)
	--当前积分
	Util:WidgetLabel(self:GetWidget("Text_curIntergralTxt"),CPPGameLib.GetString("monster_stormcastle_cur_intergral"),Macros.TypefaceSize.largish, Macros.Color.headline )	
	--当前积分数值
	Util:WidgetLabel(self:GetWidget("Text_curIntergralNum"),self.__data.context,Macros.TypefaceSize.largish,Macros.Color.content)
	--公会
	Util:WidgetLabel(self:GetWidget("Text_groupTxt"),CPPGameLib.GetString("rank_union"),Macros.TypefaceSize.largish,Macros.Color.keypoint)
	--公会名字
	Util:WidgetLabel(self:GetWidget("Text_groupName"),self.__data.guild_name == "" and CPPGameLib.GetString("rank_haveNoUnion") or self.__data.guild_name,Macros.TypefaceSize.largish,Macros.Color.content)
	--战力文字
	Util:WidgetLabel(self:GetWidget("Text_powertext"),CPPGameLib.GetString("arena_powertext"),Macros.TypefaceSize.largish,Macros.Color.keypoint)			
	--战力数值
	Util:WidgetLabel(self:GetWidget("Text_powerNum"),self.__data.battle_value,Macros.TypefaceSize.largish,Macros.Color.content)
end