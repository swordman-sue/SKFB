
--
-- @brief 公会图标墙
-- @author: yjg
-- @date: 2016年10月18日19:44:36
--

LogicGUI = LogicGUI or {}

LogicGUI.TemplUnionWall = LogicGUI.TemplUnionWall or BaseClass(GUI.Template)

function LogicGUI.TemplUnionWall:__init()
    self.__is_auto_load = true
    self.__bg_params = {type = BaseView.BGType.SOLID} 

    self.__tab_params_ex = {6}

    self.__open_callback = function ()
		self.onIndex = nil
		self.onIconTips = nil
        self:__Getwidget()
        self:__ShowUI()
    end 

	self:InitTemplate(GUI.TemplateRegistry.UNION_ICON_WALL)	
end


function LogicGUI.TemplUnionWall:__delete()
end

function LogicGUI.TemplUnionWall:__Getwidget( ... )
	--背景
	self.Image_bg = self:GetWidget("Image_bg")
	--光
	self.Image_light = self:GetWidget("Image_light")	
	--关闭
	self.Button_close = Util:WidgetButton( 
                                            self:GetWidget("Button_close") , 
                                            nil , CPPGameLib.Handler(self,self.OnButtonClose)
                                        )
	--内容框
	self.Image_content = self:GetWidget("Image_content")	
	--确定
	self.Button_confirm = Util:WidgetButton( 
                                            self:GetWidget("Button_confirm") , 
                                            CPPGameLib.GetString("common_confirm") , CPPGameLib.Handler(self,self.OnButtonConfirm),
                                            nil , nil , Macros.Color.button_yellow
                                        )
end

function LogicGUI.TemplUnionWall:__ShowUI( ... )
	local union_iconWall = Util:Label( CPPGameLib.GetString("union_iconWall") , Macros.TypefaceSize.popup , Macros.Color.white )	
	union_iconWall:SetAnchorPoint( 0.5 , 0.5 )
	self.Image_light:AddChild( union_iconWall )
	PosTool.Center( union_iconWall )

	local lst = {}
	for i,v in ipairs(CPPGameLib.GetConfig("GuildIcon", nil, nil , true )) do
		table.insert( lst , { id = i , need_rank = v.need_rank } )
	end
	lst = Util:SegmentationTab( lst , 4 )

    self.__tab:SetContentSize( self.Image_content:GetContentWidth() , self.Image_content:GetContentHeight())
    self.__tab:SetData( lst )
    self.__tab:SetCeil()
    self.__tab:SetAnchorPoint( 0.5 , 0.5 )
    self.Image_content:AddChild( self.__tab:GetNode() )
    PosTool.Center( self.__tab )

end

function LogicGUI.TemplUnionWall:Ceil( index , date )
	local layer = Util:Layout( self.Image_content:GetContentWidth() , self.Image_content:GetContentHeight()/2 )

	local interval = layer:GetContentWidth() / 4

	for i,v in ipairs(date) do
		local layou = nil
		layou = Util:Layout( layer:GetContentWidth()/4 , layer:GetContentHeight() , function ( ... )
			self:OnIcon( v , layou )
		end)
		layou:SetAnchorPoint( 0.5 , 0.5 )
		layou:SetPosition( (layou:GetContentWidth()/2) + interval * (i-1) , layer:GetContentHeight()/2 + 30 )
		layer:AddChild(layou)

	    local icon_ = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.UNION, "union_icon".. v.id )
	    local icon = Util:Sprite( icon_ )
	    icon:SetAnchorPoint( 0.5 , 0.5 )
		icon:SetPosition( layou:GetContentWidth()/2 , layou:GetContentHeight()/2 )
		layou:AddChild(icon)

		local tips = Util:Name( CPPGameLib.GetString("union_iconTips" , v.need_rank) , Macros.TypefaceSize.slightly , nil , nil , layou:GetContentWidth()  )
		tips:SetAnchorPoint( 0.5 , 1 )
		tips:SetPosition( layou:GetContentWidth()/2 , icon:GetPositionY() - (icon:GetContentWidth()/2) - 5 )
		layou:AddChild( tips )
	end

	return layer
end

--选中图标
function LogicGUI.TemplUnionWall:OnIcon( data , icon )	
	if data.need_rank > CallModuleFunc(ModuleType.UNION , "GetUnionLevel") then
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_iconNoTips") )
	else
		self.onIndex = data.id
		if self.onIconTips then
			self.onIconTips:RemoveSelf(true)
			self.onIconTips = nil
		end
		local btn_cbox_select_yes = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BTN_NEW, "btn_cbox_select_yes" )
		self.onIconTips = Util:Sprite( btn_cbox_select_yes )
		self.onIconTips:SetAnchorPoint( 0.5 , 0.5 )
		icon:AddChild( self.onIconTips )
		PosTool.Center( self.onIconTips )
	end
end

-- 关闭按钮
function LogicGUI.TemplUnionWall:OnButtonClose( ... )	
	self:DeleteMe()
end

-- 确定按钮
function LogicGUI.TemplUnionWall:OnButtonConfirm( ... )	
	if self.onIndex then
		local info = {}
		info.type = Macros.Game.UNION.ICON
		info.icon_id = self.onIndex
		CallModuleFunc(ModuleType.UNION , "GuildInfoModifyReq",info)
		self:DeleteMe()
	else
		GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("union_iconNoOnIconTips") )
	end
end
