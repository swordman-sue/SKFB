
--
-- @brief 转盘积分排行
-- @author: yjg
-- @date: 2018年3月17日17:57:55
--

LogicGUI = LogicGUI or {}

LogicGUI.TempTurntableRank = LogicGUI.TempTurntableRank or BaseClass(GUI.Template)

function LogicGUI.TempTurntableRank:__init(params)
	-- self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__is_auto_load = true
	self.__open_callback = function()
	    self:__Getwidget()
	    self:__ShowUI()
	end
	-- self.__bg_params.close_func = function ( ... )
	-- 	self:DeleteMe()
	-- end

	self:InitTemplate(GUI.TemplateRegistry.TEMPL_TURNTABLE_RANK)	
end

function LogicGUI.TempTurntableRank:__Getwidget()

	self.Image_center = self:GetWidget("Image_center")

	self.Image_light = self:GetWidget("Image_light")

	self.Image_content = self:GetWidget("Image_content")

	self.Image_hint = self:GetWidget("Image_hint")
	self.Image_hint:SetVisible(false)

	--关闭按钮
	self.btn_close = Util:WidgetButton( self:GetWidget("Button_close") , " " , CPPGameLib.Handler(self,self.OnBtnClose) )
end

function LogicGUI.TempTurntableRank:__ShowUI()
	--标题
	local headline = Util:Label( CPPGameLib.GetString("chasm_rank") , Macros.TypefaceSize.popup , Macros.Color.white )
	headline:SetAnchorPoint( 0.5 , 0.5 )
	self.Image_light:AddChild( headline )
	PosTool.Center( headline )

	--我的排名
	local worldBoss_MyRank = Util:Label( CPPGameLib.GetString("worldBoss_MyRank") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
	worldBoss_MyRank:SetAnchorPoint( 0 , 0.5 )
	worldBoss_MyRank:SetPosition( 15 , 20 )
	self.Image_center:AddChild( worldBoss_MyRank )

	self.myRank = Util:Label( "" , Macros.TypefaceSize.normal , Macros.Color.content )
	self.Image_center:AddChild( self.myRank )
	PosTool.RightTo( worldBoss_MyRank , self.myRank )

	--排名奖励
	local arena_rankEncourageText = Util:Label( CPPGameLib.GetString("service_award") , Macros.TypefaceSize.normal , Macros.Color.content )
	arena_rankEncourageText:SetAnchorPoint( 0 , 0.5 )
	arena_rankEncourageText:SetPosition( 300 , 20 )
	self.Image_center:AddChild( arena_rankEncourageText )

	--列表
	local params =  
	{
	    item_class = Modules.TempTurntableRankItem,
	    item_width = self.Image_content:GetContentWidth() - 5 ,
	    item_height = 125 ,
	    row = 4,
	    col = 1,
	    view_height = self.Image_content:GetContentHeight() - 10 ,
	}
	-- 创建参数
	self.lst_list = GUI.ScrollView.New(params)
	self.Image_content:AddChild(self.lst_list:GetNode())
	PosTool.Center(self.lst_list:GetNode())

	local hint = Util:Label(CPPGameLib.GetString("monster_stormcastle_no_rank"), Macros.TypefaceSize.normal, Macros.Color.headline )
	hint:SetAnchorPoint( 0.5 , 0.5 )
	hint:SetPosition( 310 , 165 )
	self.Image_hint:AddChild( hint )

	self:__SetShowUI()
end

--设置UI
function LogicGUI.TempTurntableRank:__SetShowUI()
	--我的排名
	local myRank = CallModuleFunc(ModuleType.TURNTABLE,"GetMyRank")
	self.myRank:SetString( myRank )

	-- --排名奖励
	-- local rankAward = CallModuleFunc(ModuleType.TURNTABLE,"GetMyRankAward")
	-- if not rankAward then
	-- 	self.settlement_time:SetString( CPPGameLib.GetString("arena_notInRankListText") )
	-- else
	-- 	self.settlement_time:SetString( rankAward )
	-- end

	local rank = CallModuleFunc(ModuleType.TURNTABLE,"GetRank")
	if #rank == 0 then
		self.Image_hint:SetVisible(true)
	end
	self.lst_list:SetDataList( rank )

end

--关闭
function LogicGUI.TempTurntableRank:OnBtnClose()
	self:DeleteMe()
end


Modules = Modules or {}
Modules.TempTurntableRankItem = Modules.TempTurntableRankItem or BaseClass(GUI.ScrollItem)

function Modules.TempTurntableRankItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TempTurntableRankItem:__CreateWidgetFromLuaCode()  
    self.layout = Util:Layout(585 , 125)
    self.layout:SetTouchEnabled(false)
    return self.layout
end

function Modules.TempTurntableRankItem:__delete()
end

function Modules.TempTurntableRankItem:SetData( data )
    GUI.ScrollItem.SetData(self, data)
    self:__ShowUI(data)
end

function Modules.TempTurntableRankItem:__ShowUI( data )

    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_label_frame")
    local sp_2 = Util:Sprite9(sp)
    sp_2:SetContentSize(self.layout:GetContentWidth()  , sp_2:GetContentHeight() )
    sp_2:SetPosition(self.layout:GetContentWidth() / 2 , self.layout:GetContentHeight() /2)
    self.layout:AddChild(sp_2)

    --区间
    if data.rank <= 3 then
        local imageData = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"com_sp_rank_num_"..data.rank)
        local icon = Util:Sprite(imageData)
        icon:SetAnchorPoint(0.5,0.5)
        icon:SetPosition( 65 , self.layout:GetContentHeight() / 2)
        self.layout:AddChild(icon)
    else
        local iconValue = Util:Label(data.rank,nil, Macros.Color.headline ) 
        iconValue:SetPosition( 65 , self.layout:GetContentHeight() / 2)
        self.layout:AddChild(iconValue)  
    end

    --信息背景
    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp_equip_attribute")
    local bg_sp_equip_attribute = Util:Sprite9(sp)
    bg_sp_equip_attribute:SetContentSize( 300 , sp_2:GetContentHeight() - 25 )
    bg_sp_equip_attribute:SetPosition( sp_2:GetContentWidth() / 2 - 10 , sp_2:GetContentHeight() /2)
    sp_2:AddChild(bg_sp_equip_attribute) 

	--图标
	local icon_layer = LogicGUI.RoleIcon.New(true)
	icon_layer:SetData( data.icon )
	icon_layer:SetPosition( 20 , bg_sp_equip_attribute:GetContentHeight()/2)
	icon_layer:SetAnchorPoint( 0 ,0.5)
	icon_layer:SetScaleFactor(0.95)
	bg_sp_equip_attribute:AddChild(icon_layer:GetNode())  

	--名字
	local name = Util:Label( data.name , Macros.TypefaceSize.normal , Macros.Color.headline )
	name:SetAnchorPoint( 0 , 0.5 )
	name:SetPosition( 
						( icon_layer:GetNode():GetContentWidth() + icon_layer:GetNode():GetPositionX() + 30 ) ,
						( (icon_layer:GetNode():GetContentHeight()/2) + icon_layer:GetNode():GetPositionY() - 5 )
					 )
	bg_sp_equip_attribute:AddChild( name )

	--等级
	local levelText = Util:Label( CPPGameLib.GetString("common_lev1") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
	levelText:SetAnchorPoint( 0 , 0.5 )
	levelText:SetPosition( 
							name:GetPositionX() ,
							icon_layer:GetNode():GetPositionY()
						 )
	bg_sp_equip_attribute:AddChild( levelText )

	--等级
	local level = Util:Label( data.level , Macros.TypefaceSize.normal , Macros.Color.content )
	bg_sp_equip_attribute:AddChild( level )
	PosTool.RightTo( levelText , level )

	--战力
	local powerText = Util:Label( CPPGameLib.GetString("zhandouli") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
	powerText:SetAnchorPoint( 0 , 0.5 )
	powerText:SetPosition( 
							name:GetPositionX() ,
							( icon_layer:GetNode():GetPositionY() - (icon_layer:GetNode():GetContentHeight()/2) + 5 )
						 )
	bg_sp_equip_attribute:AddChild( powerText )

	--战力
	local power = Util:Label( data.power , Macros.TypefaceSize.normal , Macros.Color.content )
	bg_sp_equip_attribute:AddChild( power )
	PosTool.RightTo( powerText , power )

	--积分背景
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_hero_name2")
	local bg_hero_name2 = Util:Sprite(sp)
	bg_hero_name2:SetPosition(self.layout:GetContentWidth() / 2 + 215 , self.layout:GetContentHeight() /2 - 15 )
	self.layout:AddChild(bg_hero_name2)

	local integralText = Util:Label( CPPGameLib.GetString("ladder_integral") , Macros.TypefaceSize.normal , Macros.Color.headline )
	bg_hero_name2:AddChild( integralText )
	PosTool.Center( integralText , 0 , 35 )

	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NUMBER_NEW, "power_num", true)
	local integral = GUI.ImgFont.New(sp)	
	integral:SetAnchorPoint(0.5 , 0.5)
	bg_hero_name2:AddChild(integral:GetNode())
	PosTool.Center( integral:GetNode() )
	integral:SetText(tostring(data.integral))
end