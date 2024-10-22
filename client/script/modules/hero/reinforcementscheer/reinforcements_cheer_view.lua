--
-- @brief 援军助威
-- @auther: ldx
-- @date 2018年8月02日 10:17:22

Modules = Modules or {}
Modules.ReinforcementsCheerView = Modules.ReinforcementsCheerView or BaseClass(BaseView)

function Modules.ReinforcementsCheerView:__init()
 	self.__layout_name = "reinforcements_cheer_view"
 	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_reinforcementscheer")}
 	self.__title_bar_params = 
	{
		title = CPPGameLib.GetString("reinforcements_cheerlevel_title"),
		close_func = function()
			if self.__sub_type == BaseViewSubType.HeroLineup.MAIN then
				self:CloseManual()
			else
				--主界面
				self:__ChangeSubView(BaseViewSubType.HeroLineup.MAIN)
			end
		end,
	}
 	self.__open_callback = function ()
 		self:__OpenCallback()
 	end
 	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_REINFORCEMENTS_CHEER_HERO_CELL)
 	self:__AddPreloadTemplate(GUI.TemplateRegistry.HERO_LINEUP_ITEM)
 	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.REINFORCEMENTS_CHEER)
end 	

function Modules.ReinforcementsCheerView:__RegistAllEvents()
    --援军上阵事件
	self:BindGlobalEvent(Macros.Event.HeroModule.HERO_REINFORCEMENTS_ON_BATTLE, function(_, pos) 
		self.__before_cheer_level = self.__cur_cheer_level  
        self:__ChangeSubView(BaseViewSubType.HeroLineup.MAIN)
    end)  
end

function Modules.ReinforcementsCheerView:__OpenCallback()
	self.__before_cheer_level = unpack(self.__open_data)
	--援军容器
	self.__panel_reinforcementscheer = self:GetWidget("Panel_ReinforcementsCheer")
	--选择容器
	self.__panel_select = self:GetWidget("Panel_Select")
	--领取提示
	self.__txt_reinforcementscheer = self:GetWidget("Text_ReinforcementsCheer")
	Util:WidgetLabel(self.__txt_reinforcementscheer,CPPGameLib.GetString("reinforcements_cheer_work_tips"),Macros.TypefaceSize.largish, Macros.Color.orange,0,0,Macros.Color.orange_stroke)	
	--左箭头
  	WidgetUnity.SetWidgetListener(self:GetWidget("Image_LeftPage"),nil,nil,function ()
    	if self.__reinforcement_scrollview then
    		self.__reinforcement_scrollview:LocateToTop()
    	end
  	end)
	--右箭头
	WidgetUnity.SetWidgetListener(self:GetWidget("Image_RightPage"),nil,nil,function ()
	    if self.__reinforcement_scrollview then
    		self.__reinforcement_scrollview:LocateToLast()
    	end
	end)
	--主界面
	self:__ChangeSubView(BaseViewSubType.HeroLineup.MAIN)
end

function Modules.ReinforcementsCheerView:GetOpenParams()
	return {self.__cur_cheer_level}
end

--切换子界面
function Modules.ReinforcementsCheerView:__ChangeSubView(type)
	--子页面
	self.__sub_type = type or BaseViewSubType.HeroLineup.MAIN	
	if self.__panel_now then
		self.__panel_now:SetVisible(false)
		self.__panel_now = nil
	end
	self.__panel_select:SetVisible(false)

	local title = ""
	CPPGameLib.Switch(self.__sub_type)
	{
		[BaseViewSubType.HeroLineup.MAIN] = function()
			title = CPPGameLib.GetString("reinforcements_cheerlevel_title")
			self.__panel_now = self.__panel_reinforcementscheer
			self:__HandleMain()
		end,
		[BaseViewSubType.HeroLineup.SELECT] = function()
			title = CPPGameLib.GetString("hero_lineup_select_title")
			self.__panel_now = self.__panel_select
			self:__HandleSelect()
		end,
	}

	if self.__panel_now then
		self.__panel_now:SetVisible(true)
	end

	self:SetTitle(title)
end

--------------------------------------------------------------------------------------------------------------------------------
--主界面
--------------------------------------------------------------------------------------------------------------------------------
function Modules.ReinforcementsCheerView:__HandleMain()
	--角色等级
	local roleLevel = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
  	--获取等级限制
  	local config_reinforcement = CPPGameLib.GetConfig("HeroLineup",Macros.Game.HeroLineup.REINFORCEMENT)
  	--获取援军队列
  	local hero_reinforcement_list = self:CallModuleFunc("GetHeroReinforcementList")
 	if not hero_reinforcement_list then
    	return
  	end
  	--援军最低等级
  	local least_reinforcement_level
  	--队列数据列表
  	self.__reinforcement_lineup_info_list = {}
  	local cur_lineup_num = 0
  	--是否有空位
  	local is_have_nil = false
  	--遍历援军
  	for lineup_index, lineup_info in ipairs(hero_reinforcement_list) do
	    local hero_info = self:CallModuleFunc("GetHeroInfo",lineup_info.uid)   
	    if hero_info then
	    	least_reinforcement_level = least_reinforcement_level or hero_info.level
	    	least_reinforcement_level = hero_info.level < least_reinforcement_level and hero_info.level or least_reinforcement_level
	    	local lineup_data = CPPGameLib.CopyTblShallow(hero_info)
	    	lineup_data.lineup_index = lineup_index
	    	lineup_data.is_add = false
	      	cur_lineup_num = cur_lineup_num + 1
	      	table.insert(self.__reinforcement_lineup_info_list,lineup_data)
	    else
	    	if roleLevel >= config_reinforcement.pos_list[lineup_index].need_level then
	    		is_have_nil = true
	    		table.insert(self.__reinforcement_lineup_info_list,{is_add = true,lineup_index = lineup_index})
	    	end
	    end
  	end
  	least_reinforcement_level = not is_have_nil and least_reinforcement_level or nil
  	print("skfb","!!!!!trrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr ",least_reinforcement_level,is_have_nil)
  	--有英雄在前面
  	table.sort(self.__reinforcement_lineup_info_list,function (lineup_data1,lineup_data2)
  		if lineup_data1.is_add or lineup_data2.is_add then
  			if lineup_data1.is_add and lineup_data2.is_add then
  				return lineup_data1.lineup_index < lineup_data2.lineup_index
  			elseif lineup_data1.is_add and not lineup_data2.is_add then
  				return false
  			elseif not lineup_data1.is_add and lineup_data2.is_add then
  				return true
  			end
  		else
  			return lineup_data1.lineup_index < lineup_data2.lineup_index
  		end
  	end) 	
  	--已上阵援军
  	if not self.__txt_havereinforcementsnum then
	  	self.__txt_havereinforcementsnum = self:GetWidget("Text_HaveReinforcementsNum") 
	  	Util:WidgetLabel(self.__txt_havereinforcementsnum,"",Macros.TypefaceSize.largish, Macros.Color.orange,0,0,Macros.Color.orange_stroke)
  	end
  	self.__txt_havereinforcementsnum:SetText(CPPGameLib.GetString("reinforcements_cheer_lineup_num",cur_lineup_num,#self.__reinforcement_lineup_info_list))
  	--助威表
  	local config_encourage = CPPGameLib.GetConfig("ReinforcementEncourage",nil,nil,true)
  	--助威增加属性
  	local cur_cheer_level = 0
  	local next_cheer_level = 1
  	local cur_cheer_info
  	local max_config_encourage = config_encourage[table.maxn(config_encourage)]
  	if least_reinforcement_level then	
  		if least_reinforcement_level > max_config_encourage.reinforcement_level_max then
  			cur_cheer_level = max_config_encourage.encourage_level
  			cur_cheer_info = max_config_encourage
  		else
	  		for _,reinforcement_info in pairs(config_encourage) do
	  			if reinforcement_info.reinforcement_level_max >= least_reinforcement_level and reinforcement_info.reinforcement_level_min <= least_reinforcement_level then 
	  				cur_cheer_level = reinforcement_info.encourage_level	
	  				cur_cheer_info = reinforcement_info		
	  				break
	  			end  			
	  		end 		
	  	end
  	end
  	next_cheer_level = math.min(cur_cheer_level+1,max_config_encourage.encourage_level)
  	--下一级信息
  	local next_cheer_info
  	for _,v in pairs(config_encourage) do
  		if v.encourage_level == next_cheer_level then
	  		next_cheer_info = v
	  		break
	  	end
  	end
  	self.__cur_cheer_level = cur_cheer_level
  	if self.__before_cheer_level and self.__cur_cheer_level > self.__before_cheer_level then
  		self.__is_windwords = true
  	end
  	self.__windwords_pos_list = self.__windwords_pos_list or {}
  	self.__windword_org_pos_list = self.__windword_org_pos_list or {}
  	--处理属性
  	local HandlePropertyFunc = function (is_next)
  		local link_str = is_next and "Text_AfterProperty" or "Text_BeforeProperty"
  		local link_num_str = is_next and "Text_AfterPropertyNum" or "Text_BeforePropertyNum"
  		local cheer_level = is_next and next_cheer_level or (self.__is_windwords and self.__before_cheer_level or cur_cheer_level)
  		local cheer_info = is_next and next_cheer_info or cur_cheer_info
  		--援军助威等级
  		if not self[(is_next and "Text_CheerLevelAfter" or "Text_CheerLevelBefore")] then
	  		self[is_next and "Text_CheerLevelAfter" or "Text_CheerLevelBefore"] = self:GetWidget(is_next and "Text_CheerLevelAfter" or "Text_CheerLevelBefore")
	  		Util:WidgetLabel(self[is_next and "Text_CheerLevelAfter" or "Text_CheerLevelBefore"],CPPGameLib.GetString("reinforcements_cheerlevel_txt"),Macros.TypefaceSize.normal, Macros.Color.orange,0,0,Macros.Color.orange_stroke)
  		end
  		if not self[is_next and "Text_CheerLevelNumAfter" or "Text_CheerLevelNumBefore"] then
	  		self[is_next and "Text_CheerLevelNumAfter" or "Text_CheerLevelNumBefore"] = self:GetWidget(is_next and "Text_CheerLevelNumAfter" or "Text_CheerLevelNumBefore")
	  		Util:WidgetLabel(self[is_next and "Text_CheerLevelNumAfter" or "Text_CheerLevelNumBefore"],"",Macros.TypefaceSize.slightly,is_next and Macros.Color.green or Macros.Color.content,0,0,is_next and Macros.Color.green_stroke)
	  		if not is_next then
	  			local cur_txt = self[is_next and "Text_CheerLevelNumAfter" or "Text_CheerLevelNumBefore"]
  				self.__windwords_pos_list[0] = NodeUnity.ConvertPosBetweenTwoContainer(cur_txt:GetParent(), self.__panel_reinforcementscheer, COCOPoint(cur_txt:GetPositionX(),cur_txt:GetPositionY()))				
  			end
  		end
  		self[is_next and "Text_CheerLevelNumAfter" or "Text_CheerLevelNumBefore"]:SetText(CPPGameLib.GetString("common_lev8",is_next and next_cheer_level or (self.__is_windwords and self.__before_cheer_level or cur_cheer_level)))
  		--属性
  		self.__property_list = {Macros.Game.BattleProp.HP,Macros.Game.BattleProp.ATTACK,Macros.Game.BattleProp.PHYSICAL_DEFENCE,Macros.Game.BattleProp.MAGIC_DEFENCE}
  		for prop_index,prop_type in ipairs(self.__property_list) do
  			--飘字属性
  			print("skfb","p------------------------------------------ ",self["wind_words_levelup_"..prop_type],prop_type)
  			if not self["wind_words_levelup_"..prop_type] then
				self["wind_words_levelup_"..prop_type] = Util:WidgetLabel(self:GetWidget("Text_WindWords"..prop_type))
				Util:WidgetLabel(self["wind_words_levelup_"..prop_type],"", Macros.TypefaceSize.popup, Macros.Color.green,0,0,Macros.Color.green_stroke)
  				self.__windword_org_pos_list[prop_type] = {pos_x = self["wind_words_levelup_"..prop_type]:GetPositionX(),pos_y = self["wind_words_levelup_"..prop_type]:GetPositionY()}
  				print("skfb","t------------------------------------------ ",self["wind_words_levelup_"..prop_type],prop_type)
  			end
  			--文字
  			if not self[link_str..prop_type] then
	  			self[link_str..prop_type] = self:GetWidget(link_str..prop_type)
	  			Util:WidgetLabel(self[link_str..prop_type],CPPGameLib.GetString("common_str_with_colon",CPPGameLib.GetString("nature"..prop_type)),Macros.TypefaceSize.normal,Macros.Color.headline)
  			end
  			--数值
  			if not self[link_num_str..prop_type] then
	  			self[link_num_str..prop_type] = self:GetWidget(link_num_str..prop_type)
	  			Util:WidgetLabel(self[link_num_str..prop_type],"",Macros.TypefaceSize.slightly,is_next and Macros.Color.green or Macros.Color.content,0,0,is_next and Macros.Color.green_stroke)
  				if not is_next then
	  				--飘字属性位置
					self.__windwords_pos_list[prop_type] = NodeUnity.ConvertPosBetweenTwoContainer(self[link_num_str..prop_type]:GetParent(), self.__panel_reinforcementscheer, COCOPoint(self[link_num_str..prop_type]:GetPositionX(),self[link_num_str..prop_type]:GetPositionY()))
  				end
  			end
  			self[link_num_str..prop_type]:SetText(CPPGameLib.GetString("common_add9",cheer_info and cheer_info.pro_value_list[prop_index] or 0))
  		end
  		--下一级条件
  		if is_next then
  			if not self.__txt_reinforcementscondition then
	  			self.__txt_reinforcementscondition = self:GetWidget("Text_ReinforcementsCondition")
	  			Util:WidgetLabel(self.__txt_reinforcementscondition,"",14,Macros.Color.red,0,0,Macros.Color.red_stroke) 			
  			end
  			self.__txt_reinforcementscondition:SetText(CPPGameLib.GetString("reinforcements_cheerlevel_condition",next_cheer_info and next_cheer_info.reinforcement_level_min or 1))
  		end
  	end
  	HandlePropertyFunc()
  	HandlePropertyFunc(true)
  	--援军助威等级
  	if not self.__txt_reinforcementscheerlevel then
	  	self.__txt_reinforcementscheerlevel = self:GetWidget("Text_ReinforcementsCheerLevel") 
	  	Util:WidgetLabel(self.__txt_reinforcementscheerlevel,"",Macros.TypefaceSize.minimum, Macros.Color.orange,0,0,Macros.Color.orange_stroke)  	
  	end
  	self.__txt_reinforcementscheerlevel:SetText(CPPGameLib.GetString("reinforcements_cheer_level",cur_cheer_level))
  	--飘字
  	if not self.__txt_windwords_levelup then
  		self.__txt_windwords_levelup = self:GetWidget("Text_levelUp")
	  	Util:WidgetLabel(self.__txt_windwords_levelup,"",Macros.TypefaceSize.tab,Macros.Color.golden,0,0,Macros.Color.common_stroke)   		
  		self.__windword_org_pos_list[0] = {pos_x = self.__txt_windwords_levelup:GetPositionX(),pos_y = self.__txt_windwords_levelup:GetPositionY()}
  	end
  	--设置目标等级
  	for i,v in ipairs(self.__reinforcement_lineup_info_list) do
  		v.target_level = next_cheer_info and next_cheer_info.reinforcement_level_min or 1
  	end
  	if not self.__reinforcement_scrollview then
	  	--容器
		local panel_scrollview = self:GetWidget("Panel_ScrollView")
	  	--列表
	  	local params = {
	  		item_class = Modules.TemplReinforcementsCheerHeroCell,
		    item_width = 190,
		    item_height = 240,
		    row	= 1,
		    col	= 5,
		    horizon = true,
		    item_space_c = 10,
		    touchable = true,
	        touch_callback = function(item_index, item_data)	
	        	if item_data.is_add then        	
		        	--打开添加英雄界面
					self.__reinforcements_lineup_index = item_data.lineup_index   
					--选择英雄
					self:__ChangeSubView(BaseViewSubType.HeroLineup.SELECT)
				else
					CallModuleFunc(ModuleType.HERO, "OpenView", BaseViewType.HERO_PROMOTE, BaseViewSubType.HeroPromote.UPGRADE, item_data.uid,true)
				end
	        end,
		    view_width = 990,
		    view_height = 240,
	  	}
	  	self.__reinforcement_scrollview = GUI.ScrollView.New(params)
	  	panel_scrollview:AddChild(self.__reinforcement_scrollview:GetNode())
	  	PosTool.Center(self.__reinforcement_scrollview)
  	end
  	self.__reinforcement_scrollview:SetDataList(self.__reinforcement_lineup_info_list)
  	--飘字
    if self.__is_windwords then
    	self:__BounceTextSpecialEffect()
    end
end

--------------------------------------------------------------------------------------------------------------------------------
--选择英雄界面
--------------------------------------------------------------------------------------------------------------------------------
function Modules.ReinforcementsCheerView:__HandleSelect()
	if not self.__cbox_hide_lineuped then
		self.__is_filter_lineuped = true
		local function cbox_selected_func(selected)				
			self.__is_filter_lineuped = selected
			self.__scrollview_select_list:RefreshLayout()
		end  
		self.__cbox_hide_lineuped = self:GetWidget("CheckBox_HideLineuped")
		self.__cbox_hide_lineuped:SetSelectedState(true)
		WidgetUnity.SetCheckboxListener(self.__cbox_hide_lineuped, 
			function()
				cbox_selected_func(true)
			end, 
			function()
				cbox_selected_func(false)
			end)

		local cbox_title = Util:WidgetLabel(				
												self:GetWidget("Text_HideLineuped"), 
												CPPGameLib.GetString("hero_lineup_hide_linuped_hero"),
												nil,nil,nil,nil,Macros.Color.white_stroke
											)
		cbox_title:SetPosition( cbox_title:GetPositionX() + 20 , cbox_title:GetPositionY() )
	end

	--英雄列表
	if not self.__scrollview_select_list then
		local panel_select_list = self:GetWidget("Panel_SelectList")
		local size = panel_select_list:GetContentSize()
	    local params = 
	    {
	        item_class = Modules.TemplHeroReinforcementsLineupCell,
	        item_width = 455,
	        item_height = 122,
	        row = 4,
	        col = 2,
	        item_space_r = 5,
	        item_space_c = 7,
	        view_height = size.height,
	        filter_func = function(item_data)
	        	if not self.__is_filter_lineuped then
		        	return true
	        	end
	        	return item_data.hero_info.is_to_battle == Macros.Global.FALSE
	        end,
	        touchable = true,
	        touch_callback = function(item_index, item_data)	        	
	        	--援军上阵
	        	CallModuleFunc(ModuleType.HERO,"RecinforcementToBattleReq", item_data.hero_info.uid, self.__reinforcements_lineup_index,true)     
	        end,
	    }
	    self.__scrollview_select_list = GUI.ScrollView.New(params)
	    panel_select_list:AddChild(self.__scrollview_select_list:GetNode())
	    PosTool.Center(self.__scrollview_select_list)	
	end

	local lineup_info = CallModuleFunc(ModuleType.HERO,"GetReinforcementLineupInfo", self.__reinforcements_lineup_index)
	local hero_info = CallModuleFunc(ModuleType.HERO,"GetHeroInfo", lineup_info.uid)
	local hero_info_list = CallModuleFunc(ModuleType.HERO,"GetLineupSelectList", hero_info and hero_info.info_id or nil, Macros.Game.HeroLineup.REINFORCEMENT, self.__reinforcements_lineup_index)     
    self.__scrollview_select_list:SetDataList(hero_info_list)  
end

--播放文字特效
function Modules.ReinforcementsCheerView:__BounceTextSpecialEffect()	
	local config_encourage
	local before_config_encourage = self.__before_cheer_level == 0 and {} or nil
	for _,encourage_info in ipairs(CPPGameLib.GetConfig("ReinforcementEncourage",nil,nil,true)) do
		if encourage_info.encourage_level == self.__cur_cheer_level then
		 	config_encourage = encourage_info
		end
		if encourage_info.encourage_level == self.__before_cheer_level then
		 	before_config_encourage = encourage_info
		end
		if before_config_encourage and config_encourage then
			break
		end
	end
	if not config_encourage or not before_config_encourage then
		return
	end
	local windwords_list = {}
	for prop_index,prop_type in ipairs(self.__property_list) do
		local info = {}
		self["wind_words_levelup_"..prop_type]:SetVisible(true)
		self["wind_words_levelup_"..prop_type]:SetPosition(self.__windword_org_pos_list[prop_type].pos_x,self.__windword_org_pos_list[prop_type].pos_y)
		self["wind_words_levelup_"..prop_type]:SetText(Macros.Game.BattlePropName[prop_type].." "..CPPGameLib.GetString("common_add4",config_encourage.pro_value_list[prop_index] - (before_config_encourage.pro_value_list and before_config_encourage.pro_value_list[prop_index] or 0)))
		info.node = self["wind_words_levelup_"..prop_type]
		info.anim_type = Macros.Game.WIND_WORDS_TYPE.MOVING_TO_LOCATION		
		info.tgt_pos = {x = self.__windwords_pos_list[prop_type].x,y = self.__windwords_pos_list[prop_type].y} 
		table.insert(windwords_list,info)
	end
	self.__txt_windwords_levelup:SetVisible(true)
	self.__txt_windwords_levelup:SetPosition(self.__windword_org_pos_list[0].pos_x,self.__windword_org_pos_list[0].pos_y)
	self.__txt_windwords_levelup:SetText(CPPGameLib.GetString("reinforcements_cheerlevel_windwords",config_encourage.encourage_level))
	local info = {}
	info.node = self.__txt_windwords_levelup
	info.anim_type = Macros.Game.WIND_WORDS_TYPE.MOVING_TO_LOCATION		
	info.tgt_pos = {x = self.__windwords_pos_list[0].x,y = self.__windwords_pos_list[0].y} 
	table.insert(windwords_list,info)
	local windwords_effect = WindWordsEffects.New()
	windwords_effect:Wind(windwords_list,function ()
		self:__TextScaleToSpecialEffect()
		if windwords_effect then
			windwords_effect:DeleteMe()
			windwords_effect = nil
		end
	end)	
end

--文字滚动效果
function Modules.ReinforcementsCheerView:__TextScaleToSpecialEffect()
	local config_encourage
	local before_config_encourage = self.__before_cheer_level == 0 and {} or nil
	for _,encourage_info in ipairs(CPPGameLib.GetConfig("ReinforcementEncourage",nil,nil,true)) do
		if encourage_info.encourage_level == self.__cur_cheer_level then
		 	config_encourage = encourage_info
		end
		if encourage_info.encourage_level == self.__before_cheer_level then
		 	before_config_encourage = encourage_info
		end
		if before_config_encourage and config_encourage then
			break
		end
	end
	if not config_encourage or not before_config_encourage then
		return
	end
	self:__TextDelete()
	--滚字
	for prop_index, prop_type in ipairs(self.__property_list) do
		self["__txt_roll"..prop_type] = TypefaceEffects.New()
		self["__txt_roll"..prop_type]:RollEx(self["Text_BeforePropertyNum"..prop_type],before_config_encourage.pro_value_list and before_config_encourage.pro_value_list[prop_index] or 0,config_encourage.pro_value_list[prop_index],0.2,nil,function (cur_value)
			return CPPGameLib.GetString("common_add1")..cur_value
		end)
	end
	self.__txt_roll0 = TypefaceEffects.New()
	self.__txt_roll0:RollEx(self.Text_CheerLevelNumBefore,before_config_encourage.encourage_level or 0,config_encourage.encourage_level,0.2,nil,function (cur_value)
		return cur_value..CPPGameLib.GetString("reinforcements_cheerlevel_windwords_level")
	end)
	
end

--删除
function Modules.ReinforcementsCheerView:__TextDelete()
	for prop_index, prop_type in pairs(self.__property_list) do
		if self["__txt_roll"..prop_type] then
			self["__txt_roll"..prop_type]:DeleteMe()
			self["__txt_roll"..prop_type] = nil
		end
	end
	if self.__txt_roll0 then
		self.__txt_roll0:DeleteMe()
		self.__txt_roll0 = nil
	end
	self.__before_cheer_level = nil
end

function Modules.ReinforcementsCheerView:__Dispose()

	if self.__reinforcement_scrollview then
		self.__reinforcement_scrollview:DeleteMe()
		self.__reinforcement_scrollview = nil
	end

	if self.__scrollview_select_list then
		self.__scrollview_select_list:DeleteMe()
		self.__scrollview_select_list = nil
	end

	self:__TextDelete()
end

--
Modules = Modules or {}

Modules.TemplReinforcementsCheerHeroCell = Modules.TemplReinforcementsCheerHeroCell or BaseClass(GUI.ScrollItem)

function Modules.TemplReinforcementsCheerHeroCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_REINFORCEMENTS_CHEER_HERO_CELL)
end

function Modules.TemplReinforcementsCheerHeroCell:SetData(data)
	GUI.ScrollItem.SetData(self,data)
	--进度条
	local img_loadingbarbg = self:GetWidget("Image_LoadingBarBg")
	img_loadingbarbg:SetVisible(not self.__data.is_add)
	--英雄名字背景
	local img_heronamebg = self:GetWidget("Image_HeroNameBg")
	img_heronamebg:SetVisible(not self.__data.is_add)
	--加号
	self.__img_add = self:GetWidget("Image_Add")
	self.__img_add:SetVisible(self.__data.is_add)
	--
	if self.__data.is_add then
		--加号动作
		self.__funid = GlobalCallbackMgr:AddCallBackFunc(function ()
			CPPActionManager.FadeIn(self.__img_add, 0.5, GlobalCallbackMgr:AddCallBackFunc(function()
				self:RunAddAction()
			end))	
		end)
		self:RunAddAction()
	else
		local config_hero = CPPGameLib.GetConfig("Hero",self.__data.info_id)
		if not config_hero then
			return
		end
		--模型
		local panel_heromodule = self:GetWidget("Panel_HeroModule")
		self.__ui_hero = LogicGUI.Model.New({info_id = config_hero.model_id, parent = panel_heromodule})	
		self.__ui_hero:SetPosition(5,0)
		--缩放比例
		self.__ui_hero:SetScaleFactor(config_hero.ui_scale)
		--名字
		local txt_heroname = self:GetWidget("Text_HeroName")
		Util:WidgetLabel(txt_heroname,self.__data.break_level > 0 and CPPGameLib.GetString("common_add6", config_hero.name, self.__data.break_level) or config_hero.name,Macros.TypefaceSize.normal,Macros.Quality[self.__data.quality_level],0,0,Macros.Quality_Stroke[self.__data.quality_level])  	
		--进度条
		local loading_bar = self:GetWidget("LoadingBar")
		loading_bar:SetPercent(math.min(self.__data.level/self.__data.target_level,1)*100)
		--进度条进度
		local txt_progressnum = self:GetWidget("Text_ProgressNum")
		Util:WidgetLabel(txt_progressnum,CPPGameLib.GetString("spirit_numcost",self.__data.level,self.__data.target_level),Macros.TypefaceSize.minimum,Macros.Color.btn)
	end
end

function Modules.TemplReinforcementsCheerHeroCell:GetTouchNode()
	return self:GetNode()
end

function Modules.TemplReinforcementsCheerHeroCell:__delete()
	if self.__ui_hero then
		self.__ui_hero:DeleteMe()
		self.__ui_hero = nil
	end
end

--加号渐隐效果
function Modules.TemplReinforcementsCheerHeroCell:RunAddAction()
	CPPActionManager.FadeOut(self.__img_add,0.5,self.__funid)
end