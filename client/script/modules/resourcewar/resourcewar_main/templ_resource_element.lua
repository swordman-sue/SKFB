--
-- @brief 资源战自定义矿场
-- @author: yjg
-- @date: 2018年8月9日11:21:29
--


Modules = Modules or {}
Modules.TemplResourceElement = Modules.TemplResourceElement or BaseClass(GUI.Template)

function Modules.TemplResourceElement:__init( id )
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil) 
    self.resourceId = id
    self:__ShowUI( id )
end

function Modules.TemplResourceElement:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout( 50 , 50 ,function ( ... )
        -- self:DeleteMe()
    end )
    return self.layout
end

function Modules.TemplResourceElement:__AddToParent()
end

function Modules.TemplResourceElement:__delete()
    -- upstar
end

function Modules.TemplResourceElement:__ShowUI( )
	
	local ResourceWarModellingCnf = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetResourceWarModellingCnf", self.resourceId )

    --缩放
    local scale = 1
    if ResourceWarModellingCnf.icon_sc then
        scale = ResourceWarModellingCnf.icon_sc
    end

	--资源
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.RESOURCE_WAR, "resource_mine_".. ResourceWarModellingCnf.icon )
	local resource_mine = Util:Sprite( sp , function( ... )
        self:OnGoTo(  )
    end )
	resource_mine:SetAnchorPoint( 0.5 , 0.5 )
	self.layout:AddChild( resource_mine )
		
	--资源坐标偏移
	if ResourceWarModellingCnf.icon_pos then
		PosTool.Center( resource_mine , tonumber(ResourceWarModellingCnf.icon_pos[1]) , tonumber(ResourceWarModellingCnf.icon_pos[2]) )
	else
		PosTool.Center( resource_mine )
	end

	--缩放
	resource_mine:SetScaleFactor( scale )

    --特效
    if ResourceWarModellingCnf.effects_lst then
        --特效
        for i , v in ipairs(ResourceWarModellingCnf.effects_lst) do
            local id = tonumber(v)
            local tab = {}
            if id then
                tab = { id = id }
            else
                tab = { res_name = v }
            end

            local effectid,effect = self:__CreateEffect( tab ,resource_mine , cnf.effects_zob_lst[i] )
            effect:SetAnchorPoint(0.5,0.5)
            local x , y
            if ResourceWarModellingCnf.effects_pos_lst then
                x = tonumber(ResourceWarModellingCnf.effects_pos_lst[i][1])
                y = tonumber(ResourceWarModellingCnf.effects_pos_lst[i][2])
            end

            PosTool.Center( effect , x , y )
        end
    end

    --名字
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.RESOURCE_WAR, "resource_mine_name" )
    local resource_mine_name = Util:Sprite( sp )
    resource_mine_name:SetAnchorPoint( 0.5 , 0.5 )
    resource_mine:AddChild( resource_mine_name )
    resource_mine_name:SetPosition( resource_mine:GetContentWidth()/2 , 50 )

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.RESOURCE_WAR, "resource_mine_name_" .. ResourceWarModellingCnf.icon , true )
    local resource_name = Util:Sprite( sp )
    resource_name:SetAnchorPoint( 0.5 , 0.5 )
    resource_mine_name:AddChild( resource_name )
    PosTool.Center( resource_name )

	--资料卡
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.RESOURCE_WAR, "resource_mine_info" )
    local resource_mine_info = Util:Sprite9( sp )

    --缩放调整
    resource_mine_info:SetContentSize( resource_mine_info:GetContentWidth() * scale + 20 , resource_mine_info:GetContentHeight() * scale )

    resource_mine_info:SetAnchorPoint( 0 , 0.5 )
    resource_mine_info:SetPosition( resource_mine:GetContentWidth() - 110 , resource_mine:GetContentHeight()/2 - 40 )
    resource_mine:AddChild( resource_mine_info , -1)
    --缩放调整
	resource_mine_info:SetScaleFactor( 1 - ( scale - 1 ) )

    --玩家名
    self.name = Util:Label( "玩家名玩家名" .. " S00 " , Macros.TypefaceSize.minimum )
    self.name:SetPosition( resource_mine_info:GetContentWidth()/2 - 40 , resource_mine_info:GetContentHeight() - 35 )
	self.name:SetAnchorPoint( 0 , 0.5 )
	resource_mine_info:AddChild( self.name )
    --缩放调整
	self.name:SetScaleFactor( scale )

    --战力
    self.zhandouli_text = Util:Label( " "  , Macros.TypefaceSize.minimum )
    self.zhandouli_text:SetPosition( self.name:GetPositionX() , resource_mine_info:GetContentHeight()/2 )
	self.zhandouli_text:SetAnchorPoint( 0 , 0.5 )
	resource_mine_info:AddChild( self.zhandouli_text )
    --缩放调整
	self.zhandouli_text:SetScaleFactor( scale )

    self.zhandouli = Util:Label( " " , Macros.TypefaceSize.minimum , Macros.Color.red )
    resource_mine_info:AddChild( self.zhandouli )
    PosTool.RightTo( self.zhandouli_text , self.zhandouli )
    --缩放调整
    self.zhandouli:SetScaleFactor( scale )

    --公会名
    self.union_name = Util:Label( " " , Macros.TypefaceSize.slightly )
    self.union_name:SetPosition( self.name:GetPositionX() , 15 )
	self.union_name:SetAnchorPoint( 0 , 0 )
	resource_mine_info:AddChild( self.union_name )
    --缩放调整
	self.union_name:SetScaleFactor( scale )

    --公会加成提示
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_upgrade_tips")   
    self.icon_upgrade_tips = Util:Sprite( sp )
    self.icon_upgrade_tips:SetAnchorPoint( 0 , 0 )
    self.icon_upgrade_tips:SetPosition( self.union_name:GetPositionX() + (self.union_name:GetContentWidth() * scale ) , self.union_name:GetPositionY() )
    resource_mine_info:AddChild( self.icon_upgrade_tips )
    --缩放调整
    self.icon_upgrade_tips:SetScaleFactor( 0.8 * scale )

    --头像
    self.RoleIcon = LogicGUI.RoleIcon.New()
    self.RoleIcon:SetData(39999)
    self.RoleIcon:SetAnchorPoint( 1 , 1 )
    self.RoleIcon:SetPosition( resource_mine_info:GetContentWidth() - 20 , resource_mine_info:GetContentHeight() - 20 )
    resource_mine_info:AddChild(self.RoleIcon:GetNode())
    --缩放调整
    self.RoleIcon:SetScaleFactor( 0.55 * scale )

    --等级
    self.lv = Util:Label( "lv 000" , Macros.TypefaceSize.minimum  )
    self.lv:SetAnchorPoint( 0.5 , 1 )
    self.lv:SetPosition( 
                        self.RoleIcon:GetPositionX() - ( ( self.RoleIcon:GetNode():GetContentWidth() * (0.55 * scale) )  /2 ) , 
                        self.RoleIcon:GetPositionY() - ( ( self.RoleIcon:GetNode():GetContentWidth() * (0.55 * scale) ) ) - 5  
                        )
    resource_mine_info:AddChild( self.lv )

    --未占领
    self.resources_war_Notoccupy = Util:Label( CPPGameLib.GetString("resources_war_Notoccupy") , Macros.TypefaceSize.headlineII , Macros.Color.red )
    self.resources_war_Notoccupy:SetAnchorPoint( 0 , 0.5 )
    self.resources_war_Notoccupy:SetPosition( resource_mine_info:GetContentWidth()/2 - 40 , resource_mine_info:GetContentHeight() / 2 )
    resource_mine_info:AddChild( self.resources_war_Notoccupy )

    self:__SetShowUI()
end

function Modules.TemplResourceElement:__SetShowUI()

    local ResourceWarModellingCnf = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetResourceWarModellingCnf", self.resourceId )
    --缩放
    local scale = 1
    if ResourceWarModellingCnf.icon_sc then
        scale = ResourceWarModellingCnf.icon_sc
    end
    local chapterData = CallModuleFunc(ModuleType.RESOURCE_WAR,"GetChapterData" , self.resourceId )

    if not chapterData then
        chapterData = {}
    end

    if not chapterData.role_name then
        self.resources_war_Notoccupy:SetVisible(true)
    else
        self.resources_war_Notoccupy:SetVisible(false)
    end 

    --玩家名
    if chapterData.role_name  then
        self.name:SetString( chapterData.role_name .. " " .. "S" .. chapterData.server_id )
    else
        self.name:SetString( "" )
    end

    --战力
    if chapterData.power then
        self.zhandouli_text:SetString( CPPGameLib.GetString("zhandouli") )
        self.zhandouli:SetString( chapterData.power )
        PosTool.RightTo( self.zhandouli_text , self.zhandouli )
    end

    --公会名
    self.union_name:SetString( chapterData.guild_name or " " )
    --公会加成提示
    self.icon_upgrade_tips:SetVisible(false)
    self.icon_upgrade_tips:SetPosition( self.union_name:GetPositionX() + (self.union_name:GetContentWidth() * scale ) , self.union_name:GetPositionY() )
    
    if chapterData.guild_name and chapterData.guild_name ~= "" then
        self.icon_upgrade_tips:SetVisible(true)
    else
        if chapterData.role_name then
            self.union_name:SetString( CPPGameLib.GetString("rank_haveNoUnion") )
        end
    end

    --头像
    self.RoleIcon:SetData( chapterData.head_img or 39999 )
    self.RoleIcon:SetVisible( false )
    if chapterData.head_img then
        self.RoleIcon:SetVisible( true )
    end

    --等级
    if chapterData.level then
        self.lv:SetString( CPPGameLib.GetString("common_lev9" , chapterData.level or " " ) )
    else
        self.lv:SetString( "" )
    end
end

function Modules.TemplResourceElement:OnGoTo()
    Modules.TemplResourceMine.New( self.resourceId )
end
