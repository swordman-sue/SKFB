--
-- @brief 竞技场
-- @auther: ldx
-- @date 2016年10月17日 11:04:36
--

Modules = Modules or {}
Modules.ArenaView = Modules.ArenaView or BaseClass(BaseView)
local ArenaViewType = {
	MainView = 1,
	ReputationShop = 2,

}
function Modules.ArenaView:__init()
	self.__layout_name = "arena"
	self.__title_bar_params = {
		close_func = function ()
			self:CloseManual()		
		end,
		title = CPPGameLib.GetString("arena_systemTitle"),
		show_power = true,
		show_energy	= true,
		show_arena_prestige	= true,
		show_diamond = true,		
		
	}
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.ARENA)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.ARENA,true)
	--todo
	self:__AddPreloadTemplate(GUI.TemplateRegistry.ARENA_HEROMESSAGE_CELL)
	self.__open_callback = function ()
		self:__SetArenaViewBg()
		self:__OpenCallback()
	end
end

function Modules.ArenaView:__OpenCallback()
	local isWin = unpack(self.__open_data)
	self.__data = self:CallModuleFunc("GetData")	
	--头像
	local head_icon = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.HEAD_ICON)
	local Panel_icon = self:GetWidget("Panel_RoleHead")
	local role_icon = Util:Sprite(Resource.PathTool.GetHeroHeadPath(head_icon))
    Panel_icon:AddChild(role_icon)
    PosTool.Center(role_icon)
    --图片适配
    for i=10,13 do
    	local image = self:GetWidget("Image_"..i)
    	image:IgnoreContentAdaptWithSize(true)
    end
    self:GetWidget("Image_3"):IgnoreContentAdaptWithSize(true)
	--排名
	Util:WidgetLabel(self:GetWidget("Text_topRank"),CPPGameLib.GetString("arena_rankText"), Macros.TypefaceSize.normal, Macros.Color.Special)
	--排名（数字）
	self.__text_rankNum = self:GetWidget("Text_rankNum")
	Util:WidgetLabel(self.__text_rankNum,"", Macros.TypefaceSize.normal, Macros.Color.btn)
	--排名奖励
	Util:WidgetLabel(self:GetWidget("Text_rankEncourage"),CPPGameLib.GetString("arena_rankEncourageText"), Macros.TypefaceSize.normal, Macros.Color.Special)
	--奖励容器
	self.__panel_encourage = self:GetWidget("Panel_encourage")	
	self.__text_reputationNum = self:GetWidget("Text_reputationNum")
	Util:WidgetLabel(self.__text_reputationNum,"", Macros.TypefaceSize.normal, Macros.Color.btn)
	self.__text_goldNum = self:GetWidget("Text_goldNum")
	Util:WidgetLabel(self.__text_goldNum,"", Macros.TypefaceSize.normal, Macros.Color.btn)
	--未入榜
	self.__text_weirubang = self:GetWidget("Text_weirubang")
	Util:WidgetLabel(self.__text_weirubang,CPPGameLib.GetString("arena_notInRankListText"), Macros.TypefaceSize.normal, Macros.Color.btn)
	--未入榜提示
	self.__text_encourageTip = self:GetWidget("Text_encourageTip")
	Util:WidgetLabel(self.__text_encourageTip,CPPGameLib.GetString("arena_rankEncourageTip"), Macros.TypefaceSize.popup, Macros.Color.btn)	
	--每次消耗精力
	Util:WidgetLabel(self:GetWidget("Text_costenery"),CPPGameLib.GetString("arena_CostTip"), Macros.TypefaceSize.normal, Macros.Color.Special)
	local txt_costenerynum = self:GetWidget("Text_costeneryNum")
	Util:WidgetLabel(txt_costenerynum,ConfigAdapter.Common.GetArenaCostEnery(), Macros.TypefaceSize.normal, Macros.Color.btn)	
	--插入自己排行榜位置			
	self:insertSelfRankPos()	
	--计算机器人的战力
	self:countBattleValue()	
	--规则
	Util:WidgetButton(self:GetWidget("Button_rule"),"",function ()
	    local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
        help:SetLoadedCallback( function ( ... )
           help:SetCell("Rule_Athletics")
        end )
	end)

	--排行榜
	Util:WidgetButton(self:GetWidget("Button_rank"),"",function ()
		local info = {}
		info.type = Macros.Game.RankType.ARENA
		CallModuleFunc(ModuleType.RANK,"RankListRequest",info)
	end)	
	--战报
	Util:WidgetButton(self:GetWidget("Button_fightReport"),"",function ()
		self:CallModuleFunc("ArenaFightReportReq")
	end)
	--布阵
	Util:WidgetButton(self:GetWidget("Button_buzhen"),"",function ()
		CallModuleFunc(ModuleType.HERO, "OpenView", BaseViewType.HERO_LINEUP)
	end)	
	--声望商店	
	local button_reputationshop = self:GetWidget("Button_reputationShop")
	Util:WidgetButton(button_reputationshop,"",function ()
		GlobalModulesMgr:OpenSystemModule(Macros.Game.SystemID.ARENA_REPUTATIONSHOP)
	end)	

	--红点
	local red_num = CallModuleFunc(ModuleType.RED_MGR, "GetRed", Macros.Game.SystemID.ARENA)
	local entry_red_dot_image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
	self.__entry_red_dot = SpriteEx.CreateWithImageFile(entry_red_dot_image_data)
	self.__entry_red_dot:SetAnchorPoint(1,1)
	self.__entry_red_dot:SetVisible(red_num > 0)
	button_reputationshop:AddChild(self.__entry_red_dot,1)
	PosTool.RightTop(self.__entry_red_dot,-5,-5)	

	--遮挡容器
	self.__panel_shield = self:GetWidget("Panel_shield")
	self.__panel_shield:SetVisible(false) 
	--排行榜容器
	self.randomNum = self.__myIndex	
	local Panel_ArenaScrollView = self:GetWidget("Panel_ArenaScrollView")
	local params = {
		item_class = Modules.TemplArenaHeroMessageCell,
	    item_width	= 257,
	    item_height	= Macros.Global.DesignSize.height,
	    row	= 1,
	    col	= 5,
	    horizon = true,
	    view_width = Macros.Global.DesignSize.width,
    	view_height = Macros.Global.DesignSize.height,
	}
	self.__scrollView = GUI.ArenaScrollView.New(params)
	Panel_ArenaScrollView:AddChild(self.__scrollView:GetNode())
	PosTool.Center(self.__scrollView)
	self.__scrollView:SetDataList(self.__data.target_list)	

	self.funId = GlobalCallbackMgr:AddCallBackFunc(function ()
		local item = self.__scrollView:GetItem(self.__talk_bubble_index)
		if item then
			local talk_bubble = item:GetImageBubble(true)
			CPPActionManager.FadeOut(talk_bubble, 0.3, GlobalCallbackMgr:AddCallBackFunc(function()
				 self:RunBubbleAction()
			end))
		else
			self:RunBubbleAction()
		end
	end)	
	--移动到某个位置
	self:LocateToSomeWhere()

	--判断是否需要踢人
	self:__BeforeRunHitSomeOne(isWin)
			
	--新手指引特殊操作检测(竞技场挑战最后一个)
	local item = self.__scrollView:GetItem(1)
	if item then
		item:CheckSectionOperateSP()
	end
end

function Modules.ArenaView:__RegistAllEvents()
	--英雄模型动作响应
	self:BindGlobalEvent(Macros.Event.ArenaModule.ARENA_QUICK_BATTLE_ACTION_RESP, function(_,is_action)
		if self.__scrollView then
			self.__scrollView:ItemIter(function (index, item)
				local hero_module,powernum,text_playername,image_myself = item:GetHeroMessageWidget()
				if is_action then
					hero_module:ResumeAnimation()
				else
					hero_module:PauseAnimation()
				end				
			end)
		end
		if self.__talk_bubble_index then
			local item = self.__scrollView:GetItem(self.__talk_bubble_index)
			if item then
				local talk_bubble = item:GetImageBubble()
				if is_action then
					self:RunBubbleAction()
				else
					CPPActionManager.StopAllActions(self.__scrollView:GetNode())
					CPPActionManager.StopAllActions(talk_bubble)
				end
			end
		end
	end)

	--红点更新事件
	self:BindGlobalEvent(Macros.Event.RedModule.RED_UPDATE, function(_, system_id)
		--更新阵容红点
		if system_id == Macros.Game.SystemID.ARENA then
			local red_num = CallModuleFunc(ModuleType.RED_MGR, "GetRed", Macros.Game.SystemID.ARENA)		
			self.__entry_red_dot:SetVisible(red_num > 0)
		end
	end)	
end

--气泡
function Modules.ArenaView:RunBubbleAction()
	self:ShowTalk()
	local randomNum = MathTool.GetRandom(5,10)
	CPPActionManager.DelayTo(self.__scrollView:GetNode(),randomNum,self.funId)	
end

--插入自己的排行榜位置
function Modules.ArenaView:insertSelfRankPos()
	--玩家未入榜排名默认为0
	if self.__data.rank <= #CPPGameLib.GetConfig("ArenaFirstRankAward", nil, nil , true ) and self.__data.rank ~= 0 then
		local roleInfo = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo")	
		local isInsert = false
		--判断是否排行榜列表里面是否已经有该玩家
		for i,data in ipairs(self.__data.target_list) do
			data.__curplayer_rank = self.__data.rank
			if data.target_id == roleInfo.role_id then
				data.__is_myself = true
				isInsert = true
			end
		end
		--没有就插入该玩家
		if not isInsert then
			local mydata = {}
			local titleId , titleId_ = CallModuleFunc(ModuleType.TITLE ,"GetTitleId")
			mydata.target_id  = roleInfo.role_id
			mydata.rank = self.__data.rank
			mydata.target_name = roleInfo.role_name
			mydata.battle_value = roleInfo.battle_value
			mydata.head_icon = roleInfo.head_icon
			mydata.__is_myself = true
			mydata.title_id = titleId
			mydata.nickname_id = titleId_
			table.insert(self.__data.target_list,mydata)
		end
		--倒序玩家信息列表
		table.sort(self.__data.target_list,function (data_1,data_2)
			return data_1.rank > data_2.rank
		end)
		--获取当前玩家的位置
		for i,target_info in ipairs(self.__data.target_list) do
			if target_info.target_id == roleInfo.role_id then				
				self.__myIndex = i
				break
			end
		end
		self.__text_rankNum:SetText(self.__data.rank)
		self.__text_weirubang:SetVisible(false)
		self.__panel_encourage:SetVisible(true)		
		local rankEncourageData = {}
		for k,v in pairs(CPPGameLib.GetConfig("Arena", nil, nil , true )) do
			if self.__data.rank >= v.rank_min and self.__data.rank <= v.rank_max then
				rankEncourageData = v
				break
			end
		end				
		self.__text_reputationNum:SetText(rankEncourageData.award_prestige)		
		self.__text_goldNum:SetText(rankEncourageData.award_gold)
	else
		--倒序玩家信息列表
		table.sort(self.__data.target_list,function (data_1,data_2)
			return data_1.rank >= data_2.rank
		end)
		self.__myIndex = 0
		self.__text_rankNum:SetText(CPPGameLib.GetString("arena_weirubang"))
		self.__text_weirubang:SetVisible(true)
		self.__panel_encourage:SetVisible(false)
	end
	if self.__data.rank ~= 0 then
		self.__rank = self.__data.rank
		self.__history_rank = self.__data.history_rank
	else
		--标记未入榜
		self.__rank = #CPPGameLib.GetConfig("ArenaFirstRankAward", nil, nil , true ) + 1
		self.__history_rank = #CPPGameLib.GetConfig("ArenaFirstRankAward", nil, nil , true ) + 1
	end
	
end

--计算战力
function Modules.ArenaView:countBattleValue()
	for i,v in ipairs(self.__data.target_list) do
		local isRobot = ObjIDTool.GetArenaRobotData(v.target_id)
		if isRobot then				
			local robotData = CPPGameLib.GetConfig("ArenaRobot",v.target_id)
			local battleValue = 0
			for key,monsterId in ipairs(robotData.monster_list) do
				battleValue = battleValue + CallModuleFunc(ModuleType.POWER, "CalcMonsterPower", monsterId,robotData.add_pro_type_list,robotData.add_pro_value_list)
			end
			v.battle_value = battleValue
		end
	end
end

function Modules.ArenaView:ShowTalk()
	local item = self.__scrollView:GetItem(self.__talk_bubble_index)
	if item then
		local talk_bubble = item:GetImageBubble(true)
		talk_bubble:SetOpacity(0)		
	end

	local x = math.abs(math.floor(self.__scrollView:GetScrollX()/self.__scrollView:GetItemWidth())) + 1
	self.randomNum = self.__myIndex
	while(self.randomNum == self.__myIndex or self.__scrollView:GetItem(self.randomNum) == nil) do
		if x == 0 then
			self.randomNum = MathTool.GetRandom(x, x+4)
		else
			self.randomNum = MathTool.GetRandom(x, x+4)
		end		
	end	

	item = self.__scrollView:GetItem(self.randomNum)
	if item then
		self.__talk_bubble_index = self.randomNum		
		local talk_bubble = item:GetImageBubble()
		talk_bubble:SetScaleFactor(0.1)
		talk_bubble:SetOpacity(255)
		CPPActionManager.ScaleTo(talk_bubble,0.3,0.45,nil)	
	end
end

function Modules.ArenaView:LocateToSomeWhere()
	if self.__myIndex ~= 0 then
		if self.__myIndex <= 4 then
			--调整条目位置
			self.__scrollView:MyScrollCallBack(self.__close_layer1,self.__close_layer2)
			return
		else
			self.__scrollView:LocatePos(-(self.__myIndex - 3)*self.__scrollView:GetItemWidth(), 0)
		end	
	end
	--调整条目位置
	self.__scrollView:MyScrollCallBack(self.__close_layer1,self.__close_layer2)
end

function Modules.ArenaView:__BeforeRunHitSomeOne(isWin)
	--判断是否赢了
	if isWin then
		--赢了并且排名向前了
		local isMovetoFront = self:CallModuleFunc("GetIsMovetoFront")
		if isMovetoFront then
			--屏蔽层
			self.__panel_shield:SetVisible(true)
			--获取敌人信息
			local enemyname,enemyicon,enemy_battlevalue = self:CallModuleFunc("GetArenaEnemyInfo")
			local item = self.__scrollView:GetItem(self.__myIndex)
			--设置敌人的英雄模型为自己的
			local hero_module,powernum,text_playername,image_myself,panel_battlevalue,myriad_font = item:GetHeroMessageWidget()
			hero_module:SetInfoID(enemyicon)											
			--战力调整
			local battle_value = enemy_battlevalue
		    if battle_value >= 1000000 then
		    	battle_value = math.floor( enemy_battlevalue/10000 )
		    	--万
		    	if not myriad_font then
			    	myriad_font = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "myriad", true) )
			    	panel_battlevalue:AddChild(myriad_font)
			    	myriad_font:SetTag(255)
			    end
			    myriad_font:SetVisible(true)
		    elseif battle_value < 1000000 and myriad_font then
		    	myriad_font:SetVisible(false)
		    end
		    --敌人战力
			powernum:SetText(tostring(battle_value))
			PosTool.RightTo(powernum, myriad_font )
			--敌人名字
			text_playername:SetText(tostring(enemyname))
			--隐藏自己的标记
			image_myself:SetVisible(false)
			--踢飞某人
			self:__HitSomebodyFly()
		else				
			self:RunBubbleAction()
		end
	else
		self:RunBubbleAction()
	end
end

--踢飞某人
function Modules.ArenaView:__HitSomebodyFly()
	--战队
	local roleInfo = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo")	
	self.__fly_heromodel = LogicGUI.Model.New({info_id = roleInfo.head_icon ,parent = self:GetNode()})
	local pos_x, pos_y = self.__scrollView:__CalcPositionByIndex(self.__myIndex)
	local target_pos_x = self.__scrollView:GetScrollX() + pos_x + 643
	local act_target_pos_x = target_pos_x >= 1000 and (target_pos_x + 100) or (target_pos_x - 100)
	local fly_dregree = target_pos_x >= 1000 and 50 or -50
	self.__fly_heromodel:SetPosition(act_target_pos_x,500)
	self.__fly_heromodel:SetRotation(fly_dregree)
	CPPActionManager.MoveTo(self.__fly_heromodel:GetNode(),0.6,target_pos_x,300,GlobalCallbackMgr:AddCallBackFunc(function ()
		self.__fly_heromodel:DeleteMe()
		self.__fly_heromodel = nil		
		--刷新为自己的信息
		local roleInfo = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo")
		local item = self.__scrollView:GetItem(self.__myIndex)
		local hero_module,powernum,text_playername,image_myself,panel_battlevalue,myriad_font = item:GetHeroMessageWidget()
		hero_module:SetInfoID(roleInfo.head_icon)		
		--战力调整
		local battle_value = roleInfo.battle_value
	    if battle_value >= 1000000 then
	    	battle_value = math.floor( roleInfo.battle_value/10000 )
	    	--万
	    	if not myriad_font then
		    	myriad_font = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "myriad", true) )
		    	panel_battlevalue:AddChild(myriad_font)
		    	myriad_font:SetTag(255)
		    end
		    myriad_font:SetVisible(true)
	    elseif battle_value < 1000000 and myriad_font then
	    	myriad_font:SetVisible(false)
	    end
	    --自己战力
		powernum:SetText(tostring(battle_value))
	   	PosTool.RightTo(powernum, myriad_font )
		--自己名字
		text_playername:SetText(tostring(roleInfo.role_name))
		--显示自己的标记
		image_myself:SetVisible(true)
		--特效
		self.__effect_timer = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
		    self.__effect_timer = nil
			local effect_id,effect = self:__CreateEffect({id = 100049,scale = 1.2},self:GetNode())
			effect:SetPosition(target_pos_x,300)			
		end), 0.1)		
		--飞走的敌人
		local enemyname,enemyicon,enemy_battlevalue = self:CallModuleFunc("GetArenaEnemyInfo")
		self.__fly_heromodel = LogicGUI.Model.New({info_id = enemyicon ,parent = self:GetNode()})
		self.__fly_heromodel:SetPosition(target_pos_x,279)	
		self.__fly_heromodel:SetScaleFactor(1.2)	
		local dregree = target_pos_x >= 1000 and -120 or 120		
		local fly_height = 450
		CPPActionManager.RotateBy(self.__fly_heromodel:GetNode(),0.8,360 * 60)
		ActionManager.GetInstance():RunScaleToAndMoveToEffect(self.__fly_heromodel:GetNode(),0.8,target_pos_x + dregree,fly_height,0,GlobalCallbackMgr:AddCallBackFunc(function ()
				--星星特效
				local effect_id,effect = self:__CreateEffect({res_name = "UI_hejishangguang"},self:GetNode())
				effect:SetAnchorPoint(0.5,0.5)
				effect:SetPosition(target_pos_x + dregree,fly_height)
				self.__fly_txt_timer = GlobalTimerQuest:AddDelayQuest(GlobalCallbackMgr:AddCallBackFunc(function()
				    self.__fly_txt_timer = nil
					--飘字
					local label = Util:Label(CPPGameLib.GetString("arena_i_will_call_back"), Macros.TypefaceSize.headlineII, Macros.Color.blinkcontent, 0, 0, Macros.Color.common_stroke)
					self:GetNode():AddChild(label)
					label:SetPosition(target_pos_x + dregree,fly_height)
					ActionManager:RunFadeToAndMoveByEffect(label,1,0,80,0)
					self.__fly_heromodel:DeleteMe()
					self.__fly_heromodel = nil
					self:AfterFightRunAction()			
				end), 0.4)					
			end))
	end))
end

function Modules.ArenaView:AfterFightRunAction()
	self.__panel_shield:SetVisible(false)	
	local history_rank = self:CallModuleFunc("GetLastHistoryRank")
	local enemy_rank = self:CallModuleFunc("GetArenaEnemyRank")		
	if enemy_rank < history_rank then
		local nowRank = self:CallModuleFunc("GetLastRank")
		LogicGUI.TemplArenaRankBreakView.New(nowRank, enemy_rank, history_rank)
	end
	self:RunBubbleAction()
end

function Modules.ArenaView:__Dispose()
	if self.__ShowTalkEvent then
		GlobalTimerQuest:CancelQuest(self.__ShowTalkEvent)
		self.__ShowTalkEvent = nil
	end
	if self.__scrollView then
		self.__scrollView:DeleteMe()
		self.__scrollView = nil
	end
	if self.__fly_heromodel then
		self.__fly_heromodel:DeleteMe()
		self.__fly_heromodel = nil
	end
	if self.__fly_txt_timer then
        GlobalTimerQuest:CancelQuest(self.__fly_txt_timer)
        self.__fly_txt_timer = nil
    end
    if self.__effect_timer then
        GlobalTimerQuest:CancelQuest(self.__effect_timer)
        self.__effect_timer = nil
    end
end

function Modules.ArenaView:__SetArenaViewBg()
	--背景
	self.__bg_params = {image_path_list = {Resource.PathTool.GetUIResPath("bigbg/bg_sp_arena")}}
	self.__bg = MainArenaBG.New(self.__bg_params,self)					
	self:AddChild(self.__bg, Macros.Global.z_order_bottom)
	PosTool.Center(self.__bg)
	--前景1
	self.__close_layer_params1 = {image_path_list = {Resource.PathTool.GetUIResPath("bigbg/bg_sp_arena_close_layer")}}
	self.__close_layer1 = MainArenaBG.New(self.__close_layer_params1,self)					
	self:AddChild(self.__close_layer1, Macros.Global.z_order_bottom)
	PosTool.Center(self.__close_layer1)
	--前景2
	self.__close_layer_params2 = {image_path_list = {Resource.PathTool.GetUIResPath("bigbg/bg_sp_arena_close_layer")}}
	self.__close_layer2 = MainArenaBG.New(self.__close_layer_params2,self)					
	self:AddChild(self.__close_layer2, Macros.Global.z_order_bottom)
	PosTool.Center(self.__close_layer2,Macros.Global.DesignSize.width,0)
end

MainArenaBG = MainArenaBG or BaseClass(UILayer)

--[[
@ params：
	必须：
    image_path_list		[string]贴图路径
    image_size		[cocosize]背景图原尺寸
    image_anchor 	[cocopoint]背景图锚点
--]]
function MainArenaBG:__init(params,parent_view)
	self.parent_view = parent_view
	self.image_size = params.image_size or Macros.Global.BGDefaultSize
	self.image_anchor = params.image_anchor or COCOPoint(0.5, 0.5)

	self:SetAnchorPoint(0.5, 0.5)
	self:SetContentSize(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height)
	self.image_path_list = params.image_path_list
	for i,image_path in ipairs(self.image_path_list) do
		local image_node = Util:Sprite(image_path)
		image_node:SetAnchorPoint(self.image_anchor.x, self.image_anchor.y)
		self:AddChild(image_node,i)
		PosTool.Center(image_node)

		local scale_x = Macros.Global.DesignSize.width / self.image_size.width
		local scale_y = Macros.Global.DesignSize.height / self.image_size.height
		self:SetScaleFactor(math.max(scale_x, scale_y))	
	end	

	--多分辨率适配
	GUI.MultiResolutionAdapter.AdaptForBG(
		self, self.parent_view:GetNode(), self.parent_view:IsForceStretch(), self.parent_view:IsIgnoreIPhoneSafeArea())
end

function MainArenaBG:IsFixedSize()
	return false
end
