LogicGUI = LogicGUI or {}

LogicGUI.TemplSpiritOneKeyTrain = LogicGUI.TemplSpiritOneKeyTrain or BaseClass(GUI.Template)

function LogicGUI.TemplSpiritOneKeyTrain:__init(data)
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function ()
		self:InitMainUI(data)
	end
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_SPIRIT_ONEKEY_TRAIN)
end

function LogicGUI.TemplSpiritOneKeyTrain:InitMainUI(data)
	self.__data = data
	--标题
	Util:WidgetLabel(self:GetWidget("Text_title"),CPPGameLib.GetString("spirit_onekey_train"), Macros.TypefaceSize.tab, Macros.Color.btn)
	--关闭按钮
	Util:WidgetButton(self:GetWidget("Button_close"),"",function ()
		self:DeleteMe()
	end)
	--提示
	local txt_tips = self:GetWidget("Text_tips")
	Util:WidgetLabel(txt_tips,CPPGameLib.GetString("spirit_onekey_train_tips"),Macros.TypefaceSize.normal,Macros.Color.content)
	--消耗
	Util:WidgetLabel(self:GetWidget("Text_cost"),CPPGameLib.GetString("hero_recruitCostText"),Macros.TypefaceSize.largish,Macros.Color.keypoint)	
	--取消
	Util:WidgetButton(self:GetWidget("Button_cancel"),CPPGameLib.GetString("common_cancel"),function ()
		self:DeleteMe()
	end,Macros.TypefaceSize.popup,Macros.Color.btn,Macros.Color.button_red)
	--初始化文字
	for i=1,4 do
		--图片
		local image_pos = self:GetWidget("Image_pos"..i)
		local sp = Resource.PathTool.GetItemPath(10030 + i - 1)
		image_pos:SetImage(sp,TextureResType.LOCAL)
		--数量
		Util:WidgetLabel(self:GetWidget("Text_pos"..i),self.__data.cost_num or 1,Macros.TypefaceSize.normal,Macros.Color.content)			
	end
	--确定
	Util:WidgetButton(self:GetWidget("Button_sure"),CPPGameLib.GetString("common_confirm"),function ()
		self:DeleteMe()
		--回调
		if self.__data.func then
			self.__data.func()
		end		
	end,Macros.TypefaceSize.popup,Macros.Color.btn,Macros.Color.button_yellow)
end

