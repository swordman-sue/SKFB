GUI = GUI or {}

GUI.TemplPolytropicTypeVipItem = GUI.TemplPolytropicTypeVipItem or BaseClass(GUI.Template)

function GUI.TemplPolytropicTypeVipItem:__init(data)
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_VIP_ITEM)
	self:CreateView(data)
end

function GUI.TemplPolytropicTypeVipItem:__AddToParent()
	
end

function GUI.TemplPolytropicTypeVipItem:CreateView(data)
	GUI.Template.SetData(self,data)

	self:GetWidget("Image_content"):SetVisible(false)

	local img_content = self:GetWidget("Image_content_1")
	img_content:SetAnchorPoint(0.5,0)
	PosTool.CenterBottom(img_content)
	
	--系统名字
	local system_name = Util:Label(self.__data.module_txt or "", Macros.TypefaceSize.headlineII, Macros.Color.golden, 0, 0, Macros.Color.common_stroke, 2)
	system_name:SetAnchorPoint(0.5,1)
	img_content:AddChild(system_name)
	PosTool.CenterTop(system_name,0,-15)

	--按钮
    self.__entry_btn = Util:Sprite(Resource.PathTool.GetSystemEntryIconPath(self.__data.res_name))
    self.__entry_btn:SetScaleFactor(1.5)
    img_content:AddChild(self.__entry_btn)
    PosTool.Center(self.__entry_btn,0,10)

    --前往按钮
    local btn_go = self:GetWidget("Button_goGet")
    Util:WidgetButton(btn_go, CPPGameLib.GetString("polytropic_maincity_go_for"), function ()
    	self:GoForSystem(self.__data)
    end, Macros.TypefaceSize.largish, Macros.Color.btn, Macros.Color.button_red, 2)
end

function GUI.TemplPolytropicTypeVipItem:__delete()
	
end

--前往系统
function GUI.TemplPolytropicTypeVipItem:GoForSystem(entry_info)
	if not entry_info then
		return
	end
	
    --检查等级开放
	local is_open = GlobalModulesMgr:IsOpen(entry_info.system_id,true)  
	if not is_open then
		return
	end

	if entry_info.system_id == Macros.Game.SystemID.UNION then
		local role_info = CallModuleFunc(ModuleType.ROLE,"GetRoleInfo")
		if role_info.guild_id == 0 then
			CallModuleFunc(ModuleType.UNION , "GuildDateReq" , "union" )
    	else
			--系统模块跳转
			GlobalModulesMgr:OpenSystemModule(entry_info.system_id, nil, false)
    	end
    elseif entry_info.system_id == Macros.Game.SystemID.LIMIT_TEAM then	
    	CallModuleFunc(ModuleType.SCENE, "EnterSceneReq",Macros.Game.SceneType.TEAM_HALL,1)	
    elseif entry_info.system_id == Macros.Game.SystemID.FIRST_RECHARGE then	
    	CallModuleFunc(ModuleType.CHARGE , "SetFirstRechargeRed" , 0 )
		--系统模块跳转
		GlobalModulesMgr:OpenSystemModule(entry_info.system_id, nil, false) 
	-- unlock定制FB功能  	
	elseif entry_info.system_id == Macros.Game.SystemID.UNLOCK_FABEBOOK then
		self:__OnUnlockFacebook()	
	-- unlock定制看视频功能  	
	elseif entry_info.system_id == Macros.Game.SystemID.UNLOCK_AD then
		self:__OnUnlockAdvertising()	
	else
		--系统模块跳转
		GlobalModulesMgr:OpenSystemModule(entry_info.system_id, nil, false)
	end
end