
--
-- @brief 叛军界面
-- @author: yjg
-- @date: 2016年9月19日17:44:49
--

Modules = Modules or {}

Modules.RebelView = Modules.RebelView or BaseClass(BaseView)

function Modules.RebelView:__init()
	self.__layout_name = "rebel"  
	self.__bg_params = {type = BaseView.BGType.IMAGE, image_path = Resource.PathTool.GetUIResPath("bigbg/bg_sp_panjun")}
	self.__title_bar_params = 
	{
		title = CPPGameLib.GetString("rebel"),
		show_diamond = true,
		show_gold = true,
		show_stamina = true,
		show_energy = true,
		close_func = function()
			self:CloseManual()
		end,
	}

    self.__open_callback = function()
    	self:__OpenCallback()
    end

	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.REBEL)
	self:__AddPreloadTextureAtlas(Resource.UITextureAtlasType.REBEL,true)
end

function Modules.RebelView:__OpenCallback()  
	self.btn = {} 
	self.lst = {}	
	self:__Getwidget()
	self:__ShowUI()

	self:Countdown()

	self:IsCombat()
	--请求奖励记录列表
	CallModuleFunc(ModuleType.REBEL , "RebelExploitAwardRecordReq")
	--奖励记录
	self:BindGlobalEvent(Macros.Event.RebelModule.REBEL_AWARD_RECORD,function (_, data)
		self:SetRed()
	end)
end

function Modules.RebelView:__RegistAllEvents()
	self:BindGlobalEvent(Macros.Event.RebelModule.REBEL_DATA_RESP, function()
	    self:ShowLst()
	    self:ShowInfo()
	end)	
end

function Modules.RebelView:__Dispose()
	for i=1,#self.lst do
		if self.lst[i].modes then
			self.lst[i].modes:DeleteMe()
			self.lst[i].modes = nil
		end
	end
end

function Modules.RebelView:__Getwidget( ... )
	--怪物层
    self.Panel_center  = self:GetWidget("Panel_center") 
    self.Panel_center:SetVisible(false)
    --看板娘
    self.Image_hero = self:GetWidget("Image_hero"):SetImage(Resource.PathTool.GetHalfHeadPath("guide"),TextureResType.LOCAL)
    --对话版
    self.Image_guestBook = self:GetWidget("Image_guestBook") 
    --信息榜
    self.Image_notice = self:GetWidget("Image_notice") 
    --货币板
    self.Image_currency = self:GetWidget("Image_currency") 
    --活动提示
    self.Image_tips = self:GetWidget("Image_tips") 
	self.Image_tips:SetVisible( false )

    --帮助
    local Button_rule = Util:WidgetButton( self:GetWidget("Button_rule") , nil , function ( ... )
        print("xy" , "(6)异度入侵，大图标：" )
        local help = GUITemplateMgr:CreateTemplateInstance(GUI.TemplateRegistry.TEMPL_RULE , true )
        help:SetLoadedCallback( function ( ... )
           help:SetCell("Rule_Invade")
        end )
    end )
    Button_rule:SetZOrder(100)


	local txt = {
		[2] = { zi = CPPGameLib.GetString("rebel_rank"), sp = "btn_rank" },
		[3] = { zi = CPPGameLib.GetString("rebel_work_award"), sp = "btn_store"},
		[4] = { zi = CPPGameLib.GetString("rebel_shop"), sp = "btn_store"},
	}
	for i=2,4 do
		self.btn[i] = self:GetWidget("btn_"..i)
		self.btn[i]:SetNormalImage( Resource.PathTool.GetSystemEntryIconPath(txt[i].sp) , TextureResType.LOCAL )
		WidgetUnity.SetWidgetListener(self.btn[i],nil,nil,function ( ... )
			self:OnBtn(i)
		end)

		local txt = Util:RichText(
									string.format(Macros.Color.dungeon_chapter_name_rt_gradient, txt[i].zi),
									Macros.TypefaceSize.normal , nil, nil, nil, Macros.Color.white_stroke_hex, 4
								)
		txt:SetPosition(self.btn[i]:GetContentWidth()/2, - 5)
		txt:SetAnchorPoint(0.5,0.5)
		self.btn[i] :AddChild(txt)
	end
end

function Modules.RebelView:__ShowUI( ... )
	--累计功勋
	local txt = Util:Name(CPPGameLib.GetString("rebel_leijigongxun"),Macros.TypefaceSize.slightly)
	txt:SetAnchorPoint(0,0.5)
	txt:SetPosition(25,self.Image_notice:GetContentHeight() - 20)
	self.Image_notice :AddChild(txt)

	self.merit = Util:Name("0",Macros.TypefaceSize.slightly)
	PosTool.RightTo(txt,self.merit)
	self.Image_notice:AddChild(self.merit)

	self.merit_rank = Util:Name(CPPGameLib.GetString("rebel_meimingci"),Macros.TypefaceSize.slightly)
	PosTool.RightTo(self.merit,self.merit_rank)
	self.Image_notice:AddChild(self.merit_rank)

	--累计伤害
	local txt_1 = Util:Name(CPPGameLib.GetString("rebel_leijishanghai"),Macros.TypefaceSize.slightly)
	txt_1:SetPosition(25, 20)
	txt_1:SetAnchorPoint(0,0.5)
	self.Image_notice :AddChild(txt_1)

	self.harm = Util:Name("0",Macros.TypefaceSize.slightly)
	PosTool.RightTo(txt_1,self.harm)
	self.Image_notice:AddChild(self.harm)

	self.harm_rank = Util:Name(CPPGameLib.GetString("rebel_meimingci"),Macros.TypefaceSize.slightly)
	PosTool.RightTo(self.harm,self.harm_rank , 3 )
	self.Image_notice:AddChild(self.harm_rank)

	-- --跳转提示
	self.tips = Util:Label( CPPGameLib.GetString("rebel_toTips") , nil ,Macros.Color.brown , 295 )
	self.tips:SetAnchorPoint(0,0.5)
	self.tips:SetPosition( 40 , self.Image_guestBook:GetContentHeight()/2 )
	self.Image_guestBook:AddChild(self.tips)

	-- 跳转按钮
	self.skip = Util:Text(CPPGameLib.GetString("rebel_skip"),nil, Macros.Color.btn_1 ,function ( ... )
		CallModuleFunc(ModuleType.DUNGEON,"OpenView",BaseViewType.DUNGEON)
	end )
	self.skip:SetStrokeColor(unpack( Macros.Color.system_headline_stroke ))
	self.skip:SetAnchorPoint(0.5,0.5)
	self.skip:SetPosition(self.Image_guestBook:GetContentWidth() - self.skip:GetContentWidth()/2 - 40 , 30 )
	self.Image_guestBook:AddChild(self.skip) 

	--怪物摆放位置
	self.layou = Util:Layout()
    self.layou:SetContentSize(0,0)
    self.layou:SetAnchorPoint(0.5,0.5)
    self:AddChild(self.layou)
    PosTool.Center(self.layou , 0 , 30 )

	--拥有货币
	local userDate = CallModuleFunc(ModuleType.ROLE , "GetRoleInfo")
    local diamond = Util:LabCenter({
								{ tp = Util:GetMoneyIconImgData( Macros.Game.RolePropType.REBEL_VALUE ) , dx = 0.8 },
							    { zi = CPPGameLib.GetString("rebel_exploit") , mb = true , dx = Macros.TypefaceSize.minimum},
							    { zi = userDate.rebel_value , mb = true , dx = Macros.TypefaceSize.minimum},
							})
    self.Image_currency:AddChild(diamond)
    PosTool.Center(diamond)



    local sp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
    self.icon_red_point_4 = Util:Sprite(sp)
    self.icon_red_point_4:SetPosition(self.btn[4]:GetContentWidth() - 5 , self.btn[4]:GetContentHeight() - 5 )
    self.btn[4]:AddChild(self.icon_red_point_4)
    self.icon_red_point_4:SetVisible(false)

    self:ShowLst()
    self:ShowInfo()
end

function Modules.RebelView:OnBtn( index )
	if index == 1 then
	    -- LogicGUI.TemplRecommend.New()
	    -- CallModuleFunc(ModuleType.FRIEND, "FriendRecommendListRequest")
	elseif index == 2 then
		LogicGUI.TemplRebelRank.New()

	elseif index == 3 then
		LogicGUI.TemplRebelAward.New(function ( ... )
			self:SetRed()
		end)

	elseif index == 4 then
		-- LogicGUI.TemplRebelShop.New()
		CallModuleFunc(ModuleType.REBEL, "OpenView", BaseViewType.REBELSHOP)

	end
end

function Modules.RebelView:SetRed( ... )
	if self.icon_red_point then
		self.icon_red_point:RemoveSelf(true)
		self.icon_red_point = nil
	end
    local num = CallModuleFunc(ModuleType.REBEL , "GetNoeAwardRed")
    if num ~= 0 then
        local icon_red_point = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_NEW, "icon_red_point")
        self.icon_red_point = Util:Sprite(icon_red_point)
        self.icon_red_point:SetPosition( self.btn[3]:GetContentWidth() - 10 ,  self.btn[3]:GetContentHeight() - 10 )
        self.btn[3]:AddChild(self.icon_red_point , 20)
    end
end

--显示叛军列表
function Modules.RebelView:ShowLst( ... )
	for i=1,10 do
		if self.lst[i] and self.lst[i].layou then
			self.lst[i].layou:RemoveSelf(true)
			self.lst[i].layou = nil

			self.lst[i].bar:RemoveSelf(true)
			self.lst[i].bar = nil

			self.lst[i].heidi:RemoveSelf(true)
			self.lst[i].heidi = nil

			self.lst[i].modes:DeleteMe()
			self.lst[i].modes = nil
		else
			break
		end
	end

	local lst = CallModuleFunc(ModuleType.REBEL, "RebelLst")
	if #lst <= 0 then
		self.Panel_center:SetVisible(true)
	else
		self.Panel_center:SetVisible(false)
	end

	self.layou:SetContentSize( #lst * 213  , 250 )

	local heidisp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.CITY_COMMON_BG_NEW, "bg_hero_shadow")
	local hpBG = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_PROGRESS, "prog_bar_bg")
	local hp = Resource.LoadTool.GetImageData(Resource.UITextureAtlasType.COMMON_PROGRESS, "prog_bar_bar_green2")

	for i,v in ipairs(lst) do

		self.lst[i] = {}
	    --头顶信息条
	    local info = CPPGameLib.GetConfig("RebelQuality", v.quality)
	    v.endTime = (info.live_time * 60 ) + v.create_time
	    self.lst[i].layou = Util:LabCenter(	{
	    										{ zi = v.name , ys = v.quality , mb = true , dx = Macros.TypefaceSize.normal },
											    { zi = CPPGameLib.GetString("common_lev6" , v.level ) , ys = 0 , jg = 5 , mb = true , dx = Macros.TypefaceSize.normal },
											})
	    self.layou:AddChild(self.lst[i].layou)

	    --进度条
	    self.lst[i].bar = GUI.ProgressBar9Layer.New(hp, hpBG, 130,10, 132, 12, 100, 100)
	    self.lst[i].bar:SetAnchorPoint(0.5, 0.5)
		
		self.layou:AddChild(self.lst[i].bar:GetNode())

		if v.total_damage == 0  then
			self.lst[i].bar:SetProgress(100)
		else
			self.lst[i].bar:SetProgress(100 - ( v.total_damage / v.sumHp * 100 ) )
		end

		-- 黑底
	    self.lst[i].heidi = Util:Sprite(heidisp)
		self.lst[i].heidi:SetAnchorPoint(0.5,0.5)
		self.lst[i].heidi:SetPosition(105 + ( ( i - 1 ) * 213 ) , - 50 )
		self.layou:AddChild(self.lst[i].heidi)

		--模型

		self.lst[i].modes = LogicGUI.Model.New({info_id = v.modes_id, parent = self.layou , model_type = v.odel_type })
	    self.lst[i].modes:SetPosition(105 + ( ( i - 1 ) * 213 ) , -50 )
	    self.lst[i].modes:SetTouchRect(-80 , 0 , 150 , 150 )
	    self.lst[i].modes:SetTouchFun(nil,nil,function ( ... )
			self:ClickRebel(v)
	    end)

	    local layou = Util:LabCenter({
									    { zi = CPPGameLib.GetString("rebel_faxianzhe") , ys = nil , dx = Macros.TypefaceSize.normal , mb = true },
										{ zi = v.discoverer_name ,ys = Macros.Color.slightly , dx = Macros.TypefaceSize.normal , mb = true },
									})
	    layou:SetPosition(self.lst[i].heidi:GetContentWidth()/2 , self.lst[i].layou:GetContentHeight()/2 - 28 )
	    self.lst[i].heidi:AddChild(layou)


	    local height = self.lst[i].modes:GetHeight()
	    if self.lst[i].modes:GetHeight() < 5 then
	    	height = 180
	    end

	    self.lst[i].bar:SetPosition(105 + ( ( i - 1 ) * 213 ) , height + self.lst[i].modes:GetNode():GetPositionY() + 20 )

	    self.lst[i].layou:SetPosition(105 + ( ( i - 1 ) * 213 ) , self.lst[i].bar:GetPositionY() + self.lst[i].bar:GetNode():GetContentHeight() + 10 )
	

		CPPGameLib.PrintTable("xy" , v , "v---------------------") 


	end
end

--战功显示&伤害显示
function Modules.RebelView:ShowInfo( ... )

    local index = CallModuleFunc(ModuleType.REBEL, "GetNowBuyNum")
    if index and index ~= 0 then
	    self.icon_red_point_4:SetVisible(true)
	end

	--累计
	local sum = CallModuleFunc(ModuleType.REBEL, "RebelSum")
	--修改功勋
	self.merit:SetString(sum.today_exploit or  0) 
	--修改伤害
	self.harm:SetString(sum.today_damage or  0)

	--排位
	local merit = CallModuleFunc(ModuleType.REBEL, "RebelMerit")

	if merit.damage_rank ~= 0 then
		self.harm_rank:SetString(CPPGameLib.GetString("rebel_mingci_1" , merit.damage_rank ))
	end

	if merit.exploit_rank ~= 0 then
		self.merit_rank:SetString(CPPGameLib.GetString("rebel_mingci_1" , merit.exploit_rank ))
	end

	PosTool.RightTo(self.merit,self.merit_rank , 10) 
	PosTool.RightTo(self.harm,self.harm_rank , 10)
end

--是不是刚刚打完战斗
function Modules.RebelView:IsCombat( ... )
	local lst = CallModuleFunc(ModuleType.REBEL, "RebelLst")
	for i,v in ipairs(lst) do
		if v.uid == unpack(self.__open_data) then
			if v.total_damage >= v.sumHp then
				return
			end

			local function combat( ... )
				self.combat = LogicGUI.TemplRebelCombat.New(v , function ( ... )
					self.combat:DeleteMe()
					self.combat = nil
				end)
			end
			
			local type_ = CallModuleFunc(ModuleType.REBEL, "IsAlreadyShare")
					
			local id = CallModuleFunc(ModuleType.REBEL, "GetCombatObject" )

			local roleId = CallModuleFunc(ModuleType.ROLE , "GetRoleID")

			if type_ == false and id == roleId then
				local tips = LogicGUI.TemplAffirmTips.New( { 
															type = LogicGUI.TemplAffirmTipsType.YES_NO,
															content	= CPPGameLib.GetString("rebel_share"),
															cancel_text = CPPGameLib.GetString("common_cancel") ,
															confirm_func = function ( ... )
																GlobalTipMsg.GetInstance():Show( CPPGameLib.GetString("rebel_shareSucceed") )
																CallModuleFunc(ModuleType.REBEL, "RebelShaReReq")
																combat()
															end,
															cancel_text = CPPGameLib.GetString("common_cancel") ,
															cancel_func = function ( ... )
																combat()
															end,
														} )
			else
				combat()
			end
		end
	end
end

--点击叛军
function Modules.RebelView:ClickRebel( date )
	self.combat = LogicGUI.TemplRebelCombat.New(date , function ( ... )
		self.combat:DeleteMe()
		self.combat = nil
	end)
end


function Modules.RebelView:SetTime( )

	self.Image_tips:SetVisible(false)

	local config = {}
	table.insert( config , { start_time = ConfigAdapter.Common.GetRebelConsumeHalve()[1] , over_time = ConfigAdapter.Common.GetRebelConsumeHalve()[2] } )
	table.insert( config , { start_time = ConfigAdapter.Common.GetRebelExploitDouble()[1] , over_time = ConfigAdapter.Common.GetRebelExploitDouble()[2] } )
	for i,v in ipairs(config) do
		local start = TimeTool.TransTimeStamp( "HH:MM" , (v.start_time * 0.01 * 60 * 60) )
		local over = TimeTool.TransTimeStamp( "HH:MM" , (v.over_time * 0.01 * 60 * 60) )

		local bool = CallModuleFunc(ModuleType.REBEL, "GetActivityOpen", start , over )
		local index = 0
		if bool then
			self.Image_tips:SetVisible(true)

			local time = Util:Label( start .." - ".. over , Macros.TypefaceSize.normal , Macros.Color.white )
			time:SetPosition(self.Image_tips:GetContentWidth()/2 , self.Image_tips:GetContentHeight() - 20)
			self.Image_tips:AddChild(time)

			local rebel_tips = Util:Label( CPPGameLib.GetString("rebel_tips"..i) , Macros.TypefaceSize.slightly , color , self.Image_tips:GetContentWidth() - 20  )
			rebel_tips:SetTextHorizontalAlign(LabelEx.ALIGIN_MIDDLE)
			self.Image_tips:AddChild(rebel_tips)
			PosTool.Center( rebel_tips , 0 , -10 )
		end
	end

end

--倒计时
function Modules.RebelView:Countdown( )
	-- CPPActionManager.StopAllActions(self.Image_guestBook)
	CPPActionManager.DelayTo(  self.Image_guestBook , 0.1 , GlobalCallbackMgr:AddCallBackFunc(function()
		print("xy" , "==============Countdown")
		self:SetTime()
	end))
end