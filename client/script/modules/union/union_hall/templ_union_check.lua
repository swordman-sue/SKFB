
--
-- @brief 申请审核
-- @author: yjg
-- @date: 2016年10月18日19:44:36
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplUnionCheck = LogicGUI.TemplUnionCheck or BaseClass(GUI.Template)


function LogicGUI.TemplUnionCheck:__init()
	CallModuleFunc(ModuleType.UNION , "GuildApplyListReq")

	self.ShowList = self:BindGlobalEvent(Macros.Event.UnionModule.UNION_CHECK,function ( _,lst ,type_)
		self:SetShowUI(lst,type_)
	end)

	self.UpList = self:BindGlobalEvent(Macros.Event.UnionModule.UNION_AUDIT,function ( _,info )
		self:UpShowList(info)
	end)

	self.__is_auto_load = true
	self.__open_callback = function()
		self.__shield_layer = LogicGUI.ShieldLayer.New(function()
			-- self:DeleteMe()
		end)
		self.__root_widget:AddChild(self.__shield_layer:GetNode() , -1)
	
		self:__Getwidget()
		self:__ShowUI()
	end

	self:__AddPreloadTemplate(GUI.TemplateRegistry.UNION_CHECK_ITEM)
	self:InitTemplate(GUI.TemplateRegistry.UNION_CHECK)	
end


function LogicGUI.TemplUnionCheck:__delete()
	if self.__shield_layer then
		self.__shield_layer:DeleteMe()
		self.__shield_layer = nil
	end
end

function LogicGUI.TemplUnionCheck:SetData(data)
end

function LogicGUI.TemplUnionCheck:__Getwidget( ... )
	--背景
	self.Image_1 = self:GetWidget("Image_1")
	--关闭按钮
	self.Button_close = Util:WidgetButton(self:GetWidget("Button_close"), nil , CPPGameLib.Handler(self,self.OnButtonClose))
	--标题
	self.Image_headline = self:GetWidget("Image_headline")
	--内容
	self.Image_content = self:GetWidget("Image_content")
	--关闭
	self.Button_ok = Util:WidgetButton(self:GetWidget("Button_ok"), CPPGameLib.GetString("union_close") , CPPGameLib.Handler(self,self.OnButtonClose))
	--数量
	self.Text_member = self:GetWidget("Text_member")
end

function LogicGUI.TemplUnionCheck:__ShowUI( ... )	
	--标题
	local headline = Util:Label(CPPGameLib.GetString("union_audit"),Macros.TypefaceSize.popup,Macros.Color.headline)
	headline:SetPosition(self.Image_headline:GetContentWidth()/2 , self.Image_headline:GetContentHeight()/2)
	self.Image_headline:AddChild(headline)

	--成员数量
	self.memberNum = Util:Label(CPPGameLib.GetString("union_num",0,0))
	self.memberNum:SetAnchorPoint(0,0.5)
	self.memberNum:SetPosition(self.Text_member:GetContentWidth()/2 , self.Text_member:GetContentHeight()/2)
	self.Text_member:AddChild(self.memberNum)

	--列表
	local params =  
	{
	  item_class = Modules.TemplUnionCheckItem,
	  item_width = 580,
	  item_height = 124,
	  row = 4,
	  col = 1,
	  item_space_r = 1,
	  selectable = true,
	  view_width = self.Image_content:GetContentWidth() ,
	  view_height = self.Image_content:GetContentHeight() - 10 ,
	}
	-- 创建参数
	self.__zones_scrollview = GUI.ScrollView.New(params)
	self.__zones_scrollview:SetPosition(self.Image_content:GetContentWidth()/2 , self.Image_content:GetContentHeight()/2 )
	self.Image_content:AddChild(self.__zones_scrollview:GetNode())
end

function LogicGUI.TemplUnionCheck:SetShowUI( lst , type_)
	self.memberNum:SetString( CPPGameLib.GetString("union_num",#lst,0) )
	if type_ == true then
		self.__zones_scrollview:SetDataList( lst )
	end
end

function LogicGUI.TemplUnionCheck:UpShowList( info )
	--列表相关调整
	local del_index
	self.__zones_scrollview:ItemDataIter(function(i, item_data)
	    if item_data.roleid == info.roleid then
	    	del_index = i
	    	return true
	    end
	end)
	if del_index then
		self.__zones_scrollview:DetachItem(del_index)
		self.__zones_scrollview:RefreshLayout()
	end
end

-- 关闭按钮
function LogicGUI.TemplUnionCheck:OnButtonClose( ... )	
	self:DeleteMe()
end



Modules = Modules or {}

Modules.TemplUnionCheckItem = Modules.TemplUnionCheckItem or BaseClass(GUI.ScrollItem)

function Modules.TemplUnionCheckItem:__init(data)
	self:InitTemplate(GUI.TemplateRegistry.UNION_CHECK_ITEM) 
	self:__Getwidget()
end


function Modules.TemplUnionCheckItem:__delete(  )
	-- body
end

function Modules.TemplUnionCheckItem:SetData( data )
	GUI.ScrollItem.SetData(self, data)

	self:__ShowUI(data)
end

function Modules.TemplUnionCheckItem:__Getwidget( info )
	--背景
	self.Image_8_0 = self:GetWidget("Image_8_0")
	--图标
	self.Panel_icon = self:GetWidget("Panel_icon")
	--名字
	self.Text_name = self:GetWidget("Text_name")
	--等级
	self.Text_lv = self:GetWidget("Text_lv")
	--战斗力
	self.Text_hit = self:GetWidget("Text_hit")
	--拒绝按钮
	self.Button_no = Util:WidgetButton(self:GetWidget("Button_no") , CPPGameLib.GetString("union_refuse") , CPPGameLib.Handler(self,self.OnButtonNo) )
	--同意按钮	
	self.Button_ok = Util:WidgetButton(self:GetWidget("Button_ok") , CPPGameLib.GetString("union_receive") , CPPGameLib.Handler(self,self.OnButtonOk) )
end

function Modules.TemplUnionCheckItem:__ShowUI( data )
	--名字
	local name = Util:Label(data.name,Macros.TypefaceSize.largish,Macros.Color.content)
	name:SetPosition(self.Text_name:GetContentWidth()/2 , self.Text_name:GetContentHeight()/2)
	name:SetAnchorPoint(0,0.5)
	self.Text_name:AddChild(name)

	--等级
	local lv = Util:Label( CPPGameLib.GetString("common_lev2",data.level))
	lv:SetPosition(self.Text_lv:GetContentWidth()/2 , self.Text_lv:GetContentHeight()/2)
	lv:SetAnchorPoint(0,0.5)
	self.Text_lv:AddChild(lv)

	--战斗力
	local hit = Util:Label( CPPGameLib.GetString("common_power_prefix",data.battle_value))
	hit:SetPosition(self.Text_hit:GetContentWidth()/2 , self.Text_hit:GetContentHeight()/2)
	hit:SetAnchorPoint(0,0.5)
	self.Text_hit:AddChild(hit)

	--头像
	local sp = LogicGUI.RoleIcon.New()
	sp:SetData(data.head_icon)
	sp:SetPosition(self.Panel_icon:GetContentWidth()/2 , self.Panel_icon:GetContentHeight()/2)
	sp:SetTouchCallback(function(info)
	     -- CallModuleFunc(ModuleType.CHECKROLEINFO, "OpenView", BaseViewType.CHECKROLEINFO,data)
	    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.CHECKROLEINFO , false , data )
	end)
	self.Panel_icon:AddChild(sp:GetNode())

end

--拒绝
function Modules.TemplUnionCheckItem:OnButtonNo( ... )
	local info = {}
	info.target_id = self.__data.role_id
	info.is_accept = 0
	CallModuleFunc(ModuleType.UNION , "GuildApplyCheckReq",info)
end

--接受
function Modules.TemplUnionCheckItem:OnButtonOk( ... )
	local info = {}
	info.target_id = self.__data.role_id
	info.is_accept = 1
	CallModuleFunc(ModuleType.UNION , "GuildApplyCheckReq",info)	
end
