
--
-- @brief 一键精炼
-- @author: yjg
-- @date: 2017年11月9日10:32:09
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplTreasureOnekeyIntensify = LogicGUI.TemplTreasureOnekeyIntensify or BaseClass(GUI.Template)

function LogicGUI.TemplTreasureOnekeyIntensify:__init( uid , func )
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__render_queue = Scene.RenderQueue.GRQ_UI_UP	

	self.__is_auto_load = true
	self.__open_callback = function()
	    self.uid = uid
		self.func = func
		self.__levelup = 0
		self.__max_level_up = CallModuleFunc(ModuleType.TREASURE, "GetMaxLevelupForOneKeyStrengthen", uid )

	    self:__Getwidget()
	    self:__ShowUI( uid )
	end
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_EQUIP_ONEKEY_REFINE)	
end


function LogicGUI.TemplTreasureOnekeyIntensify:__delete()
end

--获得控件
function LogicGUI.TemplTreasureOnekeyIntensify:__Getwidget( ... )
	--背景
	self.img_bg = self:GetWidget("img_bg") 
	--光
	self.img_headline = self:GetWidget("img_headline") 
	--内容背景
	self.img_btnBG = self:GetWidget("img_btnBG") 

	--数字显示框
	self.img_textBg = self:GetWidget("img_textBg") 
	--添加按钮背景
	self.btn_addBG = self:GetWidget("btn_addBG") 
	--减少按钮背景
	self.btn_subtractBG = self:GetWidget("btn_subtractBG") 
	--加十背景
	self.btn_add10BG = self:GetWidget("btn_add10BG") 
	--减十背景
	self.btn_subtract10BG = self:GetWidget("btn_subtract10BG") 

	--关闭按钮
	self.btn_close = Util:WidgetButton( self:GetWidget("btn_close") ,nil,CPPGameLib.Handler(self,self.OnBtnClose) )
	--取消按钮
	self.btn_no = Util:WidgetButton( 
										self:GetWidget("btn_no") , 
										CPPGameLib.GetString("common_cancel") ,
										CPPGameLib.Handler(self,self.OnBtnClose) , 
										Macros.TypefaceSize.largish,
										nil,Macros.Color.button_red 
										)
	--确定按钮
	self.btn_ok = Util:WidgetButton( 
										self:GetWidget("btn_ok") , 
										CPPGameLib.GetString("common_confirm") , 
										CPPGameLib.Handler(self,self.OnBtnOk) , 
										Macros.TypefaceSize.largish,
										nil,Macros.Color.button_yellow 
										)

	--添加按钮
	self.btn_add = Util:WidgetButton( self:GetWidget("btn_add") , nil , function()
		self:OnBtnAdd(1)
	end ,nil,nil,nil,nil,0) 
	--加十按钮
	self.btn_add10 = Util:WidgetButton(self:GetWidget("btn_add10") ,nil,function()
		self:OnBtnAdd(10)
	end ,nil,nil,nil,nil,0) 
	--减少按钮
	self.btn_subtract = Util:WidgetButton( self:GetWidget("btn_subtract") , nil ,function()
		self:OnBtnSubtract(1)
	end ,nil,nil,nil,nil,0) 
	--减十按钮
	self.btn_subtract10 = Util:WidgetButton( self:GetWidget("btn_subtract10") ,nil,function()
		self:OnBtnSubtract(10)
	end ,nil,nil,nil,nil,0) 
end

function LogicGUI.TemplTreasureOnekeyIntensify:__ShowUI( uid )
	--标题
	self.lab = Util:Label(CPPGameLib.GetString("hero_upgrade_onekey_levelup_btn"),Macros.TypefaceSize.popup,Macros.Color.white)
	self.img_headline:AddChild(self.lab)
	PosTool.Center(self.lab)

    --选中数量
    self.tips3 = Util:Label( CPPGameLib.GetString("hero_onekey_levelup_tips3") , Macros.TypefaceSize.normal , Macros.Color.keypoint)
    self.tips3:SetAnchorPoint(0.5,0.5)
    self.tips3:SetPosition(self.img_textBg:GetContentWidth()/2,self.img_textBg:GetContentHeight()/2  + 70 )
    self.img_textBg:AddChild(self.tips3)    

    local info = CallModuleFunc(ModuleType.TREASURE, "GetTreasure", uid )
    --选中数量
    self.union_nowGrade = Util:Label( CPPGameLib.GetString("union_nowGrade") .. info.strengthen_level  , Macros.TypefaceSize.normal , Macros.Color.keypoint)
    self.union_nowGrade:SetAnchorPoint(0.5,0.5)
    self.union_nowGrade:SetPosition(self.img_textBg:GetContentWidth()/2,self.img_textBg:GetContentHeight()/2 + 40 )
    self.img_textBg:AddChild(self.union_nowGrade)   
    
    --选中数量
    self.lab_setnum = Util:Label( self.__levelup , Macros.TypefaceSize.slightly, Macros.Color.content )
    self.lab_setnum:SetAnchorPoint(0.5,0.5)
    self.lab_setnum:SetPosition(self.img_textBg:GetContentWidth()/2,self.img_textBg:GetContentHeight()/2)
    self.img_textBg:AddChild(self.lab_setnum)    

    --消耗
    self.rebel_consume = Util:Label( CPPGameLib.GetString("rebel_consume") , Macros.TypefaceSize.normal,Macros.Color.keypoint)
    self.rebel_consume:SetAnchorPoint(0 , 0.5)
    self.rebel_consume:SetPosition(self.img_btnBG:GetContentWidth()/2 - 180  , self.img_btnBG:GetContentHeight()/2 - 45 )
    self.img_btnBG:AddChild(self.rebel_consume)    

	local icon = Util:Sprite( Util:GetMoneyIconImgData(Macros.Game.RolePropType.GOLD) )  
	icon:SetAnchorPoint( 0 , 0.5 )
	icon:SetPosition( self.img_btnBG:GetContentWidth()/2 - 120  , self.img_btnBG:GetContentHeight()/2 - 45 )
	self.img_btnBG:AddChild( icon )
	icon:SetScaleFactor( 0.75 )

    self.__label_cost_gold = Util:Label( 0 , Macros.TypefaceSize.normal,Macros.Color.content)
    self.img_btnBG:AddChild( self.__label_cost_gold )  
    PosTool.RightTo( icon , self.__label_cost_gold  )

	local icon = Util:Sprite( Util:GetMoneyIconImgData(Macros.Game.RolePropType.EXP) )  
	icon:SetAnchorPoint( 0 , 0.5 )
	icon:SetPosition( self.img_btnBG:GetContentWidth()/2  , self.img_btnBG:GetContentHeight()/2 - 45 )
	self.img_btnBG:AddChild( icon )
	icon:SetScaleFactor( 0.75 )

    self.__label_cost_exp = Util:Label( 0 , Macros.TypefaceSize.normal,Macros.Color.content)
    self.img_btnBG:AddChild( self.__label_cost_exp )  
    PosTool.RightTo( icon , self.__label_cost_exp  )

	local text = CPPGameLib.GetString("treasure_oneKeyIntensify")
	if self.__max_level_up == 0 then
		text = CPPGameLib.GetString("treasure_oneKeyIntensify_")
		self:__SetShowUI()
	end
    local oneKeyIntensify = Util:Label(text, Macros.TypefaceSize.normal, Macros.Color.keypoint)
    oneKeyIntensify:SetPosition( self.img_btnBG:GetContentWidth()/2 , self.img_btnBG:GetContentHeight()/2 - 90 )
    self.img_btnBG:AddChild( oneKeyIntensify )  
end

--添加
function LogicGUI.TemplTreasureOnekeyIntensify:OnBtnAdd(index)
	if self.__levelup + index >= self.__max_level_up then
		self.__levelup = self.__max_level_up
		self.lab_setnum:SetString(self.__levelup)
	else
		self.__levelup = self.__levelup + index
		self.lab_setnum:SetString(self.__levelup)
	end

	self:__SetShowUI()
end

--减少
function LogicGUI.TemplTreasureOnekeyIntensify:OnBtnSubtract(index)
	if self.__levelup - index <= 0  then
		self.__levelup = 0
		self.lab_setnum:SetString(0)
	else 
		self.__levelup = self.__levelup - index
		self.lab_setnum:SetString(self.__levelup)
	end
	self:__SetShowUI()
end

--设置
function LogicGUI.TemplTreasureOnekeyIntensify:__SetShowUI()
	local _, cost_exp, cost_gold
	if self.__max_level_up > 0 and self.__levelup <= 0 then
		--材料可供提升1级或以上，但没选择需要提升的等级，则显示消耗0经验
		cost_exp = 0
		cost_gold = 0
	else
		_, cost_exp, cost_gold = CallModuleFunc(ModuleType.TREASURE, "TreasureOneKeyStrengthen", self.uid, self.__levelup)
	end

	self.__label_cost_exp:SetString( cost_exp )
	self.__label_cost_gold:SetString( cost_gold )
end

--确定
function LogicGUI.TemplTreasureOnekeyIntensify:OnBtnOk()
	--材料可供提升1级或以上，但没选择需要提升的等级
	if self.__max_level_up > 0 and self.__levelup <= 0 then
		GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("divination_noSelectLevl"))
		return 
	end

	local material_list, cost_exp, cost_gold = CallModuleFunc(ModuleType.TREASURE, "TreasureOneKeyStrengthen", self.uid , self.__levelup)
	if self.func then
		self.func( self.__levelup, material_list, cost_exp, cost_gold )
	end
	self:DeleteMe()
end

--取消
function LogicGUI.TemplTreasureOnekeyIntensify:OnBtnClose()
	self:DeleteMe()
end

