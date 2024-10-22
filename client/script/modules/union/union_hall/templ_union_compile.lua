
--
-- @brief 编辑弹窗
-- @author: yjg
-- @date: 2016年10月18日15:14:15
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplUnionCompile = LogicGUI.TemplUnionCompile or BaseClass(GUI.Template)


function LogicGUI.TemplUnionCompile:__init()
	self.__is_auto_load = true
	self.__open_callback = function()
		self.__shield_layer = LogicGUI.ShieldLayer.New(function()
			-- self:DeleteMe()
		end)
		self.__root_widget:AddChild(self.__shield_layer:GetNode() , -1)
	
		self:__Getwidget()
		self:__ShowUI()
	end

	self:InitTemplate(GUI.TemplateRegistry.UNION_COMPILE)	
end


function LogicGUI.TemplUnionCompile:__delete()
	if self.__shield_layer then
		self.__shield_layer:DeleteMe()
		self.__shield_layer = nil
	end
end

function LogicGUI.TemplUnionCompile:SetData(data)
end

function LogicGUI.TemplUnionCompile:__Getwidget( ... )
	--背景图
	self.Image_bg = self:GetWidget("Image_bg")
	--关闭按钮
	self.Button_close = Util:WidgetButton(self:GetWidget("Button_close") , nil , CPPGameLib.Handler(self,self.OnButtonClose) )
	--标题
	self.Image_headline = self:GetWidget("Image_headline")
	--内容
	self.Image_content = self:GetWidget("Image_content")
	--提示
	self.Text_maxTips = self:GetWidget("Text_maxTips")
	--取消按钮
	self.Button_no = Util:WidgetButton(self:GetWidget("Button_no") , CPPGameLib.GetString("common_cancel") ,CPPGameLib.Handler(self,self.OnButtonClose) )
	--确定按钮
	self.Button_yes = Util:WidgetButton(self:GetWidget("Button_yes") , CPPGameLib.GetString("common_confirm") ,CPPGameLib.Handler(self,self.OnButtonYes) )
end

function LogicGUI.TemplUnionCompile:__ShowUI( ... )	
	--标题
	self.headline = Util:Label(" ", nil, Macros.Color.content)
	self.headline:SetPosition(self.Image_headline:GetContentWidth()/2 ,self.Image_headline:GetContentHeight()/2)
	self.Image_headline:AddChild(self.headline)
	--提示
	self.tips = Util:Label(" ")
	self.tips:SetPosition(self.Text_maxTips:GetContentWidth()/2,self.Text_maxTips:GetContentHeight()/2)
	self.Text_maxTips:AddChild(self.tips)
	--输入框
	self.textField = Util:TextField()
	self.textField:SetContentSize(self.Image_content:GetContentWidth() - 10 , self.Image_content:GetContentHeight() - 10 )
	self.Image_content:AddChild(self.textField)
	PosTool.Center(self.textField)
end

--设置标题
function LogicGUI.TemplUnionCompile:SetHeadline( lab )	
	self.headline:SetString(lab)
end

--设置提示
function LogicGUI.TemplUnionCompile:SetTips( lab )	
	self.tips:SetString(lab)
end

--设置占位符和最大
function LogicGUI.TemplUnionCompile:SetTextField( lab , max)	
	if lab then
		self.textField:SetPlaceHolderText(lab)
	end
	if max then
		self.textField:SetMaxLength(max)
	end
end

--设置回调
function LogicGUI.TemplUnionCompile:SetFun(fun)	
	self.fun = fun
end

-- 关闭按钮
function LogicGUI.TemplUnionCompile:OnButtonClose( ... )	
	self:DeleteMe()
end

--确定按钮
function LogicGUI.TemplUnionCompile:OnButtonYes( ... )
	if self.fun then
		self.fun(self.textField:GetText())
	end
	self:OnButtonClose()
end