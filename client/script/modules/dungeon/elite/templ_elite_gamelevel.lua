
Modules = Modules or {}

--精英副本关卡模板
Modules.TemplEliteDungeonGameLevel = Modules.TemplEliteDungeonGameLevel or BaseClass(GUI.Template)

function Modules.TemplEliteDungeonGameLevel:__init(parent_node)
	self:InitTemplate(GUI.TemplateRegistry.ELITE_DUNGEON_GAMELEVEL, parent_node)
	self.__txt_name = Util:WidgetLabel(self:GetWidget("Text_GameLevel"), "", Macros.TypefaceSize.normal)
	self.__panel_heroDungeon = self:GetWidget("Panel_heroDungeon")
	self.__img_challenged = self:GetWidget("Image_Challenged")
	self.__img_box = self:GetWidget("Image_Box")
	self.__panel_box_special_effect = self:GetWidget("Panel_boxSpecialEffect")

	self.__img_challenged:SetVisible(false)

	--点击关卡
	WidgetUnity.SetWidgetListener(self.__panel_heroDungeon, nil, nil, function()
		self:__HandleGamelevelClick()
	end)

	--点击宝箱
	WidgetUnity.SetWidgetListener(self.__img_box, nil, nil, function()
		self:__HandleBoxClick()
	end)

	--点击宝箱
	WidgetUnity.SetWidgetListener(self.__panel_box_special_effect, nil, nil, function()
		self:__HandleBoxClick()
	end)

	--监听宝箱领取响应事件
	self.__award_get_event = self:BindGlobalEvent(Macros.Event.DungeonModule.DUNGEON_AWARD_GET_RESP, function(_ ,award_type, index)
		if award_type == Macros.Game.FBRewardType.GAMELEVEL and self.__data.gamelevel_index == index then
			self:__ShowBox()
		end
	end)
end

function Modules.TemplEliteDungeonGameLevel:__delete()
	if self.__hero_module then
		self.__hero_module:DeleteMe()
		self.__hero_module = nil
	end
	if self.__fightting_effect_id then
		self:__DelEffect(self.__fightting_effect_id)
		self.__fightting_effect_id = nil
	end
	if self.__light_effect_id then
		self:__DelEffect(self.__light_effect_id)
		self.__light_effect_id = nil
	end
	self:DeleteSpecialEffect()
end

function Modules.TemplEliteDungeonGameLevel:SetData(data)
	GUI.Template.SetData(self, data)

	local gamelevel_info = self.__data.gamelevel_info
	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", self.__data.gamelevel_id)
	if not config_gamelevel then
		return
	end
	--怪物信息
	local monster_info = CPPGameLib.GetConfig("Monster",config_gamelevel.icon_id) 
	if not self.__hero_module then	
		self.__hero_module = LogicGUI.Model.New({info_id = monster_info.hero_id and monster_info.hero_id or monster_info.head_icon, parent = self.__panel_heroDungeon})
		self.__hero_module:SetScaleFactor(0.8)
		self.__hero_module:SetPosition(60,0)
	end
	if not gamelevel_info then
		self.__hero_module:SetGray(true)
		self.__hero_module:SetToPlayTime(0)
		--self.__hero_module:SetVisible(false)
	else
		self.__hero_module:SetGray(false)
		self.__hero_module:SetToPlayTime(nil)
	end
	--关卡名
	self.__txt_name:SetText(config_gamelevel.name or "")
	self.__txt_name:SetColor(Macros.QualityColor(monster_info.quality))
	self.__txt_name:SetStrokeColor(Macros.QualityColorStroke(monster_info.quality))
	--特效
	--self:__ShowEffect()

	--星级
	self:__ShowStar()

	--宝箱
	self:__ShowBox()

end

function Modules.TemplEliteDungeonGameLevel:__ShowEffect()
	local gamelevel_info = self.__data.gamelevel_info
	local flag = Cookies.Get(Cookies.LevelType.ROLE, string.format("%s_%d", Macros.Setting.EliteDungeonGameLevelEffect, self.__data.gamelevel_id)) ~= "1"
	if gamelevel_info and gamelevel_info.no_passed_flag then
		local effect
		if flag then
	        self.__light_effect_id, effect = self:__CreateEffect({id = 100003,loop = false}, self.__root_widget,3)
			PosTool.Center(effect,0,-75)				
			Cookies.Set(Cookies.LevelType.ROLE, string.format("%s_%d", Macros.Setting.EliteDungeonGameLevelEffect, self.__data.gamelevel_id), "1")
		end	
		self.__fightting_effect_id, effect = self:__CreateEffect({res_name = "UI_xiaodao",scale = 0.7}, self.__root_widget,2)
		PosTool.CenterTop(effect,5,95)	
	end
end

function Modules.TemplEliteDungeonGameLevel:__ShowBox()
	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", self.__data.gamelevel_id)
	local has_box = config_gamelevel and config_gamelevel.box_id_list ~= nil
	self.__panel_box_special_effect:SetVisible(false)	
	self.__img_box:SetVisible(has_box)	
	if has_box then
		local gamelevel_info = self.__data.gamelevel_info
		local box_imgdata = (not gamelevel_info or gamelevel_info.box_award_flag == Modules.DungeonModule.BoxStatus.NONE) and 
		Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON_GAMELEVEL, "elite_box_close") or 
		Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON_GAMELEVEL, "elite_box_open")
		self.__img_box:IgnoreContentAdaptWithSize(true)
		self.__img_box:SetImage(box_imgdata)
		self:DeleteSpecialEffect()
		local effect
		if not gamelevel_info or gamelevel_info.box_award_flag == Modules.DungeonModule.BoxStatus.NONE then
			self.game_level_box_specialeffect_none, effect = self:__CreateEffect({res_name = "UI_baoxiangdaiji"}, self.__img_box,1)
			PosTool.Center(effect)
		elseif gamelevel_info and gamelevel_info.box_award_flag == Modules.DungeonModule.BoxStatus.CAN_RECEIVE then
			self.game_level_box_specialeffect_receive, effect = self:__CreateEffect({res_name = "UI_baoxiangtiaoyue"}, self.__panel_box_special_effect,1)
			PosTool.Center(effect,0,10)
			self.__panel_box_special_effect:SetVisible(true)
			self.__img_box:SetVisible(false)
		else
		end
	end
end

function Modules.TemplEliteDungeonGameLevel:__ShowStar()
	local gamelevel_info = self.__data.gamelevel_info
	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", self.__data.gamelevel_id)
	
	local star
	for i = 1, Macros.Game.FBMaxStarPerMap do
		star = self:GetWidget("Image_Star"..i)
		if gamelevel_info then
			star:SetGray(gamelevel_info.star < i)
		else
			star:SetVisible(false)
		end
	end
end

function Modules.TemplEliteDungeonGameLevel:DeleteSpecialEffect()
	if self.game_level_box_specialeffect_none then
		self:__DelEffect(self.game_level_box_specialeffect_none)
		self.game_level_box_specialeffect_none = nil
	end
	if self.game_level_box_specialeffect_receive then
		self:__DelEffect(self.game_level_box_specialeffect_receive)
		self.game_level_box_specialeffect_receive = nil
	end
end

function Modules.TemplEliteDungeonGameLevel:__HandleGamelevelClick()
	local gamelevel_info = self.__data.gamelevel_info
	if not gamelevel_info then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("dungeon_challenge_tips2"))
		return
	end

	--打开挑战界面		
	Modules.TemplDungeonGameLevelChallenge.New(self.__data)		
end

function Modules.TemplEliteDungeonGameLevel:__HandleBoxClick()
	--打开宝箱信息界面
	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", self.__data.gamelevel_id)
	local params = {}
	params.title = CPPGameLib.GetString("dungeon_elite_box")
	params.reward_list = ConfigAdapter.Item.GetGiftPackRewardListII(config_gamelevel.box_id_list, config_gamelevel.box_num_list)
	params.type = Macros.Game.FBRewardType.GAMELEVEL
	params.status = self.__data.gamelevel_info and self.__data.gamelevel_info.box_award_flag
	params.condition = string.format(CPPGameLib.GetString("dungeon_can_receive_after", config_gamelevel.name))
	params.chapter_index = self.__data.chapter_index
	params.index = self.__data.gamelevel_index
	params.fb_type = Macros.Game.FBType.ELITE_DUNGEON
	Modules.TemplDungeonBoxInfo.New(params)
end

--重设数据
function Modules.TemplEliteDungeonGameLevel:RestData(data)
	GUI.Template.SetData(self, data)
end

