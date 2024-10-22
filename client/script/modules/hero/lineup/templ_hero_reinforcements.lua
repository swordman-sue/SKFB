
--[[
%% @module：英雄援军信息模板
%% @author: ldx
%% @created: 2016-10-26
注：需手动释放(DeleteMe)
--]]

LogicGUI = LogicGUI or {}

LogicGUI.TemplHeroReinforcements = LogicGUI.TemplHeroReinforcements or BaseClass(GUI.Template)
local ReinforcementState = {
	EMPTY = 1,
	FULL = 2,
}

function LogicGUI.TemplHeroReinforcements:__init(parent_node)
	self.__is_auto_load = true
	self.__open_callback = function ()
		self:__InitUI()	
	end
	self:__AddPreloadTemplate(GUI.TemplateRegistry.HERO_REINFORCEMENTS_INFO)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.HERO_REINFORCEMENTS_CELL)
	self:InitTemplate(GUI.TemplateRegistry.HERO_REINFORCEMENTS, parent_node, true)	
end

function LogicGUI.TemplHeroReinforcements:__InitUI()
	--队列更新事件
	self:BindGlobalEvent(Macros.Event.HeroModule.HERO_LINEUP_UPDATE, function()
        self:ChangeItem()
    end)
    --等级升级事件
	self:BindGlobalEvent(Macros.Event.HeroModule.REINFORCEMENT_LINEUP_POS_OPENED, function()
			self.__hero_reinforcement_list = CallModuleFunc(ModuleType.HERO,"GetHeroReinforcementList")
			if self.__hero_reinforcement_list then
				self:lockReinforcement()
			end	
	    end)

	--上阵界面更新
	self:BindGlobalEvent(Macros.Event.HeroModule.HERO_REINFORCEMENTS_ON_BATTLE, function(_ , pos,uid)
	        local info = CallModuleFunc(ModuleType.HERO,"GetHeroInfo",uid)	
	        if self["HeroHelpIcon_"..pos]:GetChildByTag(1) then
	        	self["HeroHelpIcon_"..pos]:GetChildByTag(1):RemoveSelf(true)
	        end      	        
	        local icon = LogicGUI.IconLayer.New(false,false)
	        self["HeroHelpIcon_"..pos]:AddChild(icon:GetNode())
	        icon:GetNode():SetTag(1)
	        PosTool.Center(icon)
	        icon:SetData({id = info.info_id,obj_info = info})
	        icon:ShowHeroName(true,0,0)
			self["HeroHelperId_"..pos] = uid
			self["HeroHelpIconState_"..pos] = ReinforcementState.FULL
			self:ChangeItem()			
	    end)

	--卸下英雄事件
	self:BindGlobalEvent(Macros.Event.HeroModule.HERO_REINFORCEMENTS_OFF_BATTLE, function(_,pos)
			local icon = self["HeroHelpIcon_"..pos]:GetChildByTag(1)
			if icon then
				icon:RemoveSelf(true)
			end
			self["HeroHelperId_"..pos] = 0
			self["HeroHelpIconState_"..pos] = ReinforcementState.EMPTY
			self["Add_"..pos]:SetVisible(true)
			self:RunAddAction(pos)
			self:ChangeItem()
	    end)

	--红点事件
	self:BindGlobalEvent(Macros.Event.RedModule.RED_UPDATE, function(_, system_id, sub_system_id)
			if sub_system_id == Macros.Game.SystemID.REINFORCEMENT then
				for i, linup_info in ipairs(self.__hero_reinforcement_list) do
					local red_dot_num = CallModuleFunc(ModuleType.HERO, "GetReinforcementRedDotNumByLineUpIndex", i)
					if red_dot_num > 0 then
						self:__AddRedDot(i)
					else
						self:__HideRedDot(i)
					end
				end
			end
	    end)
	--英雄进化事件
	self:BindGlobalEvent(Macros.Event.HeroModule.HERO_EVOLUTION_NOTICE, function(_,evolution_info,before_heroevolution_req_quality)
        --品质升了
        if evolution_info.quality > before_heroevolution_req_quality then
	        self:ChangeItem()
	    end
    end)
	--宿命效果标题	
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("reinforcements_EffectTitle"), Macros.TypefaceSize.popup, Macros.Color.headline, 0, 0, nil, Macros.Color.shadow_color)
	--宿命效果显示
	self.ListView_lineUpHeroReinforcements = self:GetWidget("ListView_lineUpHeroReinforcements")
	--获取等级限制
	local reinforcementPosInfo = CPPGameLib.GetConfig("HeroLineup",Macros.Game.HeroLineup.REINFORCEMENT)
	self.PosLevelLimit = {}
	for i,v in ipairs(reinforcementPosInfo.pos_list) do
		table.insert(self.PosLevelLimit,v.pos,v.need_level)
	end
	--援军框初始化
	local roleLevel = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	for i=1,8 do
		local function touchReinforcementKuangCallBack()
			if self["HeroHelpIconState_"..i] == ReinforcementState.EMPTY then				
				if roleLevel < self.PosLevelLimit[i] then
					GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("reinforcements_lockLevelLimitTip",self.PosLevelLimit[i]))
					return
				end
				self:FireNextFrame(Macros.Event.HeroModule.HERO_REINFORCEMENTS_ONLINE,i)
			else
				local hero_info = CallModuleFunc(ModuleType.HERO, "GetHeroInfo", self["HeroHelperId_"..i])
				if hero_info then
					CallModuleFunc(ModuleType.HERO, "OpenView", BaseViewType.HERO_PROMOTE, nil,self["HeroHelperId_"..i],true,i)				
				else
					CallModuleFunc(ModuleType.HERO, "RecinforcementToBattleReq", 0, i)			
				end
			end
		end
		self["HeroHelpIcon_"..i] = self:GetWidget("Image_kuang"..i)
		WidgetUnity.SetWidgetListener(self["HeroHelpIcon_"..i],nil, nil,touchReinforcementKuangCallBack ,nil)
		self["Add_"..i] = WidgetUnity.GetWidgetByName(self["HeroHelpIcon_"..i],"Image_add")
		--加号动作
		self["funId_"..i] = GlobalCallbackMgr:AddCallBackFunc(function ()
			CPPActionManager.FadeIn(self["Add_"..i], 0.5, GlobalCallbackMgr:AddCallBackFunc(function()
					self:RunAddAction(i)
			end))	
		end)
		self:RunAddAction(i)
		self["Image_lock_"..i] = WidgetUnity.GetWidgetByName(self["HeroHelpIcon_"..i],"Image_lock")
		local textLock = WidgetUnity.GetWidgetByName(self["Image_lock_"..i],"Text_lockLevel")	
		Util:WidgetLabel(textLock, CPPGameLib.GetString("dungeon_assist_open_level",self.PosLevelLimit[i]), Macros.TypefaceSize.normal, Macros.Color.btn, 60, 60,Macros.Color.button_yellow)

		--阵容-援军-阵位（阵位索引，从1开始）
		CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.LEINUP_REINFORCEMENTS_POS, self["HeroHelpIcon_"..i], i)								
	end		
	--获取援军队列
	self.__hero_reinforcement_list = CallModuleFunc(ModuleType.HERO,"GetHeroReinforcementList")
	CPPGameLib.PrintTable("ldx",self.__hero_reinforcement_list,"self.__hero_reinforcement_list")
	if self.__hero_reinforcement_list then
		self:lockReinforcement()
	end	

	--规则按钮
	local Button_rule = Util:WidgetButton( self:GetWidget("Button_rule") , nil , function ( ... )
		print("xy" , "(6)援军，小图标（用于介绍援军）：" )
        local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
        help:SetLoadedCallback( function ( ... )
           help:SetCell("Rule_Reinforcement")
        end )
	end )
	Button_rule:SetZOrder(100)

	--援军助威
	local is_open,is_pre_open = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.REINFORCEMENTS_CHEER)
	self.__btn_cheer = self:GetWidget("Button_Cheer")
	Util:WidgetButton(self.__btn_cheer,"",function ()
		--预告
		local is_open,is_pre_open,open_level = GlobalModulesMgr:IsOpen(Macros.Game.SystemID.REINFORCEMENTS_CHEER)
	    if is_pre_open and not is_open then
	    	GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("common_open_lev2", open_level))
	    	return
	    end
        --打开援军助威界面
        GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.REINFORCEMENTS_CHEER)
    end)
	self.__btn_cheer:SetBright(is_open)
	self.__btn_cheer:SetVisible(is_open or is_pre_open)
	--文字
	Util:WidgetLabel(self:GetWidget("Text_Cheer"),CPPGameLib.GetString("reinforcements_cheerlevel_title"),Macros.TypefaceSize.slightly,Macros.Color.btn,0,0,Macros.Color.button_red)
	
	self:ChangeItem()
end

--加号渐隐效果
function LogicGUI.TemplHeroReinforcements:RunAddAction(number)
	CPPActionManager.FadeOut(self["Add_"..number],0.5,self["funId_"..number])
end

--更新解锁的援军阵位
function LogicGUI.TemplHeroReinforcements:lockReinforcement()
	local roleLevel = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
	for i, linup_info in ipairs(self.__hero_reinforcement_list) do
	 	if self["HeroHelpIcon_"..i] and self["HeroHelpIcon_"..i]:GetChildByTag(1) then
        	self["HeroHelpIcon_"..i]:GetChildByTag(1):RemoveSelf(true)
        end 
        CPPActionManager.StopAllActions(self["Add_"..i])
		if linup_info.uid == 0 then
			self["HeroHelperId_"..i] = 0
			self["HeroHelpIconState_"..i] = ReinforcementState.EMPTY
			if roleLevel >= self.PosLevelLimit[i] then				
				self["Image_lock_"..i]:SetVisible(false)
				self["Add_"..i]:SetVisible(true)
				self:RunAddAction(i)
				local red_dot_num = CallModuleFunc(ModuleType.HERO,"GetReinforcementRedDotNumByLineUpIndex",i)
				if red_dot_num > 0 then
					self:__AddRedDot(i)
				else
					self:__HideRedDot(i)
				end
			else
				self["Add_"..i]:SetVisible(false)								
			end			
		else
			self["HeroHelpIconState_"..i] = ReinforcementState.FULL
			self["HeroHelperId_"..i] = linup_info.uid
			self["Image_lock_"..i]:SetVisible(false)
			self["Add_"..i]:SetVisible(false)			
			local info = CallModuleFunc(ModuleType.HERO,"GetHeroInfo", linup_info.uid)
			if info then
		        local icon = LogicGUI.IconLayer.New(false,false)
		        self["HeroHelpIcon_"..i]:AddChild(icon:GetNode())
		        PosTool.Center(icon)
		        icon:GetNode():SetTag(1)
		        icon:SetData({id = info.info_id,obj_info = info})
		        icon:ShowHeroName(true,0,0)
		    end
		end
	end
end

function LogicGUI.TemplHeroReinforcements:ChangeItem()
	if not self:IsOpen() then
		self.__need_change_item = true
		return
	end

	--先清除所有的Item
	self.ListView_lineUpHeroReinforcements:RemoveAllItems()
	--获取阵位列表英雄	
	local OnFightHeroList = CallModuleFunc(ModuleType.HERO,"GetAllLineupHeroList")	
	--获取英雄上阵队列
	local lineupList = CallModuleFunc(ModuleType.HERO,"GetLineupList")
	--重新创建新的宿命文字
	for i=1,5 do
		if lineupList[i].uid ~= 0 then
			local table = CallModuleFunc(ModuleType.HERO,"GetLineupEquipInfo",i)
			--对应阵位卡牌的穿戴情况
			local divination_type_list = CallModuleFunc(ModuleType.HERO,"GetLineupDivinationInfo",i)
			local TemplReinforcements = GUI.TemplReinforcementsCell.New(lineupList[i].uid,OnFightHeroList,table,divination_type_list)
			self.ListView_lineUpHeroReinforcements:PushBackCustomItem(TemplReinforcements:GetNode())
		end
	end		
end

function LogicGUI.TemplHeroReinforcements:OpenCallbackEx()
	if self.__need_change_item then
		self:ChangeItem()
		self.__need_change_item = false
	end
end

function LogicGUI.TemplHeroReinforcements:__AddRedDot(index)
	--红点
	if not self["entry_red_dot"..index] then
		local entry_red_dot_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
		self["entry_red_dot"..index] = SpriteEx.CreateWithImageFile(entry_red_dot_image_data)
		self["entry_red_dot"..index]:SetAnchorPoint(1, 1)
		self["HeroHelpIcon_"..index]:AddChild(self["entry_red_dot"..index],2)
		PosTool.RightTop(self["entry_red_dot"..index])
	end
	self["entry_red_dot"..index]:SetVisible(true)	
end

--隐藏红点
function LogicGUI.TemplHeroReinforcements:__HideRedDot(index)
	if self["entry_red_dot"..index] then
		self["entry_red_dot"..index]:SetVisible(false)
	end	
end

--界面销毁
function LogicGUI.TemplHeroReinforcements:__delete()
end