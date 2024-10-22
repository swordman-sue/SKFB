--
-- @brief 工会战（星球）
-- @author: yjg
-- @date: 2016年10月14日10:39:24
--
LogicGUI = LogicGUI or {}

LogicGUI.TempUnionCombatEvacuate = LogicGUI.TempUnionCombatEvacuate or BaseClass(GUI.Template)

function LogicGUI.TempUnionCombatEvacuate:__init( id )
	self.__layout_mode = GUI.Template.LayoutMode.CODE
	self.__bg_params = {type = BaseView.BGType.SOLID}	
    self:InitTemplate(nil)  
	self.id = id
	self:__Getwidget()
end

function LogicGUI.TempUnionCombatEvacuate:__delete( )
end

function LogicGUI.TempUnionCombatEvacuate:__Server( )
end

function LogicGUI.TempUnionCombatEvacuate:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height,function( ... )
    	self:DeleteMe()
    end)
    return self.layout
end

function LogicGUI.TempUnionCombatEvacuate:__Getwidget( )
	--背景
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "sp_union_secondary")
	self.bg = Util:Sprite( sp , function( ... )
		print("xy" , "=====")
	end)
	self.bg:SetAnchorPoint( 0.5 , 0.5 )
	self.layout:AddChild( self.bg )
	PosTool.Center( self.bg )
	
	--撤离确认
	local union_combat_evacuateAffirm = Util:Label( CPPGameLib.GetString("union_combat_evacuateAffirm") , Macros.TypefaceSize.largish , Macros.Color.white )
	union_combat_evacuateAffirm:SetPosition( self.bg:GetContentWidth()/2 , self.bg:GetContentHeight() - 20 )
	self.bg:AddChild( union_combat_evacuateAffirm )

	--是否确定从XXX撤离
	local starCnf = CallModuleFunc(ModuleType.UNION , "GetGuildWarStarCnf" , self.id )
	local lab = string.format( Macros.Quality_RT[starCnf.star_type] , starCnf.name )
	local union_combat_evacuateAffirmTips_1 = Util:RichText( CPPGameLib.GetString("union_combat_evacuateAffirmTips_1" , lab ) )
	union_combat_evacuateAffirmTips_1:SetAnchorPoint( 0.5 , 0.5 )
	union_combat_evacuateAffirmTips_1:SetPosition( self.bg:GetContentWidth()/2 , self.bg:GetContentHeight()/2 + 40 )
	self.bg:AddChild( union_combat_evacuateAffirmTips_1 )

	local union_combat_evacuateAffirmTips_2 = Util:RichText( CPPGameLib.GetString("union_combat_evacuateAffirmTips_2" ) , nil , self.bg:GetContentWidth() - 80 )
	union_combat_evacuateAffirmTips_2:SetAnchorPoint( 0.5 , 1 )
	union_combat_evacuateAffirmTips_2:SetPosition( self.bg:GetContentWidth()/2 , self.bg:GetContentHeight()/2 + 25 )
	self.bg:AddChild( union_combat_evacuateAffirmTips_2 )
	
	--取消
	local btn_gather_littlegreen = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "btn_gather_big_green")
	--确定
	local btn_gather_little = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "btn_gather_big")

	--确定
	self.common_confirm = Util:Button( btn_gather_little , function( ... )
		local type_ = CallModuleFunc(ModuleType.UNION , "GetMyPermission" , Macros.Game.UNION.DECLARE_WAR )
		if type_ == 0 then
			GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_combat_adjustment") )
		else
			CallModuleFunc( ModuleType.UNION , "GuildWarEvacuateReq" , self.id )
		end
		self:DeleteMe()
	end , CPPGameLib.GetString("common_confirm") , Macros.TypefaceSize.slightly , Macros.Color.lilyWhite , nil )
	self.common_confirm:SetAnchorPoint( 0.5 , 0.5 )
	self.common_confirm:SetPosition( self.bg:GetContentWidth()/2 + 90 , self.bg:GetContentHeight()/2 - 55 )
	self.bg:AddChild( self.common_confirm )

    --取消
	self.common_cancel = Util:Button( btn_gather_littlegreen , function( ... )
		self:DeleteMe()
	end , CPPGameLib.GetString("common_cancel") , Macros.TypefaceSize.slightly , Macros.Color.lilyWhite , nil )
	self.common_cancel:SetAnchorPoint( 0.5 , 0.5 )
	self.common_cancel:SetPosition( self.bg:GetContentWidth()/2 - 90 , self.bg:GetContentHeight()/2 - 55 )
	self.bg:AddChild( self.common_cancel )

end
