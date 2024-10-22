--
-- @brief 资源战战报
-- @author: yjg
-- @date: 2018年8月9日11:24:24
--

Modules = Modules or {}
Modules.TemplResourceShark = Modules.TemplResourceShark or BaseClass(GUI.Template)

function Modules.TemplResourceShark:__init( )
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self:InitTemplate(nil) 

	self:__Getwidget()
    self:__Server()
end

function Modules.TemplResourceShark:__Server()
    CallModuleFunc(ModuleType.RESOURCE_WAR,"ResourceWarBattleRecordReq" )
    --刷新敌人
    self:BindGlobalEvent(Macros.Event.ResourceWarModule.RESOURCE_BATTLE_RECORD,function (_, data)
        self:__SetShowUI()
    end)    
end

function Modules.TemplResourceShark:__CreateWidgetFromLuaCode()
    self.widget = Util:Layout( Macros.Global.DesignSize.width, Macros.Global.DesignSize.height )
    return self.widget
end

function Modules.TemplResourceShark:__delete()
end

function Modules.TemplResourceShark:__Getwidget()
    --背景
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "resource_report_bg" )
    local resource_report_bg = Util:Sprite( sp )
    resource_report_bg:SetAnchorPoint( 0.5 , 0.5 )
    self.widget:AddChild( resource_report_bg )
    PosTool.Center( resource_report_bg )

    --标题
    local worldBoss_score = Util:Name( CPPGameLib.GetString("worldBoss_score") , Macros.TypefaceSize.largish )
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
        item_class = Modules.TemplResourceSharkItme,
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
    PosTool.Center( self.lst_list , 0 , -23 )

    self.item = Util:Layout( 560 , 20 )
    self.item:SetAnchorPoint( 0.5 , 0 )
    self.item:SetPosition( resource_report_bg:GetContentWidth()/2 , resource_report_bg:GetContentHeight() - 65  )
    resource_report_bg:AddChild( self.item )
end

function Modules.TemplResourceShark:__SetShowUI()
    local t1 = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetSharkLst" )
    self.lst_list:SetDataList( t1 )
end

function Modules.TemplResourceShark:OnBtnClose()
	self:DeleteMe()
end

--
-- @brief 战报条目
-- @author: yjg
-- @date: 2018年8月9日11:26:55
--

Modules = Modules or {}
Modules.TemplResourceSharkItme = Modules.TemplResourceSharkItme or BaseClass(GUI.ScrollItem)

function Modules.TemplResourceSharkItme:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplResourceSharkItme:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(559,74)
    self.layout:SetBackGroundColor(0, 0, 0)
    return self.layout
end

function Modules.TemplResourceSharkItme:__delete()
    
end

function Modules.TemplResourceSharkItme:SetData( data )
    GUI.ScrollItem.SetData(self, data)
    self:__ShowUI( data )
end

function Modules.TemplResourceSharkItme:__ShowUI( info )
	--背景
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "resource_report_frame_long" )
    local resource_report_frame_long = Util:Sprite( sp )
    resource_report_frame_long:SetAnchorPoint( 0.5 , 0.5 )
    self.layout:AddChild( resource_report_frame_long )
    PosTool.Center( resource_report_frame_long )

    --时间框
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "resource_report_frame_time" )
    local resource_report_frame_time = Util:Sprite( sp )
    resource_report_frame_time:SetAnchorPoint( 0 , 1 )
    resource_report_frame_time:SetPosition( 0 , self.layout:GetContentHeight() ) 
    self.layout:AddChild( resource_report_frame_time )
    --时间
    local time =  TimeTool.GetTimeFormat( info.time  , TimeTool.defaul_format[24] )
    local time_ = Util:Label( time , Macros.TypefaceSize.minimum )  
    time_:SetAnchorPoint( 0.5 , 0.5 )
    resource_report_frame_time:AddChild( time_ )
    PosTool.Center( time_ )

    --提示
    local tab = {
                 { zi =  CPPGameLib.GetString("common_brackets" , info.name_1 ) , ys = Macros.Color.green , dx = Macros.TypefaceSize.minimum },
                 { zi =  CPPGameLib.GetString("resources_war_shark_1") , dx = Macros.TypefaceSize.minimum },
                 { zi =  CPPGameLib.GetString("common_brackets" , info.name_2 ) , ys = Macros.Color.red , dx = Macros.TypefaceSize.minimum },
                 { zi =  CPPGameLib.GetString("resources_war_shark_2" , info.section ) , dx = Macros.TypefaceSize.minimum },
                 { zi =  CPPGameLib.GetString("common_brackets" , info.target ) , ys = Macros.Color.orange , dx = Macros.TypefaceSize.minimum },
                }
    --用作好几个组件拼起来居中显示的强迫症控件 2.0版
    local shark = Util:LabCenter(tab)
    shark:SetAnchorPoint( 0 , 0.5 )
    shark:SetPosition( 10 , resource_report_frame_long:GetContentHeight()/2 - 10 )
    resource_report_frame_long:AddChild( shark )
end
