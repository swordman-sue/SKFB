
--[[
%% @module: 战斗失败界面
%% @author: swordman sue
%% @created: 2016-11-16
--]]

Modules = Modules or {}

Modules.PVPLoseView = Modules.PVPLoseView or BaseClass(BaseView)

function Modules.PVPLoseView:__init()
	self.__layout_name = "pvp_battle_result_lose"	
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__open_loading = false

    self.__open_callback = function()
    	self:__OpenCallback()
    end

	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.PVP)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.PVP,true)  
end

function Modules.PVPLoseView:__OpenCallback()
	self:__Getwidget()

	self:MyHeroInfo()
	self:HeHeroInfo()
	self:ShowHeadline()

	self:PlayActionTimeline("animation0", false, function()
		self:SetShowEffects()
	end)
	
end

function Modules.PVPLoseView:__Getwidget()

	self.Image_2 = self:GetWidget("Image_2")
	--左边
	self.Panel_left = self:GetWidget("Panel_left")
	--左边英雄
	self.Panel_left_hero = self:GetWidget("Panel_left_hero")
	--左边属性
	self.Panel_left_property = self:GetWidget("Panel_left_property")
	--状态
	self.Panel_left_headline = self:GetWidget("Panel_left_headline")
	--属性
	self.Text_left = {}
	for i = 1 , 3 do
		self.Text_left[i] = self:GetWidget( "Text_left_" .. i )
	end

	--右边
	self.Panel_right = self:GetWidget("Panel_right")
	--右边英雄
	self.Panel_right_hero = self:GetWidget("Panel_right_hero")
	--右边属性
	self.Panel_right_property = self:GetWidget("Panel_right_property")
	--状态
	self.Panel_right_headline = self:GetWidget("Panel_right_headline")
	--属性
	self.Text_right = {}
	for i = 1 , 3 do
		self.Text_right[i] = self:GetWidget( "Text_right_" .. i )
	end

	--vs
	self.Panel_vs = self:GetWidget("Panel_vs")
end

function Modules.PVPLoseView:__Dispose()
	if self.faceEffects_1 then
		self.faceEffects_1:DeleteMe()
		self.faceEffects_1 = nil
	end
	if self.faceEffects_2 then
		self.faceEffects_2:DeleteMe()
		self.faceEffects_2 = nil
	end
end

function Modules.PVPLoseView:MyHeroInfo()
	local roleInfo = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
	--左边
	local panel_left = Util:Sprite( Resource.PathTool.GetHalfHeadPath( roleInfo.head_icon ) )
	panel_left:SetAnchorPoint( 0.5 , 0 )
	panel_left:SetPosition( self.Panel_left_hero:GetContentWidth()/2 , 0 )
	self.Panel_left_hero:AddChild( panel_left )

    --名字
    local name = Util:Label( roleInfo.role_name )
    name:SetAnchorPoint( 0.5 , 0.5 )
    name:SetPosition( self.Panel_left_hero:GetContentWidth()/2 , -20 )
    self.Panel_left_hero:AddChild( name )

    local info = CallModuleFunc(ModuleType.BATTLE, "GetMyResultData_PVP")

    if info.last_week_rank ~= 0 then
        local rankSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.PVP, "rank_"..info.last_week_rank , true )
        local left = Util:Sprite( rankSp )
        left:SetAnchorPoint( 1 , 0.5 )
        left:SetPosition( name:GetPositionX() - (name:GetContentWidth()/2) - 3  , name:GetPositionY()  )
        self.Panel_left_hero:AddChild( left )

        local rankSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.PVP, "rank_"..info.last_week_rank )
        local right = Util:Sprite( rankSp )
        right:SetAnchorPoint( 0 , 0.5 )
        right:SetPosition( name:GetPositionX() + (name:GetContentWidth()/2) + 5  , name:GetPositionY()  )
        self.Panel_left_hero:AddChild( right )
    end

    local lab = CPPGameLib.GetString("rank_reflection")
    if info.old_rank and info.old_rank > 0 then 
    	lab = info.old_rank
    end
    	
    local lst = {
    		[1] = { name = CPPGameLib.GetString("pvp_power") , value = roleInfo.battle_value or 0 , color = Macros.Color.white },
    		[2] = { name = CPPGameLib.GetString("pvp_ranking_") , value = lab , color = Macros.Color.white },
    		[3] = { name = CPPGameLib.GetString("pvp_integral_") , value = 0 , color = Macros.Color.green }
	}

	local text_ = {}
	self.myTabLst = {}
	for i,v in ipairs( lst ) do
	    --战力
		local tab = {
						{ zi = v.name , ys = Macros.Color.Special , dx = Macros.TypefaceSize.normal },
						{ zi = v.value , ys = v.color , dx = Macros.TypefaceSize.normal },
					}
		text_[i] , self.myTabLst[i] = Util:LabCenter( tab )
		text_[i]:SetAnchorPoint( 0 , 0.5 )
	    text_[i]:SetPosition( 0 , self.Text_left[i]:GetContentHeight()/2 )
	    self.Text_left[i]:AddChild(text_[i])

		local num = CallModuleFunc(ModuleType.PVP, "GetPVPAwardTime")
	    if v.name == CPPGameLib.GetString("pvp_ranking_") and info.new_rank ~= info.old_rank then

	    	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON, "team_updatearrow")
			local team_updatearrow = Util:Sprite( sp )
			self.Text_left[i]:AddChild( team_updatearrow )	
			PosTool.RightTo( text_[i] , team_updatearrow , 2 )

			local color = Macros.Color.white

	    	if info.old_rank < info.new_rank then
	    		color = Macros.Color.red
	    	elseif info.old_rank > info.new_rank then
	    		color = Macros.Color.green
	    	end
	    	if info.old_rank <= 0 then
				color = Macros.Color.green
	    	end

		    if not info.new_rank or info.new_rank <= 0 then 
		    	info.new_rank = CPPGameLib.GetString("rank_reflection")
		    end
		    --名字
		    local new_rank = Util:Label( info.new_rank , nil , color )
		    self.Text_left[i]:AddChild( new_rank )
		    PosTool.RightTo( team_updatearrow , new_rank , 2 )
	    end
	end
end

function Modules.PVPLoseView:HeHeroInfo()
	local battle_info = CallModuleFunc(ModuleType.BATTLE, "GetCurInfo")
	local roleInfo = battle_info.target_data

	--左边
	local panel_left = Util:Sprite( Resource.PathTool.GetHalfHeadPath( roleInfo.head_icon ) )
	panel_left:SetFlipX(true)
	panel_left:SetAnchorPoint( 0.5 , 0 )
	panel_left:SetPosition( self.Panel_right_hero:GetContentWidth()/2 , 0 )
	self.Panel_right_hero:AddChild( panel_left )

    --名字
    local name = Util:Label( roleInfo.name )
    name:SetAnchorPoint( 0.5 , 0.5 )
    name:SetPosition( self.Panel_right_hero:GetContentWidth()/2 , -20 )
    self.Panel_right_hero:AddChild( name )

    local info = CallModuleFunc(ModuleType.BATTLE, "GetEnemyResultData_PVP")

    if info.last_week_rank ~= 0 then
        local rankSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.PVP, "rank_"..info.last_week_rank , true )
        local left = Util:Sprite( rankSp )
        left:SetAnchorPoint( 1 , 0.5 )
        left:SetPosition( name:GetPositionX() - (name:GetContentWidth()/2) - 3  , name:GetPositionY()  )
        self.Panel_right_hero:AddChild( left )

        local rankSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.PVP, "rank_"..info.last_week_rank )
        local right = Util:Sprite( rankSp )
        right:SetAnchorPoint( 0 , 0.5 )
        right:SetPosition( name:GetPositionX() + (name:GetContentWidth()/2) + 5  , name:GetPositionY()  )
        self.Panel_right_hero:AddChild( right )
    end

    if not info.new_rank or info.new_rank <= 0 then 
    	info.new_rank = CPPGameLib.GetString("rank_reflection")
    end

    local lst = {
    		[1] = { name = CPPGameLib.GetString("pvp_power") , value = roleInfo.fighting_power or 0 },
    		[2] = { name = CPPGameLib.GetString("pvp_ranking_") , value = info.new_rank or 0 },
    		[3] = { name = CPPGameLib.GetString("pvp_integral_") , value = 0 }
	}

	local text_ = {}
	self.heTabLst = {}
	for i,v in ipairs( lst ) do
	    --战力
		local tab = {
						{ zi = v.name , ys = Macros.Color.Special , dx = Macros.TypefaceSize.normal },
						{ zi = v.value , ys = Macros.Color.white , dx = Macros.TypefaceSize.normal },
					}
		text_[i] , self.heTabLst[i] = Util:LabCenter( tab )
		text_[i]:SetAnchorPoint( 0 , 0.5 )
	    text_[i]:SetPosition( 0 , self.Text_right[i]:GetContentHeight()/2 )
	    self.Text_right[i]:AddChild(text_[i])

	  --   if v.name == CPPGameLib.GetString("pvp_ranking") then

	  --   	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON, "team_updatearrow")
			-- local team_updatearrow = Util:Sprite( sp )
			-- self.Text_left[i]:AddChild( team_updatearrow )	
			-- PosTool.RightTo( text_[i] , team_updatearrow , 2 )

			-- local color = Macros.Color.white
	  --   	if info.blue_old_rank < info.blue_new_rank then
	  --   		color = Macros.Color.green
	  --   	elseif info.blue_old_rank > info.blue_new_rank then
	  --   		color = Macros.Color.blue
	  --   	end

		 --    --名字
		 --    local new_rank = Util:Label( info.blue_new_rank , nil , color )
		 --    self.Text_left[i]:AddChild( new_rank )
		 --    PosTool.RightTo( team_updatearrow , new_rank , 2 )
	  --   end
	end
end

function Modules.PVPLoseView:ShowHeadline()
	local win = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.PVP, "win",true)
	local lose = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.PVP, "lose",true)
	local draw = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.PVP, "draw",true)

	if unpack(self.__open_data) == "win" then

		self.Panel_left_headline:SetBackGroundImage( win )
		self.Panel_left_headline:SetVisible(false)
		self.Panel_right_headline:SetBackGroundImage( lose )

		local effectId , effectCon = self:__CreateEffect({res_name = "UI_pvpjiesuan_language_chs"}, self.Panel_left )
		effectCon:SetPosition( self.Panel_left_headline:GetPositionX() , self.Panel_left_headline:GetPositionY() )

	elseif unpack(self.__open_data) == "tie" then

		self.Panel_left_headline:SetBackGroundImage( draw )
		self.Panel_right_headline:SetBackGroundImage( draw )

	elseif unpack(self.__open_data) == "lose" then

		self.Panel_left_headline:SetBackGroundImage( lose )
		self.Panel_right_headline:SetBackGroundImage( win )
		self.Panel_right_headline:SetVisible(false)

		local effectId , effectCon = self:__CreateEffect({res_name = "UI_pvpjiesuan_language_chs"}, self.Panel_right )
		effectCon:SetPosition( self.Panel_right_headline:GetPositionX() , self.Panel_right_headline:GetPositionY() )

	end
end

function Modules.PVPLoseView:SetShowEffects()
	CPPActionManager.DelayTo( self.Image_2 , 0.7 , GlobalCallbackMgr:AddCallBackFunc( function ( ... )
		local info = CallModuleFunc(ModuleType.BATTLE, "GetMyResultData_PVP")

		local num = CallModuleFunc(ModuleType.PVP, "GetPVPAwardTime")
    	if num and num ~= 0 then
		    self.faceEffects_1 = TypefaceEffects.New()
		    self.faceEffects_1:Roll( self.myTabLst[3][2] , info.score , nil , function ( ... )
		    
		    end)
		else
			self.myTabLst[3][2]:SetString( 0 )
    	end

		local info = CallModuleFunc(ModuleType.BATTLE, "GetEnemyResultData_PVP")
	    self.faceEffects_2 = TypefaceEffects.New()
	    self.faceEffects_2:Roll( self.heTabLst[3][2] , info.score , nil , function ( ... )
	    
			--点击
			self.Panel_click = self:GetWidget("Panel_click")
			--点击屏幕继续
			WidgetUnity.SetWidgetListener(self.__root_widget, nil, nil, function()
				self:CloseManual(true)
				CallModuleFunc(ModuleType.BATTLE, "BattleEndReturnRelevantView")
			end)
			local battle_result_continue = Util:Label( 
														CPPGameLib.GetString("battle_result_continue") ,  
														Macros.TypefaceSize.popup,
														Macros.Color.btn,nil,nil,
														Macros.Color.white_stroke
													)
			self.Panel_click:AddChild( battle_result_continue )

			PosTool.Center( battle_result_continue )
			
	    end)

	end ) )
end