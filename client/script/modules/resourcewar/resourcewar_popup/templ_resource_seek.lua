--
-- @brief 资源战搜索
-- @author: yjg
-- @date: 2018年8月9日15:17:23
--

Modules = Modules or {}
Modules.TemplResourceSeek = Modules.TemplResourceSeek or BaseClass(GUI.Template)

function Modules.TemplResourceSeek:__init( btn )
    print("xy" , "=========btn" , btn )
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    -- self.__bg_params = {type = BaseView.BGType.SOLID}
    self:InitTemplate(nil) 
    self.btn = btn
	self:__Getwidget()
    self:__ShowUI()
end

function Modules.TemplResourceSeek:__CreateWidgetFromLuaCode()
    self.widget = Util:Layout( Macros.Global.DesignSize.width, Macros.Global.DesignSize.height , function ( ... )
        self:OnBtnClose()
    end)
    self.widget:SetBackGroundColor(0, 0, 0)
    self.widget:SetBackGroundColorType(1)
    self.widget:SetBackGroundColorOpacity(120)
    return self.widget
end

function Modules.TemplResourceSeek:__delete()
end

function Modules.TemplResourceSeek:__Getwidget()
 
end

function Modules.TemplResourceSeek:__ShowUI()

    --背景
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.RESOURCE_WAR, "resource_seek_bg")
    local resource_seek_bg = Util:Sprite( sp )
    resource_seek_bg:SetAnchorPoint( 0 , 0 )
    resource_seek_bg:SetPosition( self.btn:GetPositionX() - (self.btn:GetContentWidth()/2) - 21  , self.btn:GetPositionY() + (self.btn:GetContentHeight()/2) + 12 )
    self.widget:AddChild( resource_seek_bg )

    local layou = Util:Layout( resource_seek_bg:GetContentWidth() - 60 , resource_seek_bg:GetContentHeight() - 20 , function ( ... )
        -- body
    end)
    layou:SetAnchorPoint( 0 , 1 )
    layou:SetPosition( 30 , resource_seek_bg:GetContentHeight() - 2 )
    resource_seek_bg:AddChild( layou )

    local interval = layou:GetContentWidth() / 6
    for i = 1 , 6 do
        local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_ICON, "icon_sp_item_bg_" ..i-1 )
        local occupy = {}
        occupy.bg = Util:Sprite(imgdata)
        occupy.bg:SetAnchorPoint( 0.5 , 0.5 )
        occupy.bg:SetPosition( interval/2 + ( (i-1) * interval ) , layou:GetContentHeight()/2 )
        layou:AddChild( occupy.bg )
        occupy.bg:SetScaleFactor( 0.6 )

        local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.RESOURCE_WAR, "resource_seek_"..i)
        occupy.content = Util:Sprite(imgdata)
        occupy.bg:AddChild( occupy.content )
        PosTool.Center( occupy.content )
        occupy.content:SetScaleFactor( 0.57 )

        local imgdata = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_ICON, "icon_sp_item_"..i-1)
        occupy.frame = Util:Sprite(imgdata , function( ... )
            self:OnGoTo( i )
        end )
        occupy.bg:AddChild( occupy.frame ) 
        PosTool.Center( occupy.frame ) 
    end
end

function Modules.TemplResourceSeek:OnBtnClose()
	self:DeleteMe()
end

function Modules.TemplResourceSeek:OnGoTo( id )
    local info = {}
    info.resource_type = id
    CallModuleFunc(ModuleType.RESOURCE_WAR,"ResourceWarSameResourceReq" , info )

end