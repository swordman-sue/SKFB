--
-- @brief 物品背包使用 
-- @author: yjg
-- @date: 2016年8月20日18:31:58
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplPackOneuse = LogicGUI.TemplPackOneuse or BaseClass(GUI.Template)

function LogicGUI.TemplPackOneuse:__init(id,direct,itemResolveId , func )
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}	
	self.__open_callback = function()
		self.id = id
		self.pitchNum = 1
		self.func = func
		--数量
		self.itemNum = CallModuleFunc(ModuleType.Pack, "GetNumByItemID", self.id)

		self:__Getwidget()
		self:__ShowUI(data)

		self:BindGlobalEvent(Macros.Event.PackModule.USE_ITEM_SUCCESS, function(_, pre_section_info)
			self:OnBtnClose()
		end)  
		
	    --关闭界面事件
	    self:BindGlobalEvent(Macros.Event.ApproachOfAchievingModule.DELETE_APPROACHVIEW, function()
	        self:DeleteMe()
	    end)

	end
	if itemResolveId then
		self.itemResolveId = itemResolveId
	end	
	self:InitTemplate(GUI.TemplateRegistry.PACK_ONEUSE)	
end

function LogicGUI.TemplPackOneuse:__delete()
	if self.__shield_layer then
		self.__shield_layer:DeleteMe()
		self.__shield_layer = nil
	end
end

function LogicGUI.TemplPackOneuse:__Getwidget()
	--背景
	self.img_bg = self:GetWidget("img_bg") 
	--光
	self.img_headline = self:GetWidget("img_headline") 
	--内容背景
	self.img_btnBG = self:GetWidget("img_btnBG") 
	--头像
	self.node_head = self:GetWidget("node_head") 
	--数字显示框
	self.img_textBg = self:GetWidget("img_textBg") 
	self.img_textBg:SetContentSize( self.img_textBg:GetContentWidth()/2 , self.img_textBg:GetContentHeight() )
	--添加按钮背景
	self.btn_addBG = self:GetWidget("btn_addBG") 
	PosTool.MoveBy( self:GetWidget("btn_add")  , -30 , 0 )
	--减少按钮背景
	self.btn_subtractBG = self:GetWidget("btn_subtractBG") 
	PosTool.MoveBy( self:GetWidget("btn_subtract")  , 30 , 0 )
	--加十背景
	self.btn_add10BG = self:GetWidget("btn_add10BG") 
	PosTool.MoveBy( self:GetWidget("btn_add10")  , -40 , 0 )
	--减十背景
	self.btn_subtract10BG = self:GetWidget("btn_subtract10BG") 
	PosTool.MoveBy( self:GetWidget("btn_subtract10")  , 40 , 0 )
	--名字
	self.Text_name = self:GetWidget("Text_name") 
	--数量
	self.Text_num = self:GetWidget("Text_num") 
	
	--关闭按钮
	self.btn_close = Util:WidgetButton( self:GetWidget("btn_close") ,nil,CPPGameLib.Handler(self,self.OnBtnClose) )
	--取消按钮
	self.btn_no = Util:WidgetButton( 
										self:GetWidget("btn_no") , 
										CPPGameLib.GetString("common_cancel") ,
										CPPGameLib.Handler(self,self.OnBtnClose) , 
										Macros.TypefaceSize.largish,
										nil , Macros.Color.button_red

										)
	--确定按钮
	self.btn_ok = Util:WidgetButton( 
										self:GetWidget("btn_ok") , 
										CPPGameLib.GetString("common_confirm") , 
										CPPGameLib.Handler(self,self.OnBtnOk) , 
										Macros.TypefaceSize.largish,
										nil , Macros.Color.button_yellow
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

function LogicGUI.TemplPackOneuse:__ShowUI()
	--信息
	local type_,info = ObjIDTool.GetInfo(self.id)

	--名字
	local name = Util:Name(info.name , Macros.TypefaceSize.normal , info.quality)
	name:SetPosition(self.Text_name:GetContentWidth() / 2 ,self.Text_name:GetContentHeight() / 2)
	name:SetAnchorPoint(0,0.5)
	self.Text_name:AddChild(name)

	--数量text
	local numText = Util:Label(CPPGameLib.GetString("pack_num") , Macros.TypefaceSize.normal , Macros.Color.keypoint ) 
	numText:SetPosition(self.Text_num:GetContentWidth() / 2 ,self.Text_num:GetContentHeight() / 2)
	numText:SetAnchorPoint(0,0.5)
	self.Text_num:AddChild(numText)

	local num = Util:Label(self.itemNum,nil, Macros.Color.content )
	self.Text_num:AddChild(num)
	PosTool.RightTo(numText,num)

	--图标
	local icon = LogicGUI.IconLayer.New(true)
	self.node_head:AddChild(icon:GetNode())
	PosTool.Center(icon)
	icon:SetData({id = self.id})

	--标题
	self.lab = Util:Label(CPPGameLib.GetString("pack_oneuseHeadline"),Macros.TypefaceSize.popup,Macros.Color.lilyWhite)
	self.img_headline:AddChild(self.lab)
	PosTool.Center(self.lab)
	
    --选中数量
    self.lab_setnum = Util:Label( self.pitchNum,Macros.TypefaceSize.slightly , Macros.Color.content )
    self.lab_setnum:SetAnchorPoint(0.5,0.5)
    self.lab_setnum:SetPosition(self.img_textBg:GetContentWidth()/2,self.img_textBg:GetContentHeight()/2)
    self.img_textBg:AddChild(self.lab_setnum)    

    --分解
    if self.itemResolveId then    	
    	local textResolve = self:GetWidget("Text_resolve") 
    	textResolve:SetVisible(true)
    	Util:WidgetLabel(textResolve,CPPGameLib.GetString("Awaken_Equip_ResolveText"),18, Macros.Color.keypoint )
    	self.Text_godSoulNum = self:GetWidget("Text_godSoul") 
    	self.Text_godSoulNum:SetVisible(true)   	
    	local itemConfig = CPPGameLib.GetConfig("AwakeningItem",self.itemResolveId)
    	if not itemConfig then
    		return
    	end
    	Util:WidgetLabel(self.Text_godSoulNum,self.pitchNum * itemConfig.god_soul,18, Macros.Color.content )
    	self.godSoulNum = itemConfig.god_soul
    end

    local btn_add10 = self:GetWidget("btn_add10")
    local maxSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "bg_sp_buttonBg" )
    local btnBg = Util:Sprite9( maxSp )
    btnBg:SetContentSize( 55 , 45 )
    btnBg:SetAnchorPoint( 0.5,0.5 )
	btnBg:SetPosition( btn_add10:GetPositionX() + 60  , btn_add10:GetPositionY() )
	self.img_bg:AddChild( btnBg )

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_num_max")
    local btn = Util:Button( sp ,  function ( ... )
    	self:OnBtnAdd( ConfigAdapter.Common.GetOnceUserMax() )
    end )
    btnBg:AddChild(btn) 
    PosTool.Center( btn , 0 , 2 )
end

--添加
function LogicGUI.TemplPackOneuse:OnBtnAdd(index)
	if self.pitchNum >= self.itemNum then
		return
	elseif self.pitchNum + index >= ConfigAdapter.Common.GetOnceUserMax() and self.pitchNum + index <= self.itemNum then 
		self.pitchNum = ConfigAdapter.Common.GetOnceUserMax()
		self.lab_setnum:SetString(self.pitchNum)
	elseif self.pitchNum + index >= self.itemNum then
		self.pitchNum = self.itemNum
		self.lab_setnum:SetString(self.pitchNum)
	else
		self.pitchNum = self.pitchNum + index
		self.lab_setnum:SetString(self.pitchNum)
	end
	if self.itemResolveId then
		self.Text_godSoulNum:SetText(self.pitchNum * self.godSoulNum)
	end
end

--减少
function LogicGUI.TemplPackOneuse:OnBtnSubtract(index)
	if self.pitchNum <= 1 then
		return 
	elseif self.pitchNum - index <= 1  then
		self.pitchNum = 1
		self.lab_setnum:SetString(1)
	else 
		self.pitchNum = self.pitchNum - index
		self.lab_setnum:SetString(self.pitchNum)
	end
	if self.itemResolveId then
		self.Text_godSoulNum:SetText(self.pitchNum * self.godSoulNum)
	end
end

--确定
function LogicGUI.TemplPackOneuse:OnBtnOk()

	local info = {}
	if self.itemResolveId then
		info.item_id = self.id
		info.item_num = self.pitchNum
		CallModuleFunc(ModuleType.Pack, "ItemDecomposeReq",info)
		--关闭界面
		self:OnBtnClose()
	else
        CallModuleFunc(ModuleType.SUNDRY, "GiftBagPackVacancy", self.id , self.pitchNum , true , function ( text )
            if #text == 0 then
				info.item_id = self.id
				info.item_num = self.pitchNum
				info.select_gift_id = 0
				CallModuleFunc(ModuleType.Pack, "ItemUseReq",info)
            end
        end )
	end	
	
end

--取消
function LogicGUI.TemplPackOneuse:OnBtnClose()
	if self.func then
		self.func()
	end
	self:DeleteMe()
end

function LogicGUI.TemplPackOneuse:SetName( text )
	self.lab:SetString(CPPGameLib.GetString(text))
end