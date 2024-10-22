
--
-- @brief 爬塔排行榜
-- @author: yjg
-- @date: 2016年9月26日17:01:28

LogicGUI = LogicGUI or {}

LogicGUI.TempTowerRank = LogicGUI.TempTowerRank or BaseClass(GUI.Template)


function LogicGUI.TempTowerRank:__init()
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__open_callback = function()
		self:__Getwidget()
		self:__ShowUI()
		CallModuleFunc(ModuleType.TOWER, "TowerRankReq",Macros.Game.RankType.KRUUNU)
	end

	self.rankInfo = self:BindGlobalEvent(Macros.Event.TowerModule.TOWER_RANK,function (_,data)
		self:SetShowUI()
	end)

	self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_WORLDBOSS_RANK_ITEM)
	self:InitTemplate(GUI.TemplateRegistry.TOWER_RANK)	
end

function LogicGUI.TempTowerRank:__delete()

	if self.__shield_layer then
		self.__shield_layer:DeleteMe()
		self.__shield_layer = nil
	end
	if self.__zones_scrollview then
		self.__zones_scrollview:DeleteMe()
		self.__zones_scrollview = nil
	end
end

function LogicGUI.TempTowerRank:SetData(data)
end

function LogicGUI.TempTowerRank:__Getwidget( ... )
	--居中显示
    self.Image_center = self:GetWidget("Image_center") 
    --光
    self.Image_light = self:GetWidget("Image_light") 
    --关闭
    self.Button_close = Util:WidgetButton(self:GetWidget("Button_close"), nil , CPPGameLib.Handler( self,self.OnBtnExit ) )  
    --内容
    self.Image_content = self:GetWidget("Image_content") 
    --我的排名
    self.Text_ranking = self:GetWidget("Text_ranking") 
    --历史最高
    self.Text_history = self:GetWidget("Text_history") 
end

function LogicGUI.TempTowerRank:__ShowUI( ... )	
	--标题
	self.light = Util:Label( CPPGameLib.GetString("rebel_rank") , Macros.TypefaceSize.popup , Macros.Color.lilyWhite )
	self.light:SetAnchorPoint(0.5,0.5)
	self.light:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2 )
	self.Image_light:AddChild(self.light)

	--我的排名
	local rank_myRank = Util:Label( CPPGameLib.GetString("rank_myRank") , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
	rank_myRank:SetAnchorPoint(0,0.5)
	rank_myRank:SetPosition(self.Text_ranking:GetContentWidth()/2 , self.Text_ranking:GetContentHeight()/2)
	self.Text_ranking:AddChild(rank_myRank)

	--第几名
	self.rank = Util:Label("rank", Macros.TypefaceSize.slightly , Macros.Color.content )
	self.rank:SetPosition(rank_myRank:GetPositionX() + rank_myRank:GetContentWidth() , self.Text_ranking:GetContentHeight()/2 )
	self.rank:SetAnchorPoint(0,0.5)
	self.Text_ranking:AddChild(self.rank)

	--第几名
	self.star = Util:Label("star", Macros.TypefaceSize.slightly , Macros.Color.content )
	self.star:SetPosition(self.Text_history:GetContentWidth() + 180 , self.Text_history:GetContentHeight()/2 )
	self.star:SetAnchorPoint( 1 , 0.5)
	self.Text_history:AddChild(self.star)

	--历史最高
	self.tower_history = Util:Label( CPPGameLib.GetString("tower_history_2") , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
	self.tower_history:SetAnchorPoint( 1,0.5 )
	self.tower_history:SetPosition(self.star:GetPositionX() - self.star:GetContentWidth() - 5 , self.Text_history:GetContentHeight()/2)
	self.Text_history:AddChild(self.tower_history)	

	local params =  
	{
	  item_class = Modules.TemplRankItem,
	  item_width = self.Image_content:GetContentWidth(),
	  item_height = 122.00,
	  row = 4,
	  col = 1,
	  item_space_r = 5,
	  view_width = self.Image_content:GetContentWidth() ,
	  view_height = self.Image_content:GetContentHeight() - 5 ,
	}
	-- 创建参数
	self.__zones_scrollview = GUI.ScrollView.New(params)
	self.__zones_scrollview:SetAnchorPoint(0.5,0.5)
	self.__zones_scrollview:SetPosition(self.Image_content:GetContentWidth()/2,self.Image_content:GetContentHeight()/2)
	self.Image_content:AddChild(self.__zones_scrollview:GetNode())
end

function LogicGUI.TempTowerRank:SetShowUI( ... )	
	local lst = CallModuleFunc(ModuleType.TOWER, "GetRankLst").rank_list
	local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")

	local myRank = CPPGameLib.GetString("tower_noranking")
	for i,v in ipairs(lst) do
		v.rank = i
		if userDate.role_name == v.role_name then
			myRank = v.rank
		end
	end
	self.rank:SetString(myRank)
	self.star:SetString(CallModuleFunc(ModuleType.TOWER, "GetSumStar") or 0 )
	self.tower_history:SetPosition(self.star:GetPositionX() - self.star:GetContentWidth() - 5 , self.Text_history:GetContentHeight()/2)

	if #lst <= 0 then
		return 
	end 
	self.__zones_scrollview:SetDataList(lst)
end

--关闭按钮
function LogicGUI.TempTowerRank:OnBtnExit( ... )
	self:DeleteMe()
end

Modules = Modules or {}

Modules.TemplRankItem = Modules.TemplRankItem or BaseClass(GUI.ScrollItem)

function Modules.TemplRankItem:__init(data,direct)
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_WORLDBOSS_RANK_ITEM)
    self:__Getwidget()
end

function Modules.TemplRankItem:__delete()
end

function Modules.TemplRankItem:__Getwidget()
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
	--排行图标
	self.Image_2 = self:GetWidget("Image_2") 

end

function Modules.TemplRankItem:SetData(data)
	-- 图标
	if data.rank <= 3 then
		local imageData = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"com_sp_rank_num_"..data.rank)
		local icon = Util:Sprite(imageData)
		icon:SetAnchorPoint(0.5,0.5)
		icon:SetPosition(self.Panel_rankIcon:GetContentWidth() / 2 , self.Panel_rankIcon:GetContentHeight() / 2)
		self.Panel_rankIcon:AddChild(icon)
	else
		-- local imageData = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"com_sp_rank_num_"..4)
		-- local icon = Util:Sprite(imageData)
		-- icon:SetAnchorPoint(0.5,0.5)
		-- icon:SetPosition(self.Panel_rankIcon:GetContentWidth() / 2 , self.Panel_rankIcon:GetContentHeight() / 2)
		-- self.Panel_rankIcon:AddChild(icon)	

		local iconValue = Util:Label(data.rank,nil,Macros.Color.golden)
		self.Panel_rankIcon:AddChild(iconValue)	
		PosTool.Center(iconValue)
	end

	-- 图标
	self.icon_layer = LogicGUI.RoleIcon.New(true)
	self.icon_layer:SetTouchCallback(function()
		local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
		if data.role_id ~= userDate.role_id then
			GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.CHECKROLEINFO , false , data )
		end
	end)
	self.icon_layer:SetData(data.head_icon)
	self.Panel_icon:AddChild(self.icon_layer:GetNode())
	PosTool.Center(self.icon_layer)

    --名字
    local name = Util:Label(data.role_name,Macros.TypefaceSize.slightly,Macros.Color.headline)
    name:SetPosition(self.Text_name:GetContentWidth()/2,self.Text_name:GetContentHeight()/2)
    name:SetAnchorPoint(0,0.5)
    self.Text_name:AddChild(name)

    --公会
    local combatText = Util:Label(CPPGameLib.GetString("friend_gulidText"), Macros.TypefaceSize.slightly ,Macros.Color.keypoint)
    combatText:SetPosition(self.Text_union:GetContentWidth()/2,self.Text_union:GetContentHeight()/2)
    combatText:SetAnchorPoint(0,0.5)
    self.Text_union:AddChild(combatText)

    local guild_name = data.guild_name
    if data.guild_name == "" then
    	guild_name = CPPGameLib.GetString("weijiarugonghui")
    end

    local combat = Util:Label( guild_name , Macros.TypefaceSize.slightly , Macros.Color.content )
    PosTool.RightTo(combatText,combat)
    self.Text_union:AddChild(combat)

    --最高
    local maxHarm = Util:Label(CPPGameLib.GetString("tower_history") , Macros.TypefaceSize.slightly , Macros.Color.headline )
    maxHarm:SetPosition(self.Text_maxHarm:GetContentWidth()/2,self.Text_maxHarm:GetContentHeight()/2)
    maxHarm:SetAnchorPoint(0.5 , 0.5)
    self.Text_maxHarm:AddChild(maxHarm)

 --    local harm = Util:Label(data.context,Macros.TypefaceSize.minimum,Macros.Color.golden)
 --    PosTool.RightTo(maxHarm,harm)
 --    self.Text_maxHarm:AddChild(harm)


	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "star")
	-- --星1
	-- local star1 = Util:Sprite(sp)
	-- PosTool.RightTo(harm,star1,5)
	-- self.Text_maxHarm:AddChild(star1)

	local harm = Util:LabCenter( { 
									{ zi = data.context ,ys = Macros.Color.content , dx = Macros.TypefaceSize.slightly },
									{ tp = sp , dx = 1 , jg = 5 },
								} )
	harm:SetAnchorPoint(0.5,0.5)
	harm:SetPosition( self.Image_2:GetContentWidth()/2 , self.Image_2:GetContentHeight()/2 )
	self.Image_2:AddChild(harm)
end

function Modules.TemplRankItem:__AddToParent()

end

