--
-- @brief 工会战（我的宣战列表）
-- @author: yjg
-- @date: 2018年10月17日11:44:00
--
LogicGUI = LogicGUI or {}

LogicGUI.TempUnionCombatMyDeclarewar = LogicGUI.TempUnionCombatMyDeclarewar or BaseClass(GUI.Template)

function LogicGUI.TempUnionCombatMyDeclarewar:__init( star_id , type_ )
	self.__layout_mode = GUI.Template.LayoutMode.CODE
	self.__bg_params = {type = BaseView.BGType.SOLID}	
    self:InitTemplate(nil)  

    self.type_ = type_

	self:__Getwidget()
	self:__Server( star_id )
end

function LogicGUI.TempUnionCombatMyDeclarewar:__delete( )
end

function LogicGUI.TempUnionCombatMyDeclarewar:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height,function( ... )
    	self:DeleteMe()
    end)
    return self.layout
end

--数据
function LogicGUI.TempUnionCombatMyDeclarewar:__Server( star_id )
	-- local info = {}
	-- info.star_id = star_id
	-- CallModuleFunc(ModuleType.UNION , "GuildWarStarDetailReq" , info )

 --    --星球信息
 --    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_STAR_INFO , function (_, data)
    	self:__SetShowUI()
    -- end)
end

function LogicGUI.TempUnionCombatMyDeclarewar:__Getwidget( )
	local hero_recruit_touchscreen_2 = Util:Label( CPPGameLib.GetString("hero_recruit_touchscreen_2") )
	hero_recruit_touchscreen_2:SetAnchorPoint( 0.5 , 0.5 )
	hero_recruit_touchscreen_2:SetPosition( self.layout:GetContentWidth()/2 , 70 )
	self.layout:AddChild( hero_recruit_touchscreen_2 )

	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "sp_union_secondary") 
	local sp_frame_bg = Util:Sprite9( sp , nil ,function ( ... )
		-- body
	end)
	sp_frame_bg:SetContentSize( sp_frame_bg:GetContentWidth() , sp_frame_bg:GetContentHeight() * 2  )
	sp_frame_bg:SetAnchorPoint( 0.5 , 0.5 )
	sp_frame_bg:SetPosition( self.layout:GetContentWidth()/2 , self.layout:GetContentHeight()/2 )
	self.layout:AddChild( sp_frame_bg )

	--已宣战军团
	local union_combat_declareWarLst = Util:Label( CPPGameLib.GetString("union_combat_declareWarLst") , Macros.TypefaceSize.largish )
	union_combat_declareWarLst:SetAnchorPoint( 0.5 , 0.5 )
	union_combat_declareWarLst:SetPosition( sp_frame_bg:GetContentWidth()/2 , sp_frame_bg:GetContentHeight() - 20 )
	sp_frame_bg:AddChild( union_combat_declareWarLst )

	local content = Util:Layout( sp_frame_bg:GetContentWidth() - 60 , sp_frame_bg:GetContentHeight() - 85 )
	content:SetAnchorPoint( 0.5 , 0 )
	content:SetPosition( sp_frame_bg:GetContentWidth()/2 , 40 )
	sp_frame_bg:AddChild( content )
    content:SetBackGroundColor(0, 0, 0)
    content:SetBackGroundColorType(1)
    content:SetBackGroundColorOpacity( 200 )

    
    local lst = CallModuleFunc(ModuleType.UNION , "GetUnionCombatMyDeclareWarLst" )
    local max = CallModuleFunc(ModuleType.UNION , "GetUnionCombatMyDeclareWarMax" )

    local index = 0
    for k , v in pairs( lst ) do
    	index = index + 1
    end

	local union_combat_declarewar = Util:Label(CPPGameLib.GetString("union_combat_declarewar" , index , max ), Macros.TypefaceSize.normal, Macros.Color.lilyWhite)
	union_combat_declarewar:SetAnchorPoint(0.5 , 0.5 )
	union_combat_declarewar:SetPosition( sp_frame_bg:GetContentWidth()/2 , 23 )
	sp_frame_bg:AddChild( union_combat_declarewar )


  --图标列表
    local params =  
    {
        item_class = Modules.TempUnionCombatMyDeclarewarItem,
        item_width = 369,
        item_height = 73,
        row = 5 ,
        col = 1 ,
        item_space_r = 2 ,
        view_width = 369,
        view_height = content:GetContentHeight() - 10 ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    content:AddChild(self.lst_list:GetNode())
    self.lst_list:SetAnchorPoint( 0.5 , 0.5 )
    self.lst_list:SetPosition( content:GetContentWidth()/2 , content:GetContentHeight()/2 )
end

function LogicGUI.TempUnionCombatMyDeclarewar:__SetShowUI( )
	local lst = CallModuleFunc(ModuleType.UNION , "GetUnionCombatMyDeclareWarLst" )
	local t1 = {}
	for k , v in pairs( lst ) do
		table.insert( t1 , v )
	end
    self.lst_list:SetDataList( t1 )
end


Modules = Modules or {}
Modules.TempUnionCombatMyDeclarewarItem = Modules.TempUnionCombatMyDeclarewarItem or BaseClass(GUI.ScrollItem)

function Modules.TempUnionCombatMyDeclarewarItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TempUnionCombatMyDeclarewarItem:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(369,70)
    return self.layout
end

function Modules.TempUnionCombatMyDeclarewarItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)  
    self:__ShowUI( data )
end

function Modules.TempUnionCombatMyDeclarewarItem:__ShowUI( data )
    --底
   	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "resource_report_frame_long")
	local sp_label_up = Util:Sprite9( sp )
	sp_label_up:SetAnchorPoint( 0.5 , 0 )
	sp_label_up:SetPosition( self.layout:GetContentWidth()/2 , 0 )
	sp_label_up:SetContentSize( self.layout:GetContentWidth() - 10 , self.layout:GetContentHeight() )
	self.layout:AddChild( sp_label_up )
    
	CPPGameLib.PrintTable("xy" , data , "data----") 
    local starCnf = CallModuleFunc(ModuleType.UNION , "GetGuildWarStarCnf" , data.star_id )
    CPPGameLib.PrintTable("xy" , starCnf , "TempUnionCombatMyDeclarewarItem---")  

    --icon
   	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_STAR , starCnf.sp[1] )
	local icon = Util:Sprite( sp )
	icon:SetAnchorPoint( 0.5 , 0.5 )
	icon:SetPosition( 50 , sp_label_up:GetContentHeight()/2  )
	sp_label_up:AddChild( icon )
	icon:SetScaleFactor( 0.15 )

	--名字
	local name = Util:Name( starCnf.name , Macros.TypefaceSize.normal , starCnf.star_type )
	name:SetAnchorPoint( 0 , 0.5 )
	name:SetPosition( icon:GetPositionX() + 50 , sp_label_up:GetContentHeight()/2 )
	sp_label_up:AddChild( name )

	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "btn_gather_big")
    local exploit = Util:Button( sp , function( ... )
    	LogicGUI.TempUnionCombatStarInfo.New( data.star_id )
    end , CPPGameLib.GetString("dungeon_go_forward") , Macros.TypefaceSize.normal , Macros.Color.lilyWhite )
    exploit:SetAnchorPoint( 1 , 0.5 )
    exploit:SetPosition( sp_label_up:GetContentWidth() - 20 , sp_label_up:GetContentHeight()/2 )
    sp_label_up:AddChild( exploit )

end
