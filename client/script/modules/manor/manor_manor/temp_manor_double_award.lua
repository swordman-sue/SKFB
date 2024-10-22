
--[[
%% @module: 领地双倍奖励
%% @author: yjg
%% @created: 2016年10月8日20:29:00
--]]

Modules = Modules or {}

Modules.TempManorDoubleAward = Modules.TempManorDoubleAward or BaseClass(GUI.Template)

function Modules.TempManorDoubleAward:__init(lst1 , lst2 , fun)
	self.__is_auto_load = true
		
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function()
		self.fun = fun
		self:__Getwidget()
		self:__ShowUI( lst1 , lst2 )
	end
	self:InitTemplate(GUI.TemplateRegistry.MANOR_DOUBEL_AWARD)	
end

function Modules.TempManorDoubleAward:__delete()
	if self.scroll_Lst1 then
		self.scroll_Lst1:DeleteMe()
		self.scroll_Lst1 = nil
	end
end

function Modules.TempManorDoubleAward:__Getwidget()
	--背景
    self.Image_bg = self:GetWidget("Image_bg") 
    --光
    self.Image_light = self:GetWidget("Image_light") 
    --关闭
    self.Button_close = Util:WidgetButton( self:GetWidget("Button_close") ,nil,CPPGameLib.Handler(self,self.OnBtnClose) )
    --内容框1
    self.Image_content1 = self:GetWidget("Image_content1") 
    --标题1
    self.Text_headline1 = self:GetWidget("Text_headline1") 
    --内容框2
    self.Image_content2 = self:GetWidget("Image_content2") 
    --标题2
    self.Text_headline2 = self:GetWidget("Text_headline2") 
    --确定按钮
    self.Button_confirm = Util:WidgetButton( 
    											self:GetWidget("Button_confirm") ,
    											CPPGameLib.GetString("common_confirm"),
    											CPPGameLib.Handler(self,self.OnButtonConfirm),
    											nil,nil,Macros.Color.button_yellow
    											)
end

function Modules.TempManorDoubleAward:__ShowUI( lst1 , lst2  )
	--标题
	local manor_patrolEnd = Util:Label( CPPGameLib.GetString("manor_patrolEnd") , Macros.TypefaceSize.largish , Macros.Color.white )
	manor_patrolEnd:SetAnchorPoint(0.5,0.5)
	manor_patrolEnd:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2 )
	self.Image_light:AddChild(manor_patrolEnd)

	--提示
	local manor_patrolEndTips = Util:Label( CPPGameLib.GetString("manor_patrolEndTips") , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
	manor_patrolEndTips:SetPosition(self.Text_headline1:GetContentWidth()/2 , self.Text_headline1:GetContentHeight()/2 )
	manor_patrolEndTips:SetAnchorPoint(0,0.5)
	self.Text_headline1:AddChild(manor_patrolEndTips)

	--提示2
	local manor_patrolGetTips2 = Util:Label( CPPGameLib.GetString("manor_patrolGetTips2") , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
	manor_patrolGetTips2:SetPosition(self.Text_headline2:GetContentWidth()/2 , self.Text_headline2:GetContentHeight()/2 )
	manor_patrolGetTips2:SetAnchorPoint(0,0.5)
	self.Text_headline2:AddChild(manor_patrolGetTips2)

   	local common =  
    {
        item_class = Modules.TempManorAwardItem,
        item_width = 90,
        item_height = 110,
        row = 2,
        col = 4,
        item_space_c = 15,

        -- view_width = self.Image_content1:GetContentWidth() - 10 ,
        view_height = self.Image_content1:GetContentHeight() - 10
    }

    -- 普通列表
    self.scroll_Lst1 = GUI.ScrollView.New(common)
    self.scroll_Lst1:SetDataList( lst1 )
    self.Image_content1:AddChild(self.scroll_Lst1:GetNode())
    PosTool.Center(self.scroll_Lst1:GetNode())



	local t1 = {}
	for i=1,5 do
		for i,v in ipairs(lst1) do
			table.insert( t1 , {id = v.id , num = v.num} )
		end
	end

   	local common =  
    {
        item_class = Modules.TempManorAwardItem,
        item_width = 90,
        item_height = 110,
        row = 2,
        col = 4,
        item_space_c = 15,

        -- view_width = self.Image_content2:GetContentWidth() - 10 ,
        view_height = self.Image_content2:GetContentHeight() - 10
    }

    -- 普通列表
    local scroll_Lst2 = GUI.ScrollView.New(common)
    scroll_Lst2:SetDataList( lst2 )
    self.Image_content2:AddChild(scroll_Lst2:GetNode())
    PosTool.Center(scroll_Lst2:GetNode())

end

function Modules.TempManorDoubleAward:OnBtnClose( )
	self:DeleteMe()
end

function Modules.TempManorDoubleAward:OnButtonConfirm( )
	self.fun()
	self:DeleteMe()
end




--[[
%% @module: 领地单倍奖励
%% @author: yjg
%% @created: 2016年10月8日20:29:00
--]]



Modules = Modules or {}

Modules.TempManorAward = Modules.TempManorAward or BaseClass(GUI.Template)

function Modules.TempManorAward:__init(lst , fun)
	self.__is_auto_load = true

	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function()
		self.fun = fun
		self:__Getwidget()
		self:__ShowUI(lst)
	end
	self:InitTemplate(GUI.TemplateRegistry.MANOR_AWARD)	
end

function Modules.TempManorAward:__delete()
end

function Modules.TempManorAward:__Getwidget()
	--背景
    self.img_selectBG = self:GetWidget("img_selectBG") 
    --关闭
    self.btn_close = Util:WidgetButton( self:GetWidget("btn_close") ,nil,CPPGameLib.Handler(self,self.OnBtnClose) )
    --确定
    self.btn_ok = Util:WidgetButton( 
    									self:GetWidget("btn_ok") , 
    									CPPGameLib.GetString("common_confirm") ,
    									CPPGameLib.Handler(self,self.OnButtonConfirm) ,
    									nil,nil,Macros.Color.button_yellow)
    --光
    self.img_headline = self:GetWidget("img_headline") 
    --内容
    self.img_dataBG = self:GetWidget("img_dataBG") 
	--提示
    self.Text_tips = self:GetWidget("Text_tips") 
end

function Modules.TempManorAward:__ShowUI( lst )
	--标题
	local manor_patrolEnd = Util:Label( CPPGameLib.GetString("manor_patrolEnd") , Macros.TypefaceSize.popup , Macros.Color.lilyWhite )
	manor_patrolEnd:SetAnchorPoint(0.5,0.5)
	manor_patrolEnd:SetPosition(self.img_headline:GetContentWidth()/2 , self.img_headline:GetContentHeight()/2 )
	self.img_headline:AddChild(manor_patrolEnd)

	--提示
	local manor_patrolEndTips = Util:Label( CPPGameLib.GetString("manor_patrolEndTips") , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
	manor_patrolEndTips:SetPosition(self.Text_tips:GetContentWidth()/2 , self.Text_tips:GetContentHeight()/2 )
	manor_patrolEndTips:SetAnchorPoint(0,0.5)
	self.Text_tips:AddChild(manor_patrolEndTips)

   	local common =  
    {
        item_class = Modules.TempManorAwardItem,
        item_width = 93.75,
        item_height = 110,
        row = 4,
        col = 4,
        item_space_c = 5,
        view_height = self.img_dataBG:GetContentHeight() - 10 
    }

    -- 普通列表
    self.common = GUI.ScrollView.New(common)
    self.common:SetDataList( lst )
    self.img_dataBG:AddChild(self.common:GetNode())
    PosTool.Center(self.common:GetNode())
end

function Modules.TempManorAward:OnBtnClose( )
	self:DeleteMe()
end

function Modules.TempManorAward:OnButtonConfirm( )
	self.fun()
	self:DeleteMe()
end


--[[
%% @module: 可能获得道具预览
%% @author: yjg
%% @created: 2016年10月11日10:59:36
--]]

Modules = Modules or {}

Modules.TempManorAwardItem = Modules.TempManorAwardItem or BaseClass(GUI.ScrollItem)

function Modules.TempManorAwardItem:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)
end

function Modules.TempManorAwardItem:__CreateWidgetFromLuaCode()
	self.layout = Util:Layout()
	self.layout:SetAnchorPoint(0.5, 0.5)
	self.layout:SetContentSize(90, 110)
	return self.layout
end

function Modules.TempManorAwardItem:__delete()
end

function Modules.TempManorAwardItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)

	self.icon = LogicGUI.IconLayer.New(true)
	self.icon:SetPosition(self.layout:GetContentWidth()/2 ,self.layout:GetContentHeight()/2 + 10 )
	-- self.icon:SetScaleFactor(0.9)
	self.icon:SetTouchSwallow(false)
	self.layout:AddChild(self.icon:GetNode())


	local num = data.num
	if data.double == 1 then
		num = data.num * 2
	end

	local iconData = { id = data.id ,num = num }
	self.icon:SetData(iconData)

	local type_ , cnf = ObjIDTool.GetInfo(data.id)

    self.__lab_item_name = Util:Label( cnf.name , Macros.TypefaceSize.slightly, Macros.Color.keypoint, self:GetWidth()+30)
    self.__lab_item_name:SetAnchorPoint(0.5, 1)
    self.__lab_item_name:SetTextHorizontalAlign(LabelEx.ALIGIN_CENTER)
    self.icon:AddChild(self.__lab_item_name)
    PosTool.CenterBottom(self.__lab_item_name, 0, -8)   

end

function Modules.TempManorAwardItem:__AddToParent()
end














































-- --------------------------------------------------------------------------------------------------------------------------
-- --挑战层
-- --------------------------------------------------------------------------------------------------------------------------
-- function Modules.TempManorChallenge:__ShowUI_1(data)

-- 	self.node_scene1:SetVisible(true)
-- 	--显示那什么
-- 	local city = data[1]
-- 	local hero = CPPGameLib.GetConfig("Hero", data[2].info_id)
-- 	local heroDebris = CPPGameLib.GetConfig("HeroCompound", data[2].info_id)

-- 	--标题
-- 	local biaoti = Util:Label(city.name , Macros.TypefaceSize.popup , Macros.Color.headline) 
-- 	biaoti:SetPosition(self.img_biaotiBg:GetContentWidth()/2 , self.img_biaotiBg:GetContentHeight() / 2 )
-- 	self.img_biaotiBg:AddChild(biaoti)

--     --英雄模型
--     local heroModes = LogicGUI.Model.New({info_id = hero.model_id , parent = self.img_fazheng})
--     heroModes:SetPosition(self.img_fazheng:GetContentWidth()/2 , 50)
--     heroModes:SetScaleFactor(1.2)

-- 	--奖励列表
--     self.lst_list_1 = UIListViewEx.Create()
--     self.lst_list_1:SetAnchorPoint(0,0)
--     self.lst_list_1:SetContentSize(372.00,138.00)
--     self.lst_list_1:SetPosition(2.5,2.5)
--     self.lst_list_1:SetDirection(2)
--     self.img_huodeBg:AddChild(self.lst_list_1)
--     local pack = CallModuleFunc(ModuleType.MANOR, "GetCityPack",city.id)

--     --添加巡逻英雄碎片
--     if heroDebris then
--     	local info = CPPGameLib.GetConfig("Item", heroDebris.item_id)
-- 		local data = {}
-- 		data.object_id = heroDebris.item_id
-- 		data.data = info
-- 		table.insert(pack,1,data)
--     end
--     self:GetPackLst(pack)

--     --整个模式的表
-- 	self.choiceLst = CallModuleFunc(ModuleType.MANOR, "GetPattern")
-- 	--当前选中的模式
-- 	self.pattern = self.choiceLst[1]

-- 	--选择框文字
-- 	local CheckBoxTips = {}
	

-- 	--选择框回调
-- 	local function OnCheckBox( index )
-- 		for j=1,10 do
-- 			if self["CheckBox_"..j] then
-- 				self["CheckBox_"..j]:SetSelectedState(false)
-- 			else
-- 				break
-- 			end
-- 		end
-- 		self["CheckBox_"..index]:SetSelectedState(true)
-- 		self.ontime = self.pattern[index]
-- 		self.time_type = index
-- 		self.hero = data[2]
-- 		self.city = city

-- 		if self.consume then
-- 			self.consume:RemoveSelf(true)
-- 		end

-- 		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON, "energy")
-- 		local show = self.ontime.vigor
-- 		if self.ontime.vigor == 0 then
-- 			sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON, "diamond")
-- 			show = self.ontime.jewel
-- 		end

-- 		self.consume = Util:LabCenter( {{tp = sp , dx = 0.65},
-- 									   {zi = show}})
-- 		self.consume:SetPosition(self.btn_kaishi:GetContentWidth()/2 ,self.btn_kaishi:GetContentHeight()/2 + 40)
-- 		self.consume:SetAnchorPoint(0.5,0.5)
-- 		self.btn_kaishi:AddChild(self.consume)
-- 	end

-- 	--选择框
-- 	for i=1,10 do
-- 		if self:GetWidget("CheckBox_"..i) then
-- 			self["CheckBox_"..i] = self:GetWidget("CheckBox_"..i)
-- 			CheckBoxTips[i] = Util:Label( CPPGameLib.GetString("manor_patrolTime" , self.pattern[i].time ) )
-- 			PosTool.RightTo(self["CheckBox_"..i] , CheckBoxTips[i])
-- 			self.node_scene1:AddChild(CheckBoxTips[i])
-- 			WidgetUnity.SetCheckboxListener(self["CheckBox_"..i], 
-- 								function()
-- 									OnCheckBox(i)
-- 								end,
-- 								function()
-- 									OnCheckBox(i)
-- 								end)
-- 		else
-- 			break
-- 		end
-- 	end

-- 	OnCheckBox(1)
	
--     --三角按钮的背景图
--     local sp = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.MANOR, "ld_an"))
--     sp:SetPosition(self.Image_1:GetContentWidth() + 24 , self.Image_1:GetContentHeight()/2 )
--     self.Image_1:AddChild(sp)

--     --下拉菜单
--     local ld_sj = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.MANOR, "ld_sj")
--     local menu = LogicGUI.Menu.New( ld_sj,
--     								{ CPPGameLib.GetString("manor_common"),CPPGameLib.GetString("manor_intermediate"),CPPGameLib.GetString("manor_advanced")},
--     								{self.Image_1:GetContentWidth(),self.Image_1:GetContentHeight()},
--     								function (index,data)
--     									self.pattern = self.choiceLst[index]
--     									for i=1,#self.pattern do
--     										CheckBoxTips[i]:SetString(CPPGameLib.GetString("manor_patrolTime" , self.pattern[i].time) )
--     									end
--     									OnCheckBox(1)
-- 								    end)
--     local dialogueBG = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.MANOR, "ld_wzk")
--     menu:SetItemSp(dialogueBG,10/44,10/44,10/44,10/44,113,36)
--     menu:SetMenuSize(self.Image_1:GetContentWidth(),self.Image_1:GetContentHeight() * 3 )
-- 	menu:SetPosition(0,0)
-- 	self.Image_1:AddChild(menu:GetNode())
-- end

-- --可获得的物品列表
-- function Modules.TempManorChallenge:GetPackLst( data )
-- 	--标题
-- 	local name = Util:Label(CPPGameLib.GetString("manor_possibleGet") , Macros.TypefaceSize.slightly , Macros.Color.content) 
-- 	name:SetAnchorPoint(0,1)
-- 	name:SetPosition(5,self.img_huodeBg:GetContentHeight()-5)
-- 	self.img_huodeBg:AddChild(name)

--     for i,v in ipairs(data) do
-- 		local layou = Util:Layout()
-- 	    layou:SetAnchorPoint(0.5,0.5)
-- 	    layou:SetContentSize(100,100)
-- 	    layou:SetTouchEnabled(true)

-- 		local icon = LogicGUI.IconLayer.New(true)
-- 		icon:SetPosition(layou:GetContentWidth()/2 ,layou:GetContentHeight()/2)
-- 		icon:SetScaleFactor(0.9)
-- 		layou:AddChild(icon:GetNode())
-- 		local iconData = { id = v.object_id ,obj_info = v.data }
-- 		icon:SetData(iconData)
-- 		icon:ShowItemName(true)
-- 		icon:ShowHeroName(true)

-- 	    self.lst_list_1:PushBackCustomItem(layou)
--     end
-- end

-- --开始巡逻按钮
-- function Modules.TempManorChallenge:OnBtnKaishi( ... )
-- 	local info = {}
-- 	info.city_id = self.city.id
-- 	info.patrol_type = self.ontime.least
-- 	info.time_type = self.time_type
-- 	info.patrol_hero_uid = self.hero.uid
-- 	CallModuleFunc(ModuleType.MANOR, "LandPatrolReq",info)
-- 	self:OnBtnJieshu()
-- end

-- --------------------------------------------------------------------------------------------------------------------------
-- --选择层
-- --------------------------------------------------------------------------------------------------------------------------
-- function Modules.TempManorChallenge:__ShowUI_2()
-- 	self.node_scene2:SetVisible(true)
-- 	--添加提示
-- 	local addTips = Util:Label( CPPGameLib.GetString("manor_addPatrolTips") )
-- 	addTips:SetPosition(self.btn_add:GetContentWidth() / 2 ,  -25 )
-- 	self.btn_add:AddChild(addTips)

-- 	--标题
-- 	local biaoti = Util:Label(self.data.name , Macros.TypefaceSize.popup , Macros.Color.headline) 
-- 	biaoti:SetPosition(self.img_biaotiBg:GetContentWidth()/2 , self.img_biaotiBg:GetContentHeight() / 2 )
-- 	self.img_biaotiBg:AddChild(biaoti)

--     self.lst_list_2 = UIListViewEx.Create()
--     self.lst_list_2:SetAnchorPoint(0,0)
--     self.lst_list_2:SetContentSize(372.00,330.00)
--     self.lst_list_2:SetPosition(2.5,2.5)
--     self.img_huodeBg2:AddChild(self.lst_list_2)

-- 	local pack,hero = CallModuleFunc(ModuleType.MANOR, "GetCityPack",self.data.id,true)
--     self:GetPack(hero)
--     self:GetPack(pack)
-- end

-- --获得的英雄
-- function Modules.TempManorChallenge:GetPack(data)
-- 	--名字
-- 	local tab = ""
-- 	local lst = {}
-- 	for i,v in ipairs(data) do
-- 		if v.name then
-- 			tab = tab .. v.name
-- 			if i ~= #data then
-- 				tab = tab .. "、"
-- 			end
-- 		end 
-- 		if lst[ math.ceil(i/4) ] then
-- 			table.insert(lst[ math.ceil(i/4) ] , v )
-- 		else
-- 			lst[ math.ceil(i/4) ] = {}
-- 			table.insert(lst[ math.ceil(i/4) ] , v )
-- 		end
-- 	end

-- 	--标题
-- 	local layou = Util:Layout()
--     layou:SetAnchorPoint(0.5,0.5)
--     layou:SetTouchEnabled(true)

--     local name = Util:Label(tab .. CPPGameLib.GetString("manor_patrolGetTips") , Macros.TypefaceSize.slightly , Macros.Color.content , 372 )
--     name:SetAnchorPoint(0,0.5)
--     layou:AddChild(name)

--     layou:SetContentSize(372,name:GetContentHeight())
--     name:SetPosition(0,layou:GetContentHeight()/2)

-- 	self.lst_list_2:PushBackCustomItem(layou)

-- 	--显示图标
-- 	for _,v in ipairs(lst) do
-- 		--获得图标大小
-- 		local icon = LogicGUI.IconLayer.New(false)
-- 		icon:SetData( { id = data[1].object_id ,obj_info = data[1].data }) 

-- 		local interval = icon:GetNode():GetContentWidth() + 3
-- 		local x = icon:GetNode():GetContentWidth() / 2 + 5

-- 		local layou = Util:Layout()
-- 	    layou:SetContentSize(372,icon:GetNode():GetContentHeight() + 25)
-- 	    layou:SetAnchorPoint(0.5,0.5)
-- 	    layou:SetTouchEnabled(true)	

-- 	    for __,vv in ipairs(v) do
-- 		    local icon = LogicGUI.IconLayer.New(true)
-- 		    icon:SetPosition(x ,layou:GetContentHeight()/2 + 5)
-- 		    icon:SetScaleFactor(0.9)
-- 		    layou:AddChild(icon:GetNode())
-- 		    local iconData = { id = vv.object_id ,obj_info = vv.data }
-- 		    icon:SetData(iconData)
-- 		    icon:ShowItemName(true)
-- 		    icon:ShowHeroName(true)
-- 		    x = x + interval
-- 	    end
-- 	    self.lst_list_2:PushBackCustomItem(layou)
-- 	end
-- end

-- --添加按钮
-- function Modules.TempManorChallenge:OnBtnAdd( ... )
-- 	CallModuleFunc(ModuleType.MANOR, "OpenView", BaseViewType.MANORHEROLST,self.data,"xunluo")
-- 	self:OnBtnJieshu()
-- end