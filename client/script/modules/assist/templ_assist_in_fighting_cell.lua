Modules = Modules or {}

Modules.TemplDungeonAssistInFightingCell = Modules.TemplDungeonAssistInFightingCell or BaseClass(GUI.ScrollItem)

function Modules.TemplDungeonAssistInFightingCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.DUNGEON_ASSIST_IN_FIGHTING_CELL)
end

function Modules.TemplDungeonAssistInFightingCell:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	--特殊情况显示
	local is_sp_visible = false
	local is_lock_visible = false
	if self.__data.target_type == Macros.Game.ASSIST_TAGGET.GUILD then
		--军团
		if CallModuleFunc(ModuleType.ROLE, "GetPropValue", Macros.Game.RolePropType.GUILD_ID) == 0 then
			is_sp_visible = true
			is_lock_visible = true
		else
			is_sp_visible = not self.__data.id and true or false
		end		    		
	else
		is_sp_visible = not self.__data.id and true or false
	end
	--裁剪半身像容器
	self.__panel_cut = self:GetWidget("Panel_cut")
	self.__panel_cut:SetVisible(not is_sp_visible)
	--半身像
	self.__img_halfhead = self:GetWidget("Image_HalfHead")
	if self.__data.id then
		local config_hero = CPPGameLib.GetConfig("Hero",self.__data.hero_info_id)
		local res_name = Resource.PathTool.GetHalfHeadPath(config_hero and config_hero.half_head or self.__data.hero_info_id)
		self.__img_halfhead:IgnoreContentAdaptWithSize(true)
		self.__img_halfhead:SetImage(res_name,TextureResType.LOCAL)
		--self.__img_halfhead:SetFlipX(true)
	end
	--选择框
	self.__img_selectedframe = self:GetWidget("Image_SelectedFrame")
	self.__img_selectedframe:SetVisible(false)
	--目标标签
	local image_tag = self:GetWidget("Image_TargetTag")
	local res_str = self.__data.target_type == Macros.Game.ASSIST_TAGGET.GUILD and "guild_icon" or (self.__data.target_type == Macros.Game.ASSIST_TAGGET.FRIEND and "friend_icon" or "stranger_icon") 
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.SIXTH_MAN, res_str)
	image_tag:SetImage(sp)
	--锁
	self:GetWidget("Image_LockIcon"):SetVisible(is_lock_visible)
	--提示文字
	local txt_locktips = self:GetWidget("Text_LockTips")
	txt_locktips:SetVisible(is_lock_visible)
	local spirit_system_info = CPPGameLib.GetConfig("SystemEntry",Macros.Game.SystemID.UNION)
	Util:WidgetLabel(txt_locktips,CPPGameLib.GetString("assist_union_lock_tips",spirit_system_info.level), Macros.TypefaceSize.slightly, Macros.Color.Special,142,48)
	--屏蔽层
	self:GetWidget("Panel_shield"):SetVisible(is_sp_visible)

	--友情点图标
	local img_friendpoint = self:GetWidget("Image_friendpoint")
	img_friendpoint:SetVisible(not is_sp_visible)
	--友情点点数
	local txt_friendpointnum = self:GetWidget("Text_friendPointNum")
	txt_friendpointnum:SetVisible(not is_sp_visible)
	--玩家名字
	local text_playername = self:GetWidget("Text_PlayerName")
	text_playername:SetVisible(not is_sp_visible)
	--等级
	local txt_playerlevel = self:GetWidget("Text_PlayerLevel")
	txt_playerlevel:SetVisible(not is_sp_visible)
	--战力
	local img_power = self:GetWidget("Image_Power")
	img_power:SetVisible(not is_sp_visible)
	--战力容器
	local panel_powernum = self:GetWidget("Panel_PowerNum")
	panel_powernum:SetVisible(not is_sp_visible)
	--是否有好友标识
	local img_nofriendtag = self:GetWidget("Image_NoFriendTag")
	img_nofriendtag:SetVisible(is_sp_visible and not is_lock_visible and true or false)
	Util:WidgetLabel(self:GetWidget("Text_NoFriendTips"),CPPGameLib.GetString("assist_no_assist_tips"..self.__data.target_type), Macros.TypefaceSize.slightly, Macros.Color.btn,142,48)
	if self.__data.id then
	    --战斗力	    
		local power_font = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "maincity_battle_num", true))
	    power_font:SetAnchorPoint(0, 0.5)
	    panel_powernum:AddChild(power_font:GetNode())
	    PosTool.LeftCenter(power_font)

	    --战力	    		
	    local battle_value = self.__data.fighting_power
	    if battle_value >= 1000000 then
	    	battle_value = math.floor(battle_value/10000 )
			---万字
		    self.__my_riad = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "myriad", true) )
		    panel_powernum:AddChild(self.__my_riad)
	    end	    
	    --当前类型数值	
	    power_font:SetText(tostring(battle_value))
	    if self.__my_riad then
		   PosTool.RightTo(power_font,self.__my_riad)
		end

	    --玩家名字
	    Util:WidgetLabel(text_playername,self.__data.name, Macros.TypefaceSize.slightly, Macros.Color.orange,0,0,Macros.Color.orange_stroke)
	    --等级数量
	    Util:WidgetLabel(txt_playerlevel,CPPGameLib.GetString("common_lev9",tostring(self.__data.level)), Macros.TypefaceSize.slightly, Macros.Color.btn,0,0,Macros.Color.lilyBlack_stroke,2)
	    --友情点图标
	    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_MONEYICON, "type_"..Macros.Game.RolePropType.TOTAL_FRIENDSHIP_POINT)
		img_friendpoint:SetImage(sp)
		img_friendpoint:SetVisible(self.__data.target_type ~= Macros.Game.ASSIST_TAGGET.STRANGER)
		Util:WidgetLabel(txt_friendpointnum,CPPGameLib.GetString("common_add9",ConfigAdapter.Common.GetAssistFriendPoint()), Macros.TypefaceSize.slightly, Macros.Color.btn,0,0,Macros.Color.lilyBlack_stroke,2)
	    txt_friendpointnum:SetVisible(self.__data.target_type ~= Macros.Game.ASSIST_TAGGET.STRANGER)
	
	    --VIP图标
	    local vip_icon_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_vip_bg", true)
	    local vip_icon = Util:Sprite(vip_icon_sp)	    
	    self:GetNode():AddChild(vip_icon)
	    vip_icon:SetScaleFactor(0.6)
	    PosTool.LeftTop(vip_icon,28,-75)
	    --VIP数字
	    self.__vip_font = GUI.ImgFont.New(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "poodlenum", true))
	    self.__vip_font:SetText(tostring(self.__data.vip_level))
	    self.__vip_font:SetAnchorPoint(0.5, 0.5)
	    vip_icon:AddChild(self.__vip_font:GetNode())	
	    PosTool.Center(self.__vip_font, 0, 1)
	end
end

function Modules.TemplDungeonAssistInFightingCell:SetSelected(var)
	self.__img_selectedframe:SetVisible(var)
	if var and not self.__selected_effectid then
		local effect
		self.__selected_effectid,effect = self:__CreateEffect({res_name = "UI_haoyouzhuzhan",loop = true},self:GetNode(),10)
		effect:SetPosition(96,129)
	elseif not var and self.__selected_effectid then
		self:__DelEffect(self.__selected_effectid)
		self.__selected_effectid = nil
	end
end