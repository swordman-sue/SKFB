--
-- @brief 资源战技能
-- @author: yjg
-- @date: 2018年8月9日14:42:08
--

Modules = Modules or {}
Modules.TemplResourceTech = Modules.TemplResourceTech or BaseClass(GUI.Template)

function Modules.TemplResourceTech:__init( )
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self:InitTemplate(nil) 

	self:__Getwidget()
    self:__ShowUI()
    self:__Server()
end

function Modules.TemplResourceTech:__Server()
    -- CallModuleFunc(ModuleType.RESOURCE_WAR,"ResourceWarTechnologyUpgradeReq" )
    -- --刷新敌人
    self:BindGlobalEvent(Macros.Event.ResourceWarModule.RESOURCE_WAR_DATA,function (_, data)
        self:__SetShowUI( true )
    end)  
    self:__SetShowUI() 
end

function Modules.TemplResourceTech:__CreateWidgetFromLuaCode()
    self.widget = Util:Layout( Macros.Global.DesignSize.width, Macros.Global.DesignSize.height )
    return self.widget
end

function Modules.TemplResourceTech:__delete()
end

function Modules.TemplResourceTech:__Getwidget()
    --背景
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "resource_report_bg" )
    local resource_report_bg = Util:Sprite( sp )
    resource_report_bg:SetAnchorPoint( 0.5 , 0.5 )
    self.widget:AddChild( resource_report_bg )
    PosTool.Center( resource_report_bg )

    --标题
    local worldBoss_score = Util:Name( CPPGameLib.GetString("resources_war_function_4") , Macros.TypefaceSize.largish )
    worldBoss_score:SetAnchorPoint( 0.5 , 0.5 )
    worldBoss_score:SetPosition( resource_report_bg:GetContentWidth()/2 , resource_report_bg:GetContentHeight() - 20 )
    resource_report_bg:AddChild( worldBoss_score )

    --关闭按钮
    local btn_btn_delete2 = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_BTN, "btn_btn_delete2")
    local close = Util:Button( btn_btn_delete2 , CPPGameLib.Handler(self,self.OnBtnClose) )
    close:SetAnchorPoint( 1 , 1 )
    close:SetPosition( resource_report_bg:GetContentWidth() - 20 , resource_report_bg:GetContentHeight() - 12 )
    resource_report_bg:AddChild( close )

    --图标列表
    local params =  
    {
        item_class = Modules.TemplResourceTechItme,
        item_width = 279.5 ,
        item_height = 74,
        row = 4,
        col = 2,
        item_space_r = 5 ,
        view_width = 560 ,
        view_height = resource_report_bg:GetContentHeight() - 85 ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    resource_report_bg:AddChild(self.lst_list:GetNode())
    PosTool.Center( self.lst_list , 0 , -23 )

    self.item = Util:Layout( 560 , 20 )
    self.item:SetAnchorPoint( 0.5 , 0 )
    self.item:SetPosition( resource_report_bg:GetContentWidth()/2 , resource_report_bg:GetContentHeight() - 65  )
    resource_report_bg:AddChild( self.item )

end

function Modules.TemplResourceTech:__ShowUI()
	-- 玩家信息
	self.resources_war_accumulateTime_1 = Util:Label( CPPGameLib.GetString("resources_war_accumulateTime_1") , Macros.TypefaceSize.minimum )
	self.resources_war_accumulateTime_1:SetAnchorPoint( 0 , 0.5 )
	self.resources_war_accumulateTime_1:SetPosition( 80 , self.item:GetContentHeight()/2 )
	self.item:AddChild( self.resources_war_accumulateTime_1 )

	--军团
	self.resources_war_bonusToReward = Util:Label( CPPGameLib.GetString("resources_war_bonusToReward") , Macros.TypefaceSize.minimum )
	self.resources_war_bonusToReward:SetAnchorPoint( 0 , 0.5 )
	self.resources_war_bonusToReward:SetPosition( self.item:GetContentWidth()/2 + 20 , self.item:GetContentHeight()/2 )
	self.item:AddChild( self.resources_war_bonusToReward )
end

function Modules.TemplResourceTech:__SetShowUI( up )
    --累计时长
    local addUpDuration = CallModuleFunc(ModuleType.RESOURCE_WAR , "GetAddUpDuration" )
    self.resources_war_accumulateTime_1:SetString( CPPGameLib.GetString("resources_war_accumulateTime_1" , addUpDuration ) )

    --科技技能等级
    local technologyLevel = CallModuleFunc(ModuleType.RESOURCE_WAR , "GetTechnologyLevel" )
    local cnf = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetResourceWarTechnology" , technologyLevel )
    self.resources_war_bonusToReward:SetString( CPPGameLib.GetString("resources_war_bonusToReward" , MathTool.TransProbCoeffPercentage( cnf.base_reward_add_rate ) .. "%" ) )

    if not up then
        local t1 = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetResourceWarTechnology" )
        self.lst_list:SetDataList( t1 )
    else
        self.lst_list:ItemIter( function( index , item , data )
            if item then
                item:__SetShowUI()
            end
        end )
    end
end

function Modules.TemplResourceTech:OnBtnClose()
	self:DeleteMe()
end

--
-- @brief 战报条目
-- @author: yjg
-- @date: 2018年8月9日11:26:55
--

Modules = Modules or {}
Modules.TemplResourceTechItme = Modules.TemplResourceTechItme or BaseClass(GUI.ScrollItem)

function Modules.TemplResourceTechItme:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplResourceTechItme:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(279,74)
    self.layout:SetBackGroundColor(0, 0, 0)
    return self.layout
end

function Modules.TemplResourceTechItme:__delete()
    
end

function Modules.TemplResourceTechItme:SetData( data )
    GUI.ScrollItem.SetData(self, data)
    self:__ShowUI( data )
end

function Modules.TemplResourceTechItme:__ShowUI( info )
	--背景
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "resource_report_frame_short" )
    local resource_report_frame_long = Util:Sprite( sp )
    resource_report_frame_long:SetAnchorPoint( 0.5 , 0.5 )
    self.layout:AddChild( resource_report_frame_long )
    PosTool.Center( resource_report_frame_long )

    --头像
    -- local roleIcon = LogicGUI.RoleIcon.New()
    -- roleIcon:SetData( 39999 )
    -- roleIcon:SetAnchorPoint( 0 , 0.5 )
    -- roleIcon:SetPosition( 25 , resource_report_frame_long:GetContentHeight()/2 )
    -- resource_report_frame_long:AddChild(roleIcon:GetNode())
    -- --缩放调整
    -- roleIcon:SetScaleFactor( 0.7 )

    local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_ICON, "icon_sp_item_bg_"..info.quality - 1)
    self.occupy = {}
    self.occupy.bg = Util:Sprite(imgdata)
    self.occupy.bg:SetAnchorPoint( 0 , 0.5 )
    self.occupy.bg:SetPosition( 25 , resource_report_frame_long:GetContentHeight()/2 )
    resource_report_frame_long:AddChild( self.occupy.bg )
    self.occupy.bg:SetScaleFactor( 0.7 )

    local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.RESOURCE_WAR_SKILL, info.icon)
    self.occupy.content = Util:Sprite(imgdata)
    self.occupy.bg:AddChild( self.occupy.content )
    PosTool.Center( self.occupy.content )
    self.occupy.content:SetScaleFactor( 0.77 )

    local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_ICON, "icon_sp_item_"..info.quality - 1 )
    self.occupy.frame = Util:Sprite(imgdata , function( ... )
    end )
    self.occupy.bg:AddChild( self.occupy.frame )
    PosTool.Center( self.occupy.frame )

    --等级
    local level = Util:Label( CPPGameLib.GetString("common_lev8" , info.level ) , Macros.TypefaceSize.minimum )  
    level:SetAnchorPoint( 0 , 0.5 )
    level:SetPosition( 90 , resource_report_frame_long:GetContentHeight() - 15 )
    resource_report_frame_long:AddChild( level )

    --奖励加成
    local resources_war_union_popup_3 = Util:Label( CPPGameLib.GetString("resources_war_union_popup_3") ..  CPPGameLib.GetString("common_colon") .. MathTool.TransProbCoeffPercentage( info.base_reward_add_rate ) .. "%" , Macros.TypefaceSize.minimum )  
    resources_war_union_popup_3:SetAnchorPoint( 1 , 0.5 )
    resources_war_union_popup_3:SetPosition( resource_report_frame_long:GetContentWidth() - 10 , level:GetPositionY())
    resource_report_frame_long:AddChild( resources_war_union_popup_3 )


    -- 已经开启
    local black1_bg_sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "black1_bg")
    self.black1_bg = Util:Sprite9(black1_bg_sp)
    self.black1_bg:SetAnchorPoint( 0.5 , 0 )
    self.black1_bg:SetPosition( resource_report_frame_long:GetContentWidth()/2 + 20  , 3 )
    self.black1_bg:SetContentSize( 90 , 35 )
    self.black1_bg:SetVisible( false )
    resource_report_frame_long:AddChild( self.black1_bg )

    local activity_activate = Util:Name( CPPGameLib.GetString("activity_activate") , nil , 1 )
    activity_activate:SetAnchorPoint( 0.5 , 0.5 )
    self.black1_bg:AddChild( activity_activate )
    PosTool.Center( activity_activate )

    --可激活
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW , "btn_btn_littlered")
    self.up = Util:Button(sp , function( ... )
        -- body
        self:OnUp()
    end ,CPPGameLib.GetString("resources_war_activate") , Macros.TypefaceSize.normal , nil , Macros.Color.lab_red )
    self.up:SetScale9Enabled(true)
    self.up:SetContentSize( 90 , 45 )
    self.up:SetAnchorPoint( 1 , 0 )  
    self.up:SetPosition( resource_report_frame_long:GetContentWidth() - 5 , 0 )
    self.up:SetVisible( false )
    resource_report_frame_long:AddChild( self.up )

    --描述
    self.resources_war_accumulateTime = Util:Label( CPPGameLib.GetString("resources_war_accumulateTime" , info.need_accupy_time ) , Macros.TypefaceSize.minimum )  
    self.resources_war_accumulateTime:SetAnchorPoint( 0 , 0 )
    self.resources_war_accumulateTime:SetPosition( 90 , 10 )
    self.resources_war_accumulateTime:SetVisible( false )
    resource_report_frame_long:AddChild( self.resources_war_accumulateTime )

    self:__SetShowUI()
end

function Modules.TemplResourceTechItme:__SetShowUI()
    self.black1_bg:SetVisible( false )
    self.up:SetVisible( false )
    self.resources_war_accumulateTime:SetVisible( false )

    local info = self.__data
    --累计时长
    local addUpDuration = CallModuleFunc(ModuleType.RESOURCE_WAR , "GetAddUpDuration" )
    --科技技能等级
    local technologyLevel = CallModuleFunc(ModuleType.RESOURCE_WAR , "GetTechnologyLevel" )

    -- 3232 【资源战】军团科技，已激活的等级应该也显示已激活
    -- 2018-08-28 09:50:37, 由 黄炜健 激活。
    -- 美术图是看按钮状态的，这样应该是点过的都会显示已激活
    --已经开启
    if technologyLevel >= info.level then
        self.black1_bg:SetVisible( true )
    --可激活
    elseif technologyLevel + 1 == info.level and addUpDuration >= info.need_accupy_time then
        self.up:SetVisible( true )
    --描述
    else
        self.resources_war_accumulateTime:SetVisible( true )
    end
end


function Modules.TemplResourceTechItme:OnUp( data )
    CallModuleFunc(ModuleType.RESOURCE_WAR , "ResourceWarTechnologyUpgradeReq" )
end