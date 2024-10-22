
--
-- @brief 无双密藏
-- @author: yjg
-- @date: 2016年9月27日11:44:59
--

LogicGUI = LogicGUI or {}

LogicGUI.TempTowerTreasure = LogicGUI.TempTowerTreasure or BaseClass(GUI.Template)

function LogicGUI.TempTowerTreasure:__init(index)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__open_callback = function()
		self.index = 3
		self.price = 0

		self:__Getwidget()
		self:__ShowUI()
	end
	self:InitTemplate(GUI.TemplateRegistry.TOWER_TREASURE)	
end


function LogicGUI.TempTowerTreasure:__delete()
	if self.__shield_layer then
		self.__shield_layer:DeleteMe()
		self.__shield_layer = nil
	end
end


function LogicGUI.TempTowerTreasure:SetData(data)
	print("////")
end

function LogicGUI.TempTowerTreasure:__Getwidget( ... )
	--标题
    self.Image_bg = self:GetWidget("Image_bg") 
    --内容
    self.Image_content = self:GetWidget("Image_content") 
    --图标
    self.Panel_icon = self:GetWidget("Panel_icon") 
    --名称
    self.Text_name = self:GetWidget("Text_name") 
    --原价
    self.Text_raw = self:GetWidget("Text_raw") 
    --现价
    self.Text_now = self:GetWidget("Text_now") 
    --光
    self.Image_light = self:GetWidget("Image_light") 
    --关闭按钮
    self.Button_close = Util:WidgetButton(self:GetWidget("Button_close"), nil , CPPGameLib.Handler( self,self.OnBtnExit ) )  
    --取消
    self.Button_cancel = Util:WidgetButton(
    										self:GetWidget("Button_cancel"), 
    										CPPGameLib.GetString("common_cancel") , 
    										CPPGameLib.Handler( self,self.OnBtnExit ) ,
    										nil,nil,Macros.Color.button_red
    										)  
    --确定
    self.Button_confirm = Util:WidgetButton(
    										self:GetWidget("Button_confirm"), 
    										CPPGameLib.GetString("shop_sure_Buy") , 
    										CPPGameLib.Handler( self,self.OnBtnBuy ) ,
    										nil,nil,Macros.Color.button_yellow
    										)  
    --提示
    self.Text_tips = self:GetWidget("Text_tips") 
end

function LogicGUI.TempTowerTreasure:__ShowUI( ... )	
	--标题
	local headline = Util:Label( CPPGameLib.GetString("tower_treasure") , Macros.TypefaceSize.largish , Macros.Color.lilyWhite )
	headline:SetAnchorPoint(0.5,0.5)
	headline:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2 )
	self.Image_light:AddChild(headline)

	local type_ , lst = CallModuleFunc(ModuleType.TOWER , "GetStarGoods")
	if type_ == false then
		return
	end
	local type , itemData = ObjIDTool.GetInfo(lst.award_id)
	local sp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.DIAMOND)

    local curve = Util:LabCenter(  {
    									{zi = CPPGameLib.GetString("tower_storehouseTips1") , dx = Macros.TypefaceSize.minimum , ys = Macros.Color.keypoint } , 
    									{zi = CallModuleFunc(ModuleType.TOWER , "GetTotalStar") , dx = Macros.TypefaceSize.minimum , jg = 5 , ys = Macros.Color.keypoint } ,
    									{tp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "star") , dx = 1 , jg = 5} ,
    									{zi = CPPGameLib.GetString("tower_storehouseTips2") , dx = Macros.TypefaceSize.minimum , jg = 5 , ys = Macros.Color.keypoint } ,
    								} )
    curve:SetPosition(self.Text_tips:GetContentWidth()/2 , self.Text_tips:GetContentWidth()/2)
    curve:SetAnchorPoint(0.5,0.5)
    self.Text_tips:AddChild(curve)	

	--图标
    local icon_layer = LogicGUI.IconLayer.New(true)
    icon_layer:SetData({id = lst.award_id , num = lst.award_num , obj_info = itemData })
    icon_layer:SetPosition( self.Panel_icon:GetContentWidth()/2 , self.Panel_icon:GetContentHeight()/2 )
    self.Panel_icon:AddChild(icon_layer:GetNode())

    --名字
	local name = Util:Name(itemData.name , nil , itemData.quality )
	name:SetPosition( self.Text_name:GetContentWidth()/2 , self.Text_name:GetContentHeight()/2)
	name:SetAnchorPoint(0.5,0.5)
	self.Text_name:AddChild(name)

	--原价
	local origina_t = Util:Label( CPPGameLib.GetString("tower_origina") ,Macros.TypefaceSize.normal, Macros.Color.keypoint)
	origina_t:SetPosition( -72 , self.Text_raw:GetContentHeight()/2)
	origina_t:SetAnchorPoint(0,0.5)
	self.Text_raw:AddChild(origina_t)	

	local origina = Util:Label(lst.origina , Macros.TypefaceSize.normal , Macros.Color.content )
	PosTool.RightTo(origina_t,origina , 5)
	self.Text_raw:AddChild(origina)	

	local diamond = Util:Sprite(sp)
	PosTool.RightTo(origina,diamond , 5)
	diamond:SetScaleFactor(0.75)
	self.Text_raw:AddChild(diamond)

	--红线
	local node = Util:Sprite9(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "delete_line"))
	node:SetContentSize( origina_t:GetContentWidth() + diamond:GetContentWidth() + origina:GetContentWidth() , 3 )
    node:SetPosition(origina_t:GetPosition())
    node:SetAnchorPoint(0,0.5)
    self.Text_raw:AddChild(node)	

    --现价
	local price = Util:Label( CPPGameLib.GetString("shop_after_price") ,Macros.TypefaceSize.normal, Macros.Color.keypoint)
	price:SetPosition( -72 , self.Text_now:GetContentHeight()/2 )
	price:SetAnchorPoint(0,0.5)
	self.Text_now:AddChild(price)

	self.price = lst.award_price
	local price_ = Util:Label(lst.award_price , Macros.TypefaceSize.normal , Macros.Color.content )
	PosTool.RightTo(price,price_ , 5)
	self.Text_now:AddChild(price_)

	local diamond = Util:Sprite(sp)
	PosTool.RightTo(price_,diamond , 5)
	diamond:SetScaleFactor(0.75)
	self.Text_now:AddChild(diamond)
end

--关闭
function LogicGUI.TempTowerTreasure:OnBtnExit( ... )
	self.__bg:SetVisible(false)
	--缩放效果
	local node = self:GetNode()
	 -- node:GetPositionX() , node:GetPositionY() - 50

	 print("xy" , node:GetPositionX() )


	CPPActionManager.MoveTo(node, 0.13 , node:GetPositionX() , node:GetPositionY() - 150 )
	CPPActionManager.ScaleTo(node, 0.13, 0.1, GlobalCallbackMgr:AddCallBackFunc( function ( ... )
		self:DeleteMe()
	end ))
end

--购买
function LogicGUI.TempTowerTreasure:OnBtnBuy( ... )
	local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")

	if self.price > userDate.diamond then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("common_diamond_not_enough"))
	else
		CallModuleFunc(ModuleType.TOWER , "TowerBuyStarGoosReq")
	end
	self:OnBtnExit()
end