
--[[
%% @module: 角色信息、设置界面
%% @author: swordman sue
%% @created: 2016-08-10
--]]

Modules = Modules or {}

Modules.RoleInfoView = Modules.RoleInfoView or BaseClass(BaseView)

function Modules.RoleInfoView:__init()
	self.__layout_name = "role_info"	
    self.__bg_params = {type = BaseView.BGType.SOLID}

    local t1 = {
            CPPGameLib.GetString("role_info_tab"), 
            CPPGameLib.GetString("role_setting_tab"),
        }

    local tab_sp1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bigblue")
    local tab_sp2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_biggreen")
    self.__tab_params = {
        {tab_sp1, tab_sp2},
        t1 , 5 , CPPGameLib.Handler(self, self.OnYeMei)
    }
 
    self.__tab_view_params = 
    {
        {Modules.RoleInfoTabView, "Panel_RoleInfo"},
        {Modules.SettingTabView, "Panel_Setting"},
    }

    self.__open_callback = function()
    	self:__OpenCallback()
    end
end

function Modules.RoleInfoView:OnYeMei(index)
    self:__TabChange(index)
    if index == 1 then
        self.role_info_tab:SetString( CPPGameLib.GetString("role_info_tab") )
    else
        self.role_info_tab:SetString( CPPGameLib.GetString("role_setting_tab") )
    end
end

function Modules.RoleInfoView:__OpenCallback()
    local btn_close = self:GetWidget("Button_Close")
    WidgetUnity.SetWidgetListener(btn_close, nil, nil, function()
        self:CloseManual()
    end)    

    self.role_info_tab = Util:Label( CPPGameLib.GetString("role_info_tab") , 
                                        Macros.TypefaceSize.popup ,
                                        Macros.Color.lilyWhite
                                    )
    self:GetWidget("Image_1"):AddChild(self.role_info_tab)
    PosTool.Center( self.role_info_tab )

    local widget = self:GetWidget("Panel_Tab")
    self.__tab:On( unpack(self.__open_data) or 1)
    self.__tab:SetAnchorPoint(0.5, 1)
    widget:AddChild(self.__tab:GetNode()) 
    PosTool.CenterTop(self.__tab)   
end
