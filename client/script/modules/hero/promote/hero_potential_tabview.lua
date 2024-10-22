
--[[
%% @module: 英雄潜能界面
%% @author: swordman sue
%% @created: 2016-08-26
--]]

Modules = Modules or {}

Modules.HeroPotentialTabView = Modules.HeroPotentialTabView or BaseClass(BaseTabView)

local out_poy = -185

function Modules.HeroPotentialTabView:__init()
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_BREAK, true)
end

--选项卡界面创建完成回调
function Modules.HeroPotentialTabView:__LoadedCallback()
	self.__txt_cur_lev = Util:WidgetLabel(self:GetWidget("Text_Potential_CurLev"), nil, Macros.TypefaceSize.largish, Macros.Color.green,0,0,Macros.Color.green_stroke)
	self.__txt_cur_lev:SetPosition(self.__txt_cur_lev:GetPositionX()-20,self.__txt_cur_lev:GetPositionY())
	self.__txt_next_lev = Util:WidgetLabel(self:GetWidget("Text_Potential_NextLev"), nil, Macros.TypefaceSize.largish, Macros.Color.green,0,0,Macros.Color.green_stroke)

	self.__panel_cur_prop = self:GetWidget("Panel_Potential_CurProp")
	self.__panel_next_prop = self:GetWidget("Panel_Potential_NextProp")
	self.__image_full_level = self:GetWidget("Image_FullLevel")
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.HERO_BREAK,"full_level",true)
	self.__image_full_level:SetImage(sp)

	self.__txt_potential_value = self:GetWidget("Text_Potential_CurValue")
	self.__txt_potential_value:SetZOrder(3)
	Util:WidgetLabel(self.__txt_potential_value, nil, Macros.TypefaceSize.minimum, Macros.Color.golden )
	
	self.__img_cost_item = self:GetWidget("Image_Potential_CostItem")
	self.__img_cost_item:SetZOrder(3)
	self.__img_cost_item:IgnoreContentAdaptWithSize(true)
	widget = self:GetWidget("Panel_Potential_CostInfo")
	widget:SetZOrder(2)
	self.__richtxt_cost_info = Util:RichText(nil, Macros.TypefaceSize.normal)	
	self.__richtxt_cost_info:SetAnchorPoint(0.5, 0.5)
	widget:AddChild(self.__richtxt_cost_info)
	PosTool.Center(self.__richtxt_cost_info)

	--自动潜能提示背景
	self.__image_autopotential = self:GetWidget("Image_AutoPotential")
	self.__image_autopotential:SetZOrder(10)
	self.__image_autopotential:SetVisible(false)
	--自动潜能提示
	Util:WidgetLabel(self:GetWidget("Text_AutoPotential"), CPPGameLib.GetString("hero_potential_auto_promote_tips"), Macros.TypefaceSize.minimum,Macros.Color.golden)

	--自动潜能中提示背景
	self.__image_on_autopotential = self:GetWidget("Image_OnAutoPotential")
	self.__image_on_autopotential:SetZOrder(8)
	self.__image_on_autopotential:SetVisible(false)
	--自动潜能提示
	Util:WidgetLabel(self:GetWidget("Text_OnAutoPotential"), CPPGameLib.GetString("hero_potential_on_auto_promote_tips"), Macros.TypefaceSize.minimum,Macros.Color.btn)

	--消耗潜能数量
	self.__text_cost_item_num = self:GetWidget("Text_cost_itemnum")
	self.__text_cost_item_num:SetZOrder(3)
	Util:WidgetLabel(self.__text_cost_item_num, nil, Macros.TypefaceSize.slightly,Macros.Color.blinkcontent,0,0,Macros.Color.common_stroke)

	widget = self:GetWidget("Panel_Tips")
	self.__richtxt_potential_probability = Util:RichText(nil, Macros.TypefaceSize.normal,0)	
	self.__richtxt_potential_probability:SetAnchorPoint(0.5, 0.5)
	widget:AddChild(self.__richtxt_potential_probability)
	PosTool.Center(self.__richtxt_potential_probability)

	local txt_costtitle = self:GetWidget("Text_CostTitle")
	Util:WidgetLabel(txt_costtitle, CPPGameLib.GetString("hero_potential_cost_title"), Macros.TypefaceSize.slightly, Macros.Color.blinkcontent,0,0,Macros.Color.common_stroke)
	txt_costtitle:SetZOrder(3)
	
	self:GetWidget("Image_10"):SetZOrder(2)

	local same_act = function (is_auto)
		if self.__delay_timer then
			GlobalTimerQuest:CancelQuest(self.__delay_timer)
			self.__delay_timer = nil
		end	
		--是否自动
		if not is_auto then	
			if not self.__is_on_touching and self.__is_first_bounce then
				self.__is_first_bounce = false
				self:__HandlePotential()
			end					
		end
		self.__is_on_touching = false
		--向上划特效
		if self.__up_arrows_effect_id then
			self:__DelEffect(self.__up_arrows_effect_id)
			self.__up_arrows_effect_id = nil
		end	
		--提示背景
		self.__image_autopotential:SetVisible(false)
		-- if not self.__is_auto_run then
		-- 	self:__HandlePotential()
		-- else
		-- 	self:__StopAutoPotential()
		-- end
	end

	self.__btn_charge = Util:WidgetButton(self:GetWidget("Button_Potential_Charge"))
	self.__btn_charge:SetZOrder(1)
	WidgetUnity.SetWidgetListener(self.__btn_charge, 
		function(x,y)
			self.__touch_posy = y
			self.__is_first_bounce = true
			self.__is_on_touching = false	
		    self.__delay_timer = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
		    	self.__delay_timer = nil
		    	self.__is_on_touching = true
		    	local is_pass = self:__HandlePotential()
		    	--是否开启自动提升
		    	if is_pass then
					local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
					if role_level >= ConfigAdapter.Common.GetHeroPotentialAutoLevelUpOpenCondition() then
						--向上划特效
						self.__up_arrows_effect_id,effect = self:__CreateEffect({res_name = "UI_nuqitouxiangjiaqiang1",loop = true,scale = 0.5},self.__root_widget,10)
						effect:SetPosition(229,120)
						--提示背景
						self.__image_autopotential:SetVisible(true)
					end
				else
					self.__is_first_bounce = false		
				end    	
		    	--self:__StartAutoPotential()
		    end), 0.3)   			
		end, nil, 
		function()
			same_act()
		end,function(x,y)
			local is_auto
			--是否开启自动提升
			local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
			if role_level >= ConfigAdapter.Common.GetHeroPotentialAutoLevelUpOpenCondition() then
				if  y - self.__touch_posy >= 80 and self.__is_on_touching then
					self.__image_on_autopotential:SetVisible(true)
					--特效
					--自动提升特效
					self.__auto_potential_effect_id,effect = self:__CreateEffect({res_name = "UI_qianneng_zidongtisheng_language_chs",loop = true},self.__root_widget,1000)
					effect:SetPosition(0,455)
					self.__is_on_autopotential = true
		    		is_auto = true
		    		if not self.__shield_view then
		    			self.__shield_view = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height, function ()		    				
		    				self:__HideSomeWidget()
		    			end)
		    			self.__shield_view:SetAnchorPoint(0.5,0.5)
		    			self.__parent_view:AddChild(self.__shield_view,Macros.Global.z_order_top+1)
		    			PosTool.Center(self.__shield_view)
		    		else
		    			self.__shield_view:SetVisible(true)
		    		end
				end
			end		
			same_act(is_auto)	
		end,nil,0)	
end

--选项卡界面打开回调
function Modules.HeroPotentialTabView:__OpenCallback()
	local hero_info, config_hero = self:__GetHeroData()

	self:__Reset()
	--英雄详细信息反馈事件
	self.__detail_info_resp_event = self:BindGlobalEvent(Macros.Event.HeroModule.HERO_DETAIL_INFO_RESP, function(_,detail_info)
		if self:IsOpen() and self.__parent_view:GetCurHeroUID() == detail_info.hero_uid then
			self:__ResetDetail(detail_info)
		end
	end)
	self:CallModuleFunc("HeroDetailInfoReq", hero_info.uid)

	--英雄充能反馈事件
	self.__potential_resp_event = self:BindGlobalEvent(Macros.Event.HeroModule.HERO_POTENTIAL_RESP, function(_,resp_info)
		if self:IsOpen() and self.__parent_view:GetCurHeroUID() == resp_info.uid then	
			--天使翅膀
			if self.__angel_resp_effect_id then
				self:__DelEffect(self.__angel_resp_effect_id)
				self.__angel_resp_effect_id = nil
			end
			--天使之剑
			if self.__angel_sword_resp_effect_id then
				self:__DelEffect(self.__angel_sword_resp_effect_id)
				self.__angel_sword_resp_effect_id = nil
			end
			self.__is_on_sending = false	
			--天使翅膀
			self.__angel_resp_effect_id,effect = self:__CreateEffect({res_name = "UI_qianneng3",loop = false},self.__root_widget,2)
			effect:SetPosition(225,225)
			--天使之剑
			self.__angel_sword_resp_effect_id,special_effect = self:__CreateEffect({res_name = "UI_qianneng4",loop = false},self.__clipping_node1,1)
			--进度条进度	
			PosTool.CenterBottom(special_effect,0,out_poy + 158 *(resp_info.potential_exp/self.__cur_need_exp or resp_info.potential_exp))
			if self.__potential_level == resp_info.potential_level then							
				----潜能值增加飘字		
				local faceEffect = TypefaceEffects.New()
				local lab = faceEffect:LittleFloat(CPPGameLib.GetString("hero_potential_tips3", resp_info.potential_exp - self.__potential_value),Macros.Color.btn_hex,1,0,40,nil,nil,nil,Macros.TypefaceSize.largish)
				self.__root_widget:AddChild(lab,4)
				lab:SetPosition(229,130)
				if self.__is_on_touching or self.__is_on_autopotential then
					self:__HandlePotential()
				end
			else
				self:__HideSomeWidget()
				self:__StopAutoPotential()
				--不可点击
				self.__btn_charge:SetTouchEnabled(false)
				--潜能升级
				Modules.TemplHeroPotentialSucc.New(resp_info.uid)
				--恢复点击
				self.__btn_charge:SetTouchEnabled(true)
			end
			self:__ResetDetail(resp_info)
		end
	end)

	local Button_rule = Util:WidgetButton( self:GetWidget("Button_rule") , nil , function ( ... )
		print("xy" , "(3)英雄潜能，小图标（用于说明英雄潜能系统）：" )
        local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
        help:SetLoadedCallback( function ( ... )
           help:SetCell("Rule_Hero_Potency")
        end )
	end )
	Button_rule:SetZOrder(100)
end

function Modules.HeroPotentialTabView:__HideSomeWidget()
	self.__image_on_autopotential:SetVisible(false)
	self.__is_on_touching = false
	self.__is_on_autopotential = false
	--向上划特效
	if self.__up_arrows_effect_id then
		self:__DelEffect(self.__up_arrows_effect_id)
		self.__up_arrows_effect_id = nil
	end	
	--提示背景
	self.__image_autopotential:SetVisible(false)
	--任意背景
	if self.__shield_view then
		self.__shield_view:SetVisible(false)
	end
	--自动提升特效
	if self.__auto_potential_effect_id then
		self:__DelEffect(self.__auto_potential_effect_id)
		self.__auto_potential_effect_id = nil
	end
end

function Modules.HeroPotentialTabView:__AddEffect()
	if self.__effect_id then
		self:__DelEffect(self.__effect_id)
		self.__effect_id = nil
	end
	if self.__angel_effect_id then
		self:__DelEffect(self.__angel_effect_id)
		self.__angel_effect_id = nil
	end
	--天使翅膀
	local effect
	self.__angel_effect_id,effect = self:__CreateEffect({res_name = "UI_qianneng2"},self.__root_widget)
	effect:SetPosition(225,225)
	--遮罩1
	local clipping_sprite = SpriteEx.CreateWithImageFile(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.HERO_PROMOTE, "sword_clippingnode1"))
	self.__clipping_node1 = ClippingNodeEx.Node()
	self.__clipping_node1:SetAlphaTestValue(0)
	self.__clipping_node1:SetStencilMask(clipping_sprite)
	self.__root_widget:AddChild(self.__clipping_node1,5)
	self.__clipping_node1:SetPosition(230,256)
	--遮罩
	local clipping_sprite = SpriteEx.CreateWithImageFile(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.HERO_PROMOTE, "sword_clippingnode"))
	self.__clipping_node = ClippingNodeEx.Node()
	self.__clipping_node:SetAlphaTestValue(0)
	self.__clipping_node:SetStencilMask(clipping_sprite)
	self.__root_widget:AddChild(self.__clipping_node,5)
	self.__clipping_node:SetPosition(230,256)
	self.__effect_id,self.__special_effect = self:__CreateEffect({res_name = "UI_qianneng1",scale = 1},self.__clipping_node,2)
	PosTool.CenterBottom(self.__special_effect,0,out_poy)
end

--选项卡界面关闭回调
function Modules.HeroPotentialTabView:__CloseCallback()
	self:__StopAutoPotential()

	if self.__delay_timer then
		GlobalTimerQuest:CancelQuest(self.__delay_timer)
		self.__delay_timer = nil
	end	

	if self.__effect_id then
		self:__DelEffect(self.__effect_id)
		self.__effect_id = nil
	end
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.HeroPotentialTabView:__Dispose()
end

--获取英雄信息
function Modules.HeroPotentialTabView:__GetHeroData()
	local hero_uid = self.__parent_view:GetCurHeroUID()
	local hero_info = self:CallModuleFunc("GetHeroInfo", hero_uid)
	local config_hero = CPPGameLib.GetConfig("Hero", hero_info.info_id)
	return hero_info, config_hero
end

--刷新信息
function Modules.HeroPotentialTabView:__Reset()
	local hero_info, config_hero = self:__GetHeroData()

	--重置自动进化标记
	self.__is_auto_run = nil

	--重置潜能信息
	self.__potential_level = nil
	self.__potential_value = nil

	--添加特效
	self:__AddEffect()
end

--刷新详细信息
function Modules.HeroPotentialTabView:__ResetDetail(reset_data)
	local old_potential_level = self.__potential_level
	self.__potential_level = reset_data.potential_level
	self.__potential_value = reset_data.potential_exp

	if old_potential_level ~= self.__potential_level then
		--潜能等级
		self.__txt_cur_lev:SetText(CPPGameLib.GetString("hero_potential_info3", self.__potential_level))		
		local is_full_level = self.__potential_level >= ConfigAdapter.Hero.GetPotentialMaxLevel()
		self.__txt_next_lev:SetText(CPPGameLib.GetString("hero_potential_info3", not is_full_level and self.__potential_level + 1 or self.__potential_level))

		--满级提示
		self.__image_full_level:SetVisible(is_full_level)

		--属性加成
		self:__ResetPotentialAttr()
	end

	self:__ResetPotentialValueAndCostItem()
end

--刷新潜能值和材料
function Modules.HeroPotentialTabView:__ResetPotentialValueAndCostItem()
	local config_potential = CPPGameLib.GetConfig("HeroDestiny", self.__potential_level)
	local hero_info, config_hero = self:__GetHeroData()	
	local need_exp = config_potential.need_exp > 0 and config_potential.need_exp or self.__potential_value

	self.__cur_need_exp = need_exp	

	--潜能值
	self.__txt_potential_value:SetText(string.format("%d/%d", self.__potential_value, need_exp))

	--潜能概率提示	
	local prob_desc
	
	--英雄等级检测
	if hero_info.level < config_potential.need_level then
		prob_desc = GUI.RichText.GetColorWord(CPPGameLib.GetString("hero_potential_NeedHeroLevelTips", config_potential.need_level),Macros.Color.blinkcontent_hex,Macros.Color.common_stroke_hex,2)
	else
		prob_desc = ConfigAdapter.Hero.GetPotentialLevelupDesc(self.__potential_level, self.__potential_value)
 		prob_desc = GUI.RichText.GetColorWord(CPPGameLib.GetString("hero_potential_info2", prob_desc),Macros.Color.blinkcontent_hex,Macros.Color.common_stroke_hex,2)
	end

	self.__richtxt_potential_probability:SetText(prob_desc)

	--消耗材料
	local cost_str = ""
	local cost_item_id = config_potential.cost_item[1]
	local cost_item_num = config_potential.cost_item[2]
	local _, config_info = ObjIDTool.GetInfo(cost_item_id)
	if config_info then
		self.__img_cost_item:SetImage(Resource.PathTool.GetItemPath(config_info.res_id), TextureResType.LOCAL)

		local own_item_num = ObjIDTool.GetOwnNum(cost_item_id)
		self.__text_cost_item_num:SetText(cost_item_num)
		cost_str = own_item_num >= cost_item_num and 
		GUI.RichText.GetColorWord(tostring(own_item_num), Macros.Color.green_hex,Macros.Color.green_stroke_hex,2) or 
		GUI.RichText.GetColorWord(tostring(own_item_num), Macros.Color.red_hex,Macros.Color.red_stroke_hex,2)
	end
	self.__richtxt_cost_info:SetText(cost_str)	

	--进度条进度	
	PosTool.CenterBottom(self.__special_effect,0,out_poy + 158 *(self.__potential_value/need_exp))
end

--刷新属性加成
function Modules.HeroPotentialTabView:__ResetPotentialAttr()	
	local hero_info, config_hero = self:__GetHeroData()	
	local config_potential = CPPGameLib.GetConfig("HeroDestiny", self.__potential_level)
	local next_config_potential = CPPGameLib.GetConfig("HeroDestiny", self.__potential_level + 1, false)
	if not next_config_potential then
		next_config_potential = config_potential
	end

	self.__panel_cur_prop:RemoveAllChild(true)
	self.__panel_next_prop:RemoveAllChild(true)

	print("xyz" , "================刷新属性加成")

	local function handle_props(is_next, prop_name_color, prop_value_color, skill_color)
		prop_name_color = prop_name_color or Macros.Color.blinkcontent_hex
		prop_value_color = prop_value_color or Macros.Color.green_hex
		skill_color = skill_color or Macros.Color.blinkcontent_hex

		local config = not is_next and config_potential or next_config_potential
		local panel_prop = not is_next and self.__panel_cur_prop or self.__panel_next_prop
		local plus_prop_map = not is_next and ConfigAdapter.Hero.GetPotentialPlusPropMap(self.__potential_level , hero_info.info_id ) or (ConfigAdapter.Hero.GetPotentialPlusPropMap(self.__potential_level + 1 , hero_info.info_id ) or ConfigAdapter.Hero.GetPotentialPlusPropMap(self.__potential_level , hero_info.info_id ))

		local prop_x = 0
		local prop_y = 0
		local prop_interval = 6
		local lab_prop, prop_name, prop_value, prop_str
		--基础属性
		local prop_type_list = {Macros.Game.BattleProp.ATTACK_RATE, Macros.Game.BattleProp.HP_RATE, Macros.Game.BattleProp.PHYSICAL_DEFENCE_RATE, Macros.Game.BattleProp.MAGIC_DEFENCE_RATE}
		for prop_index, prop_type in ipairs(prop_type_list) do
			prop_name = CPPGameLib.GetString("common_str_with_colon", Macros.Game.BattlePropName[prop_type])
			prop_value = plus_prop_map[prop_type] and MathTool.TransProbCoeffPercentage(plus_prop_map[prop_type]) or 0
			local str1,str2 = GUI.RichText.GetColorWord(prop_name, prop_name_color,Macros.Color.common_stroke_hex,2),GUI.RichText.GetColorWord(string.format("+%d%%", prop_value), prop_value_color,Macros.Color.green_stroke_hex,2)
			prop_str = str1 .. str2
			lab_prop = Util:RichText(prop_str, Macros.TypefaceSize.normal)
			lab_prop:SetAnchorPoint(is_next and 1 or 0, 1)
			panel_prop:AddChild(lab_prop)
			if is_next then
				PosTool.RightTop(lab_prop, prop_x, prop_y)
			else
				PosTool.LeftTop(lab_prop, prop_x, prop_y)
			end			
			prop_y = prop_y - lab_prop:GetContentHeight() - prop_interval
			if prop_index == #prop_type_list then
				prop_y = prop_y - 7
			end
		end

		--技能等级
		local config_skill, lab_skill
		--品阶是否改变
		local is_quality_change = hero_info.quality_level > config_hero.init_quality
		local cur_skill_list = is_quality_change and config_hero.skill_ex_list or config_hero.skill_list
		for _, skill_id in ipairs(cur_skill_list) do
			config_skill = CPPGameLib.GetConfig("Skill", skill_id)
			--只影响特殊、怒气/合击技能
			if config_skill and 
				config_skill.type == Macros.Game.SkillType.SPECIAL or 
				config_skill.type == Macros.Game.SkillType.ANGER or
				config_skill.type == Macros.Game.SkillType.JOINT then
				lab_skill = Util:RichText(CPPGameLib.GetString("hero_potential_skill_name", config_skill.name, is_next and CPPGameLib.GetString("hero_potential_skill_green_name",config.skill_level) or ("Lv"..config.skill_level)), Macros.TypefaceSize.normal,0,0,skill_color,Macros.Color.common_stroke_hex,2)
				lab_skill:SetAnchorPoint(is_next and 1 or 0, 1)
				panel_prop:AddChild(lab_skill)
				if is_next then
					PosTool.RightTop(lab_skill, prop_x, prop_y)
				else
					PosTool.LeftTop(lab_skill, prop_x, prop_y)
				end	
				prop_y = prop_y - lab_skill:GetContentHeight() - prop_interval
			end
		end	
	end

	--当前属性、技能
	handle_props()	

	--下级属性、技能
	handle_props(true)
	
end

--充能
function Modules.HeroPotentialTabView:__HandlePotential()
	if not self.__potential_level or not self.__potential_value then
		self:__StopAutoPotential()
		self:__HideSomeWidget()
		return
	end	

	--等级上限检测
	if self:__CheckUpperLimit() then
		self:__StopAutoPotential()
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_potential_tips1"))	
		self:__HideSomeWidget()	
		return
	end

	local config_potential = CPPGameLib.GetConfig("HeroDestiny", self.__potential_level)
	local hero_info, config_hero = self:__GetHeroData()	
	--英雄等级检测
	if hero_info.level < config_potential.need_level then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_potential_HeroLevelTips",config_potential.need_level))
		self:__HideSomeWidget()		
		return
	end

	--材料检测
	local cost_item_id = config_potential.cost_item[1]
	local cost_item_num = config_potential.cost_item[2]
	if ObjIDTool.GetOwnNum(cost_item_id) < cost_item_num then
		self:__StopAutoPotential()
		local _, config_info = ObjIDTool.GetInfo(cost_item_id)
		GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, cost_item_id)
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_potential_tips2", config_info.name))		
		self:__HideSomeWidget()
		return
	end
	if self.__is_on_sending then
		self:__HideSomeWidget()
		return
	end
	self.__is_on_sending = true
	self:CallModuleFunc("HeroPotentialReq", self.__parent_view:GetCurHeroUID())
	return true
end

--开始自动充能
function Modules.HeroPotentialTabView:__StartAutoPotential()
	self:__StopAutoPotential()

	self.__is_auto_run = true
	self.__auto_run_timer = GlobalTimerQuest:AddPeriodQuest(GlobalCallbackMgr:AddCallBackFunc(
	function()
		self:__HandlePotential()
	end), 0.2, -1)		
	self:__HandlePotential()
end

--停止自动充能
function Modules.HeroPotentialTabView:__StopAutoPotential()
	if self.__is_auto_run then
		self.__is_auto_run = false
		if self.__auto_run_timer then
			GlobalTimerQuest:CancelQuest(self.__auto_run_timer)
			self.__auto_run_timer = nil
		end
	end
end

--检测潜能等级是否已达上限
function Modules.HeroPotentialTabView:__CheckUpperLimit()
	return self.__potential_level >= ConfigAdapter.Hero.GetPotentialMaxLevel()
end


--切换英雄
function Modules.HeroPotentialTabView:RefreshBySelectedHero()
	self:__Reset()
	if self.__delay_timer then
		GlobalTimerQuest:CancelQuest(self.__delay_timer)
		self.__delay_timer = nil
	end	
	if self.__auto_run_timer then
		GlobalTimerQuest:CancelQuest(self.__auto_run_timer)
		self.__auto_run_timer = nil
	end
	local hero_info = self:__GetHeroData()
	self:CallModuleFunc("HeroDetailInfoReq", hero_info.uid)
end
