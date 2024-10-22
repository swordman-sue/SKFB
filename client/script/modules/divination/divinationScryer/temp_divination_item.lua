
--
-- @brief 占卜条目
-- @author: yjg
-- @date: 2017年11月30日23:09:20
--

LogicGUI = LogicGUI or {}

LogicGUI.TempDivinationItem = LogicGUI.TempDivinationItem or BaseClass(UILayer)

function LogicGUI.TempDivinationItem:__init()

    local bg = Util:Sprite( Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.DIVINATION, "sp_divination_cardBg" ) )
	self:SetAnchorPoint( 0.5 , 0.5 )
	self:SetContentSize( bg:GetContentWidth() , bg:GetContentHeight() )

    self:SetBackGroundColor(0, 0, 0)
    self:SetBackGroundColorType(1)
    self:SetBackGroundColorOpacity(120)

    --self.card
    self.card = nil
    self.show = nil
    self.price = nil
end

function LogicGUI.TempDivinationItem:__delete( )
	self:RemoveCardFront()
	self:RemoveCardPrice()
	self:RemoveCard()
end

--盖牌
function LogicGUI.TempDivinationItem:ShowCardBack( type_ )
	self.type = type_
	self:RemoveCardFront()

	local mapIdx = CallModuleFunc(ModuleType.DIVINATION, "GetCurLowCardGroupId" )

	if self.type == Macros.Game.CARD_TYPE.LOW then
	    local con = nil
	    self.card , con = GlobalEffectMgr:CreateUIEffect( {     
	                                                        res_name = "UI_zhanbufanpai" .. mapIdx , 
	                                                        type = Effect.EffectType.SKELETON , 
	                                                        act_index = RenderObj.ActionIndex.IDLE ,
	                                                    } , self , 10 )
	    con:SetAnchorPoint(0.5 , 0.5 )
	    con:SetPosition( self:GetNode():GetContentWidth()/2 ,  self:GetNode():GetContentHeight()/2 )
	else
	    local con = nil
	    self.card , con = GlobalEffectMgr:CreateUIEffect( {     
	                                                        res_name = "UI_zhanbufanpai" .. 9 , 
	                                                        type = Effect.EffectType.SKELETON , 
	                                                        act_index = RenderObj.ActionIndex.IDLE ,
	                                                    } , self , 10 )
	    con:SetAnchorPoint(0.5 , 0.5 )
	    con:SetPosition( self:GetNode():GetContentWidth()/2 ,  self:GetNode():GetContentHeight()/2 )
	end

    --价钱
    self:ShowCardPrice( type_ )
end

--翻牌
function LogicGUI.TempDivinationItem:ShowCardDraw( id  )

	self:RemoveCardPrice()

	if self.card then
        local node = GlobalEffectMgr:GetInnerEffect( self.card )
        if node then

        	local _ , cnf = ObjIDTool.GetInfo(id)
        	if not cnf then
        		return
        	end

            --播放特效
            local effect = 100076
            if cnf.quality < Macros.Game.QualityType.PURPLE then
                effect = 100075 
            end

            local effectid , effectcon = GlobalEffectMgr:CreateUIEffect( {id = effect , loop = false} , self , 11 )
            PosTool.Center(effectcon)

            node:SetAnim( RenderObj.ActionIndex.IDLE2 , false , function ( ... )
            	if self.card then
            		self.card = nil
            	end
				self:ShowCardFront( id )
            end)
        end
	end
end

--删除模型
function LogicGUI.TempDivinationItem:RemoveCard()
	if self.card then
		local node = GlobalEffectMgr:GetInnerEffect( self.card ) 
		if node then
			if self.card then
				GlobalEffectMgr:DelEffect(self.card)
				self.card = nil
			end
		else
			self.card = nil
		end
	end	
end

--正面
function LogicGUI.TempDivinationItem:ShowCardFront( id )
	self:RemoveCard()
	self:RemoveCardPrice()

	--图标
    local _ , cnf = ObjIDTool.GetInfo( id  )
    local path = Resource.PathTool.GetBigIconPath( cnf.bigicon )
    self.show = Util:Sprite( path )
    self.show:SetAnchorPoint( 0.5 , 0.5 )
    self:AddChild( self.show )
    PosTool.Center( self.show )
    self.show:SetScaleFactor( 0.5 )

	--品质框    
    local frame = Util:Sprite( Resource.PathTool.GetUIResPath("divinationbig/icon_sp_card_".. cnf.quality ) )
    	-- Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_ICON, "icon_sp_card_" .. cnf.quality ) )
    frame:SetAnchorPoint( 0.5 , 0.5 )
    self.show:AddChild( frame )
    PosTool.Center( frame )

    --名字
    local name = Util:Name( cnf.name , Macros.TypefaceSize.tab , cnf.quality )
    name:SetAnchorPoint( 0.5 , 0.5 )
    name:SetPosition( 60 + 70 + 13 , 35 )
    frame:AddChild( name )
end

--删除正面
function LogicGUI.TempDivinationItem:RemoveCardFront( )
	if self.show then
		self.show:RemoveSelf(false)
		self.show = nil
	end
end

--价钱
function LogicGUI.TempDivinationItem:ShowCardPrice( type_ )

	self:RemoveCardPrice()

	self.price = Util:Sprite(Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "sp_divination_price" ))
    self.price:SetAnchorPoint( 0.5 , 0 )
    self.price:SetPosition( self:GetNode():GetContentWidth()/2 , 5 )
    self:AddChild( self.price , 12 )

	if type_ == Macros.Game.CARD_TYPE.LOW then

		local card = CallModuleFunc(ModuleType.DIVINATION, "GetLowBCardGroupInfo" )
		local num = card.today_low_card_group_open_card_times

		if num >= ConfigAdapter.Common.GetLowBDivinationFree() then
			local config = CPPGameLib.GetConfig("HuoBi", Macros.Game.RolePropType.GOLD )
			local tab = {
							{ tp = Resource.PathTool.GetItemPath( config.big_icon ) , dx = 0.4 },
							{ zi = ConfigAdapter.Common.GetLowBDivinationDraw() , jg = 5 },
						}
			local itemLst = nil
			self.item , itemLst = Util:LabCenter( tab )
			self.item:SetAnchorPoint( 0 , 0.5 )
			self.item:SetPosition( 5 , self.price:GetContentHeight()/2 )
			self.price:AddChild( self.item )
		else
			local tab = {
							{ tp = "" , dx = 0.4 },
							{ zi = CPPGameLib.GetString("hero_recruitFreeText") },
						}
			local itemLst = nil
			self.item , itemLst = Util:LabCenter( tab )
			self.item:SetAnchorPoint( 0.5 , 0.5 )
			self.item:SetPosition( self.price:GetContentWidth()/2 , self.price:GetContentHeight()/2 )
			self.price:AddChild( self.item )	
		end

	elseif type_ == Macros.Game.CARD_TYPE.TALL then

		local icon = ConfigAdapter.Common.GetTyrantDivinationDraw()

		local tab = {
						{ tp = Resource.PathTool.GetItemPath( ConfigAdapter.Common.GetTyrantDivinationDraw() ) , dx = 0.4 },
						{ zi = 1 , jg = 5 },
					}
		local item = Util:LabCenter( tab )
		item:SetAnchorPoint( 0 , 0.5 )
		item:SetPosition( 5 , self.price:GetContentHeight()/2 )
		self.price:AddChild( item )

	end
end

--设置价钱
function LogicGUI.TempDivinationItem:SetShowCardPrice( extra )

	extra = extra or 0
	if not self.price then
		return true
	end

	if self.item then
		self.item:RemoveSelf(true)
		self.item = nil
	end
	if self.type == Macros.Game.CARD_TYPE.LOW then
		local card = CallModuleFunc(ModuleType.DIVINATION, "GetLowBCardGroupInfo" )
		local num = card.today_low_card_group_open_card_times + extra

		if num >= ConfigAdapter.Common.GetLowBDivinationFree() then
			local config = CPPGameLib.GetConfig("HuoBi", Macros.Game.RolePropType.GOLD )
			local tab = {
							{ tp = Resource.PathTool.GetItemPath( config.big_icon ) , dx = 0.4 },
							{ zi = ConfigAdapter.Common.GetLowBDivinationDraw() , jg = 5 },
						}
			local itemLst = nil
			self.item , itemLst = Util:LabCenter( tab )
			self.item:SetAnchorPoint( 0 , 0.5 )
			self.item:SetPosition( 5 , self.price:GetContentHeight()/2 )
			self.price:AddChild( self.item )
		else
			local tab = {
							{ tp = "" , dx = 0.4 },
							{ zi = CPPGameLib.GetString("hero_recruitFreeText") },
						}
			local itemLst = nil
			self.item , itemLst = Util:LabCenter( tab )
			self.item:SetAnchorPoint( 0.5 , 0.5 )
			self.item:SetPosition( self.price:GetContentWidth()/2 , self.price:GetContentHeight()/2 )
			self.price:AddChild( self.item )
		end
	end	
end

--删除价钱
function LogicGUI.TempDivinationItem:RemoveCardPrice( )
	if self.item then
		self.item:RemoveSelf(true)
		self.item = nil
	end
	
	if self.price then
		self.price:RemoveSelf(true)
		self.price = nil
	end
end

