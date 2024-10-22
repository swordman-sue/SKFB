--
-- @brief 英雄进化
-- @auther: ldx
-- @date 2018年7月17日 10:17:22

Modules = Modules or {}
Modules.HeroEvolutionView = Modules.HeroEvolutionView or BaseClass(BaseView)

local BASE_ZORDER = 10
local CRYSTAL_SPEED = -3
local CRYSTAL_SCALEFACTOR = 0.2
local ELLIPTIC_LENGTH = 70
local CRYSTAL_SCALEFACTOR_2 = 0.2

function Modules.HeroEvolutionView:__init()
 	self.__layout_name = "hero_evolution_view"
 	self.__bg_params = { type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_hero_evolution")}
 	self.__title_bar_params = {
 		title = CPPGameLib.GetString("hero_evolution_title"),
 		show_diamond = true,
 		show_gold = true,
 		show_stamina = true,
 		show_energy = true,
 		close_func = function ()			
 			--判断是否在进化中
 			if self.__is_on_hero_evolutioning then
 				return
 			end
 			self:CloseManual()
 		end,
 	}	 	

 	--绑定一个回调函数刷新剩余时间	
 	self.__open_callback = function ()
 		self:__OpenCallback()
 	end
 	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_EVOLUTION)
 	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_EVOLUTION,true)
end 

function Modules.HeroEvolutionView:__RegistAllEvents()
    --请求进化响应
    self:BindGlobalEvent(Macros.Event.HeroModule.HERO_EVOLUTION_NOTICE, function(_,protocol_data)
    	if protocol_data.hero_uid == self.__cur_hero_uid then  		
    		--打开进化成功界面
    		Modules.TemplHeroEvolutionSucc.New({hero_uid = self.__cur_hero_uid,is_evolution = protocol_data.quality > self.__cur_hero_quality},function ()
    			--品质升了
	        	if protocol_data.quality > self.__cur_hero_quality then
	        		--转盘
	        		for _,rotary_info in ipairs(self.__elliptic_rotary_info_list) do
	        			rotary_info.layout:RemoveAllChild(true)
	        			rotary_info.layout:RemoveSelf(true)
	        		end
	        		self.__elliptic_rotary_info_list = {}
	        		self:__InitHeroEvolutionMessage()
	        		self.__is_on_hero_evolutioning = false
	        	else
	        		local hero_info = CallModuleFunc(ModuleType.HERO,"GetHeroInfo",self.__cur_hero_uid)
					if not hero_info then
						return
					end
					local config_hero = CPPGameLib.GetConfig("Hero",hero_info.info_id)
					--当前品阶信息
					local config_heroevolution = CPPGameLib.GetConfig("HeroEvolution",hero_info.quality_level)
					if not config_heroevolution then
						return
					end
					self:__InitHeroEvolutionMessage()
					if hero_info.quality_stage < table.maxn(config_heroevolution.stage_list) then
		        		--转转盘
		        		self.__update_count_degrees = 0
		        		self.__update_degrees = -self.__stage_degrees
		        		self.__update_move = true	
		        	else
		        		local rotary_info = self.__elliptic_rotary_info_list[hero_info.quality_stage]
		        		if rotary_info and rotary_info.crystal then
		        			rotary_info.crystal:SetGray(false)
		        		end
		        		self.__is_on_hero_evolutioning = false
		        	end
	        	end
    		end)   		       	
        end
    end)
end

function Modules.HeroEvolutionView:GetOpenParams()
	return {self.__cur_hero_uid}
end

function Modules.HeroEvolutionView:__OpenCallback()
	local uid = unpack(self.__open_data)
	if not uid then
		return
	end
	--设置定时器
	self:SetUpdateFunc(function ()
		if self.__update_move then
			self:__UpdateCrystalCircle()
		end
	end)
	self.__cur_hero_uid = uid
	--大容器
	self.__panel_heroevolution = self:GetWidget("Panel_HeroEvolution")
	--突破预览按钮
	self.__btn_breakprelook = self:GetWidget("Button_BreakPreLook")
	local sp = Resource.PathTool.GetSystemEntryIconPath("btn_hero")
	self.__btn_breakprelook:SetNormalImage(sp,TextureResType.LOCAL)
	Util:WidgetButton(self.__btn_breakprelook,"",function ()
		--突破预览
		CallModuleFunc(ModuleType.HERO, "OpenView", BaseViewType.HERO_HANDBOOK_DETAIL, self.__cur_hero_uid,nil,true,true)
	end)
	--突破预览按钮文字
	Util:WidgetLabel(self:GetWidget("Text_BreakPreLook"),CPPGameLib.GetString("hero_evolution_breakprelook"), Macros.TypefaceSize.popup, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
	--英雄名字
	self.__txt_heroname = self: GetWidget("Text_HeroName")
	Util:WidgetLabel(self.__txt_heroname,"", Macros.TypefaceSize.headline, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
	--容器
	self.__panel_heromodule = self:GetWidget("Panel_HeroModule")
	self.__panel_heromodule:SetZOrder(BASE_ZORDER)
	--圆盘
	self.__img_turntable = self:GetWidget("Image_turntable")
	self.__img_turntable:SetVisible(false)
	--获取椭圆中心点
	self.__circle_center_pointx = self.__img_turntable:GetPositionX()
	self.__circle_center_pointy = self.__img_turntable:GetPositionY() - 15
	--之前的阶
	self.__txt_beforesteps = self:GetWidget("Text_BeforeSteps")
	Util:WidgetLabel(self.__txt_beforesteps,"", Macros.TypefaceSize.headlineII, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
	--现在的阶
	self.__txt_aftersteps = self:GetWidget("Text_AfterSteps")
	Util:WidgetLabel(self.__txt_aftersteps,"", Macros.TypefaceSize.headlineII, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
	--属性队列
	self.__prop_list = {0,Macros.Game.BattleProp.HP,Macros.Game.BattleProp.ATTACK,Macros.Game.BattleProp.PHYSICAL_DEFENCE,Macros.Game.BattleProp.MAGIC_DEFENCE}
	for _,prop_type in ipairs(self.__prop_list) do
		--属性
		Util:WidgetLabel(self:GetWidget("Text_Type"..prop_type),prop_type ~= 0 and CPPGameLib.GetString("common_str_with_colon",CPPGameLib.GetString("nature"..prop_type)) or CPPGameLib.GetString("spirit_qulaity_txt2"), Macros.TypefaceSize.normal, Macros.Color.btn)
		--当前阶属性值
		self["__text_beforeproperty"..prop_type] = self:GetWidget("Text_BeforeProperty"..prop_type)
		Util:WidgetLabel(self["__text_beforeproperty"..prop_type],"", Macros.TypefaceSize.normal, Macros.Color.btn)
		--往后阶属性值
		self["__text_afterproperty"..prop_type] = self:GetWidget("Text_AfterProperty"..prop_type)
		Util:WidgetLabel(self["__text_afterproperty"..prop_type],"", Macros.TypefaceSize.normal, Macros.Color.green)
	end
	--品质上界
	self.__txt_qualityupstage = self:GetWidget("Text_QualityUpStage")
	Util:WidgetLabel(self.__txt_qualityupstage,"", Macros.TypefaceSize.tab, Macros.Color.red, 0, 0, Macros.Color.red_stroke)
	--品质上界提示
	self.__txt_qualityupstage_tips = self:GetWidget("Text_QualityUpStageTips")
	Util:WidgetLabel(self.__txt_qualityupstage_tips,CPPGameLib.GetString("hero_evolution_qualityupstage_tips"), Macros.TypefaceSize.minimum, Macros.Color.btn)
	--属性队列
	self.__prop_list = {0,Macros.Game.BattleProp.HP,Macros.Game.BattleProp.ATTACK,Macros.Game.BattleProp.PHYSICAL_DEFENCE,Macros.Game.BattleProp.MAGIC_DEFENCE}
	--升阶需要材料以及条件
	for i=1,2 do
		--材料图标容器
		self["__panel_material"..i] = self:GetWidget("Panel_Material"..i)
		--材料名字
		self["__txt_materialname"..i] = self:GetWidget("Text_MaterialName"..i)
		Util:WidgetLabel(self["__txt_materialname"..i],"", Macros.TypefaceSize.normal, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
		--材料数量
		self["__txt_materialnum"..i] = Util:RichText("", Macros.TypefaceSize.normal)
		self["__txt_materialnum"..i]:SetAnchorPoint(0,0.5)
		self["__panel_material"..i]:AddChild(self["__txt_materialnum"..i])
		self["__txt_materialnum"..i]:SetPosition(87.5,15)
		--进阶需要条件
		self["__txt_needcondiiton"..i] = self:GetWidget("Text_NeedCondiiton"..i)
		self["__txt_needcondiiton_posx"..i] = self["__txt_needcondiiton"..i]:GetPositionX()
		Util:WidgetLabel(self["__txt_needcondiiton"..i],"", Macros.TypefaceSize.minimum, Macros.Color.red)
	end
	--突破按钮
	self.__btn_break = self:GetWidget("Button_Break")
	Util:WidgetButton(self.__btn_break,CPPGameLib.GetString("hero_evolution_break"),function ()
		--检测条件
		local is_pass,refuse_tips = self:__CheckEvolutionConditional()
		if not is_pass then
			GlobalTipMsg.GetInstance():Show(refuse_tips or "")
			return
		end
		self.__is_on_hero_evolutioning = true
		--播动作
		self:RunBeforeEvolutionEffect()
	end, Macros.TypefaceSize.button, Macros.Color.btn, Macros.Color.button_red, 2)
	--货币类型
	self.__img_moneytype = self:GetWidget("Image_MoneyType")
	--货币类型数字
	self.__txt_moneytype = self:GetWidget("Text_MoneyType")
	Util:WidgetLabel(self.__txt_moneytype,"", Macros.TypefaceSize.popup, Macros.Color.btn)
	--
	self:__InitHeroEvolutionMessage()
end

--初始化进化信息
function Modules.HeroEvolutionView:__InitHeroEvolutionMessage()
	local hero_info = CallModuleFunc(ModuleType.HERO,"GetHeroInfo",self.__cur_hero_uid)
	if not hero_info then
		return
	end
	--突破预览按钮
	self.__btn_breakprelook:SetVisible(hero_info.quality_level <= Macros.Game.QualityType.ORANGE)
	--当前品质
	self.__cur_hero_quality = hero_info.quality_level
	self.__cur_hero_quality_stage = hero_info.quality_stage
	local config_hero = CPPGameLib.GetConfig("Hero",hero_info.info_id)
	--名字
	if hero_info.break_level > 0 then
		self.__txt_heroname:SetText(CPPGameLib.GetString("common_add6", config_hero.name, hero_info.break_level))
	else
		self.__txt_heroname:SetText(config_hero.name)
	end
	self.__txt_heroname:SetTextColor(Macros.QualityColor(hero_info.quality_level))
	self.__txt_heroname:SetStrokeColor(Macros.QualityColorStroke(hero_info.quality_level))
	--模型
	if not self.__ui_hero then
		self.__ui_hero = LogicGUI.Model.New({info_id = config_hero.model_id, parent = self.__panel_heromodule})	
		self.__ui_hero:SetPosition(5,0)
		self.__ui_hero:SetEndCallback(function ()
			self.__ui_hero:PlayAnimation(RenderObj.ActionIndex.IDLE)
		end)
		--缩放比例
		self.__ui_hero:SetScaleFactor(config_hero.ui_scale*1.5)
		--随机动作
		local random_action_index = MathTool.GetRandom(1,#config_hero.evolution_show_action_list)
		--播动作
		self.__ui_hero:PlayAnimation(config_hero.evolution_show_action_list[random_action_index],false)
	end
	--当前品阶信息
	local config_heroevolution = CPPGameLib.GetConfig("HeroEvolution",hero_info.quality_level)
	if not config_heroevolution then
		return
	end
	--判断是否已经升至最高
	self.__is_full_evolution = false
	--限制品质和品阶
	if hero_info.quality_level >= config_hero.quality_limit and hero_info.quality_stage >= config_hero.quality_stage_limit then
		self.__is_full_evolution = true
	else
		if config_heroevolution and config_heroevolution.stage_list[hero_info.quality_stage] then
			--最后一阶
			if config_heroevolution.stage_list[hero_info.quality_stage].is_the_last_stage == Macros.Global.TRUE then
				if hero_info.quality_level >= config_hero.quality_limit then
					self.__is_full_evolution = true
				end
			end
		else
			self.__is_full_evolution = true
		end 
	end
	--创建转盘阶数
	self.__elliptic_rotary_info_list = self.__elliptic_rotary_info_list or {}	
	if #self.__elliptic_rotary_info_list <= 0 then
		--当前最大阶数
		self.__max_stage = table.maxn(config_heroevolution.stage_list)
		self.__stage_degrees = 360/self.__max_stage
		for stage=1,self.__max_stage do
			local rotary_info = {}
			--容器
			rotary_info.layout = Util:Layout(2,2)
			rotary_info.layout:SetClippingEnable(false)
	  		rotary_info.layout:SetBackGroundColor(0, 0, 0)
	  		self.__panel_heroevolution:AddChild(rotary_info.layout)
	  		--阴影
	  		local image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.HERO_EVOLUTION, "bg_black_bottom")
	  		rotary_info.shadow_img = Util:Sprite(image_data)
	  		rotary_info.shadow_img:SetAnchorPoint(0.5,0)
	  		rotary_info.shadow_img:SetScaleFactor(0.8)
	  		rotary_info.shadow_img:SetOpacity(255*0.6)
	  		rotary_info.layout:AddChild(rotary_info.shadow_img)
	  		PosTool.CenterBottom(rotary_info.shadow_img,0,-18)
	  		--水晶
	  		image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.HERO_EVOLUTION, "center_crystal")
	  		rotary_info.crystal = Util:Sprite(image_data)
	  		rotary_info.crystal:SetAnchorPoint(0.5,0)	  		
	  		rotary_info.crystal:SetGray(self.__cur_hero_quality_stage < stage and true or false) 
	  		rotary_info.layout:AddChild(rotary_info.crystal)
	  		PosTool.CenterBottom(rotary_info.crystal,0,3)
	  		--阶级
	  		rotary_info.stage_label = Util:Label(CPPGameLib.GetString("hero_evolution_stage_level",CPPGameLib.GetString("hero_evolution_stage_quality"..hero_info.quality_level),stage),Macros.TypefaceSize.tab, Macros.Quality[hero_info.quality_level], 0, 0, Macros.Quality_Stroke[hero_info.quality_level], 2)
	  		rotary_info.layout:AddChild(rotary_info.stage_label)
	  		PosTool.CenterBottom(rotary_info.stage_label,0,10)
	  		--位置
	  		local cur_degrees = -90 + (hero_info.quality_stage >= self.__max_stage and  (stage - hero_info.quality_stage) or (stage - hero_info.quality_stage - 1)) * self.__stage_degrees
	  		local x,y = self:__GetPositionByDegrees(cur_degrees)
	  		rotary_info.layout:SetAnchorPoint(0.5,0.5)
	  		rotary_info.layout:SetPosition(x,y)
	  		--大小以及渲染深度  
	  		rotary_info.layout:SetZOrder(BASE_ZORDER-math.sin(math.rad(cur_degrees)))	
	  		-- rotary_info.crystal:SetScaleFactor(CRYSTAL_SCALEFACTOR + 0.3 * math.max(((self.__circle_center_pointy + ELLIPTIC_LENGTH - y)/(ELLIPTIC_LENGTH*2)),0.1))
	  		rotary_info.crystal:SetScaleFactor(math.min(CRYSTAL_SCALEFACTOR + CRYSTAL_SCALEFACTOR_2 * ((self.__circle_center_pointy + ELLIPTIC_LENGTH - y)/(ELLIPTIC_LENGTH*2))),0.2)
	  		--阶级
	  		rotary_info.stage_label:SetScaleFactor(0.5 + 0.5 * math.max(((self.__circle_center_pointy + ELLIPTIC_LENGTH - y)/(ELLIPTIC_LENGTH*2)),0.1))	  		
			--阴影
			rotary_info.shadow_img:SetScaleFactor(0.5 + 0.5 * math.max(((self.__circle_center_pointy + ELLIPTIC_LENGTH - y)/(ELLIPTIC_LENGTH*2)),0.1))	
			--加入table
			table.insert(self.__elliptic_rotary_info_list,rotary_info)
		end
	end
	--属性
	local handle_property_info = function (is_next)
		local hero_quality = hero_info.quality_level
		local hero_quality_stage = hero_info.quality_stage
		--判断是否进入下一阶
		if is_next and not self.__is_full_evolution then
			--最后一阶
			if config_heroevolution.stage_list[hero_info.quality_stage].is_the_last_stage == Macros.Global.TRUE then
				--判断是否能进化到下一阶
				hero_quality = hero_quality + 1
				hero_quality_stage = 0
			else
				hero_quality_stage = hero_quality_stage + 1
			end
		end
		if is_next then
			self.__next_hero_quality = hero_quality
			self.__next_hero_quality_stage = hero_quality_stage
		end
		--获取英雄进化属性
		local basic_prop_map = ConfigAdapter.Hero.GetHeroEvolutionPropMap(hero_quality,hero_info.info_id,hero_info.level,hero_quality_stage) 
		--阶
		local txt_steps = is_next and self.__txt_aftersteps or self.__txt_beforesteps
		txt_steps:SetText(CPPGameLib.GetString("hero_evolution_stage_level",CPPGameLib.GetString("hero_evolution_stage_quality"..hero_quality),hero_quality_stage))
		txt_steps:SetColor(unpack(Macros.Quality[hero_quality]))
		txt_steps:SetStrokeColor(unpack(Macros.Quality_Stroke[hero_quality]))
		--属性队列
		for _,prop_type in ipairs(self.__prop_list) do
			--属性
			local prop_label = is_next and self["__text_afterproperty"..prop_type] or self["__text_beforeproperty"..prop_type]
			prop_label:SetText(prop_type ~= 0 and basic_prop_map[prop_type] or basic_prop_map.qualifications)
		end
	end
	handle_property_info()
	handle_property_info(true)
	--品质上界
	self.__txt_qualityupstage:SetVisible(hero_info.quality_level < config_hero.quality_limit)
	if hero_info.quality_level < config_hero.quality_limit then		
		self.__txt_qualityupstage:SetText(CPPGameLib.GetString("hero_evolution_qualityupstage",table.maxn(config_heroevolution.stage_list)+1-hero_info.quality_stage,
			CPPGameLib.GetString("hero_evolution_stage_quality"..(hero_info.quality_level+1))))
	end
	--品质上界提示
	self.__txt_qualityupstage_tips:SetVisible(hero_info.quality_level < config_hero.quality_limit)
	--升阶需要材料以及条件
	for i=1,2 do
		--材料图标容器
		self["__panel_material"..i]:SetVisible(false)
		--进阶需要条件
		self["__txt_needcondiiton"..i]:SetVisible(not self.__is_full_evolution)
	end
	
	--未满阶
	local cur_stage_list
	if not self.__is_full_evolution then	
		local material_panel_num = 1
		--当前品阶信息
		local config_heroevolution = CPPGameLib.GetConfig("HeroEvolution",hero_info.quality_level)
		if not config_heroevolution then
			return
		end
		cur_stage_list = config_heroevolution.stage_list[hero_info.quality_stage]
		--处理材料
		local handle_material_func = function (cost_material_list,cost_material_num_list,is_currency)
			if cost_material_list then
				for i,cost_item_id in ipairs(cost_material_list) do
					local cur_currency_id
					self["__panel_material"..material_panel_num]:SetVisible(true)
					--材料图标
					if not self["__material_icon"..material_panel_num] then
						self["__material_icon"..material_panel_num] = LogicGUI.IconLayer.New(false,true)
						self["__panel_material"..material_panel_num]:AddChild(self["__material_icon"..material_panel_num]:GetNode())
						PosTool.Center(self["__material_icon"..material_panel_num])					
					end
					if is_currency then
						cur_currency_id = cost_item_id
						local huobi_config = CPPGameLib.GetConfig("HuoBi",cost_item_id)
						cost_item_id = huobi_config.big_icon
					end
					self["__material_icon"..material_panel_num]:SetData({id = cost_item_id})
					self["__material_icon"..material_panel_num]:SetTouchCallback(function ()
						--获取途径
						GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, cost_item_id)
					end)
					local type, config_info = ObjIDTool.GetInfo(cost_item_id)
					--材料名字
					local material_quality = type == Macros.Game.ObjType.HERO and config_info.init_quality or config_info.quality
					self["__txt_materialname"..material_panel_num]:SetText(config_info.name or "")
					self["__txt_materialname"..material_panel_num]:SetColor(unpack(Macros.Quality[material_quality]))
					self["__txt_materialname"..material_panel_num]:SetStrokeColor(unpack(Macros.Quality_Stroke[material_quality]))
					local have_obj_num = ObjIDTool.GetOwnNum(is_currency and cur_currency_id or cost_item_id)
					--材料数量
					if self.__is_material_enough then
						self.__is_material_enough = have_obj_num >= cost_material_num_list[i]
					end
					local own_num_str = GUI.RichText.GetColorWord(tostring(have_obj_num), have_obj_num >= cost_material_num_list[i] and Macros.Color.front_color_hex or Macros.Color.red_hex)
					local num_str = GUI.RichText.GetColorWord(string.format("%s/%d", own_num_str, cost_material_num_list[i]), Macros.Color.btn_hex )
					self["__txt_materialnum"..material_panel_num]:SetText(num_str)
					material_panel_num = material_panel_num + 1
				end
			end
		end
		self.__is_material_enough = true
		handle_material_func(cur_stage_list.cost_item_id_list,cur_stage_list.cost_item_num_list)
		handle_material_func(cur_stage_list.cost_currency_type_list,cur_stage_list.cost_currency_value_list,true)
		--进阶需要条件
		local num = 0 
		num = num + (hero_info.level < cur_stage_list.need_level and 1 or 0)
		num = num + (hero_info.break_level < cur_stage_list.need_break_level and 1 or 0)
		self.__txt_needcondiiton1:SetPosition(self.__txt_needcondiiton_posx1+(num >= 2 and 0 or 55),self.__txt_needcondiiton1:GetPositionY()) 	
		self.__txt_needcondiiton1:SetVisible(hero_info.level < cur_stage_list.need_level)
		self.__txt_needcondiiton1:SetText(CPPGameLib.GetString("hero_evolution_need_level",cur_stage_list.need_level))
		self.__txt_needcondiiton2:SetPosition(self.__txt_needcondiiton_posx2-(num >= 2 and 0 or 70),self.__txt_needcondiiton2:GetPositionY()) 
		self.__txt_needcondiiton2:SetVisible(hero_info.break_level < cur_stage_list.need_break_level)
		self.__txt_needcondiiton2:SetText(CPPGameLib.GetString("hero_evolution_need_break_level",cur_stage_list.need_break_level))			             
	end
	--未达满阶
	self.__btn_break:SetText(self.__is_full_evolution and CPPGameLib.GetString("hero_evolution_full_stage") or CPPGameLib.GetString("hero_evolution_break"))
	self.__btn_break:SetEnabled(not self.__is_full_evolution)	
	
	local is_pass
	--按钮状态
	if not self.__is_full_evolution then			
    	local r, g, b, a = unpack(Macros.Color.button_yellow)
		self.__btn_break:SetOutline(r, g, b, a, 2)
		--检测条件	
		is_pass = self:__CheckEvolutionConditional(true)
		if is_pass and not self.__button_zoubian_effectid then
			local effect
			self.__button_zoubian_effectid,effect = self:__CreateEffect({res_name = "ui_anniuzoubian"},self.__btn_break)
			PosTool.Center(effect)
		end
	else
		--突破按钮
		Util:GreyButton(self.__btn_break)   			
	end
	--特效
	if self.__button_zoubian_effectid then
		local effect = GlobalEffectMgr:GetEffect(self.__button_zoubian_effectid)
		if effect then
			effect:SetVisible(not self.__is_full_evolution and is_pass or false)
		end
	end
	--货币类型
	self.__img_moneytype:SetVisible(not self.__is_full_evolution)
	--货币类型数字
	if not self.__is_full_evolution then
		self.__txt_moneytype:SetText(cur_stage_list.cost_gold or 0)
	end
	self.__txt_moneytype:SetVisible(not self.__is_full_evolution)
end

function Modules.HeroEvolutionView:__Dispose()
	if self.__ui_hero then
		self.__ui_hero:DeleteMe()
		self.__ui_hero = nil
	end
	self:DelFlashEffect()
end

function Modules.HeroEvolutionView:__GetPositionByDegrees(degrees)
	local x = self.__circle_center_pointx + 250*math.cos(math.rad(degrees))
	local y = self.__circle_center_pointy + ELLIPTIC_LENGTH*math.sin(math.rad(degrees))
	return x,y
end

--更新转圈位置
function Modules.HeroEvolutionView:__UpdateCrystalCircle()
	if self.__update_count_degrees <= self.__update_degrees then
		self.__update_move = false	
		self.__is_on_hero_evolutioning = false
		return		
	end
	local count_degrees = CRYSTAL_SPEED
	if (self.__update_count_degrees + count_degrees) < self.__update_degrees then
		count_degrees = self.__update_degrees - self.__update_count_degrees
		self.__update_count_degrees = self.__update_degrees
	else
		self.__update_count_degrees = self.__update_count_degrees + count_degrees		
	end	
	--位置
	for stage,rotary_info in ipairs(self.__elliptic_rotary_info_list) do	
		--位置
		local cur_degrees = -90 + (stage - self.__cur_hero_quality_stage) * self.__stage_degrees + self.__update_count_degrees
  		local x,y = self:__GetPositionByDegrees(cur_degrees)
  		rotary_info.layout:SetAnchorPoint(0.5,0.5)
  		rotary_info.layout:SetPosition(x,y)
		--水晶
	  	rotary_info.crystal:SetGray(self.__cur_hero_quality_stage < stage and true or false) 
  		--大小以及渲染深度  
  		rotary_info.layout:SetZOrder(BASE_ZORDER-math.sin(math.rad(cur_degrees)))	
  		-- rotary_info.crystal:SetScaleFactor(CRYSTAL_SCALEFACTOR + 0.3 * math.max(((self.__circle_center_pointy + ELLIPTIC_LENGTH - y)/(ELLIPTIC_LENGTH*2)),0.1))
  		rotary_info.crystal:SetScaleFactor(math.min(CRYSTAL_SCALEFACTOR + CRYSTAL_SCALEFACTOR_2 * ((self.__circle_center_pointy + ELLIPTIC_LENGTH - y)/(ELLIPTIC_LENGTH*2))),0.2)
  		--阶级
  		rotary_info.stage_label:SetScaleFactor(0.5 + 0.5 * math.max(((self.__circle_center_pointy + ELLIPTIC_LENGTH - y)/(ELLIPTIC_LENGTH*2)),0.1))	  		
		--阴影
		rotary_info.shadow_img:SetScaleFactor(0.5 + 0.5 * math.max(((self.__circle_center_pointy + ELLIPTIC_LENGTH - y)/(ELLIPTIC_LENGTH*2)),0.1))	
	end	  		
end

--检测进化条件
function Modules.HeroEvolutionView:__CheckEvolutionConditional(only_check)
	local hero_info = CallModuleFunc(ModuleType.HERO,"GetHeroInfo",self.__cur_hero_uid)
	if not hero_info then
		return
	end
	--当前品阶信息	
	local config_heroevolution = CPPGameLib.GetConfig("HeroEvolution",hero_info.quality_level)
	if not config_heroevolution then
		return false
	end
	--检测等级
	if hero_info.level < config_heroevolution.stage_list[hero_info.quality_stage].need_level then
		return false,CPPGameLib.GetString("hero_break_tips1")
	end
	--检测进阶等级
	if hero_info.break_level < config_heroevolution.stage_list[hero_info.quality_stage].need_break_level then
		return false,CPPGameLib.GetString("hero_evolution_break_lack")
	end
	--检测材料
	if not self.__is_material_enough then
		return false,CPPGameLib.GetString("hero_evolution_material_lack")
	end
	--金币
	local have_gold = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.GOLD)
	if have_gold < config_heroevolution.stage_list[hero_info.quality_stage].cost_gold then
		--只检测
		if not only_check then
			local currency_config = CPPGameLib.GetConfig("HuoBi",Macros.Game.RolePropType.GOLD)
			GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, currency_config.big_icon)
		end
		return false,CPPGameLib.GetString("hero_evolution_gold_lack")
	end
	return true
end

--觉醒之前的特效
function Modules.HeroEvolutionView:RunBeforeEvolutionEffect()
	self:DelFlashEffect()
	local effect,effectid
	local count_index = 1
	local function runFlashEffect()
		if count_index <= 2 then
			effectid,effect = self:__CreateEffect({res_name = "UI_juexingzhuangbeifeiru",loop = false,scale = 1.2,time_scale = 1.5},self["__panel_material"..count_index],3)
			PosTool.Center(effect)
			CPPActionManager.DelayTo(self["__panel_material"..count_index],0.08,GlobalCallbackMgr:AddCallBackFunc(function ()
				self["__flash_effectid"..count_index],self["__flash_effect"..count_index] = self:__CreateEffect({res_name = "UI_juexingzhuangbeifeiru1"},self["__panel_material"..count_index],4)
				PosTool.Center(self["__flash_effect"..count_index])
				self["__flash_effect"..count_index]:SetScaleFactor(0.1)
				ActionManager:RunScaleToAndMoveByEffectEx(self["__flash_effect"..count_index],0.2,0,80,1,count_index == 2 and function ()
					self:RunBeforeEvolutionEffectTwo()
				end or nil)	
				count_index = count_index + 1
				runFlashEffect()							
			end))
		end
	end
	runFlashEffect()
end

--飞过去
function Modules.HeroEvolutionView:RunBeforeEvolutionEffectTwo()
	--当前需要激活水晶索引
	local cur_crystal_index = math.min(self.__cur_hero_quality_stage +1,self.__max_stage)	
	CPPActionManager.DelayTo(self.__panel_material1,0.2,GlobalCallbackMgr:AddCallBackFunc(function ()
		for count_index=1,2 do
			local pos = NodeUnity.ConvertPosBetweenTwoContainer(self.__elliptic_rotary_info_list[cur_crystal_index].layout,self["__panel_material"..count_index],COCOPoint(1,40))
			CPPActionManager.MoveBy(self["__flash_effect"..count_index],0.6,0,-30,GlobalCallbackMgr:AddCallBackFunc(function ()
				CPPActionManager.MoveTo(self["__flash_effect"..count_index],0.2,pos.x,pos.y,GlobalCallbackMgr:AddCallBackFunc(function ()
					self:__DelEffect(self["__flash_effectid"..count_index])
					self["__flash_effectid"..count_index] = nil
					if count_index == 2 then
						local upgrade_effect_id,effect = self:__CreateEffect({res_name = "shouji_13",loop = false,scale = 2},self.__elliptic_rotary_info_list[cur_crystal_index].layout,10)
						PosTool.CenterBottom(effect,0,50)
						--英雄进化请求
						self:CallModuleFunc("HeroEvolutionReq",self.__cur_hero_uid)					
					end
				end))	
			end))		
		end	
	end))
end

function Modules.HeroEvolutionView:DelFlashEffect()
	for count_index=1,4 do
		if self["__flash_effectid"..count_index] then			
			CPPActionManager.StopAllActions(self["__flash_effect"..count_index])
			self["__flash_effect"..count_index] = nil
			self:__DelEffect(self["__flash_effectid"..count_index])
			self["__flash_effectid"..count_index] = nil
		end					
	end	
end