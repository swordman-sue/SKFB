
--[[
%% @module: 主角升级界面
%% @author: yjg
%% @created: 2019年1月19日15:29:08
--]]

Modules = Modules or {}

Modules.HeroUpgradeTabView = Modules.HeroUpgradeTabView or BaseClass(GUI.Template)

--刷新
function Modules.HeroUpgradeTabView:SetRoleInfo( )
	if not self.RoleInfoUI then
		self.RoleInfoUI = true
	else
		return
	end
	self:__Getwidget()
	self:__ShowUI()
end

function Modules.HeroUpgradeTabView:__Getwidget( )
	--箭头
	self.Image_arrows = self.__parent_view:GetWidget("Image_arrows")
	--当前	
	self.Image_now = self.__parent_view:GetWidget("Image_now")
	--下级
	self.Image_nex = self.__parent_view:GetWidget("Image_nex")
	--分割线
	self.Image_48_0_0 = self.__parent_view:GetWidget("Image_48_0_0")
end

function Modules.HeroUpgradeTabView:__ShowUI( )
	local hero_info , config_hero = self:__GetHeroData()
	local config_prop = CPPGameLib.GetConfig( "HeroProperty", 1 )
	config_prop = config_prop[hero_info.quality_level]
	
	local now_config_prop = config_prop[hero_info.level]
	local nex_config_prop = config_prop[hero_info.level + 1]

	--当前
	if now_config_prop then
		
		local common_lev6 = Util:Label( CPPGameLib.GetString( "common_lev6" , hero_info.level ) , Macros.TypefaceSize.largish , Macros.Color.keypoint )
		common_lev6:SetAnchorPoint( 0.5 , 0.5 )
		common_lev6:SetPosition( self.Image_now:GetContentWidth()/2 , self.Image_now:GetContentHeight()/2 + 15 )
		self.Image_now:AddChild( common_lev6 )

		local prop = {}
		prop[1] = { name = CPPGameLib.GetString("nature1") , value = now_config_prop.attack } 
		prop[2] = { name = CPPGameLib.GetString("nature4") , value = now_config_prop.blood_max }
		prop[3] = { name = CPPGameLib.GetString("nature2") , value = now_config_prop.physical_defence }
		prop[4] = { name = CPPGameLib.GetString("nature3") , value = now_config_prop.magic_defence }

		for i , v in ipairs( prop ) do
			local name = Util:Label( CPPGameLib.GetString( "common_str_with_colon" , v.name ) , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
			name:SetAnchorPoint( 0 , 0.5 )
			name:SetPosition( 30 , ( -20 ) - ( (i-1) * 28 ) )
			self.Image_now:AddChild( name )

			local value = Util:Label( v.value , Macros.TypefaceSize.slightly , Macros.Color.content )
			self.Image_now:AddChild( value )
			PosTool.RightTo( name , value )
		end
	end

	--下级
	if nex_config_prop then

		local common_lev6 = Util:Label( CPPGameLib.GetString( "common_lev6" , hero_info.level + 1 ) , Macros.TypefaceSize.largish , Macros.Color.keypoint )
		common_lev6:SetAnchorPoint( 0.5 , 0.5 )
		common_lev6:SetPosition( self.Image_nex:GetContentWidth()/2 , self.Image_nex:GetContentHeight()/2 + 15 )
		self.Image_nex:AddChild( common_lev6 )

		local prop = {}
		prop[1] = { name = CPPGameLib.GetString("nature1") , value = nex_config_prop.attack } 
		prop[2] = { name = CPPGameLib.GetString("nature4") , value = nex_config_prop.blood_max }
		prop[3] = { name = CPPGameLib.GetString("nature2") , value = nex_config_prop.physical_defence }
		prop[4] = { name = CPPGameLib.GetString("nature3") , value = nex_config_prop.magic_defence }

		for i , v in ipairs( prop ) do
			local name = Util:Label( CPPGameLib.GetString( "common_str_with_colon" , v.name ) , Macros.TypefaceSize.slightly , Macros.Color.keypoint )
			name:SetAnchorPoint( 0 , 0.5 )
			name:SetPosition( 30 , ( -20 ) - ( (i-1) * 28 ) )
			self.Image_nex:AddChild( name )

			local value = Util:Label( v.value , Macros.TypefaceSize.slightly , Macros.Color.front_color )
			self.Image_nex:AddChild( value )
			PosTool.RightTo( name , value )
		end
	else
		local sp_level_max = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW , "level_max" , true)
		local level_max = Util:Sprite( sp_level_max )
		level_max:SetAnchorPoint( 0.5 , 0.5 )
		level_max:SetPosition( self.Image_nex:GetContentWidth()/2 , self.Image_nex:GetContentHeight()/2 - 75 )
		self.Image_nex:AddChild( level_max )
	end

	local value = Util:Label( CPPGameLib.GetString("login_RoleTips") , Macros.TypefaceSize.tab , Macros.Color.keypoint ,  400 )
	value:SetAnchorPoint( 0.5 , 1 )
	self.Image_48_0_0:AddChild( value )
	PosTool.Center( value , 0 , -50 )

end