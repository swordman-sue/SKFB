--[[
%% @module: 挑战界面
%% @author: yjg
%% @created: 2016年12月7日18:29:20
--]]

Modules = Modules or {} 
Modules.ChallengeView = Modules.ChallengeView or BaseClass(BaseView)

function Modules.ChallengeView:__init()
    self.__layout_name = ""	
    self.__layout_mode = BaseView.LayoutMode.CODE 
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bp_sp_challenge")}
    self.__title_bar_params = 
    {
      title = CPPGameLib.GetString("challenge"), 
      show_diamond = true,
      show_gold = true,
      show_stamina = true,
      show_energy = true,
      close_func = function()
        self:CloseManual()
      end,
    }

    self.__open_callback = function()
        self.__tab_index = nil
        self:__OpenCallback()
    end
    self:__AddPreloadTextureAtlas( Resource.UITextureAtlasType.CHALLENGE )  
    self:__AddPreloadTextureAtlas( Resource.UITextureAtlasType.CHALLENGE ,true )  
end

function Modules.ChallengeView:__OpenCallback( )
    self:__Getwidget()
    self:__SettleData()
end	

function Modules.ChallengeView:__Dispose()   
    if self.__zones_scrollview then
        self.__zones_scrollview:DeleteMe()
        self.__zones_scrollview = nil
    end
end

function Modules.ChallengeView:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height)
    self.layout:SetClippingEnable(false)
    self.layout:SetAnchorPoint(0.5,0.5)
    return self.layout
end

function Modules.ChallengeView:__Getwidget( )
 
end 

function Modules.ChallengeView:__SettleData( )
    local lst = {}
    for k,v in pairs( CPPGameLib.GetConfig("Challenge", nil, nil , true ) ) do
        local cnf = CPPGameLib.GetConfig("SystemEntry", k)
        local info = {}
        info.id = k
        info.name = cnf.name
        info.prepare = cnf.prepare_level
        info.open = cnf.level
        info.module = cnf.module_type
        info.view = cnf.view_type

        info.frame = v.type_frame_pic
        info.bg = v.type_bg_pic
        info.name = v.type_name_pic
        info.introduce = v.introduce
        info.reward = v.reward_icon
        table.insert(lst , info )
    end
    table.sort( lst, function ( a , b )
        return a.open < b.open
    end )

    self:__ShowUI( lst )
end 

function Modules.ChallengeView:__ShowUI( lst )
    if #lst > 3 then
        local params =  
        {
            item_class = Modules.TemplChallengeItem,
            item_width = 300 ,
            item_height = 600 ,
            row = 1,
            col = 3,
            horizon = true,
            item_space_c = 40 ,
            selectable = true,
            select_callback = function( index , data )
                self.__tab_index = index
                local level = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo").level
                if level < data.open then
                    GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("challenge_insufficient" , data.open) )
                elseif data.id == Macros.Game.SystemID.LIMIT_TEAM then
                    CallModuleFunc(ModuleType.SCENE, "EnterSceneReq",Macros.Game.SceneType.TEAM_HALL,1)
                else
                    GlobalModulesMgr:OpenSystemModule(data.id, nil, false)
                end
            end,

            view_width = self.layout:GetContentWidth(),
        }
        -- 创建参数
        self.__zones_scrollview = GUI.ScrollView.New(params)
        self.__zones_scrollview:SetDataList( lst )
        self.layout:AddChild(self.__zones_scrollview:GetNode())
        PosTool.Center(self.__zones_scrollview:GetNode() )

        self:__CheckSectionOperateSP()

        if not CallModuleFunc(ModuleType.LEAD, "IsInLead") then
            self.__zones_scrollview:LocateItem( unpack(self.__open_data) or 1 )
        end

    else
        local challengeItem = Modules.TemplChallengeItem.New()
        local layou = Util:Layout()
        layou:SetAnchorPoint(0.5,0.5)
        layou:SetTouchSwallow(false)
        self.layout:AddChild(layou)
        PosTool.Center( layou )

        local idenx = 5
        if #lst == 2 then
            idenx = 160 
        end

        layou:SetContentSize(#lst * (challengeItem:GetNode():GetContentWidth() + idenx) , challengeItem:GetNode():GetContentHeight() )

        local interval = layou:GetContentWidth() / #lst
        for i,v in ipairs(lst) do
            local icon = Modules.TemplChallengeItem.New()
            icon:SetData(v)
            icon:SetPosition(  (icon:GetNode():GetContentWidth()/2) + (idenx/2) + interval * (i - 1)  , layou:GetContentHeight()/2)
            icon:SetAnchorPoint(0.5,0.5)
            layou:AddChild(icon:GetNode())
            
            local layer = icon:GetChangeWidget()
            WidgetUnity.SetWidgetListener( layer , nil,nil,function ( ... )
                local level = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo").level
                if level < v.open then
                    GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("challenge_insufficient" , v.open) )
                else
                    GlobalModulesMgr:OpenSystemModule(v.id, nil, false)
                end
            end )

            icon:CheckSectionOperateSP()
        end  
    end
end

--新手指引特殊操作检测
function Modules.ChallengeView:__CheckSectionOperateSP()
    local operate_id, data_id = CallModuleFunc(ModuleType.LEAD, "GetOperateInfo")
    if operate_id == Macros.Game.LeadOperateSpecialID.CHALLENGE_CHOOSE_ENTRY then
        local locate_index
        self.__zones_scrollview:ItemDataIter(function(item_index, item_data)
            if item_data.id == data_id then
                locate_index = item_index
                return true
            end
        end)                
        if locate_index then
            self.__zones_scrollview:LocateItem(locate_index, true)
            local item = self.__zones_scrollview:GetItem(locate_index)
            if item then
                item:CheckSectionOperateSP()
            end
        end
    end
end

--获取打开参数
function Modules.ChallengeView:GetOpenParams()
    if self.__tab_index then
        return {self.__tab_index}
    end
end

--------------------------------------------------------------------------------------------------------------------------------
--系统入口条目
--------------------------------------------------------------------------------------------------------------------------------
Modules = Modules or {}

Modules.TemplChallengeItem = Modules.TemplChallengeItem or BaseClass(GUI.ScrollItem)

function Modules.TemplChallengeItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
    self:__Getwidget()
end

function Modules.TemplChallengeItem:__delete()
end

function Modules.TemplChallengeItem:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(300,600)
    self.layout:SetAnchorPoint(0.5,0.5)
    return self.layout
end

function Modules.TemplChallengeItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self:__ShowUI(data)
end

function Modules.TemplChallengeItem:OnTou(key)
    self.key = key
end

function Modules.TemplChallengeItem:__AddToParent()
end

function Modules.TemplChallengeItem:__Getwidget( ... )
end

function Modules.TemplChallengeItem:__ShowUI( data )
    --背景
    local sp = Resource.PathTool.GetUIResPath("challenge_bg/".. data.bg ),TextureResType.LOCAL
    local bg = Util:Sprite( sp )
    bg:SetAnchorPoint(0.5,0.5)
    bg:SetPosition(self.layout:GetContentWidth()/2 , self.layout:GetContentHeight()/2 + 60 )
    self.layout:AddChild(bg)  

    --框
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHALLENGE, data.frame)
    local frame = Util:Sprite(sp)
    frame:SetAnchorPoint(0.5,0.5)
    frame:SetPosition(self.layout:GetContentWidth()/2 , self.layout:GetContentHeight()/2 + 30 )
    self.layout:AddChild(frame)

    --字
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHALLENGE, data.name , true)
    local name = Util:Sprite(sp)
    name:SetAnchorPoint(0.5,0.5)
    name:SetPosition(frame:GetContentWidth()/2 , frame:GetContentHeight()/2 - 148 )
    frame:AddChild(name)

    --描述
    local introduce = Util:Name(data.introduce , Macros.TypefaceSize.minimum , nil , nil ,frame:GetContentWidth() - 60 ) 
    introduce:SetPosition(frame:GetContentWidth()/2 , frame:GetContentHeight()/3 - 80   )
    introduce:SetAnchorPoint(0.5 , 1)
    frame:AddChild(introduce)

     --图标容器
    local layou = Util:Layout()
    layou:SetAnchorPoint(0.5,0.5)
    layou:SetPosition(frame:GetContentWidth()/2 , -50 )
    layou:SetTouchEnabled(true)
    frame:AddChild(layou)
    layou:SetContentSize(frame:GetContentWidth() , 90 )


    --图标容器
    local layou = Util:Layout()
    layou:SetAnchorPoint(0.5,0.5)
    layou:SetPosition(frame:GetContentWidth()/2 , -50 )
    layou:SetTouchEnabled(true)
    frame:AddChild(layou)
    layou:SetContentSize(#data.reward * 90 , 90 )
    local interval = layou:GetContentWidth() / #data.reward


    for i,v in ipairs(data.reward) do
        local icon = LogicGUI.IconLayer.New(true)
        icon:SetPosition( 45 + interval * (i - 1)  , layou:GetContentHeight()/2)
        icon:SetData({id = v})
        icon:SetAnchorPoint(0.5,0.5)
        icon:SetTouchSwallow(false)
        layou:AddChild(icon:GetNode())
    end  

    --未开启的置灰
    local level = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo").level
    local openLevel = true
    if level < data.open then
        local layou = Util:Layout( bg:GetContentWidth() , bg:GetContentHeight() )
        layou:SetAnchorPoint(0.5,0.5)
        layou:SetPosition( bg:GetPositionX() , bg:GetPositionY() )
        layou:SetBackGroundColor(0, 0, 0)
        layou:SetBackGroundColorType(1)
        layou:SetBackGroundColorOpacity(120)
        self.layout:AddChild(layou)      

        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHALLENGE, "challenge_lock")
        local lock = Util:Sprite(sp)
        lock:SetAnchorPoint(0.5,0.5)
        lock:SetPosition(layou:GetContentWidth()/2 , layou:GetContentHeight()/2 )
        layou:AddChild(lock)

        local text = Util:Name( CPPGameLib.GetString("challenge_insufficient" , data.open) , Macros.TypefaceSize.minimum)
        text:SetAnchorPoint(0,0.5)
        text:SetPosition( 65 , lock:GetContentHeight()/2 - 2 )
        lock:AddChild(text)

        openLevel = false
    end

    --红点
    local type_ = CallModuleFunc(ModuleType.RED_MGR , "GetRed" ,data.id )
    if openLevel == true and type_ ~= 0 then
        local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
        local icon_red_point = Util:Sprite(sp)
        icon_red_point:SetPosition(frame:GetContentWidth() - 40 , frame:GetContentHeight() / 2 - 120 )
        frame:AddChild(icon_red_point , 20)
    end

    --触摸
    self.layou = Util:Layout( bg:GetContentWidth() , bg:GetContentHeight() ,function ( ... )

    end ) 
    self.layou:SetAnchorPoint(0.5,0.5)
    self.layou:SetTouchSwallow(false)
    self.layou:SetPosition( frame:GetContentWidth()/2 , frame:GetContentHeight()/2 )
    frame:AddChild(self.layou)
end

function Modules.TemplChallengeItem:GetChangeWidget()
    return self.layou
end

--新手指引特殊操作检测(挑战界面-选择入口（系统ID）)
function Modules.TemplChallengeItem:CheckSectionOperateSP()
    CallModuleFunc(ModuleType.LEAD, "CheckSectionOperateSP", Macros.Game.LeadOperateSpecialID.CHALLENGE_CHOOSE_ENTRY, self.layou, self.__data.id)                                          
end