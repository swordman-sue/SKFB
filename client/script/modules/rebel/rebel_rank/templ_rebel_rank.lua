--
-- @brief 叛军排行榜
-- @author: yjg
-- @date: 2016年9月23日10:09:29
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplRebelRank = LogicGUI.TemplRebelRank or BaseClass(GUI.Template)

function LogicGUI.TemplRebelRank:__init()
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}

    local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bigblue")
    self.__tab_params = {
    	{sp_2,sp_1},
    	{
			CPPGameLib.GetString("rebel_harmRank"),
			CPPGameLib.GetString("rebel_exploitRank"),
			CPPGameLib.GetString("rebel_awardRank"),
		},
		10,
		CPPGameLib.Handler(self, self.OnYeMei)
	}

	self.__open_callback = function()
		self:__Getwidget()
		self:__ShowUI()
	end
	self:__AddPreloadTemplate(GUI.TemplateRegistry.REBEL_RANK_ITEM)
	self:InitTemplate(GUI.TemplateRegistry.REBEL_RANK)	

	CallModuleFunc(ModuleType.REBEL, "RebelRankReq",Macros.Game.RankType.REBEL_HARM)
	CallModuleFunc(ModuleType.REBEL, "RebelRankReq",Macros.Game.RankType.REBEL_FEATS)
	self.rankInfo = self:BindGlobalEvent(Macros.Event.RebelModule.REBEL_RANK,function (_,data)
		self:ShowData()
	end)
end

function LogicGUI.TemplRebelRank:__delete()
	if self.lst_list then
		self.lst_list:DeleteMe()
		self.lst_list = nil
	end

	if self.lst_list2 then
		self.lst_list2:DeleteMe()
		self.lst_list2 = nil
	end

end

function LogicGUI.TemplRebelRank:__Getwidget()
	--背景
    self.Image_bg = self:GetWidget("Image_bg") 
    --关闭
    self.Button_close = Util:WidgetButton( self:GetWidget("Button_close") , nil , CPPGameLib.Handler(self,self.OnButtonClose) )
    --光
    self.Image_light = self:GetWidget("Image_light") 
    --内容框
    self.Image_lstBg = self:GetWidget("Image_lstBg") 
end	

function LogicGUI.TemplRebelRank:__ShowUI()
    --标题
	local txt = Util:Label(CPPGameLib.GetString("rebel_rank"),Macros.TypefaceSize.popup , Macros.Color.lilyWhite )
	txt:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2)
	self.Image_light:AddChild(txt)

	--我的排名
	self.myRankText = Util:Label(CPPGameLib.GetString("rank_myRank"), Macros.TypefaceSize.slightly , Macros.Color.keypoint )
	self.myRankText:SetAnchorPoint(0,0.5)
	self.myRankText:SetPosition(15, -20)
	self.Image_lstBg:AddChild(self.myRankText)

	self.myRank = Util:Label("123",Macros.TypefaceSize.slightly , Macros.Color.content )
	PosTool.RightTo(self.myRankText,self.myRank)
	self.Image_lstBg:AddChild(self.myRank)

	-- --目标排名
	-- self.heRankText = Util:Label(CPPGameLib.GetString("rank_heRank"),Macros.TypefaceSize.slightly , Macros.Color.keypoint )
	-- self.heRankText:SetAnchorPoint(0,0.5)
	-- self.heRankText:SetPosition(15, -45)
	-- self.Image_lstBg:AddChild(self.heRankText)

	-- self.heRank = Util:Label("123",Macros.TypefaceSize.slightly , Macros.Color.content)
	-- PosTool.RightTo(self.heRankText,self.heRank)
	-- self.Image_lstBg:AddChild(self.heRank)	

	--提示
	self.tips = Util:Label("A",Macros.TypefaceSize.slightly , Macros.Color.keypoint )
	self.tips:SetAnchorPoint(0.5,0.5)
	self.tips:SetVisible(false)
	self.tips:SetPosition(self.Image_lstBg:GetContentWidth()/2 + 150 , -20)
	self.Image_lstBg:AddChild(self.tips)

	-- 奖励提示
	self.awardTips = Util:Label( CPPGameLib.GetString("rebel_tips3") ,Macros.TypefaceSize.slightly , Macros.Color.keypoint )
	self.awardTips:SetPosition(self.Image_lstBg:GetContentWidth()/2 , - 20)
	self.Image_lstBg:AddChild(self.awardTips)

	--排名奖励
	local imgdata = Util:GetMoneyIconImgData(Macros.Game.RolePropType.REBEL_VALUE)
	self.rankAward = Util:LabCenter( {
									{zi = CPPGameLib.GetString("rebel_rankAward"), jg = 2 , dx = Macros.TypefaceSize.slightly , ys = Macros.Color.keypoint },
									{tp = imgdata , jg = 2 , dx = 0.75},
									{zi = "0",dx = Macros.TypefaceSize.slightly , ys = Macros.Color.content },
								} )
	self.rankAward:SetAnchorPoint(0.5,0.5)
	self.rankAward:SetVisible(false)
	self.rankAward:SetPosition(self.Image_lstBg:GetContentWidth()/2 + 150 , - 20)
	self.Image_lstBg:AddChild(self.rankAward)	

	--页签
    self.__tab:On(1)
    self.__tab:SetPosition( 160 , 450 )
    self.Image_bg:AddChild(self.__tab:GetNode())
end

--页眉回调
function LogicGUI.TemplRebelRank:OnYeMei( index )
	self.index = index
	if self.lst_list then
		self.lst_list:SetVisible(false)
	end

	if self.lst_list2 then
		self.lst_list2:SetVisible(false)
		self.sp_1:SetVisible(false)
		self.sp_2:SetVisible(false)
	end
	
	self.awardTips:SetVisible(false)

	if index == 1 or index == 2 then
		if not self.lst_list then
		    --条目
		    local params =  
		    {
		        item_class = Modules.TemplRebelRankItem,
		        item_width = 598.00,
		        item_height = 122.00,
		        row = 4,
		        col = 1,
		        item_space_r = 10,
		        view_width = self.Image_lstBg:GetContentWidth() ,
		        view_height = self.Image_lstBg:GetContentHeight() - 5 ,
		    }
			self.lst_list = GUI.ScrollView.New(params)
			self.Image_lstBg:AddChild(self.lst_list:GetNode())
			PosTool.Center(self.lst_list:GetNode())
		else
			self.lst_list:SetVisible(true)
		end

		self:ShowData()
		self:__ShowUIData(self.index)
		self:SetVisibleUi(true)
	else
		if not self.lst_list2 then
		    --条目
		    local params =  
		    {
		        item_class = Modules.TemplRebelPreviewItem,
		        item_width = 598.00,
		        item_height = 38.00,
		        row = 9,
		        col = 1,
		        view_height = self.Image_lstBg:GetContentHeight() - 46 ,
		    }
			self.lst_list2 = GUI.ScrollView.New(params)
			self.Image_lstBg:AddChild(self.lst_list2:GetNode())
			PosTool.Center(self.lst_list2:GetNode() , 0 , -23)
			self:__ShowUIData2()
		else
			self.lst_list2:SetVisible(true)
			self.sp_1:SetVisible(true)
			self.sp_2:SetVisible(true)
		end

		self.awardTips:SetVisible(true)

		self:SetVisibleUi(false)
	end
end

--排行榜
function LogicGUI.TemplRebelRank:__ShowUIData(index)
	local myLv = CPPGameLib.GetString("rebel_meimingci")
	local type = "exploit_award"
	local merit = CallModuleFunc(ModuleType.REBEL, "RebelMerit")
	local meritLv = 0
	if index == 1 then
		if merit.damage_rank ~= 0 then
			myLv = CPPGameLib.GetString("rebel_mingci" , merit.damage_rank )
			meritLv = merit.damage_rank
		end
		type = "exploit_award"
	elseif index == 2 then
		if merit.exploit_rank ~= 0 then
			myLv = CPPGameLib.GetString("rebel_mingci" , merit.exploit_rank )
			meritLv = merit.exploit_rank
		end
		type = "damage_award"
	end

	self.myRank:SetString(myLv)

	local info = {}
	local helv = " "
	local tips = " "
	local award = " "
	if myLv == CPPGameLib.GetString("rebel_meimingci") then
		info = CPPGameLib.GetConfig("RebelRankAward",#CPPGameLib.GetConfig("RebelRankAward", nil, nil , true ))
		helv = info.rank_max
		tips = CPPGameLib.GetString("rebel_mayGet",info.rank_max)
		award = info[type]

		self.tips:SetString(tips)
		self.tips:SetVisible(false)
	else
		for i = #CPPGameLib.GetConfig("RebelRankAward", nil, nil , true ), 1 , -1  do
			local config_info = CPPGameLib.GetConfig("RebelRankAward",i)
			if config_info.rank_min < tonumber(meritLv) and config_info.rank_max < tonumber(meritLv) then
				helv = config_info.rank_max
				tips = CPPGameLib.GetString("rebel_mayGet",config_info.rank_max)
				award = config_info[type]
				break
			end
		end

		if self.rankAward then
			self.rankAward:RemoveSelf(true)
			self.rankAward = nil
		end

		local imgdata = Util:GetMoneyIconImgData(Macros.Game.RolePropType.REBEL_VALUE)
		self.rankAward = Util:LabCenter( {
										{zi = CPPGameLib.GetString("rebel_rankAward"), jg = 2 , dx = Macros.TypefaceSize.slightly , ys = Macros.Color.keypoint },
										{tp = imgdata , jg = 2 , dx = 0.75},
										{zi = award,dx = Macros.TypefaceSize.slightly , ys = Macros.Color.content },
									} )
		self.rankAward:SetAnchorPoint(0.5,0.5)
		self.rankAward:SetPosition(self.Image_lstBg:GetContentWidth()/2 + 150 , -20)
		self.Image_lstBg:AddChild(self.rankAward)	
	end
end

--奖励预览
function LogicGUI.TemplRebelRank:__ShowUIData2()
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_property_tag")
	self.sp_1 = Util:Sprite9(sp)
	self.sp_1:SetContentSize(192 , 44 )
	self.sp_1:SetAnchorPoint(0,1)
	self.sp_1:SetPosition(2.5 , self.Image_lstBg:GetContentHeight() - 2.5)
	self.Image_lstBg:AddChild(self.sp_1)

	self.sp_2 = Util:Sprite9(sp)
	self.sp_2:SetContentSize(395 , 44 )
	self.sp_2:SetAnchorPoint(1,1)
	self.sp_2:SetPosition(self.Image_lstBg:GetContentWidth() - 2.5 , self.Image_lstBg:GetContentHeight() - 2.5 )
	self.Image_lstBg:AddChild(self.sp_2)

	--排名区间
	local ranking = Util:Label( CPPGameLib.GetString("rebel_section") , nil , Macros.Color.white )
	ranking:SetPosition(self.sp_1:GetContentWidth()/2 , self.sp_1:GetContentHeight()/2)
	self.sp_1:AddChild(ranking)
	PosTool.Center(ranking)

	--功勋奖励
	local rankingAward = Util:Label( CPPGameLib.GetString("rebel_exploitPreview") , nil , Macros.Color.white)
	rankingAward:SetPosition(self.sp_2:GetContentWidth()/3 - 30 , self.sp_2:GetContentHeight()/2)
	rankingAward:SetAnchorPoint(0.5,0.5)
	self.sp_2:AddChild(rankingAward)

	--伤害奖励
	local damage = Util:Label(CPPGameLib.GetString("rebel_harmPreview") , nil , Macros.Color.white)
	damage:SetPosition(self.sp_2:GetContentWidth()/1.5 + 30, self.sp_2:GetContentHeight()/2)
	damage:SetAnchorPoint(0.5,0.5)
	self.sp_2:AddChild(damage)

	local index = false
	local t1 = {}
	for i,v in ipairs(CPPGameLib.GetConfig("RebelRankAward", nil, nil , true )) do
		if index == false then
			v.show = true
			index = true
		else 
			v.show = false
			index = false
		end
		table.insert( t1, v )
	end

	if #t1 < 10 then

		for i=1 , 10 - #t1 do

			if t1[#t1].show == true then
				table.insert( t1, {show = false} )
			else
				table.insert( t1, {show = true} )
			end 
			
		end

	end


	self.lst_list2:SetDataList( t1 )
end

--显示内容
function LogicGUI.TemplRebelRank:ShowData( ... )
	local info = {}
	if self.index == 1 then
		info = CallModuleFunc(ModuleType.REBEL, "GetRebelRankHarm")
	elseif self.index == 2 then
		info = CallModuleFunc(ModuleType.REBEL, "GetRebelRankFarm")
	end

	if not info then return end
	if not self.lst_list then return end
	self.lst_list:SetDataList( info )
end

--关闭按钮
function LogicGUI.TemplRebelRank:OnButtonClose( ... )
	self:DeleteMe()
end

--隐藏奖励列表相关组件
function LogicGUI.TemplRebelRank:SetVisibleUi( bool )
	self.myRankText:SetVisible( bool )
	self.myRank:SetVisible( bool )
	self.rankAward:SetVisible( bool )
end




Modules = Modules or {}
Modules.TemplRebelPreviewItem = Modules.TemplRebelPreviewItem or BaseClass(GUI.ScrollItem)

function Modules.TemplRebelPreviewItem:__init(data,direct)
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)
end

function Modules.TemplRebelPreviewItem:__delete()
end

function Modules.TemplRebelPreviewItem:__CreateWidgetFromLuaCode(...)
	self.layout = Util:Layout(598, 38)
	self.layout:SetAnchorPoint(0.5, 0.5)
	return self.layout
end


function Modules.TemplRebelPreviewItem:SetData(data)
	local sp = nil
	if data.show == false then
		sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_lst_rankdeep")
		local sp_2 = Util:Sprite9(sp)
		sp_2:SetContentSize(self.layout:GetContentWidth() - 5 , self.layout:GetContentHeight() )
		sp_2:SetPosition(self.layout:GetContentWidth() /2 , self.layout:GetContentHeight() /2)
		self.layout:AddChild(sp_2)
	end

	if not data.rank_min then
		return 
	end

	--区间
	if data.rank_min == data.rank_max then
		local imageData = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"com_sp_rank_num_"..data.rank_min)
		local icon = Util:Sprite(imageData)
		icon:SetAnchorPoint(0.5,0.5)
		icon:SetScaleFactor(0.5)
		icon:SetPosition(192 / 2 ,self.layout:GetContentHeight()/2)
		self.layout:AddChild(icon)
	else
		local qj = CPPGameLib.GetString("rebel_mingciqujian2",data.rank_min,data.rank_max)
		local section = Util:Label(qj , Macros.TypefaceSize.normal , Macros.Color.headline )
		section:SetAnchorPoint(0.5,0.5)
		section:SetPosition(192 / 2 ,self.layout:GetContentHeight()/2)
		self.layout:AddChild(section)
	end

	local imgdata = Util:GetMoneyIconImgData(Macros.Game.RolePropType.REBEL_VALUE)
	local exploit = Util:LabCenter( {
									{tp = imgdata , dx = 0.75},
									{zi = data.exploit_award , dx = Macros.TypefaceSize.slightly , ys = Macros.Color.content ,jg = 5 },
								} )
	exploit:SetAnchorPoint(0.5,0.5)
	exploit:SetPosition(302 ,self.layout:GetContentHeight()/2)
	self.layout:AddChild(exploit)	

	local damage = Util:LabCenter( {
									{tp = imgdata , dx = 0.75},
									{zi = data.damage_award , dx = Macros.TypefaceSize.slightly , ys = Macros.Color.content ,jg = 5 },
								} )
	damage:SetAnchorPoint(0.5,0.5)
	damage:SetPosition(494 ,self.layout:GetContentHeight()/2)
	self.layout:AddChild(damage)	

end

Modules = Modules or {}

Modules.TemplRebelRankItem = Modules.TemplRebelRankItem or BaseClass(GUI.ScrollItem)

function Modules.TemplRebelRankItem:__init(index,data)
    self:InitTemplate(GUI.TemplateRegistry.REBEL_RANK_ITEM) 
    self:__Getwidget()
end

function Modules.TemplRebelRankItem:__delete()
end

function Modules.TemplRebelRankItem:SetData(data)
	CPPGameLib.PrintTable(data,"TemplRebelRankItem")
	self:__ShowUI(data)
end

function Modules.TemplRebelRankItem:SetSelected()
end

function Modules.TemplRebelRankItem:__AddToParent()
end

function Modules.TemplRebelRankItem:__Getwidget()
	--条目
	self.Image_item = self:GetWidget("Image_item") 
	--名字
	self.Text_name = self:GetWidget("Text_name") 
	--等级
	self.Text_lv = self:GetWidget("Text_lv") 
	--公会
	self.Text_union = self:GetWidget("Text_union") 
	--最高伤害Bg
	self.Image_2 = self:GetWidget("Image_2") 
	--最高伤害
	self.Text_maxHarm = self:GetWidget("Text_maxHarm") 
	--图标
	self.Panel_icon = self:GetWidget("Panel_icon") 
	--排行图标
	self.Panel_rankIcon = self:GetWidget("Panel_rankIcon") 
end

function Modules.TemplRebelRankItem:__ShowUI(data)
	if data.rank <= 3 then
		local imageData = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"com_sp_rank_num_"..data.rank)
		local icon = Util:Sprite(imageData)
		icon:SetAnchorPoint(0.5,0.5)
		icon:SetPosition(self.Panel_rankIcon:GetContentWidth() / 2 , self.Panel_rankIcon:GetContentHeight() / 2)
		self.Panel_rankIcon:AddChild(icon)
	else
		local iconValue = Util:Label( data.rank , Macros.TypefaceSize.normal , Macros.Color.headline )
		iconValue:SetAnchorPoint(0.5,0.5)
		iconValue:SetPosition(self.Panel_rankIcon:GetContentWidth() / 2 , self.Panel_rankIcon:GetContentHeight() / 2)
		self.Panel_rankIcon:AddChild(iconValue)	
	end

	-- 图标
	self.icon_layer = LogicGUI.RoleIcon.New(true)
	self.icon_layer:SetTouchCallback(function()
			-- CallModuleFunc(ModuleType.CHECKROLEINFO, "OpenView", BaseViewType.CHECKROLEINFO,data)
			GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.CHECKROLEINFO , false , data )
	    end)
	self.icon_layer:SetData(data.head_icon)
	self.Panel_icon:AddChild(self.icon_layer:GetNode())
	PosTool.Center(self.icon_layer)

    --名字
    local name = Util:Label(data.role_name , Macros.TypefaceSize.normal , Macros.Color.headline)
    name:SetPosition(self.Text_name:GetContentWidth()/2,self.Text_name:GetContentHeight()/2)
    name:SetAnchorPoint(0,0.5)
    self.Text_name:AddChild(name)

    --等级
	local lvText = Util:Label(CPPGameLib.GetString("common_lev1") , Macros.TypefaceSize.normal , Macros.Color.keypoint)
	lvText:SetPosition(self.Text_lv:GetContentWidth()/2,self.Text_lv:GetContentHeight()/2)
	lvText:SetAnchorPoint(0,0.5)
	self.Text_lv:AddChild(lvText)

    local lv = Util:Label(data.level,Macros.TypefaceSize.normal , Macros.Color.content )
    PosTool.RightTo(lvText,lv)
    self.Text_lv:AddChild(lv)	

    --战斗力
    local combatText = Util:Label(CPPGameLib.GetString("friend_battleText"),Macros.TypefaceSize.normal,Macros.Color.keypoint)
    combatText:SetPosition(self.Text_union:GetContentWidth()/2,self.Text_union:GetContentHeight()/2)
    combatText:SetAnchorPoint(0,0.5)
    self.Text_union:AddChild(combatText)

    local combat = Util:Label(data.battle_value,Macros.TypefaceSize.normal , Macros.Color.content )
    PosTool.RightTo(combatText,combat)
    self.Text_union:AddChild(combat)


    local txt = " "
    if data.type == Macros.Game.RankType.REBEL_HARM then
    	txt = CPPGameLib.GetString("rebel_harm")
    elseif data.type == Macros.Game.RankType.REBEL_FEATS then
    	txt = CPPGameLib.GetString("rebel_exploit")
    end
	
    --属性1
    local nature1 = Util:Label(txt,nil, Macros.Color.keypoint )
    nature1:SetPosition(self.Text_maxHarm:GetContentHeight()/2 , self.Text_maxHarm:GetContentHeight()/2 - 10 )
	nature1:SetAnchorPoint( 0.5 , 0 )
    self.Text_maxHarm:AddChild(nature1)

    CPPGameLib.PrintTable("xy" ,  data ,"物品合成响应")  
    --属性1
    local natureValue1 = Util:Label(data.context,Macros.TypefaceSize.normal , Macros.Color.content)
    natureValue1:SetPosition(self.Image_2:GetContentWidth()/2 , self.Image_2:GetContentHeight()/2 )
	natureValue1:SetAnchorPoint( 0.5 , 0.5 )
    self.Image_2:AddChild(natureValue1)
end
