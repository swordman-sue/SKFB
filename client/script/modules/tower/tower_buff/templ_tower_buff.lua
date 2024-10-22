
--
-- @brief buff选择
-- @author: yjg
-- @date: 2016年9月27日11:44:59
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplTowerBuff = LogicGUI.TemplTowerBuff or BaseClass(GUI.Template)


function LogicGUI.TemplTowerBuff:__init(fun)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__open_callback = function()
		self.fun = fun
		self:__Getwidget()
		self:__ShowUI()
	end
	self:InitTemplate(GUI.TemplateRegistry.TOWER_BUFF)	
end

function LogicGUI.TemplTowerBuff:__delete()

end


function LogicGUI.TemplTowerBuff:SetData(data)
	print("////")
end

function LogicGUI.TemplTowerBuff:__Getwidget( ... )
	--背景层
    self.Image_1 = self:GetWidget("Image_1") 
    --光
    self.Image_2_0 = self:GetWidget("Image_2_0") 
    --关闭
    self.Button_close = Util:WidgetButton(self:GetWidget("Button_close"), nil , CPPGameLib.Handler( self,self.OnBtnExit))  
    self.Button_close:SetVisible(false)
    --内容
    self.Image_content = self:GetWidget("Image_content") 
    --选择
    self.Button_continue = Util:WidgetButton(
    											self:GetWidget("Button_continue"), 
    											CPPGameLib.GetString("tower_select") , 
    											CPPGameLib.Handler( self,self.OnButtonContinue),
    											nil,nil,Macros.Color.button_yellow
    										)  
    --提示
    self.Text_residue = self:GetWidget("Text_residue") 

    self.Image_type = {}
    self.Panel_headline = {}
    self.Panel_icon = {}
    self.Text_describe = {}
    self.Text_consume = {}

    for i = 1 , 3 do
	    --背景
	    self.Image_type[i] = self:GetWidget("Image_type"..i) 
	    --标题
	    self.Panel_headline[i] = self:GetWidget("Panel_headline"..i) 
	    --图标
	    self.Panel_icon[i] = self:GetWidget("Panel_icon"..i) 
	    --内容
	    self.Text_describe[i] = self:GetWidget("Text_describe"..i) 
	    --消耗
	    self.Text_consume[i] = self:GetWidget("Text_consume"..i) 
    end

end

function LogicGUI.TemplTowerBuff:__ShowUI( ... )	
	--标题
	local headline = Util:Label( CPPGameLib.GetString("tower_buyBuff") , Macros.TypefaceSize.largish , Macros.Color.lilyWhite )
	headline:SetAnchorPoint(0.5,0.5)
	headline:SetPosition(self.Image_2_0:GetContentWidth()/2 , self.Image_2_0:GetContentHeight()/2 )
	self.Image_2_0:AddChild(headline)

	local lst = CallModuleFunc(ModuleType.TOWER , "GetRandProList")

	local star = {3,6,9}
	for i,v in ipairs(lst) do
		--标题
		local type_ = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TOWER, "tp_"..v.pro_name,true))
		type_:SetPosition( self.Panel_headline[i]:GetContentWidth()/2 , self.Panel_headline[i]:GetContentHeight()/2 )
		self.Panel_headline[i]:AddChild(type_)

		--图标背景
		local iconBG = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TOWER, "buff_icon_bg"))
		iconBG:SetPosition( self.Panel_icon[i]:GetContentWidth()/2 , self.Panel_icon[i]:GetContentHeight()/2 )
		self.Panel_icon[i]:AddChild(iconBG)
		--图标
		local icon = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TOWER, "tp_"..v.pro_name))
		icon:SetPosition( iconBG:GetContentWidth()/2 , iconBG:GetContentHeight()/2)
		iconBG:AddChild(icon)

		local type_ , info_ = Util:Nature( v.pro_type , v.pro_value , true)
		--消耗
		local tips = Util:LabCenter(  {
										{zi = type_ .. CPPGameLib.GetString("common_colon") , dx = Macros.TypefaceSize.normal , ys = Macros.Color.keypoint } , 
										{zi = info_ , dx = Macros.TypefaceSize.normal , jg = 5 , ys = Macros.Color.front_color } ,
									} )
		tips:SetPosition(self.Text_describe[i]:GetContentWidth()/2 , self.Text_describe[i]:GetContentHeight()/2)
		tips:SetAnchorPoint(0.5,0.5)
		self.Text_describe[i]:AddChild(tips)	

		local color = Macros.Color.red_color
		local num = CallModuleFunc(ModuleType.TOWER,"GetCurStar")
		if num >= star[i] then
			color = Macros.Color.content
		end

		--消耗
		local curve = Util:LabCenter(  {
										{zi = CPPGameLib.GetString("rebel_consume") , dx = Macros.TypefaceSize.normal ,ys = Macros.Color.keypoint } , 
										{tp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "star") , dx = 1 , jg = 5} ,
										{zi = star[i] , dx = Macros.TypefaceSize.normal , jg = 5 , ys = color } ,
									} )
		curve:SetPosition(self.Text_consume[i]:GetContentWidth()/2 , self.Text_consume[i]:GetContentHeight()/2)
		curve:SetAnchorPoint(0.5,0.5)
		self.Text_consume[i]:AddChild(curve)	

		--触摸层
		local touch = Util:Layout( self.Image_type[i]:GetContentWidth() , self.Image_type[i]:GetContentHeight() , function()
			local x = CallModuleFunc(ModuleType.TOWER , "GetCurStar")
			if x < star[i] then
				GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("tower_centos") )
				return
			end

			if self.on then
				self.on:RemoveSelf(true)
				self.on = nil
			end
			self.on = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TOWER, "bigselect") )
			self.on:SetPosition( self.Panel_icon[i]:GetContentWidth()/2 , self.Panel_icon[i]:GetContentHeight()/2 )
			self.on:SetAnchorPoint(0.5,0.5)
			self.Panel_icon[i]:AddChild(self.on)

			if self.frame then
				self.frame:RemoveSelf(true)
				self.frame = nil
			end
			self.frame = Util:Sprite9( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.TOWER, "onbuff") )
			self.frame:SetContentSize( self.Image_type[i]:GetContentWidth() + 5 , self.Image_type[i]:GetContentHeight() + 5 )
			self.frame:SetPosition( self.Image_type[i]:GetContentWidth()/2 , self.Image_type[i]:GetContentHeight()/2 )
			self.frame:SetAnchorPoint(0.5,0.5)
			self.Image_type[i]:AddChild(self.frame)

			self.info = {}
			self.info.star = star[i]
			self.info.pro_index = v.pro_index - 1
		end  )
		touch:SetAnchorPoint(0.5,0.5)
		touch:SetPosition( self.Image_type[i]:GetContentWidth()/2 , self.Image_type[i]:GetContentHeight()/2 )
		self.Image_type[i]:AddChild(touch)
	end


	local x = CallModuleFunc(ModuleType.TOWER , "GetCurStar")
	--持有
	local curve = Util:LabCenter(  {
									{zi = CPPGameLib.GetString("approach_have") , ys = Macros.Color.keypoint } , 
									{tp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "star") , dx = 1 , jg = 5} ,
									{zi = x , jg = 5 , ys = Macros.Color.content } ,
								} )
	curve:SetPosition(self.Text_residue:GetContentWidth()/2 , self.Text_residue:GetContentHeight()/2 + 2)
	curve:SetAnchorPoint(0.5,0.5)
	self.Text_residue:AddChild(curve)	
end

--关闭
function LogicGUI.TemplTowerBuff:OnBtnExit( ... )
	self:DeleteMe()
end

--购买
function LogicGUI.TemplTowerBuff:OnButtonContinue( ... )
	if self.info then
		CallModuleFunc(ModuleType.TOWER , "TowerSelectProReq" , self.info)
		self:OnBtnExit()
		self.fun()
	else
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("tower_noSelectBuff") )
	end
end