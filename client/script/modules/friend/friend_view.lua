--[[
%% @module: 好友
%% @author: yj
%% @created: 2016-09-05
--]]


Modules = Modules or {} 


Modules.FriendView = Modules.FriendView or BaseClass(BaseView)

function Modules.FriendView:__init()
    self.__layout_name = "friend"
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_scene")}

    --顶部资源栏
    self.__title_bar_params = 
   { 
     title = CPPGameLib.GetString("friend_good"), 
     show_diamond = true,
     show_gold = true,
     show_level = true,
     show_energy = true,
     close_func = function()
         self:CloseManual()
    end,
    }


    local t1 = {  
            CPPGameLib.GetString("friend_good_list"),
            CPPGameLib.GetString("friend_get_energy"),
            CPPGameLib.GetString("friend_add_friend"),
            CPPGameLib.GetString("friend_apply_list"),
            CPPGameLib.GetString("friend_black_list"),
        }

    local sp_1 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bigyellow")
    local sp_2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_btn_bigwhite")
    self.__tab_params = {
        {sp_2,sp_1},
        t1,
        10,
        CPPGameLib.Handler(self, self.OnYeMei),
        true,
        true
    }

    self.__loaded_callback = function ()
        self:__LockedCallback()
    end
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.FRIEND)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.FRIEND,true)
    self:__AddPreloadTemplate(GUI.TemplateRegistry.FRIEND_ALL_CELL)
end

function Modules.FriendView:__LockedCallback()
    local init_type = unpack(self.__open_data)   --获得传进来的参数  unpack 接受一个table，并且返回下标从1开始的所有元素
    init_type = init_type or Macros.Game.FRIEND_TYPE.FRIEND_LIST
    --各个Panel
    --好友列表
    local panel_friendlist = self:GetWidget("Panel_friendList")
    --领取精力
    local panel_getenergy = self:GetWidget("Panel_getEnery") 
    --添加好友
    local panel_addfriend = self:GetWidget("Panel_addFriend")
    --申请好友
    local panel_applylist = self:GetWidget("Panel_applyList")
    --黑名单
    local panel_blacklist = self:GetWidget("Panel_blackList")
    --领取精力滚动容器
    local panel_getfriendenery = self:GetWidget("Panel_getFriendEnery")
    --添加好友滚动容器
    local panel_recommend = self:GetWidget("Panel_recommend")
    --容器列表
    self.__panel_table = {[1] = panel_friendlist,[2] = panel_getenergy,[3] = panel_addfriend,[4] = panel_applylist,[5] = panel_blacklist}
    --滚动容器父容器列表
    self.__scrollView_panel_table = {[1] = panel_friendlist,[2] = panel_getfriendenery,[3] = panel_recommend,[4] = panel_applylist,[5] = panel_blacklist}
    --滚动容器列表
    self.__scrollView_table = {}
    --字符串列表
    self.__string_table = {[1] = "FriendList",[2] = "Getenergy",[3] = "AddFriend",[4] = "ApplyList",[5] = "BlackList"}

    --背景
    self.__image_scrollviewdeepbg = self:GetWidget("Image_scrollviewDeepBg")

    --好友数量文字
    Util:WidgetLabel(self:GetWidget("Text_friend"),CPPGameLib.GetString("friend_text_num"),Macros.TypefaceSize.normal, Macros.Color.keypoint)

    --好友数量
    self.__text_friendnum = self:GetWidget("Text_friendNum")
    Util:WidgetLabel(self.__text_friendnum,"",Macros.TypefaceSize.normal, Macros.Color.content)
       
    --选项卡
    local panel_tab = self:GetWidget("Panel_tab") 
    self.__tab:SetFontSize( Macros.TypefaceSize.tab )      
    self.__tab:On(init_type)      --默认显示第一个选项卡的
    panel_tab:AddChild(self.__tab:GetNode())
    self.__tab:SetDirection(true,0)
    PosTool.LeftTop(self.__tab,0,0)

    --计算红点
    self:__AddRedDot()
end

function Modules.FriendView:__RegistAllEvents()
   --移动条目
    self:BindGlobalEvent(Macros.Event.FriendModule.MOVE_ITEM, function(_, move_index)
        local item = self.__scrollView_table[Macros.Game.FRIEND_TYPE.FRIEND_LIST]:GetItem(move_index)
        if item then
            item:SetEneryData(Macros.Global.TRUE,1)
            self.__scrollView_table[Macros.Game.FRIEND_TYPE.FRIEND_LIST]:MoveItemToLast(move_index,true)
        end               
    end)

    --删除条目
    self:BindGlobalEvent(Macros.Event.FriendModule.DELETE_ITEM, function(_,index,friend_type,roleId)
        if roleId then
            index = self:countCurIndex(friend_type,roleId)
        end       
        if self.__scrollView_table[friend_type] and index then
            self.__scrollView_table[friend_type]:DetachItem(index)
            --计算红点
            self:__AddRedDot()
            if friend_type == Macros.Game.FRIEND_TYPE.GET_ENERY then
                self:UpdateEneryNumString()                              
            elseif friend_type == Macros.Game.FRIEND_TYPE.FRIEND_LIST then
                self:UpdateFriendNumString()                            
            end
        end
    end)

     --增加条目
    self:BindGlobalEvent(Macros.Event.FriendModule.ADD_ITEM, function(_, index,friend_type,friend_data) 
        if self.__scrollView_table[friend_type] then
            if friend_type == Macros.Game.FRIEND_TYPE.FRIEND_LIST then
                self:UpdateFriendNumString()                          
            end  
            friend_data.friend_Type = friend_type
            self.__scrollView_table[friend_type]:AttachItem(friend_data,index,true)    
        end  
    end)

    --初始化滚动容器
    self:BindGlobalEvent(Macros.Event.FriendModule.INIT_CORRESPONDING_PANEL, function(_,friend_type,friend_data) 
        print("ldx","friend_typefriend_typefriend_type: ",friend_type)
        self:InitCorrespondingPanel(friend_type,friend_data) 
    end)

end

function Modules.FriendView:__Dispose() 
   for i,scrollview in pairs(self.__scrollView_table) do
       scrollview:DeleteMe()
       scrollview = nil
   end
end

function Modules.FriendView:OnYeMei(index)
    for key,panel in ipairs(self.__panel_table) do
        panel:SetVisible(key == index)
        if self.__scrollView_table[key] then
            self.__scrollView_table[key]:SetVisible(key == index)
        end
    end
    self.__image_scrollviewdeepbg:SetVisible(index ~= Macros.Game.FRIEND_TYPE.ADD_FRIEND)
    --各个好友类型初始化
    if not self.__scrollView_table[index] then  
        if index == Macros.Game.FRIEND_TYPE.FRIEND_LIST then    
            self:CallModuleFunc("GetFriendDataAndSetScrollViewByIndex",index)  
        elseif index == Macros.Game.FRIEND_TYPE.GET_ENERY then  
            --可领取数量文字   
            local txt_couldgettime = self:GetWidget("Text_couldGetTime")
            txt_couldgettime:SetAnchorPoint(1,0.5)
            PosTool.RightBottom(txt_couldgettime,-205,-40)
            Util:WidgetLabel(txt_couldgettime,CPPGameLib.GetString("friend_Text_couldGetTime"),Macros.TypefaceSize.normal,Macros.Color.keypoint)  
            --可领取数量数量
            self.__text_could_gettimenum = self:GetWidget("Text_couleGetTimeNum")
            Util:WidgetLabel(self.__text_could_gettimenum,"",Macros.TypefaceSize.normal,Macros.Color.content)  
            --一键领取按钮         
            Util:WidgetButton(self:GetWidget("Button_oneUseGet"),CPPGameLib.GetString("friend_Button_oneUseGet"),function () 
                local eneryNum =  self:CallModuleFunc("GetHaveGetEneryTimes") 
                local level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
                local config = CPPGameLib.GetConfig("RoleLevel",level)
                if eneryNum >= config.friend_num_max then
                    GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("friend_get_max_energy"))
                    return
                end               
                self:CallModuleFunc("OneUseGetAllEnery")
                --TODO
                --self.GetenergyScrollView:SetDataList(nil)
            end,Macros.TypefaceSize.popup,Macros.Color.btn,Macros.Color.button_yellow) 
            self:CallModuleFunc("GetFriendDataAndSetScrollViewByIndex",index) 
        elseif index == Macros.Game.FRIEND_TYPE.ADD_FRIEND then
            self:CallModuleFunc("FriendRecommendListRequest")
            --输入框
            self.__textfield = Util:WidgetTextField(self:GetWidget("TextField"))
            self.__textfield:SetPlaceHolderText(CPPGameLib.GetString("friend_AddFriendPlaceHolder"))
            self.__textfield:SetPlaceHolderColor(unpack(Macros.Color.content))
            self.__textfield:SetTextColor(unpack(Macros.Color.content))         
            self.__textfield:SetTextHorizontalAlign(UITextFieldEx.ALIGIN_CENTER)
            self.__textfield:SetFontName(Util.FontType)
            self.__textfield:SetFontSize(Macros.TypefaceSize.largish)
            --查找
            self.__findbutton = self:GetWidget("Button_find")
            Util:WidgetButton(self.__findbutton,CPPGameLib.GetString("friend_findButton_text"),function ()
                local string = self.__textfield:GetText()
                if string == "" then
                    GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("friend_input_player_name_tip"))
                    return
                end
                self:CallModuleFunc("FriendTargetFindReq",string)
            end,Macros.TypefaceSize.popup,Macros.Color.btn,Macros.Color.button_yellow)
            --刷新  
             Util:WidgetButton(self:GetWidget("Button_refrash"),CPPGameLib.GetString("friend_refrashButton_text"),function ()
                self:CallModuleFunc("FriendRecommendListRequest")
            end,Macros.TypefaceSize.popup,Macros.Color.btn,Macros.Color.button_yellow)
            --系统推荐文字
            Util:WidgetLabel(self:GetWidget("Text_titleSystem"),CPPGameLib.GetString("friend_AddFriendTitle"),Macros.TypefaceSize.largish, Macros.Color.headline)
        elseif index == Macros.Game.FRIEND_TYPE.APPLY_LIST then
            self:CallModuleFunc("GetFriendDataAndSetScrollViewByIndex",index) 
        elseif index == Macros.Game.FRIEND_TYPE.BLACK_LIST then
            self:CallModuleFunc("BlcakListRequest")
        end
    end    
    if index == Macros.Game.FRIEND_TYPE.GET_ENERY then
        self:UpdateEneryNumString()
    end
end

--初始化对应的Panel
function Modules.FriendView:InitCorrespondingPanel(friend_type,friend_data)
    if not friend_type then
        return
    end
    local height = friend_type == Macros.Game.FRIEND_TYPE.ADD_FRIEND and 310 or 400 
    if friend_type == Macros.Game.FRIEND_TYPE.FRIEND_LIST then
        self:UpdateFriendNumString()  
    elseif friend_type == Macros.Game.FRIEND_TYPE.GET_ENERY then
        self:UpdateEneryNumString()          
    end
    --红点
    self:__AddRedDot()
    --初始化滚动容器
    if not self.__scrollView_table[friend_type] then
        local params = 
        {
            item_class = Modules.TemplAllFriendCell,
            item_width = 616,
            item_height = 122,
            row = 4,
            col = 1,
            item_space_r = 10,
            view_width = 616,
            view_height = height,       
        }
        self.__scrollView_table[friend_type] = GUI.ScrollView.New(params)
        self.__scrollView_panel_table[friend_type]:AddChild(self.__scrollView_table[friend_type]:GetNode())
        PosTool.Center(self.__scrollView_table[friend_type])
    end
    --修改数据
    if friend_data then
        for i,friendData in ipairs(friend_data) do
            friendData.friend_Type = friend_type
        end
    end
    CPPGameLib.PrintTable("ldx",friend_data,"friend_data: ")
    self.__scrollView_table[friend_type]:SetDataList(friend_data)
end

function Modules.FriendView:UpdateFriendNumString()
    local str = ""
    local friendNum =  self:CallModuleFunc("GetFriendNumber") 
    local level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
    local config = CPPGameLib.GetConfig("RoleLevel",level)
    str = str..friendNum.."/"..config.friend_num_max
    self.__text_friendnum:SetText(str)
end

function Modules.FriendView:UpdateEneryNumString()
    local str = ""
    local eneryNum =  self:CallModuleFunc("GetHaveGetEneryTimes") 
    local level = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.LEVEL)
    local config = CPPGameLib.GetConfig("RoleLevel",level)
    str = str..eneryNum.."/"..config.friend_num_max
    self.__text_could_gettimenum:SetText(str)
end

function Modules.FriendView:countCurIndex(index, roleId)
    --列表相关调整
    local ret_index
    local lst = self.__scrollView_table[index]
    if lst then
        lst:ItemDataIter(function(i, item_data)
            if item_data.role_id == roleId then
                ret_index = i
                return true
            end
        end)
    end
    return ret_index
end

function Modules.FriendView:__AddRedDot()
    local red_dot_num = 0
    local red_dot_list = {Macros.Game.FRIEND_TYPE.GET_ENERY,Macros.Game.FRIEND_TYPE.APPLY_LIST}
    for i,friend_type in ipairs(red_dot_list) do
        local friend_type_list = self:CallModuleFunc("GetFriendDataByIndex",friend_type)
        if #friend_type_list > 0 then
            red_dot_num = red_dot_num + 1
            self.__tab:AddRedDot(friend_type)
        else
            self.__tab:HideRedDot(friend_type)
        end
    end    
end