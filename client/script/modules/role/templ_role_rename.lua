--
-- @brief 改名 
-- @author: yjg
-- @date: 2016年12月13日10:07:10
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplRoleRename = LogicGUI.TemplRoleRename or BaseClass(GUI.Template)

--道具
LogicGUI.TemplRoleRename.Item = ConfigAdapter.Common.GetRenameExpendItem()
--需求
LogicGUI.TemplRoleRename.DEMAND = ConfigAdapter.Common.GetRenameExpendNum()
--持有
LogicGUI.TemplRoleRename.HOLD = CallModuleFunc(ModuleType.Pack,"GetNumByItemID" , ConfigAdapter.Common.GetRenameExpendItem() ) 

function LogicGUI.TemplRoleRename:__init(id,direct,itemResolveId)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function()
		self:__Getwidget()
		self:__ShowUI()
	end

    self:BindGlobalEvent(Macros.Event.ApproachOfAchievingModule.DELETE_APPROACHVIEW, function()
        self:OnBtnClose()
    end)
	    
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.LOGIN)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.LOGIN, true)
	self:InitTemplate(GUI.TemplateRegistry.ROLE_RENAME)
end

function LogicGUI.TemplRoleRename:__delete()

end

function LogicGUI.TemplRoleRename:__SettleData()
	-- CallModuleFunc(ModuleType.HERO, "HeroCollectionReq")
end

function LogicGUI.TemplRoleRename:__Getwidget()
	--背景
	self.Panel_CreateRole = self:GetWidget("Panel_CreateRole") 

	--输入框
	self.TextField_RoleName = self:GetWidget("TextField_RoleName") 

	--英雄模型
	self.Panel_heroMode = self:GetWidget("Panel_heroMode")

	self.Image_dialogue = self:GetWidget("Image_dialogue")

	--随机按钮
	self.Image_Dice =  Util:WidgetButton( 
											self:GetWidget("Image_Dice") , 
											nil ,
											CPPGameLib.Handler(self,self.OnImageDice) 
											)
	--确定
	self.Button_Start = Util:WidgetButton( 
											self:GetWidget("Button_Start") , 
											CPPGameLib.GetString("common_confirm") , 
											CPPGameLib.Handler(self,self.OnButtonStart) ,
											nil,nil,Macros.Color.button_yellow
											)
end

function LogicGUI.TemplRoleRename:__ShowUI()

	LogicGUI.TemplRoleRename.HOLD = CallModuleFunc(ModuleType.Pack,"GetNumByItemID" , ConfigAdapter.Common.GetRenameExpendItem() ) 
	
	self.__root_widget:SetTouchEnabled(true)
	WidgetUnity.SetWidgetListener( self.__root_widget , nil, nil, function( ... )
		self:OnBtnClose()
	end)

	self.Image_1 = self:GetWidget("Image_1")
	self.Image_1:SetImage( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.LOGIN, "name_2", true))

	--模型
	self.__panel_hero_model = LogicGUI.Model.New({
													info_id = 31341, 
													parent = self:GetWidget("Panel_heroMode"),
													dir = MathTool.HORZ_LEFT
												})
	self.__panel_hero_model:SetScaleFactor(1.5)
	PosTool.CenterBottom(self.__panel_hero_model)

	--设置初始名字
	self.edit_role_name = Util:WidgetTextField(self:GetWidget("TextField_RoleName"))
	self.edit_role_name:SetTextColor( unpack(Macros.Color.golden) )
	self.edit_role_name:SetTextHorizontalAlign(UITextFieldEx.ALIGIN_CENTER)
	self.edit_role_name:SetTextVertiacalAlign(UITextFieldEx.ALIGIN_CENTER)
    self.edit_role_name:SetMaxLengthEnabled(true)
    self.edit_role_name:SetMaxLength( 6 )

    self.edit_role_name:SetPlaceHolderText(CPPGameLib.GetString("union_import"))

    local color = Macros.Color.front_color
    if LogicGUI.TemplRoleRename.HOLD < LogicGUI.TemplRoleRename.DEMAND then
    	color = Macros.Color.red_color
    end
    --持有
	local tab = {
					{ zi = CPPGameLib.GetString("login_renameConsume") , dx = Macros.TypefaceSize.normal },
					{ tp = Resource.PathTool.GetItemPath( LogicGUI.TemplRoleRename.Item ) , jg = -10, dx = 0.5 },
					{ zi = " (".. CPPGameLib.GetString("login_possess") , dx = Macros.TypefaceSize.normal , jg = 4 },
					{ zi = LogicGUI.TemplRoleRename.HOLD , dx = Macros.TypefaceSize.normal , jg = 4 , ys = color },
					{ zi = ") " , dx = Macros.TypefaceSize.normal }
				}
	local login_renameConsume = Util:LabCenter(tab)
	login_renameConsume:SetAnchorPoint( 0.5 , 0.5 )
	self.__root_widget:AddChild( login_renameConsume )
	PosTool.Center( login_renameConsume , 0 , -250 )

	if not self.__create_role_tips then
		self.__create_role_tips = Util:Label( CPPGameLib.GetString("login_renametips") , Macros.TypefaceSize.popup , Macros.Color.content , self.Image_dialogue:GetContentWidth() - 28 )	
		self.Image_dialogue:AddChild(self.__create_role_tips)
		PosTool.Center(self.__create_role_tips, 5)
	end

end

--随机码
function LogicGUI.TemplRoleRename:OnImageDice()
	print("xy" , "随机名")
	--设置初始名字
	self.edit_role_name:SetText( CallModuleFunc(ModuleType.LOGIN, "NameRandom" ) )
end

--确定
function LogicGUI.TemplRoleRename:OnButtonStart()
	print("xy" , "确定")
	local role_name = self.edit_role_name:GetText()
	local tab , type_ = LanguageFilter.NameFilter(role_name)
	if not role_name or role_name == "" then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_please_input_role_name"))	
	elseif not type_ then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_role_name_invalid"))	
	elseif LogicGUI.TemplRoleRename.HOLD < LogicGUI.TemplRoleRename.DEMAND then

		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_renameItemLackof"))
		GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.APPROACH_OF_ACHIEVINGVIEW, false, LogicGUI.TemplRoleRename.Item)
	elseif type_ then
		local _ ,num ,__ = StringTool.CountUTF8String(role_name)
		if num > 6 then
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("login_role_name_long"))
		else
			if not CallModuleFunc(ModuleType.SUNDRY, "IsNameConflitWithArenaRobot", role_name) then
				CallModuleFunc(ModuleType.LOGIN, "ChangeRoleNameReq" , role_name )
				self:OnBtnClose()
			else
				local config_error_code = CPPGameLib.GetConfig("ErrorCode", 1003 )
				GlobalTipMsg.GetInstance():Show(config_error_code.err_tips)
			end
		end
	end
end

--取消
function LogicGUI.TemplRoleRename:OnBtnClose()
	self:DeleteMe()
end
