--
-- @brief 工会战（宣战列表）
-- @author: yjg
-- @date: 2018年9月14日15:24:33
--
LogicGUI = LogicGUI or {}

LogicGUI.TempUnionCombatDeclarewar = LogicGUI.TempUnionCombatDeclarewar or BaseClass(GUI.Template)

function LogicGUI.TempUnionCombatDeclarewar:__init( star_id , type_ )
	self.__layout_mode = GUI.Template.LayoutMode.CODE
	self.__bg_params = {type = BaseView.BGType.SOLID}	
    self:InitTemplate(nil)  

    self.type_ = type_

	self:__Getwidget()
	self:__Server( star_id )
end

function LogicGUI.TempUnionCombatDeclarewar:__delete( )
end

function LogicGUI.TempUnionCombatDeclarewar:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height,function( ... )
    	self:DeleteMe()
    end)
    return self.layout
end

--数据
function LogicGUI.TempUnionCombatDeclarewar:__Server( star_id )
	local info = {}
	info.star_id = star_id
	CallModuleFunc(ModuleType.UNION , "GuildWarStarDetailReq" , info )

    --星球信息
    self:BindGlobalEvent(Macros.Event.UnionModule.UNION_STAR_INFO , function (_, data)
    	self:__SetShowUI()
    end)
end

function LogicGUI.TempUnionCombatDeclarewar:__Getwidget( )
	local hero_recruit_touchscreen_2 = Util:Label( CPPGameLib.GetString("hero_recruit_touchscreen_2") )
	hero_recruit_touchscreen_2:SetAnchorPoint( 0.5 , 0.5 )
	hero_recruit_touchscreen_2:SetPosition( self.layout:GetContentWidth()/2 , 70 )
	self.layout:AddChild( hero_recruit_touchscreen_2 )

	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "sp_frame2_bg") 
	local sp_frame_bg = Util:Sprite( sp , function ( ... )
		-- body
	end)
	sp_frame_bg:SetAnchorPoint( 0.5 , 0.5 )
	sp_frame_bg:SetPosition( self.layout:GetContentWidth()/2 , self.layout:GetContentHeight()/2 )
	self.layout:AddChild( sp_frame_bg )

	local text = ""
	if self.type_ == 0 then
		text = CPPGameLib.GetString("union_combat_declareWarLst")
	elseif self.type_ == 1 then
		text = CPPGameLib.GetString("union_combat_integral")
	end

	--已宣战军团
	local union_combat_declareWarLst = Util:Label( text , Macros.TypefaceSize.largish )
	union_combat_declareWarLst:SetAnchorPoint( 0.5 , 0.5 )
	union_combat_declareWarLst:SetPosition( sp_frame_bg:GetContentWidth()/2 , sp_frame_bg:GetContentHeight() - 20 )
	sp_frame_bg:AddChild( union_combat_declareWarLst )

  --图标列表
    local params =  
    {
        item_class = Modules.TempUnionCombatDeclarewarItem,
        item_width = 559,
        item_height = 74,
        row = 5 ,
        col = 1 ,
        item_space_r = 2 ,
        view_width = 559,
        view_height = sp_frame_bg:GetContentHeight() - 80 ,
    }
    -- 创建参数
    self.lst_list = GUI.ScrollView.New(params)
    sp_frame_bg:AddChild(self.lst_list:GetNode())
    self.lst_list:SetAnchorPoint( 0.5 , 0.5 )
    self.lst_list:SetPosition( sp_frame_bg:GetContentWidth()/2 , sp_frame_bg:GetContentHeight()/2 - 15 )
end

function LogicGUI.TempUnionCombatDeclarewar:__SetShowUI( )
	local lst = CallModuleFunc(ModuleType.UNION , "GetUnionCombatALLDeclareWarLst" )
	CPPGameLib.PrintTable("xy" , lst , "lst---------")

	for i , v in ipairs( lst ) do
		v.type = self.type_
	end
	table.sort( lst , function( a , b )
		return a.active > b.active
	end )

    self.lst_list:SetDataList( lst )
end


Modules = Modules or {}
Modules.TempUnionCombatDeclarewarItem = Modules.TempUnionCombatDeclarewarItem or BaseClass(GUI.ScrollItem)

function Modules.TempUnionCombatDeclarewarItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TempUnionCombatDeclarewarItem:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(180,70)
    return self.layout
end

function Modules.TempUnionCombatDeclarewarItem:SetData(data)
    GUI.ScrollItem.SetData(self, data)  
    self:__ShowUI( data )
end

function Modules.TempUnionCombatDeclarewarItem:__ShowUI( data )
    --底
   	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "resource_report_frame_long")
	local sp_label_up = Util:Sprite( sp )
	sp_label_up:SetAnchorPoint( 0.5 , 0 )
	sp_label_up:SetPosition( self.layout:GetContentWidth()/2 , 0 )
	self.layout:AddChild( sp_label_up )
    
    -- CPPGameLib.PrintTable("xy" , data , "TempUnionCombatDeclarewarItem---")  

    --icon
   	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION, "union_icon" .. data.union_icon )
	local icon = Util:Sprite( sp )
	icon:SetAnchorPoint( 0.5 , 0.5 )
	icon:SetPosition( 80 , sp_label_up:GetContentHeight()/2  )
	sp_label_up:AddChild( icon )
	icon:SetScaleFactor( 0.5 )

	--名字
	local name = Util:Name( data.union_name .. " (S.".. data.server ..") " , Macros.TypefaceSize.normal , 4 )
	name:SetAnchorPoint( 0 , 0.5 )
	name:SetPosition( sp_label_up:GetPositionX() + 80 , sp_label_up:GetContentHeight()/2 )
	sp_label_up:AddChild( name )


	local text = CPPGameLib.GetString("union_combat_integral_" , data.active )

	if data.type == 0 then
		text = CPPGameLib.GetString("union_combat_active" , data.active )
	end

	--活跃
	local active = Util:Label( text , Macros.TypefaceSize.slightly  )
	active:SetAnchorPoint( 0 , 0.5 )
	active:SetPosition( sp_label_up:GetContentWidth()/2 + 110 , sp_label_up:GetContentHeight()/2 - 5 )
	sp_label_up:AddChild( active )
end
