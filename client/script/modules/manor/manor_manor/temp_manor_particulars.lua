
--[[
%% @module: 领地详情
%% @author: yjg
%% @created: 2016年10月11日10:59:36
--]]

Modules = Modules or {}

Modules.TempManorParticulars = Modules.TempManorParticulars or BaseClass(GUI.Template)

function Modules.TempManorParticulars:__init( id )
	self.__is_auto_load = true
		
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function()
		self.id = id
		self:__Getwidget()
		self:__ShowUI()
	end
	self:InitTemplate(GUI.TemplateRegistry.MANOR_START)	
end

function Modules.TempManorParticulars:__delete()
	-- if self.common then
	-- 	self.common:DeleteMe()
	-- 	self.common = nil
	-- end

	-- if self.specialty then
	-- 	self.specialty:DeleteMe()
	-- 	self.specialty = nil
	-- end
end

function Modules.TempManorParticulars:__Getwidget()
	--背景
	self.Image_BG = self:GetWidget("Image_BG") 
	--光
	self.Image_light = self:GetWidget("Image_light") 
	--关闭
	self.Button_close = Util:WidgetButton(self:GetWidget("Button_close") , nil , CPPGameLib.Handler(self,self.OnButtonClose) )

	--副本图片
	self.Image_setting = self:GetWidget("Image_setting") 
	--添加
	self.Button_add = Util:WidgetButton(self:GetWidget("Button_add") , nil , CPPGameLib.Handler(self,self.OnButtonAdd) )
	ActionManager.GetInstance():CycleBlinkEffect(self.Button_add, 1 )

	--控制面板
	self.Image_controlPanel = self:GetWidget("Image_controlPanel") 

	--奖励
	self.Image_award = self:GetWidget("Image_award") 
	--描述
	self.Image_state = self:GetWidget("Image_state") 
	--iconBg
	self.Panel_iconBg = self:GetWidget("Panel_iconBg") 

	--左
	self.Image_left = self:GetWidget("Image_left") 
	self.Image_left:SetVisible(false)
	--右
	self.Image_right = self:GetWidget("Image_right") 
	self.Image_right:SetVisible(false)
end

function Modules.TempManorParticulars:__ShowUI( )
	--领地表
	local landCityCnf = CPPGameLib.GetConfig("LandCity", self.id)

	--标题
	local light = Util:Label( landCityCnf.name , Macros.TypefaceSize.popup , Macros.Color.lilyWhite )
	light:SetAnchorPoint(0.5,0.5)
	light:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2 )
	self.Image_light:AddChild(light)

----------
	self.Image_setting:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.MANOR_BG, landCityCnf.build_bg ) )
	
	--点击添加探索英雄
	local manor_addPatrolTips = Util:Label(
											CPPGameLib.GetString("manor_addPatrolTips") ,
											Macros.TypefaceSize.normal,
											Macros.Color.golden,
											nil,nil,Macros.Color.white_stroke
											)
	manor_addPatrolTips:SetPosition(self.Button_add:GetPositionX() , self.Button_add:GetPositionY() - (self.Button_add:GetContentHeight()/2) - 15 )
	self.Image_BG:AddChild(manor_addPatrolTips)

	--每次探索都可获得探索英雄或英雄碎片
	local manor_addPatrolTips_1 = Util:Label( CPPGameLib.GetString("manor_addPatrolTips_1") , nil , nil, nil, nil, Macros.Color.common_stroke, nil)
	manor_addPatrolTips_1:SetAnchorPoint(0.5,0.5)
	manor_addPatrolTips_1:SetPosition( self.Button_add:GetPositionX() , self.Button_add:GetPositionY() - (self.Button_add:GetContentHeight()/2) - 45 )
	self.Image_BG:AddChild(manor_addPatrolTips_1)

----------
	--巡逻奖励
	local manor_patrolEnd = Util:Label( CPPGameLib.GetString("manor_patrolEnd") , Macros.TypefaceSize.normal , Macros.Color.lilyWhite )
	manor_patrolEnd:SetAnchorPoint(0,0.5)
	manor_patrolEnd:SetPosition( 10 , self.Image_award:GetContentHeight()/2  )
	self.Image_award:AddChild(manor_patrolEnd)

	--icon列表
	local pack = CallModuleFunc(ModuleType.MANOR, "GetCityPack",self.id)
    local common =  
    {
        item_class = Modules.TempManorParticularsItem,
        horizon = true,
        item_width = 90,
        item_height = 90,
        row = 1,
        col = 4,
        selectable = true,
        update_move_func = function ( data )
        	if #pack > 4 then
	        	local min = self.common:GetScrollMinX()
				local max = self.common:GetScrollMaxX()
				local x = self.common:GetScrollX()
				self.Image_left:SetVisible(false)
				self.Image_right:SetVisible(false)
				if x <= -30 then
					self.Image_left:SetVisible(true)
				end 

				if math.abs(x - min) > 30 then
					self.Image_right:SetVisible(true)
				end
			end
        end,
        view_width = self.Panel_iconBg:GetContentWidth() - 10 ,
    }

    -- 普通列表
    self.common = GUI.ScrollView.New(common)
    self.common:SetDataList( pack )
    self.Panel_iconBg:AddChild(self.common:GetNode())
    PosTool.Center(self.common:GetNode())

----------
	--描述
	local manor_referral = Util:Label( CPPGameLib.GetString("manor_referral") , Macros.TypefaceSize.normal , Macros.Color.lilyWhite )
	manor_referral:SetAnchorPoint(0,0.5)
	manor_referral:SetPosition( 10 , self.Image_state:GetContentHeight()/2  )
	self.Image_state:AddChild(manor_referral)

	--描述
	local desc = Util:Label( landCityCnf.desc , Macros.TypefaceSize.normal , Macros.Color.content , self.Image_controlPanel:GetContentWidth() - 20 )
	desc:SetAnchorPoint(0,1)
	desc:SetPosition( 10 , self.Image_state:GetPositionY() - (self.Image_state:GetContentHeight()/2) - 10 )
	self.Image_controlPanel:AddChild(desc)
end

--关闭按钮
function Modules.TempManorParticulars:OnButtonClose( ... )
	self:DeleteMe()
end

--添加按钮
function Modules.TempManorParticulars:OnButtonAdd( ... )
	CallModuleFunc(ModuleType.MANOR, "OpenView", BaseViewType.MANORHEROLST , self.id )
	self:DeleteMe()
end



Modules = Modules or {}

Modules.TempManorParticularsItem = Modules.TempManorParticularsItem or BaseClass(GUI.ScrollItem)

function Modules.TempManorParticularsItem:__init()
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)
end

function Modules.TempManorParticularsItem:__CreateWidgetFromLuaCode()
	self.layout = Util:Layout()
	self.layout:SetAnchorPoint(0.5, 0.5)
	self.layout:SetContentSize(90, 90)
	return self.layout
end

function Modules.TempManorParticularsItem:__delete()
end

function Modules.TempManorParticularsItem:SetData(data)
	local icon = LogicGUI.IconLayer.New(true)
	icon:SetPosition(self.layout:GetContentWidth()/2 ,self.layout:GetContentHeight()/2)
	icon:SetTouchSwallow(false)
	self.layout:AddChild(icon:GetNode())
	local iconData = { id = data.id , num = data.num }
	icon:SetData(iconData)
end

function Modules.TempManorParticularsItem:__AddToParent()
end
