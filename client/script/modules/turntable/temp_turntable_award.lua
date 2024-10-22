
--
-- @brief 转盘积分奖励
-- @author: yjg
-- @date: 2018年3月17日16:22:48
--

LogicGUI = LogicGUI or {}

LogicGUI.TempTurntableAward = LogicGUI.TempTurntableAward or BaseClass(GUI.Template)

function LogicGUI.TempTurntableAward:__init(params)
	self.__bg_params = {type = BaseView.BGType.SOLID}
	self.__is_auto_load = true
	self.__open_callback = function()
	    self:__Getwidget()
	    self:__ShowUI()
	    self:__Server()
	end
	-- self.__bg_params.close_func = function ( ... )
	-- 	self:DeleteMe()
	-- end
-- 
	self:InitTemplate(GUI.TemplateRegistry.TEMPL_TURNTABLE_AWARD)	
end

function LogicGUI.TempTurntableAward:__Server()
    CallModuleFunc(ModuleType.TURNTABLE,"TurntableRankReq")
    self:BindGlobalEvent(Macros.Event.RankModule.RANK_LIST_REQ , function (  )
        self:__SetShowUI( )
    end)
end

function LogicGUI.TempTurntableAward:__Getwidget()

	self.Image_bg = self:GetWidget("Image_bg")

	self.Image_1 = self:GetWidget("Image_1")

	self.Image_lstBg = self:GetWidget("Image_lstBg")

	--关闭按钮
	self.btn_close = Util:WidgetButton( self:GetWidget("Button_close") , " " , CPPGameLib.Handler(self,self.OnBtnClose) )

	--查看排行
	self.Button_no = Util:WidgetButton( 
										self:GetWidget("Button_no") , 
										CPPGameLib.GetString("rank_activity_check") , 
										CPPGameLib.Handler(self,self.OnButtonNo) ,
										Macros.TypefaceSize.popup , 
										Macros.Color.lilyWhite , 
										Macros.Color.button_yellow
									)
	--列表
	self.Panel_lst = self:GetWidget("Panel_lst")

	--标题1
	self.Image_headline_1 = self:GetWidget("Image_headline_1")
	--标题2
	self.Image_headline_2 = self:GetWidget("Image_headline_2")
end

function LogicGUI.TempTurntableAward:__ShowUI()
	--标题
	local headline = Util:Label( CPPGameLib.GetString("turntable_award") , Macros.TypefaceSize.popup , Macros.Color.white )
	headline:SetAnchorPoint( 0.5 , 0.5 )
	self.Image_1:AddChild( headline )
	PosTool.Center( headline )

	--标题1
	local arena_rankZoom = Util:Label( CPPGameLib.GetString("arena_rankZoom") , Macros.TypefaceSize.popup , Macros.Color.white )
	arena_rankZoom:SetAnchorPoint( 0.5 , 0.5 )
	self.Image_headline_1:AddChild( arena_rankZoom )
	PosTool.Center( arena_rankZoom )

	--标题2
	local arena_rankEncourage = Util:Label( CPPGameLib.GetString("arena_rankEncourage") , Macros.TypefaceSize.popup , Macros.Color.white )
	arena_rankEncourage:SetAnchorPoint( 0.5 , 0.5 )
	self.Image_headline_2:AddChild( arena_rankEncourage )
	PosTool.Center( arena_rankEncourage )

	--我的排名
	local worldBoss_MyRank = Util:Label( CPPGameLib.GetString("worldBoss_MyRank") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
	worldBoss_MyRank:SetAnchorPoint( 0 , 0.5 )
	worldBoss_MyRank:SetPosition( 15 , 50 )
	self.Image_bg:AddChild( worldBoss_MyRank )

	self.myRank = Util:Label( "" , Macros.TypefaceSize.normal , Macros.Color.content )
	self.Image_bg:AddChild( self.myRank )
	PosTool.RightTo( worldBoss_MyRank , self.myRank )

	--榜单结算时间
	local rank_settlement_time = Util:Label( CPPGameLib.GetString("rank_settlement_time") , Macros.TypefaceSize.normal , Macros.Color.keypoint )
	rank_settlement_time:SetAnchorPoint( 0 , 0.5 )
	rank_settlement_time:SetPosition( 15 , 20 )
	self.Image_bg:AddChild( rank_settlement_time )

	self.settlement_time = Util:Label( "" , Macros.TypefaceSize.normal , Macros.Color.content )
	self.Image_bg:AddChild( self.settlement_time )
	PosTool.RightTo( rank_settlement_time , self.settlement_time )

	--入榜提示
	local cnf = CallModuleFunc(ModuleType.TURNTABLE,"GetTurntableCnf")
	if not cnf then
		return 
	end
	local turntable_rankTips = Util:Label( CPPGameLib.GetString("turntable_rankTips" , cnf.rank_need_score) , Macros.TypefaceSize.normal , Macros.Color.keypoint )
	turntable_rankTips:SetAnchorPoint( 0 , 0.5 )
	turntable_rankTips:SetPosition( 220 , 50 )
	self.Image_bg:AddChild( turntable_rankTips )

	--列表
	local params =  
	{
	    item_class = Modules.TempTurntableAwardItem,
	    item_width = self.Panel_lst:GetContentWidth() ,
	    item_height = 85,
	    row = 4,
	    col = 1,
	    view_height = self.Panel_lst:GetContentHeight() ,
	}
	-- 创建参数
	self.lst_list = GUI.ScrollView.New(params)
	self.Panel_lst:AddChild(self.lst_list:GetNode())
	PosTool.Center(self.lst_list:GetNode())

	self:__SetShowUI()
end

--设置UI
function LogicGUI.TempTurntableAward:__SetShowUI()
	--我的排名
	
	local myRank = CallModuleFunc(ModuleType.TURNTABLE,"GetMyRank")
	if not myRank or myRank <= 0 then
		self.myRank:SetString( CPPGameLib.GetString("rebel_meimingci") )
	else
		self.myRank:SetString( CPPGameLib.GetString("rebel_mingci_1" , myRank ) )
	end

	--结算时间
	local endTime = CallModuleFunc(ModuleType.TURNTABLE,"GetActivityEndTime")
	if endTime then
        local year, month, day, hour, minute, second, week = TimeTool.GetDateFromTimeStamp( endTime )
        local over = year .."." .. month .."." .. day  .." " .. hour ..":" ..  minute
		self.settlement_time:SetString( over )
	else
		self.settlement_time:SetString( CPPGameLib.GetString("monster_stormcastle_haveendtxt") )
	end

	local lst = CallModuleFunc(ModuleType.TURNTABLE,"GetTurntableRankCnf")
	self.lst_list:SetDataList( lst )
end

--关闭
function LogicGUI.TempTurntableAward:OnBtnClose()
	self:DeleteMe()
end

--查看排行
function LogicGUI.TempTurntableAward:OnButtonNo()
	LogicGUI.TempTurntableRank.New()
end


Modules = Modules or {}
Modules.TempTurntableAwardItem = Modules.TempTurntableAwardItem or BaseClass(GUI.ScrollItem)

function Modules.TempTurntableAwardItem:__init()
    self.__layout_mode = GUI.Template.LayoutMode.CODE
    self:InitTemplate(nil)  
end

function Modules.TempTurntableAwardItem:__CreateWidgetFromLuaCode()  
    self.layout = Util:Layout(585 , 85)
    self.layout:SetTouchEnabled(false)
    return self.layout
end

function Modules.TempTurntableAwardItem:__delete()
end

function Modules.TempTurntableAwardItem:SetData( data )
    GUI.ScrollItem.SetData(self, data)
    self:__ShowUI(data)
end

function Modules.TempTurntableAwardItem:__ShowUI( data )
    local sp = nil
    if data.show == true then
        sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_sp9_lst_rankdeep")
        local sp_2 = Util:Sprite9(sp)
        sp_2:SetContentSize(self.layout:GetContentWidth()  , self.layout:GetContentHeight() )
        sp_2:SetPosition(self.layout:GetContentWidth() / 2 , self.layout:GetContentHeight() /2)
        self.layout:AddChild(sp_2)
    end

    if not data.rank then
        return
    end

    --区间
    if tonumber(data.rank) and tonumber(data.rank) <= 3 then
        local imageData = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW,"com_sp_rank_num_"..data.rank)
        local icon = Util:Sprite(imageData)
        icon:SetAnchorPoint(0.5,0.5)
        icon:SetPosition( 90 , self.layout:GetContentHeight() / 2)
        self.layout:AddChild(icon)
    else
        local iconValue = Util:Label(data.rank,nil,Macros.Color.headline)
        iconValue:SetPosition( 90 , self.layout:GetContentHeight() / 2)
        self.layout:AddChild(iconValue)  
    end

    --奖励
    for i , v in ipairs( data.award ) do
        --图标
        local icon_layer = LogicGUI.IconLayer.New(true)
        icon_layer:SetData({id = v.id , num = v.num })
        icon_layer:SetPosition( 210 + (90 * (i - 1))  , self.layout:GetContentHeight()/2)
        icon_layer:SetAnchorPoint(0,0.5)
        icon_layer:SetScaleFactor(0.95)
        icon_layer:SetTouchSwallow(false)
        self.layout:AddChild(icon_layer:GetNode())  
    end

end