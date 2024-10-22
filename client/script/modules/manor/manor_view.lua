
--[[
%% @module: 领地
%% @author: yjg
%% @created: 2016年10月8日09:52:19
--]]

Modules = Modules or {}

Modules.ManorView = Modules.ManorView or BaseClass(BaseView)

function Modules.ManorView:__init()
	self.__layout_name = "manor"	
	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bp_sp_manor")}

    self.__title_bar_params = {
        title = CPPGameLib.GetString("manor"),
        show_bg = false,
        lucency_bg = true,
        show_diamond = true,
        show_gold = true,
        show_stamina = true,
        show_energy = true,
        res_beg_offset_x = -90,
        res_unit_bg_imageinfo = {Resource.UITextureAtlasType.CITY, "bg_resource"},
        res_unit_title_offset_x = -16,
        close_func = function()
            self:CloseManual()
        end,
    }

    self.__open_callback = function()
    	self:__OpenCallback()
    end

    --加载资源
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.MANOR)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.MANOR,true)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.MANOR_BG)
end

function Modules.ManorView:__OpenCallback()
    CallModuleFunc(ModuleType.MANOR, "PlayerLandDataReq")
    --城市
    self.city = {}
	self:__Getwidget()
    self:__ShowUI()
end

function Modules.ManorView:__RegistAllEvents()
    --刷新城市状态
    self:BindGlobalEvent(Macros.Event.ManorModule.MANOR_UP, function ()
        --刷新城市状态
        self:UpCityType()
        --好友组件
        self:SetFriend()
        --暴动
        self:SetShowUi()
	end)

    --刷新城市状态
    self:BindGlobalEvent(Macros.Event.ManorModule.MANOR_FRIEND, function ()
        --暴动
        self:SetShowUi()
    end)
end

function Modules.ManorView:__Dispose()
    for id , v in ipairs(CPPGameLib.GetConfig("LandCity", nil, nil , true )) do
        if self.city[id] then
            self.city[id]:__Dispose()
        end
    end
end

function Modules.ManorView:__Getwidget( ... )
	--居中层
    self.Panel_2 = self:GetWidget("Panel_2") 

    --背景
    self.Image_map1 = self:GetWidget("Image_map1") 
    --暗黑
    self.Image_dark1 = self:GetWidget("Image_dark1") 
    --名称
    self.Image_name1 = self:GetWidget("Image_name1")
    self.Image_name1:IgnoreContentAdaptWithSize(true)
 
    --居中控件
    self.Panel_type1 = self:GetWidget("Panel_type1") 

    --背景
    self.Image_map2 = self:GetWidget("Image_map2") 
    --暗黑
    self.Image_dark2 = self:GetWidget("Image_dark2") 
    --名称
    self.Panel_type2 = self:GetWidget("Panel_type2") 
    --居中控件
    self.Image_name2 = self:GetWidget("Image_name2") 
    self.Image_name2:IgnoreContentAdaptWithSize(true)

    --背景
    self.Image_map3 = self:GetWidget("Image_map3") 
    --暗黑
    self.Image_dark3 = self:GetWidget("Image_dark3") 
    --名称
    self.Image_name3 = self:GetWidget("Image_name3")
    self.Image_name3:IgnoreContentAdaptWithSize(true) 
    --居中控件
    self.Panel_type3 = self:GetWidget("Panel_type3") 

    --背景
    self.Image_map4 = self:GetWidget("Image_map4") 
    --暗黑
    self.Image_dark4 = self:GetWidget("Image_dark4") 
    --名称
    self.Image_name4 = self:GetWidget("Image_name4")
    self.Image_name4:IgnoreContentAdaptWithSize(true) 
    --居中控件
    self.Panel_type4 = self:GetWidget("Panel_type4") 

    --背景
    self.Image_map5 = self:GetWidget("Image_map5") 
    --暗黑
    self.Image_dark5 = self:GetWidget("Image_dark5") 
    --名称
    self.Image_name5 = self:GetWidget("Image_name5") 
    self.Image_name5:IgnoreContentAdaptWithSize(true) 
    --居中控件
    self.Panel_type5 = self:GetWidget("Panel_type5") 

    --升级
    self.Button_upgrade = Util:WidgetButton(
                                                self:GetWidget("Button_upgrade"),nil,
                                                CPPGameLib.Handler(self,self.OnButtonUpgrade)
                                            )
    self.Image_1 = self:GetWidget("Image_1") 

    --好友
    self.Button_friend = Util:WidgetButton(
                                                self:GetWidget("Button_friend"),nil,
                                                CPPGameLib.Handler(self,self.OnButtonFriend)
                                            )

    self.Image_2 = self:GetWidget("Image_2") 

	--提示    
    self.Image_friendTips = self:GetWidget("Image_friendTips") 

    --好友组件
    self.Panel_friend = self:GetWidget("Panel_friend") 

    --返回领地
    self.Button_gohome = Util:WidgetButton(
                                                self:GetWidget("Button_gohome"),nil,
                                                CPPGameLib.Handler(self,self.OnButtonGohome)
                                            )
    self.Image_2 = self:GetWidget("Image_2") 

    --好友名称
    self.Image_friendNameBg = self:GetWidget("Image_friendNameBg") 
    self.Image_icon = self:GetWidget("Image_icon") 


    --规则按钮
    local Button_rule = Util:WidgetButton( self:GetWidget("Button_rule") , nil , function ( ... )
        print("xy" , "(5)寻访遗迹，小图标：" )
        local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
        help:SetLoadedCallback( function ( ... )
           help:SetCell("Rule_Relic")
        end )
    end )
    Button_rule:SetZOrder(100)
    
end

function Modules.ManorView:__ShowUI( ... )
    --名字
    self.name = Util:Name( "      " , Macros.TypefaceSize.normal )
    self.name:SetAnchorPoint(0,0.5)
    self.name:SetPosition( self.Image_icon:GetPositionX() + (self.Image_icon:GetContentWidth()/2) , self.Image_friendNameBg:GetContentHeight()/2 )
    self.Image_friendNameBg:AddChild(self.name)

    local manor_helptime = Util:Label( CPPGameLib.GetString("manor_helptime") , Macros.TypefaceSize.normal , Macros.Color.lilyWhite  )
    manor_helptime:SetPosition( 5 , self.Image_friendTips:GetContentHeight()/2 )
    manor_helptime:SetAnchorPoint(0,0.5)
    self.Image_friendTips:AddChild( manor_helptime )

    self.helptimeNum = Util:Label( "0", Macros.TypefaceSize.normal , Macros.Color.lilyWhite  )
    PosTool.RightTo(manor_helptime , self.helptimeNum )
    self.Image_friendTips:AddChild( self.helptimeNum )
end

--刷新城市状态
function Modules.ManorView:UpCityType()
    --按照id刷城市
    for id , v in ipairs(CPPGameLib.GetConfig("LandCity", nil, nil , true )) do
        if not self.city[id] then
            self.city[id] = LogicGUI.ManorCity.New( id , self["Image_map"..id] , self["Panel_type"..id] , self["Image_dark"..id] , self["Image_name"..id])
        else
            self.city[id]:SetShowUi()
        end
    end

    local type_ = CallModuleFunc(ModuleType.MANOR, "GetPatrolType")
    if type_ then
        self.__open_data = {}
        CallModuleFunc(ModuleType.MANOR, "SetPatrolType" , false )
    end

    --打开界面时
    if #self.__open_data ~= 0 then
        self.city[ self.__open_data[1] ] : SetChallenge( self.__open_data )
    end

end

function Modules.ManorView:OnButtonUpgrade()
    print("xy" , "OnButtonUpgrade")
    Modules.TempManorSkill.New()
end

function Modules.ManorView:OnButtonFriend()
    print("xy" , "OnButtonFriend")
    Modules.ManorFriendView.New()
end

function Modules.ManorView:OnButtonGohome()
    print("xy" , "OnButtonGohome")
    CallModuleFunc(ModuleType.MANOR, "PlayerLandDataReq")
end

function Modules.ManorView:SetFriend()
    --当前是谁的id
    local role = CallModuleFunc(ModuleType.MANOR, "GetRoleInfo")
    --我的id
    local id =  CallModuleFunc(ModuleType.ROLE , "GetRoleInfo").role_id
    if role.target_id == id then
        self.Button_upgrade:SetVisible(true)
        self.Panel_friend:SetVisible(false)
    else
        self.Button_upgrade:SetVisible(false)
        self.Panel_friend:SetVisible(true)
    end

    local lst = CallModuleFunc(ModuleType.MANOR, "GetFriend",role.target_id)
    if lst then
        self.name:SetString( lst.name )
    end
end

function Modules.ManorView:SetShowUi()
    self.helptimeNum:SetString( CallModuleFunc(ModuleType.MANOR, "RiotSuppressTimes") )
end