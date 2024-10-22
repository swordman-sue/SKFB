
--
-- @brief 公会入会申请
-- @author: yjg
-- @date: 2016年10月18日19:44:36
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplUnionAudit = LogicGUI.TemplUnionAudit or BaseClass(GUI.Template)

function LogicGUI.TemplUnionAudit:__init()
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID} 
    self.__open_callback = function ()
		self.onIndex = nil
		self.onIconTips = nil

        self:__Getwidget()
        self:__ShowUI()

        self:__Server()
    end 
    self:__AddPreloadTemplate(GUI.TemplateRegistry.UNION_AUDIT_ITEM)
	self:InitTemplate(GUI.TemplateRegistry.UNION_AUDIT)	
end


function LogicGUI.TemplUnionAudit:__delete()
	if self.lst_list then
		self.lst_list:DeleteMe()
		self.lst_list = nil
	end
end

--数据
function LogicGUI.TemplUnionAudit:__Server()
	CallModuleFunc(ModuleType.UNION , "GuildApplyListReq")
	--公会信息
    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_APPLY , function (_, data)
    	self:__SetShowUI()
    end)
end

function LogicGUI.TemplUnionAudit:__Getwidget( ... )
	--背景
	self.Image_bg = self:GetWidget("Image_bg")
	--光
	self.Image_light = self:GetWidget("Image_light")	
	--关闭
	self.Button_close = Util:WidgetButton( 
                                            self:GetWidget("Button_close") , 
                                            nil , CPPGameLib.Handler(self,self.OnButtonClose)
                                        )
	--内容框
	self.Image_content = self:GetWidget("Image_content")	
end

function LogicGUI.TemplUnionAudit:__ShowUI( ... )
	local union_audit = Util:Label( CPPGameLib.GetString("union_audit") , Macros.TypefaceSize.popup , Macros.Color.white )	
	union_audit:SetAnchorPoint( 0.5 , 0.5 )
	self.Image_light:AddChild( union_audit )
	PosTool.Center( union_audit )

	local params =  
    {
        item_class = Modules.TemplUnionAuditItem,
        item_width = self.Image_content:GetContentWidth() ,
        item_height = 134.00,
        row = 4,
        col = 1,
        view_height = self.Image_content:GetContentHeight() - 10 ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.Image_content:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode())

    -- 申请数：
    self.union_NNT = Util:Label( CPPGameLib.GetString("union_NNT") , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
    self.union_NNT:SetAnchorPoint( 0 , 0.5 )
    self.union_NNT:SetPosition( 
    							self.Image_content:GetPositionX() - (self.Image_content:GetContentWidth()/2) , 
    							self.Image_bg:GetContentHeight() - self.Image_light:GetContentHeight() - self.union_NNT:GetContentHeight() + 5 )
    self.Image_bg:AddChild( self.union_NNT )

    self.NNT = Util:Label( "-----" , Macros.TypefaceSize.slightly , Macros.Color.content )
    PosTool.RightTo( self.union_NNT , self.NNT )
    self.Image_bg:AddChild(self.NNT)

	-- 当前成员数量：
    self.union_nowMember = Util:Label( CPPGameLib.GetString("union_nowMember") , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
    self.Image_bg:AddChild( self.union_nowMember )
	PosTool.RightTo( self.NNT , self.union_nowMember , 10 )

    self.nowMember = Util:Label( "-----" , Macros.TypefaceSize.slightly , Macros.Color.content )
    self.Image_bg:AddChild(self.nowMember)
    PosTool.RightTo( self.union_nowMember , self.nowMember )

end

function LogicGUI.TemplUnionAudit:__SetShowUI( ... )
	local lst = CallModuleFunc(ModuleType.UNION , "GetUnionApplyList")
    self.lst_list:SetDataList( lst )
    
    self.NNT:SetString( #lst  )
	PosTool.RightTo( self.NNT , self.union_nowMember , 10 )

	local memberNum , memberNumMax = CallModuleFunc(ModuleType.UNION , "GetUnionMemberNum")
	self.nowMember:SetString( memberNum .. "/" .. memberNumMax  )

	PosTool.RightTo( self.union_nowMember , self.nowMember )
end

-- 关闭按钮
function LogicGUI.TemplUnionAudit:OnButtonClose( ... )	
	self:DeleteMe()
end

--等级礼包商品
Modules = Modules or {}
Modules.TemplUnionAuditItem = Modules.TemplUnionAuditItem or BaseClass(GUI.ScrollItem)

function Modules.TemplUnionAuditItem:__init()
	self:InitTemplate(GUI.TemplateRegistry.UNION_AUDIT_ITEM)   
	self:__Getwidget()
end

function Modules.TemplUnionAuditItem:__delete()
end

function Modules.TemplUnionAuditItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	self:__SetShowUI( data )
end

function Modules.TemplUnionAuditItem:__Getwidget()
	--条目
	self.Image_item = self:GetWidget("Image_item")
	--图标
	self.Panel_icon = self:GetWidget("Panel_icon")	
	--vip背景
	self.Image_vipTips = self:GetWidget("Image_vipTips")	
	--名字
	self.Text_name = self:GetWidget("Text_name")	
	--等级
	self.Text_level = self:GetWidget("Text_level")	
	--战斗力
	self.Text_battleValue = self:GetWidget("Text_battleValue")	
	
	--同意
	self.Button_ok = Util:WidgetButton( 
                                            self:GetWidget("Button_ok") , 
                                            CPPGameLib.GetString("union_receive") , 
                                            CPPGameLib.Handler(self,self.OnButtonOk) , 
                                            nil,nil,Macros.Color.button_yellow
                                        )
	--拒绝
	self.Button_no = Util:WidgetButton( 
                                            self:GetWidget("Button_no") , 
                                            CPPGameLib.GetString("union_refuse") , 
                                            CPPGameLib.Handler(self,self.OnButtonNo),
                                            nil,nil,Macros.Color.button_yellow
                                        )
end

function Modules.TemplUnionAuditItem:__SetShowUI( data )
-- 	role_id
-- head_icon
	--头像
	local img_head = LogicGUI.RoleIcon.New(true)
	img_head:SetTouchCallback(function()
		-- CallModuleFunc(ModuleType.CHECKROLEINFO, "OpenView", BaseViewType.CHECKROLEINFO,data)
		GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.CHECKROLEINFO , false , data )
    end)
 	img_head:SetData(data.head_icon)
	self.Panel_icon:AddChild(img_head:GetNode())
	PosTool.Center(img_head)

	--名字
	Util:WidgetLabel( self.Text_name , data.name , Macros.TypefaceSize.normal , Macros.Color.Special )

	--等级
	Util:WidgetLabel( self.Text_level , CPPGameLib.GetString("common_lev1") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
	local text_level = Util:Label( data.level , Macros.TypefaceSize.normal , Macros.Color.content )
	self.Image_item:AddChild( text_level )
	PosTool.RightTo( self.Text_level , text_level )

	--战斗力
	Util:WidgetLabel( self.Text_battleValue , CPPGameLib.GetString("common_power_prefix1") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
	local text_battleValue = Util:Label( data.battle_value , Macros.TypefaceSize.normal , Macros.Color.content )
	self.Image_item:AddChild( text_battleValue )
	PosTool.RightTo( self.Text_battleValue , text_battleValue )

	--vip
	self.Image_vipTips:SetVisible(false)
	if data.vip_level ~= 0 then
		local vip = Util:Label( CPPGameLib.GetString( "vip_level" , data.vip_level ) , Macros.TypefaceSize.normal , Macros.Color.golden )
		vip:SetAnchorPoint(0.5, 0 )
		self.Image_vipTips:AddChild( vip )
		PosTool.Center(vip , -5 , 3 )
		CPPActionManager.RotateTo(vip, 0 , -45)
		self.Image_vipTips:SetVisible(true)
	end

end

function Modules.TemplUnionAuditItem:OnButtonOk()
	print("xy" , "OnButtonOk")

    if CallModuleFunc(ModuleType.UNION , "GetMyPermission" , Macros.Game.UNION.APPLY ) == 0 then
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_noRights") )
        return
    end

    local memberNum , memberNumMax = CallModuleFunc(ModuleType.UNION , "GetUnionMemberNum")
    if memberNum >= memberNumMax then
    	GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_MaxMember") )
    	return
    end

	local info = {}
	info.target_id = self.__data.role_id
	info.is_accept = 1
	CallModuleFunc(ModuleType.UNION , "GuildApplyCheckReq" , info )
end

function Modules.TemplUnionAuditItem:OnButtonNo()
	print("xy" , "OnButtonOk")

    if CallModuleFunc(ModuleType.UNION , "GetMyPermission" , Macros.Game.UNION.APPLY ) == 0 then
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_noRights") )
        return
    end

	local info = {}
	info.target_id = self.__data.role_id
	info.is_accept = 0
	CallModuleFunc(ModuleType.UNION , "GuildApplyCheckReq" , info )
end