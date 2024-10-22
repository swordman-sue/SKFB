--
-- @brief 世界boss伤害弹窗 
-- @author: yjg
-- @date: 2016年11月19日16:30:36
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplWorldbossHarm = LogicGUI.TemplWorldbossHarm or BaseClass(GUI.Template)

function LogicGUI.TemplWorldbossHarm:__init(id,direct,itemResolveId)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function()
		self:__Getwidget()
		self:__ShowUI()
	end

    local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bigblue")
    self.__tab_params = {
    	{sp_2,sp_1},
    	{
			CPPGameLib.GetString("hero_handbook_buttonType_1"),
			CPPGameLib.GetString("hero_handbook_buttonType_2"),
			CPPGameLib.GetString("hero_handbook_buttonType_3"),
			CPPGameLib.GetString("worldBoss_rankAward"),
		},
		10,CPPGameLib.Handler(self, self.OnYeMei)
	}

	self.event_rank = self:BindGlobalEvent(Macros.Event.WorldBossModule.WORLDBOSS_RANK,function()
		self:UpLst()
	end)

	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_WORLDBOSS_RANK_ITEM)
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_WORLDBOSS_RANK)	
end

function LogicGUI.TemplWorldbossHarm:__delete()
	--超能
	if self.super then
		self.super:DeleteMe(false)
		self.super = nil 
	end
	--传说
	if self.legend then
		self.legend:DeleteMe(false)
		self.legend = nil 
	end
	--幻想
	if self.warcraft then
		self.warcraft:DeleteMe(false)
		self.warcraft = nil 
	end
	--预览
	if self.preview then
		self.preview:DeleteMe(false)
		self.preview = nil 
	end

end

function LogicGUI.TemplWorldbossHarm:__Getwidget()
	--背景
    self.Image_bg = self:GetWidget("Image_bg") 
    --关闭
    self.Button_close = Util:WidgetButton( self:GetWidget("Button_close") , nil , CPPGameLib.Handler(self,self.OnButtonClose) )
    --光
    self.Image_light = self:GetWidget("Image_light") 
    --内容框
    self.Image_lstBg = self:GetWidget("Image_lstBg") 
end

function LogicGUI.TemplWorldbossHarm:__ShowUI()
	--荣誉
	local award = Util:Label( CPPGameLib.GetString("worldBoss_harm") , Macros.TypefaceSize.largish, Macros.Color.headline )
	award:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2)
	award:SetAnchorPoint(0.5,0.5)
	self.Image_light:AddChild(award)

    --条目
    self.params =  
    {
        item_class = Modules.TemplWorldbossHarmItem,
        item_width = 576.00,
        item_height = 112.00,
        row = 4,
        col = 1,
        view_width = self.Image_lstBg:GetContentWidth() ,
        view_height = self.Image_lstBg:GetContentHeight() - 5 ,
    }

	--页签
    self.__tab:On(1)
    self.__tab:SetPosition( 155 , 470 )
    self.Image_bg:AddChild(self.__tab:GetNode())
end

function LogicGUI.TemplWorldbossHarm:OnButtonClose()
	CallModuleFunc(ModuleType.WORLDBOSS, "LocalRankRemove")
	self:DeleteMe()
end


function LogicGUI.TemplWorldbossHarm:OnYeMei(index)
	self.index = index
	--超能
	if self.super then
		self.super:SetVisible(false)
	end
	--传说
	if self.legend then
		self.legend:SetVisible(false)
	end
	--幻想
	if self.warcraft then
		self.warcraft:SetVisible(false)
	end
	--预览
	if self.preview then
		self.preview:SetVisible(false)
	end
	if index == 1 then
		if self.super then
			self.super:SetVisible(true)
		else
			self:EstablishSuperLst()
		end
	elseif index == 2 then
		if self.legend then
			self.legend:SetVisible(true)
		else
			self:EstablishLegendLst()
		end
	elseif index == 3 then
		if self.warcraft then
			self.warcraft:SetVisible(true)
		else
			self:EstablishWarcraftLst()
		end
	elseif index == 4 then
		if self.preview then
			self.preview:SetVisible(true)
		else
			self:EstablishPreviewLst()
		end
	end
end

--超能
function LogicGUI.TemplWorldbossHarm:EstablishSuperLst()
	self.super = GUI.ScrollView.New(self.params)
	self.Image_lstBg:AddChild(self.super:GetNode())
	PosTool.Center(self.super:GetNode())

	local info = {}
	info.type = Macros.Game.RankType.WORLDBOSS_HARM
	info.sub_type = Macros.Game.FORCE.CAMP_SUPER
	CallModuleFunc(ModuleType.WORLDBOSS, "RankReq",info)
end

--传说
function LogicGUI.TemplWorldbossHarm:EstablishLegendLst()
	self.legend = GUI.ScrollView.New(self.params)
	self.Image_lstBg:AddChild(self.legend:GetNode())
	PosTool.Center(self.legend:GetNode())

	local info = {}
	info.type = Macros.Game.RankType.WORLDBOSS_HARM
	info.sub_type = Macros.Game.FORCE.CAMP_LEGEND
	CallModuleFunc(ModuleType.WORLDBOSS, "RankReq",info)
end

--幻想
function LogicGUI.TemplWorldbossHarm:EstablishWarcraftLst()
	self.warcraft = GUI.ScrollView.New(self.params)
	self.Image_lstBg:AddChild(self.warcraft:GetNode())
	PosTool.Center(self.warcraft:GetNode())

	local info = {}
	info.type = Macros.Game.RankType.WORLDBOSS_HARM
	info.sub_type = Macros.Game.FORCE.CAMP_WARCRAFT
	CallModuleFunc(ModuleType.WORLDBOSS, "RankReq",info)
end

--预览
function LogicGUI.TemplWorldbossHarm:EstablishPreviewLst()
	
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "bg_sp9_lst_nameBg")
	local sp_1 = Util:Sprite9(sp)
	sp_1:SetContentSize(192 , 44 )
	sp_1:SetAnchorPoint(0,1)
	sp_1:SetPosition(2.5 , self.Image_lstBg:GetContentHeight() - 2.5)
	self.Image_lstBg:AddChild(sp_1)

	local ranking = Util:Label( CPPGameLib.GetString("worldBoss_ranking") , nil , Macros.Color.headline )
	ranking:SetPosition(sp_1:GetContentWidth()/2 , sp_1:GetContentHeight()/2)
	ranking:SetShadow(true)
	ranking:SetShadowColor(0, 0, 0, 255)
	sp_1:AddChild(ranking)
	PosTool.Center(ranking)

	local sp_2 = Util:Sprite9(sp)
	sp_2:SetContentSize(378 , 44 )
	sp_2:SetAnchorPoint(1,1)
	sp_2:SetPosition(self.Image_lstBg:GetContentWidth() - 2.5 , self.Image_lstBg:GetContentHeight() - 2.5 )
	self.Image_lstBg:AddChild(sp_2)

	local rankingAward = Util:Label( CPPGameLib.GetString("worldBoss_rankingAward") , nil , Macros.Color.headline)
	rankingAward:SetPosition(sp_2:GetContentWidth()/2 , sp_2:GetContentHeight()/2)
	rankingAward:SetShadow(true)
	rankingAward:SetShadowColor(0, 0, 0, 255)
	sp_2:AddChild(rankingAward)
	PosTool.Center(rankingAward)


    local params =  
    {
        item_class = Modules.TemplWorldbossPreviewItem,
        item_width = 576.00,
        item_height = 42.00,
        row = 4,
        col = 1,
        view_width = self.Image_lstBg:GetContentWidth() ,
        view_height = self.Image_lstBg:GetContentHeight() - sp_2:GetContentHeight() - 7 ,
    }
	self.preview = GUI.ScrollView.New(params)
	self.preview:SetPosition( self.Image_lstBg:GetContentWidth()/2 , (self.Image_lstBg:GetContentHeight()/2) - (sp_2:GetContentHeight()/2) )
	self.Image_lstBg:AddChild(self.preview:GetNode())

	local lst = CallModuleFunc(ModuleType.WORLDBOSS, "GetRankAward",Macros.Game.RankType.WORLDBOSS_HARM)
	self.preview:SetDataList( lst )
end

--刷新列表
function LogicGUI.TemplWorldbossHarm:UpLst()
	local info = {}
	info.type = Macros.Game.RankType.WORLDBOSS_HARM
	if self.index == 1 then
		info.sub_type = Macros.Game.FORCE.CAMP_SUPER	
		local lst = CallModuleFunc(ModuleType.WORLDBOSS, "GetLocalRankRecord",info)
		self.super:SetDataList( lst )
	elseif self.index == 2 then
		info.sub_type = Macros.Game.FORCE.CAMP_LEGEND
		local lst = CallModuleFunc(ModuleType.WORLDBOSS, "GetLocalRankRecord",info)
		self.legend:SetDataList( lst )
	elseif self.index == 3 then
		info.sub_type = Macros.Game.FORCE.CAMP_WARCRAFT
		local lst = CallModuleFunc(ModuleType.WORLDBOSS, "GetLocalRankRecord",info)
		self.warcraft:SetDataList( lst )
	end
end


Modules = Modules or {}
Modules.TemplWorldbossPreviewItem = Modules.TemplWorldbossPreviewItem or BaseClass(GUI.ScrollItem)

function Modules.TemplWorldbossPreviewItem:__init(data,direct)
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)
end

function Modules.TemplWorldbossPreviewItem:__delete()
end

function Modules.TemplWorldbossPreviewItem:__CreateWidgetFromLuaCode(...)
	self.layout = Util:Layout(565, 40)
	self.layout:SetAnchorPoint(0.5, 0.5)
	return self.layout
end


function Modules.TemplWorldbossPreviewItem:SetData(data)
	local sp = nil
	if data.show == true then
		sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_lst_rankdeep")
	else  
		sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_lst_rankpale")
	end

	local sp_2 = Util:Sprite9(sp)
	sp_2:SetContentSize(self.layout:GetContentWidth() , self.layout:GetContentHeight() )
	sp_2:SetPosition(self.layout:GetContentWidth() /2 , self.layout:GetContentHeight() /2)
	self.layout:AddChild(sp_2)

	local icon = nil
	if data.rank <= 3 then
		local imageData = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"com_sp_rank_num_"..data.rank)
		icon = Util:Sprite(imageData)
		icon:SetScaleFactor(0.65)
		
		sp_2:AddChild(icon)	
	else
		icon = Util:Label(data.rank,nil,Macros.Color.golden)
		sp_2:AddChild(icon)	
	end
	icon:SetAnchorPoint(0.5,0.5)
	icon:SetPosition( 90 , sp_2:GetContentHeight() / 2)

	local layout = Util:Layout(300 , 44)
	layout:SetAnchorPoint(1,0.5)
	layout:SetPosition(self.layout:GetContentWidth() - 35 , self.layout:GetContentHeight()/2)
	self.layout:AddChild(layout)

	for i,v in ipairs(data) do
	    --图标
	    self.icon_layer = LogicGUI.IconLayer.New(false)
	    self.icon_layer:SetData({id = v.item})
	    self.icon_layer:SetPosition( 0 + (150 * (i - 1))  , layout:GetContentHeight()/2)
	    self.icon_layer:SetAnchorPoint(0,0.5)
	    self.icon_layer:SetScaleFactor(0.5)
	    layout:AddChild(self.icon_layer:GetNode())  

	    local num = Util:Label(v.itemNum , Macros.TypefaceSize.slightly )
	    PosTool.RightTo(self.icon_layer , num , -40)
	    layout:AddChild(num)  
	end
end

Modules = Modules or {}
Modules.TemplWorldbossHarmItem = Modules.TemplWorldbossHarmItem or BaseClass(GUI.ScrollItem)

function Modules.TemplWorldbossHarmItem:__init(data,direct)
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_WORLDBOSS_RANK_ITEM) 
    self:__Getwidget()
end

function Modules.TemplWorldbossHarmItem:__delete()
end

function Modules.TemplWorldbossHarmItem:SetData(data)
	self.data = data
	CPPGameLib.PrintTable(data,"排行榜回调")
	self:__ShowUI(data)
end

function Modules.TemplWorldbossHarmItem:__AddToParent()
end

function Modules.TemplWorldbossHarmItem:__Getwidget( ... )
	--条目
	self.Image_item = self:GetWidget("Image_item") 
	--名字
	self.Text_name = self:GetWidget("Text_name") 
	--公会
	self.Text_union = self:GetWidget("Text_union") 
	--最高伤害
	self.Text_maxHarm = self:GetWidget("Text_maxHarm") 
	--图标
	self.Panel_icon = self:GetWidget("Panel_icon") 
	--排行图标
	self.Panel_rankIcon = self:GetWidget("Panel_rankIcon") 
end

function Modules.TemplWorldbossHarmItem:__ShowUI( data )

	if data.rank <= 3 then
		local imageData = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"com_sp_rank_num_"..data.rank)
		local icon = Util:Sprite(imageData)
		icon:SetAnchorPoint(0.5,0.5)
		icon:SetPosition(self.Panel_rankIcon:GetContentWidth() / 2 , self.Panel_rankIcon:GetContentHeight() / 2)
		self.Panel_rankIcon:AddChild(icon)
	else
		local imageData = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"com_sp_rank_num_"..4)
		local icon = Util:Sprite(imageData)
		icon:SetAnchorPoint(0.5,0.5)
		icon:SetPosition(self.Panel_rankIcon:GetContentWidth() / 2 , self.Panel_rankIcon:GetContentHeight() / 2)
		self.Panel_rankIcon:AddChild(icon)	

		local iconValue = Util:Label(data.rank,nil,Macros.Color.golden)
		icon:AddChild(iconValue)	
		PosTool.Center(iconValue)
	end

	-- 图标
	self.icon_layer = LogicGUI.RoleIcon.New(true)
	self.icon_layer:SetTouchCallback(function()
			-- CallModuleFunc(ModuleType.CHECKROLEINFO, "OpenView", BaseViewType.CHECKROLEINFO,data)
			GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.CHECKROLEINFO , false , data )
	    end)
	self.icon_layer:SetData(data.head_icon)
	self.icon_layer:SetScaleFactor(0.85)
	self.Panel_icon:AddChild(self.icon_layer:GetNode())
	PosTool.Center(self.icon_layer)

    --名字
    local name = Util:Label(data.role_name,nil,Macros.Color.keypoint)
    name:SetPosition(self.Text_name:GetContentWidth()/2,self.Text_name:GetContentHeight()/2)
    name:SetAnchorPoint(0,0.5)
    self.Text_name:AddChild(name)

    local unionName = data.guild_name
    if unionName == "" then
    	unionName = CPPGameLib.GetString("rank_haveNoUnion")
    end
    --公会
    local unionText = Util:Label(CPPGameLib.GetString("rank_union"),Macros.TypefaceSize.minimum,Macros.Color.keypoint)
    unionText:SetPosition(self.Text_union:GetContentWidth()/2,self.Text_union:GetContentHeight()/2)
    unionText:SetAnchorPoint(0,0.5)
    self.Text_union:AddChild(unionText)

    local union = Util:Label(unionName,Macros.TypefaceSize.minimum)
    PosTool.RightTo(unionText,union)
    self.Text_union:AddChild(union)

    --最高
    local maxHarm = Util:Label(CPPGameLib.GetString("worldBoss_maxHarm") , Macros.TypefaceSize.minimum , Macros.Color.keypoint)
    maxHarm:SetPosition(self.Text_maxHarm:GetContentWidth()/2,self.Text_maxHarm:GetContentHeight()/2)
    maxHarm:SetAnchorPoint(0,0.5)
    self.Text_maxHarm:AddChild(maxHarm)

    local harm = Util:Label(data.context,Macros.TypefaceSize.slightly,Macros.Color.golden)
    PosTool.RightTo(maxHarm,harm)
    self.Text_maxHarm:AddChild(harm)  
end