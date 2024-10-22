
Modules = Modules or {}

--助战英雄操作模板
Modules.TemplBattleHeroOperAssist = Modules.TemplBattleHeroOperAssist or BaseClass(GUI.Template)

--[[
@ params：
	parent_node
	info_id
--]]
function Modules.TemplBattleHeroOperAssist:__init(params)
	self.__layout_mode = GUI.Template.LayoutMode.CODE	

	self.__params = params

	self:InitTemplate(nil, params.parent_node)

	WidgetUnity.SetWidgetListener(self.__root_widget, nil, nil, function()
		self:__HandleAssist()
	end)
end

function Modules.TemplBattleHeroOperAssist:__delete()
end

function Modules.TemplBattleHeroOperAssist:__RegistAllEvents()
end

function Modules.TemplBattleHeroOperAssist:__CreateWidgetFromLuaCode()
	local layout = Util:Layout()
	layout:SetAnchorPoint(0, 0)
	layout:SetContentSize(100, 108)
	layout:SetTouchEnabled(true)

	--头像
	local sp_head_bg = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE, "help_bg"))
	sp_head_bg:SetAnchorPoint(0.5, 0.5)
	layout:AddChild(sp_head_bg)
	PosTool.Center(sp_head_bg, 0 , 10 )

	local sp_head_border = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.BATTLE, "help_frame"))
	sp_head_border:SetAnchorPoint(0.5, 0.5)
	sp_head_bg:AddChild(sp_head_border)
	PosTool.Center(sp_head_border)

	local config_hero = CPPGameLib.GetConfig("Hero", self.__params.info_id)

	self.__sp_head = Util:Sprite(Resource.PathTool.GetHeroHeadPath(config_hero and config_hero.head_icon or 0))
	self.__sp_head:SetAnchorPoint(0.5, 0.5)
	sp_head_bg:AddChild(self.__sp_head)
	PosTool.Center(self.__sp_head , 0 , 0 )

	--助战标记
	local dungeon_assist_flag = Util:Label( CPPGameLib.GetString("dungeon_assist_flag") , Macros.TypefaceSize.normal , Macros.Color.itemName )
	dungeon_assist_flag:SetAnchorPoint( 0.5 , 0.5 )
	layout:AddChild(dungeon_assist_flag)
	PosTool.Center(dungeon_assist_flag , 0 , -43 )

	--创建可释放特效
	local effect_root
	self.__can_released_effect_id, effect_root = GlobalEffectMgr:CreateUIEffect({res_name = "UI_zhandoujineng1", loop = true}, sp_head_bg)
	PosTool.Center(effect_root, 0 , 0)

	return layout
end

function Modules.TemplBattleHeroOperAssist:__CreateLeadEffect()
	--创建可释放特效
	local effect_root
	self.__friend_assist_finger_effect_id,effect_root = GlobalEffectMgr:CreateUIEffect({res_name = "UI_zhiyinshouzhi",loop = true}, self:GetNode())
	PosTool.Center(effect_root, 0, 0)
	self.__friend_assist_lead_effect_id, effect_root = GlobalEffectMgr:CreateUIEffect({id = 100054, loop = true,time_scale = 0.8}, self:GetNode())
	PosTool.Center(effect_root, 0, 0)
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--处理助战(创建助战英雄、释放怒气技能)
function Modules.TemplBattleHeroOperAssist:__HandleAssist()
	if CallModuleFunc(ModuleType.BATTLE, "CreateHeroAssist") then
		--移除可释放特效
		if self.__can_released_effect_id then
			GlobalEffectMgr:DelEffect(self.__can_released_effect_id)
			self.__can_released_effect_id = nil
		end
		--移除指引特效特效
		if self.__friend_assist_finger_effect_id then
			GlobalEffectMgr:DelEffect(self.__friend_assist_finger_effect_id)
			self.__friend_assist_finger_effect_id = nil
		end
		if self.__friend_assist_lead_effect_id then
			GlobalEffectMgr:DelEffect(self.__friend_assist_lead_effect_id)
			self.__friend_assist_lead_effect_id = nil
		end
	end
end
