--
-- @brief 爬塔主界面(主界面)
-- @author: yjg
-- @date: 2016年9月26日11:00:01
--

Modules = Modules or {}

Modules.TowerView = Modules.TowerView or BaseClass(BaseView)

function Modules.TowerView:__init()
	self.__layout_name = "tower"  
	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bp_sp_newtower")}

	self.__title_bar_params = {
		title = CPPGameLib.GetString("tower"),
		show_bg = false,
		lucency_bg = true,
		show_diamond = true,
		show_gold = true,
		show_power = true,
		-- show_energy	= true,
		res_beg_offset_x = -90,
		res_unit_bg_imageinfo = {Resource.UITextureAtlasType.CITY, "bg_resource"},
		res_unit_title_offset_x = -16,
		close_func = function()
			--设置弹出密藏
			CallModuleFunc(ModuleType.TOWER,"SetShowAwardType" , true )
			self:CloseManual()
		end,
	}

    self.__open_callback = function()
    	self:__OpenCallback()
    end

    self:__AddPreloadTemplate(GUI.TemplateRegistry.TOWER_RANK)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.TOWER)
    self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.TOWER,true)
end

function Modules.TowerView:__OpenCallback()
	-- LogicGUI.TemplAffirmTips.New()
	self:__Getwidget()
	self:__ShowUI()
	self:__Server() 
end

function Modules.TowerView:__Dispose()
	if self.modes then
		self.modes:DeleteMe()
		self.modes = nil
	end

	if self.__loading_effect_id then
		self:__DelEffect(self.__loading_effect_id)
		self.__loading_effect_id = nil
	end	

end

function Modules.TowerView:__Getwidget()
	self.Panel_1 = self:GetWidget("Panel_1")

	self.Panel_terrace1 = self:GetWidget("Panel_terrace1")
	self.Panel_terrace2 = self:GetWidget("Panel_terrace2")
	self.Panel_terrace3 = self:GetWidget("Panel_terrace3")

	--神装商店	
	self.Button_shop = Util:WidgetButton( self:GetWidget("Button_shop") , nil , CPPGameLib.Handler(self,self.OnButtonShop) )
	self.Image_1 = self:GetWidget("Image_1")

	--精英挑战
	self.Button_elite = Util:WidgetButton( self:GetWidget("Button_elite") , nil , CPPGameLib.Handler(self,self.OnButtonElite) )
	self.Image_elite_1 = self:GetWidget("Image_elite_1")
	self.Image_lock = self:GetWidget("Image_lock")
	-- self.Image_lock:SetVisible(false)
	-- self.Button_elite:SetVisible(true)

	-- --按钮设置
	-- self:UpShowUI()

	--奖励
	self.Button_award = Util:WidgetButton( self:GetWidget("Button_award") , nil , CPPGameLib.Handler(self,self.OnButtonAward) )
	self.Image_1 = self:GetWidget("Image_1")
    self:GetWidget("Image_award"):IgnoreContentAdaptWithSize(true)

	self.Button_awardPos = {}
	self.Button_awardPos.x = self.Button_award:GetPositionX()
	self.Button_awardPos.y = self.Button_award:GetPositionY()
	
	--排行榜
	self.Button_ranking = Util:WidgetButton( self:GetWidget("Button_ranking") , nil , CPPGameLib.Handler(self,self.OnButtonRanking) )
	self.Image_1 = self:GetWidget("Image_1")
    self:GetWidget("Image_ranking"):IgnoreContentAdaptWithSize(true)
	
	--试炼
	self.Button_trial = Util:WidgetButton( self:GetWidget("Button_trial") , nil , CPPGameLib.Handler(self,self.OnButtonTrial) )
	self.Image_1 = self:GetWidget("Image_1")
	
	--条件背景
	self.Image_conditionBg = self:GetWidget("Image_conditionBg")
	--威望背景
	self.Image_prestigeBg = self:GetWidget("Image_prestigeBg")
	--可用背景
	self.Image_usable = self:GetWidget("Image_usable")	
	--资源背景
	self.Image_message = self:GetWidget("Image_message")
	--属性背景
	self.Image_property = self:GetWidget("Image_property")
	--重置按钮
	self.Button_reset = Util:WidgetButton( 
											self:GetWidget("Button_reset") , 
											CPPGameLib.GetString("dungeon_reset") , 
											CPPGameLib.Handler(self,self.OnButtonReset) ,
											nil,nil,Macros.Color.button_yellow
										)
	--扫荡按钮
	self.Button_sweep = Util:WidgetButton( 
											self:GetWidget("Button_sweep") , 
											CPPGameLib.GetString("dungeon_clean_title") , 
											CPPGameLib.Handler(self,self.OnButtonSweep) ,
											nil,nil,Macros.Color.button_yellow
										)

	--一键扫荡按钮
	self.Button_onekey_reset = Util:WidgetButton( 
											self:GetWidget("Button_onekey_reset") , 
											CPPGameLib.GetString("tower_onekey_reset") , 
											CPPGameLib.Handler(self,self.OnButtonOnekeyReset) ,
											nil,nil,Macros.Color.button_yellow
										)
	-- self.Button_onekey_reset:SetVisible(false)
	
	--失败界面
	self.Image_lose = self:GetWidget("Image_lose")
    self.Image_lose:SetImage(Resource.PathTool.GetUIResPath("citycommonslices/bg_sp9_lose_bigbg"), TextureResType.LOCAL)
	-- self.Image_lose:SetVisible(false)
	--失败提示
	self.Text_lose = self:GetWidget("Text_lose")

    --规则按钮
    local Button_rule = Util:WidgetButton( self:GetWidget("Button_rule") , nil , function ( ... )
        print("xy" , "(3)勇者试炼，大图标：" )
        local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
        help:SetLoadedCallback( function ( ... )
           help:SetCell("Rule_Trial")
        end )
    end )
    self:GetWidget("Image_rule"):IgnoreContentAdaptWithSize(true)

    --重置次数
    self.Image_reset = self:GetWidget("Image_reset")

    --隐藏所有变动控件
    self:SetHideAlterUI()
end

function Modules.TowerView:__Server()
	CallModuleFunc(ModuleType.TOWER , "FbTowerDataReq")

	--爬塔数据
	self:BindGlobalEvent(Macros.Event.TowerModule.TOWER_UP,function (_, data)
		self:SetShowUI()
		--新手指引检测
		self:CheckSectionOperateSP()
	end)

	--重置关卡响应
	self:BindGlobalEvent(Macros.Event.TowerModule.TOWER_RESET,function (_, data)
		--刷新buff
		self:OnReset()
	end)

	--扫荡
	self:BindGlobalEvent(Macros.Event.TowerModule.TOWER_SWEEP,function (_, data)
		--扫荡
		if CallModuleFunc(ModuleType.TOWER,"GetIsOneKeyClean") == "oneKey" then
			self:SetShowUI()
		else
			self:OnSweep()
		end
	end)

	--属性
	self:BindGlobalEvent(Macros.Event.TowerModule.TOWER_BUFF,function (_, data)
		--设置ui
		self:SetShowUI()
	end)

	--爬塔购买
    self:BindGlobalEvent(Macros.Event.TowerModule.TOWER_RECORD,function (_,data)
    	local index = CallModuleFunc(ModuleType.TOWER,"GetNowBuyNum")
    	if self.red_shop then
	    	if index ~= 0 then
	    		self.red_shop:SetVisible(true)
	    	else
	    		self.red_shop:SetVisible(false)
	    	end
	    end
    end) 

	--爬塔精英挑战次数
    self:BindGlobalEvent(Macros.Event.TowerModule.TOWER_ELITE_TIME,function (_,data)
    	local index = CallModuleFunc(ModuleType.TOWER,"GetEliteTime")
		local level = CallModuleFunc(ModuleType.TOWER , "GetLevelRecord")
		local cnf = CPPGameLib.GetConfig("TowerEliteMap", 1 )
		if index ~= 0 and level >= cnf.normal_map_level then
			print("xy" , "SetVisible" )
    		self.elite_red:SetVisible(true)
    	else
    		self.elite_red:SetVisible(false)
    	end
    end) 
end

function Modules.TowerView:__ShowUI()
	--威望lab
	local tower_prestige = Util:Name( CPPGameLib.GetString("tower_prestige") )
	tower_prestige:SetPosition( 10 , self.Image_prestigeBg:GetPositionY() )
	tower_prestige:SetAnchorPoint( 0 , 0.5 )
	self.Panel_1:AddChild( tower_prestige )

	--图标
	local prestigeSp = Util:Sprite( Util:GetMoneyIconImgData(Macros.Game.RolePropType.TOWER_PRESTIGE) )
	PosTool.RightTo(tower_prestige, prestigeSp , -5 )
	prestigeSp:SetScaleFactor(0.85)
	self.Panel_1:AddChild( prestigeSp )

	--威望
	self.prestige = Util:Name( "0" )
	PosTool.RightTo(prestigeSp, self.prestige)
	self.Panel_1:AddChild( self.prestige )

	self.reset_text = Util:Label( CPPGameLib.GetString("tower_residueResetTimes" , "0" ) )
	self.Image_reset:AddChild( self.reset_text )
	PosTool.Center( self.reset_text )

------------
	local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "star")

	--历史最高lab
	local tower_history_2 = Util:Label(CPPGameLib.GetString("tower_history_2"), Macros.TypefaceSize.largish  , Macros.Color.Special)
	tower_history_2:SetAnchorPoint( 0 , 0.5 )
	tower_history_2:SetPosition( 10 , self.Image_message:GetContentHeight()/2 + 20 )
	self.Image_message:AddChild(tower_history_2)

	--历史最高
	self.tower_history = Util:Label("0" , Macros.TypefaceSize.largish )
	PosTool.RightTo(tower_history_2, self.tower_history , -5 )
	self.Image_message:AddChild(self.tower_history)

	--图标
	self.tower_historySp = Util:Sprite( sp )
	PosTool.RightTo(self.tower_history, self.tower_historySp )
	self.Image_message:AddChild( self.tower_historySp )

------------
	--本次挑战lab
	local tower_the = Util:Label(CPPGameLib.GetString("tower_the"), Macros.TypefaceSize.largish  , Macros.Color.Special)
	tower_the:SetAnchorPoint( 0 , 0.5 )
	tower_the:SetPosition( 10 , self.Image_message:GetContentHeight()/2 - 20 )
	self.Image_message:AddChild(tower_the)

	--本次挑战
	self.tower_the = Util:Label("0" , Macros.TypefaceSize.largish )
	PosTool.RightTo(tower_the, self.tower_the , -5 )
	self.Image_message:AddChild(self.tower_the)

	--图标
	self.tower_theSp = Util:Sprite( sp )
	PosTool.RightTo(self.tower_the, self.tower_theSp )
	self.Image_message:AddChild( self.tower_theSp )

------------
	--可用lab
	local tower_curve_nature = Util:Name( CPPGameLib.GetString("tower_curve_nature") )
	tower_curve_nature:SetPosition( 10 , self.Image_usable:GetPositionY() )
	tower_curve_nature:SetAnchorPoint( 0 , 0.5 )
	self.Panel_1:AddChild( tower_curve_nature )

	--图标
	local tower_curve_natureSp = Util:Sprite( sp )
	PosTool.RightTo(tower_curve_nature, tower_curve_natureSp , -5 )
	self.Panel_1:AddChild( tower_curve_natureSp )

	--可用
	self.tower_curve_nature = Util:Name( "0" )
	PosTool.RightTo(tower_curve_natureSp, self.tower_curve_nature)
	self.Panel_1:AddChild( self.tower_curve_nature )

------------
	--累计属性
	local tower_addUp = Util:Label( CPPGameLib.GetString("tower_addUp") , Macros.TypefaceSize.largish  , Macros.Color.Special )
	tower_addUp:SetPosition( self.Image_property:GetContentWidth()/2 , self.Image_property:GetContentHeight() - (tower_addUp:GetContentHeight()/2) - 8 )
	tower_addUp:SetAnchorPoint(0.5,0.5)
	self.Image_property:AddChild(tower_addUp)

	local split_lineSp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "split_line")
	--图标
	self.split_line = Util:Sprite9( split_lineSp )
	self.split_line:SetAnchorPoint(0.5,0.5)
	self.split_line:SetContentSize( self.Image_property:GetContentWidth() - 10 , self.split_line:GetContentHeight() )
	self.split_line:SetPosition( self.Image_property:GetContentWidth()/2 , tower_addUp:GetPositionY() - 10 )
	self.Image_property:AddChild( self.split_line )

	--属性
	self.nature = {}
	self.natureValue = {}

	for i=1 , 2 do
		local y = self.split_line:GetPositionY() - 25 
		if self.nature[i-1] then
			y = self.nature[i-1]:GetPositionY() - 30 
		end

		self.nature[i] = Util:Name(" ")
		self.nature[i]:SetAnchorPoint(1,0.5)
		self.nature[i]:SetPosition(self.Image_property:GetContentWidth()/2 - 10 , y )
		self.Image_property:AddChild( self.nature[i] )

		self.natureValue[i] = Util:Label(" " , nil , Macros.Color.front_color)
		self.natureValue[i]:SetAnchorPoint(0,0.5)
		self.natureValue[i]:SetPosition(self.Image_property:GetContentWidth()/2  , y )
		self.Image_property:AddChild( self.natureValue[i] )
	end

------------
	--更多属性
	local tower_moreNature = Util:Text( 
										CPPGameLib.GetString("tower_moreNature") , 
										Macros.TypefaceSize.minimum , 
										Macros.Color.golden , function ( ... )
										end)
	tower_moreNature:SetAnchorPoint(1,0.5)
	tower_moreNature:SetPosition( self.Image_property:GetContentWidth() - 5 , tower_moreNature:GetContentHeight()/2 + 5 )
	self.Image_property:AddChild( tower_moreNature )

	local layout = Util:Layout( self.Image_property:GetContentWidth() , self.Image_property:GetContentHeight() ,function ( ... )
		self:OnMoreNature()
	end )
	layout:SetAnchorPoint(0.5, 0.5)
	self.Image_property:AddChild(layout)
	PosTool.Center( layout )

------------
	-- 第%s层通关条件
	self.tower_condition = Util:Label(CPPGameLib.GetString("tower_condition") , Macros.TypefaceSize.largish  , Macros.Color.Special ) 
	self.tower_condition:SetAnchorPoint( 0.5,0.5 )
	self.tower_condition:SetPosition( self.Image_conditionBg:GetContentWidth()/2 , self.Image_conditionBg:GetContentHeight()/2 + 15  )
	self.Image_conditionBg:AddChild(self.tower_condition)

------------
	--失败提示文字
	self.tower_encourageTips = Util:Label( 
											CPPGameLib.GetString("tower_encourageTips"),
											Macros.TypefaceSize.normal,
											Macros.Color.lilyWhite , 225
											)
	self.tower_encourageTips:SetAnchorPoint(0.5,0.5)
	self.tower_encourageTips:SetTextHorizontalAlign(LabelEx.ALIGIN_CENTER)
	self.Text_lose:AddChild(self.tower_encourageTips)
	PosTool.Center(self.tower_encourageTips)


	local info = CallModuleFunc(ModuleType.TOWER,"GetbattleData")
	if info and info[3] and info[3].type == Macros.Game.FBType.TOWER_ELITE then
		print("xy" , ":===")
		self:OnButtonElite()
		CallModuleFunc(ModuleType.BATTLE, "__FirstWinAwardConvert" , info[3] )
	end

------------
	--红点
	--精英红点
	local red_image = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
	self.elite_red = Util:Sprite( red_image )
	self.elite_red:SetAnchorPoint(0.5,0.5)
	self.elite_red:SetPosition( self.Button_elite:GetContentWidth() , self.Button_elite:GetContentHeight() )
	self.Button_elite:AddChild( self.elite_red )
	self.elite_red:SetVisible(false)

	--奖励购买红点
	self.red_shop = Util:Sprite( red_image )
	self.red_shop:SetAnchorPoint(0.5,0.5)
	self.red_shop:SetPosition( self.Button_shop:GetContentWidth() , self.Button_shop:GetContentHeight() )
	self.red_shop:SetVisible(false)
	self.Button_shop:AddChild( self.red_shop )
	self.Button_shop:SetVisible(GlobalModulesMgr:IsOpen(Macros.Game.SystemID.TOWER_SHOP))
end
