--
-- @brief 宝石信息
-- @author: yjg
-- @date: 2017年9月21日10:15:43
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplGemIproperty = LogicGUI.TemplGemIproperty or BaseClass(GUI.Template)

function LogicGUI.TemplGemIproperty:__init( )
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID}   
    self.__open_callback = function()
        self:__Getwidget( )
        self:__ShowUI( )
    end

    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.GEM)
    self:InitTemplate(GUI.TemplateRegistry.TEMPL_GEM_IPROPERTY)  
end

function LogicGUI.TemplGemIproperty:__Getwidget( btnType )
    -- 内容背景
    self.Image_bg = self:GetWidget("Image_bg") 
    -- 内容
    self.Image_content = self:GetWidget("Image_content")
    --图标
    self.Image_headline_1 = self:GetWidget("Image_headline_1")
    --图标
    self.Image_headline_2 = self:GetWidget("Image_headline_2")
end

function LogicGUI.TemplGemIproperty:__ShowUI( )
    WidgetUnity.SetWidgetListener(self.__root_widget, nil, nil, function ( ... )
        self:DeleteMe()
    end)

    --宝石属性
    local gem_gem_property = Util:Label( CPPGameLib.GetString("gem_now_property") , Macros.TypefaceSize.largish , Macros.Color.Special )
    gem_gem_property:SetAnchorPoint( 0.5 , 0.5 )
    self.Image_headline_1:AddChild( gem_gem_property )
    PosTool.Center( gem_gem_property )

    --技能属性
    local gem_skill_property = Util:Label( CPPGameLib.GetString("gem_yet_activate_Kill") , Macros.TypefaceSize.largish , Macros.Color.Special )
    gem_skill_property:SetAnchorPoint( 0.5 , 0.5 )
    self.Image_headline_2:AddChild( gem_skill_property )
    PosTool.Center( gem_skill_property )

    local gemLst , skillLst = CallModuleFunc(ModuleType.GEM, "GetNowStationNature" )

    local posLst = {
            [1] = { x = 65 , y = self.Image_headline_1:GetPositionY() - 65 },
            [2] = { x = self.Image_content:GetContentWidth()/2 + 65 , y = self.Image_headline_1:GetPositionY() - 65 },
            [3] = { x = 65 , y = self.Image_headline_1:GetPositionY() - 105 },
            [4] = { x = self.Image_content:GetContentWidth()/2 + 65 , y = self.Image_headline_1:GetPositionY() - 105 },
    }

    for j , vv in pairs( gemLst ) do
        local key , worth = Util:Nature( vv.type , vv.num , true )

        local lst = Util:Label( key , Macros.TypefaceSize.normal , Macros.Color.Special )
        lst:SetAnchorPoint( 0 , 0.5 )
        lst:SetPosition( posLst[j].x , posLst[j].y )
        self.Image_content:AddChild( lst )

        local nature = Util:Label( worth , Macros.TypefaceSize.normal , Macros.Color.btn )
        self.Image_content:AddChild( nature )
        PosTool.RightTo( lst , nature , 15 )
    end

    --属性
    self.listView = UIListViewEx.Create()
    self.listView:SetContentSize(self.Image_content:GetContentWidth() , self.Image_content:GetContentHeight() - 5 )
    self.listView:SetPosition( 0 , 2.5 )
    self.listView:SetBounceEnabled(true)
    self.Image_content:AddChild(self.listView)

    for i , v in ipairs(skillLst) do

        local sp9_gem_skil_bg1 = Util:Sprite9( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.GEM, "sp9_gem_skil_bg1" ) )

        local layout = Util:Layout( self.Image_content:GetContentWidth() , sp9_gem_skil_bg1:GetContentHeight()  )
        layout:SetAnchorPoint(0.5, 0.5)

        sp9_gem_skil_bg1:SetContentSize( self.Image_content:GetContentWidth() - 20 , sp9_gem_skil_bg1:GetContentHeight() )
        sp9_gem_skil_bg1:SetAnchorPoint( 0.5 , 0.5 )
        layout:AddChild( sp9_gem_skil_bg1 )
        PosTool.Center( sp9_gem_skil_bg1 )


        local sp9_gem_skil_bg = Util:Sprite9( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.GEM, "sp9_gem_skil_bg" ) )
        sp9_gem_skil_bg:SetAnchorPoint( 0 , 0.5 )
        sp9_gem_skil_bg:SetPosition( 20 , sp9_gem_skil_bg1:GetContentHeight()/2 )
        sp9_gem_skil_bg1:AddChild( sp9_gem_skil_bg )

        --技能
        local skill_icon = Util:Sprite( Resource.PathTool.GetUIResPath("gem/sp_gem_skill_icon_" .. v[1].skill ,true) )
        skill_icon:SetAnchorPoint( 0.5 , 0.5 )
        skill_icon:SetPosition( sp9_gem_skil_bg:GetContentWidth()/2 , sp9_gem_skil_bg:GetContentHeight()/2 )
        sp9_gem_skil_bg:AddChild( skill_icon )

        --技能
        local sp_gem_skillframe = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.GEM, "sp_gem_skillframe" ) )
        sp_gem_skillframe:SetAnchorPoint( 0.5 , 0.5 )
        sp_gem_skillframe:SetPosition( sp9_gem_skil_bg:GetContentWidth()/2 , sp9_gem_skil_bg:GetContentHeight()/2 )
        sp9_gem_skil_bg:AddChild( sp_gem_skillframe )

        --技能
        local skill_name = Util:Sprite( Resource.PathTool.GetUIResPath("gem/sp_gem_skill_name_" .. v[1].skill ,true) )
        skill_name:SetAnchorPoint( 0 , 0.5 )
        skill_name:SetPosition( skill_icon:GetContentWidth() + 25 , sp9_gem_skil_bg1:GetContentHeight()/2 + 30 )
        sp9_gem_skil_bg1:AddChild( skill_name )

        --名字等级
        local skilllevel = Util:Label(  CPPGameLib.GetString("common_lev9" , v[1].level ) , Macros.TypefaceSize.normal , Macros.Color.green )
        sp9_gem_skil_bg1:AddChild( skilllevel )
        PosTool.RightTo( skill_name , skilllevel , 15 )

        local posLst = {
                [1] = { x = skill_name:GetPositionX() + 10 , y = skill_name:GetPositionY() - 35 },
                [2] = { x = layout:GetContentWidth()/2 + 50 , y = skill_name:GetPositionY() - 35 },
                [3] = { x = skill_name:GetPositionX() + 10 , y = skill_name:GetPositionY() - 65 },
                [4] = { x = layout:GetContentWidth()/2 + 50 , y = skill_name:GetPositionY() - 65 },
        }
    
        for j , vv in pairs( v ) do
            local key , worth = Util:Nature( vv.type , vv.num , true )

            local lst = Util:Label( key , Macros.TypefaceSize.normal , Macros.Color.btn )
            lst:SetAnchorPoint( 0 , 0.5 )
            lst:SetPosition( posLst[j].x , posLst[j].y )
            layout:AddChild( lst )

            local nature = Util:Label( worth , Macros.TypefaceSize.normal , Macros.Color.btn )
            layout:AddChild( nature )
            PosTool.RightTo( lst , nature , 15 )
        end
        self.listView:PushBackCustomItem(layout)
    end
    
end
