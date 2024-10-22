
--
-- @brief 一键精炼
-- @author: yjg
-- @date: 2017年11月9日10:32:09
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplEquipOnekeyRefine = LogicGUI.TemplEquipOnekeyRefine or BaseClass(GUI.Template)

function LogicGUI.TemplEquipOnekeyRefine:__init( uid , func )
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__render_queue = Scene.RenderQueue.GRQ_UI_UP	

	self.__is_auto_load = true
	self.__open_callback = function()
		self.func = func
		self.pitchNum = 0
	    self:__Getwidget()
	    self.uid = uid
	    self:__ShowUI( uid )
	end
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_EQUIP_ONEKEY_REFINE)	
end


function LogicGUI.TemplEquipOnekeyRefine:__delete()
end

--获得控件
function LogicGUI.TemplEquipOnekeyRefine:__Getwidget( ... )
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

function LogicGUI.TemplEquipOnekeyRefine:__ShowUI( uid )
	--标题
	self.lab = Util:Label(CPPGameLib.GetString("hero_upgrade_onekey_levelup_btn"),Macros.TypefaceSize.popup,Macros.Color.white)
	self.img_headline:AddChild(self.lab)
	PosTool.Center(self.lab)

    --选中数量
    self.tips3 = Util:Label( CPPGameLib.GetString("hero_onekey_levelup_tips3") , Macros.TypefaceSize.normal , Macros.Color.keypoint)
    self.tips3:SetAnchorPoint(0.5,0.5)
    self.tips3:SetPosition(self.img_textBg:GetContentWidth()/2,self.img_textBg:GetContentHeight()/2 + 70 )
    self.img_textBg:AddChild(self.tips3)    


    local info = CallModuleFunc(ModuleType.EQUIP, "GetEquip", uid )
    --选中数量
    self.union_nowGrade = Util:Label( CPPGameLib.GetString("union_nowGrade") .. info.refine_level  , Macros.TypefaceSize.normal , Macros.Color.keypoint)
    self.union_nowGrade:SetAnchorPoint(0.5,0.5)
    self.union_nowGrade:SetPosition(self.img_textBg:GetContentWidth()/2,self.img_textBg:GetContentHeight()/2 + 40 )
    self.img_textBg:AddChild(self.union_nowGrade)   
    
    --选中数量
    self.lab_setnum = Util:Label( self.pitchNum , Macros.TypefaceSize.slightly , Macros.Color.content )
    self.lab_setnum:SetAnchorPoint(0.5,0.5)
    self.lab_setnum:SetPosition(self.img_textBg:GetContentWidth()/2,self.img_textBg:GetContentHeight()/2)
    self.img_textBg:AddChild(self.lab_setnum)    

    --消耗
    self.rebel_consume = Util:Label( CPPGameLib.GetString("rebel_consume") , Macros.TypefaceSize.normal,Macros.Color.keypoint)
    self.rebel_consume:SetAnchorPoint(0 , 0.5)
    self.rebel_consume:SetPosition(self.img_btnBG:GetContentWidth()/2 - 180  , self.img_btnBG:GetContentHeight()/2 - 35 )
    self.img_btnBG:AddChild(self.rebel_consume)    

    local itemLst = ConfigAdapter.Common.GetRefineItemLst()

    local t1 = {
    		[1] = { x = self.img_btnBG:GetContentWidth()/2 - 120  , y = self.img_btnBG:GetContentHeight()/2 - 35 },
    		[2] = { x = self.img_btnBG:GetContentWidth()/2 + 20  , y = self.img_btnBG:GetContentHeight()/2 - 35 },
    		[3] = { x = self.img_btnBG:GetContentWidth()/2 - 120  , y = self.img_btnBG:GetContentHeight()/2 - 75 },
    		[4] = { x = self.img_btnBG:GetContentWidth()/2 + 20  , y = self.img_btnBG:GetContentHeight()/2 - 75 },
	}


    self.iconLst = {}
    for i,v in ipairs( itemLst ) do
	    local icon = LogicGUI.IconLayer.New( false )
		icon:SetData({id = v})
	    icon:ShowIconQuality(false)
	    icon:ShowIconShade(false)
	    icon:ShowIconBG(false)
	    self.img_btnBG:AddChild(icon:GetNode())
	    icon:SetAnchorPoint( 0 , 0.5 )
	    icon:SetScaleFactor( 0.5 )
	    icon:SetPosition( t1[i].x , t1[i].y )

	    self.iconLst[v] = Util:Label( 0 , Macros.TypefaceSize.normal,Macros.Color.content)
	    self.img_btnBG:AddChild(self.iconLst[v])  
	    PosTool.RightTo( icon , self.iconLst[v] , -35 )

    end

	self.itemNum = CallModuleFunc(ModuleType.EQUIP, "ToUidGetEquipRefineMax", uid )
end

--添加
function LogicGUI.TemplEquipOnekeyRefine:OnBtnAdd(index)
	if self.pitchNum + index >= self.itemNum then
		self.pitchNum = self.itemNum
		self.lab_setnum:SetString(self.pitchNum)
	else
		self.pitchNum = self.pitchNum + index
		self.lab_setnum:SetString(self.pitchNum)
	end

	self:__SetShowUI()
end

--减少
function LogicGUI.TemplEquipOnekeyRefine:OnBtnSubtract(index)
	if self.pitchNum - index <= 0  then
		self.pitchNum = 0
		self.lab_setnum:SetString(0)
	else 
		self.pitchNum = self.pitchNum - index
		self.lab_setnum:SetString(self.pitchNum)
	end
	self:__SetShowUI()
end

--设置
function LogicGUI.TemplEquipOnekeyRefine:__SetShowUI()
	local lst = CallModuleFunc(ModuleType.EQUIP, "OneKeyRefine", self.uid , self.pitchNum)
	for i,v in pairs( lst ) do
		self.iconLst[i]:SetString( v.num )
	end
end

--确定
function LogicGUI.TemplEquipOnekeyRefine:OnBtnOk()
	local lst = CallModuleFunc(ModuleType.EQUIP, "OneKeyRefine", self.uid , self.pitchNum)
	if self.func then
		self.func( self.pitchNum , lst )
	end
	self:DeleteMe()
end

--取消
function LogicGUI.TemplEquipOnekeyRefine:OnBtnClose()
	self:DeleteMe()
end

