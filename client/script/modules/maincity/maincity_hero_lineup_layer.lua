
--[[
%% @module：主城上阵英雄模板
%% @author: swordman sue
%% @created: 2016-11-05
--]]

Modules = Modules or {}

Modules.MaincityHeroLineupLayer = Modules.MaincityHeroLineupLayer or BaseClass(UILayer)

function Modules.MaincityHeroLineupLayer:__init(is_empty, lineup_index)
	self.__is_empty = is_empty
	self.__lineup_index = lineup_index

	local bg_shadow = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY, "hero_shadow"))
	self:AddChild(bg_shadow, -2)

	--空阵位
	if self.__is_empty then
		self:UpdateEmptyLineupPos()
		return
	end

	self.__bg_hero_level = Util:Sprite9(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY, "bg_hero_level")) 
	self.__bg_hero_level:SetContentSize(100, 26)
	self.__bg_hero_level:SetAnchorPoint(0.5, 0)
	self.__bg_hero_level:SetPosition(0, 160)
	self.__bg_hero_level:SetZOrder(8)
	self:AddChild(self.__bg_hero_level)

	self.__txt_hero_name = Util:Label(nil, Macros.TypefaceSize.normal, nil, nil, nil)
	self.__txt_hero_name:SetAnchorPoint(0, 0.5)
	self.__txt_hero_name:SetStrokeWidth(2)
	self.__bg_hero_level:AddChild(self.__txt_hero_name)
	PosTool.LeftCenter(self.__txt_hero_name,45,-2)

	self.__img_camp = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "camp_icon_1", true))
	self.__img_camp:SetAnchorPoint(0, 0.5)
	self.__img_camp:SetScaleFactor(0.6)
	self.__bg_hero_level:AddChild(self.__img_camp)
	PosTool.LeftCenter(self.__img_camp, -10,3)
end

function Modules.MaincityHeroLineupLayer:__delete()
	if self.__ui_hero then
		self.__ui_hero:DeleteMe()
		self.__ui_hero = nil
	end
	--精灵上特效
	if self.__bless_spirit_effectid then
		self:__DelEffect(self.__bless_spirit_effectid)
		self.__bless_spirit_effectid = nil
	end
	--精灵下特效
	if self.__down_bless_spirit_effectid then
		self:__DelEffect(self.__down_bless_spirit_effectid)
		self.__down_bless_spirit_effectid = nil
	end
end

function Modules.MaincityHeroLineupLayer:SetData(lineup_index, hero_info)
	self.__lineup_index = lineup_index
	self.__hero_uid = hero_info.uid

	if not hero_info then
		return
	end
	local config_hero = CPPGameLib.GetConfig("Hero", hero_info.info_id)
	if not config_hero then
		return
	end

	--名字
	local name_str = config_hero.name
	self.__txt_hero_name:SetString(name_str)

	--阵营
	self.__img_camp:SetImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "camp_icon_"..config_hero.camp, true))

	--模型
	if not self.__ui_hero then
		self.__ui_hero = LogicGUI.Model.New({info_id = hero_info.info_id, parent = self:GetNode(), zorder = -1})
		self.__ui_hero:SetScaleFactor(config_hero.city_scale or 1)
		self.__ui_hero:SetTouchRect(-50, 0, 100, 200)
		self.__ui_hero:SetTouchFun(nil, nil, function()
			GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.HERO_LINEUP, {BaseViewSubType.HeroLineup.MAIN, lineup_index})
		end)
	else
		self.__ui_hero:SetInfoID(hero_info.info_id)
	end

	--英雄等级和进阶等级
	self:UpdateUpgradeOrBreak()

	--进化信息
	self:UpdateEvolution()

	--觉醒星级
	self:UpdateAwaken()

	--精灵祝福特效
	self:__ShowSpiritBlessEffectId(config_hero.city_scale)
end

--更新等级和进阶信息
function Modules.MaincityHeroLineupLayer:UpdateUpgradeOrBreak()
	if not self.__is_empty then	
		local isCouldUpgrade = CallModuleFunc(ModuleType.HERO,"JudgeIsCouldUpgrade", self.__hero_uid)  
		local isCouldBreak = CallModuleFunc(ModuleType.HERO,"JudgeIsCouldBreak", self.__hero_uid) 
		if isCouldBreak or isCouldUpgrade then
			local str = isCouldBreak and CPPGameLib.GetString("hero_could_break") or CPPGameLib.GetString("hero_could_upgrade")
			self.__ui_hero:AddUpgradeTips(str)
		else
			self.__ui_hero:HideUpgradeTips()
		end
	end
end

--更新英雄进化
function Modules.MaincityHeroLineupLayer:UpdateEvolution()
	if not self.__is_empty then	
		local hero_info = CallModuleFunc(ModuleType.HERO, "GetHeroInfo", self.__hero_uid)
		self.__txt_hero_name:SetColor(Macros.QualityColor(hero_info.quality_level))
		self.__txt_hero_name:SetStrokeColor(Macros.QualityColorStroke(hero_info.quality_level))
	end
end

--更新觉醒信息
function Modules.MaincityHeroLineupLayer:UpdateAwaken()
	if not self.__is_empty then	
		local hero_info = CallModuleFunc(ModuleType.HERO, "GetHeroInfo", self.__hero_uid)
		self:__ShowAwakenStar(hero_info)
	end
end

--更新空阵位
function Modules.MaincityHeroLineupLayer:UpdateEmptyLineupPos()
	if not self.__is_empty then
		return
	end

	local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	local hero_config = CPPGameLib.GetConfig("HeroLineup",Macros.Game.HeroLineup.HERO_LINEUP)
	local lineup_pos_info = hero_config and hero_config.pos_list and hero_config.pos_list[self.__lineup_index]

	--已达到开启等级，显示添加按钮
	if lineup_pos_info and role_level >= lineup_pos_info.need_level then
		if not self.__icon_add then
			self.__icon_add = UIImageViewEx.CreateWithImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_add"))
			self.__icon_add:SetTouchEnabled(true)
			self:AddChild(self.__icon_add, 1)
			PosTool.CenterBottom(self.__icon_add,0,80)
			WidgetUnity.SetWidgetListener(self.__icon_add, nil, nil,function ()
				CallModuleFunc(ModuleType.HERO,"OpenView", BaseViewType.HERO_LINEUP, BaseViewSubType.HeroLineup.MAIN, self.__lineup_index)
			end)

			local function RunFadeoutAction()
				CPPActionManager.FadeOut(self.__icon_add, 0.5, self.__addicon_fadeout_func)
			end

			--加号动作
			self.__addicon_fadeout_func = GlobalCallbackMgr:AddCallBackFunc(function ()
				CPPActionManager.FadeIn(self.__icon_add, 0.5, GlobalCallbackMgr:AddCallBackFunc(function()
					RunFadeoutAction()
				end))	
			end)	
			RunFadeoutAction()
		end

		if self.__lock_text then
			self.__lock_text:RemoveSelf(true)
			self.__lock_text = nil
		end
		if self.__lock_icon then
			self.__lock_icon:RemoveSelf(true)
			self.__lock_icon = nil
		end

	--未达到开启等级，显示锁上的图标
	else
		if not self.__lock_text then
			self.__lock_text = Util:Label(CPPGameLib.GetString("hero_lock_text",lineup_pos_info.need_level),Macros.TypefaceSize.slightly,nil,0,0,Macros.Color.cityblack_stroke)
			self:AddChild(self.__lock_text, 1)			
			PosTool.CenterBottom(self.__lock_text,0,35)				
		end

		if not self.__lock_icon then		
			self.__lock_icon = UIImageViewEx.CreateWithImage(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "lock_icon"))
			self.__lock_icon:SetTouchEnabled(true)
			self:AddChild(self.__lock_icon)	
			PosTool.CenterBottom(self.__lock_icon,0,70)

			WidgetUnity.SetWidgetListener(self.__lock_icon, nil, nil,function ()
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_lock_text_tips",lineup_pos_info.need_level))
			end)		
		end

		if self.__icon_add then
			self.__icon_add:RemoveSelf(true)
			self.__icon_add = nil
		end
	end
end

function Modules.MaincityHeroLineupLayer:__ShowAwakenStar(hero_info)
	self.__awaken_star_list = self.__awaken_star_list or {}
	for _, sp_star in ipairs(self.__awaken_star_list) do
		sp_star:SetVisible(false)
	end

	--觉醒星数
	local star_img = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"bigstar")
	local org_pos = -(hero_info.awaken_star - 1) * 28/2
	local org_dis
	if hero_info.awaken_star > 0 then
		org_dis = hero_info.awaken_star == 1 and 0 or math.abs(org_pos/(hero_info.awaken_star-1))*2
	end	

	local sp_star
	for i = 1, hero_info.awaken_star do
		sp_star = self.__awaken_star_list[i]
		if not sp_star then
			sp_star = Util:Sprite(star_img)
			sp_star:SetAnchorPoint(0.5, 0.5)
			self:AddChild(sp_star, 2)
			table.insert(self.__awaken_star_list, sp_star)
		end
		sp_star:SetVisible(true)
		sp_star:SetPosition((i-1)*org_dis+org_pos,160)
	end

	if hero_info.awaken_star > 0 then
		self.__bg_hero_level:SetPosition(0,180)
	end
end

--新手指引特殊操作检测（主界面-英雄阵位（索引，从1开始））
function Modules.MaincityHeroLineupLayer:CheckSectionOperateSP()
	if self.__icon_add then
		CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.CITY_HERO_LINEUP_BTN, self.__icon_add, self.__lineup_index)								
	end
end

--------------------------------------------------------------------------------------------------------------------------------
--保护函数
--------------------------------------------------------------------------------------------------------------------------------
--精灵祝福特效
function Modules.MaincityHeroLineupLayer:__ShowSpiritBlessEffectId(city_scale)
	local lineup_info = CallModuleFunc(ModuleType.HERO,"GetLineupInfo",self.__lineup_index)
	--精灵上特效
	if self.__bless_spirit_effectid then
		self:__DelEffect(self.__bless_spirit_effectid)
		self.__bless_spirit_effectid = nil
	end
	--精灵下特效
	if self.__down_bless_spirit_effectid then
		self:__DelEffect(self.__down_bless_spirit_effectid)
		self.__down_bless_spirit_effectid = nil
	end
	if lineup_info.bless_sprite_id ~= 0 then
		local spirit_config = CPPGameLib.GetConfig("Spirit",lineup_info.bless_sprite_id)
		--精灵下特效
		local effect
		if spirit_config and spirit_config.down_bless_effect_id then
			self.__down_bless_spirit_effectid,effect = self:__CreateEffect({id = spirit_config.down_bless_effect_id,scale = city_scale or 1}, self:GetNode(), -2)	
			PosTool.CenterBottom(effect)	
		end
		--精灵上特效
		if spirit_config and spirit_config.bless_effect_id then
			self.__bless_spirit_effectid,effect = self:__CreateEffect({id = spirit_config.bless_effect_id,scale = city_scale or 1}, self:GetNode(),2)	
			PosTool.CenterBottom(effect)	
		end
	end
end

