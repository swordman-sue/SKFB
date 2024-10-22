
--
-- @brief 爬塔奖励
-- @author: yjg
-- @date:2016年9月26日20:45:35
--

LogicGUI = LogicGUI or {}

LogicGUI.TempTowerAward = LogicGUI.TempTowerAward or BaseClass(GUI.Template)


function LogicGUI.TempTowerAward:__init()
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__open_callback = function()
		self:__Getwidget()
		self:__ShowUI()
		self:SetShowUI()
	end
	self:InitTemplate(GUI.TemplateRegistry.TOWER_AWARD)	
end

function LogicGUI.TempTowerAward:__delete()
	if self.__shield_layer then
		self.__shield_layer:DeleteMe()
		self.__shield_layer = nil
	end
end

function LogicGUI.TempTowerAward:SetData(data)
end

function LogicGUI.TempTowerAward:__Getwidget( ... )
	--背景层
    self.Image_bg = self:GetWidget("Image_bg")
    --光 
    self.Image_light = self:GetWidget("Image_light") 
    --内容背景
    self.Image_content = self:GetWidget("Image_content") 

    self.Image_award = {}
    self.Image_content_ = {}
    for i=1,3 do
    	self.Image_award[i] = self:GetWidget("Image_award"..i) 
    	self.Image_content_[i] = self:GetWidget("Image_content"..i) 
    end

    --持有
    self.Text_hold = self:GetWidget("Text_hold") 
    --提示
    self.Panel_hint = self:GetWidget("Panel_hint") 
    --关闭
    self.Button_close = Util:WidgetButton(self:GetWidget("Button_close"), " " , CPPGameLib.Handler( self,self.OnBtnExit ) )  
end

function LogicGUI.TempTowerAward:__ShowUI( ... )	
	--标题
	local headline = Util:Label( CPPGameLib.GetString("tower_previewAward") , Macros.TypefaceSize.popup , Macros.Color.lilyWhite )
	headline:SetAnchorPoint(0.5,0.5)
	headline:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2 )
	self.Image_light:AddChild(headline)

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "star")

    local curve = Util:LabCenter(  {
    									{zi = CPPGameLib.GetString("tower_nowStar" , CallModuleFunc(ModuleType.TOWER , "GetCurStar") ) , dx = Macros.TypefaceSize.normal , ys = Macros.Color.content } , 
    									{tp = sp , dx = 1 } } )
    curve:SetPosition(self.Text_hold:GetContentWidth()/2 , self.Text_hold:GetContentWidth()/2 + 2)
    curve:SetAnchorPoint(0.5,0.5)
    self.Text_hold:AddChild(curve)	

 	local tab = {}
	local lst = CallModuleFunc(ModuleType.TOWER , "TowerMonsterStar")
	tab = { {zi = lst[1].game_level .. " - " .. lst[#lst].game_level , dx = Macros.TypefaceSize.normal , ys = Macros.Color.darkOrange } }

    local nature = Util:Echo( CPPGameLib.GetString("tower_nowStarTips") , tab , Macros.Color.content , Macros.TypefaceSize.normal )
    nature:SetPosition(self.Panel_hint:GetContentWidth()/2 , self.Panel_hint:GetContentHeight()/2)
    nature:SetAnchorPoint(0.5,0.5)
    self.Panel_hint:AddChild(nature)	

end

function LogicGUI.TempTowerAward:SetShowUI( ... )	
	local lst = CallModuleFunc(ModuleType.TOWER , "GetRewardPreview")

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "star")
	for i,v in ipairs(lst) do
	    local curve = Util:LabCenter(  {
	    									{zi = CPPGameLib.GetString("tower_reach") .. " " .. v[1].star ,dx = Macros.TypefaceSize.normal , ys = Macros.Color.keypoint } , 
	    									{tp = sp , dx = 1 ,jg = 2} } )
	    curve:SetPosition(self.Image_award[i]:GetContentWidth()/2 - 120   , self.Image_award[i]:GetContentHeight()/2 )
	    curve:SetAnchorPoint(0.5,0.5)
	    self.Image_award[i]:AddChild(curve)	

		local interval = self.Image_content_[i]:GetContentWidth() / #v

		for x , vv in ipairs(v) do
			local icon = LogicGUI.IconLayer.New(true)
			local type_ , item = ObjIDTool.GetInfo(vv.id)
			icon:SetData({id = vv.id , obj_info = item , num = vv.num})
			icon:SetAnchorPoint(0.5,0.5)

			local jg = self.Image_content_[i]:GetContentWidth() - ( icon:GetNode():GetContentWidth() * 2 )
			icon:SetPosition( (jg/4) + (icon:GetNode():GetContentWidth()/2 ) + interval * (x - 1)  , self.Image_content_[i]:GetContentHeight()/2 )
			
			self.Image_content_[i]:AddChild(icon:GetNode())
		end
	end

end

--关闭按钮
function LogicGUI.TempTowerAward:OnBtnExit( ... )
	self:DeleteMe()
end






--
-- @brief 通章奖励
-- @author: yjg
-- @date:2016年9月26日20:45:35
--

LogicGUI = LogicGUI or {}

LogicGUI.TempTowerSection = LogicGUI.TempTowerSection or BaseClass(GUI.Template)


function LogicGUI.TempTowerSection:__init( fun )
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__open_callback = function()
		self.fun = fun
		self:__Getwidget()
		self:__ShowUI()
		self:SetShowUI()
	end
	self:InitTemplate(GUI.TemplateRegistry.TOWER_TREASURE)	
end

function LogicGUI.TempTowerSection:__delete()

end

function LogicGUI.TempTowerSection:SetData(data)

end

function LogicGUI.TempTowerSection:__Getwidget( ... )
	--背景层
    self.Image_bg = self:GetWidget("Image_bg")
    --光 
    self.Image_light = self:GetWidget("Image_light") 
    --内容背景
    self.Image_content = self:GetWidget("Image_content") 

    self.Text_tips = self:GetWidget("Text_tips")

    --持有
    self.Button_confirm = self:GetWidget("Button_confirm") 
    self.Button_confirm:SetVisible(false)

    --领取
    self.Button_cancel = Util:WidgetButton(	
    											self:GetWidget("Button_cancel"), 
    											CPPGameLib.GetString("manor_get") , 
    											CPPGameLib.Handler( self,self.OnBtnExit ) ,
    											nil,nil,Macros.Color.button_yellow
    										)  

    self.Button_cancel:SetPosition( self.Image_bg:GetContentWidth()/2 , self.Button_cancel:GetPositionY() )

    --关闭
    self.Button_close = Util:WidgetButton(self:GetWidget("Button_close"), " " , CPPGameLib.Handler( self,self.OnBtnExit ) )  
end	

function LogicGUI.TempTowerSection:__ShowUI( ... )	
	--标题
	local dungeon_star_box_title = Util:Label( CPPGameLib.GetString("dungeon_star_box_title") , Macros.TypefaceSize.popup , Macros.Color.lilyWhite )
	dungeon_star_box_title:SetAnchorPoint(0.5,0.5)
	dungeon_star_box_title:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2 )
	self.Image_light:AddChild(dungeon_star_box_title)

	local num = CallModuleFunc(ModuleType.TOWER , "GetNowHoldStar")

	--恭喜通关
	self.towar_clearance = Util:Label(CPPGameLib.GetString("towar_clearance",num) , Macros.TypefaceSize.normal , Macros.Color.keypoint )
	self.towar_clearance:SetAnchorPoint(0,0.5)
	self.towar_clearance:SetPosition( self.Image_content:GetPositionX() - (self.Image_content:GetContentWidth()/2) +3 , self.Text_tips:GetPositionY() )
	self.Image_bg:AddChild(self.towar_clearance)
end

function LogicGUI.TempTowerSection:SetShowUI( ... )	
	local lst = CallModuleFunc(ModuleType.TOWER , "GetRewardPreview" , -1 )

	local num = CallModuleFunc(ModuleType.TOWER , "GetNowHoldStar")

	local t1 = {}

	for i,v in ipairs(lst) do
		if v[1].star <= num then
			t1 = v
		end
	end

	local interval = self.Image_content:GetContentWidth() / 4

	for i,v in ipairs(t1) do
		local icon = LogicGUI.IconLayer.New(true)
		local item_type , item_info = CPPGameLib.GetConfig("Item", v.id)
		icon:SetData({id = v.id , obj_info = item_info , num = v.num})
		icon:SetAnchorPoint(0.5,0.5)
		icon:ShowItemName(true)

		local jg = self.Image_content:GetContentWidth() - ( icon:GetNode():GetContentWidth() * 4 )
		icon:SetPosition( (jg/4) + (icon:GetNode():GetContentWidth()/2 ) + interval * (i - 1)  , self.Image_content:GetContentHeight()/2 + 20 )
		
		self.Image_content:AddChild(icon:GetNode())
	end

end

--关闭按钮
function LogicGUI.TempTowerSection:OnBtnExit( ... )
	if self.fun then
		self.fun()
	end
	self:DeleteMe()
end