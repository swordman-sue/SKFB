--[[
%% @module: 无尽深渊
%% @author: yjg
%% @created: 2017年7月17日17:31:09
--]]

Modules = Modules or {} 
Modules.ChasmRankView = Modules.ChasmRankView or BaseClass(BaseView)

function Modules.ChasmRankView:__init()
    self.__layout_name = "chasm_rank"  
    self.__bg_params = {type = BaseView.BGType.IMAGE , image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_cham_rank") }
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("chasm_rank"),
        close_func = function()
            self:CloseManual()
        end,
    }
    self.__open_callback = function()
        self:__OpenCallback()
    end
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.CHASM_RANK)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.CHASM_RANK,true)
    self:__AddPreloadTemplate(GUI.TemplateRegistry.TEMPL_CHASM_RANK_ITEM)
end

function Modules.ChasmRankView:__OpenCallback( )
    self:__Getwidget()
    -- self:__SetPuppetStage()
    self:__ShowUI()
    self:__Server()
    
end	

function Modules.ChasmRankView:__Dispose()
end

function Modules.ChasmRankView:__Server()
    CallModuleFunc(ModuleType.CHASM, "EndlessRankReq")

    self:BindGlobalEvent(Macros.Event.ChasmModule.CHASM_RANK,function (_, data)
        self:__SetShowUI()
    end)

end

function Modules.ChasmRankView:__Getwidget( )
    -- --背景
    -- self.Image_bg = self:GetWidget("Image_bg") 

    --光
    self.Image_light = self:GetWidget("Image_light") 

    -- --关闭
    -- self.Button_close = Util:WidgetButton( 
    --                                         self:GetWidget("Button_close") , 
    --                                         nil , 
    --                                         CPPGameLib.Handler(self,self.OnButtonClose)
    --                                     )
    --列表背景
    self.Image_lst = self:GetWidget("Image_lst") 

    --列表
    self.Panel_lst = self:GetWidget("Panel_lst") 
    
    --进入
    self.Button_award = Util:WidgetButton( 
                                            self:GetWidget("Button_award") , 
                                            nil, 
                                            CPPGameLib.Handler(self,self.OnButtonAward),
                                            nil,nil,Macros.Color.button_yellow
                                        )

    local chasm_award = Util:Label( CPPGameLib.GetString("chasm_award") , Macros.TypefaceSize.largish , Macros.Color.btn , nil , nil , Macros.Color.button_yellow )
    chasm_award:SetAnchorPoint( 0.5 , 0.5 )
    chasm_award:SetPosition( self.Button_award:GetContentWidth()/2  , self.Button_award:GetContentHeight()/2 + 15 )
    self.Button_award:AddChild( chasm_award )

    --英雄
    self.Panel_icon = {} 
    for i = 1 , 3 do
        self.Panel_icon[i] = self:GetWidget("Panel_icon_"..i) 
    end

end 

function Modules.ChasmRankView:__ShowUI()

    local chasm_rankUpTips = Util:Label( CPPGameLib.GetString("chasm_rankUpTips") , Macros.TypefaceSize.largish , Macros.Color.Special )
    chasm_rankUpTips:SetAnchorPoint( 0.5 , 0.5 )
    chasm_rankUpTips:SetPosition( self.Panel_icon[1]:GetPositionX() , 40 )
    self:AddChild( chasm_rankUpTips )

    -- self.server = {}
    self.name = {}

    for i = 1 , #self.Panel_icon do
        local rank_icon = Util:Sprite( Resource.PathTool.GetTitleIconPath( "sp_title_".. ( 2000 + (i - 1 ) ) , true ) )
        rank_icon:SetAnchorPoint( 0.5 , 0.5 )
        self.Panel_icon[i]:AddChild( rank_icon )
        PosTool.Center( rank_icon , 0 , 250 )

        -- self.server[i] = Util:Name( " " , Macros.TypefaceSize.normal )
        -- self.server[i]:SetAnchorPoint(0.5,0.5)
        -- self.server[i]:SetPosition( rank_icon:GetContentWidth()/2 , rank_icon:GetPositionY() - rank_icon:GetContentHeight() )
        -- self.Panel_icon[i]:AddChild( self.server[i] )

        local com_sp_rank_num = Util:Sprite( Resource.LoadTool.GetImageData( Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp_rank_num_"..i ) )
        com_sp_rank_num:SetAnchorPoint( 1 , 0.5 )
        com_sp_rank_num:SetPosition( rank_icon:GetPositionX() - (rank_icon:GetContentWidth()/2) , (self.Panel_icon[i]:GetContentHeight()/2) + 250 )
        self.Panel_icon[i]:AddChild( com_sp_rank_num )

        self.name[i] = Util:Name( " " , Macros.TypefaceSize.normal )
        self.name[i]:SetAnchorPoint(0.5 , 1 )
        self.name[i]:SetPosition( self.Panel_icon[i]:GetContentWidth()/2 , self.Panel_icon[i]:GetContentHeight() - 30 )
        self.Panel_icon[i]:AddChild( self.name[i] )
    end


   local params =  
    {
        item_class = Modules.ChasmRankItem,
        item_width = self.Panel_lst:GetContentWidth() ,
        item_height = 90.00,
        row = 4,
        col = 1,
        -- item_space_r = 5,
        view_height = self.Panel_lst:GetContentHeight() - 5 ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    self.Panel_lst:AddChild(self.lst_list:GetNode())
    PosTool.Center(self.lst_list:GetNode())


    local tab = {
                 { zi = CPPGameLib.GetString("chasm_myRank") , ys = Macros.Color.Special , dx = Macros.TypefaceSize.normal },
                 { zi = CPPGameLib.GetString("chasm_myRank_" , CPPGameLib.GetString("tower_noranking") , 0 ) , ys = Macros.Color.white , dx = Macros.TypefaceSize.normal },
                }

    --我的排名
    local rank_myRank = nil
    rank_myRank , self.rank_myRank = Util:LabCenter( tab )
    rank_myRank:SetAnchorPoint( 0.5 , 0.5 )
    self.Image_lst:AddChild( rank_myRank )
    PosTool.Center( rank_myRank , 0 , - 163 )

end

function Modules.ChasmRankView:__SetShowUI()
    local lst = CallModuleFunc(ModuleType.CHASM, "GetRankLst")
    --设置模型
    for i = 1 , #self.Panel_icon do
        if lst[i] then 
            local hero = LogicGUI.Model.New({info_id = lst[i].head_icon  , parent = self.Panel_icon[i] })
            hero:SetScaleFactor( 0.9 )
            PosTool.Center( hero , 0 , self.Panel_icon[i]:GetContentHeight()/2 )

            -- local lst_ = CallModuleFunc(ModuleType.LOGIN, "GetServerInfo", lst[i].server_id)
            -- local text =  CPPGameLib.GetString("common_district" , lst_.id ) .. " - " .. lst_.name
            -- self.server[i]:SetString(text)
            self.name[i]:SetString( lst[i].role_name or " " )
        else
            local rank_icon = Util:Sprite( Resource.LoadTool.GetImageData( Resource.UITextureAtlasType.CITY_COMMON_NEW, "sp_chasm_copy" , true ) )
            rank_icon:SetAnchorPoint( 0.5 , 0 )
            self.Panel_icon[i]:AddChild( rank_icon )
            PosTool.Center( rank_icon , 0 , self.Panel_icon[i]:GetContentHeight() - 50  )
            -- self.server[i]:SetString(" ")
            self.name[i]:SetString(" ")
        end
    end

    --设置列表
    self.lst_list:SetDataList( lst )

    --我的信息
    local rank = CallModuleFunc(ModuleType.CHASM, "GetMyRank")
    local rank_text = CPPGameLib.GetString( "tower_noranking" )
    if rank ~= 0 then
        rank_text = CPPGameLib.GetString("rebel_mingciqujian1", rank )
    end
    self.rank_myRank[2]:SetString( CPPGameLib.GetString("chasm_myRank_" , rank_text , CallModuleFunc(ModuleType.CHASM, "GetKeepKillNum") ) )
end

--奖励
function Modules.ChasmRankView:OnButtonAward()
    LogicGUI.TemplChasmRank.New()
end




Modules = Modules or {}
Modules.ChasmRankItem = Modules.ChasmRankItem or BaseClass(GUI.ScrollItem)

function Modules.ChasmRankItem:__init()
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_CHASM_RANK_ITEM)   
    self:__Getwidget()
end

function Modules.ChasmRankItem:__delete()

end

function Modules.ChasmRankItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    self:__ShowUI(data)
end

function Modules.ChasmRankItem:__Getwidget()
    --背景
    self.Image_item = self:GetWidget("Image_item")
    --排行
    self.Panel_rank = self:GetWidget("Panel_rank")
    -- -- --内容
    -- self.Image_content = self:GetWidget("Image_content")
    --玩家头像
    self.Panel_icon = self:GetWidget("Panel_icon")
    --名字
    self.Text_name = self:GetWidget("Text_name")
    --力量
    self.Text_power = self:GetWidget("Text_power")
    --最高击杀
    self.Text_maxkill = self:GetWidget("Text_maxkill")
end

function Modules.ChasmRankItem:__ShowUI( data )
    --排位
    local imageData = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHASM_RANK,"sp_chasm_rank_"..4)
    if data.rank < 4 then
        imageData = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CHASM_RANK,"sp_chasm_rank_"..data.rank)

        local icon = Util:Sprite( Resource.LoadTool.GetImageData( Resource.UITextureAtlasType.CITY_COMMON_NEW, "com_sp_rank_num_"..data.rank ) )
        icon:SetAnchorPoint( 0.5 , 0.5 )
        icon:SetPosition( 0 + (icon:GetContentWidth()/2) + 5 , self.Image_item:GetContentHeight()/2 )
        self.Image_item:AddChild(icon)
    else

        local icon = Util:Label( data.rank , Macros.TypefaceSize.popup , Macros.Color.headline )
        icon:SetAnchorPoint( 0.5 , 0.5 )
        icon:SetPosition( 5 + 30 , self.Image_item:GetContentHeight()/2 )
        self.Image_item:AddChild(icon)
    end
    self.Image_item:SetImage( imageData )

    --图标
    local icon = LogicGUI.RoleIcon.New(true)
    self.Panel_icon:AddChild(icon:GetNode())
    icon:SetTouchCallback(function()
        GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.CHECKROLEINFO , false , data )
    end)
    icon:SetData(data.head_icon)
    PosTool.Center(icon)
    icon:SetScaleFactor( 0.85 )

    --玩家名
    Util:WidgetLabel( self.Text_name , data.role_name , Macros.TypefaceSize.normal , Macros.Color.headline )

    --战斗力
    Util:WidgetLabel( self.Text_power , CPPGameLib.GetString("zhandouli") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
    local battle_value = Util:Label( data.battle_value , Macros.TypefaceSize.normal , Macros.Color.content )
    self.Image_item:AddChild( battle_value )
    PosTool.RightTo( self.Text_power , battle_value )

    --最高击杀
    Util:WidgetLabel( self.Text_maxkill , CPPGameLib.GetString("chasm_maxKill") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
    local context = Util:Label( data.context , Macros.TypefaceSize.normal , Macros.Color.content )
    self.Image_item:AddChild( context )
    PosTool.RightTo( self.Text_maxkill , context )
end



--
-- @brief 获得弹窗
-- @author: yjg
-- @date: 2016年10月28日17:51:53
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplChasmRank = LogicGUI.TemplChasmRank or BaseClass(GUI.Template)

function LogicGUI.TemplChasmRank:__init()
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID}   
    self.__open_callback = function()
        self:__Getwidget()
        self:__ShowUI()
    end

    self:InitTemplate(GUI.TemplateRegistry.TEMPL_CHASM_RANK ) 
end

function LogicGUI.TemplChasmRank:__delete()
end

function LogicGUI.TemplChasmRank:__Getwidget()
    --背景
    self.Image_bg = self:GetWidget("Image_bg")
    --光
    self.Image_light = self:GetWidget("Image_light")
    --关闭
    self.Button_close = Util:WidgetButton( self:GetWidget("Button_close") , nil , CPPGameLib.Handler(self,self.OnButtonClose) )
    --内容
    self.Image_content = self:GetWidget("Image_content")
    --标题1
    self.Image_headline_1 = self:GetWidget("Image_headline_1")
    --标题2
    self.Image_headline_2 = self:GetWidget("Image_headline_2")
    --列表背景
    self.Panel_lst = self:GetWidget("Panel_lst")
end

function LogicGUI.TemplChasmRank:__ShowUI()

    --标题
    self.light = Util:Label( CPPGameLib.GetString("chasm_rank") , Macros.TypefaceSize.popup , Macros.Color.white  )
    self.light:SetPosition(self.Image_light:GetContentWidth()/2 , self.Image_light:GetContentHeight()/2)
    self.Image_light:AddChild(self.light)

    --标题1
    local chasm_packAward = Util:Label( CPPGameLib.GetString("chasm_packAward") , Macros.TypefaceSize.popup , Macros.Color.white  )
    chasm_packAward:SetPosition(self.Image_headline_1:GetContentWidth()/2 , self.Image_headline_1:GetContentHeight()/2)
    self.Image_headline_1:AddChild(chasm_packAward)

    --标题2
    local chasm_titleAward = Util:Label( CPPGameLib.GetString("chasm_titleAward") , Macros.TypefaceSize.popup , Macros.Color.white  )
    chasm_titleAward:SetPosition(self.Image_headline_2:GetContentWidth()/2 , self.Image_headline_2:GetContentHeight()/2)
    self.Image_headline_2:AddChild(chasm_titleAward)

    --我的排名
    local chasm_myRank = Util:Label( CPPGameLib.GetString("chasm_myRank") , Macros.TypefaceSize.normal , Macros.Color.keypoint  )
    chasm_myRank:SetAnchorPoint( 0 , 0.5 )
    chasm_myRank:SetPosition( 10 , 20 )
    self.Image_bg:AddChild(chasm_myRank)

    local rank = CallModuleFunc(ModuleType.CHASM, "GetMyRank")
    local rank_text = CPPGameLib.GetString( "tower_noranking" )
    if rank ~= 0 then
        rank_text = CPPGameLib.GetString("rebel_mingciqujian1", rank )
    end

    local chasm_myRank_ = Util:Label( rank_text , Macros.TypefaceSize.normal , Macros.Color.content  )
    self.Image_bg:AddChild(chasm_myRank_)
    PosTool.RightTo( chasm_myRank , chasm_myRank_ )

    local chasm_awardTips = Util:Label( CPPGameLib.GetString("chasm_awardTips") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
    chasm_awardTips:SetAnchorPoint( 1 , 0.5 )
    chasm_awardTips:SetPosition( self.Image_bg:GetContentWidth() - 10 , chasm_myRank:GetPositionY() )
    self.Image_bg:AddChild(chasm_awardTips)


    --排行榜
    local params =  
    {
        item_class = Modules.TemplChasmRankItem,
        item_width = self.Panel_lst:GetContentWidth() ,
        item_height = 65,
        row = 4,
        col = 1,
        view_height = self.Panel_lst:GetContentHeight() - 5 ,
    }
    -- 创建参数
    self.rank_list = GUI.ScrollView.New(params)
    self.Panel_lst:AddChild(self.rank_list:GetNode())
    PosTool.Center(self.rank_list:GetNode())

    local t1 = CallModuleFunc( ModuleType.CHASM , "GetRankReward" )
    self.rank_list:SetDataList( t1 )
end

function LogicGUI.TemplChasmRank:OnButtonClose()
    self:DeleteMe()
end



--奖励条目
Modules = Modules or {}
Modules.TemplChasmRankItem = Modules.TemplChasmRankItem or BaseClass(GUI.ScrollItem)

function Modules.TemplChasmRankItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplChasmRankItem:__CreateWidgetFromLuaCode()
  self.layout = Util:Layout( 587 , 65 )
  self.layout:SetClippingEnable(false)
  self.layout:SetTouchEnabled(false)
  self.layout:SetBackGroundColor(0, 0, 0)
  return self.layout
end

function Modules.TemplChasmRankItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)
    local sp = nil
    if data.show == true then
        sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_lst_rankdeep")
        local sp_2 = Util:Sprite9(sp)
        sp_2:SetContentSize(self.layout:GetContentWidth() - 10  , self.layout:GetContentHeight() )
        sp_2:SetPosition(self.layout:GetContentWidth() / 2 , self.layout:GetContentHeight() /2)
        self.layout:AddChild(sp_2)
    end

    if not data.text then return end

    if data.title_id ~= 0 then
        local rankSp = Resource.PathTool.GetTitleIconPath( "sp_title_".. data.title_id , true )
        local rank_1 = Util:Sprite( rankSp )
        rank_1:SetAnchorPoint( 0.5 , 0.5 )
        rank_1:SetPosition(self.layout:GetContentWidth() / 2 + 201 , self.layout:GetContentHeight() /2)
        self.layout:AddChild(rank_1)
    end

    local text = Util:Label( CPPGameLib.GetString( "pvp_ranking" , data.text ) , Macros.TypefaceSize.normal , Macros.Color.headline )
    text:SetAnchorPoint( 1 , 0.5 )
    text:SetPosition( 150 , self.layout:GetContentHeight()/2 )
    self.layout:AddChild( text )

    local x = 180
    for i , v in ipairs( data.item ) do
        local icon = LogicGUI.IconLayer.New()
        icon:SetPosition( x , self.layout:GetContentHeight()/2 )
        icon:SetScaleFactor( 0.7 )
        self.layout:AddChild(icon:GetNode())
        icon:SetData({id = v.id, num = v.num})
        x = x + 60 
        icon:SetTouchSwallow( false )
    end
end