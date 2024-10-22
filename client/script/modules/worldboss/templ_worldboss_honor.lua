--
-- @brief 世界boss荣誉弹窗 
-- @author: yjg
-- @date: 2016年11月19日16:30:36
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplWorldbossHonor = LogicGUI.TemplWorldbossHonor or BaseClass(GUI.Template)

function LogicGUI.TemplWorldbossHonor:__init( type )
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}

    self.__tab_params_ex = {6}

 	self.type = type
	self.typeInfo = {}
	self.typeInfo[Macros.Game.RankType.WORLDBOSS_HONOR] = {}
	self.typeInfo[Macros.Game.RankType.WORLDBOSS_HONOR].name = CPPGameLib.GetString( "worldBoss_honorRank" )
	self.typeInfo[Macros.Game.RankType.WORLDBOSS_HONOR].info = "GetBossHonor"
	
	self.typeInfo[Macros.Game.RankType.WORLDBOSS_HARM] = {}
	self.typeInfo[Macros.Game.RankType.WORLDBOSS_HARM].name = CPPGameLib.GetString( "worldBoss_harmRank" )
	self.typeInfo[Macros.Game.RankType.WORLDBOSS_HARM].info = "GetBossDamage"

	self.__open_callback = function()
		self:__Getwidget()
		self:__ShowUI()
	end


	self.event_rank = self:BindGlobalEvent(Macros.Event.WorldBossModule.WORLDBOSS_RANK,function()
		self:UpLst()
	end)

	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_WORLDBOSS_RANK_ITEM)
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_WORLDBOSS_RANK)	
end

function LogicGUI.TemplWorldbossHonor:__delete()
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

function LogicGUI.TemplWorldbossHonor:__Getwidget()
	--背景
    self.Image_bg = self:GetWidget("Image_bg") 
    --关闭
    self.Button_close = Util:WidgetButton( self:GetWidget("Button_close") , nil , CPPGameLib.Handler(self,self.OnButtonClose) )
    --光
    self.Image_light = self:GetWidget("Image_light") 
    --内容框
    self.Image_lstBg = self:GetWidget("Image_lstBg") 
    --内容框
    self.Panel_tit = self:GetWidget("Panel_tit") 
end

function LogicGUI.TemplWorldbossHonor:__ShowUI()
	--荣誉
	local award = Util:Label( self.typeInfo[ self.type ].name , Macros.TypefaceSize.popup, Macros.Color.white )
	award:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2)
	award:SetAnchorPoint(0.5,0.5)
	self.Image_light:AddChild(award)

    --条目
    self.params =  
    {
        item_class = Modules.TemplWorldbossHonorItem,
        item_width = self.Image_lstBg:GetContentWidth() ,
        item_height = 122.00,
        row = 3,
        col = 1,
        item_space_r = 5 ,
        view_height = self.Image_lstBg:GetContentHeight() - 5 ,
    }

     self.text = {
            [1] = {name = CPPGameLib.GetString("hero_handbook_buttonType_1") , value = Macros.Game.FORCE.CAMP_LEGEND },
            [2] = {name = CPPGameLib.GetString("hero_handbook_buttonType_0") , value = Macros.Game.FORCE.CAMP_SUPER },
            [3] = {name = CPPGameLib.GetString("hero_handbook_buttonType_2") , value = Macros.Game.FORCE.CAMP_WARCRAFT },
            [4] = {name = CPPGameLib.GetString("worldBoss_award") , value = Macros.Game.SystemID.LEVEL_SHOP },
    }

    self.__tab:SetAnchorPoint(0.5,0.5)
    self.__tab:SetContentSize( self.Panel_tit:GetContentWidth() , self.Panel_tit:GetContentHeight() - 10 )
    self.__tab:SetData( self.text )

    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen")
    local no = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bigblue")
    self.__tab:SetOnCeil( on , no )  

    self.Panel_tit:AddChild(self.__tab:GetNode())
    PosTool.Center( self.__tab:GetNode() , 16 )

    self.__tab:SetTouchEnabled(false)
    self.__tab:On( 1 )

    --提示容器
    self.tipsLayer = Util:Layout( self.Image_bg:GetContentWidth() , self.Image_bg:GetContentHeight() )
    self.tipsLayer:SetAnchorPoint(0.5,0.5)
    self.Image_bg:AddChild( self.tipsLayer )
    PosTool.Center( self.tipsLayer )

    --我的排名
    local MyRank = Util:Label( CPPGameLib.GetString("worldBoss_MyRank") , nil , Macros.Color.keypoint  )
    MyRank:SetAnchorPoint( 0 , 0.5 )
    MyRank:SetPosition(  self.Image_lstBg:GetPositionX() - (self.Image_lstBg:GetContentWidth()/2) + 10 , self.Image_lstBg:GetPositionY() - self.Image_lstBg:GetContentHeight() - MyRank:GetContentHeight() + 5 )
    self.tipsLayer:AddChild( MyRank )

    local honor , honorRank = CallModuleFunc(ModuleType.WORLDBOSS,  self.typeInfo[self.type].info )

    local text = CPPGameLib.GetString("worldBoss_NoRank")
    if honorRank ~= 0 then
    	text = CPPGameLib.GetString( "worldBoss_Rank" , honorRank )
    end
    
    self.MyRank = Util:Label( text , nil , Macros.Color.content  )
    PosTool.RightTo( MyRank , self.MyRank )
    self.tipsLayer:AddChild( self.MyRank )

    local lst = CallModuleFunc(ModuleType.WORLDBOSS, "GetRankAward", self.type )
    
    local type_ = Macros.Game.RolePropType.REBEL_VALUE
    local num = CPPGameLib.GetString("worldBoss_NO")
    if lst[honorRank] then
		for i,v in pairs(lst[honorRank]) do
			if type(v) == "table" then
				local type_ , info_ = ObjIDTool.GetInfo( v.item )
				if info_.sub_type == Macros.Game.NormalItemSubType.CURRENCY then
					type_ = info_.param1
					num = v.itemNum
				end
			end
		end
    end

	local tab = {
			{ zi = CPPGameLib.GetString("worldBoss_rankingAward"), ys = Macros.Color.keypoint },
			{ tp = Util:GetMoneyIconImgData(type_) },
			{ zi = num, ys = Macros.Color.content },
		}
	self.award = Util:LabCenter(tab)
    self.award:SetAnchorPoint( 1 , 0.5 )
    self.award:SetPosition( self.Image_lstBg:GetPositionX() + (self.Image_lstBg:GetContentWidth()/2) - 10 , self.MyRank:GetPositionY() )
    self.tipsLayer:AddChild( self.award )

end

function LogicGUI.TemplWorldbossHonor:OnButtonClose()
	CallModuleFunc(ModuleType.WORLDBOSS, "LocalRankRemove")
	self:DeleteMe()
end


function LogicGUI.TemplWorldbossHonor:Ceil( index , data )
    local on = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bigblue")
    local sizeSp = Util:Sprite( on )

    local layer = Util:Layout( sizeSp:GetContentWidth() , sizeSp:GetContentHeight() )

    if data.value then
        local image_data = nil
        if data.value == Macros.Game.SystemID.LEVEL_SHOP then
        	local iconSp = CPPGameLib.GetConfig("SystemEntry", Macros.Game.SystemID.LEVEL_GIFTBAG )
			image_data = Resource.PathTool.GetSystemEntryIconPath( iconSp.team_icon )

	    else
	        image_data = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"camp_icon_"..data.value , true)
        end

        local icon = Util:Sprite(image_data)
        icon:SetAnchorPoint( 0.5 , 0.5 )
        icon:SetScaleFactor( 0.8 )
        icon:SetPosition( (icon:GetContentWidth()/2) - 5  , icon:GetContentHeight()/2 )
        layer:AddChild(icon)

        if data.value == Macros.Game.SystemID.LEVEL_SHOP then
        	icon:SetPosition( (icon:GetContentWidth()/2) - 8  , icon:GetContentHeight()/2 - 10 )
        end

        local name = Util:Label( data.name , Macros.TypefaceSize.popup , Macros.Color.blinkcontent , nil , nil , Macros.Color.button_yellow )
        name:SetAnchorPoint( 0 , 0.5 )
        name:SetPosition( 70 , layer:GetContentHeight()/2 )
        layer:AddChild( name )

    else

        local name = Util:Label( data.name , Macros.TypefaceSize.popup , Macros.Color.blinkcontent , nil , nil , Macros.Color.button_yellow )
        name:SetAnchorPoint( 0.5 , 0.5 )
        name:SetPosition( layer:GetContentWidth()/2 , layer:GetContentHeight()/2 )
        layer:AddChild( name )
    end

    return layer
end

function LogicGUI.TemplWorldbossHonor:OnCallBack( index , data )
    self:OnYeMei(index)
end


function LogicGUI.TemplWorldbossHonor:OnYeMei(index)
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

	if self.previewLayer then
		self.previewLayer:SetVisible(false)
	end

	if self.tipsLayer then
		self.tipsLayer:SetVisible(true)
	end

	if index == 1 then
		if self.legend then
			self.legend:SetVisible(true)
		else
			self:EstablishLegendLst()
		end
	elseif index == 2 then
		if self.super then
			self.super:SetVisible(true)
		else
			self:EstablishSuperLst()
		end
	elseif index == 3 then
		if self.warcraft then
			self.warcraft:SetVisible(true)
		else
			self:EstablishWarcraftLst()
		end
	elseif index == 4 then
		self.tipsLayer:SetVisible(false)

		if self.preview then
			self.previewLayer:SetVisible(true)
			self.preview:SetVisible(true)
		else
			self:EstablishPreviewLst()
		end
	end
end

--超能
function LogicGUI.TemplWorldbossHonor:EstablishSuperLst()
	self.super = GUI.ScrollView.New(self.params)
	self.Image_lstBg:AddChild(self.super:GetNode())
	PosTool.Center(self.super:GetNode())

	local info = {}
	info.type = self.type
	info.sub_type = Macros.Game.FORCE.CAMP_SUPER
	CallModuleFunc(ModuleType.WORLDBOSS, "RankReq",info)
end

--传说
function LogicGUI.TemplWorldbossHonor:EstablishLegendLst()
	self.legend = GUI.ScrollView.New(self.params)
	self.Image_lstBg:AddChild(self.legend:GetNode())
	PosTool.Center(self.legend:GetNode())

	local info = {}
	info.type = self.type
	info.sub_type = Macros.Game.FORCE.CAMP_LEGEND
	CallModuleFunc(ModuleType.WORLDBOSS, "RankReq",info)
end

--幻想
function LogicGUI.TemplWorldbossHonor:EstablishWarcraftLst()
	self.warcraft = GUI.ScrollView.New(self.params)
	self.Image_lstBg:AddChild(self.warcraft:GetNode())
	PosTool.Center(self.warcraft:GetNode())

	local info = {}
	info.type = self.type
	info.sub_type = Macros.Game.FORCE.CAMP_WARCRAFT
	CallModuleFunc(ModuleType.WORLDBOSS, "RankReq",info)
end

--预览
function LogicGUI.TemplWorldbossHonor:EstablishPreviewLst()

	self.previewLayer = Util:Layout( self.Image_lstBg:GetContentWidth() , self.Image_lstBg:GetContentHeight() )
	self.previewLayer:SetAnchorPoint( 0.5 , 0.5 )
	self.Image_lstBg:AddChild( self.previewLayer )
	PosTool.Center( self.previewLayer )

	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_property_tag")
	local sp_1 = Util:Sprite9(sp)
	sp_1:SetContentSize(192 , 44 )
	sp_1:SetAnchorPoint(0,1)
	sp_1:SetPosition(2.5 , self.previewLayer:GetContentHeight() - 2.5)
	self.previewLayer:AddChild(sp_1)

	local ranking = Util:Label( CPPGameLib.GetString("worldBoss_ranking") , nil , Macros.Color.white )
	ranking:SetPosition(sp_1:GetContentWidth()/2 , sp_1:GetContentHeight()/2)
	sp_1:AddChild(ranking)
	PosTool.Center(ranking)

	local sp_2 = Util:Sprite9(sp)
	sp_2:SetContentSize(400 , 44 )
	sp_2:SetAnchorPoint(1,1)
	sp_2:SetPosition(self.previewLayer:GetContentWidth() - 2.5 , self.previewLayer:GetContentHeight() - 2.5 )
	self.previewLayer:AddChild(sp_2)

	local rankingAward = Util:Label( CPPGameLib.GetString("worldBoss_rankingAward") , nil , Macros.Color.white )
	rankingAward:SetPosition(sp_2:GetContentWidth()/2 , sp_2:GetContentHeight()/2)
	sp_2:AddChild(rankingAward)
	PosTool.Center(rankingAward)

	local worldBoss_AwardTimeTips = Util:Label( CPPGameLib.GetString("worldBoss_AwardTimeTips") , nil ,  Macros.Color.keypoint )
	worldBoss_AwardTimeTips:SetAnchorPoint(0.5,0.5)
	worldBoss_AwardTimeTips:SetPosition( self.previewLayer:GetContentWidth()/2 , self.previewLayer:GetPositionY() - (self.previewLayer:GetContentHeight()/2) - worldBoss_AwardTimeTips:GetContentHeight() + 5 )
	self.previewLayer:AddChild( worldBoss_AwardTimeTips )

    local params =  
    {
        item_class = Modules.TemplWorldbossHonorPreviewItem,
        item_width = 598.00,
        item_height = 42.00,
        row = 4,
        col = 1,
        item_space_r = 3 ,
        view_width = self.Image_lstBg:GetContentWidth() ,
        view_height = self.Image_lstBg:GetContentHeight() - sp_2:GetContentHeight() - 7 ,
    }
	self.preview = GUI.ScrollView.New(params)
	self.preview:SetPosition( self.Image_lstBg:GetContentWidth()/2 , (self.Image_lstBg:GetContentHeight()/2) - (sp_2:GetContentHeight()/2) )
	self.Image_lstBg:AddChild(self.preview:GetNode())

	local lst = CallModuleFunc(ModuleType.WORLDBOSS, "GetRankAward", self.type )
	self.preview:SetDataList( lst )
end

--刷新列表
function LogicGUI.TemplWorldbossHonor:UpLst()
	local info = {}
	info.type = self.type
	if self.index == 1 then
		info.sub_type = Macros.Game.FORCE.CAMP_LEGEND
		local lst = CallModuleFunc(ModuleType.WORLDBOSS, "GetLocalRankRecord",info)
		
		for k,v in pairs(lst) do
			v.type = self.type
		end
		self.legend:SetDataList( lst )
	elseif self.index == 2 then
		info.sub_type = Macros.Game.FORCE.CAMP_SUPER	
		local lst = CallModuleFunc(ModuleType.WORLDBOSS, "GetLocalRankRecord",info)
		for k,v in pairs(lst) do
			v.type = self.type
		end
		self.super:SetDataList( lst )
	elseif self.index == 3 then
		info.sub_type = Macros.Game.FORCE.CAMP_WARCRAFT
		local lst = CallModuleFunc(ModuleType.WORLDBOSS, "GetLocalRankRecord",info)
		for k,v in pairs(lst) do
			v.type = self.type
		end
		self.warcraft:SetDataList( lst )
	end
end







Modules = Modules or {}
Modules.TemplWorldbossHonorPreviewItem = Modules.TemplWorldbossHonorPreviewItem or BaseClass(GUI.ScrollItem)

function Modules.TemplWorldbossHonorPreviewItem:__init(data,direct)
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)
end

function Modules.TemplWorldbossHonorPreviewItem:__delete()
end

function Modules.TemplWorldbossHonorPreviewItem:__CreateWidgetFromLuaCode(...)
	self.layout = Util:Layout(585, 40)
	self.layout:SetAnchorPoint(0.5, 0.5)
	return self.layout
end

function Modules.TemplWorldbossHonorPreviewItem:SetData(data)

	local sp_2 = nil
	if data.show == true then
		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_lst_rankdeep")
		sp_2 = Util:Sprite9(sp)
		sp_2:SetContentSize(self.layout:GetContentWidth() , self.layout:GetContentHeight() )
		sp_2:SetPosition(self.layout:GetContentWidth() /2 , self.layout:GetContentHeight() /2)
		self.layout:AddChild(sp_2)
	else  
		sp_2 = Util:Layout(self.layout:GetContentWidth() , self.layout:GetContentHeight())
		sp_2:SetAnchorPoint( 0.5 ,0.5 )
		sp_2:SetPosition(self.layout:GetContentWidth() /2 , self.layout:GetContentHeight() /2)
		self.layout:AddChild(sp_2)
	end


	local icon = nil
	if data.rank <= 3 then
		local imageData = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"com_sp_rank_num_"..data.rank)
		icon = Util:Sprite(imageData)
		icon:SetScaleFactor(0.65)
		
		sp_2:AddChild(icon)	
	else
		icon = Util:Label( data.rank , nil , Macros.Color.headline )
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
	    self.icon_layer:ShowIconQuality(false)
		self.icon_layer:ShowIconShade(false)
		self.icon_layer:ShowIconBG(false)
	    layout:AddChild(self.icon_layer:GetNode())  

	    local num = Util:Label(v.itemNum , Macros.TypefaceSize.slightly , Macros.Color.content )
	    PosTool.RightTo(self.icon_layer , num , -35)
	    layout:AddChild(num)  
	end
end

Modules = Modules or {}
Modules.TemplWorldbossHonorItem = Modules.TemplWorldbossHonorItem or BaseClass(GUI.ScrollItem)

function Modules.TemplWorldbossHonorItem:__init(data,direct)
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_WORLDBOSS_RANK_ITEM) 
    self:__Getwidget()
end

function Modules.TemplWorldbossHonorItem:__delete()
end

function Modules.TemplWorldbossHonorItem:SetData(data)
	self.data = data
	self:__ShowUI(data)
end

function Modules.TemplWorldbossHonorItem:__AddToParent()
end

function Modules.TemplWorldbossHonorItem:__Getwidget( ... )
	--条目
	self.Image_item = self:GetWidget("Image_item") 
	--名字
	self.Text_name = self:GetWidget("Text_name") 
	--等级
	self.Text_lv = self:GetWidget("Text_lv")
	--公会
	self.Text_union = self:GetWidget("Text_union") 
	--最高伤害
	self.Text_maxHarm = self:GetWidget("Text_maxHarm") 
	--图标
	self.Panel_icon = self:GetWidget("Panel_icon") 
	--排行图标
	self.Panel_rankIcon = self:GetWidget("Panel_rankIcon") 

	self.Image_2 = self:GetWidget("Image_2")
	
end

function Modules.TemplWorldbossHonorItem:__ShowUI( data )

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
    local name = Util:Label(data.role_name , Macros.TypefaceSize.largish , Macros.Color.headline )
    name:SetPosition(self.Text_name:GetContentWidth()/2,self.Text_name:GetContentHeight()/2)
    name:SetAnchorPoint(0,0.5)
    self.Text_name:AddChild(name)

    --战力
    local zhandouli = Util:Label( CPPGameLib.GetString("zhandouli") , Macros.TypefaceSize.largish , Macros.Color.keypoint )
    zhandouli:SetPosition(self.Text_lv:GetContentWidth()/2,self.Text_lv:GetContentHeight()/2)
    zhandouli:SetAnchorPoint(0,0.5)
    self.Text_lv:AddChild(zhandouli)

    local zhandouli_value = Util:Label( data.battle_value , Macros.TypefaceSize.largish , Macros.Color.content )
    self.Text_lv:AddChild(zhandouli_value)
    PosTool.RightTo( zhandouli , zhandouli_value )


    local unionName = data.guild_name
    if unionName == "" then
    	unionName = CPPGameLib.GetString("rank_haveNoUnion")
    end
    --公会
    local unionText = Util:Label(CPPGameLib.GetString("rank_union") , Macros.TypefaceSize.largish , Macros.Color.keypoint )
    unionText:SetPosition(self.Text_union:GetContentWidth()/2,self.Text_union:GetContentHeight()/2)
    unionText:SetAnchorPoint(0,0.5)
    self.Text_union:AddChild(unionText)

    local union = Util:Label(unionName,Macros.TypefaceSize.largish , Macros.Color.content )
    PosTool.RightTo(unionText,union)
    self.Text_union:AddChild(union)

	local txt = ""
    if data.type == Macros.Game.RankType.WORLDBOSS_HONOR then
		txt = CPPGameLib.GetString("worldBoss_AccumulativeHonor")
    elseif data.type == Macros.Game.RankType.WORLDBOSS_HARM then
    	txt = CPPGameLib.GetString("worldBoss_AccumulativeHarm")
    end

	CPPGameLib.PrintTable("xy" , data , "排行榜回调")
	print("xy","=--------========")

    --最高
    local maxHarm = Util:Label( txt , Macros.TypefaceSize.largish , Macros.Color.headline )
    maxHarm:SetPosition(self.Text_maxHarm:GetContentWidth()/2,self.Text_maxHarm:GetContentHeight()/2)
    maxHarm:SetAnchorPoint( 0.5 , 0.5 )
    self.Text_maxHarm:AddChild(maxHarm)

    local harm = Util:Label(data.context , Macros.TypefaceSize.normal , Macros.Color.content )
    self.Image_2:AddChild(harm)  
    PosTool.Center( harm )
end