--
-- @brief 物品背包使用 四选一
-- @author: yjg
-- @date: 2016年8月20日18:31:58
--


LogicGUI = LogicGUI or {}

LogicGUI.TemplPackSelect = LogicGUI.TemplPackSelect or BaseClass(GUI.Template)

function LogicGUI.TemplPackSelect:__init(data)
	-- --总数据
	self.data = data
	--选中数量
	self.pitchNum = 1
	--最大数量
	self.itemNum = self.data.item_num
	self.xuanze = 0
	--单选框
	self.cbx = {}
	self.__is_auto_load = true
	self.__open_callback = function()
	    --遮罩层
	    self.__shield_layer = LogicGUI.ShieldLayer.New(function()
	        self:Onbtn_no()
	    end)
	    self.__root_widget:AddChild(self.__shield_layer:GetNode() , -1)

	    self:__Getwidget()
	    self:__ShowUI()
        
        --关闭界面事件
        self:BindGlobalEvent(Macros.Event.ApproachOfAchievingModule.DELETE_APPROACHVIEW, function()
            self:DeleteMe()
        end)
    end

    self:InitTemplate(GUI.TemplateRegistry.PACK_SELECT) 
    
end

function LogicGUI.TemplPackSelect:__delete()
end

--获得控件
function LogicGUI.TemplPackSelect:__Getwidget( ... )
	--背景
    self.img_selectBG = self:GetWidget("img_selectBG") 
    --内容背景
    self.img_dataBG_0 = self:GetWidget("img_dataBG_0") 
    --标题
    self.img_headline_0 = self:GetWidget("img_headline_0") 
    --选择数量
    self.img_textBg = self:GetWidget("img_textBg") 
    -- self.img_textBg:SetContentSize( self.img_textBg:GetContentWidth()/2 , self.img_textBg:GetContentHeight() )
    --提示
    self.Text_tips = self:GetWidget("Text_tips") 

    --添加按钮背景
    self.btn_addBG = self:GetWidget("btn_addBG") 
    -- PosTool.MoveBy( self:GetWidget("btn_add")  , -30 , 0 )
    --减少按钮背景
    self.btn_subtractBG = self:GetWidget("btn_subtractBG") 
    -- PosTool.MoveBy( self:GetWidget("btn_subtract")  , 30 , 0 )
    --加十按钮背景
    self.btn_add10BG = self:GetWidget("btn_add10BG") 
    PosTool.MoveBy( self:GetWidget("btn_add10")  , -20 , 0 )
    --减十按钮背景
    self.btn_subtract10BG = self:GetWidget("btn_subtract10BG") 
    PosTool.MoveBy( self:GetWidget("btn_subtract10")  , 20 , 0 )

    --添加按钮
    self.btn_add = Util:WidgetButton( self:GetWidget("btn_add") , nil , function ( )
    	self:OnBtnAdd(1)
    end,nil,nil,nil,nil,0) 
    --加十按钮
    self.btn_add10 = Util:WidgetButton( self:GetWidget("btn_add10") , nil , function ( )
    	self:OnBtnAdd(10)
    end ,nil,nil,nil,nil,0) 
	--减少按钮
    self.btn_subtract = Util:WidgetButton( self:GetWidget("btn_subtract") , nil , function ( )
    	self:OnBtnSubtract(1)
    end ,nil,nil,nil,nil,0) 
	--减十按钮
    self.btn_subtract10 = Util:WidgetButton( self:GetWidget("btn_subtract10") , nil , function ( )
    	self:OnBtnSubtract(10)
    end ,nil,nil,nil,nil,0) 

	--关闭按钮
    self.btn_close_0 = Util:WidgetButton( self:GetWidget("btn_close_0") , nil , CPPGameLib.Handler(self,self.OnBtnClose) ) 
    --确定按钮
    self.btn_ok = Util:WidgetButton( 
                                        self:GetWidget("btn_ok") , 
                                        CPPGameLib.GetString("common_confirm") , 
                                        CPPGameLib.Handler(self,self.OnBtnOk) ,
                                        nil,nil,Macros.Color.button_yellow
                                        ) 
end

--显示UI
function LogicGUI.TemplPackSelect:__ShowUI()
	--标题
    local lab = Util:Label(CPPGameLib.GetString("pack_select"), Macros.TypefaceSize.popup , Macros.Color.lilyWhite )
    self.img_headline_0:AddChild(lab)  
    PosTool.Center(lab)

    local config_item = CPPGameLib.GetConfig("Item", self.data.item_id)
    if not config_item then
        return
    end

    local width = 140
    if #config_item.item_id_list == 3 then
    	width = 150
    elseif #config_item.item_id_list == 2 then
    	width = 170
    end

    --选择列表
	local layou = Util:Layout()
	layou:SetContentSize( (#config_item.item_id_list * width ) ,175 )
	layou:SetAnchorPoint(0.5,0.5)
	self.img_dataBG_0:AddChild(layou)
	PosTool.Center(layou)

	--创建物品
	local long = #config_item.item_id_list * width 
	local interval = layou:GetContentWidth() / #config_item.item_id_list
	for i,v in ipairs(config_item.item_id_list) do
		local layer =  self:SelectWidget(i,v,config_item.item_min_num_list[i])
		layer:SetPosition( (width/2) + interval * (i - 1) , layou:GetContentHeight()/2 + 53 )
		layer:SetAnchorPoint(0.5,0.5)
		layou:AddChild(layer)
	end

    --提示
    local tips = Util:Label( CPPGameLib.GetString("pack_useMore") , Macros.TypefaceSize.slightly ,Macros.Color.keypoint)
    tips:SetPosition(self.Text_tips:GetContentWidth()/2 , self.Text_tips:GetContentHeight()/2 )
    tips:SetAnchorPoint(0,0.5)
    self.Text_tips:AddChild(tips)

    --选中数量
    self.lab_setnum = Util:Label( self.pitchNum , Macros.TypefaceSize.slightly , Macros.Color.content )
    self.lab_setnum:SetAnchorPoint(0.5,0.5)
    self.lab_setnum:SetPosition(self.img_textBg:GetContentWidth()/2,self.img_textBg:GetContentHeight()/2)
    self.img_textBg:AddChild(self.lab_setnum)    

    local btn_add10 = self:GetWidget("btn_add10")
    local maxSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "bg_sp_buttonBg" )
    local btnBg = Util:Sprite9( maxSp )
    btnBg:SetContentSize( 55 , 45 )
    btnBg:SetAnchorPoint( 0.5,0.5 )
    btnBg:SetPosition( btn_add10:GetPositionX() + 60  , btn_add10:GetPositionY() )
    self.img_selectBG:AddChild( btnBg )

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_num_max")
    local btn = Util:Button( sp ,  function ( ... )
        self:OnBtnAdd( ConfigAdapter.Common.GetOnceUserMax() )
    end )
    btnBg:AddChild(btn) 
    PosTool.Center( btn , 0 , 2 )
end

--添加
function LogicGUI.TemplPackSelect:OnBtnAdd( index )
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
end

--减少
function LogicGUI.TemplPackSelect:OnBtnSubtract( index )
	if self.pitchNum <= 1 then
		return 
	elseif self.pitchNum - index <= 1  then
		self.pitchNum = 1
		self.lab_setnum:SetString(1)
	else 
		self.pitchNum = self.pitchNum - index
		self.lab_setnum:SetString(self.pitchNum)
	end
end

--单选控件
function LogicGUI.TemplPackSelect:SelectWidget( index,data,num)
	local layou = Util:Layout()
	layou:SetContentSize(124,181)
	layou:SetTouchEnabled(true)

	--背景
    local sp = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp_pack_award"))
    sp:SetPosition(layou:GetContentWidth()/2 , layou:GetContentHeight()/2)
    layou:AddChild(sp)

    --图标
	self.icon = LogicGUI.IconLayer.New()
	self.icon:SetData({id = data,num = num})
	self.icon:SetPosition(layou:GetContentWidth()/2,135)
	layou:AddChild( self.icon:GetNode() )

	--名字
	local type_ , data_ = ObjIDTool.GetInfo(data)
	local lab_name = Util:Name(data_.name , Macros.TypefaceSize.slightly ,  data_.quality or data_.init_quality , nil , self.icon:GetNode():GetContentWidth() + 20 )
    lab_name:SetAnchorPoint(0.5,1)
    lab_name:SetPosition(self.icon:GetNode():GetContentWidth()/2, - 2 )
    self.icon:AddChild(lab_name)  

	local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_select_no")
	local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_select_yes")
	--选择框
    self.cbx[index] = UICheckBoxEx.CreateWithImage(sp_1,sp_2)
    self.cbx[index]:SetPosition(layou:GetContentWidth()/2,20)
    layou:AddChild(self.cbx[index],0)
    self.cbx[index]:SetEventListener(
    	GlobalCallbackMgr:AddCallBackFunc(function()
    		for i=1,#self.cbx do
    			self.cbx[i]:SetSelectedState(false)
    		end
    		self.xuanze = data
    		self.cbx[index]:SetSelectedState(true)
    	end),
     	GlobalCallbackMgr:AddCallBackFunc(function()
    		self.xuanze = 0
    	end))
    return layou
end

--确定按钮
function LogicGUI.TemplPackSelect:OnBtnOk( ... )
    if self.xuanze == 0 then
        GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("pack_tipe_pack") )
    else
        local type_ = ObjIDTool.GetInfo( self.data.item_id )
        CallModuleFunc(ModuleType.SUNDRY, "GiftBagPackVacancy", self.data.item_id , self.pitchNum , true , function ( text )
            if #text == 0 then
                local info = {}
                info.item_id = self.data.item_id
                info.item_num = self.pitchNum
                info.select_gift_id = self.xuanze
                CallModuleFunc(ModuleType.Pack, "ItemUseReq",info)
                self:OnBtnClose()
            end
        end )
    end
end

--关闭按钮
function LogicGUI.TemplPackSelect:OnBtnClose( ... )
	self:DeleteMe()
end