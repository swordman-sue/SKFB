Modules = Modules or {}

Modules.HeroHandBookDetailView = Modules.HeroHandBookDetailView or BaseClass(BaseView)

local MESSAGE_TYPE = {
	HERO_CHIP = 0,		--英雄碎片
	HERO_MESSAGE = 1,	--英雄信息	
	HERO_TOGETHERATTACK = 2,	--英雄合击
	HERO_FETTER = 3,	--英雄宿命
}

function Modules.HeroHandBookDetailView:__init()
	self.__layout_name = "hero_hanbook_detailmessageview"
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:__OpenCallback()
	end
	self:__AddPreloadTemplate(GUI.TemplateRegistry.HERO_HANDBOOK_HEROMESSAGEVIEW)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.HERO_HANDBOOK_HEROMESSAGECELL)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.HERO_HANDBOOK_HEROCHIPVIEW)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGCELL)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.HERO_HANDBOOK_HEROTOGETHERATTACKVIEW)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.HERO_HANDBOOK_HEROFETTERVIEW)
	self:__AddPreloadTemplate(GUI.TemplateRegistry.HERO_HANDBOOK_HEROFETTERVIEWCELL)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_HANDBOOK)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.HERO_HANDBOOK,true)
end

function Modules.HeroHandBookDetailView:__OpenCallback()
	--关闭界面事件
	self.__DeleteApproachViewEvent = self:BindGlobalEvent(Macros.Event.ApproachOfAchievingModule.DELETE_APPROACHVIEW, function()
	        self:DeleteMe()
	    end)
	--调整透明度事件
	self.__AdjustOpacityEvent = self:BindGlobalEvent(Macros.Event.HeroModule.HERO_HAND_BOOK_ADJUST_OPACITY, function(_ , params)
	        self.__bg:AdjustOpacity(params)
	    end)
	--打开界面需要一个id
	local id,open_index,is_lineup_uid,is_prelook = unpack(self.__open_data)
	local is_hero_piece = false
	local hero_info
	--当前品质高于初始品质则做调整
	if is_lineup_uid then
		hero_info = CallModuleFunc(ModuleType.HERO,"GetHeroInfo",id)
		if not hero_info then
			return
		end
		local config_hero = CPPGameLib.GetConfig("Hero",hero_info.info_id)
		--预览
		if is_prelook and hero_info.quality_level < config_hero.quality_limit then
			local copy_info = CPPGameLib.CopyTblShallow(hero_info)
			copy_info.quality_level = copy_info.quality_level + 1
			copy_info.quality_stage = 0
			hero_info = copy_info
		end
		is_lineup_uid = hero_info.quality_level > config_hero.init_quality
		id = is_lineup_uid and id or hero_info.info_id
	end
	--是否是阵营UID
	if not is_lineup_uid then
		local type = ObjIDTool.GetInfo(id)
		if type == Macros.Game.ObjType.ITEM then
			for hero_id,hero_info in pairs(CPPGameLib.GetConfig("HeroCompound",nil,nil,true)) do
				if hero_info.item_id == id then
					is_hero_piece = true
					id = hero_id
					break
				end
			end
		end	
	end
	
	--关闭界面
	local function closeView()
		self:CloseManual()
	end
	local closeButton = self:GetWidget("Button_close")
	WidgetUnity.SetWidgetListener(closeButton,nil,nil,closeView,nil)
	--翻页容器
	self.__pageview = self:GetWidget("PageView_heroDetailMessage")
	--翻页监听
	local function pageViewEvent()
        local index = self.__pageview:GetCurPageIndex()
        for i=0,3 do
        	if self.PanelTable[i] == index then
        		self:changePage(i)
        		break
        	end
        end       
    end
	WidgetUnity.SetPageViewListener(self.__pageview,pageViewEvent)
	--获取英雄属性
	local info = CPPGameLib.GetConfig("Hero",is_lineup_uid and hero_info.info_id or id)
	local quality = is_lineup_uid and hero_info.quality_level or info.init_quality
	--初始当前页面位置
	self.curPageType = -1
	--记录容器位置和存放
	self.PanelTable = {}
	local PanelCount = 0
	--英雄碎片界面
	if quality >= Macros.Game.QualityType.BLUE and info.type ~= Macros.Game.HeroType.EXP then
		self.chipView = GUI.TemplHeroHandBookHeroChipView.New(is_lineup_uid and hero_info.info_id or id,is_lineup_uid and hero_info)
		self.__pageview:AddPage(self.chipView:GetNode())
		PosTool.CenterBottom(self.chipView)
		self.PanelTable[MESSAGE_TYPE.HERO_CHIP] = PanelCount
		PanelCount = PanelCount + 1
	end
	--英雄信息界面	
	self.messageView = GUI.TemplHeroHandBookMessageView.New(is_lineup_uid and hero_info.info_id or id,is_lineup_uid and hero_info)
	self.__pageview:AddPage(self.messageView:GetNode())
	PosTool.CenterBottom(self.messageView)
	self.PanelTable[MESSAGE_TYPE.HERO_MESSAGE] = PanelCount
	PanelCount = PanelCount + 1
	--英雄合击界面
	local isHaveTogetherAttack = false
	for i,skillId in ipairs(info.skill_list) do
		local skillInfo = CPPGameLib.GetConfig("Skill",skillId)
		if skillInfo then
			if skillInfo.type == Macros.Game.SkillType.JOINT then
				isHaveTogetherAttack = true
				break
			end
		end
	end
	if isHaveTogetherAttack then
		self.attackView =  GUI.TemplHeroHandBookTogetherAttack.New(is_lineup_uid and hero_info.info_id or id,is_lineup_uid and hero_info)
		self.__pageview:AddPage(self.attackView:GetNode())
		PosTool.CenterBottom(self.attackView)
		self.PanelTable[MESSAGE_TYPE.HERO_TOGETHERATTACK] = PanelCount
		PanelCount = PanelCount + 1
	end
	--英雄宿命界面
	local isHaveReinfocement = false
	--英雄宿命 装备宿命 圣器宿命
	if is_lineup_uid then
		if info.relation_hero_ex_list or info.relation_equip_ex_list or info.relation_treasure_ex_list then
			isHaveReinfocement = true
		end
	else
		if info.relation_hero_list or info.relation_equip_list or info.relation_treasure_list then
			isHaveReinfocement = true
		end
	end
	if isHaveReinfocement then
		self.fetterView =  GUI.TemplHeroHandBookHeroFetterView.New(is_lineup_uid and hero_info.info_id or id,is_lineup_uid and hero_info)
		self.__pageview:AddPage(self.fetterView:GetNode())
		PosTool.CenterBottom(self.fetterView)
		self.PanelTable[MESSAGE_TYPE.HERO_FETTER] = PanelCount
	end
	--给对应的模块设置监听
	self:setPanelListener()
	--默认跳到第二个信息界面
	self.__pageview:ForceDoLayout()
	--传进来的跳转参数
	if open_index and self.PanelTable[open_index] then
		self:changePage(open_index,true)
	else
		self:changePage(is_hero_piece and MESSAGE_TYPE.HERO_CHIP or MESSAGE_TYPE.HERO_MESSAGE,true)
	end	
end

--设置监听绑定
function Modules.HeroHandBookDetailView:setPanelListener()
	for i=0,3 do
		local panel = self:GetWidget("Panel_messageType_"..i)
		--初始化切换标签文字
		local tagName = WidgetUnity.GetWidgetByName(panel,"Text_2")  
		tagName:SetText(CPPGameLib.GetString("hero_handbook_messageType_"..i))		
		--选中状态
		local tagButton = WidgetUnity.GetWidgetByName(panel,"Image_4")  
		--未选中状态
		local tagButtonMap = WidgetUnity.GetWidgetByName(panel,"Image_1") 
		if self.PanelTable[i] then
			panel:SetTouchEnabled(true)
			WidgetUnity.SetWidgetListener(panel,nil,nil,function ()
				self:changePage(i,true)
			end,nil)
		else
			panel:SetTouchEnabled(false)
			tagButton:SetVisible(false)
			tagButtonMap:SetGray(true)
			tagName:SetColor(unpack( {0x7F,0x7F,0x7F} ))
		end
	end
end

--换页
function Modules.HeroHandBookDetailView:changePage(messageType,IsNeedTurnPage)
	if self.curPageType ~= messageType then
		self.curPageType = messageType
	else
		return
	end
	for i=0,3 do 
		local panel = self:GetWidget("Panel_messageType_"..i)
		--选中状态
		local tagButton = WidgetUnity.GetWidgetByName(panel,"Image_4")  
		--文字
		local tagName = WidgetUnity.GetWidgetByName(panel,"Text_2")  
		if self.PanelTable[i] then
			if i == messageType then
				tagButton:SetVisible(true)
				tagName:SetColor(unpack(Macros.Color.keypoint))
			else
				tagButton:SetVisible(false)
				tagName:SetColor(unpack(Macros.Color.content))			
			end				
		end
	end	
	if IsNeedTurnPage then
		self.__pageview:ScrollToPage(self.PanelTable[messageType])
	end	
end

function Modules.HeroHandBookDetailView:__Dispose()
	if self.chipView then
		self.chipView:DeleteMe()
	end
	if self.attackView then
		self.attackView:DeleteMe()
	end
	if self.fetterView then
		self.fetterView:DeleteMe()
	end
	self.messageView:DeleteMe()		
end