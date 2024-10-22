--[[
%% @module: 资源战
%% @author: yjg
%% @created: 2018年8月8日18:31:44
--]]

Modules = Modules or {} 
Modules.ResourcesWarView = Modules.ResourcesWarView or BaseClass(BaseView)

function Modules.ResourcesWarView:__init()
    self.__layout_name = ""  
    self.__layout_mode = BaseView.LayoutMode.CODE 
    self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bp_sp_resources_bg"), image_size = COCOSize(1136,640) }
    self.__title_bar_params = 
    {
        title = CPPGameLib.GetString("resources_war"),
        -- show_power = true,
        -- show_diamond = true,
        -- show_gold = true,
        -- show_level = true,
        close_func = function()
            self:CloseManual()
        end,
    }
    self.__open_callback = function()
        self:__OpenCallback()
    end
    self:__AddPreloadTextureAtlas( Resource.UITextureAtlasType.RESOURCE_WAR )
    self:__AddPreloadTextureAtlas( Resource.UITextureAtlasType.RESOURCE_WAR , true )
    self:__AddPreloadTextureAtlas( Resource.UITextureAtlasType.RESOURCE_WAR_SKILL )
end

function Modules.ResourcesWarView:__OpenCallback( )
    self:__Getwidget()
    self:__ShowUI()
    self:__Server()
end	

function Modules.ResourcesWarView:__Dispose()
    if self.ResourceUI then
        self.ResourceUI:DeleteMe()
        self.ResourceUI = nil
    end
end

function Modules.ResourcesWarView:__CreateWidgetFromLuaCode()
    self.widget = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height)
    return self.widget
end

function Modules.ResourcesWarView:__Server()
    --刷新章节
    self:BindGlobalEvent(Macros.Event.ResourceWarModule.RESOURCE_CHAPTER,function (_, data)
        self:__SetShowUI()
    end)  
    self:__SetShowUI()  
end

function Modules.ResourcesWarView:__Getwidget()

    self.ResourceUI = Modules.TemplResourceUI.New( unpack(self.__open_data) )
end

function Modules.ResourcesWarView:__ShowUI()
end

function Modules.ResourcesWarView:__SetShowUI()

    --当前章节
    local atPresentChapter = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetAtPresentChapter" )

    --章节信息
    local ResourceWarChapterCnf = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetResourceWarChapterCnf", atPresentChapter )
    CPPGameLib.PrintTable("xy" , ResourceWarChapterCnf , "ResourceWarChapterCnf")  

    --创建资源点
    if not self.resource_lst then
        self.resource_lst = {}
    end
    for i , v in ipairs( self.resource_lst ) do
        self.resource_lst[i]:DeleteMe()
        self.resource_lst[i] = nil
    end

    if ResourceWarChapterCnf.resource_id_list then
        for i , v in ipairs( ResourceWarChapterCnf.resource_id_list ) do
            self.resource_lst[i] = Modules.TemplResourceElement.New( v )
            self.__root_widget:AddChild( self.resource_lst[i]:GetNode() )
            PosTool.Center( self.resource_lst[i] )

            --层级
            local ResourceWarModellingCnf = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetResourceWarModellingCnf", v )
            if ResourceWarModellingCnf.icon_zod then
                self.resource_lst[i]:SetZOrder( ResourceWarModellingCnf.icon_zod )
            end
        end
    end
end