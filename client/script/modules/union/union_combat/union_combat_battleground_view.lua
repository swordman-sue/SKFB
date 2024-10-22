--
-- @brief 公会战场界面
-- @author: yjg
-- @date: 2018年10月17日15:59:36
--

Modules = Modules or {}

Modules.UnionCombatBattlegroundView = Modules.UnionCombatBattlegroundView or BaseClass(BaseView)

function Modules.UnionCombatBattlegroundView:__init()
    self.__layout_name = ""  
    self.__layout_mode = BaseView.LayoutMode.CODE 
	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_union_combat_2")}
	self.__title_bar_params = 
	{
		title = CPPGameLib.GetString("union_combat_battleground"),
		close_func = function()
			self:CloseManual()
		end,
	}

    self.__open_callback = function()
    	self:__OpenCallback()
    end

	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.UNION_COMBAT)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.UNION_COMBAT_BATTLEGROUND)
end

function Modules.UnionCombatBattlegroundView:__CreateWidgetFromLuaCode()
    self.widget = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height)
    return self.widget
end


function Modules.UnionCombatBattlegroundView:__OpenCallback()  
	self:__Getwidget()
	self:__Server() 
end

function Modules.UnionCombatBattlegroundView:__Dispose()
end

function Modules.UnionCombatBattlegroundView:__Server()
	CallModuleFunc(ModuleType.UNION , "GuildWarTowerListReq" )

	self:BindGlobalEvent(Macros.Event.UnionModule.UNION_TOWER,function (_, data)
        self:__SetShowUI()
		self:Result()
    end)	
    
	self:BindGlobalEvent(Macros.Event.UnionModule.UNION_STAR_INFO,function (_, data)
        self:__SetShowUI()
        self:Result()
    end)	

	self:BindGlobalEvent(Macros.Event.UnionModule.UNION_DEFENSE,function (_, data)
        self:__SetShowUI()
    end)
end

function Modules.UnionCombatBattlegroundView:__Getwidget()
	local towLst = CallModuleFunc(ModuleType.UNION , "ToIdGetUnionCombatTowerInfo")
	self.towLst = {}
	for i , v in ipairs( towLst ) do
		self.towLst[v.tower_type] = {}

		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT_BATTLEGROUND, v.icon )
		self.towLst[v.tower_type].house = Util:Sprite( sp )
		self.towLst[v.tower_type].house:SetAnchorPoint( 0.5 , 0.5 )
		self.widget:AddChild( self.towLst[v.tower_type].house )
		PosTool.Center( self.towLst[v.tower_type].house , v.pos[1] , v.pos[2] )
		self.towLst[v.tower_type].house:SetScaleFactor( v.icon_scale )

		self.towLst[v.tower_type].touch = Util:Layout( self.towLst[v.tower_type].house:GetContentWidth() , self.towLst[v.tower_type].house:GetContentHeight() ,function( ... )
			local myProtagonist = CallModuleFunc(ModuleType.UNION , "GetBattlegroundProtagonist")
			if myProtagonist then
				LogicGUI.TempUnionCombatDefense.New( v )
			else
				GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_combat_notMe") )
			end
		end )
		self.towLst[v.tower_type].touch:SetAnchorPoint( 0.5 , 0.5 )
		self.towLst[v.tower_type].house:AddChild( self.towLst[v.tower_type].touch )
		PosTool.Center( self.towLst[v.tower_type].touch )

        local preview_bar = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_big_house")
        local preview_bg = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_PROGRESS_NEW, "prog_big_house_bg")

	    self.towLst[v.tower_type].preview = GUI.ProgressBarLayer.New(preview_bar, preview_bg, v.druability , v.druability )
	    self.towLst[v.tower_type].preview:SetAnchorPoint(0.5, 0)
	    self.towLst[v.tower_type].preview:SetPosition( self.towLst[v.tower_type].house:GetContentWidth()/2 + v.hp_pos[1] , self.towLst[v.tower_type].house:GetContentHeight() + v.hp_pos[2] )
	    self.towLst[v.tower_type].house:AddChild( self.towLst[v.tower_type].preview:GetNode() )
	    self.towLst[v.tower_type].preview:SetScaleFactor( v.hp_scale )

	    local size = Util:Sprite( preview_bg )

	    --耐久度
	    local union_combat_last = Util:Label( CPPGameLib.GetString("union_combat_last") )
	    union_combat_last:SetAnchorPoint( 0 , 0.5 )
	    union_combat_last:SetPosition( 20 - (size:GetContentWidth()/2) , 23 )
	    self.towLst[v.tower_type].preview:AddChild( union_combat_last )

	    --耐久度
	    self.towLst[v.tower_type].hp = Util:Label( CPPGameLib.GetString("monster_stormcastle_total_hp" , v.druability , v.druability ) )
	    self.towLst[v.tower_type].hp:SetAnchorPoint( 0 , 0.5 )
	    self.towLst[v.tower_type].hp:SetPosition( -80 , 13 )
	    self.towLst[v.tower_type].preview:AddChild( self.towLst[v.tower_type].hp )

	    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "sp_float_bg")
	    local sp_float_bg = Util:Sprite9( sp )
	    sp_float_bg:SetAnchorPoint( 0.5 , 0.5 )
	    sp_float_bg:SetContentSize( sp_float_bg:GetContentWidth() , sp_float_bg:GetContentHeight() + 30 )
	    sp_float_bg:SetPosition( self.towLst[v.tower_type].house:GetPositionX() , self.towLst[v.tower_type].house:GetPositionY() - ( (self.towLst[v.tower_type].house:GetContentHeight()*v.icon_scale) /2) )
	    self.widget:AddChild( sp_float_bg )

	    local name = Util:Label( v.name , nil , Macros.Quality[v.tower_type] )
	    name:SetAnchorPoint( 0.5 , 0.5 )
	    name:SetPosition( sp_float_bg:GetContentWidth()/2 , sp_float_bg:GetContentHeight()/2 + 15 )
	    sp_float_bg:AddChild( name )

	    self.towLst[v.tower_type].time = Util:Label( CPPGameLib.GetString("union_combat_defenseLastTime" , " " ) , Macros.TypefaceSize.slightly )
	    self.towLst[v.tower_type].time:SetAnchorPoint( 0.5 , 0.5 )
	    self.towLst[v.tower_type].time:SetPosition( sp_float_bg:GetContentWidth()/2 , sp_float_bg:GetContentHeight()/2 - 15 )
	    sp_float_bg:AddChild( self.towLst[v.tower_type].time )

	end

	local starDetail = CallModuleFunc(ModuleType.UNION , "GetUnionCombatStarDetail" )
	local starInfo = CallModuleFunc(ModuleType.UNION , "ToIdGetUnionCombatStarInfo" , starDetail.star_id )

	--进攻方
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "sp_bule")
	self.sp_bule = Util:Sprite( sp )
	self.sp_bule:SetAnchorPoint( 0 , 0.5 )
	self.sp_bule:SetPosition( 0 , self.widget:GetContentHeight()/2 )
	self.widget:AddChild( self.sp_bule )

	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION, "union_icon".. starInfo[1].union_icon )
	local icon = Util:Sprite( sp )
	icon:SetAnchorPoint( 0.5 , 0.5 )
	icon:SetPosition( self.sp_bule:GetContentWidth()/2 - 10 , self.sp_bule:GetContentHeight()/2 + 15 )
	self.sp_bule:AddChild( icon )

	local name = Util:Label( starInfo[1].union_name , Macros.TypefaceSize.largish , Macros.Color.ladderVs_1 )
	name:SetAnchorPoint( 0.5 , 0.5 )
	name:SetPosition( icon:GetPositionX() , 20 )
	self.sp_bule:AddChild( name )

	--防守方
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "sp_red")
	self.sp_red = Util:Sprite( sp )
	self.sp_red:SetAnchorPoint( 1 , 0.5 )
	self.sp_red:SetPosition( self.widget:GetContentWidth() , self.widget:GetContentHeight()/2 )
	self.widget:AddChild( self.sp_red )

	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION, "union_icon".. starInfo[2].union_icon )
	if starInfo[2].isPerson == false then
		sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_STAR , starInfo[2].union_icon )
	end

	local icon = Util:Sprite( sp )
	icon:SetAnchorPoint( 0.5 , 0.5 )
	icon:SetPosition( self.sp_red:GetContentWidth()/2 + 10 , self.sp_red:GetContentHeight()/2 + 15 )
	self.sp_red:AddChild( icon )

	if starInfo[2].isPerson == false then
		icon:SetScaleFactor( 0.3 )
	end

	local name = Util:Label( starInfo[2].union_name , Macros.TypefaceSize.largish , Macros.Color.ladderVs_2 )
	name:SetAnchorPoint( 0.5 , 0.5 )
	name:SetPosition( icon:GetPositionX() , 20 )
	self.sp_red:AddChild( name )

    --规则说明
    self.__btn_rule = Util:Button(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON, "icon_rule"), function()
        local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
        help:SetLoadedCallback( function ( ... )
           help:SetCell("Rule_UnionCombat")
        end )
    end)
    self.__btn_rule:SetPosition( self.widget:GetContentWidth() - 50 , self.widget:GetContentHeight() - 50 )
    self.widget:AddChild(self.__btn_rule)
    self.__btn_rule:SetScaleFactor( 0.6 )  
    local label = Util:Label(CPPGameLib.GetString("integral_race_rule_txt"), Macros.TypefaceSize.tab , Macros.Color.btn, 0, 0, Macros.Color.common_stroke,2)
    self.__btn_rule:AddChild(label)
    PosTool.CenterBottom(label,0,12)
end

function Modules.UnionCombatBattlegroundView:__SetShowUI()
	local towLst = CallModuleFunc(ModuleType.UNION , "ToIdGetUnionCombatTowerInfo")

	local starDetail = CallModuleFunc(ModuleType.UNION , "GetUnionCombatStarDetail" )
	local starInfo = CallModuleFunc(ModuleType.UNION , "ToIdGetUnionCombatStarInfo" , starDetail.star_id )

	for i , v in ipairs( towLst ) do
		local towerInfo = CallModuleFunc(ModuleType.UNION , "ToIdGetTowerDefenseInfo" , v.tower_id )
		self.towLst[v.tower_type].preview:SetProgress( towerInfo.cur_durability )
		self.towLst[v.tower_type].hp:SetString( CPPGameLib.GetString("monster_stormcastle_total_hp" , towerInfo.cur_durability , v.druability ) )
		
		local time = v.monster_defense_times - towerInfo.monster_beattack_times
		if starInfo[2].isPerson == true then
			time = 0

			local towerCnf = CallModuleFunc(ModuleType.UNION , "GetGuildWarTowerCnf" , v.tower_id )
			local defenseInfo = CallModuleFunc(ModuleType.UNION , "ToIdGetTowerDefenseInfo" , v.tower_id )
			for __ , vv in ipairs( defenseInfo.defender_list.__list ) do
				time = time + ( towerCnf.role_defense_times - vv.be_attack_times )
			end
		end
		self.towLst[v.tower_type].time:SetString( CPPGameLib.GetString("union_combat_defenseLastTime" , time ) )

		--倒塌
		self:SetCollapse( v.tower_id )
	end

	self:SetFlag()
end

function Modules.UnionCombatBattlegroundView:SetFlag()
	local starDetail = CallModuleFunc(ModuleType.UNION , "GetUnionCombatStarDetail" )
	local occupyInfo = CallModuleFunc(ModuleType.UNION , "ToIdGetUnionCombatOccupyInfo" , starDetail.star_id )
	if occupyInfo then
		self.sp_bule:SetVisible( false )
		self.sp_red:SetVisible( false )
	end
end

--倒塌
function Modules.UnionCombatBattlegroundView:SetCollapse( tower_id )
	local isBase , isAttack , isOver = CallModuleFunc(ModuleType.UNION , "JudgeCanItAttack" , tower_id )
	if isOver == true then
		local towerCnf = CallModuleFunc(ModuleType.UNION , "GetGuildWarTowerCnf" , tower_id )
		self.towLst[towerCnf.tower_type].house:SetGray( true )
		WidgetUnity.SetWidgetListener( self.towLst[towerCnf.tower_type].touch , nil, nil, function( ... )
			GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_combat_ruins") )
		end)
	end
end

function Modules.UnionCombatBattlegroundView:Result()
	local starInfo = CallModuleFunc(ModuleType.UNION , "GetUnionCombatStarDetail")
	local type_ = CallModuleFunc( ModuleType.UNION , "GetGuildWarVSActivityTimeCnf" )
	local duiZhanKey = false
	for i , v in ipairs( type_ ) do
		if v == "duiZhan" then
			duiZhanKey = true
		end
	end

	if duiZhanKey and starInfo.is_over == 1 then

		self:SetRuins()

		self.shield = nil
	    self.shield = LogicGUI.ShieldLayer.New( function ( ... )
	    	self.shield:RemoveSelf(false)
	    	self.shield = nil
	    	self:CloseManual()
	    end )
	    self.shield:SetAnchorPoint(0.5,0.5)
	    self:AddChild(self.shield:GetNode())  
	    PosTool.Center(self.shield)

		local sp = Resource.LoadTool.GetImageData( Resource.UITextureAtlasType.UNION_COMBAT, "sp_ladder_vs_bg" )
		local sp_ladder_vs_bg = Util:Sprite( sp )
		sp_ladder_vs_bg:SetAnchorPoint( 0.5 , 0.5 )
		self.shield:AddChild( sp_ladder_vs_bg )
		PosTool.Center( sp_ladder_vs_bg )

		local sp = Resource.LoadTool.GetImageData( Resource.UITextureAtlasType.UNION , "VS" , true )
		local VS = Util:Sprite( sp )
		VS:SetAnchorPoint( 0.5 , 0.5 )
		sp_ladder_vs_bg:AddChild( VS )
		PosTool.Center( VS )

		local sp = Resource.LoadTool.GetImageData( Resource.UITextureAtlasType.UNION , "VS" , true )
		local VS = Util:Sprite( sp )
		VS:SetAnchorPoint( 0.5 , 0.5 )
		sp_ladder_vs_bg:AddChild( VS )
		PosTool.Center( VS )

		local sp = Resource.LoadTool.GetImageData( Resource.UITextureAtlasType.UNION , "sp_success" , true )
		local sp_success = Util:Sprite( sp )
		sp_success:SetAnchorPoint( 0.5 , 0.5 )
		sp_ladder_vs_bg:AddChild( sp_success )
		PosTool.Center( sp_success , -300 , 130 )
		sp_success:SetScaleFactor(1.5)

		local sp = Resource.LoadTool.GetImageData( Resource.UITextureAtlasType.UNION , "sp_failed" , true )
		local sp_failed = Util:Sprite( sp )
		sp_failed:SetAnchorPoint( 0.5 , 0.5 )
		sp_ladder_vs_bg:AddChild( sp_failed )
		PosTool.Center( sp_failed , 300 , 130 )
		sp_failed:SetScaleFactor(1.5)

		local starDetail = CallModuleFunc(ModuleType.UNION , "GetUnionCombatStarDetail" )
		local starInfo = CallModuleFunc(ModuleType.UNION , "ToIdGetUnionCombatStarInfo" , starDetail.star_id )

		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION, "union_icon".. starInfo[2].union_icon )
		local icon = Util:Sprite( sp )
		icon:SetAnchorPoint( 0.5 , 0.5 )
		sp_ladder_vs_bg:AddChild( icon )
		PosTool.Center( icon , -300 )

		local name = Util:Label( starInfo[2].union_name , Macros.TypefaceSize.largish , Macros.Color.ladderVs_1 )
		name:SetAnchorPoint( 0.5 , 0.5 )
		sp_ladder_vs_bg:AddChild( name )
		PosTool.Center( name , -300 , -100 )

		--防守方
		local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION, "union_icon".. starInfo[1].union_icon )
		if starInfo[1].isPerson == false then
			sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_STAR , starInfo[1].union_icon )
		end

		local icon = Util:Sprite( sp )
		icon:SetAnchorPoint( 0.5 , 0.5 )
		sp_ladder_vs_bg:AddChild( icon )
		PosTool.Center( icon , 300 )

		if starInfo[1].isPerson == false then
			icon:SetScaleFactor( 0.3 )
		end

		local name = Util:Label( starInfo[1].union_name , Macros.TypefaceSize.largish , Macros.Color.ladderVs_2 )
		name:SetAnchorPoint( 0.5 , 0.5 )
		sp_ladder_vs_bg:AddChild( name )
		PosTool.Center( name , 300 , -100 )

		local union_combat_battleEnd = Util:Label( CPPGameLib.GetString("union_combat_battleEnd") , Macros.TypefaceSize.largish )
		union_combat_battleEnd:SetAnchorPoint( 0.5 , 0.5 )
		sp_ladder_vs_bg:AddChild( union_combat_battleEnd )
		PosTool.Center( union_combat_battleEnd , 0 ,  -270 )
	end
end


function Modules.UnionCombatBattlegroundView:SetRuins()
	local towLst = CallModuleFunc(ModuleType.UNION , "ToIdGetUnionCombatTowerInfo")
	for i , v in ipairs( towLst ) do
		self.towLst[v.tower_type].preview:SetProgress( 0 )
		self.towLst[v.tower_type].hp:SetString( CPPGameLib.GetString("monster_stormcastle_total_hp" , 0 , v.druability ) )
		self.towLst[v.tower_type].time:SetString( CPPGameLib.GetString("union_combat_defenseLastTime" , 0 ) )
		self.towLst[v.tower_type].house:SetGray( true )
	end

end
