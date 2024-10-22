
--
-- @brief 物品预览获得
-- @author: yjg
-- @date: 2016年9月19日10:02:20
--
Modules = Modules or {}
Modules.TemplResolveEquip = Modules.TemplResolveEquip or BaseClass(GUI.Template)

function Modules.TemplResolveEquip:__init(data,fun , headline , tips ) 
	self.__is_auto_load = true
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_callback = function()
		self.fun = fun
		self:__Getwidget()
		self:__DataTidy(data)
		self:SetName( headline or " " )
		self:SetTips( tips or " " )
	end
	self:InitTemplate(GUI.TemplateRegistry.RECYCLE_PREVIEW)	
end

function Modules.TemplResolveEquip:__delete()
end

function Modules.TemplResolveEquip:__Getwidget()
	--背景
	self.img_selectBG = self:GetWidget("img_selectBG")
	--标题
	self.img_headline = self:GetWidget("img_headline")
	--关闭按钮
	self.btn_close = Util:WidgetButton( self:GetWidget("btn_close") , "  " , CPPGameLib.Handler(self,self.OnBtnclose) )
	--列表容器
	self.img_dataBG = self:GetWidget("img_dataBG")
	--确定按钮
	self.btn_ok = Util:WidgetButton( 
										self:GetWidget("btn_ok") , 
										CPPGameLib.GetString("common_confirm") , 
										CPPGameLib.Handler(self,self.OnBtn_ok) ,
										nil , nil , Macros.Color.button_yellow )
	--取消按钮
	self.btn_no = Util:WidgetButton( 
										self:GetWidget("btn_no") , 
										CPPGameLib.GetString("common_cancel") , 
										CPPGameLib.Handler(self,self.OnBtnclose) ,
										nil , nil , Macros.Color.button_red )
	--提示
	self.Text_tips = self:GetWidget("Text_tips")
end

function Modules.TemplResolveEquip:__DataTidy(data)
	local t1 = {}
	--金钱列表
	for i,v in ipairs(data.return_currency_list) do
		local config = CPPGameLib.GetConfig("HuoBi", v.type)
		if v.value ~= 0 then
			table.insert(t1 , { id = config.big_icon , num = v.value })
		end
	end

	--道具列表
	for i,v in ipairs(data.return_object_list) do
		if v.obj_num ~= 0 then
			table.insert(t1 , { id = v.obj_id , num = v.obj_num })
		end
	end
	self.idLst = t1
	self:__ShowUI(Util:SegmentationTab(t1 , 4))	
end

function Modules.TemplResolveEquip:__ShowUI(data)
	--标题
	self.headline = Util:Label( " " , Macros.TypefaceSize.popup )
	self.headline:SetPosition(self.img_headline:GetContentWidth()/2 , self.img_headline:GetContentHeight()/2 )
	self.img_headline:AddChild(self.headline)

	--列表
	self.lst_list = UIListViewEx.Create()
    self.lst_list:SetContentSize(self.img_dataBG:GetContentWidth() - 5 ,self.img_dataBG:GetContentHeight() - 5)
    self.lst_list:SetAnchorPoint(0,0)
    self.lst_list:SetPosition(2.5,2.5)
    self.lst_list:SetItemsMargin(5) 
    self.lst_list:SetBounceEnabled(true)
	self.img_dataBG:AddChild(self.lst_list)	

	for i,v in ipairs(data) do
		self:Cell(i,v)
	end
end

function Modules.TemplResolveEquip:Cell(index,data)
  	local layou = Util:Layout()
    layou:SetContentSize(self.lst_list:GetContentWidth() , 130)
    layou:SetAnchorPoint(0.5,0.5)

	local x = 62
    for i,v in ipairs(data) do
	    local type_ , info_ = ObjIDTool.GetInfo(v.id)

	    local icon = LogicGUI.IconLayer.New(false)
	    icon:SetPosition(x + ( (i-1) * 118 ) ,layou:GetContentHeight()/2 + 20 )
	    icon:SetData({id = v.id , num = v.num , obj_info = info_ }) 
		icon:ShowItemName(true)
		icon:ShowEquipName(true)
		icon:ShowTreasureName(true)
		icon:ShowHeroName(true)
		icon:ShowDivinationName(true)
	    layou:AddChild(icon:GetNode())
    end
	self.lst_list:PushBackCustomItem(layou)
end

function Modules.TemplResolveEquip:SetName(name)
	if self.headline then
		self.headline:SetString( name )
	end
end

--设置消耗
function Modules.TemplResolveEquip:SetConsume(reset)
    if self.consume then
    	self.consume:RemoveSelf(true)
    	self.consume = nil
    end

    self.consume = Util:MoneyControl( Macros.Game.RolePropType.DIAMOND , 0.8 , reset , nil , Macros.Color.content , 10 )
    self.consume:SetAnchorPoint(0.5,0.5)
    self.consume:SetPosition(self.btn_ok:GetContentWidth()/2 , self.btn_ok:GetContentHeight() + 15)
    self.btn_ok:AddChild(self.consume)
end

--设置提示
function Modules.TemplResolveEquip:SetTips(reset)
    if self.tips then
    	self.tips:RemoveSelf(true)
    	self.tips = nil
    end

    self.tips = Util:Label(reset , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
    self.tips:SetPosition(0 , self.Text_tips:GetContentHeight()/2)
    self.tips:SetAnchorPoint(0,0.5)
    self.Text_tips:AddChild(self.tips)
end


--取消按钮
function Modules.TemplResolveEquip:OnBtnclose( ... )
	self:DeleteMe()
end

--确定按钮
function Modules.TemplResolveEquip:OnBtn_ok( ... )
	CallModuleFunc(ModuleType.SUNDRY, "LstPackVacancy", self.idLst , true , function ( text )
		if #text == 0 then
			if self.fun then
				self.fun()
			end
			self:OnBtnclose()
		end
	end)
end

