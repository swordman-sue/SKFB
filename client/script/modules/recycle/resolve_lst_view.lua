
--
-- @brief 分解列表
-- @author: yjg
-- @date: 2016年11月23日16:04:03
--

Modules = Modules or {}

Modules.ResolveLstView = Modules.ResolveLstView or BaseClass(BaseView)

function Modules.ResolveLstView:__init()
	self.__layout_name = "sale"  
	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}
	self.__title_bar_params = 
	{
        title = " ",
		show_diamond = true,
		show_gold = true,
		show_stamina = true,
		show_energy = true,
		close_func = function()
			-- self:OnButtonSell()
            self:CloseManual()
		end,
	}

    self.__open_callback = function()
    	self:__OpenCallback()
    end

    local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_bookmark_yse")
    self.__tab_params = {
        {sp_2,sp_1},
        {  
            CPPGameLib.GetString("tower_shopCommodity"), 
            -- CPPGameLib.GetString("equip_quality3"),
            -- CPPGameLib.GetString("equip_quality4"),
            -- CPPGameLib.GetString("tower_shopAward"),
        },10,CPPGameLib.Handler(self, self.OnYeMei),false
    }

 --    --列表条目
    self:__AddPreloadTemplate(GUI.TemplateRegistry.SALE_ITEM)
end

function Modules.ResolveLstView:__OpenCallback()    
	self:__Getwidget()
	self:__ShowUI()
end

function Modules.ResolveLstView:__Dispose()
    if self.__zones_scrollview then
        self.__zones_scrollview:DeleteMe()
        self.__zones_scrollview = nil
    end
end

function Modules.ResolveLstView:__Getwidget( ... )
	-- --标题
 --    self.Button_1_0 = Util:WidgetButton( self:GetWidget("Button_1_0") , " " , function ( ... )
 --    	-- body
 --    end)
    -- self.Button_1_0:SetVisible(false)
    --下背景
    -- self.Image_below = self:GetWidget("Image_below") 
    --上背景
    self.Panel_Backpackcbx = self:GetWidget("Panel_Backpackcbx") 
    --内容背景
    self.Image_content = self:GetWidget("Image_content") 
    --列表背景
    self.Image_5 = self:GetWidget("Image_5") 
    --出售可获得
    self.Text_gain_0 = self:GetWidget("Text_gain_0") 
    --筛选
    self.Button_screen_0 =  Util:WidgetButton( self:GetWidget("Button_screen_0") , CPPGameLib.GetString("sell_pinzhishaixuan") , CPPGameLib.Handler(self,self.OnButtonScreen) )
    self.Button_screen_0:SetVisible(false)

    --出售
    self.Button_sell_0 = Util:WidgetButton( 
                                            self:GetWidget("Button_sell_0") , 
                                            CPPGameLib.GetString("common_confirm") , 
                                            CPPGameLib.Handler(self,self.OnButtonSell) ,
                                            nil,nil,Macros.Color.button_yellow
                                            )
    self.Button_sell_0:SetPosition(self.Button_sell_0:GetPositionX() - 50 , self.Button_sell_0:GetPositionY() )

    --选中数量
    self.Text_onNum_0 = self:GetWidget("Text_onNum_0") 
    self.Text_onNum_0:SetPosition(self.Text_onNum_0:GetPositionX() , self.Button_sell_0:GetPositionY() )
end

function Modules.ResolveLstView:__ShowUI( ... )
	local tab = {
			[Macros.Game.ObjType.EQUIP] = CPPGameLib.GetString("recycle_onEquipLst"),
			[Macros.Game.ObjType.HERO] = CPPGameLib.GetString("recycle_onHeroLst"),
            [Macros.Game.ObjType.FAIRY] = CPPGameLib.GetString("recycle_onCard"),
		}

    self.__title_bar:SetTitle( tab[unpack(self.__open_data)] )

    self.__tab:SetPosition(0 , 10)
    self.Panel_Backpackcbx:AddChild(self.__tab:GetNode())
    self.__tab.btn_list[1]:SetText( tab[unpack(self.__open_data)] )
    self.__tab:On( 1 )

    local params =  
    {
        item_class = Modules.ResolveLstItem,
        item_width = 455.5,
        item_height = 120,
        row = 4,
        col = 2,
        item_space_r = 2.5,
        item_space_c = 2.5,
        -- view_width = 925.00,
        view_height = self.Image_5:GetContentHeight() - 10 ,
    }
    -- 创建参数
    self.__zones_scrollview = GUI.ScrollView.New(params)
    self.Image_5:AddChild(self.__zones_scrollview:GetNode())
    PosTool.Center(self.__zones_scrollview:GetNode())
    self.lst = CallModuleFunc(ModuleType.RECYCLE, "DateLst",unpack(self.__open_data))
    self.__zones_scrollview:SetDataList( self.lst )

	--选择数量
	local onNum = Util:Label(CPPGameLib.GetString("sell_yixuanshuliang") , nil , Macros.Color.keypoint )
	onNum:SetAnchorPoint(0,0.5)
	onNum:SetPosition(10,self.Text_onNum_0:GetContentHeight() )
	self.Text_onNum_0:AddChild(onNum)

	self.onNum = Util:Label( "0" , nil ,  Macros.Color.content)
	PosTool.RightTo(onNum,self.onNum)
	self.Text_onNum_0:AddChild(self.onNum)

	self:ShowOnData()
end

--选择信息显示
function Modules.ResolveLstView:ShowOnData()
    local num = CallModuleFunc(ModuleType.RECYCLE, "GetOnData")
	self.onNum:SetString(#num or 0)
end

--确定
function Modules.ResolveLstView:OnButtonSell()
    local obj_type = unpack(self.__open_data)
    local tab_index = CallModuleFunc(ModuleType.RECYCLE, "GetTabIndex", obj_type)    
	CallModuleFunc(ModuleType.RECYCLE, "OpenView", BaseViewType.RECYCLE, tab_index)
end


function Modules.ResolveLstView:OnYeMei( index )
    local com_sp_cbox_wire = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp_cbox_wire")
    if self.com_sp_cbox_wire then
        self.com_sp_cbox_wire:RemoveSelf(true)
        self.com_sp_cbox_wire = nil
    end
    self.com_sp_cbox_wire = Util:Sprite(com_sp_cbox_wire)
    self.__tab.btn_list[index]:AddChild( self.com_sp_cbox_wire )
    PosTool.Center( self.com_sp_cbox_wire , 0 , -35 )
end















Modules = Modules or {}
Modules.ResolveLstItem = Modules.ResolveLstItem or BaseClass(GUI.ScrollItem)

function Modules.ResolveLstItem:__init()
    self:InitTemplate(GUI.TemplateRegistry.SALE_ITEM)   
end

function Modules.ResolveLstItem:__delete()

end

function Modules.ResolveLstItem:SetData(data)
	GUI.ScrollItem.SetData(self, data)
	self.data = data
	self:__Getwidget()
	self:__ShowUI(data)
end

function Modules.ResolveLstItem:__Getwidget()
	--条目
    self.Image_sellItem = self:GetWidget("Image_sellItem") 
    --条目容器
    self.Panel_sellItem = self:GetWidget("Panel_sellItem") 
    --名字
    self.Text_sellName = self:GetWidget("Text_sellName") 
    --强化&属性
    self.Text_intensify = self:GetWidget("Text_intensify") 
    --价钱
    self.Text_price = self:GetWidget("Text_price") 
    --图标
    self.Panel_icon = self:GetWidget("Panel_icon") 
    --选择框
    self.Panel_cbox = self:GetWidget("Panel_cbox") 
end

function Modules.ResolveLstItem:__ShowUI(data)
    -- 头像
    local icon = LogicGUI.IconLayer.New(false)
    icon:SetPosition(self.Panel_icon:GetContentWidth()/2 ,self.Panel_icon:GetContentHeight()/2)
    self.Panel_icon:AddChild(icon:GetNode())
    local iconData = { id = data.info_id }
    icon:SetData(iconData)

    local type_ , info_ = ObjIDTool.GetInfo(data.info_id)
    if type_ == Macros.Game.ObjType.EQUIP then
        local info = CallModuleFunc(ModuleType.EQUIP, "GetEquip", data.uid )
        icon:SetEquipStar( info.star  )
    end

    --名字
    self.name = Util:Name(data.name , Macros.TypefaceSize.normal , data.quality)
    self.name:SetAnchorPoint(0, 0.5)
    self.name:SetPosition( 0 , self.Text_sellName:GetContentHeight()/2 )
    self.Text_sellName:AddChild(self.name)

    --选择框
    local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_select_no")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_select_yes")
    self.cbx = UICheckBoxEx.CreateWithImage(sp_1,sp_2)
    self.cbx:SetPosition(self.Panel_cbox:GetContentWidth()/2,self.Panel_cbox:GetContentHeight()/2)
    self.Panel_cbox:AddChild(self.cbx)
    self.cbx:SetEventListener(
        GlobalCallbackMgr:AddCallBackFunc(function()
        	if #CallModuleFunc(ModuleType.RECYCLE, "GetOnData") < 5 then
	            CallModuleFunc(ModuleType.RECYCLE, "AddOnData", data )
	        else
	        	self.cbx:SetSelectedState(false)
                GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("recycle_selectMax") )
	        end
        end),
        GlobalCallbackMgr:AddCallBackFunc(function()
            CallModuleFunc(ModuleType.RECYCLE, "RemoveOnData", data )
        end)) 
           
    local lst = CallModuleFunc(ModuleType.RECYCLE, "GetOnData")
    for i,v in ipairs(lst) do
    	if v.uid == data.uid then
			self.cbx:SetSelectedState(true)
    	end
    end

	local type_ , info = ObjIDTool.GetInfo(data.info_id)
	if type_ == Macros.Game.ObjType.EQUIP then
		--装备特色属性显示
		self:__EquipShowUI(data,info)
	elseif data.type == Macros.Game.ObjType.HERO then
		--英雄特色属性显示
		self:__HeroShowUI(data,info)

    elseif data.type == Macros.Game.ObjType.FAIRY then
        --卡牌特色属性显示
        self:__CardShowUI(data,info)
	end
end

------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--装备条目
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
function Modules.ResolveLstItem:__EquipShowUI(data,info)
    print("--*-*-*--*")
	local intensifyType = Util:Label(CPPGameLib.GetString("recycle_intensify") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
	intensifyType:SetAnchorPoint(0,0.5)
	intensifyType:SetPosition(0 , self.Text_intensify:GetContentHeight() / 2 )
	self.Text_intensify:AddChild(intensifyType)

	local intensifyLv = Util:Label(data.strengthen_level , Macros.TypefaceSize.normal , Macros.Color.content)
	PosTool.RightTo(intensifyType , intensifyLv)
	self.Text_intensify:AddChild(intensifyLv)

	local intensifyType = Util:Label(CPPGameLib.GetString("recycle_refine") , Macros.TypefaceSize.normal , Macros.Color.keypoint)
	intensifyType:SetAnchorPoint(0,0.5)
	intensifyType:SetPosition(130 , self.Text_intensify:GetContentHeight() / 2 )
	self.Text_intensify:AddChild(intensifyType)

	local intensifyLv = Util:Label(data.refine_level , Macros.TypefaceSize.normal , Macros.Color.content )
	PosTool.RightTo(intensifyType , intensifyLv)
	self.Text_intensify:AddChild(intensifyLv)
end


------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--英雄条目
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
function Modules.ResolveLstItem:__HeroShowUI(data,info)
	local intensifyType = Util:Label(CPPGameLib.GetString("common_lev1"), Macros.TypefaceSize.normal , Macros.Color.keypoint)
	intensifyType:SetAnchorPoint(0,0.5)
	intensifyType:SetPosition(0 , self.Text_intensify:GetContentHeight() / 2 )
	self.Text_intensify:AddChild(intensifyType)

	local intensifyLv = Util:Label(data.lv , Macros.TypefaceSize.normal , Macros.Color.content)
	PosTool.RightTo(intensifyType , intensifyLv)
	self.Text_intensify:AddChild(intensifyLv)

	local intensifyType = Util:Label(CPPGameLib.GetString("recycle_break") , Macros.TypefaceSize.normal , Macros.Color.keypoint)
	intensifyType:SetAnchorPoint(0,0.5)
	intensifyType:SetPosition(160 , self.Text_intensify:GetContentHeight() / 2 )
	self.Text_intensify:AddChild(intensifyType)

	local intensifyLv = Util:Label(data.break_lv , Macros.TypefaceSize.normal , Macros.Color.content)
	PosTool.RightTo(intensifyType , intensifyLv)
	self.Text_intensify:AddChild(intensifyLv)
end


------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--卡牌条目
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
function Modules.ResolveLstItem:__CardShowUI(data,info)
    local intensifyType = Util:Label(CPPGameLib.GetString("common_lev1"), Macros.TypefaceSize.normal , Macros.Color.keypoint)
    intensifyType:SetAnchorPoint(0,0.5)
    intensifyType:SetPosition(0 , self.Text_intensify:GetContentHeight() / 2 )
    self.Text_intensify:AddChild(intensifyType)

    local intensifyLv = Util:Label( data.strengthen_level , Macros.TypefaceSize.normal , Macros.Color.content)
    PosTool.RightTo(intensifyType , intensifyLv)
    self.Text_intensify:AddChild(intensifyLv)


    local intensifyType = Util:Label(CPPGameLib.GetString("hero_advance").. CPPGameLib.GetString("common_colon") , Macros.TypefaceSize.normal , Macros.Color.keypoint)
    intensifyType:SetAnchorPoint(0,0.5)
    intensifyType:SetPosition(160 , self.Text_intensify:GetContentHeight() / 2 )
    self.Text_intensify:AddChild(intensifyType)

    local intensifyLv = Util:Label( data.refine_level , Macros.TypefaceSize.normal , Macros.Color.content)
    PosTool.RightTo(intensifyType , intensifyLv)
    self.Text_intensify:AddChild(intensifyLv)
end
