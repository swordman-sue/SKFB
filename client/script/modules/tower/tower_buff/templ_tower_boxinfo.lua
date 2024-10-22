
--
-- @brief 奖励获得
-- @author: yjg
-- @date: 2016年9月27日11:44:59
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplTowerBoxinfo = LogicGUI.TemplTowerBoxinfo or BaseClass(GUI.Template)

function LogicGUI.TemplTowerBoxinfo:__init(fun)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__open_callback = function()
		self.fun = fun
		self:__Getwidget()
		self:__ShowUI()
	end
	self:InitTemplate(GUI.TemplateRegistry.DUNGEON_BOXINFO)	
end


function LogicGUI.TemplTowerBoxinfo:__delete()

end


function LogicGUI.TemplTowerBoxinfo:SetData(data)
	print("////")
end

function LogicGUI.TemplTowerBoxinfo:__Getwidget( ... )
	--居中层
    self.Image_3 = self:GetWidget("Image_3") 
    --关闭按钮
    self.Button_Close = Util:WidgetButton(self:GetWidget("Button_Close"), nil , CPPGameLib.Handler( self,self.OnBtnExit))  
    --关闭按钮
    self.Button_Receive = Util:WidgetButton(self:GetWidget("Button_Receive"), CPPGameLib.GetString("manor_get") , CPPGameLib.Handler( self,self.OnBtnExit))  

    --光
    self.Image_4_0 = self:GetWidget("Image_4_0") 
    --提示
    self.Text_GameLevelCondition = self:GetWidget("Text_GameLevelCondition") 
    self.Text_GameLevelCondition:SetText(" ")
    --内容框
    self.Image_6_0 = self:GetWidget("Image_6_0") 

    --标题n

    self.Text_Title = self:GetWidget("Text_Title") 
    self.Text_Title:SetVisible(false)
    --提示容器
    self.Panel_StarCondition = self:GetWidget("Panel_StarCondition") 
    self.Image_6 = self:GetWidget("Image_6") 
    self.Text_StarCondition = self:GetWidget("Text_StarCondition") 
    self.Panel_StarCondition:SetVisible(false)
    --滚动容器
    self.ScrollView_Rewards = self:GetWidget("ScrollView_Rewards") 
    self.ScrollView_Rewards:SetVisible(false)
    --按钮字样
    self.Image_AlreadyReceived = self:GetWidget("Image_AlreadyReceived") 
    self.Image_AlreadyReceived:SetVisible(false)

end

function LogicGUI.TemplTowerBoxinfo:__ShowUI( ... )	
	--标题
	local headline = Util:Label( CPPGameLib.GetString("tower_congratsGetAward") , Macros.TypefaceSize.largish , Macros.Color.headline )
	headline:SetShadow(true)
	headline:SetShadowColor(0, 0, 0, 255)
	headline:SetAnchorPoint(0.5,0.5)
	headline:SetPosition(self.Image_4_0:GetContentWidth()/2 , self.Image_4_0:GetContentHeight()/2 )
	self.Image_4_0:AddChild(headline)

	-- --提示
	-- local plunderedtreasures_ChooseFinshTextOne = Util:Label( CPPGameLib.GetString("plunderedtreasures_ChooseFinshTextOne") ,Macros.TypefaceSize.minimum )
	-- plunderedtreasures_ChooseFinshTextOne:SetPosition(self.Text_GameLevelCondition:GetContentWidth()/2 , self.Text_GameLevelCondition:GetContentHeight()/2)
	-- plunderedtreasures_ChooseFinshTextOne:SetAnchorPoint(0.5,0.5)
	-- self.Text_GameLevelCondition:AddChild(plunderedtreasures_ChooseFinshTextOne)	


	local z,x = CallModuleFunc(ModuleType.TOWER , "GetNowHoldStar")
	--消耗
	local curve = Util:LabCenter(  {
									{zi = CPPGameLib.GetString("tower_starGetSum") , dx = Macros.TypefaceSize.minimum ,ys = Macros.Color.keypoint} , 
									{zi = x , dx = Macros.TypefaceSize.minimum , jg = 5 , ys = Macros.Color.lilyWhite } ,
									{tp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "star") , dx = 1 , jg = 5} ,
									{zi = CPPGameLib.GetString("tower_ChooseFinshTextOne") , dx = Macros.TypefaceSize.minimum ,ys = Macros.Color.keypoint} , 
								} )
	curve:SetAnchorPoint(0.5,0.5)
	curve:SetPosition(self.Text_GameLevelCondition:GetContentWidth()/2 , self.Text_GameLevelCondition:GetContentHeight()/2)
	self.Text_GameLevelCondition:AddChild(curve)


	local lv = CallModuleFunc(ModuleType.TOWER , "GetCurMapLevel")
	local chapter = CPPGameLib.GetConfig("TowerGameLevel",lv).chapter
	local chapter = CPPGameLib.GetConfig("TowerChapter",chapter)
	local t1 = {}
	local lst = {}
	local index = 0
	for i,v in ipairs(chapter.star_award_id_list) do
		for j,vv in ipairs(v) do
			index = index + 1
			table.insert( t1 , {id = vv , num = chapter.star_award_num_list[i][j] , star = chapter.star_list[i] })
			if j == #v then
				table.insert(lst,t1)
				t1 = {}
				index = 0
			end
		end
	end
	if #t1 ~= 0 then
		table.insert(lst,t1)
	end
	
	local z , x , t , s = CallModuleFunc(ModuleType.TOWER , "GetNowHoldStar")
	local t2 = {}
	for i,v in ipairs(lst) do
		if v[1].star <= x then
			table.insert(t2,v)
		end
	end

	for i,v in ipairs(t2[#t2]) do
		local icon = LogicGUI.IconLayer.New(true)
		local item = CPPGameLib.GetConfig("Item", v.id)
		icon:SetData({id = v.id , obj_info = item , num = v.num})
		icon:SetPosition( 60 +(i-1) * 95 , self.Image_6_0:GetContentHeight()/2 )
		icon:SetAnchorPoint(0.5,0.5)
		icon:ShowItemName(true)
		self.Image_6_0:AddChild(icon:GetNode())
	end
end

--关闭
function LogicGUI.TemplTowerBoxinfo:OnBtnExit( ... )
	self.fun()
	self:DeleteMe()
end
