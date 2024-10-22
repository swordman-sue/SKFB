--[[
%% @module: 卡牌兑换界面
%% @author: yjg
%% @created: 2017年12月4日15:16:24
--]]


LogicGUI = LogicGUI or {}

LogicGUI.TemplDivinationConvert = LogicGUI.TemplDivinationConvert or BaseClass(GUI.Template)

function LogicGUI.TemplDivinationConvert:__init( func )
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__open_callback = function ()
		self.func = func
	    self:__Getwidget()
	    self:__ShowUI()
	end	
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_DIVINATION_CONVERT) 
end

function LogicGUI.TemplDivinationConvert:__delete()
end

function LogicGUI.TemplDivinationConvert:__Getwidget()
	--背景
	self.Image_bg = self:GetWidget("Image_bg") 
	--名字
	self.Image_content = self:GetWidget("Image_content") 
	--条目
	self.Image_2 = self:GetWidget("Image_2") 
	--条目
	self.Image_3 = self:GetWidget("Image_3") 
	--标题
	self.Image_light = self:GetWidget("Image_light") 
	--确定
	self.Button_convert = Util:WidgetButton( 
												self:GetWidget("Button_convert") , 
												CPPGameLib.GetString("divination_convert") ,
												CPPGameLib.Handler(self,self.OnButtonConvert) , 
												nil,nil,Macros.Color.button_yellow
											) 
	--关闭按钮
	self.Button_close = Util:WidgetButton( self:GetWidget("Button_close") , nil , CPPGameLib.Handler(self,self.OnButtonClose) ) 
end

function LogicGUI.TemplDivinationConvert:__ShowUI()

    self.goods_id = 1
    local cnf = CPPGameLib.GetConfig( "HideShop" , self.goods_id )

	--兑换
	local divination_cardPro = Util:Label( CPPGameLib.GetString("divination_convert") , Macros.TypefaceSize.tab , Macros.Color.btn )
	divination_cardPro:SetAnchorPoint( 0.5 , 0.5 )
	self.Image_light:AddChild( divination_cardPro )
	PosTool.Center( divination_cardPro )

	--名字
	local name = Util:Label( CPPGameLib.GetString("divination_convert") , Macros.TypefaceSize.tab , Macros.Color.headline )
	name:SetAnchorPoint( 0 , 0.5 )
	name:SetPosition( 10 , 115 )
	self.Image_2:AddChild( name )

    --头像
    local icon = LogicGUI.IconLayer.New(true)
    icon:SetData({ id = cnf.cost_type_list[1] , num = cnf.cost_value_list[1] })
    icon:SetAnchorPoint( 0 , 0.5 )
    icon:SetPosition( 10 , self.Image_2:GetContentHeight()/2 - 20 )
    self.Image_2:AddChild(icon:GetNode())

    local sp_activity_equal = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW , "sp_activity_equal" ) )
    sp_activity_equal:SetPosition( 60 + 75 , icon:GetNode():GetPositionY() )
    sp_activity_equal:SetAnchorPoint( 0.5 , 0.5 )
    self.Image_2:AddChild(sp_activity_equal)

    --头像
    local icon = LogicGUI.IconLayer.New(true)
    icon:SetData({id = cnf.item_id , num = cnf.item_num })
    icon:SetAnchorPoint( 0 , 0.5 )
    icon:SetPosition( 60 + 75 + 45 , self.Image_2:GetContentHeight()/2 - 20 )
    self.Image_2:AddChild(icon:GetNode())

end

--关闭
function LogicGUI.TemplDivinationConvert:OnButtonClose()
	self:DeleteMe()
	if self.func then
		self.func()
	end
end

--确定
function LogicGUI.TemplDivinationConvert:OnButtonConvert()
    
    local cnf = CPPGameLib.GetConfig( "HideShop" , self.goods_id )

	local item_num = CallModuleFunc(ModuleType.ROLE,"GetPropValue" , cnf.cost_type_list[1] )

    local t1 = {}
    for i,v in ipairs(cnf.cost_type_list) do
        table.insert(t1,{ type = v , num = cnf.cost_value_list[i] })
    end

    local info = {}
    info.item_id = cnf.item_id 
    info.num = cnf.item_num
    info.type = Macros.Game.SHOP.HIDE
    info.goods_id = self.goods_id
    info.cost = t1
   	info.max = math.floor( item_num / cnf.cost_value_list[1] )
    info.func = function ( pitchNum )
		if ( pitchNum * cnf.cost_value_list[1] ) > item_num then
			local txt = "currency_insufficient_"..cnf.cost_type_list[1]
			GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString(txt) )
		else
	        local data = {}
	        data.type = info.type
	        data.param1 = info.goods_id
	        data.param2 = pitchNum
			CallModuleFunc(ModuleType.SHOP,"ShopBuyRequest",data)
		end
    end
    GUITemplateMgr:CreateTemplateInstance( GUI.TemplateRegistry.SHOP_BUY_TIPVIEW , false , info )
end

