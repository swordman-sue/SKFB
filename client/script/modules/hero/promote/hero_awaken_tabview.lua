
--[[
%% @module: 英雄觉醒界面
%% @author: ldx
%% @created: 2016-11-08
--]]

Modules = Modules or {}

Modules.HeroAwakenTabView = Modules.HeroAwakenTabView or BaseClass(BaseTabView)

function Modules.HeroAwakenTabView:__init()
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_AWAKEN, true)
end

function Modules.HeroAwakenTabView:__LoadedCallback()	
	self.__panel_main = self.__parent_view:GetWidget("Panel_Main")
	--英雄模型容器
	self.__panel_hero_commodel = WidgetUnity.GetWidgetByName(self.__panel_main, "Panel_HeroUpgradeSpecialEffect")
	--特殊额外属性标题	
	self.__text_speciallock = Util:Label("",Macros.TypefaceSize.largish, Macros.Color.front_color )
	self:GetParent():GetWidget("Panel_10"):AddChild(self.__text_speciallock,5)
	self.__text_speciallock:SetPosition(172,90)
	--特殊额外属性文本
	self.__text_property = Util:Label("",Macros.TypefaceSize.largish, Macros.Color.front_color ,300,60)
	self.__text_property:SetTextHorizontalAlign(LabelEx.ALIGIN_CENTER)
	self:GetParent():GetWidget("Panel_10"):AddChild(self.__text_property,5)
	self.__text_property:SetAnchorPoint(0.5,1)
	self.__text_property:SetPosition(172,71)	
	--道具预览		
	self.__button_preLook = Util:Button(Resource.PathTool.GetSystemEntryIconPath("btn_first_recharge_2"),function ()
		LogicGUI.TemplHeroAwakenPropPreLookView.New(self.__special_id,self.__awaken_star,self.__awaken_level)
	end)
	self:GetParent():GetWidget("Panel_10"):AddChild(self.__button_preLook,5)
	self.__button_preLook:SetPosition(290,465)	
	local label = Util:Label(CPPGameLib.GetString("hero_awaken_Button_preLook"),Macros.TypefaceSize.largish, Macros.Color.btn,0,0,Macros.Color.button_yellow)
	self.__button_preLook:AddChild(label)
	PosTool.CenterBottom(label,0,10)
	--满级图片
	self.__image_fulllevel = self:GetWidget("Image_fullLevel")
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.HERO_AWAKEN,"full_level",true)
	self.__image_fulllevel:SetImage(sp)

	--当前和下阶属性加成
	self.__cur_prop_map = {}
	self.__next_prop_map = {}
	self.__windwords_pos_list = {}	
	local prop_type_list = {Macros.Game.BattleProp.ATTACK, Macros.Game.BattleProp.HP, Macros.Game.BattleProp.PHYSICAL_DEFENCE, Macros.Game.BattleProp.MAGIC_DEFENCE}
	local panel_awaken = self:GetWidget("Panel_Awaken")
	local type_str,value_str,prop_txt
	local function create_rt_prop(prop_type, is_cur)
		type_str = is_cur and "Text_CurPropType"..prop_type or "Text_NextPropType"..prop_type
		value_str = is_cur and "Text_CurPropValue"..prop_type or "Text_NextPropValue"..prop_type
		--属性
		Util:WidgetLabel(self:GetWidget(type_str),CPPGameLib.GetString("common_str_with_colon",Macros.Game.BattlePropName[prop_type]), Macros.TypefaceSize.largish, Macros.Color.keypoint)
		prop_txt = self:GetWidget(value_str)
		Util:WidgetLabel(prop_txt,"", Macros.TypefaceSize.largish, is_cur and Macros.Color.content or Macros.Color.front_color)
		if is_cur then			
			self.__windwords_pos_list[prop_type] = NodeUnity.ConvertPosBetweenTwoContainer(panel_awaken, self.__root_widget,COCOPoint(prop_txt:GetPositionX(),prop_txt:GetPositionY()))
		end
		local prop_map = is_cur and self.__cur_prop_map or self.__next_prop_map
		prop_map[prop_type] = prop_txt
	end
	for _, prop_type in ipairs(prop_type_list) do		
		create_rt_prop(prop_type, true)
		create_rt_prop(prop_type)
	end
	--觉醒装备
	Util:WidgetLabel(self:GetWidget("Text_awakenProp"),CPPGameLib.GetString("hero_awaken_Text_awakenequip"),Macros.TypefaceSize.popup,Macros.Color.headline)	
	self.__org_pos_list = {}
	for i=1,4 do  
		self["__panel_pos"..i] = self:GetWidget("Panel_pos"..i)
		self["__text_property"..i] = self:GetWidget("Text_property"..i)
		self["__text_property"..i]:SetZOrder(5)
		Util:WidgetLabel(self["__text_property"..i],"", Macros.TypefaceSize.popup, Macros.Color.green,0,0,Macros.Color.green_stroke)
		self["__text_property"..i]:SetVisible(false)
		self.__org_pos_list[i] = {pos_x = self["__text_property"..i]:GetPositionX(),pos_y = self["__text_property"..i]:GetPositionY()}
	end
	--星阶变化
	self.__text_starstagefirst = self:GetWidget("Text_starStageFirst")
	Util:WidgetLabel(self.__text_starstagefirst,"",Macros.TypefaceSize.popup,Macros.Color.blue,0,0,Macros.Color.blue_stroke)
	self.__text_starstageafter = self:GetWidget("Text_starStageAfter")
	Util:WidgetLabel(self.__text_starstageafter,"",Macros.TypefaceSize.popup,Macros.Color.blue,0,0,Macros.Color.blue_stroke)
	--箭头
	self.__image_arrows = self:GetWidget("Image_arrows")
	--觉醒材料
	Util:WidgetLabel(self:GetWidget("Text_awakenTitle"),CPPGameLib.GetString("hero_awaken_Text_awakenTitle"),Macros.TypefaceSize.popup,Macros.Color.headline)
	--觉醒按钮
	self.__button_awaken = self:GetWidget("Button_awaken")
	Util:WidgetButton(self.__button_awaken,CPPGameLib.GetString("hero_awaken_Button_awaken"),function ()
		local isCouldAwaken,tips_str = self:__CheckAwaken()
		if isCouldAwaken then	
			self.__is_on_awakening = true		
			self:RunBeforeAwakenEffect()	
			--走边特效
			if self.__button_zoubian_effectid then
				local effect_root_id = GlobalEffectMgr:GetEffect(self.__button_zoubian_effectid)
				if effect_root_id then
					effect_root_id:SetVisible(false)
				end
			end
			self.__button_awaken:SetEnabled(false)		
		else
			if tips_str == CPPGameLib.GetString("hero_awaken_awakenMoneyTips") then
				CallModuleFunc(ModuleType.SUNDRY, "ToIdGetPath", Macros.Game.RolePropType.GOLD )
			end
			GlobalTipMsg.GetInstance():Show(tips_str)
		end
	end,Macros.TypefaceSize.button,Macros.Color.btn,Macros.Color.button_yellow)
	--货币
	self.__image_coin = self:GetWidget("Image_coin")
	--货币数量
	self.__text_coinnum = self:GetWidget("Text_coinNum")
	Util:WidgetLabel(self.__text_coinnum,"",Macros.TypefaceSize.largish,Macros.Color.content)
	--觉醒材料容器
	for i=1,2 do
		self["__panel_material"..i] = self:GetWidget("Panel_material"..i)
		-- self["Text__materialNum"..i] = self:GetWidget("Text__materialNum"..i)
		-- Util:WidgetLabel(self["Text__materialNum"..i],"",18,Macros.Color.content,0,0,Macros.Color.common_stroke)
		-- self["Text__materialName"..i] = self:GetWidget("Text__materialName"..i)
		-- if i ~= 2 then
		-- 	Util:WidgetLabel(self["Text__materialName"..i],"",18,Macros.Color.content,0,0,Macros.Color.common_stroke)
		-- end
	end
    --规则按钮
	local Button_rule = Util:WidgetButton( self:GetWidget("Button_rule") , nil , function ( ... )
		print("xy" , "(5)英雄觉醒，小图标：" )
        local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
        help:SetLoadedCallback( function ( ... )
           help:SetCell("Rule_Hero_Awaken")
        end )
	end )
	Button_rule:SetZOrder(100)
end

function Modules.HeroAwakenTabView:__OpenCallback()
	self.__hero_uid = self.__parent_view:GetCurHeroUID()
	--道具预览
	if self.__button_preLook then
		self.__button_preLook:SetVisible(true)
		local is_visible = self.__parent_view:__GetIsHideHeroList()
		self.__text_speciallock:SetVisible(not is_visible)
		self.__text_property:SetVisible(not is_visible)
	end
	--装备觉醒道具合成事件
	self.__awaken_prop_compose_event = self:BindGlobalEvent(Macros.Event.HeroModule.HERO_AWAKEN_PROP_COMPOSE_RESP, function(_,item_id)
		local awaken_config = CPPGameLib.GetConfig("AwakeningItem",item_id) 
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("hero_awaken_CompoundSuccessTip",awaken_config.name))
		--装备状态更新
		self:__SetEquipState()
	end)
	
	--觉醒事件
	self.__hero_awaken_resp_event = self:BindGlobalEvent(Macros.Event.HeroModule.HERO_AWAKEN_RESP, function(_,awaken_info)
		--觉醒升星
		if awaken_info.awaken_star > self.__awaken_star then
			Modules.TemplHeroAwakenSucc.New(self.__hero_uid)
		end		
		self.__awaken_star = awaken_info.awaken_star
		self.__awaken_level = awaken_info.awaken_level	
		local awaken_config = CPPGameLib.GetConfig("HeroAwakening", self.__special_id)
		self.__awaken_config = awaken_config[self.__awaken_star][self.__awaken_level]
		--是否满级
		self.__is_fulllevel = self.__awaken_star >= Macros.Game.HERO_AWAKE_STAR_MAX 
		--更换装备
		self:__ClearUPEquipInfo()
		--装备状态更新
		self:__SetEquipState()
		--觉醒额外属性
		self:__SetExtalProperty()
		--当前阶段添加属性飘字
		self:__BounceTextSpecialEffect()
		--觉醒材料容器
		self:__SetAwakenMaterial()
		--按钮状态
		self:CheckButtonStatus()
		-- --按钮回复
		-- self.__button_awaken:SetEnabled(true)	
	end)

	--装备觉醒道具事件
	self.__equip_awaken_prop_event = self:BindGlobalEvent(Macros.Event.HeroModule.HERO_EQUIP_AWAKEN_PROP_RESP, function(_,data)
		if self.__templ_hero_awakenprop_view then
			self.__templ_hero_awakenprop_view:DeleteMe()
			self.__templ_hero_awakenprop_view = nil
		end
		local equip_index = 0
		for i,equipId in ipairs(self.__awaken_config.cost_item_list) do
			if equipId == data.awaken_item_id then
				self.__equipinfo[i] = true
				equip_index = i
				break
			end			
		end
		if self.__windwords_effect then
			self.__windwords_effect:__RestoreStatus()
			self.__windwords_effect:DeleteMe()
			self.__windwords_effect = nil
		end
		--重设文字
		if self.__is_on_awakening then
			self:__SetCurProperty()
		end
		--装备道具飘字
		local awaken_config = CPPGameLib.GetConfig("AwakeningItem",data.awaken_item_id)
		local windwords_list = {}
		for i,pro_type in ipairs(awaken_config.pro_type_list) do
			self["__text_property"..pro_type]:SetVisible(true)
			self["__text_property"..pro_type]:SetPosition(self.__org_pos_list[pro_type].pos_x,self.__org_pos_list[pro_type].pos_y)			
			local str = Util:Nature(pro_type, awaken_config.pro_value_list[i], nil, nil, true)
			self["__text_property"..pro_type]:SetText(str)
			local info = {}
			info.node = self["__text_property"..pro_type]
			info.anim_type = Macros.Game.WIND_WORDS_TYPE.UPSLOPE		
			info.tgt_pos = {x = -5 , y = self["__text_property"..pro_type]:GetPositionY() + 50} 
			table.insert(windwords_list,info)
		end	
		self:__TextDelete()		
		self.__windwords_effect = WindWordsEffects.New()
		self.__windwords_effect:Wind(windwords_list,function ()
			if self.__windwords_effect then
				self.__windwords_effect:DeleteMe()
				self.__windwords_effect = nil
			end
		end)
		--装备状态更新
		self:__SetEquipState()
		--添加特效
		local effectid,effect = self:__CreateEffect({res_name = "UI_juexingzhuangbei",loop = false},self["__panel_pos"..equip_index],5)
		PosTool.Center(effect)		
		--按钮状态
		self:CheckButtonStatus()
	end)

	--英雄详细信息反馈事件
	self.__detail_info_resp_event = self:BindGlobalEvent(Macros.Event.HeroModule.HERO_DETAIL_INFO_RESP, function(_,detail_info)
		if self:IsOpen() and self.__parent_view:GetCurHeroUID() == detail_info.hero_uid then
			self:__Reset(detail_info)
		end
	end)
	self:CallModuleFunc("HeroDetailInfoReq", self.__hero_uid)
end

--播放文字特效
function Modules.HeroAwakenTabView:__BounceTextSpecialEffect()		  
	local windwords_list = {}
	local hero_info = self:CallModuleFunc("GetHeroInfo", self.__hero_uid)
	local plus_prop_map = ConfigAdapter.Hero.GetAwakenPlusPropMap(hero_info.info_id,self.__awaken_star,self.__awaken_level,nil,true)
	for prop_type, lab_prop in pairs(self.__cur_prop_map) do
		self["__text_property"..prop_type]:SetVisible(true)
		local info = {}		
		self["__text_property"..prop_type]:SetPosition(self.__org_pos_list[prop_type].pos_x,self.__org_pos_list[prop_type].pos_y)			
		local str = Util:Nature(prop_type, plus_prop_map[prop_type], nil, nil, true)
		self["__text_property"..prop_type]:SetText(str)
		info.node = self["__text_property"..prop_type]
		info.anim_type = Macros.Game.WIND_WORDS_TYPE.MOVING_TO_LOCATION		
		info.tgt_pos = {x = self.__windwords_pos_list[prop_type].x,y = self.__windwords_pos_list[prop_type].y} 
		table.insert(windwords_list,info)
	end
	self.__windwords_effect = WindWordsEffects.New()
	self.__windwords_effect:Wind(windwords_list,function ()
		self:__TextScaleToSpecialEffect()
	end)	
end

--文字滚动效果
function Modules.HeroAwakenTabView:__TextScaleToSpecialEffect()
	self:__TextDelete()
	local hero_info = self:CallModuleFunc("GetHeroInfo", self.__hero_uid)
	local plus_prop_map = ConfigAdapter.Hero.GetAwakenPlusPropMap(hero_info.info_id,self.__awaken_star,self.__awaken_level,nil,true)
	local count_index = 0
	for prop_type, lab_prop in pairs(self.__cur_prop_map) do
		count_index = count_index + 1
		self["rollText_"..prop_type] = TypefaceEffects.New()
		self["rollText_"..prop_type]:Roll(lab_prop,plus_prop_map[prop_type],nil,
			count_index == 4 and function ()
				self:__SetCurProperty()
				self.__is_on_awakening = false
			end or nil)
	end
end

--删除
function Modules.HeroAwakenTabView:__TextDelete()
	for prop_type, lab_prop in pairs(self.__cur_prop_map) do
		if self["rollText_"..prop_type] then
			self["rollText_"..prop_type]:DeleteMe()
			self["rollText_"..prop_type] = nil
		end
	end
	if self.__windwords_effect then
		self.__windwords_effect:DeleteMe()
		self.__windwords_effect = nil
	end
end

--检查按钮状态
function Modules.HeroAwakenTabView:CheckButtonStatus()
	local is_could_awaken = self:__CheckAwaken(true)
	if is_could_awaken and not self.__button_zoubian_effectid then
		--加走边特效
		local effect
		self.__button_zoubian_effectid,effect = self:__CreateEffect({res_name = "ui_anniuzoubian"},self.__button_awaken,2)
		PosTool.Center(effect)
	end
	if self.__button_zoubian_effectid then
		local effect_root_id = GlobalEffectMgr:GetEffect(self.__button_zoubian_effectid)
		if effect_root_id then
			effect_root_id:SetVisible(is_could_awaken)
		end
	end
end

--觉醒之前的特效
function Modules.HeroAwakenTabView:RunBeforeAwakenEffect()
	self:DelFlashEffect()
	local pos_list = {70,80,90,75}
	local effect,effectid
	local count_index = 1
	local function runFlashEffect()
		if count_index <= 4 then
			effectid,effect = self:__CreateEffect({res_name = "UI_juexingzhuangbeifeiru",loop = false,scale = 1.2,time_scale = 1.5},self["__panel_pos"..count_index],3)
			PosTool.Center(effect)
			CPPActionManager.DelayTo(self["__panel_pos"..count_index],0.08,GlobalCallbackMgr:AddCallBackFunc(function ()
				self["__flash_effectid"..count_index],self["__flash_effect"..count_index] = self:__CreateEffect({res_name = "UI_juexingzhuangbeifeiru1"},self["__panel_pos"..count_index],4)
				PosTool.Center(self["__flash_effect"..count_index])
				self["__flash_effect"..count_index]:SetScaleFactor(0.1)
				ActionManager:RunScaleToAndMoveByEffectEx(self["__flash_effect"..count_index],0.2,0,pos_list[count_index],1,count_index == 4 and function ()
					self:RunBeforeAwakenEffectTwo()
				end or nil)	
				count_index = count_index + 1
				runFlashEffect()							
			end))
		end
	end
	runFlashEffect()
end

--飞过去
function Modules.HeroAwakenTabView:RunBeforeAwakenEffectTwo()
	CPPActionManager.DelayTo(self.__panel_pos1,0.2,GlobalCallbackMgr:AddCallBackFunc(function ()
		for count_index=1,4 do
			CPPActionManager.MoveBy(self["__flash_effect"..count_index],0.6,0,-30,GlobalCallbackMgr:AddCallBackFunc(function ()
				CPPActionManager.MoveTo(self["__flash_effect"..count_index],0.2,-250,130,GlobalCallbackMgr:AddCallBackFunc(function ()
					self:__DelEffect(self["__flash_effectid"..count_index])
					self["__flash_effectid"..count_index] = nil
					if count_index == 4 then
						local upgrade_effect_id,effect = self:__CreateEffect({id = 100013,loop = false}, self.__panel_hero_commodel, 2)
						PosTool.CenterBottom(effect)
						self:CallModuleFunc("HeroAwakenReq",self.__hero_uid,self.__cost_hero_list)
					end
				end))	
			end))		
		end	
	end))
end

function Modules.HeroAwakenTabView:DelFlashEffect()
	for count_index=1,4 do
		if self["__flash_effectid"..count_index] then			
			CPPActionManager.StopAllActions(self["__flash_effect"..count_index])
			self["__flash_effect"..count_index] = nil
			self:__DelEffect(self["__flash_effectid"..count_index])
			self["__flash_effectid"..count_index] = nil
		end					
	end	
end

--选项卡界面关闭回调
function Modules.HeroAwakenTabView:__CloseCallback()
	if self.__button_preLook then
		self.__button_preLook:SetVisible(false)
		self.__text_speciallock:SetVisible(false)
		self.__text_property:SetVisible(false)
	end
	if self.__button_zoubian_effectid then
		self:__DelEffect(self.__button_zoubian_effectid)
		self.__button_zoubian_effectid = nil
	end
	self:__TextDelete()
	self:DelFlashEffect()
end

--选项卡界面销毁回调(释放非托管资源)
function Modules.HeroAwakenTabView:__Dispose()
end

function Modules.HeroAwakenTabView:__Reset(detail_info)
	--英雄信息配置
	local hero_config = CPPGameLib.GetConfig("Hero", detail_info.hero_info_id)
	self.__info_id = detail_info.hero_info_id
	--觉醒数据
	self.__special_id = hero_config.init_quality * 10 + hero_config.profession
	local awaken_config = CPPGameLib.GetConfig("HeroAwakening", self.__special_id)
	self.__awaken_config = awaken_config[detail_info.awaken_star][detail_info.awaken_level]	
	--是否已经装备装备数据
	local equipdata = {}
	for i,equip_info in ipairs(detail_info.awaken_item_list) do
		equipdata[equip_info.item_id] = equip_info.item_id
	end

	--延迟缩小
	CPPActionManager.DelayTo(self.__panel_hero_commodel,0.2,GlobalCallbackMgr:AddCallBackFunc(function()
		for prop_type, lab_prop in pairs(self.__cur_prop_map) do
			lab_prop:SetScaleFactor(1)
			self.__next_prop_map[prop_type]:SetScaleFactor(1)
		end
	end))

	--觉醒等级
	self.__awaken_star = detail_info.awaken_star
	self.__awaken_level = detail_info.awaken_level
	--是否满级
	self.__is_fulllevel = self.__awaken_star >= Macros.Game.HERO_AWAKE_STAR_MAX 

	--装备信息
	self.__equipinfo = {}	
	--装备信息
	for i=1,4 do
		self["__panel_pos"..i]:SetVisible(not self.__is_fulllevel)
		if not self.__is_fulllevel then
			if not self["__panel_icon_pos"..i] then
				--图标
				self["__panel_icon_pos"..i] = LogicGUI.IconLayer.New(false,false) 
				self["__panel_pos"..i]:AddChild(self["__panel_icon_pos"..i]:GetNode())			
				self["__panel_pos_add"..i] = WidgetUnity.GetWidgetByName(self["__panel_pos"..i],"Image_add")
				self["__panel_pos_add"..i]:SetZOrder(2)
				self["__pos_text_condition"..i] = WidgetUnity.GetWidgetByName(self["__panel_pos"..i],"Text_condition")
				self["__pos_text_condition"..i]:SetZOrder(2)		
				PosTool.Center(self["__panel_icon_pos"..i])
				--遮罩
				self["__panel_pos_shield"..i] = Util:Layout(80, 80)
				self["__panel_pos_shield"..i]:SetAnchorPoint(0.5,0.5)
				self["__panel_pos_shield"..i]:SetZOrder(1)
				self["__panel_pos"..i]:AddChild(self["__panel_pos_shield"..i])
				PosTool.Center(self["__panel_pos_shield"..i])
				Util:SetLayoutBGColor(self["__panel_pos_shield"..i], Macros.Color.lilyBlack,128)
			end
			self["__panel_icon_pos"..i]:SetData({id = self.__awaken_config.cost_item_list[i]})
			self.__equipinfo[i] = equipdata[self.__awaken_config.cost_item_list[i]] and true or false
		end
	end
	--装备状态更新
	self:__SetEquipState()
	--觉醒额外属性
	self:__SetExtalProperty()
	--当前阶段添加属性
	self:__SetCurProperty()
	--觉醒材料容器
	self:__SetAwakenMaterial()
	--检查按钮状态
	self:CheckButtonStatus()
end

--检查能否觉醒
function Modules.HeroAwakenTabView:__CheckAwaken(is_pass)
	--觉醒装备检查
	for i=1,4 do
		if not self.__equipinfo[i] then
			return false,CPPGameLib.GetString("hero_awaken_awakenEquipTips")
		end
	end

	--觉醒消耗金币检查
	local moneyNum = CallModuleFunc(ModuleType.ROLE,"GetPropValue",self.__awaken_config.cost_money[1])
	if moneyNum < self.__awaken_config.cost_money[2] then
		return false,CPPGameLib.GetString("hero_awaken_awakenMoneyTips")
	end

	--觉醒消耗材料检查
	local num = ObjIDTool.GetOwnNum(self.__awaken_config.cost_item[1])
	if num < self.__awaken_config.cost_item[2] then
		if not is_pass then
			--获取途径
			GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, self.__awaken_config.cost_item[1])
		end
		return false,CPPGameLib.GetString("hero_awaken_awakenMaterialTips")
	end

	--觉醒消耗英雄检查
	if self.__awaken_config.cost_hero_num ~= 0 then
		num,self.__cost_hero_list = CallModuleFunc(ModuleType.HERO,"GetHeroNumAndUidByInfoId",self.__info_id,self.__hero_uid,self.__awaken_config.cost_hero_num)
		if num < self.__awaken_config.cost_hero_num then
			return false,CPPGameLib.GetString("hero_awaken_awakenMaterialTips")
		end
	end
		
	local hero_info = self:CallModuleFunc("GetHeroInfo", self.__hero_uid)
	--觉醒英雄等级检测
	if self.__awaken_config.need_hero_level > hero_info.level then
		return false,CPPGameLib.GetString("hero_awaken_awakenHeroLevelTips",self.__awaken_config.need_hero_level)
	end

	return true
end

--装备状态更新
function Modules.HeroAwakenTabView:__SetEquipState()
	if self.__is_fulllevel then
		return
	end	
	for i=1,4 do
		local propNum = ObjIDTool.GetOwnNum(self.__awaken_config.cost_item_list[i])
		--图标是否变灰
		self["__panel_pos_shield"..i]:SetVisible(not self.__equipinfo[i])
		--加号和文字是否显示
		local is_visible,condition,txt_str,txt_col
		local is_could_compose = true
		if not self.__equipinfo[i] then	
			is_visible = true			
			local ap_config = CPPGameLib.GetConfig("AwakeningItem",self.__awaken_config.cost_item_list[i])
			if ap_config.quality > Macros.Game.QualityType.WHITE then
				for i,compose_id in ipairs(ap_config.compose_item_id_list) do
					local propNum = ObjIDTool.GetOwnNum(compose_id)
					if propNum < ap_config.compose_item_num_list[i] then
						is_could_compose = false
						break
					end					
				end				
			end
			txt_str = propNum > 0 and CPPGameLib.GetString("hero_awaken_Could_Equip") or (ap_config.quality == Macros.Game.QualityType.WHITE and CPPGameLib.GetString("hero_awaken_Could_reward") or 
						(is_could_compose and CPPGameLib.GetString("hero_awaken_Could_Compound") or CPPGameLib.GetString("hero_awaken_Could_reward")))
			txt_col = propNum > 0 and Macros.Color.green or Macros.Color.btn
			condition =  propNum > 0 and 3 or (ap_config.quality == Macros.Game.QualityType.WHITE and 2 or 4)
		else
			is_visible = false
			condition = 1
		end
		self["__panel_pos_add"..i]:SetVisible(is_visible)
		self["__pos_text_condition"..i]:SetVisible(is_visible)
		--可见则设置内容
		if is_visible then
			self["__pos_text_condition"..i]:SetText(txt_str)
			self["__pos_text_condition"..i]:SetTextColor(unpack(txt_col))
		end
		--点击控件回调
		WidgetUnity.SetWidgetListener(self["__panel_pos"..i],nil, nil, function ()
			self.__templ_hero_awakenprop_view = LogicGUI.TemplHeroAwakenPropView.New(self.__awaken_config.cost_item_list[i],condition,self.__hero_uid)
		end)				
	end	
end

--觉醒材料容器
function Modules.HeroAwakenTabView:__ClearUPEquipInfo()
	for i=1,4 do
		self["__panel_pos"..i]:SetVisible(not self.__is_fulllevel)
		if not self.__is_fulllevel then
			self["__panel_icon_pos"..i]:SetData({id = self.__awaken_config.cost_item_list[i]})
			self.__equipinfo[i] = false			
		end
	end
end

--觉醒材料容器
function Modules.HeroAwakenTabView:__SetAwakenMaterial()
	local sp = nil
	self.__cost_hero_list = {}

	self.__image_coin:SetVisible(not self.__is_fulllevel)
	self.__text_coinnum:SetVisible(not self.__is_fulllevel)
	self.__panel_material1:SetVisible(not self.__is_fulllevel)
	self.__panel_material2:SetVisible(not self.__is_fulllevel)

	--货币
	self.__image_coin:SetImage(Util:GetMoneyIconImgData(self.__awaken_config.cost_money[1]))
	--货币数量
	self.__text_coinnum:SetText(self.__awaken_config.cost_money[2])

	--觉醒材料容器
	--普通材料
	if not self.__is_fulllevel then
		if not self.__material_icon1 then
			self.__material_icon1 = LogicGUI.IconLayer.New(false,true) 
			self.__panel_material1:AddChild(self.__material_icon1:GetNode())
			PosTool.Center(self.__material_icon1)
			--数量文字
			self.__show_num_label1 = Util:Label("", Macros.TypefaceSize.minimum)
			self.__show_num_label1:SetAnchorPoint(1,0)
			self.__material_icon1:GetNode():AddChild(self.__show_num_label1,100)
			PosTool.RightBottom(self.__show_num_label1,72,5)
		end	
		local num = ObjIDTool.GetOwnNum(self.__awaken_config.cost_item[1])
		local str = num.."/"..self.__awaken_config.cost_item[2]
		self.__material_icon1:SetData({id = self.__awaken_config.cost_item[1]})
		--设置回调
		self.__material_icon1:SetTouchCallback(function ()
			--获取途径
			GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, self.__awaken_config.cost_item[1])
		end)
		local txt_col = num >= self.__awaken_config.cost_item[2] and Macros.Color.green or Macros.Color.red
		local txt_stroke = num >= self.__awaken_config.cost_item[2] and Macros.Color.green_stroke or Macros.Color.red_stroke
		self.__show_num_label1:SetString(str)
		self.__show_num_label1:SetColor(unpack(txt_col))
		self.__show_num_label1:SetStrokeColor(unpack(txt_stroke))
		self.__show_num_label1:SetStrokeWidth(2)
		--需要英雄
		self.__panel_material2:SetVisible(self.__awaken_config.cost_hero_num ~= 0)
		if self.__awaken_config.cost_hero_num ~= 0 then
			--消耗英雄
			num,self.__cost_hero_list = CallModuleFunc(ModuleType.HERO,"GetHeroNumAndUidByInfoId",self.__info_id,self.__hero_uid,self.__awaken_config.cost_hero_num)
			if not self.__material_icon2 then
				self.__material_icon2 = LogicGUI.IconLayer.New(true) 
				self.__panel_material2:AddChild(self.__material_icon2:GetNode())
				PosTool.Center(self.__material_icon2)
				--数量文字
				self.__show_num_label2 = Util:Label("", Macros.TypefaceSize.minimum)
				self.__show_num_label2:SetAnchorPoint(1,0)
				self.__material_icon2:GetNode():AddChild(self.__show_num_label2,100)
				PosTool.RightBottom(self.__show_num_label2,72,5)
			end
			str = num.."/"..self.__awaken_config.cost_hero_num
			self.__material_icon2:SetData({id = self.__info_id})			
			txt_col = num >= self.__awaken_config.cost_hero_num and Macros.Color.green or Macros.Color.red
			txt_stroke = num >= self.__awaken_config.cost_hero_num and Macros.Color.green_stroke or Macros.Color.red_stroke
			self.__show_num_label2:SetString(str)
			self.__show_num_label2:SetColor(unpack(txt_col))
			self.__show_num_label2:SetStrokeColor(unpack(txt_stroke))
			self.__show_num_label2:SetStrokeWidth(2)
		end
	end			
	self.__button_awaken:SetEnabled(not self.__is_fulllevel)
end

--当前阶段添加属性
function Modules.HeroAwakenTabView:__SetCurProperty()
	local hero_info = self:CallModuleFunc("GetHeroInfo", self.__hero_uid)
	local plus_prop_map = ConfigAdapter.Hero.GetAwakenPlusPropMap(hero_info.info_id,self.__awaken_star,self.__awaken_level,nil,true)
	local next_star = self.__awaken_level == Macros.Game.HERO_AWAKE_LEVEL_MAX and (self.__awaken_star + 1) or self.__awaken_star
	local next_level = self.__awaken_level == Macros.Game.HERO_AWAKE_LEVEL_MAX and 0 or (self.__awaken_level + 1)
	local next_plus_prop_map = ConfigAdapter.Hero.GetAwakenPlusPropMap(hero_info.info_id, next_star,next_level,nil,true)

	--当前和下阶星级
	self.__text_starstagefirst:SetText(CPPGameLib.GetString("hero_awaken_propertyTitle",self.__awaken_star,self.__awaken_level))
	self.__text_starstageafter:SetText(CPPGameLib.GetString("hero_awaken_propertyTitle",next_plus_prop_map and next_star or self.__awaken_star,next_plus_prop_map and next_level or self.__awaken_level))	
	self.__image_fulllevel:SetVisible(self.__is_fulllevel)
	next_plus_prop_map = next_plus_prop_map and next_plus_prop_map or plus_prop_map
	--当前星数和下阶星数属性加成
	local str_prop_type, str_prop_value, lab_prop	
	local function handle_prop(prop_type, prop_value, is_cur)		
		lab_prop = is_cur and self.__cur_prop_map[prop_type] or self.__next_prop_map[prop_type]
		lab_prop:SetText(prop_value)
	end
	for prop_type, lab_prop in pairs(self.__cur_prop_map) do
		handle_prop(prop_type, plus_prop_map[prop_type] or 0, true)
	end

	for prop_type, lab_prop in pairs(self.__next_prop_map) do
		handle_prop(prop_type, next_plus_prop_map[prop_type] or 0)
	end
end

--觉醒额外属性
function Modules.HeroAwakenTabView:__SetExtalProperty()
	local is_needbreak = false	
	local first_time = true
	--满级
	local awaken_config,awaken_info
	if self.__is_fulllevel then
		awaken_config = CPPGameLib.GetConfig("HeroAwakening", self.__special_id)
		awaken_info = awaken_config[Macros.Game.HERO_AWAKE_STAR_MAX][self.__awaken_level]
		if awaken_info.extra_pro_des_list then
			self:__SetPropertyText(Macros.Game.HERO_AWAKE_STAR_MAX,self.__awaken_level,awaken_info)
		end
	else
		local i = self.__awaken_star
		while i <= Macros.Game.HERO_AWAKE_STAR_MAX do
			local j = 0
			while j <= Macros.Game.HERO_AWAKE_LEVEL_MAX do
				if first_time then
					if self.__awaken_level == Macros.Game.HERO_AWAKE_LEVEL_MAX then
						i = self.__awaken_star + 1
					else
						j = self.__awaken_level + 1
					end				
					first_time = false
				end
				awaken_config = CPPGameLib.GetConfig("HeroAwakening", self.__special_id)
				awaken_info = awaken_config[i][j]
				if awaken_info.extra_pro_des_list then
					is_needbreak = true
					self:__SetPropertyText(i,j,awaken_info)
					break			
				end
				j = j + 1 								
			end
			if is_needbreak then
				break
			end	
			i = i + 1	
		end
	end		
end

--觉醒额外属性文字
function Modules.HeroAwakenTabView:__SetPropertyText(star,level,data)
	self.__text_speciallock:SetString(CPPGameLib.GetString("hero_awaken_Extra_propertyTitle",star,level)) 
	self.__text_property:SetString(data.extra_pro_des_list or "")
end

--切换英雄
function Modules.HeroAwakenTabView:RefreshBySelectedHero()
	self:__TextDelete()	
	self:DelFlashEffect()
	self.__button_awaken:SetEnabled(true)
	self.__hero_uid = self.__parent_view:GetCurHeroUID()
	self:CallModuleFunc("HeroDetailInfoReq", self.__hero_uid)
end
