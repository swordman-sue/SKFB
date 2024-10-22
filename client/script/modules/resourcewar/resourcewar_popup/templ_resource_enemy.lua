--
-- @brief 资源战敌人
-- @author: yjg
-- @date: 2018年8月9日11:21:29
--

Modules = Modules or {}
Modules.TemplResourceEnemy = Modules.TemplResourceEnemy or BaseClass(GUI.Template)

function Modules.TemplResourceEnemy:__init( )
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self:InitTemplate(nil) 

	self:__Getwidget()
    self:__ShowUI()

    self:__Server()
end

function Modules.TemplResourceEnemy:__Server()
    CallModuleFunc(ModuleType.RESOURCE_WAR,"ResourceWarEnemyListReq" )
    --刷新敌人
    self:BindGlobalEvent(Macros.Event.ResourceWarModule.RESOURCE_WAR_ENEMY,function (_, data)
        self:__SetShowUI()
    end)    
end

function Modules.TemplResourceEnemy:__CreateWidgetFromLuaCode()
    self.widget = Util:Layout( Macros.Global.DesignSize.width, Macros.Global.DesignSize.height )
    return self.widget
end

function Modules.TemplResourceEnemy:__delete()
end

function Modules.TemplResourceEnemy:__Getwidget()
    --背景
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "resource_report_bg" )
    local resource_report_bg = Util:Sprite( sp )
    resource_report_bg:SetAnchorPoint( 0.5 , 0.5 )
    self.widget:AddChild( resource_report_bg )
    PosTool.Center( resource_report_bg )

    --标题
    local resources_war_enemy = Util:Name( CPPGameLib.GetString("resources_war_enemy") , Macros.TypefaceSize.largish )
    resources_war_enemy:SetAnchorPoint( 0.5 , 0.5 )
    resources_war_enemy:SetPosition( resource_report_bg:GetContentWidth()/2 , resource_report_bg:GetContentHeight() - 20 )
    resource_report_bg:AddChild( resources_war_enemy )

    --关闭按钮
    local btn_btn_delete2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_delete2")
    local close = Util:Button( btn_btn_delete2 , CPPGameLib.Handler(self,self.OnBtnClose) )
    close:SetAnchorPoint( 1 , 1 )
    close:SetPosition( resource_report_bg:GetContentWidth() - 20 , resource_report_bg:GetContentHeight() - 12 )
    resource_report_bg:AddChild( close )

    --图标列表
    local params =  
    {
        item_class = Modules.TemplResourceEnemyItme,
        item_width = 559 ,
        item_height = 74,
        row = 4,
        col = 1,
        item_space_r = 5 ,
        view_width = 560 ,
        view_height = resource_report_bg:GetContentHeight() - 85 ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    resource_report_bg:AddChild(self.lst_list:GetNode())
    PosTool.Center( self.lst_list , 0 , -25 )


    self.item = Util:Layout( 560 , 20 )
    self.item:SetAnchorPoint( 0.5 , 0 )
    self.item:SetPosition( resource_report_bg:GetContentWidth()/2 , resource_report_bg:GetContentHeight() - 65  )
    resource_report_bg:AddChild( self.item )
end

function Modules.TemplResourceEnemy:__ShowUI()
	--玩家信息
	local ladder_playerInfo = Util:Label( CPPGameLib.GetString("resources_war_player") , Macros.TypefaceSize.minimum )
	ladder_playerInfo:SetAnchorPoint( 0.5 , 0.5 )
	ladder_playerInfo:SetPosition( 110 , self.item:GetContentHeight()/2 )
	self.item:AddChild( ladder_playerInfo )

	--军团
	local resources_war_union = Util:Label( CPPGameLib.GetString("resources_war_union") , Macros.TypefaceSize.minimum )
	resources_war_union:SetAnchorPoint( 0.5 , 0.5 )
	resources_war_union:SetPosition( self.item:GetContentWidth()/2 + 10 , self.item:GetContentHeight()/2 )
	self.item:AddChild( resources_war_union )

	--攻占次数
	local resources_war_occupyTiem = Util:Label( CPPGameLib.GetString("resources_war_occupyTiem") , Macros.TypefaceSize.minimum )
	resources_war_occupyTiem:SetAnchorPoint( 0.5 , 0.5 )
	resources_war_occupyTiem:SetPosition( self.item:GetContentWidth()/2 + 120 , self.item:GetContentHeight()/2 )
	self.item:AddChild( resources_war_occupyTiem )

	--占领信息
	local resources_war_occupyInfo = Util:Label( CPPGameLib.GetString("resources_war_occupyInfo") , Macros.TypefaceSize.minimum )
	resources_war_occupyInfo:SetAnchorPoint( 1 , 0.5 )
	resources_war_occupyInfo:SetPosition( self.item:GetContentWidth() - 45 , self.item:GetContentHeight()/2 )
	self.item:AddChild( resources_war_occupyInfo )
end

function Modules.TemplResourceEnemy:__SetShowUI()
    local t1 = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetEnemyLst" )
    self.lst_list:SetDataList( t1 )
end

function Modules.TemplResourceEnemy:OnBtnClose()
	self:DeleteMe()
end

--
-- @brief 敌军条目
-- @author: yjg
-- @date: 2018年8月9日09:59:43
--

Modules = Modules or {}
Modules.TemplResourceEnemyItme = Modules.TemplResourceEnemyItme or BaseClass(GUI.ScrollItem)

function Modules.TemplResourceEnemyItme:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplResourceEnemyItme:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(559,74)
    self.layout:SetBackGroundColor(0, 0, 0)
    return self.layout
end

function Modules.TemplResourceEnemyItme:__delete()
    
end

function Modules.TemplResourceEnemyItme:SetData( data )
    GUI.ScrollItem.SetData(self, data)
    self:__ShowUI( data )
end

function Modules.TemplResourceEnemyItme:__ShowUI( info )
	--背景
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "resource_report_frame_long" )
    local resource_report_frame_long = Util:Sprite( sp )
    resource_report_frame_long:SetAnchorPoint( 0.5 , 0.5 )
    self.layout:AddChild( resource_report_frame_long )
    PosTool.Center( resource_report_frame_long )

    --头像
    local roleIcon = LogicGUI.RoleIcon.New()
    roleIcon:SetData( info.roleIcon )
    roleIcon:SetAnchorPoint( 0 , 0.5 )
    roleIcon:SetPosition( 25 , resource_report_frame_long:GetContentHeight()/2 )
    resource_report_frame_long:AddChild(roleIcon:GetNode())
    --缩放调整
    roleIcon:SetScaleFactor( 0.7 )

    --名字
    local name = Util:Label( info.name , Macros.TypefaceSize.minimum )
    name:SetAnchorPoint( 0 , 0.5 )
    name:SetPosition( roleIcon:GetNode():GetPositionX() + roleIcon:GetNode():GetContentWidth() + 5 , resource_report_frame_long:GetContentHeight()/2 + 20 )
    resource_report_frame_long:AddChild( name )

    --等级
    local lvText = Util:Label( CPPGameLib.GetString("common_lev1") , Macros.TypefaceSize.minimum )
    lvText:SetAnchorPoint( 0 , 0.5 )
    lvText:SetPosition( name:GetPositionX() , resource_report_frame_long:GetContentHeight()/2 )
    resource_report_frame_long:AddChild( lvText )

    local lvValue = Util:Label( CPPGameLib.GetString("common_lev5" , info.lv ) , Macros.TypefaceSize.minimum )
    resource_report_frame_long:AddChild( lvValue )
    PosTool.RightTo( lvText , lvValue )

    --战力
    local powerText = Util:Label( CPPGameLib.GetString("zhandouli") , Macros.TypefaceSize.minimum )
    powerText:SetAnchorPoint( 0 , 0.5 )
    powerText:SetPosition( lvText:GetPositionX() , resource_report_frame_long:GetContentHeight()/2 - 20 )
    resource_report_frame_long:AddChild( powerText )

    local powerValue = Util:Label( info.power , Macros.TypefaceSize.minimum , Macros.Color.orange )
    resource_report_frame_long:AddChild( powerValue )
    PosTool.RightTo( powerText , powerValue )

    --军团
    local union = Util:Label( CPPGameLib.GetString("common_brackets" , info.union ) , Macros.TypefaceSize.minimum )
    union:SetAnchorPoint( 0.5 , 0.5 )
    union:SetPosition( resource_report_frame_long:GetContentWidth()/2 + 10  , resource_report_frame_long:GetContentHeight()/2 )
    resource_report_frame_long:AddChild( union )

    --攻占次数
    local item = Util:Label( info.time , Macros.TypefaceSize.minimum )
    item:SetAnchorPoint( 0.5 , 0.5 )
    item:SetPosition( resource_report_frame_long:GetContentWidth()/2 + 120  , resource_report_frame_long:GetContentHeight()/2 )
    resource_report_frame_long:AddChild( item )

    if not info.type then
        return
    end

    --图标
    local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_ICON, "icon_sp_item_bg_" .. info.type - 1  )
    local occupy = {}
    occupy.bg = Util:Sprite(imgdata)
    occupy.bg:SetAnchorPoint( 1 , 0.5 )
    occupy.bg:SetPosition( resource_report_frame_long:GetContentWidth() - 50 , resource_report_frame_long:GetContentHeight()/2 )
    resource_report_frame_long:AddChild( occupy.bg )
    occupy.bg:SetScaleFactor( 0.7 )

    local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.RESOURCE_WAR, "resource_seek_" .. info.type )
    occupy.content = Util:Sprite(imgdata)
    occupy.bg:AddChild( occupy.content )
    PosTool.Center( occupy.content )
    occupy.content:SetScaleFactor( 0.77 )

    local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_ICON, "icon_sp_item_" .. info.type - 1 )
    occupy.frame = Util:Sprite(imgdata)
    occupy.bg:AddChild( occupy.frame )
    PosTool.Center( occupy.frame )

    --跳转
    local resource_skip = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_arrow")
    local btn_arrow =  Util:Button( resource_skip , function( ... )
    	self:OnGoTo( info )
    end )
    btn_arrow:SetAnchorPoint( 1 , 0.5 )
    btn_arrow:SetPosition( resource_report_frame_long:GetContentWidth() - 5 , resource_report_frame_long:GetContentHeight()/2 )
    resource_report_frame_long:AddChild( btn_arrow )
end

--跳转
function Modules.TemplResourceEnemyItme:OnGoTo( info )
    CallModuleFunc(ModuleType.RESOURCE_WAR,"SetEnemyData" , info )
    Modules.TemplResourceMine.New( info.id , info.chapter )
end