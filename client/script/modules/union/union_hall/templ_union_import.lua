
--
-- @brief 公会输入框
-- @author: yjg
-- @date: 2016年10月18日19:44:36
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplUnionImport = LogicGUI.TemplUnionImport or BaseClass(GUI.Template)

function LogicGUI.TemplUnionImport:__init( fun )
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID} 
    self.__open_callback = function ()

    	self.func = fun

        self:__Getwidget()
        self:__ShowUI()

    end 
	self:InitTemplate(GUI.TemplateRegistry.UNION_IMPORT)	
end


function LogicGUI.TemplUnionImport:__delete()
end

function LogicGUI.TemplUnionImport:__Getwidget( ... )
	--背景
	self.Image_Bg = self:GetWidget("Image_Bg")
	--光
	self.Image_light = self:GetWidget("Image_light")	
	--关闭
	self.Button_close = Util:WidgetButton( 
                                            self:GetWidget("Button_close") , 
                                            nil , CPPGameLib.Handler(self,self.OnButtonClose)
                                        )
	--内容框
	self.Image_content = self:GetWidget("Image_content")	
	--输入框
    self.node_TextField = Util:WidgetTextField(self:GetWidget("node_TextField"))
	--确定	
	self.Button_establish = Util:WidgetButton( 
                                        self:GetWidget("Button_establish") , 
                                        CPPGameLib.GetString("common_confirm") , 
                                        CPPGameLib.Handler(self,self.OnButtonEstablish),
                                        nil,nil, Macros.Color.button_yellow
                                    )



	--取消	
	self.Button_cancel = Util:WidgetButton( 
                                        self:GetWidget("Button_cancel") , 
                                        CPPGameLib.GetString("common_cancel") , 
                                        CPPGameLib.Handler(self,self.OnButtonCancel),
                                        nil,nil, Macros.Color.button_red
                                    )

end

function LogicGUI.TemplUnionImport:__ShowUI( ... )
	self.union_iconWall = Util:Label( CPPGameLib.GetString("union_messageBoard") , Macros.TypefaceSize.popup , Macros.Color.white )	
	self.union_iconWall:SetAnchorPoint( 0.5 , 0.5 )
	self.Image_light:AddChild( self.union_iconWall )
	PosTool.Center( self.union_iconWall )

    --输入框
    self.node_TextField:SetMaxLength(40)
    self.node_TextField:SetMaxLengthEnabled(true)
    self.node_TextField:SetTextColor(Macros.Color.content)
    self.node_TextField:SetPlaceHolderText(CPPGameLib.GetString("union_import"))
	self.node_TextField:SetTextHorizontalAlign(UITextFieldEx.ALIGIN_TOP)

	local union_editingTips = Util:Label( CPPGameLib.GetString("union_editingTips") , Macros.TypefaceSize.slightly , Macros.Color.keypoint )	
	union_editingTips:SetAnchorPoint( 0.5 , 0 )
	self.Image_Bg:AddChild( union_editingTips )
	PosTool.CenterBottom( union_editingTips , 0 , 70 )

end

function LogicGUI.TemplUnionImport:__SetShowUI( ... )
	local lst = CallModuleFunc(ModuleType.UNION , "GetUnionApplyList")
    self.lst_list:SetDataList( lst )
    self.NNT:SetString( #lst .. "/" .. ConfigAdapter.Common.GetUnionEntryNumMax()  )
end

-- 关闭按钮
function LogicGUI.TemplUnionImport:OnButtonClose( ... )	
	self:DeleteMe()
end

-- 取消
function LogicGUI.TemplUnionImport:OnButtonCancel( ... )	
	self:DeleteMe()
end

-- 确定按钮
function LogicGUI.TemplUnionImport:OnButtonEstablish( ... )	
	local name , isop = LanguageFilter.NameFilter( self.node_TextField:GetText() )


	if isop then
		if self.func then
			self.func( self.node_TextField:GetText() )
		end
		self:DeleteMe()
	else
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_inconformity") )
	end
end

-- 确定按钮
function LogicGUI.TemplUnionImport:SetName( name )
	self.union_iconWall:SetString(name)
end


