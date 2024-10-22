--
-- @brief 精灵系统
-- @auther: ldx
-- @date 2016年9月30日 11:58:28
--

Modules = Modules or {}
Modules.SpiritView = Modules.SpiritView or BaseClass(BaseView)

local TAB_TYPE = {
	SPIRIT_DETAIL = 1, --精灵详情
	SPIRIT_INTENSIFY = 2,	--精灵强化
	SPIRIT_SUBLIMATE = 3,  --精灵升华
}

function Modules.SpiritView:__init()
	self.__layout_name = "spirit"
	self.__bg_params = {type = BaseView.BGType.IMAGE,image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_spirit")}
	self.__title_bar_params = 
	{
		close_func = function ()
		   if not CallModuleFunc(ModuleType.LEAD,"IsInLead") and self.__on_sending then
		   		return
		   end
		   self:CloseManual()
		end ,
		title = CPPGameLib.GetString("spirit_system"),
	}
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.SPIRIT)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.SPIRIT,true)

	self.__open_callback = function ()
		self.__spirit_data, _ = self:CallModuleFunc("GetSpiritData")
		self:__OpenCallback()
	end
end

function Modules.SpiritView:__OpenCallback()
	self.__open_spirit_id = unpack(self.__open_data)
	--规则按钮
	local btn_rule = Util:WidgetButton( self:GetWidget("Button_rule") , nil , function ( ... )
        local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
        help:SetLoadedCallback( function ( ... )
           help:SetCell("Rule_Sprite")
        end )
	end )
	btn_rule:SetZOrder(100)

	--根节点
	self.__panel_spirit = self:GetWidget("Panel_spirit")
		
	--精灵名字
	self.__text_spiritname = self:GetWidget("Text_spiritName")
	Util:WidgetLabel(self.__text_spiritname, "", Macros.TypefaceSize.tab)
	--精灵模型容器
	self.__panel_spiritmodule = self:GetWidget("Panel_spiritModule")

	--精灵图鉴
	Util:WidgetButton(self:GetWidget("Button_handbook"),"",function ()
		CallModuleFunc(ModuleType.SPIRIT,"OpenView",BaseViewType.SPIRIT_HANDBOOK)
	end)
	--精灵图鉴文字
	Util:WidgetLabel(self:GetWidget("Text_handbook"),CPPGameLib.GetString("spirit_handbook"), Macros.TypefaceSize.button, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
	
	--场景特效容器
	local panel_sceneeffect = self:GetWidget("Panel_sceneEffect")
	local effect_id,effect = self:__CreateEffect({res_name = "UI_jinglingchangjin",loop = true},self.__panel_spirit,-1)
	effect:SetAnchorPoint(0.5,0.5)
	effect:SetPosition(Macros.Global.DesignSize.width/2,Macros.Global.DesignSize.height/2)

	--灵性精粹
	self.__txt_intensify = self:GetWidget("Text_intensify")
	Util:WidgetLabel(self.__txt_intensify,"", Macros.TypefaceSize.normal)
	--升级需要数量
	self.__panel_intensifynum = self:GetWidget("Panel_IntensifyNum")
	--精灵动作队列
	self.__spirit_action_list = {RenderObj.ActionIndex.BOSS_SKILL1}

	--强化按钮
	self.__button_intensify = self:GetWidget("Button_intensify")
	Util:WidgetButton(self.__button_intensify,CPPGameLib.GetString("spirit_intensify"),function ()
		local role_level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
		--系统开放所需等级
		local is_open,is_pre_open,open_level = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.SPIRIT_ENHANCE)
		if not is_open then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("common_open_lev2", open_level))
			return
		end
		--强化所需等级
		local spirit_info = CPPGameLib.GetConfig("SpiritEnhace",self.__spirit_info_list[self.__selected_index].spirit_id)
		if role_level < spirit_info.enhance_list[self.__selected_enhance_level].need_level then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("spirit_intensify_level_tips",tostring(spirit_info.enhance_list[self.__selected_enhance_level].need_level)))
			return 
		end
		--先去判断是否拥有该精灵
		local spirit_data = self:CallModuleFunc("GetSpiritInfoByID",self.__spirit_info_list[self.__selected_index].spirit_id)
		if not spirit_data then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("spirit_lineup_opensublimation"))
			return 
		end
		local item_num = ObjIDTool.GetOwnNum(spirit_info.enhance_list[self.__selected_enhance_level].cost_item_id_list[1])
		local item_info = CPPGameLib.GetConfig("Item",spirit_info.enhance_list[self.__selected_enhance_level].cost_item_id_list[1])
		if item_num < spirit_info.enhance_list[self.__selected_enhance_level].cost_item_num_list[1] then
			GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, spirit_info.enhance_list[self.__selected_enhance_level].cost_item_id_list[1])
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("spirit_intensify_no_enoughtips",item_info.name))
			return 
		end
		self.__on_sending = true
		self.__panel_shield:SetVisible(true)
		self:CallModuleFunc("SpiritEnhanceReq",self.__spirit_info_list[self.__selected_index].spirit_id)
	end,Macros.TypefaceSize.button,Macros.Color.btn,Macros.Color.button_yellow)
	--精灵升级红点
	local entry_red_dot_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
	self.__btn_intensify_entry_red_dot = SpriteEx.CreateWithImageFile(entry_red_dot_image_data)
	self.__btn_intensify_entry_red_dot:SetAnchorPoint(1, 1)
	self.__button_intensify:AddChild(self.__btn_intensify_entry_red_dot,2)
	PosTool.RightTop(self.__btn_intensify_entry_red_dot)
	self.__btn_intensify_entry_red_dot:SetVisible(false)

	--精灵升星
	self.__btn_intensifystar = self:GetWidget("Button_IntensifyStar")
	Util:WidgetButton(self.__btn_intensifystar,CPPGameLib.GetString("spirit_intensifystar"),function ()
		--先去判断是否拥有该精灵
		local spirit_data = self:CallModuleFunc("GetSpiritInfoByID",self.__spirit_info_list[self.__selected_index].spirit_id)
		if not spirit_data then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("spirit_lineup_opensublimation"))
			return 
		end
		--判断否能升星
		if not self:__JudgeIsCanStarUp() then
			local config_spirit = CPPGameLib.GetConfig("Spirit",self.__spirit_info_list[self.__selected_index].spirit_id)
			GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, config_spirit and config_spirit.star_up_cost_item_id or self.__spirit_info_list[self.__selected_index].spirit_id)
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("spirit_starup_nopiece"))
			return
		end
		self.__on_sending = true
		self.__panel_shield:SetVisible(true)
		self.__is_starup_star = true
		self:CallModuleFunc("SpiritStarUpReq",self.__spirit_info_list[self.__selected_index].spirit_id)
	end,Macros.TypefaceSize.button,Macros.Color.btn,Macros.Color.button_yellow)
	--升星需要数量
	self.__panel_intensifystar_piecenum = self:GetWidget("Panel_IntensifyStarPieceNum")
	--碎片图标
	self.__img_spiritpiece = self:GetWidget("Image_SpiritPiece")
	
	--精灵资质
	self.__txt_quality = self:GetWidget("Text_quality")
	Util:WidgetLabel(self.__txt_quality,"", Macros.TypefaceSize.normal,Macros.Color.btn, 0, 0, Macros.Color.common_stroke)

	--精灵升星星数容器
	self.__panel_awakenstar = self:GetWidget("Panel_AwakenStar")
	
	--屏蔽层
	self.__panel_shield = self:GetWidget("Panel_shield")

	--特效容器
	self.__panel_effect = self:GetWidget("Panel_effect")

	--出战按钮
	self.__button_onfight = self:GetWidget("Button_onFight")
	Util:WidgetButton(self.__button_onfight,CPPGameLib.GetString("spirit_onfight"),function ()
		if self.__is_move_to_view then
			if self.__sp_approach_spirit_id and self.__sp_approach_spirit_id == 50011 then
				GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.FIRST_RECHARGE)
			elseif self.__sp_approach_spirit_id then 				
				GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, self.__sp_approach_spirit_id)
			end			
			return 
		end
		--已祝福精灵不能出战
		local is_bless,formation_pos = CallModuleFunc(ModuleType.SPIRIT,"GetIsSpiritBlessBySpiritId",self.__spirit_info_list[self.__selected_index].spirit_id)
		if is_bless then
			LogicGUI.TemplAffirmTips.New({
				content = CPPGameLib.GetString("spirit_bless_tofight"),
				confirm_text = CPPGameLib.GetString("spirit_bless_suretofight"),
				confirm_func = function ()
					self:CallModuleFunc("SpiritBlessReq",{sprite_id = 0,bless_type = Macros.Game.BLESS_TYPE.CANCEL_BLESS,formation_pos = formation_pos},self.__spirit_info_list[self.__selected_index].spirit_id)
				end,
				cancel_text = CPPGameLib.GetString("spirit_bless_canceltofight"),})
			return
		end
		self.__on_sending = true
		self.__panel_shield:SetVisible(true)
		self:CallModuleFunc("SpiritUseReq",self.__spirit_info_list[self.__selected_index].spirit_id)
	end,Macros.TypefaceSize.button,Macros.Color.btn,Macros.Color.button_red)

	--升华按钮
	self.__btn_sublimate = self:GetWidget("Button_Sublimation")
	local sp = Resource.PathTool.GetSystemEntryIconPath("btn_sublimation")
	self.__btn_sublimate:SetNormalImage(sp,TextureResType.LOCAL)
	Util:WidgetButton(self.__btn_sublimate,"",function ()		
		--先去判断是否拥有该精灵
		local spirit_data = self:CallModuleFunc("GetSpiritInfoByID",self.__spirit_info_list[self.__selected_index].spirit_id)
		if not spirit_data then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("spirit_lineup_opensublimation"))
			return 
		end
		--系统开放所需等级
		local is_open,is_pre_open,open_level = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.SPIRIT_SUBLIMATE)
		if not is_open then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("common_open_lev2", open_level))
			return
		end
		--打开升华界面
		LogicGUI.TemplSpiritSublimationView.New(self.__spirit_info_list[self.__selected_index].spirit_id,self.__skill_id_list[1])
	end)
	--升华按钮文字
	Util:WidgetLabel(self:GetWidget("Text_Sublimation"),CPPGameLib.GetString("spirit_btn_sublimate"), Macros.TypefaceSize.largish, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
	--精灵升华红点
	self.__btn_sublimate_entry_red_dot = SpriteEx.CreateWithImageFile(entry_red_dot_image_data)
	self.__btn_sublimate_entry_red_dot:SetAnchorPoint(1, 1)
	self.__btn_sublimate:AddChild(self.__btn_sublimate_entry_red_dot,2)
	PosTool.RightTop(self.__btn_sublimate_entry_red_dot,0,-10)
	self.__btn_sublimate_entry_red_dot:SetVisible(false)

	--祝福按钮
	self.__btn_bless = self:GetWidget("Button_Bless")
	sp = Resource.PathTool.GetSystemEntryIconPath("btn_bless")
	self.__btn_bless:SetNormalImage(sp,TextureResType.LOCAL)
	Util:WidgetButton(self.__btn_bless,"",function ()
		--系统开放所需等级
		local is_open,is_pre_open,open_level = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.SPIRIT_BLESS)
		if not is_open then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("common_open_lev2", open_level))
			return
		end
		--先去判断是否拥有该精灵
		local spirit_data = self:CallModuleFunc("GetSpiritInfoByID",self.__spirit_info_list[self.__selected_index].spirit_id)
		if not spirit_data then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("spirit_lineup_opensublimation"))
			return 
		end
		--出战中不能祝福
		if self.__spirit_info_list[self.__selected_index].is_onfight then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("spirit_onfight_nobless_tips"))
			return
		end
		--打开祝福界面 TODO	
		GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_SPRITE_BLESS, false, 2, {spirit_id = self.__spirit_info_list[self.__selected_index].spirit_id})
	end)
	--祝福按钮文字
	Util:WidgetLabel(self:GetWidget("Text_Bless"),CPPGameLib.GetString("spirit_btn_bless"), Macros.TypefaceSize.button, Macros.Color.btn, 0, 0, Macros.Color.common_stroke)
	
	--被动技能等级
	self.__text_skilllevel = self:GetWidget("Text_skillLevel")
	self.__text_skilllevel:SetPosition(self.__text_skilllevel:GetPositionX(),self.__text_skilllevel:GetPositionY()+3)
	self.__text_skilllevel:SetZOrder(3)
	Util:WidgetLabel(self.__text_skilllevel,"", Macros.TypefaceSize.slightly, Macros.Color.keypoint)
		
	--初始化滚动容器控件
	self:__InitScrollViewPanel()	

	--数据队列
	self.__sublimate_data = {}
	
	--精灵头像队列
	local panel_spiritlist = self:GetWidget("Panel_spiritList")
	local params = {
		item_class = Modules.TemplSpiritIconCell,
	    item_width = 90,
	    item_height	= 80,
	    row	= 4,
	    col	= 1,
	    item_space_r = 15,
	    view_width = 90,
	    view_height = 485,
	    selectable = true,
	    select_callback	= function (item_index,item_data)	    	
	    	self:__RunSwitchOverAction(item_index)
	    end,
	    select_filter_func = function (index,data)
            if data.approach == CPPGameLib.GetString("spirit_look_forward_to") then
                return false               
            else
                return true
            end
        end,
        select_filter_func_callback = function (index,data)
            if data.approach == CPPGameLib.GetString("spirit_look_forward_to") then
                GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("spirit_look_forward_to"))                
            end
        end,
	}
	self.__spirit_scrollview = GUI.ScrollView.New(params)
	panel_spiritlist:AddChild(self.__spirit_scrollview:GetNode())
	PosTool.Center(self.__spirit_scrollview)

	--精灵排序
	self:ClearUpSpiritData()

	--初始化属性
	self.__windwords_pos_list = {} 
	--原始位置
	self.__windword_org_pos_list = {}

	for i=1,5 do	
		--精灵飘字
		self["__text_windwords"..i] = self:GetWidget("Text_windwords"..i)
		self["__text_windwords"..i]:SetVisible(false)		
		Util:WidgetLabel(self["__text_windwords"..i],"", i >= 5 and Macros.TypefaceSize.tab or Macros.TypefaceSize.popup, i >= 5 and Macros.Color.golden or Macros.Color.green,0,0,i >= 5 and Macros.Color.common_stroke or Macros.Color.green_stroke)
		--飘字属性位置      
		if self["__txt_property"..i] then
			self.__windwords_pos_list[i] = NodeUnity.ConvertPosBetweenTwoContainer(self.Panel_BasicInfo, self.__panel_spirit, COCOPoint(self["__txt_property"..i]:GetPositionX(),self["__txt_property"..i]:GetPositionY()))
		end
		self.__windword_org_pos_list[i] = {pos_x = self["__text_windwords"..i]:GetPositionX(),pos_y = self["__text_windwords"..i]:GetPositionY()}
	end
end

function Modules.SpiritView:ClearUpSpiritData(is_refrash)
	self:__SortSpiritIconList(is_refrash)
	self.__spirit_scrollview:SetDataList(self.__spirit_info_list)
	if self.__selected_index then
		self.__spirit_scrollview:SelectItem(self.__selected_index)	
		self.__spirit_scrollview:LocateItem(self.__selected_index > 2 and (self.__selected_index - 2) or (self.__selected_index > 1 and (self.__selected_index - 1) or self.__selected_index))	
	end
	--新手指引检测
	self:__CheckSectionOperateSP()
end

function Modules.SpiritView:__RegistAllEvents()
	--精灵数据更新
	self:BindGlobalEvent(Macros.Event.SpiritModule.SPIRIT_DATA_RESP, function()
		self.__spirit_data = self:CallModuleFunc("GetSpiritData")
		self:ClearUpSpiritData(true)		
	end)

	--出战回调
	self:BindGlobalEvent(Macros.Event.SpiritModule.SPIRIT_ON_FIGHT_RESP, function(_,spirit_id)		
		self.__on_sending = false
		self.__panel_shield:SetVisible(false)
		--出战标记
		self.__spirit_scrollview:ItemDataIter(function (item_index, item_data,item)
			if not item_data.is_onfight and item_data.spirit_id == spirit_id then
				self.__spirit_info_list[item_index].is_onfight = true
				item_data.is_onfight = true
				if item then
					item:SetOnfightTagVisible(item_data.is_onfight)
					GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("spirit_onfight_succeed",item_data.name))					
				end
			elseif item_data.is_onfight and item_data.spirit_id ~= spirit_id then
				self.__spirit_info_list[item_index].is_onfight = false
				item_data.is_onfight = false
				if item then
					item:SetOnfightTagVisible(item_data.is_onfight)
				end
			end
		end)
		Util:GreyButton( self.__button_onfight )

		--随机一个动作
		local random_index = MathTool.GetRandom(#self.__spirit_action_list,1) 
		self.__spirit_model:SetEndCallback(function ()
			self.__spirit_model:PlayAnimation(RenderObj.ActionIndex.IDLE)
		end)
		self.__spirit_model:PlayAnimation(self.__spirit_action_list[random_index],false)
	end)

	--精灵强化回调
	self:BindGlobalEvent(Macros.Event.SpiritModule.SPIRIT_ENHANCE_RESP, function()	
		local effect_id,effect = self:__CreateEffect({id = 100025,loop = false},self.__panel_effect, 2)
	    effect:SetAnchorPoint(0.5,0.5)
	    PosTool.Center(effect)

		--更新强化等级
		for i,spirit_info in ipairs(self.__spirit_data.spirit_list) do
			if spirit_info.id == self.__spirit_info_list[self.__selected_index].spirit_id then				
				self.__selected_enhance_level = spirit_info.enhance_level
				break
			end
		end

		--飘字
		self:__BounceTextSpecialEffect()
	end)

	--精灵培养回调
	self:BindGlobalEvent(Macros.Event.SpiritModule.SPIRIT_TRAIN_RESP, function(_, cur_spirit_id, is_refrash_star)
		--刷新
		if is_refrash_star then
			--打开升星界面
			Modules.TemplSpiritStarUpSucc.New({spirit_id = self.__spirit_info_list[self.__selected_index].spirit_id})
			--精灵排序
			self:ClearUpSpiritData(true)
		else
			--更新所选精灵信息
			self:__SelectedSpiritIconCallBack(self.__selected_index)
		end		
	end)

	--红点更新事件
	self:BindGlobalEvent(Macros.Event.RedModule.RED_UPDATE, function(_, system_id)
		if system_id == Macros.Game.SystemID.SPIRIT then
			--更新红点
			self:__UpdateSpriteIconRedDot()
		end
	end)	
end

--播放文字特效
function Modules.SpiritView:__BounceTextSpecialEffect()	  
	local windwords_list = {}
	local spirit_info = CPPGameLib.GetConfig("SpiritEnhace",self.__spirit_info_list[self.__selected_index].spirit_id)
	local txt_str
	for i=1,5 do
		local info = {}
		if i == 5 then
			txt_str = CPPGameLib.GetString("spirit_intensify_level",self.__selected_enhance_level)
		else	
			local old_pro_value = (spirit_info.enhance_list[self.__selected_enhance_level - 1] and spirit_info.enhance_list[self.__selected_enhance_level - 1].pro_value_list[i] or 0)	
			local pro_value = spirit_info.enhance_list[self.__selected_enhance_level].pro_value_list[i] - old_pro_value		
			txt_str = Util:Nature(spirit_info.enhance_list[self.__selected_enhance_level].pro_type_list[i], 
								pro_value,nil,false,true)
		end
		self["__text_windwords"..i]:SetText(txt_str)	
		self["__text_windwords"..i]:SetPosition(self.__windword_org_pos_list[i].pos_x,self.__windword_org_pos_list[i].pos_y) 	
		info.node = self["__text_windwords"..i]
		info.anim_type = Macros.Game.WIND_WORDS_TYPE.MOVING_TO_LOCATION		
		info.tgt_pos = {x = self.__windwords_pos_list[i].x,y = self.__windwords_pos_list[i].y} 
		table.insert(windwords_list,info)
	end
	self.__windwords_effect = WindWordsEffects.New()
	self.__windwords_effect:Wind(windwords_list,function ()
		self:__TextScaleToSpecialEffect()
		if self.__windwords_effect then
			self.__windwords_effect:DeleteMe()
			self.__windwords_effect = nil
		end
	end)	
end

--文字滚动效果
function Modules.SpiritView:__TextScaleToSpecialEffect()
	self:__TextDelete()
	--处理精灵属性加成
	local new_spirit_data = CallModuleFunc(ModuleType.SPIRIT, "GetSpiritData")
	local plus_prop_map = ConfigAdapter.Spirit.GetPropMap(new_spirit_data,self.__spirit_info_list[self.__selected_index].spirit_id)
	local spirit_info = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritInfoByID", self.__spirit_info_list[self.__selected_index].spirit_id)
	local enhance_level = spirit_info and spirit_info.enhance_level or 0
	for prop_type=1,5 do
		self["__rollText_"..prop_type] = TypefaceEffects.New()		
		if prop_type == 5 then
			--等级
			self["__rollText_"..prop_type]:RollEx(self["__txt_property"..prop_type],self.__old_enhance_level,enhance_level or 0,0.2,nil,function (cur_value)				
					return CPPGameLib.GetString("common_lev2",cur_value)
				end,function ()
					self:__SelectedSpiritIconCallBack(self.__selected_index)
				end)
		else
			self["__rollText_"..prop_type]:RollEx(self["__txt_property"..prop_type],self.__old_plus_prop_map[prop_type] or 0,plus_prop_map[prop_type] or 0,0.2,nil,function (cur_value)				
					local pro_str = Util:Nature(prop_type,cur_value,nil,true)
					return pro_str			
				end)
		end		
	end
end

--删除
function Modules.SpiritView:__TextDelete()
	for prop_type=1,5 do
		if self["__rollText_"..prop_type] then
			self["__rollText_"..prop_type]:DeleteMe()
			self["__rollText_"..prop_type] = nil
		end
		if self["__text_windwords"..prop_type] then
			CPPActionManager.StopAllActions(self["__text_windwords"..prop_type])
			self["__text_windwords"..prop_type]:SetVisible(false)
		end
	end
	if self.__windwords_effect then
		self.__windwords_effect:__RestoreStatus()
		self.__windwords_effect:DeleteMe()
		self.__windwords_effect = nil
	end
end

--显示技能Tips
function Modules.SpiritView:__ShowSkillTips(skill_id,skill_lev,skill_icon)
	if not self.__skill_tips then
		self.__skill_tips = LogicGUI.TemplSpiritSkillTips.New()
		self.__skill_tips:SetAnchorPoint(1, 0.5)
	end

	self.__skill_tips:Retain()
	self.__skill_tips:RemoveSelf(false)
	self:AddChild(self.__skill_tips:GetNode())
	self.__skill_tips:Release()
	local pos = NodeUnity.ConvertPosBetweenTwoContainer(skill_icon, self.__root_widget, COCOPoint(-8, skill_icon:GetContentHeight() / 2))
	self.__skill_tips:SetPosition(pos.x, pos.y)
	
	self.__skill_tips:SetData(skill_id, skill_lev)
end

--隐藏技能Tips
function Modules.SpiritView:__HideSkillTips()
	if self.__skill_tips then
		self.__skill_tips:DeleteMe()			
		self.__skill_tips = nil
	end
end

--精灵排序
function Modules.SpiritView:__SortSpiritIconList(is_refrash)
	self.__spirit_info_list = {}
	for spirit_id,spirit_info in pairs(CPPGameLib.GetConfig("Spirit", nil, nil , true )) do
		local data = CPPGameLib.CopyTbl(spirit_info)
		data.spirit_id = spirit_id
		if self.__spirit_data.cur_use_spirit ~= 0 and spirit_id == self.__spirit_data.cur_use_spirit then
			data.is_onfight = true
		else
			data.is_onfight = false
		end		
		data.is_lightup = self.__spirit_data.spirit_info_map[spirit_id] and true or false
		data.starup_num = self.__spirit_data.spirit_info_map[spirit_id] and self.__spirit_data.spirit_info_map[spirit_id].star or 0
		data.sublimate_level = data.is_lightup and self.__spirit_data.spirit_info_map[spirit_id].train_stage or 1
		table.insert(self.__spirit_info_list,data)
	end
	table.sort(self.__spirit_info_list,function (spirit_info1,spirit_info2)
		if spirit_info1.is_lightup and not spirit_info2.is_lightup then
			return true
		elseif not spirit_info1.is_lightup and spirit_info2.is_lightup then
			return false
		else
			return spirit_info1.sort_id < spirit_info2.sort_id
		end
	end)
	if not self.__is_move_to_view then
		if self.__open_spirit_id then
			for i,spirit_info in ipairs(self.__spirit_info_list) do
				if spirit_info.spirit_id == self.__open_spirit_id then
					self.__selected_index = i
					break
				end
			end
			self.__open_spirit_id = nil
		elseif is_refrash then
			self.__selected_index = self.__selected_index
		elseif self.__spirit_data.cur_use_spirit ~= 0 then
			for i,spirit_info in ipairs(self.__spirit_info_list) do
				if spirit_info.spirit_id == self.__spirit_data.cur_use_spirit then
					spirit_info.is_onfight = true
					self.__selected_index = i
					break
				end
			end
		else
			self.__selected_index = 1
		end
	else
		for i,spirit_info in ipairs(self.__spirit_info_list) do
			if self.__sp_approach_spirit_id and spirit_info.spirit_id == self.__sp_approach_spirit_id then
				self.__selected_index = i
				break
			end
		end
	end
end

--切换英雄动作
function Modules.SpiritView:__RunSwitchOverAction(item_index)
	self.__panel_shield:SetVisible(true)
	self:__TextDelete()
	--精灵模型
	if not self.__spirit_model then
		self.__spirit_model = LogicGUI.Model.New({info_id = self.__spirit_info_list[item_index].model_id,
							parent = self.__panel_spiritmodule,model_type = LogicGUI.ModelType.SPRITE})		
		self.__spirit_model:SetScaleFactor(2.5)
		self.__spirit_model:SetPosition(5,self.__spirit_info_list[item_index].model_id == 50012 and -30 or 40)
		self:__SelectedSpiritIconCallBack(item_index)
	else
		if self.__is_starup_star then
			self:__SelectedSpiritIconCallBack(self.__selected_index)
			self.__is_starup_star = false
		else
			local effect_time = 0.4
	        CPPActionManager.MoveBy(self.__panel_spiritmodule,effect_time,-350,0)
	        CPPActionManager.FadeOut(self.__panel_spiritmodule,effect_time,GlobalCallbackMgr:AddCallBackFunc(function ()
	            self:__SelectedSpiritIconCallBack(item_index)
	            self.__spirit_model:SetInfoID(self.__spirit_info_list[item_index].model_id)
	            self.__spirit_model:SetPosition(5,self.__spirit_info_list[item_index].model_id == 50012 and -30 or 40)
	            CPPActionManager.MoveBy(self.__panel_spiritmodule,effect_time,350,0)
	            CPPActionManager.FadeIn(self.__panel_spiritmodule,effect_time,GlobalCallbackMgr:AddCallBackFunc(function ()
	            end))
	        end))	
        end	
	end	
end

function Modules.SpiritView:__SelectedSpiritIconCallBack(item_index)	
	--是否跳转去首冲界面
	self.__is_move_to_view = false		
	self.__selected_index = item_index
	self.__selected_enhance_level = 0
	self.__selected_spirit_skill_lv = 1
	--技能信息列表
	self.__skill_id_list = {}
	table.insert(self.__skill_id_list,self.__spirit_info_list[self.__selected_index].auto_skill_id)
	table.insert(self.__skill_id_list,self.__spirit_info_list[self.__selected_index].special_skill_id)
	table.insert(self.__skill_id_list,self.__spirit_info_list[self.__selected_index].common_skill_id)
	for i,spirit_info in ipairs(self.__spirit_data.spirit_list) do
		if spirit_info.id == self.__spirit_info_list[self.__selected_index].spirit_id then
			self.__selected_enhance_level = spirit_info.enhance_level
			local spirit_config = CPPGameLib.GetConfig("SpriteTrain",spirit_info.id) 
			self.__selected_spirit_skill_lv = spirit_config.stage_list[spirit_info.train_stage].skill_level
			break
		end
	end

	--当前的精灵Id
	local cur_spirit_id = self.__spirit_info_list[self.__selected_index].spirit_id
	--精灵名
	self.__text_spiritname:SetText(self.__spirit_data.spirit_info_map[cur_spirit_id] and self.__spirit_info_list[self.__selected_index].name..CPPGameLib.GetString("spirit_sublimate_level_sp",self.__selected_spirit_skill_lv)
									or self.__spirit_info_list[self.__selected_index].name)
	self.__text_spiritname:SetColor(unpack(Macros.Quality[self.__spirit_info_list[self.__selected_index].quality]))
	self.__text_spiritname:SetStrokeColor(unpack(Macros.Quality_Stroke[self.__spirit_info_list[self.__selected_index].quality]))		
	
	--星数
	local speciality_info = CPPGameLib.GetConfig("SpriteSpecialProperty",cur_spirit_id)
	local cur_star = self.__spirit_data.spirit_info_map[cur_spirit_id] and self.__spirit_data.spirit_info_map[cur_spirit_id].star or 0
	if speciality_info then
		local max_star = table.maxn(speciality_info.star_list)	
		local org_pos = -(max_star - 1) * 35/2 + 75
		local org_dis = 35
		for i=1,max_star do
			local star_img = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.SPIRIT,cur_star >= i and "bigstar" or "bigstar_bg")
			if not self["__spirit_star"..i] then
				self["__spirit_star"..i] = Util:Sprite(star_img)
				self["__spirit_star"..i]:SetAnchorPoint(0.5, 0.5)
				self.__panel_awakenstar:AddChild(self["__spirit_star"..i],2)
			else
				self["__spirit_star"..i]:SetImage(star_img)
			end
			self["__spirit_star"..i]:SetVisible(true)
			self["__spirit_star"..i]:SetPosition((i-1)*org_dis+org_pos,15)
		end
		--隐藏其他已在的星
		for i=10,max_star+1,-1 do
			if self["__spirit_star"..i] then
				self["__spirit_star"..i]:SetVisible(false)
			end
		end
	end
	self.__panel_awakenstar:SetVisible(speciality_info and true or false)

	--资质
	self.__txt_quality:SetText(CPPGameLib.GetString("spirit_quality",self.__spirit_info_list[self.__selected_index].qualifications[cur_star+1]))
	
	--强化是否满级
	local spirit_info = CPPGameLib.GetConfig("SpiritEnhace",cur_spirit_id)
	self.__is_enhance_full_level = self.__selected_enhance_level >= table.maxn(spirit_info.enhance_list)

	--出战按钮
	local is_enabled = self.__spirit_info_list[self.__selected_index].is_lightup and 
						not self.__spirit_info_list[self.__selected_index].is_onfight and true or false
	local bt_str = CPPGameLib.GetString("spirit_onfight")
	if not is_enabled then
		if not self:CallModuleFunc("GetSpiritInfoByID",cur_spirit_id) then
			is_enabled = true
			self.__is_move_to_view = true
			self.__sp_approach_spirit_id = cur_spirit_id
			bt_str = CPPGameLib.GetString("spirit_gofor_get")
		end
	end
	self.__button_onfight:SetText(bt_str)
	
	if is_enabled then
		self.__button_onfight:SetEnabled(true)
		local r, g, b, a = unpack(Macros.Color.button_red)
		self.__button_onfight:SetOutline(r, g, b, a, 2)
	else
		Util:GreyButton( self.__button_onfight )
	end	

	--强化
	if not self.__is_enhance_full_level then
		--精灵石
		local item_info = CPPGameLib.GetConfig("Item",spirit_info.enhance_list[self.__selected_enhance_level].cost_item_id_list[1])
		self.__txt_intensify:SetText(item_info.name)
		self.__txt_intensify:SetColor(unpack(Macros.Quality[item_info.quality]))
		self.__txt_intensify:SetStrokeColor(unpack(Macros.Quality_Stroke[item_info.quality]))
		--精灵石数量
		if not self.__rich_text then
			self.__rich_text = Util:RichText()
			self.__rich_text:SetAnchorPoint(0, 0.5)
			self.__panel_intensifynum:AddChild(self.__rich_text)
			PosTool.LeftCenter(self.__rich_text,0,3)
		end
		local item_num = ObjIDTool.GetOwnNum(spirit_info.enhance_list[self.__selected_enhance_level].cost_item_id_list[1])
		local rt_color = item_num >= spirit_info.enhance_list[self.__selected_enhance_level].cost_item_num_list[1] and
							 Macros.Quality_Stroke_RT[Macros.Game.QualityType.GREEN] or Macros.Quality_Stroke_RT[Macros.Game.QualityType.RED]
		local item_num_str = string.format(rt_color, item_num)
		local lab = GUI.RichText.GetColorWord(CPPGameLib.GetString("spirit_num_show",item_num_str,spirit_info.enhance_list[self.__selected_enhance_level].cost_item_num_list[1]),Macros.Color.btn_hex,Macros.Color.common_stroke_hex,2)
		self.__rich_text:SetText(lab)	
	end
	--数量
	self.__txt_intensify:SetVisible(not self.__is_enhance_full_level)
	self.__panel_intensifynum:SetVisible(not self.__is_enhance_full_level)
	--强化按钮
	if self.__is_enhance_full_level then
		Util:GreyButton(self.__button_intensify)
	else
		self.__button_intensify:SetEnabled(true)
		local r, g, b, a = unpack(Macros.Color.button_yellow)
		self.__button_intensify:SetOutline(r, g, b, a, 2)
	end
	--按钮
	self.__button_intensify:SetText(CPPGameLib.GetString(self.__is_enhance_full_level and "spirit_intensify_fulllevel" or "spirit_intensify"))

	local redot_map = self:CallModuleFunc("GetRedDotInfoMap")	

	--升级红点
	self.__btn_intensify_entry_red_dot:SetVisible(redot_map[Modules.RedMgr.SystemID.SPIRIT_ENHANCE] and redot_map[Modules.RedMgr.SystemID.SPIRIT_ENHANCE][cur_spirit_id] and redot_map[Modules.RedMgr.SystemID.SPIRIT_ENHANCE][cur_spirit_id] > 0 or false)
	--升华红点
	local is_sublimate_visible = false
	local temp_list = {Modules.RedMgr.SystemID.SPIRIT_TRAIN, Modules.RedMgr.SystemID.SPIRIT_SUBLIMATE}
	for k, v in ipairs(temp_list) do
		if redot_map[v] and redot_map[v][cur_spirit_id] and redot_map[v][cur_spirit_id] > 0 then
			is_sublimate_visible = true
			break
		end
	end
	self.__btn_sublimate_entry_red_dot:SetVisible(is_sublimate_visible)
	
	self.__is_starup_full_level = cur_star >= self.__spirit_info_list[self.__selected_index].star_max
	--升星
	if not self.__is_starup_full_level then
		--精灵碎片数量
		if not self.__spirit_piece_rich_txt then
			self.__spirit_piece_rich_txt = Util:RichText()
			self.__spirit_piece_rich_txt:SetAnchorPoint(0, 0.5)
			self.__panel_intensifystar_piecenum:AddChild(self.__spirit_piece_rich_txt)
			PosTool.LeftCenter(self.__spirit_piece_rich_txt,0,3)
		end
		local starup_info = CPPGameLib.GetConfig("SpriteStarUp",cur_star)
		local item_num = ObjIDTool.GetOwnNum(self.__spirit_info_list[self.__selected_index].star_up_cost_item_id)
		local rt_color = item_num >= starup_info.star_up_cost_item_num and
							 Macros.Quality_Stroke_RT[Macros.Game.QualityType.GREEN] or Macros.Quality_Stroke_RT[Macros.Game.QualityType.RED]
		local item_num_str = string.format(rt_color, item_num)
		local lab = GUI.RichText.GetColorWord(CPPGameLib.GetString("spirit_num_show",item_num_str,starup_info.star_up_cost_item_num),Macros.Color.btn_hex,Macros.Color.common_stroke_hex,2)
		self.__spirit_piece_rich_txt:SetText(lab)
	end
	--数量
	self.__img_spiritpiece:SetVisible(not self.__is_starup_full_level)
	self.__panel_intensifystar_piecenum:SetVisible(not self.__is_starup_full_level)
	--升星按钮
	if self.__is_starup_full_level then
		Util:GreyButton(self.__btn_intensifystar)
	else
		self.__btn_intensifystar:SetEnabled(true)
		local r, g, b, a = unpack(Macros.Color.button_yellow)
		self.__btn_intensifystar:SetOutline(r, g, b, a, 2)
	end
	self.__btn_intensifystar:SetText(CPPGameLib.GetString(self.__is_starup_full_level and "spirit_intensify_fullstar" or "spirit_intensifystar"))

	self.__on_sending = false
	self.__panel_shield:SetVisible(false)
	--重新设置精灵的属性
	self:__Reset()
end

--判断是否能升星
function Modules.SpiritView:__JudgeIsCanStarUp()
	--当前的精灵Id
	local cur_spirit_id = self.__spirit_info_list[self.__selected_index].spirit_id
	local cur_star = self.__spirit_data.spirit_info_map[cur_spirit_id] and self.__spirit_data.spirit_info_map[cur_spirit_id].star or 0
	local starup_info = CPPGameLib.GetConfig("SpriteStarUp",cur_star)
	local item_num = ObjIDTool.GetOwnNum(self.__spirit_info_list[self.__selected_index].star_up_cost_item_id)
	return item_num >= starup_info.star_up_cost_item_num 
end

--初始化滚动容器控件
function Modules.SpiritView:__InitScrollViewPanel()	 
	--滚动容器
	self.__scv = self:GetWidget("ScrollView_Detail")
	--初始化控件
	self.__panel_name_list = {"Panel_BasicInfo", "Panel_SkillInfo", "Panel_TalentInfo", "Panel_SpecialityInfo","Panel_BlessOfSkillInfo"}
	for _, panel_name in ipairs(self.__panel_name_list) do
		self[panel_name] = self:GetWidget(panel_name)
	end
	--基础属性
	Util:WidgetLabel(self:GetWidget("Text_BasicTitle"), CPPGameLib.GetString("hero_basic_info_title"), Macros.TypefaceSize.popup, Macros.Color.btn, nil, nil, nil, Macros.Color.shadow_color)
	--精灵技能
	Util:WidgetLabel(self:GetWidget("Text_SkillTitle"), CPPGameLib.GetString("spirit_spiritskill"), Macros.TypefaceSize.popup, Macros.Color.btn, nil, nil, nil, Macros.Color.shadow_color)
	--精灵天赋
	Util:WidgetLabel(self:GetWidget("Text_TalentTitle"), CPPGameLib.GetString("spirit_talent_title"), Macros.TypefaceSize.popup, Macros.Color.btn, nil, nil, nil, Macros.Color.shadow_color)
	--精灵特性
	Util:WidgetLabel(self:GetWidget("Text_SpecialityTitle"), CPPGameLib.GetString("spirit_speciality_title"), Macros.TypefaceSize.popup, Macros.Color.btn, nil, nil, nil, Macros.Color.shadow_color)
	--祝福技能
	Util:WidgetLabel(self:GetWidget("Text_BlessingOfSkillTitle"), CPPGameLib.GetString("spirit_blessingofskill_title"), Macros.TypefaceSize.popup, Macros.Color.btn, nil, nil, nil, Macros.Color.shadow_color)
end

--刷新
function Modules.SpiritView:__Reset()
	self:__ShowBasicInfo()
	self:__ShowSkillInfo()
	self:__ShowTalentInfo()
	self:__ShowSpecialityInfo()
	self:__ShowBlessOfSkillInfo()

	--重置滚动区域
	local height = 0
	local panel
	for _, panel_name in ipairs(self.__panel_name_list) do
		panel = self[panel_name]
		PosTool.CenterTop(panel, 0, -height)
		height = height + panel:GetContentHeight()
	end
	self.__scv:SetInnerContainerSize(0, height)
	self.__scv:RefreshLayoutComponent()
	self.__scv:ScrollToTop()
end

--基础属性
function Modules.SpiritView:__ShowBasicInfo()
	--处理精灵属性加成
	local spirit_data = CallModuleFunc(ModuleType.SPIRIT, "GetSpiritData")
	self.__old_plus_prop_map = ConfigAdapter.Spirit.GetPropMap(spirit_data,self.__spirit_info_list[self.__selected_index].spirit_id)
	--属性
	for pro_type, pro_value in ipairs(self.__old_plus_prop_map) do
		local pro_str = Util:Nature(pro_type,math.floor(pro_value),nil,true)
		if not self["__txt_property"..pro_type] and self:GetWidget("Text_property"..pro_type) then
			self["__txt_property"..pro_type] = self:GetWidget("Text_property"..pro_type)
			Util:WidgetLabel(self["__txt_property"..pro_type],"", Macros.TypefaceSize.normal, Macros.Color.btn)
		end
		self["__txt_property"..pro_type]:SetText(pro_str or "")
	end
	--
	local spirit_info = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritInfoByID", self.__spirit_info_list[self.__selected_index].spirit_id)
	self.__old_enhance_level = spirit_info and spirit_info.enhance_level or 0
	--等级
	if not self.__txt_property5 then
		self.__txt_property5 = self:GetWidget("Text_property5")
		Util:WidgetLabel(self.__txt_property5,"", Macros.TypefaceSize.normal, Macros.Color.btn)
	end
	self.__txt_property5:SetText(CPPGameLib.GetString("common_lev2",self.__old_enhance_level))
	--转换率
	local config_train = CPPGameLib.GetConfig("SpriteTrain", self.__spirit_info_list[self.__selected_index].spirit_id)	
	local pro_conversion_rate = spirit_info and MathTool.TransProbCoeffPercentage(config_train.stage_list[spirit_info.train_stage].pro_conversion_rate) or 0
	if not self.__txt_translatetips then
		self.__txt_translatetips = self:GetWidget("Text_TranslateTips")
		Util:WidgetLabel(self.__txt_translatetips,"", Macros.TypefaceSize.minimum, Macros.Color.Special)
	end
	self.__txt_translatetips:SetText(CPPGameLib.GetString("spirit_transformtips_txt",pro_conversion_rate))
end

--精灵技能
function Modules.SpiritView:__ShowSkillInfo()
	if not self.__init_skillinfo then
		--技能
		for i=1,3 do
			self["__image_skillicon"..i] = self:GetWidget("Image_skillIcon"..i)
			WidgetUnity.SetWidgetListener(self["__image_skillicon"..i], 
			function()
				local skill_lev = self.__selected_spirit_skill_lv
				self:__ShowSkillTips(self.__skill_id_list[i], skill_lev, self["__image_skillicon"..i])
			end, nil,
			function()
				self:__HideSkillTips()
			end,
			function()
				self:__HideSkillTips()
			end)
		end
		--技能被动文字层级
		self:GetWidget("Image_passivityTxt"):SetZOrder(2) 
		--技能通用文字层级
		self:GetWidget("Image_commonTxt"):SetZOrder(2) 
		--被动技能等级背景
		self:GetWidget("Image_skillLevelBg"):SetZOrder(2) 
		--技能专属文字层级
		self:GetWidget("Image_specialTxt"):SetZOrder(2)
		self.__init_skillinfo = true
	end
	--技能图标
	for i=1,3 do
		local path = Resource.PathTool.GetSpiritSkillIconPath(self.__skill_id_list[i])
		if not self["__spirit_icon"..i] then						
			self["__spirit_icon"..i] = Util:Sprite(path)
			self["__image_skillicon"..i]:AddChild(self["__spirit_icon"..i],0)
			PosTool.Center(self["__spirit_icon"..i])
		else
			self["__spirit_icon"..i]:SetImage(path)
		end	
	end	
	--技能等级
	self.__text_skilllevel:SetText(CPPGameLib.GetString("spirit_skill_level",self.__selected_spirit_skill_lv or 1))	
end

--精灵天赋
function Modules.SpiritView:__ShowTalentInfo()
	--天赋技能名
	if not self.__txt_talentname then
		self.__txt_talentname = self:GetWidget("Text_TalentName")
		Util:WidgetLabel(self.__txt_talentname,"", Macros.TypefaceSize.largish, Macros.Color.Special)
	end
	local spirit_info = self:CallModuleFunc("GetSpiritInfoByID", self.__spirit_info_list[self.__selected_index].spirit_id)
	local train_info = ConfigAdapter.Spirit.GetTrainInfo(self.__spirit_info_list[self.__selected_index].spirit_id, spirit_info and spirit_info.train_stage or 1)
	local talent_info = ConfigAdapter.Spirit.GetTalentInfo(self.__spirit_info_list[self.__selected_index].spirit_id, train_info and train_info.talent_level or 1)
	self.__txt_talentname:SetText((talent_info and talent_info.name or "")..CPPGameLib.GetString("common_lev11",talent_info and talent_info.level or 1))
	--天赋技能描述
	if not self.__txt_talentdes then
		self.__txt_talentdes = Util:RichText("", Macros.TypefaceSize.normal,350,0)
		self.__txt_talentdes:SetAnchorPoint(0,1)
		self.Panel_TalentInfo:AddChild(self.__txt_talentdes)
	end
	local lab = GUI.RichText.GetColorWord("     "..(talent_info and talent_info.desc or ""), Macros.Color.btn_hex)
	self.__txt_talentdes:SetText(lab)
	self.Panel_TalentInfo:SetContentHeight(self.__txt_talentdes:GetContentHeight()+80)
	PosTool.LeftTop(self.__txt_talentname,16.5,-44.5)
	PosTool.LeftTop(self.__txt_talentdes,19,-74)
end

--精灵特性
function Modules.SpiritView:__ShowSpecialityInfo()
	if not self.__panel_speciality then
		self.__panel_speciality = self:GetWidget("Panel_Speciality")
	end
	local cur_spirit_id = self.__spirit_info_list[self.__selected_index].spirit_id
	local config_speciality = CPPGameLib.GetConfig("SpriteSpecialProperty", cur_spirit_id)
	if not config_speciality then
		self.__panel_speciality:RemoveAllChild(true)
		self.__lab_speciality_list = nil
		return
	end
	self.__lab_speciality_list = self.__lab_speciality_list or {}

	local y = -0.5
	local row_space = 12
	local level_index
	local cur_star = self.__spirit_data.spirit_info_map[cur_spirit_id] and self.__spirit_data.spirit_info_map[cur_spirit_id].star or 0
	for i, speciality_info in ipairs(config_speciality.star_list) do
		level_index = i
		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, cur_star >= i and "sp_bright_star" or "sp_sky_star")
		if not self.__lab_speciality_list[i] then
			--星
			self.__lab_speciality_list[i] = {}
			self.__lab_speciality_list[i].star = Util:Sprite(sp)
			self.__lab_speciality_list[i].star:SetAnchorPoint(0.5, 1)
			self.__panel_speciality:AddChild(self.__lab_speciality_list[i].star)			
			--描述
			self.__lab_speciality_list[i].rt = Util:RichText("", Macros.TypefaceSize.normal,320,0)
			self.__lab_speciality_list[i].rt:SetAnchorPoint(0, 1)
			self.__panel_speciality:AddChild(self.__lab_speciality_list[i].rt)			
		end
		PosTool.LeftTop(self.__lab_speciality_list[i].star, 30.5, y)
		PosTool.LeftTop(self.__lab_speciality_list[i].rt, 53.5, y - 3)
		self.__lab_speciality_list[i].star:SetImage(sp)

		local lab = GUI.RichText.GetColorWord(speciality_info and speciality_info.speciality_des or "", Macros.Color.btn_hex)
		self.__lab_speciality_list[i].rt:SetText(lab)	

		y = y - self.__lab_speciality_list[i].rt:GetContentHeight() - row_space		
	end

	for i = #self.__lab_speciality_list, level_index + 1, -1 do
		self.__lab_speciality_list[i].star:RemoveSelf(true)
		self.__lab_speciality_list[i].rt:RemoveSelf(true)
		table.remove(self.__lab_speciality_list, i)
	end

	self.Panel_SpecialityInfo:SetContentHeight(43 - y)
	PosTool.CenterTop(self.__panel_speciality,0,-43)
end

--祝福技能
function Modules.SpiritView:__ShowBlessOfSkillInfo()
	if not self.__init_blessofskill_info then
		--技能图标容器
		self.__panel_skillicon = self:GetWidget("Panel_SkillIcon")
		--祝福技能名字
		self.__txt_blessofskillname = self:GetWidget("Text_BlessOfSkillName")
		Util:WidgetLabel(self.__txt_blessofskillname,"", Macros.TypefaceSize.largish, Macros.Color.Special)
		--祝福技能描述
		self.__txt_blessofskilldes = Util:RichText("", Macros.TypefaceSize.normal,265,0)
		self.__txt_blessofskilldes:SetAnchorPoint(0, 1)
		self.Panel_BlessOfSkillInfo:AddChild(self.__txt_blessofskilldes)
		PosTool.LeftTop(self.__txt_blessofskilldes, 109.50, 72.5)
		self.__init_blessofskill_info = true
	end
	local cur_spirit_id = self.__spirit_info_list[self.__selected_index].spirit_id
	--精灵配置
	local config_spirit = CPPGameLib.GetConfig("Spirit",cur_spirit_id)
	--特性配置
	local config_speciality = CPPGameLib.GetConfig("SpriteSpecialProperty",cur_spirit_id)
	--祝福技能名字
	self.__txt_blessofskillname:SetText(config_speciality.star_list and config_speciality.star_list[1].bless_skill_name or "")
	--当前星级
	local cur_star = self.__spirit_data.spirit_info_map[cur_spirit_id] and self.__spirit_data.spirit_info_map[cur_spirit_id].star or 0
	--祝福技能描述
	local lab = GUI.RichText.GetColorWord(config_speciality.star_list and config_speciality.star_list[cur_star].bless_skill_des or "", Macros.Color.btn_hex)
	self.__txt_blessofskilldes:SetText(lab)
	--祝福技能图标
	local path = Resource.PathTool.GetSpiritSkillIconPath(config_spirit.bless_skill_icon)
	if not self.__bless_skill_icon then						
		self.__bless_skill_icon = Util:Sprite(path)
		self.__panel_skillicon:AddChild(self.__bless_skill_icon,2)
		PosTool.Center(self.__bless_skill_icon)
	else
		self.__bless_skill_icon:SetImage(path)
	end	
	--大小
	local height = self.__txt_blessofskilldes:GetContentHeight() >= 50 and self.__txt_blessofskilldes:GetContentHeight() or 50
	self.Panel_BlessOfSkillInfo:SetContentHeight(72.5 + height + 5)
	PosTool.LeftTop(self.__txt_blessofskilldes,109.5,-72.5)
	--self.Panel_BlessOfSkillInfo:SetVisible(false)
end

function Modules.SpiritView:__Dispose()
	if self.__spirit_scrollview then
		self.__spirit_scrollview:DeleteMe()
		self.__spirit_scrollview = nil
	end
	self:__TextDelete()
end

--新手指引特殊操作检测()
function Modules.SpiritView:__CheckSectionOperateSP()
	local operate_id, data_id = CallModuleFunc(ModuleType.LEAD, "GetOperateInfo")
	if Macros.Game.LeadOperateSpecialID.SPIRIT_CHOOSE_SPIRIT and operate_id == Macros.Game.LeadOperateSpecialID.SPIRIT_CHOOSE_SPIRIT then
		local locate_index
		for i, lineup_info in ipairs(self.__spirit_info_list) do
			if lineup_info.spirit_id == data_id then
				locate_index = i
				break
			end
		end
		if locate_index then
			self.__spirit_scrollview:LocateItem(locate_index, true)
			local item = self.__spirit_scrollview:GetItem(locate_index)
			if item then
				item:__CheckSectionOperateSP()
			end
		end
	elseif Macros.Game.LeadOperateSpecialID.SPIRIT_SKIP_SPIRIT and operate_id == Macros.Game.LeadOperateSpecialID.SPIRIT_SKIP_SPIRIT then

		local locate_index
		local section_info = CallModuleFunc(ModuleType.LEAD, "GetSectionInfo")
		if section_info then
			for i, lineup_info in ipairs(self.__spirit_info_list) do
				if lineup_info.spirit_id == section_info.operate_params[2] then
					locate_index = i
					break
				end
			end
		end
		if locate_index then
			self.__spirit_scrollview:SelectItem(locate_index)
		end
	end
end

--更新精灵图标红点
function Modules.SpiritView:__UpdateSpriteIconRedDot()
	self.__spirit_scrollview:ItemIter(function(index, item, item_data)
		item:JudgeRedDotAdd()
	end)
end

----------------------------------------------------------------------------------------------------------------------
--精灵队列精灵球
----------------------------------------------------------------------------------------------------------------------
Modules = Modules or {}

Modules.TemplSpiritIconCell = Modules.TemplSpiritIconCell or BaseClass(GUI.ScrollItem)

function Modules.TemplSpiritIconCell:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplSpiritIconCell:__CreateWidgetFromLuaCode()
  self.__layout = Util:Layout(90,80)
  self.__layout:SetClippingEnable(false)
  self.__layout:SetBackGroundColor(0, 0, 0)
  return self.__layout
end

function Modules.TemplSpiritIconCell:SetData(data)
	GUI.ScrollItem.SetData(self,data)

	--选中框
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_selected")
	self.__image_selectedframe = Util:Sprite(sp)
	self.__layout:AddChild(self.__image_selectedframe,2)
	self.__image_selectedframe:SetVisible(false)
	PosTool.Center(self.__image_selectedframe)

	--出战
	sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.SPIRIT, "spirit_on_fighting",true)
	self.__image_onfight = Util:Sprite(sp)
	self.__image_onfight:SetAnchorPoint(0,1)
	self.__layout:AddChild(self.__image_onfight,3)
	PosTool.LeftTop(self.__image_onfight,5,1)
	self.__image_onfight:SetVisible(self.__data.is_onfight)

	--图标
	self.__spirit_icon = LogicGUI.IconLayer.New(false,false)
	self.__layout:AddChild(self.__spirit_icon:GetNode())
	PosTool.Center(self.__spirit_icon)
	self.__spirit_icon:SetData({id = self.__data.spirit_id,obj_info = self.__data})
	--星数
	if self.__data.starup_num > 0 then
		self.__spirit_icon:ShowSpiritAwakenStar()
	end
	self:JudgeRedDotAdd()
end

function Modules.TemplSpiritIconCell:SetSelected(var)
	self.__image_selectedframe:SetVisible(var)
end

function Modules.TemplSpiritIconCell:SetOnfightTagVisible(var)
	self.__image_onfight:SetVisible(var)
end

function Modules.TemplSpiritIconCell:JudgeRedDotAdd()
	--计算红点数
	local spirit_data, redot_map = CallModuleFunc(ModuleType.SPIRIT,"GetSpiritData")
	local red_dot_num = 0
	for k, sub_reddot_list in pairs(redot_map) do
		if sub_reddot_list[self.__data.icon_id] then
			red_dot_num = red_dot_num + sub_reddot_list[self.__data.icon_id]
			if red_dot_num > 0 then
				break
			end
		end
	end

	--判断是否添加红点
	if red_dot_num > 0 then
		--红点
		if not self.entry_red_dot then
			local entry_red_dot_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
			self.entry_red_dot = SpriteEx.CreateWithImageFile(entry_red_dot_image_data)
			self.entry_red_dot:SetAnchorPoint(1, 1)
			self:GetNode():AddChild(self.entry_red_dot,2)
			PosTool.RightTop(self.entry_red_dot)	
		end
		self.entry_red_dot:SetVisible(true)
	else
		if self.entry_red_dot then
			self.entry_red_dot:SetVisible(false)
		end
	end
end

function Modules.TemplSpiritIconCell:__CheckSectionOperateSP()
	CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.SPIRIT_CHOOSE_SPIRIT, self:GetNode(), self.__data.spirit_id)
end