--
-- @brief 查看玩家信息 
-- @author: yjg
-- @date: 2016年8月8日15:53:48
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplCheckRoleInfo = LogicGUI.TemplCheckRoleInfo or BaseClass(GUI.Template)

function LogicGUI.TemplCheckRoleInfo:__init(data)
    self.__is_auto_load = true
    self.__open_callback = function()
        self.__data = data
        self:__Getwidget()
        self:__ShowUI(data)
        CallModuleFunc(ModuleType.CHECKROLEINFO, "RoleDetailsReq", Macros.Game.HeroMessageReq_Type.CHAT , data.role_id) 
        --刷新
        self.event_chat_check = self:BindGlobalEvent(Macros.Event.ChatModule.CHAT_CHECK,function (_ , data)
            self:SetShowUI(data)
        end)
    end
    self:InitTemplate(GUI.TemplateRegistry.CHECKROLEINFO)
end

function LogicGUI.TemplCheckRoleInfo:__delete()

    if self.__shield_layer then
        self.__shield_layer:DeleteMe()
        self.__shield_layer = nil
    end
end

function LogicGUI.TemplCheckRoleInfo:__Getwidget( ... )
    --背景
    self.img_BG = self:GetWidget("img_BG") 
    --关闭按钮
    self.btn_close = Util:WidgetButton( self:GetWidget("btn_close") , nil , CPPGameLib.Handler(self,self.OnBtnClose) )
    --标题
    self.img_labBg = self:GetWidget("img_labBg") 
    --头像
    self.Panel_head = self:GetWidget("Panel_head") 
    --列表框
    self.Image_1 = self:GetWidget("Image_1") 
    --名字
    self.Text_name = self:GetWidget("Text_name") 
    --等级
    self.Text_lv = self:GetWidget("Text_lv") 
    --公会
    self.Text_union = self:GetWidget("Text_union") 
    --战斗力
    self.Text_atk = self:GetWidget("Text_atk") 
    
    local tab = {
            [1] = {zi = CPPGameLib.GetString("jiahaoyou") , fun = function ( ... )
                self:OnBtn_friend()
            end},
            [2] = {zi = CPPGameLib.GetString("lahei") , fun = function ( ... )
                self:OnBtn_blacklist()
            end},
            [3] = {zi = CPPGameLib.GetString("siliao") , fun = function ( ... )
                self:OnBtn_quietly()
            end},
            [4] = {zi = CPPGameLib.GetString("chakanzhenrong") , fun = function ( ... )
                self:OnBtn_peep() 
            end},
            [5] = {zi = CPPGameLib.GetString("qiecuo") , fun = function ( ... )
                self:OnBtn_hit() 
            end},
        }

    self.Button = {}
    for i = 1 , 5 do
        local color = Macros.Color.button_yellow
        if i == 1 then
            color = Macros.Color.button_red
        end
        self.Button[i] = Util:WidgetButton( self:GetWidget("Button_"..i) , tab[i].zi , function ( ... )
            tab[i].fun(i)
        end ,nil,nil, color)
    end

end

function LogicGUI.TemplCheckRoleInfo:__ShowUI( ... )
    local bt = Util:Label(CPPGameLib.GetString("wanjiaxinxi"),Macros.TypefaceSize.popup,Macros.Color.lilyWhite)
    bt:SetPosition(self.img_labBg:GetContentWidth()/2 ,self.img_labBg:GetContentHeight()/2)
    self.img_labBg:AddChild(bt)

    --头像
    self.sp = LogicGUI.RoleIcon.New()
    -- self.sp:SetData(self.info.head_icon)
    self.sp:SetPosition(self.Panel_head:GetContentWidth()/2 , self.Panel_head:GetContentHeight()/2)
    self.Panel_head:AddChild(self.sp:GetNode())
    --名字
    self.name = Util:Label(" ",Macros.TypefaceSize.largish, Macros.Color.headline )
    self.name:SetAnchorPoint(0,0.5)
    self.name:SetPosition(self.Text_name:GetContentWidth()/2,self.Text_name:GetContentHeight()/2)
    self.Text_name:AddChild(self.name)

    --等级
    local lab_lv = Util:Label(CPPGameLib.GetString("dengji"), Macros.TypefaceSize.normal , Macros.Color.keypoint )
    lab_lv:SetAnchorPoint(0, 0.5) 
    lab_lv:SetPosition(self.Text_lv:GetContentWidth()/2 , self.Text_lv:GetContentHeight()/2 )
    self.Text_lv:AddChild(lab_lv)  

    --等级value
    self.lab_lv_value = Util:Label(" ", Macros.TypefaceSize.normal, Macros.Color.content )
    PosTool.RightTo(lab_lv,self.lab_lv_value)
    self.Text_lv:AddChild(self.lab_lv_value) 

    --公会
    local lab_union = Util:Label(CPPGameLib.GetString("gonghui"), Macros.TypefaceSize.normal, Macros.Color.keypoint )
    lab_union:SetAnchorPoint(0, 0.5)
    lab_union:SetPosition(self.Text_union:GetContentWidth()/2, self.Text_union:GetContentHeight()/2 )
    self.Text_union:AddChild(lab_union)  

    --公会value
    self.lab_union_value = Util:Label(" ", Macros.TypefaceSize.normal, Macros.Color.content )
    PosTool.RightTo(lab_union,self.lab_union_value)
    self.Text_union:AddChild(self.lab_union_value) 

    --战力
    local lab_strength = Util:Label(CPPGameLib.GetString("zhandouli"),Macros.TypefaceSize.normal, Macros.Color.keypoint )
    lab_strength:SetAnchorPoint(0, 0.5)
    lab_strength:SetPosition(self.Text_atk:GetContentWidth()/2,self.Text_atk:GetContentHeight()/2)
    self.Text_atk:AddChild(lab_strength)  

    --战力value
    self.lab_strength_value = Util:Label(" ",Macros.TypefaceSize.normal, Macros.Color.content )
    PosTool.RightTo(lab_strength,self.lab_strength_value)
    self.Text_atk:AddChild(self.lab_strength_value)   

    --vip图样
    local vip_title_icon_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "vip_title_icon" , true )
    self.vip_title_icon = Util:Sprite(vip_title_icon_sp)
    self.img_BG:AddChild( self.vip_title_icon )
    self.vip_title_icon:SetScaleFactor( 1 )
    self.vip_title_icon:SetAnchorPoint( 0 , 0.5 )
    PosTool.Center( self.vip_title_icon , 80 , 90 )
    self.vip_title_icon:SetVisible(false)

    --VIP等级
    local vipnum = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "vipnum", true)
    self.viplv = GUI.ImgFont.New(vipnum)
    self.viplv:SetText(tostring(0))
    self.img_BG:AddChild(self.viplv:GetNode()) 
    PosTool.RightTo( self.vip_title_icon , self.viplv )
    self.viplv:SetVisible(false)
end

function LogicGUI.TemplCheckRoleInfo:SetShowUI()
    self.info = CallModuleFunc(ModuleType.CHECKROLEINFO, "GetPalyInfo")
    -- CPPGameLib.PrintTable(self.info,"self.info")

    if self.info.vip_level and self.info.vip_level ~= 0 then
        self.vip_title_icon:SetVisible(true)
        self.viplv:SetVisible(true)
    end
    self.viplv:SetText( tostring(self.info.vip_level) )

    self.sp:SetData(self.info.head_icon)

    self.name:SetString(self.info.role_name)

    self.lab_lv_value:SetString(self.info.level)

    local guild_name = CPPGameLib.GetString("weijiarugonghui")
    if self.info.guild_name ~= "" then
        guild_name = self.info.guild_name
    end
    self.lab_union_value:SetString(guild_name)

    self.lab_strength_value:SetString(self.info.battle_value)

    if self.info.is_friend == 1 then
        self.Button[1]:SetText(CPPGameLib.GetString("friend_delete"))
        WidgetUnity.SetWidgetListener(self.Button[1],nil,nil,function( ... )
            self:OnBtn_NoFriend()
        end)      
    end

    if self.info.is_black == 1 then
        self.Button[2]:SetText(CPPGameLib.GetString("friend_remove_black"))  
        WidgetUnity.SetWidgetListener(self.Button[2],nil,nil,function( ... )
            self:OnBtn_NoBlackList()
        end)  
    end
end

--关闭按钮
function LogicGUI.TemplCheckRoleInfo:OnBtnClose( ... )
    self:DeleteMe()
end

--好友
function LogicGUI.TemplCheckRoleInfo:OnBtn_friend( ... )
    print("添加好友")
    CallModuleFunc(ModuleType.CHECKROLEINFO, "AddFriend",self.__data.role_id)
    self:OnBtnClose()
end

--删除好友
function LogicGUI.TemplCheckRoleInfo:OnBtn_NoFriend( ... )
    CallModuleFunc(ModuleType.CHECKROLEINFO, "RemoveFriend",self.__data.role_id)
    self:OnBtnClose()
end

--拉黑
function LogicGUI.TemplCheckRoleInfo:OnBtn_blacklist( ... )
    print("拉黑")
    CallModuleFunc(ModuleType.CHECKROLEINFO, "AddShield",self.__data.role_id)
    self:OnBtnClose()
end

--取消拉黑
function LogicGUI.TemplCheckRoleInfo:OnBtn_NoBlackList( ... )
    CallModuleFunc(ModuleType.CHECKROLEINFO, "RemoveShield",self.__data.role_id)
    self:OnBtnClose()
end

--私聊
function LogicGUI.TemplCheckRoleInfo:OnBtn_quietly( ... )
    CallModuleFunc(ModuleType.CHAT, "SetWhisper", self.info) 
    CallModuleFunc(ModuleType.CHAT, "OpenView", BaseViewType.CHAT, Macros.Game.CHAT.PRIVATE)

    self:OnBtnClose()
end

--查看
function LogicGUI.TemplCheckRoleInfo:OnBtn_peep( ... )
    print("查看")
    GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.PLAYER_DETAILMESSAGE, false, self.info)
end

--切磋
function LogicGUI.TemplCheckRoleInfo:OnBtn_hit( ... )
    print("切磋")
    self:OnBtnClose()
end
