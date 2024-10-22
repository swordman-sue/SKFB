Modules = Modules or {}

Modules.TemplArenaHeroMessageCell = Modules.TemplArenaHeroMessageCell or BaseClass(GUI.ScrollItem)

function Modules.TemplArenaHeroMessageCell:__init()
	self:InitTemplate(GUI.TemplateRegistry.ARENA_HEROMESSAGE_CELL)
end

function Modules.TemplArenaHeroMessageCell:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	self:GetNode():SetZOrder(100001 - self.__data.rank)
	--位置容器
	self.__panel_pos = self:GetWidget("Panel_pos")
	--背景图片
	local image_bg_str = self.__data.rank == 1 and "rank_pillar_champion" or (self.__data.rank == 2 and "rank_pillar_second_place"
							or (self.__data.rank == 3 and "rank_pillar_third_winner" or "rank_pillar_others"))	
	local image_Bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ARENA,image_bg_str)
	self:GetWidget("Image_bg"):SetImage(image_Bg)
	--排名信息背景框
	local Image_message = self:GetWidget("Image_rankMessageBg")
	if self.__data.rank <= 3 then
		local message_bg_str = self.__data.rank == 1 and "bg_firstrank" or (self.__data.rank == 2 and "bg_secondrank"
							or "bg_thirdrank")
		local message_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.ARENA,message_bg_str,true)
		Image_message:SetImage(message_bg)
	end
	--排名信息位置偏移
	if self.__data.rank ~= 1 then		
		local offset_y = self.__data.rank == 2 and 10 or (self.__data.rank == 3 and 20 or 30)
		Image_message:SetPosition(Image_message:GetPositionX(),Image_message:GetPositionY() + offset_y)		
	end
	--英雄模型和名字
	self.__panel_forlead = self:GetWidget("Panel_forLead")
	self.__panel_forlead:SetTouchSwallow(false)
	local panel_heroModule = self:GetWidget("Panel_heroModule")
	self.__text_playername = self:GetWidget("Text_playerName")	
	Util:WidgetLabel(self.__text_playername,"",Macros.TypefaceSize.largish,Macros.Color.btn,0,0,Macros.Color.common_stroke)	

	--判断是否为机器人
	local IsRobot,config = ObjIDTool.GetArenaRobotData(data.target_id)
	self.__heromodel = LogicGUI.Model.New({info_id = IsRobot and config.id or data.head_icon,parent = panel_heroModule})
	self.__text_playername:SetText(IsRobot and config.name or data.target_name)
	self.__name = IsRobot and config.name or data.target_name
	self.__data.__actual_headicon =	IsRobot and config.id or data.head_icon

	--缩放比例
    local config_hero = CPPGameLib.GetConfig("Hero", IsRobot and config.id or data.head_icon)
	self.__heromodel:SetScaleFactor(config_hero and config_hero.ui_scale or 1)
	self.__heromodel:SetPosition(0,0)

	--当前自己排名
	local bt_fight_five_times = self:GetWidget("Button_fiveTimes")
	bt_fight_five_times:IgnoreContentAdaptWithSize(true)
	if self.__data.__curplayer_rank then		
		bt_fight_five_times:SetVisible(self.__data.__curplayer_rank < self.__data.rank)
		Util:WidgetButton(bt_fight_five_times, "", function ()	
			--判断精力
			local enery = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.ENERGY)
	    	if enery < ConfigAdapter.Common.GetArenaCostEnery() then
	    		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("arena_noEnough_stamina"))
	    		LogicGUI.TemplConsumeBuy.New( Macros.Game.RolePropType.ENERGY )
	    		return
	    	end
	    	print("skfb", "xxxxxxxxxxxxxxxxxxx ", data.battle_value)
	    	--存储敌人战力
	    	CallModuleFunc(ModuleType.ARENA,"SetEnemyBattleValue",data.battle_value)
	    	--挑战5次
	    	LogicGUI.TemplArenaFightFiveTimesView.New(self.__data.__actual_headicon,Macros.Game.FBType.ARENA,data.target_id)
		end)
	else
		bt_fight_five_times:SetVisible(false)
	end
	
	--设置回调	
	WidgetUnity.SetWidgetListener(self.__panel_forlead,function ()
		self.__is_attack = true
		self.__is_on_lead = CallModuleFunc(ModuleType.LEAD, "IsInLead")
		CPPActionManager.DelayTo(bt_fight_five_times,0.2,GlobalCallbackMgr:AddCallBackFunc(function ()
			self.__is_attack = false
		end))
		end, nil, function()
			print("ldx","CallModuleFunc(ModuleType.LEADCallModuleFunc(ModuleType.LEAD: ",self.__is_on_lead)
			if self.__is_attack or self.__is_on_lead then
				self:ChallengeFunc()
		    end
		end)
	--排名
	local rank_txt = Util:RichText("",Macros.TypefaceSize.largish, 0, 0, Macros.Color.Special_hex)
	rank_txt:SetHorizonFontSpace(2)
	rank_txt:SetText(CPPGameLib.GetString("arena_rank_txt",tostring(self.__data.rank)))
	Image_message:AddChild(rank_txt)
	PosTool.CenterBottom(rank_txt,0,85)
	rank_txt:SetVisible(self.__data.rank > 3)
	
	--战力
	self.__panel_battlevalue = self:GetWidget("Panel_battlevalue")
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "power_num", true)
	self.__powernum = GUI.ImgFont.New(sp)	
	self.__powernum:SetAnchorPoint(0,0.5)
	self.__panel_battlevalue:AddChild(self.__powernum:GetNode())
	PosTool.LeftCenter(self.__powernum)	
	--战力调整
	local battle_value = self.__data.battle_value
    if self.__data.battle_value >= 1000000 then
    	battle_value = math.floor( self.__data.battle_value/10000 )
    	--万
    	self.__myriad_font = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "myriad", true) )
    	self.__panel_battlevalue:AddChild(self.__myriad_font)
    	self.__myriad_font:SetTag(255)
    end
    self.__powernum:SetText(tostring(battle_value))
   	PosTool.RightTo( self.__powernum , self.__myriad_font )
	--我自己的标记
	self.__image_myself = self:GetWidget("Image_mySelf")
	self.__image_myself:SetVisible(self.__data.__is_myself ~= nil)
	--气泡
	self.__image_buddle = self:GetWidget("Image_buddle")
	self.__image_buddle:SetOpacity(0)
	self:GetWidget("Text_buddle"):SetVisible(false)
	self.__text_buddle = Util:Label("", 40, Macros.Color.keypoint,285,145)	
	self.__text_buddle:SetAnchorPoint(0,0.5)
	self.__image_buddle:AddChild(self.__text_buddle)
	self.__text_buddle:SetPosition(45,79)
	--称号
	if self.__data.title_id and self.__data.title_id ~= 0 then
		local titlesp = Resource.PathTool.GetTitleIconPath("sp_title_"..self.__data.title_id,true)
        local title_sp = Util:Sprite(titlesp)
        self:GetWidget("Panel_pos"):AddChild(title_sp,5)
        PosTool.Center(title_sp,0,170)
	end
end

function Modules.TemplArenaHeroMessageCell:ChallengeFunc()
	--挑战
	--不能打自己
	if self.__data.__is_myself then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("arena_fightself"))
		return
	end
	--判断精力
	local enery = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.ENERGY)
	if enery < ConfigAdapter.Common.GetArenaCostEnery() then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("arena_noEnough_stamina"))
		LogicGUI.TemplConsumeBuy.New( Macros.Game.RolePropType.ENERGY )
		return
	end

	--存储敌人战力
	CallModuleFunc(ModuleType.ARENA,"SetEnemyBattleValue",self.__data.battle_value)
	
	--根据排名情况   	
	if self.__data.rank <= 10 then
    	if self.__data.__curplayer_rank and self.__data.__curplayer_rank <= 20 then
    		CallModuleFunc(ModuleType.ARENA,"SetArenaEnemyInfo",self.__data.rank,self.__index,self.__name,self.__data.__actual_headicon,self.__data.battle_value)
	    	CallModuleFunc(ModuleType.BATTLE, "BattleStartReq", Macros.Game.FBType.ARENA,self.__data.target_id)
    	else
    		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("arena_cannotFightTip"))
    	end
	else
    	CallModuleFunc(ModuleType.ARENA,"SetArenaEnemyInfo",self.__data.rank,self.__index,self.__name,self.__data.__actual_headicon,self.__data.battle_value)
    	CallModuleFunc(ModuleType.BATTLE, "BattleStartReq", Macros.Game.FBType.ARENA,self.__data.target_id,Macros.Global.FALSE)
	end             		
end

--新手指引特殊操作检测(竞技场挑战最后一个)
function Modules.TemplArenaHeroMessageCell:CheckSectionOperateSP()
	CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.ARENA_CHALLENGE_LAST, self.__panel_forlead)                		
end

function Modules.TemplArenaHeroMessageCell:GetImageBubble(is_shield)
	--随机喊话内容
	if not is_shield then
		local cur_info 
		for _,rank_info in pairs(CPPGameLib.GetConfig("ArenaRankTips", nil, nil , true )) do
			if rank_info.rank_min >= self.__data.rank and rank_info.rank_max <= self.__data.rank then
				cur_info = rank_info
				break
			end
		end
		local show_str = ""
		if cur_info and cur_info.tips_list then
			local rand_index = MathTool.GetRandom(#cur_info.tips_list,1) 
			local show_info = CPPGameLib.GetConfig("GameTipsList",cur_info.tips_list[rand_index])	
			show_str = show_info.text
		end
		self.__text_buddle:SetString(show_str)
	end
	return self.__image_buddle
end

function Modules.TemplArenaHeroMessageCell:GetHeroMessageWidget()
	self.__myriad_font = self.__myriad_font or self.__panel_battlevalue:GetChildByTag(255)
	return self.__heromodel,self.__powernum,self.__text_playername,self.__image_myself,self.__panel_battlevalue,self.__myriad_font
end

function Modules.TemplArenaHeroMessageCell:__delete()
	if self.__heromodel then
		self.__heromodel:DeleteMe()
		self.__heromodel = nil
	end
end

--获取位置容器
function Modules.TemplArenaHeroMessageCell:GetPanelPos()
	return self.__panel_pos
end

