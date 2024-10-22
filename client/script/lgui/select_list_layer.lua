--[[
%% @module: 选择列表
%% @author: yjg
%% @created: 2016年9月13日16:39:26
--]]

LogicGUI = LogicGUI or {}

LogicGUI.SelectListLayer = LogicGUI.SelectListLayer or BaseClass(UILayer)

function LogicGUI.SelectListLayer:__init(fun,type)
	print("选择列表")
	self.fun = fun 
    self.type = type

	--选择的列表
	self.onData = {}
    self:__Getwidget()
    self:__ShowUI()
end

function LogicGUI.SelectListLayer:__delete()
    if self.__title_bar then
        self.__title_bar:DeleteMe()
        self.__title_bar = nil
    end
end

function LogicGUI.SelectListLayer:__Getwidget()
  self.__title_bar_params = 
    {
        show_diamond = true,
        show_gold = true,
        show_stamina = true,
        show_energy = true,
        close_func = function() 
            self:Confirm()
        end,
    }

    self.__title_bar = LogicGUI.TitleBarLayer.New(self.__title_bar_params)
    self.__title_bar:SetPosition(291,599)
    self:AddChild(self.__title_bar)

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_blue")
	self.btnSell  =  Util:Button(sp,CPPGameLib.Handler(self, self.Confirm),CPPGameLib.GetString("common_confirm"),Macros.TypefaceSize.button,Macros.Color.content)
	self.btnSell:SetAnchorPoint(0,0)
	self.btnSell:SetScale9Enabled(true)
	self.btnSell:SetContentSize(156.00,61.00)
	self.btnSell:SetPosition(-181 ,13)
	self:AddChild(self.btnSell) 

    self.lst_list = UIListViewEx.Create()
    self.lst_list:SetAnchorPoint(0,0)
    self.lst_list:SetContentSize(685.00,485.00)
    self.lst_list:SetPosition(0,5)
    self:AddChild(self.lst_list)
end

function LogicGUI.SelectListLayer:__ShowUI()

end

function LogicGUI.SelectListLayer:ShowUIType( type )
    local list = CallModuleFunc(ModuleType.RECYCLE, "DateLst",type)

    self.lst_list:RemoveAllChild()
    local info = CallModuleFunc(ModuleType.RECYCLE, "ResolvePitchLst")
    local t1 = {}
    for i,v in ipairs(info) do
    	t1[v.uid] = true
    end
	for i,v in ipairs(list) do
		if not t1[v.uid] then
			self:Cell(i,v)
		end
	end
end

--条目
function LogicGUI.SelectListLayer:Cell(index,data)
    local layou = Util:Layout()
    layou:SetContentSize(685,100)
    layou:SetAnchorPoint(0.5,0.5)
    layou:SetTouchEnabled(true)

    --背景
    local dialogueBG = Scale9SpriteEx.CreateWithImageFile(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "bg_sp9_lst_layer1"))
    dialogueBG:SetAnchorPoint(0.5,0.5)
    dialogueBG:SetInnerRect(10/44,10/44,10/44,10/44)
    dialogueBG:SetStretchSize(670,100)
    layou:AddChild(dialogueBG)
    PosTool.Center(dialogueBG)

    -- 头像
    local icon = LogicGUI.IconLayer.New(false)
    icon:SetPosition(60 ,layou:GetContentHeight()/2)
    dialogueBG:AddChild(icon:GetNode())
    local iconData = { id = data.info_id }
    icon:SetData(iconData)

    --标题条
    local headBG = Scale9SpriteEx.CreateWithImageFile(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bb_biaoti"))
    headBG:SetAnchorPoint(0.5,0.5)
    headBG:SetPosition(290,80)
    layou:AddChild(headBG)

    --物品名称
    local name = Util:Name(data.name,nil, data.quality)
    name:SetAnchorPoint(0, 0.5)
    name:SetPosition(10,headBG:GetContentHeight()/2)
    headBG:AddChild(name)

    --属性框
    local propertyBG = Scale9SpriteEx.CreateWithImageFile(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BG, "bg_sp9_lst_layer1"))
    propertyBG:SetAnchorPoint(0.5,0.5)
    propertyBG:SetInnerRect(10/40,10/40,10/40,10/40)
    propertyBG:SetStretchSize(390,50)
    propertyBG:SetPosition(dialogueBG:GetContentWidth()/2 - 22 , dialogueBG:GetContentHeight()/2 - 15 )
    layou:AddChild(propertyBG)

    if not self.type then
        local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_select_no")
        local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_select_yes")
        local cbx = UICheckBoxEx.CreateWithImage(sp_1,sp_2)
        cbx:SetPosition(dialogueBG:GetContentWidth()/2 + 240 ,dialogueBG:GetContentHeight()/2 - 15)
        layou:AddChild(cbx)
        cbx:SetEventListener(
            GlobalCallbackMgr:AddCallBackFunc(function()
            	local info = CallModuleFunc(ModuleType.RECYCLE, "ResolvePitchLst")
            	if #info >= 5 then
            		cbx:SetSelectedState(false)
            	else
    	            table.insert(self.onData,data)
    	        end
                self:OnPressed()
            end),
            GlobalCallbackMgr:AddCallBackFunc(function()
                local t1 = self.onData
                for i,v in ipairs(t1) do
                    if v.uid == data.uid then
                        table.remove(self.onData,i)
                    end
                end
                self:OnPressed()
            end))  
    else
        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN,"btn_btn_green")
        local btn_1 = Util:Button(sp,function ( ... )
            local info = CallModuleFunc(ModuleType.RECYCLE, "ResolvePitchLst")
            table.insert(self.onData,data)
            self:OnPressed()
            self:Confirm()
        end,CPPGameLib.GetString("common_confirm"),Macros.TypefaceSize.normal,Macros.Color.keypoint)
        btn_1:SetPosition(dialogueBG:GetContentWidth()/2 + 240 ,dialogueBG:GetContentHeight()/2 - 15)
        btn_1:SetAnchorPoint(0.5,0.5)
        layou:AddChild(btn_1)
    end

    self.lst_list:PushBackCustomItem(layou)
end

function LogicGUI.SelectListLayer:Confirm( ... )
    self.onData = {}
	if self.fun then
		self.fun()
	end
end

function LogicGUI.SelectListLayer:OnPressed( ... )
	for i,v in ipairs(self.onData) do
		local data = CallModuleFunc(ModuleType.RECYCLE, "ResolvePitchAdd",v)
	end
end