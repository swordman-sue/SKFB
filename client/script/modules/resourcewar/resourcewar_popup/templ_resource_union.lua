--
-- @brief 资源战公会
-- @author: yjg
-- @date: 2018年8月9日14:51:22
--

Modules = Modules or {}
Modules.TemplResourceUnion = Modules.TemplResourceUnion or BaseClass(GUI.Template)

function Modules.TemplResourceUnion:__init( )
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self.__bg_params = {type = BaseView.BGType.SOLID}
    self:InitTemplate(nil) 

	self:__Getwidget()
    self:__ShowUI()
    self:__Server()
end

function Modules.TemplResourceUnion:__Server()
    CallModuleFunc(ModuleType.RESOURCE_WAR,"ResourceWarGuildResourceReq" )
    --刷新公会
    self:BindGlobalEvent(Macros.Event.ResourceWarModule.RESOURCE_UNION,function (_, data)
        self:__SetShowUI()
    end)    

    --刷新章节时关闭弹窗
    self:BindGlobalEvent(Macros.Event.ResourceWarModule.RESOURCE_CHAPTER,function (_, data)
        self:OnBtnClose()
    end)   

end

function Modules.TemplResourceUnion:__CreateWidgetFromLuaCode()
    self.widget = Util:Layout( Macros.Global.DesignSize.width, Macros.Global.DesignSize.height )
    return self.widget
end

function Modules.TemplResourceUnion:__delete()
end

function Modules.TemplResourceUnion:__Getwidget()
    --背景
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "resource_report_bg" )
    local resource_report_bg = Util:Sprite( sp )
    resource_report_bg:SetAnchorPoint( 0.5 , 0.5 )
    self.widget:AddChild( resource_report_bg )
    PosTool.Center( resource_report_bg )

    --标题
    local worldBoss_score = Util:Name( CPPGameLib.GetString("resources_war_function_5") , Macros.TypefaceSize.largish )
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
        item_class = Modules.TemplResourceUnionItme,
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

function Modules.TemplResourceUnion:__ShowUI()
	-- 章节
	local resources_war_union_popup_1 = Util:Label( CPPGameLib.GetString("resources_war_union_popup_1") , Macros.TypefaceSize.minimum )
	resources_war_union_popup_1:SetAnchorPoint( 0.5 , 0.5 )
	resources_war_union_popup_1:SetPosition( 80 , self.item:GetContentHeight()/2 )
	self.item:AddChild( resources_war_union_popup_1 )

	--军团
	local resources_war_union_popup_2 = Util:Label( CPPGameLib.GetString("resources_war_union_popup_2") , Macros.TypefaceSize.minimum )
	resources_war_union_popup_2:SetAnchorPoint( 0.5 , 0.5 )
	resources_war_union_popup_2:SetPosition( 250 , self.item:GetContentHeight()/2 )
	self.item:AddChild( resources_war_union_popup_2 )

	--奖励加成
	local resources_war_union_popup_3 = Util:Label( CPPGameLib.GetString("resources_war_union_popup_3") , Macros.TypefaceSize.minimum )
	resources_war_union_popup_3:SetAnchorPoint( 0.5 , 0.5 )
	resources_war_union_popup_3:SetPosition( 400 , self.item:GetContentHeight()/2 )
	self.item:AddChild( resources_war_union_popup_3 )
end

function Modules.TemplResourceUnion:__SetShowUI()
    local t1 = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetUnionLst" )
    self.lst_list:SetDataList( t1 )
end

function Modules.TemplResourceUnion:OnBtnClose()
	self:DeleteMe()
end

--
-- @brief 军团条目
-- @author: yjg
-- @date: 2018年8月9日11:26:55
--

Modules = Modules or {}
Modules.TemplResourceUnionItme = Modules.TemplResourceUnionItme or BaseClass(GUI.ScrollItem)

function Modules.TemplResourceUnionItme:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TemplResourceUnionItme:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(559,74)
    self.layout:SetBackGroundColor(0, 0, 0)
    return self.layout
end

function Modules.TemplResourceUnionItme:__delete()
    
end

function Modules.TemplResourceUnionItme:SetData( data )
    GUI.ScrollItem.SetData(self, data)
    self:__ShowUI( data )
end

function Modules.TemplResourceUnionItme:__ShowUI( info )
	--背景
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "resource_report_frame_long" )
    local resource_report_frame_long = Util:Sprite( sp )
    resource_report_frame_long:SetAnchorPoint( 0.5 , 0.5 )
    self.layout:AddChild( resource_report_frame_long )
    PosTool.Center( resource_report_frame_long )

    --章节
    local resources_war_section_1 = Util:Label( CPPGameLib.GetString("resources_war_section_1" , info.section ) , Macros.TypefaceSize.popup )
    resources_war_section_1:SetAnchorPoint( 0.5 , 0.5 )
    resources_war_section_1:SetPosition( 80 , resource_report_frame_long:GetContentHeight()/2 )
    resource_report_frame_long:AddChild( resources_war_section_1 )


    --占领人数
    local num = Util:Label( info.num , Macros.TypefaceSize.popup )
    num:SetAnchorPoint( 0.5 , 0.5 )
    num:SetPosition( 250 , resource_report_frame_long:GetContentHeight()/2 )
    resource_report_frame_long:AddChild( num )


    --奖励加成
    local addition = Util:Label( info.addition , Macros.TypefaceSize.popup )
    addition:SetAnchorPoint( 0.5 , 0.5 )
    addition:SetPosition( 400 , resource_report_frame_long:GetContentHeight()/2 )
    resource_report_frame_long:AddChild( addition )


    --前往
    local manor_go = Util:Name( CPPGameLib.GetString("manor_go") , Macros.TypefaceSize.slightly , 4 )
    manor_go:SetAnchorPoint( 0.5 , 0.5 )
    manor_go:SetPosition( 500 , resource_report_frame_long:GetContentHeight()/2 )
    resource_report_frame_long:AddChild( manor_go )

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
function Modules.TemplResourceUnionItme:OnGoTo( data )
    local info = {}
    info.chapter = data.section
    CallModuleFunc(ModuleType.RESOURCE_WAR,"ResourceWarChapterDataReq" , info )
end
