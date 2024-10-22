
--[[
%% @module: 选服界面
%% @author: swordman sue
%% @created: 2016-08-08
--]]

Modules = Modules or {}

Modules.LoginServerListView = Modules.LoginServerListView or BaseClass(BaseView)

local last_login_servers_max = 4
local recommend_servers_max = 4

function Modules.LoginServerListView:__init()
	self.__layout_name = "login_serverlist"	
    self.__close_mode = BaseView.CloseMode.VISIBLE
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_login", nil, "sdz"), image_size = COCOSize(1136, 640)}

    self.is_auto_close = false

    self.__loaded_callback = function()
        self:__LoadedCallback()
    end

    self.__open_callback = function()
    	self:__OpenCallback()
    end

    self.__close_callback = function()
        self:__CloseCallback()
    end

    self:__AddPreloadTemplate(GUI.TemplateRegistry.LOGIN_SERVER_ITEM)
    self:__AddPreloadTemplate(GUI.TemplateRegistry.LOGIN_SERVER_ZONE)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.LOGIN)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.LOGIN, true)    
end

function Modules.LoginServerListView:__LoadedCallback()
    self.__panel_recommend = self:GetWidget("Panel_Recommend")
    self.__panel_zone_servers = self:GetWidget("Panel_ZoneServers")

    local btn_close = self:GetWidget("Button_Close")
    WidgetUnity.SetWidgetListener(btn_close, nil, nil, function()
        GlobalStateMgr:ToLoginState(BaseViewSubType.Login.LOGIN_II)       
    end)

    local btn_recommand = self:GetWidget("Button_Recommend")
    btn_recommand = Util:WidgetButton( 
                                        btn_recommand, 
                                        CPPGameLib.GetString("server_recommend"), 
                                        function ( ... )
                                            self:__ChangeSubView(true)
                                            if self.__zones_scrollview then
                                                self.__zones_scrollview:UnSelectItem()
                                            end   
                                        end, 
                                        Macros.TypefaceSize.largish,
                                        Macros.Color.keypoint
                                    )

    Util:WidgetLabel( 
                        self:GetWidget("Text_3") ,
                        CPPGameLib.GetString("server_region"),
                        Macros.TypefaceSize.popup)
    
    Util:WidgetLabel( 
                        self:GetWidget("Text_1") ,
                        CPPGameLib.GetString("server_lastTime"),
                        Macros.TypefaceSize.normal,
                        Macros.Color.headline
                    )

    Util:WidgetLabel( 
                        self:GetWidget("Text_1_0") ,
                        CPPGameLib.GetString("server_recommend"),
                        Macros.TypefaceSize.normal,
                        Macros.Color.headline
                    )
end

function Modules.LoginServerListView:__Dispose()
    self:__CloseCallback()

    if self.__zones_scrollview then
        self.__zones_scrollview:DeleteMe()
        self.__zones_scrollview = nil
    end

    if self.__zone_servers_scrollview then
        self.__zone_servers_scrollview:DeleteMe()
        self.__zone_servers_scrollview = nil
    end

    CPPGameLib.ClearObjList(self.__recommend_server_list)    
    self.__recommend_server_list = nil
end

function Modules.LoginServerListView:__OpenCallback()
    self:__ChangeSubView(true)
    self:__RefreshZones()
    self:__RefreshRecommend()
end

function Modules.LoginServerListView:__CloseCallback()
    if self.__zones_scrollview then
        self.__zones_scrollview:UnSelectItem()
    end

    CPPGameLib.ClearObjList(self.__last_login_server_list)
    self.__last_login_server_list = nil
end

function Modules.LoginServerListView:__ChangeSubView(is_recommend)
    self.__panel_recommend:SetVisible(is_recommend)
    self.__panel_zone_servers:SetVisible(not is_recommend)
end

function Modules.LoginServerListView:__RefreshZones()
    local zones = self:CallModuleFunc("GetZones", true)
    if not zones then
        return
    end

    if self.__zones_scrollview then
        return
    end

    local params = 
    {
        item_class = Modules.LoginServerZone,
        item_width = 308.00,
        item_height = 80.00,
        row = 5,
        col = 1,
        item_space_r = 0,
        selectable = true,
        select_callback = function(item_index, item_data)
            self:__ChangeSubView(false)
            self:__RefreshZoneServers(item_data.index)
        end,
    }
    self.__zones_scrollview = GUI.ScrollView.New(params)
    self.__zones_scrollview:SetDataList(zones)

    local panel_zone = self:GetWidget("Panel_ServerZone")
    panel_zone:AddChild(self.__zones_scrollview:GetNode())
    PosTool.Center(self.__zones_scrollview)
end

function Modules.LoginServerListView:__RefreshRecommend()
    local last_login_servers = self:CallModuleFunc("GetLastLoginServers")
    local recommend_servers = self:CallModuleFunc("GetRecommendServers")  

    if not last_login_servers and not recommend_servers then
        return
    end

    local col_num = 2
    local row_num
    local col_index, row_index
    local item

    local item_select_callback = function(server_id)
        self:CallModuleFunc("HandleSelectServer", server_id)
    end

    --最近登陆服务器
    if last_login_servers and not self.__last_login_server_list then        
        self.__last_login_server_list = {}

        local panel_last_login_servers = self:GetWidget("Panel_LastServers")
        local row_num = math.ceil(last_login_servers_max/col_num)

        local intervalW = panel_last_login_servers:GetContentWidth() / col_num
        local intervalH = panel_last_login_servers:GetContentHeight() / col_num

        for i = 1, #last_login_servers do
            if i > last_login_servers_max then
                break
            end

            local col_index = (i-1)%col_num
            local row_index = row_num - math.floor((i-1)/col_num) - 1

            item = Modules.TemplLastServerItem.New( 
                                                    panel_last_login_servers, 
                                                    (intervalW/2) + intervalW * (col_index) , 
                                                    (intervalH/2) + intervalH * (row_index) 
                                                    )
            item:SetData(last_login_servers[i])
            item:SetSelectCallback(item_select_callback)
            table.insert(self.__last_login_server_list, item)
        end
    end

    --推荐服务器
    if recommend_servers and not self.__recommend_server_list then
        self.__recommend_server_list = {}
        local panel_recommend_servers = self:GetWidget("Panel_RecommendServers")
        local row_num = math.ceil(last_login_servers_max/col_num)

        local intervalW = panel_recommend_servers:GetContentWidth() / col_num
        local intervalH = panel_recommend_servers:GetContentHeight() / col_num


        for i = 1, #recommend_servers do        
            if i > recommend_servers_max then
                break
            end

            local col_index = (i-1)%col_num
            local row_index = row_num - math.floor((i-1)/col_num) - 1
            item = Modules.TemplRecommendServerItem.New(
                                                            panel_recommend_servers, 
                                                            (intervalW/2) + intervalW * (col_index) , 
                                                            (intervalH/2) + intervalH * (row_index) 
                                                        )
            item:SetData(recommend_servers[i] , self:CallModuleFunc("GetLastLoginInfoByID" , recommend_servers[i].id ) )
            item:SetSelectCallback(item_select_callback)
            table.insert(self.__recommend_server_list, item)
        end    
    end
end

function Modules.LoginServerListView:__RefreshZoneServers(zone_index)
    local server_list = self:CallModuleFunc("GetZoneServers", zone_index, true)
    if not server_list then
        return
    end

    if not self.__zone_servers_scrollview then
        local panel_servers = self:GetWidget("Panel_ServersList")
        local params = 
        {
            item_class = Modules.TemplLoginServerItem,
            item_width = 298.76,
            item_height = 70.00,
            row = 5,
            col = 2,
            item_space_r = 20,
            selectable = true,
            select_callback = function(item_index, item_data)
                self:CallModuleFunc("HandleSelectServer", item_data.id)
            end,
            view_width = panel_servers:GetContentWidth(),
            view_height = panel_servers:GetContentHeight(),
        }
        self.__zone_servers_scrollview = GUI.ScrollView.New(params)

        panel_servers:AddChild(self.__zone_servers_scrollview:GetNode())        
        PosTool.Center(self.__zone_servers_scrollview)
    end

    self.__zone_servers_scrollview:SetDataList(server_list)    
end

