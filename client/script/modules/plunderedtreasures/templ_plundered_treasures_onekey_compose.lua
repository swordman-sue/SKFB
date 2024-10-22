LogicGUI = LogicGUI or {}

LogicGUI.TemplPlunderedTreasuresOneKeyCompose = LogicGUI.TemplPlunderedTreasuresOneKeyCompose or BaseClass(GUI.Template)

function LogicGUI.TemplPlunderedTreasuresOneKeyCompose:__init(data)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:InitMainUI(data)
	end
	self:InitTemplate(GUI.TemplateRegistry.PLUNDERED_TREASURES_ONEKEY_COMPOSE)
end

function LogicGUI.TemplPlunderedTreasuresOneKeyCompose:InitMainUI(data)
	self.__data = data
	--默认选中数量
	self.__choose_num = 1
	--配置表数据
	local type,item_Config = ObjIDTool.GetInfo(data.item_id)
	--标题
	Util:WidgetLabel(self:GetWidget("Text_tile"),CPPGameLib.GetString("plunderedtreasures_onekey_compose_title"), Macros.TypefaceSize.headline, Macros.Color.btn)
	--关闭按钮
	Util:WidgetButton(self:GetWidget("Button_close"),"",function ()
		self:DeleteMe()
	end)
	--头像
	local icon = LogicGUI.IconLayer.New()
	self:GetWidget("Panel_head"):AddChild(icon:GetNode())
	PosTool.Center(icon)
	icon:SetData({id = data.item_id})		
	--可合成
	Util:WidgetLabel(self:GetWidget("Text_CouldComposeNum"),CPPGameLib.GetString("plunderedtreasures_onekey_could_compose"),Macros.TypefaceSize.normal,Macros.Color.keypoint)
	--可合成数量
	local txt_composenum = self:GetWidget("Text_ComposeNum")	
	Util:WidgetLabel(txt_composenum,self.__data.could_compose_num,Macros.TypefaceSize.normal,Macros.Color.content)
	--名字
	local name = Util:Name(item_Config.name or "",Macros.TypefaceSize.largish,item_Config.quality)
	name:SetAnchorPoint(0,0.5)
	self:GetWidget("Panel_name"):AddChild(name)
	PosTool.LeftCenter(name)
	--数量按钮
	--加1
	Util:WidgetButton(self:GetWidget("Button_add"),"",function ()
		self:OnBtnAdd(1)
	end,nil,nil,nil,nil,0)
	--加10
	Util:WidgetButton(self:GetWidget("Button_add10"),"",function ()
		self:OnBtnAdd(10)
	end,nil,nil,nil,nil,0)
	--减1
	Util:WidgetButton(self:GetWidget("Button_sub"),"",function ()
		self:OnBtnSub(1)
	end,nil,nil,nil,nil,0)
	--减10
	Util:WidgetButton(self:GetWidget("Button_sub10"),"",function ()
		self:OnBtnSub(10)
	end,nil,nil,nil,nil,0)
	--选中数字
	self.__num_txt = self:GetWidget("Text_chooseNum")
	Util:WidgetLabel(self.__num_txt,1,Macros.TypefaceSize.normal,Macros.Color.content)
	--取消
	Util:WidgetButton(self:GetWidget("Button_cancel"),CPPGameLib.GetString("common_cancel"),function ()
		self:DeleteMe()
	end,Macros.TypefaceSize.popup,Macros.Color.btn, Macros.Color.button_red)
	--确定
	Util:WidgetButton(self:GetWidget("Button_buy"),CPPGameLib.GetString("common_confirm"),function ()
		self:DeleteMe()	
		if self.__data.func then
			self.__data.func(self.__choose_num)
		end
	end,Macros.TypefaceSize.popup,Macros.Color.btn, Macros.Color.button_yellow)
end

--增加数量
function LogicGUI.TemplPlunderedTreasuresOneKeyCompose:OnBtnAdd(num)
	if self.__choose_num >= self.__data.could_compose_num then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("plunderedtreasures_onekey_compose_tips"))
	elseif self.__choose_num + num >= self.__data.could_compose_num then
		self.__choose_num = self.__data.could_compose_num			
	else
		self.__choose_num = self.__choose_num + num
	end
	self.__num_txt:SetText(self.__choose_num)
end

--减少数量
function LogicGUI.TemplPlunderedTreasuresOneKeyCompose:OnBtnSub(num)
	if self.__choose_num <= 1 then
		return 
	elseif self.__choose_num - num <= 1  then
		self.__choose_num = 1
	else 
		self.__choose_num = self.__choose_num - num		
	end
	self.__num_txt:SetText(self.__choose_num)
end


