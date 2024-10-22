
Modules = Modules or {}

--主线副本关卡模板
Modules.TemplDungeonGameLevel = Modules.TemplDungeonGameLevel or BaseClass(GUI.Template)

local IconType = 
{
	MONSTER = 1,
	ELITE_MONSTER = 2,
	BOSS = 3,
}

function Modules.TemplDungeonGameLevel:__init(parent_node)
	self:InitTemplate(GUI.TemplateRegistry.DUNGEON_GAMELEVEL, parent_node)

	self.__txt_name = Util:WidgetLabel(self:GetWidget("Text_GameLevel"), "", Macros.TypefaceSize.largish, Macros.Color.blinkcontent, nil, nil, Macros.Color.common_stroke)

	self:__SetUI(  )

	self.__img_box = self:GetWidget("Image_Box")
	self.__image_bottom = self:GetWidget("Image_bottom")
	self.__panel_box_special_effect = self:GetWidget("Panel_boxSpecialEffect")

	--点击关卡
	WidgetUnity.SetWidgetListener(self.__root_widget, nil, nil, function()
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
	self.__award_get_event = self:BindGlobalEvent(Macros.Event.DungeonModule.DUNGEON_AWARD_GET_RESP, function(_ , award_type, index)
		if award_type == Macros.Game.FBRewardType.GAMELEVEL and self.__data.gamelevel_index == index then
			self:__ShowBox()
		end
	end)		
end

function Modules.TemplDungeonGameLevel:__delete()
	if self.__light_effect_id then
		self:__DelEffect(self.__light_effect_id)
		self.__light_effect_id = nil
	end
	if self.__world_heart_effect_id then
		self:__DelEffect(self.__world_heart_effect_id)
		self.__world_heart_effect_id = nil
	end
end

function Modules.TemplDungeonGameLevel:SetData(data)
	GUI.Template.SetData(self, data)

	local gamelevel_info = self.__data.gamelevel_info
	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", self.__data.gamelevel_id)
	if not config_gamelevel then
		return
	end
	
	self.__is_boss = config_gamelevel.icon_type == IconType.BOSS
	self.__is_normal = config_gamelevel.icon_type == IconType.MONSTER 

	local img_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON_GAMELEVEL, gamelevel_info and "open_point" or "close_point")
	self.__image_bottom:SetImage(img_data)
	--关卡名
	self.__txt_name:SetText(config_gamelevel.name or "")
	local color = self.__is_boss and Macros.Color.purple or (self.__is_normal and Macros.Color.btn or Macros.Color.blue)
	local stroke_color = self.__is_boss and Macros.Color.purple_stroke or (self.__is_normal and Macros.Color.common_stroke or Macros.Color.blue_stroke)
	self.__txt_name:SetColor(unpack(color))
	self.__txt_name:SetStrokeColor(unpack(stroke_color))
	--是否是精英
	local is_middle = false
	if self.__is_boss then
		PosTool.CenterBottom(self.__txt_name, 0, 130)
		PosTool.CenterBottom(self.__image_bottom, 0, -25)
	elseif self.__is_normal then
		PosTool.CenterBottom(self.__txt_name, 0, 110)
		PosTool.CenterBottom(self.__image_bottom, 0,7)
	else
		is_middle = true
		PosTool.CenterBottom(self.__txt_name, 0, 120)
		PosTool.CenterBottom(self.__image_bottom, 0,-2)
	end
	self.__txt_name:SetVisible(not self.__is_normal)
	--图标
	self.__img_low_border:SetVisible(self.__is_normal)
	self.__image_middle_border:SetVisible(is_middle)
	self.__img_high_border:SetVisible(self.__is_boss)
	self.__img_low_border:SetGray(not gamelevel_info)
	self.__image_middle_border:SetGray(not gamelevel_info)
	self.__img_high_border:SetGray(not gamelevel_info)

	local img_headbg = self.__is_boss and self:GetWidget("Image_HighHeadBg") or (self.__is_normal and self:GetWidget("Image_LowHeadBg") or self:GetWidget("Image_MiddleHeadBg"))
	img_headbg:SetZOrder(-1)
	self.__clipping_node = Util:CircleClippingNode(0,0,self.__is_normal and 28 or 35)
	img_headbg:AddChild(self.__clipping_node)
	PosTool.Center(self.__clipping_node)

	local img_head 
	if config_gamelevel.icon_id then
		img_head = Util:Sprite(Resource.PathTool.GetHeadPathByObjID(config_gamelevel.icon_id, true))
		img_head:SetScaleFactor(self.__is_boss and 1 or (self.__is_normal and 0.8 or 1))
		self.__clipping_node:AddChild(img_head)		
		PosTool.Center( img_head )
	end
	img_head:SetGray(not gamelevel_info)

	--判断是否加世界之心
	local is_add_world_heart
	if self.__is_boss then
		if gamelevel_info and gamelevel_info.no_passed_flag then
			is_add_world_heart = true
		elseif not gamelevel_info then
			is_add_world_heart = true
		end
	end
	--加世界之心
	if is_add_world_heart then
		local effect
		self.__world_heart_effect_id, effect = self:__CreateEffect({res_name = "shijiezhixin",scale = 0.2},self.__root_widget,2)
		effect:SetAnchorPoint(1,0.5)
		PosTool.LeftCenter(effect,-33,0)	
	end

	--星级
	self:__ShowStar()

	--宝箱
	self:__ShowBox()

	--新手指引特殊操作检测
	self:CheckSectionOperateSP()
end

function Modules.TemplDungeonGameLevel:RunAddAction(node,i)
	ActionManager.GetInstance():RunJumpByAction(node,0.5,0,20,0,0,self["funId_"..i])
end

--新手指引特殊操作检测
function Modules.TemplDungeonGameLevel:CheckSectionOperateSP()
	--选择主线关卡（关卡ID）
	CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.MAIN_DUNGEON_SELECT_GAMELEVEL, self:GetNode(), self.__data.gamelevel_id)				

	--选择主线关卡宝箱（关卡ID）
	CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.MAIN_DUNGEON_SELECT_GAMELEVEL_BOX, self.__panel_box_special_effect, self.__data.gamelevel_id)			
end

function Modules.TemplDungeonGameLevel:__ShowEffect()
	local gamelevel_info = self.__data.gamelevel_info	
	local widget = self.__is_boss and self.__img_high_border or (self.__is_normal and self.__img_low_border or self.__image_middle_border)
	local flag = Cookies.Get(Cookies.LevelType.ROLE, string.format("%s_%d", Macros.Setting.MainDungeonGameLevelEffect, self.__data.gamelevel_id)) ~= "1"	
	if gamelevel_info and gamelevel_info.no_passed_flag then
		if flag then
			local effect,effectid
			self.__light_effect_id, effect = self:__CreateEffect({id = 100003, forbid_sound = CallModuleFunc(ModuleType.STORY, "IsInStory", 1)}, self.__root_widget,2)
			PosTool.Center(effect,0,-30)	
			Cookies.Set(Cookies.LevelType.ROLE, string.format("%s_%d", Macros.Setting.MainDungeonGameLevelEffect, self.__data.gamelevel_id), "1")			
		end        
		self.funId_1 = GlobalCallbackMgr:AddCallBackFunc(function ()
			ActionManager.GetInstance():RunJumpByAction(widget,0.5,0,-20,0,0,GlobalCallbackMgr:AddCallBackFunc(function()
				self:RunAddAction(widget,1)
			end))	
		end)
		self.funId_2 = GlobalCallbackMgr:AddCallBackFunc(function ()
			ActionManager.GetInstance():RunJumpByAction(self.__txt_name,0.5,0,-20,0,0,GlobalCallbackMgr:AddCallBackFunc(function()
				self:RunAddAction(self.__txt_name,2)
			end))	
		end)
		self:RunAddAction(widget,1)	
		self:RunAddAction(self.__txt_name,2)
		--
		effectid, effect = self:__CreateEffect({res_name = "UI_fubendizuo"},self.__image_bottom,2)
		PosTool.Center(effect,0,2)		
	end
end

function Modules.TemplDungeonGameLevel:__ShowStar()
	local gamelevel_info = self.__data.gamelevel_info
	local config_gamelevel = CPPGameLib.GetConfig("GameLevel", self.__data.gamelevel_id)
	
	local star
	for i = 1, Macros.Game.FBMaxStarPerMap do
		star = self.__is_boss and self:GetWidget("Image_HighStar"..i) or (self.__is_normal and self:GetWidget("Image_LowStar"..i)
				or self:GetWidget("Image_MiddleStar"..i))
		if gamelevel_info then
			star:SetGray(gamelevel_info.star < i)
		else
			star:SetVisible(false)
		end
	end
end

function Modules.TemplDungeonGameLevel:__ShowBox()
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
			self.__game_level_box_specialeffect_none, effect = self:__CreateEffect({res_name = "UI_baoxiangdaiji"}, self.__img_box,1)
			PosTool.Center(effect)
		elseif gamelevel_info and gamelevel_info.box_award_flag == Modules.DungeonModule.BoxStatus.CAN_RECEIVE then
			self.__game_level_box_specialeffect_receive, effect = self:__CreateEffect({res_name = "UI_baoxiangtiaoyue"}, self.__panel_box_special_effect,1)
			PosTool.Center(effect,0,10)
			self.__panel_box_special_effect:SetVisible(true)
			self.__img_box:SetVisible(false)
		else
		end
	end
end

function Modules.TemplDungeonGameLevel:DeleteSpecialEffect()
	if self.__game_level_box_specialeffect_none then
		self:__DelEffect(self.__game_level_box_specialeffect_none)
		self.__game_level_box_specialeffect_none = nil
	end
	if self.__game_level_box_specialeffect_receive then
		self:__DelEffect(self.__game_level_box_specialeffect_receive)
		self.__game_level_box_specialeffect_receive = nil
	end
end

function Modules.TemplDungeonGameLevel:__HandleGamelevelClick()
	if not self.__data.gamelevel_info then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("dungeon_challenge_tips2"))
		return
	end
	--屏蔽关卡界面点击
	--self:FireNextFrame(Macros.Event.DungeonModule.SHIELD_GAMELEVEL_TOUCH,true)

	--除了普通关卡都请求留言
	--打开挑战界面		
	Modules.TemplDungeonGameLevelChallenge.New(self.__data)	
	-- if self.__is_normal then
			
	-- else
	-- 	CallModuleFunc(ModuleType.DUNGEON,"MapCommentListReq",self.__data,self.__data.gamelevel_id)
	-- end
end

function Modules.TemplDungeonGameLevel:__HandleBoxClick()
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
	params.fb_type = Macros.Game.FBType.MAIN_DUNGEON
	Modules.TemplDungeonBoxInfo.New(params)
end

--重设数据
function Modules.TemplDungeonGameLevel:RestData(data)
	GUI.Template.SetData(self, data)
end

function Modules.TemplDungeonGameLevel:GetWorldStarEffectID()
	return self.__world_heart_effect_id
end


--设置UI
function Modules.TemplDungeonGameLevel:__SetUI( )

	--普通框
	self.__img_low_border = self:GetWidget("Image_LowBorder")

	--精英头像框
	self.__image_middle_border = self:GetWidget("Image_MiddleBorder")
	self.__image_middle_border:SetScale9Enabled( false )
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON_GAMELEVEL , "Infernal_cover")
	local sp_ = Util:Sprite(sp)
	self.__image_middle_border:SetContentSize( sp_:GetContentWidth() , sp_:GetContentHeight() )

	local Image_MiddleHeadBg = self:GetWidget("Image_MiddleHeadBg")
	Image_MiddleHeadBg:SetPosition( self.__image_middle_border:GetContentWidth()/2 , Image_MiddleHeadBg:GetPositionY() )

	--boss框
	self.__img_high_border = self:GetWidget("Image_HighBorder")
	self.__img_high_border:SetScale9Enabled( false )
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DUNGEON_GAMELEVEL , "boss_cover")
	local sp_ = Util:Sprite(sp)
	self.__img_high_border:SetContentSize( sp_:GetContentWidth() , sp_:GetContentHeight() )

	local Image_HighHeadBg = self:GetWidget("Image_HighHeadBg")
	Image_HighHeadBg:SetPosition( self.__img_high_border:GetContentWidth()/2 , Image_HighHeadBg:GetPositionY() )

	for i = 1 , 3 do
		local lowStar = self:GetWidget( "Image_LowStar" .. i )
		local middleStar = self:GetWidget( "Image_MiddleStar" .. i )
		local highStar1 = self:GetWidget( "Image_HighStar" .. i )
		if i == 1 then
			lowStar:SetPosition( self.__img_low_border:GetContentWidth()/2 - 20 , lowStar:GetPositionY() )
			middleStar:SetPosition( self.__image_middle_border:GetContentWidth()/2 - 20 , middleStar:GetPositionY() )
			highStar1:SetPosition( self.__img_high_border:GetContentWidth()/2 - 20 , highStar1:GetPositionY() )
		elseif i == 3 then
			lowStar:SetPosition( self.__img_low_border:GetContentWidth()/2 + 20 , lowStar:GetPositionY() )
			middleStar:SetPosition( self.__image_middle_border:GetContentWidth()/2 + 20 , middleStar:GetPositionY() )
			highStar1:SetPosition( self.__img_high_border:GetContentWidth()/2 + 20 , highStar1:GetPositionY() )
		else
			lowStar:SetPosition( self.__img_low_border:GetContentWidth()/2 , lowStar:GetPositionY() )
			middleStar:SetPosition( self.__image_middle_border:GetContentWidth()/2 , middleStar:GetPositionY() )
			highStar1:SetPosition( self.__img_high_border:GetContentWidth()/2 , highStar1:GetPositionY() )
		end
	end
end
