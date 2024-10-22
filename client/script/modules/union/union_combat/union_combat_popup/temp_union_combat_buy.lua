
-- @brief 次数购买 
-- @author: yjg
-- @date: 2018年10月22日17:38:23
--
LogicGUI = LogicGUI or {}

LogicGUI.TempUnionCombatBuy = LogicGUI.TempUnionCombatBuy or BaseClass(GUI.Template)

function LogicGUI.TempUnionCombatBuy:__init( data )
	self.__layout_mode = GUI.Template.LayoutMode.CODE
	self.__bg_params = {type = BaseView.BGType.SOLID,color = {0,0,0,100}}
    self:InitTemplate(nil)  
	self:__Getwidget( )
end

function LogicGUI.TempUnionCombatBuy:__delete( )
end

function LogicGUI.TempUnionCombatBuy:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout(Macros.Global.DesignSize.width, Macros.Global.DesignSize.height,function( ... )
   		self:DeleteMe()
    end)
    return self.layout
end

function LogicGUI.TempUnionCombatBuy:__Getwidget( )
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "sp_union_secondary") 
	self.sp_frame_bg = Util:Sprite( sp , function ( ... )
		-- body
	end)
	self.sp_frame_bg:SetAnchorPoint( 0.5 , 0.5 )
	self.sp_frame_bg:SetPosition( self.layout:GetContentWidth()/2 , self.layout:GetContentHeight()/2 )
	self.layout:AddChild( self.sp_frame_bg )
	--标题
	local heroShop_buyOk = Util:Label( CPPGameLib.GetString("heroShop_buyOk") , Macros.TypefaceSize.popup , Macros.Color.lilyWhite )
	heroShop_buyOk:SetPosition( self.sp_frame_bg:GetContentWidth()/2 , self.sp_frame_bg:GetContentHeight() - 20 )
	self.sp_frame_bg:AddChild( heroShop_buyOk )
	--内容
	-- 

	local consume = CallModuleFunc(ModuleType.UNION , "GetGuildWarCommonCnf" , 10 )
	local tab = {
					{ zi = CPPGameLib.GetString("union_combat_buyTime_1" , consume ) },
					{ tp = Util:GetMoneyIconImgData(Macros.Game.RolePropType.DIAMOND) , jg = 5 ,dx = 0.8},
					{ zi = CPPGameLib.GetString("union_combat_buyTime_2" ) , jg = 5 },
				}
	local buy = Util:LabCenter(tab)
	buy:SetAnchorPoint( 0.5 , 0.5 )  
	buy:SetPosition( self.sp_frame_bg:GetContentWidth()/2 , self.sp_frame_bg:GetContentHeight()/2 )
	self.sp_frame_bg:AddChild( buy )


	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "btn_gather_big_green" )
	local btn = Util:Button( sp , function( ... )
		-- local buyTime = CallModuleFunc(ModuleType.UNION , "GetUnionBuyTime")
		-- 
		self:DeleteMe()
	end , CPPGameLib.GetString("common_cancel") , nil , Macros.Color.lilyWhite )
    btn:SetAnchorPoint( 0.5 , 0.5 )
    btn:SetPosition( self.sp_frame_bg:GetContentWidth()/2 - 100 , 50 )
    self.sp_frame_bg:AddChild( btn )

	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT, "btn_gather_big" )
	local btn = Util:Button( sp , function( ... )
		local buyTime = CallModuleFunc(ModuleType.UNION , "GetUnionBuyTime")
		if buyTime ~= 0 then
			local diamond = CallModuleFunc(ModuleType.ROLE,"GetPropValue",Macros.Game.RolePropType.DIAMOND)
			if consume <= diamond then
				CallModuleFunc(ModuleType.UNION , "GuildWarBuyHighStarBattleTimesReq")
			else
				GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("common_diamond_not_enough"))
			end
		else
			GlobalTipMsg.GetInstance():Show(CPPGameLib.GetString("shop_limit_buy_num"))
		end
		self:DeleteMe()
	end , CPPGameLib.GetString("common_confirm") , nil , Macros.Color.lilyWhite )
    btn:SetAnchorPoint( 0.5 , 0.5 )
    btn:SetPosition( self.sp_frame_bg:GetContentWidth()/2 + 100 , 50 )
    self.sp_frame_bg:AddChild( btn )

end

