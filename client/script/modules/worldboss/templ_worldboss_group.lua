--
-- @brief 世界boss势力弹窗 
-- @author: yjg
-- @date: 2016年11月18日17:32:25
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplWorldbossGroup = LogicGUI.TemplWorldbossGroup or BaseClass(GUI.Template)

function LogicGUI.TemplWorldbossGroup:__init(id,direct,itemResolveId)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function()
		self:__Getwidget()
		self:__ShowUI()
	end
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_WORLDBOSS_GROUP)	
end

function LogicGUI.TemplWorldbossGroup:__delete()

end

function LogicGUI.TemplWorldbossGroup:__Getwidget()
	--背景
	self.Image_bg = self:GetWidget("Image_bg") 
	--光
	self.Image_light = self:GetWidget("Image_light") 
	--关闭
	self.Button_close = Util:WidgetButton( self:GetWidget("Button_close") , nil , function ( ... )
		self:OnButtonClose()
	end)
	--内容背景
	self.Image_content = self:GetWidget("Image_content") 
	--势力一
	self.Image_influence1 = self:GetWidget("Image_influence1") 
	--按钮一
	self.Button_icon1 = Util:WidgetButton( self:GetWidget("Button_icon1") , nil , function ( ... )
		self:OnButtonIcon(Macros.Game.FORCE.CAMP_LEGEND)
	end)
	--势力二
	self.Image_influence2 = self:GetWidget("Image_influence2") 
	--按钮二
	self.Button_icon2 = Util:WidgetButton( self:GetWidget("Button_icon2") , nil , function ( ... )
		self:OnButtonIcon(Macros.Game.FORCE.CAMP_SUPER)
	end)
	--势力三
	self.Image_influence3 = self:GetWidget("Image_influence3") 
	--按钮三
	self.Button_icon3 = Util:WidgetButton( self:GetWidget("Button_icon3") , nil , function ( ... )
		self:OnButtonIcon(Macros.Game.FORCE.CAMP_WARCRAFT)
	end)
end

function LogicGUI.TemplWorldbossGroup:__ShowUI()
	local worldBoss_select = Util:Label( CPPGameLib.GetString("worldBoss_select") , Macros.TypefaceSize.popup , Macros.Color.white )
	worldBoss_select:SetAnchorPoint( 0.5 , 0.5 )
	self.Image_light:AddChild( worldBoss_select )
	PosTool.Center( worldBoss_select )

	local worldBoss_selectTips = Util:Label( CPPGameLib.GetString("worldBoss_selectTips") , Macros.TypefaceSize.popup , Macros.Color.content )
	worldBoss_selectTips:SetAnchorPoint( 0.5 , 0.5 )
	self.Image_bg:AddChild( worldBoss_selectTips )
	PosTool.Center( worldBoss_selectTips  , 0 , 0 - self.Image_bg:GetContentHeight()/2 + worldBoss_selectTips:GetContentHeight() )

	local stringTab = {
			[1] = CPPGameLib.GetString("worldBoss_story"),
			[2] = CPPGameLib.GetString("worldBoss_super"),
			[3] = CPPGameLib.GetString("worldBoss_fantasy"),
	}

	local typelst = {
			[1] = 1,
			[2] = 0,
			[3] = 2,
	}

	for i = 1 , 3 do
		local name = Util:Label( stringTab[i] , Macros.TypefaceSize.largish , Macros.Color.keypoint )
		name:SetAnchorPoint(0.5 , 0.5 )
		name:SetPosition( self["Image_influence"..i]:GetContentWidth()/2 , self["Image_influence"..i]:GetContentHeight()/5  )
		self["Image_influence"..i]:AddChild(name)

		local cnf = CPPGameLib.GetConfig("Camp", typelst[i])
	    CPPGameLib.PrintTable("xy", cnf ,"物品合成响应")
		local worldBoss_harm = Util:Label( CPPGameLib.GetString("worldBoss_harm" , cnf.world_boss_damage_add[1][2] / 10000 * 100 ) , Macros.TypefaceSize.largish , Macros.Color.front_color )
		worldBoss_harm:SetAnchorPoint(0.5,0.5)
		worldBoss_harm:SetPosition( self["Image_influence"..i]:GetContentWidth()/2 , self["Image_influence"..i]:GetContentHeight()/10 )
		self["Image_influence"..i]:AddChild(worldBoss_harm)
	end

end

function LogicGUI.TemplWorldbossGroup:OnButtonIcon(index)
	local info = {}
	info.camp = index
	CallModuleFunc(ModuleType.WORLDBOSS, "WorldBossSelectCampReq",info)
	self:OnButtonClose()
end

function LogicGUI.TemplWorldbossGroup:OnButtonClose()
	self:DeleteMe()
end