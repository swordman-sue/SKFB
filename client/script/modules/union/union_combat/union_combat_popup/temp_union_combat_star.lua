--
-- @brief 工会战（星球）
-- @author: yjg
-- @date: 2016年10月14日10:39:24
--
LogicGUI = LogicGUI or {}

LogicGUI.TempUnionCombatStar = LogicGUI.TempUnionCombatStar or BaseClass(GUI.Template)

function LogicGUI.TempUnionCombatStar:__init( data )
	self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  

    self.data = data
	self:__Getwidget( data )
	self:__ShowUI() 
end

function LogicGUI.TempUnionCombatStar:__delete( )
end

function LogicGUI.TempUnionCombatStar:__CreateWidgetFromLuaCode()
    self.layout = Util:Layout( 10 , 10 )
    return self.layout
end
function LogicGUI.TempUnionCombatStar:__AddToParent()
end

function LogicGUI.TempUnionCombatStar:__Getwidget( data )

	self.star = nil
	self.scale = 1
	local scale = 1

	--图片
	if data.sp then
		for i , v in ipairs( data.sp ) do
			local sp = nil
			if i == 2 then
				sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_COMBAT , v )
			else
				sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION_STAR , v )
			end
			local icon = Util:Sprite( sp )
			--旋转
			if data.sp_rotat and data.sp_rotat[i] then
				icon:SetRotation( data.sp_rotat[i] )
			end

			--缩放
			if data.sp_scale and data.sp_scale[i] then
				icon:SetScaleFactor( data.sp_scale[i] )
				if i == 1 then
					scale = data.sp_scale[i]
					self.scale = data.sp_scale[i]
				end
			end

			-- 层级
			if data.sp_zorder and data.sp_zorder[i] then
				self.layout:AddChild( icon , data.sp_zorder[i] )
			else
				self.layout:AddChild( icon )
			end

			if i ~= 1 then
				--坐标		
				if data.sp_pos and data.sp_pos[i] then
					PosTool.Center( icon , data.sp_pos[i][1] , data.sp_pos[i][2] )
				else
					PosTool.Center( icon )
				end
			else
				self.star = icon
				PosTool.Center( icon )
			end
		end
	end

	if self.star then
		--名字
		if data.name then
			local name = Util:Name( data.name , data.name_scale , data.star_type )
			name:SetAnchorPoint( 0.5 , 0.5 )
			name:SetPosition( tonumber(data.name_pos[1]) , tonumber(data.name_pos[2]) )
			self.layout:AddChild( name , 9 )
		end
		local tou = Util:Layout( self.star:GetContentWidth() * scale , self.star:GetContentHeight() * scale , function( ... )
			local type_ = CallModuleFunc(ModuleType.UNION , "GetIsCanTouchStar" )
			if type_ then
				LogicGUI.TempUnionCombatStarInfo.New( data.star_id )
			end
		end )
		tou:SetAnchorPoint( 0.5 , 0.5 )
	    self:AddChild( tou )
	    PosTool.Center( tou )
    end


	if data.effects then
		for j , v in ipairs( data.effect ) do
	        local effect_con = nil

	        local zorder = 1
	        if data.effects_zorder[j] then
	        	zorder = data.effects_zorder[j]
	        end

	        local effect_con , effect_con = self:__CreateEffect({ id = v }, self.star , zorder )
	        if data.effects_pos and data.effects_pos[j] then
		        PosTool.Center(effect_con , data.effects_pos[j][1] , data.effects_pos[j][1] )
			else
		       	PosTool.Center(effect_con)
		    end

			--旋转
			if data.effects_rotat and data.effects_rotat[j] then
				effect_con:SetRotation( data.effects_rotat[j] )
			end
		end
	end
end

function LogicGUI.TempUnionCombatStar:__ShowUI( )
end

function LogicGUI.TempUnionCombatStar:GetIcon( )
	return self.star
end

function LogicGUI.TempUnionCombatStar:SetUnionName( )
	if self.Panel_icon then
		self.Panel_icon:RemoveSelf(false)
		self.Panel_icon = nil
	end
	
	local unionInfo = CallModuleFunc(ModuleType.UNION , "GuildWarIsMyUnion" , self.data.star_id )
	if not unionInfo.icon_id or unionInfo.icon_id == 0 then
		return 
	end

	local iconSp = "union_icon" .. unionInfo.icon_id
	local icon = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION , iconSp)

	local tab = {
		{ tp = icon, dx = 0.3 },
		{ zi = unionInfo.guild_name , ys = 0 , jg = 5 , mb = true , dx = Macros.TypefaceSize.minimum },
	}
	self.Panel_icon = Util:LabCenter(tab)
	self.Panel_icon:SetPosition( self.star:GetPositionX() , self.star:GetPositionY() + ( ( self.star:GetContentHeight() * self.scale ) /2) )
	self.layout:AddChild( self.Panel_icon , 99 )
end

function LogicGUI.TempUnionCombatStar:SetCombatType( )
	if self.Effect_Id then
		self:__DelEffect(self.Effect_Id)
		self.Effect_Id = nil
	end	
	
	local unionInfo = CallModuleFunc(ModuleType.UNION , "GuildWarIsMyUnion" , self.data.star_id )
	if unionInfo.is_battle ~= 1 then
		return 
	end

	self.Effect_Id, effect = self:__CreateEffect({id = 100107}, self.star , 1)
	PosTool.Center(effect)
end
